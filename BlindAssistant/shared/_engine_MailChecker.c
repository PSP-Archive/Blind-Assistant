/* 
   ----------------------------------------------------------------------------
   BLIND ASSISTANT CFW0006
   ----------------------------------------------------------------------------
   Copyright 2009-2014 Filippo Battaglia
   This program has been developed in Visilab Research Center 
   University of Messina - Italy
   
   All right reserved. The use of this program is allowed only for not
   commercial uses. 
   
   The source code is released under GPL license. See file Blind Assistant
   License.Pdf for further details
   ----------------------------------------------------------------------------
   FUNZIONI DEL SISTEMA DI VERIFICA DELLA PRESENZA DI NUOVE MAIL
   ----------------------------------------------------------------------------
*/

        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        #undef  ND_NOT_MAIN_SOURCE
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        
        #include "BlindAssistant.h"
        #include "version.h"
 
/*
    ---------------------------------------------------------------------------
    VARIABILI
    ---------------------------------------------------------------------------
*/
 
char  baMailChk_EnableDedNetConnection;

int   mailchkThreadID;
int   mailchkYouCanExit;
int   mailchkThreadPriority;
char  mailchkInhibitedByInternal = 0;

static popsession     *PntTo_Pop3_Session;
static char           *Pop3_GlobalErrMsg;
static int             Pop3_NrMessagesAvailable;

 
/*
    ---------------------------------------------------------------------------
    FUNZIONI DI CONNESSIONE
    ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

char baMailChk_ConnectToNetForMailDownload ()
/*
    Provvede a creare una connessione per scaricare le mail dalla rete
*/
{
   char ErrRep = 0; 
   char IntErrRep;
   
   if (!MainNetConn_IsOperative)
   {
           // Non c'è una connessione principale di rete attivata.
           // Provvedi autonomamente al processo di connessione
          
           IntErrRep=ndHAL_WLAN_GetAccessPointInfo (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj);                         
     
           if (!IntErrRep)
           {
                  IntErrRep = ndHAL_WLAN_OpenPSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj, ND_SILENT_AP_ACCESS, 10);        
           
                  if (!IntErrRep)       // E' riuscito a creare una connessione dedicata
                  {
                       ErrRep                               =  0;
                       baMailChk_EnableDedNetConnection     =  1;            
                  }
                  else
                  {
                       ErrRep                               =  -2;
                       baMailChk_EnableDedNetConnection     =  0;    
                  }
           }
           else   // L'access point è vuoto
           {
                ErrRep                               = -1;
                baMailChk_EnableDedNetConnection     =  0;  
           }
     }
     else         // C'è una connessione principale di rete attivata
     {
           ErrRep                              = 0;
           baMailChk_EnableDedNetConnection    = 0;
     }
   
     return ErrRep;
}

void baMailChk_DisconnectToNetForMailDownload ()
/*
    Disconnetti la rete, eliminando eventuali connessioni dedicate per 
    l'aggiornamento
*/
{
    if (!MainNetConn_IsOperative)
    {
        if (baMailChk_EnableDedNetConnection)
        {
            ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1);
            baMailChk_EnableDedNetConnection     = 0;  
        }   
    }
}

#endif

/*
   ----------------------------------------------------------------------------
   VALUTATORE LOGICO DELLA NECESSITA' DI SEGNALAZIONE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

void INTERNAL_EvaluateIfYouMustSignalTheNewMessages (int NewMsgDetected)
{
    // Ci sono nuove mail rispetto a quanto ritenevamo ?
    
    if (baMailChecker_HyphActualMailMessages<NewMsgDetected)
    {
         // Sono effettivamente presenti nuovi messaggi. E' necessario
         // segnalare al cieco che i nuovi messaggi sono stati trovati
         
         baMailChecker_HyphActualMailMessages = NewMsgDetected;
         baMailChecker_NrTimesAlreadySignaled = 1;
         
         // Segnalazione acustica e visiva
         baMicroIcons_Enable (MICROICON__UNREADED_MAILS, RENDER);
         baaud_YouHaveUnreadedMails (); 
         
         return;                                                
    } 
    
    // Nulla è cambiato rispetto alla situazione precedente.
    
    if ( (baMailChecker_HyphActualMailMessages==NewMsgDetected) && (baMailChecker_NrTimesAlreadySignaled>0) )
    {
         // Il messaggio di avviso deve essere ripetuto per un massimo
         // di 3 volte, dopodichè si presume che sia stato compreso
         
         if (baMailChecker_NrTimesAlreadySignaled < 3)
         {
               // Segnalazione acustica e visiva
               baMicroIcons_Enable (MICROICON__UNREADED_MAILS, RENDER);
               baaud_YouHaveUnreadedMails (); 
               
               // e incremento del contatore
               baMailChecker_NrTimesAlreadySignaled++;                                  
         } 
         
         return;                                                  
    }
    
    // Ci sono meno mail nel server rispetto a quello che ritenevamo: qualcuno
    // le ha lette ma non da BlindAssistant. Togli la microicona di segnalazione
    
    if (baMailChecker_HyphActualMailMessages>NewMsgDetected)
    {
         baMailChecker_HyphActualMailMessages = NewMsgDetected;
         baMailChecker_NrTimesAlreadySignaled = 0;
         
         baMicroIcons_Disable (MICROICON__UNREADED_MAILS, RENDER);    
         
         return;                                                
    }
}

#endif

/*
   ----------------------------------------------------------------------------
   SISTEMA DI CONTROLLO DELLE NUOVE MAIL
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

void INTERNAL_PeriodicalCheckForNewMails (void)
{
    char *Pop3Header; 
    char *Pop3Message;
    int   Pop3LastMailID;
    int   Pop3NrBytes; 
    
    int   Counter;
    
    char ServerPortString [512];
    char ErrRep, Abort;
    
    if (!NetworkIsBusy)
    {
            if (strcmp (SystemOptPacket.Pop3ServerAddr, ""))          // I controlli vengono eseguiti solo se la stringa non è vuota
            {
                 if (strcmp (SystemOptPacket.Pop3UserName, ""))         // I controlli vengono eseguiti solo se la stringa non è vuota
                 {
                        NetworkIsBusy = 1;
                        baInhibitVideoEngine();                         // Inibisce l'engine video
                   
                        // Cerca di creare una connessione di rete dedicata
                        ErrRep = baMailChk_ConnectToNetForMailDownload();
                        
                        if (!ErrRep)
                        {
                             sprintf (ServerPortString, "%s:%d", SystemOptPacket.Pop3ServerAddr, SystemOptPacket.Pop3Port); 
                             Pop3_GlobalErrMsg = popbegin (ServerPortString, SystemOptPacket.Pop3UserName, SystemOptPacket.Pop3Password, &PntTo_Pop3_Session);

                             if (Pop3_GlobalErrMsg == 0)
                             {
                                 // Porta la variabile Abort a 0. Sarà cambiata solo in caso di errore
                                 Abort = 0;
                                 
                                 // Per prima cosa determina il numero di messaggi sul server POP3
                                 Pop3LastMailID = poplast (PntTo_Pop3_Session);
                                 
                                 // E' necessario contare solo il numero di messaggi che sono di dimensione superiore a 250 kb
                                 Pop3_NrMessagesAvailable=0; 
               
                                 for (Counter=1; Counter<=Pop3LastMailID; Counter++)
                                 {
                                    // Ritardo necessario per consentire la connessione su alcuni server
                                    ndHAL_Delay (1);
                                
                                    // Ricava l'intestazione
                                    Pop3Header = popgethead (PntTo_Pop3_Session, Counter);
                                    
                                    if (Pop3Header)
                                    {
                                          // Incrementa il contatore del numero di messaggi disponibili
                                          Pop3NrBytes = popmsgsize (PntTo_Pop3_Session, Counter);
                                          if (Pop3NrBytes<STD_MAIL_MAX_SIZE) Pop3_NrMessagesAvailable++;    
                                    }
                                    else
                                    {
                                          // In caso di errore nel scaricare l'intestazione termina subito
                                          Abort = 1;
                                          break;
                                    }
                                    
                                 }        // End ciclo for
                             
                                 // Chiudi la sessione POP3
                                 popend (PntTo_Pop3_Session);
                             
                                 // Adesso provvedi a verificare, se non ci sono stati errori, se ci sono nuovi 
                                 // messaggi da segnalare
                             
                                 if (!Abort) INTERNAL_EvaluateIfYouMustSignalTheNewMessages (Pop3_NrMessagesAvailable);   
                                 
                             }   // Pop3_GlobalErrMsg == 0
                             
                             // Lavoro finito: disabilita la connessione di rete dedicata
                             baMailChk_DisconnectToNetForMailDownload ();                                        
                        }
                        
                        // Routines completata: deinibisce video engine   
                                        
                        baDeInhibitVideoEngine_NoForce();                      // DeInibisce l'engine video           
                        NetworkIsBusy=0;
                 }
            }  
    }
}

#endif

/*
   ----------------------------------------------------------------------------
   ENGINE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

int baMailCheckerEngine (SceSize args, void *argp)
{
    int Counter;
    
    ndHAL_NET_SetupStdNetworkTimeOutValue (4000000);
    
    mailchkYouCanExit=0;
    NetworkIsBusy   =0;
    
    while (!mailchkYouCanExit)
    {      
          Label0:
          
          if (SystemOptPacket.CheckForNewMails)
          {      
                  if (!mailchkInhibitedByInternal)    // Non c'è stata inibizione interna 
                  {
                          // Controlla se ci sono aggiornamenti  
                          Counter=0;
                          
                          while (Counter<2*SystemOptPacket.MinBetweenChecksForMails)
                          {  
                                sceKernelChangeThreadPriority (mailchkThreadID, 70);
                                sceKernelDelayThread (30000000);  
                                sceKernelChangeThreadPriority (mailchkThreadID, mailchkThreadPriority);
                                
                                Counter++;
                                
                                // CHECK:
                                // Ci sono casi in cui il cambiamento di stato (con disattivazione del
                                // motore di update), avviene durante la fase di delay del thread. 
                                // In quel caso, ovviamente, è necessario eseguire un goto in modo
                                // da bloccare il thread senza eseguire la procedura di aggiornamento
                                
                                if (!SystemOptPacket.CheckForNewMails) goto Label0;
                                
                                // Per la stessa ragione, si deve controllare se il sistema è in 
                                // modalità MailReader, prima di uscire dal ciclo while. Se lo è,
                                // vai a Label0 per impedire che l'engine parta
                                
                                if (baMode==BA_MODE_MAILREADER) goto Label0;
                                
                                // Un controllo di questo tipo si esegue anche per il processo
                                // di inibizione interna, per ragioni di sicurezza
                                
                                if (mailchkInhibitedByInternal) goto Label0;
                          }
                          
                          // Fai partire il controllo per la presenza di nuove mail
                          
                          INTERNAL_PeriodicalCheckForNewMails ();
                  }
                  else        // Il sistema è internamente inibito
                  {
                       // Ritardo di quattro secondi: impedisce il blocco dello scheduler e 
                       // lascia il tempo al sistema di eseguire le altre operazioni
                       // per cui è stata richiesta l'inibizione interna (ad esempio,
                       // le routines di training o di test)
                       
                       sceKernelDelayThread (4000000);         
                  }
          }
          else   // Il thread è inibito
          {
                // Sospendi il thread: sarà riattivato in seguito dall'esterno
                NetworkIsBusy=0;
                sceKernelSleepThread ();
          }
    }
    
}

#endif


/*
   ----------------------------------------------------------------------------
   API PER L'INIZIALIZZAZIONE
   ----------------------------------------------------------------------------
*/ 
 

void baStartMailCheckerEngine ()
{
    #ifndef PSP1000_VERSION
    
        mailchkThreadPriority = sceKernelGetThreadCurrentPriority ()+2; 
        
        mailchkThreadID = sceKernelCreateThread ("baMailCheckerEngine", &baMailCheckerEngine, mailchkThreadPriority, 16384, 
                                                     PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU, 0);                                               
    
        sceKernelStartThread (mailchkThreadID, 0, 0);  
    
    #endif 
} 

void baStopMailCheckerEngine ()
{
    #ifndef PSP1000_VERSION
    
        mailchkYouCanExit=1;
        ndHAL_Delay (2);
        
        sceKernelTerminateDeleteThread (mailchkThreadID);
        
    #endif
}

void baInhibitMailCheckerEngine ()
{
    mailchkInhibitedByInternal=1;  
}

void baDeInhibitMailCheckerEngine ()
{
    mailchkInhibitedByInternal=0;  
}



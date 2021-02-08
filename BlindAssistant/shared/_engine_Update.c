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
   UPDATE MANAGER ED UPDATE ENGINE
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
   ----------------------------------------------------------------------------
   VARIABILI PER USO INTERNO
   ----------------------------------------------------------------------------
*/ 


int  updengThreadID;
int  updengYouCanExit;
char updengInhibitedByInternal = 0;

int  updengThreadPriority;
char UpdateFromSiteCopy [512];

char baupd_EnableDedNetConnection; 
         
static struct ndMediaObj_Type      AudioMsgObj;
static struct ndNetworkObject_Type NetObjProbe;

/*
   ----------------------------------------------------------------------------
   VALUTAZIONE NECESSITA' UPDATES
   ----------------------------------------------------------------------------
*/ 

char INTERNAL_IHaveEvaluatedUpdateIsNecessary (char *UpdCodeID)
{
     char OwnCodeID [128];
     char OwnCodeA [128];
     char OwnCodeB [128];
     int  OwnVersionNumber;
     
     char UpdCodeA [128];
     char UpdCodeB [128];
     int  UpdVersionNumber;
     
     char FileDestination    [512];
     
     strcpy (OwnCodeID, BA_VERSION_CODE);
     ndStrCpyBounded (OwnCodeA, OwnCodeID,  0, 16, 0);
     ndStrCpyBounded (OwnCodeB, OwnCodeID, 18, 21, 0);
     OwnVersionNumber = atoi (OwnCodeB);
     
     ndStrCpyBounded (UpdCodeA, UpdCodeID,  0, 16, 0);
     ndStrCpyBounded (UpdCodeB, UpdCodeID, 18, 21, 0);
     UpdVersionNumber = atoi (UpdCodeB);
     
     // La verifica della necessità dell'aggiornamento avviene sulla base del numero
     // di versione e del codice A: i codici A devono essere uguali ed il numero di
     // versione dell'aggiornamento deve essere superiore rispetto a quello del
     // programma presente
     
     if ( (!strcmp (UpdCodeA, OwnCodeA)) && (OwnVersionNumber<UpdVersionNumber) )
          return 1;           // Segnala la necessità del download
     else
          return 0;           // Nessun download necessario  
}


/*
   ----------------------------------------------------------------------------
   SEGNALAZIONI DI ERRORE
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_SignalWrongConnectionProfile (void)
{
    int ErrDialogBox;
    
    ErrDialogBox = ndLP_CreateWindow (Windows_MaxScreenX-290, Windows_MaxScreenY-130, Windows_MaxScreenX-1, Windows_MaxScreenY-1,
                                   "ERROR", COLOR_WHITE, COLOR_RED, COLOR_GRAY05, COLOR_GRAY05, 0);
                     
    ndLP_MaximizeWindow (ErrDialogBox);
    
    ndWS_GrWriteLn (5,  5, "Connection profile in firmware", COLOR_BLACK, COLOR_GRAY05, ErrDialogBox, NORENDER);
    ndWS_GrWriteLn (5, 13, "is wrong or not configured till", COLOR_BLACK, COLOR_GRAY05, ErrDialogBox, NORENDER);
    ndWS_GrWriteLn (5, 21, "now  ", COLOR_BLACK, COLOR_GRAY05, ErrDialogBox, NORENDER);
    XWindowRender (ErrDialogBox);
    
    // Segnala l'errore via audio
    baaud_WrongNetworkConfig ();
    ndDelay (4);
    
    // Cancella la finestra di segnalazione
    ndLP_DestroyWindow (ErrDialogBox); 
}

/*
   ----------------------------------------------------------------------------
   CONNESSIONI/DISCONNESSIONI DEDICATE PER L'AGGIORNAMENTO
   ----------------------------------------------------------------------------
*/ 

char INTERNAL_ConnectToNetForUpdate ()
/*
    Provvede a scaricare gli aggiornamenti alla rete
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
                       ErrRep                           =  0;
                       baupd_EnableDedNetConnection     =  1;            
                  }
                  else
                  {
                       ErrRep                           =  -2;
                       baupd_EnableDedNetConnection     =  0;    
                  }
           }
           else   // L'access point è vuoto
           {
                ErrRep                           = -1;
                baupd_EnableDedNetConnection     =  0;  
           }
     }
     else         // C'è una connessione principale di rete attivata
     {
           ErrRep                           = 0;
           baupd_EnableDedNetConnection     = 0;
     }
   
     return ErrRep;
}

void INTERNAL_DisconnectToNetForUpdate ()
/*
    Disconnetti la rete, eliminando eventuali connessioni dedicate per 
    l'aggiornamento
*/
{
    if (!MainNetConn_IsOperative)
    {
        if (baupd_EnableDedNetConnection)
        {
            ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1);
            baupd_EnableDedNetConnection     = 0;                                      
        }
    }
}


/*
   ----------------------------------------------------------------------------
   ROUTINES DI AGGIORNAMENTO
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_StartUpdatingProcedure (char *NameUpdate, char *PathUpdate)
{
     char FileToDownload     [512];
     char FileDestination    [512];
     char ErrRep;
     
     strcpy (&FileDestination, "ms0:/BLINDASSISTANT/UPDATES/");
     strcat (&FileDestination, NameUpdate);
     strcat (&FileDestination, ".PBP");
     
     if (!fexist (FileDestination))
     {
         // Avverti che stai provvedendo a scaricarlo
         baaud_IWillDownloadItNow ();
         
         // Provvedi a scaricare il file
         strcpy (&FileToDownload, &UpdateFromSiteCopy);
         strcat (&FileToDownload, PathUpdate);
         
         ErrRep=ndGCURL_FileTransfer (&NetObj, &FileToDownload, &FileDestination, &NetworkOptions);
         
         // Se era stato previsto il log delle operazioni eseguite, dai il tempo di leggere
         // i risultati e poi chiudi
               
         if (NetworkOptions.BeVerbose)
         {
              ndDelay (2);
              ndDisableStdOutDialogBox ();
         }
         
         if (!ErrRep)
         {
             baaud_DownloadSuccess ();        
         }
         else    // L'aggiornamento è fallito
         {
             baaud_DownloadFailed ();  
          
             // Cancella l'eventuale file caricato parzialmente 
             remove (&FileDestination);    
         }
     }
}

void INTERNAL_InstallNewUpdatesBinaryFile (char *NameBinaryFile)
{
    char FileDest   [512];
    
    int  FileLength,  Counter;
    int  FileHandle0, FileHandle1;
    int  WndHandle,   ErrRep,    YouCanExit; 
    int  NrCharsRead, NrCharsWritten;
    void *SymbolBuffer;
     
    
    WndHandle = ndLP_CreateWindow (50, 77, Windows_MaxScreenX-50, Windows_MaxScreenY-77,
                                                 "Install new BA core", COLOR_WHITE, COLOR_GREEN,
                                                 COLOR_GRAY, COLOR_BLACK, 0);
    
    ndLP_MaximizeWindow (WndHandle);
    
    ndWS_GrWriteLn (10, 10, "Waiting.... I'm copying the data ", COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);
    ndWS_GrWriteLn (10, 18, "related to the new core...       ", COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);
    XWindowRender (WndHandle);
    
    ndTBAR_DrawProgressBar   (10, 45, 340, 65, 0, 0, 100, 
                              COLOR_WHITE, COLOR_RED, COLOR_GRAY03, COLOR_BLACK, 
                              PBAR_PERCENT, WndHandle, RENDER);  
    
    strcpy (FileDest, BA_CORE_FILE);
    
    FileLength = flen (NameBinaryFile);
    
    // Segnala l'inizio del processo di update
    baaud_IWillInstallItNow ();
    
    // Apri il nuovo file core
    
    FileHandle0 = fopen (NameBinaryFile,  "r");
    FileHandle1 = fopen (&FileDest, "w");
    
    YouCanExit=0;
    Counter=0;
    ErrRep=0;
    
    SymbolBuffer = malloc (100000);
    
    while (!YouCanExit)
    {
      NrCharsRead   =fread  (SymbolBuffer, 1, 100000, FileHandle0); 
      if (NrCharsRead!=100000) { YouCanExit=1; }
    
      if (NrCharsRead!=0)
      {
         NrCharsWritten=fwrite (SymbolBuffer, 1, NrCharsRead, FileHandle1); 
         if (NrCharsWritten!=NrCharsRead) { ErrRep=-1; break; } 
      }
      
      Counter=Counter+NrCharsRead;
      
      ndTBAR_DrawProgressBar   (10, 45, 340, 65, (float)(Counter)/(float)(FileLength)*100, 0, 100, 
                            COLOR_WHITE, COLOR_RED, COLOR_GRAY03, COLOR_BLACK, 
                            PBAR_PERCENT, WndHandle, RENDER);  
    }
    
    free (SymbolBuffer);
    
    fclose (FileHandle0);
    fclose (FileHandle1);
    
    if (!ErrRep)
    {
      ndWS_DrawRectangle (10, 10, 340, 30, COLOR_GRAY, COLOR_GRAY, WndHandle, NORENDER);
      
      ndWS_GrWriteLn (10, 10, "Update completed successfully", COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);
      
      ndTBAR_DrawProgressBar   (10, 45, 340, 65, 100, 0, 100, 
                                COLOR_WHITE, COLOR_RED, COLOR_GRAY03, COLOR_BLACK, 
                                PBAR_PERCENT, WndHandle, RENDER);  
     
      ndDelay (3);
      ndLP_DestroyWindow (WndHandle);
      
      // Segnala il completamento del processo di update
      baaud_InstallationComplete ();
      ndDelay (4);
      
      // Segnala che il sistema sarà riavviato per caricare la copia aggionata
      baaud_TheSystemWillBeRebootedNow ();
      ndDelay (4);
      
      // RIAVVIO DEL SISTEMA: Queste due righe provocheranno l'uscita dal sistema
      // ed il riavvio
      baNewExecRequested=1;
      baUnlockMainThread ();           
    }
    else
    {
      ndWS_DrawRectangle (10, 10, 340, 30, COLOR_GRAY, COLOR_GRAY, WndHandle, NORENDER);
      
      ndWS_GrWriteLn (10, 10, "Update failed. I/O error.", COLOR_RED, COLOR_GRAY, WndHandle, NORENDER);
      
      ndTBAR_DrawProgressBar   (10, 45, 340, 65, 100, 0, 100, 
                                COLOR_WHITE, COLOR_RED, COLOR_GRAY03, COLOR_BLACK, 
                                PBAR_PERCENT, WndHandle, RENDER);  
     
      ndDelay (3);
      ndLP_DestroyWindow (WndHandle);
    }
}

void INTERNAL_InstallNewUpdates (char *NameUpdate)
{
    char FileSrc    [512];
    
    strcpy (&FileSrc, "ms0:/BLINDASSISTANT/UPDATES/");
    strcat (&FileSrc, NameUpdate);
    strcat (&FileSrc, ".PBP");
    
    INTERNAL_InstallNewUpdatesBinaryFile (&FileSrc);
}



char INTERNAL_CheckIfNewUpdatesExist (char *NameUpdate, char *PathUpdate)
{
    char DirectiveRow0 [128];
    char DirectiveRow1 [128];
    char DirectiveRow2 [128];
    char DirectiveRow3 [128];
        
    int  FListOfUpdates_Handle;
     
    char FileToDownload     [512];
    char NecessaryUpdate = 0;
    char ErrRep;
                   
    // Scarica il file UpdatesList.Txt
    strcpy (&FileToDownload, &UpdateFromSiteCopy);
    strcat (&FileToDownload, "UpdatesList.txt");
    
    // Resetta ai valori standard le opzioni di rete per il modulo GCurl
    memset (&NetworkOptions, 0, sizeof(NetworkOptions));
    
    // Setup delle impostazioni di rete in base a quanto previsto dalle
    // advanced options
    NetworkOptions.BeVerbose     = SystemOptPacket.BeVerbose;
    NetworkOptions.HttpTunneling = SystemOptPacket.HttpTunneling;
    
    // Scarica il file
    
    ErrRep=ndGCURL_FileTransfer (&NetObj, &FileToDownload, BA_UPDATES_FILE, &NetworkOptions);   
    
    // Gestisci il risultato conseguito
    
    if (!ErrRep)                // Scaricata una copia in locale
    {
       // Apri il file degli updates in lettura                        
       FListOfUpdates_Handle=fopen (BA_UPDATES_FILE, "r");  
       
       if (FListOfUpdates_Handle>0)     // Aperto senza errori
       { 
                    while (1)           // Inizia scansione downloads
                    {
                          memset (&DirectiveRow0, 0, 128);
                          memset (&DirectiveRow1, 0, 128);
                          memset (&DirectiveRow2, 0, 128);
                          memset (&DirectiveRow3, 0, 128);
                          
                          fscanf (FListOfUpdates_Handle, "%s\n", &DirectiveRow0);
                          
                          if ( !strcmp (&DirectiveRow0, "[ENDFILE]") )
                          {
                               break;
                          }
                          else
                          {
                                  if ( !strcmp (&DirectiveRow0, "[BEGIN]") )
                                  {
                                       fscanf (FListOfUpdates_Handle, "%s\n", &DirectiveRow1);
                                       
                                       if ( strcmp (&DirectiveRow1, "") )
                                       {
                                              fscanf (FListOfUpdates_Handle, "%s\n", &DirectiveRow2);
                                              
                                              if ( strcmp (&DirectiveRow2, "") )
                                              {
                                                  fscanf (FListOfUpdates_Handle, "%s\n", &DirectiveRow3); 
                                                  
                                                  if ( !strcmp (&DirectiveRow3, "[END]") )
                                                  {
                                                         if ( INTERNAL_IHaveEvaluatedUpdateIsNecessary (&DirectiveRow1) )
                                                         {
                                                            NecessaryUpdate=1;   
                                                            break;
                                                         }
                                                         else    // Questo aggiornamento non è necessario o compatibile
                                                            continue;
                                                  }
                                                  else   // La direttiva del blocco non è [END]
                                                  {
                                                         break;   // Esci dal ciclo while
                                                  }
                                              }
                                              else
                                              {
                                                  break;   // Esci dal ciclo while
                                              }
                                       }
                                       else   // La seconda direttiva del blocco è vuota
                                       {
                                              break;  // Esci dal ciclo while
                                       }
                                  }     
                                  else  // La prima direttiva del blocco non è [BEGIN]
                                  {
                                        break; // Esci dal ciclo while;
                                  }   
                                  
                          }                           
                    }
                    
                    // Dopo essere uscito dal ciclo while, chiudi il file che lista gli updates
                    fclose (FListOfUpdates_Handle); 
                    
                    if (NecessaryUpdate)
                    {
                         // Se era stato previsto il log delle operazioni eseguite, dai il tempo di leggere
                         // i risultati e poi chiudi
        
                         if (NetworkOptions.BeVerbose)
                         {
                            ndDelay (2);
                            ndDisableStdOutDialogBox ();
                         }  
                    
                         // Segnala alla routine chiamante che ci sono aggiornamenti  
                    
                         strcpy (NameUpdate, DirectiveRow1);
                         strcpy (PathUpdate, DirectiveRow2);
                         return 1;
                    }
                    else   // Non ci sono aggiornamenti
                    {
                         // Se era stato previsto il log delle operazioni eseguite, dai il tempo di leggere
                         // i risultati e poi chiudi
        
                         if (NetworkOptions.BeVerbose)
                         {
                            ndDelay (2);
                            ndDisableStdOutDialogBox ();
                         }  
                    
                         strcpy (NameUpdate, "");
                         strcpy (PathUpdate, "");
                         return 0;
                    }
                      
       }
       else   // Problemi nell'apertura del file di configurazione
       {
            // Se era stato previsto il log delle operazioni eseguite, dai il tempo di leggere
            // i risultati e poi chiudi
    
            if (NetworkOptions.BeVerbose)
            {
                ndDelay (2);
                ndDisableStdOutDialogBox ();
            }
            
            strcpy (NameUpdate, "");
            strcpy (PathUpdate, "");
                            
            return -1;  
       } 
       
    }
    else // Problemi nel download del file di configurazione
    {
        // Se era stato previsto il log delle operazioni eseguite, dai il tempo di leggere
        // i risultati e poi chiudi
        
        if (NetworkOptions.BeVerbose)
        {
            ndDelay (2);
            ndDisableStdOutDialogBox ();
        }
        
        strcpy (NameUpdate, "");
        strcpy (PathUpdate, "");
                        
        return -2;   
    }
                 
}





/*
   ----------------------------------------------------------------------------
   UPDATE ENGINE
   ----------------------------------------------------------------------------
*/ 
 
void INTERNAL_PeriodicalCheckForUpdate (void)
{
    char NameUpdate [128];
    char PathUpdate [128];
    
    char ThereAreNewUpdates;
    char ErrRep;
    
    if (!NetworkIsBusy)
    {
            NetworkIsBusy = 1;
            baInhibitVideoEngine();                         // Inibisce l'engine video
           
            // Provvedi alla connessione (usando la connessione principale oppure 
            // creandone una secondaria appositamente)
           
            ErrRep=INTERNAL_ConnectToNetForUpdate ();
            
            switch (ErrRep)
            {
                   case 0:                  // Nessun problema
                   {
                        // Preleva una copia del sito di aggiornamento
                        strcpy (&UpdateFromSiteCopy, &(SystemOptPacket.UpdateFromSite));
                        
                        // Aggiungi il path per gli aggiornamenti
                        strcat (&UpdateFromSiteCopy, BA_UPDATE_PATH);
                
                        // Controlla se sono presenti nuovi aggiornamenti
                        ThereAreNewUpdates=INTERNAL_CheckIfNewUpdatesExist (&NameUpdate, &PathUpdate);
                
                        if (ThereAreNewUpdates==1)
                        {
                            // Avverti l'utente che il Visilab ha rilasciato un nuovo aggiornamento        
                            baaud_VisilabHasReleasedAnUpdate ();
                         
                            // Ritardo per migliorare l'ascolto
                            ndDelay (5);
                          
                            if (SystemOptPacket.AutoDownloadNewUpdates)
                            {
                                INTERNAL_StartUpdatingProcedure (&NameUpdate, &PathUpdate);
                                
                                if (SystemOptPacket.AutoInstallNewUpdates)
                                {
                                   INTERNAL_InstallNewUpdates (&NameUpdate);
                                }
                            }
                        }  
                
                        // Sessione di aggiornamento finita: chiudi la connessione
                        INTERNAL_DisconnectToNetForUpdate ();
                        break;
                   }
            
                   case -1:  // Problemi nella configurazione del profilo di rete
                   {
                        INTERNAL_SignalWrongConnectionProfile (); 
                        break;
                   }       
                   
                   case -2:  // Problemi nella connessione di rete
                   {
                        /* Non è prevista alcuna segnalazione */
                        break;
                   }
           }      
                      
           // Routines completata: deinibisce video engine   
                            
           baDeInhibitVideoEngine_NoForce();                      // DeInibisce l'engine video           
           NetworkIsBusy=0;
    }    
}


int baUpdateEngine (SceSize args, void *argp)
{
    int Counter;
    
    ndHAL_NET_SetupStdNetworkTimeOutValue (4000000);
    
    updengYouCanExit=0;
    NetworkIsBusy   =0;
    
    while (!updengYouCanExit)
    {      
          Label0:
          
          if (SystemOptPacket.CheckForUpdate)
          {      
                  if (!updengInhibitedByInternal)     // Non c'è stata inibizione interna
                  {
                          // Controlla se ci sono aggiornamenti  
                          Counter=0;
                          
                          while (Counter<2*SystemOptPacket.MinBetweenChecks)
                          {  
                                sceKernelChangeThreadPriority (updengThreadID, 70);
                                sceKernelDelayThread (30000000);  
                                sceKernelChangeThreadPriority (updengThreadID, updengThreadPriority);
                                
                                Counter++;
                                
                                // CHECK:
                                // Ci sono casi in cui il cambiamento di stato (con disattivazione del
                                // motore di update), avviene durante la fase di delay del thread. 
                                // In quel caso, ovviamente, è necessario eseguire un goto in modo
                                // da bloccare il thread senza eseguire la procedura di aggiornamento
                                
                                if (!SystemOptPacket.CheckForUpdate) goto Label0;
                                
                                // Un controllo di questo tipo si esegue anche per il processo
                                // di inibizione interna, per ragioni di sicurezza
                                
                                if (updengInhibitedByInternal) goto Label0;
                          }
                          
                          INTERNAL_PeriodicalCheckForUpdate ();
                  }
                  else        // Il sistema è internamente inibito
                  {
                       // Ritardo di 4 secondi: impedisce il blocco dello scheduler e 
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
 

/*
   ----------------------------------------------------------------------------
   ROUTINES FOR L'AGGIORNAMENTO MANUALE
   ----------------------------------------------------------------------------
*/ 

void baUpd_ManualChecksForUpdates (char DoDownload, char DoInstall)
{
    char NameUpdate [128];
    char PathUpdate [128];
    
    char ThereAreNewUpdates;
    char ErrRep;
    
    if (!NetworkIsBusy)
    {
            NetworkIsBusy = 1;
            baInhibitVideoEngine();                         // Inibisce l'engine video
           
            // Provvedi alla connessione (usando la connessione principale oppure 
            // creandone una secondaria appositamente)
           
            ErrRep=INTERNAL_ConnectToNetForUpdate ();
            
            switch (ErrRep)
            {
                   case 0:                  // Nessun problema
                   {
                        // Preleva una copia del sito di aggiornamento
                        strcpy (&UpdateFromSiteCopy, &(SystemOptPacket.UpdateFromSite));
                        
                        // Aggiungi il path di gestione
                        strcat (&UpdateFromSiteCopy, BA_UPDATE_PATH);
                        
                        // Controlla se sono presenti nuovi aggiornamenti
                        ThereAreNewUpdates=INTERNAL_CheckIfNewUpdatesExist (&NameUpdate, &PathUpdate);
                        
                        if (ThereAreNewUpdates==1)
                        {
                            // Avverti l'utente che il Visilab ha rilasciato un nuovo aggiornamento        
                            baaud_VisilabHasReleasedAnUpdate ();
                         
                            // Ritardo per migliorare l'ascolto
                            ndDelay (5);
                          
                            if (DoDownload)
                            {
                                INTERNAL_StartUpdatingProcedure (&NameUpdate, &PathUpdate);
                                
                                if (DoInstall)
                                {
                                   INTERNAL_InstallNewUpdates (&NameUpdate);
                                }
                            }
                        }  
                        
                        // Sessione di aggiornamento finita: chiudi la connessione
                        INTERNAL_DisconnectToNetForUpdate ();
                        
                        break;
                   }
            
                   case -1:  // Problemi nella configurazione del profilo di rete
                   {
                        INTERNAL_SignalWrongConnectionProfile (); 
                        break;
                   }       
                   
                   case -2:  // Problemi nella connessione di rete
                   {
                        /* Non è prevista alcuna segnalazione */
                        break;
                   }
           }      
                      
           // Routines completata: deinibisce video engine   
                            
           baDeInhibitVideoEngine_NoForce();                      // DeInibisce l'engine video           
           NetworkIsBusy=0; 
    }  
}

void baUpd_ManuallyInstallANewUpdate (void)
{
    char FileSrc    [512];
    char Result;
    
    if (!NetworkIsBusy)
    {
        NetworkIsBusy=1;
    
        Result=FileManager ("Choose the new core to install (.PBP)", 0, 0, &FileSrc); 
        
        if (Result==0)     // L'utente ha scelto
        {
            INTERNAL_InstallNewUpdatesBinaryFile (&FileSrc);
        }
    
        NetworkIsBusy=0;
    }
}

 
 
 
/*
   ----------------------------------------------------------------------------
   API PER L'INIZIALIZZAZIONE
   ----------------------------------------------------------------------------
*/ 
 
void baStartUpdateEngine ()
{
    updengThreadPriority = sceKernelGetThreadCurrentPriority ()+2; 
    
    updengThreadID = sceKernelCreateThread ("baUpdateEngine", &baUpdateEngine, updengThreadPriority, 16384, 
                                                 PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU, 0);                                               

    sceKernelStartThread (updengThreadID, 0, 0);   
} 

void baStopUpdateEngine ()
{
    updengYouCanExit=1;
    ndHAL_Delay (2);
    
    sceKernelTerminateDeleteThread (updengThreadID);
    ndGCURL_GlobalDeInit ();
}

void baInhibitUpdateEngine ()
{
    updengInhibitedByInternal=1;  
}

void baDeInhibitUpdateEngine ()
{
    updengInhibitedByInternal=0;  
}

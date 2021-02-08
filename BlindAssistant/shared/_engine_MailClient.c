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
   FUNZIONI DEL CLIENT EMAIL
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

char           baMail_EnableDedNetConnection;
char           baMailClient_Window;

static popsession     *PntTo_Pop3_Session;
static char           *Pop3_GlobalErrMsg;

static int             Pop3_NrMessagesAvailable;


/*
    ---------------------------------------------------------------------------
    PROTOTIPI API C++ VMIME
    ---------------------------------------------------------------------------
*/

char baMail_VMIME_GetMailSender (char *C_Input, char *C_Output);
char baMail_VMIME_GetMailSubject (char *C_Input, char *C_Output);
char baMail_VMIME_DecodeMessageContent (char *C_Input, char *C_Output, int *NrCharsOutput, int NrChars);

/*
    ---------------------------------------------------------------------------
    FUNZIONI DI CONNESSIONE
    ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

char baMail_ConnectToNetForMailDownload ()
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
                       ErrRep                            =  0;
                       baMail_EnableDedNetConnection     =  1;            
                  }
                  else
                  {
                       ErrRep                            =  -2;
                       baMail_EnableDedNetConnection     =  0;    
                  }
           }
           else   // L'access point è vuoto
           {
                ErrRep                           = -1;
                baMail_EnableDedNetConnection     =  0;  
           }
     }
     else         // C'è una connessione principale di rete attivata
     {
           ErrRep                           = 0;
           baMail_EnableDedNetConnection    = 0;
     }
   
     return ErrRep;
}

void baMail_DisconnectToNetForMailDownload ()
/*
    Disconnetti la rete, eliminando eventuali connessioni dedicate per 
    l'aggiornamento
*/
{
    if (!MainNetConn_IsOperative)
    {
        if (baMail_EnableDedNetConnection)
        {
            ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1);
            baMail_EnableDedNetConnection     = 0;  
        }   
    }
}

#endif

/*
    ---------------------------------------------------------------------------
    CONNESSIONE POP3
    ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

char baMail_CreatePop3SvrAndDownloadMail (void)
{
    char *Pop3Header; 
    char *Pop3Message;
    int   Pop3LastMailID; 
    int   Pop3NrBytes; 
    
    char  ServerPortString [512];
    char *ErrMsg;
    
    int   Counter, NrRecord;
    int   ErrRep = 0;
    
    // Cancella la struttura del gestore mail
    
    for (Counter=0; Counter<STD_NR_MAIL_AVAILABLE; Counter++)
    {
        baMailClnRegister [Counter].MsgHeader = 0;
        baMailClnRegister [Counter].Message = 0;
        baMailClnRegister [Counter].Pop3ID = 0;
        baMailClnRegister [Counter].IsAvailable = 0;
        baMailClnRegister [Counter].YouMustDeleteIt = 0;   
    }
    
    // Connettiti al server POP3
    
    sprintf (ServerPortString, "%s:%d", SystemOptPacket.Pop3ServerAddr, SystemOptPacket.Pop3Port); 
    Pop3_GlobalErrMsg = popbegin (ServerPortString, SystemOptPacket.Pop3UserName, SystemOptPacket.Pop3Password, &PntTo_Pop3_Session);

    if (Pop3_GlobalErrMsg == 0)
    {
        // Per prima cosa determina il numero di messaggi sul server POP3
        Pop3LastMailID = poplast (PntTo_Pop3_Session);
        
        // Adesso provvedi al download di tutti i messaggi che abbiano dimensione
        // inferiore a 250 kb. 
        
        Pop3_NrMessagesAvailable=0; 
               
        for (Counter=1; Counter<=Pop3LastMailID; Counter++)
        {
            // Ritardo necessario per consentire la connessione su alcuni server
            ndHAL_Delay (1);
        
            // Ricava l'intestazione
            Pop3Header = popgethead (PntTo_Pop3_Session, Counter);
            
            if (Pop3Header)
            {
                  Pop3NrBytes = popmsgsize (PntTo_Pop3_Session, Counter);
                                                      
                  if (Pop3NrBytes<STD_MAIL_MAX_SIZE)
                  {
                      // Provvedi a leggere il corpo del messaggio
                      Pop3Message = popgetmsg (PntTo_Pop3_Session, Counter);
                      
                      if (Pop3Message)
                      {
                           // Copia il messaggio nella struttura locale
                           
                           baMailClnRegister [Pop3_NrMessagesAvailable].Message = malloc (Pop3NrBytes+1);
                           memcpy (baMailClnRegister [Pop3_NrMessagesAvailable].Message, Pop3Message, Pop3NrBytes);
                           
                           baMailClnRegister [Pop3_NrMessagesAvailable].MsgHeader = malloc (strlen(Pop3Header)+1);
                           memcpy (baMailClnRegister [Pop3_NrMessagesAvailable].MsgHeader, Pop3Header, strlen(Pop3Header));
                      
                           baMailClnRegister [Pop3_NrMessagesAvailable].Pop3NrBytes = Pop3NrBytes;
                           baMailClnRegister [Pop3_NrMessagesAvailable].Pop3ID = Counter;
                           baMailClnRegister [Pop3_NrMessagesAvailable].IsAvailable = 1;
                           
                           Pop3_NrMessagesAvailable++;
                      }
                      else
                      {
                           // Se c'è stato un problema tecnico, interrompe e restituisce un codice di errore in uscita
                  
                           ErrRep=-3;
                           break;
                      }
                  }     
            }
            else
            {
                  // Se c'è stato un problema tecnico, interrompe e restituisce un codice di errore in uscita
                  
                  ErrRep=-2;
                  break;
            }
        }
        
        // Se c'è stato qualche errore, cancella le aree di memoria che erano 
        // impegnate all'interno dell'array
        if (ErrRep) baMail_FreeMailRegister ();
        
        // Chiudi la sessione POP3
        popend (PntTo_Pop3_Session);
        return ErrRep;       
    }
    else   // Impossibile connettersi al server POP3
      return -1;
}

char baMail_ManageClosingOperations (void)
{
    char *Pop3Header; 
    char *Pop3Message;
    int   Pop3LastMailID; 
    int   Pop3NrBytes; 
    
    char  ServerPortString [512];
    char *ErrMsg;
    
    int   Counter;
    int   ErrRep=0;
    
    // Connettiti al server POP3
    
    sprintf (ServerPortString, "%s:%d", SystemOptPacket.Pop3ServerAddr, SystemOptPacket.Pop3Port); 
    Pop3_GlobalErrMsg = popbegin (ServerPortString, SystemOptPacket.Pop3UserName, SystemOptPacket.Pop3Password, &PntTo_Pop3_Session);

    if (Pop3_GlobalErrMsg == 0)
    {
       // Per prima cosa determina il numero di messaggi sul server POP3
       Pop3LastMailID = poplast (PntTo_Pop3_Session);
       
       // Adesso provvedi a contare tutti i messaggi sul server che abbiano dimensione
       // inferiore a 250 kb. 
        
       Pop3_NrMessagesAvailable=0; 
               
       for (Counter=1; Counter<=Pop3LastMailID; Counter++)
       {
            // Ritardo necessario per consentire la connessione su alcuni server
            ndHAL_Delay (1);
        
            // Ricava l'intestazione
            Pop3Header = popgethead (PntTo_Pop3_Session, Counter);
            
            if (Pop3Header)
            {
                  Pop3NrBytes = popmsgsize (PntTo_Pop3_Session, Counter);
                                                      
                  if (Pop3NrBytes<STD_MAIL_MAX_SIZE)
                  {
                      // Incrementa di 1 il contatore messaggi
                      Pop3_NrMessagesAvailable++;
                  }
            }
            else
            {
                  // Se c'è stato un problema tecnico, interrompe e restituisce un codice di errore in uscita
                  
                  ErrRep=-2;
                  break;
            }
       }
       
       // Lavoro completato. Se non ci sono stati errori nel server, adesso in Pop3_NrMessagesAvailable
       // c'è il numero di messaggi di dimensione superiore a 250 kb
       
       if (!ErrRep)
       {
             // Se non ci sono stati errori nelle operazioni precedenti, provvedi a cancellare
             // dal server le mail di cui è stata richiesta la cancellazione, una ad una
             
             for (Counter=0; Counter<STD_NR_MAIL_AVAILABLE; Counter++)
             {
                   if (baMailClnRegister [Counter].IsAvailable) 
                   {
                       if (baMailClnRegister [Counter].YouMustDeleteIt) 
                       {
                           // Invia al server il codice di cancellazione
                           
                           popdelmsg (PntTo_Pop3_Session, baMailClnRegister [Counter].Pop3ID);
                           ndHAL_Delay (1);
                           
                           // Per ogni messaggio cancellato, occorre diminuire di 1 il numero di 
                           // messaggi POP3 disponibili e memorizzati nel server
                           
                           Pop3_NrMessagesAvailable--;
                       }
                   }
             } 
             
             // Adesso memorizza il numero di messaggi che sono rimasti nel server nel
             // registro baMailChecker_HyphActualMailMessages del mail checker
             
             baMailChecker_HyphActualMailMessages = Pop3_NrMessagesAvailable;
             if (baMailChecker_HyphActualMailMessages<0) baMailChecker_HyphActualMailMessages=0;       // Non dovrebbe mai accadere
             
             baMailChecker_NrTimesAlreadySignaled = 0;
       }
       else
       {
             // Durante la fase precedente, non siamo riusciti a scaricare tutte le intestazioni,
             // probabilmente per un problema tecnico. Per failsafe, poni a zero il registro
             // baMailChecker_HyphActualMailMessages. Questo provocherà una nuova segnalazione
             // al successivo controllo da parte del mail checker ma... pazienza..
             
             baMailChecker_HyphActualMailMessages = 0;
             baMailChecker_NrTimesAlreadySignaled = 0;
       }
       
       // Chiudi la connessione POP3
       
       popend (PntTo_Pop3_Session); 
       return 0;       
    }
    else   // Impossibile connettersi al server POP3
    {
       return -1;
    }
}

void baMail_FreeMailRegister ()
{
    int Counter;
    
    for (Counter=0; Counter<STD_NR_MAIL_AVAILABLE; Counter++)
    {
        if (baMailClnRegister [Counter].IsAvailable) 
        {
            if (baMailClnRegister [Counter].MsgHeader) 
               free (baMailClnRegister [Counter].MsgHeader);
            
            if (baMailClnRegister [Counter].Message)
               free (baMailClnRegister [Counter].Message);
        }
    }   
}

#endif

/*
    ---------------------------------------------------------------------------
    RIPRODUZIONE MESSAGGI WORD BY WORD CON OUTPUT GRAFICO
    ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

int baMail_ReproduceMessageWordByWord_SingleLine (char *Title, char *Message)
{
   // Destinato ad accogliere i dati sulla pressione del pad
   struct ndDataControllerType ndPadRecord;
   
   char baMail_WndSpeaker;
   
   int  stringbuffer [64];
   int  whereifound;
   int  start = 0;
   int  posx;
     
   char YouCanExit = 0;
   char Answer;
  
       
   baMail_WndSpeaker = ndLP_CreateWindow (Windows_MaxScreenX - 280, Windows_MaxScreenY - 100, Windows_MaxScreenX - 10, Windows_MaxScreenY - 10,
                                                   Title, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY10, COLOR_GRAY10, 0); 
                                
   if (baMail_WndSpeaker>=0)
   {
         ndLP_MaximizeWindow (baMail_WndSpeaker);
   
         audengServerIsBusy=1;
         Answer=0;
       
         while (!YouCanExit)
         {
              // Pulisci la finestra
                 
              ndWS_DrawRectangle (5, 15, 255, 55, COLOR_GRAY10, COLOR_GRAY10, baMail_WndSpeaker, NORENDER); 
            
              memset (stringbuffer, 0, 63);
              whereifound = baSearch (Message, " ", start);
              
              if (whereifound>=0)
              {
                  ndStrCpyBounded (stringbuffer, Message, start, whereifound, 0);
                  start = whereifound+1;          
              }
              else
              {
                  ndStrCpyBounded (stringbuffer, Message, start, strlen(Message)-1, 0);
                  YouCanExit=1;
              }
              
              ndWS_GrWriteLn ( (240-8*(strlen(stringbuffer)))/2, 21, stringbuffer, COLOR_GRAY06, COLOR_GRAY10, baMail_WndSpeaker, NDKEY_SETFONT (3) | RENDER);  
              
              flite_text_to_speech (stringbuffer, VoiceObj, "play"); 
               
              // Il cieco può, in qualsiasi momento, interrompere la riproduzione premendo il
              // tasto triangolo
              ndHAL_GetPad (&ndPadRecord);
              
              if (ndPadRecord.Buttons & ND_BUTTON3) 
              {
                 Answer=-2;
                 break;
              } 
         }
         
         audengServerIsBusy=0;
        
         ndLP_DestroyWindow (baMail_WndSpeaker);        // Disattiva la finestra
         return Answer;  
   }
   else return -1;
} 

int baMail_ReproduceMessageWordByWord_NChars (char *Title, char *Message, int NrChars)
{
   // Destinato ad accogliere i dati sulla pressione del pad
   struct ndDataControllerType ndPadRecord;
   
   char baMail_WndSpeaker;
   
   int  stringbuffer [64], posx;
   int  Counter, a, b;
     
   char YouCanExit = 0;
   char Answer;
  
   char Letter;  
   char Reproduce;
   char State;
           
   baMail_WndSpeaker = ndLP_CreateWindow (Windows_MaxScreenX - 280, Windows_MaxScreenY - 100, Windows_MaxScreenX - 10, Windows_MaxScreenY - 10,
                                                   Title, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY10, COLOR_GRAY10, 0); 
                                
   if (baMail_WndSpeaker>=0)
   {
          ndLP_MaximizeWindow (baMail_WndSpeaker);
   
          audengServerIsBusy=1;
          Answer=0;
       
          // Inizia una scansione del buffer carattere per carattere
          
          State     = 0;
          Reproduce = 0;
          
          for (Counter=0; Counter<=NrChars; Counter++)   
          // E' necessario che il ciclo for includa Counter=NrChars per prendere lo zero finale           
          {
             Letter = *(Message+Counter);
             
             if (!State)
             {
                  if ( isalpha (Letter) || isdigit (Letter) )
                  {
                       State=1;
                       a = Counter;
                       
                       continue;
                  }      
             }
             else
             {
                  if ( ! ( isalpha (Letter) || isdigit (Letter) ) ) 
                  {
                       State=0;
                       b = Counter-1;
                  
                       // Provvedi a riprodurre la parola
                       ndStrCpyBounded (stringbuffer, Message, a, b, 0);
                  
                       Reproduce=1;
                  }
             }
                    
             if (Reproduce)
             {     
                  // Pulisci la finestra....
                  ndWS_DrawRectangle (5, 15, 255, 55, COLOR_GRAY10, COLOR_GRAY10, baMail_WndSpeaker, NORENDER); 

                  // ... ed indica la parola riprodotta
                  ndWS_GrWriteLn ( (240-8*(strlen(stringbuffer)))/2, 21, stringbuffer, COLOR_GRAY06, COLOR_GRAY10, baMail_WndSpeaker, NDKEY_SETFONT (3) | RENDER);  
  
                  flite_text_to_speech (stringbuffer, VoiceObj, "play"); 
   
                  // Il cieco può, in qualsiasi momento, interrompere la riproduzione premendo il
                  // tasto triangolo
                  ndHAL_GetPad (&ndPadRecord);
                  
                  if (ndPadRecord.Buttons & ND_BUTTON3) 
                  {
                     Answer=-2;
                     break;
                  }
                  
                  Reproduce = 0;
             } 
     
          }
         
          audengServerIsBusy=0;
        
          ndLP_DestroyWindow (baMail_WndSpeaker);        // Disattiva la finestra
          return Answer;  
   }
   else return -1;
} 

#endif





/*
    ---------------------------------------------------------------------------
    BARRE DI OUTPUT
    ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

char baMail_OutputBar_Author (char *Author, char RenderNow)
{
    char InternalBuffer [45];
    
    ndWS_DrawRectangle (5, 15, 380, 27, COLOR_GRAY15, COLOR_GRAY20, baMailClient_Window, NORENDER);
    
    ndStrCpy (InternalBuffer, Author, 45, 0);
    ndWS_GrPrintLn (8,  17, COLOR_BLACK, COLOR_GRAY15, baMailClient_Window, RenderNow, "%s", InternalBuffer);
}

char baMail_OutputBar_Object (char *Object, char RenderNow)
{
    char InternalBuffer [45];
    
    ndWS_DrawRectangle (5, 45, 380, 57, COLOR_GRAY15, COLOR_GRAY20, baMailClient_Window, NORENDER);
    
    ndStrCpy (InternalBuffer, Object, 45, 0);
    ndWS_GrPrintLn (8,  47, COLOR_BLACK, COLOR_GRAY15, baMailClient_Window, RenderNow, "%s", InternalBuffer);
}

char baMail_OutputBar_Message (char *Message, int NrChars, char RenderNow)
{
    int  Counter;
    char Letter;
    
    int  PosX=0;
    int  PosY=0;
    
    ndWS_DrawRectangle (5, 75, 380, 215, COLOR_GRAY15, COLOR_GRAY20, baMailClient_Window, NORENDER);
      
    for (Counter=0; Counter<NrChars; Counter++)
    {
        Letter = *(Message+Counter);
        
        if ((Letter!='\n') && (Letter!='\r'))
        {
            if ((PosX<45) && (PosY<14)) 
            {
               ndWS_GrWriteChar (8 + 8*PosX, 77 + 10*PosY, Letter, COLOR_BLACK, COLOR_GRAY15, baMailClient_Window, NORENDER);             
               PosX++;
            }
        }
        else
        {
            PosX=0;
            PosY++;
            
            if (PosY==14) break;
        }
    }  
    
    if (RenderNow) XWindowRender (baMailClient_Window);
}

void baMail_OutputBar_MessageError (char *ErrMessage, char RenderNow)
{
    int  Counter;
    char Letter;
    
    int  PosX=0;
    int  PosY=0;
    
    ndWS_DrawRectangle (5, 75, 380, 215, COLOR_GRAY15, COLOR_GRAY20, baMailClient_Window, NORENDER);
      
    for (Counter=0; Counter<strlen(ErrMessage); Counter++)
    {
        Letter = *(ErrMessage+Counter);
        
        if (Letter!='\n')
        {
            if ((PosX<45) && (PosY<14)) 
            {
               ndWS_GrWriteChar (8 + 8*PosX, 77 + 10*PosY, Letter, COLOR_RED, COLOR_GRAY15, baMailClient_Window, NORENDER);             
               PosX++;
            }
        }
        else
        {
            PosX=0;
            PosY++;
            
            if (PosY==14) break;
        }
    }  
    
    if (RenderNow) XWindowRender (baMailClient_Window); 
}

void baMail_ClearGraphInterface ()
{
    ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
                             
    ndWS_GrWriteLn (5, 5, "Author", COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER);
    baMail_OutputBar_Author ("", NORENDER);
    
    ndWS_GrWriteLn (5, 35, "Subject", COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER);
    baMail_OutputBar_Object ("", NORENDER); 
    
    ndWS_GrWriteLn (5, 65, "Message", COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER);
    baMail_OutputBar_Message ("", 0, NORENDER); 
    
    XWindowRender (baMailClient_Window);
}

#endif

/*
    ---------------------------------------------------------------------------
    INIZIO DEL CODICE DEL CLIENT
    ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

void baMail_StartASessionOfTheClient ()
{
    char  InternalBuffer [256];
    
    int   ErrRep, DelErrRep;
    char *ErrMsg;
    char  UserAnswer;
    
    char *Pop3Header, *Pop3Message, *Pop3OutputMessage;
    
    char  Pop3Sender [256];
    char  Pop3Object [256];
    int   Pop3Position;
    int   Pop3NrBytes;
    int   Pop3OutputMsgLen;
    
    int   Pop3Counter;
    
    ndLP_EnableTrasparency ();
    
    baMailClient_Window = ndLP_CreateWindow (10, 6, 410, 255, "Mail client", COLOR_WHITE, COLOR_GREEN, COLOR_GRAY21, COLOR_GRAY21, 0);  
    
    if (baMailClient_Window>=0)
    {
       ndLP_MaximizeWindow (baMailClient_Window);
    
       // Tenta di creare una connessione dedicata ad Internet per scaricare l'email
       ErrRep = baMail_ConnectToNetForMailDownload ();
       
       if (!ErrRep)
       {
           ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
           ndWS_GrPrintLn (5,  5, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "Connecting to the POP3 server:");
           ndWS_GrPrintLn (5, 15, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "%s", SystemOptPacket.Pop3ServerAddr);
           XWindowRender (baMailClient_Window);
           
           // FASE 1: Provvedi a connetterti al server POP3 ed a scaricare in locale
           // tutte le mail, determinando il numero di mail disponibili
           
           ErrRep = baMail_CreatePop3SvrAndDownloadMail();     
           
           if (!ErrRep)
           {
                ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
                ndWS_GrWriteLn (5,  5, "Connection established", COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, RENDER);
                ndWS_GrWriteLn (5, 15, "Informations acquired.", COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, RENDER);
                ndDelay (0.5);
           
                if (Pop3_NrMessagesAvailable!=0)
                {
                     ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
                     ndWS_GrPrintLn (5,  5, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "There are %d new messages on your server", Pop3_NrMessagesAvailable);
                     ndWS_GrPrintLn (5, 15, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, RENDER, "Do you want to listen the new messages now ?");
                     
                     baaud_ThereAreNNewMessagesOnMailSvr (Pop3_NrMessagesAvailable);
                     ndHAL_Delay (1);
                                          
                     // Richiedi per mezzo del sistema di controllo vocale se si
                     // vogliono ascoltare i messaggi oppure no
                     
                     UserAnswer = baVoiceRec_AnswerYesNo ( &baaud_DoYouWantToListenNewMessages );
                     
                     // Arrivati a questo punto, l'utente ha risposto sì oppure no
                     
                     if (UserAnswer==1)   // L'utente ha risposto sì
                     {
                             // Inizia la riproduzione del contenuto delle email 
                             
                             Pop3Counter=0;
                             
                             while (Pop3Counter<STD_NR_MAIL_AVAILABLE)
                             {
                                if (baMailClnRegister [Pop3Counter].IsAvailable)
                                {
                                          _ReadThisMail:  
                                                
                                                // L'intestazione è già in locale: tenta di determinare il mittente
                                                                                                 
                                                ErrRep = baMail_VMIME_GetMailSender (baMailClnRegister [Pop3Counter].Message, Pop3Sender);
                                                
                                                if (!ErrRep)     // Il mittente è stato determinato
                                                {
                                                    // Pulisci l'interfaccia grafica
                                                    
                                                    baMail_ClearGraphInterface ();
                                                    
                                                    // Segnala il nome dell'intestatario della prossima mail
                                                    
                                                    baaud_TheNextMailIsSentBy ();
                                                    baMail_OutputBar_Author (Pop3Sender, RENDER);
                                                    ndHAL_Delay (3.0);
                                                    
                                                    baaud_ReproduceAMessageWordByWord (Pop3Sender);
                                                    
                                                    // Richiedi per mezzo del sistema di controllo vocale se si
                                                    // vogliono ascoltare i messaggi di questo sender oppure no
                                                    
                                                    UserAnswer = baVoiceRec_AnswerYesNo ( &baaud_DoYouWantToListenThisMessage );               
                                                    
                                                    if (UserAnswer==1)   // L'utente ha risposto sì
                                                    {
                                                         // Tenta di determinare l'oggetto
                                                         
                                                         ErrRep = baMail_VMIME_GetMailSubject (baMailClnRegister [Pop3Counter].Message, Pop3Object);
                                                         
                                                         if (!ErrRep)    // L'oggetto della mail è stato determinato
                                                         {   
                                                              Pop3OutputMessage = malloc (baMailClnRegister [Pop3Counter].Pop3NrBytes);
                                                              
                                                              if (Pop3OutputMessage)
                                                              {
                                                                  ErrRep=baMail_VMIME_DecodeMessageContent (baMailClnRegister [Pop3Counter].Message, Pop3OutputMessage, &Pop3OutputMsgLen, baMailClnRegister [Pop3Counter].Pop3NrBytes);
                                                                  
                                                                  if (!ErrRep)
                                                                  {
                                                                      baMail_OutputBar_Object  (Pop3Object, NORENDER);
                                                                      
                                                                      if (Pop3OutputMsgLen>0)
                                                                         baMail_OutputBar_Message (Pop3OutputMessage, Pop3OutputMsgLen, RENDER);
                                                                      else
                                                                         baMail_OutputBar_MessageError ("Impossible to decode the mail content", RENDER);    
                                                      
                                                                      // Provvedi a leggere il titolo dell'oggetto della mail
                                                      
                                                                      sprintf (InternalBuffer, "Subject: %s", Pop3Object);
                                                                      ErrRep=baMail_ReproduceMessageWordByWord_SingleLine ("Subject", InternalBuffer);
                                                                      
                                                                      if (!ErrRep)          // L'utente non ha premuto il testo Triangle per interrompere la lettura
                                                                      {
                                                                           // Provvedi a riprodurre il corpo del messaggio, oppure segnala che non sei
                                                                           // stato in grado di decodificarlo
                                                                           
                                                                           if (Pop3OutputMsgLen>0)
                                                                              baMail_ReproduceMessageWordByWord_NChars ("Body of the message", Pop3OutputMessage, Pop3OutputMsgLen);
                                                                           else
                                                                           {
                                                                              baaud_ICannotDecodeMailContent (); 
                                                                              ndHAL_Delay (3.0);
                                                                           }
                                                                      }
                                                                  }
                                                                  
                                                                  // Il buffer di output POP3 non serve più: la mail già è stata letta
                                                                  
                                                                  free (Pop3OutputMessage);
                                                                  
                                                                  // Adesso è necessario richiedere se si vuole riascoltare il messaggio
                                                                  // Se l'utente risponde sì, provvede a rieseguire l'operazione di ascolto
                                                                  
                                                                  UserAnswer = baVoiceRec_AnswerYesNo ( &baaud_DoYouWantToListenThisMessageAgain );
                                                                  if (UserAnswer==1) goto _ReadThisMail;
                                                                  
                                                              }
                                                              else   // Non riesco ad allocare lo spazio in memoria per il buffer di output
                                                              {
                                                                  ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
                                                                  ndWS_GrPrintLn (5,  5, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "Internal error:");
                                                                  ndWS_GrPrintLn (5, 15, COLOR_RED,   COLOR_GRAY21, baMailClient_Window, RENDER,   "I cannot allocate memory buffer");
                                  
                                                                  baaud_InternalError ();
                                                                  ndHAL_Delay (3.0);  
                                                              }           
                                                         }
                                                         else            // Impossibile determinare l'oggetto della mail
                                                         {
                                                              ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
                                                              ndWS_GrPrintLn (5,  5, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "Internal error:");
                                                              ndWS_GrPrintLn (5, 15, COLOR_RED,   COLOR_GRAY21, baMailClient_Window, RENDER,   "I cannot determine subject");
                                  
                                                              baaud_ICannotDetermineSubject ();
                                                              ndHAL_Delay (3.0);         
                                                         }                
                                                    }
                                                    
                                                    // Quando il controllo arriva qui, la mail è stata letta
                                                    // oppure è stato eseguito uno skip 
                                                    
                                                    // Ora bisogna richiedere se si vuole cancellare questo messaggio dal
                                                    // server
                                                                  
                                                    UserAnswer = baVoiceRec_AnswerYesNo ( &baaud_DoYouWantToDeleteThisMessage );
                                                                  
                                                    // Se l'utente ha chiesto la cancellazione, segna il flag: questo
                                                    // provocherà la cancellazione al passaggio successivo 
                                                    if (UserAnswer == 1) 
                                                    {
                                                       baaud_ThisMessageWillBeDeleted ();
                                                       ndHAL_Delay (3.0); 
                                                       
                                                       baMailClnRegister [Pop3Counter].YouMustDeleteIt=1;
                                                    }         
                                                }
                                                else   // Non è stato possibile determinare il mittente del messaggio
                                                {
                                                    ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
                                                    ndWS_GrPrintLn (5,  5, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "Internal error:");
                                                    ndWS_GrPrintLn (5, 15, COLOR_RED,   COLOR_GRAY21, baMailClient_Window, RENDER,   "I cannot determine sender");
                                  
                                                    baaud_ICannotDetermineSender ();
                                                    ndHAL_Delay (3.0);
                                                }
                                       
                                }   // End of IsAvailable  
                             
                                // Provvedi ad analizzare la mail successiva
                                Pop3Counter++;
                             
                             }   // End of while cycle        
                     
                             // FASE 2: Provvedi a cancellare dal server POP3 i messaggi che
                             // erano stati richiesti, e nello stesso tempo ricalcola il numero
                             // di messaggi che sono ancora nel server
                             
                             DelErrRep = baMail_ManageClosingOperations ();
                             
                             // Se la routine precedente restituisce 0, vuol dire che la
                             // cancellazione dei messaggi obsoleti ed il ricalcolo dei 
                             // messaggi ancora sul server hanno avuto successo: cancelliamo
                             // la microicona di segnalazione
                             
                             if (!DelErrRep) baMicroIcons_Disable (MICROICON__UNREADED_MAILS, RENDER);  
                     
                     }   // Do you want to listen all messages now ?
                     
                     // Quando il controllo arriva qui, vuol dire che il lavoro è finito
                     
                     ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
                     ndWS_GrPrintLn (5,  5, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "Deleting mails from POP3 server", Pop3_NrMessagesAvailable);
                     
                     baMail_FreeMailRegister ();
                     baMail_DisconnectToNetForMailDownload (); 
                     
                     // L'emissione di un beep avverte l'utente che il sistema ha terminato il suo
                     // lavoro
                     
                     ndHAL_SND_Beep (2, 8);              
                }
                else    // Non c'è alcun messaggio sul server POP3
                {
                     ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
                     ndWS_GrPrintLn (5,  5, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "There is no new message on your");
                     ndWS_GrPrintLn (5, 15, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, RENDER,   "POP3 server");
                          
                     baaud_ThereIsNoNewMailForYou ();
                     ndHAL_Delay (3.0);
               
                     baMail_DisconnectToNetForMailDownload ();    
                }                 
           }
           else    // Non è stato possibile eseguire la connessione
           {
               ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
               ndWS_GrPrintLn (5,  5, COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, NORENDER, "Impossible to contact the POP3 server");
               ndWS_GrPrintLn (5, 15, COLOR_RED,   COLOR_GRAY21, baMailClient_Window, RENDER, "Error message: %s", ErrMsg);
                          
               baaud_ImpossibleToConnectToThePop3Server ();
               ndHAL_Delay (3.0);
               
               baMail_DisconnectToNetForMailDownload ();
           }
                 
       }
       else    // Non è stato possibile creare una connessione per scaricare le email
       {
           ndWS_DrawRectangle (5, 5, 380, 215, COLOR_GRAY21, COLOR_GRAY21, baMailClient_Window, NORENDER); 
           ndWS_GrWriteLn (5, 5, "Impossible to establish a network connection", COLOR_BLACK, COLOR_GRAY21, baMailClient_Window, RENDER);
           
           baaud_WrongNetworkConfig ();
           ndHAL_Delay (3.0);
       }
       
       // Quando il controllo arriva qui, vuol dire che il processo di lettura è già
       // terminato 
       
       ndLP_DestroyWindow (baMailClient_Window);    
    } 
    
    ndLP_DisableTrasparency ();
}

#endif

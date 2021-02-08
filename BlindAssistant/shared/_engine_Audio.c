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
   AUDIO ENGINE
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
   VARIABILI INTERNE
   ----------------------------------------------------------------------------
*/ 

static struct ndMediaObj_Type AudioMsgObj;

int  audengThreadID;
int  audengEventID; 
int  audengYouCanExit;
int  audengServerIsBusy; 

// Struttura destinata a contenere le informazioni passate all'AudioServer

struct audengInfoPacket_Type
{
     int  PosX, PosY;
     int  NrSlot;
     int  Message;
     char *NameAudioMsg0;
};   

struct audengInfoPacket_Type audengInfoPacket;



/*
   ----------------------------------------------------------------------------
   CODICE SERVER DELL'AUDIOSERVER
   ----------------------------------------------------------------------------
*/ 


int baAudioServer (SceSize args, void *argp)
{
    char NameFile0 [256];
    char NameFile1 [16];
    char MsgSample;
    int  ErrRep;
    
    audengYouCanExit=0;
    
    while (!audengYouCanExit)
    {
         // Attende che ci sia un messaggio in attesa
         sceKernelWaitEventFlag (audengEventID, AUDIOSVR_MSG_IS_READY, PSP_EVENT_WAITAND | PSP_EVENT_WAITCLEAR, 0, 0); 
                                 
         // Quando il sistema arriva a questo punto, vuol dire che c'è un messaggio in attesa
         audengServerIsBusy=1;      // Messaggio raccolto: locka il thread in modo che esso appaia impegnato
         
         switch (audengInfoPacket.Message)
         {
             case AUDIOMSG_WELLCOME_TO_BA:
             case AUDIOMSG_VISILAB_RELEASED_UPD: 
             case AUDIOMSG_I_LL_DOWNLOAD_IT_NOW:
             case AUDIOMSG_DOWNLOAD_SUCCESS:             
             case AUDIOMSG_DOWNLOAD_FAILED:
             case AUDIOMSG_WRONG_NETWORK_CONFIG: 
             case AUDIOMSG_I_LL_INSTALL_NOW:             
             case AUDIOMSG_INSTALL_COMPLETE:  
             case AUDIOMSG_THE_SYSTEM_WILL_BE_REBOOTED: 
             case AUDIOMSG_WAIT: 
             case AUDIOMSG_GO:
             case AUDIOMSG_WAIT_I_AM_FIXING_A_PROBLEM:
             case AUDIOMSG_PRESS_LTRIGGER_OR_RTRIGGER:
             case AUDIOMSG_LOADING:    
             case AUDIOMSG_NOLIGHT: 
             case AUDIOMSG_I_DONT_KNOW_WHERE_YOU_ARE: 
             case AUDIOMSG_TRASMISSION_COMPLETE:
             case AUDIOMSG_BLINDSERVER_IS_NOT_CONNECTED:
             case AUDIOMSG_IMPOSSIBLE_TO_DOWNLOAD_DICT:
             case AUDIOMSG_PRESS_SQUARE_AND_SAY_THE_CMD:          
             case AUDIOMSG_AVAILABLE_COMMANDS_ARE:                
             case AUDIOMSG_I_AM_LISTENING_FOR_YOUR_NEXT_COMMAND:  
             case AUDIOMSG_COMMAND_NOT_RECOGNIZED: 
             case AUDIOMSG_VOICEREC_SYSTEM_NOT_SUPPORTED: 
             case AUDIOMSG_ENABLE_NO_FUNCTION_MODE:               
             case AUDIOMSG_ENABLE_FACE_RECOGNITION_SYSTEM:        
             case AUDIOMSG_ENABLE_POS_RECOGNITION_SYSTEM:        
             case AUDIOMSG_ENABLE_OPTICAL_CHAR_RECOGNITION:       
             case AUDIOMSG_ENABLE_COLOR_SCANNER:                  
             case AUDIOMSG_ENABLE_MAIL_READER:                    
             case AUDIOMSG_ENABLE_TICKETS_SCANNER:
             case AUDIOMSG_ENABLE_DATA_MATRIX_SCANNER:            
             case AUDIOMSG_NO_FUNCTION_MODE_ENABLED:              
             case AUDIOMSG_FACE_RECOGNITION_SYSTEM_ENABLED:       
             case AUDIOMSG_POS_RECOGNITION_SYSTEM_ENABLED:       
             case AUDIOMSG_OPTICAL_CHAR_RECOGNITION_ENABLED:      
             case AUDIOMSG_COLOR_SCANNER_ENABLED:                 
             case AUDIOMSG_MAIL_READER_ENABLED:                   
             case AUDIOMSG_TICKET_SCANNER_ENABLED:
             case AUDIOMSG_DATA_MATRIX_SCANNER_ENABLED:
             case AUDIOMSG_I_CANNOT_CONTACT_POP3_SERVER:
             case AUDIOMSG_THERE_IS_NO_NEW_MAIL_FOR_YOU:
             case AUDIOMSG_DO_YOU_WANT_TO_LISTEN_NEW_MESSAGES:
             case AUDIOMSG_THE_NEXT_MAIL_IS_SENT_BY:
             case AUDIOMSG_DO_YOU_WANT_TO_LISTEN_THIS_MESSAGE:
             case AUDIOMSG_DO_YOU_WANT_TO_LISTEN_AGAIN:
             case AUDIOMSG_DO_YOU_WANT_TO_DELETE_THIS_MESSAGE:
             case AUDIOMSG_THIS_MESSAGE_WILL_BE_DELETED:
             case AUDIOMSG_I_CANNOT_DETERMINE_SENDER:
             case AUDIOMSG_I_CANNOT_DETERMINE_SUBJECT:
             case AUDIOMSG_INTERNAL_ERROR:
             case AUDIOMSG_I_CANNOT_DECODE_MAIL_CONTENT:
             case AUDIOMSG_I_DISCOVERED_YOU_HAVE_UNREADED_MAIL:
             case AUDIOMSG_NDTESSERACT_INIT_ERROR:
             case AUDIOMSG_NDTESSERACT_INTERNAL_ERROR:
             case AUDIOMSG_PROCEED:
             case AUDIOMSG_PROCEED_00_PERCENT:
             case AUDIOMSG_PROCEED_10_PERCENT:
             case AUDIOMSG_PROCEED_20_PERCENT:
             case AUDIOMSG_PROCEED_30_PERCENT: 
             case AUDIOMSG_PROCEED_40_PERCENT:
             case AUDIOMSG_PROCEED_50_PERCENT:
             case AUDIOMSG_PROCEED_60_PERCENT:
             case AUDIOMSG_PROCEED_70_PERCENT:
             case AUDIOMSG_PROCEED_80_PERCENT:
             case AUDIOMSG_PROCEED_90_PERCENT: 
             case AUDIOMSG_PROCEED_100_PERCENT:                      
             {
                    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SERVICEMSG/MSG");
                    _itoa  (audengInfoPacket.Message, &NameFile1, 10);
                    strcat (NameFile0, NameFile1);
                    strcat (NameFile0, ".WAV");
                  
                    ErrRep=ndHAL_MEDIA_LoadFile (&NameFile0, -1, &AudioMsgObj, 0, 0);
                    
                    if (!ErrRep)
                    {
                        ndHAL_MEDIA_Play (&AudioMsgObj, ND_LOCK_UNTIL_SONG_FINISH);
                        // Esegui la riproduzione del file
                        
                        ndHAL_MEDIA_UnLoad (&AudioMsgObj);
                        // Scarica l'oggetto relativo al vecchio file         
                    }
             
                    break;
             }
             
             
             case AUDIOMSG_SIGNAL_A_PERSON: 
             {
                    // Genera il nome del file che deve essere decodificato
                    if ((audengInfoPacket.PosX>=STD_FACEREC__CAMERAIMG__DIMX*0.00) && (audengInfoPacket.PosX<STD_FACEREC__CAMERAIMG__DIMX*0.42)) MsgSample=0;
                    if ((audengInfoPacket.PosX>=STD_FACEREC__CAMERAIMG__DIMX*0.42) && (audengInfoPacket.PosX<STD_FACEREC__CAMERAIMG__DIMX*0.58)) MsgSample=1;
                    if ((audengInfoPacket.PosX>=STD_FACEREC__CAMERAIMG__DIMX*0.58) && (audengInfoPacket.PosX<STD_FACEREC__CAMERAIMG__DIMX*1.00)) MsgSample=2;       
                    
                    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SLOT");
                    _itoa  (audengInfoPacket.NrSlot, &NameFile1, 10);
                    strcat (NameFile0, NameFile1);
                    strcat (NameFile0, "/VOICEMSG");
                    _itoa  (MsgSample, &NameFile1, 10);   
                    strcat (NameFile0, NameFile1);
                    strcat (NameFile0, ".WAV");
                    
                    ErrRep=ndHAL_MEDIA_LoadFile (&NameFile0, -1, &AudioMsgObj, 0, 0);
                    
                    if (!ErrRep)
                    {
                        ndHAL_MEDIA_Play (&AudioMsgObj, ND_LOCK_UNTIL_SONG_FINISH);
                        // Esegui la riproduzione del file
                        
                        ndHAL_MEDIA_UnLoad (&AudioMsgObj);
                        // Scarica l'oggetto relativo al vecchio file         
                    }
             
                    break;
             }  
             
             case AUDIOMSG_YOU_ARE_IN_THIS_ROOM:
             {
                   strcpy  (NameFile0, "ms0:/BLINDASSISTANT/AUDCACHEFORPLACESMSG/"); 
                   strcat  (NameFile0, "$PLACEMSG$");
                   strcat  (NameFile0, audengInfoPacket.NameAudioMsg0 );
                   strcat  (NameFile0, ".WAV");
     
                   ErrRep=ndHAL_MEDIA_LoadFile (&NameFile0, -1, &AudioMsgObj, 0, 0);
                        
                   if (!ErrRep)
                   {
                        ndHAL_MEDIA_Play (&AudioMsgObj, ND_LOCK_UNTIL_SONG_FINISH);
                        // Esegui la riproduzione del file
                        
                        ndHAL_MEDIA_UnLoad (&AudioMsgObj);
                        // Scarica l'oggetto relativo al vecchio file         
                   }
               
                   break;
             } 
             
         }
         
         // Libera il server
         audengServerIsBusy=0;
    }
    
    return 0;
}


int baReStartAudioEngine ()
{
    // Creazione del thread che contiene l'Audio Server
    
    audengServerIsBusy   = 0;
    baMainThreadPriority  = sceKernelGetThreadCurrentPriority ();
    
    audengThreadID = sceKernelCreateThread ("baAudioServer", &baAudioServer, baMainThreadPriority, 16384, 
                                                   PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU, 0); 
                                                 
    sceKernelStartThread (audengThreadID, 0, 0);
    
    // Creazione del field eventi per l'Audio Server
    
    audengEventID = sceKernelCreateEventFlag ("baAudioSvrEventID", PSP_EVENT_WAITMULTIPLE, 0, 0); 
}

void baExitAudioEngine ()
{
    audengYouCanExit=1;
    sceKernelDelayThread (500000);   // Attende il tempo affinchè il thread dell'audio engine possa concludersi    
} 



/*
   ----------------------------------------------------------------------------
   CODICE CLIENT DELL'AUDIOSERVER
   ----------------------------------------------------------------------------
*/ 


void baaud_WellcomeToBlindAssistant (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_WELLCOME_TO_BA;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}

void baaud_SignalThePresenceOfAPerson (unsigned short int PosX, unsigned short int PosY, int NrElement)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = PosX;
       audengInfoPacket.PosY    = PosY;
       audengInfoPacket.NrSlot  = NrElement;
       audengInfoPacket.Message = AUDIOMSG_SIGNAL_A_PERSON;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   } 
}

void baaud_VisilabHasReleasedAnUpdate (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_VISILAB_RELEASED_UPD;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}

void baaud_IWillDownloadItNow (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_I_LL_DOWNLOAD_IT_NOW;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}

void baaud_DownloadSuccess (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_DOWNLOAD_SUCCESS;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}


void baaud_DownloadFailed (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_DOWNLOAD_FAILED;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}

void baaud_WrongNetworkConfig (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_WRONG_NETWORK_CONFIG;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}

             
void baaud_IWillInstallItNow (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_I_LL_INSTALL_NOW;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}


void baaud_InstallationComplete (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_INSTALL_COMPLETE;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}

 
                                      
void baaud_TheSystemWillBeRebootedNow (void)
{
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
       audengInfoPacket.PosX    = 0;
       audengInfoPacket.PosY    = 0;
       audengInfoPacket.NrSlot  = 0;
       audengInfoPacket.Message = AUDIOMSG_THE_SYSTEM_WILL_BE_REBOOTED;
       
       // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
       // attivarsi
       
       sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                     
   }  
}                                      
   
void baaud_Wait (void)
{
   while (audengServerIsBusy) ndDelay (0.01);
   
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_WAIT;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                      
}


void baaud_Loading (void)
{
   while (audengServerIsBusy) ndDelay (0.01);
   
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_LOADING;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                      
}


void baaud_Go (void)
{
   while (audengServerIsBusy) ndDelay (0.01);
   
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_GO;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                      
}



void baaud_WaitIAmFixingAProblem (void)
{
   while (audengServerIsBusy) ndDelay (0.01);
   
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_WAIT_I_AM_FIXING_A_PROBLEM;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                      
}



void baaud_PressLTriggerOrRTrigger (void)
{
   while (audengServerIsBusy) ndDelay (0.01);
   
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_PRESS_LTRIGGER_OR_RTRIGGER;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);                      
}


void baaud_NoLight (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_NOLIGHT;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_YouAreInThisRoom (char *NameOfTheRoom)
{
   char NameFile0 [256];
   int  ErrRep;
   
   if (!audengServerIsBusy)  // Se l'audio server non è impegnato, altrimenti skippa la richiesta
   {
      // Prima di procedere all'esecuzione del comando, si ricostruisce il nome del file e
      // si verifica che esso sia effettivamente presente sul disco. Se non lo è, allora
      // si avvia una procedura di autoriparazione
      
      strcpy  (NameFile0, "ms0:/BLINDASSISTANT/AUDCACHEFORPLACESMSG/"); 
      strcat  (NameFile0, "$PLACEMSG$");
      strcat  (NameFile0, NameOfTheRoom );
      strcat  (NameFile0, ".WAV");
     
      _TryToPlayRoomAudioMessage:
                   
         if (fexist (NameFile0))
         {
             audengInfoPacket.PosX          = 0;
             audengInfoPacket.PosY          = 0;
             audengInfoPacket.NrSlot        = 0;
             audengInfoPacket.Message       = AUDIOMSG_YOU_ARE_IN_THIS_ROOM;
             audengInfoPacket.NameAudioMsg0 = NameOfTheRoom;
   
             // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa attivarsi
   
             sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);             
         }
         else
         {
             // Se il file non esiste, vuol dire che la cache del place engine
             // non è sincronizzata con il database del BlindServer. Avvia una
             // routine di autoriparazione per cercare di risolvere il problema
         
             ErrRep = SYSFIXER00_SyncPlaceAudioCache (NameOfTheRoom);  
             if (!ErrRep) goto _TryToPlayRoomAudioMessage;    // Risolto! Ora possiamo riprovare    
         }
   }
}

void baaud_IDontKnowWhereYouAre (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_I_DONT_KNOW_WHERE_YOU_ARE;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_TrasmissionComplete (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_TRASMISSION_COMPLETE;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_BlindServerIsNotConnected (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_BLINDSERVER_IS_NOT_CONNECTED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}


void baaud_ImpossibleToDownloadDict (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_IMPOSSIBLE_TO_DOWNLOAD_DICT;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_PressSquareAndSayTheCmd (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_PRESS_SQUARE_AND_SAY_THE_CMD;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_AvailableCommandsAre (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_AVAILABLE_COMMANDS_ARE;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_ListeningForYourNextCommand (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_I_AM_LISTENING_FOR_YOUR_NEXT_COMMAND;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_CommandNotRecognized (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_COMMAND_NOT_RECOGNIZED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_VoiceRecognitionSystemNotSupported (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_VOICEREC_SYSTEM_NOT_SUPPORTED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_EnableNoFunctionMode (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_ENABLE_NO_FUNCTION_MODE;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_EnableFaceRecognitionSystem (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_ENABLE_FACE_RECOGNITION_SYSTEM;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_EnablePosRecognitionSystem (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_ENABLE_POS_RECOGNITION_SYSTEM;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_EnableOpticalCharRecognition (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_ENABLE_OPTICAL_CHAR_RECOGNITION;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_EnableColorScanner (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_ENABLE_COLOR_SCANNER;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_EnableMailReader (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_ENABLE_MAIL_READER;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_EnableTicketsScanner (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_ENABLE_TICKETS_SCANNER;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}


void baaud_EnableDataMatrixScanner (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_ENABLE_DATA_MATRIX_SCANNER;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}


void baaud_NoFunctionModeEnabled (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_NO_FUNCTION_MODE_ENABLED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_FaceRecognitionSystemEnabled (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_FACE_RECOGNITION_SYSTEM_ENABLED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_PosRecognitionSystemEnabled (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_POS_RECOGNITION_SYSTEM_ENABLED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_OpticalCharRecognitionEnabled (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_OPTICAL_CHAR_RECOGNITION_ENABLED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_ColorScannerEnabled (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_COLOR_SCANNER_ENABLED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_MailReaderEnabled (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_MAIL_READER_ENABLED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_TicketScannerEnabled (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_TICKET_SCANNER_ENABLED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_DataMatrixScannerEnabled (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_DATA_MATRIX_SCANNER_ENABLED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}


char baaud_ReproduceASingleWord (char *FirstName, char *SecondaryName)
/*
    Viene eseguito direttamente e non nell'audio engine perchè deve
    agire in modo bloccante sul thread
*/
{
   int ErrRep;
   
   audengServerIsBusy=1;
   
   ErrRep=ndHAL_MEDIA_LoadFile (FirstName, -1, &AudioMsgObj, 0, 0);
                         
   if (!ErrRep)
   {
        ndHAL_MEDIA_Play (&AudioMsgObj, ND_LOCK_UNTIL_SONG_FINISH);
        // Esegui la riproduzione del file
        
        ndHAL_MEDIA_UnLoad (&AudioMsgObj);
        // Scarica l'oggetto relativo al vecchio file         
   }
   
   audengServerIsBusy=0;
   return ErrRep;
}

char baaud_ReproduceASingleWordRT (char *Word)
/*
    Riproduce in realtime il termine richiesto
*/
{
   int ErrRep;
   
   audengServerIsBusy=1;
   
   flite_text_to_speech (Word, VoiceObj, "play");        
                       
   audengServerIsBusy=0;
   return 0;
}


void baaud_ColorScannerResult (int ChannelR, int ChannelG, int ChannelB, int ColorCode)
/*
    Riproduce in realtime il termine richiesto
*/
{
   char Message0 [255];
   int ErrRep;
   
   audengServerIsBusy=1;
   
   sprintf (Message0, "The average color seems %s", baColorInfo [ColorCode].NameColor);
   flite_text_to_speech (Message0, VoiceObj, "play");
                       
   audengServerIsBusy=0;
   return;
}



void baaud_ImpossibleToConnectToThePop3Server (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_I_CANNOT_CONTACT_POP3_SERVER;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}



void baaud_ThereIsNoNewMailForYou (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_THERE_IS_NO_NEW_MAIL_FOR_YOU;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}


void baaud_ThereAreNNewMessagesOnMailSvr (int NrNewMessages)
/*
    Comunica all'utente che vi sono n messaggi in linea
*/
{
   char Message0 [255];
   int ErrRep;
   
   int Thread, Priority;
   
   audengServerIsBusy=1;
   
   Thread   = sceKernelGetThreadId ();
   Priority = sceKernelGetThreadCurrentPriority (); 
   
   sprintf (Message0, "%d new messages", NrNewMessages);
   
   sceKernelChangeThreadPriority (Thread, 11);
   flite_text_to_speech (Message0, VoiceObj, "play");
   sceKernelChangeThreadPriority (Thread, Priority);
                       
   audengServerIsBusy=0;
   return;
}



void baaud_DoYouWantToListenNewMessages (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_DO_YOU_WANT_TO_LISTEN_NEW_MESSAGES;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}


void baaud_TheNextMailIsSentBy (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_THE_NEXT_MAIL_IS_SENT_BY;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}


void baaud_ReproduceAMessageWordByWord (char *Message)
{
     int stringbuffer [64];
     
     int whereifound;
     int start = 0;
     
     char YouCanExit = 0;
     
     audengServerIsBusy=1;
   
     while (!YouCanExit)
     {
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
          
          flite_text_to_speech (stringbuffer, VoiceObj, "play");  
     }
     
     audengServerIsBusy=0;
}



void baaud_DoYouWantToListenThisMessage (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_DO_YOU_WANT_TO_LISTEN_THIS_MESSAGE;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}




void baaud_DoYouWantToListenThisMessageAgain (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_DO_YOU_WANT_TO_LISTEN_AGAIN;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}




void baaud_DoYouWantToDeleteThisMessage (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_DO_YOU_WANT_TO_DELETE_THIS_MESSAGE;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}




void baaud_ThisMessageWillBeDeleted (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_THIS_MESSAGE_WILL_BE_DELETED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}



void baaud_ICannotDetermineSender (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_I_CANNOT_DETERMINE_SENDER;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);    
}

void baaud_ICannotDetermineSubject (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_I_CANNOT_DETERMINE_SUBJECT;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);      
}


void baaud_InternalError (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_INTERNAL_ERROR;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);      
}


void baaud_ICannotDecodeMailContent (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_I_CANNOT_DECODE_MAIL_CONTENT;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);      
}



void baaud_YouHaveUnreadedMails (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_I_DISCOVERED_YOU_HAVE_UNREADED_MAIL;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);      
}


void baaud_ndTessInitError (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_NDTESSERACT_INIT_ERROR;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);      
}


void baaud_ndTessInternalError (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_NDTESSERACT_INTERNAL_ERROR;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);      
}


void baaud_ndProceedMessage (void)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_PROCEED;
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY);      
}


void baaud_ndProceedXXPerCent (int Progress)
{
   audengInfoPacket.PosX    = 0;
   audengInfoPacket.PosY    = 0;
   audengInfoPacket.NrSlot  = 0;
   audengInfoPacket.Message = AUDIOMSG_PROCEED_00_PERCENT + (Progress/10);
   
   // I dati sono memorizzati, adesso setta l'opportuno Flag in modo che l'audio server possa
   // attivarsi
   
   sceKernelSetEventFlag   (audengEventID, AUDIOSVR_MSG_IS_READY); 
   
   // Un ritardo di un secondo è necessario per consentire la lettura del messaggio
   ndHAL_Delay (1);     
}


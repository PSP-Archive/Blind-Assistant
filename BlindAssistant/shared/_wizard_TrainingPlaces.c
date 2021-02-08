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
   TRAINING DEI LUOGHI 
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

char wzplWindow;

char wzplButtonBack,  wzplButtonNext,  wzplButtonErase,   wzplButtonOk;
char wzplButtonOne,   wzplButtonTwo;
char wzplButtonAbort, wzplButtonVoiceTest;
char wzplTxtAreaName;

int  wzplYouCanExit;
int  wzplExitReason;

IplImage *wzpl_cvCameraImage;

#define WZPL_GONEXT      1
#define WZPL_GOBACK      2
#define WZPL_GORESET     3
#define WZPL_REQABORT   -1

char        wzplTmpImgAlreadyInit = 0;
struct      cvTmpPlacePacket_Type cvTmpPlacePacket;

static char TextAreaBuffer [64];
char        wzpl_IsAlreadyOpened=0;

char        wzpl_EnableDedNetConnection;
char        wzpl_EnableDedBldSvrConnection;

/*
   ----------------------------------------------------------------------------
   RIFERIMENTI EXTERN AD IMMAGINI IN FORMATO NDF
   ----------------------------------------------------------------------------
*/ 

extern unsigned int PlaceWizard0_Width;
extern unsigned int PlaceWizard0_Height;
extern unsigned short PlaceWizard0[];


/*
   ----------------------------------------------------------------------------
   INIZIALIZZAZIONE/DEINIZIALIZZAZIONE SISTEMA VIDEO
   ----------------------------------------------------------------------------
*/ 

static void INTERNAL_wzplInitGoCamSystem () 
{
   // Inibisci l'engine video principale. Questo eviterà conflitti.
   baInhibitVideoEngine ();
    
   // Provvedi a settare la corretta risoluzione del sensore video
   cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_480_272);
    
   // Provvedi a settare la modalità video del sensore
   cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, ND_USBCAM_EFFECTMODE_BLACKWHITE);
   
   // Cambia la risoluzione della videocamera
   baChangeCameraResolution (STD_BLDSVR__SAMPLEIMG__DIMX, STD_BLDSVR__SAMPLEIMG__DIMY, 8);                      
}

static void INTERNAL_wzplEND_DeInitGoCamSystem () 
{
   // Quando l'engine video verrà riavviato, l'opzione Force sarà attiva e quindi il
   // sensore verrà riconfigurato per svolgere la funzione attuale
   baDeInhibitVideoEngine ();    
}


/*
   ----------------------------------------------------------------------------
   ALLOCAZIONE IMMAGINE TEMPORANEE
   ----------------------------------------------------------------------------
*/ 

static void INTERNAL_AllocateTmpImages (void)
{
   int Counter;
   
   if (!wzplTmpImgAlreadyInit)  
   {  
      for (Counter=0; Counter<NR_SAMPLES_FOR_PLACE; Counter++)
      {
          cvTmpPlacePacket.Img [Counter]=cvCreateImage( cvSize (STD_BLDSVR__SAMPLEIMG__DIMX, STD_BLDSVR__SAMPLEIMG__DIMY), 8, 1 );  
      }
      
      wzplTmpImgAlreadyInit=1;
   }       
}


/*
   ----------------------------------------------------------------------------
   CODICE INIZIALE PER DISEGNO INTERFACCIA
   ----------------------------------------------------------------------------
*/ 


int INTERNAL_wzplDrawInterface (int Slot)
/*
     Questa routine provvede a disegnare l'interfaccia grafica del wizard per
     il training dei luoghi. La routine restituisce 0 in caso di successo ed 1 in caso
     di errore.
*/
{
    wzplWindow = ndLP_CreateWindow (10, 5, 420, Windows_MaxScreenY-5, "Place training wizard", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wzplWindow>=0)
    {
        ndLP_MaximizeWindow (wzplWindow);
        
        wzplButtonBack  = ndCTRL_CreateButton (335, WindowData [wzplWindow].WSWndHeight - 75, 
                                               385, WindowData [wzplWindow].WSWndHeight - 55,
                                               wzplWindow, "wzplBack", "Back", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED, 
                                               0, 0, NORENDER);
        
        wzplButtonNext  = ndCTRL_CreateButton (335, WindowData [wzplWindow].WSWndHeight - 50, 
                                               385, WindowData [wzplWindow].WSWndHeight - 30,
                                               wzplWindow, "wzplNext", "Next", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED, 
                                               0, 0, NORENDER);
        
        wzplButtonAbort = ndCTRL_CreateButton (335, WindowData [wzplWindow].WSWndHeight - 25, 
                                               385, WindowData [wzplWindow].WSWndHeight - 5,
                                               wzplWindow, "wzplAbort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               0, 0, NORENDER);
                                             
        // Indica lo slot interessato
        ndWS_GrPrintLn (340, 5, COLOR_BLACK, COLOR_GRAY, wzplWindow, NDKEY_SETFONT(3), "%d ", Slot+1);
        
        XWindowRender (wzplWindow);                                     
        return 0;
    }
    else
        return -1;
}


/*
   ----------------------------------------------------------------------------
   CODICE PER ERRORE DI MODALITA'
   ----------------------------------------------------------------------------
*/ 


static ndint64 cb_wzplOk (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit=1;
   return 0;   
}


int INTERNAL_wzplDrawInterfaceModeMismatch (void)
{ 
    wzplWindow = ndLP_CreateWindow (10, 5, 420, Windows_MaxScreenY-5, "Place training wizard", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wzplWindow>=0)
    {
        ndLP_MaximizeWindow (wzplWindow);
        
        ndWS_GrWriteLn (5, 10, "You are not in position recognizer mode.", COLOR_BLUE,  COLOR_GRAY, wzplWindow, NORENDER);
        
        ndWS_GrWriteLn (5, 26, "This function can work only when the   ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
        ndWS_GrWriteLn (5, 34, "position recognizer mode is operative.", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
        ndWS_GrWriteLn (5, 42, "You can switch to this mode going into ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
        ndWS_GrWriteLn (5, 50, "the menu Fnc, Enable position recognizer", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    
        ndWS_GrWriteLn (5, 66, "Now click on Continue and after switching", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
        ndWS_GrWriteLn (5, 74, "retry to select this function.           ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    
        
        wzplButtonOk   = ndCTRL_CreateButton  (310, WindowData [wzplWindow].WSWndHeight - 25, 
                                               385, WindowData [wzplWindow].WSWndHeight - 5,
                                               wzplWindow, "wzplOk", "Continue", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &cb_wzplOk, 0, NORENDER);
                                              
        XWindowRender (wzplWindow);                                     
        return 0;
    }
    else
        return -1;
} 





/*
   ----------------------------------------------------------------------------
   STEP0: TENTATIVO DI CONNESSIONE CON IL BLIND SERVER
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzpl0_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl0_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl0_Next (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = 0;    
}



char INTERNAL_wzpl0_ConnectionWithBlindServer ()
{
     int ErrRep, IntErrRep;
     
     if (!NetworkIsBusy)
     {
             ndWS_GrWriteLn (5,  5, "Trying to connect with BlindServer ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             
             ndWS_GrPrintLn (5, 15, COLOR_BLACK, COLOR_GRAY, wzplWindow, 0, "Connecting to... : %s", SystemOptPacket.BlindServerAddrStr);            
             ndWS_GrPrintLn (5, 25, COLOR_BLACK, COLOR_GRAY, wzplWindow, 0, "Port             : %d", SystemOptPacket.BlindServerPort);            
     
             // Esegui il rendering
             
             XWindowRender (wzplWindow);
             
             // Inizia il processo di connessione. Prima occupati della connessione di rete
             
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
                               ErrRep                          =  0;
                               wzpl_EnableDedNetConnection     =  1;            
                          }
                          else
                          {
                               ErrRep                          =  -2;
                               wzpl_EnableDedNetConnection     =  0;    
                          }
                   }
                   else   // L'access point è impegnato
                   {
                        ErrRep                          = -1;
                        wzpl_EnableDedNetConnection     =  0;  
                   }
             }
             else         // C'è una connessione principale di rete attivata
             {
                   ErrRep                          = 0;
                   wzpl_EnableDedNetConnection     = 0;
             }
             
             // Adesso occupati della connessione BlindServer
             
             if (!MainBldSvrConn_IsOperative)
             {
                   // Non c'è una connessione principale BldSvr attivata.
                   // Provvedi autonomamente al processo di connessione
                   // La routine ConnectToBlindServer provvede automaticamente
                   // al processo di connessione
                   
                   IntErrRep = baBldSvr_TryAConnectToBlindServer (); 
                   
                   if (!IntErrRep)           // Connessione Blind Server attivata
                   {
                       ErrRep                          = 0;
                       wzpl_EnableDedBldSvrConnection  = 1;  
                   }
                   else
                   {
                       ErrRep                          = IntErrRep;
                       wzpl_EnableDedBldSvrConnection  = 0;                           
                   }
             }
             else
             {
                   // C'è una connessione principale BldSvr attivata. 
                   // Nessuna operazione necessaria. Simula successo
                   
                   ErrRep                          = 0;
                   wzpl_EnableDedBldSvrConnection  = 0;
             }
             
             
             if (!ErrRep)    // Processo di connessione riuscito
             {
                 // Impegna il sistema di rete, in modo da impedire conflitti
                 
                 NetworkIsBusy=1;
                 
                 // Disegna l'interfaccia
                 
                 ndWS_GrWriteLn (5, 40, "Connection established", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                 ndWS_GrWriteLn (5, 50, "Press [Next] to continue", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                  
                 // Inibisce il pulsante di Abort e deinibisce il pulsante Next
                 ndCTRL_InhibitButton   (wzplButtonAbort, wzplWindow, NORENDER); 
                 ndCTRL_DeInhibitButton (wzplButtonNext,  wzplWindow, NORENDER); 
            
                 // Setta le callback ai valori necessari
                 ndCTRL_ChangeCallBack (&cb_wzpl0_Next,   0, wzplButtonNext,  wzplWindow, NORENDER); 

                 // Setup della callback di uscita finestra: la callback viene disattivata
                 ndLP_SetupCloseWndCallback (0, 0, 0, wzplWindow);

                 // Esegui il rendering
                 XWindowRender (wzplWindow);
                 
                 // Provvedi all'uso del mouse per uscire
                 
                 wzplYouCanExit=0;
                 ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
                 
                 // Segnala che è tutto ok. Si noti che da questo momento il supporto
                 // di rete è attivato
                 
                 return 0;
             }
             else    // C'è stato un errore di connessione
             {
                 switch (ErrRep)
                 {
                        case -1:
                        {
                             ndWS_GrWriteLn (5, 40, "The access point that has been",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 50, "choosen in Options dialog box is", COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             ndWS_GrWriteLn (5, 60, "empty",                            COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             
                             ndWS_GrWriteLn (5, 80, "Press [Abort] to exit",         COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                             
                             break;
                        }
                        
                        case -2:
                        {
                             ndWS_GrWriteLn (5, 40, "Impossible to access to the   ",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 50, "WI-FI network. Check if your  ",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             ndWS_GrWriteLn (5, 60, "device is near a working WI-FI",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             ndWS_GrWriteLn (5, 70, "access point.                 ",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             
                             ndWS_GrWriteLn (5, 90, "Press [Abort] to exit",         COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                             break;
                        }
                        
                        case ERR_IMPOSSIBLE_TO_DETERMINE_BLDSVR_IP:
                        {
                             ndWS_GrWriteLn (5, 40, "Impossible to determine the IP",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 50, "address of your BlindServer.  ",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             ndWS_GrWriteLn (5, 60, "Check that the address that you",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             ndWS_GrWriteLn (5, 70, "have entered in the Options   ",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             ndWS_GrWriteLn (5, 80, "dialog-box is correct.        ",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER); 
                             
                             ndWS_GrWriteLn (5, 100, "Press [Abort] to exit",         COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                             break;
                        }
                        
                        case ERR_IMPOSSIBLE_TO_CREATE_BLDSVR_SOCKET:
                        {
                             ndWS_GrWriteLn (5, 40, "Troubles in socket opening.   ",   COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER);            
                             
                             ndWS_GrWriteLn (5, 80, "Press [Abort] to exit",         COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                             break;
                        }
                        
                        case ERR_IMPOSSIBLE_TO_CONNECT_TO_BLDSVR:
                        {
                             ndWS_GrWriteLn (5, 40, "I cannot connect to BlindServer",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 50, "Host unreachable.    ",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             
                             ndWS_GrWriteLn (5, 80, "Press [Abort] to exit",         COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                             break;
                        }
                        
                        case ERR_IMPOSSIBLE_TO_SEND_PASSWORD_PACKET:
                        case ERR_IMPOSSIBLE_TO_RECV_PASSWORD_RESPONSE:
                        {
                             ndWS_GrWriteLn (5, 40, "Troubles in send/recv password  ",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 50, "to server. Please retry the     ",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 60, "operation.                      ",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             
                             ndWS_GrWriteLn (5, 80, "Press [Abort] to exit",         COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                             break;
                        }
                        
                        case ERR_AUTHENTIFICATION_FAILED:
                        {
                             ndWS_GrWriteLn (5, 40, "Authentification failed. Check  ",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 50, "that the password is correct.   ",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             
                             ndWS_GrWriteLn (5, 80, "Press [Abort] to exit",         COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                             break;
                        }
                        
                        case ERR_SERVER_VERSION_MISMATCH:
                        {
                             ndWS_GrWriteLn (5, 40, "This version of the core has been",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 50, "designed for a different version ",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 60, "of BlindServer. Check your PC and",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                             ndWS_GrWriteLn (5, 70, "documentation for further details",   COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);            
                        
                             ndWS_GrWriteLn (5, 80, "Press [Abort] to exit",         COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER); 
                             break;
                        }     
                             
                 }    
                 
                 // DeInibisce il solo pulsante di Abort
                 ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
            
                 // Setta le callback ai valori necessari
                 ndCTRL_ChangeCallBack (&cb_wzpl0_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER); 

                 // Setup della callback di uscita finestra
                 ndLP_SetupCloseWndCallback (&cb_wzpl0_CloseWnd, 0, 0, wzplWindow);

                 // Esegui il rendering
                 XWindowRender (wzplWindow);
                 
                 // Provvedi all'uso del mouse per uscire
                 wzplYouCanExit=0;
                 ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
                 
                 return -3;    // Segnala il problema alla routine chiamante
             }     
         
     }
     else   // Il sistema di rete è impegnato
     {
             ndWS_GrWriteLn (5,  5, "In this moment another network    ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 15, "operation is in progress.         ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             
             ndWS_GrWriteLn (5, 35, "Please, retry this operation when ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 45, "the current network operation is  ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 55, "finished (it requires some minutes)", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             
             // DeInibisce il solo pulsante di Abort
             ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
        
             // Setta le callback ai valori necessari
             ndCTRL_ChangeCallBack (&cb_wzpl0_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER); 

             // Setup della callback di uscita finestra
             ndLP_SetupCloseWndCallback (&cb_wzpl0_CloseWnd, 0, 0, wzplWindow);

             // Esegui il rendering
             
             XWindowRender (wzplWindow);
             
             // Provvedi all'uso del mouse per uscire
             
             wzplYouCanExit=0;
             ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
             
             return -2;    // Segnala il fallimento dell'operazione perchè il supporto di
                           // rete era impegnato 
     }
}

/*
   ----------------------------------------------------------------------------
   STEP1: INTERROGAZIONE DEL BLIND SERVER
   ----------------------------------------------------------------------------
*/ 

struct wzpl1_Data_Type
{
       int NrSlot;
       char *NameSlot;
};



/*
   ----------------------------------------------------------------------------
   STEP1a: PRELEVA LE IMMAGINI MEMORIZZATE
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_wzpl1a_VisualizeSamples (char NrGroup, char RenderNow)
{
     // Provvede a visualizzare le immagini prelevate dal BlindServer
     
     // Crea quattro rettangoli per la visualizzazione
     ndWS_DrawRectangle (5,       35,      5+160,       35+65,       COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);      
     ndWS_DrawRectangle (5+160+2, 35,      5+160+2+160, 35+65,       COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);  
     ndWS_DrawRectangle (5,       35+65+2, 5+160,       35+65+2+65,  COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);  
     ndWS_DrawRectangle (5+160+2, 35+65+2, 5+160+2+160, 35+65+2+65,  COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);  
        
     // Visualizza immagini
     zcvShowImage8bit (cvTmpPlacePacket.Img [NrGroup+0], 5,       35,      5+160,       35+65,      wzplWindow, NORENDER);
     zcvShowImage8bit (cvTmpPlacePacket.Img [NrGroup+1], 5+160+2, 35,      5+160+2+160, 35+65,      wzplWindow, NORENDER);
     zcvShowImage8bit (cvTmpPlacePacket.Img [NrGroup+2], 5,       35+65+2, 5+160,       35+65+2+65, wzplWindow, NORENDER);
     zcvShowImage8bit (cvTmpPlacePacket.Img [NrGroup+3], 5+160+2, 35+65+2, 5+160+2+160, 35+65+2+65, wzplWindow, NORENDER);        

     if (RenderNow) XWindowRender (wzplWindow);
}

static ndint64 cb_wzpl1a_ButtonOne (char *StringID, ndint64 InfoField, char WndHandle)
{
    INTERNAL_wzpl1a_VisualizeSamples (0, RENDER);   
}

static ndint64 cb_wzpl1a_ButtonTwo (char *StringID, ndint64 InfoField, char WndHandle)
{
    INTERNAL_wzpl1a_VisualizeSamples (1, RENDER);      
}


static ndint64 cb_wzpl1a_Erase (char *StringID, ndint64 InfoField, char WndHandle)
{
   struct wzpl1_Data_Type *wzpl1_Data = InfoField;     // Recupera il numero di slot dove cancellare
                                                       // ed il nome 
   char NameFileWav [512]; 
   char EraseWndHandle;
   
   
   EraseWndHandle = ndLP_CreateWindow (Windows_MaxScreenX-213, Windows_MaxScreenY-75, Windows_MaxScreenX-13, Windows_MaxScreenY-13, 
                                       "Deleting....", 
                                       COLOR_WHITE, COLOR_GREEN, COLOR_GRAY05, COLOR_GRAY05, NO_RESIZEABLE);
   
   if (EraseWndHandle>=0)
   {
       ndLP_MaximizeWindow (EraseWndHandle);
       
       ndTBAR_DrawProgressBar   (15, 15, 185, 30, 0, 0, 100, COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                            PBAR_PERCENT, EraseWndHandle, RENDER);
       
       // Cancella il contenuto nel server
       
       baBldSvr_SendToSvrEraseCommand (wzpl1_Data->NrSlot);
       
       // Cancella il contenuto nella cache audio
       
       strcpy  (NameFileWav, "ms0:/BLINDASSISTANT/AUDCACHEFORPLACESMSG/"); 
       strcat  (NameFileWav, "$PLACEMSG$");
       strcat  (NameFileWav, wzpl1_Data->NameSlot);
       strcat  (NameFileWav, ".WAV");
     
       remove (NameFileWav);

       // Segnala all'utente che il processo è completato...
       
       ndTBAR_DrawProgressBar   (15, 15, 185, 30, 100, 0, 100, COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                            PBAR_PERCENT, EraseWndHandle, RENDER);
       
       ndLP_DestroyWindow (EraseWndHandle);
   }
   
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GORESET;        // Provocherà il reset dell'interfaccia    
}



static ndint64 cb_wzpl1a_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl1a_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl1a_Next (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = 0;    
}

static void cb_wzpl1a_GraphCallback0 (int MessageData, int Progress)
{
   int TypeMessage = MessageData & 0xFFFF0000;
   int NrSample    = MessageData & 0x0000FFFF;
   
   int TotalProgress;
   float TotalProgressForSample;
   
   TotalProgressForSample = 80.0/NR_SAMPLES_FOR_PLACE; 
   
   switch (TypeMessage)
   {
          case 0:         // C'è una immagine in invio
          {
               TotalProgress = NrSample*TotalProgressForSample + ((float)(Progress)/100.0)*TotalProgressForSample; 
          
               ndTBAR_DrawProgressBar   (5, 80, 325, 92, TotalProgress, 0, 100, 
                                        COLOR_WHITE, COLOR_RED, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, wzplWindow, RENDER); 
                                        
               break;
          } 
          
          case 0x00010000:   // Fasi successive del trasferimento
          {
               ndTBAR_DrawProgressBar   (5, 80, 325, 92, Progress, 0, 100, 
                                        COLOR_WHITE, COLOR_RED, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, wzplWindow, RENDER); 
                                        
               break;
          }   
   }
}


void INTERNAL_wzpl1a_TitleBar (char *Name)
{
     int LenX   = 325;
     int LenStr = strlen (Name)*8;
     int PosX;
     
     ndWS_DrawRectangle (5, 23, 327, 34, COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);
     
     PosX = (LenX - LenStr)/2;
     ndWS_GrWriteLn (5+PosX, 25, Name, COLOR_BLACK, COLOR_WHITE, wzplWindow, NORENDER); 
}

char INTERNAL_wzpl1a_ThereAreDataInTheSlot (char NrSlot)
{
     struct wzpl1_Data_Type wzpl1_Data;
     
     char NameOfThisSlot [256];
     char ErrRep;
     
     
     // Cancella il vecchio contenuto della finestra
     ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);

     ndWS_GrWriteLn (5,  5, "Wait. I am downloading the images   ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 13, "from the Blind Server               ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     
     ndTBAR_DrawProgressBar  (5, 80, 325, 92, 0, 0, 100, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                                         PBAR_PERCENT, wzplWindow, NORENDER);   
    
     // Metti i due dati che bisogna passare alla callback del pulsante Erase in una struct apposita
    
     wzpl1_Data.NrSlot   = NrSlot;
     wzpl1_Data.NameSlot = &NameOfThisSlot;
     
     // Crea il pulsante Erase ed i due pulsanti 1 e 2
    
     wzplButtonErase  = ndCTRL_CreateButton (335, 35, 385, 55, wzplWindow, "wzplErase", "Erase", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, &cb_wzpl1a_Erase, &(wzpl1_Data), NORENDER);
    
     wzplButtonOne    = ndCTRL_CreateButton (355, 70, 385,  90, wzplWindow, "wzplButtonOne", "1", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, &cb_wzpl1a_ButtonOne, 0, NORENDER);
    
     wzplButtonTwo    = ndCTRL_CreateButton (355, 95, 385, 115, wzplWindow, "wzplButtonTwo", "2", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, &cb_wzpl1a_ButtonTwo, 0, NORENDER);

    
     // Inibisci tutti i pulsanti finchè il download non è completato
     ndCTRL_InhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
     ndCTRL_InhibitButton (wzplButtonBack,  wzplWindow, NORENDER); 
     ndCTRL_InhibitButton (wzplButtonNext,  wzplWindow, NORENDER);
     ndCTRL_InhibitButton (wzplButtonErase, wzplWindow, NORENDER);
     ndCTRL_InhibitButton (wzplButtonOne,   wzplWindow, NORENDER);
     ndCTRL_InhibitButton (wzplButtonTwo,   wzplWindow, NORENDER);
     
     // Disattiva la callback di chiusura finestra finchè non viene completato il download
     ndLP_SetupCloseWndCallback (0, 0, 0, wzplWindow);
        
     XWindowRender (wzplWindow);
    
     ErrRep=baBldSvr_RetrieveFromSvrASlotPackage (NrSlot, &cb_wzpl1a_GraphCallback0, &cvTmpPlacePacket, &NameOfThisSlot);
     
     if (!ErrRep)       // Non ci sono errori
     {
             // Cancella il vecchio contenuto della finestra
             ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);

             ndWS_GrWriteLn (5,  3, "This is the content of the slot     ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 13, "in the memory of the BlindServer    ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             
             // Visualizza il nome dello slot
             INTERNAL_wzpl1a_TitleBar (NameOfThisSlot);

             // Visualizza i primi 4 campioni
             INTERNAL_wzpl1a_VisualizeSamples (0, NORENDER);   
             
             // Visualizza messaggi
             ndWS_GrWriteLn (5, 172, "Press on [Next] to overwrite these  ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 182, "informations, [Abort] to exit from  ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 192, "the wizard, [Erase] to delete all   ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 202, "stored informations in the slot.    ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             
             // DeInibisce il pulsante di Abort ed i pulsanti Next e 1,2
             ndCTRL_DeInhibitButton   (wzplButtonAbort,  wzplWindow, NORENDER); 
             ndCTRL_DeInhibitButton   (wzplButtonNext,   wzplWindow, NORENDER); 
             ndCTRL_DeInhibitButton   (wzplButtonErase,  wzplWindow, NORENDER); 
             ndCTRL_DeInhibitButton   (wzplButtonOne,    wzplWindow, NORENDER);
             ndCTRL_DeInhibitButton   (wzplButtonTwo,    wzplWindow, NORENDER);
     
             // Setta le callback ai valori necessari
             ndCTRL_ChangeCallBack (&cb_wzpl1a_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER); 
             ndCTRL_ChangeCallBack (&cb_wzpl1a_Next,   0, wzplButtonNext,  wzplWindow, NORENDER);
             
             // Setup della callback di uscita finestra
             ndLP_SetupCloseWndCallback (&cb_wzpl1a_CloseWnd, 0, 0, wzplWindow);
        
             // Esegui il rendering
             XWindowRender (wzplWindow);
     }
     else   // Problemi nel recupero delle informazioni
     {
             ndWS_GrWriteLn (5, 100, "FATAL:", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 110, "The system has encountered errors", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             ndWS_GrWriteLn (5, 120, "in download informations         ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
             
             ndWS_GrPrintLn (5, 140, COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER, "Error code: %d ", ErrRep);
             
             // DeInibisce il solo pulsante di Abort 
             ndCTRL_DeInhibitButton   (wzplButtonAbort,  wzplWindow, NORENDER); 
             
             // Setup della callback di uscita finestra
             ndCTRL_ChangeCallBack (&cb_wzpl1a_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER); 
             
             // Esegui il rendering
             XWindowRender (wzplWindow);
     }
     
     // Provvedi all'uso del mouse per uscire
     
     wzplYouCanExit=0;
     ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
     
     // Cancella il pulsante Erase
     ndCTRL_DestroyButton (wzplButtonErase, wzplWindow, 0, COLOR_GRAY, NORENDER);
     ndCTRL_DestroyButton (wzplButtonOne,   wzplWindow, 0, COLOR_GRAY, NORENDER); 
     ndCTRL_DestroyButton (wzplButtonTwo,   wzplWindow, 0, COLOR_GRAY, NORENDER);
     XWindowRender (wzplWindow); 
     
     // Segnala che è tutto ok. 
     return 0;  
}


/*
   ----------------------------------------------------------------------------
   STEP1b: NON CI SONO IMMAGINI MEMORIZZATE
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzpl1b_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl1b_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl1b_Next (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = 0;    
}

char INTERNAL_wzpl1b_NoDataInTheSlot (char NrSlot)
{
     // Cancella il vecchio contenuto della finestra
     ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);

     ndWS_GrWriteLn (5,  5, "The BlindServer hasn't stored images", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 15, "in this slot.                       ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     
     ndWS_GrWriteLn (5, 35, "If you click on Next, the system   ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 45, "will start a procedure that allows ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 55, "you to train the program and to    ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 65, "store the new images into the server", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     
     // DeInibisce il pulsante di Abort ed il pulsante Next
     ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
     ndCTRL_InhibitButton   (wzplButtonBack,  wzplWindow, NORENDER); 
     ndCTRL_DeInhibitButton (wzplButtonNext,  wzplWindow, NORENDER); 

     // Setta le callback ai valori necessari
     ndCTRL_ChangeCallBack (&cb_wzpl1b_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER); 
     ndCTRL_ChangeCallBack (&cb_wzpl1b_Next,   0, wzplButtonNext,  wzplWindow, NORENDER); 

     // Setup della callback di uscita finestra
     ndLP_SetupCloseWndCallback (&cb_wzpl1b_CloseWnd, 0, 0, wzplWindow);

     // Esegui il rendering
     XWindowRender (wzplWindow);
     
     // Provvedi all'uso del mouse per uscire
     
     wzplYouCanExit=0;
     ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
     
     // Segnala che è tutto ok. Si noti che da questo momento il supporto
     // di rete è attivato
     
     return 0;   
}

/*
   ----------------------------------------------------------------------------
   STEP1c: ERRORE SCONOSCIUTO NELLA COMUNICAZIONE CON BLINDSERVER
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzpl1c_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl1c_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

char INTERNAL_wzpl1c_UnknownErrorForTheSlot (char NrSlot)
{
     // Cancella il vecchio contenuto della finestra
     ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);

     ndWS_GrWriteLn (5,  5, "Unknown error in communication with ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 15, "Blind Server.                       ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     
     ndWS_GrWriteLn (5, 35, "Click on Abort to exit by this     ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 45, "wizard                             ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     
     // DeInibisce il pulsante di Abort ed il pulsante Next
     ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
     ndCTRL_InhibitButton   (wzplButtonBack,  wzplWindow, NORENDER); 
     ndCTRL_InhibitButton   (wzplButtonNext,  wzplWindow, NORENDER); 

     // Setta le callback ai valori necessari
     ndCTRL_ChangeCallBack (&cb_wzpl1c_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER); 
     
     // Setup della callback di uscita finestra
     ndLP_SetupCloseWndCallback (&cb_wzpl1c_CloseWnd, 0, 0, wzplWindow);

     // Esegui il rendering
     XWindowRender (wzplWindow);
     
     // Provvedi all'uso del mouse per uscire
     
     wzplYouCanExit=0;
     ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
     
     // Segnala che è tutto ok. Si noti che da questo momento il supporto
     // di rete è attivato
     
     return 0;   
}



/*
   ----------------------------------------------------------------------------
   STEP1    : SWITCH DI GESTIONE PRINCIPALE
   ----------------------------------------------------------------------------
*/ 



char INTERNAL_wzpl1_AskToTheBlindServerIfItHasImageStored (char NrSlot)
{
     char AnImageIsStored;
             
     LABEL1_ResetInterface:        
             
         // Chiedi al server se c'è un'immagine memorizzata nel sistema
         AnImageIsStored = baBldSvr_AskToSvrIfItHasAnImageStored (NrSlot);
    
         switch (AnImageIsStored)        // Gestisci la situazione
         {
                case 1:                  // C'è una immagine memorizzata
                {
                     INTERNAL_wzpl1a_ThereAreDataInTheSlot (NrSlot);
                     if (wzplExitReason == WZPL_GORESET) goto LABEL1_ResetInterface;
                     
                     break;
                }
                
                case 0:                  // Non c'è alcuna immagine memorizzata
                {
                     INTERNAL_wzpl1b_NoDataInTheSlot (NrSlot);
                     
                     break;
                }
                
                case -1:                 // Errore di comunicazione con il BlindServer
                {
                     INTERNAL_wzpl1c_UnknownErrorForTheSlot (NrSlot);
                     
                     break;
                } 
         }
     
     return 0;
}

/*
   ----------------------------------------------------------------------------
   STEP2: ISTRUZIONI DEL WIZARD
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzpl2_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl2_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl2_Next (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GONEXT;    
}

static ndint64 cb_wzpl2_Back (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GOBACK;    
}

void INTERNAL_wzpl2_IstructionsForTheUser (void)
{
     struct ndImage_Type MyImage;
     
     // Cancella il vecchio contenuto della finestra
     ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);
     
     // Messaggi all'utente
     ndWS_GrWriteLn (5,  5, "Capture the 4 corners and the 4 sides", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 15, "of the room using your camera        ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    
     ndWS_DrawRectangle (5, 30, 320, 180, COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);
     
     ndIMG_LoadImageFromNDFArray (&MyImage, PlaceWizard0_Width, PlaceWizard0_Height, &PlaceWizard0, NDMGKNB);
     ndIMG_ShowImageScaled (&MyImage, 10, 40, (305.0/315.0), (135.0/150.0), wzplWindow, NORENDER); 
     ndIMG_ReleaseImage (&MyImage);
     
     ndWS_GrWriteLn (5, 190, "Press [Next] to proceed and [Abort]", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     ndWS_GrWriteLn (5, 200, "to exit                            ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
     
     // DeInibisce il pulsante di Abort, il pulsante Next ed il pulsante Back
     ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
     ndCTRL_DeInhibitButton (wzplButtonBack,  wzplWindow, NORENDER); 
     ndCTRL_DeInhibitButton (wzplButtonNext,  wzplWindow, NORENDER); 

     // Setta le callback ai valori necessari
     ndCTRL_ChangeCallBack (&cb_wzpl2_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER);
     ndCTRL_ChangeCallBack (&cb_wzpl2_Back,   0, wzplButtonBack,  wzplWindow, NORENDER); 
     ndCTRL_ChangeCallBack (&cb_wzpl2_Next,   0, wzplButtonNext,  wzplWindow, NORENDER); 

     // Setup della callback di uscita finestra
     ndLP_SetupCloseWndCallback (&cb_wzpl2_CloseWnd, 0, 0, wzplWindow);

     // Esegui il rendering
     XWindowRender (wzplWindow);
     
     // Provvedi all'uso del mouse per uscire
     wzplYouCanExit=0;
     ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
     
     // Segnala che è tutto ok. Si noti che da questo momento il supporto
     // di rete è attivato
     
     return;    
}



/*
   ----------------------------------------------------------------------------
   STEP3: ACQUISIZIONE DI UN IMMAGINE
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzpl3_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl3_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl3_GoBack (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GOBACK;    
}

static ndint64 cb_wzpl3_GoNext (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GONEXT;    
}


char INTERNAL_wzpl3_GrabTheNewImage (int NrSample)
{
    int RectX1, RectY1, RectX2, RectY2, LenX, LenY, MarginX;
    int Counter, wzplWeHaveAnImage;
    static int InternalAverage;
    
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);
    
    // Indica il numero d'ordine del sample
    ndWS_GrPrintLn (340, 20, COLOR_YELLOW, COLOR_GRAY, wzplWindow, NDKEY_SETFONT(3), "%d ", NrSample+1);
    
    // Messaggio di partenza
    ndWS_GrWriteLn (5, 5,  "This is the image that is grabbed by your", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 15, "camera at this moment.", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    
    // Crea un rettangolo bianco nella finestra
    LenX   = STD_BLDSVR__SAMPLEIMG__DIMX/2;
    LenY   = STD_BLDSVR__SAMPLEIMG__DIMY/2;
    
    MarginX = (325 - LenX)/2;
    
    RectX1 = 5   + MarginX;
    RectX2 = 330 - MarginX;
    
    RectY1 = 25;
    RectY2 = 25 + LenY;
    
    ndWS_DrawRectangle (RectX1, RectY1, RectX2, RectY2, COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);
        
    // Visualizza un messaggio per l'utente  
    ndWS_GrWriteLn (5, 170, "Push the NEXT button when you see  ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 180, "the image that you want to store in", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 190, "the BlindServer                    ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_DeInhibitButton (wzplButtonBack,  wzplWindow, NORENDER);
    ndCTRL_DeInhibitButton (wzplButtonNext,  wzplWindow, NORENDER); 
    ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
    
    // Setta le callback ai valori necessari
    ndCTRL_ChangeCallBack (&cb_wzpl3_GoBack, 0, wzplButtonBack,  wzplWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzpl3_GoNext, 0, wzplButtonNext,  wzplWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzpl3_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzpl3_CloseWnd, 0, 0, wzplWindow);
    
    // Rendering     
    XWindowRender (wzplWindow); 
    
    // Inizia il ciclo di richiesta immagine
    
    wzplYouCanExit    = 0;
    wzplExitReason    = 0;
    
    while (!wzplYouCanExit)
    {
        wzpl_cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
        if( !wzpl_cvCameraImage ) break;
            
        // Normalizza l'immagine a 8 bit
        //zcvNormalizeImage8Bit (wzpl_cvCameraImage, &InternalAverage);
        
        // Tenta di compensare le variazioni di luminosità 
        cvEqualizeHist (wzpl_cvCameraImage, wzpl_cvCameraImage);
        
        // Visualizza l'immagine OpenCV
        zcvShowImage8bit (wzpl_cvCameraImage, RectX1, RectY1, RectX2, RectY2, wzplWindow, NORENDER);
        
        XWindowRender (wzplWindow);
    }
    
    // Adesso l'utente ha fatto una scelta. Se l'utente ha scelto di andare avanti, esegui
    // il salvataggio dei dati nella struct IplImage temporanea opportuna
    
    if (wzplExitReason==WZPL_GONEXT)
    {
       zcvCopy (wzpl_cvCameraImage, cvTmpPlacePacket.Img [NrSample]);
    }
    
    // Ritorna alla routine chiamante che dovrà gestire il flusso di esecuzione
    
    return;
}

/*
   ----------------------------------------------------------------------------
   STEP4: RIASSUNTO DI QUANTO E' STATO RACCOLTO
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_wzpl4_VisualizeSamples (char NrGroup, char RenderNow)
{
    // Provvede a visualizzare le immagini prelevate dal BlindServer
     
    // Crea quattro rettangoli per la visualizzazione
    ndWS_DrawRectangle (5,       25,      5+160,       25+70,       COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);      
    ndWS_DrawRectangle (5+160+2, 25,      5+160+2+160, 25+70,       COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);  
    ndWS_DrawRectangle (5,       25+70+2, 5+160,       25+70+2+70,  COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);  
    ndWS_DrawRectangle (5+160+2, 25+70+2, 5+160+2+160, 25+70+2+70,  COLOR_WHITE, COLOR_BLACK, wzplWindow, NORENDER);  

    // Visualizza immagini
    zcvShowImage8bit (cvTmpPlacePacket.Img[NrGroup*4+0], 5,       25,      5+160,       25+70,      wzplWindow, NORENDER);
    zcvShowImage8bit (cvTmpPlacePacket.Img[NrGroup*4+1], 5+160+2, 25,      5+160+2+160, 25+70,      wzplWindow, NORENDER);
    zcvShowImage8bit (cvTmpPlacePacket.Img[NrGroup*4+2], 5,       25+70+2, 5+160,       25+70+2+70, wzplWindow, NORENDER);
    zcvShowImage8bit (cvTmpPlacePacket.Img[NrGroup*4+3], 5+160+2, 25+70+2, 5+160+2+160, 25+70+2+70, wzplWindow, NORENDER);
 
    if (RenderNow) XWindowRender (wzplWindow);
}

static ndint64 cb_wzpl4_ButtonOne (char *StringID, ndint64 InfoField, char WndHandle)
{
    INTERNAL_wzpl4_VisualizeSamples (0, RENDER);   
}

static ndint64 cb_wzpl4_ButtonTwo (char *StringID, ndint64 InfoField, char WndHandle)
{
    INTERNAL_wzpl4_VisualizeSamples (1, RENDER);      
}


static ndint64 cb_wzpl4_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl4_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl4_GoBack (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GOBACK;    
}

static ndint64 cb_wzpl4_GoNext (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GONEXT;    
}

void INTERNAL_wzpl4_MakeASummary ()
{
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);
    
    // Crea i pulsanti button1 e button2
    
    wzplButtonOne    = ndCTRL_CreateButton (355, 70, 385,  90, wzplWindow, "wzplButtonOne", "1", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, &cb_wzpl4_ButtonOne, 0, NORENDER);
    
    wzplButtonTwo    = ndCTRL_CreateButton (355, 95, 385, 115, wzplWindow, "wzplButtonTwo", "2", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, &cb_wzpl4_ButtonTwo, 0, NORENDER);
    
    // Indicazioni per l'utente
    ndWS_GrWriteLn (5,  5, "These are the images that the camera", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 15, "has captured                        ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    
    // Visualizza i campioni raccolti fino a questo momento      
    INTERNAL_wzpl4_VisualizeSamples (0, NORENDER);
    
    // Indicazioni per l'utente
    ndWS_GrWriteLn (5, 175, "Press [Next] to proceed with the    ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 185, "following step or [Back] to repeat  ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 195, "the capture process                 ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);

    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_DeInhibitButton (wzplButtonBack,  wzplWindow, NORENDER);
    ndCTRL_DeInhibitButton (wzplButtonNext,  wzplWindow, NORENDER); 
    ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
    
    // Setta le callback ai valori necessari
    ndCTRL_ChangeCallBack (&cb_wzpl4_GoBack, 0, wzplButtonBack,  wzplWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzpl4_GoNext, 0, wzplButtonNext,  wzplWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzpl4_Abort,  0, wzplButtonAbort, wzplWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzpl4_CloseWnd, 0, 0, wzplWindow);
    
    // Rendering     
    XWindowRender (wzplWindow); 
   
    // Provvedi all'uso del mouse per uscire
    wzplYouCanExit=0;
    ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
    
    // Distruggi i pulsanti 1 e 2
    ndCTRL_DestroyButton (wzplButtonOne,   wzplWindow, 0, COLOR_GRAY, NORENDER); 
    ndCTRL_DestroyButton (wzplButtonTwo,   wzplWindow, 0, COLOR_GRAY, NORENDER);
    XWindowRender (wzplWindow); 
     
    return; 
} 


/*
   ----------------------------------------------------------------------------
   STEP5: NOME DEL LUOGO
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzpl5_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl5_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl5_GoBack (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GOBACK;    
}

static ndint64 cb_wzpl5_GoNext (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_GONEXT;    
}


static ndint64 cb_wzpl5_TestMyVoice (char *StringID, ndint64 InfoField, char WndHandle)
{
    // Inibizione senza rendering: si impedisce che l'utente possa cambiare
    // step durante il processo di sintesi vocale e che possa tentare di
    // avviare un altro processo di sintesi vocale intanto che il primo
    // è in corso
    
    ndCTRL_ChangeCallBack (0, 0, wzplButtonBack,      wzplWindow, NORENDER); 
    ndCTRL_ChangeCallBack (0, 0, wzplButtonNext,      wzplWindow, NORENDER); 
    ndCTRL_ChangeCallBack (0, 0, wzplButtonAbort,     wzplWindow, NORENDER); 
    ndCTRL_ChangeCallBack (0, 0, wzplButtonVoiceTest, wzplWindow, NORENDER); 
    
    // Segnala
    
    ndWS_GrWriteLn (140, 91, "Listen my voice...", COLOR_BLACK, COLOR_GRAY, wzplWindow, RENDER);
    
    // Usa FLite per sintetizzare la voce
    
    flite_text_to_speech (&TextAreaBuffer, VoiceObj, "play");
    
    // Cancella la segnalazione
    
    ndWS_DrawRectangle (140, 85, 330, 105, COLOR_GRAY, COLOR_GRAY, wzplWindow, RENDER);
    
    // Ristabilisce le callbacks originarie
    
    ndCTRL_ChangeCallBack (&cb_wzpl5_GoBack,      0, wzplButtonBack,      wzplWindow, NORENDER); 
    ndCTRL_ChangeCallBack (&cb_wzpl5_GoNext,      0, wzplButtonNext,      wzplWindow, NORENDER); 
    ndCTRL_ChangeCallBack (&cb_wzpl5_Abort,       0, wzplButtonAbort,     wzplWindow, NORENDER); 
    ndCTRL_ChangeCallBack (&cb_wzpl5_TestMyVoice, 0, wzplButtonVoiceTest, wzplWindow, NORENDER); 
    
    return 0;
}

static char cb_wzpl5_ProcessName (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
/*
    Questa callback ha il compito di validare la stringa che viene immessa
    come nome della persona che è stata individuata dal software
*/
{       
    if (strlen (TextData)>0)      // La stringa non è vuota
    {
        // Deinibisce i pulsanti
        
        ndCTRL_DeInhibitButton (wzplButtonNext,       wzplWindow, NORENDER);  
        ndCTRL_DeInhibitButton (wzplButtonVoiceTest,  wzplWindow, NORENDER); 
        
        // Pulisce l'area dove teoricamente potrebbe esserci qualche
        // vecchia segnalazione di errore
        
        ndWS_DrawRectangle (5, 160, 330, 180, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);
        XWindowRender (wzplWindow);
        
        return 1;              // I dati sono accettabili      
    }   
    else
    {
        // Inibisce i pulsanti
        
        ndCTRL_InhibitButton (wzplButtonNext,       wzplWindow, NORENDER);  
        ndCTRL_InhibitButton (wzplButtonVoiceTest,  wzplWindow, NORENDER); 
        
        // Segnalazione di errore
        
        ndWS_GrWriteLn (5, 160, "The name of a person cannot be", COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER);
        ndWS_GrWriteLn (5, 168, "void. Insert a different name.", COLOR_RED, COLOR_GRAY, wzplWindow, NORENDER);
        XWindowRender (wzplWindow);
         
        return 0;              // I dati non sono accettabili 
    }  
}

void INTERNAL_wzpl5_NameOfThePlace ()
{
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);
    
    // Invia un messaggio all'utente
    ndWS_GrWriteLn (5, 10, "Now, you must tell me what is the  ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 18, "name of the place or room that has ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 26, "been captured thought your camera. ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    
    ndWS_GrWriteLn (5, 42, "(maximum 32 chars) ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    

    // Disegna una TextBox
    
    memset (&TextAreaBuffer, 0, 33);
    
    wzplTxtAreaName = ndTBOX_CreateTextArea    (5, 60, 330, 75, "NameTextArea", 
                                                NDKEY_SETTEXTAREA (32, 1), &TextAreaBuffer, 
                                                COLOR_BLACK,  COLOR_WHITE,
                                                COLOR_BLACK,  COLOR_GRAY, 
                                                COLOR_BLACK,  COLOR_GRAY02,
                                                0, &cb_wzpl5_ProcessName, 0, 0, 
                                                wzplWindow, NORENDER);
                                                
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_DeInhibitButton (wzplButtonBack,  wzplWindow, NORENDER);
    ndCTRL_InhibitButton   (wzplButtonNext,  wzplWindow, NORENDER); 
    ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
    
    // Setta le callback ai valori necessari
    
    ndCTRL_ChangeCallBack (&cb_wzpl5_GoBack, 0,    wzplButtonBack,  wzplWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzpl5_GoNext, 0,    wzplButtonNext,  wzplWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzpl5_Abort,  0,    wzplButtonAbort, wzplWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzpl5_CloseWnd, 0, 0, wzplWindow);
    
    // Crea il pulsante per il test FLite
    
    wzplButtonVoiceTest = ndCTRL_CreateButton (5, 85, 130, 105, wzplWindow, "VoiceTestBtn", "Test my voice", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED, 
                                               &cb_wzpl5_TestMyVoice, 0, NORENDER);
    
    // Esegui il rendering
    
    XWindowRender (wzplWindow); 
    
    // Ritardo necessario per impedire pressioni multiple
    ndDelay (1);
    
    wzplYouCanExit    = 0;
    wzplExitReason    = 0;
    
    ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
    
    // Il controllo ritornerà al sistema solo quando l'utente avrà premuto un
    // pulsante. 
    
    // Controllo restituito: Ora è necessario cancellare i vecchi elementi grafici, 
    // ormai obsoleti
    
    ndCTRL_DestroyButton   (wzplButtonVoiceTest, wzplWindow, 0, COLOR_GRAY, NORENDER);
    ndTBOX_DestroyTextArea (wzplTxtAreaName, wzplWindow, 0, COLOR_GRAY, NORENDER);
    
    return;
} 

/*
   ----------------------------------------------------------------------------
   STEP6: COLLEGAMENTO AL SERVER E DOWNLOAD DELL'IMMAGINE
   ----------------------------------------------------------------------------
*/ 

static void cb_wzpl6_GraphCallback0 (int MessageData, int Progress)
{
   int TypeMessage = MessageData & 0xFFFF0000;
   int NrSample    = MessageData & 0x0000FFFF;
   
   int TotalProgress;
   float TotalProgressForSample;
   
   TotalProgressForSample = 80.0/NR_SAMPLES_FOR_PLACE; 
   
   switch (TypeMessage)
   {
          case 0:         // C'è una immagine in invio
          {
               TotalProgress = NrSample*TotalProgressForSample + ((float)(Progress)/100.0)*TotalProgressForSample; 
          
               ndTBAR_DrawProgressBar   (5, 40, 325, 52, TotalProgress, 0, 100, 
                                        COLOR_WHITE, COLOR_RED, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, wzplWindow, RENDER); 
                                        
               break;
          } 
          
          case 0x00010000:   // Fasi successive del trasferimento
          {
               ndTBAR_DrawProgressBar   (5, 40, 325, 52, Progress, 0, 100, 
                                        COLOR_WHITE, COLOR_RED, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, wzplWindow, RENDER); 
                                        
               break;
          }   
   }
}

static ndint64 cb_wzpl6_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static ndint64 cb_wzpl6_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzplYouCanExit = 1;
   wzplExitReason = WZPL_REQABORT;    
}

static void wzpl6_CreateAudioPlaceMessage (void)
{
     char NameFileWav [512]; 
     char VoiceMsg  [512];
     
     strcpy  (NameFileWav, "ms0:/BLINDASSISTANT/AUDCACHEFORPLACESMSG/"); 
     strcat  (NameFileWav, "$PLACEMSG$");
     strcat  (NameFileWav, TextAreaBuffer);
     strcat  (NameFileWav, ".WAV");
     
     strcpy  (VoiceMsg, "You are in ");
     strcat  (VoiceMsg, TextAreaBuffer);
     
     // Provvede a 
     flite_text_to_speech (VoiceMsg, VoiceObj, NameFileWav);
}


void INTERNAL_wzpl6_StoreTheDataToBlindServer (char NrSlot)
{
   
    int  ErrRep;
    
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 1, 330, 215, COLOR_GRAY, COLOR_GRAY, wzplWindow, NORENDER);
    
    // Invia un messaggio all'utente
    ndWS_GrWriteLn (5, 10, "Waiting... I am storing the new   ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
    ndWS_GrWriteLn (5, 20, "images into the BlindServer       ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
                                                
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_InhibitButton (wzplButtonBack,  wzplWindow, NORENDER);
    ndCTRL_InhibitButton (wzplButtonNext,  wzplWindow, NORENDER); 
    ndCTRL_InhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (0, 0, 0, wzplWindow);
    
    // Esegui il rendering
    XWindowRender (wzplWindow); 
    
    // Esegui il salvataggio dei dati
    ErrRep=baBldSvr_SendToSvrASlotPackage (NrSlot, &cb_wzpl6_GraphCallback0, &cvTmpPlacePacket, &TextAreaBuffer);
    
    if (!ErrRep)     // Non ci sono stati errori nel salvataggio delle informazioni
    {
         // Invia un messaggio all'utente
         ndWS_GrWriteLn (5, 70, "BlindServer has saved successfully", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
         ndWS_GrWriteLn (5, 80, "the images that you sent.         ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
         ndWS_GrWriteLn (5, 90, "Generating FLite audio sample.    ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
         
         ndTBAR_DrawProgressBar        (5, 100, 325, 112, 0, 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, wzplWindow, RENDER);   
    
         wzpl6_CreateAudioPlaceMessage ();
         
         ndTBAR_DrawProgressBar        (5, 100, 325, 112, 100, 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, wzplWindow, RENDER);   
         
         ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
         
         ndCTRL_ChangeButtonParameters (wzplButtonAbort, wzplWindow,
                                        "wzplAbort", "Done", "", 
                                        COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                        ND_BUTTON_ROUNDED, RENDER);
                                        
         ndWS_GrWriteLn (5, 125, "Press [Done] to complete operation", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
           
    }
    else
    {
         // Invia un messaggio all'utente
         ndWS_GrWriteLn (5, 70, "There was a trouble in BlindServer", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
         ndWS_GrWriteLn (5, 80, "operations. The images hasn't     ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
         ndWS_GrWriteLn (5, 90, "been saved.                       ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
         ndWS_GrPrintLn (5, 100, COLOR_BLACK, COLOR_GRAY, wzplWindow, 0, "Error code: %d ", -ErrRep); 
         
         ndWS_GrWriteLn (5, 120, "Press [Abort] to exit             ", COLOR_BLACK, COLOR_GRAY, wzplWindow, NORENDER);
         
         ndCTRL_DeInhibitButton (wzplButtonAbort, wzplWindow, NORENDER); 
    }
    
                                      
    ndCTRL_ChangeCallBack (&cb_wzpl6_Abort,  0,    wzplButtonAbort, wzplWindow, NORENDER); 
        
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzpl6_CloseWnd, 0, 0, wzplWindow);
        
    // Esegui il rendering
    XWindowRender (wzplWindow); 
    
    // Controllo mouse
    
    wzplYouCanExit    = 0;
    wzplExitReason    = 0;
    
    ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
    
    // Il controllo ritornerà al sistema solo quando l'utente avrà premuto un
    // pulsante. 
    
    return;
} 




/*
   ----------------------------------------------------------------------------
   CODICE FINALE
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_wzplEND_DestroyInterface (void)
{
    ndLP_DestroyWindow (wzplWindow); 
}   

void INTERNAL_wzplEND_FreeTmpImg (void)
{
   int Counter;
   
   for (Counter=0; Counter<NR_SAMPLES_FOR_PLACE; Counter++)
   {
      cvReleaseImage ( &(cvTmpPlacePacket.Img [Counter]) );
   }
  
   wzplTmpImgAlreadyInit=0;
}

void INTERNAL_wzplEND_TerminateDedConnections (void)
{
   if (wzpl_EnableDedBldSvrConnection)
   {
       baBldSvr_CloseConnectionWithBlindServer (); 
       wzpl_EnableDedBldSvrConnection=0;                              
   }  
   
   if (wzpl_EnableDedNetConnection)
   {
       ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1);
       wzpl_EnableDedNetConnection=0;                
   }
   
   // Libera la gestione della rete
   NetworkIsBusy=0;
}


/*
   ----------------------------------------------------------------------------
   ROUTINE PRINCIPALE
   ----------------------------------------------------------------------------
*/ 




int wzPlaceTraining (int Slot)
/*
     Questa routine avvia il wizard per il place training. La routine restituisce il 
     codice 0 se non vi sono errori ed un codice negativo in caso di errore.
     
     Sono previsti i seguenti codici di errore:
        BAERR_TROUBLE_WITH_GRAPH     Problemi con l'interfaccia grafica
        BAERR_IN_SERVER_CONN         Problemi di connessione con il BlindServer
        
        BAERR_ABORTED_BY_USER        L'utente ha rinunciato all'operazione 
                                     
*/
{
    int Sample;
    int ErrRep;
    
    if ( (!wzvwsample_IsAlreadyOpened) && (!wzft_IsAlreadyOpened) && (!wzpl_IsAlreadyOpened) && (!wztest_IsAlreadyOpened) )
    {       
            sceKernelChangeThreadPriority (sceKernelGetThreadId (), baMainThreadPriority);   // Dai la stessa priorità del main thread                    
            
            if (baMode==BA_MODE_POSREC)
            {
                    INTERNAL_AllocateTmpImages ();
                    ErrRep=INTERNAL_wzplDrawInterface (Slot);    
                    if (ErrRep) goto wzpl_ErrorInGrInterf;         // Problemi nella creazione dell'interfaccia grafica 
                        
                    wzpl_Step0: 
                        INTERNAL_wzplInitGoCamSystem ();
                        wzpl_IsAlreadyOpened=1;
                        
                        ErrRep=INTERNAL_wzpl0_ConnectionWithBlindServer ();        
                        if  ( ErrRep)                                          goto wzpl_ErrorInServerConn;        
                        if ((!ErrRep) && (wzplExitReason == WZPL_REQABORT))    goto wzpl_AbortOnStage0;     
                          
                    wzpl_Step1:
                        INTERNAL_wzpl1_AskToTheBlindServerIfItHasImageStored (Slot);
                        if (wzplExitReason == WZPL_REQABORT)                   goto wzpl_AbortOnStage1;       
                        
                    wzpl_Step2:    
                        INTERNAL_wzpl2_IstructionsForTheUser ();
                        if (wzplExitReason == WZPL_REQABORT)                   goto wzpl_AbortOnStage2;
                        if (wzplExitReason == WZPL_GOBACK)                     goto wzpl_Step1;
                        
                    wzpl_Step3:
                        
                        // Inizia il ciclo di richiesta dei samples
                                         
                        Sample=0;
                        
                        while (Sample<NR_SAMPLES_FOR_PLACE)
                        {
                            INTERNAL_wzpl3_GrabTheNewImage (Sample);
                            
                            if (wzplExitReason == WZPL_GOBACK)
                            {
                                if (Sample==0)
                                {
                                    goto wzpl_Step2;
                                }
                                else
                                {
                                    Sample--;
                                    continue;
                                }
                            }   
                            
                            if (wzplExitReason == WZPL_GONEXT)
                            {
                                Sample++;
                                continue;
                            }
                            
                            if (wzplExitReason == WZPL_REQABORT)
                            {
                                goto wzpl_AbortOnStage3;               
                            }
                        }
                        
                    wzpl_Step4:
                        INTERNAL_wzpl4_MakeASummary ();                             // Fai un riassunto di quanto preso
                        if (wzplExitReason == WZPL_REQABORT)      goto wzpl_AbortOnStage2;
                        if (wzplExitReason == WZPL_GOBACK)        goto wzpl_Step3;
                    
                    wzpl_Step5:
                        INTERNAL_wzpl5_NameOfThePlace ();                           // Ricava il nome del posto
                        if (wzplExitReason == WZPL_REQABORT)      goto wzpl_AbortOnStage2;
                        if (wzplExitReason == WZPL_GOBACK)        goto wzpl_Step4;       
                        
                    wzpl_Step6:
                        INTERNAL_wzpl6_StoreTheDataToBlindServer (Slot);            // Registra i dati che servono  
                        
                    // Arrivati a questo punto, il sistema deve uscire dal wizard
                    
                    wzpl_StdExit:
                        INTERNAL_wzplEND_TerminateDedConnections ();
                        INTERNAL_wzplEND_DeInitGoCamSystem ();                // Deinibisce l'engine video
                        INTERNAL_wzplEND_DestroyInterface (); 
                        INTERNAL_wzplEND_FreeTmpImg ();
                        
                        wzpl_IsAlreadyOpened=0;
                        return 0;
                        
                    // -------------------------------------------------------------         
                     
                    wzpl_AbortOnStage3:
                    wzpl_AbortOnStage2:
                    wzpl_AbortOnStage1:
                    wzpl_AbortOnStage0:
                        INTERNAL_wzplEND_TerminateDedConnections ();
                        INTERNAL_wzplEND_DeInitGoCamSystem ();                // Deinibisce l'engine video
                        INTERNAL_wzplEND_DestroyInterface (); 
                        INTERNAL_wzplEND_FreeTmpImg ();
                        
                        wzpl_IsAlreadyOpened=0;
                        return BAERR_ABORTED_BY_USER;  
                      
                    // Gestione delle condizioni di errore  
                      
                    wzpl_ErrorInServerConn:
                        INTERNAL_wzplEND_DeInitGoCamSystem ();                // Deinibisce l'engine video
                        INTERNAL_wzplEND_DestroyInterface (); 
                        INTERNAL_wzplEND_FreeTmpImg ();
                        
                        wzpl_IsAlreadyOpened=0;
                        return BAERR_IN_SERVER_CONN;       
                               
                    wzpl_ErrorInGrInterf:
                        INTERNAL_wzplEND_FreeTmpImg ();
                        
                        wzpl_IsAlreadyOpened=0;
                        return BAERR_TROUBLE_WITH_GRAPH;
            }
            else   // La modalità di funzionamento è errata
            {
                        INTERNAL_wzplDrawInterfaceModeMismatch ();
                        wzpl_IsAlreadyOpened=1; 
                        
                        // Locka il cursore e fallo comparire sullo schermo, in modo che l'utente possa
                        // premere il tasto Continue....
                        
                        wzplYouCanExit=0;
                        ndProvideMeTheMouse_Until (&wzplYouCanExit, ND_IS_EQUAL, 1, 0, 0);
                            
                        // ... pulsante premuto. Distruggi interfaccia
                            
                        INTERNAL_wzplEND_DestroyInterface (); 
                            
                        wzpl_IsAlreadyOpened=0;         
                        return 0;
            }
                          
    }
}
 
 
 



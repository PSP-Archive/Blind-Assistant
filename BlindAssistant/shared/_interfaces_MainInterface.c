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
   INTERFACCIA PRINCIPALE DEL PROGRAMMA
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

/* 
   ----------------------------------------------------------------------------
   VARIABILI 
   ----------------------------------------------------------------------------
*/


int   mainitf_Window; 
   
int   baMainThreadPriority;
int   baMainThreadID;





/* 
   ----------------------------------------------------------------------------
   ROUTINES DI INIZIALIZZAZIONE
   ----------------------------------------------------------------------------
*/

void mainitf_Init ()
{
      
}


/* 
   ----------------------------------------------------------------------------
   CONTROLLO PRIORITA' DEL MAIN THREAD
   ----------------------------------------------------------------------------
*/

void baGetMainThreadInfos (void)
{
    baMainThreadID       = sceKernelGetThreadId ();
    baMainThreadPriority = sceKernelGetThreadCurrentPriority (); 
}

void baLockMainThread (void)
{
    sceKernelSleepThread ();
}

void baUnlockMainThread (void)
{
    sceKernelWakeupThread (baMainThreadID);
}


/* 
   ----------------------------------------------------------------------------
   BARRA DI STATO E BARRA FUNZIONI 
   ----------------------------------------------------------------------------
*/

void mainitf_StatusBar (char *Message, char RenderNow)
{
     ndWS_DrawRectangle (0, 181, 310, 193, COLOR_GRAY, COLOR_BLACK, mainitf_Window, NORENDER);
     ndWS_GrWriteLn (10, 183, Message, COLOR_BLACK, COLOR_GRAY, mainitf_Window, NORENDER);
     if (RenderNow) XWindowRender (mainitf_Window); 
}

void mainitf_FunctionBar (char *Message, char RenderNow)
{
     ndWS_DrawRectangle (0, 193, 310, 205, COLOR_LGREEN, COLOR_BLACK, mainitf_Window, NORENDER);
     ndWS_GrWriteLn (10, 195, Message, COLOR_BLACK, COLOR_LGREEN, mainitf_Window, NORENDER);
     if (RenderNow) XWindowRender (mainitf_Window); 
}



/* 
   ----------------------------------------------------------------------------
   CALLBACKS DI MENU
   ----------------------------------------------------------------------------
*/

static ndint64 cbLoad (char WndHandle)
{
     wzXLoadData ();
     return 0;   
}

static ndint64 cbSave (char WndHandle)
{
     wzXSaveData ();
     return 0;    
}

static ndint64 cbAbout (char WndHandle)
{
     baOpenAboutWindow ();
     return 0;   
}

static ndint64 cbOptions (char WndHandle)
{
     wzOptions ();
     return 0; 
}

static ndint64 cbHelp (char WndHandle)
{
     baVoiceRec_Help ();
     return 0; 
}

static ndint64 cbCheckForUpdates (char WndHandle)
{
     baUpd_ManualChecksForUpdates (0, 0);
     return 0; 
}

static ndint64 cbCheckAndDownloadNewUpdates (char WndHandle)
{
     baUpd_ManualChecksForUpdates (1, 0);
     return 0; 
}

static ndint64 cbCheckDownloadAndInstallNewUpdates (char WndHandle)
{
     baUpd_ManualChecksForUpdates (1, 1);
     return 0; 
}

static ndint64 cbManuallyInstallNewUpdates (char WndHandle)
{
     baUpd_ManuallyInstallANewUpdate ();
     return 0;  
}

static ndint64 cbExit (char WndHandle)
{
     baUnlockMainThread ();           // Provocherà l'uscita dal main thread
     return 0;
}

static ndint64 cbTestFaceRecognition (char WndHandle)
{
     wzTestForFaceRecognition ();
     return 0;   
}

static ndint64 cbBlindServerManualConnect (char WndHandle)
{
     bldsvrcnx_ServerManualConnect ();
     return 0;   
}

static ndint64 cbBlindServerManualDisconnect (char WndHandle)
{
     bldsvrcnx_ServerManualDisconnect (); 
     return 0;   
}


/* 
   ----------------------------------------------------------------------------
   CALLBACKS PCA SAMPLES
   ----------------------------------------------------------------------------
*/


static ndint64 cbPCASample01 (char WndHandle) { wzViewSample (0); return 0;   }
static ndint64 cbPCASample02 (char WndHandle) { wzViewSample (1); return 0;   }
static ndint64 cbPCASample03 (char WndHandle) { wzViewSample (2); return 0;   }
static ndint64 cbPCASample04 (char WndHandle) { wzViewSample (3); return 0;   }
static ndint64 cbPCASample05 (char WndHandle) { wzViewSample (4); return 0;   }
static ndint64 cbPCASample06 (char WndHandle) { wzViewSample (5); return 0;   }
static ndint64 cbPCASample07 (char WndHandle) { wzViewSample (6); return 0;   }
static ndint64 cbPCASample08 (char WndHandle) { wzViewSample (7); return 0;   }
static ndint64 cbPCASample09 (char WndHandle) { wzViewSample (8); return 0;   }
static ndint64 cbPCASample10 (char WndHandle) { wzViewSample (9); return 0;   }


/* 
   ----------------------------------------------------------------------------
   CALLBACK PLACES
   ----------------------------------------------------------------------------
*/


static ndint64 cbPlace01 (char WndHandle) {  wzPlaceTraining (0); return 0;   }
static ndint64 cbPlace02 (char WndHandle) {  wzPlaceTraining (1); return 0;   }
static ndint64 cbPlace03 (char WndHandle) {  wzPlaceTraining (2); return 0;   }
static ndint64 cbPlace04 (char WndHandle) {  wzPlaceTraining (3); return 0;   }
static ndint64 cbPlace05 (char WndHandle) {  wzPlaceTraining (4); return 0;   }
static ndint64 cbPlace06 (char WndHandle) {  wzPlaceTraining (5); return 0;   }
static ndint64 cbPlace07 (char WndHandle) {  wzPlaceTraining (6); return 0;   }
static ndint64 cbPlace08 (char WndHandle) {  wzPlaceTraining (7); return 0;   }
static ndint64 cbPlace09 (char WndHandle) {  wzPlaceTraining (8); return 0;   }
static ndint64 cbPlace10 (char WndHandle) {  wzPlaceTraining (9); return 0;   }
static ndint64 cbPlace11 (char WndHandle) {  wzPlaceTraining (10); return 0;   }
static ndint64 cbPlace12 (char WndHandle) {  wzPlaceTraining (11); return 0;   }
static ndint64 cbPlace13 (char WndHandle) {  wzPlaceTraining (12); return 0;   }
static ndint64 cbPlace14 (char WndHandle) {  wzPlaceTraining (13); return 0;   }
static ndint64 cbPlace15 (char WndHandle) {  wzPlaceTraining (14); return 0;   }
static ndint64 cbPlace16 (char WndHandle) {  wzPlaceTraining (15); return 0;   }
static ndint64 cbPlace17 (char WndHandle) {  wzPlaceTraining (16); return 0;   }
static ndint64 cbPlace18 (char WndHandle) {  wzPlaceTraining (17); return 0;   }
static ndint64 cbPlace19 (char WndHandle) {  wzPlaceTraining (18); return 0;   }
static ndint64 cbPlace20 (char WndHandle) {  wzPlaceTraining (19); return 0;   }
static ndint64 cbPlace21 (char WndHandle) {  wzPlaceTraining (20); return 0;   }
static ndint64 cbPlace22 (char WndHandle) {  wzPlaceTraining (21); return 0;   }
static ndint64 cbPlace23 (char WndHandle) {  wzPlaceTraining (22); return 0;   }
static ndint64 cbPlace24 (char WndHandle) {  wzPlaceTraining (23); return 0;   }
static ndint64 cbPlace25 (char WndHandle) {  wzPlaceTraining (24); return 0;   }
static ndint64 cbPlace26 (char WndHandle) {  wzPlaceTraining (25); return 0;   }
static ndint64 cbPlace27 (char WndHandle) {  wzPlaceTraining (26); return 0;   }
static ndint64 cbPlace28 (char WndHandle) {  wzPlaceTraining (27); return 0;   }
static ndint64 cbPlace29 (char WndHandle) {  wzPlaceTraining (28); return 0;   }
static ndint64 cbPlace30 (char WndHandle) {  wzPlaceTraining (29); return 0;   }

/* 
   ----------------------------------------------------------------------------
   CALLBACKS COMMUTAZIONE FUNZIONI
   ----------------------------------------------------------------------------
*/

static ndint64 cbSwitchToNullMode (char WndHandle)
{
   baManualSwitchToMode=BA_MODE_NOWORK;  
   return 0;
}

static ndint64 cbSwitchToFaceRec (char WndHandle)
{
   baManualSwitchToMode=BA_MODE_FACEREC;
   return 0;    
}

static ndint64 cbSwitchToPosRec (char WndHandle)
{
   baManualSwitchToMode=BA_MODE_POSREC;
   return 0;     
}

static ndint64 cbSwitchToOcrSystem (char WndHandle)
{
   baManualSwitchToMode=BA_MODE_OCR;
   return 0;    
}

static ndint64 cbSwitchToColorScanner (char WndHandle)
{
   baManualSwitchToMode=BA_MODE_COLORSCANNER;
   return 0;    
}

static ndint64 cbSwitchToMailReader (char WndHandle)
{
   baManualSwitchToMode=BA_MODE_MAILREADER;
   return 0;
}

static ndint64 cbSwitchToDataMatrixScanner (char WndHandle)
{
   baManualSwitchToMode=BA_MODE_DATAMATRIX;
   return 0;
}

/* 
   ----------------------------------------------------------------------------
   INIBIZIONE ELEMENTI DELL'INTERFACCIA A SECONDA DELLE PIATTAFORME
   ----------------------------------------------------------------------------
*/

static void mainitf_InhibitElementsForPlatform (void)
{
     #ifdef PSP1000_VERSION
     
         ndWMENU_InhibitL1WMI (mainitf_Window, "Eigen", NORENDER);
         
         ndWMENU_InhibitL2WMI (mainitf_Window, "File.Load", NORENDER);   
         ndWMENU_InhibitL2WMI (mainitf_Window, "File.Save", NORENDER); 
         
         ndWMENU_InhibitL2WMI (mainitf_Window, "Func.FaceRec", NORENDER); 
         ndWMENU_InhibitL2WMI (mainitf_Window, "Func.MailReader", NORENDER); 
         
         ndWMENU_InhibitL2WMI (mainitf_Window, "Misc.PCATest", NORENDER); 
         ndWMENU_InhibitL2WMI (mainitf_Window, "Misc.ListCmd", NORENDER); 
     
     #endif
}


/* 
   ----------------------------------------------------------------------------
   INIZIO DEL CODICE DELL'INTERFACCIA
   ----------------------------------------------------------------------------
*/
   
void mainitf_DrawInterface (void)
{
    mainitf_Window = ndLP_CreateWindow (10, 1, 340, 253, "Blind assistant", COLOR_WHITE, COLOR_BLUE, COLOR_GRAY04, COLOR_BLACK, MENUSUPPORTED);  
    ndLP_MaximizeWindow (mainitf_Window);
    
    // Disegna il rettangolo che conterrà l'immagine e definisci il menù di sistema
    
    ndWS_DrawRectangle (35, 3, 285, 180, COLOR_WHITE, COLOR_BLACK, mainitf_Window, NORENDER);
    
    ndWMENU_DefineMenuItem (mainitf_Window, "File",         "File",   0,                          0,          NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "File.Load",    "Load",   ND_CALLBACK_IN_NEW_CONTEXT, &cbLoad,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "File.Save",    "Save",   ND_CALLBACK_IN_NEW_CONTEXT, &cbSave,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "File.About",   "About",  ND_CALLBACK_IN_NEW_CONTEXT, &cbAbout,   NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "File.Quit",    "Quit",   0,                          &cbExit,    NORENDER); 
    
    ndWMENU_DefineMenuItem (mainitf_Window, "Func",         "Fnc",    0,                          0,          NORENDER); 
    
    ndWMENU_DefineMenuItem (mainitf_Window, "Func.NoFunc",         "Enable no func mode",       0,  &cbSwitchToNullMode,     NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Func.FaceRec",        "Enable face recognizer",    0,  &cbSwitchToFaceRec,      NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Func.PosRec",         "Enable position recognizer",    0,  &cbSwitchToPosRec,      NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Func.OcrRec",         "Enable OCR recognizer",     0,  &cbSwitchToOcrSystem,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Func.ColorScn",       "Enable color scanner",      0,  &cbSwitchToColorScanner, NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Func.MailReader",     "Enable mail reader",        0,  &cbSwitchToMailReader,   NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Func.DataMatrix",     "Enable data matrix scanner",  0,  &cbSwitchToDataMatrixScanner,   NORENDER); 
    
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen",         "Faces",      0,                          0,                 NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E1",      "Sample 1",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample01,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E2",      "Sample 2",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample02,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E3",      "Sample 3",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample03,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E4",      "Sample 4",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample04,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E5",      "Sample 5",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample05,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E6",      "Sample 6",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample06,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E7",      "Sample 7",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample07,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E8",      "Sample 8",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample08,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E9",      "Sample 9",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample09,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Eigen.E10",     "Sample 10",   ND_CALLBACK_IN_NEW_CONTEXT, &cbPCASample10,    NORENDER); 
     
    ndWMENU_DefineMenuItem (mainitf_Window, "Place",          "Places",       0,                          0,                 NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E1",       "Place 1",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace01,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E2",       "Place 2",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace02,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E3",       "Place 3",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace03,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E4",       "Place 4",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace04,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E5",       "Place 5",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace05,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E6",       "Place 6",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace06,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E7",       "Place 7",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace07,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E8",       "Place 8",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace08,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E9",       "Place 9",      ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace09,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E10",      "Place 10",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace10,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E11",      "Place 11",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace11,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E12",      "Place 12",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace12,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E13",      "Place 13",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace13,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E14",      "Place 14",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace14,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E15",      "Place 15",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace15,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E16",      "Place 16",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace16,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E17",      "Place 17",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace17,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E18",      "Place 18",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace18,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E19",      "Place 19",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace19,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E20",      "Place 20",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace20,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E21",      "Place 21",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace21,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E22",      "Place 22",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace22,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E23",      "Place 23",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace23,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E24",      "Place 24",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace24,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E25",      "Place 25",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace25,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E26",      "Place 26",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace26,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E27",      "Place 27",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace27,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E28",      "Place 28",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace28,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E29",      "Place 29",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace29,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Place.E30",      "Place 30",     ND_CALLBACK_IN_NEW_CONTEXT, &cbPlace30,    NORENDER); 
     
    ndWMENU_DefineMenuItem (mainitf_Window, "Misc",          "Misc",   0,                          0,          NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Misc.BdSvrON",  "Connect to BldSvr",   ND_CALLBACK_IN_NEW_CONTEXT, &cbBlindServerManualConnect,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Misc.BdSvrOFF", "Disconnect BldSvr",   ND_CALLBACK_IN_NEW_CONTEXT, &cbBlindServerManualDisconnect,    NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Misc.PCATest",  "Test face recognizer",   ND_CALLBACK_IN_NEW_CONTEXT, &cbTestFaceRecognition,    NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Misc.Options",  "Options",  ND_CALLBACK_IN_NEW_CONTEXT, &cbOptions,          NORENDER); 
    ndWMENU_DefineMenuItem (mainitf_Window, "Misc.ListCmd",  "List of voice commands",     ND_CALLBACK_IN_NEW_CONTEXT, &cbHelp,          NORENDER); 
    
    ndWMENU_DefineMenuItem (mainitf_Window, "Updates",       "Upd",   0,                          0,          NORENDER);
   
    ndWMENU_DefineMenuItem (mainitf_Window, "Updates.Check",      "Check for new updates",         ND_CALLBACK_IN_NEW_CONTEXT, &cbCheckForUpdates, NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Updates.ChkDwl",     "Check,Download new updates",  ND_CALLBACK_IN_NEW_CONTEXT, &cbCheckAndDownloadNewUpdates, NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Updates.ChkDwlInst", "Chk,Dwload,Inst new updates",   ND_CALLBACK_IN_NEW_CONTEXT, &cbCheckDownloadAndInstallNewUpdates, NORENDER);
    ndWMENU_DefineMenuItem (mainitf_Window, "Updates.ForceInst",  "Manually install a new core",   ND_CALLBACK_IN_NEW_CONTEXT, &cbManuallyInstallNewUpdates, NORENDER);
    
    // Disegna la barra di stato
    
    if (MainNetConn_IsOperative && MainBldSvrConn_IsOperative)
       mainitf_StatusBar ("", NORENDER);        
    else
       mainitf_StatusBar ("BlindServer isn't connected", NORENDER);
       
    // Disegna la barra funzioni
    
    mainitf_FunctionBar ("", NORENDER); 
    
    // Inibisci gli elementi della interfaccia a seconda della piattaforma
    
    mainitf_InhibitElementsForPlatform ();    
    
    // Esegui il rendering
    
    XWindowRender (mainitf_Window);
} 
 





void baMainInterface (void)
{
    mainitf_Init ();
    mainitf_DrawInterface (); 
    
    baGetMainThreadInfos ();                      // Ricava le informazioni sul main thread
    baReStartAudioEngine ();                      // Avvia l'audio server
    
    baaud_WellcomeToBlindAssistant ();            // Messaggio di benvenuto
    ndDelay (3);                                  // Ritardo di 3 sec. 
    
    baaud_PressSquareAndSayTheCmd ();             // Chiedi di fornire un messaggio vocale
    ndDelay (3);
    
    baCreateVideoEngineThread ();                 // Crea il thread dell'engine video principale
    baReStartVideoEngine ();                      // Avvia l'engine video principale
    baReStartPlaceEngine ();                      // Avvia l'engine per il riconoscimento dei luoghi
    
    // BLOCCO DEL THREAD: Sarà riattivato solo all'uscita
    // -----------------------------------------------------------------------------------------------
    
    ndDelay (8);
    baLockMainThread ();

    // -----------------------------------------------------------------------------------------------
    // USCITA DAL BLOCCO: Quando il controllo arriva qui, vuol dire che l'utente ha richiesto di uscire.
    
    baExitPlaceEngine ();                        // Arresta l'engine di riconoscimento dei luoghi
    baExitVideoEngine ();                        // Arresta il Video Engine
    baExitAudioEngine ();                        // Arresta l'audio engine
    
    // Arresta i wizards che sono eventualmente attivati in questo momento
    
    HALT_wzTestForFaceRecognition ();
    HALT_wzViewSample ();
    
}

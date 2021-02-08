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
   CODICE DELL'INTERFACCIA PER LA GESTIONE DEGLI ERRORI 
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
   VARIABILI
   ----------------------------------------------------------------------------
*/
 


/*
   ----------------------------------------------------------------------------
   IMPOSSIBILE TROVARE UNO DEI FILES HAAR
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

int baerr0_Window;
int baerr0_ButtonRetry;
int baerr0_ButtonAbort;
int baerr0_OptionChoosen;


ndint64 baerr0_cbButtonAbort (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr0_OptionChoosen=BA_ABORT;       
}


int baitf_ErrorCannotFindHaarCascadeTrainingSet ()
{
    baerr0_Window = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "I cannot find haarcascade file", 
                                       COLOR_WHITE, COLOR_RED, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (baerr0_Window>=0)
    {
        ndLP_MaximizeWindow (baerr0_Window);
        
        // Output audio dell'errore
        
        flite_text_to_speech ("Error: I cannot find haarcascade file", VoiceObj, "play");
        
        // Spiega le ragioni dell'errore
        
        ndWS_GrWriteLn (5, 10, "The system is unable to find one of the", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        ndWS_GrWriteLn (5, 18, "haarcascade files. These files are very", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        ndWS_GrWriteLn (5, 26, "important because they allow the detection", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        ndWS_GrWriteLn (5, 34, "of the faces.            ", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 50, "Usually, these files are in the folder", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 66, "ms0:/BLINDASSISTANT/CVHAAR/        ", COLOR_RED, COLOR_GRAY, baerr0_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 82, "The file that is required is called", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        ndWS_GrWriteLn (5, 92, "haarcascade_frontalface_default.xml    ", COLOR_RED, COLOR_GRAY, baerr0_Window, NORENDER);
        ndWS_GrWriteLn (5,102, "or                                 ", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        ndWS_GrWriteLn (5,112, "haarcascade_frontalface_alt.xml", COLOR_RED, COLOR_GRAY, baerr0_Window, NORENDER);
        
        ndWS_GrWriteLn (5,125, "Now the application will be terminated.", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        ndWS_GrWriteLn (5,135, "Press ABORT to exit                ", COLOR_BLACK, COLOR_GRAY, baerr0_Window, NORENDER);
        
        // Definisci i pulsanti
        
        baerr0_ButtonAbort = ndCTRL_CreateButton (335, WindowData [baerr0_Window].WSWndHeight - 25, 
                                               385, WindowData [baerr0_Window].WSWndHeight - 5,
                                               baerr0_Window, "err0_Abort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr0_cbButtonAbort, 0, NORENDER);
                                             
        XWindowRender (baerr0_Window);       
        
        // Inizia un ciclo MouseControl per consentire la scelta
        
        baerr0_OptionChoosen=0;
        ndProvideMeTheMouse_Until (&baerr0_OptionChoosen, ND_IS_DIFFERENT, 0, 0, 0);
        
        // Chiudi la finestra di notifica
        
        ndLP_DestroyWindow (baerr0_Window);
        return baerr0_OptionChoosen;
    }
    else
        return BA_ABORT;     // Non è riuscito a creare la finestra: simula ABORT             
} 

#endif

/*
   ----------------------------------------------------------------------------
   IMPOSSIBILE CARICARE I FILES HAAR
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

int baerr1_Window;
int baerr1_ButtonRetry;
int baerr1_ButtonAbort;
int baerr1_OptionChoosen;


ndint64 baerr1_cbButtonAbort (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr1_OptionChoosen=BA_ABORT;       
}

int baitf_ErrorInLoadingHaarCascadeFile ()
{
    baerr1_Window = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Error in loading haarcascade file", 
                                       COLOR_WHITE, COLOR_RED, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (baerr1_Window>=0)
    {
        ndLP_MaximizeWindow (baerr1_Window);
        
        // Output audio dell'errore
        
        flite_text_to_speech ("Error in loading haarcascade file", VoiceObj, "play");
        
        // Spiega le ragioni dell'errore
        
        ndWS_GrWriteLn (5, 10, "An error in loading the haar cascade file", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 18, "occurs. It is possible that this file is ", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 26, "damaged", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 40, "The file is located in the folder", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 56, "ms0:/BLINDASSISTANT/CVHAAR/        ", COLOR_RED, COLOR_GRAY, baerr1_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 72, "And it is called", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 82, "haarcascade_frontalface_default.xml", COLOR_RED, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 92, "or (if the former isn't present)   ", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5,102, "haarcascade_frontalface_alt.xml", COLOR_RED, COLOR_GRAY, baerr1_Window, NORENDER);
        
        ndWS_GrWriteLn (5,115, "I suggest you to replace the file with", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5,123, "a new copy, that you can find in the  ", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5,131, "installation package of BlindAssistant,", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5,139, "folder CVHAAR. Contact Visilab for further", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5,147, "details.  ", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        
        ndWS_GrWriteLn (5,160, "Now the application will be terminated.", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5,170, "Press ABORT to exit                ", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        
        // Definisci i pulsanti
        
        baerr1_ButtonAbort = ndCTRL_CreateButton (335, WindowData [baerr1_Window].WSWndHeight - 25, 
                                               385, WindowData [baerr1_Window].WSWndHeight - 5,
                                               baerr1_Window, "err1_Abort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr1_cbButtonAbort, 0, NORENDER);
                                             
        XWindowRender (baerr1_Window);       
        
        // Inizia un ciclo MouseControl per consentire la scelta
        
        baerr1_OptionChoosen=0;
        ndProvideMeTheMouse_Until (&baerr1_OptionChoosen, ND_IS_DIFFERENT, 0, 0, 0);
        
        // Chiudi la finestra di notifica
        
        ndLP_DestroyWindow (baerr1_Window);
        return baerr1_OptionChoosen;
    }
    else
        return BA_ABORT;     // Non è riuscito a creare la finestra: simula ABORT             
}

#endif
 

/*
   ----------------------------------------------------------------------------
   SI STA USANDO UNA VECCHIA VERSIONE DEL CASCADE HAARFILE PER IL FACE DETECTOR
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

int baerr2_Window;
int baerr2_ButtonAbort;
int baerr2_YouCanExit;

ndint64 baerr2_cbButtonAbort (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr2_YouCanExit=1;
}

int baitf_YouAreUsingOldVersionOfCascadeFile ()
{
    baerr2_Window = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Warning: old version of haar cascade file", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (baerr2_Window>=0)
    {
        ndLP_MaximizeWindow (baerr2_Window);
        
        // Output audio dell'errore
        
        flite_text_to_speech ("Warning: old version of haar cascade file", VoiceObj, "play");
        
        // Spiega le ragioni del problema
        
        ndWS_GrWriteLn (5, 10, "You are using a version of the haarcascade", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 19, "file that is obsolete. The file in use is ", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 28, "haarcascade_frontalface_alt.xml           ", COLOR_RED, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 37, "in the folder                             ", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 46, "ms0:/BLINDASSISTANT/CVHAAR/               ", COLOR_RED, COLOR_GRAY, baerr1_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 60, "I suggest you to replace the file with the", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 69, "better version", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 78, "haarcascade_frontalface_default.xml       ", COLOR_RED, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 87, "You can find this file in the Blind Assistant", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 96, "installation package, folder /CVHAAR      ", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 110, "This advice will be repeated each time that", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 119, "this program starts. If you want to avoid this", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 128, "go to menu Misc/Options, button Engine, and", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 137, "disable the check box", COLOR_BLACK, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 151, "Advice if the system is using the old version", COLOR_BLUE, COLOR_GRAY, baerr1_Window, NORENDER);
        ndWS_GrWriteLn (5, 160, "of the face recognizer training set", COLOR_BLUE, COLOR_GRAY, baerr1_Window, NORENDER);
        
        
        // Definisci i pulsanti
        
        baerr2_ButtonAbort = ndCTRL_CreateButton (305, WindowData [baerr2_Window].WSWndHeight - 25, 
                                               385, WindowData [baerr2_Window].WSWndHeight - 5,
                                               baerr2_Window, "err2_Continue", "Continue", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr2_cbButtonAbort, 0, NORENDER);
                                             
        XWindowRender (baerr2_Window);       
        
        // Inizia un ciclo MouseControl per consentire la scelta
        
        baerr2_YouCanExit=0;
        ndProvideMeTheMouse_Until (&baerr2_YouCanExit, ND_IS_EQUAL, 1, 0, 0);
        
        // Chiudi la finestra di notifica
        
        ndLP_DestroyWindow (baerr2_Window);
        return 0;
    }
    else
        return 0;     // Non è riuscito a creare la finestra: simula OK          
}

#endif 


/*
   ----------------------------------------------------------------------------
   ERRORE NEL COLLEGAMENTO DELLA VIDEOCAMERA
   ----------------------------------------------------------------------------
*/
 
int baerr3_Window;
int baerr3_ButtonRetry;
int baerr3_ButtonAbort;
int baerr3_OptionChoosen;

ndint64 baerr3_cbButtonRetry (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr3_OptionChoosen=BA_RETRY;    
}

ndint64 baerr3_cbButtonAbort (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr3_OptionChoosen=BA_ABORT;       
}


int baitf_ErrorInCameraConnect ()
{
    baerr3_Window = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Error in camera connection", 
                                       COLOR_WHITE, COLOR_RED, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (baerr3_Window>=0)
    {
        ndLP_MaximizeWindow (baerr3_Window);
        
        // Output audio dell'errore
        
        flite_text_to_speech ("Error: you have not connected your camera", VoiceObj, "play");
        
        // Spiega le ragioni dell'errore
        
        ndWS_GrWriteLn (5, 10, "This application has been designed ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        ndWS_GrWriteLn (5, 18, "to work with a webcam (Eyeserver   ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        ndWS_GrWriteLn (5, 26, "based or Sony GoCam).              ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 42, "The system has not been able to    ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        ndWS_GrWriteLn (5, 50, "connect to your camera             ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 66, "Press RETRY button if you want that", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        ndWS_GrWriteLn (5, 74, "Blind Assistant retries to connect ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        ndWS_GrWriteLn (5, 82, "to this device.                    ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 98, "Otherwise, press ABORT if you want ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        ndWS_GrWriteLn (5,108, "to terminate this application      ", COLOR_BLACK, COLOR_GRAY, baerr3_Window, NORENDER);
        
        // Definisci i pulsanti
        
        baerr3_ButtonRetry  = ndCTRL_CreateButton (335, WindowData [baerr3_Window].WSWndHeight - 50, 
                                               385, WindowData [baerr3_Window].WSWndHeight - 30,
                                               baerr3_Window, "err3_Retry", "Retry", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr3_cbButtonRetry, 0, NORENDER);
        
        baerr3_ButtonAbort = ndCTRL_CreateButton (335, WindowData [baerr3_Window].WSWndHeight - 25, 
                                               385, WindowData [baerr3_Window].WSWndHeight - 5,
                                               baerr3_Window, "err3_Abort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr3_cbButtonAbort, 0, NORENDER);
                                             
        XWindowRender (baerr3_Window);       
        
        // Inizia un ciclo MouseControl per consentire la scelta
        
        baerr3_OptionChoosen=0;
        ndProvideMeTheMouse_Until (&baerr3_OptionChoosen, ND_IS_DIFFERENT, 0, 0, 0);
        
        // Chiudi la finestra di notifica
        
        ndLP_DestroyWindow (baerr3_Window);
        return baerr3_OptionChoosen;
    }
    else
        return BA_ABORT;     // Non è riuscito a creare la finestra: simula ABORT             
} 
 
 


/*
   ----------------------------------------------------------------------------
   FIRMWARE CON VERSIONE INSUFFICIENTE
   ----------------------------------------------------------------------------
*/
 
int baerr4_Window;
int baerr4_ButtonRetry;
int baerr4_ButtonAbort;
int baerr4_OptionChoosen;

ndint64 baerr4_cbButtonAbort (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr4_OptionChoosen=BA_ABORT;       
}

int baitf_ErrorFirmwareTooOld ()
{
    baerr4_Window = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Firmware too old", 
                                       COLOR_WHITE, COLOR_RED, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (baerr4_Window>=0)
    {
        ndLP_MaximizeWindow (baerr4_Window);
        
        // Output audio dell'errore
        
        flite_text_to_speech ("Error: your firmware is too old", VoiceObj, "play");
        
        // Spiega le ragioni dell'errore
        
        ndWS_GrWriteLn (5, 10, "This application requires at least  ", COLOR_BLACK, COLOR_GRAY, baerr4_Window, NORENDER);
        ndWS_GrWriteLn (5, 20, "a custom firmware version 3.80 M33", COLOR_BLACK, COLOR_GRAY, baerr4_Window, NORENDER);
         
        ndWS_GrWriteLn (5, 40, "The program will be terminated ", COLOR_BLACK, COLOR_GRAY, baerr4_Window, NORENDER);
        
        // Definisci i pulsanti
        
        baerr4_ButtonAbort = ndCTRL_CreateButton (335, WindowData [baerr4_Window].WSWndHeight - 25, 
                                               385, WindowData [baerr4_Window].WSWndHeight - 5,
                                               baerr4_Window, "err4_Abort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr4_cbButtonAbort, 0, NORENDER);
                                               
        XWindowRender (baerr4_Window);       
        
        // Inizia un ciclo MouseControl per consentire la scelta
        
        baerr4_OptionChoosen=0;
        ndProvideMeTheMouse_Until (&baerr4_OptionChoosen, ND_IS_DIFFERENT, 0, 0, 0);
        
        // Chiudi la finestra di notifica
        
        ndLP_DestroyWindow (baerr4_Window);
        return BA_ABORT;
        
    }
    else
        return BA_ABORT;     // Non è riuscito a creare la finestra: simula ABORT             
} 


/*
   ----------------------------------------------------------------------------
   IMPOSSIBILITA' DI SCARICARE IL PACCHETTO DI CONTROLLO VOCALE
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

int baerr5_Window;
int baerr5_ButtonRetry;
int baerr5_ButtonAbort;
int baerr5_NetworkOptions;
int baerr5_OptionChoosen;

ndint64 baerr5_cbButtonRetry (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr5_OptionChoosen=BA_RETRY;    
}

ndint64 baerr5_cbButtonAbort (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr5_OptionChoosen=BA_ABORT;       
}

ndint64 baerr5_cbChangeNetworkOptions (char *StringID, ndint64 InfoField, char WndHandle)
{
    // Questo consentirà di aprire le opzioni di rete
    
    wzAdvancedOptions ();      
}

int baitf_ErrorInDownloadingHmmPackage ()
{
    baerr5_Window = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "I cannot download voice control package", 
                                       COLOR_WHITE, COLOR_RED, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (baerr5_Window>=0)
    {
        ndLP_MaximizeWindow (baerr5_Window);
        
        // Output audio dell'errore
        
        flite_text_to_speech ("Error: I cannot download voice control package", VoiceObj, "play");
        
        // Spiega le ragioni dell'errore
        
        ndWS_GrWriteLn (5, 10, "This application uses a system for voice ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5, 18, "control that allows to the blind the use ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5, 26, "of its functionalities.                  ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 42, "The voice control system requires a special", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5, 50, "package to work. It isn't on your disk and ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5, 58, "the attempt to download it from Visilab    ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5, 66, "website has failed.                        ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
       
        ndWS_GrWriteLn (5, 82, "There are two solutions:                   ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5, 90, "a) modify the network settings and retry to", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5, 98, "   download the package via wi-fi          ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5,106, "b) download manually the new /BLINDASSISTANT", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5,114, "   folder from Visilab website (it is      ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5,122, "   included in the last BlindAssistant zip ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5,130, "   package) and manually copy in the root  ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5,138, "   folder of the memory stick              ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
       
        ndWS_GrWriteLn (5,154, "Press RETRY to attempt again to download ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        ndWS_GrWriteLn (5,162, "the package from Visilab, ABORT to quit  ", COLOR_BLACK, COLOR_GRAY, baerr5_Window, NORENDER);
        
        // Definisci i pulsanti
        
        baerr5_ButtonRetry  = ndCTRL_CreateButton (335, WindowData [baerr5_Window].WSWndHeight - 50, 
                                               385, WindowData [baerr5_Window].WSWndHeight - 30,
                                               baerr5_Window, "err5_Retry", "Retry", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr5_cbButtonRetry, 0, NORENDER);
        
        baerr5_ButtonAbort = ndCTRL_CreateButton (335, WindowData [baerr5_Window].WSWndHeight - 25, 
                                               385, WindowData [baerr5_Window].WSWndHeight - 5,
                                               baerr5_Window, "err5_Abort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr5_cbButtonAbort, 0, NORENDER);
               
        baerr5_NetworkOptions = ndCTRL_CreateButton (100, WindowData [baerr5_Window].WSWndHeight - 25, 
                                               330, WindowData [baerr5_Window].WSWndHeight - 5,
                                               baerr5_Window, "err5_NetwOptions", "Change network", "options", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                               &baerr5_cbChangeNetworkOptions, 0, NORENDER);
        
                                             
        XWindowRender (baerr5_Window);       
        
        // Inizia un ciclo MouseControl per consentire la scelta
        
        baerr5_OptionChoosen=0;
        ndProvideMeTheMouse_Until (&baerr5_OptionChoosen, ND_IS_DIFFERENT, 0, 0, 0);
        
        // Chiudi la finestra di notifica
        
        ndLP_DestroyWindow (baerr5_Window);
        return baerr5_OptionChoosen;
    }
    else
        return BA_ABORT;     // Non è riuscito a creare la finestra: simula ABORT             
} 

#endif


/*
   ----------------------------------------------------------------------------
   IMPOSSIBILITA' DI INIZIALIZZARE IL SISTEMA DI CONTROLLO VOCALE SPHINX
   ----------------------------------------------------------------------------
*/


#ifndef PSP1000_VERSION

int baerr6_Window;
int baerr6_ButtonAbort;
int baerr6_OptionChoosen;


ndint64 baerr6_cbButtonAbort (char *StringID, ndint64 InfoField, char WndHandle)
{
    baerr6_OptionChoosen=BA_ABORT;       
}


int baitf_ErrorInSphinxInitialization ()
{
    baerr6_Window = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "I cannot init Sphinx voice recognizer", 
                                       COLOR_WHITE, COLOR_RED, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (baerr6_Window>=0)
    {
        ndLP_MaximizeWindow (baerr6_Window);
        
        // Output audio dell'errore
        
        flite_text_to_speech ("Error: I cannot initialize Sphinx voice recognizer", VoiceObj, "play");
        
        // Spiega le ragioni dell'errore
        
        ndWS_GrWriteLn (5, 10, "This application uses a system for voice ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5, 18, "control that allows to the blind the use ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5, 26, "of its functionalities.                  ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 42, "Blind Assistant has detected that the     ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5, 50, "voice control system cannot be initialized ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5, 58, "for an internal error.                     ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5, 66, "                                           ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
       
        ndWS_GrWriteLn (5, 82, "Please, contact Visilab Research Center    ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5, 90, "for further help.                          ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5,106, "See the documentation of this program      ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5,114, "(Appendix A) for further informations      ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5,122, "about the mail that you can contact        ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        ndWS_GrWriteLn (5,130, "for technical assistance.                  ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        
        ndWS_GrWriteLn (5,154, "The program now will be terminated.        ", COLOR_BLACK, COLOR_GRAY, baerr6_Window, NORENDER);
        
        // Definisci i pulsanti
        
        baerr6_ButtonAbort = ndCTRL_CreateButton (335, WindowData [baerr6_Window].WSWndHeight - 25, 
                                               385, WindowData [baerr6_Window].WSWndHeight - 5,
                                               baerr6_Window, "err6_ok", "OK", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &baerr6_cbButtonAbort, 0, NORENDER); 
                                             
        XWindowRender (baerr6_Window);       
        
        // Inizia un ciclo MouseControl per consentire la scelta
        
        baerr6_OptionChoosen=0;
        ndProvideMeTheMouse_Until (&baerr6_OptionChoosen, ND_IS_DIFFERENT, 0, 0, 0);
        
        // Chiudi la finestra di notifica
        
        ndLP_DestroyWindow (baerr6_Window);
        return -1;
    }
    else
        return BA_ABORT;     // Non è riuscito a creare la finestra: simula ABORT             
} 

#endif

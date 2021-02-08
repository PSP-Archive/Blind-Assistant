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
   SOTTOSISTEMA DELL'ENGINE VIDEO PER LA FUNZIONALITA' NULL-MODE
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
    -------------------------------------------------------------------------------
    VARIABILI
    -------------------------------------------------------------------------------
*/ 



/*
    -------------------------------------------------------------------------------
    CODICE PRINCIPALE
    -------------------------------------------------------------------------------
*/ 

void videng_NULLMODE_Constructor ()
{
     // Provvedi a settare la corretta risoluzione del sensore video
     cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_640_480);
    
     // Provvedi a settare la modalità video del sensore
     cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, ND_USBCAM_EFFECTMODE_NORMAL);

     // Provvedi a settare la risoluzione video della videocamera
     baChangeCameraResolution (STD_NOFUNC__CAMERAIMG__DIMX, STD_NOFUNC__CAMERAIMG__DIMY, 8);

     // Segnala la funzione per mezzo della function bar
     mainitf_FunctionBar ("Null function mode", RENDER);
      
     // Messaggio che segnala l'attivazione della funzione null
     baaud_NoFunctionModeEnabled ();
}

void videng_NULLMODE_Core ()
{
     // Prendi una immagine
     cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
     if( !cvCameraImage ) return;
                                      
     // Visualizza l'immagine OpenCV
     zcvShowImage8bit (cvCameraImage, 35, 3, 285, 180, mainitf_Window, RENDER);
}

void videng_NULLMODE_Destructor ()
{
     /* Nothing to do */
}


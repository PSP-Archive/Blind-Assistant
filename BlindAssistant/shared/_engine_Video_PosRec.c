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
   SOTTOSISTEMA DELL'ENGINE VIDEO PER LA FUNZIONALITA' POSITION RECOGNIZER
   ----------------------------------------------------------------------------
*/

        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        #undef  ND_NOT_MAIN_SOURCE
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        #include <pspusbcam.h>
        
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

void videng_POSREC_Constructor ()
{
    // Provvedi a settare la corretta risoluzione del sensore video
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_480_272);

    // Provvedi a settare la modalità video del sensore
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, ND_USBCAM_EFFECTMODE_BLACKWHITE);

    // Cambia la risoluzione della videocamera
    baChangeCameraResolution (STD_BLDSVR__SAMPLEIMG__DIMX, STD_BLDSVR__SAMPLEIMG__DIMY, 8); 
    
    // Tenta automaticamente di connetterti al BlindServer
    bldsvrcnx_ServerManualConnect ();
    
    // Segnala la funzione per mezzo della function bar
    mainitf_FunctionBar ("Position recognizer system mode", RENDER);
      
    // Messaggio che segnala l'attivazione della funzione position recognizer
    baaud_PosRecognitionSystemEnabled ();
}

void videng_POSREC_Core (int TrianglePressed)
{
    // Cattura una immagine con la webcam in alta risoluzione
    cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
    if( !cvCameraImage ) return;
    
    // Provvedi a visualizzae l'immagine
    zcvShowImage8bit (cvCameraImage, 35, 3, 285, 180, mainitf_Window, RENDER);
    
    if (TrianglePressed)
    {
         // Se il BlindServer non è disponibile, tenta innanzitutto di avviare manualmente
         // una connessione 
         
         if (!(MainNetConn_IsOperative && MainBldSvrConn_IsOperative)) bldsvrcnx_ServerManualConnect ();
         
         // Adesso riesegui una seconda volta il controllo: se nel precedente passaggio non
         // è riuscito a connettersi al server dovrai emettere una segnalazione di errore
         
         if (MainNetConn_IsOperative && MainBldSvrConn_IsOperative)
         {
             // Provvedi a normalizzare l'immagine inviata per compensare
             // le variazioni di luminosità
             cvEqualizeHist (cvCameraImage, cvCameraImage);
    
             // Invia un messaggio al place engine: in questo modo il
             // sistema avvierà la procedura di richiesta al server
             // senza influire sul thread video principale
             baPlcEng_AskWhereIAmToServer (cvCameraImage);
         }
         else   // Il BlindServer non è disponibile per problemi di connessione                     
         {
             baaud_BlindServerIsNotConnected ();  
         }               
    }  
}

void videng_POSREC_Destructor ()
{
    /* Nothing to do */ 
}


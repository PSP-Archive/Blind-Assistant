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
   SOTTOSISTEMA DELL'ENGINE VIDEO PER LA FUNZIONALITA' OCR
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

void videng_OCR_Constructor ()
{
    // Provvedi a settare la corretta risoluzione del sensore video
    //cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_1024_768);
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_640_480);

    // Provvedi a settare la modalità video del sensore
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, ND_USBCAM_EFFECTMODE_BLACKWHITE);

    // Cambia la risoluzione della videocamera
    baChangeCameraResolution (STD_OCR__CAMERAIMG__DIMX, STD_OCR__CAMERAIMG__DIMY, 8); 
    
    // Se il metodo OCR scelto richiede la connessione al BlindServer, tenta automaticamente di effettuarla
    if (baOcrAPI_IsNeededBlindServerConnection (SystemOptPacket.OcrMethod)) bldsvrcnx_ServerManualConnect ();
    
    // Il processo di scansione è molto oneroso: per tale ragione, per motivi di sicurezza, si provvede sempre a 
    // disattivare l'update engine ed il mail checker prima di iniziare le operazioni
    baInhibitUpdateEngine ();
    baInhibitMailCheckerEngine ();
    
    // Segnala la funzione per mezzo della function bar
    mainitf_FunctionBar ("OCR system mode", RENDER);
      
    // Messaggio che segnala l'attivazione della funzione OCR
    baaud_OpticalCharRecognitionEnabled ();  
}

void videng_OCR_Core (int TrianglePressed)
{
    // Cattura una immagine con la webcam in alta risoluzione
    cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
    if( !cvCameraImage ) return;
    
    // Provvedi a visualizzae l'immagine
    zcvShowImage8bit (cvCameraImage, 35, 3, 285, 180, mainitf_Window, RENDER);
    
    if (TrianglePressed)
    {
        // Se il BlindServer non è disponibile, tenta innanzitutto di avviare manualmente
        // una connessione, ma solo ove questo fosse necessario in base alle richieste
        // dell'OCR scelto 
         
        if (  (!(MainNetConn_IsOperative && MainBldSvrConn_IsOperative)) && (baOcrAPI_IsNeededBlindServerConnection (SystemOptPacket.OcrMethod))  )
        {
              bldsvrcnx_ServerManualConnect (); 
        }
         
        // Adesso riesegui una seconda volta il controllo: se nel precedente passaggio non
        // è riuscito a connettersi al server, e se questo era richiesto dall'OCR scelto,
        // dovrai emettere una segnalazione di errore
        
        if (  (MainNetConn_IsOperative && MainBldSvrConn_IsOperative) || ( !baOcrAPI_IsNeededBlindServerConnection (SystemOptPacket.OcrMethod) ) )
        {
           // Esegui l'analisi OCR sull'immagine che ha subito il grab
           baOcrAPI_DoMainOCRJob (cvCameraImage, SystemOptPacket.OcrMethod, SystemOptPacket.OcrLanguage);
        }
        else   // La connessione BlindServer non è disponibile                   
        {
           baaud_BlindServerIsNotConnected ();  
        }
    } 
}

void videng_OCR_Destructor ()
{
     // Riattiva il mail checker engine e l'update engine in ordine inverso
     baDeInhibitMailCheckerEngine ();
     baDeInhibitUpdateEngine ();
}


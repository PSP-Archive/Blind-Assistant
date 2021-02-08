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
   SOTTOSISTEMA DELL'ENGINE VIDEO PER LA FUNZIONALITA' COLOR-SCANNER
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

void videng_COLORSCANNER_Constructor ()
{
    // Provvedi a settare la corretta risoluzione del sensore video
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_480_272);

    // Provvedi a settare la modalità video del sensore
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, ND_USBCAM_EFFECTMODE_NORMAL);

    // Cambia la risoluzione della videocamera
    baChangeCameraResolution (STD_COLORSCANNER__CAMERAIMG__DIMX, STD_COLORSCANNER__CAMERAIMG__DIMY, 24); 
    
    // Segnala la funzione per mezzo della function bar
    mainitf_FunctionBar ("Color scanner mode", RENDER);
      
    // Messaggio che segnala l'attivazione della funzione OCR
    baaud_ColorScannerEnabled ();  
}

void videng_COLORSCANNER_Core (int TrianglePressed)
{
    // Destinato ad accogliere i valori medii dell'intensità dei canali
    static int    AverageR, AverageG, AverageB;
    static float  RatioAverageR, RatioAverageG, RatioAverageB;
    static float  Average;
    
    // Altre variabili
    static char ScannerMessage [128];
    int  BestColor;
    
    // Cattura una immagine con la webcam a colori
    cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
    if( !cvCameraImage ) return;
    
    // Provvedi a visualizzae l'immagine a colori
    zcvShowImage24bit (cvCameraImage, 35, 3, 285, 180, mainitf_Window, NORENDER);           

    // Calcola i valori medii dei canali RGB dell'immagine
    zcvCalculateAverageRGBChannels (cvCameraImage, &AverageR, &AverageG, &AverageB);
        
    // Calcola la luminosità dal canale RGB media
    zcvCalculateAverageRGBBrightness (cvCameraImage, &Average);
    
    if (Average!=0)
    {
        // Calcola le medie colore in rapporto alla luminosità
        RatioAverageR = (float)(AverageR)/(float)(Average);
        RatioAverageG = (float)(AverageG)/(float)(Average);
        RatioAverageB = (float)(AverageB)/(float)(Average);
    }
    else
    {
        RatioAverageR = 0;
        RatioAverageG = 0;
        RatioAverageB = 0;
    }  
        
    // Visualizza un messaggio che mostra i livelli dello scanner colori
    sprintf (ScannerMessage, "Color scanner R:%1.3f G:%1.3f B:%1.3f", RatioAverageR, RatioAverageG, RatioAverageB);
    mainitf_FunctionBar (ScannerMessage, RENDER);
  
    if (TrianglePressed)
    {
       BestColor = baColorScanner_DeterminateTheMinimumDistanceColor (RatioAverageR, RatioAverageG, RatioAverageB);
       baaud_ColorScannerResult (AverageR, AverageG, AverageB, BestColor);
    } 
}

void videng_COLORSCANNER_Destructor ()
{
     /* Nothing to do */
}


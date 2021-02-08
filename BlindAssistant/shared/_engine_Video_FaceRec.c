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
   SOTTOSISTEMA DELL'ENGINE VIDEO PER LA FUNZIONALITA' FACE RECOGNIZER
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

#ifndef PSP1000_VERSION

// Destinati ad accogliere i dati in arrivo dalla videocamera
IplImage *cvScaledImgForFD;
IplImage *cvBalancedImgForFD;
IplImage *cvFaceImage;

// Destinato ad accogliere i risultati dell'analisi face detection
static struct FDRecord_Type FDRecord;

// Contatore per il numero di frame e per il numero di frame senza un viso
int vdengNrFrame;
int vdengNrImagesWithoutFaces;

// Fattori di scaling precalcolati
float Factor_ScaledImg_To_CameraImg_X, Factor_ScaledImg_To_CameraImg_Y; 
float Factor_CameraImg_To_ShownImg_X,  Factor_CameraImg_To_ShownImg_Y;

#endif


/* 
   ----------------------------------------------------------------------------
   ROUTINES DI SUPPORTO DEL COSTRUTTORE
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

static void ReLoadAllFaceDatabase_GraphCallback (int NrSlot, int InitSysCode, struct OpInfoPacket_Type *InfoPacket)
{
    switch (InitSysCode)
    {
           case BEGIN_TO_LOAD_IMG_SAMPLE:
           {
              // Genera una finestra sullo schermo per informare l'utente che il sistema sta caricando il 
              // database dei campioni facciali
              InfoPacket->WndHandle = ndLP_CreateWindow (0, 180, 320, 250, "Waiting", COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY10, COLOR_BLACK, NO_RESIZEABLE);  
              ndLP_MaximizeWindow (InfoPacket->WndHandle);
    
              // Visualizza i messaggi di stato
              ndWS_GrWriteLn (5, 5, "The system is loading the samples..", COLOR_WHITE, COLOR_GRAY10, InfoPacket->WndHandle, NORENDER);
    
              // Barra di controllo
              ndTBAR_DrawProgressBar   (10, 25, 300, 40, 0, 0, 100, COLOR_WHITE, COLOR_GREEN, COLOR_GRAY10, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, NORENDER); 
    
              // Rendering finale
              XWindowRender (InfoPacket->WndHandle);
              break;
           }
           
           case LOADING_IMG_SAMPLE:
           {
              ndTBAR_DrawProgressBar   (10, 25, 300, 40, 100.0*(float)(NrSlot)/(float)(NR_SLOTS_FACE_RECOGNIZER), 0, 100, COLOR_WHITE, COLOR_GREEN, COLOR_GRAY10, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
                      
           case LOADED_IMG_SAMPLE:
           {
              ndTBAR_DrawProgressBar   (10, 25, 300, 40, 100.0*((float)(NrSlot)+0.5)/(float)(NR_SLOTS_FACE_RECOGNIZER), 0, 100, COLOR_WHITE, COLOR_GREEN, COLOR_GRAY10, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;    
           }
           
           case DONE3:
           {
              ndTBAR_DrawProgressBar   (10, 25, 300, 40, 100.0, 0, 100, COLOR_WHITE, COLOR_GREEN, COLOR_GRAY10, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER); 
             
              ndDelay (0.5);
              ndLP_DestroyWindow (InfoPacket->WndHandle);
             
              break;
           }
    }   
}


static void ReLoadAllFaceDatabase (void)
{
    static struct OpInfoPacket_Type InitFsInfoPacket;
    
    // Provvedi al caricamento
    baLoadAllSampleImagesOfTheSystem (&ReLoadAllFaceDatabase_GraphCallback, &InitFsInfoPacket);
}

#endif



/* 
   ----------------------------------------------------------------------------
   ROUTINES DI SUPPORTO DELLA FUNZIONE CORE
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

static inline void  videng_FaceDetection(IplImage  *PntToImage, struct FDRecord_Type *PntToFDRecord)
/*
    Questa routine esegue l'algoritmo OpenCV per la face detection.
 
    I risultati dell'operazione vengono registrati nella struct di tipo
    FDRecord_Type che è puntata dal secondo parametro: si tratta del
    numero di face zone rilevate (che non può essere superiore a 10,
    le altre zone vengono ignorate) e le relative coordinate
*/
{
    register int   probe, i;
    register float thres;
    
    cvEqualizeHist ( PntToImage,  cvBalancedImgForFD );
    cvClearMemStorage ( FDStorage );
     
    // Calcoliamo due variabili: le prendiamo dalla struct SystemOptions. Non possono
    // essere passate direttamente alle routine perchè una ha bisogno di un cast
    // (lo facciamo solo una volta per ragioni di efficienza), l'altra ha bisogno
    // di essere divisa per 100 
     
    probe        = (int)(SystemOptPacket.FaceDetectorProbingArea);   
    thres        = SystemOptPacket.FaceDetectorThreshold*0.01;
    
    CvSeq* faces = cvHaarDetectObjects ( cvBalancedImgForFD, FDCascade, FDStorage, thres, 2, 0, cvSize(probe, probe) );
    
    if (faces)
    {
        if (faces->total<10)
           PntToFDRecord->NrFaces = faces->total;
        else
           PntToFDRecord->NrFaces = 10;
        
        for( i = 0; ((i < faces->total) && (i<10)); i++ )
        {
            CvPoint pt1, pt2;        
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
            
            PntToFDRecord->RectCoord [i].PosX1 = r->x;
            PntToFDRecord->RectCoord [i].PosY1 = r->y;
            PntToFDRecord->RectCoord [i].PosX2 = r->x + r->width;
            PntToFDRecord->RectCoord [i].PosY2 = r->y + r->height;
            
            PntToFDRecord->Area [i] = r->width*r->height;
        } 
    
        return;
    }
    else
    {
        PntToFDRecord->NrFaces = 0; 
        return;
    }        
}

#endif

/* 
   ----------------------------------------------------------------------------
   ROUTINES DI SUPPORTO DEL DISTRUTTORE
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

static void UnLoadAllFaceDatabase_GraphCallback (int NrSlot, int InitSysCode, struct OpInfoPacket_Type *InfoPacket)
{
    switch (InitSysCode)
    {
           case BEGIN_TO_UNLOAD:
           {
              // Genera una finestra sullo schermo per informare l'utente che il sistema sta scaricando il 
              // database dei campioni facciali
              InfoPacket->WndHandle = ndLP_CreateWindow (0, 180, 320, 250, "Waiting", COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY10, COLOR_BLACK, NO_RESIZEABLE);  
              ndLP_MaximizeWindow (InfoPacket->WndHandle);
    
              // Visualizza i messaggi di stato
              ndWS_GrWriteLn (5, 5, "The system is unloading the samples..", COLOR_WHITE, COLOR_GRAY10, InfoPacket->WndHandle, NORENDER);
    
              // Barra di controllo
              ndTBAR_DrawProgressBar   (10, 25, 300, 40, 0, 0, 100, COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY10, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, NORENDER); 
    
              // Rendering finale
              XWindowRender (InfoPacket->WndHandle);
              break;
           }
           
           case UNLOADING_SAMPLE:
           {
              ndTBAR_DrawProgressBar   (10, 25, 300, 40, 100.0*(float)(NrSlot)/(float)(NR_SLOTS_FACE_RECOGNIZER), 0, 100, COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY10, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
                       
           case DONE0:
           {
              ndTBAR_DrawProgressBar   (10, 25, 300, 40, 100.0, 0, 100, COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY10, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER); 
             
              ndDelay (0.5);
              ndLP_DestroyWindow (InfoPacket->WndHandle);
             
              break;
           }
    }   
}

static void UnLoadAllFaceDatabase (void)
{
    static struct OpInfoPacket_Type InitFsInfoPacket;
    
    // Provvedi al caricamento
    baUnLoadAllSampleImagesOfTheSystem (&UnLoadAllFaceDatabase_GraphCallback, &InitFsInfoPacket);
}

#endif


/*
    -------------------------------------------------------------------------------
    CODICE PRINCIPALE
    -------------------------------------------------------------------------------
*/ 

void videng_FACEREC_Constructor ()
{
   #ifndef PSP1000_VERSION
      
      // Messaggio per indicare che Blind Assistant sta caricando dal disco
      baaud_Loading ();
      
      // Provvedi a settare la corretta risoluzione del sensore video
      cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_640_480);
    
      // Provvedi a settare la modalità video del sensore
      cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, PSP_USBCAM_EFFECTMODE_NORMAL);
    
      // Provvedi a settare la risoluzione video software della videocamera: l'acquisizione
      // avviene ad una risoluzione massima. Per risparmiare cicli macchina, si invierà una
      // immagine scalata al face detector
      baChangeCameraResolution (STD_FACEREC__CAMERAIMG__DIMX, STD_FACEREC__CAMERAIMG__DIMY, 8);
      
      // Alloca l'area di memoria che conterrà i dati del face detector
      FDStorage = cvCreateMemStorage(0);
      
      // Ricarica dal disco l'intero database immagini in memoria
      ReLoadAllFaceDatabase ();
      
      // Provvedi ad allocare le immagini OpenCV che conterranno i dati da analizzare
      cvScaledImgForFD    = cvCreateImage( cvSize (STD_FACEREC__SCALEDIMGFD__DIMX, STD_FACEREC__SCALEDIMGFD__DIMY), 8, 1);
      cvBalancedImgForFD  = cvCreateImage( cvSize (STD_FACEREC__SCALEDIMGFD__DIMX, STD_FACEREC__SCALEDIMGFD__DIMY), 8, 1 );
      cvFaceImage         = cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), 8, 1 );
      
      // Precalcola queste costanti
      Factor_ScaledImg_To_CameraImg_X = (float)(STD_FACEREC__CAMERAIMG__DIMX)/(float)(STD_FACEREC__SCALEDIMGFD__DIMX);
      Factor_ScaledImg_To_CameraImg_Y = (float)(STD_FACEREC__CAMERAIMG__DIMY)/(float)(STD_FACEREC__SCALEDIMGFD__DIMY);
      Factor_CameraImg_To_ShownImg_X  = 250.0/(float)(STD_FACEREC__CAMERAIMG__DIMX);
      Factor_CameraImg_To_ShownImg_Y  = 177.0/(float)(STD_FACEREC__CAMERAIMG__DIMY);
    
      // Reinizializza il contatore del nr frames
      vdengNrFrame              = 0;
      vdengNrImagesWithoutFaces = 0;
    
      // Segnala la funzione per mezzo della function bar
      mainitf_FunctionBar ("Face recognition mode", RENDER);
      
      // Messaggio che segnala l'attivazione della funzione face recognition
      baaud_FaceRecognitionSystemEnabled ();
   
   #endif
}

void videng_FACEREC_Core ()
{
    #ifndef PSP1000_VERSION 
    
      static float Average;                // Per la luminosità media dell'immagine
        
      int       BestElement, BestArea;
      int       Counter;
      TypeColor Colore;
        
      int       ZPosX, ZPosY;
      int       Result; 
        
        
      // Prendi una immagine
      cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
      if( !cvCameraImage ) return;
        
      // Incrementa il contatore frame
      vdengNrFrame++;
      
      // Visualizza l'immagine OpenCV
      zcvShowImage8bit (cvCameraImage, 35, 3, 285, 180, mainitf_Window, NORENDER);
    
      // Resetta i registri di presenza dei visi per il frame corrente
      baResetPresenceRegisterForFrame ();
        
      // Provvedi a creare una immagine scalata per la face detection
      zcvCopy (cvCameraImage, cvScaledImgForFD);
                
      // Esegue la face detection sull'immagine scalata
      videng_FaceDetection (cvScaledImgForFD, &FDRecord);
        
      if (FDRecord.NrFaces>0)    // Se ci sono facce sullo schermo
      {
                int X1,    Y1,    X2,    Y2;
                int camX1, camY1, camX2, camY2;
                
                // Resetta il contatore delle immagine senza viso
                vdengNrImagesWithoutFaces = 0;
                
                // Il sistema prende in considerazione esclusivamente il rettangolo
                // più grande. Questo consente di escludere eventuali falsi positivi
                // del face detector e permette di risparmiare tempo di calcolo
                
                BestElement=0;
                BestArea=0;
                
                for (Counter=0; Counter<FDRecord.NrFaces; Counter++)
                {
                    if (FDRecord.Area [Counter] > BestArea)
                    {
                        BestArea    = FDRecord.Area [Counter];
                        BestElement = Counter;             
                    }
                }
                
                // Determina le coordinate del viso all'interno dell'immagine
                // catturata dalla webcam
                
                camX1 =  Factor_ScaledImg_To_CameraImg_X*FDRecord.RectCoord [BestElement].PosX1;
                camY1 =  Factor_ScaledImg_To_CameraImg_Y*FDRecord.RectCoord [BestElement].PosY1;
                camX2 =  Factor_ScaledImg_To_CameraImg_X*FDRecord.RectCoord [BestElement].PosX2;
                camY2 =  Factor_ScaledImg_To_CameraImg_Y*FDRecord.RectCoord [BestElement].PosY2;
                
                // Disegna i rettangoli: nota che il disegno avviene nel Window Space e
                // non nell'immagine OpenCV...
                
                X1 =   35 + Factor_CameraImg_To_ShownImg_X*camX1;
                Y1 =    3 + Factor_CameraImg_To_ShownImg_Y*camY1;
                X2 =   35 + Factor_CameraImg_To_ShownImg_X*camX2;
                Y2 =    3 + Factor_CameraImg_To_ShownImg_Y*camY2;
                
                // Copia dentro FaceImage l'area dove è stata rilevata una faccia,
                // normalizzandone le dimensioni
                    
                zcvCopyRegion (cvCameraImage, cvFaceImage, camX1, camY1, camX2, camY2);
                
                // Calcola la luminosità media dell'immagine
                //zcvCalculateAvgBrightness (cvFaceImage, &Average);
                
                // Esegui il bilanciamento della luminosità
                //cvEqualizeHist (cvFaceImage, cvFaceImage);
                
                // Esegui il bilanciamento della luminosità e calcola
                // la luminosità media
                zcvNormalizeImage8BitUsingZone (cvFaceImage, &Average);
                
                if (Average>=SystemOptPacket.MinBrightnessLevel)        // L'illuminazione è sufficiente
                {
                        // Calcola l'elemento di minima distanza: il viso ottenuto verrà
                        // memorizzato in Result
                                
                        Result = baUpdateDistanceData (cvFaceImage);
                                 
                        if (Result>=0)        // Abbiamo trovato un viso
                        {       
                             // Disegna i rettangoli di gestione per l'elemento
                                 
                             Colore = ndWndColorVector [Result];
                                 
                             ndWS_DrawSpRectangle (X1,   Y1,   X2,   Y2,    Colore, Colore, ND_VOID, mainitf_Window, NORENDER); 
                             ndWS_DrawSpRectangle (X1+1, Y1+1, X2-1, Y2-1,  Colore, Colore, ND_VOID, mainitf_Window, NORENDER); 
                                 
                             ZPosX = (camX1+camX2)/2;
                             ZPosY = (camY1+camY2)/2;
                        
                             // Segna la presenza di una persona nel registro presenze
                             
                             baSignThePresenceOfAPerson (ZPosX, ZPosY, Result);      
            
                        } 
                        else     // Valore negativo. C'è un qualche tipo di errore 
                        { 
                             switch (Result) 
                             {
                                case -1:    // Non ci sono elementi con cui eseguire il confronto
                                {
                                     // Disegna il rettangolo di gestione
                                     ndWS_DrawSpRectangle (X1, Y1, X2, Y2, 0, COLOR_BLACK, ND_VOID, mainitf_Window, NORENDER); 
                                     
                                     break;
                                }
                                
                                case -2:    // C'è un elemento ma non è una faccia nota
                                {
                                     // Disegna il rettangolo di gestione
                                     ndWS_DrawSpRectangle (X1, Y1, X2, Y2, 0, COLOR_LBLUE, ND_VOID, mainitf_Window, NORENDER); 
                                     ndWS_GrWriteLn (X2-10, Y2-10, "?", COLOR_WHITE, COLOR_GRAY05, mainitf_Window, NORENDER); 
                                     break;
                                }
                                
                             }       // End of switch
                           
                        }   // End of else per valore negativo e codice di errore 
                                 
                            
                }
                else    // Illuminazione insufficiente
                {
                     // Disegna i rettangoli di gestione   
                     ndWS_DrawSpRectangle (X1, Y1, X2, Y2, 0, COLOR_RED, ND_VOID, mainitf_Window, NORENDER); 
                     
                     // Segnala che non c'è illuminazione sufficiente
                     baaud_NoLight ();
                }    
                
                baDispatchMessageForPeoplePresence ();
                // Emetti un messaggio audio che segnala all'utente che ci sono 
                // persone nella stanza 
         
      }   
      else    // Non ci sono visi
      {
                // Incrementa di 1 il contatore dei frame senza viso
                vdengNrImagesWithoutFaces++;
      }
        
      // Esegui il rendering dell'immagine
      XWindowRender (mainitf_Window);  
        
    #endif                    
}

void videng_FACEREC_Destructor ()
{
    #ifndef PSP1000_VERSION
    
      // Libera la memoria che era stata occupata
      cvReleaseImage (&cvFaceImage);
      cvReleaseImage (&cvBalancedImgForFD);     
      cvReleaseImage (&cvScaledImgForFD);
    
      // Scarica dalla memoria il database dei sample 
      UnLoadAllFaceDatabase ();
    
      // Rilascia l'area di memoria del face detector
      cvReleaseMemStorage (&FDStorage);
      
    #endif
}


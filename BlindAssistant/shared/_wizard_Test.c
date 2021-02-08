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
   WIZARD PER IL TEST DI RICONOSCIMENTO
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
 

/*
   ----------------------------------------------------------------------------
   SIMBOLI
   ----------------------------------------------------------------------------
*/ 

// Messaggi per la ragione di uscita

#define WZTEST_GONEXT      1
#define WZTEST_GOBACK      2
#define WZTEST_REQABORT   -1

// Dimensioni del rettangolo risultati

#define TESTRESULT_POSX1 260
#define TESTRESULT_POSY1   5
#define TESTRESULT_POSX2 410 
#define TESTRESULT_POSY2 180

#define TESTRESULT_LENX  150
#define TESTRESULT_LENY  175



/*
   ----------------------------------------------------------------------------
   VARIABILI 
   ----------------------------------------------------------------------------
*/ 

IplImage *wztest_cvCameraImage;

IplImage *wztest_cvBalancedImgForFD, *wztest_cvScaledImgForFD;
IplImage *wztest_cvFaceImage;


int  wztestYouCanExit;
int  wztestExitReason;
int  wztestWindow;

char wztestButtonExit; 
char wztestButtonOk;
   
char wztest_IsAlreadyOpened = 0;   

int  VSlotVector [NR_SLOTS_FACE_RECOGNIZER];
int  FirstVSlot, NrVSlotsOnScreen;
char VSlotCfgIsChanged;



/*
   ----------------------------------------------------------------------------
   INIZIALIZZAZIONE/DEINIZIALIZZAZIONE SISTEMA VIDEO
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static void INTERNAL_wztestInitGoCamSystem () 
{
   // Inibisci l'engine video principale. Questo eviterà conflitti.
   baInhibitVideoEngine ();
    
   // Provvedi a settare la corretta risoluzione del sensore video
   cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_640_480);
    
   // Provvedi a settare la modalità video del sensore
   cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, PSP_USBCAM_EFFECTMODE_NORMAL);
   
   // Provvedi a settare la risoluzione video software della videocamera: l'acquisizione
   // avviene ad una risoluzione massima. Per risparmiare cicli macchina, si invierà una
   // immagine scalata al face detector
   baChangeCameraResolution (STD_FACEREC__CAMERAIMG__DIMX, STD_FACEREC__CAMERAIMG__DIMY, 8);                      
}

static void INTERNAL_wztestEND_DeInitGoCamSystem () 
{
   // Al riavvio del sistema, non è più necessario (come invece era in BA CFW0005) reinizializzare
   // la webcam con l'opzione Force, perchè abbiamo imposto che la modalità test possa
   // essere avviata solo ed esclusivamente mentre la modalità face recognizer è operativa
   
   baDeInhibitVideoEngine_NoForce ();    
}

#endif

/*
   ----------------------------------------------------------------------------
   ROUTINES DI SUPPORTO
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static inline void   wztest_FaceDetection (IplImage  *PntToImage, struct FDRecord_Type *PntToFDRecord)
/*
    Questa routine esegue l'algoritmo OpenCV per la face detection.
 
    I risultati dell'operazione vengono registrati nella struct di tipo
    FDRecord_Type che è puntata dal secondo parametro: si tratta del
    numero di face zone rilevate (che non può essere superiore a 10,
    le altre zone vengono ignorate) e le relative coordinate
*/
{
    register int probe, i;
    register float thres;
    
    cvEqualizeHist ( PntToImage, wztest_cvBalancedImgForFD );
    cvClearMemStorage ( FDStorage );
    
    // Calcoliamo due variabili: le prendiamo dalla struct SystemOptions. Non possono
    // essere passate direttamente alle routine perchè una ha bisogno di un cast
    // (lo facciamo solo una volta per ragioni di efficienza), l'altra ha bisogno
    // di essere divisa per 100 
     
    probe        = (int)(SystemOptPacket.FaceDetectorProbingArea);   
    thres        = SystemOptPacket.FaceDetectorThreshold*0.01;
    
    CvSeq* faces = cvHaarDetectObjects ( wztest_cvBalancedImgForFD, FDCascade, FDStorage, thres, 2, 0, cvSize(probe, probe) );
    
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
   ROUTINES DI INIZIALIZZAZIONE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static void INTERNAL_AllocateTmpImages (void)
{
   int Counter;
   
   wztest_cvScaledImgForFD   = cvCreateImage( cvSize (STD_FACEREC__SCALEDIMGFD__DIMX, STD_FACEREC__SCALEDIMGFD__DIMY), 8, 1);
   wztest_cvBalancedImgForFD = cvCreateImage( cvSize (STD_FACEREC__SCALEDIMGFD__DIMX, STD_FACEREC__SCALEDIMGFD__DIMY), 8, 1 );
   wztest_cvFaceImage        = cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), 8, 1 );
}

#endif


/*
   ----------------------------------------------------------------------------
   INTERFACCIA GRAFICA
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

int INTERNAL_wztestDrawInterface ()
/*
     Questa routine provvede a disegnare l'interfaccia grafica per il test del
     face recognition. La routine restituisce 0 in caso di successo ed 1 in caso
     di errore.
*/
{   
    FirstVSlot        = 0; 
    NrVSlotsOnScreen  = 4;
    VSlotCfgIsChanged = 1;
    
    wztestWindow = ndLP_CreateWindow (10, 10, 445, Windows_MaxScreenY-10, "Face recognition test", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wztestWindow>=0)
    {
        ndLP_MaximizeWindow (wztestWindow);
        
        ndWS_DrawRectangle (  5,   5, 125, 125, COLOR_WHITE, COLOR_BLACK, wztestWindow, NORENDER); 
        ndWS_DrawRectangle (130,   5, 250, 125, COLOR_WHITE, COLOR_BLACK, wztestWindow, NORENDER); 
        
        // Riquadro per i test results
        ndWS_DrawRectangle (TESTRESULT_POSX1, TESTRESULT_POSY1, TESTRESULT_POSX2, TESTRESULT_POSY2, 
                                 COLOR_WHITE, COLOR_BLACK, wztestWindow, NORENDER); 
        
        wztestButtonExit = ndCTRL_CreateButton (335, WindowData [wztestWindow].WSWndHeight - 20, 
                                                410, WindowData [wztestWindow].WSWndHeight - 2,
                                                wztestWindow, "wztextExit", "Exit", "", 
                                                COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                ND_BUTTON_ROUNDED, 
                                                0, 0, NORENDER);
         
        // Messaggio per l'utente
        
        ndWS_GrWriteLn (8, 140, "Evaluate the accuracy of the", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
        ndWS_GrWriteLn (8, 148, "Pentland analysis.          ", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
        ndWS_GrWriteLn (8, 164, "The widest bar indicates the", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
        ndWS_GrWriteLn (8, 172, "best result in recognition  ", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
                                           
        XWindowRender (wztestWindow);                                     
        return 0;
    }
    else
        return -1;
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE PER ERRORE DI MODALITA'
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wztestOk (char *StringID, ndint64 InfoField, char WndHandle)
{
   wztestYouCanExit=1;
   return 0;   
}


int INTERNAL_wztestDrawInterfaceModeMismatch (void)
{ 
    wztestWindow = ndLP_CreateWindow (10, 10, 445, Windows_MaxScreenY-10, "Face recognition test", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wztestWindow>=0)
    {
        ndLP_MaximizeWindow (wztestWindow);
        
        ndWS_GrWriteLn (5, 10, "You are not in face recognition mode.  ", COLOR_BLUE,  COLOR_GRAY, wztestWindow, NORENDER);
        
        ndWS_GrWriteLn (5, 26, "This function can work only when the   ", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
        ndWS_GrWriteLn (5, 34, "face recognition mode is operative.    ", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
        ndWS_GrWriteLn (5, 42, "You can switch to this mode going into ", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
        ndWS_GrWriteLn (5, 50, "the menu Fnc, Enable face recognizer   ", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
    
        ndWS_GrWriteLn (5, 66, "Now click on Continue and after switching", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
        ndWS_GrWriteLn (5, 74, "retry to select this function.           ", COLOR_BLACK, COLOR_GRAY, wztestWindow, NORENDER);
    
        
        wztestButtonOk   = ndCTRL_CreateButton  (310, WindowData [wztestWindow].WSWndHeight - 25, 
                                               385, WindowData [wztestWindow].WSWndHeight - 5,
                                               wztestWindow, "wztestOk", "Continue", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &cb_wztestOk, 0, NORENDER);
                                              
        XWindowRender (wztestWindow);                                     
        return 0;
    }
    else
        return -1;
} 

#endif


/*
   ----------------------------------------------------------------------------
   GESTIONE NAMESLOT
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static void INTERNAL_wztest0_VisualizeDistances (char RenderNow)
/* 
    Provvede a tutto il processo di visualizzazione dei nomi e di ordinamento
    delle distanze
*/
{
    struct facerec_VectorForOrder_Type SwapElement;
    
    int  Counter, CounterSlot, CounterSample, CounterTarget, VSlotCounter, NrSlot;
    
    int  PosX1,   PosY1,   PosX2,   PosY2;
    int  L2PosX1, L2PosY1, L2PosX2, L2PosY2; 
    int  R2PosX1, R2PosY1, R2PosX2, R2PosY2;
    
    char InternalBuffer [32];
    char YouCanExit;

    float MaximumDistance, CValue;
    
    // Cancelliamo il vecchio rettangolo dei nomi
        
    ndWS_DrawRectangle (TESTRESULT_POSX1, TESTRESULT_POSY1, TESTRESULT_POSX2, TESTRESULT_POSY2, 
                                 COLOR_WHITE, COLOR_BLACK, wztestWindow, NORENDER); 
      
    // Inizializza il vettore per l'ordinamento delle distanze  
         
    for (Counter=0; Counter<NR_SLOTS_FACE_RECOGNIZER; Counter++)
    {
        facerec_VectorOrder [Counter].IsActive=0;
    }
    
    // Ora carica tutte le immagini con le relative distanze all'interno del vettore
    
    Counter=0;
    
    for (CounterSlot=0; CounterSlot<NR_SLOTS_FACE_RECOGNIZER; CounterSlot++)
    {
        if ( facerec_ImagesInRam [CounterSlot].IsActive )
        {
                // Caricamento dei dati
                
                facerec_VectorOrder [Counter].NrSlot   = CounterSlot;
                facerec_VectorOrder [Counter].Distance = facerec_ImagesInRam [CounterSlot].GeneralDistance;
                facerec_VectorOrder [Counter].IsActive = 1;
                
                Counter++;
               
        }
    }
    
    // Iniziamo una analisi bubble sort che ha due funzioni: deve ordinare le distanze e deve
    // porre tutti gli elementi attivi all'inizio
    
    YouCanExit=0;
    
    while (!YouCanExit)
    {
         YouCanExit=1;     // Sarà cambiato in seguito se viene eseguito uno switch tra gli elementi
    
         for (Counter=0; Counter<(NR_SLOTS_FACE_RECOGNIZER-1); Counter++)
         {
             // L'elemento successivo è attivo mentre questo no
             
             if   (   (!facerec_VectorOrder [Counter].IsActive) && (facerec_VectorOrder [Counter+1].IsActive)   )
             {
                  SwapElement = facerec_VectorOrder [Counter+1];
                      
                  facerec_VectorOrder [Counter+1] = facerec_VectorOrder [Counter];
                  facerec_VectorOrder [Counter]   = SwapElement;
                      
                  YouCanExit=0;    // Provocherà la ripetizione del loop                
                  continue;  
             }
             
             // La distanza dell'elemento successivo è più piccola
             
             if   (   (facerec_VectorOrder [Counter].IsActive) && (facerec_VectorOrder [Counter+1].IsActive)   )
             {
                  if (facerec_VectorOrder [Counter].Distance>facerec_VectorOrder [Counter+1].Distance)
                  {
                      SwapElement = facerec_VectorOrder [Counter+1];
                      
                      facerec_VectorOrder [Counter+1] = facerec_VectorOrder [Counter];
                      facerec_VectorOrder [Counter]   = SwapElement;
                      
                      YouCanExit=0;    // Provocherà la ripetizione del loop  
                      continue;              
                  }  
             }
         }
    } 
            
    // Ok, adesso dobbiamo determinare il massimo allo scopo di normalizzare        
         
    MaximumDistance=0;  
             
    for (CounterTarget=0; CounterTarget<NR_SLOTS_FACE_RECOGNIZER; CounterTarget++)
    {
        if (facerec_VectorOrder [CounterTarget].IsActive)
        {
            if (facerec_VectorOrder [CounterTarget].Distance > MaximumDistance)
            {
                MaximumDistance = facerec_VectorOrder [CounterTarget].Distance;                
            }
        }
    }         
    
    if (MaximumDistance>0)
    {
         // Calcoliamo le distanze normalizzate
         
         for (CounterTarget=0; CounterTarget<NR_SLOTS_FACE_RECOGNIZER; CounterTarget++)
         {
            if (facerec_VectorOrder [CounterTarget].IsActive)
            {
                if ((MaximumDistance - facerec_MinimumDistance)!=0)
                {
                   facerec_VectorOrder [CounterTarget].NormDistance = (facerec_VectorOrder [CounterTarget].Distance - facerec_MinimumDistance) / (MaximumDistance - facerec_MinimumDistance);                
                }
                else
                {
                   // Se c'è un solo elemento MaximumDistance = facerec_MinimumDistance, e la 
                   // precedente normalizzazione provocherebbe una divisione per zero
                   
                   facerec_VectorOrder [CounterTarget].NormDistance = (facerec_VectorOrder [CounterTarget].Distance - facerec_MinimumDistance);
                }
            }
         } 
         
         // Ok: disegniamo i nomi degli elementi
         
         VSlotCounter=0;
         
         for (CounterTarget=0; CounterTarget<NR_SLOTS_FACE_RECOGNIZER; CounterTarget++)
         {
            if (facerec_VectorOrder [CounterTarget].IsActive)
            {
                  // Determiniamo la posizione del vslot
                
                  if (VSlotCounter<6)
                  {  
                     {
                         PosX1 = TESTRESULT_POSX1 + 5;
                         PosY1 = TESTRESULT_POSY1 + 6 + 27*VSlotCounter+1;
                         PosX2 = TESTRESULT_POSX2 - 5;
                         PosY2 = TESTRESULT_POSY1 + 6 + 27*(VSlotCounter+1)-1;
                 
                         // Risali dal VSlot al numero di Slot
                         
                         NrSlot = facerec_VectorOrder [CounterTarget].NrSlot;             
                         ndStrCpy (InternalBuffer, facerec_ImagesInRam [NrSlot].Name, 17, 1);
                         
                         ndWS_GrWriteLn (PosX1+2, PosY1+8, InternalBuffer, COLOR_BLACK, COLOR_WHITE, wztestWindow, NDKEY_SETFONT(4) | NORENDER);
                     
                         // Provvedi a disegnare le barre di distanza
                         
                         CValue = 1 - facerec_VectorOrder [CounterTarget].NormDistance;
                         
                         L2PosX1 = 10 + PosX1;
                         L2PosY1 =      PosY1 + 19;
                         L2PosX2 = 10 + PosX1 + CValue*120;
                         L2PosY2 =      PosY1 + 22;
                         ndWS_DrawRectangle (L2PosX1, L2PosY1, L2PosX2, L2PosY2, ndWndColorVector [VSlotCounter], ndWndColorVector [VSlotCounter], wztestWindow, NORENDER);    

                         R2PosX1 = 10 + PosX1 + CValue*120;
                         R2PosY1 = L2PosY1;
                         R2PosX2 = 10 + PosX1 + 120;
                         R2PosY2 = L2PosY2;
                         ndWS_DrawRectangle (R2PosX1, R2PosY1, R2PosX2, R2PosY2, COLOR_WHITE, COLOR_WHITE, wztestWindow, NORENDER);    

                     }
                     
                     VSlotCounter++;
                  }
            }
         }
                          
    }
    
    if (RenderNow) XWindowRender (wztestWindow);
} 

#endif





/*
   ----------------------------------------------------------------------------
   ESECUZIONE DEL TEST
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wztest0_Exit (char *StringID, ndint64 InfoField, char WndHandle)
{
   wztestYouCanExit = 1;  
}

static ndint64 cb_wztest0_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wztestYouCanExit = 1;  
}


void INTERNAL_wztest0_StartTest (void)
/*
    Esegue materialmente il test di riconoscimento mostrando i risultati in
    tempo reale
*/
{
    int   camX1, camX2, camY1, camY2;
    int   X1,    X2,    Y1,    Y2;
    int   Counter, BestElement, BestArea; 
    char  Evid;
    
    // Calcolo di fattori di moltiplicazione che saranno usati nel seguito
    float Factor_ScaledImg_To_CameraImg_X = (float)(STD_FACEREC__CAMERAIMG__DIMX)/(float)(STD_FACEREC__SCALEDIMGFD__DIMX);
    float Factor_ScaledImg_To_CameraImg_Y = (float)(STD_FACEREC__CAMERAIMG__DIMY)/(float)(STD_FACEREC__SCALEDIMGFD__DIMY);
    
    float Factor_CameraImg_To_WindowImg_X = 120.0/(float)(STD_FACEREC__CAMERAIMG__DIMX);
    float Factor_CameraImg_To_WindowImg_Y = 120.0/(float)(STD_FACEREC__CAMERAIMG__DIMY);
    
    // Destinato ad accogliere i risultati dell'analisi face detection
    static struct FDRecord_Type FDRecord;
    
    // Setta le callback ai valori necessari
    ndCTRL_ChangeCallBack (&cb_wztest0_Exit, 0, wztestButtonExit, wztestWindow, NORENDER);
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wztest0_CloseWnd, 0, 0, wztestWindow);
    
    // Inizia il ciclo di richiesta immagine
    
    wztestYouCanExit    = 0;
    wztestExitReason    = 0;
    
    while (!wztestYouCanExit)
    {
        wztest_cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
        if( !wztest_cvCameraImage ) break;
        
        // Provvedi a creare una immagine scalata per la face detection
        zcvCopy (wztest_cvCameraImage, wztest_cvScaledImgForFD);
            
        // Esegue la face detection
        wztest_FaceDetection (wztest_cvScaledImgForFD, &FDRecord);
        
        // Visualizza l'immagine OpenCV
        zcvShowImage8bit (wztest_cvCameraImage, 5, 5, 125, 125, wztestWindow, NORENDER);
        
        if (FDRecord.NrFaces>0)
        {
                // Determina quale, tra i rettangoli presenti nel sistema, ha
                // l'area maggiore. Sarà preso in considerazione esclusivamente questo
                
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
          
                // Disegna il rettangolo di gestione sullo schermo
                                                        
                X1 =   5 + Factor_CameraImg_To_WindowImg_X*camX1;
                Y1 =   5 + Factor_CameraImg_To_WindowImg_Y*camY1;
                X2 =   5 + Factor_CameraImg_To_WindowImg_X*camX2;
                Y2 =   5 + Factor_CameraImg_To_WindowImg_Y*camY2;
                                                        
                ndWS_DrawSpRectangle (X1,   Y1,   X2,   Y2,    COLOR_BLACK, COLOR_RED, ND_VOID, wztestWindow, NORENDER); 
                ndWS_DrawSpRectangle (X1+1, Y1+1, X2-1, Y2-1,  COLOR_BLACK, COLOR_RED, ND_VOID, wztestWindow, NORENDER); 
                
                // Copia l'ingrandimento del viso
                zcvCopyRegion  (wztest_cvCameraImage, wztest_cvFaceImage, camX1, camY1, camX2, camY2);
                
                 // Compensa le variazioni di luminosità
                //cvEqualizeHist (wztest_cvFaceImage, wztest_cvFaceImage); 
                zcvNormalizeImage8BitUsingZone (wztest_cvFaceImage, NULL);
                
                // Mostra l'immagine
                zcvShowImage8bit (wztest_cvFaceImage, 130, 5, 250, 125, wztestWindow, NORENDER);                        
        
                // Update dei dati di distanza
        
                baUpdateDistanceData    (wztest_cvFaceImage);
                INTERNAL_wztest0_VisualizeDistances (NORENDER);
        }
        
        XWindowRender (wztestWindow);
    }

    return;
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE FINALE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

void INTERNAL_wztestEND_DestroyInterface (void)
{
     ndLP_DestroyWindow (wztestWindow); 
}  

void INTERNAL_wztestEND_FreeTmpImg (void)
{ 
    // wztest_cvCameraImage non deve essere sottoposto alla funzione cvReleaseImage
    // perchè punta ad un buffer interno (gestito da ndHighGUI: è quello che
    // è associato a cvQueryFrame
    
    if (wztest_cvFaceImage)         cvReleaseImage (&wztest_cvFaceImage);
    if (wztest_cvBalancedImgForFD)  cvReleaseImage (&wztest_cvBalancedImgForFD);
    if (wztest_cvScaledImgForFD)    cvReleaseImage (&wztest_cvScaledImgForFD);
}

#endif

/*
   ----------------------------------------------------------------------------
   ROUTINE PRINCIPALE
   ----------------------------------------------------------------------------
*/ 


int wzTestForFaceRecognition (void)
/*
     Questa routine avvia il wizard per il test del face recognition. La 
     routine restituisce il codice 0 se non vi sono errori ed un codice 
     negativo in caso di errore.
     
     Sono previsti i seguenti codici di errore:
        BAERR_TROUBLE_WITH_GRAPH     Problemi con l'interfaccia grafica
       
*/
{
    #ifndef PSP1000_VERSION
    
        int ErrRep;
        
        if ( (!wzvwsample_IsAlreadyOpened) && (!wzft_IsAlreadyOpened) && (!wzpl_IsAlreadyOpened) && (!wztest_IsAlreadyOpened) )
        {
                sceKernelChangeThreadPriority (sceKernelGetThreadId (), baMainThreadPriority);   // Dai la stessa priorità del main thread                    
                
                if (baMode==BA_MODE_FACEREC)
                {
                            // Alloca i buffer interni
                            
                            INTERNAL_AllocateTmpImages ();
                            
                            // Disegna l'interfaccia grafica
                                 
                            ErrRep=INTERNAL_wztestDrawInterface ();    
                            if (ErrRep) goto wztest_ErrorInGrInterf;          // Problemi nella creazione dell'interfaccia grafica 
                     
                        wztest_Step0:
                            INTERNAL_wztestInitGoCamSystem ();                // Reinizializza la GoCam
                            wztest_IsAlreadyOpened=1; 
                            
                            // Avvia il processo di test
                            
                            INTERNAL_wztest0_StartTest ();
                        
                        wztest_AllIsOK:  
                            INTERNAL_wztestEND_DeInitGoCamSystem (); 
                            INTERNAL_wztestEND_DestroyInterface (); 
                            INTERNAL_wztestEND_FreeTmpImg ();
                            
                            wztest_IsAlreadyOpened=0;         
                            return 0;
                            
                        wztest_ErrorInGrInterf:
                            INTERNAL_wztestEND_FreeTmpImg ();
                            
                            wztest_IsAlreadyOpened=0;                          
                            return BAERR_TROUBLE_WITH_GRAPH;  
                 
                 }
                 else     // La modalità di gestione è errata    
                 {
                            INTERNAL_wztestDrawInterfaceModeMismatch ();
                            wztest_IsAlreadyOpened=1; 
                        
                            // Locka il cursore e fallo comparire sullo schermo, in modo che l'utente possa
                            // premere il tasto Continue....
                        
                            wztestYouCanExit=0;
                            ndProvideMeTheMouse_Until (&wztestYouCanExit, ND_IS_EQUAL, 1, 0, 0);
                            
                            // ... pulsante premuto. Distruggi interfaccia
                            
                            INTERNAL_wztestEND_DestroyInterface (); 
                            
                            wztest_IsAlreadyOpened=0;         
                            return 0;
                 }
        }
        
    #endif
}
 
void HALT_wzTestForFaceRecognition (void)
{
    #ifndef PSP1000_VERSION
    
        if (wztest_IsAlreadyOpened) 
        {
            wztestYouCanExit=1;
            wztestExitReason=-1;
        } 
        
        ndDelay (0.5);
        
    #endif
}

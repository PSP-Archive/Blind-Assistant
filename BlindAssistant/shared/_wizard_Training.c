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
   WIZARD PER IL FACE TRAINING
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
   VARIABILI PER USO INTERNO
   ----------------------------------------------------------------------------
*/ 

int  wzftYouCanExit;
int  wzftExitReason;
int  wzftWindow;
char wzftButtonAbort;
char wzftButtonNext;
char wzftButtonBack;
char wzftTxtAreaName;
char wzftButtonVoiceTest;

char wzft_IsAlreadyOpened = 0;

   
// Messaggi per la ragione di uscita

#define WZFT_GONEXT      1
#define WZFT_GOBACK      2
#define WZFT_REQABORT   -1

static char TextAreaBuffer [64];

IplImage *wzft_cvTmpImage [NR_SAMPLES_FOR_FACE];

IplImage *wzft_cvCameraImage;

IplImage *wzft_cvBalancedImgForFD, *wzft_cvScaledImgForFD;
IplImage *wzft_cvFaceImage;






/*
   ----------------------------------------------------------------------------
   ROUTINES DI SUPPORTO
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static inline void   wzft_FaceDetection (IplImage  *PntToImage, struct FDRecord_Type *PntToFDRecord)
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
    
    cvEqualizeHist ( PntToImage, wzft_cvBalancedImgForFD );
    cvClearMemStorage ( FDStorage );
    
    // Calcoliamo due variabili: le prendiamo dalla struct SystemOptions. Non possono
    // essere passate direttamente alle routine perchè una ha bisogno di un cast
    // (lo facciamo solo una volta per ragioni di efficienza), l'altra ha bisogno
    // di essere divisa per 100 
     
    probe        = (int)(SystemOptPacket.FaceDetectorProbingArea);   
    thres        = SystemOptPacket.FaceDetectorThreshold*0.01;
    
    CvSeq* faces = cvHaarDetectObjects ( wzft_cvBalancedImgForFD, FDCascade, FDStorage, thres, 2, 0, cvSize(probe, probe) );
    
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
   REGISTRAZIONE DEI NUOVI SAMPLE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static void INTERNAL_RegisterNewSamples_InXPCAStruct (int Slot)
{
    int Counter;
    
    facerec_ImagesInRam [Slot].IsActive=1;
    ndStrCpy ( &(facerec_ImagesInRam [Slot].Name), &TextAreaBuffer, 32, 0);
    
    for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
    {
        if ((facerec_ImagesInRam [Slot].Img [Counter])==0) 
           facerec_ImagesInRam [Slot].Img [Counter] = cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), 8, 1 );                    
        
        zcvCopy (wzft_cvTmpImage [Counter], facerec_ImagesInRam [Slot].Img [Counter]);       
    }   
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE INIZIALE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static void INTERNAL_AllocateTmpImages (void)
{
    int Counter;
   
    for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
    {
        wzft_cvTmpImage [Counter]=cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), 8, 1 );  
    }
    
    wzft_cvScaledImgForFD   = cvCreateImage( cvSize (STD_FACEREC__SCALEDIMGFD__DIMX, STD_FACEREC__SCALEDIMGFD__DIMY), 8, 1);
    wzft_cvBalancedImgForFD = cvCreateImage( cvSize (STD_FACEREC__SCALEDIMGFD__DIMX, STD_FACEREC__SCALEDIMGFD__DIMY), 8, 1 );
    wzft_cvFaceImage        = cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), 8, 1 );
}


int INTERNAL_wzftDrawInterface (int Slot)
/*
     Questa routine provvede a disegnare l'interfaccia grafica del wizard per
     il face training. La routine restituisce 0 in caso di successo ed 1 in caso
     di errore.
*/
{
    wzftWindow = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Face training wizard", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wzftWindow>=0)
    {
        ndLP_MaximizeWindow (wzftWindow);
        
        wzftButtonBack  = ndCTRL_CreateButton (335, WindowData [wzftWindow].WSWndHeight - 75, 
                                               385, WindowData [wzftWindow].WSWndHeight - 55,
                                               wzftWindow, "wzftBack", "Back", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED, 
                                               0, 0, NORENDER);
        
        wzftButtonNext  = ndCTRL_CreateButton (335, WindowData [wzftWindow].WSWndHeight - 50, 
                                               385, WindowData [wzftWindow].WSWndHeight - 30,
                                               wzftWindow, "wzftNext", "Next", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED, 
                                               0, 0, NORENDER);
        
        wzftButtonAbort = ndCTRL_CreateButton (335, WindowData [wzftWindow].WSWndHeight - 25, 
                                               385, WindowData [wzftWindow].WSWndHeight - 5,
                                               wzftWindow, "wzftAbort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               0, 0, NORENDER);
                                             
        // Indica lo slot interessato
        ndWS_GrPrintLn (340, 5, COLOR_BLACK, COLOR_GRAY, wzftWindow, NDKEY_SETFONT(3), "%d ", Slot+1);
        
        XWindowRender (wzftWindow);                                     
        return 0;
    }
    else
        return -1;
}

#endif

/*
   ----------------------------------------------------------------------------
   CODICE A STEP0
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wzft0_GoNext (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GONEXT;     
}

static ndint64 cb_wzft0_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;    
}

static ndint64 cb_wzft0_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;      
}


void INTERNAL_wzft0_ChooseAnImage (int Slot, int Sample)
/*
    Questa routine richiede alla webcam un frame, vi esegue la routine di
    face detection per rilevare un viso al suo interno, ed infine copia
    l'immagine scalata in cvTmpImage
*/
{
    int Counter, wzftWeHaveAnImage;
    int BestElement, BestArea;
    int camX1, camY1, camX2, camY2;
    int X1, X2, Y1, Y2;
    
    // Destinato ad accogliere i risultati dell'analisi face detection
    struct FDRecord_Type FDRecord;
    
    // Calcolo di fattori di moltiplicazione che saranno usati nel seguito
    float Factor_ScaledImg_To_CameraImg_X = (float)(STD_FACEREC__CAMERAIMG__DIMX)/(float)(STD_FACEREC__SCALEDIMGFD__DIMX);
    float Factor_ScaledImg_To_CameraImg_Y = (float)(STD_FACEREC__CAMERAIMG__DIMY)/(float)(STD_FACEREC__SCALEDIMGFD__DIMY);
    
    float Factor_CameraImg_To_WindowImg_X = 150.0/(float)(STD_FACEREC__CAMERAIMG__DIMX);
    float Factor_CameraImg_To_WindowImg_Y = 150.0/(float)(STD_FACEREC__CAMERAIMG__DIMY);
    
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 5, 330, 205, COLOR_GRAY, COLOR_GRAY, wzftWindow, NORENDER);
 
    // Indica il numero d'ordine del sample
    ndWS_GrPrintLn (340, 20, COLOR_YELLOW, COLOR_GRAY, wzftWindow, NDKEY_SETFONT(3), "%d ", Sample+1);
        
    // Crea due rettangoli bianchi nella finestra
    ndWS_DrawRectangle (  5, 5, 155, 155, COLOR_WHITE, COLOR_BLACK, wzftWindow, NORENDER);
    ndWS_DrawRectangle (175, 5, 325, 155, COLOR_WHITE, COLOR_BLACK, wzftWindow, NORENDER);
        
    // Visualizza un messaggio per l'utente  
    ndWS_GrPrintLn (5, 160, COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER, "I need the #%d sample of this face", Sample+1);
    ndWS_GrWriteLn (5, 168, "Push the NEXT button when you see a", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 176, "rectangle on a face.", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_InhibitButton   (wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_InhibitButton   (wzftButtonNext,  wzftWindow, NORENDER); 
    ndCTRL_DeInhibitButton (wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setta le callback ai valori necessari
    ndCTRL_ChangeCallBack (&cb_wzft0_GoNext, 0, wzftButtonNext,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft0_Abort,  0, wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzft0_CloseWnd, 0, 0, wzftWindow);
    
    // Rendering     
    XWindowRender (wzftWindow); 
    
    // Inizia il ciclo di richiesta immagine
    
    wzftWeHaveAnImage = 0;
    
    wzftYouCanExit    = 0;
    wzftExitReason    = 0;
    
    while (!wzftYouCanExit)
    {
        // Ottieni un'immagine dalla videocamera
        
        wzft_cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
        if( !wzft_cvCameraImage ) break;
            
        // Provvedi a creare una immagine scalata per la face detection
        zcvCopy (wzft_cvCameraImage, wzft_cvScaledImgForFD);
                                            
        // Esegue la face detection
        wzft_FaceDetection (wzft_cvScaledImgForFD, &FDRecord);
        
        // Visualizza l'immagine OpenCV
        zcvShowImage8bit (wzft_cvCameraImage, 5, 5, 155, 155, wzftWindow, NORENDER);
        
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
                                                        
                ndWS_DrawSpRectangle (X1,   Y1,   X2,   Y2,    COLOR_BLACK, COLOR_RED, ND_VOID, wzftWindow, NORENDER); 
                ndWS_DrawSpRectangle (X1+1, Y1+1, X2-1, Y2-1,  COLOR_BLACK, COLOR_RED, ND_VOID, wzftWindow, NORENDER); 
                
                // Copia l'ingrandimento del viso
                zcvCopyRegion  (wzft_cvCameraImage, wzft_cvFaceImage, camX1, camY1, camX2, camY2);
                
                // Provvedi a compensare le variazioni di luminosità
                //cvEqualizeHist (wzft_cvFaceImage, wzft_cvFaceImage);
                zcvNormalizeImage8BitUsingZone (wzft_cvFaceImage, NULL);
                
                // Mostra l'immagine del viso che è stata catturata
                zcvShowImage8bit (wzft_cvFaceImage, 175, 5, 325, 155, wzftWindow, NORENDER);   
                
                if (!wzftWeHaveAnImage)   // Adesso un'immagine l'abbiamo 
                {
                   wzftWeHaveAnImage=1;
                   ndCTRL_DeInhibitButton (wzftButtonNext, wzftWindow, NORENDER);   
                }
        }
        
        XWindowRender (wzftWindow);
    }

    return;
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE A STEP1
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

static ndint64 cb_wzft1_GoNext (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GONEXT;     
}

static ndint64 cb_wzft1_GoBack (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GOBACK;     
}

static ndint64 cb_wzft1_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;    
}

static ndint64 cb_wzft1_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;      
}


void INTERNAL_wzft1_ConfirmThatThisIsAFace (void)
/*
    In questo step l'utente deve confermare che sullo schermo è effettivamente
    stata catturata una faccia
*/
{
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 5, 330, 205, COLOR_GRAY, COLOR_GRAY, wzftWindow, NORENDER);
     
    // Crea un rettangolo bianco al centro della finestra...
    ndWS_DrawRectangle ( 40, 5, 190, 155, COLOR_WHITE, COLOR_BLACK, wzftWindow, NORENDER);
    
    // ... e di disegna all'interno l'immagine che era stata catturata
    zcvShowImage8bit (wzft_cvFaceImage, 40, 5, 190, 155, wzftWindow, NORENDER);     
    
    // Visualizza un messaggio per l'utente  
    ndWS_GrWriteLn (5, 160, "This is the face that the software", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 168, "has captured. Click on Next if the", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 176, "rectangle above really contains a ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 184, "face, otherwise click on Back and ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 192, "retry the capture ",                 COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_DeInhibitButton (wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_DeInhibitButton (wzftButtonNext,  wzftWindow, NORENDER); 
    ndCTRL_DeInhibitButton (wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setta le callback ai valori necessari
    ndCTRL_ChangeCallBack (&cb_wzft1_GoBack, 0, wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft1_GoNext, 0, wzftButtonNext,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft1_Abort,  0, wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzft1_CloseWnd, 0, 0, wzftWindow);
    
    // Rendering
    XWindowRender (wzftWindow); 
    
    // Ritardo necessario per impedire pressioni multiple
    ndDelay (0.5);
    
    wzftYouCanExit    = 0;
    wzftExitReason    = 0;
    
    ndProvideMeTheMouse_Until (&wzftYouCanExit, ND_IS_EQUAL, 1, 0, 0);
    
    // Il controllo ritornerà al sistema solo quando l'utente avrà premuto un
    // pulsante
    
    return;
}

void INTERNAL_wzft1_RegisterAnImage (int Sample)
/*
     Questa routine provvede a registrare la faccia catturata dentro
     l'immagine wzft_cvTmpImage[Sample]. Normalmente, questa faccia
     si trova dentro l'immagine wzft_cvFaceImage
*/
{
     zcvCopy   (wzft_cvFaceImage, wzft_cvTmpImage [Sample]);                
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE A STEP2
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

static ndint64 cb_wzft2_GoNext (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GONEXT;     
}

static ndint64 cb_wzft2_GoBack (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GOBACK;     
}

static ndint64 cb_wzft2_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;    
}

static ndint64 cb_wzft2_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;      
}



void INTERNAL_wzft2_ShowMeASummary (void)
/*
    In questo step viene mostrato un riepilogo dei quattro campioni che
    erano stati inseriti 
*/
{
    int CounterX, CounterY, NrElement;
    int PosX1, PosY1, PosX2, PosY2;
    
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 5, 330, 205, COLOR_GRAY, COLOR_GRAY, wzftWindow, NORENDER);
     
    for (CounterY=0; CounterY<=2; CounterY++)
    {
        for (CounterX=0; CounterX<=2; CounterX++)
        {
            PosX1 =  5 + 65*CounterX;
            PosY1 =  5 + 50*CounterY;
            PosX2 =  5 + 65*(CounterX+1);
            PosY2 =  5 + 50*(CounterY+1);
            
            NrElement = 3*CounterY + CounterX;
            
            if (NrElement!=4)
            {
                // Crea gli otto rettangoli che conterranno gli otto sample...
                ndWS_DrawRectangle (PosX1, PosY1, PosX2, PosY2, COLOR_WHITE, COLOR_BLACK, wzftWindow, NORENDER);
            
                // Necessario per impedire che l'immagine sia visualizzata nella sezione centrale
                if (NrElement>4) NrElement=NrElement-1; 
                
                // ... e poni al suo interno le immagini
                zcvShowImage8bit (wzft_cvTmpImage [NrElement], PosX1+1, PosY1+1, PosX2-1, PosY2-1, wzftWindow, NORENDER);  
            }
        }
    }
    
    // Visualizza un messaggio per l'utente  
    ndWS_GrWriteLn (5, 160, "These are the four samples of the ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 168, "same face that have been captured ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 176, "by this software. Click Next to   ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 184, "continue, otherwise click on Back ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 192, "to retry the capture ",              COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_DeInhibitButton (wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_DeInhibitButton (wzftButtonNext,  wzftWindow, NORENDER); 
    ndCTRL_DeInhibitButton (wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setta le callback ai valori necessari
    ndCTRL_ChangeCallBack (&cb_wzft2_GoBack, 0, wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft2_GoNext, 0, wzftButtonNext,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft2_Abort,  0, wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzft2_CloseWnd, 0, 0, wzftWindow);
    
    // Rendering
    XWindowRender (wzftWindow); 
    
    // Ritardo necessario per impedire pressioni multiple
    ndDelay (0.5);
    
    wzftYouCanExit    = 0;
    wzftExitReason    = 0;
    
    ndProvideMeTheMouse_Until (&wzftYouCanExit, ND_IS_EQUAL, 1, 0, 0);
    
    // Il controllo ritornerà al sistema solo quando l'utente avrà premuto un
    // pulsante
    
    return;
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE A STEP3
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wzft3_GoNext (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GONEXT;     
}

static ndint64 cb_wzft3_GoBack (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GOBACK;     
}

static ndint64 cb_wzft3_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;    
}

static ndint64 cb_wzft3_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;      
}


static char cb_wzft3_ProcessName (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
/*
    Questa callback ha il compito di validare la stringa che viene immessa
    come nome della persona che è stata individuata dal software
*/
{       
    if (strlen (TextData)>0)      // La stringa non è vuota
    {
        // Deinibisce i pulsanti
        
        ndCTRL_DeInhibitButton (wzftButtonNext,       wzftWindow, NORENDER);  
        ndCTRL_DeInhibitButton (wzftButtonVoiceTest,  wzftWindow, NORENDER); 
        
        // Pulisce l'area dove teoricamente potrebbe esserci qualche
        // vecchia segnalazione di errore
        
        ndWS_DrawRectangle (5, 160, 330, 180, COLOR_GRAY, COLOR_GRAY, wzftWindow, NORENDER);
        XWindowRender (wzftWindow);
        
        return 1;              // I dati sono accettabili      
    }   
    else
    {
        // Inibisce i pulsanti
        
        ndCTRL_InhibitButton (wzftButtonNext,       wzftWindow, NORENDER);  
        ndCTRL_InhibitButton (wzftButtonVoiceTest,  wzftWindow, NORENDER); 
        
        // Segnalazione di errore
        
        ndWS_GrWriteLn (5, 160, "The name of a person cannot be", COLOR_RED, COLOR_GRAY, wzftWindow, NORENDER);
        ndWS_GrWriteLn (5, 168, "void. Insert a different name.", COLOR_RED, COLOR_GRAY, wzftWindow, NORENDER);
        XWindowRender (wzftWindow);
         
        return 0;              // I dati non sono accettabili 
    }  
}
     
static ndint64 cb_wzft3_TestMyVoice (char *StringID, ndint64 InfoField, char WndHandle)
{
    // Inibizione senza rendering: si impedisce che l'utente possa cambiare
    // step durante il processo di sintesi vocale e che possa tentare di
    // avviare un altro processo di sintesi vocale intanto che il primo
    // è in corso
    
    ndCTRL_ChangeCallBack (0, 0, wzftButtonBack,      wzftWindow, NORENDER); 
    ndCTRL_ChangeCallBack (0, 0, wzftButtonNext,      wzftWindow, NORENDER); 
    ndCTRL_ChangeCallBack (0, 0, wzftButtonAbort,     wzftWindow, NORENDER); 
    ndCTRL_ChangeCallBack (0, 0, wzftButtonVoiceTest, wzftWindow, NORENDER); 
    
    // Segnala
    
    ndWS_GrWriteLn (140, 81, "Listen my voice...", COLOR_BLACK, COLOR_GRAY, wzftWindow, RENDER);
    
    // Usa FLite per sintetizzare la voce
    
    flite_text_to_speech (&TextAreaBuffer, VoiceObj, "play");
    
    // Cancella la segnalazione
    
    ndWS_DrawRectangle (140, 75, 330, 95, COLOR_GRAY, COLOR_GRAY, wzftWindow, RENDER);
    
    // Ristabilisce le callbacks originarie
    
    ndCTRL_ChangeCallBack (&cb_wzft3_GoBack,      0, wzftButtonBack,      wzftWindow, NORENDER); 
    ndCTRL_ChangeCallBack (&cb_wzft3_GoNext,      0, wzftButtonNext,      wzftWindow, NORENDER); 
    ndCTRL_ChangeCallBack (&cb_wzft3_Abort,       0, wzftButtonAbort,     wzftWindow, NORENDER); 
    ndCTRL_ChangeCallBack (&cb_wzft3_TestMyVoice, 0, wzftButtonVoiceTest, wzftWindow, NORENDER); 
    
    return 0;
}



void INTERNAL_wzft3_WhatIsTheNameOfThisPerson (int Slot)
/*
    Questo step provvede a richiedere all'utente qual è il nome della persona
    che era stata in precedenza inquadrata dalla videocamera
*/ 
{
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 5, 330, 205, COLOR_GRAY, COLOR_GRAY, wzftWindow, NORENDER);
    
    // Invia un messaggio all'utente
    ndWS_GrWriteLn (5, 10, "Now, you must tell me what is the", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 18, "name of the person that has been ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 26, "captured by the camera.          ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);

    // Disegna una TextBox
    
    memset (&TextAreaBuffer, 0, 33);
    
    wzftTxtAreaName = ndTBOX_CreateTextArea    (5, 50, 330, 65, "NameTextArea", 
                                                NDKEY_SETTEXTAREA (32, 1), &TextAreaBuffer, 
                                                COLOR_BLACK,  COLOR_WHITE,
                                                COLOR_BLACK,  COLOR_GRAY, 
                                                COLOR_BLACK,  COLOR_GRAY02,
                                                0, &cb_wzft3_ProcessName, 0, 0, 
                                                wzftWindow, NORENDER);
                                                
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_DeInhibitButton (wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_InhibitButton   (wzftButtonNext,  wzftWindow, NORENDER); 
    ndCTRL_DeInhibitButton (wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setta le callback ai valori necessari
    
    ndCTRL_ChangeCallBack (&cb_wzft3_GoBack, 0,    wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft3_GoNext, 0,    wzftButtonNext,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft3_Abort,  0,    wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzft3_CloseWnd, 0, 0, wzftWindow);
    
    // Crea il pulsante per il test FLite
    
    wzftButtonVoiceTest = ndCTRL_CreateButton (5, 75, 130, 95, wzftWindow, "VoiceTestBtn", "Test my voice", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED, 
                                               &cb_wzft3_TestMyVoice, 0, NORENDER);
    
    // Esegui il rendering
    
    XWindowRender (wzftWindow); 
    
    // Ritardo necessario per impedire pressioni multiple
    ndDelay (0.5);
    
    wzftYouCanExit    = 0;
    wzftExitReason    = 0;
    
    ndProvideMeTheMouse_Until (&wzftYouCanExit, ND_IS_EQUAL, 1, 0, 0);
    
    // Il controllo ritornerà al sistema solo quando l'utente avrà premuto un
    // pulsante. 
    
    // Controllo restituito: Ora è necessario cancellare i vecchi elementi grafici, 
    // ormai obsoleti
    
    ndCTRL_DestroyButton (wzftButtonVoiceTest, wzftWindow, 0, COLOR_GRAY, NORENDER);
    ndTBOX_DestroyTextArea (wzftTxtAreaName, wzftWindow, 0, COLOR_GRAY, NORENDER);
    
    return;
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE A STEP4
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION


#define AUDIOGEN_BARSTEP   20
#define IMAGESAVE_BARSTEP  20
#define SIFTSAVE_BARSTEP   2

static ndint64 cb_wzft4_GoNext (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GONEXT;     
}

static ndint64 cb_wzft4_GoBack (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_GOBACK;     
}

static ndint64 cb_wzft4_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;    
}

static ndint64 cb_wzft4_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzftYouCanExit = 1;
   wzftExitReason = WZFT_REQABORT;      
}

static void SaveGraphCallback (int NrSlot, int NrSample, int SaveCode, struct OpInfoPacket_Type *InfoPacket)
{
    float ImageSampleStep;
    
    char  MsgString0 [256];
    char  MsgString1 [16];
    
    ImageSampleStep = (float)(IMAGESAVE_BARSTEP)/(float)(NR_SAMPLES_FOR_FACE);
    
    switch (SaveCode)
    {
           case BEGIN_TO_SAVE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
        
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, InfoPacket->PosX2, InfoPacket->PosY2, 
                                        0, 0, 100, 
                                        COLOR_WHITE, InfoPacket->BarColor, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                
                break;
           }
    
           case SAVING_AUDIO_SAMPLE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                strcpy (MsgString0, "Waiting: saving audio sample #");
                _itoa (NrSample, &MsgString1, 10);
                strcat (MsgString0, MsgString1);
               
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, MsgString0, COLOR_BLACK, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        AUDIOGEN_BARSTEP*NrSample, 0, 100, 
                                        COLOR_WHITE, InfoPacket->BarColor, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;
           }
           
           case SAVED_AUDIO_SAMPLE:
           {
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        AUDIOGEN_BARSTEP*(NrSample+1), 0, 100, 
                                        COLOR_WHITE, InfoPacket->BarColor, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;
           }
           
           case SAVING_IMG_SAMPLE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                strcpy (MsgString0, "Waiting: saving image sample #");
                _itoa (NrSample, &MsgString1, 10);
                strcat (MsgString0, MsgString1);
               
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, MsgString0, COLOR_BLACK, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        60+ImageSampleStep*NrSample, 0, 100, 
                                        COLOR_WHITE, InfoPacket->BarColor, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;
           }
           
           case SAVED_IMG_SAMPLE:
           {
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        60+ImageSampleStep*(NrSample+1), 0, 100, 
                                        COLOR_WHITE, InfoPacket->BarColor, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;
           }
           
           case ERROR_IN_SAVING_IMG_SAMPLE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                strcpy (MsgString0, "ERROR: I could not save image sample #");
                _itoa (NrSample, &MsgString1, 10);
                strcat (MsgString0, MsgString1);
               
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, MsgString0, COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER); 
                        
                break;
           }
           
           case SAVING_NAME:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                strcpy (MsgString0, "Waiting: saving the name of the person");
                
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, MsgString0, COLOR_BLACK, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        80, 0, 100, 
                                        COLOR_WHITE, InfoPacket->BarColor, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;
           }
           
           case SAVED_NAME:
           {
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        85, 0, 100, 
                                        COLOR_WHITE, InfoPacket->BarColor, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;
           }
           
           case ERROR_IN_SAVING_NAME:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                strcpy (MsgString0, "ERROR: I couldn't create file for the name");
                
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, MsgString0, COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER); 
                        
                break;
           }
           
           
           case DONE0:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "Done", COLOR_BLACK, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        100, 0, 100, 
                                        COLOR_WHITE, InfoPacket->BarColor, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;
           }
           
           // Routine eseguite in fase di cancellazione successiva ad un errore di salvataggio
           
           case BEGIN_TO_ERASE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "PARTIAL DATA: Begin erasing", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        0, 0, 100, 
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);                           
                break;     
           } 
           
           
             
           case ERASING_MEMORY:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "PARTIAL DATA: Del mem info", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        10, 0, 100, 
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);    
                  
                break;    
           }    
        
           case ERASING_VOICE_SAMPLE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "PARTIAL DATA: Del voice samples", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        20+NrSample*(20.0/VOICE_SAMPLES_FOR_FACE), 0, 100,  
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
               
                        
                break;  
           }
             
           case ERASED_VOICE_SAMPLE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "PARTIAL DATA: Del voice samples (OK)", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        20+(NrSample+1)*(20.0/VOICE_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;  
           }
             
           case ERASING_IMAGE_SAMPLE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "PARTIAL DATA: Del faces samples ", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        40+NrSample*(20.0/NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;
           }
             
           case ERASED_IMAGE_SAMPLE:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "PARTIAL DATA: Del faces samples (OK)", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        40+(NrSample+1)*(20.0/NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;  
           }
             
           case ERASING_NAME:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "PARTIAL DATA: Del name file", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        60, 0, 100, 
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;  
           }
             
           case ERASED_NAME:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "PARTIAL DATA: Del name file (OK)", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        70, 0, 100, 
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break; 
           }
           
             
           case DONE1:
           {
                ndWS_DrawRectangle (InfoPacket->PosX1, InfoPacket->PosY1, InfoPacket->PosX2, InfoPacket->PosY2, 
                                    InfoPacket->BgColor, InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
       
                ndWS_GrWriteLn     (InfoPacket->PosX1, InfoPacket->PosY2-22, "ALL PARTIAL DATA ERASED", COLOR_RED, 
                                    InfoPacket->BgColor, InfoPacket->WndHandle, NORENDER);
               
                ndTBAR_DrawProgressBar (InfoPacket->PosX1, InfoPacket->PosY2-12, 
                                        InfoPacket->PosX2, InfoPacket->PosY2, 
                                        100, 0, 100, 
                                        COLOR_WHITE, COLOR_GRAY08, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
                break;  
           } 
           
              
    }   
}



void INTERNAL_wzft4_UpdateDataOfFaceRecognizer (int Slot)
/*
    Questa routine provvede a registrare le nuove immagini che sono state 
    catturate nell'apposita struct facerec_ImagesInRam.
    
    Inoltre, essa provvede ad aggiornare le informazioni del face
    recognizer
*/
{
    struct OpInfoPacket_Type SaveInfoPacket;
    int ErrRep, Counter;
    
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 5, 330, 205, COLOR_GRAY, COLOR_GRAY, wzftWindow, NORENDER);
    
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_InhibitButton (wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_InhibitButton (wzftButtonNext,  wzftWindow, NORENDER); 
    ndCTRL_InhibitButton (wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setta le callback ai valori necessari
    
    ndCTRL_ChangeCallBack (&cb_wzft4_GoBack, 0, wzftButtonBack,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft4_GoNext, 0, wzftButtonNext,  wzftWindow, NORENDER);
    ndCTRL_ChangeCallBack (&cb_wzft4_Abort , 0, wzftButtonAbort, wzftWindow, NORENDER); 
    
    // Setup della callback di uscita finestra
    ndLP_SetupCloseWndCallback (&cb_wzft4_CloseWnd, 0, 0, wzftWindow);
    
    // Invia un messaggio all'utente
    ndWS_GrWriteLn (5, 10, "Waiting. I am calculating the", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 18, "parameters for face recognizer", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 26, "using these new samples      ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
   
    XWindowRender (wzftWindow); 
    
    ndWS_GrWriteLn (5, 42, "Done.                        ", COLOR_BLACK, COLOR_GRAY, wzftWindow, RENDER);
    
    INTERNAL_RegisterNewSamples_InXPCAStruct (Slot);     // Memorizza i dati relativi alle immagini 
    baUpdateFaceRecognizerDBase (Slot);                  // Aggiorna i dati del face recognizer   
    
    ndWS_GrWriteLn (5, 58, "Waiting. I am saving new     ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    ndWS_GrWriteLn (5, 66, "informations to disk         ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
    
    // Genera i nuovi campioni audio e memorizzali sulla memory stick
    
    SaveInfoPacket.PosX1     = 5;
    SaveInfoPacket.PosY1     = 80;
    SaveInfoPacket.PosX2     = 330;
    SaveInfoPacket.PosY2     = 110;
    SaveInfoPacket.BarColor  = COLOR_RED;
    SaveInfoPacket.BgColor   = COLOR_GRAY;
    SaveInfoPacket.WndHandle = wzftWindow;
    
    ErrRep=baSave_GenerateAndSaveDataRelativeToASlot (Slot, &SaveGraphCallback, &SaveInfoPacket);
    
    if (ErrRep)
    {
        // Se ci sono stati errori nel salvataggio dei dati, devi cancellare i dati dalla 
        // memoria ram per mantenere il sincronismo tra quanto registrato sul disco e
        // quanto presente in ram, e poi devi cancellare i dati parziali dal disco
    
        baErase_DeleteDataRelativeToASlot (Slot, &SaveGraphCallback, &SaveInfoPacket);   
    
        ndWS_GrWriteLn (5, 120, "Blind Assistant has found troubles", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
        ndWS_GrWriteLn (5, 128, "in saving the new informations    ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
        ndWS_GrWriteLn (5, 136, "on the disk.                      ", COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER);
        ndWS_GrPrintLn (5, 152, COLOR_BLACK, COLOR_GRAY, wzftWindow, NORENDER, "Error code: %d", -ErrRep);
        XWindowRender  (wzftWindow);
    }
    
    // Deinibisce il pulsante di Next che provocherà l'uscita
    
    ndCTRL_DeInhibitButton (wzftButtonNext,  wzftWindow, RENDER); 
    
    // Ritardo necessario per impedire pressioni multiple
    ndDelay (0.5);
    
    wzftYouCanExit    = 0;
    wzftExitReason    = 0;
    
    ndProvideMeTheMouse_Until (&wzftYouCanExit, ND_IS_EQUAL, 1, 0, 0);
    
    return;
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE FINALE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

void INTERNAL_wzftEND_DestroyInterface (void)
{
    ndLP_DestroyWindow (wzftWindow); 
}   

void INTERNAL_wzftEND_FreeTmpImg (void)
{
    int Counter;
    
    for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
    {
        if (wzft_cvTmpImage [Counter]) cvReleaseImage ( &(wzft_cvTmpImage [Counter]) ); 
    }
    
    // wzft_cvCameraImage non deve essere sottoposto alla funzione cvReleaseImage
    // perchè punta ad un buffer interno (gestito da ndHighGUI: è quello che
    // è associato a cvQueryFrame
        
    if (wzft_cvFaceImage)         cvReleaseImage (&wzft_cvFaceImage);
    if (wzft_cvBalancedImgForFD)  cvReleaseImage (&wzft_cvBalancedImgForFD);
    if (wzft_cvScaledImgForFD)    cvReleaseImage (&wzft_cvScaledImgForFD); 
}

#endif


/*
   ----------------------------------------------------------------------------
   ROUTINE PRINCIPALE
   ----------------------------------------------------------------------------
*/ 


int wzFaceTraining (int Slot)
/*
     Questa routine avvia il wizard per il face training. La routine restituisce il 
     codice 0 se non vi sono errori ed un codice negativo in caso di errore.
     
     Sono previsti i seguenti codici di errore:
        BAERR_TROUBLE_WITH_GRAPH     Problemi con l'interfaccia grafica
        BAERR_ABORTED_BY_USER        L'utente ha rinunciato all'operazione  
*/
{
    #ifndef PSP1000_VERSION
    
        int Sample, ErrRep;
        
        if ( (!wzft_IsAlreadyOpened) && (!wzpl_IsAlreadyOpened) && (!wztest_IsAlreadyOpened) )
        {
            // NOTA: Questo thread viene eseguito solo se il viewer per i samples già è
            // stato attivato. Pertanto, non è necessario che esso provveda a settare
            // correttamente la GoCam ed ad inibire il video engine perchè questo è
            // stato già fatto da SampleViewer. 
            
            sceKernelChangeThreadPriority (sceKernelGetThreadId (), baMainThreadPriority);   // Dai la stessa priorità del main thread                    
            INTERNAL_AllocateTmpImages ();
            
            ErrRep=INTERNAL_wzftDrawInterface (Slot);    
            if (ErrRep) goto wzft_ErrorInGrInterf;         // Problemi nella creazione dell'interfaccia grafica 
                
            wzft_Step0:   
                wzft_IsAlreadyOpened = 1;
                
                // Comincia a richiedere campione per campione
                                  
                Sample=0;
                
                while (Sample<NR_SAMPLES_FOR_FACE)
                {
                      wzft_Step0a:
                      
                          INTERNAL_wzft0_ChooseAnImage (Slot, Sample);  
                          if (wzftExitReason == WZFT_REQABORT) goto wzft_AbortOnStage0;
                      
                      wzft_Step1:
                          
                          INTERNAL_wzft1_ConfirmThatThisIsAFace ();
                          if (wzftExitReason == WZFT_GOBACK)   goto wzft_Step0a;
                          if (wzftExitReason == WZFT_REQABORT) goto wzft_AbortOnStage0;
                          
                          INTERNAL_wzft1_RegisterAnImage (Sample);
                          Sample++;
                }
                
            wzft_Step2:    
                
                INTERNAL_wzft2_ShowMeASummary ();
                if (wzftExitReason == WZFT_GOBACK)   goto wzft_Step0;
                if (wzftExitReason == WZFT_REQABORT) goto wzft_AbortOnStage2;
                
            wzft_Step3:
                       
                INTERNAL_wzft3_WhatIsTheNameOfThisPerson (Slot);
                if (wzftExitReason == WZFT_GOBACK)   goto wzft_Step2;
                if (wzftExitReason == WZFT_REQABORT) goto wzft_AbortOnStage3;
                
            wzft_Step4:    
                
                INTERNAL_wzft4_UpdateDataOfFaceRecognizer (Slot);
                    
            wzft_AllIsOK: 
                INTERNAL_wzftEND_DestroyInterface ();
                INTERNAL_wzftEND_FreeTmpImg ();
                
                wzft_IsAlreadyOpened=0;
                return 0;
                
            wzft_AbortOnStage0:
            wzft_AbortOnStage1:
            wzft_AbortOnStage2:
            wzft_AbortOnStage3:
                INTERNAL_wzftEND_DestroyInterface (); 
                INTERNAL_wzftEND_FreeTmpImg ();
                
                wzft_IsAlreadyOpened = 0;
                return BAERR_ABORTED_BY_USER;  
                     
            wzft_ErrorInGrInterf:
                INTERNAL_wzftEND_FreeTmpImg ();
                return BAERR_TROUBLE_WITH_GRAPH;         
        }
        else
          return 0;    // Simula successo. In realtà non fa nulla perchè il wizard è già aperto 
          
    #endif
}
 
 
 


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
   SOTTOSISTEMA DELL'ENGINE VIDEO PER LA FUNZIONALITA' DATA MATRIX SCANNER
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
        
        #include "dmtx.h"
        





/*
    -------------------------------------------------------------------------------
    STRUTTURA OPZIONI STANDARD DMTX
    -------------------------------------------------------------------------------
*/ 

typedef struct 
{
   int codewords;       /* -c, --codewords */
   int edgeMin;         /* -e, --minimum-edge */
   int edgeMax;         /* -E, --maximum-edge */
   int scanGap;         /* -g, --gap */
   int timeoutMS;       /* -m, --milliseconds */
   int newline;         /* -n, --newline */
   int page;            /* -p, --page */
   int squareDevn;      /* -q, --square-deviation */
   int dpi;             /* -r, --resolution */
   int sizeIdxExpected; /* -s, --symbol-size */
   int edgeThresh;      /* -t, --threshold */
   char *xMin;          /* -x, --x-range-min */
   char *xMax;          /* -X, --x-range-max */
   char *yMin;          /* -y, --y-range-min */
   char *yMax;          /* -Y, --y-range-max */
   int correctionsMax;  /* -C, --corrections-max */
   int diagnose;        /* -D, --diagnose */
   int mosaic;          /* -M, --mosaic */
   int stopAfter;       /* -N, --stop-after */
   int pageNumbers;     /* -P, --page-numbers */
   int corners;         /* -R, --corners */
   int shrinkMax;       /* -S, --shrink */
   int shrinkMin;       /* -S, --shrink (if range specified) */
   int unicode;         /* -U, --unicode */
   int verbose;         /* -v, --verbose */
} UserOptions;

/*
    -------------------------------------------------------------------------------
    VARIABILI
    -------------------------------------------------------------------------------
*/ 

static DmtxImage    *MyDmtxImage;
static UserOptions   MyDmtxOption;
static DmtxDecode   *MyDmtxDecoder;
static DmtxRegion   *MyDmtxRegion;
static DmtxMessage  *MyDmtxMessage;
static DmtxTime      MyDmtxTimeOut;

static char *DmtxMessageDecoded;

static int   DmtxIsInhibited;

#define MSG_DECODED_LEN 1024

/*
    -------------------------------------------------------------------------------
    ALTRE FUNZIONI
    -------------------------------------------------------------------------------
*/ 

static int min (int a, int b)
{
    return (a<b) ? a : b;   
} 

static char isaccented (unsigned char x)
{
       switch (x)
       {
              case 'à':
              case 'è':
              case 'é':
              case 'ì':
              case 'ò':
              case 'ù':
                   return 1;
                   
              default:
                   return 0;
       }
}


/*
    -------------------------------------------------------------------------------
    GESTORE OPZIONI STANDARD DMTX
    -------------------------------------------------------------------------------
*/ 

static UserOptions GetDefaultOptions(void)
{
   static UserOptions opt;

   memset(&opt, 0x00, sizeof(UserOptions));

   /* Default options */
   opt.codewords = DmtxFalse;
   opt.edgeMin = DmtxUndefined;
   opt.edgeMax = DmtxUndefined;
   opt.scanGap = 2;
   opt.timeoutMS = DmtxUndefined;
   opt.newline = DmtxFalse;
   opt.page = DmtxUndefined;
   opt.squareDevn = DmtxUndefined;
   opt.dpi = DmtxUndefined;
   opt.sizeIdxExpected = DmtxSymbolShapeAuto;
   opt.edgeThresh = 5;
   opt.xMin = NULL;
   opt.xMax = NULL;
   opt.yMin = NULL;
   opt.yMax = NULL;
   opt.correctionsMax = DmtxUndefined;
   opt.diagnose = DmtxFalse;
   opt.mosaic = DmtxFalse;
   opt.stopAfter = DmtxUndefined;
   opt.pageNumbers = DmtxFalse;
   opt.corners = DmtxFalse;
   opt.shrinkMin = 1;
   opt.shrinkMax = 1;
   opt.unicode = DmtxFalse;
   opt.verbose = DmtxFalse;

   return opt;
}

/*
    -------------------------------------------------------------------------------
    EVIDENZIATORE PER LA REGIONE DMTX INTERESSATA
    -------------------------------------------------------------------------------
*/

void baDataMatrix_PutInEvidenceRegion (DmtxDecode *MyDmtxDecoder, DmtxRegion *MyDmtxRegion)
{
     int height;
     
     float Factor_ScaledImg_To_CameraImg_X = 250.0/(float)(STD_DATAMATRIX__CAMERAIMG__DIMX);
     float Factor_ScaledImg_To_CameraImg_Y = 177.0/(float)(STD_DATAMATRIX__CAMERAIMG__DIMY);
     
     DmtxVector2 p00, p10, p11, p01;
     DmtxVector2 s00, s10, s11, s01;
     DmtxVector2 w00, w10, w11, w01;

     height = dmtxDecodeGetProp(MyDmtxDecoder, DmtxPropHeight);

     p00.X = p00.Y = p10.Y = p01.X = 0.0;
     p10.X = p01.Y = p11.X = p11.Y = 1.0;
     dmtxMatrix3VMultiplyBy(&p00, MyDmtxRegion->fit2raw);
     dmtxMatrix3VMultiplyBy(&p10, MyDmtxRegion->fit2raw);
     dmtxMatrix3VMultiplyBy(&p11, MyDmtxRegion->fit2raw);
     dmtxMatrix3VMultiplyBy(&p01, MyDmtxRegion->fit2raw);
     
     // Calcolo coefficienti S
     
     s00.X = p00.X;
     s10.X = p10.X;
     s11.X = p11.X;
     s01.X = p01.X;
     
     s00.Y = height - 1 - p00.Y;
     s10.Y = height - 1 - p10.Y;
     s11.Y = height - 1 - p11.Y;
     s01.Y = height - 1 - p01.Y;
     
     // Calcolo dei coefficienti w
     
     w00.X = 35 + s00.X*Factor_ScaledImg_To_CameraImg_X;
     w10.X = 35 + s10.X*Factor_ScaledImg_To_CameraImg_X;
     w11.X = 35 + s11.X*Factor_ScaledImg_To_CameraImg_X;
     w01.X = 35 + s01.X*Factor_ScaledImg_To_CameraImg_X;
     
     w00.Y = 3 + s00.Y*Factor_ScaledImg_To_CameraImg_Y;
     w10.Y = 3 + s10.Y*Factor_ScaledImg_To_CameraImg_Y;
     w11.Y = 3 + s11.Y*Factor_ScaledImg_To_CameraImg_Y;
     w01.Y = 3 + s01.Y*Factor_ScaledImg_To_CameraImg_Y;
     
     ndWS_DrawLine (w00.X, w00.Y, w10.X, w10.Y, COLOR_RED, mainitf_Window, NORENDER);
     ndWS_DrawLine (w10.X, w10.Y, w11.X, w11.Y, COLOR_RED, mainitf_Window, NORENDER);
     ndWS_DrawLine (w11.X, w11.Y, w01.X, w01.Y, COLOR_RED, mainitf_Window, NORENDER);
     ndWS_DrawLine (w01.X, w01.Y, w00.X, w00.Y, COLOR_RED, mainitf_Window, NORENDER);
     
     XWindowRender (mainitf_Window); 
}


/*
    -------------------------------------------------------------------------------
    SPEAKER PER IL MESSAGGIO DECODIFICATO
    -------------------------------------------------------------------------------
*/ 

void baDataMatrix_DigitalizeWords (char *MessageDecoded, int MessageLen)
{
    // Destinato ad accogliere i dati sulla pressione del pad
    struct ndDataControllerType ndPadRecord;
    
    char TheWord [MSG_DECODED_LEN];
    char NextChar  [2];
    
    char WndHandle;
    int  PosChar;
    
    WndHandle = ndLP_CreateWindow (Windows_MaxScreenX/2, 50+Windows_MaxScreenY/2, Windows_MaxScreenX-1, Windows_MaxScreenY-1, 
                                   "I have read this for you", COLOR_WHITE, COLOR_BLUE, COLOR_GRAY13, COLOR_GRAY13, NO_RESIZEABLE);
    
    
    
    if (WndHandle>0)
    {
        // Analizza a poco a poco le varie parole
        
        PosChar = 0;
        
        // La stringa NextChar verrà usata per concatenamento
        
        NextChar [0]=0;
        NextChar [1]=0;
        
        // Un primo ciclo che agisce sulle varie parole
        
        while (PosChar<MessageLen)
        {
              strcpy (TheWord, "");     // Crea una stringa vuota
              
              // Un secondo ciclo che agisce sulla singola parola
              
              while (PosChar<MessageLen)
              {  
                  NextChar [0] = *(MessageDecoded+PosChar);
                  PosChar++;
                  
                  if ( (isdigit (NextChar[0])) || (isalpha (NextChar[0])) || (isaccented (NextChar[0])) )       // Se è un carattere o una cifra....
                  {
                       strcat (TheWord, NextChar);
                  }
                  else        // Non è un carattere: termina la parola
                  {
                       break;       
                  }   
              }
              
              // Quando si arriva qui, c'è una parola che va sottoposta a digitalizzazione
              
              if (strlen (TheWord)>0)
              {
                   char ShownWord [32];                   // Buffer temporaneo per la parte di parola mostrata su schermo
                   
                   ndLP_MaximizeWindow (WndHandle);
            
                   ndWS_DrawRectangle (0, 0, 200, 50, COLOR_GRAY13, COLOR_GRAY13, WndHandle, NORENDER);
            
                   ndStrCpy (ShownWord, TheWord, 15, 1);
                   ndWS_GrWriteLn ( (240-8*(strlen(ShownWord)))/2, 21, ShownWord, COLOR_GRAY06, COLOR_GRAY13, WndHandle, NDKEY_SETFONT (3) | RENDER);  
            
                   // Riproduci in realtime la parola richiesta
                    
                   baaud_ReproduceASingleWordRT ( TheWord );     
              }
              
              // Il cieco può, in qualsiasi momento, interrompere la riproduzione premendo il tasto triangolo
              ndHAL_GetPad (&ndPadRecord);
              if (ndPadRecord.Buttons & ND_BUTTON3) break;
        }
        
        // Lavoro finito... andiamo a casa
        
        ndLP_DestroyWindow (WndHandle);  
    }               
}



/*
    -------------------------------------------------------------------------------
    CODICE PRINCIPALE
    -------------------------------------------------------------------------------
*/ 

void videng_DATAMATRIX_Constructor ()
{
     // Provvedi a settare la corretta risoluzione del sensore video
     cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_640_480);
    
     // Provvedi a settare la modalità video del sensore
     cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, ND_USBCAM_EFFECTMODE_NORMAL);

     // Provvedi a settare la risoluzione video della videocamera
     baChangeCameraResolution (STD_DATAMATRIX__CAMERAIMG__DIMX, STD_DATAMATRIX__CAMERAIMG__DIMY, 8);

     // Segnala la funzione per mezzo della function bar
     mainitf_FunctionBar ("Data matrix scanner mode", RENDER);
      
     // Messaggio che segnala l'attivazione della funzione null
     baaud_DataMatrixScannerEnabled ();
     
     // Allora l'area di decodifica messaggio
     DmtxMessageDecoded = malloc (MSG_DECODED_LEN);
     
     // Registro di inibizione a 0
     DmtxIsInhibited = 0;
}



void videng_DATAMATRIX_Core (int TrianglePressed)
{
     // Prendi una immagine
     cvCameraImage = cvQueryFrame ( cvCameraCaptureObj );
     if( !cvCameraImage ) return;
     
     // Visualizza l'immagine OpenCV
     zcvShowImage8bit (cvCameraImage, 35, 3, 285, 180, mainitf_Window, RENDER);
     
     if (DmtxIsInhibited==0)
     {
             // Attiva l'immagine DMTX
             DmtxImage *MyDmtxImage = dmtxImageCreate(cvCameraImage->imageData, cvCameraImage->width, cvCameraImage->height, DmtxPack8bppK);
             
             // Setup delle proprietà
             dmtxImageSetProp(MyDmtxImage, DmtxPropImageFlip, DmtxFlipNone);
             
             // Inizializza il decoder DMTX
             MyDmtxDecoder = dmtxDecodeCreate(MyDmtxImage, 1);
             
             // Il registro MyDmtxTimeOut deve essere reinizializzato dinamicamente ad ogni scansione
             MyDmtxOption.timeoutMS = 1000;
             MyDmtxTimeOut = dmtxTimeAdd(dmtxTimeNow(), MyDmtxOption.timeoutMS);
             
             // Setta le opzioni del decoder
             //SetDecodeOptions (MyDmtxDecoder, MyDmtxImage, &MyDmtxOption);
               
             // Analizza il frame per verificare se sono presenti data matrix:
             // cerca in quali zone dell'immagine è presente una matrice 
             MyDmtxRegion = dmtxRegionFindNext (MyDmtxDecoder, &MyDmtxTimeOut);
            
             // Arresta se non ci sono più regioni da cercare
             if (MyDmtxRegion) 
             {
                // Decode region based on requested barcode mode 
                MyDmtxMessage = dmtxDecodeMatrixRegion(MyDmtxDecoder, MyDmtxRegion, DmtxUndefined);
        
                if (MyDmtxMessage) 
                {
                   // Metti in evidenza l'area interessata
                   baDataMatrix_PutInEvidenceRegion (MyDmtxDecoder, MyDmtxRegion);
                   
                   // Pulisci l'area di decodifica del messaggio
                   memset (DmtxMessageDecoded, 0, MSG_DECODED_LEN-1);
                   
                   // La massima lunghezza del messaggio..
                   int MaxLen = min (MyDmtxMessage->outputIdx, MSG_DECODED_LEN-1);
                   
                   // Copia il messaggio nell'area dei messaggi decodificati
                   memcpy (DmtxMessageDecoded, MyDmtxMessage->output, MaxLen); 
                     
                   // Comunica al cieco il messaggio decodificato
                   mainitf_StatusBar ("Spelling message...", RENDER); 
                   ndHAL_SND_Beep (2, 6);
                   
                   // Inizia il processo di digitalizzazione
                   baDataMatrix_DigitalizeWords (DmtxMessageDecoded, MaxLen);
                   
                   // Fine lavoro
                   mainitf_StatusBar ("", RENDER);
                   ndHAL_SND_Beep (2, 6);
                   
                   // Disalloca la struttura di gestione messaggio
                   dmtxMessageDestroy (&MyDmtxMessage);
                   
                   // Setta il registro di inibizione a 10 frame per evitare ripetizioni della scansione
                   DmtxIsInhibited=10;
                }
                
                dmtxRegionDestroy (&MyDmtxRegion);
            }
         
            dmtxDecodeDestroy (&MyDmtxDecoder);
            dmtxImageDestroy  (&MyDmtxImage);
     }
     else   // Il registro di inibizione è settato
     {
            DmtxIsInhibited--;
     }
}

void videng_DATAMATRIX_Destructor ()
{
     // Pulisci la status bar
     mainitf_StatusBar ("", RENDER);
     
     // Rilascia l'area di memoria del decoder
     free (DmtxMessageDecoded);
}


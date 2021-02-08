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
   ROUTINES METATESSERACT C++
   ----------------------------------------------------------------------------
   Queste routine provvedono a richiamare l'engine del motore ndTesseract. 
   L'API generale viene bypassata. In questo modo, si riesce a gestire in
   modo migliore e più veloce il motore di Tesseract
   ----------------------------------------------------------------------------
*/

        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        #undef ND_NOT_MAIN_SOURCE
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        
        #include "BlindAssistant.h"

        // File headers di ndTesseract
        #include "ccmain/baseapi.h"
        #include "image/img.h"
        #include "image/imgs.h"
        #include "strngs.h"

        


/*
   ----------------------------------------------------------------------------
   STRUTTURE PER ASSISTENZA AL MONITOR DI SISTEMA
   ----------------------------------------------------------------------------
*/

typedef struct                   /*single character */
{
// It should be noted that the format for char_code for version 2.0 and beyond is UTF8
// which means that ASCII characters will come out as one structure but other characters
// will be returned in two or more instances of this structure with a single byte of the
// UTF8 code in each, but each will have the same bounding box.
// Programs which want to handle languagues with different characters sets will need to
// handle extended characters appropriately, but *all* code needs to be prepared to
// receive UTF8 coded characters for characters such as bullet and fancy quotes.
  unsigned short char_code;              /*character itself */
  short          left;                    /*of char (-1) */
  short          right;                   /*of char (-1) */
  short          top;                     /*of char (-1) */
  short          bottom;                  /*of char (-1) */
  short          font_index;              /*what font (0) */
  unsigned char  confidence;              /*0=perfect, 100=reject (0/100) */
  unsigned char  point_size;              /*of char, 72=i inch, (10) */
  char           blanks;                   /*no of spaces before this char (1) */
  unsigned char  formatting;              /*char formatting (0) */
} ZEANYCODE_CHAR;                 /*single character */

typedef bool (*ZCANCEL_FUNC)(void* cancel_this, int words);

typedef struct ZETEXT_STRUCT      /*output header */
{
  short          count;                   /*chars in this buffer(0) */
  short          progress;                /*percent complete increasing (0-100) */
  char           more_to_come;             /*true if not last */
  char           ocr_alive;                /*ocr sets to 1, HP 0 */
  char           err_code;                 /*for errcode use */
  ZCANCEL_FUNC   cancel;            /*returns true to cancel */
  void*          cancel_this;             /*this or other data for cancel*/
  clock_t        end_time;              /*time to stop if not 0*/
  ZEANYCODE_CHAR text[1];         /*character data */
} ZETEXT_DESC;                    /*output header */


// Definisci la callback custom che viene definita dal sistema e
// che gestisce il feedback audio e grafico

typedef int (*CustomProgressCallbackType)(int Progress);

/*
   ----------------------------------------------------------------------------
   VARIABILI DEL GESTORE CALLBACK DI PROGRESSIONE
   ----------------------------------------------------------------------------
*/

static ZETEXT_DESC                  *baNdTesseractMonitor;
static CustomProgressCallbackType    baCustomProgressCallback;


/*
   ----------------------------------------------------------------------------
   CALLBACK STATICA DEL MONITOR DI PROGRESSIONE
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

        static bool baStaticProgressCallback(void* cancel_this, int words)
        {
             int IWantToStop;
             
             // Richiama la callback custom che deve gestire il feedback audio/video
             if (baCustomProgressCallback) 
                IWantToStop = (*baCustomProgressCallback)(baNdTesseractMonitor->progress);
             else
                IWantToStop = 0;
             
             // Tiene viva la scansione OCR fino al termine se la callback ha restituito 0
             return (IWantToStop) ? true : false;             
        }

#endif


/*
   ----------------------------------------------------------------------------
   INIZIO DEL CODICE
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

        void zTesseractImage(const char* input_file, IMAGE* image, STRING* text_out, void *MyCustomCallback) 
        {
          int bytes_per_line = check_legal_image_size(image->get_xsize(), image->get_ysize(), image->get_bpp());
          char* text;
          
          // Alloca la struttura che implementerà il monitor per ndTesseract
          baNdTesseractMonitor = (ZETEXT_DESC *) malloc (sizeof(ZETEXT_DESC));
          memset ( (void *)(baNdTesseractMonitor), 0, sizeof(ZETEXT_DESC));
          
          // Setta la CANCEL callback che sarà usata come monitor di progressione (callback statica)
          baNdTesseractMonitor->cancel = (ZCANCEL_FUNC) baStaticProgressCallback;
          
          // Registra la callback custom che sarà richiamata e che gestirà il feedback audio/video
          baCustomProgressCallback = (CustomProgressCallbackType)(MyCustomCallback);
          
          // Richiama il motore OCR settando il monitor appena allocato
          text = TessBaseAPI::TesseractRectCB (image->get_buffer(), image->get_bpp()/8, bytes_per_line, 0, 0, 
                                               image->get_xsize(), image->get_ysize(), (void *)(baNdTesseractMonitor) );
                                               
          // Lavoro finito: disalloca i dati del monitor di scansione
          free ( (void *)(baNdTesseractMonitor) );
          
          // Risultato out
          *text_out += text;
          delete [] text;                 
        }
        
        int CPP_CALL_ndTesseractForBA (char *FileName, int OcrLanguage, char **PntToResultsArea, void *MyCustomCallback)
        {
            IMAGE image;
            STRING text_out;
            
            char DataPath  [128]   = "Tesseract";
            
            char OutputBasePath [128];
            char OutputBase     [128];        
            char OutputBaseExt  [128];      
            
            // A dispetto del nome OutputBase contiene l'input senza path ed estensione !!!!
            fssSplitNameFile (FileName, OutputBasePath, OutputBase, OutputBaseExt);
            
            // Setta il selettore di linguaggio in base all'impostazione scelta dal sistema
        
            char Language[16];
        
            switch (OcrLanguage)
            {
                   case OCRLANG_ENGLISH:
                   {
                        strcpy (Language, "eng");
                        break;
                   }
                   
                   case OCRLANG_ITALIAN:
                   {
                        strcpy (Language, "ita");
                        break;
                   }
                   
                   case OCRLANG_FRANCOIS:
                   {
                        strcpy (Language, "fra");
                        break;
                   }
        
                   case OCRLANG_GERMAN:
                   {
                        strcpy (Language, "deu");
                        break;
                   }     
        
                   case OCRLANG_SPANISH:
                   {
                        strcpy (Language, "spa");
                        break;
                   }       
        
                   case OCRLANG_DUTCH:
                   {
                        strcpy (Language, "nld");
                        break;
                   }  
            }     
        
            // Prepara le altre variabili da passare al sistema di inizializzazione
            
            char *ConfigFile = NULL;             // ConfigFile va a null
            bool  NumericMode = false;           // NumericMode  a false
            int   ArgC = 0;                      // Numero di argomenti passato pari a 0
            char *ArgV [] = {NULL, NULL, NULL};  // Vettore argomenti emulato: è composto da tre stringhe nulle
        
            // Esegui la reinizializzazione dell'engine di Tesseract al valore richiesto
        
            int result = TessBaseAPI::InitWithLanguage(DataPath, OutputBase, Language, ConfigFile, NumericMode, ArgC, ArgV);
            
            if (result==0)    // Nessun errore nella fase di inizializzazione
            {
                 // Setta il nome del file di input
                 TessBaseAPI::SetInputName (FileName);   
                 
                 if (image.read_header(FileName) >= 0)
                 {
                      if (image.read(image.get_ysize ()) >= 0) 
                      {
                           // Ok! Vai con il motore di scansione
                           zTesseractImage(FileName, &image, &text_out, MyCustomCallback);  
                           
                           // Alloca lo spazio in memoria per contenere il risultato
                           *PntToResultsArea = (char *)(malloc ( text_out.length() + 1));
                           memset (*PntToResultsArea, 0, text_out.length() + 1);
                           
                           // Copia il risultato nell'area di memoria appena allocata
                           memcpy (*PntToResultsArea, text_out.string(), text_out.length());
                           
                           // Rilascia le aree di memoria che sono state utilizzate
                           TessBaseAPI::End();
                           return 0;                       
                      }
                      else   // Errore nella lettura del contenuto della bitmap. Non dovrebbe accadere mai
                      {
                           baaud_ndTessInternalError ();
                           
                           TessBaseAPI::End();
                           return -3;  
                      }                          
                 }
                 else    // Errore nella lettura dell'header dell'immagine di input. Non dovrebbe accadere mai.
                 {
                      baaud_ndTessInternalError ();
                      
                      TessBaseAPI::End();
                      return -2;
                 }     
            }
            else  // Errore di inizializzazione ndTesseract
            {
                baaud_ndTessInitError ();
                
                TessBaseAPI::End();
                return -1;
            }                             
        }
                                          
        // Entry point dell'API per il C++
        
        extern "C"
        {
               
            int C_CALL_ndTesseractForBA (char *FileName, int OcrLanguage, char **PntToResultsArea, void *MyCustomCallback)
            {
                return CPP_CALL_ndTesseractForBA (FileName, OcrLanguage, PntToResultsArea, MyCustomCallback);
            }
               
        }
        
#endif

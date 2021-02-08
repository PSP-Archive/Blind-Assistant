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
   API PER LA GESTIONE DELL'OCR 1 (NDTESSERACT INTERNAL)
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
   VARIABILI INTERNE 
   ----------------------------------------------------------------------------
*/

#define STD_NDTESS_IMG_DIMX STD_OCR__SAMPLEIMG__DIMX*5/8
#define STD_NDTESS_IMG_DIMY STD_OCR__SAMPLEIMG__DIMY*5/8

IplImage *OcrImageEroded;
IplImage *OcrImageResized;
IplImage *OcrImageThresholded;

/*
   ----------------------------------------------------------------------------
   RIFERIMENTI A PROTOTIPI DEFINITI DA UN'ALTRA PARTE
   ----------------------------------------------------------------------------
*/

int C_CALL_ndTesseractForBA (char *FileName, int OcrLanguage, char **PntToResultsArea, void *MyCustomCallback);


/*
   ----------------------------------------------------------------------------
   CALLBACK GRAFICA/AUDIO DI CONTROLLO PROGRESSIONE OCR
   ----------------------------------------------------------------------------
*/


#ifndef PSP1000_VERSION
    
    char INTERNAL_IsTheFirstTimeForCB;
    int  INTERNAL_LastProgressMsgCB;
    int  INTERNAL_ProgressWindowCB;
    
    static int INTERNAL_ProgressCallback (int Progress)
    {
       struct ndDataControllerType ndPadRecord;
       int    TrianglePressed;
       
       // Mostra a video l'avanzamento della scansione
       
       if ( (INTERNAL_IsTheFirstTimeForCB) || (Progress-INTERNAL_LastProgressMsgCB>=10) )
       {
          ndTBAR_DrawProgressBar (5, 5, 105, 17, Progress, 0, 100, COLOR_BLACK, COLOR_LBLUE, COLOR_WHITE, COLOR_BLACK, 
                                  PBAR_PERCENT, INTERNAL_ProgressWindowCB, RENDER);
          
          // Invia un messaggio vocale per informare il cieco della progressione della scansione
          
          baaud_ndProceedXXPerCent (Progress);
          
          // Aggiorna dati
          
          INTERNAL_IsTheFirstTimeForCB = 0;
          INTERNAL_LastProgressMsgCB   = Progress; 
       } 
       
       // Verifica se è stato premuto il tasto TRIANGLE. In quel caso interrompi
       
       ndHAL_GetPad (&ndPadRecord);
       TrianglePressed = (ndPadRecord.Buttons & ND_BUTTON3);
       
       // Se non è stato premuto il tasto TRIANGLE, dovrà essere restituito 0 in modo
       // che il processo di scansione proceda
       
       return TrianglePressed;  
    }

#endif

/*
   ----------------------------------------------------------------------------
   FUNZIONI GENERICHE 
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

    char INTERNAL_RecallNdTessEngine (IplImage *OcrImage, int OcrLanguage, char **PntToResultsArea)
    {
        char DecoderOption [1024];
        int  ErrRep;
        
        OcrImageThresholded = cvCreateImage ( cvSize (STD_OCR__SAMPLEIMG__DIMX, STD_OCR__SAMPLEIMG__DIMY), 8, 1); 
        
        if (OcrImageThresholded)
        {
           OcrImageEroded     = cvCreateImage ( cvSize (STD_OCR__SAMPLEIMG__DIMX, STD_OCR__SAMPLEIMG__DIMY), 8, 1); 
           
           if (OcrImageEroded)
           {
                 OcrImageResized     = cvCreateImage ( cvSize (STD_NDTESS_IMG_DIMX, STD_NDTESS_IMG_DIMY), 8, 1); 
                                        
                 if (OcrImageResized)
                 {
                       // Segnala l'inizio del processo di scansione
                       
                       baaud_ndProceedMessage (); 
                       
                       mainitf_StatusBar ("Activating ndTesseract OCR...", RENDER);
                       
                       // Il sistema implementa un filtro speciale che rimuove il rumore dall'immagine
                       
                       cvAdaptiveThreshold (OcrImage, OcrImageThresholded, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, 63, 9);    // 75, 12
                       cvErode  (OcrImageThresholded, OcrImageEroded, NULL, 1);                     
                       cvResize (OcrImageEroded, OcrImageResized, CV_INTER_AREA);
                       cvNot    (OcrImageResized, OcrImageResized);                   
                                            
                       cvNamedWindow ("TEXT", CVKEY_SETPOS (Windows_MaxScreenX/2, Windows_MaxScreenY/2) | CVKEY_SETSIZE (Windows_MaxScreenX/2-1, Windows_MaxScreenY/2-1) | CV_FORCE);
                       cvShowImage ("TEXT", OcrImageResized);     
                       cvSaveImage ("/BLINDASSISTANT/SYSTEM/ndTessImage.bmp", OcrImageResized);            
                                                 
                       cvReleaseImage (&OcrImageResized); 
                       cvReleaseImage (&OcrImageEroded);                   
                       cvReleaseImage (&OcrImageThresholded);
                       
                       // Alcune variabili per la gestione delle callback devono essere impostate qui
                       INTERNAL_IsTheFirstTimeForCB = 1;
                       INTERNAL_LastProgressMsgCB   = 0;
                       INTERNAL_ProgressWindowCB    = cvGetNdWindowHandler ("TEXT");
                       
                       // Richiama l'engine
                       
                       ndHAL_SND_Beep (1, 8);
                       ErrRep = C_CALL_ndTesseractForBA ("/BLINDASSISTANT/SYSTEM/ndTessImage.bmp", OcrLanguage, PntToResultsArea, INTERNAL_ProgressCallback);
                       ndHAL_SND_Beep (1, 8);
                                          
                       // Cancella la finestra TEXT
                       
                       cvDestroyWindow ("TEXT");
                       mainitf_StatusBar ("", RENDER);
                       
                       ndHAL_SND_Beep (2, 8);          // Bip per segnalare la fine del lavoro
        
                       // Segnala alla routine chiamante che è tutto a posto
                                            
                       return ErrRep;
                 }
                 else    // Impossibile allocare OcrImageResized
                 {
                     cvReleaseImage (&OcrImageEroded);
                     cvReleaseImage (&OcrImageThresholded);
                     return -3;
                 }
            }
            else   // Impossibile allocare OcrImageEroded
            {
                cvReleaseImage (&OcrImageThresholded);
                return -2;
            }            
        }
        else              // Impossibile allocare  OcrImageThresholded
          return -1;    
    }
    
    
    
    void baOcr1_DecodeResults (IplImage *OcrImage, int OcrLanguage, char *ResultsArea)
    {
        char MergeWord       [MAXLENOFAWORD];
        char MergeWordFixed  [MAXLENOFAWORD];
        
        struct baListOfWords_Type *PntToListOfWords, *PntToListOfWords_Fixed;
        register int ErrRep, Counter, FixResult, NrFixedWords;
        
        // Emette un bip per segnalare all'utente che si sta iniziando l'analisi
        // vocale
        
        ndHAL_SND_Beep (2, 8);
        
          
        // Inizia la decodifica con lo scanner ndTesseract
        
        mainitf_StatusBar ("Decoding OCR results...", RENDER);
            
        // Alloca lo spazio per contenere la lista di parole riconosciute
        PntToListOfWords = malloc ( sizeof (struct baListOfWords_Type) );
             
        if (PntToListOfWords)
        {   
             mainitf_StatusBar ("Loading dictionary..", RENDER);
             
             ndHAL_SND_Beep (2, 6);           // Questo bip segnala l'inizio del processo di correzione
             
             baOcrAPI_FillListOfWords (ResultsArea, PntToListOfWords);
             baOcrAPI_EnableDictionary (OcrLanguage);
             
             ndHAL_SND_Beep (2, 6);           // Questo bip segnala l'inizio del processo di correzione
             
             mainitf_StatusBar ("Fixing errata words..", RENDER);
             
             // Alloca lo spazio per contenere la lista di parole corrette dal sistema
             PntToListOfWords_Fixed = malloc ( sizeof (struct baListOfWords_Type) );
             
             if (PntToListOfWords_Fixed)
             {
                   // Provvede a reinizializzare la lista di parole corrette dal sistema 
                   memset (PntToListOfWords_Fixed, 0, sizeof (struct baListOfWords_Type));
             
                   // Adesso provvedi a correggere le varie parole, una ad una
             
                   for (Counter=0; Counter<PntToListOfWords->NrWords; Counter++)
                   {
                         baOcrAPI_CorrectASingleWord (  &(PntToListOfWords->Word       [Counter][0]), 
                                                        &(PntToListOfWords_Fixed->Word [Counter][0]), 0.2 );
                   }
                   
                   // Iniziamo il secondo passaggio: dobbiamo innanzitutto spostare tutte le parole
                   // fixate dentro il buffer delle parole non fixate in modo da eseguire il 
                   // reprocessing. Si tenga presente che il numero di parole sta dentro
                   // PntToListOfWords->NrWords
                   
                   if (PntToListOfWords->NrWords>=2)
                   {
                       // Travaso dalla lista di parole fixed a quella di base: il numero di
                       // parola non va copiato perchè tanto è uguale
                       
                       for (Counter=0; Counter<PntToListOfWords->NrWords; Counter++)
                       {
                             strcpy ( &(PntToListOfWords->Word [Counter][0]), &(PntToListOfWords_Fixed->Word [Counter][0]) );
                       }
                       
                       // Per ogni coppia di parole, genera una parola merge e verifica se trovi
                       // una parola corrispondente nel dizionario
                       
                       NrFixedWords=0;
                       
                       for (Counter=0; Counter<PntToListOfWords->NrWords-1; Counter++)
                       {
                           if (PntToListOfWords->Word [Counter][0]!=0)
                           {
                                   strcpy (MergeWord, &(PntToListOfWords->Word [Counter][0]));
                                   strcat (MergeWord, "#");
                                   strcat (MergeWord, &(PntToListOfWords->Word [Counter+1][0])); 
                                   
                                   FixResult=baOcrAPI_CorrectASingleWord (  &MergeWord, &MergeWordFixed, 0.05 );
                 
                                   if (FixResult==0)       // Non è stata effettuata alcuna correzione
                                   {
                                       // E' necessario solo riportare la prima delle parole fixate nel dizionario
                                       strcpy (&(PntToListOfWords_Fixed->Word [NrFixedWords][0]), &(PntToListOfWords->Word [Counter][0]));
                                   }
                                   else       // E' stata effettuata una correzione
                                   {
                                       // La parola merge, opportunamente corretta, viene caricata nel dizionario
                                       
                                       strcpy (&(PntToListOfWords_Fixed->Word [NrFixedWords][0]), &MergeWordFixed);
                                       
                                       // La seconda parola del dizionario viene invalidata
                                       PntToListOfWords->Word [Counter+1][0]=0;       
                                   }
                                   
                                   NrFixedWords++;
                                   
                           }
                       }
                       
                       PntToListOfWords_Fixed->NrWords = NrFixedWords;                             
                   }
                   
                   // In questo punto il secondo stadio delle elaborazioni è completato
                   
                   // FINE DELLE ELABORAZIONI DI CORREZIONE: PROVVEDIAMO A DIGITALIZZARE LE PAROLE NECESSARIE
                   
                   mainitf_StatusBar ("Playing..", RENDER);
                   
                   baOcrAPI_DigitalizeWords (PntToListOfWords_Fixed, 0, OcrLanguage);
                   
                   // LAVORO FINITO: Provvedi a deallocare i buffer creati fino a questo momento
                   
                   free (PntToListOfWords_Fixed);  // Rilascia la memoria impegnata dalla lista di parole corrette 
                   baOcrAPI_DisableDictionary ();  // Disattiva il dizionario
                   
                   free (PntToListOfWords);        // Rilascia la memoria impegnata dalla lista di parole di primo livello 
                   
                   mainitf_StatusBar ("", RENDER);
                   
                   ndHAL_SND_Beep (2, 8);          // Bip per segnalare la fine del lavoro
                   return;  
             }
             else    // Non è stato possibile allocare la lista di parole fixed
             {
                   // Provvedi anche a disattivare il dizionario
                   baOcrAPI_DisableDictionary ();
                   
                   free (PntToListOfWords);         // Rilascia la memoria impegnata dalla lista di parole di primo livello 
                   
                   ndHAL_SND_Beep (2, 8);           // Bip per segnalare la fine del lavoro
                   return;  
             } 
                  
         }
         else    // Non è stato possibile allocare la lista di parole di primo livello
         {
             ndHAL_SND_Beep (2, 8);         // Bip per segnalare la fine del lavoro
             return;    
         }     
         
    }

#endif





       
void baOcr1_DoMainOCRJob (IplImage *OcrImage, int OcrLanguage)
{
    #ifndef PSP1000_VERSION
    
        char *ResultsArea;
        
        // In questa sezione avverrà il rilascio dinamico della memoria, verrà 
        // richiamato l'engine Tesseract e verranno restituiti i risultati.
        // E' questa routine che provvede ad eseguire una malloc su ResultsArea:
        // poi noi dobbiamo solo provvedere a rilasciare il risultato dopo la
        // decodifica vocale
        
        int ErrRep = INTERNAL_RecallNdTessEngine (OcrImage, OcrLanguage, &ResultsArea);
    
        if (!ErrRep) 
        {
           baOcr1_DecodeResults (OcrImage, OcrLanguage, ResultsArea);
           free (ResultsArea);
        }
        
    #endif
        
        return; 
}
       









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
   API PER LA GESTIONE DELL'OCR 0 (TESSERACT VIA BLINDSERVER)
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
   FUNZIONI DI RECUPERO DELLA MEMORIA DIPENDENTI DALLA PIATTAFORMA
   ----------------------------------------------------------------------------
*/

void baOcr0_RecoverMemory_PlatDep (void)
{
     #ifdef PSP1000_VERSION
     
            mainitf_StatusBar ("Recovering ram memory..", RENDER);
            cvEmptyCapture (cvCameraCaptureObj);
           
     #endif
}

void baOcr0_RestoreMemoryConfig_PlatDep (void)
{
     #ifdef PSP1000_VERSION
 
            mainitf_StatusBar ("Restoring old configuration..", RENDER);
    
     #endif
}

/*
   ----------------------------------------------------------------------------
   FUNZIONI GENERICHE 
   ----------------------------------------------------------------------------
*/


void baOcr0_DoMainOCRJob (IplImage *OcrImage, int OcrLanguage)
{
    char MergeWord       [MAXLENOFAWORD];
    char MergeWordFixed  [MAXLENOFAWORD];
    
    struct baListOfWords_Type *PntToListOfWords, *PntToListOfWords_Fixed;
    register int ErrRep, Counter, FixResult, NrFixedWords;
    
    // Emette un bip per segnalare all'utente che si sta iniziando l'analisi
    // vocale
    
    ndHAL_SND_Beep (2, 8);
    
    // Alloca dinamicamente il TextBuffer: riceverà il testo OCR in arrivo dal
    // BlindServer
      
    PntToTextBuffer = malloc (TEXTBUFFERSIZE);
    memset (PntToTextBuffer, 0, TEXTBUFFERSIZE);
    
    // Passa al BlindServer un messaggio speciale che chiede di processare una
    // immagine allo scanner Tesseract. Viene anche passata una infomazione
    // speciale che indica il linguaggio richiesto
    
    mainitf_StatusBar ("Contacting Tesseract via WIFI", RENDER);
    
    ErrRep=baBldSvr_SendToSvrAnImage_ForOCR (OcrImage, PntToTextBuffer, SystemOptPacket.OcrLanguage); 
    
    if (!ErrRep)
    {    
         // Alloca lo spazio per contenere la lista di parole riconosciute
         PntToListOfWords = malloc ( sizeof (struct baListOfWords_Type) );
             
         if (PntToListOfWords!=0)
         {   
             // Esegui delle operazioni di recupero della memoria che sono dipendenti dalla piattaforma
             
             baOcr0_RecoverMemory_PlatDep ();
             
             // Inizia le operazioni di analisi caricando il dizionario
             
             mainitf_StatusBar ("Loading dictionary..", RENDER);
             
             ndHAL_SND_Beep (2, 6);           // Questo bip segnala l'inizio del processo di correzione
             
             baOcrAPI_FillListOfWords (PntToTextBuffer, PntToListOfWords);
             baOcrAPI_EnableDictionary (SystemOptPacket.OcrLanguage);
             
             ndHAL_SND_Beep (2, 6);           // Questo bip segnala l'inizio del processo di correzione
             
             mainitf_StatusBar ("Fixing errata words..", RENDER);
             
             // Alloca lo spazio per contenere la lista di parole corrette dal sistema
             PntToListOfWords_Fixed = malloc ( sizeof (struct baListOfWords_Type) );
             
             if (PntToListOfWords_Fixed!=0)
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
                   free (PntToTextBuffer);         // Rilascia la memoria del TextBuffer
                   
                   // Sulle piattaforme dove è necessario, ripristina la vecchia configurazione della memoria
                   // (per esempio riattivando la videocamera)
                   
                   baOcr0_RestoreMemoryConfig_PlatDep ();
                   
                   // Ripristina la status bar
                   
                   mainitf_StatusBar ("", RENDER);
                   
                   ndHAL_SND_Beep (2, 8);          // Bip per segnalare la fine del lavoro
                   return;  
             }
             else    // Non è stato possibile allocare la lista di parole fixed
             {
                   // Provvedi anche a disattivare il dizionario
                   baOcrAPI_DisableDictionary ();
                   
                   free (PntToListOfWords);                     // Rilascia la memoria impegnata dalla lista di parole di primo livello 
                   free (PntToTextBuffer);                      // Rilascia la memoria del TextBuffer
                   
                   baOcr0_RestoreMemoryConfig_PlatDep ();       // Ripristino della vecchia configurazione ram
                   
                   ndHAL_SND_Beep (2, 8);                       // Bip per segnalare la fine del lavoro
                   return;  
             } 
                  
         }
         else    // Non è stato possibile allocare la lista di parole di primo livello
         {
             free (PntToTextBuffer);        // Rilascia la memoria del TextBuffer
             
             ndHAL_SND_Beep (2, 8);         // Bip per segnalare la fine del lavoro
             return;    
         }     
    }
    else  // Non è stato possibile trasmettere i dati al BlindServer
    {
          free (PntToTextBuffer);        // Rilascia la memoria del TextBuffer
          
          ndHAL_SND_Beep (2, 8);         // Bip per segnalare la fine del lavoro
          return;
    }
}




































































































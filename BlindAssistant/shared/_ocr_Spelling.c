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
   ALGORITMO PER LA CORREZIONE DELLE PAROLE (OCR)
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
   SIMBOLI PER USO INTERNO 
   ----------------------------------------------------------------------------
*/

static float ISpellInvFactor;

/*
   ----------------------------------------------------------------------------
   MANIPOLAZIONE TESTO 
   ----------------------------------------------------------------------------
*/


void baOcrAPI_FillListOfWords (char *PntToTextBuffer, struct baListOfWords_Type *PntToListOfWords)
{
     int  PntToActualWord, PntToActualChar, Pointer;
     
     char SingleChar, IsAStandardChar, LastCharWasStandard;
     char YouCanExit;
     
     // Inizia ripulendo l'area che dovrà contenere la lista di parole richiesta
     
     memset (PntToListOfWords, 0, sizeof (struct baListOfWords_Type));
     
     PntToActualWord=0;
     PntToActualChar=0;
     
     Pointer             = 0;
     YouCanExit          = 0;
     LastCharWasStandard = 0;
     
     while (!YouCanExit)
     {
          // Preleva un singolo carattere dal TextBuffer 
          SingleChar = *(PntToTextBuffer+Pointer); 
          
          IsAStandardChar=0;
          
          if ((SingleChar>='A') && (SingleChar<='Z')) IsAStandardChar=1;
          if ((SingleChar>='a') && (SingleChar<='z')) IsAStandardChar=1;
          if ((SingleChar>='0') && (SingleChar<='9')) IsAStandardChar=1;
          if (SingleChar==0x60) IsAStandardChar=1;      // L'apostrofo ' è considerato un carattere standard
          if (SingleChar=='_') IsAStandardChar=1;     // L'underscore è un carattere standard
          
          if (IsAStandardChar)
          {
              if ((PntToActualWord<MAXWORDS-1) && (PntToActualChar<MAXLENOFAWORD-1))
              {
                 PntToListOfWords->Word [PntToActualWord][PntToActualChar] = SingleChar;  
                 LastCharWasStandard = 1;
                 
                 PntToActualChar++;              
              }
          }
          else   // Non è un carattere standard: in questo caso è necessario decidere se
          {      // passare semplicemente alla parola successiva o interrompere il ciclo
                 
              if (SingleChar != 0x00)
              {
                      if (LastCharWasStandard)
                      {
                          LastCharWasStandard = 0;  
                          
                          PntToActualWord++;                 // Passa alla parola successiva
                          PntToActualChar = 0;                    
                          
                          if (PntToActualWord==MAXWORDS)     // Siamo al limite della lista di parole
                          {
                              YouCanExit=1;                  // Provvederà ad uscire dal loop
                              continue;                          
                          }
                      }       
              }
              else    // Un carattere pari a 0 ci dice che il TextBuffer ormai è stato tutto processato
              {
                      YouCanExit=1;                  // Provvederà ad uscire dal loop
                      continue; 
              }
          }
          
          Pointer++;                                 // Avanza con il puntatore    
          
          if (Pointer==TEXTBUFFERSIZE)               // Siamo al limite del TextBuffer
          {
              YouCanExit=1;                          // Provvederà ad uscire dal loop                                    
              continue;             
          }
     }           // Fine del ciclo while

     PntToListOfWords->NrWords = PntToActualWord;
     return;
}

/*
   -----------------------------------------------------------------------------
   FUNZIONI ENABLE/DISABLE DEL DIZIONARIO
   -----------------------------------------------------------------------------
*/


void baOcrAPI_EnableDictionary (int LanguageCode)
{
    char DictionaryName  [256];
    char TemporaryBuffer [256];
    FILE *DictFile;
    
    char YouCanExit;
    int  Result; 
    
    register int StrLen, Counter;
    
    // Determina il nome del dizionario
    
    baOcrFNames_GenerateNameOfDictionary (&DictionaryName, LanguageCode);
    
    // Ok, adesso verifica se è presente e portalo nella memoria
    
    if (fexist (DictionaryName))
    {
         // La prima cosa da fare è determinare quante parole fanno parte del
         // dizionario e qual è la massima lunghezza dello stesso
         
         DictFile = fopen (DictionaryName, "r");
         
         DictNrWords = 0;
         DictMaxLen  = 0;
         
         YouCanExit=0;
         
         while (!YouCanExit)
         {
              Result = ba_fgets (TemporaryBuffer, 255, DictFile);
              
              if (Result!=0)
              {
                  StrLen = strlen (TemporaryBuffer);
                  
                  if (StrLen<23)     // La parole con più di 23 caratteri vanno semplicemente ignorate
                  {
                     if (StrLen>DictMaxLen) DictMaxLen=StrLen;
                     DictNrWords++;          
                  }
              } 
              else
              {
                  break;
              }
         }  
         
         // Ok, adesso provvedi ad allocare i buffer di memoria necessarii
         
         DictionaryArea = malloc (DictNrWords*DictMaxLen);
         memset (DictionaryArea, 0, DictNrWords*DictMaxLen);
         
         DictionaryLenDB = malloc (DictNrWords);
         memset (DictionaryLenDB, 0, DictNrWords);
         
         DictionaryDistDB = memalign (4, DictNrWords*sizeof(float));
         memset (DictionaryDistDB, 0, DictNrWords*sizeof(float));
         
         // Bene, iniziamo a caricare le parole nel dizionario: per ogni parola,
         // viene precalcolata la lunghezza e memorizzata dentro DictionaryLenDB
         
         fseek (DictFile, 0, SEEK_SET);
         
         // Secondo ciclo while
         
         YouCanExit=0;
         Counter   =0;
         
         while (!YouCanExit)
         {
              Result = ba_fgets (TemporaryBuffer, 255, DictFile);
              
              if (Result!=0)
              {
                  StrLen = strlen (TemporaryBuffer);
                  
                  if (StrLen<23)     // La parole con più di 23 caratteri vanno semplicemente ignorate
                  {
                     strcpy ( DictionaryArea+DictMaxLen*Counter, TemporaryBuffer );
                     *(unsigned char *)(DictionaryLenDB + Counter) = StrLen;
                     
                     Counter++;
                     continue;          
                  }
              } 
              else
              {
                  break;
              }
         } 
         
         fclose (DictFile);
         DictIsOperative=1;  
    }
    else   // Il dizionario non è presente
    {
         DictIsOperative=0;  
    }
}

void baOcrAPI_DisableDictionary (void)
{
    free (DictionaryDistDB);
    free (DictionaryLenDB);
    free (DictionaryArea);
    
    DictIsOperative=0; 
}



/*
   -----------------------------------------------------------------------------
   ALGORITMO DI CORREZIONE DELLE PAROLE
   -----------------------------------------------------------------------------
*/

static inline float INTERNAL_CalculateDistanceChar (unsigned char LetterA, unsigned char LetterB)
{
    register unsigned char _LetterA;
    register unsigned char _LetterB;
    
    register unsigned char _DwLetterA;
    register unsigned char _DwLetterB;
    
    {                                                       
            _LetterA = LetterA;
            _LetterB = LetterB;
            
            if ((LetterA>='A') && (LetterA<='Z'))            // E' una lettera maiuscola
               _DwLetterA = _LetterA + 32;
            else
               _DwLetterA = _LetterA;
               
            if ((LetterB>='A') && (LetterB<='Z'))            // E' una lettera maiuscola
               _DwLetterB = _LetterB + 32;
            else
               _DwLetterB = _LetterB;
                 
            if (_DwLetterA==_DwLetterB) return 0.0;          // Le lettere sono uguali: la distanza è zero
            
            // Inizia a fare analisi basate sulle similitudini tra i caratteri
            
            if ((_LetterA=='e') && (_LetterB=='c')) return 0.5;
            if ((_LetterA=='c') && (_LetterB=='e')) return 0.5;
            
            if ((_LetterA=='I') && (_LetterB=='l')) return 0.5;
            if ((_LetterA=='l') && (_LetterB=='I')) return 0.5;
            
            if ((_LetterA=='m') && (_LetterB=='n')) return 0.3;
            if ((_LetterA=='n') && (_LetterB=='m')) return 0.3;
            
            if ((_LetterA=='r') && (_LetterB=='n')) return 0.5;
            if ((_LetterA=='n') && (_LetterB=='r')) return 0.5;
            
            if ((_LetterA=='t') && (_LetterB=='x')) return 0.7;
            if ((_LetterA=='x') && (_LetterB=='t')) return 0.7;
            
            if ((_LetterA=='1') && (_LetterB=='l')) return 0.2;
            if ((_LetterA=='0') && (_LetterB=='o')) return 0.2;
            
            if ((_LetterA=='y') && (_LetterB=='v')) return 0.5;
            if ((_LetterA=='v') && (_LetterB=='y')) return 0.5;
            
            if ((_LetterA=='o') && (_LetterB=='q')) return 0.5;
            if ((_LetterA=='q') && (_LetterB=='o')) return 0.5;
            
            if ((_LetterA=='o') && (_LetterB=='c')) return 0.5;
            if ((_LetterA=='c') && (_LetterB=='o')) return 0.5;
            
            if ((_LetterA=='A') && (_LetterB=='H')) return 0.5;
            if ((_LetterA=='H') && (_LetterB=='A')) return 0.5;
            
            if ((_LetterA=='o') && (_LetterB=='0')) return 0.1;
            if ((_LetterA=='0') && (_LetterB=='o')) return 0.1;
            
            if ((_LetterA=='O') && (_LetterB=='0')) return 0.1;
            if ((_LetterA=='0') && (_LetterB=='O')) return 0.1;
            
            if (LetterA=='#') return 0.0;    // Elemento speciale per eseguire comparazioni di secondo livello
            if (LetterA=='_') return 0.0;    // Elemento speciale per supporto OCRAD
            
    }        

    // Le lettere sono completamente diverse
    return 1.0;
}


static inline float INTERNAL_CalculateDistanceWord (char *OriginalWord, int NrWordInDict, int LenOrigWord, float DiscrThreshold)
{
    register char  *DictWord = DictionaryArea+DictMaxLen*NrWordInDict;
    register char  LetterA;
    register char  LetterB;
    register int   Counter;
    register float SumDistance, Distance, ReturnedDst;
    register float DivergenceFactor;
    
    // Analizziamo la prima lettera: per la prima lettera c'è minore tolleranza. Se
    // le due lettere sono diverse, la distanza tra le parole si impone direttamente
    // pari ad 1
    
    LetterA = *(unsigned char *)(OriginalWord);
    LetterB = *(unsigned char *)(DictWord);
        
    Distance = INTERNAL_CalculateDistanceChar (LetterA, LetterB);
        
    if (Distance!=0.0)         // Non la stessa iniziale: le parole sono di sicuro diverse                
    {
       *(DictionaryDistDB + NrWordInDict) = 1.0;
       return 1.0;            
    }
    
    // Contiamo il numero di lettere totalmente differenti
    // e la distanza tra i caratteri
    
    DivergenceFactor=0;
    SumDistance=0;
    
    for (Counter=1; Counter<LenOrigWord; Counter++)
    {
        // Preleva due lettere della parola originale e della parola del dizionario
        
        LetterA = *(unsigned char *)(OriginalWord+Counter);
        LetterB = *(unsigned char *)(DictWord+Counter);
        
        Distance = INTERNAL_CalculateDistanceChar (LetterA, LetterB);
        
        SumDistance+=Distance;      
      
        if (Distance==1.0) 
        {
               DivergenceFactor += ISpellInvFactor;
               
               if (DivergenceFactor>DiscrThreshold)     // Le parole sono troppo differenti
               {
                    *(DictionaryDistDB + NrWordInDict) = 1.0;
                    return 1.0;                                  
               }
        }
    }
                                   
    // Il calcolo di distanza viene fatto solo quando il
    // numero di lettere totalmente differenti è minore della
    // percentuale prevista da DiscrThreshold
    // La riga successiva equivale ad eseguire un SumDistance
    // diviso per il numero di caratteri.
            
    // Questo valore deve essere registrato nel database delle distanze, ed 
    // anche restituito alla routine chiamante tramite lo stack
    
    *(DictionaryDistDB + NrWordInDict) = SumDistance * ISpellInvFactor;
    return ReturnedDst;             
}


             


char baOcrAPI_CorrectASingleWord (char *OriginalWord, char *DestWord, float DiscrThreshold)
{
    register int   LenOrigWord, Counter, MinimumDistElem;
    register float MinimumDist, DistanceOfAWord; 
    
    float          InvFactor,   ReturnedDist;
    
    
    LenOrigWord     = strlen (OriginalWord);
    ISpellInvFactor = 1.0/LenOrigWord;
    
    if ( (DictIsOperative) && (LenOrigWord>0) && (LenOrigWord<23) )
    {
         // Analizza tutte le singole parole del dizionario
         
         for (Counter=0; Counter<DictNrWords; Counter++)
         {
             if ( LenOrigWord == *(unsigned char *)(DictionaryLenDB + Counter) )
             {
                  // Il processo di confronto viene eseguito esclusivamente
                  // per le parole che hanno il medesimo numero di caratteri
                  
                  // La riga seguente memorizza in un float la distanza tra la
                  // parola originale e la parola di destinazione (non è 
                  // necessaria moltiplicazione per 4 perchè DictionaryDistDB
                  // è un puntatore di tipo float)
                  
                  ReturnedDist=INTERNAL_CalculateDistanceWord (OriginalWord, Counter, LenOrigWord, DiscrThreshold);
             
                  if (ReturnedDist==0.0)                      // Se viene trovata una distanza pari a 0, allora la
                  {                                           // la parola di sicuro è corretta
                      strcpy (DestWord, OriginalWord);        // Non sono necessarie altre elaborazioni
                      return 0;                               // Nessuna correzione
                  }
             }
         }
         
         // Quando il sistema giunge qui, vuol dire che non è stato possibile
         // trovare un elemento identico nel dizionario. La parola è certamente
         // sbagliata, è necessario trovare l'elemento di minima distanza che
         // sarà considerato come l'elemento corretto (fix)
         
         MinimumDist     = 1.0;
         MinimumDistElem =  -1;
         
         for (Counter=0; Counter<DictNrWords; Counter++)
         {
             if ( LenOrigWord == *(unsigned char *)(DictionaryLenDB + Counter) )
             {
                  // Il processo di confronto viene eseguito esclusivamente
                  // per le parole che hanno il medesimo numero di caratteri
                  
                  DistanceOfAWord = *(DictionaryDistDB + Counter);
                  
                  if ( DistanceOfAWord<MinimumDist )
                  {
                       // E' stato trovato un elemento che ha una distanza dalla parola
                       // errata inferiore rispetto alle parole incontrate in precedenza
                       
                       MinimumDistElem = Counter;
                       MinimumDist     = DistanceOfAWord;
                       continue;
                  }
             }
         }
         
         // Quando il sistema arriva qui, vuol dire che ha già eseguito la scansione
         // di tutti i vocaboli simili nel dizionario. Ci sono due possibilità:
               
         if (MinimumDistElem==-1)      // Nel sistema non ci sono vocaboli simili
         {
             strcpy (DestWord, OriginalWord);         // (nessuna correzione) 
             return 0;                         
         }
         else                          // Copia il vocabolo che appare più simile
         {
             strcpy (DestWord, DictionaryArea + DictMaxLen*MinimumDistElem );
             return 1;                                // correzione effettuata
         }
    }
    else   
    {      
         // Il dizionario non è operativo, oppure la parola originale è più
         // lunga di 23 caratteri. In questo caso, si deve semplicemente  
         // copiare la parola originale nel buffer della parola vecchia
         // (nessuna correzione)
         
         strcpy (DestWord, OriginalWord);
         return 0;                         // nessuna correzione
    }  
}







/*
   ----------------------------------------------------------------------------
   AGGIUNTA DI NUOVE PAROLE AL DIZIONARIO
   ----------------------------------------------------------------------------
*/

void baOcrAPI_AddNewWordToDwg (char *Word, int LanguageCode)
{
   char NameDictionary [255];
   FILE *DictHandle;
        
   // Aggiunge la parola in fondo al dizionario
   baOcrFNames_GenerateNameOfDictionary (&NameDictionary, LanguageCode);
   
   DictHandle = fopen (NameDictionary, "a");
   
   if (DictHandle!=0)
   {
       fprintf (DictHandle, "%s\n", Word);
       fclose (DictHandle);              
   }    
}

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
   API GENERICA PER LA GESTIONE DEGLI OCR
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
   RIFERIMENTI AD ALTRE FUNZIONI OCR SPECIFICHE 
   ----------------------------------------------------------------------------
*/

void baOcr0_DoMainOCRJob (IplImage *OcrImage, int OcrLanguage);
void baOcr1_DoMainOCRJob (IplImage *OcrImage, int OcrLanguage);

/*
   ----------------------------------------------------------------------------
   VISUALIZZAZIONE DEI RISULTATI 
   ----------------------------------------------------------------------------
*/


void baOcrAPI_DigitalizeWords (struct baListOfWords_Type *PntToListOfWords, char GenerateNotFoundWords, int LanguageCode)
{
    // Destinato ad accogliere i dati sulla pressione del pad
    struct ndDataControllerType ndPadRecord;
    
    char NameFile [255];
    char ShownWord [20];
    char WndHandle, Counter;
    int  PosX, PosY;
    int  ErrRep;
    
    if (PntToListOfWords->NrWords>0)
    {
        WndHandle = ndLP_CreateWindow (Windows_MaxScreenX/2, 50+Windows_MaxScreenY/2, Windows_MaxScreenX-1, Windows_MaxScreenY-1, 
                                       "I have read this for you", COLOR_WHITE, COLOR_BLUE, COLOR_GRAY13, COLOR_GRAY13, NO_RESIZEABLE);
        
        if (WndHandle>0)
        {
            for (Counter=0; Counter<PntToListOfWords->NrWords; Counter++)
            {
                ndLP_MaximizeWindow (WndHandle);
                
                ndWS_DrawRectangle (0, 0, 200, 50, COLOR_GRAY13, COLOR_GRAY13, WndHandle, NORENDER);
                
                ndStrCpy (ShownWord, &(PntToListOfWords->Word [Counter][0]), 15, 1);
                ndWS_GrWriteLn ( (240-8*(strlen(ShownWord)))/2, 21, ShownWord, COLOR_GRAY06, COLOR_GRAY13, WndHandle, NDKEY_SETFONT (3) | RENDER);  
                
                //if (SystemOptPacket.OcrRTGenerateWords)
                
                // NOTA: A partire dalla versione CFW0006, le parole sono sempre riprodotte sempre in real-time,
                // indipendentemente dal valore di OcrRTGenerateWords che è mantenuta solo per retrocompatibilità
                
                baaud_ReproduceASingleWordRT ( &(PntToListOfWords->Word [Counter][0]) );                            
                
                // Il cieco può, in qualsiasi momento, interrompere la riproduzione premendo il tasto triangolo
                ndHAL_GetPad (&ndPadRecord);
                if (ndPadRecord.Buttons & ND_BUTTON3) break;
            }
            
            ndLP_DestroyWindow (WndHandle);  
        }
    }               
}


/*
   ----------------------------------------------------------------------------
   FUNZIONI GENERICHE 
   ----------------------------------------------------------------------------
*/

int baOcrAPI_IsNeededNetworkConnection (int OcrMethod)
{
    switch (OcrMethod)
    {
           case OCRMETHOD_TESSERACT_VIA_BLINDSERVER:
           {
                return 1;
           }
           
           case OCRMETHOD_NDTESSERACT:
           {
                return 0;
           }
    }
}

int baOcrAPI_IsNeededBlindServerConnection (int OcrMethod)
{
    switch (OcrMethod)
    {
           case OCRMETHOD_TESSERACT_VIA_BLINDSERVER:
           {
                return 1;
           }
           
           case OCRMETHOD_NDTESSERACT:
           {
                return 0;
           }
    }
}

void baOcrAPI_DoMainOCRJob (IplImage *OcrImage, int OcrMethod, int OcrLanguage)
{
    switch (OcrMethod)
    {
           case OCRMETHOD_TESSERACT_VIA_BLINDSERVER:
           {
                baOcr0_DoMainOCRJob (OcrImage, OcrLanguage);
                break;
           }
           
           case OCRMETHOD_NDTESSERACT:
           {
                baOcr1_DoMainOCRJob (OcrImage, OcrLanguage);
                break;
           }
    } 
}




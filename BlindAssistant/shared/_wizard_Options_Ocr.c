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
   WIZARD PER IL SISTEMA DI OPZIONI DEL GESTORE OCR
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
   VARIABILI PER USO INTERNO
   ----------------------------------------------------------------------------
*/ 

int wzocroptWindow;
int wzocroptBtnClosed;
int wzocroptBtnDefault; 
int wzocroptBtnDwLoadDict;
int wzocroptBtnAddWordToDwg;


int wzocropt_TboxAddNewWordToDwg;
int wzocropt_SkipUnknownWords;

int wzengocr_LBoxOcrMethodHandle;
int wzengocr_LBoxOcrLanguageHandle;

static int wzocroptDialogBoxIsOpen = 0;
static int wzocroptYouCanExit;

static struct ndListBoxColors_Type wzocropt_MyLBoxColor =
{
COLOR_BLACK, // ColorBorder
COLOR_WHITE, // ColorBackground
COLOR_GRAY13, // ColorBorderSlot
COLOR_WHITE, // ColorSlot
COLOR_BLUE, // ColorSlotActive
COLOR_WHITE, // ColorSlotInh
COLOR_BLACK, // ColorTextSlot
COLOR_WHITE, // ColorTextSlotActive
COLOR_GRAY28 // ColorTextSlotInh
};

/*
   ----------------------------------------------------------------------------
   CODICE: CALLBACK DI GESTIONE
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzocropt_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzocroptYouCanExit=1;
   return 0;    
}

static ndint64 cb_wzocropt_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{   
   wzocroptYouCanExit=1;
   return 0;   
}

static ndint64 cb_wzocropt_DwLoadDict (char *StringID, ndint64 InfoField, char WndHandle)
{   
   // Richiama l'apposita funzione che deve scaricare lo zip e decomprimere i dati
   baDwDict_DownloadDictionaryZip ();  
}

static ndint64 cb_wzocropt_AddWordToDwg (char *StringID, ndint64 InfoField, char WndHandle)
{   
   // Microbarra di avanzamento per segnalare che l'elaborazione è in corso
   ndWS_DrawRectangle (5, 163, 150, 165, COLOR_LGREEN, COLOR_LGREEN, WndHandle, RENDER); 
   
   baOcrAPI_AddNewWordToDwg (ndTBOX_GetRowAddr (0, wzocropt_TboxAddNewWordToDwg, WndHandle), SystemOptPacket.OcrLanguage);   

   // Tutta la barra attivata perchè il lavoro è finito
   ndWS_DrawRectangle (5, 163, 295, 165, COLOR_LGREEN, COLOR_LGREEN, WndHandle, RENDER);
   
   // Cancella la barra
   ndWS_DrawRectangle (5, 163, 295, 165, COLOR_GRAY09, COLOR_GRAY09, WndHandle, RENDER); 
}

static void cblbox_wzocropt_OcrMethod (int NrItem, ndint64 LBoxCode, int ListBoxHandle, char WndHandle)
{
   baOpt_ChangeOcrMethod (LBoxCode);    
}

static void cblbox_wzocropt_OcrLanguage (int NrItem, ndint64 LBoxCode, int ListBoxHandle, char WndHandle)
{
   baOpt_ChangeOcrLanguage (LBoxCode);     
}

static void cbcbox_wzocopt_SkipUnknownWords (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
   baOpt_ChangeSkipUnknownWords (ActualValue);    
}


/*
   ----------------------------------------------------------------------------
   CODICE: INIBIZIONE DEGLI ELEMENTI A SECONDA DELLA PIATTAFORMA
   ----------------------------------------------------------------------------
*/ 

static void INTERNAL_wzocropt_InhibitElementsForPlatform (void)
{
     #ifdef PSP1000_VERSION

            ndLBOX_InhibitListBoxItem (1, wzengocr_LBoxOcrMethodHandle, wzocroptWindow, NORENDER);

     #endif
}

/*
   ----------------------------------------------------------------------------
   CODICE: DISEGNO DELL'INTERFACCIA
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_wzocropt_DrawInterface (void)
{
    struct  ndNetworkObject_Type ProbeNetObj;
    
    char    Message0 [32];
    char    Message1 [32];
    
    ndint64 Options;
    int     Counter;
    int     ErrRep;
    
    wzocroptWindow = ndLP_CreateWindow (30, 10, 450, Windows_MaxScreenY-10, "Options for the Optical Char Recognizer", 
                                       COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY09, COLOR_GRAY09, 0);
                                       
    if (wzocroptWindow>=0)
    {
        ndLP_MaximizeWindow (wzocroptWindow);
        
        ndWS_GrWriteLn (5,  5, "Choose an OCR method", COLOR_BLACK, COLOR_GRAY09, wzocroptWindow, NORENDER);
        
        // Crea la prima list box
        
        wzengocr_LBoxOcrMethodHandle = ndLBOX_CreateListBox (5, 15, 295, 55, 0, &wzocropt_MyLBoxColor, wzocroptWindow, NORENDER);
        
        ndLBOX_AddItemToListBox ("Tesseract via BlindServer", &cblbox_wzocropt_OcrMethod, OCRMETHOD_TESSERACT_VIA_BLINDSERVER, 
                                                      0, wzengocr_LBoxOcrMethodHandle, wzocroptWindow);
        
        ndLBOX_AddItemToListBox ("ndTesseract PSP (internal)", &cblbox_wzocropt_OcrMethod, OCRMETHOD_NDTESSERACT, 
                                                      0, wzengocr_LBoxOcrMethodHandle, wzocroptWindow);
        
        // Evidenzia l'elemento della listbox che corrisponde al metodo OCR attuale
        
        ndLBOX_EvidenceListBoxItem (SystemOptPacket.OcrMethod, wzengocr_LBoxOcrMethodHandle, wzocroptWindow, NORENDER);
        
        // Disegna la seconda list box
        
        ndWS_GrWriteLn (5,  65, "Choose the language", COLOR_BLACK, COLOR_GRAY09, wzocroptWindow, NORENDER);
        
        wzengocr_LBoxOcrLanguageHandle = ndLBOX_CreateListBox (5, 75, 295, 115, 0, &wzocropt_MyLBoxColor, wzocroptWindow, NORENDER);
        
        ndLBOX_AddItemToListBox ("English", &cblbox_wzocropt_OcrLanguage, OCRLANG_ENGLISH, 
                                                      0, wzengocr_LBoxOcrLanguageHandle, wzocroptWindow);
        
        ndLBOX_AddItemToListBox ("Italian", &cblbox_wzocropt_OcrLanguage, OCRLANG_ITALIAN, 
                                                      0, wzengocr_LBoxOcrLanguageHandle, wzocroptWindow);
        
        ndLBOX_AddItemToListBox ("Francois", &cblbox_wzocropt_OcrLanguage, OCRLANG_FRANCOIS, 
                                                      0, wzengocr_LBoxOcrLanguageHandle, wzocroptWindow);
        
        ndLBOX_AddItemToListBox ("German",   &cblbox_wzocropt_OcrLanguage, OCRLANG_GERMAN, 
                                                      0, wzengocr_LBoxOcrLanguageHandle, wzocroptWindow);
        
        ndLBOX_AddItemToListBox ("Spanish",   &cblbox_wzocropt_OcrLanguage, OCRLANG_SPANISH, 
                                                      0, wzengocr_LBoxOcrLanguageHandle, wzocroptWindow);
        
        ndLBOX_AddItemToListBox ("Dutch",     &cblbox_wzocropt_OcrLanguage, OCRLANG_DUTCH, 
                                                      0, wzengocr_LBoxOcrLanguageHandle, wzocroptWindow);
        
        
        // Evidenzia l'elemento della listbox che corrisponde alla lingua attuale scelta per l'OCR
        
        ndLBOX_EvidenceListBoxItem (SystemOptPacket.OcrLanguage, wzengocr_LBoxOcrLanguageHandle, wzocroptWindow, NORENDER);
        
                                                            
        // Crea il pulsante di scaricamento del dizionario
        
        wzocroptBtnDwLoadDict = ndCTRL_CreateButton (305, 65, 390, 85,
                                                wzocroptWindow, "wzocroptDwLoadDict", "Download", "dictionary", 
                                                COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                                &cb_wzocropt_DwLoadDict, 0, NORENDER);
                                          
        // Crea la textbox ed il pulsante per aggiungere una parola
        
        ndWS_GrWriteLn (5, 125, "You can add a word in the dictionary", COLOR_BLACK, COLOR_GRAY09, wzocroptWindow, NORENDER);
        ndWS_GrWriteLn (5, 135, "so that software can recognize it", COLOR_BLACK, COLOR_GRAY09, wzocroptWindow, NORENDER);
        
        wzocropt_TboxAddNewWordToDwg  = ndTBOX_CreateTextArea    (5, 145, 295, 160, "AddNewWordToDwg", 
                                                NDKEY_SETTEXTAREA (32, 1), 0, 
                                                COLOR_BLACK,  COLOR_WHITE,
                                                COLOR_BLACK,  COLOR_GRAY, 
                                                COLOR_BLACK,  COLOR_GRAY02,
                                                0, 0, 0, 0, 
                                                wzocroptWindow, NORENDER);
        
        wzocroptBtnAddWordToDwg   = ndCTRL_CreateButton (305, 140, 390, 160,
                                                wzocroptWindow, "wzocroptAdd", "Add", "", 
                                                COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                ND_BUTTON_ROUNDED, 
                                                &cb_wzocropt_AddWordToDwg, 0, NORENDER);
        
        wzocropt_SkipUnknownWords = ndCBOX_CreateCBox (5, 185, "wzocrSkipUnknownWords", SystemOptPacket.OcrSkipUnknownWords, 
                                       0, &(SystemOptPacket.OcrSkipUnknownWords), COLOR_WHITE, 
                                       cbcbox_wzocopt_SkipUnknownWords, wzocroptWindow, NORENDER);
        
        ndWS_GrWriteLn (25, 187, "Skip unknown words", COLOR_BLACK, COLOR_GRAY09, wzocroptWindow, NORENDER);
        
        // Crea il pulsante di chiusura finestra
         
        wzocroptBtnClosed   = ndCTRL_CreateButton (305, WindowData [wzocroptWindow].WSWndHeight - 25, 
                                                   390, WindowData [wzocroptWindow].WSWndHeight - 5,
                                                   wzocroptWindow, "wzocroptClose", "Close", "", 
                                                   COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                   ND_BUTTON_ROUNDED, 
                                                   &cb_wzocropt_Abort, 0, NORENDER);
         
       
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzocropt_CloseWnd, 0, 0, wzocroptWindow);
        
        // Inibisci gli elementi dell'interfaccia che non sono disponibili su questa piattaforma
        
        INTERNAL_wzocropt_InhibitElementsForPlatform ();
        
        // Richiama il renderer
        
        XWindowRender (wzocroptWindow);                                     
        return 0;
    }
    else
        return -1;
}




/*
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/ 
 
 
void wzOcrOptions (void)
{
    if (!wzocroptDialogBoxIsOpen)
    {
       // Impedisci l'avvio di due istanze della stessa dialog-box                         
       wzocroptDialogBoxIsOpen=1;     
       
       INTERNAL_wzocropt_DrawInterface ();
     
       wzocroptYouCanExit=0;
       ndProvideMeTheMouse_Until (&wzocroptYouCanExit, ND_IS_EQUAL, 1, 0, 0);
       
       // Ok... adesso provvedi a distruggere la vecchia dialog box di interfaccia e
       // Ripristina la possibilità di gestire la dialog-box
       
       ndLP_DestroyWindow (wzocroptWindow);
       wzocroptDialogBoxIsOpen=0;
    }
}

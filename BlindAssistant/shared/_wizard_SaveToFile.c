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
   WIZARD PER IL SALVATAGGIO DEI FILE IN UN CONTAINER
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

#ifndef PSP1000_VERSION

int wzxsaveWindow;

int wzxsaveBtnChoose;
int wzxsaveBtnSave;
int wzxsaveBtnAbort;
int wzxsaveBtnOk;

static int wzxsaveDialogBoxIsOpen = 0;
static int wzxsaveYouCanExit;

int wzxsaveListBox;
int ChoosenSlot;

static char NameFile [255];
static char DBAFileName [512];

static char Path [512];
static char Name [512];
static char Ext [16];

static char DBAPart1 [512];
static char DBAPart2 [512];

#endif

/*
   ----------------------------------------------------------------------------
   CODICE: CALLBACK DEL WIZARD DI SALVATAGGIO
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION
 
static void XSaveGraphCallback (int NrSlot, int NrSample, int SaveCode, struct OpInfoPacket_Type *InfoPacket)
{ 
    switch (SaveCode)
    {
           case BEGIN_TO_XSAVE:
           {  
              InfoPacket->WndHandle = ndLP_CreateWindow (70, 77, Windows_MaxScreenX-70, Windows_MaxScreenY-77,
                                                         "Saving DBA file", COLOR_WHITE, COLOR_BLUE,
                                                         COLOR_GRAY, COLOR_BLACK, 0);
              
              ndLP_MaximizeWindow (InfoPacket->WndHandle);
              
              ndWS_GrWriteLn (10, 10, "Waiting.... I'm saving the data  ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              ndWS_GrWriteLn (10, 18, "in a DBA file                    ",   COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              XWindowRender (InfoPacket->WndHandle);
         
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 0, 0, 100, 
                                        COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              ndDelay (0.5);      
              break; 
           
           }
    
           case XSAVING_NAME:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Saving the name of the person", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 10, 0, 100, 
                                        COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case XSAVING_IMG_SAMPLE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Saving image sample...", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 20+70*(float)(NrSample)/(float)(NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break; 
           }
           
           case XSAVED_IMG_SAMPLE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Success", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 20+70*(float)(NrSample+1)/(float)(NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;   
           }
           
           case ERROR_IN_XSAVING_IMG_SAMPLE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Failure", COLOR_RED, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 20+70*(float)(NrSample+1)/(float)(NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              
              break;    
           }
           
           case DONE0:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Operation complete", COLOR_RED, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 100, 0, 100, 
                                        COLOR_WHITE, COLOR_YELLOW, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
                        
              ndDelay (0.5);
              ndLP_DestroyWindow (InfoPacket->WndHandle);
             
              break;
           }          
    }      
}

#endif

 
/*
   ----------------------------------------------------------------------------
   CODICE: INTERFACCIA DI SCELTA
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wzxsave_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzxsaveYouCanExit=1;
   return 0;   
}

static ndint64 cb_wzxsave_Ok (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzxsaveYouCanExit=1;
   return 0;   
}

static ndint64 cb_wzxsave_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzxsaveYouCanExit=1;
   return 0;    
}

static void lbcb_wzxsave_SelectSlot (int NrItem, ndint64 LBoxCode, int ListBoxHandle, char WndHandle)
{
   ChoosenSlot = NrItem;
   ndCTRL_DeInhibitButton (wzxsaveBtnChoose, wzxsaveWindow, RENDER);     
}

static ndint64 cb_wzxsave_SaveTheData (char *StringID, ndint64 InfoField, char WndHandle)
{
   struct OpInfoPacket_Type XSaveInfoPacket;
   int    ErrRep;
   
   ErrRep=baXSave_SaveTheDataInTheSlot (ChoosenSlot, &DBAFileName, &XSaveGraphCallback, &XSaveInfoPacket);
   
   if (!ErrRep)        // Il caricamento è andato a buon fine: cambia il pulsante
   {
       ndCTRL_ChangeButtonParameters (wzxsaveBtnAbort, wzxsaveWindow,
                                      "wzxsaveAbort", "Done", "", 
                                      COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                      ND_BUTTON_ROUNDED, RENDER);         
   }
   
   return  0;  
}

static ndint64 cb_wzxsave_ChooseTheName (char *StringID, ndint64 InfoField, char WndHandle)
{
    int Result, Len;
    
    Result=FileManager ("Choose the name for DBA file (save)", 0, 1, &DBAFileName);  
    
    if (Result==0)     // L'utente ha scelto un file
    {
         // Sostituisci l'estensione con l'estensione DBA
        
        fssSplitNameFile (&DBAFileName, &Path, &Name, &Ext);
        
        strcpy (&DBAFileName, &Path);
        strcat (&DBAFileName, &Name);
        strcat (&DBAFileName, ".BDA");
        
        // Visualizza i dati nella name area
        
        ndWS_DrawRectangle (10, 140, 300, 160, COLOR_GRAY12, COLOR_BLACK, wzxsaveWindow, NORENDER);
       
        ndStrCpyBounded ( &DBAPart1, &DBAFileName, 0, 32, 0);
        ndWS_GrWriteLn  ( 15, 142, &DBAPart1, COLOR_BLACK, COLOR_GRAY12, wzxsaveWindow, NORENDER);
        
        Len = strlen (DBAFileName);
        
        if (Len>32)
        {
            ndStrCpyBounded ( &DBAPart2, &DBAFileName, 33, Len, 0);
            ndWS_GrWriteLn  ( 15, 150, &DBAPart2, COLOR_BLACK, COLOR_GRAY12, wzxsaveWindow, NORENDER);       
        }               
        
        // Deinibisce il pulsante che permette il salvataggio
        
        ndCTRL_DeInhibitButton (wzxsaveBtnSave, wzxsaveWindow, RENDER);     
    } 
    
    return 0;
}

void INTERNAL_wzxsave_DrawInterface (void)
{ 
    char Message0 [16];
    char Message1 [16];
    
    int Counter;
    
    wzxsaveWindow = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Save data in .DBA file", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wzxsaveWindow>=0)
    {
        ndLP_MaximizeWindow (wzxsaveWindow);
        
        ndWS_GrWriteLn (5, 10, "This option allows to store in a file ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
        ndWS_GrWriteLn (5, 18, "with extension DBA the infos relative ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
        ndWS_GrWriteLn (5, 26, "to a person that are already memorized", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
        ndWS_GrWriteLn (5, 34, "in a slot of this program.            ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
    
        ndWS_GrWriteLn (5, 50, "Choose the slot from which the data", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
        ndWS_GrWriteLn (5, 58, "will be recovered and stored.      ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
    
        // Crea una list box
        
        wzxsaveListBox = ndLBOX_CreateListBox (10, 70, 300, 130, 0, 0, wzxsaveWindow, NORENDER);
        
        for (Counter=0; Counter<NR_SLOTS_FACE_RECOGNIZER; Counter++)
        {
            strcpy (&Message0, "Slot ");
            _itoa (Counter+1, &Message1, 10);
            strcat (&Message0, &Message1);
            
            ndLBOX_AddItemToListBox (&Message0, &lbcb_wzxsave_SelectSlot, 0, !(facerec_ImagesInRam [Counter].IsActive), wzxsaveListBox, wzxsaveWindow);
        }
        
        ndLBOX_UpdateListBox (wzxsaveListBox, wzxsaveWindow, NORENDER);
        
        // Crea la name area
        
        ndWS_DrawRectangle (10, 140, 300, 160, COLOR_GRAY12, COLOR_BLACK, wzxsaveWindow, NORENDER);
        memset ( &(DBAPart1), 0, 511 );
        memset ( &(DBAPart2), 0, 511 );
        
        // Crea i pulsanti
        
        wzxsaveBtnChoose  = ndCTRL_CreateButton (325, WindowData [wzxsaveWindow].WSWndHeight - 75, 
                                               385, WindowData [wzxsaveWindow].WSWndHeight - 55,
                                               wzxsaveWindow, "wzxsaveChoose", "Choose", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                               &cb_wzxsave_ChooseTheName, 0, NORENDER);
        
        wzxsaveBtnSave    = ndCTRL_CreateButton (325, WindowData [wzxsaveWindow].WSWndHeight - 50, 
                                               385, WindowData [wzxsaveWindow].WSWndHeight - 30,
                                               wzxsaveWindow, "wzxsaveSave", "Save", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED, 
                                               &cb_wzxsave_SaveTheData, 0, NORENDER);
        
        wzxsaveBtnAbort   = ndCTRL_CreateButton (325, WindowData [wzxsaveWindow].WSWndHeight - 25, 
                                               385, WindowData [wzxsaveWindow].WSWndHeight - 5,
                                               wzxsaveWindow, "wzxsaveAbort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &cb_wzxsave_Abort, 0, NORENDER);
                                             
       
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzxsave_CloseWnd, 0, 0, wzxsaveWindow);
        
        // Richiama il renderer
        
        XWindowRender (wzxsaveWindow);                                     
        return 0;
    }
    else
        return -1;
} 

void INTERNAL_wzxsave_DrawInterfaceForModeMismatch (void)
{ 
    wzxsaveWindow = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Save data in .DBA file", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wzxsaveWindow>=0)
    {
        ndLP_MaximizeWindow (wzxsaveWindow);
        
        ndWS_GrWriteLn (5, 10, "You are not in face recognition mode.  ", COLOR_BLUE,  COLOR_GRAY, wzxsaveWindow, NORENDER);
        
        ndWS_GrWriteLn (5, 26, "This function can work only when the   ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
        ndWS_GrWriteLn (5, 34, "face recognition mode is operative.    ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
        ndWS_GrWriteLn (5, 42, "You can switch to this mode going into ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
        ndWS_GrWriteLn (5, 50, "the menu Fnc, Enable face recognizer   ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
    
        ndWS_GrWriteLn (5, 66, "Now click on Continue and after switching", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
        ndWS_GrWriteLn (5, 74, "retry to select this function.           ", COLOR_BLACK, COLOR_GRAY, wzxsaveWindow, NORENDER);
    
        
        wzxsaveBtnOk   = ndCTRL_CreateButton  (310, WindowData [wzxsaveWindow].WSWndHeight - 25, 
                                               385, WindowData [wzxsaveWindow].WSWndHeight - 5,
                                               wzxsaveWindow, "wzxsaveOk", "Continue", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &cb_wzxsave_Ok, 0, NORENDER);
                                             
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzxsave_CloseWnd, 0, 0, wzxsaveWindow);
        
        // Richiama il renderer
        
        XWindowRender (wzxsaveWindow);                                     
        return 0;
    }
    else
        return -1;
} 

#endif


 
int wzXSaveData (void)
{
    #ifndef PSP1000_VERSION
    
        if (!wzxsaveDialogBoxIsOpen)
        {
           // Impedisci l'avvio di due istanze della stessa dialog-box                         
           wzxsaveDialogBoxIsOpen=1;     
           
           baInhibitVideoEngine ();              
           baInhibitUpdateEngine ();
           baInhibitMailCheckerEngine ();
           
           if (baMode==BA_MODE_FACEREC)
              INTERNAL_wzxsave_DrawInterface ();
           else
              INTERNAL_wzxsave_DrawInterfaceForModeMismatch (); 
         
           wzxsaveYouCanExit=0;
           ndProvideMeTheMouse_Until (&wzxsaveYouCanExit, ND_IS_EQUAL, 1, 0, 0);
           
           // Ok... adesso provvedi a distruggere la vecchia dialog box di interfaccia e
           // Ripristina la possibilità di gestire la dialog-box
           
           ndLP_DestroyWindow (wzxsaveWindow);
           
           baDeInhibitVideoEngine_NoForce ();              
           baDeInhibitUpdateEngine ();
           baDeInhibitMailCheckerEngine ();
           
           wzxsaveDialogBoxIsOpen=0;
        }
        
    #endif
}

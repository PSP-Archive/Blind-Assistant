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
   WIZARD PER IL CARICAMENTO DEI FILE DA UN CONTAINER
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

int wzxloadWindow;

int wzxloadBtnChoose;
int wzxloadBtnSave;
int wzxloadBtnAbort;
int wzxloadBtnOk;

static int wzxloadDialogBoxIsOpen = 0;
static int wzxloadYouCanExit;

static int wzxloadListBox;
static int ChoosenSlot;

static char NameFile [255];
static char DBAFileName [512];

static char Path [512];
static char Name [512];
static char Ext [16];

static char DBAPart1 [512];
static char DBAPart2 [512];

static struct OpInfoPacket_Type xloadInfoPacket;

#endif

/*
   ----------------------------------------------------------------------------
   CODICE: CALLBACK DEL WIZARD DI SALVATAGGIO
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION
 
static void XLoadGraphCallback (int NrSlot, int NrSample, int LoadCode, struct OpInfoPacket_Type *InfoPacket)
{
    switch (LoadCode)
    {
           case BEGIN_TO_XLOAD:
           {  
              InfoPacket->WndHandle = ndLP_CreateWindow (70, 77, Windows_MaxScreenX-70, Windows_MaxScreenY-77,
                                                         "Loading DBA file", COLOR_WHITE, COLOR_BLUE,
                                                         COLOR_GRAY, COLOR_BLACK, 0);
              
              ndLP_MaximizeWindow (InfoPacket->WndHandle);
              
              ndWS_GrWriteLn (10, 10, "Waiting.... I'm loading the data ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              ndWS_GrWriteLn (10, 18, "in a DBA file                    ",   COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              XWindowRender (InfoPacket->WndHandle);
         
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 0, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              ndDelay (0.5);      
              break; 
           }
    
           case XLOAD_SIGNATURE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Loading file signature", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 5, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case ERROR_IN_XLOAD_SIGN:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Error in file signature", COLOR_RED, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 95, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              
              ndDelay (0.5);
              break;  
           }
           
           case XLOAD_DIMS:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Loading file dimensions", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 10, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case ERROR_IN_XLOAD_DIMS:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Incongruent dimensions of the image", COLOR_RED, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 95, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              
              ndDelay (0.5);
              break;  
           }
           
           case XLOAD_NAME:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Loading name of the person", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 15, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case XLOAD_IMG_SAMPLE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Loading image sample ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 20 + 30*(float)(NrSample)/(float)(NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case XLOAD_IMG_SAMPLE_COMPLETE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Loading image sample complete", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 20 + 30*(float)(NrSample+1)/(float)(NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case XLOAD_IMG_SAMPLE_ERROR:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Error in loading image sample", COLOR_RED, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 95, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              
              ndDelay (0.5);
              break;  
           }
           
           case XLOAD_INHIBIT_VIDEOENG:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Inhibit the video engine", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 55, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case XCOPY_MEM_AREA:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Copy memory area", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 60, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case DEALLOC_MEM_AREA:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Deallocation temporary buffers", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 65, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case XLOAD_FACESQ_UPDATE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Update face recognition database", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 70, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case XLOAD_GENERAL_FAILURE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Error in loading image sample", COLOR_RED, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 95, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              
              ndDelay (0.5);
              break;  
           }
           
           case SAVING_AUDIO_SAMPLE:    // Caso previsto perchè il loading da un file prevede il salvataggio nello slot
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Generating new audio sample", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 75 + 10*(float)(NrSample)/3.0, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;   
           }
           
           case SAVED_AUDIO_SAMPLE:    // Caso previsto perchè il loading da un file prevede il salvataggio nello slot
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Generated...", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 75 + 10*(float)(NrSample+1)/3.0, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;   
           }
           
           case SAVING_IMG_SAMPLE:   // Caso previsto perchè il loading da un file prevede il salvataggio nello slot
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Updating image sample in the slot", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 85 + 10*(float)(NrSample)/(float)(NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;   
           }
           
           case SAVED_IMG_SAMPLE:    // Caso previsto perchè il loading da un file prevede il salvataggio nello slot
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Updating image sample in the slot", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 85 + 10*(float)(NrSample+1)/(float)(NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;   
           }
           
           case XLOAD_DEINHIBIT_VIDEOENG:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Deinhibit the video engine", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 95, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case XLOAD_FAILURE_IN_MS_UPDATE:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Failure in memory stick update", COLOR_RED, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 95, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case DONE0:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Operation complete", COLOR_RED, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 100, 0, 100, 
                                        COLOR_WHITE, COLOR_GREEN, COLOR_GRAY03, COLOR_BLACK, 
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

static ndint64 cb_wzxload_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzxloadYouCanExit=1;
   return 0;   
}

static ndint64 cb_wzxload_Ok (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzxloadYouCanExit=1;
   return 0;   
}

static ndint64 cb_wzxload_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzxloadYouCanExit=1;
   return 0;    
}

static void lbcb_wzxload_SelectSlot (int NrItem, ndint64 LBoxCode, int ListBoxHandle, char WndHandle)
{
   ChoosenSlot = NrItem;
   ndCTRL_DeInhibitButton (wzxloadBtnChoose, wzxloadWindow, RENDER);     
}

static ndint64 cb_wzxload_LoadTheData (char *StringID, ndint64 InfoField, char WndHandle)
{
   
   int ErrRep;
   
   ErrRep=baXLoad_LoadTheDataFromTheSlot (ChoosenSlot, &DBAFileName, &XLoadGraphCallback, &xloadInfoPacket);
   
   if (!ErrRep)        // Il caricamento è andato a buon fine: cambia il pulsante
   {
       ndCTRL_ChangeButtonParameters (wzxloadBtnAbort, wzxloadWindow,
                                      "wzxloadAbort", "Done", "", 
                                      COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                      ND_BUTTON_ROUNDED, RENDER);         
   }
   
   return  0;  
}

static ndint64 cb_wzxload_ChooseTheName (char *StringID, ndint64 InfoField, char WndHandle)
{
    int Result, Len;
    
    Result=FileManager ("Choose the name for DBA file (load)", 0, 0, &DBAFileName);  
    
    if (Result==0)     // L'utente ha scelto un file
    { 
        fssSplitNameFile (&DBAFileName, &Path, &Name, &Ext);
        
        if (!strcmp (&Ext, "BDA"))
        {   
            // Visualizza i dati nella name area
            
            ndWS_DrawRectangle (10, 140, 300, 160, COLOR_GRAY12, COLOR_BLACK, wzxloadWindow, NORENDER);
           
            ndStrCpyBounded ( &DBAPart1, &DBAFileName, 0, 32, 0);
            ndWS_GrWriteLn  ( 15, 142, &DBAPart1, COLOR_BLACK, COLOR_GRAY12, wzxloadWindow, NORENDER);
            
            Len = strlen (DBAFileName);
            
            if (Len>32)
            {
                ndStrCpyBounded ( &DBAPart2, &DBAFileName, 33, Len, 0);
                ndWS_GrWriteLn  ( 15, 150, &DBAPart2, COLOR_BLACK, COLOR_GRAY12, wzxloadWindow, NORENDER);       
            }               
            
            // Deinibisce il pulsante che permette il salvataggio
            
            ndCTRL_DeInhibitButton (wzxloadBtnSave, wzxloadWindow, RENDER);     
        }
    } 
    
    return 0;
}


void INTERNAL_wzxload_DrawInterface (void)
{ 
    char Message0 [16];
    char Message1 [16];
    
    int Counter;
    
    wzxloadWindow = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Load data from .DBA file", 
                                       COLOR_WHITE, COLOR_GREEN, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wzxloadWindow>=0)
    {
        ndLP_MaximizeWindow (wzxloadWindow);
        
        ndWS_GrWriteLn (5, 10, "This option allows to recover from a  ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
        ndWS_GrWriteLn (5, 18, "file with extension DBA the infos     ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
        ndWS_GrWriteLn (5, 26, "relative to a person. They will be    ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
        ndWS_GrWriteLn (5, 34, "transferred to a slot of this program.", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
    
        ndWS_GrWriteLn (5, 50, "Choose the slot in which the recovered", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
        ndWS_GrWriteLn (5, 58, "data will be stored.                  ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
    
        // Crea una list box
        
        wzxloadListBox = ndLBOX_CreateListBox (10, 70, 300, 130, 0, 0, wzxloadWindow, NORENDER);
        
        for (Counter=0; Counter<NR_SLOTS_FACE_RECOGNIZER; Counter++)
        {
            strcpy (&Message0, "Slot ");
            _itoa (Counter+1, &Message1, 10);
            strcat (&Message0, &Message1);
            
            ndLBOX_AddItemToListBox (&Message0, &lbcb_wzxload_SelectSlot, 0, 0, wzxloadListBox, wzxloadWindow);
        }
        
        ndLBOX_UpdateListBox (wzxloadListBox, wzxloadWindow, NORENDER);
        
        // Crea la name area
        
        ndWS_DrawRectangle (10, 140, 300, 160, COLOR_GRAY12, COLOR_BLACK, wzxloadWindow, NORENDER);
        memset ( &(DBAPart1), 0, 511 );
        memset ( &(DBAPart2), 0, 511 );
        
        // Crea i pulsanti
        
        wzxloadBtnChoose  = ndCTRL_CreateButton (325, WindowData [wzxloadWindow].WSWndHeight - 75, 
                                               385, WindowData [wzxloadWindow].WSWndHeight - 55,
                                               wzxloadWindow, "wzxloadChoose", "Choose", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                               &cb_wzxload_ChooseTheName, 0, NORENDER);
        
        wzxloadBtnSave    = ndCTRL_CreateButton (325, WindowData [wzxloadWindow].WSWndHeight - 50, 
                                               385, WindowData [wzxloadWindow].WSWndHeight - 30,
                                               wzxloadWindow, "wzxloadLoad", "Load", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_BUTTON_INHIBITED, 
                                               &cb_wzxload_LoadTheData, 0, NORENDER);
        
        wzxloadBtnAbort   = ndCTRL_CreateButton (325, WindowData [wzxloadWindow].WSWndHeight - 25, 
                                               385, WindowData [wzxloadWindow].WSWndHeight - 5,
                                               wzxloadWindow, "wzxloadAbort", "Abort", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &cb_wzxload_Abort, 0, NORENDER);
                                             
       
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzxload_CloseWnd, 0, 0, wzxloadWindow);
        
        // Richiama il renderer
        
        XWindowRender (wzxloadWindow);                                     
        return 0;
    }
    else
        return -1;
} 


void INTERNAL_wzxload_DrawInterfaceForModeMismatch (void)
{ 
    wzxloadWindow = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Load data from .DBA file", 
                                       COLOR_WHITE, COLOR_GREEN, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wzxloadWindow>=0)
    {
        ndLP_MaximizeWindow (wzxloadWindow);
        
        ndWS_GrWriteLn (5, 10, "You are not in face recognition mode.  ", COLOR_BLUE,  COLOR_GRAY, wzxloadWindow, NORENDER);
        
        ndWS_GrWriteLn (5, 26, "This function can work only when the   ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
        ndWS_GrWriteLn (5, 34, "face recognition mode is operative.    ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
        ndWS_GrWriteLn (5, 42, "You can switch to this mode going into ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
        ndWS_GrWriteLn (5, 50, "the menu Fnc, Enable face recognizer   ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
    
        ndWS_GrWriteLn (5, 66, "Now click on Continue and after switching", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
        ndWS_GrWriteLn (5, 74, "retry to select this function.           ", COLOR_BLACK, COLOR_GRAY, wzxloadWindow, NORENDER);
    
        
        wzxloadBtnOk   = ndCTRL_CreateButton  (310, WindowData [wzxloadWindow].WSWndHeight - 25, 
                                               385, WindowData [wzxloadWindow].WSWndHeight - 5,
                                               wzxloadWindow, "wzxloadOk", "Continue", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &cb_wzxload_Ok, 0, NORENDER);
                                             
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzxload_CloseWnd, 0, 0, wzxloadWindow);
        
        // Richiama il renderer
        
        XWindowRender (wzxloadWindow);                                     
        return 0;
    }
    else
        return -1;
} 

#endif




 

int wzXLoadData (void)
{
    #ifndef PSP1000_VERSION
    
        if (!wzxloadDialogBoxIsOpen)
        {
           // Impedisci l'avvio di due istanze della stessa dialog-box                         
           wzxloadDialogBoxIsOpen=1;  
           
           baInhibitVideoEngine ();              
           baInhibitUpdateEngine ();
           baInhibitMailCheckerEngine ();
             
           if (baMode==BA_MODE_FACEREC)
              INTERNAL_wzxload_DrawInterface ();
           else
              INTERNAL_wzxload_DrawInterfaceForModeMismatch ();
         
           wzxloadYouCanExit=0;
           ndProvideMeTheMouse_Until (&wzxloadYouCanExit, ND_IS_EQUAL, 1, 0, 0);
           
           // Ok... adesso provvedi a distruggere la vecchia dialog box di interfaccia e
           // Ripristina la possibilità di gestire la dialog-box
           
           ndLP_DestroyWindow (wzxloadWindow);
           
           baDeInhibitVideoEngine_NoForce ();              
           baDeInhibitUpdateEngine ();
           baDeInhibitMailCheckerEngine ();
           
           wzxloadDialogBoxIsOpen=0;
        }
    
    #endif
}


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
   WIZARD PER IL SISTEMA DI OPZIONI ENGINE
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

int wzengoptWindow;
int wzengoptBtnClosed;
int wzengoptBtnDefault; 

int wzopteng_MinBrightness;
int wzopteng_SkipThreshold;
int wzopteng_FaceDetectorProbingArea;
int wzopteng_FaceDetectorThreshold;
int wzopteng_CheckForOldTS_FaceDetect;

static int wzengoptDialogBoxIsOpen = 0;
static int wzengoptYouCanExit;


/*
   ----------------------------------------------------------------------------
   CALLBACKS
   ----------------------------------------------------------------------------
*/ 


#ifndef PSP1000_VERSION

static ndint64 cb_wzengopt_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzengoptYouCanExit=1;
   return 0;    
}

static ndint64 cb_wzengopt_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{   
   wzengoptYouCanExit=1;
   return 0;   
}

static void cbtbar_MinBrightness (float Value, int TB_InfoField, unsigned char WndHandle)
{
    baOpt_ChangeMinBrightnessLevel ( Value );
    return;    
}

static void cbtbar_SkipThreshold (float Value, int TB_InfoField, unsigned char WndHandle)
{
    baOpt_ChangeSkipThreshold ( Value );
    return;    
}

static void cbtbar_FaceDetectorProbingArea (float Value, int TB_InfoField, unsigned char WndHandle)
{
    baOpt_ChangeFaceDetectorProbingArea ( Value );
    return;    
}

static void cbtbar_FaceDetectorThreshold (float Value, int TB_InfoField, unsigned char WndHandle)
{
    baOpt_ChangeFaceDetectorThreshold ( Value );
    return;    
}

static void cbcbox_CheckForOldTS_FaceDetect (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
    baOpt_CheckForOldTS_FaceDetect (ActualValue);
    return;    
}

static ndint64 cb_wzengopt_Default (char *StringID, ndint64 InfoField, char WndHandle)
{   
   ndTBAR_SetValue  (MIN_BRIGHTNESS_THRESHOLD_DEFAULT,  wzopteng_MinBrightness, wzengoptWindow, NORENDER);
   ndTBAR_SetValue  (PCA_SKIP_THRESHOLD_DEFAULT,        wzopteng_SkipThreshold, wzengoptWindow, NORENDER); 
   ndTBAR_SetValue  (FACEDETECTOR_PROBING_AREA_DEFAULT, wzopteng_FaceDetectorProbingArea, wzengoptWindow, NORENDER); 
   ndTBAR_SetValue  (FACEDETECTOR_THRESHOLD_DEFAULT,    wzopteng_FaceDetectorThreshold, wzengoptWindow, NORENDER); 
   
   ndCBOX_SetCboxOn (wzopteng_CheckForOldTS_FaceDetect, wzengoptWindow, 1, NORENDER);
   
   XWindowRender (wzengoptWindow);
   return 0;   
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE INTERFACCIA
   ----------------------------------------------------------------------------
*/ 


#ifndef PSP1000_VERSION

void INTERNAL_wzengopt_DrawInterface (void)
{
    struct  ndNetworkObject_Type ProbeNetObj;
    
    char    Message0 [32];
    char    Message1 [32];
    
    ndint64 Options;
    int     Counter;
    int     ErrRep;
    
    wzengoptWindow = ndLP_CreateWindow (30, 10, 440, Windows_MaxScreenY-10, "Options for recognizer", 
                                       COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY09, COLOR_GRAY09, 0);
                                       
    if (wzengoptWindow>=0)
    {
        ndLP_MaximizeWindow (wzengoptWindow);
        
        ndWS_GrWriteLn (30,  5, "Choose the options for face recognizer", COLOR_BLACK, COLOR_GRAY09, wzengoptWindow, NORENDER);
        ndWS_DrawLine  (30, 15, 360, 15, COLOR_GRAY, wzengoptWindow, NORENDER);
        
        // Controllo luminosità
        
        ndWS_GrWriteLn (30, 20, "Minimum brightness accepted", COLOR_BLACK, COLOR_GRAY09, wzengoptWindow, NORENDER);
        
        wzopteng_MinBrightness = ndTBAR_CreateTrackBar (30, 30, 290, 50, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_BLACK, 
                                                            COLOR_GRAY09, COLOR_BLACK, SystemOptPacket.MinBrightnessLevel, 0, 255, 
                                                            0, 
                                                            &cbtbar_MinBrightness, 0, wzengoptWindow, NORENDER); 
        
        // Skip threshold
        
        ndWS_GrWriteLn (30, 55, "Threshold for skipping faces", COLOR_BLACK, COLOR_GRAY09, wzengoptWindow, NORENDER);
        
        wzopteng_SkipThreshold = ndTBAR_CreateTrackBar (30, 65, 290, 85, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_BLACK, 
                                                            COLOR_GRAY09, COLOR_BLACK, SystemOptPacket.SkipThreshold, 0, 1000, 
                                                            0, 
                                                            &cbtbar_SkipThreshold, 0, wzengoptWindow, NORENDER); 
        
        // Face detector probe area
        
        ndWS_GrWriteLn (30, 90, "Face detector probe area", COLOR_BLACK, COLOR_GRAY09, wzengoptWindow, NORENDER);
        
        wzopteng_FaceDetectorProbingArea = ndTBAR_CreateTrackBar (30, 100, 290, 120, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_BLACK, 
                                                            COLOR_GRAY09, COLOR_BLACK, SystemOptPacket.FaceDetectorProbingArea, 5, 60, 
                                                            0, 
                                                            &cbtbar_FaceDetectorProbingArea, 0, wzengoptWindow, NORENDER); 
        
       // Face detector threshold
        
        ndWS_GrWriteLn (30, 125, "Face detector probe threshold", COLOR_BLACK, COLOR_GRAY09, wzengoptWindow, NORENDER);
        
        wzopteng_FaceDetectorThreshold = ndTBAR_CreateTrackBar (30, 135, 290, 155, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_BLACK, 
                                                                COLOR_GRAY09, COLOR_BLACK, SystemOptPacket.FaceDetectorThreshold, 110, 400, 
                                                                0, 
                                                                &cbtbar_FaceDetectorThreshold, 0, wzengoptWindow, NORENDER); 
        
        // Crea la check-box per il controllo della presenza del vecchio training set
        
        ndWS_GrWriteLn (35, 160, "Advice if the system is using ",   COLOR_BLACK, COLOR_GRAY09, wzengoptWindow, NORENDER);
        ndWS_GrWriteLn (35, 169, "the old version of the face   ", COLOR_BLACK, COLOR_GRAY09, wzengoptWindow, NORENDER);
        ndWS_GrWriteLn (35, 178, "recognizer training set       ", COLOR_BLACK, COLOR_GRAY09, wzengoptWindow, NORENDER);
        
        
        wzopteng_CheckForOldTS_FaceDetect = ndCBOX_CreateCBox (10, 163, "cboxCheckOldTS", SystemOptPacket.CheckForOldTS_FaceDetect, 
                                       0, &(SystemOptPacket.CheckForOldTS_FaceDetect), COLOR_WHITE, 
                                       &cbcbox_CheckForOldTS_FaceDetect, wzengoptWindow, NORENDER);
       
        // Crea il pulsante di reimpostazione dei valori di default
        
        wzengoptBtnDefault   = ndCTRL_CreateButton (305, WindowData [wzengoptWindow].WSWndHeight - 50, 
                                                385, WindowData [wzengoptWindow].WSWndHeight - 30,
                                                wzengoptWindow, "wzengoptDefault", "Default", "", 
                                                COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                ND_BUTTON_ROUNDED, 
                                                &cb_wzengopt_Default, 0, NORENDER);
        
        // Crea il pulsante di chiusura
        
        wzengoptBtnClosed   = ndCTRL_CreateButton (305, WindowData [wzengoptWindow].WSWndHeight - 25, 
                                                385, WindowData [wzengoptWindow].WSWndHeight - 5,
                                                wzengoptWindow, "wzengoptClose", "Close", "", 
                                                COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                ND_BUTTON_ROUNDED, 
                                                &cb_wzengopt_Abort, 0, NORENDER);
                                             
       
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzengopt_CloseWnd, 0, 0, wzengoptWindow);
        
        
        
        // Richiama il renderer
        
        XWindowRender (wzengoptWindow);                                     
        return 0;
    }
    else
        return -1;
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/ 
 
 
void wzEngineOptions (void)
{
    #ifndef PSP1000_VERSION
    
        if (!wzengoptDialogBoxIsOpen)
        {
           // Impedisci l'avvio di due istanze della stessa dialog-box                         
           wzengoptDialogBoxIsOpen=1;     
           
           INTERNAL_wzengopt_DrawInterface ();
         
           wzengoptYouCanExit=0;
           ndProvideMeTheMouse_Until (&wzengoptYouCanExit, ND_IS_EQUAL, 1, 0, 0);
           
           // Ok... adesso provvedi a distruggere la vecchia dialog box di interfaccia e
           // Ripristina la possibilità di gestire la dialog-box
           
           ndLP_DestroyWindow (wzengoptWindow);
           wzengoptDialogBoxIsOpen=0;
        }
        
    #endif
}

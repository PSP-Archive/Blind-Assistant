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
   WIZARD PER IL GESTORE OPZIONI
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
 
/*
   ----------------------------------------------------------------------------
   VARIABILI PER USO INTERNO
   ----------------------------------------------------------------------------
*/ 

int wzoptWindow;

int wzoptBtnChoose;
int wzoptBtnSave;
int wzoptBtnClosed;

static int wzoptDialogBoxIsOpen = 0;
static int wzoptYouCanExit;

char wzoptCbox_WallpaperOn,     wzoptBtn__ChooseWallPaper;
char wzoptCbox_CheckForUpdate;
char wzoptCbox_ConnectToNetStartup;
char wzoptCbox_ConnectToBldSvrStartup;
char wzoptTbar_MinutesBetUpdate;
char wzoptTbox_UpdateSite;
char wzoptCbox_AutoDownload; 
char wzoptCbox_AutoInstall; 
char wzoptBtn__Advanced;
char wzoptBtn__BlindServer;
char wzoptBtn__FaceRecEngine;
char wzoptBtn__DefaultSite;
char wzoptBtn__OcrOptions;
char wzoptBtn__MailClient;

char Temp_DownloadFromSite [1025];










/*
   ----------------------------------------------------------------------------
   CODICE: CALLBACKS DEGLI ELEMENTI
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzopt_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{   
   UpdateOptionsOnTheDisk ();       // Salva le eventuali opzioni cambiate sul disco
   
   wzoptYouCanExit=1;
   return 0;   
}

static ndint64 cb_wzopt_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   UpdateOptionsOnTheDisk ();       // Salva le eventuali opzioni cambiate sul disco
   
   wzoptYouCanExit=1;
   return 0;    
}

static void cbcbox_wzopt_ConnectToNetStartup (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
    baOpt_EnableDisableConnectToNetAtStartup (ActualValue);
    return;   
}

static void cbcbox_wzopt_ConnectToBldSvrStartup (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
    baOpt_EnableDisableConnectToBldSvrAtStartup (ActualValue);
    return;   
}

static void cbcbox_wzopt_WallpaperOn (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
    if (SystemOptPacket.EnableWallPaper) 
            ndCTRL_DeInhibitButton (wzoptBtn__ChooseWallPaper, wzoptWindow, RENDER); 
        else
            ndCTRL_InhibitButton (wzoptBtn__ChooseWallPaper, wzoptWindow, RENDER); 
        
    baOpt_EnableDisableWallPaper();
    return;   
}

static ndint64 cb_wzopt_ChooseWallPaper (char *StringID, ndint64 InfoField, char WndHandle)
{
    char WallPaperName [512];
    char ErrRep;
    
    ErrRep=FileManager ("Choose the wallpaper name", 0, 0, &WallPaperName);  
    
    if (!ErrRep)   // L'utente ha scelto
    {
        baOpt_ChangeWallPaper (&WallPaperName);             
    }
}

static ndint64 cb_wzopt_CallAdvancedOptions (char *StringID, ndint64 InfoField, char WndHandle)
{
    wzAdvancedOptions ();
}

static ndint64 cb_wzopt_CallEngineOptions (char *StringID, ndint64 InfoField, char WndHandle)
{
    wzEngineOptions ();
}

static ndint64 cb_wzopt_CallBlindServerOptions (char *StringID, ndint64 InfoField, char WndHandle)
{
    wzBlindServerOptions ();
}

static ndint64 cb_wzopt_CallOcrOptions (char *StringID, ndint64 InfoField, char WndHandle)
{
    wzOcrOptions ();
}

static ndint64 cb_wzopt_CallMailOptions (char *StringID, ndint64 InfoField, char WndHandle)
{
    wzMailOptions ();
}


static void cbcbox_wzopt_CheckForUpdate (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
    if (ActualValue)
    {
        ndTBAR_DeInhibitTrackBar (wzoptTbar_MinutesBetUpdate, wzoptWindow, NORENDER);
        ndTBOX_DeInhibitTextArea (wzoptTbox_UpdateSite, wzoptWindow, NORENDER);
        ndCBOX_DeInhibitCBox     (wzoptCbox_AutoInstall, wzoptWindow, NORENDER);
        ndCBOX_DeInhibitCBox     (wzoptCbox_AutoDownload, wzoptWindow, NORENDER);
        ndCTRL_DeInhibitButton   (wzoptBtn__DefaultSite, wzoptWindow, NORENDER);   
        ndCTRL_DeInhibitButton   (wzoptBtn__Advanced, wzoptWindow, NORENDER);   
        
        XWindowRender (wzoptWindow);
    }
    else
    {
        ndTBAR_InhibitTrackBar (wzoptTbar_MinutesBetUpdate, wzoptWindow, NORENDER);
        ndTBOX_InhibitTextArea (wzoptTbox_UpdateSite, wzoptWindow, NORENDER);
        ndCBOX_InhibitCBox     (wzoptCbox_AutoInstall, wzoptWindow, NORENDER);  
        ndCBOX_InhibitCBox     (wzoptCbox_AutoDownload, wzoptWindow, NORENDER); 
        ndCTRL_InhibitButton   (wzoptBtn__DefaultSite, wzoptWindow, NORENDER); 
        ndCTRL_InhibitButton   (wzoptBtn__Advanced, wzoptWindow, NORENDER);   
        
        XWindowRender (wzoptWindow);
    }
    
    // Provvede ad attivare/disattivare il motore di update
    baOpt_EnableDisableChecksForUpdate (ActualValue);
    return;   
}

static void cbcbox_wzopt_AutoDownload (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
    baOpt_EnableDisableAutoDownload (ActualValue);
    return;   
}

static void cbcbox_wzopt_AutoInstall (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
    baOpt_EnableDisableAutoInstall (ActualValue);
    return;      
}


static void cbtbox_wzopt_ChangeDownloadSite (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
{
    // Segnala il cambiamento, in modo che il sistema salvi il file delle opzioni in uscita
    
    baOpt_ChangeDownloadSite (TextData);
    return;           
}


static ndint64 cb_wzopt_SetDefaultSite (char *StringID, ndint64 InfoField, char WndHandle)
{
    char DefaultSite [1025];
    
    memset (&DefaultSite, 0, 1024);
    strcpy (&DefaultSite, BA_SITE);
    
    ndTBOX_ChangeTextAreaContent (wzoptTbox_UpdateSite, wzoptWindow, &DefaultSite, RENDER);
    baOpt_ChangeDownloadSite (&DefaultSite);
    return;           
}

static void cbtbar_MinutesBetUpdate (float Value, int TB_InfoField, unsigned char WndHandle)
{
    baOpt_ChangeMinBetweenChecks ( (int)(Value) );
    return;    
}


/*
   ----------------------------------------------------------------------------
   CODICE: INIBIZIONE SPECIFICI ELEMENTI A SECONDA DELLA PIATTAFORMA
   ----------------------------------------------------------------------------
*/ 

static void INTERNAL_wzopt_InhibitElementsForPlatform (void)
{
     #ifdef PSP1000_VERSION
     
            ndCBOX_InhibitCBox   (wzoptCbox_WallpaperOn,   wzoptWindow, NORENDER);
            ndCTRL_InhibitButton (wzoptBtn__FaceRecEngine, wzoptWindow, NORENDER);
            ndCTRL_InhibitButton (wzoptBtn__MailClient,    wzoptWindow, NORENDER);
            
     #endif
}     


/*
   ----------------------------------------------------------------------------
   CODICE: CREAZIONE INTERFACCIA
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_wzopt_DrawInterface (void)
{
    ndint64 Options;
    int     Counter;
    
    wzoptWindow = ndLP_CreateWindow (10, 10, 420, Windows_MaxScreenY-10, "Blind Assistant Options", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0);
                                       
    if (wzoptWindow>=0)
    {
        ndLP_MaximizeWindow (wzoptWindow);
        
        // Connetti ad internet all'avvio
        
        wzoptCbox_ConnectToNetStartup = ndCBOX_CreateCBox (10, 5, "cboxConnectToNETStartup", SystemOptPacket.ConnectToNetAtStartup, 
                                    0, &(SystemOptPacket.ConnectToNetAtStartup), COLOR_WHITE, 
                                    &(cbcbox_wzopt_ConnectToNetStartup), wzoptWindow, NORENDER);
        
        ndWS_GrWriteLn (30, 8, "Connect to network at startup ", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        
        // Connetti al Blind Server all'avvio
        
        wzoptCbox_ConnectToBldSvrStartup = ndCBOX_CreateCBox (10, 20, "cboxConnectToBDStartup", SystemOptPacket.ConnectToBldSvrAtStartup, 
                                    0, &(SystemOptPacket.ConnectToBldSvrAtStartup), COLOR_WHITE, 
                                    &(cbcbox_wzopt_ConnectToBldSvrStartup), wzoptWindow, NORENDER);
        
        ndWS_GrWriteLn (30, 23, "Connect to BlindServer at startup ", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        
        // Attivazione wallpaper
        
        wzoptCbox_WallpaperOn = ndCBOX_CreateCBox (10, 35, "cboxWallPaperOn", SystemOptPacket.EnableWallPaper, 
                                    0, &(SystemOptPacket.EnableWallPaper), COLOR_WHITE, 
                                    cbcbox_wzopt_WallpaperOn, wzoptWindow, NORENDER);
        
        ndWS_GrWriteLn (30, 38, "Enable wallpaper ", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        
        wzoptBtn__ChooseWallPaper = ndCTRL_CreateButton (305, 10, 385, 30,
                                        wzoptWindow, "wzoptChWallpaper", "Choose", "", 
                                        COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                        ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                        &cb_wzopt_ChooseWallPaper, 0, NORENDER);

        if (SystemOptPacket.EnableWallPaper) 
            ndCTRL_DeInhibitButton (wzoptBtn__ChooseWallPaper, wzoptWindow, NORENDER); 
        else
            ndCTRL_InhibitButton (wzoptBtn__ChooseWallPaper, wzoptWindow, NORENDER); 
        
        // Crea la check box per il controllo degli aggiornamenti da parte del Visilab
        
        wzoptCbox_CheckForUpdate = ndCBOX_CreateCBox (10, 50, "cboxCheckForUpdate", SystemOptPacket.CheckForUpdate, 
                                       0, &(SystemOptPacket.CheckForUpdate), COLOR_WHITE, 
                                       cbcbox_wzopt_CheckForUpdate, wzoptWindow, NORENDER);
        
        ndWS_GrWriteLn (30, 51, "Check if Visilab has released an", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        ndWS_GrWriteLn (30, 59, "update                          ", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        
        // Crea il pulsante per l'opzione advanced di rete
        
        wzoptBtn__Advanced       = ndCTRL_CreateButton (305, 35, 385, 55,
                                        wzoptWindow, "wzoptAdvOptions", "Advanced", "", 
                                        COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                        ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                        &cb_wzopt_CallAdvancedOptions, 0, NORENDER);
                                        
        // Crea il pulsante per le opzioni dell'engine video
        
        wzoptBtn__FaceRecEngine  = ndCTRL_CreateButton (305, 60, 385, 80,
                                        wzoptWindow, "wzoptFaceRecOptions", "FaceRec", "", 
                                        COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                        ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                        &cb_wzopt_CallEngineOptions, 0, NORENDER);
                                        
        // Crea il pulsante per le opzioni del Blind Server
        
        wzoptBtn__BlindServer    = ndCTRL_CreateButton (305, 85, 385, 105,
                                        wzoptWindow, "wzoptBlSvrOptions", "BServer", "", 
                                        COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                        ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                        &cb_wzopt_CallBlindServerOptions, 0, NORENDER);
        
        // Crea il pulsante per l'accesso alle opzioni dell'OCR
        
        wzoptBtn__OcrOptions     = ndCTRL_CreateButton (305, 110, 385, 130,
                                        wzoptWindow, "wzoptOcrOptions", "OCR", "", 
                                        COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                        ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                        &cb_wzopt_CallOcrOptions, 0, NORENDER);
           
        // Crea il pulsante per l'accesso alle opzioni del client email   
                                        
        wzoptBtn__MailClient     = ndCTRL_CreateButton (305, 135, 385, 155,
                                        wzoptWindow, "wzoptMail", "Mail", "", 
                                        COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                        ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                        &cb_wzopt_CallMailOptions, 0, NORENDER);
        
        // Crea la track bar per indicare l'intervallo temporale tra due aggiornamenti
        
        ndWS_GrWriteLn (30, 81, "You'll do checks every minutes..", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        
        wzoptTbar_MinutesBetUpdate = ndTBAR_CreateTrackBar (30, 93, 290, 113, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_BLACK, 
                                                            COLOR_GRAY08, COLOR_BLACK, SystemOptPacket.MinBetweenChecks, 1, 240, 
                                                            0, 
                                                            &cbtbar_MinutesBetUpdate, 0, wzoptWindow, NORENDER); 
        
        // Crea la text box per indicare l'indirizzo da cui scaricare l'aggiornamento
        
        ndWS_GrWriteLn (30, 120, "Download updates from", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        
        memset ( &Temp_DownloadFromSite, 0, 1024);
        strcpy ( &Temp_DownloadFromSite, &SystemOptPacket.UpdateFromSite );
        
        wzoptTbox_UpdateSite = ndTBOX_CreateTextArea (30, 135, 290, 155, "Download site",
                                                      NDKEY_SETTEXTAREA (511, 1), Temp_DownloadFromSite,
                                                      COLOR_BLACK, COLOR_WHITE,
                                                      COLOR_BLACK, COLOR_GRAY,
                                                      COLOR_BLACK, COLOR_GRAY03,
                                                      NULL, NULL, cbtbox_wzopt_ChangeDownloadSite,
                                                      0,
                                                      wzoptWindow, NORENDER);
        
        // Crea il pulsante per l'opzione indirizzo di default
        
        wzoptBtn__DefaultSite       = ndCTRL_CreateButton (305, 160, 385, 180,
                                        wzoptWindow, "wzoptDefaultSite", "Default", "site", 
                                        COLOR_BLACK, COLOR_GRAY11, COLOR_BLACK, 
                                        ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                        &cb_wzopt_SetDefaultSite, 0, NORENDER);
        
        // Crea la check box per il download automatico dei nuovi update
        
        wzoptCbox_AutoDownload    = ndCBOX_CreateCBox (10, 165, "cboxAutoDownload", SystemOptPacket.AutoDownloadNewUpdates, 
                                       0, &(SystemOptPacket.AutoDownloadNewUpdates), COLOR_WHITE, 
                                       cbcbox_wzopt_AutoDownload, wzoptWindow, NORENDER);
        
        ndWS_GrWriteLn (30, 165, "Download automatically the updates", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        ndWS_GrWriteLn (30, 173, "released by Visilab Research      ", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        
        // Crea la check box per l'autoupdate
        
        wzoptCbox_AutoInstall    = ndCBOX_CreateCBox (10, 185, "cboxAutoInstall", SystemOptPacket.AutoInstallNewUpdates, 
                                       0, &(SystemOptPacket.AutoInstallNewUpdates), COLOR_WHITE, 
                                       cbcbox_wzopt_AutoInstall, wzoptWindow, NORENDER);
        
        ndWS_GrWriteLn (30, 185, "Install automatically the updates", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        ndWS_GrWriteLn (30, 193, "that the system has downloaded  ", COLOR_BLACK, COLOR_GRAY, wzoptWindow, NORENDER);
        
        // Inibisci nell'ipotesi che il check per aggiornamenti non fosse attivato
        
        if (!SystemOptPacket.CheckForUpdate)
        {
            ndTBAR_InhibitTrackBar (wzoptTbar_MinutesBetUpdate, wzoptWindow, NORENDER);
            ndTBOX_InhibitTextArea (wzoptTbox_UpdateSite, wzoptWindow, NORENDER);
            ndCBOX_InhibitCBox     (wzoptCbox_AutoInstall, wzoptWindow, NORENDER); 
            ndCBOX_InhibitCBox     (wzoptCbox_AutoDownload, wzoptWindow, NORENDER); 
            ndCTRL_InhibitButton   (wzoptBtn__DefaultSite, wzoptWindow, NORENDER); 
        }
        
        // Crea il pulsante di chiusura
        
        wzoptBtnClosed   = ndCTRL_CreateButton (305, 185, 385, 205,
                                                wzoptWindow, "wzoptClose", "Close", "", 
                                                COLOR_BLACK, COLOR_GRAY11, COLOR_BLACK, 
                                                ND_BUTTON_ROUNDED, 
                                                &cb_wzopt_Abort, 0, NORENDER);
                                             
       
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzopt_CloseWnd, 0, 0, wzoptWindow);
        
        // Provvede ad inibire gli elementi dell'interfaccia che non sono disponibili 
        // su questa piattaforma
        
        INTERNAL_wzopt_InhibitElementsForPlatform ();
        
        // Richiama il renderer
        
        XWindowRender (wzoptWindow);                                     
        return 0;
    }
    else
        return -1;
}




/*
   ----------------------------------------------------------------------------
   ROUTINE PRINCIPALE
   ----------------------------------------------------------------------------
*/ 

void wzOptions (void)
{    
    if (!wzoptDialogBoxIsOpen)
    {
       // Impedisci l'avvio di due istanze della stessa dialog-box                         
       wzoptDialogBoxIsOpen=1;     
       
       INTERNAL_wzopt_DrawInterface ();
     
       wzoptYouCanExit=0;
       ndProvideMeTheMouse_Until (&wzoptYouCanExit, ND_IS_EQUAL, 1, 0, 0);
       
       // Ok... adesso provvedi a distruggere la vecchia dialog box di interfaccia e
       // Ripristina la possibilità di gestire la dialog-box
       
       ndLP_DestroyWindow (wzoptWindow);
       wzoptDialogBoxIsOpen=0;
    }
} 

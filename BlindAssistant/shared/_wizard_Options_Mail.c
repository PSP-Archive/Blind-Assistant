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
   WIZARD PER LE OPZIONI DEL CLIENT E-MAIL
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

int  wzmailoptWindow;

int  wzmailoptDialogBoxIsOpen = 0;
int  wzmailoptYouCanExit;

char wzmailoptTbox_ServerAddrStr;
char wzmailoptTbox_ServerPort; 
char wzmailoptTbox_ServerUserName; 
char wzmailoptTbox_ServerPasswd; 

char wzmailoptCbox_CheckForNewMails;
char wzmailoptTBar_MinutesBetweenCheck;

static char wzmailopt_StrPort [20];

char wzmailoptBtnClosed;

#endif

/*
   ----------------------------------------------------------------------------
   CALLBACKS
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wzmailopt_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzmailoptYouCanExit=1;
   return 0;    
}

static ndint64 cb_wzmailopt_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{   
   wzmailoptYouCanExit=1;
   return 0;   
}

static char cbtbox_ServerAddrStr (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
{
    baOpt_ChangePop3Addr (TextData);
    return 1;   
}

static char cbtbox_ServerAddrPort (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
{
    int x = atoi (TextData);
    
    if (x!=0)    // I dati sono autentificati solo se non viene immessa una stringa
    {
        baOpt_ChangePop3Port (x);
        return 1;         
    }
    else
       return 0;
}

static char cbtbox_Pop3UserName (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
{
    baOpt_ChangePop3UserName (TextData);
    return 1;   
}

static char cbtbox_Pop3Password (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
{
    baOpt_ChangePop3Password (TextData);
    return 1;   
}

static void cbcbox_wzmail_CheckForNewMails (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
    baOpt_EnableDisableChecksForNewMails (ActualValue);
    return 1;  
}

static void cbtbar_wzmail_MinutesBetweenCheck (float Value, int TB_InfoField, unsigned char WndHandle)
{
    baOpt_ChangeMinBetweenChecksForMails (Value);
    return; 
}

#endif

/*
   ----------------------------------------------------------------------------
   CODICE INTERFACCIA
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

void INTERNAL_wzmailopt_DrawInterface (void)
{
    char    Message0 [32];
    char    Message1 [32];
    
    ndint64 Options;
    int     Counter;
    int     ErrRep;
    
    wzmailoptWindow = ndLP_CreateWindow (30, 10, 440, Windows_MaxScreenY-20, "Options for mail client", 
                                       COLOR_WHITE, COLOR_LGREEN, COLOR_GRAY09, COLOR_GRAY09, 0);
                                       
    if (wzmailoptWindow>=0)
    {
        ndLP_MaximizeWindow (wzmailoptWindow);
        
        ndWS_GrWriteLn (30,  5, "Choose the options for the mail client", COLOR_BLACK, COLOR_GRAY09, wzmailoptWindow, NORENDER);
         
        // Indirizzo del server
        
        ndWS_GrWriteLn (10, 30, "Pop3 server", COLOR_BLACK, COLOR_GRAY09, wzmailoptWindow, NORENDER);
        
        wzmailoptTbox_ServerAddrStr = ndTBOX_CreateTextArea    (155, 27, 385, 41, "Pop3_Server", 
                                                                 NDKEY_SETTEXTAREA (128, 1), 0, 
                                                                 COLOR_BLACK,  COLOR_WHITE,
                                                                 COLOR_BLACK,  COLOR_GRAY, 
                                                                 COLOR_BLACK,  COLOR_GRAY02,
                                                                 0, 0, &cbtbox_ServerAddrStr, 0, 
                                                                 wzmailoptWindow, NORENDER);
        
        ndTBOX_ChangeTextAreaContent (wzmailoptTbox_ServerAddrStr, wzmailoptWindow, &(SystemOptPacket.Pop3ServerAddr), NORENDER);
        
        // Porta utilizzata
        
        _itoa (SystemOptPacket.Pop3Port, &wzmailopt_StrPort, 10);
        
        ndWS_GrWriteLn (10, 50, "Pop3 port", COLOR_BLACK, COLOR_GRAY09, wzmailoptWindow, NORENDER);
        
        wzmailoptTbox_ServerPort  = ndTBOX_CreateTextArea    (155, 47, 385, 61, "Pop3_Port", 
                                                                NDKEY_SETTEXTAREA (20, 1), 0, 
                                                                COLOR_BLACK,  COLOR_WHITE,
                                                                COLOR_BLACK,  COLOR_GRAY, 
                                                                COLOR_BLACK,  COLOR_GRAY02,
                                                                0, &cbtbox_ServerAddrPort, 0, 0, 
                                                                wzmailoptWindow, NORENDER);
        
        ndTBOX_ChangeTextAreaContent (wzmailoptTbox_ServerPort, wzmailoptWindow, &wzmailopt_StrPort, NORENDER);
        
        // UserName
        
        ndWS_GrWriteLn (10, 70, "UserName", COLOR_BLACK, COLOR_GRAY09, wzmailoptWindow, NORENDER);
        
        wzmailoptTbox_ServerUserName  = ndTBOX_CreateTextArea    (155, 67, 385, 81, "Pop3_Username", 
                                                                    NDKEY_SETTEXTAREA (32, 1), 0, 
                                                                    COLOR_BLACK,  COLOR_WHITE,
                                                                    COLOR_BLACK,  COLOR_GRAY, 
                                                                    COLOR_BLACK,  COLOR_GRAY02,
                                                                    0, 0, &cbtbox_Pop3UserName, 0, 
                                                                    wzmailoptWindow, NORENDER);
        
        ndTBOX_ChangeTextAreaContent (wzmailoptTbox_ServerUserName, wzmailoptWindow, &(SystemOptPacket.Pop3UserName), NORENDER);
        
        // Password 
        
        ndWS_GrWriteLn (10, 90, "Password", COLOR_BLACK, COLOR_GRAY09, wzmailoptWindow, NORENDER);
        
        wzmailoptTbox_ServerPasswd  = ndTBOX_CreateTextArea    (155, 87, 385, 101, "Pop3_Passwd", 
                                                                  NDKEY_SETTEXTAREA (32, 1), 0, 
                                                                  COLOR_BLACK,  COLOR_WHITE,
                                                                  COLOR_BLACK,  COLOR_GRAY, 
                                                                  COLOR_BLACK,  COLOR_GRAY02,
                                                                  0, 0, &cbtbox_Pop3Password, 0, 
                                                                  wzmailoptWindow, NORENDER);
        
        ndTBOX_ChangeTextAreaContent (wzmailoptTbox_ServerPasswd, wzmailoptWindow, &(SystemOptPacket.Pop3Password), NORENDER);
        
        
        // Crea il pulsante di chiusura
        
        wzmailoptBtnClosed   = ndCTRL_CreateButton (305, WindowData [wzmailoptWindow].WSWndHeight - 25, 
                                                    385, WindowData [wzmailoptWindow].WSWndHeight - 5,
                                                    wzmailoptWindow, "wzmailoptClose", "Close", "", 
                                                    COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                    ND_BUTTON_ROUNDED, 
                                                    &cb_wzmailopt_Abort, 0, NORENDER);
                                           
        // Crea la check-box per il controllo delle e-mail
        
        wzmailoptCbox_CheckForNewMails = ndCBOX_CreateCBox (5, 120, "wzmailCheckForNewMails", SystemOptPacket.CheckForNewMails, 
                                                            0, &(SystemOptPacket.CheckForNewMails), COLOR_WHITE, 
                                                            cbcbox_wzmail_CheckForNewMails, wzmailoptWindow, NORENDER);
        
        ndWS_GrWriteLn (25, 123, "Autocheck if there are new mails every", COLOR_BLACK, COLOR_GRAY09, wzmailoptWindow, NORENDER);
                                             
        // Crea la trackbar per i minuti di controllo
        
        wzmailoptTBar_MinutesBetweenCheck  = ndTBAR_CreateTrackBar (5, 140, 235, 160, COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_BLACK, 
                                                                    COLOR_GRAY09, COLOR_BLACK, 
                                                                    SystemOptPacket.MinBetweenChecksForMails, 2, 60, 
                                                                    0, 
                                                                    &cbtbar_wzmail_MinutesBetweenCheck, NULL, wzmailoptWindow, NORENDER); 
       
        ndWS_GrWriteLn (245, 146, "minutes", COLOR_BLACK, COLOR_GRAY09, wzmailoptWindow, NORENDER);
       
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzmailopt_CloseWnd, 0, 0, wzmailoptWindow);
        
        // Richiama il renderer
        
        XWindowRender (wzmailoptWindow);                                     
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
 
 
void wzMailOptions (void)
{
    #ifndef PSP1000_VERSION
    
        if (!wzmailoptDialogBoxIsOpen)
        {
           // Impedisci l'avvio di due istanze della stessa dialog-box                         
           wzmailoptDialogBoxIsOpen=1;     
           
           INTERNAL_wzmailopt_DrawInterface ();
         
           wzmailoptYouCanExit=0;
           ndProvideMeTheMouse_Until (&wzmailoptYouCanExit, ND_IS_EQUAL, 1, 0, 0);
           
           // Ok... adesso provvedi a distruggere la vecchia dialog box di interfaccia e
           // Ripristina la possibilità di gestire la dialog-box
           
           ndLP_DestroyWindow (wzmailoptWindow);
           wzmailoptDialogBoxIsOpen=0;
        }
        
    #endif
}


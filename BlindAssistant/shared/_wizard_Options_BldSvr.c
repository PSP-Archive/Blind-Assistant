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
   WIZARD PER LE OPZIONI DEL BLIND SERVER
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

int wzbldsvrWindow;

int wzbldsvrDialogBoxIsOpen = 0;
int wzbldsvrYouCanExit;

char wzbldsvrTbox_ServerAddrStr;
char wzbldsvrTbox_ServerPort; 
char wzbldsvrTbox_ServerPasswd; 

static char wzbldsvr_StrPort [20];

char wzbldsvrBtnClosed;


/*
   ----------------------------------------------------------------------------
   CALLBACKS
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzbldsvr_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzbldsvrYouCanExit=1;
   return 0;    
}

static ndint64 cb_wzbldsvr_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{   
   wzbldsvrYouCanExit=1;
   return 0;   
}


static char cbtbox_ServerAddrStr (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
{
    baOpt_ChangeBlindServerIPAddr (TextData);
    return 1;   
}

static char cbtbox_ServerAddrPort (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
{
    int x = atoi (TextData);
    
    if (x!=0)    // I dati sono autentificati solo se non viene immessa una stringa
    {
        baOpt_ChangeBlindServerPort (x);
        return 1;         
    }
    else
       return 0;
}

static char cbtbox_ServerPasswd (void *ProcessValueData, void *TextData, int TextBoxHandle, char TextBoxWndHandle)
{
    baOpt_ChangeBlindServerPasswd (TextData);
    return 1;   
}




/*
   ----------------------------------------------------------------------------
   CODICE INTERFACCIA
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_wzbldsvr_DrawInterface (void)
{
    char    Message0 [32];
    char    Message1 [32];
    
    ndint64 Options;
    int     Counter;
    int     ErrRep;
    
    wzbldsvrWindow = ndLP_CreateWindow (30, 10, 440, Windows_MaxScreenY-20, "Options for BlindServer", 
                                       COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY09, COLOR_GRAY09, 0);
                                       
    if (wzbldsvrWindow>=0)
    {
        ndLP_MaximizeWindow (wzbldsvrWindow);
        
        ndWS_GrWriteLn (30,  5, "Choose the options for the BlindServer", COLOR_BLACK, COLOR_GRAY09, wzbldsvrWindow, NORENDER);
         
        // Indirizzo del server
        
        ndWS_GrWriteLn (10, 50, "IP Address", COLOR_BLACK, COLOR_GRAY09, wzbldsvrWindow, NORENDER);
        
        wzbldsvrTbox_ServerAddrStr = ndTBOX_CreateTextArea    (155, 47, 385, 61, "BldServer_Address", 
                                                NDKEY_SETTEXTAREA (20, 1), 0, 
                                                COLOR_BLACK,  COLOR_WHITE,
                                                COLOR_BLACK,  COLOR_GRAY, 
                                                COLOR_BLACK,  COLOR_GRAY02,
                                                0, 0, &cbtbox_ServerAddrStr, 0, 
                                                wzbldsvrWindow, NORENDER);
        
        ndTBOX_ChangeTextAreaContent (wzbldsvrTbox_ServerAddrStr, wzbldsvrWindow, &(SystemOptPacket.BlindServerAddrStr), NORENDER);
        
        // Porta utilizzata
        
        _itoa (SystemOptPacket.BlindServerPort, &wzbldsvr_StrPort, 10);
        
        ndWS_GrWriteLn (10, 70, "Port", COLOR_BLACK, COLOR_GRAY09, wzbldsvrWindow, NORENDER);
        
        wzbldsvrTbox_ServerPort  = ndTBOX_CreateTextArea    (155, 67, 385, 81, "BldServer_Port", 
                                                NDKEY_SETTEXTAREA (20, 1), 0, 
                                                COLOR_BLACK,  COLOR_WHITE,
                                                COLOR_BLACK,  COLOR_GRAY, 
                                                COLOR_BLACK,  COLOR_GRAY02,
                                                0, &cbtbox_ServerAddrPort, 0, 0, 
                                                wzbldsvrWindow, NORENDER);
        
        ndTBOX_ChangeTextAreaContent (wzbldsvrTbox_ServerPort, wzbldsvrWindow, &wzbldsvr_StrPort, NORENDER);
        
        
        // Password per il server
        
        ndWS_GrWriteLn (10, 90, "Password", COLOR_BLACK, COLOR_GRAY09, wzbldsvrWindow, NORENDER);
        
        wzbldsvrTbox_ServerPasswd  = ndTBOX_CreateTextArea    (155, 87, 385, 101, "BldServer_Passwd", 
                                                NDKEY_SETTEXTAREA (32, 1), 0, 
                                                COLOR_BLACK,  COLOR_WHITE,
                                                COLOR_BLACK,  COLOR_GRAY, 
                                                COLOR_BLACK,  COLOR_GRAY02,
                                                0, 0, &cbtbox_ServerPasswd, 0, 
                                                wzbldsvrWindow, NORENDER);
        
        ndTBOX_ChangeTextAreaContent (wzbldsvrTbox_ServerPasswd, wzbldsvrWindow, &(SystemOptPacket.Password), NORENDER);
        
        
        // Crea il pulsante di chiusura
        
        wzbldsvrBtnClosed   = ndCTRL_CreateButton (305, WindowData [wzbldsvrWindow].WSWndHeight - 25, 
                                                385, WindowData [wzbldsvrWindow].WSWndHeight - 5,
                                                wzbldsvrWindow, "wzbldsvrClose", "Close", "", 
                                                COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                ND_BUTTON_ROUNDED, 
                                                &cb_wzbldsvr_Abort, 0, NORENDER);
                                             
       
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzbldsvr_CloseWnd, 0, 0, wzbldsvrWindow);
        
        
        
        // Richiama il renderer
        
        XWindowRender (wzbldsvrWindow);                                     
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
 
 
void wzBlindServerOptions (void)
{
    if (!wzbldsvrDialogBoxIsOpen)
    {
       // Impedisci l'avvio di due istanze della stessa dialog-box                         
       wzbldsvrDialogBoxIsOpen=1;     
       
       INTERNAL_wzbldsvr_DrawInterface ();
     
       wzbldsvrYouCanExit=0;
       ndProvideMeTheMouse_Until (&wzbldsvrYouCanExit, ND_IS_EQUAL, 1, 0, 0);
       
       // Ok... adesso provvedi a distruggere la vecchia dialog box di interfaccia e
       // Ripristina la possibilità di gestire la dialog-box
       
       ndLP_DestroyWindow (wzbldsvrWindow);
       wzbldsvrDialogBoxIsOpen=0;
    }
}

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
   OPZIONI AVANZATE DI RETE
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

int wzadvoptWindow;
int wzadvoptBtnClosed;

static int wzadvoptDialogBoxIsOpen = 0;
static int wzadvoptYouCanExit;

int wzadvoptListBox;
int wzadvoptCbox_HttpTunneling;
int wzadvoptCbox_BeVerbose;


/*
   ----------------------------------------------------------------------------
   CODICE INTERFACCIA
   ----------------------------------------------------------------------------
*/ 

static ndint64 cb_wzadvopt_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
   wzadvoptYouCanExit=1;
   return 0;    
}

static ndint64 cb_wzadvopt_Abort (char *StringID, ndint64 InfoField, char WndHandle)
{   
   wzadvoptYouCanExit=1;
   return 0;   
}

static void cbcbox_wzadvopt_HttpTunneling (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
   baOpt_EnableDisableHttpTunneling (ActualValue);
   return; 
}

static void cbcbox_wzadvopt_BeVerbose (int ActualValue, char *StringID, int CBoxHandle, char WndHandle)
{
   baOpt_EnableDisableBeVerbose (ActualValue);
   return;     
}

static void cblbox_wzadvopt_ChooseNetworkProfile (int NrItem, ndint64 LBoxCode, int ListBoxHandle, char WndHandle)
{
    baOpt_ChooseConnectionProfile (NrItem); 
    return;
}

void INTERNAL_wzadvopt_DrawInterface (void)
{
    struct  ndNetworkObject_Type ProbeNetObj;
    
    char    Message0 [32];
    char    Message1 [32];
    
    ndint64 Options;
    int     Counter;
    int     ErrRep;
    
    wzadvoptWindow = ndLP_CreateWindow (20, 15, 430, Windows_MaxScreenY-15, "Advanced option", 
                                       COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY09, COLOR_GRAY09, 0);
                                       
    if (wzadvoptWindow>=0)
    {
        ndLP_MaximizeWindow (wzadvoptWindow);
        
        ndWS_GrWriteLn (30,  5, "Choose the network configuration ", COLOR_BLACK, COLOR_GRAY09, wzadvoptWindow, NORENDER);
        ndWS_GrWriteLn (30, 13, "profile that BA will have to use ", COLOR_BLACK, COLOR_GRAY09, wzadvoptWindow, NORENDER);
        
        // Crea la list-box di gestione
        
        wzadvoptListBox = ndLBOX_CreateListBox (10, 25, 300, 90, 0, &ndLBoxDefaultColor1, wzadvoptWindow, NORENDER);
        
        for (Counter=0; Counter<4; Counter++)
        {
            strcpy (&Message0, "Profile ");
            _itoa (Counter+1, &Message1, 10);
            strcat (&Message0, &Message1);
            
            ndLBOX_AddItemToListBox (&Message0, &cblbox_wzadvopt_ChooseNetworkProfile, 0, 0, wzadvoptListBox, wzadvoptWindow);
        }
        
        // Provvedi ad inibire quelle voci che non sono state definite dall'utente
        // nella configurazione del firmware della PSP
        
        for (Counter=0; Counter<4; Counter++)
        {
            ErrRep=ndHAL_WLAN_GetAccessPointInfo (Counter+1, &ProbeNetObj);
        
            if (ErrRep)
            {
                ndLBOX_InhibitListBoxItem (Counter, wzadvoptListBox, wzadvoptWindow, NORENDER);       
            }
        }
        
        ndLBOX_EvidenceListBoxItem (SystemOptPacket.FwNetworkProfileChoosen, wzadvoptListBox, wzadvoptWindow, NORENDER);      
        
        // Update delle informazioni provienienti dalla ListBox
        
        ndLBOX_UpdateListBox (wzadvoptListBox, wzadvoptWindow, NORENDER);
        
        // Check-box per l'abilitazione Http Tunneling
        
        wzadvoptCbox_HttpTunneling  = ndCBOX_CreateCBox (10, 100, "cboxHttpTunneling", SystemOptPacket.HttpTunneling, 
                                       0, &(SystemOptPacket.HttpTunneling), COLOR_WHITE, 
                                       &cbcbox_wzadvopt_HttpTunneling, wzadvoptWindow, NORENDER);
        
        ndWS_GrWriteLn (30, 103, "Enable HTTP tunneling", COLOR_BLACK, COLOR_GRAY09, wzadvoptWindow, NORENDER);
        
        // Check-box per l'abilitazione dell'opzione BeVerbose
        
        wzadvoptCbox_BeVerbose      = ndCBOX_CreateCBox (10, 115, "cboxBeVerbose", SystemOptPacket.BeVerbose, 
                                       0, &(SystemOptPacket.BeVerbose), COLOR_WHITE, 
                                       &cbcbox_wzadvopt_BeVerbose, wzadvoptWindow, NORENDER);
        
        ndWS_GrWriteLn (30, 118, "Be verbose", COLOR_BLACK, COLOR_GRAY09, wzadvoptWindow, NORENDER);
        
        // Crea il pulsante di chiusura
        
        wzadvoptBtnClosed   = ndCTRL_CreateButton (305, WindowData [wzadvoptWindow].WSWndHeight - 25, 
                                                385, WindowData [wzadvoptWindow].WSWndHeight - 5,
                                                wzadvoptWindow, "wzadvoptClose", "Close", "", 
                                                COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                                ND_BUTTON_ROUNDED, 
                                                &cb_wzadvopt_Abort, 0, NORENDER);
                                             
       
        // Stabilisci la close window callback
        
        ndLP_SetupCloseWndCallback (&cb_wzadvopt_CloseWnd, 0, 0, wzadvoptWindow);
        
        // Richiama il renderer
        
        XWindowRender (wzadvoptWindow);                                     
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
 
void wzAdvancedOptions (void)
{
    if (!wzadvoptDialogBoxIsOpen)
    {
       // Impedisci l'avvio di due istanze della stessa dialog-box                         
       wzadvoptDialogBoxIsOpen=1;     
       
       INTERNAL_wzadvopt_DrawInterface ();
     
       wzadvoptYouCanExit=0;
       ndProvideMeTheMouse_Until (&wzadvoptYouCanExit, ND_IS_EQUAL, 1, 0, 0);
       
       // Ok... adesso provvedi a distruggere la vecchia dialog box di interfaccia e
       // Ripristina la possibilità di gestire la dialog-box
       
       ndLP_DestroyWindow (wzadvoptWindow);
       wzadvoptDialogBoxIsOpen=0;
    }
} 

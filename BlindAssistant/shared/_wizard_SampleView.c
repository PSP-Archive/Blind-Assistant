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
   WIZARD PER IL VIEW SAMPLE
   ----------------------------------------------------------------------------
*/





        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        #undef  ND_NOT_MAIN_SOURCE
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        #include <pspusbcam.h>
        
        #include "BlindAssistant.h"
 
 
/*
   ----------------------------------------------------------------------------
   VARIABILI 
   ----------------------------------------------------------------------------
*/ 

int  wzvwsample_Window;
int  wzvwsample_BtnTraining;
int  wzvwsample_BtnDelete;
int  wzvwsample_BtnExit;
int  wzvwsample_ButtonOk;

int  wzvwsample_ExitReason;
char wzvwsample_IsAlreadyOpened = 0;


/*
   ----------------------------------------------------------------------------
   SIMBOLI 
   ----------------------------------------------------------------------------
*/ 

#define WZSAMPLE_REPEAT   2
#define WZSAMPLE_REQABORT 1


/*
   ----------------------------------------------------------------------------
   INIZIALIZZAZIONE/DEINIZIALIZZAZIONE SISTEMA VIDEO
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static void INTERNAL_wzvwsampleInitGoCamSystem () 
{
   // Inibisci l'engine video principale. Questo eviterà conflitti.
   baInhibitVideoEngine ();
    
   // Provvedi a settare la corretta risoluzione del sensore video
   cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_HW_RESOLUTION, ND_RESOLUTION_CODE_640_480);
    
   // Provvedi a settare la modalità video del sensore
   cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_SENSOR_MODE, PSP_USBCAM_EFFECTMODE_NORMAL);
   
   // Provvedi a settare la risoluzione video software della videocamera: l'acquisizione
   // avviene ad una risoluzione massima. Per risparmiare cicli macchina, si invierà una
   // immagine scalata al face detector
   baChangeCameraResolution (STD_FACEREC__CAMERAIMG__DIMX, STD_FACEREC__CAMERAIMG__DIMY, 8);                      
}

static void INTERNAL_wzvwsampleEND_DeInitGoCamSystem () 
{
   // Al riavvio del sistema, non è più necessario (come invece era in BA CFW0005) reinizializzare
   // la webcam con l'opzione Force, perchè abbiamo imposto che la visione dei campioni
   // facciali e l'eventuale training avvenga solo ed esclusivamente mentre la 
   // modalità face recognizer è operativa
   
   baDeInhibitVideoEngine_NoForce ();    
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE INIZIALE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wzvwsample_Delete (char *StringID, ndint64 InfoField, char WndHandle);
static ndint64 cb_wzvwsample_Training (char *StringID, ndint64 InfoField, char WndHandle);
static ndint64 cb_wzvwsample_Exit (char *StringID, ndint64 InfoField, char WndHandle);
static ndint64 cb_wzvwsample_CloseWnd (char WndHandle, ndint64 WndInfoField);

int INTERNAL_wzvwsampleDrawInterface (int Slot)
{
    wzvwsample_Window = ndLP_CreateWindow (5, 5, 415, Windows_MaxScreenY-15, "Face recognizer slot", 
                                       COLOR_WHITE, COLOR_BLUE, COLOR_GRAY06, COLOR_GRAY, 0);
                                       
    if (wzvwsample_Window>=0)
    {
        ndLP_MaximizeWindow (wzvwsample_Window);
        
        wzvwsample_BtnDelete    = ndCTRL_CreateButton (290, WindowData [wzvwsample_Window].WSWndHeight - 75, 
                                               385, WindowData [wzvwsample_Window].WSWndHeight - 55,
                                               wzvwsample_Window, "wzwsampleDelete", "Delete", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                               &cb_wzvwsample_Delete, Slot, NORENDER);
        
        wzvwsample_BtnTraining  = ndCTRL_CreateButton (290, WindowData [wzvwsample_Window].WSWndHeight - 50, 
                                               385, WindowData [wzvwsample_Window].WSWndHeight - 30,
                                               wzvwsample_Window, "wzwsampleTrain", "Training", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                               &cb_wzvwsample_Training, Slot, NORENDER);
        
        wzvwsample_BtnExit      = ndCTRL_CreateButton (290, WindowData [wzvwsample_Window].WSWndHeight - 25, 
                                               385, WindowData [wzvwsample_Window].WSWndHeight - 5,
                                               wzvwsample_Window, "wzwsampleExit", "Exit", "", 
                                               COLOR_BLACK, COLOR_GRAY, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED | ND_CALLBACK_IN_NEW_CONTEXT, 
                                               &cb_wzvwsample_Exit, Slot, NORENDER);
                                             
        // Indica lo slot interessato
        ndWS_GrPrintLn (340, 5, COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NDKEY_SETFONT(3), "%d ", Slot+1);
        
        // Setup della callback di uscita finestra
        ndLP_SetupCloseWndCallback (&cb_wzvwsample_Exit, 0, 0, wzvwsample_Window);
        
        XWindowRender (wzvwsample_Window);                                     
        return 0;
    }
    else
        return -1;
}  

#endif

/*
   ----------------------------------------------------------------------------
   CODICE PER ERRORE DI MODALITA'
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wzvwsample_Ok (char *StringID, ndint64 InfoField, char WndHandle)
{
   wzvwsample_ExitReason=1;
   return 0;   
}


void INTERNAL_wzwvsampleDrawInterfaceForModeMismatch (int Slot)
{ 
    wzvwsample_Window = ndLP_CreateWindow (5, 5, 415, Windows_MaxScreenY-15, "Face recognizer slot", 
                                           COLOR_WHITE, COLOR_BLUE, COLOR_GRAY06, COLOR_GRAY06, 0);
                                       
    if (wzvwsample_Window>=0)
    {
        ndLP_MaximizeWindow (wzvwsample_Window);
        
        ndWS_GrWriteLn (5, 10, "You are not in face recognition mode.  ", COLOR_BLUE,  COLOR_GRAY06, wzvwsample_Window, NORENDER);
        
        ndWS_GrWriteLn (5, 26, "This function can work only when the   ", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
        ndWS_GrWriteLn (5, 34, "face recognition mode is operative.    ", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
        ndWS_GrWriteLn (5, 42, "You can switch to this mode going into ", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
        ndWS_GrWriteLn (5, 50, "the menu Fnc, Enable face recognizer   ", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    
        ndWS_GrWriteLn (5, 66, "Now click on Continue and after switching", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
        ndWS_GrWriteLn (5, 74, "retry to select this function.           ", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    
        
        wzvwsample_ButtonOk   = ndCTRL_CreateButton  (310, WindowData [wzvwsample_Window].WSWndHeight - 25, 
                                               385, WindowData [wzvwsample_Window].WSWndHeight - 5,
                                               wzvwsample_Window, "wzvwsample_Ok", "Continue", "", 
                                               COLOR_BLACK, COLOR_GRAY06, COLOR_BLACK, 
                                               ND_BUTTON_ROUNDED, 
                                               &cb_wzvwsample_Ok, 0, NORENDER);
                                             
        // Indica lo slot interessato
        ndWS_GrPrintLn (340, 5, COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NDKEY_SETFONT(3), "%d ", Slot+1);
         
        XWindowRender (wzvwsample_Window);                                     
        return 0;
    }
    else
        return -1;
} 

#endif


/*
   ----------------------------------------------------------------------------
   CALLBACKS GRAFICA PER CANCELLAZIONE DEI DATI
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static void EraseGraphCallback (int NrSlot, int NrSample, int EraseCode, struct OpInfoPacket_Type *InfoPacket)
{
    char MsgString0 [256];
    char MsgString1 [16];
    
    switch (EraseCode)
    {
         case BEGIN_TO_ERASE:
         {
              InfoPacket->WndHandle = ndLP_CreateWindow (70, 77, Windows_MaxScreenX-70, Windows_MaxScreenY-77,
                                                         "Deleting data on disk", COLOR_WHITE, COLOR_BLUE,
                                                         COLOR_GRAY, COLOR_BLACK, 0);
              
              ndLP_MaximizeWindow (InfoPacket->WndHandle);
              
              ndWS_GrWriteLn (10, 10, "Waiting.... I'm deleting the data on", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              ndWS_GrWriteLn (10, 18, "the disk",                             COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              XWindowRender (InfoPacket->WndHandle);
         
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 0, 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              ndDelay (0.5);
              break;    
         } 
         
         case ERASING_MEMORY:
         {
              ndWS_DrawRectangle (10, 10, 300, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Erasing memory informations....     ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 20, 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              break;
         }    
    
         case ERASING_VOICE_SAMPLE:
         {
              ndWS_DrawRectangle (10, 10, 300, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Erasing voice samples....     ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 20+NrSample*(20.0/VOICE_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              break;
         }
         
         case ERASED_VOICE_SAMPLE:
         {
              ndWS_DrawRectangle (10, 10, 300, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Deleted successful....     ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 20+(NrSample+1)*(20.0/VOICE_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              break;
         }
         
         case ERASING_IMAGE_SAMPLE:
         {
              ndWS_DrawRectangle (10, 10, 300, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Erasing image samples....     ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 40+NrSample*(20.0/NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              break;
         }
         
         case ERASED_IMAGE_SAMPLE:
         {
              ndWS_DrawRectangle (10, 10, 300, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Deleted successful....     ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 40+(NrSample+1)*(20.0/NR_SAMPLES_FOR_FACE), 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              break;
         }
         
         case ERASING_NAME:
         {
              ndWS_DrawRectangle (10, 10, 300, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Erasing name file....     ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 60, 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              break;
         }
         
         case ERASED_NAME:
         {
              ndWS_DrawRectangle (10, 10, 300, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Deleted successful....     ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 80, 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              break;
         }
         
         case DONE1:
         {
              ndWS_DrawRectangle (10, 10, 300, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Done     ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 100, 0, 100, 
                                        COLOR_WHITE, COLOR_BLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              ndDelay (1);
              ndLP_DestroyWindow (InfoPacket->WndHandle);
             
              break;
         }
    }
}

#endif


/*
   ----------------------------------------------------------------------------
   CALLBACKS DI PULSANTE (COMUNI AD A E B)
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

static ndint64 cb_wzvwsample_Delete (char *StringID, ndint64 InfoField, char WndHandle)
{
    struct OpInfoPacket_Type InfoPacket;
    
    int Counter;
    int Slot = InfoField;               // Recupera il numero di slot
    
    baErase_DeleteDataRelativeToASlot (Slot, &EraseGraphCallback, &InfoPacket);
    
    wzvwsample_ExitReason=WZSAMPLE_REPEAT;          // Indica che è necessario rieseguire il codice che
                                                    // mostra l'immagine nello slot   
    return 0;
}

static ndint64 cb_wzvwsample_Training (char *StringID, ndint64 InfoField, char WndHandle)
{
    int Slot = InfoField;                   // Recupera il numero di slot
    wzFaceTraining (Slot);                  // Avvia il training di gestione dello slot
    
    wzvwsample_ExitReason=WZSAMPLE_REPEAT;  // Indica che è necessario rieseguire il codice che
                                            // mostra l'immagine nello slot                          
    return 0;   
}

static ndint64 cb_wzvwsample_Exit (char *StringID, ndint64 InfoField, char WndHandle)
{
    wzvwsample_ExitReason=WZSAMPLE_REQABORT;        // Indica la necessità di uscire dal sistema 
    return 0;               
}

static ndint64 cb_wzvwsample_CloseWnd (char WndHandle, ndint64 WndInfoField)
{
    wzvwsample_ExitReason=WZSAMPLE_REQABORT;        // Indica la necessità di uscire dal sistema 
    return 0;    
}

#endif

/*
   ----------------------------------------------------------------------------
   CODICE STEP 0 (A)
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

void INTERNAL_wzvwsample_ShowTheContentOfTheSlot (int Slot)
{
    unsigned short int PosX, PosY; 
    char InternalBuffer [45];
    
    int CounterX, CounterY, NrElement;
    int PosX1, PosY1, PosX2, PosY2;
    
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 5, 285, 205, COLOR_GRAY06, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_DeInhibitButton   (wzvwsample_BtnDelete,    wzvwsample_Window, NORENDER);
    ndCTRL_DeInhibitButton   (wzvwsample_BtnTraining,  wzvwsample_Window, NORENDER); 
    ndCTRL_DeInhibitButton   (wzvwsample_BtnExit,      wzvwsample_Window, NORENDER); 
    
    // Mostra un messaggio
    ndWS_GrWriteLn (5,  5, "In this slot there is an image of", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    
    // Rettangolo di contenimento del nome
    ndWS_DrawRectangle ( 5, 21, 280, 33, COLOR_GRAY, COLOR_WHITE, wzvwsample_Window, NORENDER);
    
    ndStrCpy (InternalBuffer, facerec_ImagesInRam [Slot].Name, 33, 0);
    PosX = 5+(275-8*strlen (InternalBuffer))/2;
    PosY = 23;
    ndWS_GrWriteLn (PosX, PosY, InternalBuffer, COLOR_BLACK, COLOR_GRAY, wzvwsample_Window, NORENDER);
    
    for (CounterY=0; CounterY<=2; CounterY++)
    {
        for (CounterX=0; CounterX<=2; CounterX++)
        {
            PosX1 =  7 + 90*CounterX;
            PosY1 = 35 + 56*CounterY;
            PosX2 =  7 + 90*(CounterX+1);
            PosY2 = 35 + 56*(CounterY+1);
            
            NrElement = 3*CounterY + CounterX;
            
            if (NrElement!=4)
            {
                // Crea gli otto rettangoli che conterranno gli otto sample...
                ndWS_DrawRectangle (PosX1, PosY1, PosX2, PosY2, COLOR_WHITE, COLOR_BLACK, wzvwsample_Window, NORENDER);
            
                // Necessario per impedire che l'immagine sia visualizzata nella sezione centrale
                if (NrElement>4) NrElement=NrElement-1; 
                
                // ... e poni al suo interno le immagini
                zcvShowImage8bit (facerec_ImagesInRam [Slot].Img [NrElement], PosX1+1, PosY1+1, PosX2-1, PosY2-1,  wzvwsample_Window, NORENDER);  
            }
        }
    }
                                        
    // Esegui la visualizzazione        
    XWindowRender (wzvwsample_Window);
}

#endif


/*
   ----------------------------------------------------------------------------
   CODICE STEP 0 (B)
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

void INTERNAL_wzvwsample_ThereIsNoContentInTheSlot (int Slot)
{
    // Cancella il vecchio contenuto della finestra
    ndWS_DrawRectangle (5, 5, 285, 205, COLOR_GRAY06, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    
    // Setta la condizione di inibizione/deinibizione ai valori necessari
    
    ndCTRL_InhibitButton     (wzvwsample_BtnDelete,    wzvwsample_Window, NORENDER);
    ndCTRL_DeInhibitButton   (wzvwsample_BtnTraining,  wzvwsample_Window, NORENDER); 
    ndCTRL_DeInhibitButton   (wzvwsample_BtnExit,      wzvwsample_Window, NORENDER); 
     
    // Segnala all'utente che lo slot è vuoto
    
    ndWS_GrWriteLn (5,  5, "There is no image stored in this", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    ndWS_GrWriteLn (5, 13, "slot. Click on Training button  ", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    ndWS_GrWriteLn (5, 21, "if you want to store an image in", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    ndWS_GrWriteLn (5, 29, "this slot                       ", COLOR_BLACK, COLOR_GRAY06, wzvwsample_Window, NORENDER);
    
    XWindowRender (wzvwsample_Window);
}  

#endif 

/*
   ----------------------------------------------------------------------------
   CODICE FINALE
   ----------------------------------------------------------------------------
*/ 


#ifndef PSP1000_VERSION

void INTERNAL_wzvwsampleEND_DestroyInterface (void)
{
     ndLP_DestroyWindow (wzvwsample_Window);
} 

#endif 


/*
   ----------------------------------------------------------------------------
   ROUTINE PRINCIPALE
   ----------------------------------------------------------------------------
*/ 


int wzViewSample (int Slot)
/*
     Questa routine avvia il viewer che consente di vedere il contenuto
     attuale di uno slot ed eventualmente di fare partire il wizard per
     il training
*/
{
    #ifndef PSP1000_VERSION
    
    int ErrRep;
    
    if ( (!wzvwsample_IsAlreadyOpened) && (!wzft_IsAlreadyOpened) && (!wzpl_IsAlreadyOpened) && (!wztest_IsAlreadyOpened) )
    {
        sceKernelChangeThreadPriority (sceKernelGetThreadId (), baMainThreadPriority);   // Dai la stessa priorità del main thread                    
        
        if (baMode==BA_MODE_FACEREC)
        {
                        // Disegna l'interfaccia grafica
                    
                        ErrRep=INTERNAL_wzvwsampleDrawInterface (Slot);
                        if (ErrRep) goto wzvwsample_ErrorInGrInterf;          // Problemi nella creazione dell'interfaccia grafica 
                    
                    wzvwsample_step0:
                                     
                        INTERNAL_wzvwsampleInitGoCamSystem ();            // Reinizializza la GoCam
                        wzvwsample_IsAlreadyOpened=1; 
                    
                    wzvwsample_show:
                        
                        // Esegui il lavoro
                                                    
                        if (facerec_ImagesInRam [Slot].IsActive)
                             INTERNAL_wzvwsample_ShowTheContentOfTheSlot (Slot);
                        else
                             INTERNAL_wzvwsample_ThereIsNoContentInTheSlot (Slot);    
                        
                        // Esegui il lock e fai apparire il mouse sullo schermo
                             
                        wzvwsample_ExitReason=0;
                        ndProvideMeTheMouse_Until (&wzvwsample_ExitReason, ND_IS_GREATER, 0, 0, 0); 
                        
                    wzvwsample_decidefuture:     
                        if (wzvwsample_ExitReason==WZSAMPLE_REPEAT) goto  wzvwsample_show;  
                        
                    wzvwsample_AllIsOK:  
                        INTERNAL_wzvwsampleEND_DeInitGoCamSystem (); 
                        INTERNAL_wzvwsampleEND_DestroyInterface ();
                        
                        wzvwsample_IsAlreadyOpened=0;         
                        return 0;
                    
                    wzvwsample_ErrorInGrInterf:
                                               
                        wzvwsample_IsAlreadyOpened=0;                        
                        return BAERR_TROUBLE_WITH_GRAPH;
        }
        else     // Il sistema non è in modalità face recognizer
        {
                        INTERNAL_wzwvsampleDrawInterfaceForModeMismatch (Slot);
                        wzvwsample_IsAlreadyOpened=1; 
                        
                        // Esegui il lock e fai apparire il mouse sullo schermo in modo che
                        // l'utente possa premere il tasto Continue
                             
                        wzvwsample_ExitReason=0;
                        ndProvideMeTheMouse_Until (&wzvwsample_ExitReason, ND_IS_GREATER, 0, 0, 0); 
                        
                        // Ok, pulsante premuto. Cancella la dialog-box ed esci
                        
                        INTERNAL_wzvwsampleEND_DestroyInterface ();
                        
                        wzvwsample_IsAlreadyOpened=0;         
                        return 0;
        }
        
    }
    else
      return 0;    // Simula successo. In realtà non fa nulla perchè il wizard è già aperto    
      
    #else
    
    return 0;      // Non supportato sotto PSP-1000
    
    #endif
}
 
 
void HALT_wzViewSample (void)
{
    #ifndef PSP1000_VERSION
    
    if (wzvwsample_IsAlreadyOpened) 
    {
        wzftYouCanExit=1;
        wzftExitReason=-1;
    }                             
    
    #endif
} 

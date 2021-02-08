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
   ENGINE VIDEO PRINCIPALE DEL PROGRAMMA
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



int   vdengThreadID;
int   vdengYouCanExit;
int   vdengIsInhibited;


/* 
   ----------------------------------------------------------------------------
   CONTROLLO DEL MAIN VIDEO ENGINE 
   ----------------------------------------------------------------------------
*/


void baCreateVideoEngineThread (void)
{ 
    // Necessario stack di almeno 300kb per consentire esecuzione ndTesseract
    
    vdengThreadID = sceKernelCreateThread ("baVideoEngine", &baVideoEngine, baMainThreadPriority, 300000, 
                                           PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU, 0);                                               
}

void baReStartVideoEngine (void)
{
    sceKernelStartThread (vdengThreadID, 0, 0);   
}

void baInhibitVideoEngine (void)
{
    vdengIsInhibited=1;
    ndHAL_Delay (1);
}         

void baDeInhibitVideoEngine (void)
{
    vdengIsInhibited=0;
    sceKernelWakeupThread (vdengThreadID);        // Riattiva il thread del video-engine
    
    baForce=1;                                    // Reinizializzerà la webcam
    ndHAL_Delay (1);
}

void baDeInhibitVideoEngine_NoForce (void)
{
    vdengIsInhibited=0;
    sceKernelWakeupThread (vdengThreadID);        // Riattiva il thread del video-engine
    
    ndHAL_Delay (1);
}

void baExitVideoEngine (void)
{
    vdengYouCanExit=1;
    sceKernelDelayThread (1000000);   // Attende il tempo affinchè il thread del VideoEngine principale possa concludersi   
}


/* 
   ----------------------------------------------------------------------------
   FUNZIONI DISPONIBILI
   ----------------------------------------------------------------------------
*/

char baIsFunctionAvailable (int baMode)
{
     #ifndef PSP1000_VERSION
     
         switch (baMode)
         {
                case BA_MODE_NOWORK:
                case BA_MODE_FACEREC:
                case BA_MODE_POSREC:
                case BA_MODE_OCR:
                case BA_MODE_COLORSCANNER:
                case BA_MODE_MAILREADER:
                case BA_MODE_DATAMATRIX:
                     return 1;
                     
                default:
                     return 0;   
         }
     
     #else
     
         switch (baMode)    // PSP-1000
         {
                case BA_MODE_NOWORK:
                case BA_MODE_POSREC:
                case BA_MODE_OCR:
                case BA_MODE_COLORSCANNER:
                case BA_MODE_DATAMATRIX:
                     return 1;
                
                case BA_MODE_FACEREC:
                case BA_MODE_MAILREADER:
                     return 0;
                     
                default:
                     return 0;   
         }
     
     #endif
}


/* 
   ----------------------------------------------------------------------------
   SWITCH VELOCE TRA LE FUNZIONI
   ----------------------------------------------------------------------------
*/

static void baGoToThePreviousMode ()
{
       ndHAL_SND_Beep (1, 12);
       
       do
       {
           baMode--;
           if (baMode<0) baMode=BA_NR_MODES-1;
       }
       while (!baIsFunctionAvailable (baMode));
}

static void baGoToTheFollowingMode ()
{
       ndHAL_SND_Beep (1, 12);
       
       do
       {   
           baMode++;
           if (baMode==BA_NR_MODES) baMode=0;
       }
       while (!baIsFunctionAvailable (baMode));    
}



/* 
   ----------------------------------------------------------------------------
   MAIN VIDEO ENGINE 
   ----------------------------------------------------------------------------
*/


int baVideoEngine (SceSize args, void *argp)
{
    // Destinato ad accogliere i dati sulla pressione del pad
    struct ndDataControllerType ndPadRecord;
    
    int  StartPressed, SquarePressed, TrianglePressed, RTriggerPressed, LTriggerPressed;
    int  ModeIsChanged;
    
    char StrCommand [256];
                                                     
    // Flag di inibizione a 0
    vdengYouCanExit  =0;
    vdengIsInhibited =0;
    
    // Reinizializza la modalità di funzionamento del software
    baMode               = BA_MODE_NOWORK;
    baPrevMode           = -1;
    baManualSwitchToMode = -1;
    
    // Dai la stessa priorità del main thread
    sceKernelChangeThreadPriority (sceKernelGetThreadId (), baMainThreadPriority); 
    
    while (!vdengYouCanExit)
    {
        if (!vdengIsInhibited)
        {            
                ndHAL_GetPad (&ndPadRecord);
                
                // HACK: Un codice spurio del tipo 0x08xxxxxx viene generato in alcuni
                // firmware ogni tanto. Deve essere ignorato
                if (ndPadRecord.Buttons & 0xFF000000) continue;
                
                // Verifichiamo i tasti premuti
                StartPressed    = (ndPadRecord.Buttons & ND_BUTTON6);
                SquarePressed   = (ndPadRecord.Buttons & ND_BUTTON2);
                TrianglePressed = (ndPadRecord.Buttons & ND_BUTTON3);
                
                RTriggerPressed = (ndPadRecord.Buttons & ND_BUTTON_AUXR);
                LTriggerPressed = (ndPadRecord.Buttons & ND_BUTTON_AUXL);
                
                // Verifica se LTrigger o RTrigger sono premuti per switch manuale
                
                if (LTriggerPressed) baGoToThePreviousMode ();
                if (RTriggerPressed) baGoToTheFollowingMode ();
                
                // Inizia lo switch
                
                if (baManualSwitchToMode<0)
                {
                    // Se è stato premuto il tasto SQUARE attiva il sistema di
                    // controllo vocale
                    
                    if (SquarePressed)
                    {
                        baVoiceRec_AcquireCommand (StrCommand, 256);
                        baVoiceRec_DecodeCommand (StrCommand);              
                    }
                }
                else
                {
                    baMode               = baManualSwitchToMode;
                    baManualSwitchToMode = -1;
                }
                
                // Verifica se una modalità video è stata cambiata
                
                ModeIsChanged = ((baMode!=baPrevMode) || (baForce));
                
                if (ModeIsChanged)
                {
                    // Esegue il distruttore per la precedente modalità che provvederà a pulire la memoria
                    // del dispositivo dai dati delle precedenti modalità                
                
                    switch (baPrevMode)
                    {
                           case BA_MODE_NOWORK:
                           {
                                videng_NULLMODE_Destructor ();
                                break;
                           }
                           
                           case BA_MODE_FACEREC:
                           {
                                videng_FACEREC_Destructor ();
                                break;
                           }
                           
                           case BA_MODE_POSREC:
                           {
                                videng_POSREC_Destructor ();
                                break;
                           }  
                           
                           case BA_MODE_OCR:
                           {
                                videng_OCR_Destructor ();
                                break;
                           }
                           
                           case BA_MODE_COLORSCANNER:
                           {
                                videng_COLORSCANNER_Destructor ();
                                break;
                           }
                           
                           case BA_MODE_MAILREADER:
                           {
                                videng_MAILREADER_Destructor ();
                                break;
                           }   
                           
                           case BA_MODE_DATAMATRIX:
                           {
                                videng_DATAMATRIX_Destructor ();
                                break;
                           }    
                    }
                
                    // Esegui un costruttore per la nuova modalità che allocherà le aree di memoria
                    // necessarie e caricherà i dati dal disco
                    
                    switch (baMode)
                    {
                           case BA_MODE_NOWORK:
                           {
                                videng_NULLMODE_Constructor ();
                                break;
                           }
                           
                           case BA_MODE_FACEREC:
                           {
                                videng_FACEREC_Constructor ();
                                break;
                           } 
                           
                           case BA_MODE_POSREC:
                           {
                                videng_POSREC_Constructor ();
                                break;
                           }
                           
                           case BA_MODE_OCR:
                           {
                                videng_OCR_Constructor ();
                                break;
                           }
                           
                           case BA_MODE_COLORSCANNER:
                           {
                                videng_COLORSCANNER_Constructor ();
                                break;
                           }
                           
                           case BA_MODE_MAILREADER:
                           {
                                videng_MAILREADER_Constructor ();
                                break;
                           }   
                           
                           case BA_MODE_DATAMATRIX:
                           {
                                videng_DATAMATRIX_Constructor ();
                                break;
                           }     
                    }
                }    
                    
                // Esegui il nucleo video per la funzione selezionata
                
                switch (baMode)
                {
                       case BA_MODE_NOWORK:
                       {
                            videng_NULLMODE_Core ();
                            break;
                       }
                       
                       case BA_MODE_FACEREC:
                       {
                            videng_FACEREC_Core ();
                            break;
                       }
                       
                       case BA_MODE_POSREC:
                       {
                            videng_POSREC_Core (TrianglePressed);
                            break;
                       }
                       
                       case BA_MODE_OCR:
                       {
                            videng_OCR_Core (TrianglePressed);
                            break;
                       }
                       
                       case BA_MODE_COLORSCANNER:
                       {
                            videng_COLORSCANNER_Core (TrianglePressed);
                            break;
                       }
                       
                       case BA_MODE_MAILREADER:
                       {
                            videng_MAILREADER_Core (TrianglePressed);
                            break;
                       }
                       
                       case BA_MODE_DATAMATRIX:
                       {
                            videng_DATAMATRIX_Core (TrianglePressed);
                            break;
                       }       
                }
                
                // Se il tasto START è premuto, si avvia il sistema ora esatta
                if (StartPressed) videng_SayDayTime();
                
                // Registra la vecchia modalità
                baPrevMode =baMode;
                
                // Resetta il flag baForce
                baForce    =0;        
        }
        else   // Il thread è inibito
        {
                // Sospendi il thread: sarà riattivato in seguito dall'esterno
                sceKernelSleepThread ();
        }
    }
    
    return 0;
}












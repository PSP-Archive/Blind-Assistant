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
   ROUTINES DI GESTIONE DEL SISTEMA DI RICONOSCIMENTO VOCALE
   ----------------------------------------------------------------------------
*/



#define ND_NOT_MAIN_SOURCE  
        #include <nanodesktop.h>
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        
        #include "BlindAssistant.h"
        #include "version.h"
        
        #ifndef PSP1000_VERSION
                #include <ndsphinx_api.h>
        #endif

/*
    ---------------------------------------------------------------------------
    VARIABILI 
    ---------------------------------------------------------------------------
*/

char baVoiceRec_WndHandle;
char baVoiceRec_HelpWndHandle;



/*
    ---------------------------------------------------------------------------
    CONTROLLO CONGRUENZA PACCHETTO SPHINX
    ---------------------------------------------------------------------------
*/

char ndPSphinx_ThePackageIsOk()
{
     #ifndef PSP1000_VERSION
     
     /* All'avvio Blind Assistant esegue un controllo dei file di PocketSphinx,
        allo scopo di assicurarsi che essi siano nella versione corretta e
        che siano tutti presenti. Se non è così, verrà avviata una procedura
        che aggiornerà/correggerà i file scaricandoli da internet */
        
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.corpus")!=183) return 0;
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.dic")!=355) return 0;
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.lm")!=2145) return 0;
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.lm.DMP")!=2215) return 0;
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.sent")!=260) return 0;
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.sent.arpabo")!=2145) return 0;
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.token")!=136) return 0;
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.txt")!=181) return 0;
        if (flen ("/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.vocab")!=104) return 0;
        
     #endif
        
        return 1;
}


/*
    ---------------------------------------------------------------------------
    HELP DEL SISTEMA DI RICONOSCIMENTO VOCALE
    ---------------------------------------------------------------------------
*/

void baVoiceRec_Help ()
{
   #ifndef PSP1000_VERSION
   
           baVoiceRec_HelpWndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 300, Windows_MaxScreenY - 120, Windows_MaxScreenX - 30, Windows_MaxScreenY - 30,
                                                       "List of voice commands", COLOR_WHITE, COLOR_BLUE, COLOR_GRAY11, COLOR_GRAY08, 0); 
                                    
           if (baVoiceRec_HelpWndHandle>=0)
           {
                 ndLP_MaximizeWindow (baVoiceRec_HelpWndHandle);
           
                 ndWS_GrWriteLn (5, 15, "Please, press the SQUARE ", COLOR_GRAY04, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);             
                 ndWS_GrWriteLn (5, 25, "button and say your command ", COLOR_GRAY04, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);             
                 ndWS_GrWriteLn (5, 35, "for this system", COLOR_GRAY04, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER);             
                 baaud_PressSquareAndSayTheCmd ();
                 ndDelay (8);
                 
                 ndWS_DrawRectangle (5, 15, 255, 45, COLOR_GRAY11, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);      
                 ndWS_GrWriteLn (5, 15, "Available commands are:", COLOR_GRAY04, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER);             
                 baaud_AvailableCommandsAre ();
                 ndDelay (8);
                 
                 ndWS_DrawRectangle (5, 25, 255, 45, COLOR_GRAY11, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);      
                 ndWS_GrWriteLn (5, 35, "Enable no function mode", COLOR_GRAY16, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER | NDKEY_SETFONT (2) );             
                 baaud_EnableNoFunctionMode ();
                 ndDelay (8);
            
                 ndWS_DrawRectangle (5, 25, 255, 45, COLOR_GRAY11, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);      
                 ndWS_GrWriteLn (5, 35, "Enable face recognizer", COLOR_GRAY16, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER | NDKEY_SETFONT (2) );             
                 baaud_EnableFaceRecognitionSystem ();
                 ndDelay (8);
            
                 ndWS_DrawRectangle (5, 25, 255, 45, COLOR_GRAY11, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);      
                 ndWS_GrWriteLn (5, 35, "Enable position recognizer", COLOR_GRAY16, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER | NDKEY_SETFONT (2) );             
                 baaud_EnablePosRecognitionSystem ();
                 ndDelay (8);
                 
                 ndWS_DrawRectangle (5, 25, 255, 45, COLOR_GRAY11, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);      
                 ndWS_GrWriteLn (5, 35, "Enable optical char recognizer", COLOR_GRAY16, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER | NDKEY_SETFONT (2) );                  
                 baaud_EnableOpticalCharRecognition ();
                 ndDelay (8);
                 
                 ndWS_DrawRectangle (5, 25, 255, 45, COLOR_GRAY11, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);      
                 ndWS_GrWriteLn (5, 35, "Enable color scanner", COLOR_GRAY16, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER | NDKEY_SETFONT (2) );                      
                 baaud_EnableColorScanner ();
                 ndDelay (8);
            
                 ndWS_DrawRectangle (5, 25, 255, 45, COLOR_GRAY11, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);      
                 ndWS_GrWriteLn (5, 35, "Enable mail reader", COLOR_GRAY16, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER | NDKEY_SETFONT (2) );                      
                 baaud_EnableMailReader ();
                 ndDelay (8);
                 
                 ndWS_DrawRectangle (5, 25, 255, 45, COLOR_GRAY11, COLOR_GRAY11, baVoiceRec_HelpWndHandle, NORENDER);      
                 ndWS_GrWriteLn (5, 35, "Enable data matrix scanner", COLOR_GRAY16, COLOR_GRAY11, baVoiceRec_HelpWndHandle, RENDER | NDKEY_SETFONT (2) );                      
                 baaud_EnableDataMatrixScanner ();
                 ndDelay (8);
                 
                 ndLP_DestroyWindow (baVoiceRec_HelpWndHandle); 
            }
            
   #endif
} 

/*
    ---------------------------------------------------------------------------
    ACQUISIZIONE COMANDO VOCALE: CALLBACKS DI GESTIONE
    ---------------------------------------------------------------------------
*/


#ifndef PSP1000_VERSION

static void INTERNAL_baVoiceRec_AcquireCmd_Listen ()
{
     ndHAL_SND_Beep (2, 2);
     ndHAL_Delay (0.20);
     
     ndWS_DrawRectangle (5, 15, 255, 25, COLOR_GRAY14, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
     ndWS_GrWriteLn (5, 15, "Listening: ",   COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, RENDER);                  
}

static void INTERNAL_baVoiceRec_AcquireCmd_StopListen ()
{
     ndWS_DrawRectangle (5, 15, 255, 25, COLOR_GRAY14, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
     ndWS_GrWriteLn (5, 15, "Decoding: ",   COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, RENDER);                 
}

static void INTERNAL_baVoiceRec_AcquireCmd_Manager (void *StringBuffer, int StrBufferLen, int Score)
{
     ndWS_DrawRectangle (5, 15, 255, 25, COLOR_GRAY14, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
     ndWS_GrWriteLn (5, 15, "Your command: ",   COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER);  
     ndWS_GrPrintLn (5, 30, COLOR_BLUE,  COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER, "%s", StringBuffer);             
     ndWS_GrPrintLn (5, 45, COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, RENDER, "score: %d", Score);             
}

void baVoiceRec_FreeMyMemoryCB (char *Message, int WndHandle)
{
     ndWS_GrWriteLn (5, 35, Message, COLOR_RED, COLOR_GRAY14, WndHandle, RENDER);  
}

void baVoiceRec_RecoverMyMemoryCB (char *Message, int WndHandle)
{
     ndWS_GrWriteLn (5, 35, Message, COLOR_RED, COLOR_GRAY14, WndHandle, RENDER);  
}

#endif




/*
    ---------------------------------------------------------------------------
    ACQUISIZIONE COMANDO ROUTINE PRINCIPALE
    ---------------------------------------------------------------------------
*/


void baVoiceRec_AcquireCommand (char *StrCommand, int StrCommandLen)
{
   #ifndef PSP1000_VERSION
   
           baVoiceRec_WndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 280, Windows_MaxScreenY - 100, Windows_MaxScreenX - 10, Windows_MaxScreenY - 10,
                                                       "Listening for your command", COLOR_WHITE, COLOR_BLUE, COLOR_GRAY14, COLOR_GRAY14, 0); 
                                    
           if (baVoiceRec_WndHandle>=0)
           {
                 ndLP_MaximizeWindow (baVoiceRec_WndHandle);
                         
                 ndWS_DrawRectangle (5, 15, 255, 55, COLOR_GRAY14, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
                 baaud_ListeningForYourNextCommand ();
                 
                 // E' necessario un ritardo per impedire che il decoder rilevi il
                 // messaggio emesso da BlindAssistant
                 
                 ndHAL_Delay (4);
                 
                 // Ricevi il comando audio
                 
                 ndPSphinx_ExecuteDecoding (&INTERNAL_baVoiceRec_AcquireCmd_Listen, 
                                            &INTERNAL_baVoiceRec_AcquireCmd_StopListen, 
                                            &INTERNAL_baVoiceRec_AcquireCmd_Manager, 
                                            StrCommand, StrCommandLen, 10);
                 
                 ndHAL_Delay (2);
                 ndLP_DestroyWindow (baVoiceRec_WndHandle);        // Disattiva la finestra
           }
           
   #else   // PSP-1000: Non supporta controllo vocale e quindi serve una segnalazione d'errore
   
           baVoiceRec_WndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 280, Windows_MaxScreenY - 100, Windows_MaxScreenX - 10, Windows_MaxScreenY - 10,
                                                       "ERROR: Voice ctrl not supported", COLOR_WHITE, COLOR_RED, COLOR_GRAY14, COLOR_GRAY14, 0); 
                                    
           if (baVoiceRec_WndHandle>=0)
           {
                 ndLP_MaximizeWindow (baVoiceRec_WndHandle);
                 baaud_VoiceRecognitionSystemNotSupported ();
                         
                 ndWS_DrawRectangle (5, 15, 255, 55, COLOR_GRAY14, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
           
                 ndWS_GrWriteLn (5, 15, "The voice control system is not ",   COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, RENDER); 
                 ndWS_GrWriteLn (5, 25, "supported under PSP-1000 (FAT). ",   COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, RENDER); 
                 ndWS_GrWriteLn (5, 35, "Try to use PSP-2000 (SLIM). ",       COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, RENDER); 
                 
                 ndHAL_Delay (2);
                 ndLP_DestroyWindow (baVoiceRec_WndHandle);        // Disattiva la finestra
           }
           
            // Alla routine chiamante viene restituita una stringa vuota
            strcpy (StrCommand, "");
   
   #endif   
}




/*
    ---------------------------------------------------------------------------
    DECODIFICA COMANDO VOCALE
    ---------------------------------------------------------------------------
*/

void baVoiceRec_DecodeCommand (char *StrCommand)
{
    #ifndef PSP1000_VERSION
    
            char SecStrCommand [256];
            
            ndStrCpy (SecStrCommand, StrCommand, strlen ("ENABLE NO FUNCTION MODE"), 1);
            
            if (!strcmp (SecStrCommand, "ENABLE NO FUNCTION MODE"))
            {
                baMode=BA_MODE_NOWORK;
                return;
            }
            
            ndStrCpy (SecStrCommand, StrCommand, strlen ("ENABLE FACE RECOGNIZER"), 1); 
            
            if (!strcmp (SecStrCommand, "ENABLE FACE RECOGNIZER"))
            {
                baMode=BA_MODE_FACEREC;
                return;
            }
            
            ndStrCpy (SecStrCommand, StrCommand, strlen ("ENABLE POSITION RECOGNIZER"), 1); 
            
            if (!strcmp (SecStrCommand, "ENABLE POSITION RECOGNIZER"))
            {
                baMode=BA_MODE_POSREC;
                return;
            }
            
            ndStrCpy (SecStrCommand, StrCommand, strlen ("ENABLE OPTICAL CHAR RECOGNIZER"), 1); 
            
            if (!strcmp (SecStrCommand, "ENABLE OPTICAL CHAR RECOGNIZER"))
            {
                baMode=BA_MODE_OCR;
                return;
            }
            
            ndStrCpy (SecStrCommand, StrCommand, strlen ("ENABLE COLOR SCANNER"), 1); 
            
            if (!strcmp (SecStrCommand, "ENABLE COLOR SCANNER"))
            {
                baMode=BA_MODE_COLORSCANNER;
                return;
            }
            
            ndStrCpy (SecStrCommand, StrCommand, strlen ("ENABLE MAIL READER"), 1); 
            
            if (!strcmp (SecStrCommand, "ENABLE MAIL READER"))
            {
                baMode=BA_MODE_MAILREADER;
                return;
            }
            
            ndStrCpy (SecStrCommand, StrCommand, strlen ("ENABLE DATA MATRIX SCANNER"), 1); 
            
            if (!strcmp (SecStrCommand, "ENABLE DATA MATRIX SCANNER"))
            {
                baMode=BA_MODE_DATAMATRIX;
                return;
            }
            
            // Arrivati a questo punto, il sistema non ha riconosciuto il comando
            baaud_CommandNotRecognized (); 
            
    #else
    
            /* PSP-1000: Nothing to do */
            
    #endif
}




/*
    ---------------------------------------------------------------------------
    RISPOSTE A DOMANDE
    ---------------------------------------------------------------------------
*/


#ifndef PSP1000_VERSION

char baVoiceRec_AnswerYesNo (void *PntToAudioMessage)
{
   char StrCommand    [256];
   char SecStrCommand [256];
   char Answer;
   
   void (*AudioMessage)() = PntToAudioMessage;
     
   baVoiceRec_WndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 280, Windows_MaxScreenY - 100, Windows_MaxScreenX - 10, Windows_MaxScreenY - 10,
                                                   "Answer yes or no", COLOR_WHITE, COLOR_GREEN, COLOR_GRAY14, COLOR_GRAY14, 0); 
                                
   if (baVoiceRec_WndHandle>=0)
   {
         ndLP_MaximizeWindow (baVoiceRec_WndHandle);
   
         while (1)
         {      
             // Cancella la finestra
             
             ndWS_DrawRectangle (5, 15, 255, 55, COLOR_GRAY14, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
             
             // Riproduci il messaggio audio associato alla richiesta
             
             (*AudioMessage)();
             ndHAL_Delay (2);
             
             // Ricevi il comando audio
         
             ndPSphinx_ExecuteDecoding (&INTERNAL_baVoiceRec_AcquireCmd_Listen, 
                                        &INTERNAL_baVoiceRec_AcquireCmd_StopListen, 
                                        &INTERNAL_baVoiceRec_AcquireCmd_Manager, 
                                        StrCommand, 255, 10);
             
             ndHAL_Delay (2);
               
             ndStrCpy (SecStrCommand, StrCommand, strlen ("YES"), 1);
             if (!strcmp (SecStrCommand, "YES")) 
             {
                 Answer=1;
                 break;
             }
           
             ndStrCpy (SecStrCommand, StrCommand, strlen ("NO"), 1);
             if (!strcmp (SecStrCommand, "NO")) 
             {
                 Answer=0;
                 break;
             }
       
             // Se il controllo arriva qui, vuol dire che non è stato risposto nè sì nè no.
             // Provvedi a reiterare la richiesta
       
             baaud_CommandNotRecognized ();
             ndHAL_Delay (2); 
         }
         
         ndLP_DestroyWindow (baVoiceRec_WndHandle);        // Disattiva la finestra
         return Answer;  
   }
   else 
     return -1;
}

#else

static void DrawAMicroButton (int CenterPosX, int CenterPosY, char *Name, TypeColor TextColor, TypeColor MyColor, char RenderNow)
{
    int PosX1 = CenterPosX - 25;
    int PosX2 = CenterPosX + 25;
    int PosY1 = CenterPosY - 15;
    int PosY2 = CenterPosY + 15;
    
    int TextPosX = CenterPosX - (strlen(Name)*8)/2;
    int TextPosY = CenterPosY - 4;
    
    ndWS_DrawSpRectangle (PosX1, PosY1, PosX2, PosY2, MyColor, COLOR_WHITE, ND_ROUNDED, baVoiceRec_WndHandle, NORENDER);    
    ndWS_GrWriteLn (TextPosX, TextPosY, Name, TextColor, MyColor, baVoiceRec_WndHandle, NORENDER);
    
    if (RenderNow) XWindowRender (baVoiceRec_WndHandle);
}

char baVoiceRec_AnswerYesNo (void *PntToAudioMessage)
{        
    int Answer = 0;
    int Btn1_X, Btn2_X, Btn_Y;
    
    void (*AudioMessage)() = PntToAudioMessage;
    
    // Crea la finestra di dialogo
    
    baVoiceRec_WndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 280, Windows_MaxScreenY - 100, Windows_MaxScreenX - 10, Windows_MaxScreenY - 10,
                                                   "Answer yes or no", COLOR_WHITE, COLOR_GREEN, COLOR_GRAY14, COLOR_GRAY14, 0); 
                                
   if (baVoiceRec_WndHandle>=0)
   {
         ndLP_MaximizeWindow (baVoiceRec_WndHandle);
   
         // Riproduci il messaggio audio associato alla richiesta
             
         (*AudioMessage)();
         ndHAL_Delay (2.5);
         
         // Emetti il messaggio vocale che chiede di premere L-TRIGGER o R-TRIGGER
         baaud_PressLTriggerOrRTrigger ();
   
         // Pulisci la finestra
         
         ndWS_DrawRectangle (5, 15, 255, 55, COLOR_GRAY14, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
 
         ndWS_GrWriteLn (5,  5, "Press L-TRIGGER for YES and ",   COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
         ndWS_GrWriteLn (5, 15, "R-TRIGGER for NO            ",   COLOR_BLACK, COLOR_GRAY14, baVoiceRec_WndHandle, NORENDER); 
             
         // Calcola le coordinate del centro dei pulsanti
         Btn1_X = WindowData [baVoiceRec_WndHandle].WSWndLength/2 - 35;
         Btn2_X = WindowData [baVoiceRec_WndHandle].WSWndLength/2 + 35;    
         Btn_Y  = 45;
             
         // Crea i due micropulsanti    
         DrawAMicroButton (Btn1_X, Btn_Y, "Yes", COLOR_WHITE, COLOR_GREEN, NORENDER);
         DrawAMicroButton (Btn2_X, Btn_Y, "No",  COLOR_WHITE, COLOR_GREEN, NORENDER);
 
         XWindowRender ( baVoiceRec_WndHandle );   
         
         // Attendi che venga premuto il tasto L-TRIGGER o R-TRIGGER
         
         while (1)
         {
               struct ndDataControllerType ndPadRecord; 
               ndHAL_GetPad (&ndPadRecord);
               
               if (ndPadRecord.Buttons & ND_BUTTON_AUXL)
               {
                      // Esegui un flash sul primo micropulsante
                      DrawAMicroButton (Btn1_X, Btn_Y, "Yes", COLOR_BLACK, COLOR_WHITE, RENDER);
                      ndHAL_Delay (0.5);
                      DrawAMicroButton (Btn1_X, Btn_Y, "Yes", COLOR_WHITE, COLOR_GREEN, RENDER);
                      ndHAL_Delay (0.5);
                      
                      Answer = 1;
                      break;                  
               }
               
               if (ndPadRecord.Buttons & ND_BUTTON_AUXR)
               {
                      // Esegui un flash sul secondo micropulsante
                      DrawAMicroButton (Btn2_X, Btn_Y, "No", COLOR_BLACK, COLOR_WHITE, RENDER);
                      ndHAL_Delay (0.5);
                      DrawAMicroButton (Btn2_X, Btn_Y, "No", COLOR_WHITE, COLOR_GREEN, RENDER);
                      ndHAL_Delay (0.5);
                      
                      Answer = 0;
                      break;                  
               }
         }
         
         // Cancella la finestra
     
         ndLP_DestroyWindow (baVoiceRec_WndHandle);        // Disattiva la finestra
         return Answer;  
   }
   else 
     return -1;
}


#endif


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
   SISTEMA DI AUTORIPARAZIONE: Condizione #00
   ----------------------------------------------------------------------------
   Questo modulo si occupa di correggere un problema che si verifica in
   caso di perdita di coerenza tra il database dei luoghi memorizzato nel
   BlindServer e quello memorizzato sulla memory stick della PSP. 
   
   Normalmente, sulla memory stick dovrebbe risiedere il nome del luogo
   che viene comunicato dal BlindServer, in modo da evitare di doverlo
   risintetizzare da zero (i file vocali sono nella directory 
   ms0:/BLINDASSISTANT/AUDCACHEFORPLACESMSG). 
   
   Se il file locale, per una qualsiasi ragione, non è presente in cache,
   il sistema reagisce richiamando questa routine. 
   
   Essa provvede a risintetizzare al volo il file vocale mancante in
   modo da ristabilire la coerenza della cache audio sulla memory stick
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

/*
   ----------------------------------------------------------------------------
   VARIABILI
   ----------------------------------------------------------------------------
*/

char autofix00_WndHandle;

/*
   ----------------------------------------------------------------------------
   CODICE PRINCIPALE
   ----------------------------------------------------------------------------
*/

char SYSFIXER00_SyncPlaceAudioCache (char *NameOfTheRoom)
{
   char NameFileWav [256];
   char VoiceMsg    [512];
   
   autofix00_WndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 350, 10, Windows_MaxScreenX - 10, 100,
                                                   "System fixer #00", COLOR_WHITE, COLOR_RED, COLOR_GRAY14, COLOR_GRAY14, 0); 
                                
   if (autofix00_WndHandle>=0)
   {
         ndLP_MaximizeWindow (autofix00_WndHandle);
    
         ndWS_GrWriteLn (5, 15, "Synchronizing the audio cache for ",   COLOR_BLACK, COLOR_GRAY14, autofix00_WndHandle, NORENDER); 
         ndWS_GrWriteLn (5, 25, "position messages with BlindServer",   COLOR_BLACK, COLOR_GRAY14, autofix00_WndHandle, NORENDER); 
         ndWS_GrWriteLn (5, 35, "database. Waiting...              ",   COLOR_BLACK, COLOR_GRAY14, autofix00_WndHandle, NORENDER); 
         XWindowRender (autofix00_WndHandle);
                 
         // Avverti l'utente che stai correggendo un problema        
         baaud_WaitIAmFixingAProblem ();  
         
         // Genera il file audio mancante usando il motore ndFLite. Determina il nome file
         strcpy  (NameFileWav, "ms0:/BLINDASSISTANT/AUDCACHEFORPLACESMSG/"); 
         strcat  (NameFileWav, "$PLACEMSG$");
         strcat  (NameFileWav, NameOfTheRoom );
         strcat  (NameFileWav, ".WAV"); 
         
         // Determina il messaggio
         strcpy  (VoiceMsg, "You are in ");
         strcat  (VoiceMsg, NameOfTheRoom);
     
         // Provvede a sintetizzare l'audio, memorizzandolo sulla memory stick
         flite_text_to_speech (VoiceMsg, VoiceObj, NameFileWav);
              
         ndLP_DestroyWindow (autofix00_WndHandle);
   
         return 0;          // Segnala che il problema è stato risolto
   }
   else
     return -1;
}


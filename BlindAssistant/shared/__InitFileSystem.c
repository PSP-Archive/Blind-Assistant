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
   REINIZIALIZZAZIONE DEL FILE SYSTEM E CARICAMENTO INIZIALE DELLE
   IMMAGINI
   ----------------------------------------------------------------------------
*/




        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        #undef ND_NOT_MAIN_SOURCE
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        
        #include "BlindAssistant.h"
        

/*
   ----------------------------------------------------------------------------
   STRUTTURE PER USO INTERNO
   ----------------------------------------------------------------------------
*/




/*
   ----------------------------------------------------------------------------
   CALLBACK GRAFICA
   ----------------------------------------------------------------------------
*/

static void InitSysGraphCallback (int NrSlot, int InitSysCode, struct OpInfoPacket_Type *InfoPacket)
{
    char MsgString0 [256];
    char MsgString1 [16];
    
    switch (InitSysCode)
    {
           case BEGIN_TO_GENFS:
           {
              InfoPacket->WndHandle = ndLP_CreateWindow (70, 77, Windows_MaxScreenX-70, Windows_MaxScreenY-77,
                                                         "Regenerating File System", COLOR_WHITE, COLOR_BLUE,
                                                         COLOR_GRAY, COLOR_BLACK, 0);
              
              ndLP_MaximizeWindow (InfoPacket->WndHandle);
              
              ndWS_GrWriteLn (10, 10, "Waiting.... I'm regenerating the ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              ndWS_GrWriteLn (10, 18, "structure of the file system     ",   COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              XWindowRender (InfoPacket->WndHandle);
         
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 0, 0, 100, 
                                        COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              ndDelay (0.5);      
              break;
           }
           
           case I_HAVE_DONE_SINGLE_GENFS:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              strcpy (MsgString0, "Creating directory for slot #");
              _itoa (NrSlot, &MsgString1, 10);
              strcat (MsgString0, MsgString1);
              
              ndWS_GrWriteLn (10, 10, &MsgString0, COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 100.0*(float)(NrSlot)/(float)(NR_SLOTS_FACE_RECOGNIZER), 0, 100, 
                                        COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case DONE2:
           {
              ndWS_DrawRectangle (10, 10, 310, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Fs regeneration complete ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 300, 65, 100, 0, 100, 
                                        COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              ndDelay (0.5); 
              ndLP_DestroyWindow (InfoPacket->WndHandle);
              break;
           } 
    }   
}




/*
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/


void InitFileSystem ()
{
    static struct OpInfoPacket_Type InitFsInfoPacket;
    
    baGenFs_GenerateNewDirsOfFS (&InitSysGraphCallback, &InitFsInfoPacket);
}

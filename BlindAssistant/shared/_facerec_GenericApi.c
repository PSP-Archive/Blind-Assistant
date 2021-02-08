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
   FACE RECOGNITION
   ----------------------------------------------------------------------------
   Questa sezione contiene le routines necessarie per il riconoscimento 
   facciale (alto livello)
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
   ---------------------------------------------------------------------------
   INIZIO DEL CODICE
   ---------------------------------------------------------------------------
*/

void baUpdateFaceRecognizerDBase (int NrSlotUpdated)
{
    #ifndef PSP1000_VERSION
        baPCA_UpdateFaceRecognizerDBase (NrSlotUpdated);
    #endif
}

int baUpdateDistanceData (IplImage *Image)
{
    #ifndef PSP1000_VERSION 
        return baPCA_UpdateDistanceData (Image);
    #else
        return 0;
    #endif
}


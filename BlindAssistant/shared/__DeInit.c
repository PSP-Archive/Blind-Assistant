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
   ROUTINES ESEGUITE ALLA CHIUSURA DEL SISTEMA
   ----------------------------------------------------------------------------
*/




        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        
        #include "BlindAssistant.h"
        
     #ifndef PSP1000_VERSION    
        #include <ndsphinx_api.h>
     #endif


void INTERNAL_DisableVoiceRecEngine ()
{
     #ifndef PSP1000_VERSION
        ndPSphinx_TerminateDecoding ();
     #endif
}

void INTERNAL_DisableCamera ()
{
     cvReleaseCapture (&cvCameraCaptureObj);
}

int baDeInit (void)
{
     INTERNAL_DisableVoiceRecEngine (); 
     INTERNAL_DisableCamera ();
     
     return 0;        
} 
 
 

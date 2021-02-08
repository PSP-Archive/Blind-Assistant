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
   ROUTINES DI GESTIONE VIDEO CAMERA
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
 


void baChangeCameraResolution (unsigned short int LenX, unsigned short int LenY, char ColorDepth)
{
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_PROP_FRAME_WIDTH,  LenX);
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_PROP_FRAME_HEIGHT, LenY);
    cvSetCaptureProperty (cvCameraCaptureObj, CV_CAP_PROP_TRASMISSION_MODE, ColorDepth); 
} 


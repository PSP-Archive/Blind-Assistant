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
   SOTTOSISTEMA DELL'ENGINE VIDEO PER LA FUNZIONALITA' MAIL READER
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
    -------------------------------------------------------------------------------
    VARIABILI
    -------------------------------------------------------------------------------
*/ 

// Destinato ad accogliere le immagini relative alle funzioni di sistema
static struct ndImage_Type MyImage;

// Riferimenti extern ad altre variabili
extern unsigned short int IconEmail_Width;
extern unsigned short int IconEmail_Height;
extern unsigned short int IconEmail[];



/*
    -------------------------------------------------------------------------------
    CODICE PRINCIPALE
    -------------------------------------------------------------------------------
*/ 

void videng_MAILREADER_Constructor ()
{
    #ifndef PSP1000_VERSION
    
        // Provvede ad inibire il checker per le mail
        baInhibitMailCheckerEngine ();
        
        // Segnala la funzione per mezzo della function bar
        mainitf_FunctionBar ("Mail reader mode", RENDER);
          
        // Messaggio che segnala l'attivazione della funzione OCR
        baaud_MailReaderEnabled ();
        
        // Cancellazione dell'area video
        ndWS_DrawRectangle (35, 3, 285, 180, COLOR_WHITE, COLOR_WHITE, mainitf_Window, NORENDER);
        
        // Visualizzazione dell'icona del mail reader
        ndIMG_LoadImageFromNDFArray (&MyImage, IconEmail_Width, IconEmail_Height, &IconEmail, NDMGKNB); 
        ndIMG_ShowImageScaled (&MyImage, 35+93, 3+56, 2.0, 2.0, mainitf_Window, NORENDER); 
        
        // Rendering
        XWindowRender (mainitf_Window); 
        
    #endif 
}

void videng_MAILREADER_Core (int TrianglePressed)
{
    #ifndef PSP1000_VERSION
    
        if (TrianglePressed) baMail_StartASessionOfTheClient ();
        ndHAL_Delay (1);
        
    #endif 
}

void videng_MAILREADER_Destructor ()
{
    #ifndef PSP1000_VERSION
    
        // Riattiva il sistema di check automatico per le email
        baDeInhibitMailCheckerEngine (); 
        
    #endif
}


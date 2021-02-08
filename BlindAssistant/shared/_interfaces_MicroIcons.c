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
   GESTORE DELLE MICROICONE DI SISTEMA
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


/* 
   ----------------------------------------------------------------------------
   VARIABILI 
   ----------------------------------------------------------------------------
*/



/* 
   ----------------------------------------------------------------------------
   DATABASE MICROICONE
   ----------------------------------------------------------------------------
*/

unsigned short int  baMicroIcons_DataBase [1][16] = 
                    {   
                        {
                             0xFFFF, // 1111111111111111
                             0xFFFF, // 1111111111111111
                             0xE007, // 1110000000000111
                             0xF00F, // 1111000000001111
                             0xD81B, // 1101100000011011
                             0xCC33, // 1100110000110011
                             0xC663, // 1100011001100011
                             0xC3C3, // 1100001111000011
                             0xC183, // 1100000110000011
                             0xC003, // 1100000000000011
                             0xC003, // 1100000000000011
                             0xC003, // 1100000000000011
                             0xC003, // 1100000000000011
                             0xC003, // 1100000000000011
                             0xFFFF, // 1111111111111111
                             0xFFFF  // 1111111111111111             
                        }
                    };


/* 
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/

void INTERNAL_UpdateMicroIconsDisplay (char RenderNow)
{
    #ifndef PSP1000_VERSION
    
            int PosX1, PosY1, PosX2, PosY2;
            int CounterX, CounterY;
            int NrIcon;
            
            for (NrIcon=0; NrIcon<MICROICON__UNREADED_MAILS; NrIcon++)
            {
                PosX1 = 4;
                PosX2 = 4  + 16;
                PosY1 = 20 + 20*NrIcon;
                PosY2 = 20 + 20*NrIcon + 16;
                
                if ( (baMicroIcons_Register >> NrIcon) & 1 )
                {
                    // Provvedi a disegnare l'icona
                    
                    for (CounterY=0; CounterY<16; CounterY++)
                    {
                        for (CounterX=0; CounterX<16; CounterX++)
                        {
                            
                            if ( (baMicroIcons_DataBase [NrIcon][CounterY] >> (15-CounterX)) & 1)
                            {
                                 ndWS_PutPixel (PosX1+CounterX, PosY1+CounterY, COLOR_GREEN, mainitf_Window, NORENDER);
                            }
                            else
                            {
                                 ndWS_PutPixel (PosX1+CounterX, PosY1+CounterY, COLOR_GRAY04, mainitf_Window, NORENDER);
                            }
                        
                        }
                    }
                }
                else
                {
                    ndWS_DrawRectangle (PosX1, PosY1, PosX2, PosY2, COLOR_GRAY04, COLOR_GRAY04, mainitf_Window, NORENDER);
                }     
            }
            
            if (RenderNow) XWindowRender (mainitf_Window);
            
    #endif
}


void baMicroIcons_Enable (int IconCode, char RenderNow)
{
    baMicroIcons_Register = baMicroIcons_Register | IconCode;
    INTERNAL_UpdateMicroIconsDisplay (RenderNow); 
}

void baMicroIcons_Disable (int IconCode, char RenderNow)
{
    baMicroIcons_Register = baMicroIcons_Register & (~IconCode);
    INTERNAL_UpdateMicroIconsDisplay (RenderNow); 
}


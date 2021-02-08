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
   ROUTINES DI GESTIONE DELLO SCANNER COLORI
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
        
        
int baColorScanner_DeterminateTheMinimumDistanceColor (float ChannelR, float ChannelG, float ChannelB)
{
   char IsTheFirstComp=1;
   int  Counter, BestElement;
   
   float DiffR, DiffG, DiffB;
   float MinimumDistance;
   
   // Calcoliamo le distanze di ciascun colore riconosciuto dal colore incognito
   
   for (Counter=0; Counter<STD_NR_COLORS_AVAILABLE; Counter++)
   {
       DiffR = ChannelR - baColorInfo [Counter].ChannelR;
       DiffG = ChannelG - baColorInfo [Counter].ChannelG;
       DiffB = ChannelB - baColorInfo [Counter].ChannelB;
   
       baColorInfo [Counter].Distance = ndHAL_XFPU_sqrtf (DiffR*DiffR + DiffG*DiffG + DiffB*DiffB);
   }  
   
   // Calcoliamo l'elemento di minima distanza
   
   for (Counter=0; Counter<STD_NR_COLORS_AVAILABLE; Counter++)
   {
       if ( (IsTheFirstComp) || (baColorInfo [Counter].Distance<MinimumDistance) )
       {
            IsTheFirstComp=0;
            
            MinimumDistance = baColorInfo [Counter].Distance;
            BestElement     = Counter;
       }
   }
   
   // Restituisci alla routine chiamante il codice colore che più assomiglia
   
   return BestElement;
}

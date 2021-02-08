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
   SOTTOSISTEMA DELL'ENGINE VIDEO PER LA FUNZIONALITA' DATE/TIME
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
     
char clockWndHandle;
        
/*
    -------------------------------------------------------------------------------
    CODICE PRINCIPALE
    -------------------------------------------------------------------------------
*/ 

void videng_SayDayTime (void)
{
  char   DecodingBuffer [256];
  time_t rawtime;
  struct tm *timeinfo;
  
  static char   *WeekDay [7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  static char   *Months [12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"}; 
   
  int    PosX, PosY;
  
  clockWndHandle = ndLP_CreateWindow (5, 5, 130, 80, "DateTime", COLOR_WHITE, COLOR_BLUE, COLOR_GRAY18, COLOR_GRAY18, NDKEY_SETTITLEFONT (3) | NO_RESIZEABLE);
  
  if (clockWndHandle>=0)
  {
     ndLP_MaximizeWindow (clockWndHandle);
     
     time ( &rawtime );
     timeinfo = localtime ( &rawtime );
     
     sprintf (DecodingBuffer, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
     PosX = (WindowData[clockWndHandle].WSWndLength - 8*strlen(DecodingBuffer))/2;
     PosY = (WindowData[clockWndHandle].WSWndHeight - 16)/2;
     
     ndWS_GrWriteLn (PosX, PosY, DecodingBuffer, COLOR_GRAY06, COLOR_GRAY18, clockWndHandle, NDKEY_SETFONT (3) | RENDER);
     
     sprintf (DecodingBuffer, "%02d/%02d/%04d", timeinfo->tm_mday, 1 + timeinfo->tm_mon, 1900 + timeinfo->tm_year);
     PosX = (WindowData[clockWndHandle].WSWndLength - 8*strlen(DecodingBuffer))/2;
     PosY = (WindowData[clockWndHandle].WSWndHeight - 16)/2 + 9;
     
     ndWS_GrWriteLn (PosX, PosY, DecodingBuffer, COLOR_GRAY12, COLOR_GRAY18, clockWndHandle, NDKEY_SETFONT (3) | RENDER);
     
     sprintf (DecodingBuffer, "It is %02d and %02d minutes. \n", timeinfo->tm_hour, timeinfo->tm_min);
     baaud_ReproduceASingleWordRT (DecodingBuffer);
     
     ndDelay (3);
     
     sprintf (DecodingBuffer, "Today is %s, %d, %s, %d %d \n", WeekDay [timeinfo->tm_wday], 
                                                              timeinfo->tm_mday, 
                                                              Months [timeinfo->tm_mon],
                                                              (1900 + timeinfo->tm_year) / 100, 
                                                              (1900 + timeinfo->tm_year) % 100 );
     baaud_ReproduceASingleWordRT (DecodingBuffer);
  
     ndLP_DestroyWindow (clockWndHandle);
  }
}


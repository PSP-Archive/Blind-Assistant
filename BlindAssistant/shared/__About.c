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
   ABOUT
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
   ----------------------------------------------------------------------------
   VARIABILI PER USO INTERNO
   ----------------------------------------------------------------------------
*/ 

int  aboutWindow;
int  about_WndAlreadyOpen = 0; 

/*
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/ 
 
 
ndint64 cbAboutWnd_CLOSE (char WndHandle, ndint64 WndInfoField)
{
     ndLP_DestroyWindow ( aboutWindow );
     about_WndAlreadyOpen=0;
}


void baOpenAboutWindow (void)
{
    if (!about_WndAlreadyOpen)
    {
         aboutWindow = ndLP_CreateWindow (10, 20, 420, 200, "About", COLOR_WHITE, COLOR_RED, COLOR_WHITE, COLOR_BLACK, NDKEY_SETWSLENGTH (390) | NDKEY_SETWSHEIGHT (550)); 
         ndLP_MaximizeWindow (aboutWindow);
         
         if (aboutWindow>=0)
         {
             ndLP_SetupCloseWndCallback (cbAboutWnd_CLOSE, 0, 1, aboutWindow);
             
             ndWS_CarriageReturn (aboutWindow);
             
             #ifndef PSP1000_VERSION
                     ndWS_WriteLn (" Blind assistant                  ", COLOR_LBLUE, aboutWindow, NORENDER);
             #else
                     ndWS_WriteLn (" Blind assistant (version for PSP1000)", COLOR_LBLUE, aboutWindow, NORENDER);
             #endif
             
             ndWS_WriteLn (" Copyright (C) 2010-2015 Filippo Battaglia", COLOR_LBLUE, aboutWindow, NORENDER);
             
             ndWS_CarriageReturn (aboutWindow);
             ndWS_WriteLn (" This application is based on Nanodesktop  ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_WriteLn (" library version 0.5                       ", COLOR_BLACK, aboutWindow, NORENDER);
             
             ndWS_CarriageReturn (aboutWindow);
             ndWS_WriteLn (" This software has been developed in Visilab", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_WriteLn (" Research Center - University of Messina    ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_WriteLn (" Italy                                      ", COLOR_BLACK, aboutWindow, NORENDER);
             
             ndWS_CarriageReturn (aboutWindow);
             ndWS_Write   (" Software version code: ", COLOR_BLUE, aboutWindow, NORENDER);
             ndWS_Write (BA_VERSION_CODE, COLOR_BLACK, aboutWindow, NORENDER);
             
             ndWS_CarriageReturn (aboutWindow);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" This program contains code of different    ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_WriteLn (" libraries provided by third parties.       ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" ndOpenCV (BSD license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) 2000, Intel Corporation      ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" ndDevIL (LGPL license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) 2000-2002 by Denton Woods    ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" IJG LibJpg (JPEG license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) 1991-1998, Thomas G. Lane.   ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" PNG library (PNG license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (c) 2000-2002 Glenn Randers-Pehrson", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" TIFF library (TIFF license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (c) 1988-1997 Sam Leffler", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (c) 1991-1997 Silicon Graphics, Inc", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" ZLIB library (ZLIB license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) 1995-2004 Jean-loup Gailly ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_WriteLn (" and Mark Adler", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" ndFLite library", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) 1999 Language Technologies ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_WriteLn (" Institute -  Carnegie Mellon University ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" Curl library ", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (c) 1996 - 2005, Daniel Stenberg ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" MINIUNZIP library ", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (c) 1998 - 2005, Gilles Vollant ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" ndTesseract (Apache license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) Google Foundation ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" PocketSphinx (BSD license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) 1999-2008 Carnegie Mellon  ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_WriteLn (" University  ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" ndLibsPopC (LGPL license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) Benoit Rouits  ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" nd Data Matrix library (LGPL license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) Mike Laughton and others ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" ndVMIME (GPL license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) Vincent Richard and others ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             ndWS_WriteLn (" ndICONV (GPL license)", COLOR_LBLUE, aboutWindow, NORENDER);
             ndWS_WriteLn (" Copyright (C) Bruno Haible ", COLOR_BLACK, aboutWindow, NORENDER);
             ndWS_CarriageReturn (aboutWindow);
             
             XWindowRender (aboutWindow);                  
             about_WndAlreadyOpen=1;              
         }
    } 
}




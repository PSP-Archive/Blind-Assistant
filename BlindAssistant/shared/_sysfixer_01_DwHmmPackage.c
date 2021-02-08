/* 
   ----------------------------------------------------------------------------
   BLIND ASSISTANT CFW0006
   ----------------------------------------------------------------------------
   Copyright 2010-2015 Filippo Battaglia
   This program has been developed in Visilab Research Center 
   University of Messina - Italy
   
   All right reserved. The use of this program is allowed only for not
   commercial uses. 
   
   The source code is released under GPL license. See file Blind Assistant
   License.Pdf for further details
   ----------------------------------------------------------------------------
   SISTEMA DI AUTORIPARAZIONE: Condizione #01
   ----------------------------------------------------------------------------
   Questo modulo si occupa di correggere un problema che si verifica solo
   qualora l'utente esegua un aggiornamento online da una versione di
   Blind Assistant che ancora non supportava il sistema di controllo
   vocale PocketSphinx. In questo caso, il sistema provvede a scaricare
   automaticamente dal sito del Visilab i pacchetti HMM necessari per
   il funzionamento del sistema vocale
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
        
        #include <ndsphinx_api.h>

/*
   -----------------------------------------------------------------------------
   RIFERIMENTI A FUNZIONI INTERNE
   -----------------------------------------------------------------------------
*/ 
 

int unzip_main(int argc, char *argv[]);

/*
   ----------------------------------------------------------------------------
   VARIABILI
   ----------------------------------------------------------------------------
*/

char autofix01_WndHandle;



/*
   -----------------------------------------------------------------------------
   FUNZIONI DI CONNESSIONE PER DOWNLOAD DEL PACCHETTO DI CONTROLLO VOCALE
   -----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

char INTERNAL_ConnectToNetForDwHmmPackage (void)
{
   char ErrRep, IntErrRep;
          
   ErrRep=ndHAL_WLAN_GetAccessPointInfo (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj);                         
   
   if (!ErrRep) 
   {
      return ndHAL_WLAN_OpenPSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj, ND_SILENT_AP_ACCESS, 10);         
   }
   else
      return ErrRep;
}

void INTERNAL_DisconnectFromNetForDwHmmPackage (void)
/*
    Disconnetti la rete, eliminando eventuali connessioni dedicate per 
    l'aggiornamento
*/
{
    ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1);
}

#endif

/*
    ---------------------------------------------------------------------------
    SISTEMA DI RECUPERO DEL PACKAGE PER IL CONTROLLO VOCALE
    ---------------------------------------------------------------------------
*/

char SYSFIXER01_DownloadHmmVocalPackage ()
{
    #ifndef PSP1000_VERSION
    
        char SrcFile  [1024];
        char DestFile [1024];
        
        char ZipCommand [1024];
        int  ErrorLevel;
        char ErrRep;
        
        autofix01_WndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 350, 10, Windows_MaxScreenX - 10, 100,
                                                   "System fixer #01", COLOR_WHITE, COLOR_RED, COLOR_GRAY14, COLOR_GRAY14, 0); 
   
        if (autofix01_WndHandle>=0)
        {
            ndLP_MaximizeWindow (autofix01_WndHandle);
    
            ndWS_GrWriteLn (5, 15, "One or more of the voice database files",   COLOR_BLACK, COLOR_GRAY14, autofix01_WndHandle, NORENDER); 
            ndWS_GrWriteLn (5, 25, "are missing, old or corrupt. I am trying",   COLOR_BLACK, COLOR_GRAY14, autofix01_WndHandle, NORENDER); 
            ndWS_GrWriteLn (5, 35, "to download them from Visilab website",   COLOR_BLACK, COLOR_GRAY14, autofix01_WndHandle, NORENDER); 
            XWindowRender (autofix01_WndHandle);
        
        
            // Prova a connetterti ad un access point per scaricare, eventualmente, il
            // pacchetto HMM dal sito del Visilab
            
            if (INTERNAL_ConnectToNetForDwHmmPackage()==0)
            {
                // Esegui il download del file del package compresso per il voice
                // control in locale
                
                // Resetta ai valori standard le opzioni di rete per il modulo GCurl
                memset (&NetworkOptions, 0, sizeof(NetworkOptions));
            
                // Setup delle impostazioni di rete in base a quanto previsto dalle
                // advanced options
                NetworkOptions.BeVerbose     = SystemOptPacket.BeVerbose;
                NetworkOptions.HttpTunneling = SystemOptPacket.HttpTunneling;
            
                // Definisci l'URL del file sorgente
                strcpy (SrcFile, BA_SITE);
                strcat (SrcFile, BA_VOICECTRL_PATH);
                strcat (SrcFile, BA_VOICECTRL_ZIPNAME);
                
                // Definisci il file di destinazione 
                strcpy (DestFile, "ms0:/BLINDASSISTANT/ZIP/");
                strcat (DestFile, BA_VOICECTRL_ZIPNAME);
                                
                // Download del file zip che contiene il file per il controllo vocale
                                
                ErrRep=ndGCURL_FileTransfer (&NetObj, &SrcFile, &DestFile, &NetworkOptions);
                
                if (!ErrRep)
                {
                     // Esegui la decompressione del file compresso
                     
                     sprintf (ZipCommand, "-o -x %s -d %s", DestFile, "ms0:/BLINDASSISTANT/SPHINX");
                     
                     pseudoExec (&unzip_main, "unzip", ZipCommand, &ErrorLevel);         
                                
                     // Fai un ritardo per consentire la lettura degli errori
                     ndDelay (2);
                                    
                     // Chiudi la finestra STDOUT/STDERR
                     ndDisableStdOutDialogBox ();
                     
                     // Chiudi la finestra informativa
                     ndLP_DestroyWindow (autofix01_WndHandle);
                     
                     INTERNAL_DisconnectFromNetForDwHmmPackage (); 
                     return ErrorLevel;
                }
                else           // Impossibile scaricare il pacchetto HMM dal server
                {  
                     // Chiudi la finestra informativa
                     ndLP_DestroyWindow (autofix01_WndHandle);
                    
                     INTERNAL_DisconnectFromNetForDwHmmPackage (); 
                     return -2;                
                }
            }
            else   // Impossibile connettersi al server per provare a scaricare il pacchetto HMM
            {
                ndLP_DestroyWindow (autofix01_WndHandle);
                return -1;
            }  
        }
        else   // Impossibile creare la finestra
          return -1; 
            
    #endif
}

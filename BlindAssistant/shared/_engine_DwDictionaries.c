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
   FUNZIONI DI DOWNLOAD DEI DIZIONARII
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
   -----------------------------------------------------------------------------
   VARIABILI PER USO INTERNO
   -----------------------------------------------------------------------------
*/ 
 
char badwdict_EnableDedNetConnection; 

/*
   -----------------------------------------------------------------------------
   RIFERIMENTI A FUNZIONI INTERNE
   -----------------------------------------------------------------------------
*/ 
 

int unzip_main(int argc, char *argv[]);



/*
   -----------------------------------------------------------------------------
   FUNZIONI DI CONNESSIONE PER DOWNLOAD DEI DIZIONARI
   -----------------------------------------------------------------------------
*/

char INTERNAL_ConnectToNetForDwDictionary (void)
{
   char ErrRep, IntErrRep;
   
   if (!MainNetConn_IsOperative)
   {
           // Non c'è una connessione principale di rete attivata.
           // Provvedi autonomamente al processo di connessione
          
           IntErrRep=ndHAL_WLAN_GetAccessPointInfo (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj);                         
     
           if (!IntErrRep)
           {
                  IntErrRep = ndHAL_WLAN_OpenPSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj, ND_SILENT_AP_ACCESS, 10);        
           
                  if (!IntErrRep)       // E' riuscito a creare una connessione dedicata
                  {
                       ErrRep                           =  0;
                       badwdict_EnableDedNetConnection  =  1;            
                  }
                  else
                  {
                       ErrRep                           =  -2;
                       badwdict_EnableDedNetConnection  =  0;    
                  }
           }
           else   // L'access point è vuoto
           {
                ErrRep                           = -1;
                badwdict_EnableDedNetConnection  =  0;  
           }
     }
     else         // C'è una connessione principale di rete attivata
     {
           ErrRep                           = 0;
           badwdict_EnableDedNetConnection  = 0;
     }
   
     return ErrRep;  
}

void INTERNAL_DisconnectToNetForDwDictionary (void)
/*
    Disconnetti la rete, eliminando eventuali connessioni dedicate per 
    l'aggiornamento
*/
{
    if (badwdict_EnableDedNetConnection)
    {
        ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1);
        badwdict_EnableDedNetConnection     = 0;                                      
    }
}

     



/*
   -----------------------------------------------------------------------------
   DOWNLOAD DEL DIZIONARIO BASE DI SISTEMA
   -----------------------------------------------------------------------------
*/


void baDwDict_DownloadDictionaryZip (void)
{
    char NameDictionary [128];
    char NameUpdate [128];
    char FileDestination [128];
    static int ErrorLevel;
    
    char ErrRep;
    
    if (!NetworkIsBusy)
    {
            NetworkIsBusy = 1;
            baInhibitVideoEngine();                         // Inibisce l'engine video
           
            // Provvedi alla connessione (usando la connessione principale oppure 
            // creandone una secondaria appositamente)
           
            ErrRep=INTERNAL_ConnectToNetForDwDictionary ();
            
            switch (ErrRep)
            {
                   case 0:                  // Nessun problema
                   {
                        // Genera l'URL sul sito del Visilab che contiene il file compresso dei 
                        // vocabolarii
                        
                        baOcrFNames_MakeDictPackg_VisilabSvr (&NameDictionary);
                        
                        // Resetta ai valori standard le opzioni di rete per il modulo GCurl
                        memset (&NetworkOptions, 0, sizeof(NetworkOptions));
    
                        // Setup delle impostazioni di rete in base a quanto previsto dalle
                        // advanced options
                        NetworkOptions.BeVerbose     = SystemOptPacket.BeVerbose;
                        NetworkOptions.HttpTunneling = SystemOptPacket.HttpTunneling;
    
                        // Definisci il file di destinazione (file compresso dei vocabolarii)
                        
                        baOcrFNames_MakeDictPackg_Target (&FileDestination);
                        
                        // Download del file zip che contiene il dizionario
                        
                        ErrRep=ndGCURL_FileTransfer (&NetObj, &NameDictionary, &FileDestination, &NetworkOptions);
                        
                        if ( (!ErrRep) && (flen (FileDestination)>1024) )
                        {
                            // Se il processo di download è terminato, si deve procedere alla
                            // decompressione del file .zip
                            pseudoExec (&unzip_main, "unzip", "-o -e ms0:/BLINDASSISTANT/ZIP/Dictionaries.zip -d ms0:/BLINDASSISTANT/DICT", &ErrorLevel);         
                        
                            // Fai un ritardo per consentire la lettura degli errori
                            ndDelay (2);
                            
                            // Chiudi la finestra STDOUT/STDERR
                            ndDisableStdOutDialogBox ();
                        }
                        else  // Non è stato possibile scaricare il file compresso che contiene 
                        {     // i dizionari. Segnalalo al sistema
                              
                            baaud_ImpossibleToDownloadDict ();
                        }
                        
                        // Sessione di aggiornamento finita: chiudi la connessione
                        INTERNAL_DisconnectToNetForDwDictionary ();
                        
                        break;
                   }
            
                   case -1:  // Problemi nella configurazione del profilo di rete
                   {
                        INTERNAL_SignalWrongConnectionProfile (); 
                        break;
                   }       
                   
                   case -2:  // Problemi nella connessione di rete
                   {
                        /* Non è prevista alcuna segnalazione */
                        break;
                   }
           }      
                      
           // Routines completata: deinibisce video engine   
                            
           baDeInhibitVideoEngine_NoForce();                      // DeInibisce l'engine video           
           NetworkIsBusy=0; 
    }  
}




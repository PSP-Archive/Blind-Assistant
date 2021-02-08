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
   MAIN SOURCE
   ----------------------------------------------------------------------------
*/




#define ND_DISABLE_RAWIR_SERVER
        #include <nanodesktop.h>
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
   
   #define BA_NEED_VARIABLES    
        #include "BlindAssistant.h"
   #undef  BA_NEED_VARIABLES

char   baNewExecRequested = 0;


int ndMain ()
{
        int ErrRep;
        
        ndInitSystem ();
        
        ErrRep=baInit();                     // Provvede alla reinizializzazione del sistema
        if (ErrRep) goto __End0;
        
        ndHAL_PWR_SuspendTimerOff ();        // Disattiva il timer che regola la disattivazione 
                                             // automatica dello schermo per risparmiare energia 
        
        baBldSvr_AllocateCommPackets ();     // Allocazione dei pacchetti di comunicazione
        
        baEnableMainNetworkConnection ();    // Attiva la connessione internet principale del sistema
        baEnableMainConnToBlindServer ();    // Abilita la connessione principale con il Blind Server
        
        baStartUpdateEngine();               // Avvia l'update engine
        baStartMailCheckerEngine();          // Avvia l'engine per il controllo delle mail
        
        baMainInterface ();                  // Disegna l'interfaccia principale del programma
        
        baStopMailCheckerEngine();           // Disattiva l'engine per il controllo delle mail 
        baStopUpdateEngine ();               // Disattiva l'update engine
        
        baDisableMainConnToBlindServer ();   // Disabilita la connessione principale con il Blind Server
        baDisableMainNetworkConnection ();   // Disattiva la connessione ad internet principale
        
        baBldSvr_DeAllocateCommPackets ();   // Deallocazione dei pacchetti di comunicazione
        
        baDeInit ();                         // Arresto del motore di sintesi vocale, disattivazione
                                             // della webcam
        
        ndHAL_PWR_SuspendTimerOn ();         // Riattiva il timer che regola la disattivazione 
                                             // automatica dello schermo per risparmiare energia  
          
    __End0:
        
        if (!baNewExecRequested)
           ndHAL_SystemHalt (0);
        else                                 // Carica il nuovo file che è stato richiesto
           ndHAL_SystemExec (BA_CORE_FILE);
           
}

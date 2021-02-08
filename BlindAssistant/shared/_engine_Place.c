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

char NameOfTheRoom [256];

int  plcengThreadID;
int  plcengThreadPriority;
int  plcengEventID;

int  plcengYouCanExit, plcengIsBusy;
char plcengInhibitedByInternal = 0;


// Destinato ad accogliere il puntatore all'immagine da analizzare
IplImage *plcengImage;






/*
   ----------------------------------------------------------------------------
   ENGINE
   ----------------------------------------------------------------------------
*/ 

int baPlaceEngine (SceSize args, void *argp)
{
    plcengYouCanExit = 0;
    plcengIsBusy     = 0;
    
    while (!plcengYouCanExit)
    {
        if (!plcengInhibitedByInternal)
        {
                // Attende che ci sia un messaggio in attesa
                sceKernelWaitEventFlag (plcengEventID, PLACESVR_MSG_IS_READY, PSP_EVENT_WAITAND | PSP_EVENT_WAITCLEAR, 0, 0); 
                
                // Ripristina una priorità di thread privilegiata: sarà abbassata alla fine del
                // processo di trasmissione
                sceKernelChangeThreadPriority (plcengThreadID, plcengThreadPriority-10); 
                
                // Impegna il place engine
                plcengIsBusy     = 1;
                
                // Segnala all'utente di attendere
                baaud_Wait ();
                
                // Segnala che il server è stato contattato
                mainitf_StatusBar ("Contacting BlindServer...", RENDER);
                
                // Chiedi a BlindServer in che luogo ci troviamo
                baBldSvr_AskToBlindServerWhereIAm (plcengImage, &NameOfTheRoom);
                     
                // Riporta il place engine ad una modalità di priorità bassa
                sceKernelChangeThreadPriority (plcengThreadID, plcengThreadPriority+20); 
                                                                     
                if (strcmp (NameOfTheRoom, "[null]")!=0)     // BlindServer non ha restituito NULL
                   baaud_YouAreInThisRoom (&NameOfTheRoom);  // Segnala all'utente in che stanza si trova
                else
                   baaud_IDontKnowWhereYouAre ();            // Segnala che non sai dove si trova l'utente
                
                // Segnala che la risposta è arrivata
                mainitf_StatusBar ("", RENDER);
                
                // Disattiva il place engine
                plcengIsBusy     = 0;   
        }
        else   // Il Place Engine è stato inibito
        {
               // Ritarda il thread per 1 secondo
               sceKernelDelayThread (1000000);
        }
    }
}


/*
   ----------------------------------------------------------------------------
   API PER LA GESTIONE DEL PLACE ENGINE
   ----------------------------------------------------------------------------
*/ 

void baPlcEng_AskWhereIAmToServer (IplImage *img)
{
    if ((!plcengIsBusy) && (!NetworkIsBusy))
    {
         // Copia l'immagine nel buffer di analisi: ora il video engine può
         // cambiare l'immagine senza problemi
         zcvCopy (img, plcengImage);
         
         // Avverte il server che c'è un messaggio in attesa
         sceKernelSetEventFlag   (plcengEventID, PLACESVR_MSG_IS_READY);                     
    }
}


/*
   ----------------------------------------------------------------------------
   API PER L'INIZIALIZZAZIONE
   ----------------------------------------------------------------------------
*/ 
 
void baReStartPlaceEngine ()
{
    plcengThreadPriority = sceKernelGetThreadCurrentPriority (); 
    
    plcengThreadID = sceKernelCreateThread ("baPlaceEngine", &baPlaceEngine, plcengThreadPriority+20, 16384, 
                                                 PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU, 0);                                               

    // Fai partire il thread in una modalità a priorità bassa                                             

    sceKernelStartThread (plcengThreadID, 0, 0); 
    
    // Creazione dell'engine di riconoscimento dei luoghi
    
    plcengEventID = sceKernelCreateEventFlag ("baPlaceEventID", PSP_EVENT_WAITMULTIPLE, 0, 0); 

    // Creazione del buffer di analisi
    plcengImage = cvCreateImage ( cvSize (STD_BLDSVR__SAMPLEIMG__DIMX, STD_BLDSVR__SAMPLEIMG__DIMY), 8, 1 );
} 

void baExitPlaceEngine ()
{
    int TimeOut = 20000000;
    
    plcengYouCanExit=1;
    mainitf_StatusBar ("Terminating last BSVR request", RENDER);
    
    sceKernelWaitThreadEnd (plcengThreadID, &TimeOut);
    // Attende la fine del thread del place engine. E' utile per impedire che il Blind
    // server si veda interrotto il collegamento da parte del client a metà di una
    // operazione. Dopo 20 secondi, il sistema provvederà ugualmente ad una uscita
    // forzata
    
    mainitf_StatusBar ("", RENDER);
    
    sceKernelTerminateDeleteThread  (plcengThreadID);
    cvReleaseImage (&plcengImage);
}

void baInhibitPlaceEngine ()
{
    plcengInhibitedByInternal=1;  
}

void baDeInhibitPlaceEngine ()
{
    plcengInhibitedByInternal=0;     
}

 

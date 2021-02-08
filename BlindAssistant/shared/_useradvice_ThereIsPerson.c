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
   ROUTINES PER LA SEGNALAZIONE DELLA PRESENZA DI PERSONE NELLA STANZA -
   FILTRAGGIO SEGNALAZIONI SPURIE DI PRESENZA
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
   VARIABILI
   ----------------------------------------------------------------------------
*/ 

int RecordWhereWrite;
int PeoplePresenceIndex = 0;


/*
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/ 

void baResetPresenceRegisterForFrame ()
{
    int CounterImg, Counter;
    
    if (PeoplePresenceIndex>=NR_PRESENCE_ELEMENTS_IN_REG)
    {
         // Se il registro di presenze si è riempito completamente, il sistema 
         // deve spostare gli ultimi risultati dei registri di presenza nei
         // posti immediatamente superiori. Il dato più vecchio sarà 
         // perduto, e si libererà un posto alla fine, pronto ad
         // acquisire gli ultimi dati
         
         for (CounterImg=0; CounterImg<NR_SLOTS_FACE_RECOGNIZER; CounterImg++)
         {
             for (Counter=0; Counter<NR_PRESENCE_ELEMENTS_IN_REG-1; Counter++)
             {
                 facerec_ImagesInRam [CounterImg].Presence [Counter] = facerec_ImagesInRam [CounterImg].Presence [Counter+1];
             }
         } 
    } 
    
    // Adesso provvede a determinare in quali registri dovranno essere memorizzati
    // i dati di presenza che verranno inviati attraverso la routine baSignThePresenceOfAPerson
    
    if (PeoplePresenceIndex<NR_PRESENCE_ELEMENTS_IN_REG)   // Registro non ancora riempito del tutto
        RecordWhereWrite=PeoplePresenceIndex;        
    else
        RecordWhereWrite=NR_PRESENCE_ELEMENTS_IN_REG-1;
    
    // Provvedi ad azzerare i registri di presenza. Saranno eventualmente cambiati in seguito
    
    for (CounterImg=0; CounterImg<NR_SLOTS_FACE_RECOGNIZER; CounterImg++)
        facerec_ImagesInRam [CounterImg].Presence [RecordWhereWrite].ThereWas=0; 
    
} 
 
void baSignThePresenceOfAPerson (unsigned short int PosX, unsigned short int PosY, int NrElement)
{
    // Provvedi a memorizzare l'informazione di presenza nel registro corretto di
    // sistema
    
    facerec_ImagesInRam [NrElement].Presence [RecordWhereWrite].ThereWas=1;
    facerec_ImagesInRam [NrElement].Presence [RecordWhereWrite].PosX=PosX;
    facerec_ImagesInRam [NrElement].Presence [RecordWhereWrite].PosY=PosY;
}


void baDispatchMessageForPeoplePresence ()
{
     register int CounterImg, Counter, Confirmed;
     register int AvgPosX, AvgPosY;
     
     char DelayIsRequired = 0;
     
     // Verifica se la presenza è confermata: viene eseguito un controllo per singolo
     // face-slot: il controllo deve essere eseguito solo a buffer pieno
     
     if (PeoplePresenceIndex>=NR_PRESENCE_ELEMENTS_IN_REG)
     {
         for (CounterImg=0; CounterImg<NR_SLOTS_FACE_RECOGNIZER; CounterImg++)
         {
             if (facerec_ImagesInRam [CounterImg].IsActive)
             {
                  Confirmed=1;                   
                                     
                  for (Counter=NR_PRESENCE_ELEMENTS_IN_REG-1; Counter>=0; Counter--)
                  {
                      if (!facerec_ImagesInRam [CounterImg].Presence [Counter].ThereWas)
                      {
                          Confirmed=0;    // Presenza non confermata per il singolo elemento. 
                          break;          // Interrompi subito il ciclo for più interno....
                      }   
                  }                   
                  
                  // ....e ricomincia il ciclo for più esterno
                  
                  if (!Confirmed) continue;   
                  
                  // Se il sistema è arrivato qui, vuol dire che c'è qualche presenza 
                  // da segnalare per mezzo della routine audio dedicata. Prima 
                  // provvediamo a calcolare la posizione media del soggetto
                  
                  AvgPosX=0;
                  AvgPosY=0;
                
                  for (Counter=0; Counter<NR_PRESENCE_ELEMENTS_IN_REG; Counter++)
                  {
                     AvgPosX += facerec_ImagesInRam [CounterImg].Presence [Counter].PosX;
                     AvgPosY += facerec_ImagesInRam [CounterImg].Presence [Counter].PosY;
                  }
                
                  AvgPosX = AvgPosX/NR_PRESENCE_ELEMENTS_IN_REG;
                  AvgPosY = AvgPosY/NR_PRESENCE_ELEMENTS_IN_REG;
                
                  // Esegui la segnalazione per mezzo del sistema audio: se nella medesima
                  // istanza di questa routine erano già stati emessi dei messaggi audio
                  // sulla presenza di una persona, il sistema deve attendere almeno 1
                  // secondo prima di dare ogni messaggio successivo
                  if (DelayIsRequired) ndHAL_Delay (2.0);
                  
                  // Segnalazione per mezzo del sistema audio...
                  baaud_SignalThePresenceOfAPerson (AvgPosX, AvgPosY, CounterImg); 
                  
                  // Attiva il flag in modo da creare i ritardi temporali successivi
                  DelayIsRequired=1;      
             }
         }                                                 
     }
     
     // Aggiorna l'indice 
     
     PeoplePresenceIndex++;
}






















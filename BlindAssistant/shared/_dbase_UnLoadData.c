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
   ROUTINES DI SCARICAMENTO DATI DALLA MEMORIA
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
   VARIABILI E COSTANTI PER USO INTERNO
   ----------------------------------------------------------------------------
*/ 



/*
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

int baUnLoadAllSampleImagesOfTheSystem (void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
     void (*PntToGraphCallback)(int NrSlot, int UnloadCode, struct OpInfoPacket_Type *InfoPacket);
     PntToGraphCallback = GraphCallback;
     
     int CounterSlot, CounterSample;
     
     // Invia un segnale che deve indicare che si sta iniziando l'unload
     if (PntToGraphCallback) (*PntToGraphCallback)(0, BEGIN_TO_UNLOAD, InfoPacket);
     
     // Inizia a scaricare i dati del facerecognizer dalla memoria
     
     for (CounterSlot=0; CounterSlot<NR_SLOTS_FACE_RECOGNIZER; CounterSlot++)
     {
         if (facerec_ImagesInRam [CounterSlot].IsActive)
         {
             // Scarica dalla memoria i campioni relativi al singolo slot
             
             for (CounterSample=0; CounterSample<NR_SAMPLES_FOR_FACE; CounterSample++)
             {
                 cvReleaseImage ( &(facerec_ImagesInRam [CounterSlot].Img [CounterSample]) );
             }   
             
             // Questo slot ora è disattivato
             
             facerec_ImagesInRam [CounterSlot].IsActive=0; 
         }
         
         // Invia un segnale che indica che l'unload è in progress....
         if (PntToGraphCallback) (*PntToGraphCallback)(CounterSlot, UNLOADING_SAMPLE, InfoPacket);    
     }
     
     // Aggiorna il manager PCA per mantenerne la coerenza
     baUpdateFaceRecognizerDBase (-1);  
     
     // Invia un segnale che indica che il lavoro è terminato
     if (PntToGraphCallback) (*PntToGraphCallback)(0, DONE0, InfoPacket);   
     return 0;
}

#endif


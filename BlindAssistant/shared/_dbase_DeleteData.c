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
   ROUTINES DI CANCELLAZIONE DEI DATI
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
   CODICE: CANCELLAZIONE DATI DALLA MEMORIA
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

void baErase_DeleteDataInMemory (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int EraseCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    int Counter;
    
    // Richiama la callback grafica con il codice che indica che stiamo cancellando i dati in memoria
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, ERASING_MEMORY, InfoPacket);                        
   
    if (facerec_ImagesInRam [NrSlot].IsActive)
    {
         facerec_ImagesInRam [NrSlot].IsActive=0; 
         
         for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
         {
             if (facerec_ImagesInRam [NrSlot].Img [Counter]) 
                 cvReleaseImage ( &(facerec_ImagesInRam [NrSlot].Img [Counter]) ); 
         } 
         
         baUpdateFaceRecognizerDBase (-1);   
    }       
}

/*
   ----------------------------------------------------------------------------
   CODICE: CANCELLAZIONE AREE SUL DISCO
   ----------------------------------------------------------------------------
*/ 

void  baErase_DeleteAudioSamplesInTheDisk (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int EraseCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    char NameFile0 [256];
    int Counter;
    
    for (Counter=0; Counter<VOICE_SAMPLES_FOR_FACE; Counter++)
    {
        // Richiama la callback grafica con il codice che indica che stiamo cancellando i files dalla MS
        if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERASING_VOICE_SAMPLE, InfoPacket);                        
   
        genstr_NameOfVoiceSample (NrSlot, Counter, &NameFile0);
        remove (&NameFile0);
        
        // Richiama la callback grafica con il codice che indica che stiamo cancellando i dati in memoria
        if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERASED_VOICE_SAMPLE, InfoPacket);                        
    }     
}


void baErase_DeleteSampleImagesInTheDisk (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int EraseCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    char NameFile0 [256]; 
    int Counter;
    
    for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
    {
        // Richiama la callback grafica con il codice che indica che stiamo cancellando i files dalla MS
        if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERASING_IMAGE_SAMPLE, InfoPacket);                        
   
        genstr_NameOfImageSample (NrSlot, Counter, &NameFile0);
        remove (&NameFile0);
        
        // Richiama la callback grafica con il codice che indica che stiamo cancellando i dati in memoria
        if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERASED_IMAGE_SAMPLE, InfoPacket);                        
    }     
}


void baErase_DeleteNameFileInTheDisk (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int EraseCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    char NameFile0 [256];
    int  Counter;
    
    // Richiama la callback grafica con il codice che indica che stiamo cancellando i files dalla MS
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERASING_NAME, InfoPacket);                        

    genstr_NameOfNameFile (NrSlot, &NameFile0);
    remove (&NameFile0);
    
    // Richiama la callback grafica con il codice che indica che stiamo cancellando i dati in memoria
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERASED_NAME, InfoPacket);                        
     
}



/*
   ----------------------------------------------------------------------------
   CANCELLAZIONE DATI DEL SINGOLO SLOT
   ----------------------------------------------------------------------------
*/ 

int baErase_DeleteDataRelativeToASlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int EraseCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    int ErrRep;
    
    // Richiama la callback grafica con il codice che indica che stiamo iniziando a cancellare
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, BEGIN_TO_ERASE, InfoPacket);                        
    
    // Cancella i dati in memoria ed aggiorna le strutture XPCA
    baErase_DeleteDataInMemory (NrSlot, GraphCallback, InfoPacket);
    
    // Cancella i files che contengono i campioni audio
    baErase_DeleteAudioSamplesInTheDisk (NrSlot, GraphCallback, InfoPacket);
    
    // Cancella i files che contengono i campioni immagine
    baErase_DeleteSampleImagesInTheDisk (NrSlot, GraphCallback, InfoPacket);
    
    // Cancella il file che contiene il nome file
    baErase_DeleteNameFileInTheDisk (NrSlot, GraphCallback, InfoPacket);
     
    // Richiama la callback grafica con il codice che indica che abbia terminato la fase di erase
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DONE1, InfoPacket);
    
    return 0;                        
}

#endif





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
   ROUTINES DI CARICAMENTO DEI DATI
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

IplImage* cvTempLoadImg = 0;



/*
   ----------------------------------------------------------------------------
   FUNZIONI DI VALIDAZIONE DEI DATI DELLO SLOT
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

char INTERNAL_DataInSlotAreValid (int NrSlot)
{
     char Stringa [256];
     int  ErrRep, Counter;
     
     genstr_NameOfNameFile (NrSlot, &Stringa);
     if (!fexist (&Stringa)) return 0;          // Il file name non esiste
     
     for (Counter=0; Counter<VOICE_SAMPLES_FOR_FACE; Counter++)
     {
         genstr_NameOfVoiceSample (NrSlot, Counter, &Stringa);
         if (!fexist (&Stringa)) return 0;      // Uno dei messaggi vocali è assente        
     }
     
     for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
     {
         genstr_NameOfImageSample (NrSlot, Counter, &Stringa);
         
         if (fexist (&Stringa)) 
         {
             HGUI_DisableErrorNotification ();
             cvTempLoadImg = cvLoadImage (&Stringa, 0);
             HGUI_EnableErrorNotification ();
             
             if ( (int)(cvTempLoadImg)!=-1 )
             {
                  if (cvTempLoadImg->nChannels==1)
                  {
                         if (cvTempLoadImg->depth==8)
                         {
                              if ( (cvTempLoadImg->width==STD_FACEREC__SAMPLEIMG__DIMX) && (cvTempLoadImg->height==STD_FACEREC__SAMPLEIMG__DIMY) )
                              {
                                  cvReleaseImage (&cvTempLoadImg); 
                                  // OK: L'immagine va bene: attendi il prossimo ciclo
                              }
                              else
                              {
                                  cvReleaseImage (&cvTempLoadImg);    // L'immagine ha dimensioni errate
                                  return 0;
                              }                       
                         }
                         else    // L'immagine non è a 8 bit per pixel
                         {
                             cvReleaseImage (&cvTempLoadImg);
                             return 0;
                         }                        
                  }
                  else   // L'immagine non è monocromatica
                  {
                         cvReleaseImage (&cvTempLoadImg);
                         return 0;
                  }
             }
             else    // Il sistema non è riuscito a caricare l'immagine in modo corretto
             {
                  return 0;   // Segnala l'invalidità alla routine chiamante
             }        
         }
         else              // Non c'è un campione immagine
             return 0;              
     }
     
     // Se il sistema arriva qui vuol dire che tutti i controlli hanno
     // avuto esito positivo
     
     return 1;       // I dati nello slot sono validi
}



/*
   ----------------------------------------------------------------------------
   CARICAMENTO DI TUTTI I DATI RELATIVI AD UNO SLOT
   ----------------------------------------------------------------------------
*/ 

int baLoad_LoadSampleImagesOfSlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int SaveCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    char Stringa [256];
    int  FileHandle, Counter;
    
    if (INTERNAL_DataInSlotAreValid (NrSlot))
    {
        // Segnala il tentativo di caricare i dati    
        if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, LOADING_IMG_SAMPLE, InfoPacket);
            
        // Legge il nome dell'utente dal NameFile
        
        genstr_NameOfNameFile (NrSlot, &Stringa);
        FileHandle = fopen (&Stringa, "r");
        
        fscanf (FileHandle, "%s", &(facerec_ImagesInRam [NrSlot].Name));
        fclose (FileHandle);
        
        // Copia nella memoria di sistema gli images sample che sono necessari
        
        for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
        {
            genstr_NameOfImageSample (NrSlot, Counter, &Stringa);
            
            // Svuota le aree di memoria che devono accogliere le nuove immagini
            // se non sono vuote
            if (facerec_ImagesInRam [NrSlot].Img [Counter]!=0)
                 cvReleaseImage ( &(facerec_ImagesInRam [NrSlot].Img [Counter]) );
                 
            // Provvedi a caricare i dati dal disco 
            facerec_ImagesInRam [NrSlot].Img [Counter] = cvLoadImage (&Stringa, 0);
        }                      
        
        // Segnala che questo slot adesso è impegnato
        facerec_ImagesInRam [NrSlot].IsActive=1;
        
        // Aggiorna i dati presenti nel face recognizer
        baUpdateFaceRecognizerDBase (NrSlot); 
        
        // Segnala che i dati sono stati caricati   
        if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, LOADED_IMG_SAMPLE, InfoPacket);
        
        return 0;
    }
    else   // Slot vuoto o dati invalidi  
    {
        // Segnala che lo slot è vuoto o che i dati non sono validi    
        if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, ERROR_IN_LOADING_IMG_SAMPLE, InfoPacket);
              
        return -1;
    }
}



/*
   ----------------------------------------------------------------------------
   CARICAMENTO DEI DATI DA TUTTI GLI SLOTS DEL SISTEMA
   ----------------------------------------------------------------------------
*/ 


int baLoadAllSampleImagesOfTheSystem (void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int SaveCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    int Counter, ErrRep;
    
    // Segnala che stai per iniziare il caricamento di tutte le immagini    
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(0, BEGIN_TO_LOAD_IMG_SAMPLE, InfoPacket);
        
    for (Counter=0; Counter<NR_SLOTS_FACE_RECOGNIZER; Counter++)
    {
        ErrRep=baLoad_LoadSampleImagesOfSlot (Counter, GraphCallback, InfoPacket);
        // codice di errore viene skippato
    }
    
    // Segnala che stai hai completato il caricamento di tutte le immagini    
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(0, DONE3, InfoPacket);
    
    return 0;
}

#endif

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
   ROUTINES DI SALVATAGGIO DEI DATI
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
   CODICE: GENERAZIONE NUOVI CAMPIONI AUDIO
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION

int baSave_GenerateANewAudioSample (int NrSlot, int NrSample, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int SaveCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    char NameFile0 [256];
    char VoiceMsg0 [1024];
    
    // Richiama la callback grafica con il codice che indica che stiamo per salvare i samples audio
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, NrSample, SAVING_AUDIO_SAMPLE, InfoPacket);                        
       
    // Genera il nome del sample .wav 
    
    genstr_NameOfVoiceSample (NrSlot, NrSample, &NameFile0);
    
    // Genera la stringa per il sintetizzatore vocale
    
    strcpy (VoiceMsg0, &(facerec_ImagesInRam [NrSlot].Name));
    
    switch (NrSample)
    {
        case 0:
        {
             strcat (VoiceMsg0, " is at your left");
             break;
        }   
        
        case 1:
        {
             strcat (VoiceMsg0, " is in front of you");
             break;
        }
        
        case 2:
        {
             strcat (VoiceMsg0, " is at your right");
             break;
        }
    }
    
    // Genera il sample usando il sintetizzatore vocale e salvalo immediatamente sul disco
    
    flite_text_to_speech (VoiceMsg0, VoiceObj, NameFile0);
    
    // Richiama la callback grafica con il codice che indica che stiamo per salvare i samples audio
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, NrSample, SAVED_AUDIO_SAMPLE, InfoPacket);                        
    
    return 0;                                     
}

int baSave_GenerateAllAudioSamplesForTheSlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    int Counter;
    int ErrRep;
    
    for (Counter=0; Counter<VOICE_SAMPLES_FOR_FACE; Counter++)
    {
        ErrRep=baSave_GenerateANewAudioSample (NrSlot, Counter, GraphCallback, InfoPacket);
        if (ErrRep) return -1;
    }                 
    
    return 0;
} 

/*
   ----------------------------------------------------------------------------
   SALVATAGGIO IMMAGINI CAMPIONE
   ----------------------------------------------------------------------------
*/ 

int baSave_SaveSampleImagesOfSlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int SaveCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    char NameFile0 [256];
    int  Counter;
    
    if (facerec_ImagesInRam [NrSlot].IsActive)
    {
        for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
        {
            // Genera il nome del file immagine 
    
            genstr_NameOfImageSample (NrSlot, Counter, &NameFile0);
            
            // Segnala il tentativo di salvare i dati
            
            if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, SAVING_IMG_SAMPLE, InfoPacket);
            
            HGUI_DisableErrorNotification ();
            cvSaveImage (NameFile0, facerec_ImagesInRam [NrSlot].Img [Counter]);
            HGUI_EnableErrorNotification ();  
            
            // Gestisci eventualmente la condizione di errore
            
            if (!HGUI_SystemError) 
            {
               if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, SAVED_IMG_SAMPLE, InfoPacket);                      
            }
            else
            {
               if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERROR_IN_SAVING_IMG_SAMPLE, InfoPacket);
               return -1;         // Segnala l'errore alla routine chiamante
            }            
        }                                    
    }   
    
    return 0;                  
}


/*
   ----------------------------------------------------------------------------
   SALVATAGGIO FILES CHE CONTENGONO I NOMI 
   ----------------------------------------------------------------------------
*/ 

int baSave_SaveNameOfThePersonThatIsASlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int SaveCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    char NameFile0 [256];
    
    int  FileHandle;
    int  Counter;
    
    // Genera il nome del file di testo 
    
    genstr_NameOfNameFile (NrSlot, &NameFile0);
    
    // Segnala il tentativo di salvare il nome file
    
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, SAVING_NAME, InfoPacket);
    
    FileHandle=fopen (&NameFile0, "w");
    
    if (FileHandle>0)
    {
         fprintf (FileHandle, "%s", &(facerec_ImagesInRam [NrSlot].Name));
         fclose (FileHandle);
         
         // Segnala il successo nell'operazione di salvataggio
         if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, SAVED_NAME, InfoPacket); 
         return 0;          
    }
    else    // Non è riuscito a creare il nuovo file
    {
         // Segnalalo all'utente
         if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERROR_IN_SAVING_NAME, InfoPacket);
         return -1;   
    }        
}




/*
   ----------------------------------------------------------------------------
   SALVATAGGIO DI TUTTI I DATI RELATIVI AD UNO SLOT
   ----------------------------------------------------------------------------
*/ 

int baSave_GenerateAndSaveDataRelativeToASlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int SaveCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    int ErrRep;
    
    // Richiama la callback grafica con il codice che indica che stiamo iniziando a salvare
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, BEGIN_TO_SAVE, InfoPacket);                        
    
    // Genera i vari samples audio e salvali sul disco
    ErrRep=baSave_GenerateAllAudioSamplesForTheSlot (NrSlot, GraphCallback, InfoPacket);
    if (ErrRep) return -1;
    
    // Salva i vari campioni come immagini bitmap
    ErrRep=baSave_SaveSampleImagesOfSlot (NrSlot, GraphCallback, InfoPacket);
    if (ErrRep) return -2;
    
    // Crea un file che deve contenere il nome della persona visualizzata
    ErrRep=baSave_SaveNameOfThePersonThatIsASlot (NrSlot, GraphCallback, InfoPacket);
    if (ErrRep) return -3;
    
    // Richiama la callback grafica con il codice che indica che abbiamo salvato con successo
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DONE0, InfoPacket);                        
    
    return 0;
} 

/*
   ----------------------------------------------------------------------------
   SALVATAGGIO DEI DATI DI TUTTI GLI SLOTS DEL SISTEMA
   ----------------------------------------------------------------------------
*/ 

int baSaveAllSampleImagesOfTheSystem (void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    int Counter;
    int ErrRep;
    
    for (Counter=0; Counter<NR_SLOTS_FACE_RECOGNIZER; Counter++)
    {
        ErrRep=baSave_SaveSampleImagesOfSlot (Counter, GraphCallback, InfoPacket);
        if (ErrRep) return -1;                   // Segnala l'errore di salvataggio alla routine chiamante
    }
    
    return 0;
}

#endif

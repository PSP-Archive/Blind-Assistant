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
   CARICAMENTO DEI DATI DA UN FILE CONTAINER
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

char Signature [255];
char Name [33];

IplImage *xload_cvTmpImage [NR_SAMPLES_FOR_FACE];





/*
   ----------------------------------------------------------------------------
   GESTIONE SALVATAGGI
   ----------------------------------------------------------------------------
*/ 

#ifndef PSP1000_VERSION 
 
int baXLoad_LoadTheDataFromTheSlot (int NrSlot, char *NameFile, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int SaveCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    int FileHandle, NrRecordsRead, IsError, ErrRep;
    int RecoveredPosX, RecoveredPosY;
    int Counter, NrFields;
    
    FileHandle=fopen (NameFile, "r");
    
    if (FileHandle>0)
    {
         // Recupera la signature dal file che viene gestito
         
         if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, BEGIN_TO_XLOAD, InfoPacket); 
         if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XLOAD_SIGNATURE, InfoPacket); 
         
         fscanf (FileHandle, "%s\n", &Signature);
         
         if (!strcmp (Signature, BA_FILE_SIGNATURE))
         { 
                 // Recupera le dimensioni dell'immagine
                 
                 if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XLOAD_DIMS, InfoPacket); 
                 
                 fscanf (FileHandle, "%d\n", &RecoveredPosX);
                 fscanf (FileHandle, "%d\n", &RecoveredPosY);
                 
                 if ( (RecoveredPosX==STD_FACEREC__SAMPLEIMG__DIMX) && (RecoveredPosY==STD_FACEREC__SAMPLEIMG__DIMY) )
                 {
                         // Recupera il nome della persona dal file
                         
                         if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XLOAD_NAME, InfoPacket); 
                         
                         fscanf (FileHandle, "%s\n", &(Name) );
                         
                         // Alloca le aree che devono contenere temporaneamente le nuove immagini 
                         // che verranno prelevate 
                         
                         for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
                         {
                              xload_cvTmpImage [Counter]=cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), 8, 1 );  
                         }
                         
                         // Copia i dati relativi all'immagine nel sistema
                         
                         NrFields = STD_FACEREC__SAMPLEIMG__DIMX*STD_FACEREC__SAMPLEIMG__DIMY;
                         
                         for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
                         {
                             if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, XLOAD_IMG_SAMPLE, InfoPacket); 
                             
                             NrRecordsRead = fread (xload_cvTmpImage [Counter]->imageData, NrFields, 1, FileHandle);
                             
                             if (NrRecordsRead==1)
                             {
                                 if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, XLOAD_IMG_SAMPLE_COMPLETE, InfoPacket);                        
                                 IsError=0;
                             }
                             else
                             {
                                 if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, XLOAD_IMG_SAMPLE_ERROR, InfoPacket);
                                 
                                 IsError=-1;
                                 break;
                             }  
                         }
                         
                         if (!IsError)
                         {
                             // Inibisce l'engine video: è importante perchè in questo momento il video engine
                             // thread sta elaborando i dati e non possiamo cambiare le informazioni da lui
                             // gestite senza prima arrestarlo
                             
                             if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XLOAD_INHIBIT_VIDEOENG, InfoPacket);                        
                             
                             baInhibitVideoEngine ();    
                             
                             // Copia i dati dalle aree di memorie temporanee a quelle definitive
                             
                             if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XCOPY_MEM_AREA, InfoPacket);                        
                             
                             for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
                             {
                                if ((facerec_ImagesInRam [NrSlot].Img [Counter])==0) 
                                   facerec_ImagesInRam [NrSlot].Img [Counter] = cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), 8, 1 );                    
                                
                                zcvCopy (xload_cvTmpImage [Counter], facerec_ImagesInRam [NrSlot].Img [Counter]);       
                             } 
                                                     
                             // Disalloca le aree di memoria temporanee
                             
                             if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DEALLOC_MEM_AREA, InfoPacket);                        
                             
                             for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
                             {
                                  if (xload_cvTmpImage [Counter]!=0) cvReleaseImage (&(xload_cvTmpImage [Counter]));  
                             }
                              
                             // Copia la stringa che contiene il nome
                             ndStrCpy ( &(facerec_ImagesInRam [NrSlot].Name), &(Name), 32, 0);
                             facerec_ImagesInRam [NrSlot].IsActive=1;
                             
                             // Chiudi il file di lavoro
                             fclose (FileHandle);
                             
                             // Segnala che stiamo aggiornando i dati del face recognizer
                             if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XLOAD_FACESQ_UPDATE, InfoPacket);  
                             baUpdateFaceRecognizerDBase (NrSlot);                      
                             
                             // Ok, adesso provvedi ad aggiornare i dati interni al sistema
                             ErrRep=baSave_GenerateAndSaveDataRelativeToASlot (NrSlot, GraphCallback, InfoPacket);
                             
                             if (!ErrRep)
                             {
                                // Deinibisce il video engine thread
                             
                                if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XLOAD_DEINHIBIT_VIDEOENG, InfoPacket); 
                                baDeInhibitVideoEngine ();    
                             
                                // Segna il termine dell'operazione
                             
                                if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DONE0, InfoPacket); 
                                return 0;
                             }
                             else  // C'è un problema nella generazione dei dati relativi allo slot
                             {
                                baErase_DeleteDataRelativeToASlot (NrSlot, GraphCallback, InfoPacket);
                                
                                // Segnala l'insuccesso dell'operazione di caricamento
                                if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XLOAD_FAILURE_IN_MS_UPDATE, InfoPacket); 
                                baDeInhibitVideoEngine ();    
                                
                                // Segna il termine dell'operazione
                             
                                if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DONE0, InfoPacket); 
                                return -5;
                             }    
                         }
                         else   // C'è un problema nel caricamento dei dati immagine
                         {
                              // Disalloca le aree di memoria temporanee
                              
                              if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DEALLOC_MEM_AREA, InfoPacket);  
                              
                              for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
                              {
                                  if (xload_cvTmpImage [Counter]!=0) cvReleaseImage ( &(xload_cvTmpImage [Counter]) );  
                              }
                              
                              // Chiudi il file
                              fclose (FileHandle);
                              
                              // Segnala l'insuccesso dell'operazione di caricamento
                              if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XLOAD_GENERAL_FAILURE, InfoPacket); 
                              if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DONE0, InfoPacket); 
                         
                              // Ritorna l'errore alla routine chiamante
                              return -4;
                         }
                          
                 }
                 else   // Le dimensioni del file sono errate
                 {
                     if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, ERROR_IN_XLOAD_DIMS, InfoPacket); 
                     if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DONE0,               InfoPacket); 
                     
                     fclose (FileHandle);
                     return -3;   
                 }
         }
         else   // La signature del file è sbagliata
         {
                if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, ERROR_IN_XLOAD_SIGN, InfoPacket); 
                if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DONE0,               InfoPacket); 
                
                fclose (FileHandle);
                return -2;
         }             
    }
    else
    {
       // if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, ERROR_IN_OPENING, InfoPacket); 
       return -1; 
    }
}

#endif









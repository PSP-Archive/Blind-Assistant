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
   SALVATAGGIO DEI DATI IN UN FILE CONTAINER
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

/*
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION 
 
int baXSave_SaveTheDataInTheSlot (int NrSlot, char *NameFile, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int NrSample, int SaveCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    int FileHandle, NrRecordsWritten, IsError;
    int Counter, NrFields;
    
    FileHandle=fopen (NameFile, "w");
    
    if (FileHandle>0)
    {
         // Copia la signature che caratterizza i containers di questa versione nel file
         
         if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, BEGIN_TO_XSAVE, InfoPacket); 
       
         strcpy (&Signature, BA_FILE_SIGNATURE);
         fprintf (FileHandle, "%s\n", Signature);
         
         // Copia le dimensioni dell'immagine nel file
         
         fprintf (FileHandle, "%d\n", STD_FACEREC__SAMPLEIMG__DIMX);
         fprintf (FileHandle, "%d\n", STD_FACEREC__SAMPLEIMG__DIMY);
         
         // Copia il nome della persona
         
         if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, XSAVING_NAME, InfoPacket); 
         
         fprintf (FileHandle, "%s\n", &(facerec_ImagesInRam [NrSlot].Name) );
         
         // Copia i dati relativi all'immagine nel sistema
         
         NrFields = STD_FACEREC__SAMPLEIMG__DIMX*STD_FACEREC__SAMPLEIMG__DIMY;
         
         for (Counter=0; Counter<NR_SAMPLES_FOR_FACE; Counter++)
         {
             if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, XSAVING_IMG_SAMPLE, InfoPacket); 
             
             NrRecordsWritten = fwrite (facerec_ImagesInRam [NrSlot].Img [Counter]->imageData, NrFields, 1, FileHandle);
             
             if (NrRecordsWritten==1)
             {
                 if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, XSAVED_IMG_SAMPLE, InfoPacket);                        
                 IsError=0;
             }
             else
             {
                 if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, Counter, ERROR_IN_SAVING_IMG_SAMPLE, InfoPacket);
                 IsError=-1;
                 break;
             }  
         }
         
         // Chiudi il file
         fclose (FileHandle);
         
         // Segnala il successo o insuccesso dell'operazione di salvataggio
         if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, DONE0, InfoPacket); 
         return IsError;              
    }
    else
    {
       if (PntToGraphCallback!=0) (*PntToGraphCallback)(NrSlot, 0, ERROR_IN_OPENING, InfoPacket); 
       return -1; 
    }
}

#endif

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
   ROUTINES PER LA GENERAZIONE DEL FILE SYSTEM DI SISTEMA
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
   CODICE
   ----------------------------------------------------------------------------
*/

int baGenFs_GenerateNewDirsOfFS (void *GraphCallback, struct OpInfoPacket_Type *InfoPacket)
{
    void (*PntToGraphCallback)(int NrSlot, int GenCode, struct OpInfoPacket_Type *InfoPacket);
    PntToGraphCallback = GraphCallback;
    
    char NameFile0 [256];
    char NameFile1 [16];
    int  Counter, ErrRep;
    
    // Richiama la callback grafica con il codice che indica che stiamo iniziando a rigenerare
    // la struttura del file system
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(0, BEGIN_TO_GENFS, InfoPacket); 
    
    for (Counter=0; Counter<NR_SLOTS_FACE_RECOGNIZER; Counter++)
    {
         strcpy (NameFile0, "ms0:/BLINDASSISTANT/SLOT");
         _itoa (Counter, &NameFile1, 10);
         strcat (NameFile0, NameFile1);
         
         ErrRep=chdir (&NameFile0);
         
         if (!ErrRep)                // Sono riuscito ad entrare nella directory
         {
            chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
         }
         else                        // Non sono riuscito ad entrare nella directory. Creala
         {
            mkdir (&NameFile0, 777);                  
         }        
         
         // Segnala all'utente se l'operazione di generazione della nuova directory 
         // è andata a buon fine
         if (PntToGraphCallback!=0) (*PntToGraphCallback)(Counter, I_HAVE_DONE_SINGLE_GENFS, InfoPacket); 
    }
    
    // Crea la cartella Updates, se non già presente
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/UPDATES");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella ServicesMessages, se non già presente
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SERVICEMSG");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella AudioCacheForPlaces, se non già presente
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/AUDCACHEFORPLACESMSG");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella linguaggi per l'inglese
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/VDICT_ENGLISH");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella linguaggi per l'italiano
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/VDICT_ITALIAN");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella linguaggi per il tedesco
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/VDICT_GERMAN");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella linguaggi per lo spagnolo
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/VDICT_SPANISH");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella linguaggi per l'olandese
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/VDICT_DUTCH");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella linguaggi ausiliaria
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/VDICT_AUX");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella dizionarii
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/DICT");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella dei file compressi
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/ZIP");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella per le elaborazioni di sistema
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SYSTEM");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Crea la cartella per il controllo vocale
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SPHINX");
    ErrRep=chdir (&NameFile0);
         
    if (!ErrRep)                // Sono riuscito ad entrare nella directory
    {
       chdir (&SysRootPath);    // Ritorna nella directory root di partenza                  
    }
    else                        // Non sono riuscito ad entrare nella directory. Creala
    {
       mkdir (&NameFile0, 777);                  
    }
    
    // Richiama la callback grafica con il codice che indica che abbia terminato di rigenerare
    // la struttura del file system
    if (PntToGraphCallback!=0) (*PntToGraphCallback)(0, DONE2, InfoPacket); 
    
    return 0;
}



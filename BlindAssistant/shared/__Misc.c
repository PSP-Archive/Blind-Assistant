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
   FUNZIONI VARIE
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
   FUNZIONI STRINGA
   ----------------------------------------------------------------------------
*/ 

void genstr_NameOfNameFile (int NrSlot, char *StrDest)
{
    char NameFile0 [256];
    char NameFile1 [16];
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SLOT");
    _itoa (NrSlot, &NameFile1, 10);
    strcat (NameFile0, NameFile1);
    strcat (NameFile0, "/NAME");
    strcat (NameFile0, ".TXT");
    
    // Copia il nome file nella stringa destinazione
    strcpy (StrDest, &NameFile0);
}

void genstr_NameOfVoiceSample (int NrSlot, int NrSample, char *StrDest)
{
    char NameFile0 [256];
    char NameFile1 [16];
    
    // Genera il nome del sample .wav 
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SLOT");
    _itoa (NrSlot, &NameFile1, 10);
    strcat (NameFile0, NameFile1);
    strcat (NameFile0, "/VOICEMSG");
    _itoa (NrSample, &NameFile1, 10);   
    strcat (NameFile0, NameFile1);
    strcat (NameFile0, ".WAV");
    
    // Copia il nome file nella stringa destinazione
    strcpy (StrDest, &NameFile0); 
}

void genstr_NameOfImageSample (int NrSlot, int NrSample, char *StrDest)
{
    char NameFile0 [256];
    char NameFile1 [16];
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SLOT");
    _itoa  (NrSlot, &NameFile1, 10);
    strcat (NameFile0, NameFile1);
    strcat (NameFile0, "/IMGSAMPLE");
    _itoa  (NrSample, &NameFile1, 10);   
    strcat (NameFile0, NameFile1);
    strcat (NameFile0, ".BMP");
 
    // Copia il nome file nella stringa destinazione
    strcpy (StrDest, &NameFile0); 
}

char *ba_fgets (char *buf, int n, FILE *stream)
/*
    Analoga a fgets, ma molto più veloce. E' usata per leggere i dizionari.
    E' inoltre in grado di saltare i tag /ABCD tipici di dizionari come
    Spice
*/
{
  
  // Acquisizione da uno stream normale
  
  register int   c   = 0; 
  register char  dis = 0;
  register char* cs;

  cs = buf;
  
  while (--n>0 && (c = getc(stream)) != EOF) 
  {
    if (c=='/') dis=1;     // Da questo momento non verranno memorizzati caratteri
    
    if ((!dis) && (c!=0x0D) && (c!='\n')) *cs++ = c;
    if (c == '\n') break;
  }
  
  if (c == EOF && cs == buf) return NULL;

  *cs++ = '\0';              
  return buf;              
}

int baSearch (char *buf, char *strtosearch, int start)
/*
    Provvede a cercare una stringa in un buffer, a partire dal
    carattere nr. start. Se viene trovata, viene restituita la posizione nel
    buffer, altrimenti viene restituito -1 
*/
{
    char  strbuffer [512];
    
    int   result, counter;
    int   strlen_to_search;
    char *bufpos;
    
    result  =-1;
    counter = start;
    strlen_to_search = strlen (strtosearch);
    
    while (1)
    {
          if (*(buf+counter)==0)
             break;
             
          ndStrCpyBounded (strbuffer, buf, counter, counter+strlen_to_search-1, 0);
          
          if (!strcmp (strtosearch, strbuffer))
          {
              result=counter;
              break;
          } 
          
          counter++;
    }
    
    return result;
}

 


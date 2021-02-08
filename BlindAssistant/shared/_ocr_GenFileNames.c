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
   ROUTINES PER LA GENERAZIONE DEI NOMI DEI COMPONENTI AUDIO E DIZIONARII
   RELATIVI AL SISTEMA OCR
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


void baOcrFNames_MakeDictPackg_VisilabSvr (char *Name)
{
    // Inserisci l'indirizzo di base del Visilab
    strcpy (Name, &(SystemOptPacket.UpdateFromSite));
                        
    // Aggiungi il path interno al sito del Visilab: è il path in cui si trovano i dizionarii
    strcat (Name, BA_DICTIONARY_PATH);
                        
    // Aggiungi il nome del package zip che contiene il dizionario
    strcat (Name, BA_DICTIONARY_ZIP_NAME);
}



void baOcrFNames_MakeDictPackg_Target (char *Name)
{
    strcpy (Name, "ms0:/BLINDASSISTANT/ZIP/Dictionaries.zip");
}



void baOcrFNames_MakeVDictName_VisilabSvr (char *Name, int LanguageCode)
{
    // Inserisci l'indirizzo di base per gli aggiornamenti
    strcpy (Name, &(SystemOptPacket.UpdateFromSite));
    
    // Aggiungi il nome della path che contiene i dizionari vocali sul server del Visilab
    strcat (Name, BA_VOCAL_DICT_PATH);
    
    // Aggiungi il nome del dizionario voluto
    switch (LanguageCode)
    {
           case OCRLANG_ENGLISH:
           {
                strcat (Name, BA_VOCAL_DICT_ENGLISH);
                break;
           }                           
           
           case OCRLANG_ITALIAN:
           {
                strcat (Name, BA_VOCAL_DICT_ITALIAN);
                break;
           }                           

           case OCRLANG_FRANCOIS:
           {
                strcat (Name, BA_VOCAL_DICT_FRANCOIS);
                break;
           }
                                     
           case OCRLANG_GERMAN:
           {
                strcat (Name, BA_VOCAL_DICT_GERMAN);
                break;
           }     
                                            
           case OCRLANG_SPANISH:
           {
                strcat (Name, BA_VOCAL_DICT_SPANISH);
                break;
           }                           

           case OCRLANG_DUTCH:
           {
                strcat (Name, BA_VOCAL_DICT_DUTCH);
                break;
           }                                  

    }
}


void baOcrFNames_MakeVDictName_Target (char *Name, int LanguageCode)
{
    // Inserisci il percorso target
    strcpy (Name, "ms0:/BLINDASSISTANT/ZIP/");
    
    // Aggiungi il nome del dizionario voluto
    switch (LanguageCode)
    {
           case OCRLANG_ENGLISH:
           {
                strcat (Name, BA_VOCAL_DICT_ENGLISH);
                break;
           }                           
           
           case OCRLANG_ITALIAN:
           {
                strcat (Name, BA_VOCAL_DICT_ITALIAN);
                break;
           }                           

           case OCRLANG_FRANCOIS:
           {
                strcat (Name, BA_VOCAL_DICT_FRANCOIS);
                break;
           }
                                     
           case OCRLANG_GERMAN:
           {
                strcat (Name, BA_VOCAL_DICT_GERMAN);
                break;
           }     
                                            
           case OCRLANG_SPANISH:
           {
                strcat (Name, BA_VOCAL_DICT_SPANISH);
                break;
           }                           

           case OCRLANG_DUTCH:
           {
                strcat (Name, BA_VOCAL_DICT_DUTCH);
                break;
           }                                  

    }
}


void baOcrFNames_MakeVDictNameSubPckg_Src (char *Name, char Letter, int LanguageCode)
{
    char Buffer [3];
    
    // Inserisci il percorso dove si trovano i subpackages
    strcpy (Name, "ms0:/BLINDASSISTANT/ZIP/");
    
    // Aggiungi un nome parziale che tiene conto del linguaggio
    switch (LanguageCode)
    {
           case OCRLANG_ENGLISH:
           {
                strcat (Name, "_ENG_");
                break;
           }                           
           
           case OCRLANG_ITALIAN:
           {
                strcat (Name, "_ITA_");
                break;
           }                           

           case OCRLANG_FRANCOIS:
           {
                strcat (Name, "_FRA_");
                break;
           }
                                     
           case OCRLANG_GERMAN:
           {
                strcat (Name, "_GER_");
                break;
           }     
                                            
           case OCRLANG_SPANISH:
           {
                strcat (Name, "_SPA_");
                break;
           }                           

           case OCRLANG_DUTCH:
           {
                strcat (Name, "_DUT_");
                break;
           }                                  
    }
    
    // Aggiungi la lettera richiesta
    Buffer [0]=toupper (Letter);
    Buffer [1]=0;
    strcat (Name, Buffer);
    
    // Aggiungi il suffisso .ZIP
    strcat (Name, ".ZIP");
} 


void baOcrFNames_GenerateVDictDir (char *Name, int LanguageCode)
{
    char UpName [255];
    char ShortString [2];
    char FirstChar;
    int  Counter, StrLen;
    
    strcpy (Name, "ms0:/BLINDASSISTANT");
    
    switch (LanguageCode)
    {
         case OCRLANG_ENGLISH:
         { 
              strcat (Name, "/VDICT_ENGLISH");
              break;
         }
         
         case OCRLANG_ITALIAN:
         {
              strcat (Name, "/VDICT_ITALIAN");
              break;
         }
         
         case OCRLANG_FRANCOIS:
         {
              strcat (Name, "/VDICT_FRANCOIS");
              break;
         }
         
         case OCRLANG_GERMAN:
         {
              strcat (Name, "/VDICT_GERMAN");
              break; 
         }
         
         case OCRLANG_SPANISH:
         {
              strcat (Name, "/VDICT_SPANISH");
              break; 
         }
         
         case OCRLANG_DUTCH:
         {
              strcat (Name, "/VDICT_DUTCH");
              break; 
         }        
    }   
}



void baOcrFNames_GenerateNameOfDictionary (char *Name, int LanguageCode)
{
    switch (LanguageCode)
    {
         case OCRLANG_ENGLISH:
         {
              strcpy (Name, "ms0:/BLINDASSISTANT/DICT/english.med");
              break;
         }
         
         case OCRLANG_ITALIAN:
         {
              strcpy (Name, "ms0:/BLINDASSISTANT/DICT/italian.med");
              break;
         }     
                   
         case OCRLANG_FRANCOIS:
         {
              strcpy (Name, "ms0:/BLINDASSISTANT/DICT/francais.med");
              break;
         }      
                   
         case OCRLANG_GERMAN:
         {
              strcpy (Name, "ms0:/BLINDASSISTANT/DICT/german.med");
              break;
         }     
                   
         case OCRLANG_SPANISH:
         {
              strcpy (Name, "ms0:/BLINDASSISTANT/DICT/espanol.med");
              break;
         }    
              
         case OCRLANG_DUTCH:
         {
              strcpy (Name, "ms0:/BLINDASSISTANT/DICT/dutch96.med");
              break;
         }
    } 
}



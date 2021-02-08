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
   ROUTINES DI INIZIALIZZAZIONE
   ----------------------------------------------------------------------------
*/




        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        
        #include "BlindAssistant.h"
        #include <ndsphinx_api.h>



int INTERNAL_CheckTheVersionOfTheFirmware ()
{
    int Version = (int)ndINFO_HAL_GetValue (NDGET_EMULATEDFIRMWAREVERSION, 0, 0, 0);
    
    if (Version<=0x03070110)
    {
         baitf_ErrorFirmwareTooOld ();
         return -1;                   
    } 
    
    return 0;
}

void INTERNAL_InitVoiceSynth (void)
{
     flite_init();
     
     #ifndef PSP1000_VERSION
             VoiceObj = register_cmu_us_kal16 ();
     #else
             VoiceObj = register_cmu_us_kal ();         // For PSP-1000
     #endif
} 

void INTERNAL_InitGCurlLibrary (void)
{
     ndGCURL_GlobalInit ();
}

void INTERNAL_InitPop3Library (void)
{
     pop3_showerrors (0);     // Disabilita la notifica automatica degli errori POP3
}

int INTERNAL_InitFaceDetectVars (void)
{
    #ifndef PSP1000_VERSION
        
        char NameTrainingSet [512];
        
        FDCascade = 0;
        FDStorage = 0;
        
        // Iniziamo verificando quale file di training è presente nel sistema
        
        strcpy (NameTrainingSet, "ms0:/BLINDASSISTANT/CVHAAR/haarcascade_frontalface_default.xml");
        
        if (!fexist (NameTrainingSet))
        {
               // Il file di training set più nuovo non è presente nel sistema. Ok, proviamo con
               // il file di training set classico
               
               strcpy (NameTrainingSet, "ms0:/BLINDASSISTANT/CVHAAR/haarcascade_frontalface_alt.xml");
               
               if (fexist (NameTrainingSet))
               {
                     // Ok, almeno c'è il file di training set più vecchio. Rispondi con una
                     // segnalazione all'utente (salvo che essa non sia stata disabilitata
                     // nelle opzioni)
                     
                     if (SystemOptPacket.CheckForOldTS_FaceDetect) baitf_YouAreUsingOldVersionOfCascadeFile ();
                     
                     // ... e poi prosegui
               }
               else
               {
                     // Non esiste nessuno dei due training set previsti. Segnalazione di errore
                     // ed esci dal programma
                     
                     baitf_ErrorCannotFindHaarCascadeTrainingSet ();
                     return -1;                          // Segnala la necessità di interrompere
               }
        }
        
        // Tentiamo di caricare il training set trovato nella fase precedente
        
        FDCascade = (CvHaarClassifierCascade*)cvLoad (NameTrainingSet, 0, 0, 0 );
        
        if (FDCascade>0)
        {
             return 0;
        }
        else
        {
             baitf_ErrorInLoadingHaarCascadeFile ();
             return -2;                          // Segnala la necessità di interrompere
        }	  
        
    #else
    
        return 0;      // Face recognizer non supportato sotto PSP-1000
        
    #endif 
}

int INTERNAL_InitCamera (void)
{
     int ErrRep;
     int CameraHandle;
     
     __TryToInitCamera:
                      
         HGUI_DisableErrorNotification ();
         cvCameraCaptureObj = cvCaptureFromCAM (-1); 
         HGUI_EnableErrorNotification ();
         
         if (cvCameraCaptureObj>0)    // Non ci sono stati errori
         {
             return 0;                   
         }             
         else    // Errore o videocamera non collegata
         {
             ErrRep=baitf_ErrorInCameraConnect ();
             if (ErrRep==BA_RETRY) goto __TryToInitCamera;           // L'utente ha scelto di tentare di ricollegare la cam
             if (ErrRep==BA_ABORT) goto __InitCam_SignalUserAbort;                  
         }   
         
     __InitCam_SignalUserAbort:
         return -1;
}
 
 
int INTERNAL_EnableVoiceRecEngine (void)
{
    #ifndef PSP1000_VERSION

            char DecoderOption [1024];
            int  ErrRep;
            
            __ReTryToActivateVocalSystem:
            
                if ( ndPSphinx_ThePackageIsOk() )
                {    
                    
                    ndPSphinx_ShowCurrentConfiguration (0);
                    ndPSphinx_ShowInfoMessages (0);
                    ndPSphinx_ShowErrorMessages (0);
                    
                    sprintf (&DecoderOption, "-fwdflat yes -bestpath yes -lm %s -dict %s -hmm %s -samprate %d -nfft 256 -mmap no",
                                                 BA_VOICECTRL_LANGUAGE_MODEL, 
                                                 BA_VOICECTRL_DICTIONARY, 
                                                 BA_VOICECTRL_HMMDIR, 8000);
                
                    ErrRep=ndPSphinx_InitDecoder (DecoderOption);
                
                    if (!ErrRep)
                    {
                        // Tutto ok. Prosegui nel programma
                        return 0;        
                    }
                    else
                    {
                        // Errore di inizializzazione: fermare il programma
                        baitf_ErrorInSphinxInitialization ();
                        return -1;
                    }
                    
                }
                else
                {
                    // Il pacchetto di gestione del sistema vocale è errato oppure alterato
                    // rispetto alla versione
                    
                    __ReTryToRecoverVocalPackage:
                    
                        ErrRep=SYSFIXER01_DownloadHmmVocalPackage ();
                        
                        // E' stato possibile risolvere il problema. Riprova ad attivare il sistema vocale
                        if (ErrRep==0) goto __ReTryToActivateVocalSystem;
                        
                        // Non è stato possibile risolvere il problema. Comunica il fatto all'utente con un
                        // messaggio in modo da avere altre disposizioni
                        ErrRep=baitf_ErrorInDownloadingHmmPackage ();
                        
                        if (ErrRep==BA_RETRY) goto __ReTryToRecoverVocalPackage;           // L'utente ha scelto di tentare di ricollegare la cam
                        
                        // Se si arriva qui vuol dire che l'utente ha chiesto l'interruzione forzata del programma
                        return -1;
                }
            
    #else
                return 0;     // Riconoscimento vocale non supportato sotto PSP FAT
    #endif
} 
 
 
void INTERNAL_InitMouse (void)
{
     ndHAL_EnableMousePointer ();
} 
 

void INTERNAL_InitXPCAImages (void)
/*
     Questa routine reinizializza la struct XPCA
*/
{
    int CounterA, CounterB;
    
    for (CounterA=0; CounterA<NR_SLOTS_FACE_RECOGNIZER; CounterA++)
    {
        facerec_ImagesInRam [CounterA].IsActive = 0;
        
        // Azzera i puntatori immagine
        
        for (CounterB=0; CounterB<NR_SAMPLES_FOR_FACE; CounterB++)
        {
            facerec_ImagesInRam [CounterA].Img [CounterB] = 0;
        }
        
        // Azzera i registri di presenza dell'immagine
        
        for (CounterB=0; CounterB<NR_PRESENCE_ELEMENTS_IN_REG; CounterB++)
        {
            facerec_ImagesInRam [CounterA].Presence [CounterB].ThereWas = 0;
        }
    }
    
    cvNrInputImages=0;
}

void INTERNAL_InitDictionarySystem (void)
{
    DictIsOperative=0; 
}

void INTERNAL_SetupTesseractFolder (void)
{
    #ifndef PSP1000_VERSION
            setenv ("TESSDATA_PREFIX", "ms0:/BLINDASSISTANT/", 1); 
    #endif
}

void INTERNAL_InitMailCheckerVariables (void)
{
    // All'inizio del programma, si presume che sul server email non ci
    // siano messaggi
    
    baMailChecker_HyphActualMailMessages = 0;
    baMailChecker_NrTimesAlreadySignaled = 0;
}

void INTERNAL_InitMicroIconManager (void)
{
     baMicroIcons_Register = 1;
}

int baInit (void)
{
     int ErrRep;
     
     cvInitSystem (0, 0);
     
     INTERNAL_InitVoiceSynth ();
     INTERNAL_InitGCurlLibrary ();
     INTERNAL_InitPop3Library ();
     
     ErrRep=INTERNAL_CheckTheVersionOfTheFirmware ();
     if (ErrRep) return -1;
     
     ErrRep=INTERNAL_InitFaceDetectVars ();
     if (ErrRep) return -1;
      
     ErrRep=INTERNAL_InitCamera ();
     if (ErrRep) return -1;
     
     INTERNAL_InitMouse ();
     INTERNAL_InitXPCAImages ();
     INTERNAL_InitDictionarySystem ();
     INTERNAL_SetupTesseractFolder ();
     INTERNAL_InitMailCheckerVariables ();
     INTERNAL_InitMicroIconManager ();
     
     InitFileSystem ();
     InitAudioMessages ();
     InitOptions ();
     InitIcons ();
     
     ErrRep=INTERNAL_EnableVoiceRecEngine ();    // Abilita il sistema di riconoscimento vocale
     if (ErrRep) return -1;  
     
     return 0;                   // Segnala alla routine main che si vuole proseguire        
} 
 
 

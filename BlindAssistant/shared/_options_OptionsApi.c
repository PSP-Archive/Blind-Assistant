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
   GESTORE OPZIONI
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
   VARIABILI INTERNE
   ----------------------------------------------------------------------------
*/



static char Signature [256];

/*
   ----------------------------------------------------------------------------
   ROUTINES DI SALVATAGGIO DEL PACCHETTO OPZIONI
   ----------------------------------------------------------------------------
*/

int  INTERNAL_SaveOptionsPacket (void)
{
     int FileHandle, NrRecordsWritten;
     
     // Per prima cosa riportiamo a 0 questo parametro. Esso deve stare ad 1 solo per
     // segnalare che c'è un nuovo cambiamento da registrare. Sul disco viene sempre
     // registrato pari a 0 (e viene recuperato dal disco sempre pari a 0).
     
     SystemOptPacket.AnOptionHasBeenChanged=0;    
     
     // Cerca di aprire il file per salvarlo
     
     FileHandle = fopen (BA_OPTIONS_FILE, "w");
     
     if (FileHandle>0)
     {
         fprintf (FileHandle, "%s\n", BA_OPT_SIGNATURE);
         NrRecordsWritten = fwrite (&SystemOptPacket, sizeof (SystemOptPacket), 1, FileHandle);             
         fclose (FileHandle);
     
         if (NrRecordsWritten==1) return 0;
            else return -1;    
     }
}

/*
   ----------------------------------------------------------------------------
   GESTIONE COLLEGAMENTO INIZIALE AD INTERNET E CON IL BLINDSERVER
   ----------------------------------------------------------------------------
*/

void baOpt_EnableDisableConnectToNetAtStartup (char IsEnabled)
{
    SystemOptPacket.ConnectToNetAtStartup = IsEnabled;
    SystemOptPacket.AnOptionHasBeenChanged =1; 
    return; 
}

void baOpt_EnableDisableConnectToBldSvrAtStartup (char IsEnabled)
{
    SystemOptPacket.ConnectToBldSvrAtStartup = IsEnabled;
    SystemOptPacket.AnOptionHasBeenChanged =1; 
    return;  
}


/*
   ----------------------------------------------------------------------------
   GESTIONE WALLPAPER
   ----------------------------------------------------------------------------
*/


void baOpt_LoadTheNewWallPaper (void)
{
    char ErrRep;
    
    if (SystemOptPacket.EnableWallPaper)
    {
        ErrRep=ndLP_LoadWallPaperFromFile (&SystemOptPacket.WallPaperName); 
        
        if (!ErrRep) 
           SystemOptPacket.NewWallPaperIsFailure=0;
        else
           SystemOptPacket.NewWallPaperIsFailure=1;
        
        return;                                
    }
    else
    {
        ndLP_DisableWallPaper ();
        return;
    }
}             

void baOpt_EnableDisableWallPaper (void)
{
    if ((SystemOptPacket.EnableWallPaper) && (!SystemOptPacket.NewWallPaperIsFailure))
    {
        ndLP_EnableWallPaper ();
        return;                                
    }
    else
    {
        ndLP_DisableWallPaper ();
        return;
    } 
}

char baOpt_ChangeWallPaper (char *Name)
{
    static struct ndImage_Type TryImage;
    char ErrRep;
    
    // Verifica se la configurazione attuale del sistema consente di caricare
    // questa immagine
    
    ErrRep=ndIMG_LoadImage (&TryImage, Name, NDMGKNB);
    
    if (!ErrRep)           // Ok, l'immagine è caricabile
    {
       ndIMG_ReleaseImage (&TryImage);     // Libera la memoria utilizzata
       
       strcpy (&SystemOptPacket.WallPaperName, Name);
       SystemOptPacket.EnableWallPaper        =1;
       SystemOptPacket.AnOptionHasBeenChanged =1;
       
       baOpt_LoadTheNewWallPaper ();
       return 0;                    
    }
    else                   // L'immagine non è caricabile
    {
       // Non toccare il flag di abilitare del wallpaper: se era abilitato lo
       // devi lasciare sulla vecchia immagine
       
       ndIMG_ReleaseImage (&TryImage);     // Libera la memoria utilizzata
       return -1;                          // Segnala il problema alla routine chiamante 
    } 
}

/*
   ----------------------------------------------------------------------------
   GESTIONE DOWNLOAD AGGIORNAMENTI
   ----------------------------------------------------------------------------
*/

void baOpt_ChangeDownloadSite (char *NewSite)
{
     strcpy ( &SystemOptPacket.UpdateFromSite, NewSite );
     SystemOptPacket.AnOptionHasBeenChanged =1; 
     return;
}

void baOpt_ChangeMinBetweenChecks (int Minutes)
{
     SystemOptPacket.MinBetweenChecks = Minutes;
     SystemOptPacket.AnOptionHasBeenChanged =1; 
     return;
}

void baOpt_EnableDisableHttpTunneling (char IsEnabled)
{
     SystemOptPacket.HttpTunneling          =IsEnabled;
     SystemOptPacket.AnOptionHasBeenChanged =1; 
     return; 
}

void baOpt_EnableDisableBeVerbose (char IsEnabled)
{
     SystemOptPacket.BeVerbose              =IsEnabled;
     SystemOptPacket.AnOptionHasBeenChanged =1; 
     return;
}

void baOpt_ChooseConnectionProfile (char Profile)
{
     SystemOptPacket.FwNetworkProfileChoosen =Profile;
     SystemOptPacket.AnOptionHasBeenChanged  =1; 
     return;
}


void baOpt_EnableDisableChecksForUpdate (char Enable)
{
     int PreviousValue;
     
     // Attendi fin quando il gestore di rete non si è liberato
     
     while (NetworkIsBusy)
     {   sceKernelDelayThread (1000000);   }
     
     if (Enable)           // Hanno chiesto l'attivazione
     {
        PreviousValue = SystemOptPacket.CheckForUpdate;
        SystemOptPacket.CheckForUpdate=1;
        
        // L'updater engine era disattivato. Riattivalo
        if (PreviousValue == 0) sceKernelWakeupThread (updengThreadID);   
     }
     else                   // Hanno chiesto la disattivazione
     {
        SystemOptPacket.CheckForUpdate=0;
        ndHAL_HardDelay (1);                    
     }      
     
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_EnableDisableAutoDownload (char Enable)
{
     SystemOptPacket.AutoDownloadNewUpdates=Enable;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_EnableDisableAutoInstall (char Enable)
{
     SystemOptPacket.AutoInstallNewUpdates=Enable;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_EnableDisableChecksForNewMails (char Enable)
{
     int PreviousValue;
     
     // Attendi fin quando il gestore di rete non si è liberato
     
     while (NetworkIsBusy)
     {   sceKernelDelayThread (1000000);   }
     
     if (Enable)           // Hanno chiesto l'attivazione
     {
        PreviousValue = SystemOptPacket.CheckForNewMails;
        SystemOptPacket.CheckForNewMails=1;
        
        // Il motore di controllo delle email era disattivato. Riattivalo
        if (PreviousValue == 0) sceKernelWakeupThread (mailchkThreadID);   
     }
     else                   // Hanno chiesto la disattivazione
     {
        SystemOptPacket.CheckForNewMails=0;
        ndHAL_HardDelay (1);                    
     }      
     
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeMinBetweenChecksForMails (int Minutes)
{
     SystemOptPacket.MinBetweenChecksForMails = Minutes;
     SystemOptPacket.AnOptionHasBeenChanged =1; 
     return;
}

/*
   ----------------------------------------------------------------------------
   GESTIONE OPZIONI DELL'ENGINE VIDEO
   ----------------------------------------------------------------------------
*/

void baOpt_ChangeMinBrightnessLevel (float MinLevel)
{
     SystemOptPacket.MinBrightnessLevel=MinLevel;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeSkipThreshold (float Level)
{
     SystemOptPacket.SkipThreshold=Level;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeFaceDetectorProbingArea (float Level)
{
     SystemOptPacket.FaceDetectorProbingArea=Level;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeFaceDetectorThreshold (float Level)
{
     SystemOptPacket.FaceDetectorThreshold=Level;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}


void baOpt_CheckForOldTS_FaceDetect (int ActualValue)
{
     SystemOptPacket.CheckForOldTS_FaceDetect=ActualValue;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

/*
   ----------------------------------------------------------------------------
   GESTIONE OPZIONI BLIND SERVER
   ----------------------------------------------------------------------------
*/

void baOpt_ChangeBlindServerIPAddr (char *IPAddr)
{
     strcpy ( &(SystemOptPacket.BlindServerAddrStr), IPAddr );
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeBlindServerPort (int Port)
{
     SystemOptPacket.BlindServerPort=Port;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeBlindServerPasswd (char *Passwd)
{
     strcpy ( &(SystemOptPacket.Password), Passwd );
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

/*
   ----------------------------------------------------------------------------
   GESTIONE OPZIONI OCR
   ----------------------------------------------------------------------------
*/

void baOpt_ChangeOcrLanguage (int LanguageCode)
{
     SystemOptPacket.OcrLanguage=LanguageCode;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeOcrMethod (int MethodCode)
{
     SystemOptPacket.OcrMethod=MethodCode;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeRTGenerateWords (int ActualValue)
{
     SystemOptPacket.OcrRTGenerateWords=ActualValue;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangeSkipUnknownWords (int NewValue)
{
     SystemOptPacket.OcrSkipUnknownWords=NewValue;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

/*
   ----------------------------------------------------------------------------
   GESTIONE OPZIONI MAIL CLIENT
   ----------------------------------------------------------------------------
*/

     
void baOpt_ChangePop3Addr (char *NewAddr)
{
     strcpy (SystemOptPacket.Pop3ServerAddr, NewAddr);
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangePop3UserName (char *NewUserName)
{
     strcpy (SystemOptPacket.Pop3UserName, NewUserName);
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangePop3Password (char *NewPassword)
{
     strcpy (SystemOptPacket.Pop3Password, NewPassword);
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}

void baOpt_ChangePop3Port (int NewPort)
{
     SystemOptPacket.Pop3Port = NewPort;
     SystemOptPacket.AnOptionHasBeenChanged=1; 
}


/*
   ----------------------------------------------------------------------------
   CODICE: INIZIALIZZAZIONE AI VALORI DI DEFAULT DEL PACCHETTO OPZIONI
   ----------------------------------------------------------------------------
*/

void INTERNAL_DEFTOPT_NetworkOptions (void)
{
     SystemOptPacket.ConnectToNetAtStartup=1;
     SystemOptPacket.ConnectToBldSvrAtStartup=1;
}

void INTERNAL_DEFTOPT_WallPaper (void)
{
     SystemOptPacket.EnableWallPaper=0;
     strcpy (&(SystemOptPacket.WallPaperName), "");
}

void INTERNAL_DEFTOPT_CheckForUpdate (void)
{
     SystemOptPacket.CheckForUpdate=1;
     strcpy ( &(SystemOptPacket.UpdateFromSite), BA_SITE );
     
     SystemOptPacket.MinBetweenChecks=30;
     SystemOptPacket.AutoInstallNewUpdates=1;
     SystemOptPacket.AutoDownloadNewUpdates=1;
     
     SystemOptPacket.HttpTunneling=0;
     SystemOptPacket.BeVerbose=0;
     SystemOptPacket.FwNetworkProfileChoosen=0;
}

void INTERNAL_DEFTOPT_VideoEngine (void)
{
     SystemOptPacket.MinBrightnessLevel       = MIN_BRIGHTNESS_THRESHOLD_DEFAULT;
     SystemOptPacket.SkipThreshold            = PCA_SKIP_THRESHOLD_DEFAULT;
     SystemOptPacket.FaceDetectorProbingArea  = FACEDETECTOR_PROBING_AREA_DEFAULT;
     SystemOptPacket.FaceDetectorThreshold    = FACEDETECTOR_THRESHOLD_DEFAULT;
     
     SystemOptPacket.CheckForOldTS_FaceDetect = 1;
}

void INTERNAL_DEFTOPT_BlindServer (void)
{
     strcpy ( &(SystemOptPacket.BlindServerAddrStr), "192.168.0.1");
     SystemOptPacket.BlindServerPort=44075;
     
     memset ( &(SystemOptPacket.Password), 0, 32 );
}

void INTERNAL_DEFTOPT_OcrSystem (void)
{
     SystemOptPacket.OcrMethod        =OCRMETHOD_TESSERACT_VIA_BLINDSERVER;
     SystemOptPacket.OcrLanguage      =OCRLANG_ENGLISH;
     SystemOptPacket.OcrSkipUnknownWords =0;
     SystemOptPacket.OcrRTGenerateWords = 1;           // attivo per default
}

void INTERNAL_DEFTOPT_MailClient (void)
{     
     strcpy ( SystemOptPacket.Pop3ServerAddr, "");
     strcpy ( SystemOptPacket.Pop3UserName, "");
     strcpy ( SystemOptPacket.Pop3Password, "");
     SystemOptPacket.Pop3Port = 110;
     
     SystemOptPacket.CheckForNewMails = 1;
     SystemOptPacket.MinBetweenChecksForMails = 5;
}


void INTERNAL_ResetTheNewDefaultOptionsFile (void)
{
     INTERNAL_DEFTOPT_NetworkOptions ();
     INTERNAL_DEFTOPT_WallPaper ();
     INTERNAL_DEFTOPT_CheckForUpdate ();
     INTERNAL_DEFTOPT_VideoEngine ();
     INTERNAL_DEFTOPT_BlindServer ();
     INTERNAL_DEFTOPT_OcrSystem ();
     INTERNAL_DEFTOPT_MailClient ();
}


/*
   ----------------------------------------------------------------------------
   CODICE: RIPRISTINO DELLE OPZIONI RECUPERATE DAL DISCO
   ----------------------------------------------------------------------------
*/


void INTERNAL_RECOVERYOLD_WallPaper (void)
{
    baOpt_LoadTheNewWallPaper ();  
}

void INTERNAL_RECOVERYOLD_CheckForUpdate (void)
{ 
   // Nulla da fare, tanto questa routine sarà richiamata all'inizio, quando ancora
   // l'update engine non è stato attivato. La variabile SystemOptPacket.CheckForUpdate
   // è stata già settata ai valori standard e farà il suo lavoro senza che sia
   // necessario attivare funzioni specifiche  
}

void INTERNAL_RECOVERYOLD_VideoEngine (void)
{
   // Nulla da fare   
}

void INTERNAL_RECOVERYOLD_BlindServer (void)
{
   // Nulla da fare  
}

void INTERNAL_RECOVERYOLD_OcrSystem (void)
{
   // Nulla da fare  
}

void INTERNAL_RECOVERYOLD_MailClient (void)
{
   // Nulla da fare  
}

void INTERNAL_RecoverOldOptionsConfig (void)
{
    INTERNAL_RECOVERYOLD_WallPaper ();        // Ripristina la vecchia configurazione del wallpaper
    INTERNAL_RECOVERYOLD_CheckForUpdate ();   // Avvia eventualmente il server per l'autoaggiornamento
    INTERNAL_RECOVERYOLD_VideoEngine ();      // Setta le opzioni di base per il videoengine
    INTERNAL_RECOVERYOLD_BlindServer ();      // Setta le opzioni di base per il BlindServer
    INTERNAL_RECOVERYOLD_OcrSystem ();        // Setta le opzioni di base per il sistema OCR
    INTERNAL_RECOVERYOLD_MailClient ();       // Setta le opzioni di base per il client email
}

void INTERNAL_ForceOptionsToLegalValues (void)
{
    // Questa routine forza alcuni valori delle opzioni a valori che sono obbligati
    // dopo i cambiamenti eseguiti con BlindAssistant CFW0006. Viene richiamata
    // dopo il ripristino dei valori da parte del vecchio file delle opzioni
    
    #ifdef PSP1000_VERSION
           SystemOptPacket.OcrMethod        = OCRMETHOD_TESSERACT_VIA_BLINDSERVER;
           
           SystemOptPacket.EnableWallPaper  = 0;
           strcpy (&(SystemOptPacket.WallPaperName), "");
    #endif
}


/*
   ----------------------------------------------------------------------------
   CODICE: INIZIALIZZAZIONE GESTIONE DELLE OPZIONI
   ----------------------------------------------------------------------------
*/

void InitOptions ()
{
     int FileHandle, NrRecordsRead;

     // Tenta di caricare il file delle opzioni
     
     if (fexist (BA_OPTIONS_FILE))
     {
        if ( flen (BA_OPTIONS_FILE)==( sizeof (SystemOptPacket)+strlen (BA_OPT_SIGNATURE)+1 ) )
        {    
            FileHandle = fopen (BA_OPTIONS_FILE, "r");
            
            if (FileHandle>0)    // Il file delle opzioni esiste già
            {
                fscanf (FileHandle, "%s\n", &Signature);
             
                if (!strcmp (Signature, BA_OPT_SIGNATURE))
                { 
                       NrRecordsRead = fread (&SystemOptPacket, 1, sizeof (SystemOptPacket), FileHandle);
                       
                       if (NrRecordsRead==sizeof (SystemOptPacket))         // Nessun errore
                       {
                           fclose (FileHandle);
                           
                           INTERNAL_RecoverOldOptionsConfig ();      // RIPRISTINO VECCHIE INFORMAZIONI
                           INTERNAL_ForceOptionsToLegalValues ();         
                           return;
                       }
                       else      // Problemi nel leggere il file delle opzioni: ripristinalo
                       {
                           fclose (FileHandle);
                           
                           INTERNAL_ResetTheNewDefaultOptionsFile ();
                           INTERNAL_SaveOptionsPacket ();
                           return;                          
                       }               
                }
                else   // La signature del file delle opzioni è errata
                {
                       fclose (FileHandle);
                       
                       INTERNAL_ResetTheNewDefaultOptionsFile ();
                       INTERNAL_SaveOptionsPacket ();
                       return;
                }
            } 
            else   // Problemi a creare l'handle
            {
                INTERNAL_ResetTheNewDefaultOptionsFile ();     
                INTERNAL_SaveOptionsPacket ();
                return 0;
            }  
               
        }
        else    // Il file delle opzioni ha una dimensione errata
        {
            INTERNAL_ResetTheNewDefaultOptionsFile ();
            INTERNAL_SaveOptionsPacket ();
            return;    
        }      
     }
     else    // Il file delle opzioni non esiste
     {
        INTERNAL_ResetTheNewDefaultOptionsFile ();     
        INTERNAL_SaveOptionsPacket ();
        return 0;
     }
}

void UpdateOptionsOnTheDisk ()
{
    if (SystemOptPacket.AnOptionHasBeenChanged)
    {
        INTERNAL_SaveOptionsPacket ();                                        
    } 
}

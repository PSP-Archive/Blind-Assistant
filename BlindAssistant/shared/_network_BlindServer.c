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
   FUNZIONI DI RETE PER LA CONNESSIONE CON IL BLIND SERVER
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
   VARIABILI
   ----------------------------------------------------------------------------
*/ 

char baPacketsHaveBeenAllocated = 0;
char baHandOnIsActive           = 0;

int  baServerFound              = 0;

/*
   ----------------------------------------------------------------------------
   ALLOCAZIONE/DEALLOCAZIONE PACCHETTI DI COMUNICAZIONE
   ----------------------------------------------------------------------------
*/ 

void baBldSvr_AllocateCommPackets (void)
{
    BldServerCommPacket = malloc ( sizeof (struct BldServerCommPacket_Type) );
                   
    if (BldServerCommPacket!=0)
    {
       BldServerCommPacketShort = malloc ( sizeof (struct BldServerCommPacketShort_Type) );
                
       if (BldServerCommPacketShort!=0)
       {
            // Segnalerà alle routine successive che i pacchetti di comunicazione
            // sono stati allocati con successo
            
            baPacketsHaveBeenAllocated=1;                           
       }
       else   // Problemi nell'allocazione del pacchetto
       {
            free (BldServerCommPacket);  
       }
    }
}

void baBldSvr_DeAllocateCommPackets (void)
{
     if (baPacketsHaveBeenAllocated)
     {
         free (BldServerCommPacketShort);
         free (BldServerCommPacket);             
     }
}


/*
   ----------------------------------------------------------------------------
   CODICE DI TRASMISSIONE A BASSO LIVELLO
   ----------------------------------------------------------------------------
*/ 

int baBldSvr_Send (SOCKET s, const void* buf, int len, int flags)
{
    int ErrRep;
    
    send_repeat:
       ErrRep = send (s, buf, len, flags);
       
       if (ErrRep==-1) 
       {
          ndDelay (0.01);
          goto send_repeat;
       }
       
    return ErrRep;
}

int baBldSvr_Recv (SOCKET s, const void* buf, int len, int flags)
{
    int ErrRep;
    
    ErrRep = recv (s, buf, len, flags);
       
    if ((ErrRep==-1) || (ErrRep!=len)) 
       return -1;
    else
       return 0;
}

int baBldSvr_SendAPacket (struct BldServerCommPacket_Type *MyBldSvrCommPacket, unsigned short Message, unsigned short SubMessage, int Data0, int Data1)
{
    memset (MyBldSvrCommPacket, 0, sizeof (struct BldServerCommPacket_Type));
                
    MyBldSvrCommPacket->Message    = Message;
    MyBldSvrCommPacket->SubMessage = SubMessage;
    MyBldSvrCommPacket->Data0      = Data0;
    MyBldSvrCommPacket->Data1      = Data1;
      
    return ( (baBldSvr_Send (bldsvr_socket, MyBldSvrCommPacket, sizeof (struct BldServerCommPacket_Type), 0)>=0) ? 0 : -1);                                
}


int baBldSvr_SendAPacket_PASSW (struct BldServerCommPacket_Type *MyBldSvrCommPacket, unsigned short Message, unsigned short SubMessage, int Data0, int Data1, char *MyPassword)
{
    memset (MyBldSvrCommPacket, 0, sizeof (struct BldServerCommPacket_Type));
                
    MyBldSvrCommPacket->Message    = Message;
    MyBldSvrCommPacket->SubMessage = SubMessage;
    MyBldSvrCommPacket->Data0      = Data0;
    MyBldSvrCommPacket->Data1      = Data1;
      
    // Copia la password nel pacchetto ed invialo
    strcpy ( &(MyBldSvrCommPacket->Data.Password), MyPassword); 
    return ( (baBldSvr_Send (bldsvr_socket, MyBldSvrCommPacket, sizeof (struct BldServerCommPacket_Type), 0)>=0) ? 0 : -1);            
}

int baBldSvr_SendAPacket_BLOCK (struct BldServerCommPacket_Type *MyBldSvrCommPacket, unsigned short Message, unsigned short SubMessage, int Data0, int Data1, char *BlockAddr, int NrBytes)
{
    int Counter;
    
    memset (MyBldSvrCommPacket, 0, sizeof (struct BldServerCommPacket_Type));
                
    MyBldSvrCommPacket->Message    = Message;
    MyBldSvrCommPacket->SubMessage = SubMessage;
    MyBldSvrCommPacket->Data0      = Data0;
    MyBldSvrCommPacket->Data1      = Data1;
      
    for (Counter=0; Counter<NrBytes; Counter++)
    {
        MyBldSvrCommPacket->Data.Linear [Counter] = *(BlockAddr+Counter);
    }  
      
    return ( (baBldSvr_Send (bldsvr_socket, MyBldSvrCommPacket, sizeof (struct BldServerCommPacket_Type), 0)>=0) ? 0 : -1);            
}

int baBldSvr_SendAShortPacket (struct BldServerCommPacketShort_Type *MyBldSvrCommPacket, unsigned short Message, unsigned short SubMessage, int Data0, int Data1)
{
    memset (MyBldSvrCommPacket, 0, sizeof (struct BldServerCommPacketShort_Type));
                
    MyBldSvrCommPacket->Message    = Message;
    MyBldSvrCommPacket->SubMessage = SubMessage;
    MyBldSvrCommPacket->Data0      = Data0;
    MyBldSvrCommPacket->Data1      = Data1;
      
    return ( (baBldSvr_Send (bldsvr_socket, MyBldSvrCommPacket, sizeof (struct BldServerCommPacketShort_Type), 0)>=0) ? 0 : -1);            
}

int baBldSvr_ReceiveAPacket (struct BldServerCommPacket_Type *MyBldSvrCommPacket)
{
    return ( (baBldSvr_Recv (bldsvr_socket, MyBldSvrCommPacket, sizeof (struct BldServerCommPacket_Type), 0)>=0) ? 0 : -1); 
}

int baBldSvr_ReceiveAShortPacket (struct BldServerCommPacketShort_Type *MyBldSvrCommPacket)
{
    return ( (baBldSvr_Recv (bldsvr_socket, MyBldSvrCommPacket, sizeof (struct BldServerCommPacketShort_Type), 0)>=0) ? 0 : -1); 
}



/*
   ----------------------------------------------------------------------------
   CODICE PER CONNESSIONE
   ----------------------------------------------------------------------------
*/ 

int baBldSvr_GenerateBldSvrSin (struct sockaddr_in *mybldsvr_sin)
/*
    Provvede a generare la struttura sockaddr_in che servirà per
    la connessione. Questa struttura contiene l'indirizzo TCP-IP
    del server. Il sistema può generare questa struttura usando il
    server DNS oppure direttamente se l'indirizzo viene espresso
    in modo esplicito
*/
{
    struct hostent *bldsvr_hostentry; 
    char Str [16];
    
    ndStrCpy ( &Str, &(SystemOptPacket.BlindServerAddrStr), 7, 0);
    
    if (!strcmp (Str, "http://"))        // Indirizzo immesso tramite il TCP-IP
    {
               // Recupera via DNS l'indirizzo IP di Blind Server sulla rete
               // L'indirizzo viene memorizzato nella struttura bldsvr_hostentry
               
               bldsvr_hostentry = gethostbyname ( &(SystemOptPacket.BlindServerAddrStr) ); 
               
               if (bldsvr_hostentry>=0)
               {
                    // Costruisci la struttura che deve essere passata alla funzione socket per
                    // una creazione di socket in modalità attiva
                    
                    memset (mybldsvr_sin, 0, sizeof(*mybldsvr_sin));    
                    
                    mybldsvr_sin->sin_family = AF_INET; 
                    mybldsvr_sin->sin_port = htons (SystemOptPacket.BlindServerPort); 
                    memcpy (&(mybldsvr_sin->sin_addr), (bldsvr_hostentry->h_addr), (bldsvr_hostentry->h_length)); 
               
                    return 0;
               }   
               else    // Problemi a determinare l'indirizzo DNS        
                  return -1;  
    }
    else    // L'indirizzo è stato ottenuto per via diretta
    {
          memset (mybldsvr_sin, 0, sizeof(*mybldsvr_sin));    
                    
          mybldsvr_sin->sin_family = AF_INET; 
          mybldsvr_sin->sin_port = htons (SystemOptPacket.BlindServerPort); 
          mybldsvr_sin->sin_addr.s_addr = htonl ( ndHAL_NET_InetInetAddr ( &(SystemOptPacket.BlindServerAddrStr) ));
          
          return 0;            
    }
}
 
int baBldSvr_CalculateCheckSum (struct BldServerCommPacket_Type *BldServerCommPacket)
{
    int CheckSum = 0;
    int Counter;
    
    for (Counter=0; Counter<256; Counter++)
    {
        CheckSum+=BldServerCommPacket->Data.Linear [Counter];
    }
    
    return CheckSum;
} 
 

/*
   ----------------------------------------------------------------------------
   HAND-ON, HAND-OFF
   ----------------------------------------------------------------------------
*/ 


int baBldSvr_HandOn (void)
/*
    Questa routine si occupa di eseguire l'autentificazione iniziale delle operazioni
    del BlindServer. A partire da BlindServer 2.0, il socket di comunicazione viene
    creato per una singola operazione, e poi cancellato alla fine di essa (la versione
    1.0, invece, usava sempre lo stesso socket di comunicazione, il chè provocava
    blocchi di comunicazione in caso di errori del sistema). 
    
    HandOn provvede a creare il socket necessario, ad inviare il pacchetto di Hello
    al sistema, a verificare la versione ed ad eseguire l'autentificazione.
    
    Restituisce 0 in caso di successo, ed un valore negativo in caso di errore
*/
{
   int ErrRep;
    
   if (!baHandOnIsActive)
   {    
       if (baPacketsHaveBeenAllocated)
       {         
           // Poni a 0 la variabile baServerFound perchè ancora non è stata scoperta
           // la versione del server
           
           baServerFound=0;
           
           // Genera la struttura BldSvrSin che ci interessa 
           
           ErrRep = baBldSvr_GenerateBldSvrSin (&bldsvr_sin);
           if (ErrRep) return ERR_IMPOSSIBLE_TO_DETERMINE_BLDSVR_IP;
           
           // Genera il socket che ci serve
                
           bldsvr_socket = socket(PF_INET, SOCK_STREAM, 0);     
           if (bldsvr_socket<0) return ERR_IMPOSSIBLE_TO_CREATE_BLDSVR_SOCKET;
           
           // Esegui la connessione con il BlindServer
           
           ErrRep = connect(bldsvr_socket, (struct sockaddr *)&bldsvr_sin, sizeof(bldsvr_sin));
           if (ErrRep<0) return ERR_IMPOSSIBLE_TO_CONNECT_TO_BLDSVR;
                                
           // Setta la modalità bloccante per la connessione
                                       
           ndHAL_NET_SetupNoBlockingConnection (bldsvr_socket, 0);
                                                            
           // Invia il pacchetto di Hello per autentificare la password e per verificare la versione
           // del server
           
           ErrRep = baBldSvr_SendAPacket_PASSW (BldServerCommPacket, BLDSVR_HELLO, 0, 0, 0, &(SystemOptPacket.Password));
           if (ErrRep) return ERR_IMPOSSIBLE_TO_SEND_PASSWORD_PACKET;
        
           // Il server risponde con un pacchetto contenente il codice di avvenuta autentificazione
           // ed il numero di versione. Attendi tale codice di risposta
           
           ErrRep = baBldSvr_ReceiveAPacket (BldServerCommPacket);
           if (ErrRep) return ERR_IMPOSSIBLE_TO_RECV_PASSWORD_RESPONSE;
           
           // Verifica se l'autentificazione è fallita
           if (BldServerCommPacket->Message != BLDSVR_AUTHENT_OK)  return ERR_AUTHENTIFICATION_FAILED;                     
            
           // Registra il numero di versione del server che è stata rilevata 
           baServerFound = BldServerCommPacket->Data0;
            
           // Verifica se il codice di versione è corretto
           if (baServerFound != BLDSVR_VERSION_CODE)
           {
              // Se la risposta è no, avvia una serie di routine di chiusura specifiche per
              // la vecchia versione del server
              
              switch (baServerFound)
              {
                     case 0x00010000:
                     {
                          OLD_BldSvr_V1_0_CloseConnectionWithBlindServer ();
                          break;
                     }
              }
           
              // Invia una segnalazione di errore alla routine chiamante
              
              return ERR_SERVER_VERSION_MISMATCH;
           }
           
           baHandOnIsActive=1;
           return 0;                      // SUCCESS     
       }
       else 
          return ERR_IMPOSSIBLE_TO_ALLOC_COMM_PACKETS; 
   }
   else
     return ERR_HANDON_IS_ALREADY_ACTIVE;
}



void baBldSvr_HandOff (void)
/*
    Provvede a chiudere il socket di comunicazione. Sarà riaperto nella sessione 
    successiva 
*/
{
   // Operazioni di chiusura specifiche per il tipo di server rilevato
    
   if (baHandOnIsActive)
   {
      closesocket (bldsvr_socket); 
      baHandOnIsActive=0;
   }
}

/*
   ----------------------------------------------------------------------------
   PRIMITIVE DI ACCESSO AL SERVER
   ----------------------------------------------------------------------------
*/ 


int baBldSvr_TryAConnectToBlindServer (void)
/*
    Questa routine si occupa di connettersi al BlindServer creando un socket. 
    Quando la funzione viene richiamata, il sistema deve già avere attivato
    una connessione wireless.
    
    Restituisce 0 in caso di successo ed un valore negativo in caso di errore. 
*/
{    
    int ErrRep;
    
    ErrRep=baBldSvr_HandOn ();        // Avvia una nuova sessione di comunicazione
    if (ErrRep) { baBldSvr_HandOff(); return (ErrRep); } 
        
    // Invia un codice di test al server in modo che reinizializzi la sessione
    // mantenendo la coerenza di funzionamento
    
    ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_IT_WAS_ONLY_A_TEST, 0, 0, 0);
    if (ErrRep) { baBldSvr_HandOff(); return (ErrRep); } 
    
    baBldSvr_HandOff (); 
    return 0;         
}




char baBldSvr_AskToSvrIfItHasAnImageStored (char NrSlot)
/*
    Richiede al Blind Server se ha un'immagine memorizzata in un certo slot.
    Restituisce 1 se c'è un'immagine memorizzata e 0 in caso negativo
    
    Se viene restituito il valore -1 siamo in presenza di una condizione
    anomala
*/
{
    int ErrRep;
    
    // Esegui l'hand-on con il server
    
    ErrRep=baBldSvr_HandOn ();        
    if (ErrRep) { baBldSvr_HandOff(); return (ErrRep); } 
    
    // Invia il pacchetto di richiesta al server
    ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_ASKING_IF_THE_SLOT_IS_BUSY, 0, NrSlot, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_SEND_FIRST_COMMAND; }

    // Ricevi il pacchetto di risposta dal server
    ErrRep = baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_RECEIVE_CONFIRM; }
    
    // Verifica la congruenza del pacchetto di risposta
    if (BldServerCommPacketShort->Message != BLDSVR_CLIENT_IS_ASKING_IF_THE_SLOT_IS_BUSY_RESPONSE)
       { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }  
     
    // Se non ci sono errori, restituisce il valore 0 o 1 restituito dal Blind Server                            
    baBldSvr_HandOff();
    return (BldServerCommPacketShort->Data0);                 
}


char baBldSvr_AskToBlindServerWhereIAm (IplImage *MyImage, char *NamePlace)
{
    ILuint DevIL_ImageCode1;        // Codice immagine DevIL 
    int    DevIL_Success;
    
    unsigned char   *DevIL_LumpBuffer;
    int     BlockCounter, Counter;
    
    int     LastByteNotNull;
    char    YouCanExit;
    int     ErrRep, ExitCode, CheckSum;
    char    FailedAttempts, TrxSuccessful;
    
    int     TotalBlocksInTrx; 
    int     Progress;
        
    // Esegui l'hand-on con il server
    
    ErrRep=baBldSvr_HandOn ();        
    if (ErrRep) { baBldSvr_HandOff(); return (ErrRep); } 
    
    // Invia a BlindServer un comando che inizia il processo di analisi SIFT
    // per determinare in che luogo ci troviamo
     
    ErrRep=baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_ASKING_WHERE_IT_IS_NOW, 0, 0, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_SEND_FIRST_COMMAND; }

    // Invia a BlindServer il segnale che una immagine è in invio
    
    ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______START, 0, 0, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_START_TRANSFER; }
    
    // Esegui la compressione in JPEG usando l'interfaccia DevIL. Intanto inizia a copiare
    // i dati dell'immagine dentro una struttura DevIL appropriata
    
    ilGenImages (1, &DevIL_ImageCode1);
    // Genera il codice che identifica l'immagine DevIL nr. 1
  
    ilBindImage (DevIL_ImageCode1);
    // Predispone la libreria DevIL per ricevere le successive chiamate ed usarle
    // sull'immagine DevIL_ImageCode1
  
    ilTexImage (STD_BLDSVR__SAMPLEIMG__DIMX, STD_BLDSVR__SAMPLEIMG__DIMY, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, MyImage->imageData);
    // Setta la libreria Dev-IL per usare i dati dell'immagine a toni di grigio fornita
     
    DevIL_LumpBuffer = malloc (2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
    // Crea il buffer lump dove comprimere le immagini
           
    if (DevIL_LumpBuffer!=0)       
    {              
            memset (DevIL_LumpBuffer, 0, 2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
            // Pulisci il buffer
            
            DevIL_Success=ilSaveL(IL_JPG, DevIL_LumpBuffer, 2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
            // Questa operazione provvederà a comprimere le immagini in formato JPG
            
            if (DevIL_Success)
            {
                // Impostiamo a zero: sarà eventualmente cambiato in seguito
                
                ExitCode=0; 
                
                // Provvediamo a determinare l'ultimo byte diverso da zero
                
                LastByteNotNull=0;
                
                for (Counter=2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY - 1; (Counter>=0); Counter--)
                {
                    if (DevIL_LumpBuffer [Counter]!=0)
                    {
                       LastByteNotNull =Counter;
                       break;
                    }
                }
                
                TotalBlocksInTrx = (LastByteNotNull/256)+1;
                    
                // Inizio operazioni di trasmissione immagine    
                    
                YouCanExit   =0;    
                BlockCounter =0;
                TrxSuccessful=0;
                    
                while (!YouCanExit)
                {
                    _WHEREIAM_SENDTOSVRIMG_TryToTransmitAPacket:
                                    
                            FailedAttempts=0; 
                            
                    _WHEREIAM_SENDTOSVRIMG_ReTransmitPacket:                                                 
                            
                            // Adesso provvedi a creare il blocco dati ed a trasmetterlo
                            ErrRep = baBldSvr_SendAPacket_BLOCK (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______PROCEED, 
                                            0, BlockCounter, 0, &(DevIL_LumpBuffer[256*BlockCounter]), 256);
        
                            if (ErrRep)          
                            {
                                 FailedAttempts++;
                                 
                                 if (FailedAttempts<3)  goto _WHEREIAM_SENDTOSVRIMG_ReTransmitPacket;
                                    else break;
                            }
                            
                            
                            // Ricevi i dati dal BlindServer. La risposta deve contenere il checksum del
                            // pacchetto richiesto... Per prima cosa calcoliamo internamente il checksum
                            // corretto
                            
                            CheckSum = baBldSvr_CalculateCheckSum (BldServerCommPacket);
                            
                    _WHEREIAM_SENDTOSVRIMG_ReceiveConfirmPacket:         
                            
                            // Ed ora riceviamo il pacchetto di conferma dal server
                            
                            ErrRep = baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);  
                                    
                            // Verifichiamo che non vi siano:
                            // Errori nella ricezione del pacchetto di conferma
                            // Errori nella signature del pacchetto
                            // Errori nel checksum       
                                    
                            if (  (ErrRep) ||         
                                  (BldServerCommPacketShort->Message!=BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______RESPONSE) ||
                                  (BldServerCommPacketShort->Data0!=CheckSum)  )
                            {
                                 FailedAttempts++;
                                 
                                 if (FailedAttempts<3)  goto _WHEREIAM_SENDTOSVRIMG_ReTransmitPacket;
                                    else break;
                            }
                            
                            // Arrivati in questo punto, il sistema ha verificato che non ci sono errori di
                            // pacchetto. Prepariamoci per la trasmissione del pacchetto successivo
                            
                            BlockCounter++;
                            
                            if (256*BlockCounter>LastByteNotNull)     // Ultimo pacchetto ?    
                            {
                                // Si: Invia il pacchetto di chiusura
                                ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______CLOSE, 0, 0, 0);

                                // Esce dal loop e segnala che la trasmissione è riuscita
                                TrxSuccessful=1;
                                break;
                            }
                }                     // Fine ciclo while per il trasferimento dell'immagine
                
                // Pulisci la memoria
                
                ilDeleteImages (1, &DevIL_ImageCode1);
                free (DevIL_LumpBuffer);
                
                // In presenza di problemi per il trasferimento dell'immagine esci
                
                if (!TrxSuccessful) 
                {
                     ExitCode = ERR_IMPOSSIBLE_TO_TRANSFER_A_PACKET;
                     goto _WHEREIAM_SENDTOSVRIMG_ERR1;
                }
                
                // IMMAGINE INVIATA. Adesso preleviamo la risposta del server (in che luogo ci
                // troviamo?)
                
                // Preleva le informazioni sul luogo
                ErrRep=baBldSvr_ReceiveAPacket (BldServerCommPacket);
                
                if ((ErrRep) || (BldServerCommPacket->Message!=BLDSVR_CLIENT_IS_ASKING_WHERE_IT_IS_NOW______RESPONSE))
                {
                    ExitCode = ERR_RESPONSE_CODE_MISMATCH;
                    goto _WHEREIAM_SENDTOSVRIMG_ERR1;        
                }

                // Copia il nome del luogo nella variabile NamePlace
                strcpy (NamePlace, BldServerCommPacket->Data.Linear);

                baBldSvr_HandOff();             // Chiudi la sessione di comunicazione
                return 0;                       // DONE!
                
             _WHEREIAM_SENDTOSVRIMG_ERR1:
                
                baBldSvr_HandOff();             // Chiudi la sessione di comunicazione
                return ExitCode;
                         
        }
        else    // Problemi nella compressione
        {
            ilDeleteImages (1, &DevIL_ImageCode1);
            free (DevIL_LumpBuffer);
          
            baBldSvr_HandOff();
            return ERR_IMPOSSIBLE_TO_COMPRESS_IMAGE;
        }   
    }
    else    // Problemi nell'allocazione del LumpBuffer
    {
      baBldSvr_HandOff();
      return ERR_IMPOSSIBLE_TO_ALLOC_LUMPBUFFER;
    }
}


char baBldSvr_SendToSvrASlotPackage (char NrSlot, void *GraphCallback, 
                                     struct cvTmpPlacePacket_Type *PntToTmpPlacePacket,
                                     char *Name)
/*
    Questa routine invia verso il server tutte le informazioni relative
    ad uno slot
    
    Se non ci sono errori viene restituito il valore 0
    
    Il campo GraphCallback consente di settare una callback del
    tipo 
    
    void GraphCallback (int MessageData, int Progress)
    
    che viene richiamata durante il trasferimento.
*/
{
    ILuint DevIL_ImageCode1;        // Codice immagine DevIL 
    int    DevIL_Success;
    
    unsigned char   *DevIL_LumpBuffer;
    int     BlockCounter, Counter, ImgCounter;
    
    char    YouCanExit;
    int     ErrRep, ExitCode, LastByteNotNull;
    int     CheckSum;
    char    FailedAttempts;
    char    TrxSuccessful, RcvSuccessful;
    
    int     TotalBlocksInTrx, TotalBlocksInRecv; 
    int     Progress;
    
    
    // Puntatore alla callback
    
    void (*PntToGraphCallback)(int MessageData, int Progress);
    PntToGraphCallback = GraphCallback; 
    
    // Esegui l'hand-on con il server
    
    ErrRep=baBldSvr_HandOn ();        
    if (ErrRep) { baBldSvr_HandOff(); return (ErrRep); } 
    
    // Invia a BlindServer il segnale che stiamo iniziando l'upload dei dati dello slot package:
    // il pacchetto deve essere lungo perchè è così che vengono acquisiti tutti i pacchetti di
    // inizio operazione
    
    ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE, 0, NrSlot, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_SEND_FIRST_COMMAND; }

    // Ricevi un pacchetto di risposta dal BlindServer.
    // Il pacchetto deve essere corto perchè in questa operazione il server risponde sempre con
    // pacchetti corti
    
    ErrRep=baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_RECEIVE_CONFIRM; }
    
    // Verifichiamo se il pacchetto di risposta ha il codice corretto
    if (BldServerCommPacketShort->Message!=BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______READY)
    { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }
    
    // Inizia l'invio delle immagini
    
    ImgCounter=0;
    
    while (ImgCounter<NR_SAMPLES_FOR_PLACE)
    {
            // Invia a BlindServer il segnale che una immagine è in invio
            
            ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______STARTIMG, 0, ImgCounter, 0);
            if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_START_TRANSFER; }
            
            // Ricevi un messaggio di conferma dal server
            ErrRep=baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);
            if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_RECEIVE_CONFIRM; }
    
            // Verifica il messaggio di conferma
            if (BldServerCommPacketShort->Message!=BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______STARTIMGRESP)
            { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }
    
            // Esegui la compressione in JPEG usando l'interfaccia DevIL. Intanto inizia a copiare
            // i dati dell'immagine dentro una struttura DevIL appropriata
            
            ilGenImages (1, &DevIL_ImageCode1);
            // Genera il codice che identifica l'immagine DevIL nr. 1
          
            ilBindImage (DevIL_ImageCode1);
            // Predispone la libreria DevIL per ricevere le successive chiamate ed usarle
            // sull'immagine DevIL_ImageCode1
          
            ilTexImage (STD_BLDSVR__SAMPLEIMG__DIMX, STD_BLDSVR__SAMPLEIMG__DIMY, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, PntToTmpPlacePacket->Img[ImgCounter]->imageData);
            // Setta la libreria Dev-IL per usare i dati dell'immagine a toni di grigio fornita
             
            DevIL_LumpBuffer = malloc (2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
            // Crea il buffer lump dove comprimere le immagini
                   
            if (DevIL_LumpBuffer!=0)       
            {              
                    memset (DevIL_LumpBuffer, 0, 2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
                    // Pulisci il buffer
                    
                    DevIL_Success=ilSaveL(IL_JPG, DevIL_LumpBuffer, 2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
                    // Questa operazione provvederà a comprimere le immagini in formato JPG
                    
                    if (DevIL_Success)
                    {
                        // Impostiamo a zero: sarà eventualmente cambiato in seguito
                        
                        ExitCode=0; 
                        
                        // Provvediamo a determinare l'ultimo byte diverso da zero
                        
                        LastByteNotNull=0;
                        
                        for (Counter=2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY - 1; (Counter>=0); Counter--)
                        {
                            if (DevIL_LumpBuffer [Counter]!=0)
                            {
                               LastByteNotNull =Counter;
                               break;
                            }
                        }
                        
                        TotalBlocksInTrx = (LastByteNotNull/256)+1;
                            
                        // Inizio operazioni di trasmissione immagine    
                            
                        YouCanExit   =0;    
                        BlockCounter =0;
                        TrxSuccessful=0;
                            
                        while (!YouCanExit)
                        {
                            _SENDSLOTPCKG_SENDTOSVRIMG_TryToTransmitAPacket:
                                            
                                    FailedAttempts=0; 
                                    
                            _SENDSLOTPCKG_SENDTOSVRIMG_ReTransmitPacket:                                                 
                                    
                                    // Adesso provvedi a creare il blocco dati ed a trasmetterlo
                                    ErrRep = baBldSvr_SendAPacket_BLOCK (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______SENDBLOCK, 
                                                    0, BlockCounter, 0, &(DevIL_LumpBuffer[256*BlockCounter]), 256);
                
                                    // Ricevi i dati dal BlindServer. La risposta deve contenere il checksum del
                                    // pacchetto richiesto... Per prima cosa calcoliamo internamente il checksum
                                    // corretto
                                    
                                    CheckSum = baBldSvr_CalculateCheckSum (BldServerCommPacket);
                                    
                            _SENDSLOTPCKG_SENDTOSVRIMG_ReceiveConfirmPacket:         
                                    
                                    // Ed ora riceviamo il pacchetto di conferma dal server
                                    
                                    ErrRep = baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);  
                                            
                                    // Verifichiamo che non vi siano:
                                    // Errori nella ricezione del pacchetto di conferma
                                    // Errori nella signature del pacchetto
                                    // Errori nel checksum       
                                            
                                    if (  (ErrRep) ||         
                                          (BldServerCommPacketShort->Message!=BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______SENDRESP) ||
                                          (BldServerCommPacketShort->Data0!=CheckSum)  )
                                    {
                                         FailedAttempts++;
                                         
                                         if (FailedAttempts<3)  goto _SENDSLOTPCKG_SENDTOSVRIMG_ReTransmitPacket;
                                            else break;
                                    }
                                    
                                    // Arrivati a questo punto, il sistema ha trasferito il pacchetto:
                                    // esegui la callback per segnalare il progresso dell'operazione
                                    
                                    if ((BlockCounter%30)==0)      // Impedisce che la callback sia richiamata troppo spesso
                                    {                              // diminuendo la velocità di trasmissione
                                    
                                        Progress = 100*( (float)(BlockCounter)/(float)(TotalBlocksInTrx) );
                                        
                                        if (GraphCallback!=0)
                                        {
                                            // Richiama la callback grafica                 
                                            (*PntToGraphCallback)(ImgCounter, Progress);                 
                                        }
                                    }
                                
                                    // Adesso prepariamoci alla trasmissione del pacchetto successivo
                                    
                                    BlockCounter++;
                                    
                                    if (256*BlockCounter>LastByteNotNull)     // Ultimo pacchetto ?    
                                    {
                                        // Si: Invia il pacchetto di chiusura
                                        ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______CLOSEIMG, 0, 0, 0);
        
                                        // Esce dal loop e segnala che la trasmissione è riuscita
                                        TrxSuccessful=1;
                                        break;
                                    }
                        }                     // Fine ciclo while per il trasferimento dell'immagine
                        
                        // Pulisci la memoria
                        
                        ilDeleteImages (1, &DevIL_ImageCode1);
                        free (DevIL_LumpBuffer);
                        
                        // In presenza di problemi per il trasferimento dell'immagine esci
                        
                        if (!TrxSuccessful) 
                        {
                             ExitCode = ERR_IMPOSSIBLE_TO_TRANSFER_A_PACKET;
                             goto _SENDSLOTPCKG_SENDTOSVRIMG_ERR1;
                        }
                        
                        // IMMAGINE INVIATA. Preparati per inviare l'immagine successiva
                        
                        ImgCounter++;
                        continue;
                        
                        // Gestione degli errori
                        
                     _SENDSLOTPCKG_SENDTOSVRIMG_ERR1:
                        
                        baBldSvr_HandOff();
                        return ExitCode;
                                 
                }
                else    // Problemi nella compressione
                {
                    ilDeleteImages (1, &DevIL_ImageCode1);
                    free (DevIL_LumpBuffer);
                  
                    baBldSvr_HandOff(); 
                    return ERR_IMPOSSIBLE_TO_COMPRESS_IMAGE;
                }   
            }
            else    // Problemi nell'allocazione del LumpBuffer
            {
                baBldSvr_HandOff();
                return ERR_IMPOSSIBLE_TO_ALLOC_LUMPBUFFER;
            }
        
    }                           // End del ciclo while di invio immagini
    
    // Quando il sistema arriva qui, significa che tutte le immagini sono state
    // trasferite. Adesso bisogna trasferire il nome dello slot package
    
    ErrRep = baBldSvr_SendAPacket_BLOCK (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______NAME, 0, NrSlot, 0, Name, strlen(Name));
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_TRANSFER_THE_NAME; } 
    
    // Attendi che il BlindServer mandi conferma del pacchetto ricevuto
    
    ErrRep = baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_TRANSFER_THE_NAME; }
    
    // Verifica che la risposta inviata dal BlindServer sia corretta
    
    if (BldServerCommPacketShort->Message!=BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______NAMERESP)
      { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }
    
    // Segnala per mezzo della callback grafica che il responso sul nome è stato ricevuto 
    // con successo
    
    if (GraphCallback!=0) (*PntToGraphCallback)(0x00010000, 90);                 
    
    // Ok, adesso BlindServer eseguirà alcune operazioni di calcolo, come
    // l'elaborazione delle SIFT features. Attendi da BlindServer il
    // pacchetto con codice di chiusura che ci dirà qual è l'esito delle
    // operazioni
   
    ErrRep = baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);
    if (ErrRep) {baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_RECEIVE_A_PACKET; }
                      
    // Verifica che sia stato inviato il codice di chiusura
    
    if (BldServerCommPacketShort->Message!=BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______CLOSE)
      { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }
    
    if (BldServerCommPacketShort->Data0!=0)
      { baBldSvr_HandOff(); return ERR_REMOTE_SERVER_ERROR; }
    
    // TUTTO LO SLOT PACKAGE E' STATO INVIATO: Segnalalo per mezzo della callback
    
    if (GraphCallback!=0) (*PntToGraphCallback)(0x00010000, 100);    
    
    // Segnala successo alla routine chiamante
    
    baBldSvr_HandOff();
    return 0;        
}


char baBldSvr_RetrieveFromSvrASlotPackage (char NrSlot, void *GraphCallback, 
                                           struct cvTmpPlacePacket_Type *PntToTmpPlacePacket,
                                           char *Name)
/*
    Questa routine preleva dal server tutte le informazioni relative ad uno slot
    Se non ci sono errori viene restituito il valore 0
    
    L'immagine viene scomposta in gruppi di 256 pixels che vengono trasferiti 
    uno ad uno dal server
      
    Il campo GraphCallback consente di settare una callback del tipo 
    
    void GraphCallback (int MessageData, int Progress)
    
    che viene richiamata durante il trasferimento.
*/
{
    IplImage *MyIPLImage;
    
    ILuint DevIL_ImageCode1;        // Codice immagine DevIL 
    int    DevIL_Success;
    
    unsigned char   *DevIL_LumpBuffer;
    int     BlockCounter, ImgCounter;
    int     Counter, CounterX, CounterY;
    
    int     LastByteNotNull;
    char    YouCanExit;
    int     ErrRep, CheckSum, ExitCode;
    char    FailedAttempts, TrxSuccessful, RcvSuccessful;
    
    int     TotalBlocksInRecv, Progress;
    int     AutoCheckSum;
    
    unsigned char Pixel;
    
    // Puntatore alla callback
    
    void (*PntToGraphCallback)(int MessageData, int Progress);
    PntToGraphCallback = GraphCallback; 
    
    // Esegui l'hand-on con il server
    
    ErrRep=baBldSvr_HandOn ();        
    if (ErrRep) { baBldSvr_HandOff(); return (ErrRep); } 
    
    // Invia a BlindServer il segnale che stiamo iniziando il download dei dati dello slot package:
    // il pacchetto deve essere lungo perchè è così che vengono acquisiti tutti i pacchetti di
    // inizio operazione
    
    ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE, 0, NrSlot, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_SEND_FIRST_COMMAND; }

    // Ricevi un pacchetto di risposta dal BlindServer che segnali se lo slot è vuoto.
    // Il pacchetto deve essere lungo perchè in questa operazione il server risponde sempre con
    // pacchetti lunghi
    ErrRep=baBldSvr_ReceiveAPacket (BldServerCommPacket);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_RECEIVE_CONFIRM; }
    
    // Verifichiamo se il pacchetto di risposta ha il codice corretto
            
    if (BldServerCommPacket->Message!=BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______SLOTINFO)
       { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }
            
    // Verifichiamo se Data0 ha un valore negativo: se è così, lo slot è vuoto
           
    if (BldServerCommPacket->Message<0) 
       { baBldSvr_HandOff(); return ERR_SLOT_IS_VOID; }

    // Inizia ad acquisire le immagini: da questo momento verranno inviati al server solo
    // pacchetti brevi e verranno ricevuti solo pacchetti lunghi
    
    ImgCounter=0;
    
    while (ImgCounter<NR_SAMPLES_FOR_PLACE)
    {         
            // Puntatore immagine IPL
            
            MyIPLImage = PntToTmpPlacePacket->Img[ImgCounter];
            
            // Invia a BlindServer un pacchetto breve che dice che si vuole ricevere un'immagine
            
            ErrRep=baBldSvr_SendAShortPacket (BldServerCommPacketShort, BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______STARTIMG, 0, ImgCounter, 0);
            if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_SEND_FIRST_COMMAND; }
             
            // Adesso attendi da BlindServer una conferma: devi ricavare il valore TotalBlocksInRecv
            
            ErrRep=baBldSvr_ReceiveAPacket (BldServerCommPacket);
            if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_RECEIVE_CONFIRM; }
            
            // Verifichiamo se il pacchetto di risposta ha il codice corretto
            
            if (BldServerCommPacket->Message!=BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______STARTIMGRESP)
               { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }
            
            // Ricaviamo il numero totale di blocchi in transito oppure rileviamo un
            // errore nel server remoto
            
            if (BldServerCommPacket->Data0>=0) 
                TotalBlocksInRecv = BldServerCommPacket->Data0;                                    
            else 
                { baBldSvr_HandOff(); return ERR_REMOTE_SERVER_ERROR; }  
            
            // Iniziamo a preparare il LumpBuffer dove comprimere le immagini
            
            DevIL_LumpBuffer = malloc (2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
            
            if (DevIL_LumpBuffer!=0)
            {
                // Pulisci tutto il buffer lump
                memset (DevIL_LumpBuffer, 0, 2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
                
                // Inizia il ciclo che preleverà l'immagine dal BlindServer
                BlockCounter  =0;
                YouCanExit    =0;
                RcvSuccessful =0;
                ExitCode      =0;
                
                while (BlockCounter<=TotalBlocksInRecv)
                {
                    _RCVASLOTPCKG_TryToReceiveAPacket:
                                                    
                        FailedAttempts=0;  
                    
                    _RCVASLOTPCKG_RetryToReceiveAPacket:
                    
                        // Invia un messaggio attraverso il quale si richiede un certo pacchetto
                        
                        ErrRep=baBldSvr_SendAShortPacket (BldServerCommPacketShort, BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______REQBLOCK, 0, BlockCounter, 0);
                        
                        if (ErrRep) 
                        {
                             FailedAttempts++;
                             
                             if (FailedAttempts<3) goto _RCVASLOTPCKG_RetryToReceiveAPacket;
                                else break;
                        }
                        
                        // Adesso preleva il pacchetto che ci è stato richiesto
                        
                        ErrRep= baBldSvr_ReceiveAPacket (BldServerCommPacket);
                        
                        // Provvedi ad eseguire un calcolo del checksum del pacchetto
                        
                        AutoCheckSum = baBldSvr_CalculateCheckSum (BldServerCommPacket);
                        
                        // Verifica se il pacchetto è errato
                      
                        if ( (ErrRep) || 
                             (BldServerCommPacket->Message!=BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______REQRESP) || 
                             (BldServerCommPacket->Data0 != AutoCheckSum) )
                        {
                             FailedAttempts++;
                             
                             if (FailedAttempts<3) goto _RCVASLOTPCKG_RetryToReceiveAPacket;
                                else break;
                        }
                      
                        // Quando il sistema arriva qui, vuol dire che il pacchetto è corretto
                        // Scarica i dati dal pacchetto al LumpBuffer
                        
                        for (Counter=0; Counter<=255; Counter++)
                        { 
                              DevIL_LumpBuffer [256*BlockCounter + Counter] = BldServerCommPacket->Data.Linear [Counter]; 
                        }     
                          
                        // Usa la callback per segnalare l'avanzamento del download
                          
                        if ((BlockCounter%30)==0)      // Impedisce che la callback sia richiamata troppo spesso
                        {                              // diminuendo la velocità di trasmissione
                        
                               Progress = 100*( (float)(BlockCounter)/(float)(TotalBlocksInRecv) );
                            
                               // Richiama la callback grafica  
                               if (GraphCallback!=0) (*PntToGraphCallback)(ImgCounter, Progress);                 
                        }
                        
                        // Preparati a ricevere il pacchetto successivo: incrementa il contatore BlockCounter
                        
                        BlockCounter++;
                        
                        // Siamo in presenza dell'ultimo pacchetto ?
                        
                        if (BlockCounter>TotalBlocksInRecv)
                        {
                             // Si: invia al server un pacchetto di chiusura
                             
                             baBldSvr_SendAShortPacket (BldServerCommPacketShort, BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______CLOSEIMG, 0, 0, 0);
                             
                             // Esce dal loop e segnala che la ricezione è riuscita
                             RcvSuccessful=1;
                             break;
                        }
                      
                    }   // End del ciclo while di ricezione
                    
                    if (!RcvSuccessful)
                    {
                        ExitCode = ERR_IMPOSSIBLE_TO_RECEIVE_A_PACKET;
                        goto _RCVASLOTPCKG_RCVIMAGE_ERR1;
                    }
                    
                    // IMMAGINE ACQUISITA: Ora si tratta di decomprimerla
                    
                    ilGenImages (1, &DevIL_ImageCode1);
                    // Genera il codice che identifica l'immagine DevIL nr. 1
            
                    ilBindImage (DevIL_ImageCode1);
                    // Predispone la libreria DevIL per ricevere le successive chiamate ed usarle
                    // sull'immagine DevIL_ImageCode1
            
                    ilLoadL (IL_JPG, DevIL_LumpBuffer, 2048 + STD_BLDSVR__SAMPLEIMG__DIMX*STD_BLDSVR__SAMPLEIMG__DIMY);
                    // Questa operazione provvederà a decomprimere le immagini in formato JPG
                    // L'immagine viene caricata dal buffer lump (dove è compressa) dentro 
                    // l'immagine DevIL 
            
                    // Ok, adesso provvedi a copiare dentro l'immagine OpenCv MyImage
                    // le informazioni Dev-IL richieste
                    
                    for (CounterY=0; CounterY<STD_BLDSVR__SAMPLEIMG__DIMY; CounterY++)
                    {
                          for (CounterX=0; CounterX<STD_BLDSVR__SAMPLEIMG__DIMX; CounterX++)
                          {
                                ilCopyPixels (CounterX, STD_BLDSVR__SAMPLEIMG__DIMY-CounterY, 0, 1, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, &Pixel);
                                // Preleva il pixel che ci serve dall'immagine DevIL. Per una qualche ragione, 
                                // quando viene decompressa l'immagine è invertita, quindi si deve correggere 
                                // con un flip software. Questo problema dovrà essere rivisto in seguito.
                                
                                *(unsigned char *)(MyIPLImage->imageData + MyIPLImage->widthStep * CounterY + CounterX) = Pixel;
                          }
                    }
                    
                    // Cancella le aree di memoria usate
                    
                    ilDeleteImages (1, &DevIL_ImageCode1);
                    free (DevIL_LumpBuffer);
                    
                    ImgCounter++;
                    
                    continue;                             // Prosegui con le immagini successive
                    
                     
                 _RCVASLOTPCKG_RCVIMAGE_ERR1:
                    
                    free (DevIL_LumpBuffer);
                    baBldSvr_HandOff();
                    return ExitCode;   
                                                   
            }
            else     // Problemi ad allocare il LumpBuffer
               { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_ALLOC_LUMPBUFFER; }          
                  
          
    }     // Fine del ciclo while di acquisizione delle immagini dei luoghi
    
    
    // IMMAGINI ACQUISITE
    // Quando si arriva in questo punto, le immagini che compongono il place
    // slot sono state già tutte acquisite. Ora bisogna acquisire il nome del luogo
    
    ErrRep=baBldSvr_SendAShortPacket (BldServerCommPacketShort, BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______NAME, 0, 0, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_TRANSFER_THE_NAME; }                  
    
    // Adesso aspetta la risposta del server
    
    ErrRep=baBldSvr_ReceiveAPacket (BldServerCommPacket);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_TRANSFER_THE_NAME; }
    
    // Verifica la congruenza del pacchetto di ritorno
    
    if (BldServerCommPacket->Message!=BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______NAMERESP)
       { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }
    
    // Copia nell'area di memoria puntata da Name il nome dello slot
    strcpy (Name, &(BldServerCommPacket->Data.Linear));
       
    // Invia un pacchetto di chiusura totale delle operazioni
    ErrRep=baBldSvr_SendAShortPacket (BldServerCommPacketShort, BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______CLOSE, 0, 0, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_TRANSFER_THE_NAME; }                  
                    
    // Richiama la callback grafica   
    if (GraphCallback!=0) (*PntToGraphCallback)(0x00010000, 100);                 
    
    // Segnala successo
    baBldSvr_HandOff();    
    return 0;
}



char baBldSvr_SendToSvrEraseCommand (char NrSlot)
/*
     Invia al server un comando di cancellazione
*/
{
    char ErrRep;
    
    // Esegui l'hand-on con il server
    
    ErrRep=baBldSvr_HandOn ();        
    if (ErrRep) { baBldSvr_HandOff(); return (ErrRep); } 
    
    // Invia il comando di cancellazione
    
    ErrRep=baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_ORDERING_THE_ERASE______START, 0, NrSlot, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_SEND_FIRST_COMMAND; }
            
    // Adesso mettiti in attesa della risposta
    
    ErrRep=baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_RECEIVE_CONFIRM; }
    
    // Verifica se la risposta è corretta
    
    if (BldServerCommPacketShort->Message != BLDSVR_CLIENT_IS_ORDERING_THE_ERASE______RESPONSE)
      { baBldSvr_HandOff(); return ERR_RESPONSE_CODE_MISMATCH; }   
    
    return 0;                    // Segnala successo
}



char baBldSvr_SendToSvrAnImage_ForOCR (IplImage *MyImage, void *TextBuffer, int LanguageData)
/*
    Questa routine invia verso il server una immagine in toni di grigio 
    di dimensioni STD_OCR__SAMPLEIMG__DIMX*STD_OCR__SAMPLEIMG__DIMY. 
    
    L'immagine viene scomposta in gruppi di 256 pixels che 
    vengono trasferiti uno ad uno verso il server
    
    Se non ci sono errori viene restituito il valore 0
    
    Il campo LabelData è un campo a 32 bit che viene usato per comunicare delle
    informazioni al BlindServer: per esempio, il tipo di OCR desiderato e la
    lingua che si vuole utilizzare
*/
{
    ILuint DevIL_ImageCode1;        // Codice immagine DevIL 
    int    DevIL_Success;
    
    unsigned char   *DevIL_LumpBuffer;
    int     BlockCounter, Counter;
    
    char    YouCanExit;
    int     ErrRep, ExitCode, LastByteNotNull;
    int     CheckSum;
    char    FailedAttempts;
    char    TrxSuccessful, RcvSuccessful;
    
    int     TotalBlocksInTrx, TotalBlocksInRecv; 
    int     Progress;
    
    int     OptionsCode = 0;
     
    // Esegui l'hand-on con il server
    
    ErrRep=baBldSvr_HandOn ();        
    if (ErrRep) { baBldSvr_HandOff(); return (ErrRep); } 
    
    // A partire dalla versione 1.2 del BlindServer, il primo bit del campo Data0 del pacchetto di richiesta
    // di un'analisi OCR viene posto ad 1, se noi stiamo usando una PSP-FAT. In questo modo, possiamo
    // informare il server che la scansione viene eseguita a 640x480 pixel e non ad una risoluzione di 
    // 1024x768 come normalmente viene fatto con la PSP-SLIM
    
    #ifdef PSP1000_VERSION
           OptionsCode |= 1;    
    #endif       
    
    // Invia a BlindServer il segnale che stiamo avviando un'analisi OCR 
    ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_ASKING_AN_OCR_ANALISYS, 0, LanguageData, OptionsCode);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_SEND_FIRST_COMMAND; }

    // Invia a BlindServer il segnale che una immagine è in invio
    
    ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______START, 0, 0, 0);
    if (ErrRep) { baBldSvr_HandOff(); return ERR_IMPOSSIBLE_TO_START_TRANSFER; }
   
    // Esegui la compressione in PSD usando l'interfaccia DevIL. Intanto inizia a copiare
    // i dati dell'immagine dentro una struttura DevIL appropriata
    
    ilGenImages (1, &DevIL_ImageCode1);
    // Genera il codice che identifica l'immagine DevIL nr. 1
  
    ilBindImage (DevIL_ImageCode1);
    // Predispone la libreria DevIL per ricevere le successive chiamate ed usarle
    // sull'immagine DevIL_ImageCode1
  
    ilTexImage (STD_OCR__SAMPLEIMG__DIMX, STD_OCR__SAMPLEIMG__DIMY, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, MyImage->imageData);
    // Setta la libreria Dev-IL per usare i dati dell'immagine a toni di grigio fornita
     
    DevIL_LumpBuffer = malloc (2048 + STD_OCR__SAMPLEIMG__DIMX*STD_OCR__SAMPLEIMG__DIMY);
    // Crea il buffer lump dove comprimere le immagini
           
    if (DevIL_LumpBuffer!=0)       
    {              
            memset (DevIL_LumpBuffer, 0, 2048 + STD_OCR__SAMPLEIMG__DIMX*STD_OCR__SAMPLEIMG__DIMY);
            // Pulisci il buffer
            
            DevIL_Success=ilSaveL(IL_PSD, DevIL_LumpBuffer, 2048 + STD_OCR__SAMPLEIMG__DIMX*STD_OCR__SAMPLEIMG__DIMY);
            // Questa operazione provvederà a comprimere le immagini in formato PSD
            
            if (DevIL_Success)
            {
                // Impostiamo a zero: sarà eventualmente cambiato in seguito
                
                ExitCode=0; 
                
                // Provvediamo a determinare l'ultimo byte diverso da zero
                
                LastByteNotNull=0;
                
                for (Counter=2048 + STD_OCR__SAMPLEIMG__DIMX*STD_OCR__SAMPLEIMG__DIMY - 1; (Counter>=0); Counter--)
                {
                    if (DevIL_LumpBuffer [Counter]!=0)
                    {
                       LastByteNotNull =Counter;
                       break;
                    }
                }
                
                TotalBlocksInTrx = (LastByteNotNull/256)+1;
                
                // Inizio operazioni di trasmissione immagine
                    
                YouCanExit    =0;    
                BlockCounter  =0;
                TrxSuccessful =0;
                    
                while (!YouCanExit)
                {
                    _SENDTOSVROCR_TryToTransmitAPacket:
                                    
                            FailedAttempts=0; 
                            
                    _SENDTOSVROCR_ReTransmitPacket:                                                 
                            
                            // Adesso provvedi a creare il blocco dati ed a trasmetterlo
                            ErrRep = baBldSvr_SendAPacket_BLOCK (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______PROCEED, 
                                            0, BlockCounter, 0, &(DevIL_LumpBuffer[256*BlockCounter]), 256);
        
                            if (ErrRep) 
                            {
                                 FailedAttempts++;
                                 
                                 if (FailedAttempts<3)  goto _SENDTOSVROCR_ReTransmitPacket;
                                    else break;
                            }
                            
                            
                            // Ricevi i dati dal BlindServer. La risposta deve contenere il checksum del
                            // pacchetto richiesto... Per prima cosa calcoliamo internamente il checksum
                            // corretto
                            
                            CheckSum = baBldSvr_CalculateCheckSum (BldServerCommPacket);
                            
                    _SENDTOSVROCR_ReceiveConfirmPacket:         
                            
                            // Ed ora riceviamo il pacchetto di conferma dal server
                            
                            ErrRep = baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);  
                                    
                            // Verifichiamo che non vi siano:
                            // Errori nella ricezione del pacchetto di conferma
                            // Errori nella signature del pacchetto
                            // Errori nel checksum       
                                    
                            if (  (ErrRep) ||         
                                  (BldServerCommPacketShort->Message!=BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______RESPONSE) ||
                                  (BldServerCommPacketShort->Data0!=CheckSum)  )
                            {
                                 FailedAttempts++;
                                 
                                 if (FailedAttempts<3)  goto _SENDTOSVROCR_ReTransmitPacket;
                                    else break;
                            }
                            
                            // Arrivati in questo punto, il sistema ha verificato che non ci sono errori di
                            // pacchetto. Prepariamoci per la trasmissione del pacchetto successivo
                            
                            BlockCounter++;
                            
                            if (256*BlockCounter>LastByteNotNull)     // Ultimo pacchetto ?    
                            {
                                // Si: Invia il pacchetto di chiusura
                                ErrRep = baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______CLOSE, 0, 0, 0);

                                // Esce dal loop e segnala che la trasmissione è riuscita
                                TrxSuccessful=1;
                                break;
                            }
                }                     // Fine ciclo while per il trasferimento dell'immagine
                
                // Pulisci la memoria
                
                ilDeleteImages (1, &DevIL_ImageCode1);
                free (DevIL_LumpBuffer);
                
                // In presenza di problemi per il trasferimento dell'immagine esci
                if (!TrxSuccessful) 
                {
                     ExitCode = ERR_IMPOSSIBLE_TO_TRANSFER_A_PACKET;
                     goto _SENDTOSVROCR_ERR1;
                }
                
                // Tutta l'immagine è stata trasferita: inizia a prelevare il risultato OCR
                
                ErrRep=baBldSvr_ReceiveAShortPacket (BldServerCommPacketShort);
                    
                // Verifichiamo se nel pacchetto di risposta del server, è presente uno dei seguenti errori    
                // errori di trasmissione
                // tag errato nel pacchetto    
                    
                if ( (ErrRep) || (BldServerCommPacketShort->Message!=BLDSVR_CLIENT_IS_SENDING_AN_IMAGE_FOR_OCR______TAGRESULT) )
                {
                     ExitCode = ERR_RESPONSE_CODE_MISMATCH;
                     goto _SENDTOSVROCR_ERR1;
                }
                
                // Recupera il numero di blocchi in ricezione     
                TotalBlocksInRecv = BldServerCommPacketShort->Data0; 
                      
                // Verifica che non ci sia un errore remoto dell'OCR      
                if (TotalBlocksInRecv<0) 
                {
                     ExitCode = ERR_REMOTE_OCR_TROUBLE;
                     goto _SENDTOSVROCR_ERR1;
                }
                
                // Inizio operazioni di recupero testo OCR
                
                BlockCounter  = 0;
                YouCanExit    = 0;
                RcvSuccessful = 0;
                
                while (!YouCanExit)
                {
                    _SENDTOSVROCR_TryToRequireAPacket:
                    
                        FailedAttempts=0;
                        
                    _SENDTOSVROCR_ReTryToRequireAPacket:
                        
                        // Invia un messaggio attraverso il quale si richiede un certo pacchetto
                        
                        ErrRep = baBldSvr_SendAShortPacket (BldServerCommPacketShort, BLDSVR_CLIENT_IS_SENDING_AN_IMAGE_FOR_OCR______REQRESULT, 0, BlockCounter, 0);
                        
                        if (ErrRep) 
                        {
                            FailedAttempts++;
                                 
                            if (FailedAttempts<3)  goto _SENDTOSVROCR_ReTryToRequireAPacket;
                               else break; 
                        }
    
                        // Richiesta inviata: adesso provvedi a caricare i dati di ritorno dal server
                       
                        ErrRep = baBldSvr_ReceiveAPacket (BldServerCommPacket);
                       
                        // Adesso verifichiamo che non ci siano:
                        // Errori di ricezione
                        // Errori di tag
                        // Errori di checksum 
                       
                        CheckSum = baBldSvr_CalculateCheckSum (BldServerCommPacket);
                                   
                        if ( (ErrRep) ||
                             (BldServerCommPacket->Message != BLDSVR_CLIENT_IS_SENDING_AN_IMAGE_FOR_OCR______RETRVRESULT) ||
                             (CheckSum != BldServerCommPacket->Data0) )
                        {
                            FailedAttempts++;
                                 
                            if (FailedAttempts<3)  goto _SENDTOSVROCR_ReTryToRequireAPacket;
                               else break; 
                        }
                              
                        // Se il sistema arriva qui, allora di sicuro il testo è stato trasferito con
                        // successo. 
                        
                        for (Counter=0; Counter<=255; Counter++)
                        { 
                             *(char *)(TextBuffer + 256*BlockCounter + Counter) = BldServerCommPacket->Data.Linear [Counter]; 
                        }        
                         
                        // Preparati a ricevere un nuovo pacchetto
                        
                        BlockCounter++;
                        
                        if (BlockCounter>TotalBlocksInRecv)     // Ricezione avvenuta completamente ?
                        {
                           // Sì, invia il segnale di chiusura al server
                           
                           baBldSvr_SendAShortPacket (BldServerCommPacketShort, BLDSVR_CLIENT_IS_SENDING_AN_IMAGE_FOR_OCR______CLOSERESULT, 0, BlockCounter, 0);
                           
                           RcvSuccessful =1; 
                           break;                              
                        }
                 }                  // End of while cycle
                       
                 // Arrivati in questo punto, il sistema può avere completato la ricezione
                 // del testo oppure no...
                 
                 if (!RcvSuccessful) 
                 {
                    ExitCode=ERR_IMPOSSIBLE_TO_RECEIVE_A_PACKET;             
                    goto _SENDTOSVROCR_ERR1;
                 }
                   
                 // Gestisci le operazioni finali  
                 
                 baBldSvr_HandOff();
                 return 0;
                   
              _SENDTOSVROCR_ERR1:
                 
                 baBldSvr_HandOff();                
                 return ExitCode;
                                    
        }
        else    // Problemi nella compressione
        {
            ilDeleteImages (1, &DevIL_ImageCode1);
            free (DevIL_LumpBuffer);
          
            baBldSvr_HandOff();
            return ERR_IMPOSSIBLE_TO_COMPRESS_IMAGE;
        }   
    }
    else    // Problemi nell'allocazione del LumpBuffer
    {  
      baBldSvr_HandOff(); 
      return ERR_IMPOSSIBLE_TO_ALLOC_LUMPBUFFER;
    }
}


void baBldSvr_CloseConnectionWithBlindServer (void)
{
    // A partire da BlindServer 1.1, non ha alcun senso chiudere la connessione
    // con BlindServer, perchè tanto il socket viene ricreato e richiuso 
    // automaticamente al termine di ogni operazione (in BlindServer 1.0, 
    // invece, il socket era creato unicamente all'inizio del processo
    // di connessione). La routine viene mantenuta esclusivamente per
    // ragioni di compatibilità. 
}


void OLD_BldSvr_V1_0_CloseConnectionWithBlindServer (void)
{
     // Questa vecchia routine si occupa di disconnettere il BlindServer versione 1.0,
     // se esso viene richiamato dal sistema. Essa agisce solo allo scopo di concludere
     // la connessione con la vecchia versione del BlindServer, dopo che il client si
     // è accorto che la sua versione è troppo vecchia

     baBldSvr_SendAPacket (BldServerCommPacket, BLDSVR_CLOSE, 0, 0, 0);
     return;                      
}


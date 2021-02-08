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
   GESTIONE CONNESSIONE PRINCIPALE
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

char DedBldSvrConnCreated = 0;

/*
   ----------------------------------------------------------------------------
   CONNESSIONE PRINCIPALE ALLA RETE
   ----------------------------------------------------------------------------
*/ 
 
 
void baEnableMainNetworkConnection (void)
/*
    Questa routine provvede ad attivare la connessione ad internet principale
*/
{
    int ErrRep;
    
    if (SystemOptPacket.ConnectToNetAtStartup)
    {
        ErrRep=ndHAL_WLAN_GetAccessPointInfo (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj);
         
        if (!ErrRep)           // Non ci sono problemi
        {
             ErrRep=ndHAL_WLAN_OpenPSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj, ND_SILENT_AP_ACCESS, 10);
             
             if (!ErrRep)             // Connessione eseguita
             {
                 MainNetConn_IsOperative = 1;
                 return;
             }
        }
    }
    
    // Se il sistema arriva qui, vuol dire che che la connessione ad internet principale
    // non è operativa    
        
    MainNetConn_IsOperative = 0;
}


void baDisableMainNetworkConnection (void)
/*
    Provvede a disattivare la connessione ad internet principale
*/
{
    if (MainNetConn_IsOperative)
    {
        ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1); 
        MainNetConn_IsOperative = 0;          
    }
}



/*
   ----------------------------------------------------------------------------
   CONNESSIONE PRINCIPALE AL BLIND SERVER (MODO AUTOMATICO)
   ----------------------------------------------------------------------------
*/ 

void baEnableMainConnToBlindServer (void)
{
     char WndHandle;
     char ErrCode;
     
     MainBldSvrConn_IsOperative=0;       // Da cambiarsi in seguito....
     
     if (MainNetConn_IsOperative)
     {
        if (SystemOptPacket.ConnectToBldSvrAtStartup)
        {
            // Attiva la finestra per comunicare i risultati
            
            WndHandle = ndLP_CreateWindow (70, Windows_MaxScreenY/2 - 60, Windows_MaxScreenX - 70, Windows_MaxScreenY/2 + 60,
                               "Connecting to Blind Server", COLOR_WHITE, COLOR_BLUE, COLOR_GRAY, COLOR_GRAY, 0); 
            
            if (WndHandle>=0)
            {
                 ndLP_MaximizeWindow (WndHandle);
                 
                 // Tenta la connessione con il BlindServer
                 ErrCode=baBldSvr_TryAConnectToBlindServer ();  
                 
                 switch (ErrCode)
                 {
                     case 0:     // Nessun problema
                     {
                          ndWS_GrWriteLn (80, 40, "Connection successful", COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);
                          XWindowRender (WndHandle); 
                          
                          MainBldSvrConn_IsOperative=1;
                          break;
                     }   
                          
                     case ERR_IMPOSSIBLE_TO_DETERMINE_BLDSVR_IP:    // Problemi a determinare l'indirizzo IP del BlindServer
                     {
                          ndWS_GrWriteLn (5, 20, "Impossible to determine the IP",    COLOR_RED, COLOR_GRAY, WndHandle, NORENDER);            
                          ndWS_GrWriteLn (5, 30, "address of your BlindServer.  ",    COLOR_RED, COLOR_GRAY, WndHandle, NORENDER); 
                          ndWS_GrWriteLn (5, 40, "Check that the address that you",   COLOR_RED, COLOR_GRAY, WndHandle, NORENDER); 
                          ndWS_GrWriteLn (5, 50, "have entered in the Options   ",    COLOR_RED, COLOR_GRAY, WndHandle, NORENDER); 
                          ndWS_GrWriteLn (5, 60, "dialog-box is correct.        ",    COLOR_RED, COLOR_GRAY, WndHandle, NORENDER); 
                          XWindowRender (WndHandle); 
                                     
                          break;           
                     }
                     
                     case ERR_IMPOSSIBLE_TO_CREATE_BLDSVR_SOCKET:
                     {
                          ndWS_GrWriteLn (5, 40, "Troubles in socket opening.   ",   COLOR_RED, COLOR_GRAY, WndHandle, NORENDER);            
                          XWindowRender (WndHandle); 
                     
                          break;
                     } 
                     
                     case ERR_IMPOSSIBLE_TO_CONNECT_TO_BLDSVR:
                     {
                         ndWS_GrWriteLn (5, 40, "I cannot connect to BlindServer",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                         ndWS_GrWriteLn (5, 50, "Host unreachable.    ",             COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                         XWindowRender (WndHandle); 
                         
                         break;
                     }
                    
                     case ERR_AUTHENTIFICATION_FAILED:
                     {
                         ndWS_GrWriteLn (5, 40, "Authentification failed. Check  ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                         ndWS_GrWriteLn (5, 50, "that the password is correct.   ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                         XWindowRender (WndHandle); 
                         
                         break;
                     }
                     
                     case ERR_IMPOSSIBLE_TO_SEND_PASSWORD_PACKET:
                     case ERR_IMPOSSIBLE_TO_RECV_PASSWORD_RESPONSE:
                     {
                         ndWS_GrWriteLn (5, 40, "Impossible to send/recv password", COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                         ndWS_GrWriteLn (5, 50, "response ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                         XWindowRender (WndHandle); 
                         
                         break;
                     }
                     
                     case ERR_SERVER_VERSION_MISMATCH:
                     {
                         ndWS_GrWriteLn (5, 40, "This program has been designed for", COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                         ndWS_GrWriteLn (5, 50, "BlindServer vers. 1.2 ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                         XWindowRender (WndHandle); 
                         
                         break;
                     }
                     
                         
                 } 
                 
                 ndHAL_ClockDelay (2);
                 
                 // Disattiva la finestra
                 ndLP_DestroyWindow (WndHandle);
            }   
        } 
     }
       
           
}

void baDisableMainConnToBlindServer (void)
{
    if (MainBldSvrConn_IsOperative)
    {
        baBldSvr_CloseConnectionWithBlindServer ();
        MainBldSvrConn_IsOperative=0;            
    } 
}



/*
   ----------------------------------------------------------------------------
   CODICE: CONNESSIONE/DISCONNESSIONE MANUALE
   ----------------------------------------------------------------------------
*/ 
 
void bldsvrcnx_ServerManualConnect (void)
{
     char WndHandle;
     int  ErrCode, ErrRep;
     
     if (!(MainNetConn_IsOperative && MainBldSvrConn_IsOperative))
     {
         if (!NetworkIsBusy)
         {
                  // Innanzitutto verifica se è necessario creare una connessione dedicata
                  
                  if (!MainNetConn_IsOperative)
                  {
                        ErrRep=ndHAL_WLAN_GetAccessPointInfo (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj);
         
                        if (!ErrRep)           // Non ci sono problemi
                        {
                             ErrRep=ndHAL_WLAN_OpenPSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1, &NetObj, ND_SILENT_AP_ACCESS, 10);
                             
                             if (!ErrRep)             // Connessione eseguita
                             {
                                 MainNetConn_IsOperative = 1;
                                 DedBldSvrConnCreated      = 1;       
                             }
                        }
                                         
                  }
                  else     // C'era già una connessione principale di rete in funzione. Simula successo
                  {
                        DedBldSvrConnCreated      = 0; 
                        ErrRep                  = 0;   
                  }
                  
                  // Adesso verifica se puoi connetterti con il BlindServer
                  
                  if (!ErrRep)
                  {
               
                            // Attiva la finestra per comunicare i risultati
                            
                            WndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 310, Windows_MaxScreenY - 100, Windows_MaxScreenX - 10, Windows_MaxScreenY - 10,
                                               "Connecting to Blind Server", COLOR_WHITE, COLOR_GREEN, COLOR_GRAY, COLOR_GRAY, 0); 
                            
                            if (WndHandle>=0)
                            {
                                 ndLP_MaximizeWindow (WndHandle);
                                 
                                 // Tenta la connessione con il BlindServer
                                 ErrCode=baBldSvr_TryAConnectToBlindServer ();  
                                 
                                 switch (ErrCode)
                                 {
                                     case 0:     // Nessun problema
                                     {
                                          ndWS_GrWriteLn (5, 25, "Connection successful", COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);
                                          XWindowRender (WndHandle); 
                                          
                                          MainBldSvrConn_IsOperative=1;
                                          break;
                                     }   
                                     
                                     // Problemi a determinare l'indirizzo IP del BlindServer     
                                     case ERR_IMPOSSIBLE_TO_DETERMINE_BLDSVR_IP:    
                                     {
                                          ndWS_GrWriteLn (5, 5,  "Impossible to determine the IP",    COLOR_RED, COLOR_GRAY, WndHandle, NORENDER);            
                                          ndWS_GrWriteLn (5, 15, "address of your BlindServer.  ",    COLOR_RED, COLOR_GRAY, WndHandle, NORENDER); 
                                          ndWS_GrWriteLn (5, 25, "Check that the address that you",   COLOR_RED, COLOR_GRAY, WndHandle, NORENDER); 
                                          ndWS_GrWriteLn (5, 35, "have entered in the Options   ",    COLOR_RED, COLOR_GRAY, WndHandle, NORENDER); 
                                          ndWS_GrWriteLn (5, 45, "dialog-box is correct.        ",    COLOR_RED, COLOR_GRAY, WndHandle, NORENDER); 
                                          XWindowRender (WndHandle); 
                                                     
                                          break;           
                                     }
                                     
                                     case ERR_IMPOSSIBLE_TO_CREATE_BLDSVR_SOCKET:
                                     {
                                          ndWS_GrWriteLn (5, 25, "Troubles in socket opening.   ",   COLOR_RED, COLOR_GRAY, WndHandle, NORENDER);            
                                          XWindowRender (WndHandle); 
                                     
                                          break;
                                     } 
                                     
                                     case ERR_IMPOSSIBLE_TO_CONNECT_TO_BLDSVR:
                                     {
                                         ndWS_GrWriteLn (5, 25, "I cannot connect to BlindServer",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                                         ndWS_GrWriteLn (5, 35, "Host unreachable.    ",             COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                                         XWindowRender (WndHandle); 
                                         
                                         break;
                                     }
                                    
                                     case ERR_IMPOSSIBLE_TO_SEND_PASSWORD_PACKET:
                                     case ERR_IMPOSSIBLE_TO_RECV_PASSWORD_RESPONSE:
                                     {
                                         ndWS_GrWriteLn (5, 25, "Impossible send/recv password",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                                         ndWS_GrWriteLn (5, 35, "response",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                                         XWindowRender (WndHandle); 
                                         
                                         break;
                                     }
                                    
                                     case ERR_AUTHENTIFICATION_FAILED:
                                     {
                                         ndWS_GrWriteLn (5, 25, "Authentification failed. Check  ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                                         ndWS_GrWriteLn (5, 35, "that the password is correct.   ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                                         XWindowRender (WndHandle); 
                                         
                                         break;
                                     }
                                     
                                     case ERR_SERVER_VERSION_MISMATCH:
                                     {
                                         ndWS_GrWriteLn (5, 25, "This program has been designed  ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                                         ndWS_GrWriteLn (5, 35, "for BlindServer vers. 1.2       ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
                                         XWindowRender (WndHandle); 
                                         
                                         break;
                                     }
                                         
                                 } 
                                 
                                 ndHAL_ClockDelay (2);
                                 
                                 // Disattiva la finestra
                                 ndLP_DestroyWindow (WndHandle);
                            }                             
                             
                            // Arrivati qui, la connessione è già avvenuta oppure è fallita 
                             
                            if (!ErrCode)       // Nessun errore
                            {
                                 mainitf_StatusBar ("", RENDER);               
                            } 
                            else                // C'è stato qualche errore
                            {
                                 if (DedBldSvrConnCreated)   // Era stata creata una connesione di rete dedicata
                                 {
                                      ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1); 
                                      MainNetConn_IsOperative = 0;
                                 }
                                 
                                 mainitf_StatusBar ("BlindServer isn't connected", RENDER); 
                            } 
                                               
                  }
         }                          
     }   
}


void bldsvrcnx_ServerManualDisconnect (void)
{
    char WndHandle;
    
    WndHandle = ndLP_CreateWindow (Windows_MaxScreenX - 280, Windows_MaxScreenY - 100, Windows_MaxScreenX - 10, Windows_MaxScreenY - 10,
                                               "Disconnecting Blind Server", COLOR_WHITE, COLOR_GREEN, COLOR_GRAY, COLOR_GRAY, 0); 
                            
    if (WndHandle>=0)
    {
         ndLP_MaximizeWindow (WndHandle);
     
         baDisableMainConnToBlindServer ();
         ndWS_GrWriteLn (5, 25, "Disconnect done  ",   COLOR_BLACK, COLOR_GRAY, WndHandle, NORENDER);            
    
         ndHAL_ClockDelay (2);
         ndLP_DestroyWindow (WndHandle);        // Disattiva la finestra
    }
    
    if (DedBldSvrConnCreated)   // Era stata creata una connesione di rete dedicata
    {
         ndHAL_WLAN_ClosePSPAccessPoint (SystemOptPacket.FwNetworkProfileChoosen+1); 
         MainNetConn_IsOperative = 0;
    }
    
    mainitf_StatusBar ("BlindServer isn't connected", RENDER);  
}

 

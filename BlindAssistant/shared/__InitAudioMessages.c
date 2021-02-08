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
   GENERAZIONE DEI MESSAGGI UTENTE PER LA CACHE AUDIO
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



/*
   ----------------------------------------------------------------------------
   CALLBACK GRAFICA
   ----------------------------------------------------------------------------
*/

static void InitSysGraphCallback (int NrServMessage, int InitSysCode, struct OpInfoPacket_Type *InfoPacket)
{
    char MsgString0 [256];
    char MsgString1 [16];
    
    switch (InitSysCode)
    { 
           case BEGIN_TO_GEN_AUDIOMSGSAMPLE:
           {
              InfoPacket->WndHandle = ndLP_CreateWindow (50, 77, Windows_MaxScreenX-50, Windows_MaxScreenY-77,
                                                         "Generating audio samples", COLOR_WHITE, COLOR_GREEN,
                                                         COLOR_GRAY, COLOR_BLACK, 0);
              
              ndLP_MaximizeWindow (InfoPacket->WndHandle);
              
              ndWS_GrWriteLn (10, 10, "Waiting.... I'm regenerating the  ", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              ndWS_GrWriteLn (10, 18, "audio samples for service messages",   COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              XWindowRender (InfoPacket->WndHandle);
         
              ndTBAR_DrawProgressBar   (10, 45, 340, 65, 0, 0, 100, 
                                        COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              ndDelay (0.5);      
              break;
           }
           
           case GENERATING_AUDIOMSGSAMPLE:
           {
              ndWS_DrawRectangle (10, 10, 350, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              strcpy (MsgString0, "Generating sample for service message #");
              _itoa (NrServMessage, &MsgString1, 10);
              strcat (MsgString0, MsgString1);
              
              ndWS_GrWriteLn (10, 10, &MsgString0, COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 340, 65, 100.0*(float)(NrServMessage)/(float)(NR_SERVICES_MESSAGES), 0, 100, 
                                        COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case GENERATED_AUDIOMSGSAMPLE:
           {
              ndWS_DrawRectangle (10, 10, 350, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              strcpy (MsgString0, "Generated sample for service message #");
              _itoa (NrServMessage, &MsgString1, 10);
              strcat (MsgString0, MsgString1);
              
              ndWS_GrWriteLn (10, 10, &MsgString0, COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 340, 65, 100.0*(float)(NrServMessage+1)/(float)(NR_SERVICES_MESSAGES), 0, 100, 
                                        COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
              break;  
           }
           
           case DONE0:
           {
              ndWS_DrawRectangle (10, 10, 340, 30, COLOR_GRAY, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndWS_GrWriteLn (10, 10, "Generation complete", COLOR_BLACK, COLOR_GRAY, InfoPacket->WndHandle, NORENDER);
              
              ndTBAR_DrawProgressBar   (10, 45, 340, 65, 100, 0, 100, 
                                        COLOR_WHITE, COLOR_LBLUE, COLOR_GRAY03, COLOR_BLACK, 
                                        PBAR_PERCENT, InfoPacket->WndHandle, RENDER);  
             
              ndDelay (0.5);
              ndLP_DestroyWindow (InfoPacket->WndHandle);
             
              break;
           }
           
           
    }   
}





/*
   ----------------------------------------------------------------------------
   CODICE
   ----------------------------------------------------------------------------
*/ 

void INTERNAL_GenerateSingleCachedSample (int NrServMessage, char *Message, struct OpInfoPacket_Type *PntToInitFsInfoPacket)
{
    char NameFile0 [256];
    char NameFile1 [16];
    
    strcpy (NameFile0, "ms0:/BLINDASSISTANT/SERVICEMSG/MSG");
    _itoa  (NrServMessage, &NameFile1, 10);
    strcat (NameFile0, NameFile1);
    strcat (NameFile0, ".WAV");
    
    InitSysGraphCallback (NrServMessage, GENERATING_AUDIOMSGSAMPLE, PntToInitFsInfoPacket);
      
    if (!fexist (NameFile0))
    {  
       flite_text_to_speech (Message, VoiceObj, &NameFile0);         
    }
    
    InitSysGraphCallback (NrServMessage, GENERATED_AUDIOMSGSAMPLE, PntToInitFsInfoPacket);
}


void InitAudioMessages ()
{
     static struct OpInfoPacket_Type InitFsInfoPacket;
     
     // Crea la finestra necessaria
     InitSysGraphCallback (0, BEGIN_TO_GEN_AUDIOMSGSAMPLE, &InitFsInfoPacket);
     
     // Genera i messaggi richiesti
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_WELLCOME_TO_BA, "Welcome to Blind Assistant", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_VISILAB_RELEASED_UPD, "Visilab has released a new update", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_LL_DOWNLOAD_IT_NOW, "I will download it now", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_DOWNLOAD_SUCCESS, "Download success", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_DOWNLOAD_FAILED, "Download failed", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_WRONG_NETWORK_CONFIG, "Wrong network config", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_LL_INSTALL_NOW, "I will install it now", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_INSTALL_COMPLETE, "Installation complete", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_THE_SYSTEM_WILL_BE_REBOOTED, "The system will be rebooted now", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_WAIT, "Wait", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_NOLIGHT, "The image is too dark", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_DONT_KNOW_WHERE_YOU_ARE, "I don't know where you are", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_TRASMISSION_COMPLETE, "Trasmission complete", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_BLINDSERVER_IS_NOT_CONNECTED, "Blind Server is not connected", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_IMPOSSIBLE_TO_DOWNLOAD_DICT, "I cannot download dictionary", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_LOADING, "Loading", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_GO, "Go", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_WAIT_I_AM_FIXING_A_PROBLEM, "Wait. I am fixing a problem.", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PRESS_LTRIGGER_OR_RTRIGGER, "Press left trigger for yes, and right trigger for no.", &InitFsInfoPacket);
     
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PRESS_SQUARE_AND_SAY_THE_CMD, "Please, press the SQUARE button... and say your command for this system ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_AVAILABLE_COMMANDS_ARE, "The available commands are ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_AM_LISTENING_FOR_YOUR_NEXT_COMMAND, "I am listening for your next command ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_COMMAND_NOT_RECOGNIZED, "Command not recognized ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_VOICEREC_SYSTEM_NOT_SUPPORTED, "Voice recognition system is not supported under this platform", &InitFsInfoPacket);

     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_ENABLE_NO_FUNCTION_MODE, "Enable no function mode ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_ENABLE_FACE_RECOGNITION_SYSTEM, "Enable face recognizer ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_ENABLE_OPTICAL_CHAR_RECOGNITION, "Enable optical char recognizer ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_ENABLE_COLOR_SCANNER, "Enable color scanner ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_ENABLE_MAIL_READER, "Enable mail reader ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_ENABLE_TICKETS_SCANNER, "Enable tickets scanner ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_ENABLE_DATA_MATRIX_SCANNER, "Enable data matrix scanner ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_ENABLE_POS_RECOGNITION_SYSTEM, "Enable position recognizer ", &InitFsInfoPacket);
     
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_NO_FUNCTION_MODE_ENABLED, "No function mode enabled ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_FACE_RECOGNITION_SYSTEM_ENABLED, "Face recognition system enabled ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_OPTICAL_CHAR_RECOGNITION_ENABLED, "Optical char recognition system enabled ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_COLOR_SCANNER_ENABLED, "Color scanner enabled ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_MAIL_READER_ENABLED, "Mail reader enabled ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_TICKET_SCANNER_ENABLED, "Ticket scanner enabled ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_DATA_MATRIX_SCANNER_ENABLED, "Data matrix scanner enabled ", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_POS_RECOGNITION_SYSTEM_ENABLED, "Position recognizer system enabled ", &InitFsInfoPacket);
      
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_CANNOT_CONTACT_POP3_SERVER, "I cannot contact the mail server", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_THERE_IS_NO_NEW_MAIL_FOR_YOU, "There is no new mail for you", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_DO_YOU_WANT_TO_LISTEN_NEW_MESSAGES, "Do you want to listen new messages ?", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_THE_NEXT_MAIL_IS_SENT_BY, "The next mail is sent by", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_DO_YOU_WANT_TO_LISTEN_THIS_MESSAGE, "Do you want to listen this mail ?", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_DO_YOU_WANT_TO_LISTEN_AGAIN, "Do you want to listen again ?", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_DO_YOU_WANT_TO_DELETE_THIS_MESSAGE, "Do you want to delete this message from server ?", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_THIS_MESSAGE_WILL_BE_DELETED, "This mail will be deleted by server", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_CANNOT_DETERMINE_SENDER,  "I cannot determine sender", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_CANNOT_DETERMINE_SUBJECT, "I cannot determine subject", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_INTERNAL_ERROR, "Internal error", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_CANNOT_DECODE_MAIL_CONTENT, "Error: I cannot decode the content of the mail", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_I_DISCOVERED_YOU_HAVE_UNREADED_MAIL, "You have unread mails. Please switch to mail reader mode for accessing to them.", &InitFsInfoPacket);
     
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_NDTESSERACT_INIT_ERROR, "n d Tesseract initialization error", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_NDTESSERACT_INTERNAL_ERROR, "n d Tesseract internal error", &InitFsInfoPacket);
     
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_00_PERCENT, "0 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_10_PERCENT, "10 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_20_PERCENT, "20 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_30_PERCENT, "30 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_40_PERCENT, "40 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_50_PERCENT, "50 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_60_PERCENT, "60 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_70_PERCENT, "70 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_80_PERCENT, "80 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_90_PERCENT, "90 per cent", &InitFsInfoPacket);
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED_100_PERCENT, "Completed", &InitFsInfoPacket);
     
     INTERNAL_GenerateSingleCachedSample (AUDIOMSG_PROCEED, "Proceed", &InitFsInfoPacket);
       
     // Distruggi la finestra di controllo
     InitSysGraphCallback (0, DONE0, &InitFsInfoPacket);
}

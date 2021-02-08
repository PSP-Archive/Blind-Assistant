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
   FILE HEADERS
   ----------------------------------------------------------------------------
*/




#ifndef _BA_HEADERS_
        #define _BA_HEADERS_

        #include "cxcore.h"
        #include "cv.h"  
        #include <cvaux.h>
        
        #include <ndhighgui.h>
        #include <flite.h>
        
        #include <il.h>
        
        #include <gcurl/gcurl.h>
        #include <libspopc.h>
        

#ifdef __cplusplus
extern "C" {
#endif


// ----------------------------------------------------------------------------
// MACRO DI GESTIONE
// ----------------------------------------------------------------------------


#ifndef BA_NEED_VARIABLES
       #define BAEXTERN extern
#else
       #define BAEXTERN
#endif

// ----------------------------------------------------------------------------
// SIMBOLI DI CONFIGURAZIONE
// ----------------------------------------------------------------------------


// Modalità no function

#define STD_NOFUNC__CAMERAIMG__DIMX 320
#define STD_NOFUNC__CAMERAIMG__DIMY 240

// Sistema di riconoscimento del viso

#define STD_FACEREC__SCALEDIMGFD__DIMX 213
#define STD_FACEREC__SCALEDIMGFD__DIMY 160

#define STD_FACEREC__CAMERAIMG__DIMX 640
#define STD_FACEREC__CAMERAIMG__DIMY 480

#define STD_FACEREC__SAMPLEIMG__DIMX 320
#define STD_FACEREC__SAMPLEIMG__DIMY 240

// Eigen

#define NR_SLOTS_FACE_RECOGNIZER 10
#define NR_SAMPLES_FOR_FACE   8

#define VOICE_SAMPLES_FOR_FACE 3
#define NR_PRESENCE_ELEMENTS_IN_REG  2

// Soglia di illuminazione minima di default

#define MIN_BRIGHTNESS_THRESHOLD_DEFAULT   55.0
#define PCA_SKIP_THRESHOLD_DEFAULT         990
#define FACEDETECTOR_PROBING_AREA_DEFAULT  35
#define FACEDETECTOR_THRESHOLD_DEFAULT     130

// Sistema di riconoscimento dei luoghi

#define STD_BLDSVR__CAMERAIMG__DIMX 480
#define STD_BLDSVR__CAMERAIMG__DIMY 272

#define STD_BLDSVR__SAMPLEIMG__DIMX 480         // Questi valori sono memorizzati nel BlindServer
#define STD_BLDSVR__SAMPLEIMG__DIMY 272         // come simboli STD_BLDSVR_IMG_DIMX e STD_BLDSVR_IMG_DIMY

#define MAX_PLACE_SLOTS 30
#define NR_SAMPLES_FOR_PLACE 8

// Sistema OCR

#ifndef PSP1000_VERSION

    #define STD_OCR__SAMPLEIMG__DIMX 1024             // Questi valori sono memorizzati nel BlindServer
    #define STD_OCR__SAMPLEIMG__DIMY 768              // come simboli STD_OCR_IMG_DIMX e STD_OCR_IMG_DIMY
    
    #define STD_OCR__CAMERAIMG__DIMX STD_OCR__SAMPLEIMG__DIMX
    #define STD_OCR__CAMERAIMG__DIMY STD_OCR__SAMPLEIMG__DIMY

#else

    #define STD_OCR__SAMPLEIMG__DIMX 640             // Questi valori sono memorizzati nel BlindServer
    #define STD_OCR__SAMPLEIMG__DIMY 480             // come simboli STD_OCR_IMG_DIMX e STD_OCR_IMG_DIMY
    
    #define STD_OCR__CAMERAIMG__DIMX STD_OCR__SAMPLEIMG__DIMX
    #define STD_OCR__CAMERAIMG__DIMY STD_OCR__SAMPLEIMG__DIMY

#endif

// Scanner colori

#define STD_COLORSCANNER__CAMERAIMG__DIMX 480
#define STD_COLORSCANNER__CAMERAIMG__DIMY 272

// Data matrix scanner

#define STD_DATAMATRIX__CAMERAIMG__DIMX 320
#define STD_DATAMATRIX__CAMERAIMG__DIMY 240



// ----------------------------------------------------------------------------
// MODALITA' DI FUNZIONAMENTO DI BLIND ASSISTANT
// ----------------------------------------------------------------------------

BAEXTERN char baMode, baPrevMode, baForce, baManualSwitchToMode;


#define BA_MODE_NOWORK       0
#define BA_MODE_FACEREC      1
#define BA_MODE_POSREC       2
#define BA_MODE_OCR          3
#define BA_MODE_COLORSCANNER 4
#define BA_MODE_MAILREADER   5
#define BA_MODE_DATAMATRIX   6

#define BA_NR_MODES          7


// ----------------------------------------------------------------------------
// SIMBOLI PER LA GESTIONE DELL'AUDIO SERVER
// ----------------------------------------------------------------------------

#define AUDIOSVR_MSG_IS_READY          1

// ----------------------------------------------------------------------------
// SIMBOLI PER LA GESTIONE DEL PLACE SERVER
// ----------------------------------------------------------------------------

#define PLACESVR_MSG_IS_READY          1

// ----------------------------------------------------------------------------
// CODICI OCR
// ----------------------------------------------------------------------------

#define OCRMETHOD_TESSERACT_VIA_BLINDSERVER       0x0000
#define OCRMETHOD_NDTESSERACT                           0x0001

#define OCRLANG_ENGLISH                           0x0000
#define OCRLANG_ITALIAN                           0x0001
#define OCRLANG_FRANCOIS                          0x0002
#define OCRLANG_GERMAN                            0x0003
#define OCRLANG_SPANISH                           0x0004
#define OCRLANG_DUTCH                             0x0005



// ----------------------------------------------------------------------------
// FILE OPZIONI ED AUTOAGGIORNAMENTO
// ----------------------------------------------------------------------------

#define BA_OPTIONS_FILE  "ms0:/BLINDASSISTANT/BAOPTIONS.CFG"
#define BA_UPDATES_FILE  "ms0:/BLINDASSISTANT/UPDATESLIST.TXT"
#define BA_SITE          "http://visilab.unime.it/~filippo/BlindAssistant/"
#define BA_UPDATE_PATH   "UpdateZone/"

#define BA_DICTIONARY_PATH        "Dictionaries/"
#define BA_DICTIONARY_ZIP_NAME    "Dictionaries.zip"

#define BA_VOCAL_DICT_PATH        "VocDictionaries/"
#define BA_VOCAL_DICT_ENGLISH     "VocDictionary_English.zip"                     
#define BA_VOCAL_DICT_ITALIAN     "VocDictionary_Italian.zip"                             
#define BA_VOCAL_DICT_FRANCOIS    "VocDictionary_Francois.zip"                       
#define BA_VOCAL_DICT_GERMAN      "VocDictionary_German.zip"                         
#define BA_VOCAL_DICT_SPANISH     "VocDictionary_Spanish.zip"                       
#define BA_VOCAL_DICT_DUTCH       "VocDictionary_Dutch.zip"                         

#ifndef PSP1000_VERSION
        #define BA_CORE_FILE     "ms0:/BLINDASSISTANT/CORE/EBOOT.PBP"  
#else
        #define BA_CORE_FILE     "ms0:/BLINDASSISTANT/CORE_FOR_FAT/EBOOT.PBP"  
#endif      

// ----------------------------------------------------------------------------
// SISTEMA DI CONTROLLO VOCALE
// ----------------------------------------------------------------------------

#define BA_VOICECTRL_LANGUAGE_MODEL "ms0:/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.lm"
#define BA_VOICECTRL_DICTIONARY     "ms0:/BLINDASSISTANT/SPHINX/model/lm/BlindAssistant/BlindAssistant.dic"
#define BA_VOICECTRL_HMMDIR         "ms0:/BLINDASSISTANT/SPHINX/model/hmm/wsj1"

#define BA_VOICECTRL_PATH           "VoiceControl/"
#define BA_VOICECTRL_ZIPNAME        "sphinx01.zip"

// ----------------------------------------------------------------------------
// STRUTTURE
// ----------------------------------------------------------------------------

   struct RectCoord_Type 
   {
          unsigned short int PosX1, PosY1, PosX2, PosY2;
   };
   
   struct FDRecord_Type
   {
          int NrFaces;
          struct RectCoord_Type RectCoord [10];
          int  Area [10];
   };
   
   struct OpInfoPacket_Type
   {
          unsigned short int PosX1, PosY1;
          unsigned short int PosX2, PosY2;
          TypeColor BarColor; 
          TypeColor BgColor; 
          char WndHandle; 
   };
   
   struct OptionsPacket_Type
   {
          int  ConnectToNetAtStartup;
          int  ConnectToBldSvrAtStartup;
          
          int  EnableWallPaper; 
          int  NewWallPaperIsFailure;
          char WallPaperName [512];
          
          int  CheckForUpdate;
          char UpdateFromSite [512];
          int  MinBetweenChecks;
          
          int  AutoDownloadNewUpdates;
          int  AutoInstallNewUpdates;
          
          char FwNetworkProfileChoosen;
          int  HttpTunneling;
          int  BeVerbose;
          
          float MinBrightnessLevel;
          float SkipThreshold;
          float FaceDetectorProbingArea;
          float FaceDetectorThreshold;   
          
          char AnOptionHasBeenChanged;
          
          char BlindServerAddrStr [128];
          int  BlindServerPort;
          char Password [32];
          
          int  OcrMethod;
          int  OcrLanguage;
          int  OcrSkipUnknownWords;
          int  OcrRTGenerateWords;      // Mantenuto solo per ragioni di compatibilità
          
          int  CheckForOldTS_FaceDetect;
          
          char Pop3ServerAddr [128];
          char Pop3UserName [32];
          char Pop3Password [32];
          int  Pop3Port;
          
          int  CheckForNewMails;
          int  MinBetweenChecksForMails;
   };


// ----------------------------------------------------------------------------
// VARIABILI COMUNI
// ----------------------------------------------------------------------------

   BAEXTERN CvCapture *cvCameraCaptureObj;
   
   BAEXTERN CvMemStorage* FDStorage;
   BAEXTERN CvHaarClassifierCascade* FDCascade;

   BAEXTERN cst_voice *VoiceObj;

   BAEXTERN IplImage *cvCameraImage;


// ----------------------------------------------------------------------------
// RIFERIMENTI A VARIABILI DEFINITE INTERNAMENTE
// ----------------------------------------------------------------------------

   extern int  wzftExitReason;      
   extern int  wzftYouCanExit;
   
   extern int  wztestExitReason;      
   extern int  wztestYouCanExit;
   
   extern int  wzvwsample_ExitReason;
   
   extern char wzvwsample_IsAlreadyOpened; 
   extern char wzft_IsAlreadyOpened; 
   extern char wzpl_IsAlreadyOpened; 
   extern char wztest_IsAlreadyOpened;

   extern int  about_WndAlreadyOpen;
      
   extern int  vdengThreadID;
   extern int  vdengYouCanExit;
   
   extern int  audengThreadID;
   extern int  audengEventID; 
   extern int  audengYouCanExit;
   extern int  audengServerIsBusy; 
   
   extern int  updengThreadID;
   extern int  updengYouCanExit;
   extern int  updengThreadPriority;
   
   extern int  mailchkThreadID;
   extern int  mailchkYouCanExit;
   extern int  mailchkThreadPriority;
   
   extern int  baMainThreadPriority;
   extern int  baMainThreadID;
   
   extern int  mainitf_Window; 
   extern int  mainitf_EventMon;

   extern char baNewExecRequested;
   
   

// ----------------------------------------------------------------------------
//  STRUCT FACEREC
// ----------------------------------------------------------------------------

   struct presence_Type
   {
          char ThereWas;
          unsigned short int PosX, PosY;
   };

   struct facerec_ImagesInRam_Type
   {
          char          IsActive;
          char          Name [33];
          IplImage*     Img [NR_SAMPLES_FOR_FACE];
          float         GeneralDistance;
          
          struct presence_Type Presence [NR_PRESENCE_ELEMENTS_IN_REG];
   };
   
   struct facerec_VectorForOrder_Type
   {
          char       IsActive;
          char       NrSlot;
          float      Distance;
          float      NormDistance;
   };
   
   BAEXTERN struct facerec_ImagesInRam_Type     facerec_ImagesInRam  [NR_SLOTS_FACE_RECOGNIZER];
   BAEXTERN struct facerec_VectorForOrder_Type  facerec_VectorOrder  [NR_SLOTS_FACE_RECOGNIZER];
   
   BAEXTERN int    facerec_MinimumElement,  facerec_SecMinimumElement;
   BAEXTERN float  facerec_MinimumDistance, facerec_SecMinimumDistance;
      
   BAEXTERN int    cvNrInputImages;
   
   
   
   
// ----------------------------------------------------------------------------
//  STRUCT OPTIONS
// ----------------------------------------------------------------------------
   
   BAEXTERN struct OptionsPacket_Type SystemOptPacket;

// ----------------------------------------------------------------------------
//  SUPPORTO DI RETE
// ----------------------------------------------------------------------------

   BAEXTERN struct ndNetworkObject_Type NetObj;
   BAEXTERN struct gCurl_FileTransfer_Options  NetworkOptions;

   BAEXTERN char   MainNetConn_IsOperative;
   BAEXTERN char   MainBldSvrConn_IsOperative;
   
   BAEXTERN char   NetworkIsBusy;    

// ----------------------------------------------------------------------------
// CODICI DI ERRORE
// ----------------------------------------------------------------------------
   
   #define BAERR_TROUBLE_WITH_GRAPH -1
   #define BAERR_ABORTED_BY_USER    -2
   #define BAERR_IN_SERVER_CONN     -3
   
// ----------------------------------------------------------------------------
// ALTRI CODICI
// ----------------------------------------------------------------------------

   #define BA_ABORT  -1
   #define BA_RETRY   1                                                                    

// ----------------------------------------------------------------------------
// SIMBOLI PER IL GESTORE DI SALVATAGGIO
// ----------------------------------------------------------------------------

   #define BEGIN_TO_SAVE                 1
   #define SAVING_AUDIO_SAMPLE           2
   #define SAVED_AUDIO_SAMPLE            3
   #define SAVING_IMG_SAMPLE             4
   #define SAVED_IMG_SAMPLE              5
   #define ERROR_IN_SAVING_IMG_SAMPLE    6
   #define SAVING_NAME                   7
   #define SAVED_NAME                    8
   #define ERROR_IN_SAVING_NAME          9
   #define BEGIN_TO_ERASE                10
   #define ERASING_MEMORY                11
   #define ERASING_VOICE_SAMPLE          12
   #define ERASED_VOICE_SAMPLE           13
   #define ERASING_IMAGE_SAMPLE          14
   #define ERASED_IMAGE_SAMPLE           15
   #define ERASING_NAME                  16
   #define ERASED_NAME                   17
   #define BEGIN_TO_GENFS                18
   #define I_HAVE_DONE_SINGLE_GENFS      19
   #define BEGIN_TO_LOAD_IMG_SAMPLE      20
   #define LOADING_IMG_SAMPLE            21
   #define LOADED_IMG_SAMPLE             22
   #define ERROR_IN_LOADING_IMG_SAMPLE   23
   #define ERROR_IN_OPENING              24
   #define BEGIN_TO_XSAVE                25
   #define XSAVING_AUDIO_SAMPLE          26
   #define XSAVED_AUDIO_SAMPLE           27
   #define XSAVING_IMG_SAMPLE            28
   #define XSAVED_IMG_SAMPLE             29
   #define ERROR_IN_XSAVING_IMG_SAMPLE   30
   #define XSAVING_NAME                  31
   #define XSAVED_NAME                   32
   #define ERROR_IN_XSAVING_NAME         33
   #define BEGIN_TO_XLOAD                34
   #define XLOAD_SIGNATURE               35
   #define ERROR_IN_XLOAD_SIGN           36
   #define XLOAD_DIMS                    37
   #define ERROR_IN_XLOAD_DIMS           38
   #define XLOAD_NAME                    39
   #define XLOAD_IMG_SAMPLE              40
   #define XLOAD_IMG_SAMPLE_COMPLETE     41
   #define XLOAD_IMG_SAMPLE_ERROR        42
   #define XLOAD_INHIBIT_VIDEOENG        43
   #define XCOPY_MEM_AREA                44
   #define DEALLOC_MEM_AREA              45
   #define XLOAD_FACESQ_UPDATE           46
   #define XLOAD_GENERAL_FAILURE         47
   #define XLOAD_DEINHIBIT_VIDEOENG      48
   #define XLOAD_FAILURE_IN_MS_UPDATE    49
   #define BEGIN_TO_GEN_AUDIOMSGSAMPLE   50
   #define GENERATING_AUDIOMSGSAMPLE     51
   #define GENERATED_AUDIOMSGSAMPLE      52
   #define BEGIN_TO_UNLOAD               60
   #define UNLOADING_SAMPLE              61
   

   #define DONE0                         250
   #define DONE1                         251
   #define DONE2                         252
   #define DONE3                         253
   
// ----------------------------------------------------------------------------
// CODICI PER I SEMAFORI LOCKER
// ----------------------------------------------------------------------------
   
   #define SEMA_LOCK      1
   #define SEMA_UNLOCK    2        

/*
   ----------------------------------------------------------------------------
   CODICI MESSAGGIO PER L'ENGINE AUDIO
   ----------------------------------------------------------------------------
*/ 

   #define NR_SERVICES_MESSAGES       0x0080

   #define AUDIOMSG_WELLCOME_TO_BA               0x0000
   #define AUDIOMSG_VISILAB_RELEASED_UPD         0x0001
   #define AUDIOMSG_I_LL_DOWNLOAD_IT_NOW         0x0002
   #define AUDIOMSG_DOWNLOAD_SUCCESS             0x0003
   #define AUDIOMSG_DOWNLOAD_FAILED              0x0004
   #define AUDIOMSG_WRONG_NETWORK_CONFIG         0x0005
   #define AUDIOMSG_I_LL_INSTALL_NOW             0x0006
   #define AUDIOMSG_INSTALL_COMPLETE             0x0007
   #define AUDIOMSG_THE_SYSTEM_WILL_BE_REBOOTED  0x0008
   #define AUDIOMSG_WAIT                         0x0009
   #define AUDIOMSG_NOLIGHT                      0x000A
   #define AUDIOMSG_YOU_ARE_IN_THIS_ROOM         0x000B
   #define AUDIOMSG_I_DONT_KNOW_WHERE_YOU_ARE    0x000C
   #define AUDIOMSG_TRASMISSION_COMPLETE         0x000D
   #define AUDIOMSG_BLINDSERVER_IS_NOT_CONNECTED 0x000E
   #define AUDIOMSG_IMPOSSIBLE_TO_DOWNLOAD_DICT  0x0010
   #define AUDIOMSG_REPRODUCE_A_SINGLE_WORD      0x0011
   #define AUDIOMSG_LOADING                      0x0012
   #define AUDIOMSG_GO                           0x0013
   #define AUDIOMSG_WAIT_I_AM_FIXING_A_PROBLEM   0x0014
   #define AUDIOMSG_PRESS_LTRIGGER_OR_RTRIGGER   0x0015
   
   #define AUDIOMSG_PRESS_SQUARE_AND_SAY_THE_CMD          0x0020
   #define AUDIOMSG_AVAILABLE_COMMANDS_ARE                0x0021
   #define AUDIOMSG_I_AM_LISTENING_FOR_YOUR_NEXT_COMMAND  0x0022
   #define AUDIOMSG_COMMAND_NOT_RECOGNIZED                0x0023
   #define AUDIOMSG_VOICEREC_SYSTEM_NOT_SUPPORTED         0x0024
   
   #define AUDIOMSG_ENABLE_NO_FUNCTION_MODE               0x0030
   #define AUDIOMSG_ENABLE_FACE_RECOGNITION_SYSTEM        0x0031
   // #define AUDIOMSG_ENABLE_ROOM_RECOGNITION_SYSTEM         0x0032           // Inhibit
   #define AUDIOMSG_ENABLE_OPTICAL_CHAR_RECOGNITION       0x0033
   #define AUDIOMSG_ENABLE_COLOR_SCANNER                  0x0034
   #define AUDIOMSG_ENABLE_MAIL_READER                    0x0035
   #define AUDIOMSG_ENABLE_TICKETS_SCANNER                0x0036
   #define AUDIOMSG_ENABLE_DATA_MATRIX_SCANNER            0x0037
   #define AUDIOMSG_ENABLE_POS_RECOGNITION_SYSTEM         0x0038
   
   #define AUDIOMSG_NO_FUNCTION_MODE_ENABLED              0x0040
   #define AUDIOMSG_FACE_RECOGNITION_SYSTEM_ENABLED       0x0041
   // #define AUDIOMSG_ROOM_RECOGNITION_SYSTEM_ENABLED        0x0042           // Inhibit
   #define AUDIOMSG_OPTICAL_CHAR_RECOGNITION_ENABLED      0x0043
   #define AUDIOMSG_COLOR_SCANNER_ENABLED                 0x0044
   #define AUDIOMSG_MAIL_READER_ENABLED                   0x0045
   #define AUDIOMSG_TICKET_SCANNER_ENABLED                0x0046
   #define AUDIOMSG_DATA_MATRIX_SCANNER_ENABLED           0x0047
   #define AUDIOMSG_POS_RECOGNITION_SYSTEM_ENABLED        0x0048
   
   #define AUDIOMSG_I_CANNOT_CONTACT_POP3_SERVER          0x0050
   #define AUDIOMSG_THERE_IS_NO_NEW_MAIL_FOR_YOU          0x0051
   #define AUDIOMSG_DO_YOU_WANT_TO_LISTEN_NEW_MESSAGES    0x0052
   #define AUDIOMSG_THE_NEXT_MAIL_IS_SENT_BY              0x0053
   #define AUDIOMSG_DO_YOU_WANT_TO_LISTEN_THIS_MESSAGE    0x0054
   #define AUDIOMSG_DO_YOU_WANT_TO_LISTEN_AGAIN           0x0055
   #define AUDIOMSG_DO_YOU_WANT_TO_DELETE_THIS_MESSAGE    0x0056
   #define AUDIOMSG_THIS_MESSAGE_WILL_BE_DELETED          0x0057
   #define AUDIOMSG_I_CANNOT_DETERMINE_SENDER             0x0058
   #define AUDIOMSG_I_CANNOT_DETERMINE_SUBJECT            0x0059
   #define AUDIOMSG_INTERNAL_ERROR                        0x005A
   #define AUDIOMSG_I_CANNOT_DECODE_MAIL_CONTENT          0x005B
   #define AUDIOMSG_I_DISCOVERED_YOU_HAVE_UNREADED_MAIL   0x005C
   
   #define AUDIOMSG_NDTESSERACT_INIT_ERROR                0x0060
   #define AUDIOMSG_NDTESSERACT_INTERNAL_ERROR            0x0061
   
   #define AUDIOMSG_PROCEED_00_PERCENT                    0x0070
   #define AUDIOMSG_PROCEED_10_PERCENT                    0x0071
   #define AUDIOMSG_PROCEED_20_PERCENT                    0x0072
   #define AUDIOMSG_PROCEED_30_PERCENT                    0x0073
   #define AUDIOMSG_PROCEED_40_PERCENT                    0x0074
   #define AUDIOMSG_PROCEED_50_PERCENT                    0x0075
   #define AUDIOMSG_PROCEED_60_PERCENT                    0x0076
   #define AUDIOMSG_PROCEED_70_PERCENT                    0x0077
   #define AUDIOMSG_PROCEED_80_PERCENT                    0x0078
   #define AUDIOMSG_PROCEED_90_PERCENT                    0x0079
   #define AUDIOMSG_PROCEED_100_PERCENT                   0x007A
   #define AUDIOMSG_PROCEED                               0x007F
   
   #define AUDIOMSG_SIGNAL_A_PERSON              0x1000


// ----------------------------------------------------------------------------
//  STRUCT PER LA COMUNICAZIONE CON IL BLINDSERVER
// ----------------------------------------------------------------------------

    struct BldServerCommPacket_Type
    {
           unsigned short Message;
           unsigned short SubMessage;
           int            Data0, Data1;
           
           union          
           {
               char          Password [256];
               unsigned char Linear [256];
               unsigned char Pixel [8][8];          
           }              Data;
    };
    
    struct BldServerCommPacketShort_Type
    {
           unsigned short Message;
           unsigned short SubMessage;
           int            Data0, Data1;
    };
    
    BAEXTERN struct sockaddr_in bldsvr_sin;   
    BAEXTERN SOCKET bldsvr_socket;  
    
    BAEXTERN struct BldServerCommPacket_Type      *BldServerCommPacket;
    BAEXTERN struct BldServerCommPacketShort_Type *BldServerCommPacketShort;


// ----------------------------------------------------------------------------
//  STRUCT PER GESTIONE PLACE
// ----------------------------------------------------------------------------

struct cvTmpPlacePacket_Type
{
       IplImage *Img [NR_SAMPLES_FOR_PLACE];
};

// ----------------------------------------------------------------------------
//  CODICI DI COMUNICAZIONE PER IL BLIND SERVER
// ----------------------------------------------------------------------------

#define BLDSVR_HELLO                                    0x0001
#define BLDSVR_AUTHENT_OK                               0x0002
#define BLDSVR_AUTHENT_WRONG                            0x0003

#define BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______START    0x0004
#define BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______PROCEED  0x0005
#define BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______RESPONSE 0x0006
#define BLDSVR_CLIENT_IS_SENDING_AN_IMAGE______CLOSE    0x0007

#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE                      0x0010
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______READY           0x0011
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______STARTIMG        0x0012
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______STARTIMGRESP    0x0013
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______SENDBLOCK       0x0014
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______SENDRESP        0x0015
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______CLOSEIMG        0x0016
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______NAME            0x0017
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______NAMERESP        0x0018
#define BLDSVR_CLIENT_IS_SENDING_A_SLOTPACKAGE______CLOSE           0x0019

#define BLDSVR_CLIENT_IS_ASKING_IF_THE_SLOT_IS_BUSY           0x0020
#define BLDSVR_CLIENT_IS_ASKING_IF_THE_SLOT_IS_BUSY_RESPONSE  0x0021

#define BLDSVR_CLIENT_IS_RETRIEVING_AN_IMAGE______START       0x0030
#define BLDSVR_CLIENT_IS_RETRIEVING_AN_IMAGE______RESPONSE    0x0031
#define BLDSVR_CLIENT_IS_RETRIEVING_AN_IMAGE______REQBLOCK    0x0032
#define BLDSVR_CLIENT_IS_RETRIEVING_AN_IMAGE______REQRESP     0x0033
#define BLDSVR_CLIENT_IS_RETRIEVING_AN_IMAGE______CLOSE       0x0034

#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE                      0x0040
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______SLOTINFO        0x0041
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______STARTIMG        0x0042
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______STARTIMGRESP    0x0043
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______REQBLOCK        0x0044
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______REQRESP         0x0045
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______CLOSEIMG        0x0046
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______NAME            0x0047
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______NAMERESP        0x0048
#define BLDSVR_CLIENT_IS_RETRIEVING_A_SLOTPACKAGE______CLOSE           0x0049

#define BLDSVR_CLIENT_IS_ORDERING_THE_ERASE______START        0x0050
#define BLDSVR_CLIENT_IS_ORDERING_THE_ERASE______RESPONSE     0x0051 

#define BLDSVR_CLIENT_IS_ASKING_WHERE_IT_IS_NOW               0x0060
#define BLDSVR_CLIENT_IS_ASKING_WHERE_IT_IS_NOW______RESPONSE 0x0061

#define BLDSVR_CLIENT_IS_ASKING_AN_OCR_ANALISYS                  0x0070
#define BLDSVR_CLIENT_IS_SENDING_AN_IMAGE_FOR_OCR______TAGRESULT 0x0074
#define BLDSVR_CLIENT_IS_SENDING_AN_IMAGE_FOR_OCR______REQRESULT 0x0075
#define BLDSVR_CLIENT_IS_SENDING_AN_IMAGE_FOR_OCR______RETRVRESULT 0x0076
#define BLDSVR_CLIENT_IS_SENDING_AN_IMAGE_FOR_OCR______CLOSERESULT 0x0077

#define BLDSVR_IT_WAS_ONLY_A_TEST                       0x0090
#define BLDSVR_CLOSE                                    0x0100

#define BLDSVR_VERSION_CODE  0x00010002


// ----------------------------------------------------------------------------
//  ERRORI DI COMUNICAZIONE BACP
// ----------------------------------------------------------------------------

#define ERR_IMPOSSIBLE_TO_SEND_FIRST_COMMAND       -1
#define ERR_IMPOSSIBLE_TO_RECEIVE_CONFIRM          -2
#define ERR_IMPOSSIBLE_TO_START_TRANSFER           -3
#define ERR_IMPOSSIBLE_TO_ALLOC_LUMPBUFFER         -4
#define ERR_IMPOSSIBLE_TO_COMPRESS_IMAGE           -5
#define ERR_IMPOSSIBLE_TO_TRANSFER_A_PACKET        -6
#define ERR_IMPOSSIBLE_TO_RECEIVE_A_PACKET         -7
#define ERR_RESPONSE_CODE_MISMATCH                 -8
#define ERR_IMPOSSIBLE_TO_TRANSFER_THE_NAME        -9
#define ERR_REMOTE_SERVER_ERROR                    -10
#define ERR_REMOTE_OCR_TROUBLE                     -11
#define ERR_SLOT_IS_VOID                           -12

#define ERR_IMPOSSIBLE_TO_DETERMINE_BLDSVR_IP      -20 
#define ERR_IMPOSSIBLE_TO_CREATE_BLDSVR_SOCKET     -21
#define ERR_IMPOSSIBLE_TO_CONNECT_TO_BLDSVR        -22 
#define ERR_IMPOSSIBLE_TO_SEND_PASSWORD_PACKET     -23
#define ERR_IMPOSSIBLE_TO_RECV_PASSWORD_RESPONSE   -24
#define ERR_AUTHENTIFICATION_FAILED                -25
#define ERR_SERVER_VERSION_MISMATCH                -26
#define ERR_IMPOSSIBLE_TO_ALLOC_COMM_PACKETS       -27
#define ERR_HANDON_IS_ALREADY_ACTIVE               -28


// ----------------------------------------------------------------------------
//  TEXTBUFFER SIZE
// ----------------------------------------------------------------------------

#define TEXTBUFFERSIZE 65536
#define MAXWORDS         250
#define MAXLENOFAWORD     30

BAEXTERN unsigned char *PntToTextBuffer;

// Struttura

struct baListOfWords_Type
{
       int  NrWords;
       char Word [MAXWORDS][MAXLENOFAWORD];
};

BAEXTERN unsigned char *DictionaryArea;
BAEXTERN unsigned char *DictionaryLenDB;
BAEXTERN float         *DictionaryDistDB;
BAEXTERN char           DictIsOperative;
BAEXTERN int            DictNrWords;
BAEXTERN int            DictMaxLen;


// ----------------------------------------------------------------------------
//  CODICI E VARIABILI PER LO SCANNER COLORI
// ----------------------------------------------------------------------------

#define STD_NR_COLORS_AVAILABLE 9

struct baColorInfo_Type
{
       float ChannelR, ChannelG, ChannelB;
       float Distance;
       char  NameColor [10];
};

#ifdef BA_NEED_VARIABLES

    struct baColorInfo_Type baColorInfo [STD_NR_COLORS_AVAILABLE] =
    { {0.000, 0.000, 0.000, 0.0, "BLACK    "},  
      {2.100, 0.530, 0.570, 0.0, "RED      "},
      {0.519, 1.348, 0.592, 0.0, "GREEN    "},
      {0.229, 1.215, 1.680, 0.0, "BLUE     "},  
      {1.111, 0.920, 1.096, 0.0, "PINK     "},
      {1.140, 0.970, 0.682, 0.0, "ORANGE   "},
      {0.940, 1.082, 0.520, 0.0, "YELLOW   "},
      {0.984, 1.003, 0.968, 0.0, "GRAY     "},
      {0.989, 1.200, 0.913, 0.0, "WHITE    "}   };
 
#else 
 
    extern struct baColorInfo_Type baColorInfo [STD_NR_COLORS_AVAILABLE];

#endif

// ----------------------------------------------------------------------------
//  CODICI E VARIABILI PER IL GESTORE MAIL
// ----------------------------------------------------------------------------

#define STD_NR_MAIL_AVAILABLE 10
#define STD_MAIL_MAX_SIZE     250000

struct baMailClnRegister_Type
{
    void *Message;
    void *MsgHeader;
    int   Pop3NrBytes;
    int   Pop3ID;
    char  IsAvailable;
    char  YouMustDeleteIt;   
};

BAEXTERN struct baMailClnRegister_Type baMailClnRegister [STD_NR_MAIL_AVAILABLE];

// ----------------------------------------------------------------------------
//  CODICI E VARIABILI PER IL SISTEMA MAIL CHECKER
// ----------------------------------------------------------------------------

BAEXTERN int   baMailChecker_HyphActualMailMessages;
BAEXTERN char  baMailChecker_NrTimesAlreadySignaled;


// ----------------------------------------------------------------------------
//  CODICI E VARIABILI PER IL SISTEMA DI GESTIONE DELLE MICROICONE
// ----------------------------------------------------------------------------

BAEXTERN int   baMicroIcons_Register;

#define NR_MICROICONS_SUPPORTED      1

#define MICROICON__UNREADED_MAILS     1



// ----------------------------------------------------------------------------
// PROTOTIPI
// ----------------------------------------------------------------------------

             int baInit (void);
             int baDeInit (void);

// Funzioni stringa

             void genstr_NameOfNameFile    (int NrSlot, char *StrDest);
             void genstr_NameOfVoiceSample (int NrSlot, int NrSample, char *StrDest);
             void genstr_NameOfImageSample (int NrSlot, int NrSample, char *StrDest);
             
// Funzioni ausiliarie

             char *ba_fgets (char *buf, int n, FILE *stream);
             int   baSearch (char *buf, char *strtosearch, int start);
             


             
// Icone

             void InitIcons (void);

// Wizards

             int wzFaceTraining (int Slot);
             int wzPlaceTraining (int Slot);
             int wzTestForFaceRecognition (void);
             int wzViewSample (int Slot);
             
             int wzXSaveData (void);
             int wzXLoadData (void);
             
// Gestione videocamera

             void baChangeCameraResolution (unsigned short int LenX, unsigned short int LenY, char ColorDepth);
             
// Funzioni di uscita coercitiva dai wizard attivati

             void HALT_wzTestForFaceRecognition (void);
             void HALT_wzViewSample (void);
             

// Funzioni zOpenCV

             int zcvNormalizeImage8Bit (IplImage *Image, float *AdrAverage);
             int zcvNormalizeImage8BitUsingZone (IplImage *Image, float *AdrAverage);
             
             int zcvShowImage8bit (IplImage  *Image, unsigned short int PosX1, unsigned short int PosY1, 
                                                     unsigned short int PosX2, unsigned short int PosY2,
                                                     char WndHandle, char RenderNow);

             int zcvShowImage24bit (IplImage *Image, unsigned short int PosX1, unsigned short int PosY1, 
                                                     unsigned short int PosX2, unsigned short int PosY2,
                                                     char WndHandle, char RenderNow);
             
             void zcvCalculateAverageRGBChannels (IplImage *Image, int *AverageRed, int *AverageGreen, int *AverageBlue);
             void zcvCalculateAverageRGBBrightness (IplImage *Image, float *Average);
             
             void zcvCopyRegion (IplImage *ImageSrc, IplImage *ImageDest, unsigned short int PosX1, unsigned short int PosY1, 
                                                                  unsigned short int PosX2, unsigned short int PosY2);
            
             void zcvCopy (IplImage *ImageSrc, IplImage *ImageDest);

             int  zcvCalculateAvgBrightness (IplImage *Image, float *AdrAverage);

             void zcvEigenDecomposite( IplImage* obj, int nEigObjs, void* eigInput, int ioFlags, void* userData, 
                                       IplImage* avg, float*    coeffs );

             void zcvEqualizeHist( const CvArr* src, CvArr* dst, CvHistogram* hist, CvMat* lut, int hist_sz);                          

// API per il face recognizer (alto livello)    
             
             void   baUpdateFaceRecognizerDBase (int NrSlotUpdated);
             int    baUpdateDistanceData (IplImage *Image);

// API per il riconoscimento facciale (basso livello - PCA)

             void   baPCA_UpdateFaceRecognizerDBase (int NrSlotUpdated);
             int    baPCA_UpdateDistanceData (IplImage *Image);
             
// Funzioni FLite

             cst_voice *register_cmu_us_kal();

// API di controllo del main engine

             void baCreateVideoEngineThread (void);
             void baReStartVideoEngine (void);
             void baExitVideoEngine (void);
             void baInhibitVideoEngine (void);
             void baDeInhibitVideoEngine (void);
             void baDeInhibitVideoEngine_NoForce (void);
             
             int baVideoEngine (SceSize args, void *argp);
        
// API di controllo di priorità del thread che contiene l'interfaccia principale

             void baGetMainThreadInfos (void);
             void baLockMainThread (void);
             void baUnlockMainThread (void);
             
// finestra About

             void baOpenAboutWindow (void);
             
// segnalazioni di errore

             int baitf_ErrorCannotFindHaarCascadeTrainingSet ();
             int baitf_ErrorInLoadingHaarCascadeFile ();
             int baitf_YouAreUsingOldVersionOfCascadeFile ();
             int baitf_ErrorInCameraConnect ();
             int baitf_ErrorFirmwareTooOld ();
             int baitf_ErrorInDownloadingHmmPackage ();
             int baitf_ErrorInSphinxInitialization ();

// salvataggio dei dati audio

             int baSave_GenerateANewAudioSample (int NrSlot, int NrSample, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
             int baSave_GenerateAllAudioSamplesForTheSlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
  
// salvataggio delle immagini relative allo slot

             int baSave_SaveSampleImagesOfSlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket); 
             
// salvataggio del nome della persona visualizzata

             int baSave_SaveNameOfThePersonThatIsASlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);

// salvataggio dei dati relativi allo slot

             int baSave_GenerateAndSaveDataRelativeToASlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
                                        
// salvataggio completo dei dati              
             
             int baSaveAllSampleImagesOfTheSystem (void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
             
// caricamento dei dati relativi allo slot             
                               
             int baLoad_LoadSampleImagesOfSlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);

// caricamento completo dei dati

             int baLoadAllSampleImagesOfTheSystem (void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
               

// cancellazione dei dati relativi ad uno slot

             void baErase_DeleteDataInMemory (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
             void baErase_DeleteAudioSamplesInTheDisk (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
             void baErase_DeleteSampleImagesInTheDisk (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
             void baErase_DeleteNameFileInTheDisk (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
             int  baErase_DeleteDataRelativeToASlot (int NrSlot, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);

// Inizializzazione del gestore messaggi

             void InitAudioMessages ();       

// Audio server (server mode)

             int  baReStartAudioEngine ();
             void baExitAudioEngine ();

// Audio server (client mode)

             void baaud_WellcomeToBlindAssistant (void);   
             void baaud_SignalThePresenceOfAPerson (unsigned short int PosX, unsigned short int PosY, int NrElement);
             void baaud_VisilabHasReleasedAnUpdate (void);
             void baaud_IWillDownloadItNow (void);
             void baaud_DownloadSuccess (void);
             void baaud_DownloadFailed (void);
             void baaud_WrongNetworkConfig (void);
             void baaud_IWillInstallItNow (void);
             void baaud_InstallationComplete (void);
             void baaud_TheSystemWillBeRebootedNow (void);
             void baaud_Wait (void);
             void baaud_Go (void);
             void baaud_WaitIAmFixingAProblem (void);
             void baaud_PressLTriggerOrRTrigger (void);
             
             void baaud_Loading (void);
             void baaud_NoLight (void);
             void baaud_YouAreInThisRoom (char *NameOfTheRoom);
             void baaud_IDontKnowWhereYouAre (void);
             void baaud_TrasmissionComplete (void);
             void baaud_BlindServerIsNotConnected (void);
             void baaud_ImpossibleToDownloadDict (void);
             char baaud_ReproduceASingleWord (char *FirstName, char *SecondaryName);
             char baaud_ReproduceASingleWordRT (char *Word);
             void baaud_ColorScannerResult (int ChannelR, int ChannelG, int ChannelB, int ColorCode);
            
             void baaud_PressSquareAndSayTheCmd (void);
             void baaud_AvailableCommandsAre (void);
             void baaud_ListeningForYourNextCommand (void);
             void baaud_CommandNotRecognized (void);
             void baaud_VoiceRecognitionSystemNotSupported (void);
             
             void baaud_EnableNoFunctionMode (void);
             void baaud_EnableFaceRecognitionSystem (void);
             void baaud_EnablePosRecognitionSystem (void);
             void baaud_EnableOpticalCharRecognition (void);
             void baaud_EnableColorScanner (void);
             void baaud_EnableMailReader (void);
             void baaud_EnableTicketsScanner (void);
             void baaud_EnableDataMatrixScanner (void);
             void baaud_NoFunctionModeEnabled (void);
             void baaud_FaceRecognitionSystemEnabled (void);
             void baaud_PosRecognitionSystemEnabled (void);
             void baaud_OpticalCharRecognitionEnabled (void);
             void baaud_ColorScannerEnabled (void);
             void baaud_MailReaderEnabled (void);
             void baaud_TicketScannerEnabled (void);
             void baaud_DataMatrixScannerEnabled (void);
             
             void baaud_ImpossibleToConnectToThePop3Server (void);
             void baaud_ThereIsNoNewMailForYou (void);
             void baaud_ThereAreNNewMessagesOnMailSvr (int NrNewMessages);
             void baaud_DoYouWantToListenNewMessages (void);
             void baaud_TheNextMailIsSentBy (void);
             void baaud_DoYouWantToListenThisMessage (void);
             void baaud_DoYouWantToListenThisMessageAgain (void);
             void baaud_DoYouWantToDeleteThisMessage (void);
             void baaud_ThisMessageWillBeDeleted (void);
             void baaud_ICannotDetermineSender (void);
             void baaud_ICannotDetermineSubject (void);
             void baaud_InternalError (void);
             void baaud_ICannotDecodeMailContent (void);
             void baaud_YouHaveUnreadedMails (void);
             
             void baaud_ndTessInitError (void);
             void baaud_ndTessInternalError (void);
             
             void baaud_ndProceedMessage (void);
             void baaud_ndProceedXXPerCent (int Progress);
             
             void baaud_ReproduceAMessageWordByWord (char *Message);
            
// Sistema di riconoscimento vocale

             void baVoiceRec_Help ();
             void baVoiceRec_AcquireCommand (char *StrCommand, int StrCommandLen);
             void baVoiceRec_DecodeCommand (char *StrCommand);
             char baVoiceRec_AnswerYesNo (void *PntToAudioMessage);
             
             char ndPSphinx_ThePackageIsOk();
             
             
// Server di filtraggio segnali presenza

             void baResetPresenceRegisterForFrame ();
             void baSignThePresenceOfAPerson (unsigned short int PosX, unsigned short int PosY, int NrElement);
             void baDispatchMessageForPeoplePresence ();

// Generazione del mini file-system dedicato dell'applicazione

             int baGenFs_GenerateNewDirsOfFS (void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);

// Inizializzazione del sistema al boot

             void InitFileSystem ();

// Gestore del sistema delle microicone

             void baMicroIcons_Enable  (int IconCode, char RenderNow);
             void baMicroIcons_Disable (int IconCode, char RenderNow);

// Salvataggio dei dati dello slot in un container

             int baXSave_SaveTheDataInTheSlot (int NrSlot, char *NameFile, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);
             int baXLoad_LoadTheDataFromTheSlot (int NrSlot, char *NameFile, void *GraphCallback, struct OpInfoPacket_Type *InfoPacket);

// Interfaccia per modifica opzioni

             void wzOptions (void);
             void wzAdvancedOptions (void);
             void wzEngineOptions (void);
             void wzBlindServerOptions (void);
             void wzOcrOptions (void);
             void wzMailOptions (void);

// Gestore opzioni

             void InitOptions ();
             void UpdateOptionsOnTheDisk ();
             
             void baOpt_EnableDisableConnectToNetAtStartup (char IsEnabled);
             void baOpt_EnableDisableConnectToBldSvrAtStartup (char IsEnabled);
             
             char baOpt_ChangeWallPaper (char *Name);
             void baOpt_EnableDisableWallPaper (void);
             void baOpt_LoadTheNewWallPaper (void);
             
             void baOpt_ChangeDownloadSite (char *NewSite);
             void baOpt_ChangeMinBetweenChecks (int Minutes);
             void baOpt_EnableDisableChecksForUpdate (char Enable);
             void baOpt_EnableDisableAutoDownload (char Enable);
             void baOpt_EnableDisableAutoInstall (char Enable);
             void baOpt_EnableDisableHttpTunneling (char IsEnabled);
             void baOpt_EnableDisableBeVerbose (char IsEnabled);
             void baOpt_ChooseConnectionProfile (char Profile);
             
             void baOpt_EnableDisableChecksForNewMails (char Enable);
             void baOpt_ChangeMinBetweenChecksForMails (int Minutes);
             
             void baOpt_ChangeMinBrightnessLevel (float MinLevel);
             void baOpt_ChangeSkipThreshold (float Level);
             void baOpt_ChangeFaceDetectorProbingArea (float Level);
             void baOpt_ChangeFaceDetectorThreshold (float Level);
             
             void baOpt_ChangeBlindServerIPAddr (char *IPAddr);
             void baOpt_ChangeBlindServerPort (int Port);
             void baOpt_ChangeBlindServerPasswd (char *Passwd);
             void baOpt_ChangeOcrLanguage (int LanguageCode);
             void baOpt_ChangeOcrMethod (int MethodCode);
             void baOpt_ChangeSkipUnknownWords (int NewValue);
             void baOpt_CheckForOldTS_FaceDetect (int ActualValue);
             
             void baOpt_ChangePop3Addr (char *NewAddr);
             void baOpt_ChangePop3UserName (char *NewUserName);
             void baOpt_ChangePop3Password (char *NewPassword);
             void baOpt_ChangePop3Port (int NewPort);

             
// Connessione di rete principale

             void baEnableMainNetworkConnection (void);
             void baDisableMainNetworkConnection (void);
             void baEnableMainConnToBlindServer (void);
             void baDisableMainConnToBlindServer (void);

// Routines di rete per l'accesso al BlindServer

             void baBldSvr_AllocateCommPackets (void);
             void baBldSvr_DeAllocateCommPackets (void);
             
             int  baBldSvr_HandOn  (void);
             void baBldSvr_HandOff (void);
             
             int  baBldSvr_GenerateBldSvrSin (struct sockaddr_in *mybldsvr_sin);
             
             int  baBldSvr_TryAConnectToBlindServer (void);
             char baBldSvr_AskToSvrIfItHasAnImageStored (char NrSlot);
             char baBldSvr_AskToBlindServerWhereIAm (IplImage *MyImage, char *NamePlace);
             char baBldSvr_SendToSvrASlotPackage (char NrSlot, void *GraphCallback, struct cvTmpPlacePacket_Type *PntToTmpPlacePacket, char *Name);
             char baBldSvr_RetrieveFromSvrAnImage (int NrSlot, int NrSample, IplImage *MyImage, void *GraphCallback, int MessageData);
             char baBldSvr_RetrieveFromSvrASlotPackage (char NrSlot, void *GraphCallback, struct cvTmpPlacePacket_Type *PntToTmpPlacePacket, char *Name);
             char baBldSvr_SendToSvrEraseCommand (char NrSlot);
             char baBldSvr_SendToSvrAnImage_ForOCR (IplImage *MyImage, void *TextBuffer, int LanguageData);
             void baBldSvr_CloseConnectionWithBlindServer (void);
             
             void OLD_BldSvr_V1_0_CloseConnectionWithBlindServer (void);

// Connessione disconnessione manuale al BlindServer
             void bldsvrcnx_ServerManualConnect (void);  
             void bldsvrcnx_ServerManualDisconnect (void); 

// Update manager

             void baStartUpdateEngine ();
             void baStopUpdateEngine ();
             void baInhibitUpdateEngine ();
             void baDeInhibitUpdateEngine ();
             
             void baUpd_ManualChecksForUpdates (char DoDownload, char DoInstall);
             void baUpd_ManuallyInstallANewUpdate (void);
             

             
// Place engine
            
             void baStartPlaceEngine ();
             void baStopPlaceEngine ();
             void baInhibitPlaceEngine ();
             void baDeInhibitPlaceEngine ();
             
             void baPlcEng_AskWhereIAmToServer (IplImage *img);
             
// Routines per la generazione dei nomi dei componenti dell'OCR

             void baOcrFNames_MakeDictPackg_VisilabSvr (char *Name);
             void baOcrFNames_MakeDictPackg_Target (char *Name);
             
             void baOcrFNames_MakeVDictName_VisilabSvr(char *Name, int LanguageCode);
             void baOcrFNames_MakeVDictName_Target (char *Name, int LanguageCode);
             
             void baOcrFNames_MakeVDictNameSubPckg_Src (char *Name, char Letter, int LanguageCode);
             void baOcrFNames_GenerateVDictDir (char *Name, int LanguageCode);

             void baOcrFNames_GenerateNameOfDictionary (char *Name, int LanguageCode);

// API generica per gestione OCR

             int  baOcrAPI_IsNeededNetworkConnection (int OcrMethod);
             int  baOcrAPI_IsNeededBlindServerConnection (int OcrMethod); 
             void baOcrAPI_DoMainOCRJob (IplImage *OcrImage, int OcrMethod, int OcrLanguage);

             void baOcrAPI_FillListOfWords (char *PntToTextBuffer, struct baListOfWords_Type *PntListOfWords);
             void baOcrAPI_EnableDictionary (int LanguageCode);
             void baOcrAPI_DisableDictionary (void);
             char baOcrAPI_CorrectASingleWord (char *OriginalWord, char *DestWord, float DiscrThreshold);
             void baOcrAPI_DigitalizeWords (struct baListOfWords_Type *PntToListOfWords, char GenerateNotFoundWords, int LanguageCode);
             
             void baOcrAPI_AddNewWordToDwg (char *Word, int LanguageCode);

// API per il download dei dizionari da Internet

             void baDwDict_DownloadDictionaryZip (void);
            

// Scanner colori

             int baColorScanner_DeterminateTheMinimumDistanceColor (float ChannelR, float ChannelG, float ChannelB);

// Funzioni di gestione client email

             char baMail_ConnectToNetForUpdate ();
             void baMail_DisconnectToNetForUpdate ();
             char baMail_TryToConnectToThePop3Server ();
             void baMail_StartASessionOfTheClient ();

// Engine per il controllo mail

             void baStartMailCheckerEngine ();
             void baStopMailCheckerEngine ();
             void baInhibitMailCheckerEngine ();
             void baDeInhibitMailCheckerEngine ();
             
// Funzioni di auto-fix del programma

             char SYSFIXER00_SyncPlaceAudioCache (char *NameRoom);
             char SYSFIXER01_DownloadHmmVocalPackage ();
            
// Costruttori funzioni del programma

            void videng_NULLMODE_Constructor ();
            void videng_FACEREC_Constructor ();
            void videng_POSREC_Constructor ();
            void videng_OCR_Constructor ();
            void videng_COLORSCANNER_Constructor ();
            void videng_MAILREADER_Constructor ();
            void videng_DATAMATRIX_Constructor ();

// Nuclei per i motori delle funzioni del programma

            void videng_NULLMODE_Core ();
            void videng_FACEREC_Core ();
            void videng_POSREC_Core (int TrianglePressed);
            void videng_OCR_Core (int TrianglePressed);
            void videng_COLORSCANNER_Core (int TrianglePressed);
            void videng_MAILREADER_Core (int TrianglePressed);
            void videng_DATAMATRIX_Core (int TrianglePressed);
            
            void videng_SayDayTime (void);

// Distruttori per le funzioni del programma

            void videng_NULLMODE_Destructor ();
            void videng_FACEREC_Destructor ();
            void videng_POSREC_Destructor ();
            void videng_OCR_Destructor ();
            void videng_COLORSCANNER_Destructor ();
            void videng_MAILREADER_Destructor ();
            void videng_DATAMATRIX_Destructor ();
            
// Interfaccia principale del programma

             void baMainInterface (void);
             void mainitf_StatusBar (char *Message, char RenderNow);
             void mainitf_FunctionBar (char *Message, char RenderNow);

#endif   // define _BA_HEADERS_



#ifdef __cplusplus
}
#endif

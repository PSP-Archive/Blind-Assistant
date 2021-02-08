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
   ROUTINES METAVMIME C++
   ----------------------------------------------------------------------------
   Queste routine provvedono a richiamare le routine della libreria C++ VMIME.
   La libreria C++ possiede una API molto complessa, che non può essere usata
   direttamente da un modulo in linguaggio C. Questo modulo agisce come un
   bridge, consentendo di usare le funzioni richieste da un modulo C. 
   ----------------------------------------------------------------------------
*/

        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        #undef ND_NOT_MAIN_SOURCE

        // ---- Headers C
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        
        #include "BlindAssistant.h"

        // ---- Headers C++

        #include <iostream>
        #include <fstream>

        #include <vmime/vmime.hpp>
        #include <vmime/platforms/posix/posixHandler.hpp>

        
/*
    -----------------------------------------------------------------------------------
    DETERMINAZIONE DEL SENDER DELLA LIBRERIA
    -----------------------------------------------------------------------------------
*/

extern "C"
{

#ifndef PSP1000_VERSION

char baMail_VMIME_GetMailSender (char *C_Input, char *C_Output)
{
     // Esegue una operazione di inizializzazione, per segnalare a VMIME che vogliamo
     // usare un handler di tipo POSIX
     vmime::platform::setHandler <vmime::platforms::posix::posixHandler>();
     
     // Crea un oggetto stringa C++ che contiene la stringa C di input ed
     // un oggetto stringa C++ per l'output inizialmente vuota
     vmime::string                                 szInput     (C_Input);
     vmime::string                                 szOutput;
     
     // Crea un oggetto di tipo <vmime::message> inizialmente vuoto
     vmime::ref <vmime::message> msg = vmime::create <vmime::message>();
     
     // Adesso esegui il parsing della stringa szInput, portando il risultato
     // dentro l'oggetto msg     
     msg->parse (szInput);
     
     // Isola l'header dell'oggetto
     vmime::ref <vmime::header> hdr = msg->getHeader();
     
     // Programma il sistema perchè operi in modalità UTF8
     vmime::charset ch(vmime::charsets::UTF_8) ;
     
     // Ricava il nome del sender: va memorizzato nell'oggetto vmime::string di output
     szOutput = hdr->From()->getValue().dynamicCast<vmime::mailbox>()->getName().getConvertedText(ch);
     
     // Copia i caratteri, troncando ai primi 256 
     ndStrCpy (C_Output, (char *)(szOutput.c_str()), 255, 0);
     
     return 0;
}


char baMail_VMIME_GetMailSubject (char *C_Input, char *C_Output)
{
     // Esegue una operazione di inizializzazione, per segnalare a VMIME che vogliamo
     // usare un handler di tipo POSIX
     vmime::platform::setHandler <vmime::platforms::posix::posixHandler>();
     
     // Crea un oggetto stringa C++ che contiene la stringa C di input ed
     // un oggetto stringa C++ per l'output inizialmente vuota
     vmime::string                                 szInput     (C_Input);
     vmime::string                                 szOutput;
     
     // Crea un oggetto di tipo <vmime::message> inizialmente vuoto
     vmime::ref <vmime::message> msg = vmime::create <vmime::message>();
     
     // Adesso esegui il parsing della stringa szInput, portando il risultato
     // dentro l'oggetto msg     
     msg->parse (szInput);
     
     // Isola l'header dell'oggetto
     vmime::ref <vmime::header> hdr = msg->getHeader();
     
     // Programma il sistema perchè operi in modalità UTF8
     vmime::charset ch(vmime::charsets::UTF_8) ;
     
     // Ricava l'oggetto (subject) della mail: va memorizzato nell'oggetto vmime::string di output
     szOutput = hdr->Subject()->getValue().dynamicCast<vmime::text>()->getConvertedText(ch);
     
     // Copia i caratteri, troncando ai primi 256 
     ndStrCpy (C_Output, (char *)(szOutput.c_str()), 255, 0);
     
     return 0;
}

char baMail_VMIME_DecodeMessageContent (char *C_Input, char *C_Output, int *NrCharsOutput, int NrCharsMax)
{
     int ErrRep = -1;
     int i;
     
     // Esegue una operazione di inizializzazione, per segnalare a VMIME che vogliamo
     // usare un handler di tipo POSIX
     vmime::platform::setHandler <vmime::platforms::posix::posixHandler>();
     
     // Crea un oggetto stringa C++ che contiene la stringa C di input 
     vmime::string                                 szInput     (C_Input);
     
     // Crea un oggetto di tipo <vmime::message> inizialmente vuoto e dopo
     // esegui il parsing della stringa szInput, portando il risultato
     // dentro l'oggetto msg    
     vmime::ref <vmime::message> msg = vmime::create <vmime::message>();
     msg->parse (szInput);
     
     // Adesso crea, a partire dall'oggetto msg, un oggetto derivato di tipo
     // MessageParser
     vmime::messageParser mp(msg);
     
     // Analizza le varie sezioni del contenuto: ad un certo punto, deve essere
     // presente una sezione plain/text
     
     for ( int i = 0 ; i < mp.getTextPartCount() ; ++i )
     {
         // Isola la i-esima sezione del contenuto
         vmime::ref <const vmime:: textPart> tp = mp.getTextPartAt(i);
         
         if (tp->getType( ).getSubType() == vmime::mediaTypes::TEXT_HTML)
         {
              // Per le sezioni in HTML, il sistema sfrutta le capacità di decodifica della
              // libreria VMIME, per ottenere il contenuto in plain text
              
              // Inserisci il testo HTML in un oggetto htmlTextPart
              vmime::ref <const vmime::htmlTextPart> htp = tp.dynamicCast <const vmime::htmlTextPart>();
              
              // E poi sfrutta l'apposita funzione per ottenere il testo PLAIN. Questo viene inserito in
              // un oggetto contentHandler
              vmime::ref <const vmime::contentHandler> cnt = htp->getPlainText ();
              
              // Dobbiamo prepararci ad eseguire il metodo extract sull'oggetto cnt.
              // Per fare questo ci servirà un oggetto di tipo vmime::utility::outputStreamStringAdapter
              // Lo costruiamo a partire dalla stringa szOutput
              vmime::string                                 szOutput;
              vmime::utility::outputStreamStringAdapter     strszOutput (szOutput);
              
              // Eseguiamo l'operazione di extract sull'oggetto cnt: adesso il contenuto del
              // messaggio è nell'oggetto szOutput (oggetto stringa)
              cnt->extract (strszOutput);
              
              // Copia il messaggio decodificato nel buffer di output
              ndStrCpy (C_Output, (char *)(szOutput.c_str()), NrCharsMax, 0);
              
              // Settiamo ErrRep a 0. Segnalerà alla routine chiamante che abbiamo decodificato
              // il contenuto di una sezione HTML in plain/text. Si noti che l'analisi prosegue
              // perchè nella mail potrebbe esserci una sezione plain/text che non richiede
              // conversioni e questa ha la precedenza sulle sezioni HTML convertite
              ErrRep = 0;
              continue;
         }
         else
         {
              // Inserisci il testo in un content-handler
              vmime::ref <const vmime::contentHandler> cnt = tp->getText ();
              
              // Dobbiamo prepararci ad eseguire il metodo extract sull'oggetto cnt.
              // Per fare questo ci servirà un oggetto di tipo vmime::utility::outputStreamStringAdapter
              // Lo costruiamo a partire dalla stringa szOutput
              vmime::string                                 szOutput;
              vmime::utility::outputStreamStringAdapter     strszOutput (szOutput);
              
              // Eseguiamo l'operazione di extract sull'oggetto cnt: adesso il contenuto del
              // messaggio è nell'oggetto szOutput (oggetto stringa)
              cnt->extract (strszOutput);
              
              // Copia il messaggio decodificato nel buffer di output
              ndStrCpy (C_Output, (char *)(szOutput.c_str()), NrCharsMax, 0);
              
              // Settiamo ErrRep a 0. Segnalerà alla routine chiamante che abbiamo decodificato
              // il contenuto di una sezione plain/text
              ErrRep = 0;
              break;
         }
     } 
     
     // Se il codice di errore è 0, memorizza la lunghezza della stringa nella variabile
     // puntata dal puntatore *NrCharsOutput
     if (!ErrRep) *NrCharsOutput = strlen (C_Output);
     
     // Ritorna il codice di errore in uscita
     return ErrRep;
}

#endif


}

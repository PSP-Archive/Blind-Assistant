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
   FACE RECOGNITION
   ----------------------------------------------------------------------------
   Questa sezione contiene le routines necessarie per il riconoscimento 
   facciale usando analisi PentLand (PCA).
   ----------------------------------------------------------------------------
*/


        #define ND_NOT_MAIN_SOURCE  
                #include <nanodesktop.h>
        #undef ND_NOT_MAIN_SOURCE
        
        #include <stdio.h>
        #include <math.h>
        #include <string.h>
        #include <time.h>
        
        #include "BlindAssistant.h"
 

/*
   ---------------------------------------------------------------------------
   SIMBOLI
   ---------------------------------------------------------------------------
*/

#define MAX_PCA_SAMPLE_ANALYZABLE    NR_SLOTS_FACE_RECOGNIZER*NR_SAMPLES_FOR_FACE
#define MAX_EIGENS_USED              MAX_PCA_SAMPLE_ANALYZABLE-1
#define EXTD_MAX_EIGENS_USED         80



/*
   ---------------------------------------------------------------------------
   STRUTTURE
   ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

struct PCA_cvInfoImagesVect_Type
{
    char NrSlot;
    char NrSample;   
};

struct PCA_cvInfoImages_Type
{
    float SampleDistance [NR_SAMPLES_FOR_FACE];    
};

#endif


/*
   ---------------------------------------------------------------------------
   VARIABILI
   ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

    IplImage*  PCA_cvInputImagesVect       [MAX_PCA_SAMPLE_ANALYZABLE];
    
    struct     PCA_cvInfoImages_Type       PCA_cvInfoImages     [NR_SLOTS_FACE_RECOGNIZER];
    struct     PCA_cvInfoImagesVect_Type   PCA_cvInfoImagesVect [MAX_PCA_SAMPLE_ANALYZABLE];
    
    int        PCA_cvNrInputImages = 0;
   
    IplImage*  PCA_cvEigenObjVect          [MAX_EIGENS_USED];
    IplImage*  PCA_cvAvgImage; 
    int        PCA_cvNrEigenObjects;
   
    float      __attribute__((aligned(16))) PCA_cvEigenValues        [EXTD_MAX_EIGENS_USED];
    float      __attribute__((aligned(16))) PCA_cvDecompCoeff        [MAX_PCA_SAMPLE_ANALYZABLE][EXTD_MAX_EIGENS_USED];
    
    float      __attribute__((aligned(16))) PCA_cvAvgDecompCoeff     [EXTD_MAX_EIGENS_USED];
    float      __attribute__((aligned(16))) PCA_cvVarDecompCoeff     [EXTD_MAX_EIGENS_USED];
    float      __attribute__((aligned(16))) PCA_cvInvVarDecompCoeff  [EXTD_MAX_EIGENS_USED];
    
    float      __attribute__((aligned(16))) PCA_cvCustomDecompCoeff  [EXTD_MAX_EIGENS_USED];

    CvTermCriteria PCA_cvCriteria;		// Criterio per bloccare il calcolo degli Eigen

    char PCAModule_Already_Init = 0;

#endif

/*
   ---------------------------------------------------------------------------
   INITIALIZATION (PCA)
   ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

void baPCA_InitRecognizer (void)
{
    int CounterA, CounterB;
    
    // Azzera il vettore di indirizzi immagine che verrà passato alle
    // routines OpenCV
    
    for (CounterA=0; CounterA<MAX_PCA_SAMPLE_ANALYZABLE; CounterA++)
    {
        PCA_cvInputImagesVect [CounterA]          = 0;
        
        PCA_cvInfoImagesVect  [CounterA].NrSlot   = 0;
        PCA_cvInfoImagesVect  [CounterA].NrSample = 0;
    }
    
    // Azzera il vettore di indirizzi Eigen
    
    for (CounterA=0; CounterA<MAX_EIGENS_USED; CounterA++)
    {
        PCA_cvEigenObjVect [CounterA] = 0;
    } 
    
    // Alloca l'immagine destinata a contenere la media degli Eigen
    
    PCA_cvAvgImage = cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), IPL_DEPTH_32F, 1 );
    
    // Segnala che il PCA recognizer è già stato inizializzato
    
    PCAModule_Already_Init=1;
}

#endif

/*
   ---------------------------------------------------------------------------
   FUNZIONI MATEMATICHE SPECIFICHE (PCA)
   ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

static inline float FASTKRN_WeightedEuclideDistance ( const float *ADR_cvCustomDecompCoeff, const float *ADR_cvDecompCoeff, const float *ADR_cvInvVarDecompCoeff  )
{
	 float PartialSum;
	
	 __asm__ volatile 
     (
        ".set push\n"						// save assember option
		".set noreorder\n"				    // suppress reordering
		
        "vzero.s S200\n"                    // Valore della somma parziale posto inizialmente a zero
        
                                            // NrBlock = 0
        "lv.q C000, 0 + %1 \n"			    // C000 = PCA_cvCustomDecompCoeff []
        "lv.q C010, 0 + %2 \n"              // C010 = PCA_cvDecompCoeff [CounterOpCvImg][]
        "lv.q C020, 0 + %3 \n"              // C020 = PCA_cvInvVarDecompCoeff []
        
        "vsub.q   C030, C000, C010\n"       // C030 = PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][]
        "vmul.q   C030, C030, C030\n"       // C030 = C030^2 = (PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][])^2
        "vdot.q   S100, C020, C030\n"       // Moltiplica Value*Value per  PCA_cvInvVarDecompCoeff []        
        "vadd.s   S200, S200, S100\n"       // Somma il risultato del prodotto parziale in S200

                                            // NrBlock = 1
        "lv.q C000, 16 + %1 \n"			    // C000 = PCA_cvCustomDecompCoeff []
        "lv.q C010, 16 + %2 \n"             // C010 = PCA_cvDecompCoeff [CounterOpCvImg][]
        "lv.q C020, 16 + %3 \n"             // C020 = PCA_cvInvVarDecompCoeff []
        
        "vsub.q   C030, C000, C010\n"       // C030 = PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][]
        "vmul.q   C030, C030, C030\n"       // C030 = C030^2 = (PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][])^2
        "vdot.q   S100, C020, C030\n"       // Moltiplica Value*Value per  PCA_cvInvVarDecompCoeff []        
        "vadd.s   S200, S200, S100\n"       // Somma il risultato del prodotto parziale in S200

                                            // NrBlock = 2
        "lv.q C000, 32 + %1 \n"			    // C000 = PCA_cvCustomDecompCoeff []
        "lv.q C010, 32 + %2 \n"             // C010 = PCA_cvDecompCoeff [CounterOpCvImg][]
        "lv.q C020, 32 + %3 \n"             // C020 = PCA_cvInvVarDecompCoeff []
        
        "vsub.q   C030, C000, C010\n"       // C030 = PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][]
        "vmul.q   C030, C030, C030\n"       // C030 = C030^2 = (PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][])^2
        "vdot.q   S100, C020, C030\n"       // Moltiplica Value*Value per  PCA_cvInvVarDecompCoeff []        
        "vadd.s   S200, S200, S100\n"       // Somma il risultato del prodotto parziale in S200

                                            // NrBlock = 3
        "lv.q C000, 48 + %1 \n"			    // C000 = PCA_cvCustomDecompCoeff []
        "lv.q C010, 48 + %2 \n"             // C010 = PCA_cvDecompCoeff [CounterOpCvImg][]
        "lv.q C020, 48 + %3 \n"             // C020 = PCA_cvInvVarDecompCoeff []
        
        "vsub.q   C030, C000, C010\n"       // C030 = PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][]
        "vmul.q   C030, C030, C030\n"       // C030 = C030^2 = (PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][])^2
        "vdot.q   S100, C020, C030\n"       // Moltiplica Value*Value per  PCA_cvInvVarDecompCoeff []        
        "vadd.s   S200, S200, S100\n"       // Somma il risultato del prodotto parziale in S200

                                            // NrBlock = 4
        "lv.q C000, 64 + %1 \n"			    // C000 = PCA_cvCustomDecompCoeff []
        "lv.q C010, 64 + %2 \n"             // C010 = PCA_cvDecompCoeff [CounterOpCvImg][]
        "lv.q C020, 64 + %3 \n"             // C020 = PCA_cvInvVarDecompCoeff []
        
        "vsub.q   C030, C000, C010\n"       // C030 = PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][]
        "vmul.q   C030, C030, C030\n"       // C030 = C030^2 = (PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][])^2
        "vdot.q   S100, C020, C030\n"       // Moltiplica Value*Value per  PCA_cvInvVarDecompCoeff []        
        "vadd.s   S200, S200, S100\n"       // Somma il risultato del prodotto parziale in S200

                                            // NrBlock = 5
        "lv.q C000, 80 + %1 \n"			    // C000 = PCA_cvCustomDecompCoeff []
        "lv.q C010, 80 + %2 \n"             // C010 = PCA_cvDecompCoeff [CounterOpCvImg][]
        "lv.q C020, 80 + %3 \n"             // C020 = PCA_cvInvVarDecompCoeff []
        
        "vsub.q   C030, C000, C010\n"       // C030 = PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][]
        "vmul.q   C030, C030, C030\n"       // C030 = C030^2 = (PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][])^2
        "vdot.q   S100, C020, C030\n"       // Moltiplica Value*Value per  PCA_cvInvVarDecompCoeff []        
        "vadd.s   S200, S200, S100\n"       // Somma il risultato del prodotto parziale in S200

                                            // NrBlock = 6
        "lv.q C000, 96 + %1 \n"			    // C000 = PCA_cvCustomDecompCoeff []
        "lv.q C010, 96 + %2 \n"             // C010 = PCA_cvDecompCoeff [CounterOpCvImg][]
        "lv.q C020, 96 + %3 \n"             // C020 = PCA_cvInvVarDecompCoeff []
        
        "vsub.q   C030, C000, C010\n"       // C030 = PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][]
        "vmul.q   C030, C030, C030\n"       // C030 = C030^2 = (PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][])^2
        "vdot.q   S100, C020, C030\n"       // Moltiplica Value*Value per  PCA_cvInvVarDecompCoeff []        
        "vadd.s   S200, S200, S100\n"       // Somma il risultato del prodotto parziale in S200

                                            // NrBlock = 7
        "lv.q C000, 112 + %1 \n"			// C000 = PCA_cvCustomDecompCoeff []
        "lv.q C010, 112 + %2 \n"            // C010 = PCA_cvDecompCoeff [CounterOpCvImg][]
        "lv.q C020, 112 + %3 \n"            // C020 = PCA_cvInvVarDecompCoeff []
        
        "vsub.q   C030, C000, C010\n"       // C030 = PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][]
        "vmul.q   C030, C030, C030\n"       // C030 = C030^2 = (PCA_cvCustomDecompCoeff [] - PCA_cvDecompCoeff [CounterOpCvImg][])^2
        "vdot.q   S100, C020, C030\n"       // Moltiplica Value*Value per  PCA_cvInvVarDecompCoeff []        
        "vadd.s   S200, S200, S100\n"       // Somma il risultato del prodotto parziale in S200

        "sv.s     S200, %0\n"               // Deposita il risultato in uscita
        ".set pop\n"						// restore assember option
	
        : "=m"(PartialSum) 
        : "m"( *ADR_cvCustomDecompCoeff ), 
          "m"( *ADR_cvDecompCoeff ), 
          "m"( *ADR_cvInvVarDecompCoeff )  
    );
    
    return (PartialSum);
}

#endif


/*
   ---------------------------------------------------------------------------
   AGGIORNAMENTO DEI DATI DEL RECOGNIZER (PCA)
   ---------------------------------------------------------------------------
*/

#ifndef PSP1000_VERSION

void baPCA_UpdateFaceRecognizerDBase (int NrSlotUpdated)
{
    int   CounterImg, CounterSample, CounterOpCvImg, DimCounter;
    int   NrImgCounted;
    
    float Sum, Diff1, Diff2;
    
    if (!PCAModule_Already_Init) baPCA_InitRecognizer ();
    
    // Provvedi ad azzerare tutti i vettori di informazione
    
    for (CounterImg=0; CounterImg<MAX_PCA_SAMPLE_ANALYZABLE; CounterImg++)
    {
        for (DimCounter=0; DimCounter<EXTD_MAX_EIGENS_USED; DimCounter++)
        {
            PCA_cvDecompCoeff [CounterImg][DimCounter] = 0;
        }
    }
    
    for (DimCounter=0; DimCounter<EXTD_MAX_EIGENS_USED; DimCounter++)
    {
        PCA_cvEigenValues        [DimCounter] = 0;
        PCA_cvAvgDecompCoeff     [DimCounter] = 0;
        PCA_cvVarDecompCoeff     [DimCounter] = 0;
        PCA_cvInvVarDecompCoeff  [DimCounter] = 0;
    
        PCA_cvCustomDecompCoeff  [DimCounter] = 0;
    }
    
    // OpenCV ha bisogno, per potere eseguire correttamente Eigen, che venga creato
    // un vettore ordinato di puntatori ad immagini, tutte con le medesime
    // dimensioni. Per prima cosa azzera completamente il vettore cvInputImages
    
    for (CounterImg=0; CounterImg<MAX_PCA_SAMPLE_ANALYZABLE; CounterImg++)
    {
        PCA_cvInputImagesVect [CounterImg] = 0;
    }
    
    // Ed adesso copia gli indirizzi delle immagini a 8 bit dentro il vettore
    // cvInputImages. Per ogni immagine viene anche copiata una coppia di
    // informazioni dentro il vettore cvInfoImagesVect: si tratta del numero
    // di slot e del numero di campione. In tal modo, diviene possibile
    // risalire velocemente dall'immagine nel vettore OpenCV 
    // al numero di slot e di campione nella struct XPCA
    
    NrImgCounted=0;
    
    for (CounterImg=0; CounterImg<NR_SLOTS_FACE_RECOGNIZER; CounterImg++)
    {
        if (facerec_ImagesInRam [CounterImg].IsActive)
        {
             for (CounterSample=0; CounterSample<NR_SAMPLES_FOR_FACE; CounterSample++)
             {
                 PCA_cvInputImagesVect [NrImgCounted] = facerec_ImagesInRam [CounterImg].Img [CounterSample];
                 
                 PCA_cvInfoImagesVect  [NrImgCounted].NrSlot   = CounterImg;
                 PCA_cvInfoImagesVect  [NrImgCounted].NrSample = CounterSample;
                 
                 NrImgCounted++;
             }
        }
    }
    
    // Adesso registra il numero di immagini che in questo momento sono memorizzate. 
    
    PCA_cvNrInputImages = NrImgCounted;
    
    // E copia questo valore nella variabile generale che viene usata da tutto il sistema
    
    cvNrInputImages     = NrImgCounted;
    
    if (PCA_cvNrInputImages>0)
    {
            // Il numero di Eigens che sono necessari: è pari al numero di immagini
            // memorizzate in quel momento - 1
            
            PCA_cvNrEigenObjects = PCA_cvNrInputImages - 1;
            
            // E' necessario andare a resettare le immagini a 32 bit float che
            // dovranno contenere gli EigenObjects: in tal modo, il loro numero
            // coinciderebbe con NrImgCounter-1. La sezione seguente provvede
            // ad allocare le aree di memoria necessarie ed a liberare quelle
            // non più necessarie
             
            for (DimCounter=0; DimCounter<MAX_EIGENS_USED; DimCounter++)
            {
                // Provvede ad allocare l'area di memoria per l'EigenObject se ciò è necessario
                
                if ((PCA_cvEigenObjVect [DimCounter]==0) && (DimCounter<PCA_cvNrEigenObjects))
                {
                   PCA_cvEigenObjVect [DimCounter] = cvCreateImage( cvSize (STD_FACEREC__SAMPLEIMG__DIMX, STD_FACEREC__SAMPLEIMG__DIMY), IPL_DEPTH_32F, 1 );
                   continue;                         
                }
                
                // Cancella le aree di memoria non più necessarie
                
                if ((PCA_cvEigenObjVect [DimCounter]!=0) && (DimCounter>=PCA_cvNrEigenObjects))
                {
                   cvReleaseImage ( &(PCA_cvEigenObjVect [DimCounter]) );           // Prima libera l'area di memoria                      
                   PCA_cvEigenObjVect [DimCounter]=0;                  // e poi pone a 0 il valore relativo nel vettore
                }
            }
            
            // Preparati a ricalcolare tutti gli EigenObjects
            
            PCA_cvCriteria.type = CV_TERMCRIT_ITER|CV_TERMCRIT_EPS;
            
            //PCA_cvCriteria.max_iter = 30;
            //PCA_cvCriteria.epsilon = 1.0e-27;
            
            PCA_cvCriteria.max_iter = 80;
            PCA_cvCriteria.epsilon = 1.0e-30;
            
            // Calcola gli eigenobjects ed gli eigenvals
            
            cvCalcEigenObjects ( PCA_cvNrInputImages, &(PCA_cvInputImagesVect[0]), &(PCA_cvEigenObjVect[0]), 0, 0, 0, &PCA_cvCriteria, PCA_cvAvgImage, PCA_cvEigenValues );
        
            // Adesso calcola i coefficienti di decomposizione
            
            for ( CounterOpCvImg=0; CounterOpCvImg<PCA_cvNrInputImages; CounterOpCvImg++ )
            {
        	    zcvEigenDecomposite ( PCA_cvInputImagesVect [CounterOpCvImg], PCA_cvNrEigenObjects, &(PCA_cvEigenObjVect[0]), 0, 0, PCA_cvAvgImage, &(PCA_cvDecompCoeff[CounterOpCvImg][0]) );
        	}
        	
            // Adesso calcoliamo le varianze delle varie dimensioni. Per prima cosa determiniamo il vettore 
            // dei valori medi
            
            for( DimCounter=0; DimCounter<PCA_cvNrEigenObjects; DimCounter++ )
            {
                Sum=0; 
                 
                for( CounterOpCvImg=0; CounterOpCvImg<PCA_cvNrInputImages; CounterOpCvImg++ ) 
                {
                    Sum += PCA_cvDecompCoeff [CounterOpCvImg][DimCounter]; 
                } 
                
                PCA_cvAvgDecompCoeff [DimCounter] = Sum/PCA_cvNrInputImages;
            }
            
            // Ora calcoliamo le varianze ed i reciproci
            
            for (DimCounter=0; DimCounter<PCA_cvNrEigenObjects; DimCounter++)
            {
                 Sum=0;
                 
                 for (CounterOpCvImg=0; CounterOpCvImg<PCA_cvNrInputImages; CounterOpCvImg++)
                 {
                     Diff1 = PCA_cvDecompCoeff [CounterOpCvImg][DimCounter] - PCA_cvAvgDecompCoeff [DimCounter];
                     
                     Sum+=Diff1*Diff1;
                 }
                 
                 PCA_cvVarDecompCoeff    [DimCounter] = (Sum/PCA_cvNrEigenObjects);
                 PCA_cvInvVarDecompCoeff [DimCounter] = (1.0/PCA_cvVarDecompCoeff [DimCounter]);
            }
    }
    else    // Non ci sono immagini memorizzate: E' necessario soltanto liberare la memoria   
    {       // impegnata dagli Eigen   
            
            PCA_cvNrEigenObjects = 0;
            
            for (DimCounter=0; DimCounter<MAX_EIGENS_USED; DimCounter++)
            {
                // Cancella le aree di memoria non più necessarie
                
                if (PCA_cvEigenObjVect [DimCounter]!=0)
                {
                   cvReleaseImage ( &(PCA_cvEigenObjVect [DimCounter]) );           // Prima libera l'area di memoria                      
                   PCA_cvEigenObjVect [DimCounter]=0;                               // e poi pone a 0 il valore relativo nel vettore
                }
            }       
    }
}


int baPCA_UpdateDistanceData (IplImage *MyImage)
/*
    Questa routine esegue un update dei dati di distanza per una determinata
    immagine. La routine restituisce in uscita il numero di face slot che è
    stato rilevato. Se viene restituito un valore negativo, siamo in presenza
    di un errore
*/
{
    register float  Distance, PartialSum, RatioThreshold;
    register int    Counter, CounterOpCvImg, CounterSlot, CounterSample;
    register int    NrSlot, NrSample;
    register char   IsFirstTime;
    
    register int    NrBlocks; 
         
    // Calcola il numero di istanze del kernel che dovranno essere chiamate: si
    // tenga presente che il kernel calcola 8 gruppi di 4 float alla volta     
            
    NrBlocks = PCA_cvNrEigenObjects / (8*4);
            
               
    if (PCA_cvNrInputImages>0)
    {  
        // Determina i coefficienti Eigen dell'immagine in analisi
        
        zcvEigenDecomposite ( MyImage, PCA_cvNrEigenObjects, &(PCA_cvEigenObjVect[0]), 0, 0, PCA_cvAvgImage, &(PCA_cvCustomDecompCoeff[0]) ); 
        
        // Calcola la distanza (euclidea ponderata con varianza)
        
        for (CounterOpCvImg=0; CounterOpCvImg<PCA_cvNrInputImages; CounterOpCvImg++)
        {
            // Calcola la distanza tra l'immagine nr. CounterOpCvImg nel vettore dei sample
            
            /*         //     SEZIONE SOSTITUITA DA UNA SEZIONE IN ASSEMBLER VFPU
            
            Distance = 0;
        
            for (Counter=0; Counter<PCA_cvNrEigenObjects; Counter++)
            {
                Value    = PCA_cvCustomDecompCoeff [Counter] - PCA_cvDecompCoeff [CounterOpCvImg][Counter];
                Distance += (Value*Value*PCA_cvInvVarDecompCoeff [Counter]);
            }
            
            */
            
            Distance = 0;
            
            for (Counter=0; Counter<=NrBlocks; Counter++)
            {
                PartialSum = FASTKRN_WeightedEuclideDistance ( &PCA_cvCustomDecompCoeff [Counter*32], 
                                                               &PCA_cvDecompCoeff [CounterOpCvImg][Counter*32],
                                                               &PCA_cvInvVarDecompCoeff [Counter*32]   );    
                
                Distance += PartialSum;
            }
            
              
            // Adesso in distance c'è il quadrato della distanza euclidea pesata. 
            // Eseguiamo la radice quadrata e memorizziamola nella struttura
            // bidimensionale per i dati PCA
            
            NrSlot   = PCA_cvInfoImagesVect [CounterOpCvImg].NrSlot;
            NrSample = PCA_cvInfoImagesVect [CounterOpCvImg].NrSample;
            
            PCA_cvInfoImages [NrSlot].SampleDistance [NrSample] = vfpu_sqrtf (Distance);   
        } 
        
        // Calcoliamo la media delle distanze reciproche per ogni singola faccia
        
        for (CounterSlot=0; CounterSlot<NR_SLOTS_FACE_RECOGNIZER; CounterSlot++)
        {
           if (facerec_ImagesInRam [CounterSlot].IsActive)
           {
               PartialSum=0;
               
               for (CounterSample=0; CounterSample<NR_SAMPLES_FOR_FACE; CounterSample++)
               {    
                   PartialSum += PCA_cvInfoImages [CounterSlot].SampleDistance [CounterSample];               
               }                                                
               
               facerec_ImagesInRam [CounterSlot].GeneralDistance = PartialSum/NR_SAMPLES_FOR_FACE; 
           }
        } 
        
        // Adesso determina la minima distanza e l'elemento di minima distanza, 
        
        IsFirstTime = 1;
        facerec_MinimumElement     = -1;
        
        for (CounterSlot=0; CounterSlot<NR_SLOTS_FACE_RECOGNIZER; CounterSlot++)
        {
           if (facerec_ImagesInRam [CounterSlot].IsActive)
           {         
               if ((IsFirstTime) || (facerec_MinimumDistance>facerec_ImagesInRam [CounterSlot].GeneralDistance))
               {
                   facerec_MinimumElement     = CounterSlot;
                   facerec_MinimumDistance    = facerec_ImagesInRam [CounterSlot].GeneralDistance;
                   
                   IsFirstTime=0;            
               }                                                  
           }
        }
        
        // Determina la minima distanza secondaria e l'elemento di minima distanza secondaria
        
        IsFirstTime = 1;
        facerec_SecMinimumElement  = -1;
        
        for (CounterSlot=0; CounterSlot<NR_SLOTS_FACE_RECOGNIZER; CounterSlot++)
        {
           if ((facerec_ImagesInRam [CounterSlot].IsActive) && (CounterSlot!=facerec_MinimumElement))
           {         
               if ((IsFirstTime) || (facerec_SecMinimumDistance>facerec_ImagesInRam [CounterSlot].GeneralDistance))
               {
                   facerec_SecMinimumElement     = CounterSlot;
                   facerec_SecMinimumDistance    = facerec_ImagesInRam [CounterSlot].GeneralDistance;
                   
                   IsFirstTime=0;            
               }                                                  
           }
        }
        
        // Adesso esegui la discriminazione dei risultati
        
        if (facerec_SecMinimumElement!=-1)         // Ci sono almeno 2 elementi che potrebbero essere 
        {                                          // gli individui corretti
            
            RatioThreshold = facerec_MinimumDistance/facerec_SecMinimumDistance;
            
            // Se il rapporto tra le distanze è inferiore al 99%, il dato si
            // considera significativo
        
            if (RatioThreshold<(SystemOptPacket.SkipThreshold*0.001)) return facerec_MinimumElement;
              else return -2;
        }
        else            // C'è solo un elemento: si presume che sia quello corretto
        {
            return facerec_MinimumElement;
        }
    }    
    else // Non ci sono immagini con cui fare il confronto
    {
        return -1;
    }                           
}

#endif

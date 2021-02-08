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
   ROUTINES METAOPENCV C++
   ----------------------------------------------------------------------------
   Queste routines sono in grado di sostituire delle routines OpenCV. Le
   routines sostitutive sono molto più veloci rispetto al codice che
   che si sarebbe dovuto eseguire se si fossero usate semplicemente le
   routines OpenCV standard.
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
        #include <_cvaux.h>

/*
    ------------------------------------------------------------------------------
    ROUTINE ZCVEIGENDECOMPOSITE
    ------------------------------------------------------------------------------
    Questa routine mima il comportamento della normale cvEigenDecomposite, ma
    ha una maggiore velocità. La maggiore velocità viene ottenuta per mezzo
    di routines in assembler che sfruttano la VFPU, unite ad alcune ottimizzazioni
    del codice che eliminano alcuni controlli inutili.
    
    Il codice è derivato dalle routines OpenCV di Intel.
    ------------------------------------------------------------------------------
*/

extern "C"
{


/*
   ----------------------------------------------------------------------------
   FAST KERNEL PER cvCalcDecompCoeff
   ----------------------------------------------------------------------------
*/



static inline void FASTKRN_cvCalcDecompCoeff (uchar *obj, float *eigObj, float *avg, float *w)
{
     __asm__ volatile ( 
             "lv.s   S700, %3 \n"           // S700 = contenuto di w 
             
             "lbu    $5,   %0       \n"     // Carica il byte obj [k]
             "mtv    $5,   S000     \n"     // Trasferisci in VFPU
             "lbu    $5,   1 + %0   \n"     // Carica il byte obj [k+1]
             "mtv    $5,   S001     \n"     // Trasferisci in VFPU
             "lbu    $5,   2 + %0   \n"     // Carica il byte obj [k+2]
             "mtv    $5,   S002     \n"     // Trasferisci in VFPU
             "lbu    $5,   3 + %0   \n"     // Carica il byte obj [k+3]
             "mtv    $5,   S003     \n"     // Trasferisci in VFPU
             "vi2f.q C000, C000, 0  \n"     // Converti in float il contenuto dei registri
             
             "lv.s   S010,      %1 \n"      // In S010 c'è eigObj [k]
             "lv.s   S011,      %2 \n"      // In S011 c'è avg [k]
             "vsub.s S701, S000, S011 \n"   // In S701 c'è S000-S011 = obj [k] - avg [k]
             "vmul.s S701, S010, S701 \n"   // In S701 c'è (S010)*(S000-S011) = eigObj [k]*(obj [k] - avg [k])
             "vadd.s S700, S701, S700 \n"   // w = w + eigObj [k]*(obj [k] - avg [k])
             
             "lv.s   S010,  4 + %1 \n"      // In S010 c'è eigObj [k+1]
             "lv.s   S011,  4 + %2 \n"      // In S011 c'è avg [k+1]
             "vsub.s S701, S001, S011 \n"   // In S701 c'è S001-S011 = obj [k+1] - avg [k+1]
             "vmul.s S701, S010, S701 \n"   // In S701 c'è (S010)*(S000-S011) = eigObj [k+1]*(obj [k+1] - avg [k+1])
             "vadd.s S700, S701, S700 \n"   // w = w + eigObj [k+1]*(obj [k+1] - avg [k+1])
             
             "lv.s   S010,  8 + %1 \n"      // In S010 c'è eigObj [k+2]
             "lv.s   S011,  8 + %2 \n"      // In S011 c'è avg [k+2]
             "vsub.s S701, S002, S011 \n"   // In S701 c'è S001-S011 = obj [k+2] - avg [k+2]
             "vmul.s S701, S010, S701 \n"   // In S701 c'è (S010)*(S000-S011) = eigObj [k+2]*(obj [k+2] - avg [k+2])
             "vadd.s S700, S701, S700 \n"   // w = w + eigObj [k+2]*(obj [k+2] - avg [k+2])
             
             "lv.s   S010, 12 + %1 \n"      // In S010 c'è eigObj [k+3]
             "lv.s   S011, 12 + %2 \n"      // In S011 c'è avg [k+3]
             "vsub.s S701, S003, S011 \n"   // In S701 c'è S001-S011 = obj [k+3] - avg [k+3]
             "vmul.s S701, S010, S701 \n"   // In S701 c'è (S010)*(S000-S011) = eigObj [k+3]*(obj [k+3] - avg [k+2])
             "vadd.s S700, S701, S700 \n"   // w = w + eigObj [k+3]*(obj [k+3] - avg [k+3])
             
             "sv.s   S700, %3 \n"           // salva contenuto di w  
             
     :  : "m"(*obj), "m"(*eigObj), "m"(*avg), "m"(*w), "r"(*eigObj), "r"(*avg) : "$5", "$6");  
     
     return;
}


static inline float zcvCalcDecompCoeff_8u32fR( uchar* obj, int objStep,
                           float *eigObj, int eigStep,
                           float *avg, int avgStep, CvSize size )
{
    register int i, k;
    float w = 0.0f;

    if( size.width > objStep || 4 * size.width > eigStep
        || 4 * size.width > avgStep || size.height < 1 )
        return -1.0e30f;
        
    if( obj == NULL || eigObj == NULL || avg == NULL )
        return -1.0e30f;

    eigStep /= 4;
    avgStep /= 4;

    if( size.width == objStep && size.width == eigStep && size.width == avgStep )
    {
        size.width *= size.height;
        size.height = 1;
        objStep = eigStep = avgStep = size.width;
    }

    for( i = 0; i < size.height; i++, obj += objStep, eigObj += eigStep, avg += avgStep )
    {
        for( k = 0; k < size.width - 4; k += 4 )
        {
            // Richiama la routine in assembler che usa la VFPU per accelerare
            // i calcoli matematici e calcolare più rapidamente i coefficienti
            // di decomposizione
            
            FASTKRN_cvCalcDecompCoeff (&(obj[k]), &(eigObj[k]), &(avg[k]), &w);  
        }
        
        for( ; k < size.width; k++ )
            w += eigObj[k] * ((float) obj[k] - avg[k]);
    }

    return w;
}



/*

static inline float zcvCalcDecompCoeff_8u32fR( uchar* obj, int objStep,
                           float *eigObj, int eigStep,
                           float *avg, int avgStep, CvSize size )
{
    int i, k;
    float w = 0.0f;

    if( size.width > objStep || 4 * size.width > eigStep
        || 4 * size.width > avgStep || size.height < 1 )
        return -1.0e30f;
        
    if( obj == NULL || eigObj == NULL || avg == NULL )
        return -1.0e30f;

    eigStep /= 4;
    avgStep /= 4;

    if( size.width == objStep && size.width == eigStep && size.width == avgStep )
    {
        size.width *= size.height;
        size.height = 1;
        objStep = eigStep = avgStep = size.width;
    }

    for( i = 0; i < size.height; i++, obj += objStep, eigObj += eigStep, avg += avgStep )
    {
        for( k = 0; k < size.width - 4; k += 4 )
        {
            float o = (float) obj[k];
            float e = eigObj[k];
            float a = avg[k];

            w += e * (o - a);
            o = (float) obj[k + 1];
            e = eigObj[k + 1];
            a = avg[k + 1];
            w += e * (o - a);
            o = (float) obj[k + 2];
            e = eigObj[k + 2];
            a = avg[k + 2];
            w += e * (o - a);
            o = (float) obj[k + 3];
            e = eigObj[k + 3];
            a = avg[k + 3];
            w += e * (o - a);
        }
        for( ; k < size.width; k++ )
            w += eigObj[k] * ((float) obj[k] - avg[k]);
    }

    return w;
}

*/

static inline CvStatus zcvEigenDecomposite_8u32fR( uchar * obj, int objStep, int nEigObjs,
                            void *eigInput, int eigStep, int ioFlags,
                            void *userData, float *avg, int avgStep,
                            CvSize size, float *coeffs )
{
    int i;

    if( nEigObjs < 2 )
        return CV_BADFACTOR_ERR;
    
    if( ioFlags < 0 || ioFlags > 1 )
        return CV_BADFACTOR_ERR;
    
    if( size.width > objStep || 4 * size.width > eigStep ||
        4 * size.width > avgStep || size.height < 1 )
        return CV_BADSIZE_ERR;
    
    if( obj == NULL || eigInput == NULL || coeffs == NULL || avg == NULL )
        return CV_NULLPTR_ERR;
    
    for( i = 0; i < nEigObjs; i++ )
        if( ((uchar **) eigInput)[i] == NULL )
            return CV_NULLPTR_ERR;

    /* no callback */
    
    for( i = 0; i < nEigObjs; i++ )
    {
        float w = zcvCalcDecompCoeff_8u32fR( obj, objStep, ((float **) eigInput)[i],
                                              eigStep, avg, avgStep, size );

        if( w < -1.0e29f )
            return CV_NOTDEFINED_ERR;
        coeffs[i] = w;
    }

    return CV_NO_ERR;
}



void zcvEigenDecomposite( IplImage* obj, int nEigObjs, void* eigInput,
                          int       ioFlags, void*     userData, 
                          IplImage* avg, float*    coeffs )
{
    float *avg_data;
    uchar *obj_data;
    int avg_step = 0, obj_step = 0;
    CvSize avg_size, obj_size;
    int i;

    cvGetImageRawData( avg, (uchar **) & avg_data, &avg_step, &avg_size );
    cvGetImageRawData( obj, &obj_data, &obj_step, &obj_size );
    
    IplImage **eigens = (IplImage **) (((CvInput *) & eigInput)->data);
    float **eigs = (float **) cvAlloc( sizeof( float * ) * nEigObjs );
    int eig_step = 0, old_step = 0;
    CvSize eig_size = avg_size, old_size = avg_size;

    for( i = 0; i < nEigObjs; i++ )
    {
        IplImage *eig = eigens[i];
        float *eig_data;

        cvGetImageRawData( eig, (uchar **) & eig_data, &eig_step, &eig_size );
        
        old_step = eig_step;
        old_size = eig_size;
        eigs[i] = eig_data;
    }

    zcvEigenDecomposite_8u32fR( obj_data, obj_step, nEigObjs, (void*) eigs, eig_step, ioFlags, userData, 
                                avg_data, avg_step, obj_size, coeffs   );
    
    cvFree( (void **) &eigs );

}



}  // Extern C

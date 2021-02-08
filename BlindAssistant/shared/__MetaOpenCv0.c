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
   ROUTINES METAOPENCV C
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
 
  

 
    
 
/*
   ----------------------------------------------------------------------------
   PROCEDURE GENERICHE
   ----------------------------------------------------------------------------
*/
  
int zcvCalculateAvgBrightness (IplImage *Image, float *AdrAverage)
/*
    Calcola la luminosità media e la salva nella variabile float puntata da
    AdrAverage
*/
{
    register int CounterX, CounterY;
    
    register unsigned char   *PntToValue8Row; 
    register unsigned char   *PntToValue8;
    
    register int   Value8;
    register float Average;
    
    if (Image!=0)
    {
        Average=0;
        
        for (CounterY=0, PntToValue8Row=Image->imageData; CounterY<Image->height; CounterY++, PntToValue8Row+=Image->widthStep)
        {
            for (CounterX=0, PntToValue8=PntToValue8Row; CounterX<Image->width; CounterX++, PntToValue8++)
            {
                Value8 = *(unsigned char *)(PntToValue8);
                Average += Value8;
            }
        }
        
        Average = Average/((float)(Image->width)*(float)(Image->height));
        
        *AdrAverage = Average;           // Registra la media in una variabile float esterna         
    }
}







int zcvNormalizeImage8Bit (IplImage *Image, float *AdrAverage)
/*
     Si occupa di eseguire la normalizzazione dell'immagine. Questo dovrebbe
     proteggere il sistema dai cambiamenti di luce.
     
     Restituisce 0 in caso di successo.
*/
{  
    register int CounterX, CounterY;
    
    register unsigned char   *PntToValue8Row; 
    register unsigned char   *PntToValue8;
    
    register int   Value8, Diff, Maximum, Minimum;
    float          ScaleFactor, Average;
    
    if (Image!=0)
    {
        // Calcoliamo la media dei pixel (ci da una informazione di illuminazione)
        
        Average=0;
        
        for (CounterY=0, PntToValue8Row=Image->imageData; CounterY<Image->height; CounterY++, PntToValue8Row+=Image->widthStep)
        {
            for (CounterX=0, PntToValue8=PntToValue8Row; CounterX<Image->width; CounterX++, PntToValue8++)
            {
                Value8 = *(unsigned char *)(PntToValue8);
                Average += Value8;
            }
        }
        
        Average = Average/((float)(Image->width)*(float)(Image->height));
        
        *AdrAverage = Average;           // Registra la media in una variabile float esterna         
        
        // Calcola il massimo ed il minimo
        
        Maximum=0;
        Minimum=255;
        
        for (CounterY=0, PntToValue8Row=Image->imageData; CounterY<Image->height; CounterY++, PntToValue8Row+=Image->widthStep)
        {
            for (CounterX=0, PntToValue8=PntToValue8Row; CounterX<Image->width; CounterX++, PntToValue8++)
            {
                Value8 = *(unsigned char *)(PntToValue8);
                if (Maximum<Value8) Maximum=Value8;
                if (Minimum>Value8) Minimum=Value8;
            }
        }
        
        // Esegui la normalizzazione
        
        ScaleFactor = 255.0/(Maximum-Minimum);
            
        for (CounterY=0, PntToValue8Row=Image->imageData; CounterY<Image->height; CounterY++, PntToValue8Row+=Image->widthStep)
        {
            for (CounterX=0, PntToValue8=PntToValue8Row; CounterX<Image->width; CounterX++, PntToValue8++)
            {
                Value8 = *(unsigned char *)(PntToValue8);
                *(unsigned char *)(PntToValue8)=ScaleFactor*(Value8-Minimum);
            }
        }
        
        return 0;       
    }
    else return -1; 
}

  
int zcvNormalizeImage8BitUsingZone (IplImage *Image, float *AdrAverage)
/*
     Si occupa di eseguire la normalizzazione dell'immagine. Questo dovrebbe
     proteggere il sistema dai cambiamenti di luce.
     
     Restituisce 0 in caso di successo.
*/
{  
    register int CounterX, CounterY, CounterBlkX, CounterBlkY;
    int          LenBlkX, LenBlkY, NrBlkX, NrBlkY;
    int          X1, Y1, X2, Y2;
    
    register unsigned char   *PntToValue8Row; 
    register unsigned char   *PntToValue8;
    
    register int   Value8, CorrValue8, Sum, SubSum;
    float          ScaleFactor, Average;
    
    if (Image!=0)
    {
        NrBlkX = 2;
        NrBlkY = 2;
        
        LenBlkX = Image->width/NrBlkX;
        LenBlkY = Image->height/NrBlkY;
        
        Sum=0;
        
        for (CounterBlkY=0; CounterBlkY<NrBlkY; CounterBlkY++)
        {
            for (CounterBlkX=0; CounterBlkX<NrBlkX; CounterBlkX++)
            {
                X1 = CounterBlkX*LenBlkX;
                Y1 = CounterBlkY*LenBlkY;
                X2 = X1+LenBlkX;
                Y2 = Y1+LenBlkY;
                
                SubSum = 0;
                
                for (CounterY=Y1, PntToValue8Row=Image->imageData+Y1*Image->widthStep; CounterY<Y2; CounterY++, PntToValue8Row+=Image->widthStep)
                {
                    for (CounterX=X1, PntToValue8=PntToValue8Row+X1; CounterX<X2; CounterX++, PntToValue8++)
                    {
                        SubSum += *PntToValue8;
                    }
                }
                
                Average  = (float)(SubSum) / (float)(LenBlkX*LenBlkY);
                Sum     += SubSum;
                
                ScaleFactor = 128.0/Average;
                
                for (CounterY=Y1, PntToValue8Row=Image->imageData+Y1*Image->widthStep; CounterY<Y2; CounterY++, PntToValue8Row+=Image->widthStep)
                {
                    for (CounterX=X1, PntToValue8=PntToValue8Row+X1; CounterX<X2; CounterX++, PntToValue8++)
                    {
                        Value8 = *PntToValue8;  
                        
                        CorrValue8 = Value8 * ScaleFactor;
                        if (CorrValue8>255) CorrValue8=255;
                        
                        *PntToValue8 = CorrValue8; 
                    }
                }
            }
        }
        
        if (AdrAverage) *AdrAverage = Sum/(Image->width*Image->height);
    }     
        
}
  
  
  
 
 
int zcvShowImage8bit (IplImage  *Image, unsigned short int PosX1, unsigned short int PosY1, 
                                        unsigned short int PosX2, unsigned short int PosY2,
                                        char WndHandle, char RenderNow)

/*
    Questa routine provvede a visualizzare una immagine IplImage a 8 bit per pixel 
    in un rettangolo all'interno di una finestra Nanodesktop. La routine si
    occupa anche di eseguire lo scaling dell'immagine
*/
{
    register int CounterX, CounterY, PosX, PosY;
    register int LenX,     LenY;
    
    register float  ScaleX,    ScaleY;
    register int    IplPixelX, IplPixelY;
    
    register unsigned char   *PntToValue8Row; 
    register unsigned char   Value8;
    
    register TypeColor *PntToWsRow, *PntToWs; 
    register TypeColor Colore;
    
    register int WsLength;
    
    LenX = PosX2-PosX1;
    LenY = PosY2-PosY1;
    
    if (Image!=0)
    {
        ScaleX = (float)(Image->width-1)/(float)(LenX-1);
        ScaleY = (float)(Image->height-1)/(float)(LenY-1);
  
        // Attenzione a questo punto. L'immagine ha pixel con coordinate da 0 a Image->width-1. E
        // sull'asse x i pixel vanno da 0 a LenX-1. Pertanto, per l'ultimo pixel deve aversi
        // (Image->width-1) / (LenX-1) * (LenX-1) = (Image->width-1)
  
        WsLength = WindowData [WndHandle].WSWndLength;
  
        for (CounterY=0, PosY=PosY1, PntToWsRow=WindowData[WndHandle]._ws + WsLength*PosY1; CounterY<LenY; CounterY++, PosY++, PntToWsRow+=WsLength)
        {
            IplPixelY=ScaleY*CounterY;
            PntToValue8Row = (unsigned char *)(Image->imageData + IplPixelY*Image->widthStep);
            
            for (CounterX=0, PosX=PosX1, PntToWs=PntToWsRow+PosX1; CounterX<LenX; CounterX++, PosX++, PntToWs++)
            {
                IplPixelX=ScaleX*CounterX;
                Value8 = *(unsigned char *)(PntToValue8Row + IplPixelX);
                
                // Massima velocità possibile: il sistema accede direttamente al window space
                // della finestra, dopo avere ricavato il colore
                
                *PntToWs = ((Value8 & 0xF8)<<7) + ((Value8 & 0xF8)<<2) + ((Value8 & 0xF8)>>3) + 0x8000;
            }
        }
        
        if (RenderNow) XWindowRender (WndHandle);  
        return 0;       
    }
    else return -1; 
}  


int zcvShowImage24bit (IplImage  *Image, unsigned short int PosX1, unsigned short int PosY1, 
                                         unsigned short int PosX2, unsigned short int PosY2,
                                         char WndHandle, char RenderNow)

/*
    Questa routine provvede a visualizzare una immagine IplImage a 24 bit per pixel 
    in un rettangolo all'interno di una finestra Nanodesktop. La routine si
    occupa anche di eseguire lo scaling dell'immagine
*/
{
    register int CounterX, CounterY, PosX, PosY;
    register int LenX,     LenY;
    
    register float  ScaleX,    ScaleY;
    register int    IplPixelX, IplPixelY;
    
    register unsigned char   *PntToValue8Row; 
    register unsigned char   Value8R, Value8G, Value8B;
    
    register TypeColor *PntToWsRow, *PntToWs; 
    register TypeColor Colore;
    
    register int WsLength;
    
    LenX = PosX2-PosX1;
    LenY = PosY2-PosY1;
    
    if (Image!=0)
    {
        ScaleX = (float)(Image->width-1)/(float)(LenX-1);
        ScaleY = (float)(Image->height-1)/(float)(LenY-1);
  
        // Attenzione a questo punto. L'immagine ha pixel con coordinate da 0 a Image->width-1. E
        // sull'asse x i pixel vanno da 0 a LenX-1. Pertanto, per l'ultimo pixel deve aversi
        // (Image->width-1) / (LenX-1) * (LenX-1) = (Image->width-1)
  
        WsLength = WindowData [WndHandle].WSWndLength;
  
        for (CounterY=0, PosY=PosY1, PntToWsRow=WindowData[WndHandle]._ws + WsLength*PosY1; CounterY<LenY; CounterY++, PosY++, PntToWsRow+=WsLength)
        {
            IplPixelY=ScaleY*CounterY;
            PntToValue8Row = (unsigned char *)(Image->imageData + IplPixelY*Image->widthStep);
            
            for (CounterX=0, PosX=PosX1, PntToWs=PntToWsRow+PosX1; CounterX<LenX; CounterX++, PosX++, PntToWs++)
            {
                IplPixelX=ScaleX*CounterX;
                Value8B = *(unsigned char *)(PntToValue8Row + IplPixelX*3 );
                Value8G = *(unsigned char *)(PntToValue8Row + IplPixelX*3 + 1);
                Value8R = *(unsigned char *)(PntToValue8Row + IplPixelX*3 + 2);
                
                // Massima velocità possibile: il sistema accede direttamente al window space
                // della finestra, dopo avere ricavato il colore
                
                *PntToWs = ((Value8B & 0xF8)<<7) + ((Value8G & 0xF8)<<2) + ((Value8R & 0xF8)>>3) + 0x8000;
            }
        }
        
        if (RenderNow) XWindowRender (WndHandle);  
        return 0;       
    }
    else return -1; 
}  


void zcvCalculateAverageRGBChannels (IplImage *Image, int *AverageRed, int *AverageGreen, int *AverageBlue)
{
    register unsigned char   *PntToValue8Row; 
    register unsigned char   Value8R, Value8G, Value8B;
    
    int IplPixelX, IplPixelY;
    
    float TotalChannelR = 0;
    float TotalChannelG = 0;
    float TotalChannelB = 0;
    
    float Area = Image->width * Image->height;
    
    for (IplPixelY=0; IplPixelY<Image->height; IplPixelY++)
    {
        PntToValue8Row = (unsigned char *)(Image->imageData + IplPixelY*Image->widthStep);
        
        for (IplPixelX=0; IplPixelX<Image->width; IplPixelX++)
        {
            Value8B = *(unsigned char *)(PntToValue8Row + IplPixelX*3 );
            Value8G = *(unsigned char *)(PntToValue8Row + IplPixelX*3 + 1);
            Value8R = *(unsigned char *)(PntToValue8Row + IplPixelX*3 + 2);
            
            // Sommiamo i valori dei canali RGB 
            
            TotalChannelB += Value8B;
            TotalChannelG += Value8G;
            TotalChannelR += Value8R;    
        }
    }
    
    // Ok, adesso depositiamo le medie dei valori riportati dentro le variabili int di cui
    // è stato passato il puntatore
    
    *AverageBlue  = TotalChannelB/Area;
    *AverageGreen = TotalChannelG/Area;
    *AverageRed   = TotalChannelR/Area;
}

void zcvCalculateAverageRGBBrightness (IplImage *Image, float *Average)
{
    register unsigned char   *PntToValue8Row; 
    register unsigned char   Value8R, Value8G, Value8B;
    
    int IplPixelX, IplPixelY;
    
    float TotalBrightness = 0;
    
    float Area = Image->width * Image->height;
    
    for (IplPixelY=0; IplPixelY<Image->height; IplPixelY++)
    {
        PntToValue8Row = (unsigned char *)(Image->imageData + IplPixelY*Image->widthStep);
        
        for (IplPixelX=0; IplPixelX<Image->width; IplPixelX++)
        {
            Value8B = *(unsigned char *)(PntToValue8Row + IplPixelX*3 );
            Value8G = *(unsigned char *)(PntToValue8Row + IplPixelX*3 + 1);
            Value8R = *(unsigned char *)(PntToValue8Row + IplPixelX*3 + 2);
            
            // Sommiamo i valori dei canali RGB 
            
            TotalBrightness += 0.3*Value8R + 0.59*Value8G + 0.11*Value8B;
        }
    }
    
    // Ok, adesso depositiamo le medie dei valori riportati dentro le variabili int di cui
    // è stato passato il puntatore
    
    *Average = TotalBrightness/Area;                           
}

void zcvCopyRegion (IplImage *ImageSrc, IplImage *ImageDest, unsigned short int PosX1, unsigned short int PosY1, 
                                                      unsigned short int PosX2, unsigned short int PosY2)
/*
    Questa routine provvede a copiare una parte dell'immagine ImageSrc, precisamente
    la ROI individuata dalle coordinate PosX1, PosY1, PosX2, PosY2, nell'immagine
    di destinazione, adattandone le dimensioni.
*/
{
    int   CounterX, CounterY;
    int   ImgSrcX,  ImgSrcY;
    int   PixelX,   PixelY;
    
    float ScaledX, ScaledY;
    
    unsigned char Value;
    
    ImgSrcX = PosX2 - PosX1;
    ImgSrcY = PosY2 - PosY1;
    
    ScaledX = ( (float)(ImgSrcX) )/( (float)(ImageDest->width)  );
    ScaledY = ( (float)(ImgSrcY) )/( (float)(ImageDest->height) );
    
    for (CounterY=0; CounterY<ImageDest->height; CounterY++)
    {
        for (CounterX=0; CounterX<ImageDest->width; CounterX++)
        {
            PixelX = PosX1 + ScaledX*CounterX;
            PixelY = PosY1 + ScaledY*CounterY;
            
            *(unsigned char *)(ImageDest->imageData + CounterY*ImageDest->widthStep + CounterX) =
            *(unsigned char *)(ImageSrc->imageData + PixelY*ImageSrc->widthStep + PixelX);       
        }
    }
}                                                       

void zcvCopy (IplImage *ImageSrc, IplImage *ImageDest)
{
    zcvCopyRegion   (ImageSrc, ImageDest, 0, 0, ImageSrc->width, ImageSrc->height);            
}
















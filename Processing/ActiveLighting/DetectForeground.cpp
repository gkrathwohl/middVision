///////////////////////////////////////////////////////////////////////////////
//  NAME:
//      DetectForeground.cpp - detects foreground object if detection technique
//                             is used
//
//  USAGE:
//      activeLighting <mode> <params>
//
//
//  AUTHOR:
//      York Kitajima
//
///////////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "imageLib/imageLib.h"
#include "Utils.h"
#include "imageLib/flowIO.h"
#include "assert.h"
#include "string.h"
#define NOMATCH -9999

using namespace std;


void mergeMasks(char* outdir, char** imList1, int mergeThresh, int count) {
    printf("merging masks\n");
    CByteImage im;
    int w, h, verbose = 1;
    printf("count is %d\n", count);
	CByteImage mask, temp;
    printf("mergeThresh is %d\n",mergeThresh);
    int num = 0;
    for (int i =0; i < count; i++) {
		ReadImageVerb(im, imList1[i], verbose);
        if (i == 0){
	        CShape sh = im.Shape();
            w = sh.width, h = sh.height;
	        mask.ReAllocate(sh);
            temp.ReAllocate(sh);
        }
        printf("num is %d\n", num);
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (im.Pixel(x, y, 0) == 0)
                  //  printf("temp.pixel(%d, %d, 0) is %d.",x, y, temp.Pixel(x, y, 0)); 
                    temp.Pixel(x, y, 0) = temp.Pixel(x, y, 0)+1;
                    //printf("temp.pixel(%d, %d, 0) is %d.",x, y, temp.Pixel(x, y, 0)); }
            }
        }
        num++;
    }
    printf("temp.Pixel(1222, 1589, 0) is %d.\n", temp.Pixel(1222, 1589, 0));
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (temp.Pixel(x, y, 0) > mergeThresh)
                mask.Pixel(x, y, 0) = 0;
            else
                mask.Pixel(x, y, 0) = 255;
        }
    }
    char outname[1024];
    sprintf(outname, "%s/result.png", outdir);
    WriteImageVerb(mask, outname, verbose);
}


void createMask(char* outdir, int diffThresh, char** imList0, int numIm0) {
    CByteImage im0, im1, mask, maskGrey;
    CShape sh;
    int i, j, w, h, nB, count = 0, verbose = 1;
    //char** imList1;
    printf("numIm0 is %d\n", numIm0);
    for (i = 0; i < numIm0-1; i++) {
        for (j = i+1; j < numIm0; j++) {
            ReadImageVerb (im0, imList0[i], verbose);
            ReadImageVerb (im1, imList0[j], verbose);
            if (i == 0 && j == 1) {//create 1-band output image
                sh = im0.Shape();
                w = sh.width, h = sh.height, nB = sh.nBands;
                CShape sh1(w, h, nB);
                mask.ReAllocate(sh1);
                CShape sh2(w, h, 1);
                maskGrey.ReAllocate(sh2);
            }
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    for (int b = 0; b < nB-1; b++) {
                        int r0 = im0.Pixel(x, y, b);
                        int r1 = im1.Pixel(x, y, b);
                        if (abs(r0- r1) < diffThresh)
                            mask.Pixel(x, y, b) = 0;
                        else
                            mask.Pixel(x, y, b) = 255;
                    }	
                    if (mask.Pixel(x, y, 0) == 0 && mask.Pixel(x, y, 1) == 0 && mask.Pixel(x, y, 2) == 0)
                        maskGrey.Pixel(x, y, 0) = 0;
                    else
                        maskGrey.Pixel(x, y, 0) = 255;
                }
            }
            char outname[1024];
            sprintf(outname, "%s/mask%d_%d.png", outdir, i, j);
            WriteImageVerb(maskGrey, outname, verbose);
            //imList1[count] = &outname;
            printf("count is %d\n", count);
            printf("outname is %s\n", outname);
            count++;
        }
    }   
   // mergeMasks(outdir, imList1, count/2, count); 
}




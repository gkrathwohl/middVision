///////////////////////////////////////////////////////////////////////////
//
// NAME
//  GreyThresh.cpp -- read images taken by the camera and threshold them
//
// DESCRIPTION
//		reads first two images ("white" and "black" reference images) and
//		uses them to label pixel of subsequent images 255, 0, or 128 for unknown
//		the resulting images are saved as pgms by prepending 'T' to the filename
//		optionally subsamples images to half size before saving
//
//		Also works with single (gray or average) image used for thresholding
//
// Copyright � Daniel Scharstein, 2002.
//
///////////////////////////////////////////////////////////////////////////

#include "imageLib/imageLib.h"
#include "Utils.h"
#include <math.h>


// NEW WAY of thresholding using bit image and its inverse
// return a confidence map
// where 128..255 == white (the higher, the more certain)
//       127..0   == black (the lower, the more certain)
void threshold_image2(CByteImage im0, CByteImage im1, CByteImage imOut)
{
    CShape sh = im0.Shape();
    int x, y;
    int w = sh.width, h = sh.height, nB = sh.nBands;
    
    for (y = 0; y < h; y++) {
        uchar *i0 = &im0.Pixel(0, y, 0);
        uchar *i1 = &im1.Pixel(0, y, 0);
        uchar *out = &imOut.Pixel(0, y, 0);
        
        for (x = 0; x < w; x++) {
            int diff = 0;
            // sum up (signed) difference over all color bands
            for (int k=0; k<nB; k++) {
                diff += i0[k] - i1[k];
            }
            out[x] = __max(0, __min(255, 128.0 + diff / 4.0));
            i0 += nB;
            i1 += nB;
        }
    }
}




// NEW WAY OF THRESHOLDING:  have images of both code and its inverse
void greyThresh(char* outdir, char **imList, int numIm)
{
	CByteImage im0, im1, imThresh;
	CShape sh;
	int i, verbose=1;

	for (i = 0; i < numIm; i += 2) {
		ReadImageVerb(im0, imList[i], verbose);
		ReadImageVerb(im1, imList[i+1], verbose);
        if (i == 0) {
	        sh = im0.Shape();
        	// create 1-band output image
        	CShape sh1(sh.width, sh.height, 1);
            imThresh.ReAllocate(sh1);
        }
		if (sh != im0.Shape() || sh != im1.Shape())
			throw CError("greyThresh2: all images need to have same size and type");

		if (verbose)
			fprintf(stderr, "labeling confidence\n");
		threshold_image2(im0, im1, imThresh);

		// Create output filename and change the file extension to 'pgm'
		char outname[1024];
		sprintf(outname, "%s/thresh%d.pgm", outdir, i/2);
	    //char *dot = strrchr(outname, '.');
		//strcpy(dot, ".pgm");
		WriteImageVerb(imThresh, outname, verbose);
	}
}

///////////////////////////////////////////////////////////////////////////
// combine confidence maps

	
// imOut contains current most confident values; now consier new image imIn	
void updateConfidence(CByteImage imIn, CByteImage imOut, CByteImage map, int index)
{
	CShape sh = imOut.Shape();
    int x, y;
	int w = sh.width, h = sh.height;

    for (y = 0; y < h; y++) {
        uchar *in = &imIn.Pixel(0, y, 0);
        uchar *out = &imOut.Pixel(0, y, 0);

        for (x = 0; x < w; x++) {
			int inDist  =  in[x] > 127 ? 255 -  in[x] : in[x];  // distance from white or black
			int outDist = out[x] > 127 ? 255 - out[x] : out[x]; // distance from white or black
			if (inDist < outDist){	// if new image more certain than old
				out[x] = in[x];		// then update outImage
				map.Pixel(x,y,index%3) = 255;
				map.Pixel(x,y,(index+1)%3) = 0;
				map.Pixel(x,y,(index+2)%3) = 0;

			}

		}
	}
}

// simple threshold for now
// certain means how close value has to be to 0 or 255
// use certain = 30 for confidence based on distance to opposite color (b and w image)
// use certain = 120 for confidence based on average image
void thresholdConfidence(CByteImage imOut, int certain)
{
	CShape sh = imOut.Shape();
    int x, y;
	int w = sh.width, h = sh.height;

    for (y = 0; y < h; y++) {
        uchar *out = &imOut.Pixel(0, y, 0);
        for (x = 0; x < w; x++) {
			if (out[x] <= certain) 
				out[x] = 0;
			else if (out[x] >= 255-certain)
				out[x] = 255;
			else
				out[x] = 128;
		}
	}
}


// combine multiple confidence maps into one 	
void combineConfidence(char *outfile, char **imList, int numIm, int certain)
{
	CByteImage imIn, imOut, map;
	CShape sh;
	int i, verbose=1;

	// read first image into imOut
	ReadImageVerb(imOut, imList[0], verbose);
	sh = imOut.Shape();
	
	map.ReAllocate(CShape(sh.width, sh.height, 3));
	map.ClearPixels();
	for(i =0; i < sh.width; i++){
		for(int j =0; j < sh.height; j++){
			map.Pixel(i,j,0) = 255;
		}
	}

	// read subsequent images and update confidence map
	for (i = 1; i < numIm; i++) {
		ReadImageVerb(imIn, imList[i], verbose);
		if (sh != imIn.Shape())
			throw CError("combineConfidence: all images need to have same size and type");
		updateConfidence(imIn, imOut, map, i);
	}

	thresholdConfidence(imOut, certain);
	//WriteImageVerb(map, "map.ppm", verbose);
	WriteImageVerb(imOut, outfile, verbose);
}






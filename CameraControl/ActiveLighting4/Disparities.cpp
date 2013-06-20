///////////////////////////////////////////////////////////////////////////
//
// NAME
//  Disparities.cpp -- compute disparity maps
//
// DESCRIPTION
//  Compute pair of disparity maps from pair of labeled (flo) images.
//  Main functions:
//  1. computes initial disparity maps from pair of code images
//  2. combine arbitrary numbers of disparity maps
//  3. fills holes and cross-checks pair of disparity maps
//  (SolveProjection is then used to create disparity maps from illumination)
//
// Copyright � Daniel Scharstein, 2011.
//
///////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "imageLib.h"
#include "Utils.h"
#include "flowIO.h"
#include "assert.h"

CFloatImage simpleCrossCheck(CFloatImage im0, CFloatImage im1){

	CFloatImage out(im0.Shape());


	for(int i =0; i < im0.Shape().width; i++){
		for(int j =0; j < im0.Shape().height; j++){

			float targeti = i+im0.Pixel(i,j,0);
			float targetj = j+im0.Pixel(i,j,1);

			float resolvedi = targeti+im1.Pixel(targeti,targetj,0);
			float resolvedj = targetj+im1.Pixel(targeti,targetj,1);

			float dx = i-resolvedi;
			float dy = j-resolvedj;
			float dist = dx*dx+dy*dy;


			if(dist >= 2){
				out.Pixel(i,j,0) = 0;
				out.Pixel(i,j,1) = 0;
			}else{
				out.Pixel(i,j,0) = im0.Pixel(i,j,0);
				out.Pixel(i,j,1) = im0.Pixel(i,j,1);
			}

		}
	}

	return out;
}

void matchImages(CFloatImage fim0, CFloatImage fim1, CFloatImage dim, int direction, int dmin, int dmax, int ymin, int ymax)
{

	CShape sh = fim0.Shape();
	int w = sh.width, h = sh.height;

	int nChannels = 2;

	// maximal allowable code difference:
	float maxdiff = 1;

	// encode unknown disparity as 0
	int nomatch = 0;

	int min = 2*dmin*direction;
	int max = 2*dmax*direction;
	int step = 2*direction;



	for(int j = 0; j < h; j++){
		float* row = &fim0.Pixel(0,j,0);
		float* disparityRow = &dim.Pixel(0,j,0);
		if (j % 100 == 0) printf(".");
		fflush(stdout);
			for(int i = 0; i < 2*w; i+= 2){

			float valx = row[i];
			float valy = row[i+1];

			if (valx < 0 || valy < 0) {
				disparityRow[i] = nomatch;
				disparityRow[i+1] = nomatch;
				continue;
			}

			int bestx = dmin;
			int besty = ymin;
			float bestdiff = 999999;


			for(int y = ymin; y <= ymax; y++){
				float* targetrow = &fim1.Pixel(0, j+y, 0);

			for(int x = min; x != max; x+= step){


					if(i+x < 0 || i+x == 2*w || j+y < 0 || j+y > h){
						continue;
					}

					float valx1 = targetrow[i+x];
					float valy1 = targetrow[i+x+1];


					float dx = valx - valx1;
					float dy = valy - valy1;
					float diff = dx*dx+dy*dy;

					if (diff < bestdiff) {
						bestdiff = diff;
						bestx = x >> 1;
						besty = y;
					}

				}
			}


			if (bestdiff <= maxdiff){
				disparityRow[i] = bestx;
				disparityRow[i+1] = besty;
			}else{
				disparityRow[i] = nomatch;
				disparityRow[i+1] = nomatch;
			}
		}
	}
	printf("\n");

}



// step 1: compute pair of disparity maps from labeled (flo) images
void computeDisparities(char *outdir,  char *in0, char *in1, int dXmin, int dXmax, int dYmin, int dYmax)
{
	CFloatImage fim0, fim1;
	int verbose=1;

	int dmin = dXmin;
	int dmax = dXmax;

	ReadFlowFile(fim0, in0);
	ReadFlowFile(fim1, in1);

	if (fim0.Shape() != fim1.Shape())
		throw CError("computeDisparities: all images need to have same size");

	CFloatImage d0(fim0.Shape());
	CFloatImage d1(fim0.Shape());
	d0.ClearPixels();
	d1.ClearPixels();


	matchImages(fim0, fim1, d0, -1, dmin, dmax, dYmin, dYmax);
	matchImages(fim1, fim0, d1, 1, dmin, dmax, dYmin, dYmax);

	char filename[1024];


	sprintf(filename, "%s/out0.flo",outdir);
	WriteFlowFile(d0, filename);
	sprintf(filename, "%s/out1.flo",outdir);
	WriteFlowFile(d1, filename);

	CFloatImage crossed0 = simpleCrossCheck(d0,d1);
	CFloatImage crossed1 = simpleCrossCheck(d1,d0);

	sprintf(filename, "%s/Xout0.flo",outdir);
	WriteFlowFile(crossed0, filename);
	sprintf(filename, "%s/Xout1.flo",outdir);
	WriteFlowFile(crossed1, filename);

	if(1){
		//Compute greyscale disparity maps, for checking
		CByteImage disp0(fim0.Shape().width, fim0.Shape().height, 1);
		CByteImage disp1(fim0.Shape().width, fim0.Shape().height, 1);


		float scale = 255.0 / (dmax - dmin);
		float offs = - scale * dmin;

		CFloatImage d0s = splitFloImage(d0).first;
		CFloatImage d1s = splitFloImage(d1).first;

		ScaleAndOffset(d0s, disp0, scale, offs);
		ScaleAndOffset(d1s, disp1, scale, offs);



		sprintf(filename, "%s/out0.pgm",outdir);
		WriteImageVerb(disp0, filename, verbose);

		sprintf(filename, "%s/out1.pgm",outdir);
		WriteImageVerb(disp1, filename, verbose);
	}
}


void mergeDisparityMaps(char** filenames, int count){


	if(count == 2){
		printf("Taking first map as primary and filling in holes...\n");

		CFloatImage primary, secondary;

		ReadFlowFile(primary, filenames[0]);
		ReadFlowFile(secondary, filenames[1]);

		for(int j =0; j < primary.Shape().height; j++){
			for(int i =0; i < primary.Shape().width; i++){

				if(primary.Pixel(i,j,0) == 0){
					primary.Pixel(i,j,0) = secondary.Pixel(i,j,0);
				}

				if(primary.Pixel(i,j,1) == 0){
					primary.Pixel(i,j,1) = secondary.Pixel(i,j,1);
				}


			}
		}

		WriteFlowFile(primary, "mergedDisps.flo");






	}







}








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
#include "imageLib/imageLib.h"
#include "Utils.h"
#include "imageLib/flowIO.h"
#include "assert.h"
#define NOMATCH -9999

CFloatImage simpleCrossCheck(CFloatImage im0, CFloatImage im1){

	CFloatImage out(im0.Shape());
	CShape sh = im0.Shape();
	int w = sh.width, h= sh.height;

	for(int i =0; i < w; i++){
		for(int j =0; j < h; j++){

			if(im0.Pixel(i,j,0) == NOMATCH || im0.Pixel(i,j,1) == NOMATCH){
				out.Pixel(i,j,0) = NOMATCH;
				out.Pixel(i,j,1) = NOMATCH;
				continue;
			}

			float targeti = i+im0.Pixel(i,j,0);
			float targetj = j+im0.Pixel(i,j,1);

			if(targeti >= w || targeti < 0 || targetj >= h || targetj < 0){
				out.Pixel(i,j,0) = NOMATCH;
				out.Pixel(i,j,1) = NOMATCH;
				continue;
			}

			if(im1.Pixel(targeti,targetj,0) == NOMATCH || im1.Pixel(targeti,targetj,1) == NOMATCH){
				out.Pixel(i,j,0) = NOMATCH;
				out.Pixel(i,j,1) = NOMATCH;
				continue;
			}

			float resolvedi = targeti+im1.Pixel(targeti,targetj,0);
			float resolvedj = targetj+im1.Pixel(targeti,targetj,1);

			float dx = i-resolvedi;
			float dy = j-resolvedj;
			float dist = dx*dx+dy*dy;


			if(dist >= 2){
				out.Pixel(i,j,0) = NOMATCH;
				out.Pixel(i,j,1) = NOMATCH;
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

	//int nChannels = 2;

	// maximal allowable code difference:
	float maxdiff = 1;

	// encode unknown disparity as 0


	int min = 2*dmin*direction;
	int max = 2*dmax*direction;
	int step = 2*direction;

	int ymin2 = ymin*direction;
	int ymax2 = ymax*direction;
	int ystep = direction;

	int good = 0;

	for(int j = 0; j < h; j++){
		float* row = &fim0.Pixel(0,j,0);
		float* disparityRow = &dim.Pixel(0,j,0);
		if (j % 100 == 0) printf(":) ");
		fflush(stdout);
		for(int i = 0; i < 2*w; i+= 2){

			float valx = row[i];
			float valy = row[i+1];

			if (valx < 0 || valy < 0) {
				disparityRow[i] = NOMATCH;
				disparityRow[i+1] = NOMATCH;
				continue;
			}

			int bestx = dmin;
			int besty = ymin;
			float bestdiff = 999999;


			for(int y = ymin2; y != ymax2; y += ystep){
				float* targetrow = &fim1.Pixel(0, j+y, 0);

				for(int x = min; x != max; x+= step){

					//old : if(i+x < 0 || i+x == 2*w || j+y < 0 || j+y > h){
					if(i+x < 0 || i+x >= 2*w-1 || j+y < 0 || j+y >= h){
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
				good++;
			}else{
				disparityRow[i] = NOMATCH;
				disparityRow[i+1] = NOMATCH;
			}
		}
	}

	printf("found %d matches (maxdiff=%f)\n", good, maxdiff);
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

	char filename[1024];
	matchImages(fim0, fim1, d0, -1, dmin, dmax, dYmin, dYmax);
	sprintf(filename, "%s/out0.flo",outdir);
	WriteFlowFile(d0, filename);


	matchImages(fim1, fim0, d1, 1, dmin, dmax, dYmin, dYmax);
	sprintf(filename, "%s/out1.flo",outdir);
	WriteFlowFile(d1, filename);

	CFloatImage crossed0 = simpleCrossCheck(d0,d1);
	CFloatImage crossed1 = simpleCrossCheck(d1,d0);

	sprintf(filename, "%s/Xout0.flo",outdir);
	WriteFlowFile(crossed0, filename);
	sprintf(filename, "%s/Xout1.flo",outdir);
	WriteFlowFile(crossed1, filename);
//	puts("hi");

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


void mergeDisparityMaps(char* output, char** filenames, int count, int mingroup){



	CFloatImage images[count];

	for(int i =0; i < count; i++){
		ReadFlowFile(images[i], filenames[i]);
	}

	CFloatImage out;
	CShape sh = images[0].Shape();
	out.ReAllocate(sh);

	int maxdiff = 2;

	for(int j =0; j < sh.height; j++){
		if(j % 100 == 0){
			printf(".");
			fflush(stdout);
		}

		float* row[count];
		for(int k =0; k < count; k++){
			row[k] = &images[k].Pixel(0,j,0);
		}

		float* outrow = &out.Pixel(0,j,0);

		for(int i =0; i < sh.width; i++){

			float newvalx = 0, newvaly = 0;
			int countx = 0, county = 0;
			int x = i*2;
			int y = x+1;
			for(int k =0; k < count; k++){

				if(row[k][x] != NOMATCH ){
					newvalx += row[k][x];
					countx++;
				}

				if(row[k][y] != NOMATCH  ){
					newvaly += row[k][y];
					county++;
				}

			}
			if(countx < mingroup){
				outrow[x] = NOMATCH;
			}else{
				newvalx /= countx;
				outrow[x] = newvalx;
			}

			if(county < mingroup){
				outrow[y] = NOMATCH;
			}else{
				newvaly /= county;
				outrow[y] = newvaly;
			}




			for(int k =0; k < count; k++){
				if(row[k][x] != NOMATCH  && abs(row[k][x] - newvalx) > maxdiff){
					vector<int> pixels;
					for(int z =0; z < count; z++){
						if(row[z][x] != NOMATCH){
							pixels.push_back(row[z][x]);
						}

					}
					if((int)pixels.size() < mingroup){
						outrow[x] = NOMATCH;

					}else{
						outrow[x] = robustAverage(pixels, maxdiff, mingroup);
						if(0){
							printf("values were: ");
							for(int i = 0; i < (int)pixels.size(); i++){
								printf(" %d",pixels[i]);
							}
							printf("\n");
							printf("average was: %f\n\n",outrow[x]);
						}
					}
					break;
				}
			}
		}

	}
	printf("\n");
	WriteFlowFile(out, output);




}








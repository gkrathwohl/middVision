/*
 * ColorSpiral.cpp
 *
 *  Created on: Jun 20, 2011
 *      Author: wwestlin
 */


#include "imageLib/imageLib.h"
#include <stdio.h>
#include "flowIO.h"
#include "assert.h"
#include <cmath>
#include <algorithm>
#define NOMATCH -9999

using namespace std;


void HSVtoRGB(float H, float S, float V, uchar* R, uchar* G, uchar* B);

pair<CFloatImage, CFloatImage> splitFloImage(CFloatImage &merged){


	CFloatImage x, y;
	CShape sh = CShape(merged.Shape().width, merged.Shape().height, 1);
	x.ReAllocate(sh, true);
	x.ClearPixels();
	y.ReAllocate(sh);
	y.ClearPixels();


	for(int i = 0 ; i < sh.width; i++){
		for(int j = 0; j < sh.height; j++){
			x.Pixel(i,j,0) = merged.Pixel(i,j,0);
			y.Pixel(i,j,0) = merged.Pixel(i,j,1);
		}
	}

	return pair<CFloatImage, CFloatImage>(x,y);

}

int main(int argc, char** argv){


	if(argc < 3){
		printf("Usage: ColorSpiral input.flo output.png <optional: outputY.png>\n Options: -s <SPEED>  -g <GREYSCALE> -x <GREYMAX> -i <GREYMIN>\n");
		exit(0);
	}


	float max = 0;
	float min = 9999;

	int argIndex = 1;
	int speed = 5;
	int grey = 0;
	int reverse = 0;

	int computemax = 1;
	int computemin = 1;

	while(argv[argIndex][0] == '-'){
		if(argv[argIndex][1] == 's'){
			argIndex++;
			speed = atoi(argv[argIndex]);
			argIndex++;
		}else if(argv[argIndex][1] == 'g'){
			grey = 1;
			argIndex++;
		}else if(argv[argIndex][1] == 'x'){
			argIndex++;
			max = atof(argv[argIndex]);
			computemax = 0;
			argIndex++;
		}else if(argv[argIndex][1] == 'i'){
			argIndex++;
			min = atof(argv[argIndex]);
			computemin = 0;
			argIndex++;
		}else if(argv[argIndex][1] == 'r'){
			reverse = 1;
			argIndex++;
		}
	}




	if(grey){
		printf("Visualizing in greyscale\n");
	}else{
		printf("Visualizing with speed %d \n",speed);
	}

	CFloatImage input;

	ReadFlowFile(input, argv[argIndex++]);

	CShape sh = CShape(input.Shape().width, input.Shape().height, 3);
	if(computemax || computemin){
		for(int i = 0; i < sh.width; i++){
			for(int j = 0; j < sh.height; j++){

				float val;

				val = input.Pixel(i,j,0);



				if(val > max && computemax){
					max = val;
				}

				if(val < min && computemin){
					min = val;
				}


			}
		}

	}

	if(grey){


		CByteImage disp0(sh.width, sh.height, 1);
		CByteImage disp1(sh.width, sh.height, 1);

		float scale = 255.0 / (max - min);
		float offs = - scale * min;
		if(reverse == 1){
			for(int i = 0; i < sh.width; i++){
				for(int j = 0; j < sh.height; j++){
					if(input.Pixel(i,j,0) != NOMATCH)
						input.Pixel(i,j,0) = -input.Pixel(i,j,0);
					if(input.Pixel(i,j,1) != NOMATCH)
						input.Pixel(i,j,1) = -input.Pixel(i,j,1);

				}
			}
		}

		pair<CFloatImage,CFloatImage> split = splitFloImage(input);
		CFloatImage d0s = split.first;
		CFloatImage d1s = split.second;

		ScaleAndOffset(d0s, disp0, scale, offs);
		ScaleAndOffset(d1s, disp1, scale, offs);

		for(int i = 0; i < sh.width; i++){
			for(int j = 0; j < sh.height; j++){

				float valx, valy;

					valx = input.Pixel(i,j,0);


					valy = input.Pixel(i,j,1);

				if(valx == NOMATCH){
					disp0.Pixel(i,j,0) = 0;

				}
				if(valy == NOMATCH){
					disp1.Pixel(i,j,0) = 0;
				}


			}
		}

		WriteImageVerb(disp0, argv[argIndex++], 1);
		if(argIndex < argc)
			WriteImageVerb(disp1, argv[argIndex++], 1);


	}else{


		CByteImage output, outputY;
		output.ReAllocate(sh);
		output.ClearPixels();
		outputY.ReAllocate(sh);
		outputY.ClearPixels();

		for(int i = 0; i < sh.width; i++){
			for(int j = 0; j < sh.height; j++){

				float valx, valy;

					valx = input.Pixel(i,j,0);


					valy = input.Pixel(i,j,1);


					if(valx != NOMATCH){

						//brightness scales from 0.25 to 0.75 across image
						float bright = ((valx -min)/(max-min))*0.9+0.1; 
						bright = std::min(bright, (float)0.75);
						bright = std::max(bright, (float)0.25);
						
						float hue = fmod(valx,speed)/speed;

						if(hue < 0){
							hue += 1;
						}

						if(hue == 1){
							hue -= 0.001;
						}

						uchar r,g,b;

						HSVtoRGB(hue,bright,1,&r,&g,&b);

						int rr = r;
						int gg = g;
						int bb = b;

						output.Pixel(i,j,0) = b;  
						output.Pixel(i,j,1) = g; 
						output.Pixel(i,j,2) = r;   
					}

					if(valy != NOMATCH){

						speed = 5;

						//brightness scales from 0.25 to 0.75 across image
						float bright = ((valy -min)/(max-min))*0.9+0.1; 
						bright = std::min(bright, (float)0.75);
						bright = std::max(bright, (float)0.25);

						float hue = fmod(valy,speed)/speed;

						if(hue < 0){
							hue += 1;
						}

						if(hue == 1){
							hue -= 0.001;
						}

						uchar r,g,b;

						
						HSVtoRGB(hue,bright,1,&r,&g,&b);
						int rr = r;
						int gg = g;
						int bb = b;

						//if (valy == 0) {
							//printf("rgb: %d,%d, %d \n", rr, gg, bb);
						//}		 

						//BGR, not RGB
						outputY.Pixel(i,j,0) = b;
						outputY.Pixel(i,j,1) = g;
						outputY.Pixel(i,j,2) = r;

					}




			}
		}

		WriteImageVerb(output, argv[argIndex++], 1);
		if(argIndex < argc)
			WriteImageVerb(outputY, argv[argIndex++], 1);
	}


}
void HSVtoRGB(float H, float S, float V, uchar* R, uchar* G, uchar* B){

	H = H*360;

	float chroma = V*S;

	float Hprime = H/60;

	float x = chroma*(1-fabs(fmod(Hprime,2)-1));

	int sextant = floor(Hprime);

	float r,g,b;

	switch(sextant){

	case 0:
		r = chroma;
		g = x;
		b = 0;
		break;
	case 1:
		r = x;
		g = chroma;
		b = 0;
		break;
	case 2:
		r = 0;
		g = chroma;
		b = x;
		break;
	case 3:
		r = 0;
		g = x;
		b = chroma;
		break;
	case 4:
		r =  x;
		g =  0;
		b = chroma;
		break;
	case 5:
		r =  chroma;
		g =  0;
		b = x;
		break;
	default:
		assert(false);
	}


	float m = V -chroma;

	r += m;
	g += m;
	b += m;

	*R = (r*255);
	*B = (b*255);
	*G = (g*255);


}



/*
 * mergeFlo.cpp
 *
 *  Created on: Jun 27, 2013
 *  Author: Greg Krathwohl
 */


#include "imageLib/imageLib.h"
#include <stdio.h>
#include "flowIO.h"
#include "assert.h"
#include <cmath>
#include <algorithm>
#define NOMATCH -9999

using namespace std;

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
		printf("Usage: mergeFlo input1.flo input2.flo output.flo <\n");
		exit(0);
	}

	int argIndex = 1;

	CFloatImage input1, input2, output;
	ReadFlowFile(input1, argv[argIndex++]);
	ReadFlowFile(input2, argv[argIndex++]);

	CShape sh = CShape(input1.Shape().width, input1.Shape().height, 3);
	output.ReAllocate(sh);
	output.ClearPixels();
	
	for(int i = 0; i < sh.width; i++){
		for(int j = 0; j < sh.height; j++){
			output.Pixel(i,j,0) = min(input1.Pixel(i,j,0), input2.Pixel(i,j,0));
		}
	}


	WriteFlowFile(output, argv[argIndex++]);

}

	

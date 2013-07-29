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

int main(int argc, char** argv){

	if(argc < 3){
		printf("Usage: mergeFlo input1.flo input2.flo output.flo <\n");
		exit(0);
	}

	int argIndex = 1;

	CFloatImage input1, input2, output;
	ReadFlowFile(input1, argv[argIndex++]);
	ReadFlowFile(input2, argv[argIndex++]);

	CShape sh = CShape(input.Shape().width, input.Shape().height, 3);
	output.ReAllocate(sh);
	output.ClearPixels();
	
	for(int i = 0; i < sh.width; i++){
		for(int j = 0; j < sh.height; j++){
			output.Pixel(i,j,0) = min(input1.Pixel(i,j,0), input2.Pixel(i,j,0));
		}
	}


	WriteImageVerb(output, argv[argIndex++], 1);

}

	

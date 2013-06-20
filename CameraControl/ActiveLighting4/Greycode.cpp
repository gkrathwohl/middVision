//Greycode.cpp: generates greycode patterns

#include "GUI.h"
#include "Utils.h"
#include "Greycode.h"


// create inverse pattern if 1
// create vertical pattern if 1
void reflectiveBinaryCode (unsigned int* arr, int inv, int vert, int bit)
{
    // pick the colors
	unsigned int c0 = (!inv) ? 0xff000000 : 0xffffffff; // inv -> black, !inv -> white
	unsigned int c1 = (!inv) ? 0xffffffff : 0xff000000; // inv -> white, !inv -> black

	// populate the image array:
	// 2^(9-m) bars, vertical if vert == true, else horizontal
	int n = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int b; // mth bit of x or y's Gray code value
			if (!vert) {
				b = greybit(y, bit);
			} else {
				b = greybit(x, bit);
			}
			arr[n++] = b ? c1 : c0; // draw with white or black
		}
	}


}


// create inverse pattern if 1
// create vertical pattern if 1
void minStripeWidthCode (unsigned int* arr , int inv, int vert, int bit)
{
	unsigned int c0 = (!inv) ? 0xff000000 : 0xffffffff; // inv -> black, !inv -> white
	unsigned int c1 = (!inv) ? 0xffffffff : 0xff000000; // inv -> white, !inv -> black

	
	BYTE* code = minSW[bit];
	unsigned int c;
	for(int i =0; i < width; i++)
	{

		if(code[i] == 0)
		{
			c = c1;
		}
		else
		{
			c = c0;
		}
		for(int j =0; j < width; j++)
		{   
			if (!vert)
				arr [i*width + j] = c;
			else
				arr [j*width + i] = c;
		}
	}
}





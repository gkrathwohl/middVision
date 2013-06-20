
#include "GUI.h"
#include "OtherCode.h"
#include <Windows.h>
#include <gdiplus.h>

using namespace Gdiplus;


void createOtherPattern (unsigned int* arr , int inv, int vert, int bit)
{
	
	//unsigned int* arr = new unsigned int[width*height];
	// pick the colors
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
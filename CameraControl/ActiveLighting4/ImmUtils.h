///////////////////////////////////////////////////////////////////////////
//
// NAME
//  Utils.h -- utility functions associated with active lighting project
//
// SEE ALSO
//  Utils.cpp             Implementation
//
// Copyright � Daniel Scharstein, 2002.
//
///////////////////////////////////////////////////////////////////////////

#include "imageLib.h"
#define ABS(x) ((x) >= 0 ? (x) : (-(x)))

typedef unsigned char uchar;

// Grey code functions

// encodes n to grey code
unsigned int greycodeOld(unsigned int n);

// decodes n from grey code
unsigned int invgreycodeOld(unsigned int n);

// returns i-th bit of grey coded number n
unsigned int greybit(unsigned int n, int i);

// test function to print grey codes
void testgreycode();


// color encoding

// map f from 0.0 .. 1.0 to hue (Red-Yellow-Green-Cyan-Blue-Magenta)
void hue(float f, uchar *rgb);

// given value f = -1..1, adjusts rgb values to black for f=-1 and white for f=1
void adjust_brightness(float f, uchar *rgb);

// map f from 0.0 .. 1.0 to spiral in Hue-Brightness space 
void hueshade(float f, uchar *rgb);



// filtering

// sort small array of uchars using bubble sort
void bsort(uchar *n, int k);

// return median of a small array of uchars by sorting it using bubble sort
uchar median(uchar *n, int k);

// apply median filter.  use k=5 for 4-connected neighbors, k=9 for full 3x3 neighborhood
void median_filter(CByteImage src, CByteImage &dst, int k);

// line and plane  fit

// line fit to int array:  val(x) ~ ax + b, where x = 0..n-1
// if unk != 0,  ignore values with label unk
void fitLine(int* val, int n, int stride, float &a, float &b, int unk);

// plane fit to int array:  val(x,y) ~ ax + by + c, where 
// x = 0..nx-1 in steps of stepx, y = 0..ny-1 in steps of stepy
// if unk != 0,  ignore values with label unk
void fitPlane(int* val, int nx, int ny, int stepx, int stepy, int stridex, int stridey,
			 float &a, float &b, float &c, int unk);


// miscellaneous

// safe parsing of integer argument
int atoiSafe(char *s);

//Combine 2 single channel float image into one .flo image
CFloatImage mergeToFloImage(CFloatImage &x, CFloatImage &y);

pair<CFloatImage,CFloatImage> splitFloImage(CFloatImage &merged);

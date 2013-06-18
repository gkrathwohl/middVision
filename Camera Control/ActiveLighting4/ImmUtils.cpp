///////////////////////////////////////////////////////////////////////////
//
// NAME
//  Utils.cpp -- utility functions associated with active lighting project
//
//
// Copyright � Daniel Scharstein, 2002.
//
///////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "imageLib.h"
#include <utility>
#include <stdarg.h>
#include <vector>

// Grey code functions

// encodes n to grey code
unsigned int greycodeOld(unsigned int n) {
    return  n ^ (n >> 1);
}

// decodes n from grey code
unsigned int invgreycodeOld(unsigned int n) {
	unsigned int r = 0;
	for (; n != 0; n >>= 1)
		r ^= n;
    return  r;
}

// returns i-th bit of grey coded number n
unsigned int greybit(unsigned int n, int i) {
    unsigned int g = greycodeOld(n);
    return  1 & (g >> i);
}

// test function to print grey codes
void testgreycode() {
    for (int n=0; n < 35; n++) {
        int g = greycodeOld(n);
	    int g2 = invgreycodeOld(g);

        printf("%3d  %3d  %3d   ", n, g, g2);
        for (int j=9; j>=0; j--)
            printf("%d ", greybit(n, j));

        printf("\n");
    }
}


///////////////////////////////////////////////////////////////////////////
// color encoding

// map f from 0.0 .. 1.0 to hue (Red-Yellow-Green-Cyan-Blue-Magenta)
void hue(float f, uchar *rgb) {
	float r=0.0, g=0.0, b=0.0;
	f *=6.0;
	int f0 = (int)f;
	f -= f0;
	switch (f0) {
	case 0:  r=1.0;   g=f; break;
	case 1:  r=1.0-f; g=1.0; break;
	case 2:  g=1.0;   b=f; break;
	case 3:  g=1.0-f; b=1.0; break;
	case 4:  b=1.0;   r=f; break;
	default: b=1.0-f; r=1.0; break;
	}
	rgb[0] = r*255;
	rgb[1] = g*255;
	rgb[2] = b*255;
}

// given value f = -1..1, adjusts rgb values to black for f=-1 and white for f=1
void adjust_brightness(float f, uchar *rgb) {
	if (f < 0) {
		for (int i=0; i<3; i++)
			rgb[i] *= (f+1);
	} else {
		for (int i=0; i<3; i++)
			rgb[i] = (1-f)*rgb[i] + f*255;
	}
}

// map f from 0.0 .. 1.0 to spiral in Hue-Brightness space 
void hueshade(float f, uchar *rgb) {
	float rounds = 64.0; // how many spirals
	float f1 = f*rounds;
	f1 = f1 - (int)f1;
	hue(f1, rgb);
	f = 2*f-1;
	adjust_brightness(f, rgb);
}


///////////////////////////////////////////////////////////////////////////
// filtering

// sort small array of uchars using bubble sort
void bsort(uchar *n, int k) {
	for (int i=k-1; i>0; i--) {
		for (int j=0; j<i; j++) {
			if (n[j] > n[j+1]) {
				uchar t = n[j];
				n[j] = n[j+1];
				n[j+1] = t;
			}
		}
	}
}

// return median of a small array of uchars by sorting it using bubble sort
uchar median(uchar *n, int k) {
	bsort(n, k);
	return n[k/2];
}

// apply median filter.  use k=5 for 4-connected neighbors, k=9 for full 3x3 neighborhood
void median_filter(CByteImage src, CByteImage &dst, int k)
{
    CShape sh = src.Shape();
    int x, y, w = sh.width, h = sh.height;

	dst.ReAllocate(sh, false);

    for (y = 0; y < h; y++) {
        uchar *pm = &src.Pixel(0, y-1, 0);
        uchar *p = &src.Pixel(0, y, 0);
        uchar *pp = &src.Pixel(0, y+1, 0);
        uchar *res = &dst.Pixel(0, y, 0);

		if (y > 0 && y < h-1) {
	        for (x = 1; x < w-1; x++) {
		        uchar n[9] = {pm[x], p[x-1], p[x], p[x+1], pp[x],   // cross
							  pm[x-1], pm[x+1],pp[x-1],pp[x+1]};	// diagonal
				res[x] = median(n, k);
			}
			res[0] = p[0];
			res[w-1] = p[w-1];
		} else {
	        for (x = 0; x < w; x++) {
				res[x] = p[x];
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////
// line and plane fit

// line fit to int array:  val(x) ~ ax + b, where x = 0..n-1
// if unk != 0,  ignore values with label unk
void fitLine(int* val, int n, int stride, float &a, float &b, int unk)
{
	int x;
	int s1=0;
	float sx=0, sy=0, sxx=0, sxy=0;

	for (x = 0; x < n; x++, val += stride) {
		int y = *val;
		if (y == unk)
			continue;
		s1++;
		sx += x;
		sy += y;
		sxx += x * x;
		sxy += x * y;
	}
	float det = 1.0 / (s1 * sxx - sx * sx);
	a = det * ( s1 * sxy - sx * sy);
	b = det * (-sx * sxy + sxx * sy);
}

// plane fit to int array:  val(x,y) ~ ax + by + c, where 
// x = 0..nx-1 in steps of stepx, y = 0..ny-1 in steps of stepy
// if unk != 0,  ignore values with label unk
void fitPlane(int* val, int nx, int ny, int stepx, int stepy, int stridex, int stridey,
			 float &a, float &b, float &c, int unk)
{
	int x, y;
	int *p;
	int s1=0;
	float sx=0, sy=0, sz=0, sxx=0, sxy=0, sxz=0, syy=0, syz=0;

	stridex *= stepx;
	stridey *= stepy;

	for (y = 0; y < ny; y += stepy, val += stridey) {
		for (x = 0, p = val; x < nx; x += stepx, p += stridex) {
			int z = *p;
			if (z == unk && unk != 0)
				continue;
			s1++;
			sx += x;
			sy += y;
			sz += z;
			sxx += x * x;
			sxy += x * y;
			sxz += x * z;
			syy += y * y;
			syz += y * z;
		}
	}
	float det = 1.0 / (sxx*syy*s1-sxx*sy*sy-sxy*sxy*s1+2.0*sxy*sx*sy-sx*sx*syy);
	a = det * ( (syy*s1-sy*sy)*sxz+(-sxy*s1+sx*sy)*syz+(sxy*sy-sx*syy)*sz );
	b = det * ( (-sxy*s1+sx*sy)*sxz+(sxx*s1-sx*sx)*syz+(-sxx*sy+sxy*sx)*sz );
	c = det * ( (sxy*sy-sx*syy)*sxz+(-sxx*sy+sxy*sx)*syz+(sxx*syy-sxy*sxy)*sz );
}

/* I generated the above equations using the following Maple program:

restart; with(linalg);
A := matrix( 3, 3, [sxx, sxy, sx, sxy, syy, sy, sx, sy, s1]);
d := det(A);
B := evalm(d*inverse(A));
z := matrix(3, 1, [sxz, syz, sz]);
r := evalm(B&*z);
readlib(C);
C(d);
C(r);
*/



///////////////////////////////////////////////////////////////////////////
// miscellaneous

// make sure a command-line argument is a non-negative integer
static void AssertIntString(char *s) {
	for (int k=0; s[k]; k++) {
		if (*s < '0' || *s > '9')
			throw CError("Positive integer expected: '%s'", s);
	}
}

// safe parsing of integer argument
int atoiSafe(char *s) {
	AssertIntString(s);
	return atoi(s);
}





// utility for adding a black frame around a pgm image
// has nothing to do with grey decode, but was used to
// create pictures for symcost paper
void addFrame(CByteImage result) {
	CShape sh = result.Shape();
	int x, y, w = sh.width, h = sh.height;
	
	for (y = 0; y < h; y++) {
		uchar *r = &result.Pixel(0, y, 0);
		
		for (x = 0; x < w; x++) {
			if (x==0 || x==w-1 || y==0 || y==h-1)
				r[x] = 0;
		}
	}
}



CFloatImage mergeToFloImage(CFloatImage &x, CFloatImage &y){


	CFloatImage merged;
	CShape sh = CShape(x.Shape().width, x.Shape().height, 2);

	merged.ReAllocate(sh);
	merged.ClearPixels();

	for(int i = 0 ; i < sh.width; i++){
		for(int j = 0; j < sh.height; j++){
			merged.Pixel(i,j,0) = x.Pixel(i,j,0);
			merged.Pixel(i,j,1) = y.Pixel(i,j,0);
		}
	}


	return merged;

}

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


CFloatImage mergeToNBandImage(vector<CFloatImage*> imgs){



	CFloatImage merged;
	CShape sh = CShape(imgs[0]->Shape().width, imgs[0]->Shape().height, imgs.size());

	merged.ReAllocate(sh);
	merged.ClearPixels();

	for(int i = 0 ; i < sh.width; i++){
		for(int j = 0; j < sh.height; j++){
			for(int k = 0 ; k < sh.nBands; k++){
				merged.Pixel(i,j,k) = imgs[k]->Pixel(i,j,0);
			}
		}
	}


	return merged;

}

vector<CFloatImage> splitNBandImage(CFloatImage &merged){



	CShape sh = CShape(merged.Shape().width, merged.Shape().height, 1);
	int n = merged.Shape().nBands;

	vector<CFloatImage> imgs;

	for(int i = 0; i < n; i++){
		CFloatImage im;
		im.ReAllocate(sh);
		imgs.push_back(im);
	}


	for(int i = 0 ; i < sh.width; i++){
		for(int j = 0; j < sh.height; j++){
			for(int k =0; k < n; k++){
				imgs[k].Pixel(i,j,0) = merged.Pixel(i,j,k);
			}
		}
	}

	return imgs;

}








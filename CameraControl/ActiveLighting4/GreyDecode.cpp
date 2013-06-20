///////////////////////////////////////////////////////////////////////////
//
// NAME
//  GreyDecode.cpp -- decodes series of grey-coded image
//
// DESCRIPTION
//   reads series of thresholded images (each labeled 0 (b), 128 (unknown), 255 (w))
//   and recovers bit pattern at each pixel.  
//
//
// Copyright � Daniel Scharstein, 2002.
//
///////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "imageLib.h"
#include <iostream>
#include <fstream>
#include "Utils.h"


#define UNK -9999	// label for uncertain pixel in both Int and Float image


int* pixelToCode;

int* codeToPixel;

// encodes n to grey code
unsigned int greycode(unsigned int n) {
    return pixelToCode[n];
}

// decodes n from grey code
unsigned int invgreycode(unsigned int n) {
	return codeToPixel[n];
}

void loadCodes(char* file){
	ifstream in(file, ios::binary | ios::in);

	int w;
	in.read((char*)&w,4);

	int* codes = new int[w];
	for(int i =0; i < w; i++){

		in.read((char*)&codes[i],4);

	}

	int* invcodes = new int[w];

	for(int i =0; i < w; i++){

		in.read((char*)&invcodes[i],4);

	}

	pixelToCode = codes;
	codeToPixel = invcodes;




}



// store image 'im' as k-th bit in 'val'  (k==0 means least-significant bit)
// if a pixel in 'im' has label 128 (unknown), mark corresponding pixel in 'unk'
void store_bit(CByteImage im, CIntImage val, CIntImage unk, int k)
{
	CShape sh = im.Shape();
	char error[200];
	
	int x, y, w = sh.width, h = sh.height;

	
	

	unsigned int bit = (1 << k);
	for (y = 0; y < h; y++) {
		uchar *p = (uchar *)&im.Pixel(0, y, 0);
		unsigned int *v = (unsigned int *)&val.Pixel(0, y, 0);
		unsigned int *u = (unsigned int *)&unk.Pixel(0, y, 0);

		for (x = 0; x < w; x++) {
			if (p[x] == 255)
				v[x] |= bit;
			else if (p[x] == 128)
				u[x] |= bit;
		}
	}
}

// decode grey codes.  val contains the grey code, unk contains the set of unknown bits.
// TODO: could be smarter here by trying to disambiguate pixels where only one one bit is 
// uncertain
void decodeGrey(CIntImage val, CIntImage unk)
{
    CShape sh = val.Shape();
    int x, y, w = sh.width, h = sh.height;
	
    for (y = 0; y < h; y++) {
	unsigned int *v = (unsigned int *)&val.Pixel(0, y, 0);
	unsigned int *u = (unsigned int *)&unk.Pixel(0, y, 0);
	for (x = 0; x < w; x++) {
		if(v[x] >= 1024){
			printf("ToBig: %d",v[x]);
		}
		unsigned int c = invgreycode(v[x]);
	    //unsigned int co = invgreycodeOld(v[x]);
	    //printf("%d %d\n",c,co);
	    if (u[x] == 0)
		v[x] = c;
	    else
		v[x] = UNK; // uncertain pixel
	}
    }
}


// fill holes in a line of code map
void fillCodeHolesLine(int *val, int stride, int n, int maxwidth, int maxborderdiff)
{	
    int oldv = -9999;
    int cnt = 0;
    int hole = 0;
    for (int x = 0; x < n; x++, val += stride) {
	int v = *val;
	if (v > 0) {
	    if (hole) {
		if (cnt <= maxwidth && ABS(v - oldv) <= maxborderdiff) {
		    int fillv = (v + oldv) / 2;
		    for (int k = 1; k <= cnt; k++)
			val[-k * stride] = fillv;
		}
	    }
	    cnt = 0;
	    oldv = v;
	    hole = 0;
	} else {
	    hole = 1;
	    cnt++;
	}
    }
}

// fill holes in code map. if directon==0, in x direction, else in y direction
void fillCodeHoles(CIntImage im0, int maxwidth, int maxborderdiff, int direction)
{
    CShape sh = im0.Shape();
    int x, y, w = sh.width, h = sh.height;
    int stride;

    if (direction==0) { // x direction
	stride = 1;
	for (y = 0; y < h; y++) {
	    int *val = &im0.Pixel(0, y, 0);
	    fillCodeHolesLine(val, stride, w, maxwidth, maxborderdiff);
	}
    } else { // y direction
        stride = &im0.Pixel(0, 1, 0) - &im0.Pixel(0, 0, 0);
	for (x = 0; x < w; x++) {
	    int *val = &im0.Pixel(x, 0, 0);
	    fillCodeHolesLine(val, stride, h, maxwidth, maxborderdiff);
	}
    }
}


// refine integer codes using a running average over window of width 2*rad+1
void refineCodesLine(int *v, float *f, int stride, int n0, int rad)
{	
    float maxgrad = 1;      // maximal gradient of values
    int minsupport = rad;   // how many "good" values are needed to trust the average

    int n = n0 * stride;
    for (int x0 = 0; x0 < n0; x0++) {
        int x = x0 * stride;
        float sum = 0;
        int cnt = 0;
        int v0 = v[x];
        f[x] = v0;
        if (v0 == UNK)
            continue;
        // compare pixels over window to v0 and include in average if close enough
        for (int r0 = -rad; r0 <= rad; r0++) {
            int r = r0 * stride;
            int x1 = x + r;
            int mirror = 0;
            int maxdiff = ABS(r0) * maxgrad;
            // if out of bounds, or unknown value, or difference too big, try mirrored
            if (x1 < 0 || x1 >= n || ABS(v0 - v[x1]) > maxdiff || v[x1] == UNK) {
                x1 = x - r;
                mirror = 1;
            }
            // if (now) out of bounds or (still) unknown or differnce too big, don't use
            if (x1 < 0 || x1 >= n || ABS(v0 - v[x1]) > maxdiff || v[x1] == UNK)
                continue;
            // close enough: add difference to center value (subtract if x was mirrored)
            sum += mirror ? (v0 - v[x1]) : (v[x1] - v0);
            cnt++;
        }
        if (cnt >= minsupport)
            f[x] = v0 + sum/cnt;    // result is center value plus average of diffs
    }
}


// refine integer codes using a running average over window of width 2*rad+1
void refineCodes(CIntImage val, CFloatImage fval, int rad, int direction)
{
    CShape sh = val.Shape();
    int x, y, w = sh.width, h = sh.height;

    if (direction == 0) {
    	for (y = 0; y < h; y++) {
	    int *v = &val.Pixel(0, y, 0);
	    float *f = &fval.Pixel(0, y, 0);
            int stride = &val.Pixel(1, 0, 0) - &val.Pixel(0, 0, 0);
            // assume that f has same stride!
            refineCodesLine(v, f, stride, w, rad);
        }
    } else {
    	for (x = 0; x < w; x++) {
	    int *v = &val.Pixel(x, 0, 0);
	    float *f = &fval.Pixel(x, 0, 0);
            int stride = &val.Pixel(0, 1, 0) - &val.Pixel(0, 0, 0);
            // assume that f has same stride!
            refineCodesLine(v, f, stride, h, rad);
        }
    }
}



// map integer code values to color map
void val2rgb(int N, CIntImage val, CByteImage result)
{
    CShape sh = val.Shape();
    int x, y, w = sh.width, h = sh.height;
	
    float scale = 1.0/(1<<N);
    for (y = 0; y < h; y++) {
	int *v = &val.Pixel(0, y, 0);
	uchar *r = &result.Pixel(0, y, 0);
		
	for (x = 0; x < w; x++) {
	    if (v[x] == UNK) {
		r[3*x] = r[3*x+1] = r[3*x+2] = 0;
	    } else {
		unsigned int c = v[x];
		float f = c * scale;
		hueshade(f, &r[3*x]);
	    }
	}
    }
}

// map integer code values to float
void val2float(CIntImage val, CFloatImage result)
{
    CShape sh = val.Shape();
    int x, y, w = sh.width, h = sh.height;
	
    for (y = 0; y < h; y++) {
	int *v = &val.Pixel(0, y, 0);
	float *r = &result.Pixel(0, y, 0);
		
	for (x = 0; x < w; x++) {
	    r[x] = v[x];
        }
        /* now, use same UNK label in both val and fval
	   if (v[x] == UNK)
	   r[x] = -1.0; // label for unknown pixels
	   else
	   r[x] = v[x];
	*/
    }
}


// subtract average plane from code values, by looking at pixel grid 'step' apart
void subtractPlane(CIntImage val, int step)
{
    CShape sh = val.Shape();
    int x, y, w = sh.width, h = sh.height;
    float ax, ay, b;
	
    /* old code: estimate ax, ay separately using two line fits
	
    // estimate horizontal slope in center of image
    int *xline = &val.Pixel(0, h/2, 0);
    int xstride = &val.Pixel(1, 0, 0) - &val.Pixel(0, 0, 0);
    fitLine(xline, w, xstride, ax, b, UNK);
    //printf("horizontal line: a=%g, b=%g\n", ax, b);
	  
    // estimate vertical slope in center of image
    int *yline = &val.Pixel(w/2, 0, 0);
    int ystride = &val.Pixel(0, 1, 0) - &val.Pixel(0, 0, 0);
    fitLine(yline, h, ystride, ay, b, UNK);
    //printf("vertical line: a=%g, b=%g\n", ay, b);
		
    b = 0; // don't need b here
    */

    int *data = &val.Pixel(0, 0, 0);
    int xstride = &val.Pixel(1, 0, 0) - &val.Pixel(0, 0, 0);
    int ystride = &val.Pixel(0, 1, 0) - &val.Pixel(0, 0, 0);
    fitPlane(data, w, h, step, step, xstride, ystride, ax, ay, b, UNK);
    //printf("plane params: ax=%g, ay=%g, b=%g\n", ax, ay, b);

    // subtract average plane
    for (y = 0; y < h; y++) {
	int *v = &val.Pixel(0, y, 0);
	for (x = 0; x < w; x++) {
	    if (v[x] != UNK) {
		float vf = v[x];
		vf -= ax * x + ay * y + b;	// subtract plane
		if (vf == UNK)
		    vf += 0.001f;	// make sure we're not introducing UNK by accident
		v[x] = vf;
	    }
	}
    }
}

// map float codes to grey image by scaling and clipping to 0..255
// if mode == 0, use FIXEDMIN, FIXEDMAX
// if mode == 1, use actual min, actual max
// if mode == 2, use min = mean - FACT * stddev, max = mean + FACT * stddev
// if mode == 3, use values * 4 modulo 256
void fval2byte(CFloatImage fval, CByteImage result, int mode) 
{
    float FIXEDMIN = 200;
    float FIXEDMAX = 900;
    float FACT = 0.5;

    CShape sh = fval.Shape();
    int x, y, w = sh.width, h = sh.height;
    float minv, maxv;

    if (mode == 0 || mode == 3) {
	minv = FIXEDMIN;
	maxv = FIXEDMAX;
    } else {
	// compute min, max, mean and stddev
	minv = 1e20f;
	maxv = -1e20f;
	int s1 = 0;
	float sv = 0;
	float svv = 0;
	for (y = 0; y < h; y++) {
	    float *v = &fval.Pixel(0, y, 0);
	    for (x = 0; x < w; x++) {
		float v0 = v[x];
		if (v0 == UNK) 
		    continue;
		s1++;
		sv += v0;
		svv += v0*v0;
		minv = __min(minv, v0);
		maxv = __max(maxv, v0);
	    }
	}
	if (mode == 2) { // use mean +/- FACT * stddev
	    float mean = sv / s1;
	    float var = svv / s1 - 2 * mean + (mean * mean);
	    float stddev = sqrt(var);
	    //printf("mean = %g, stddev = %g\n", mean, stddev);
	    //printf("    min = %g, max = %g\n", minv, maxv);
	    minv = __max(minv, mean - FACT * stddev);
	    maxv = __min(maxv, mean + FACT * stddev);
	}
    }

    if (mode < 3)
    	printf("min = %g, max = %g\n", minv, maxv);
    else
        printf("mapping * 4 modulo 256\n");

    // map to 0..255
    for (y = 0; y < h; y++) {
	float *v = &fval.Pixel(0, y, 0);
	uchar *r = &result.Pixel(0, y, 0);
	for (x = 0; x < w; x++) {
	    //if (v[x] == UNK)
	    if (v[x] < 0 ) // CHANGED to make compatible with old code -1==UNK
		r[x] = 0;
	    else {
		if (mode==3)
		    r[x] = ((int)(v[x]*4 + 0.5)) % 256;
		else
		    r[x] = __min(255, __max(0, (v[x] - minv) * 255 / (maxv - minv)));
	    }
	}
    }
}



// Decode images.  if direction==0, code goes (mainly) in x direction, otherwise in y-direction
// 1. combine all numIm thresholded images into CIntImages val and unk
// 2. decode grey code (considering unk) into val
// 3. fill holes in val
// 4. refine code values into float values fval
// 5. save fval CFloatImage
// 6. save .pgm file that contains gray-level encoding of refined code values
CFloatImage greyDecode(char* outdir, char* codefile, int direction, char **imList, int numIm)
{
	CFloatImage dummy;
    CByteImage im;
    CShape sh;
    int i, verbose=1;
    char filename[200];
    CIntImage val, unk;

    loadCodes(codefile);

	
	// 1. combine all numIm thresholded images into CIntImages val and unk
    // for both x and y directions
	for (i = 0; i < numIm; i++) {
	    ReadImage(im, imList[i]);
	    if (i == 0) {
			sh = im.Shape();
			val.ReAllocate(sh, true);
			val.ClearPixels();
			unk.ReAllocate(sh, true);
			unk.ClearPixels();
	    }
	    if (sh != im.Shape() || sh.nBands != 1)
	    	throw CError("greyDecode: all images need to have same size and 1 band");
	    
	    store_bit(im, val, unk, i);  // NEW
	
	}

	
	// 2. decode grey code (considering unk) into val
	decodeGrey(val, unk);

	
	if (1) { // save val as pgm
		fprintf(stderr, "saving val as pgm - ");
		CByteImage result;
		ScaleAndOffset(val, result, 0.25, direction);
		sprintf(filename, "%svresult%d.pgm",outdir, direction);
		WriteImageVerb(result, filename, verbose);

	}


	if (1) {
		// 3. fill holes in val
		if (verbose) printf("filling holes\n");
		int maxwidth = 5;
		int maxborderdiff = 2;
		fillCodeHoles(val, maxwidth, maxborderdiff, direction);
		maxborderdiff = 0;
		fillCodeHoles(val, maxwidth, maxborderdiff, 1-direction);
		maxborderdiff = 2;
		fillCodeHoles(val, maxwidth, maxborderdiff, direction);


	}

	if (1) { // save filled val as pgm
		fprintf(stderr, "saving val as pgm - ");
		CByteImage result;
		ScaleAndOffset(val, result, 0.25, 0);
		sprintf(filename, "%swresult%d.pgm",outdir, direction);
		WriteImageVerb(result, filename, verbose);
	}


	// 4. refine code values into float values fval
	sh.nBands = 1;
	CFloatImage fval(sh);
		if (verbose) printf("refining code values\n");
		int radius = 3;
		refineCodes(val, fval, radius, direction);


	// 5. save fval
	fprintf(stderr, "writing as float (pmf) image - ");
	sprintf(filename, "%sresult%d.pmf", outdir,direction);
	WriteImageVerb(fval, filename, verbose);


	if (1) {
		fprintf(stderr, "encoding in ppm - ");
		sh.nBands = 3;
		CByteImage result(sh);
		val2rgb(numIm, val, result);
		sprintf(filename, "%scresult%d.ppm", outdir,direction);
		WriteImageVerb(result, filename, verbose);
	}


	// 6. save .pgm file that contains gray-level encoding of refined code values
	if (1) { // pgm
		fprintf(stderr, "encoding in pgm - ");
		sh.nBands = 1;
		CByteImage result(sh);
		int mode = 1; // 0 - use fixed scaling, 3 - use val*4 modulo 256
		fval2byte(fval, result, mode);
		sprintf(filename, "%sresult%d.pgm", outdir, direction);
		WriteImageVerb(result, filename, verbose);
	}

	return fval;

	
}



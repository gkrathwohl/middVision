#include "GUI.h"
#include "Utils.h"
#include "CameraControl.h"
#include "Greycode.h"
#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <fstream>

#include <stdlib.h>
#include "imageLib.h"
#include <utility>
#include <stdarg.h>
#include <vector>


using namespace Gdiplus;
using namespace std;


//paints a checkerboard pattern with big squares
void checkerPattern (unsigned int* arr)
{ 
	int i;
	int dim = width*height;
	reflectiveBinaryCode (arr, 0, 0, 2);
	unsigned int* arr2 = new unsigned int[width*height];
	reflectiveBinaryCode (arr2, 0, 1, 2);
	unsigned int b = 0xff000000; //black
	unsigned int w = 0xffffffff;  //white

	for (i = 0; i< dim; i++)
	{
		if (((arr2[i]== w) && (arr[i]==w)))
			arr[i] = b;
		else if (((arr2[i]== w) && (arr[i]==b)))
			arr[i] = w;

	}
}

//paints a checkerboard pattern with pixel wide squares
void pixelCheckerPattern(unsigned int* arr)
{ 
	int i;
	int j;
	unsigned int b = 0xff000000; //black
	unsigned int w = 0xffffffff; //white

	for (i = 0; i< height; i++)
	{
		for (j=0; j< width; j++)
		{
			if((i%2) == (j%2))
				arr[i*width + j]=b;
			else
				arr[i*width + j]=w;
		}

	}
}



//displays a simple box with the specified message
int DisplayErrorMessageBox(LPCTSTR msg)
{  
	int msgboxID = MessageBox(
		NULL,
		msg,
		(LPCTSTR)"Error",
		MB_ICONWARNING | MB_OK | MB_SETFOREGROUND
		);
	switch (msgboxID)
	{
	case IDOK:
		// TODO: add code
		break;
	}
	return msgboxID;
}


//displays a simple box with an OK button which,
//upon pressing, minimizes the RPMC window, closes the 
//LiveView window, and takes a picture
int DisplayCalibrationMessageBox()
{  // HWND liveView = FindWindow(NULL,"Live View (all cameras)"); //
	char all [100];
	sprintf (all,"Calibration images taken: %d \n Press OK when ready. What is exposure?", c); 
	int msgboxID = MessageBox(
		NULL,
		all,
		(LPCTSTR)"Calibration Shooting",
		MB_ICONWARNING | MB_OK | MB_SETFOREGROUND
		);

	switch (msgboxID)
	{
	case IDOK:
		
		liveViewHide();
	    if (enablePS)
			shootAll();
		else
			shoot();
		break;
	}
	return msgboxID;
}


//generates a Bitmap from a 8 bit depth .bmp file.  
//idea from http://www.dreamincode.net/forums/topic/26936-how-to-make-sense-of-the-bmp-format/
Bitmap* ByteBMPfromFile(LPCSTR fileName)
{
	HANDLE hfile;
	BYTE *image;
	DWORD written;
	hfile = CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	unsigned int* arr = new unsigned int[width*height];
	int trash[1078];
	ReadFile(hfile,&trash,1078,&written,NULL); //skipping the header of the file
	int imagesize = width*height;
	image = new BYTE[imagesize]; // Create a new image (I'm creating an array during runtime)
	ReadFile(hfile,image,imagesize*sizeof(BYTE),&written,NULL); // Reads it off the disk

	for  (int j = 0; j< width*height; j++)
	{
	 arr[j] =  (int)image[j] == 0? 0xffffffff : 0xff000000;
	}
	CloseHandle(hfile);
	Bitmap* img = new Bitmap(width, height, 4*width, PixelFormat32bppARGB, (byte *)arr);
	delete arr;
	delete image;
	return img;
}


//reads a single row of patterns from a .bmp file
BYTE* CodeLineFromBMPFile(LPCSTR fileName){
	HANDLE hfile;
	BYTE *image;
	DWORD written;
	hfile = CreateFile(fileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
	unsigned int* arr = new unsigned int[width*height];
	int trash[1078];
	ReadFile(hfile,&trash,1078,&written,NULL);
	image = new BYTE[width]; 
	ReadFile(hfile,image,width*sizeof(BYTE),&written,NULL); 
	CloseHandle(hfile);
	delete arr;
	return image;
}



//minsSWcode.dat contains the data for generating a row of each pattern.
//This method returns an array of 10 arrays, each representing a line
//of a different pattern. 
//NOTE: Don't touch the minsSWcode.dat file!
BYTE** getCodesFromFile(LPCSTR fileName){

	int numCodes = 10;
	BYTE** minSW = new BYTE*[numCodes];
	for(int i =0; i < numCodes; i++)
	{
		minSW[i] = new BYTE[1024];
	}
	ifstream infile;
	infile.open("minsSWcode.dat");
	BYTE holder = 0;
	for(int i=0; i < numCodes; i++){
		BYTE* code = minSW[i];
		for(int j =0; j < width; j++){
			infile >> holder;
			code[j] = holder;
		}
	}
	infile.close();

	return minSW;
}


//In case the minsSWcode.dat file has still been modified/deleted/lost,
//it can be regenerated with this method.
void regenCodefile(){

	BYTE** minSW = new BYTE*[10];
	minSW[0] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\01.bmp");
	minSW[1] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\02.bmp");
	minSW[2] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\03.bmp");
	minSW[3] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\04.bmp");
	minSW[4] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\05.bmp");
	minSW[5] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\06.bmp");
	minSW[6] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\07.bmp");
	minSW[7] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\08.bmp");
	minSW[8] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\09.bmp");
	minSW[9] = CodeLineFromBMPFile("C:\\Documents and Settings\\nnesic\\Desktop\\ActiveLighting\\patterns\\10.bmp");
	
	ofstream outfile;
	outfile.open("minsSWcode.dat");
	for(int i = 0; i<10; i++){
		BYTE* code = minSW[i];
		for(int j =0; j< 1024; j++){
			outfile << code[j];
		}
	}
	outfile.close();
}


//prints an info text file with parameters used in shooting the scene
void saveInfo(){
	int j = 0;
	char all[200];
	ofstream info;
	sprintf (all, "%sInfo.txt", path);
	info.open (all);

	info << "sceneName			" << sceneName << endl ;
	for (j = 0; j< exposures.size(); j++)
	{
			
		info << "exposure " << j+1  << "			" << ext [exposures.at(j)] << endl; 
		
	}
	for (j = 0; j< exposuresAmb.size(); j++)
	{
		
		info << "AMBexposure " << j+1  << "			" << ext [exposuresAmb.at(j)] << endl; 
		
	}
	info << "aperture			" << aperture << endl << "projectors			" << P << endl; 
	/*for (j=0; j<L; j++)
	{
		sprintf (lightConditions,"lights %d			%d%d%d%d%d%d%d%d",			
			j+1, allLights[j][0], allLights[j][1], allLights[j][2], allLights[j][3], allLights[j][4], allLights[j][5], allLights[j][6], allLights[j][7]);
		info << lightConditions << endl;
	}*/
	info << endl << "Additional information: " << endl << endl<< szText;
	info.close();
}


//borrowed from http://www.codeproject.com/KB/files/Folder_Utility.aspx
//creates the specified directory
void CreateDir(char* Path)
{
	char DirName[256];
	char* p = Path;
	char* q = DirName; 
	while(*p)
	{
		if (('\\' == *p) || ('/' == *p))
		{
			if (':' != *(p-1))
			{
				CreateDirectory(DirName, NULL);
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	CreateDirectory(DirName, NULL);
}




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









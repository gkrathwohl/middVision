//////////////////////////////////////////////
/*
NOTE: DSLR Remote Pro Multi Camera (RPMC) should be ran before running 
	  Active Lighting. 



When the program is run, it displays a window which lets the user 
specify the desired parameters. If no parameters are changed from default, the 
program will read a text file, info.txt, and will get its parameters from there. 
This text file is written in the same format as the info files written upon exiting the 
program, and those can be used instead. 


It is possible to access certain functions by pressing the following keys:


		B - Takes an ambient shot with all the cameras, and all the exposures.
		C - Opens the Multi Live view window, and pops a message box. When the message box is 
			closed, a calibration picture is taken.
		D - Decodes all the takes of the current scene not decoded yet, and highlights in green 
			areas which don't get code values in any of the takes.
		G - Displays the first greycode pattern.
		P - Displays a pixel - checkerboard pattern 
		S - Shoots a 'take', projecting the patterns and using all the projectors and exposures.
		X - Displays a checkerboard pattern (usefull for focusing the projectors!).
		W - Clears the display and paints it white. Usefull when positioning the projectors. 
	  ESC - Exits the program, saving a text file with information on all the parameters used.

	  1-8 - Activates / deactivates the selected projector.

ALT   1-8 - Activates / deactivates the selected ambient light.
ALT     G - Switches between the two different greycodes we implemented.
*/



#include "GUI.h"
#include "Greycode.h"
#include "CameraControl.h"
#include "RelayControl.h"
#include "DSLRRemoteLib.h"
#include "parse.h"
#include "SerialUtils.h"
#include "SwitcherCtrl.h"
#include "X10Ctrl.h"
#include "imageLib.h"
#include "Utils.h"
#include "flowIO.h"
#include "assert.h"
#include <string.h>
#include <tchar.h>
#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include ".\\imageLib\\imageLib.h"
#include "Utils.h"
#include <vector>


using namespace std;
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#define ID_EDIT 1

//array for storing the "aditional info" about the scene
TCHAR szText[5000] = "";

//values of exposures in ms. Used to determine sleep time between shots. 
int ex [37] = {0, 30000, 25000, 20000, 15000, 13000, 10000, 8000, 6000, 5000, 4000, 3200, 2500, 2000, 1600, 1300, 1000,
	800, 600, 500, 400, 300, 250, 200, 170, 125, 100, 80, 70, 50, 40, 35, 25, 20, 17, 13, 10};

//values of exposures, as they appear in the RPMC menu. Used for writing th text file
char *ext [37] = {"Default", "30", "25", "20", "15", "13", "10", "8", "6", "5", "4", "3.2", "2.5", "2", "1.6", "1.3", "1", "0.8", "0.6", "0.5", "0.4", "0.3", "1/4", 
	"1/5", "1/6", "1/8", "1/10", "1/13", "1/15", "1/20", "1/25", "1/30", "1/40", "1/50", "1/60", "1/80", "1/100"};

//values of apertures, as they appear in the RPMC menu. Used for writing th text file
char *ap [24] = {"Default", "1.8", "2.0", "2.2", "2.5", "2.8", "3.2", "3.5", "4.0", "4.5", "5.0", "5.6", "6.3", "7.1", "8.0", "9.0", "10", "11", "12", "14", "16", "18", "20", "22"};


FILE *fr; 

char stage = 'i'; //char to keep track of what we are doing
int quality = -1; //keeps track of what quality images should be taken in
int takes = 5; // counts the takes of the same scene done so far
int c = 0; //counts calibratrion shots taken so far
int l = 0; // indicates the numbmer of light combination to be used
int L = 0; //counts how many different ambient light conditions were photographed
int p = 1; //projector in use
int P = 1; //numbwer of projectors usedz
bool G = false; //switch between the two codes
int projectors [8] = {1, 1, 1, 1, 1, 1, 1, 1}; //keeps track of wether a projector is on or off - simpler than quiering the switcher
int allLights [5][8]; //keeps track of all light configuration used on different shootings
int lights [8] = {0, 0, 0, 0, 0, 0, 0, 0}; // keeps track of which lights are on
vector<int> zzz; //exposure times in ms
vector<int> zzzAmb; 

vector<int> exposures;

vector<int> exposuresAmb;
int enablePS = 1;
char aperture [10];	
char dir[100] = "C:\\a";
char scname[100] = "Staircase"; 
char path[200]; //directory name where to store the picture
char sceneName [200]; //name of the current scene
char lightConditions[1000]; //for a description of the ambient light conditions 
BYTE** minSW = getCodesFromFile("minSW.dat");
HWND remotePro = FindWindow(NULL,"DSLR Remote Pro Multi-Camera - Connected to 2 cameras"); //handle for the DSLR RPMC window
HWND                hWnd;



//thresholds the confidence maps (defined in GreyThresh.cpp)
void thresholdConfidence(CByteImage imOut, int certain);

//helper method for thresholding images
void updateConfidence(CByteImage imIn, CByteImage imOut);

// threshold images (defined in GreyThresh.cpp)
void greyThresh(char* outdir, char *imm1,  char *imm2, int num);

// decode images (defined in GreyDecode.cpp)
CFloatImage greyDecode(char* outdir, char* codefile, int direction, char **imList, int numIm);

// combine multiple confidence maps into one (defined in GreyThresh.cpp)				
void combineConfidence(char *outfile, char **imList, int numIm, int certain);

// compute pair of disparity maps from labeled (pmf) images (defined in Disparity.cpp)
void computeDisparities(char *outdir,char *in0, char *in1, int dXmin, int dXmax, int dYmin, int dYmax);

CFloatImage simpleCrossCheck(CFloatImage im0, CFloatImage im1);



//paints a pattern on the screen.
//inv 1 for inverted pattern
//vert 1 for horizontal pattern
VOID OnPaint(HDC hdc, int inv, int vert, int bit)
{  
	Graphics graphics(hdc);
	unsigned int* arr = new unsigned int[width*width];
	if (!G)
	{
		minStripeWidthCode (arr, inv, vert, bit);
	}
	else
	{
		reflectiveBinaryCode (arr, inv, vert, bit);
	}
	Bitmap img(width, height, 4*width, PixelFormat32bppARGB, (byte *) arr); // create bitmap
	Status status = img.GetLastStatus();
	status = graphics.DrawImage(&img, 0, 0); // and draw it
	delete[] arr;

}


//paint the screen gray 
VOID projectGray(HDC hdc)
{
	Graphics     graphics(hdc);
	graphics.Clear(0xFF7F7F7F);
}


//paint the screen white
VOID projectWhite (HDC hdc)
{
	Graphics     graphics(hdc);
	graphics.Clear(0xFFFFFFFF);
}



//paint a checkboard pattern on the screen
VOID projectCheckerBoard (HDC hdc)
{
	Graphics graphics(hdc);
	unsigned int* arr = new unsigned int[width*height];
	checkerPattern (arr);
	Bitmap img(width, height, 4*width, PixelFormat32bppARGB, (byte *) arr); // create bitmap
	Status status = img.GetLastStatus();
	status = graphics.DrawImage(&img, 0, 0); // and draw it
	delete[] arr;
}


//paint a checkboard pattern on the screen
VOID projectPixelCheckerBoard (HDC hdc)
{
	Graphics graphics(hdc);
	unsigned int* arr = new unsigned int[width*height];
	pixelCheckerPattern (arr);
	Bitmap img(width, height, 4*width, PixelFormat32bppARGB, (byte *) arr); // create bitmap
	Status status = img.GetLastStatus();
	status = graphics.DrawImage(&img, 0, 0); // and draw it
	delete[] arr;
	
}


//like shootAmbient, but only uses one exposure
void shootBall (){
char all [100];
	sprintf(all, "%sorig\\ambient\\L%d\\%%O", path,L);
	while(SetOutputPath(all));
	Sleep(1000);
	if (enablePS)
		shootPS();
	setQuality(4);

		int sleep = 5000;
		while(SetShutterAperture(13, -1));
		Sleep(1000);
		ambientShoot();
		Sleep(1000);




}

//takes different exposure pictures of the scene with the DSLR
//and then A bracketed shot with the point and shoots
//NOTE: bracketting must be set manually on the PS camera
void shootAmbient (unsigned int a)
{
	char all [100];
	if (a == 0) {
		sprintf(all, "%sorig\\ambient\\L%d\\%%O", path,L);
	}
	else if (a == 1) {
		sprintf(all, "%sorig\\ambientBall\\L%d\\%%O", path,L);
	}

	while(SetOutputPath(all));
	Sleep(1000);
	if (enablePS)
		shootPS();
	setQuality(4);
	
	for (int t =0; t<exposuresAmb.size(); t++) //for all exposures
	{ 
		if (exposuresAmb.at(t) == 0)
			break;
		int sleep = 5000;
		if (zzzAmb.at(t) >= 4000)
			sleep = zzzAmb.at(t) + 3000;
		while(SetShutterAperture(exposuresAmb.at(t), -1));
		Sleep(1000);
		ambientShoot();
		Sleep(sleep);
	}
	setQuality(1);
	for (int t =0; t<exposuresAmb.size(); t++) //for all exposures
	{ 
		if (exposuresAmb[t] == 0)
			break;
		int sleep = 5000;
		if (zzzAmb.at(t) > 3000)
			sleep = zzzAmb.at(t) + 3000;
		while(SetShutterAperture(exposuresAmb.at(t), -1));
		Sleep(1000);
		ambientShoot();
		Sleep(sleep);
	}
	setQuality(4);
	
}




//project all the patterns and take pictures of them
//with the SLRs, with all exposures and projectors
void shootSeries(HDC hdc, HWND hWnd, int t)
{ 
	int sleep = 2*zzz[t] +300;
	if (zzz.at(t) >=5000)
		sleep = zzz.at(t) + 3000;
	int  i=0;
	char all [100];
	if(G)
		sprintf(all, "%sorig\\greycode\\take%dexp%dP%dG\\%%O", path, takes, t, p);
	else
		sprintf(all, "%sorig\\greycode\\take%dexp%dP%d\\%%O", path, takes, t, p);
	while (SetOutputPath(all)){}
	Sleep(sleep);
	while(i<10)
	{   
		//paint vertical pattern
		OnPaint(hdc, 0, 1, i);
		Sleep(100);
		shoot();
		Sleep(sleep);
		InvalidateRect (hWnd, NULL, TRUE);

		//paint vertical inverted pattern
		OnPaint(hdc, 1, 1, i);
		Sleep(100);
		shoot();
		Sleep(sleep);
		InvalidateRect (hWnd, NULL, TRUE);

		i++;
	}

	i=0;
	while(i<10)
	{   
		//paint horizontal pattern
		OnPaint(hdc, 0, 0, i);
		Sleep(100);
		shoot();
		Sleep(sleep);
		InvalidateRect (hWnd, NULL, TRUE);

		//paint horizontal inverted pattern
		OnPaint(hdc, 1, 0, i);
		Sleep(100);
		shoot();
		Sleep(sleep);
		InvalidateRect (hWnd, NULL, TRUE);

		i++;
	}
}



//creates the confidence maps of the taken pictures
void confidence (char *newPath, int t)
{
	char all [200];
	for (int e = 0; e<exposures.size(); e++)
	{
		if (exposures.at(e) == 0)
			return;
		for (int p = 1; p<=P; p++)

		{
							printf ("ok");
			//creating folders
			if(G)
			{
				sprintf(all, "%sconfidence\\exp%dP%dG\\left", newPath, e, p);
				CreateDir(all);
				sprintf(all, "%sconfidence\\exp%dP%dG\\right", newPath, e, p);
				CreateDir(all);

			}
			else
			{
				sprintf(all, "%sconfidence\\exp%dP%d\\left", newPath, e, p);
				CreateDir(all);
				sprintf(all, "%sconfidence\\exp%dP%d\\right", newPath, e, p);
				CreateDir(all);
			}



			//calculating confidence maps
			for (int i=1; i<=40; i++)
			{	
				char outputdir [200]; 
				char num1[5];
				char num2[5];
				char imm1 [200];
				char imm2 [200];
				if (i<10)
					sprintf (num1, "000%d", i);
				else 
					sprintf (num1, "00%d", i);
				i++;
				if (i<10)
					sprintf (num2, "000%d", i);
				else 
					sprintf (num2, "00%d", i);


				if (G)
				{
					sprintf (outputdir, "%sconfidence\\exp%dP%dG\\left\\", newPath, e, p);
					sprintf (imm1, "%sorig\\greycode\\take%dexp%dP%dG\\left\\IMG_%s.JPG", path, t,e, p, num1);
					sprintf (imm2, "%sorig\\greycode\\take%dexp%dP%dG\\left\\IMG_%s.JPG", path, t,e, p, num2);
					greyThresh(outputdir, imm1, imm2, i/2);

					sprintf (outputdir, "%sconfidence\\exp%dP%dG\\right\\", newPath, e, p);
					sprintf (imm1, "%sorig\\greycode\\take%dexp%dP%d\\right\\IMG_%s.JPG", path, t,e, p, num1);
					sprintf (imm2, "%sorig\\greycode\\take%dexp%dP%d\\right\\IMG_%s.JPG", path, t,e, p, num2);
					greyThresh(outputdir, imm1, imm2, i/2);
				}

				else
				{
					sprintf (outputdir, "%sconfidence\\exp%dP%d\\left\\", newPath, e, p);
					sprintf (imm1, "%sorig\\greycode\\take%dexp%dP%d\\left\\IMG_%s.JPG", path, t,e, p, num1);
					sprintf (imm2, "%sorig\\greycode\\take%dexp%dP%d\\left\\IMG_%s.JPG", path, t,e, p, num2);
					greyThresh(outputdir, imm1, imm2, i/2);

					sprintf (outputdir, "%sconfidence\\exp%dP%d\\right\\", newPath, e, p);
					sprintf (imm1, "%sorig\\greycode\\take%dexp%dP%d\\right\\IMG_%s.JPG", path, t,e, p, num1);
					sprintf (imm2, "%sorig\\greycode\\take%dexp%dP%d\\right\\IMG_%s.JPG", path, t,e, p, num2);
					greyThresh(outputdir, imm1, imm2, i/2);
				}
			}
		}
	}
}

//thresholds and decodes the taken images
void thresholdDecode (char *newPath)
{

	char all[200];
	CByteImage imm;
	CByteImage base;
	char outdir [200];

	for (int j = 1; j<=P; j++)
	{ //for all projectors

		//left camera pictures
		sprintf (outdir, "%sprojector%d\\left\\", newPath,j);
		CreateDir(outdir);
		sprintf (all, "%sthreshold\\projector%d\\left\\", newPath,j);

		CreateDir(all);
		char codefile [50] = "minSW.dat";
		for (int k = 1; k<=20; k++)
		{
			
			sprintf (all, "%sconfidence\\exp0P%d\\left\\thresh%d.pgm",  newPath, j, k);
			
			ReadImage (base, all);
			
			for (int i = 0; i< exposures.size(); i++)
			{ //for all exposures
				if (zzz.at(i) == 0)
					break;
				sprintf (all, "%sconfidence\\exp%dP%d\\left\\thresh%d.pgm",  newPath,  i, j, k);
						
				ReadImage (imm, all);
				updateConfidence(base, imm);
			}
			thresholdConfidence(base, 120);
			sprintf (all, "%sthreshold\\projector%d\\left\\conf%d.pgm", newPath, j, k);
			WriteImage(base, all);
		}

		char** imList = new char*[20];
		for (int i = 0; i<20; i++)
		{   
			char* bla = new char[200];
			sprintf (bla, "%sthreshold\\projector%d\\left\\conf%d.pgm", newPath, j, i+1);
			imList[i] = bla;

		}
		greyDecode(outdir, codefile, 0, imList, 10);
		greyDecode(outdir, codefile, 1, &imList[10], 10);


		//right camera
		sprintf (outdir, "%sprojector%d\\right\\", newPath,j);
		CreateDir(outdir);

		sprintf (all, "%sthreshold\\projector%d\\right\\", newPath,j);
		CreateDir(all);
		for (int k = 1; k<=20; k++)
		{//for all projectors

			sprintf (all, "%sconfidence\\exp0P%d\\right\\thresh%d.pgm", newPath,j, k);
			ReadImage (base, all);
			for (int i = 0; i< exposures.size(); i++)
			{ //for all exposures
				if (zzz.at(i) == 0)
					break;
				sprintf (all, "%sconfidence\\exp%dP%d\\right\\thresh%d.pgm", newPath, i, j, k);
				ReadImage (imm, all);
				updateConfidence(base, imm);
			}
			thresholdConfidence(base, 120);
			sprintf (all, "%sthreshold\\projector%d\\right\\conf%d.pgm", newPath, j, k);
			WriteImage(base, all);
		}

		for (int i = 0; i<20; i++)
		{   
			char* bla = new char[200];
			sprintf (bla, "%sthreshold\\projector%d\\right\\conf%d.pgm", newPath, j, i+1);
			imList[i] = bla;
		}
		
		greyDecode(outdir, codefile, 0, imList, 10);
		greyDecode(outdir, codefile, 1, &imList[10], 10);

    	delete[] imList;
	}
}


//creates an image highlighting all areas that are dark on all decodings 
//done so far
void highlightShadows()
{
	CByteImage left;
	CByteImage right;
	int total = (takes - 1)*P;
	char all[200];

	//base images on which hightlighting will be done
	sprintf (all, "%sorig\\ambient\\L0\\left\\IMG_0001.JPG", path);
	ReadImage(left, all);
	sprintf (all, "%sorig\\ambient\\L0\\right\\IMG_0001.JPG", path);
	ReadImage(right, all);


	//create an image array with all the decoded pictures
	vector< vector<CByteImage> > imListLeft, imListRight;
	for (int t = 1; t < takes; t++)
			{
				vector<CByteImage> rowl, rowr;
				imListLeft.push_back(rowl);
				imListRight.push_back(rowr);
				for (int p = 1; p<= P; p++)
				{
					CByteImage temp1, temp2;
					sprintf (all, "%sdecoded\\take%d\\projector%d\\left\\result0.pgm", path, t, p);
					ReadImage(temp1, all);
					
					imListLeft[t-1].push_back(temp1);
					sprintf (all, "%sdecoded\\take%d\\projector%d\\right\\result0.pgm", path, t, p);
					ReadImage(temp2, all);
					imListRight[t-1].push_back(temp2);


				}
	}

	for (int x = 0; x < 3088; x++)//width of a medium jpeg picture. 
	{
		for (int y = 0; y<2056; y++)//height of a medium jpeg picture. 
		{
			int countRight = 0; //counts how many decoded values we get 
			int countLeft = 0;
			for (int t = 1; t < takes; t++)
			{
				for (int p = 1; p<= P; p++)
				{
					if (imListLeft[t-1][p-1].Pixel(x, y, 0) != 0)
						countLeft++;
					if (imListRight[t-1][p-1].Pixel(x, y, 0) !=0)
						countRight++;
				}
			}
		
		
		if (countLeft < 1)
		{
			int red = 0;
			int green = 0;
			int blue = 0;
			switch (countLeft){

		case 0:
			red = 255;
			break;

		/*case 1: 
			red = 255;
			green = 127;
			blue = 39;
			break;

		case 2: 
			 red = 255;
			 green = 255;
			 break;

		case 3:
			 green = 255;
			 break;
			 */
		}
			left.Pixel(x, y, 0) =  blue;
			left.Pixel(x, y, 1) = green;
			left.Pixel(x, y, 2) = red;
		}

	   		
		
		if (countRight < 1)
		{
			int red = 0;
			int green = 0;
			int blue = 0;
			switch (countRight){

		case 0:
			red = 255;
			break;

		/*case 1: 
			red = 255;
			green = 127;
			blue = 39;
			break;

		case 2: 
			 red = 255;
			 green = 255;
			 break;

		case 3:
			 green = 255;
			 break; */
		}
			right.Pixel(x, y, 0) = blue;
			right.Pixel(x, y, 1) = green;
			right.Pixel(x, y, 2) = red;
		}
	}
	}

	sprintf (all, "%sdecoded\\left.ppm", path);
	WriteImage (left, all);
	sprintf (all, "%sdecoded\\right.ppm", path);
	WriteImage( right, all);
}



//combine confidence maps and decode 
void decode ()
{
	DisplayErrorMessageBox ("Press OK to start decoding");
	char all[200];
	if (takes == 1)
		DisplayErrorMessageBox("no sets of images were taken yet");
	else
	{   //go through the takes done so far
		for (int t = 1; t<takes; t++)   
		{

			sprintf (all, "%sdecoded\\take%d", path, t);
			DWORD result =  GetFileAttributes(all);
			if (result == INVALID_FILE_ATTRIBUTES)
			{
				char newPath[200];
				sprintf (newPath, "%sdecoded\\take%d\\", path, t);
				//create a folder for all the files needed for this take's decoding
				CreateDir(newPath);
				sprintf(all, "%s", newPath);
				confidence (newPath, t);
				thresholdDecode(newPath);
			}
		}
		highlightShadows();
		DisplayErrorMessageBox("Done!");
	}
}






//coodinate turning on/off of lights
void lightSwitch (int unit, char house){
	char command [20];
	if(lights[unit-1] == 0)
	{
		sprintf (command, "%c%d on", house, unit);
		sendCommand(command);
		lights[unit-1] = 1;
	}
	else
	{
		sprintf (command, "%c%d off", house, unit);
		sendCommand(command);
		lights[unit-1] = 0;
	}
}

//turns the specified projector on and off
void projectorSwitch (int projector){

	if(projectors[projector - 1] == 0)
	{
		turnOn(projector);
		projectors[projector - 1] = 1;
	}
	else
	{
		turnOff(projector);
		projectors[projector - 1] = 0;
	}
}



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK TxtProc(HWND, UINT, WPARAM, LPARAM);


//the main function that initializes the windows
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{

	HWND				hTwnd;

	MSG                 msg;
	WNDCLASS            wndClass;
	WNDCLASS			wndClass2;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;


	PAINTSTRUCT  ps;
	HDC          hdc;


	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//a class for the pattern window
	wndClass.style          = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc    = WndProc;
	wndClass.cbClsExtra     = 0;
	wndClass.cbWndExtra     = 0;
	wndClass.hInstance      = hInstance;
	wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName   = NULL;
	wndClass.lpszClassName  = TEXT("ActiveLighing");

	//class for the GUIish window
	wndClass2.style          = CS_HREDRAW | CS_VREDRAW;
	wndClass2.lpfnWndProc    = TxtProc;
	wndClass2.cbClsExtra     = 0;
	wndClass2.cbWndExtra     = 0;
	wndClass2.hInstance      = hInstance;
	wndClass2.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	wndClass2.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wndClass2.hbrBackground  = CreateSolidBrush(RGB(199, 248,175));
	wndClass2.lpszMenuName   = NULL;
	wndClass2.lpszClassName  = TEXT("Scene Information");





	//If you forget to register your windows they will not show 
	RegisterClass(&wndClass);
	RegisterClass(&wndClass2);




	//handle to the patterns window
	hWnd = CreateWindow(
		TEXT("ActiveLighing"),    // window class name
		TEXT("ActiveLighing"),    // window caption
		WS_POPUP,                 // window style
		1920,                     // initial x position
		0,                        // initial y position
		width,                    // initial x size
		height,                   // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters



	//handle to the GUIish window
	hTwnd = CreateWindow(
		TEXT("Scene Information"), 
		TEXT("Scene Information"), 
		WS_OVERLAPPEDWINDOW  | ES_MULTILINE ,
		100,
		100,
		540,
		500,
		NULL, 
		NULL,
		hInstance, 
		NULL);


	ShowWindow(hWnd, iCmdShow);
	ShowWindow(hTwnd, iCmdShow);
	UpdateWindow(hTwnd);
	ShowCursor(TRUE);
	hdc = BeginPaint(hWnd, &ps);




	startOrangeRelay();
	startGreenRelay();
	startSwitcher();
	start();
	Sleep(1000);
	turnOnAll();

	sendCommand("d allunitsoff");

	
	//reactions to key pressing. I found out that sleep works horribly when
	//called from the WinProc. 
	int ret = 0;
	while((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		// i stands for "idle"
		if(stage =='i')
		{
			char all [100];
			sprintf(all, "%sorig\\calibration\\%%O", path);
			SetOutputPath(all);
		}

		//d stands for decoding
		if (stage == 'd')
		{
			decode();
			stage = 'i';
		}

		//s stands for shoot
		if(stage =='s')
		{
			setQuality(4);
			char all [200];
			char num[5];
			int j;
			for (int t =0; t<exposures.size(); t++) //for all exposures
			{ 
				if (exposures.at(t) == 0)
					break;
				while (SetShutterAperture(exposures.at(t), -1))
				{
				}
				for(p=1; p<=P; p++) //and all projectors
				{
					turnOffAll();
					turnOn(p);
					Sleep(200);
					shootSeries(hdc, hWnd, t);
					
					if (zzz.at(t) <=1200)
						Sleep((1200 -(zzz.at(t)))*40);
					else
						Sleep (zzz.at(t)*5);
				}
			}
			
			stage = 'i';
			takes++;
		}

	
		// a stands for ambient
		if(stage =='a')
		{  
			
			shootAmbient(0);
			stage = 'i';
			L++;
		}

		// b stands for ambient with ball
		if(stage =='b')
		{  
			
			shootAmbient(1);
			stage = 'i';
			L++;
		}

		if(stage =='l')
		{  
			
			shootBall();
			stage = 'i';
			L++;
		}
		// m for medium quality ambient
		if(stage =='m')
		{  
			setQuality(4);
			shootAmbient(0);
			stage = 'i';
			L++;
		}

		//w stands for white
		else if(stage =='w')
		{

			projectWhite(hdc);
			stage = 'i';
		}

		//g stands for greycode
		else if(stage =='g')
		{

			OnPaint(hdc, 0, 1, 0);
			stage = 'i';
		}

		//c was already taken for calibration, so x stands for checkboard :)
		else if(stage =='x')
		{
			projectCheckerBoard(hdc);
			stage = 'i';
		}

		//pixel checkerboard
		else if(stage =='p')
		{
			projectPixelCheckerBoard(hdc);
			stage = 'i';
		}

		//calibration
		else if(stage == 'c')
		{
			setQuality(4);
			liveViewShow();
			DisplayCalibrationMessageBox();
			c++;
			stage = 'i';
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
	endOrangeRelay();
	endGreenRelay();
	stopSwitcher ();
	stop();
	return msg.wParam;
}  







//this takes care of the messages sent to the pattern window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{

	static HWND hwndEdit;
	PAINTSTRUCT  ps;
	HDC          hdc;
	ofstream    info;
	char		all[200];

	switch(message)
	{

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc, 1, 1, 0);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		return 0;

		//controling the ambiental lighting with alt + number
	case WM_SYSKEYDOWN:

		if (wParam>= 49 || wParam<=57)
		{ 
			lightSwitch(wParam - 48, 'C');
		}

		else if (wParam== 'G')
		{   
			G = !G;
		}
		else

			return 0;

	case WM_KEYDOWN:
		if(wParam==VK_ESCAPE)
		{	
			saveInfo();
			PostQuitMessage(0);
		}
	
		else if (wParam== 'A')
			stage = 'a';
		
		else if (wParam== 'B')
		{
			stage = 'b';

		}

		else if (wParam== 'C')
		{
			stage = 'c';

		}
		else if (wParam== 'D')
		{
			stage = 'd';
		}
		else if (wParam== 'G')
		{
			stage = 'g';
		}
			else if (wParam== 'L')
		{
			stage = 'l';
		}
		else if (wParam== 'S')
		{
			stage = 's';
		}
		else if (wParam== ('P'))
		{   
			stage = 'p';
		}
		else if (wParam== 'W')
		{
			stage = 'w';
		}
		else if (wParam== 'X')
		{
			stage = 'x';
		}
		else if (wParam== 'M')
		{
			stage = 'm';
		}
		else if (wParam== 'T')
		{
			saveInfo();
		}
		else if (wParam>= 49 || wParam<=57)
		{ 
			projectorSwitch(wParam - 48);
		}
		else

			return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}



} // WndProc


//takes care of messages sent to the GUIish window
LRESULT CALLBACK TxtProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND button, lab1, lab2, lab3, lab4, lab5, lab6, lab7, lab8, lab9, lab10, lab11, lab12, edit1, edit2, edit3, edit4, edit5, edit6, edit7, edit8, edit9, edit10, edit11, edit12;
	static HWND hwndEdit, lbox1;
	PAINTSTRUCT  ps;
	HDC          hdc;
	RECT rect;
	char temp [300];
	char all[100];
	ofstream info;
	int i;

	char a [50] = "Ohao!";

	switch(msg)
	{

	case WM_CTLCOLORSTATIC:
		{
			HDC hdc=(HDC)wParam;
			SetBkMode(hdc, TRANSPARENT);
		}
		return (INT_PTR)GetStockObject(NULL_BRUSH); 

	//creates all the labels, textboxes and buttons.
	case WM_CREATE:



		lab1 = CreateWindow("static","Exposure time (s) 1:", WS_CHILD|WS_VISIBLE , 10,10,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab1,SW_SHOW);
		edit1 = CreateWindow("combobox", "Default" ,WS_CHILD |LBS_STANDARD | CBS_SIMPLE | CBS_DROPDOWN , 160,10,100,300, hWnd, NULL,((LPCREATESTRUCT) lParam)->hInstance, NULL);
		for (i=0; i<36; i++)
			SendMessage(edit1, CB_ADDSTRING, NULL, (LPARAM) ext[i]);
		SendMessage(edit1, CB_SETCURSEL, 0, 0);
		ShowWindow(edit1,SW_SHOW);


		lab6 = CreateWindow("static", "Exposure time (s) 4:", WS_CHILD|WS_VISIBLE , 270,10,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab6,SW_SHOW);
		edit6 = CreateWindow("combobox", "Default" ,WS_CHILD |LBS_STANDARD | CBS_SIMPLE | CBS_DROPDOWN , 420,10,100,300, hWnd, NULL,((LPCREATESTRUCT) lParam)->hInstance, NULL);
		for (i=0; i<36; i++)
			SendMessage(edit6, CB_ADDSTRING, NULL, (LPARAM) ext[i]);
		SendMessage(edit6, CB_SETCURSEL, 0, 0);
		ShowWindow(edit6,SW_SHOW);


		lab2 = CreateWindow("static","Exposure time (s) 2:",WS_CHILD, 10,30,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab2,SW_SHOW);
		edit2 = CreateWindow("combobox", "Default" ,WS_CHILD |LBS_STANDARD | CBS_SIMPLE | CBS_DROPDOWN , 160,30,100,300, hWnd, NULL,((LPCREATESTRUCT) lParam)->hInstance, NULL);
		for (i=0; i<36; i++)
			SendMessage(edit2, CB_ADDSTRING, NULL, (LPARAM) ext[i]);
		SendMessage(edit2, CB_SETCURSEL, 0, 0);
		ShowWindow(edit2,SW_SHOW);


		lab7 = CreateWindow("static", "Exposure time (s) 5:", WS_CHILD|WS_VISIBLE , 270,30,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab7,SW_SHOW);
		edit7 = CreateWindow("combobox", "Default" ,WS_CHILD |LBS_STANDARD | CBS_SIMPLE | CBS_DROPDOWN , 420,30,100,300, hWnd, NULL,((LPCREATESTRUCT) lParam)->hInstance, NULL);
		for (i=0; i<36; i++)
			SendMessage(edit7, CB_ADDSTRING, NULL, (LPARAM) ext[i]);
		SendMessage(edit7, CB_SETCURSEL, 0, 0);
		ShowWindow(edit7,SW_SHOW);

		lab3 = CreateWindow("static","Exposure time (s) 3:",WS_CHILD, 10,50,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab3,SW_SHOW);
		edit3 = CreateWindow("combobox", "Default" ,WS_CHILD |LBS_STANDARD | CBS_SIMPLE | CBS_DROPDOWN , 160,50,100,300, hWnd, NULL,((LPCREATESTRUCT) lParam)->hInstance, NULL);
		for (i=0; i<36; i++)
			SendMessage(edit3, CB_ADDSTRING, NULL, (LPARAM) ext[i]);
		SendMessage(edit3, CB_SETCURSEL, 0, 0);
		ShowWindow(edit3,SW_SHOW);

		lab8 = CreateWindow("static", "Exposure time (s) 6: ", WS_CHILD|WS_VISIBLE , 270,50,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab8,SW_SHOW);
		edit8 = CreateWindow("combobox", "Default" ,WS_CHILD |LBS_STANDARD | CBS_SIMPLE | CBS_DROPDOWN , 420,50,100,300, hWnd, NULL,((LPCREATESTRUCT) lParam)->hInstance, NULL);
		for (i=0; i<36; i++)
			SendMessage(edit8, CB_ADDSTRING, NULL, (LPARAM) ext[i]);
		SendMessage(edit8, CB_SETCURSEL, 0, 0);
		ShowWindow(edit8,SW_SHOW);


		lab4 = CreateWindow("static","Aperture: ",WS_CHILD, 10,70,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab4,SW_SHOW);
		edit4 = CreateWindow("combobox", "Default" ,WS_CHILD |LBS_STANDARD | CBS_SIMPLE | CBS_DROPDOWN , 160,70,100,300, hWnd, NULL,((LPCREATESTRUCT) lParam)->hInstance, NULL);
		for (i=0; i<24; i++)
			SendMessage(edit4, CB_ADDSTRING, NULL, (LPARAM) ap[i]);
		SendMessage(edit4, CB_SETCURSEL, 0, 0);
		ShowWindow(edit4,SW_SHOW);


		lab10 = CreateWindow("static","Scene name: ",WS_CHILD  | WS_VISIBLE , 10,100,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab10,SW_SHOW);
		edit10 = CreateWindow(TEXT("edit"), NULL,	WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | WS_BORDER ,
			160, 100, 100, 20, hWnd, (HMENU) ID_EDIT,	((LPCREATESTRUCT) lParam)->hInstance, NULL);

		lab11 = CreateWindow("static", "# of projectors ", WS_CHILD|WS_VISIBLE , 270,100,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab11,SW_SHOW);
		edit11 = CreateWindow(TEXT("edit"), NULL,	WS_CHILD | WS_VISIBLE | WS_BORDER  | ES_MULTILINE ,
			420, 100, 100, 20, hWnd, (HMENU) ID_EDIT,	((LPCREATESTRUCT) lParam)->hInstance, NULL);

		lab12 = CreateWindow("static","Directory : ",WS_CHILD  | WS_VISIBLE , 10,120,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab12,SW_SHOW);
		edit12 = CreateWindow(TEXT("edit"), NULL,	WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOHSCROLL | WS_BORDER ,
			160, 120, 360, 20, hWnd, (HMENU) ID_EDIT,	((LPCREATESTRUCT) lParam)->hInstance, NULL);


		lab5 = CreateWindow("static","Additional information:",WS_CHILD, 10,150,150,20,hWnd,NULL,NULL,0);
		ShowWindow(lab5,SW_SHOW);
		hwndEdit = CreateWindow(TEXT("edit"), NULL,	WS_CHILD | WS_VISIBLE | WS_BORDER  | ES_MULTILINE,
			10, 175, 510, 200, hWnd, (HMENU) ID_EDIT,
			((LPCREATESTRUCT) lParam)->hInstance, NULL);

		button = CreateWindow(  TEXT("BUTTON"),"OK",WS_CHILD | WS_VISIBLE ,380,380,100,20,hWnd,(HMENU) 2,NULL ,NULL );
		ShowWindow(button,SW_SHOW); 
		return 0;

	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		return 0;

	case WM_COMMAND:
		if(LOWORD(wParam) == ID_EDIT)
		{
			if(HIWORD(wParam) == EN_ERRSPACE || HIWORD(wParam) == EN_MAXTEXT)


				if(HIWORD(wParam) == EN_UPDATE) 
				{
					GetWindowText(hwndEdit, szText, 5000);

					InvalidateRect(hWnd, NULL, TRUE);
				}
		}

		//handles the OK button click
		else if (LOWORD(wParam) == 2)
		{
		
			parse(); 
		

			i = SendMessage (edit1, CB_GETCURSEL, 0, 0);
			if (i!=0){	
				zzz[0] = ex[i];
				exposures[0] = i;
			}

			i = SendMessage (edit2, CB_GETCURSEL, 0, 0);
			if ( i!=0){
				zzz[1] = ex[i];
				exposures[1] = i;
			}

			

			i = SendMessage (edit3, CB_GETCURSEL, 0, 0);
			if (i!=0){
				zzz[2] = ex[i];
				exposures[2] = i;
			}

			i= SendMessage (edit4, CB_GETCURSEL, 0, 0);
			if ( i!=0)
			{
				SetShutterAperture(-1, i-1);
				sprintf(aperture, "%s", ap[i]);
			}

			i = SendMessage (edit6, CB_GETCURSEL, 0, 0);
			if ( i!=0){
				zzz[3] = ex[i];
				exposures[3] = i;
			}

			i = SendMessage (edit7, CB_GETCURSEL, 0, 0);
			if (i!=0){
				zzz[4] = ex[i];
				exposures[4] = i;
			}

			i = SendMessage (edit8, CB_GETCURSEL, 0, 0);
			if ( i!=0){
				zzz[5] = ex[i];
				exposures[5] = i;
			}


			GetWindowText(edit10, temp, 200);
			if ( strcmp(temp,"") !=0)
				sprintf(sceneName, "%s", temp);
			else 
				sprintf(sceneName, "%s", scname);

			GetWindowText(edit11, temp, 300);
			if (atoi(temp) != 0)
				P = atoi(temp);




			//
			GetWindowText(edit12, all, 200);
			if (strcmp(all, "")!=0)
				sprintf(path, "%s\\%s\\", all, sceneName);
			else
				sprintf(path, "%s\\%s\\", dir, sceneName);

			DisplayErrorMessageBox(path);

			CreateDir(path);

			GetWindowText(hwndEdit, szText, 5000);

			for (int i = 1; i<=P; i++)
			{

			}
			DestroyWindow(hWnd);

		}
		return 0;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		DrawText(hdc, szText, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		DestroyWindow(hWnd);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

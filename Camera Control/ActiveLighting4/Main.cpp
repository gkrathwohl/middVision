//#include "GUI.h"
//#include "Greycode.h"
//#include "OtherCode.h"
//#include "CameraControl.h"
//#include "RelayControl.h"
//#include "DSLRRemoteLib.h"
//#include "parse.h"
//#include "SerialUtils.h"
//#include "SwitcherCtrl.h"
//#include "X10Ctrl.h"
//#include "imageLib.h"
//#include "Utils.h"
//#include "flowIO.h"
//#include "assert.h"
//#include <string.h>
//#include <tchar.h>
//#include <windows.h>
//#include <gdiplus.h>
//#include <iostream>
//#include <fstream>
//#include <time.h>
//#include ".\\imageLib\\imageLib.h"
//#include "Utils.h"
//
//void main ()
//{
//	CByteImage img;
//	ReadImage (img, "C:\\a\\Boxes\\orig\\greycode\\take1exp0P1\\right\\IMG_0001.JPG");
//
//
//
//
//
//}
//void thresholdConfidence(CByteImage imOut, int certain);
//
//
//void updateConfidence(CByteImage imIn, CByteImage imOut);
//
//
//// threshold images (defined in GreyThresh.cpp)
//void greyThresh(char* outdir, char *imm1,  char *imm2, int num);
//
//// decode images (defined in GreyDecode.cpp)
//CFloatImage greyDecode(char* outdir, char* codefile, int direction, char **imList, int numIm);
//
//// combine multiple confidence maps into one (defined in GreyThresh.cpp)				
//void combineConfidence(char *outfile, char **imList, int numIm, int certain);
//
//// compute pair of disparity maps from labeled (pmf) images (defined in Disparity.cpp)
//void computeDisparities(char *outdir,char *in0, char *in1, int dXmin, int dXmax, int dYmin, int dYmax);
//
//CFloatImage simpleCrossCheck(CFloatImage im0, CFloatImage im1);
//
//
//void main()
//{
//
//   
//    char all[200];
//    CByteImage imm;
//    CByteImage base;
//    char outdir [200] = "C:\\a\\Pots\\decoded\\left\\";
//	char path [200] = "C:\\a\\Pots\\";
//    char codefile [50] = "minSW.dat";
//    for (int k = 1; k<=20; k++)
//    {
//       
//        sprintf (all, "%sconfidence\\exp0P1\\left\\thresh%d.pgm", path, k);
//        ReadImage (base, all);
//        for (int i = 0; i< 3; i++)
//        { //for all exposures
//            for (int j = 1; j<3; j++)
//            { //and all projectors
//                sprintf (all, "%sconfidence\\exp%dP%d\\left\\thresh%d.pgm", path, i, j, k);
//                ReadImage (imm, all);
//                updateConfidence(base, imm);
//               
//            }
//        }
//        thresholdConfidence(base, 120);
//        sprintf (all, "%sthreshold\\left\\conf%d.pgm", path, k);
//        WriteImage(base, all);
//    }
//
//   char** imList = new char*[20];
//    for (int i = 0; i<20; i++)
//    {   
//        char* bla = new char[200];
//        sprintf (bla, "%sthreshold\\left\\conf%d.pgm", path, i+1);
//		imList[i] = bla;
//       
//    }
//
//      greyDecode(outdir, codefile, 0, imList, 10);
//      greyDecode(outdir, codefile, 1, &imList[10], 10);
//
//    delete[] imList;
//
//	
//
//	DisplayErrorMessageBox("DONE!");
//}
//
//
//
//
//
////
////
////
//////}
//////int main(){
//////	int p = 1;
//////	int t = 0;
//////	char num [5];
//////	char all[200];
//////	char path [200] = "C:\\a\\Dots\\";
//////	sprintf(all, "%sppms\\exp%dP%d\\left", path, t, p);
//////	CreateDir(all);
//////			sprintf(all, "%sppms\\exp%dP%d\\right", path, t, p);
//////	for (int i =1;i<=40; i++)
//////	{		
//////		if (i<10)
//////			sprintf (num, "000%d", i);
//////		else 
//////			sprintf (num, "00%d", i);
//////		sprintf (all, "C:\\ImageMagick-6.7.0-Q16\\convert.exe %sorig\\greycode\\exp%dP%d\\left\\IMG_%s.JPG %sppms\\exp%dP%d\\left\\IMG_%s.ppm ", path,t, p, num,path,t, p, num);
//////		system(all);
//////	}
//////}
//////	//
//////	//
//////	//#include <stdio.h>
//////	//#include <iostream>
//////	//#include "RelayControl.h"
//////	//#include "SwitcherCtrl.h"
//////	//int main(){
//////	//
//////	//	int i = '1';
//////	//	printf("%d", i);
//////	//
//////	//return 0;
//////	//
//////	//}
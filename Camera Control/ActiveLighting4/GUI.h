#include <windows.h>
#include <stdio.h> 
#include <vector>

using std::vector;

extern HWND remotePro;
extern HWND hWnd;

extern FILE *fr; 
extern int ex [37];
extern char *ext [37];
extern char *ap [24];

extern int quality;
extern int c; 
extern int L;
extern int l;
extern int takes;
extern int P; //number of projectors used
extern int projectors [8]; //keeps track of wether a projector is on or off - simpler than quiering the switcher
extern int lights[8]; //keeps track of wether the ambient lights are on or off
extern int allLights [5][8];
extern vector<int> zzz; //exposure times in ms
extern vector<int> zzzAmb; 
extern vector<int> exposures;//exposures
extern vector<int> exposuresAmb;
extern int enablePS;
extern char aperture[];	
extern char dir[100];
extern char scname[100];

extern char path[200];// = "C:\\a"; //directory name where to store the picture
extern char sceneName [200];// = "Basket Scene"; //name of the current scene

const static int width = 1024; // dimensions for gray code window
const static int height = 768;

extern BYTE** minSW;
extern char lightConditions[1000]; //for a description of the ambient light conditions 
extern TCHAR szText[5000];
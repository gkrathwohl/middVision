#include "CameraControl.h"
#include "DSLRRemoteLib.h"
#include "RelayControl.h"
#include "GUI.h"
#include <stdio.h>
#include <windows.h>


//if necessary, changes the quality of the images
void setQuality(int q){
	if(q != quality){
		while(SetImageSizeQuality(q)){}
		quality = q;
	}
}

//keeps pinging both cameras till they are ready
void pingCameras()
{
bool p0 = false;
	bool p1 = false;
	while (!(p0&&p1))
	{
		SelectCamera(0);
		
		if (PingCamera()== 0)
			p0 = true;
		SelectCamera(1);
		if (PingCamera()== 0)
			p1 = true;
	}
}



//releases the shutter once the cameras are ready
void shoot()
{
	orangeRelayOn(0);
	orangeRelayOn(1);
	Sleep(50);
	reset();
}


//used in ambient shooting; for some reason,
//RPMC seems to crash less if there is a slight 
//delay between the two cameras. 
void ambientShoot()
{
	
	orangeRelayOn(0);
	Sleep(50);
	orangeRelayOn(1);
	Sleep(50);
	reset();
}


//shooting all the cameras
void shootAll()
{
	
	greenRelayOn(7);
	greenRelayOn(6);
	Sleep(1500);
	orangeRelayOn(0);
	orangeRelayOn(1);
	greenRelayOff(7);
	greenRelayOff(6);
	
	Sleep(1000);
	reset();
}


//shooting the little cameras
void shootPS()
{
	greenRelayOn(7);
	greenRelayOn(6);
	Sleep(1500);
	greenRelayOff(7);
	greenRelayOff(6);
	Sleep(1000);
	reset();
}


//a bit hakish way to show the live view window. 

void liveViewShow()
{   SetForegroundWindow (remotePro);
	ShowWindow(remotePro, SW_SHOWDEFAULT);
	//Sleep(2000);
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event(VK_SHIFT, 0, 0, 0);
	keybd_event('L', 0, 0, 0);

	::Sleep(1000);

	// stop pressing "Alt-Tab"

	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	keybd_event('L', 0, KEYEVENTF_KEYUP, 0);
	

}

void liveViewHide()
{ 
	HWND liveView = FindWindow(NULL,"Live View (all cameras)"); //
	SetForegroundWindow (liveView);
	keybd_event(VK_ESCAPE, 0, 0, 0);
	Sleep(1000);
	keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
	SetForegroundWindow (hWnd);

}
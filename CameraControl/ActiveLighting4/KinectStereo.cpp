//// KinectStereo.cpp : Defines the entry point for the console application.
////
//
////#include "stdafx.h"
//
//
//#include "X10Ctrl.h"
//#include "RelayControl.h"
//
//
//////brad added these:
////#include <fstream>
////#include <iostream>
////#include <stdlib.h>
////#include <stdio.h>
////#include <string.h>
////
////using namespace std;
//
//
//int main()
//{
//
//	//X10Ctrl x10;
//
//	//if (1) { // test GPIO
//	//	if(! openPort ("com6")) {
//	//		printf("cannot connect\n");
//	//	} else {
//	//		for (int i=0; i < 5; i++) {
//	//			printf("%d\n", i);
//	//			relayOn(7);
//	//			Sleep(2000);
//	//			relayOff(7);
//	//			Sleep(2000);
//	//		}
//	//		closePort ();
//	//	}
//	//	}
//
//	 //start the x10 controller
//		bool status = start();
//		if (!status) {
/////			fprintf(stderr, "Can't start X10 controller - exiting.\n");
//			exit(1);
//		}
//		sendCommand("D2 on");
//		Sleep(500);
//		sendCommand("D2 off");
//	
//
//	
//
//	return 0;
//
//}
//

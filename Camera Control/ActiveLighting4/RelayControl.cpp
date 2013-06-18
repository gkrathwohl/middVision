// RelayControl.cpp: defines procedures for controling the relays
//


#include "RelayControl.h"
#include "SerialUtils.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <windows.h>
using namespace std;

// global variable for open serial port
HANDLE orangeRelaysHandle;
HANDLE greenRelaysHandle;

void startOrangeRelay (){
orangeRelaysHandle = openPort ("Com4");
}

void startGreenRelay (){
greenRelaysHandle = openPort ("Com7");
}


void endGreenRelay(){
	closePort (greenRelaysHandle);\
}

void endOrangeRelay(){
	closePort (orangeRelaysHandle);\
}

//turns the specified relay on
void greenRelayOn (int relay)
{
	char command[100];
	BOOL writeStatus;
	DWORD bytesWritten;
	sprintf(command, "gpio set %d\r", relay);
	if (greenRelaysHandle == NULL)
	{
		printf("cannot connect to serial port com3!!\n");
	}
	else {
		FlushFileBuffers(greenRelaysHandle);
		writeStatus = WriteFile(greenRelaysHandle, command, strlen(command), &bytesWritten, NULL); //send command
	}
}


//turns the specified relay off
void greenRelayOff (int relay)
{
	char command[100];
	BOOL writeStatus;
	DWORD bytesWritten;
	sprintf(command, "gpio clear %d\r", relay);
	if (greenRelaysHandle == NULL)
	{
		printf("cannot connect to serial port com3!!\n");
	}
	else {
		FlushFileBuffers(greenRelaysHandle);
		writeStatus = WriteFile(greenRelaysHandle, command, strlen(command), &bytesWritten, NULL); //send command
	}
}

//turns the specified relay on
void orangeRelayOn (int relay)
{
	char command[100];
	BOOL writeStatus;
	DWORD bytesWritten;
	sprintf(command, "relay on %d\r", relay);
	if (orangeRelaysHandle == NULL)
	{
		printf("cannot connect to serial port com6!!\n");
	}
	else {
		FlushFileBuffers(orangeRelaysHandle);
		writeStatus = WriteFile(orangeRelaysHandle, command, strlen(command), &bytesWritten, NULL); //send command
		FlushFileBuffers(orangeRelaysHandle);
	}
}




//turns the specified relay on
void orangeRelayOff (int relay)
{
	char command[100];
	BOOL writeStatus;
	DWORD bytesWritten;
	sprintf(command, "relay off %d\r", relay);
	if (orangeRelaysHandle == NULL)
	{
		printf("cannot connect to serial port com6!!\n");
	}
	else {
		FlushFileBuffers(orangeRelaysHandle);
		writeStatus = WriteFile(orangeRelaysHandle, command, strlen(command), &bytesWritten, NULL); //send command
		FlushFileBuffers(orangeRelaysHandle);
	}
}






//resets all orandge relays
void reset()
{
	char *command = "reset\r";
	BOOL writeStatus;
	DWORD bytesWritten;
	if (orangeRelaysHandle == NULL)
	{
		printf("cannot connect to serial port com6!!\n");
	}
	else
	{
		FlushFileBuffers(orangeRelaysHandle);
		writeStatus = WriteFile(orangeRelaysHandle, command, strlen(command), &bytesWritten, NULL); //send command
		FlushFileBuffers(orangeRelaysHandle);
	}
}





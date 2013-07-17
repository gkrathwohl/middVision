////#include "stdafx.h"
#include <windows.h>
#include "SwitcherCtrl.h"
#include "SerialUtils.h"


HANDLE SwitcherHandle;

void startSwitcher () {
	const char* comPort = "Com3"; // the com port - should be one of Com1-Com4
	//openPort(comPort);
	// return true if comHandle is not null
	SwitcherHandle = openPort (comPort);
}

bool turnOn (int which) {
	// creates the command - see docs for explanation of syntax
	DWORD bytesWritten; // number of bytes written by WriteFile
	DWORD bytesRead; // number of bytes read by ReadFile
	BOOL bWriteRC; // holder for status of write operation
	BOOL bReadRC; // holder for status of read operation
	unsigned int cmd = 0x81808101 + (which << 16);
	unsigned int response = 0; // holder for response data
 	bWriteRC = WriteFile(SwitcherHandle, &cmd, 4, &bytesWritten, NULL); // send command
	bReadRC = ReadFile(SwitcherHandle, &response, 4, &bytesRead, NULL); // get response
	if (bytesRead != 4 || response != (cmd + 0x40)) { // it didn't work
		return false;
	} else {
		return true;
	}
}

bool turnOnAll () {
	return turnOn(0); // number 0 controls all of them at once
}

bool turnOff (int which) {
	// creates the command - see docs for explanation of syntax
	DWORD bytesWritten; // number of bytes written by WriteFile
	DWORD bytesRead; // number of bytes read by ReadFile
	BOOL bWriteRC; // holder for status of write operation
	BOOL bReadRC; // holder for status of read operation
	unsigned int cmd = 0x81808001 + (which << 16);
	unsigned int response = 0; // holder for response data
 	bWriteRC = WriteFile(SwitcherHandle, &cmd, 4, &bytesWritten, NULL); // send command
	bReadRC = ReadFile(SwitcherHandle, &response, 4, &bytesRead, NULL); // get response
	if (bytesRead != 4 || response != (cmd + 0x40)) { // it didn't work
		return false;
	} else {
		return true;
	}
}

bool turnOffAll () {
	return turnOff(0); // number 0 controls all of them at once
}

void stopSwitcher () {
	closePort(SwitcherHandle);
}
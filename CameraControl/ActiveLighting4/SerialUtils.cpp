//SerialUtils.cpp: controls opening and closing of ports

#include "SerialUtils.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;






HANDLE openPort (const char* port) {
//	 open connection with the serial port
	HANDLE comHandle = CreateFile(port,
		GENERIC_READ | GENERIC_WRITE,
		0, // exclusive access (no sharing)
		NULL, // no security
		OPEN_EXISTING,
		0, // no overlapped I/O
		NULL ); // null template

	if (comHandle == INVALID_HANDLE_VALUE) { // handle error
		return NULL; // signal error
	}

	BOOL m_bPortReady; // keep track of success of setup methods (for debugging)
	// set the properties of the serial port
	DCB m_dcb; // holder for com port settings
	m_bPortReady = SetupComm(comHandle, 128, 128); // set buffer sizes
	m_bPortReady = GetCommState(comHandle, &m_dcb); // gets settings
	// sets settings
	m_dcb.BaudRate = CBR_9600;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.fAbortOnError = TRUE;
	// update with new settings
	m_bPortReady = SetCommState(comHandle, &m_dcb);

	 //set timeouts for communications
	COMMTIMEOUTS m_CommTimeouts; // for com port timeout settings
	m_bPortReady = GetCommTimeouts (comHandle, &m_CommTimeouts);  // get settings
	 //set settings
	m_CommTimeouts.ReadIntervalTimeout = 50;
	m_CommTimeouts.ReadTotalTimeoutConstant = 50;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 10;
	m_CommTimeouts.WriteTotalTimeoutConstant = 50;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 10;
	// update the port with new settings
	m_bPortReady = SetCommTimeouts (comHandle, &m_CommTimeouts);

	return comHandle;
}






void closePort (HANDLE comHandle) 
{
	// return true if CloseHandle returns nonzero
	 CloseHandle(comHandle);
}

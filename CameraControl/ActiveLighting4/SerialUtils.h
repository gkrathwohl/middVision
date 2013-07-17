#include <windows.h>


HANDLE openPort (const char* port);
void closePort (HANDLE comHandle);
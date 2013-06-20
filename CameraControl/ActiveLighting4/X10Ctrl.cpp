// Control X10 devices
// Daniel Scharstein 2/26/07
// adapted from ahcmd.cpp sample program

//#include "stdafx.h"
#include "X10Ctrl.h"
#include <objbase.h>
#import "progid:X10.ActiveHome" 


	ActiveHomeScriptLib::IActiveHome* pActiveHome;

// start the X10 controller
bool start() {
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	HRESULT hr = CoCreateInstance( __uuidof(ActiveHomeScriptLib::ActiveHome), NULL, CLSCTX_INPROC, __uuidof(ActiveHomeScriptLib::IActiveHome), (LPVOID *) &pActiveHome );
	return SUCCEEDED(hr);
}

// send an  , e.g. "D1 on"
int sendCommand(char *cmd) {
	int nReturn = 0;
	_variant_t varReseved1;
	_variant_t varReseved2;
	_variant_t varReturn;

	_variant_t varAction(("sendplc"));
	_variant_t varParm((cmd));

	try {
		varReturn = pActiveHome->SendAction(varAction, varParm, varReseved1, varReseved2);
		nReturn = varReturn;
	} catch(_com_error& theException) {
		IErrorInfo* pErrorInfo;
		if (pErrorInfo = theException.ErrorInfo()) {
			BSTR bstrError;
			pErrorInfo->GetDescription(&bstrError);
			printf("%ls\n", bstrError);
			pErrorInfo->Release();
		}
	}
	Sleep(1000); // it takes a while for the lamp to turn on...
	//printf("%c", 'G'-'@'); // CTRL-G (bell)

	return nReturn;
}

// stop the X10 controller
void  stop() {
	int nRet = pActiveHome->Release();
	pActiveHome = NULL;
	CoUninitialize();
}

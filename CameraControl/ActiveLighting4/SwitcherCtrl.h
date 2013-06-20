#include <windows.h>
	
extern HANDLE SwitcherHandle;

    void startSwitcher (); // opens the com port
	bool turnOn (int which); // turns the given output on (1-8)
	bool turnOnAll (); // turn on all outputs
	bool turnOff (int which); // turns the given output off (1-8)
	bool turnOffAll (); // turn off all outputs
	void stopSwitcher (); // closes the com port

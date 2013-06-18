#include <windows.h>

extern HANDLE greenRelaysHandle;
extern HANDLE orangeRelaysHandle;


void startOrangeRelay ();
void startGreenRelay ();
void endGreenRelay();
void endOrangeRelay();
void greenRelayOn (int relay);
void greenRelayOff (int relay);
void orangeRelayOn (int relay);
void orangeRelayOff (int relay);
void reset();


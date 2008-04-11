#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdlib.h>
#include <assert.h>
#include "..\system.h"

extern HDC hdc;
LARGE_INTEGER gPerfCounterFrequency;
LARGE_INTEGER gInitialPerfCount;

extern unsigned char keysDown[255];
extern unsigned char keysPressed[255];

void SYS_Error( const char* error, ... )
{
	// AJT: on win32 we just ignore the arguments.
	MessageBox( NULL, error, "Fatal Error", MB_OK );
	exit( 0 );
}

void SYS_SwapBuffers()
{
	SwapBuffers( hdc );
}


void SYS_TimerInit()
{
	int retVal = QueryPerformanceFrequency( &gPerfCounterFrequency );
	assert( retVal != 0 );	
	QueryPerformanceCounter( &gInitialPerfCount );
}

float SYS_GetTimeSeconds()
{
	LARGE_INTEGER perfCount;
	__int64 ticks;

	QueryPerformanceCounter( &perfCount );

	ticks = perfCount.QuadPart - gInitialPerfCount.QuadPart;
	return (float)(ticks / (double)gPerfCounterFrequency.QuadPart);
}

unsigned long SYS_GetTimeMS()
{
	LARGE_INTEGER perfCount;
	__int64 ticks;

	QueryPerformanceCounter( &perfCount );

	ticks = perfCount.QuadPart - gInitialPerfCount.QuadPart;
	return (unsigned long)((ticks * 1000) / (double)gPerfCounterFrequency.QuadPart);
}

unsigned long SYS_GetTimeMicro()
{
	LARGE_INTEGER perfCount;
	__int64 ticks;

	QueryPerformanceCounter( &perfCount );

	ticks = perfCount.QuadPart - gInitialPerfCount.QuadPart;
	return (unsigned long)((ticks * 1000000) / (double)gPerfCounterFrequency.QuadPart);
}

// keyboard input
void SYS_InitKeyboard()
{
	// someday this might have to do some direct x stuff.
	;
}

// returns 1 if the given key is currently held down.
int SYS_KeyDown( unsigned char key )
{
	return keysDown[key];
}

// returns 1 if the given key has just been pressed, will return repeats, useful for typing.
int SYS_KeyPressed( unsigned char key )
{
	return keysPressed[key];
}


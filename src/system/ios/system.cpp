#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "../../gameloop.h"
#include "../system.h"

unsigned char keysDown[255];
unsigned char keysPressed[255];

// AJT: FIXME, get this from ViewController.mm
int g_width = 320;
int g_height = 480;

/*
void SYS_Error( const char* error, ... )
{
	// TODO: handle variable arguments
	fprintf( stderr, "%s", error );
	exit( 0 );
}
*/

void SYS_SwapBuffers()
{
	//glutSwapBuffers();
}

void SYS_TimerInit()
{

}

float SYS_GetTimeSeconds()
{
	clock_t t = clock();
	return (float)t / (float)CLOCKS_PER_SEC;
}

unsigned long SYS_GetTimeMS()
{
	return SYS_GetTimeSeconds() * 1000.0f;
}

unsigned long SYS_GetTimeMicro()
{
	return SYS_GetTimeSeconds() * 100000.0f;
}

// keyboard input
void SYS_InitKeyboard()
{
	// TODO:
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

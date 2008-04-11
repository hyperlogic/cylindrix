#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL/glut.h>
#include <time.h>
#include <string.h>
#include "../../gameloop.h"
#include "../system.h"

unsigned char keysDown[255];
unsigned char keysPressed[255];

void SYS_Error( const char* error, ... )
{
	// TODO: handle variable arguments
	fprintf( stderr, "%s", error );
	exit( 0 );
}

void SYS_SwapBuffers()
{
	glutSwapBuffers();
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

static void display()
{
	int quit = GameLoop();

	// clear keys pressed.
	memset( keysPressed, 0, 255 );
	memset( keysDown, 0, 255 );

	SYS_SwapBuffers();
}

static KeyValues asciiToKeyValue( unsigned char key )
{
	if ( key >= 'a' && key <='z' )
		return static_cast<KeyValues>(key - 'a');
	else if ( key >= 'A' && key <= 'Z' )
		return static_cast<KeyValues>(key - 'A');
	else
	{
		switch ( key )
		{
			case 0x08: return KEY_BACKSPACE;
			case 0x09: return KEY_TAB;
			case 0x0a: case 0x0d: return KEY_ENTER;
			case 0x1b: return KEY_ESC;
			case 0x20: return KEY_SPACE;
			default: return static_cast<KeyValues>(0xff);
		}
	}
}

static void keys( unsigned char key, int x, int y )
{
	keysPressed[ asciiToKeyValue( key ) ] = 1;
	keysDown[ asciiToKeyValue( key ) ] = 1;
}

static void specialKeys( int key, int x, int y )
{
	switch ( key )
	{
		case GLUT_KEY_LEFT:
			keysPressed[KEY_LEFT_ARROW] = 1;
			keysDown[KEY_LEFT_ARROW] = 1;
			break;
		case GLUT_KEY_RIGHT:
			keysPressed[KEY_RIGHT_ARROW] = 1;
			keysDown[KEY_RIGHT_ARROW] = 1;
			break;
		case GLUT_KEY_UP:
			keysPressed[KEY_UP_ARROW] = 1;
			keysDown[KEY_UP_ARROW] = 1;
			break;
		case GLUT_KEY_DOWN:
			keysPressed[KEY_DOWN_ARROW] = 1;
			keysDown[KEY_DOWN_ARROW] = 1;
			break;
		case GLUT_KEY_F2:
			keysPressed[KEY_ESC] = 1;
			keysDown[KEY_ESC] = 1;
			break;
	}
}

int main( int argc, char* argv[] )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutCreateWindow( "glcylindrix" );
	glutReshapeWindow( 640, 480 );
	glutDisplayFunc( display );
	glutIdleFunc( display );
	glutKeyboardFunc( keys );
	glutSpecialFunc( specialKeys );

	GameInit( argc, const_cast<const char**>(argv) );

	glutMainLoop();

	GameShutdown();

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <SDL/SDL.h>
#include "../../gameloop.h"
#include "../system.h"
#include <stdarg.h>

unsigned char keysDown[255];
unsigned char keysPressed[255];

void SYS_SwapBuffers()
{
	SDL_GL_SwapBuffers();
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
	memset( keysDown, 0, 255 );
	memset( keysPressed, 0, 255 );
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

static SDLKey keyToSDLKey[SIZEOFKEYTABLE] =
{
	SDLK_a, // KEY_A
	SDLK_b, // KEY_B
        SDLK_c, // KEY_C
        SDLK_d, // KEY_D
        SDLK_e, // KEY_E
        SDLK_f,	// KEY_F
        SDLK_g, // KEY_G
        SDLK_h, // KEY_H
	SDLK_i, // KEY_I
        SDLK_j,	// KEY_J,
        SDLK_k, // KEY_K,
        SDLK_l, // KEY_L,
        SDLK_m, // KEY_M,
        SDLK_n, // KEY_N,
        SDLK_o, // KEY_O,
        SDLK_p, // KEY_P,
        SDLK_q, // KEY_Q,
        SDLK_r, // KEY_R,
        SDLK_s, // KEY_S,
        SDLK_t, // KEY_T,
        SDLK_u, // KEY_U,
        SDLK_v, // KEY_V,
        SDLK_w, // KEY_W,
        SDLK_x, // KEY_X,
        SDLK_y, // KEY_Y,
        SDLK_z, // KEY_Z,
        SDLK_UP, // KEY_UP_ARROW,
        SDLK_DOWN, // KEY_DOWN_ARROW,
        SDLK_LEFT, // KEY_LEFT_ARROW,
        SDLK_RIGHT, // KEY_RIGHT_ARROW,
        SDLK_SPACE, // KEY_SPACE,
	SDLK_LCTRL, // KEY_CONTROL,
        SDLK_LALT, // KEY_ALT,
        SDLK_F1, // KEY_F1,
        SDLK_F2, // KEY_F2,
        SDLK_F3, // KEY_F3,
        SDLK_F4, // KEY_F4,
        SDLK_F5, // KEY_F5,
        SDLK_F6, // KEY_F6,
        SDLK_F7, // KEY_F7,
        SDLK_F8, // KEY_F8,
        SDLK_F9, // KEY_F9,
        SDLK_F10, // KEY_F10,
        SDLK_F11, // KEY_F11,
        SDLK_F12, // KEY_F12,
        SDLK_RETURN, // KEY_ENTER,
        SDLK_0, // KEY_0,
        SDLK_1, // KEY_1,
        SDLK_2, // KEY_2,
        SDLK_3, // KEY_3,
        SDLK_4, // KEY_4,
        SDLK_5, // KEY_5,
        SDLK_6, // KEY_6,
        SDLK_7, // KEY_7,
        SDLK_8, // KEY_8,
        SDLK_9, // KEY_9,
        SDLK_MINUS, // KEY_MINUS,
        SDLK_EQUALS, // KEY_EQUALS,
        SDLK_LEFTBRACKET, // KEY_LEFT_BRACKET,
        SDLK_RIGHTBRACKET, // KEY_RIGHT_BRACKET,
        SDLK_BACKSLASH, // KEY_BACKSLASH,
        SDLK_SEMICOLON, // KEY_SEMICOLON,
        SDLK_QUOTE, // KEY_APOSTROPHE,
        SDLK_COMMA, // KEY_COMMA,
        SDLK_PERIOD, // KEY_PERIOD,
        SDLK_SLASH, // KEY_SLASH,
        SDLK_BACKQUOTE, // KEY_TILDE,
        SDLK_TAB, // KEY_TAB,
        SDLK_LSHIFT, // KEY_SHIFT,
        SDLK_BACKSPACE, // KEY_BACKSPACE,
        SDLK_ESCAPE // KEY_ESC,
};

static void processKeys()
{
	const Uint8* keys = SDL_GetKeyState(0);

	for ( int i = 0; i < SIZEOFKEYTABLE; ++i )
	{
		SDLKey sdlKey = keyToSDLKey[i];
		
		if ( keys[sdlKey] )
		{
			keysPressed[i] = keysDown[i] ? 0 : 1;
			keysDown[i] = 1;
		}
		else
		{
			keysPressed[i] = 0;
			keysDown[i] = 0;
		}
	}
}

static bool process()
{
	processKeys();
	
	SYS_ProcessSound();

	bool done = GameLoop();

	SYS_SwapBuffers();

	return done;
}

int main( int argc, char* argv[] )
{
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		SYS_Error( "Couldn't init SDL!\n" );

	SDL_Surface* screen = SDL_SetVideoMode( 800, 600, 32, SDL_HWSURFACE | SDL_RESIZABLE | SDL_OPENGL );

	if ( !screen )
		SYS_Error( "Couldn't create SDL screen!\n" );

	GameInit( argc, const_cast<const char**>(argv) );

	bool done = false;
	while ( !done )
	{
		SDL_Event event;
		while ( SDL_PollEvent( &event ) )
		{
			switch ( event.type )
			{
				case SDL_QUIT:
					done = true;
					break;

				case SDL_VIDEORESIZE:
					screen = SDL_SetVideoMode( event.resize.w, event.resize.h, 32, SDL_HWSURFACE | SDL_RESIZABLE | SDL_OPENGL );
					break;
			}
		}

		if ( !done )
			done = process();
	}

	GameShutdown();
	SDL_Quit();

	return 0;
}

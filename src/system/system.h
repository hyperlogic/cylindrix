/*
    Copyright (C) 2001 Hotwarez LLC, Goldtree Enterprises
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; 
    version 2 of the License.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <string.h>

#ifndef SYSTEM_H
#define SYSTEM_H

// these should be specified by the game
extern const char* gameName;
extern void GameInit( int argc, const char* argv[] );
extern int GameLoop();
extern void GameShutdown();

// used for keyboard input.
enum KeyValues
{
	KEY_A = 0,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_UP_ARROW,
	KEY_DOWN_ARROW,
	KEY_LEFT_ARROW,
	KEY_RIGHT_ARROW,
	KEY_SPACE,
	KEY_CONTROL,
	KEY_ALT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_ENTER,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_MINUS,
	KEY_EQUALS,
	KEY_LEFT_BRACKET,
	KEY_RIGHT_BRACKET,
	KEY_BACKSLASH,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_TILDE,
	KEY_TAB,
	KEY_SHIFT,
	KEY_BACKSPACE,
	KEY_ESC,
	SIZEOFKEYTABLE
};
// AJT: TODO: add support for numpad and support for left/right shift, alt & ctrl

// AJT: TODO: msvc6 doesnt support varidac macros.
void SYS_Error( const char* error, ... );

void SYS_SwapBuffers();

// timer functions
void SYS_TimerInit();
float SYS_GetTimeSeconds();
unsigned long SYS_GetTimeMS();
unsigned long SYS_GetTimeMicro();

// keyboard input

// returns 1 if the given key is currently held down.
int SYS_KeyDown( unsigned char key );
// returns 1 if the given key has just been pressed, will return repeats, useful for typing.
int SYS_KeyPressed( unsigned char key );


// sound stuff
typedef unsigned int SYS_SOUNDHANDLE;
void SYS_InitSound();
void SYS_ShutdownSound();
SYS_SOUNDHANDLE SYS_LoadSound( char* fileName );
void SYS_ReleaseSound( SYS_SOUNDHANDLE soundHandle );

// plays this sound once, (fire and forget)
void SYS_TriggerSound( SYS_SOUNDHANDLE soundHandle );
void SYS_TriggerSoundVolume( SYS_SOUNDHANDLE soundHandle, unsigned char volume );

void SYS_SetListenerOrientation( float position[3], float forward[3], float up[3] );
void SYS_TriggerSound3D( SYS_SOUNDHANDLE soundHandle, float position[3], unsigned char volume );

// play this sound but only one instance of this sound is allowed to be playing at once.
void SYS_PlaySound( SYS_SOUNDHANDLE soundHandle, int looping );
void SYS_PlaySoundVolume( SYS_SOUNDHANDLE soundHandle, int looping, unsigned char volume );
void SYS_StopSound( SYS_SOUNDHANDLE soundHandle );
int SYS_IsSoundPlaying( SYS_SOUNDHANDLE soundHandle );

#endif

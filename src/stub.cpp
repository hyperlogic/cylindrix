#include "types.h"
#include <assert.h>
#include <stdio.h>
#include "commands.h"

// AJT: remove this once the menu system no longer hangs waiting for input.
int Jon_Kbhit()
{
	SYS_Error( "Jon_Kbhit()!\n" );
	return 0;
}

int Jon_Getkey()
{
	SYS_Error( "Jon_Getkey()!\n" );
	return 0;
}

void Set_Cd_Volume( int volume )
{
	;
}

void Play_Song( int tracknum )
{
	;
}

void calibrate_flightstick_throttle()
{
	SYS_Error( "Joystick not supported!\n" );
}

boolean Detect_Flightstick( void )
{
	SYS_Error( "Joystick not supported!\n" );
	return 0;
}

void calibrate_joystick_center()
{
	SYS_Error( "Joystick not supported!\n" );
}

void calibrate_joystick_lower_right()
{	
	SYS_Error( "Joystick not supported!\n" );
}

void calibrate_joystick_upper_left()
{
	SYS_Error( "Joystick not supported!\n" );
}

boolean Detect_Joystick()
{
	SYS_Error( "Detect_Joystick()\n" );
	return 0;
}

void New_Brighten_Palette( pcx_picture_ptr image, int frames )
{
	SYS_Error( "New_Brighten_Palette\n" );
}

void Play_Intro_Animation( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_One( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Two( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Three( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Four( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Five( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Six( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Seven( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Eight( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Nine( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Cut_Ten( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

void Play_Ending_Animation( void )
{
	SYS_Error( "cuts.c disabled\n" );
}

int Cylindrix_Disk_Present( void )
{
	SYS_Error( "cuts.c disabled\n" );
	return 1;
}

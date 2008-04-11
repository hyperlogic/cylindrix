#include "assert.h"
#include "../system.h"
#include <stdio.h>

void SYS_InitSound()
{
	// TODO:
}

void SYS_ShutdownSound()
{
	// TODO:
}

SYS_SOUNDHANDLE SYS_LoadSound( char* fileName )
{
	// TODO:
	return 0;
}

void SYS_ReleaseSound( SYS_SOUNDHANDLE soundHandle )
{
	// TODO:
}

// plays this sound once, (fire and forget)
void SYS_TriggerSound( SYS_SOUNDHANDLE soundHandle )
{
	// TODO:
}

void SYS_TriggerSoundVolume( SYS_SOUNDHANDLE soundHandle, unsigned char volume, unsigned char pan )
{
	// TODO:
}

// play this sound but only one instance of this sound is allowed to be playing at once.
void SYS_PlaySound( SYS_SOUNDHANDLE soundHandle, int looping )
{
	// TODO:
}

void SYS_PlaySoundVolume( SYS_SOUNDHANDLE soundHandle, int looping, unsigned char volume, unsigned char pan )
{
	// TODO:
}

void SYS_StopSound( SYS_SOUNDHANDLE soundHandle )
{
	// TODO:
}

int SYS_IsSoundPlaying( SYS_SOUNDHANDLE soundHandle )
{
	// TODO:
	return 0;
}

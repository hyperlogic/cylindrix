#include "assert.h"
#include "../system.h"
#include <stdio.h>

void SYS_InitSound() {}
void SYS_ShutdownSound() {}
void SYS_ProcessSound() {}
SYS_SOUNDHANDLE SYS_LoadSound( char* fileName ) {}
void SYS_ReleaseSound( SYS_SOUNDHANDLE soundHandle ) {}
void SYS_TriggerSound( SYS_SOUNDHANDLE soundHandle ) {}
void SYS_TriggerSoundVolume( SYS_SOUNDHANDLE soundHandle, unsigned char volume ) {}
void SYS_SetListenerOrientation( float position[3], float forward[3], float up[3] ) {}
void SYS_TriggerSound3D( SYS_SOUNDHANDLE soundHandle, float position[3], unsigned char volume ) {}
void SYS_PlaySound( SYS_SOUNDHANDLE soundHandle, int looping ) {}
void SYS_PlaySoundVolume( SYS_SOUNDHANDLE soundHandle, int looping, unsigned char volume ) {}
void SYS_StopSound( SYS_SOUNDHANDLE soundHandle ) {}
int SYS_IsSoundPlaying( SYS_SOUNDHANDLE soundHandle ) {}

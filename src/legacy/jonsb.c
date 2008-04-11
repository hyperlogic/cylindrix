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

#include <stdlib.h>

#include "types.h"
#include "jonsb.h"
#include "util.h"
#include "ai_util.h"
#include "keys.h"



/* Globals */


extern game_configuration_type game_configuration; /* All these from omega.c */
extern SYS_SOUNDHANDLE sample[MAX_WORLD_SAMPLES];
extern SYS_SOUNDHANDLE computer_sample[MAX_COMPUTER_SAMPLES];



#define NUMQSAMPLES 50
typedef struct {
     short num_samples;                 /* Number of samples qeued */
     sound_index_type sound_index[NUMQSAMPLES];  /* Type of sound events */
     Float_Point point[NUMQSAMPLES];             /* Positions each sound occurred */
} q_sample_type; 


q_sample_type q_samples;

SYS_SOUNDHANDLE current_voice = 0;

/* End of globals */ 

/* Compute the volume based on the distance between point and orientation */
void Find_Volume( int *volume, int *pan, Float_Point point, Orientation *o ) {
	/* short horizontal_angle; */ /* Angle the sound occurred from the listener */
	float distance; /* Distance that the sound occurred from the listener */	
	float tempPan;
	short horizontal_angle;
	const float distanceCutoff = 30.0f;

	horizontal_angle = Horizontal_Point_Angle_3D( point, o ); 
	distance = Point_Distance_3D( point, o->position );  

	if ( distance > distanceCutoff ) /* 60 is all the way across the cylinder */
	{
		*volume = 0;
	}
	else
	{
		*volume = (int)(((distanceCutoff - distance) / distanceCutoff) * 255.0f);
	}

	tempPan = ((horizontal_angle + 90) % 360) / 360.0f;
	if ( tempPan < 0.5f )
		tempPan *= 2.0f;
	else
		tempPan = (tempPan - 0.5f) * 2.0f;
	
	 *pan = (int)(tempPan * 255.0f);

}

/* Figure out volume we should play sample at based on variables in game config */
int Voice_Volume( int volume ) {
     float ratio;
     float return_vol;

     ratio = game_configuration.voice_vol / 255.0f;
     return_vol = volume * ratio;
     return (int)return_vol;

}

/* Figure out volume we should play sample at based on variables in game config */
int Fx_Volume( int volume ) {
     float ratio;
     float return_vol;


     ratio = game_configuration.sound_vol / 255.0f;
     return_vol = volume * ratio;
     return (int)return_vol;

}





/* Initialize all sound handles to NULL
   and init sample index to 0 so we can start at the start */
void Init_Jon_Samples( void ) 
{
     q_samples.num_samples = 0;

}



/* Adjust one samples volume according to the distance between orientation 
   and point */
void Jon_Mix_Sample( SYS_SOUNDHANDLE sound, Float_Point point, Orientation *o ) {

	int volume, pan;
     
	Find_Volume( &volume, &pan, point, o ); 
	volume = Fx_Volume( volume );	// scale the volume by the current gameconfig setting.

	SYS_TriggerSoundVolume( sound, (unsigned char)volume, (unsigned char)pan );

}


/* Qeueueueu a sample for later playing (later in the frame hopefully)
   and store the position that the sound event occurred */
void Q_Jon_Sample( sound_index_type sound_index, Float_Point point ) {                               
     if( q_samples.num_samples < NUMQSAMPLES )
         {
          q_samples.sound_index[q_samples.num_samples] = sound_index;
          q_samples.point[q_samples.num_samples][X]    = point[X];
          q_samples.point[q_samples.num_samples][Y]    = point[Y];
          q_samples.point[q_samples.num_samples][Z]    = point[Z];
          q_samples.num_samples                        += 1;
         }

}

void Play_Q_Samples( Orientation *o ) {
     int i;

     for( i = 0; i < q_samples.num_samples; i++ )
         {
          Jon_Mix_Sample( sample[ q_samples.sound_index[i] ], q_samples.point[i], o );
         } 
     
     q_samples.num_samples = 0;    
     
}

void Mix_Voice_Sample( SYS_SOUNDHANDLE sound ) 
{
 	int volume;

	volume = Voice_Volume( 255 );	// scale the volume by the current gameconfig setting.

	SYS_TriggerSoundVolume( sound, (unsigned char)volume, 128 );
}

boolean Is_Voice_Done( void ) 
{
	if ( current_voice )
		return !SYS_IsSoundPlaying( current_voice );
	else
		return TRUE;
}

void Play_Voice( SYS_SOUNDHANDLE sound ) 
{
	int volume;
	volume  = Voice_Volume( 255 );
	SYS_PlaySoundVolume( sound, 0, (unsigned char)volume, 128 );

	current_voice = sound;
}

void Play_Menu_Sound( SYS_SOUNDHANDLE sound ) 
{
	int volume;

	// play menu sounds at half volume so they wont be soo annoying
    volume = Fx_Volume( 128 );

	SYS_TriggerSoundVolume( sound, (unsigned char)volume, 128 );
}


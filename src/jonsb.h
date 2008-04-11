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

#ifndef JONSB_H

#define JONSB_H

#include <stdio.h>
#include "types.h"

#include "types.h"


#define MAX_JON_SAMPLES 64
#define MAX_VOICE_SAMPLES 20



void Find_Volume( int *left_volume, int *right_volume, Float_Point point, Orientation *o );
void Init_Jon_Samples( void );
void Jon_Mix_Sample( SYS_SOUNDHANDLE sound, Float_Point point, Orientation *o );
void Q_Jon_Sample( sound_index_type sound_index, Float_Point point );
void Play_Q_Samples( Orientation *o );
void Mix_Voice_Sample( SYS_SOUNDHANDLE sound );
boolean Is_Voice_Done( void );
void Play_Voice( SYS_SOUNDHANDLE sound );
void Play_Menu_Sound( SYS_SOUNDHANDLE sound );

#endif

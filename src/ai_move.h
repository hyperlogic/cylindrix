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

/* This file handles all of the movement of the vehicles...this is the
   semi-low level stuff...one level above motor controls but one level
   below 'attack enemy'. */

#ifndef AI_MOVE_H

#define AI_MOVE_H

#include "types.h"
#include "ai.h"
#include "ai_util.h"
#include "util.h"
#include "motor.h"

void Move_Towards_3D( short horizontal_angle, short vertical_angle, float dist, unsigned char *table, float accelerate_threshold, float decelerate_threshold );
void New_Move_Towards_3D( short horizontal_angle, short vertical_angle, float dist, unsigned char *table );
void Sidestep_Towards_3D( short horizontal_angle, short vertical_angle, float dist, unsigned char *table );
void Move_Towards_2D( short angle, float dist, unsigned char *table );
void Sidestep_Towards_2D( short angle, float dist, unsigned char *table );
void Sidestep_Away_2D( short angle, float dist, unsigned char *table );
void Aim_Gun_2D( WorldStuff *world_stuff, short angle, float dist, int index );
void Aim_Gun_3D( WorldStuff *world_stuff, int index );
void Missiles( WorldStuff *world_stuff, int index );
void Move_Towards_Point_3D( Player player[], int index, Float_Point dest_point );
void Move_Towards_Point_2D( Player player[], int index, Float_Point2d dest_point );
void Rotate_Around_Clockwise( short angle, float dist, unsigned char *table );
void Rotate_Around_Counterclockwise( short angle, float dist, unsigned char *table );
void Projectile_Special_Weapons( WorldStuff *world_stuff, int index );
void Decoy( WorldStuff *world_stuff, int index );
void Cloak_Override( WorldStuff *world_stuff, int index );
void Mines( WorldStuff *world_stuff, int index );
void Stuck_Override( WorldStuff *world_stuff, int index );

#endif


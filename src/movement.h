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

#include "types.h"

#ifndef MOVEMENT_H
#define MOVEMENT_H

#define CYLINDER_Z_MIN -19.8
#define CYLINDER_Z_MAX 19.8

void move_along_velocity_surface( Vehicle *v );
void move_against_velocity_surface( Vehicle *v );

void move_along_velocity_3d( Vehicle *v );
void move_against_velocity_3d( Vehicle *v );

/* Air rotation functions */

void update_vehicle_dive( Vehicle *v, unsigned char value );
void update_vehicle_climb( Vehicle *v, unsigned char value );
void update_vehicle_spin_counterclockwise( Vehicle *v, unsigned char value );
void update_vehicle_spin_clockwise( Vehicle *v, unsigned char value );

/* Surface rotation functions */

void update_vehicle_turn_counterclockwise( Vehicle *v, unsigned char value );
void update_vehicle_turn_clockwise( Vehicle *v, unsigned char value );

/* Function that does all the rotations */

void rotate_vehicle( Vehicle *v );

void up_throttle( Vehicle *v );
void down_throttle( Vehicle *v );

void update_vehicle_up_key( Vehicle *v, unsigned char value );
void update_vehicle_down_key( Vehicle *v, unsigned char value );
void update_vehicle_left_key( Vehicle *v, unsigned char value );
void update_vehicle_right_key( Vehicle *v, unsigned char value );
void update_vehicle_sidestep_left( Vehicle *v, unsigned char value );
void update_vehicle_sidestep_right( Vehicle *v, unsigned char value );
void update_vehicle_sidestep_up( Vehicle *v, unsigned char value );
void update_vehicle_sidestep_down( Vehicle *v, unsigned char value );

int align_vehicle_for_landing( Vehicle *v );

void move_vehicles( Player p[], Pylons *pylons, RadarBase *red_base,
                    RadarBase *blue_base, PointFace *base_laser );

void move_vehicle_forward( Vehicle *v );
void reduce_by_friction( enum VehicleMode mode, Float_Vector vel );

#endif


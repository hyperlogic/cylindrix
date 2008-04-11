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
#include "pylon.h"

#ifndef PROJECT_H
#define PROJECT_H

void create_projectile( Vehicle *v, PointFace *obj, enum projectile_type type );
void move_projectile_forward_3d( Projectile *p, Vehicle *v );
void move_projectile_forward_surface( Projectile *p, Vehicle *v );

void move_projectiles_forward( Vehicle *v, Pylons *pylons, Player *player,
                               RadarBase *red_base, RadarBase *blue_base );

void projectile_transform_and_union( Projectile *list, PointFace *result );

void move_projectile_forward_3d( Projectile *p, Vehicle *v );

void update_missile_target( Player *player_array, long index,
                RadarBase *red_base, RadarBase *blue_base );
                
void create_decoy_projectile( Vehicle *v, long type );

#endif


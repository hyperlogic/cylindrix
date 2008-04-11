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

#ifndef BASE_H
#define BASE_H

#include "types.h"

void update_radar_base( RadarBase *base );
void init_red_radar_base( RadarBase *red_base );
void init_blue_radar_base( RadarBase *blue_base );
void aim_radar_base( RadarBase *base, Player *player, PointFace *obj );
void move_base_projectiles_forward( RadarBase *base, Pylons *pylons, Player *player );

#endif


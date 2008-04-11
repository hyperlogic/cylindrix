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

#ifndef DO_STATE_H

#define DO_STATE_H

#include "types.h"
#include "ai.h"
#include "states.h"
#include "events.h"


void Attack( WorldStuff *world_stuff, int index );
void Get_Energy( WorldStuff *world_stuff, int index );
void Panic( WorldStuff *world_stuff, int index );
void Bezerk( WorldStuff *world_stuff, int index );
void Hide( WorldStuff *world_stuff, int index );
void Group( WorldStuff *world_stuff, int index );
void Get_Pylons( WorldStuff *world_stuff, int index );
void Protect( WorldStuff *world_stuff, int index );
void Kill_Radar_Base( WorldStuff *world_stuff, int index );
void Protect_Radar_Base( WorldStuff *world_stuff, int index );
void Evade( WorldStuff *world_stuff, int index );

#endif



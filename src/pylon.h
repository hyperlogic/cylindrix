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

#ifndef PYLON_H
#define PYLON_H

void pylon_union( Pylons* pylons, PointFace *result );
int pylon_exists( Pylons* pylons, short x, short y, long* index );
void get_pylons( Pylons *pylons, char *filename );
void print_pylons( Pylons *pylons );
void pylons_to_pfd( Pylons *pylons, PointFace *obj );
void change_pylon_color( Pylon *pylon, Gradient gradient );
void Tally_Pylons( Player player[], Pylons *pylons );

#endif


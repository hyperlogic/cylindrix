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

#ifndef PATH_H

#define PATH_H

#include "types.h"
#include "ai.h"

               
void Init_Pylon_Grid( WorldStuff *world_stuff );
void Init_Centers_Array( WorldStuff *world_stuff );
void Make_Path_2D( WorldStuff *world_stuff, int index, Float_Point2d destination, int pylon_index );
void Follow_Path_2D( WorldStuff *world_stuff, int index );
void Make_Path_3D( WorldStuff *world_stuff, int index, Float_Point destination, int pylon_index );
void Follow_Path_3D( WorldStuff *world_stuff, int index );


#endif

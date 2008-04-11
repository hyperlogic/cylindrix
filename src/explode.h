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

#ifndef EXPLODE_H
#define EXPLODE_H

    /* adds an explosion to the explosion list */

    void add_explosion( ExplosionType type, Orientation *o, PointFace *obj,
                        Gradient gradient );

    /* adds all the points and faces of all the explosion objects to
       result */

    void explosion_union( PointFace *result );

    /* animates all the explosions one frame */

    void animate_explosions();

#endif

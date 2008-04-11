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

/* Sutherland-Hodgeman Polygon Clipping Algorithm    */
/* Certified Bug-Free !    <grin>                    */
/* CSCI 4631            Prof: Golden G. Richard III  */

/* Modified Feb 16, 1995 by Anthony Thibault
       Now works with the Point struct (in gfx_types.h) rather than just arrays
       of doubles
*/

/* Modified June 8, 1995 by Anthony Thibault
       Now clips entire object against clipping plane &
       uses fixed point numbers.
*/

#include "types.h"     /* important defines & typedef's */

#ifndef CLIPPING_H
#define CLIPPING_H

typedef struct {
    double x;
    double y;
    double z;
} DoublePoint;

/* clip_obj clips all the faces of obj against the cannonical view volume
   hither is the MAGIC value of the front clipping plane */

void clip_obj( PointFace *clip_obj, long hither );

void clip_edges( PointEdge *pe, long hither );

int region( Point p, long hither );

#endif


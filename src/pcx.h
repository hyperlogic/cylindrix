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

/*
* pcx.h: include file for PCX file manipulation like
* allocating memory for them, loading them from file,
* and enabling the color palette in them
*
* Note that this file is dependant on prim.h
*
* Version 1.0
*/


#ifndef PCX_H

#define PCX_H

#include "types.h"

#include <stdlib.h>
#include <stdio.h>





/* Allocate 64k for image */
void PCX_Allocate( pcx_picture_ptr image );

/* Load a pcx and color palette from file */
void PCX_Load( const char *filename, pcx_picture_ptr image );

/* Enable the color palette stored in the pcx file */
void Enable_Color_Palette( pcx_picture_ptr image );

/* Enable the pcx palette from index_start to index_end */
void Enable_Palette_Range( pcx_picture_ptr image, int index_start, int index_end );

/* Enable the index'th member of the pcx palette */
void Enable_Palette_Member( pcx_picture_ptr image, int index );

/* Set one member of the hardware palette to a particular color */
void Set_One( int which_color, int index );


void Enable_Palette( palette_type palette );


#endif

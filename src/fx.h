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
* fx.h: special effects like fadeout and fade in 
* and color cycling
*
* Version 1.0
*
*/


#ifndef FX_H

#define FX_H

#include "prim.h"
#include "pcx.h"

/* Zero out color palette so it is all black */
void Zero_Palette( void );

/* Fade from full intensity image->palette to black */
void Fade_Outa_Palette( pcx_picture_ptr image );

/* Fade from black into full intensity image->palette */
void Fade_Into_Palette( pcx_picture_ptr image );

/* 'Cycle' the palette registers ( i.e. 1 = 2; 2 = 3; 3 = 4;....1 = 256 ) */
void Cycle_Palette( pcx_picture_ptr image );

/* Gradate the color palette from index one(black) to index two(white) */
void Grad_Palette( int index_one, int index_two, unsigned char color_one, unsigned char color_two );

void Dim_Palette( pcx_picture_ptr image, int speed );

void Brighten_Palette( pcx_picture_ptr image, pcx_picture_ptr image2, int speed );

void Cycle_Palette_Section( int start, int stop, pcx_picture_ptr image );

#endif


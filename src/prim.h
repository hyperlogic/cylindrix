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
* prim.h: Graphics 'primitives' for the engine
*
* version 1.0
*  NOTE THAT DB_Clear_Screen() doesn't work correctly for some reason
*/

#ifndef PRIM_H
#define PRIM_H

#include "types.h"

/* used to darken the current palette */
void Set_Palette_Fade( float fadeVal );

/* access to palette used by the double buffer */
void Set_Palette_Register( int index, RGB_color_ptr color );

/* Allocate 64k for double buffer */
void Init_Double_Buffer();

/* Clear the double buffer by filling it with zeros */
void DB_Clear_Screen();

/* Put a value in the double buffer at x,y of value color */
void DB_Fast_Pixel( int x, int y, unsigned char color );

/* Write a pixel in the double buffer but not off the screen */
void DB_Scissor_Pixel( int x, int y, unsigned char color );

/* draws a line into the double buffer */
void DB_draw_edge( long vert[][4], Edge e, unsigned char color );

/* Improved db_poly_scan */
void shade_DB_poly_scan( Face *p, long vert[][4], Window *win, unsigned char color );

/* Draw the polygon into the double buffer */
void fast_DB_poly_scan( Face *p, long vert[][4], Window *win, unsigned char color );

/* Draws a trasparent polygon into the double buffer */
void DB_transparent_poly_scan( Face *p, long vert[][4], Window *win, unsigned char color );

/* copy buffer into the double buffer */
void Pop_Buffer( unsigned char* buffer );

/* Copy the double buffer into the video buffer */
void Swap_Buffer();

/* Draw a one pixel border in the double buffer */
void draw_border();


#endif


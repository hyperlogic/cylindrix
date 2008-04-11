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
* rotbmp.h: texture mapping onto a four sided polygon
*
* Version 1.0
*
*/


#ifndef vectpix_h
#define vectpix_h

/* absolute value of a */
#define ABS(a)          (((a)<0) ? -(a) : (a))

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a)         (((a)<0) ? -1 : (a)>0 ? 1 : 0)  

/* take binary sign of a, either -1, or 1 if >= 0 */
#define SGN(a)          (((a)<0) ? -1 : 0)

#include "types.h"     /* Some defines */
#include "prim.h"      /* Pixel plotters, video mode setter etc. */
#include "pcx.h"       /* Stuff for manipulating pcx bitmaps */

/* The image structure is declared in pcx.h
   Map an image->buffer onto the 4 sided polygon defined
   By the eight points given */
void Texture( pcx_picture_ptr image, int newx1, int newy1, int newx2, int newy2,
              int newx3, int newy3, int newx4, int newy4 );


#endif

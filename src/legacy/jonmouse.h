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

#ifndef JONMOUSE_H
#define JONMOUSE_H

/*
#include <dos.h>
#include <bios.h>
*/

#include <stdio.h>
#include <math.h>
#include "types.h"


/* Defines */

#define MOUSE_INT               0x33 /* Mouse interrupt number */
#define MOUSE_RESET             0x00 /* Reset the mouse */
#define MOUSE_SHOW              0x01 /* Show the mouse */
#define MOUSE_HIDE              0x02 /* Hide the mouse  */
#define MOUSE_BUTT_POS          0x03 /* Get buttons and mouse position */
#define MOUSE_SET_SENSITIVITY   0x1A /* Set the sensitivity of mouse to 0.100 */
#define MOUSE_MOTION_REL        0x0B /* Query motion counters */

/* Defines to make reading buttons easier */

#define MOUSE_LEFT_BUTTON       0x01 /* Left button mask */
#define MOUSE_RIGHT_BUTTON      0x02 /* Right button mask */
#define MOUSE_CENTER_BUTTON     0x04 /* Center button mask */

int Squeeze_Mouse( int command, int *x, int *y, int *buttons );
void Get_Mouse_Table( input_table table );
void Get_Mouse_Table_Rel( input_table table );

#endif



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

#include <string.h>
#include "jonmouse.h"

int Squeeze_Mouse( int command, int *x, int *y, int *buttons ) {
    return(0); //Squeeze_Mouse
	/*
	union REGS inregs, outregs;

    memset( &inregs, 0, sizeof( inregs ) );
    memset( &outregs, 0, sizeof( outregs ) );
    
    
    switch(command)
        {
         case MOUSE_RESET :
             {
              inregs.x.ax = 0x00; // Subfunction 0 : reset 
              int86(MOUSE_INT, &inregs, &outregs);
              *buttons = outregs.x.bx; // Return the number of buttons 
              return(outregs.x.ax); // Return overall success or failure 
             } break;

         case MOUSE_SHOW :
             {
              // This function increments the internal show_mouse counter 
              // When it is equal to 0 the mouse is displayed 

              inregs.x.ax = 0x01; // Subfunction 1, increment show flag 
              int86(MOUSE_INT, &inregs, &outregs);
              return (1);
             } break;

         case MOUSE_HIDE :
             {
             // This function decrements the internal show_mouse counter 
             // When it is equal to -1 the mouse is hidden 

              inregs.x.ax = 0x02;  // Subfunction 2, decrement show flag 
              int86(MOUSE_INT, &inregs, &outregs);
              return(1);
              } break;

         case MOUSE_BUTT_POS :
             {
             // This function gets the buttons and returns the 
             // Absolute mouse positions in the vars x, y, and 
             // Buttons, respectively                          
              
              inregs.x.ax = 0x03; // Subfunction 3 get position and buttons 
              int86(MOUSE_INT, &inregs, &outregs);
              // Extract the info and send it back to the caller via pointers 
              *x       = outregs.x.cx;
              *y       = outregs.x.dx;
              *buttons = outregs.x.bx; 
              *x       = *x & 0xffff;
              *y       = *y & 0xffff;
              *buttons = *buttons & 0xffff;

              return(1);
             } break;

         case MOUSE_MOTION_REL :
             {
             // This function gets the relative mosue motions from the last call 
             // And puts them in the variables x and y, respectively 

              inregs.x.ax = 0x03; // Subfunction 11 : get relative motion 
              int86(MOUSE_INT, &inregs, &outregs);
              // Extract the info and send it back to the caller via pointers 
              *x = outregs.x.cx;
              *y = outregs.x.dx;

              return(1);
             } break;

         case MOUSE_SET_SENSITIVITY :
             {
             // This function sets the overall sensitivity of the mouse 
             //   Each axis can have a sensitivity from 1-100 so the caller
             //   Should put 1-100 in both x and y before calling.  Also 
             //   "Buttons" is used to send in the doublespeed value, which
             //   Also ranges from 1-100 
             
             inregs.x.bx = *x;
             inregs.x.cx = *y;
             inregs.x.dx = *buttons;

             inregs.x.ax = 0x1A; // Subfunction 26 : Set sensitivity 
             int86(MOUSE_INT, &inregs, &outregs);

             return(1);
            } break;

         default : 
             return 0; // This shouldn't happen 
             break;

        } // End switch 
*/
} /* End Squeeze_Mouse */


void Get_Mouse_Table( input_table table ) {
	/*
     int x, y, buttons;

     Squeeze_Mouse( MOUSE_BUTT_POS, &x, &y, &buttons );

     if( buttons == 1 )
         table[ INDEX_FIRE_GUN ] = 1;
     else if( buttons == 2 )
         table[ INDEX_FIRE_MISSILE ] = 1;


     if( (x >> 1) < 130 )
         table[ INDEX_LEFT ] = 1;
     else if( (x >> 1) > 190 )
         table[ INDEX_RIGHT ] = 1;
                             
     if( y < 70 )
         table[ INDEX_UP ] = 1;
     else if( y > 130 )
         table[ INDEX_DOWN ] = 1;

*/
} /* End of Get_Mouse_Table */


void Get_Mouse_Table_Rel( input_table table ) {
	/*
     int x, y, buttons;

     Squeeze_Mouse( MOUSE_BUTT_POS, &x, &y, &buttons );

     if( buttons == 1 )
         table[ INDEX_FIRE_GUN ] = 1;
     else if( buttons == 2 )
         table[ INDEX_FIRE_MISSILE ] = 1;

     Squeeze_Mouse( MOUSE_MOTION_REL, &x, &y, &buttons );                       
     
     if( (x >> 1) < - 50 )
         table[ INDEX_LEFT ] = 1;
     else if( (x >> 1) > 50 )
         table[ INDEX_RIGHT ] = 1;
                             
     if( y > 10 )
         table[ INDEX_UP ] = 1;
     else if( y < - 10 )
         table[ INDEX_DOWN ] = 1;

     printf("%d %d \n", x, y );
*/
} /* End of Get_Mouse_Table_Rel */

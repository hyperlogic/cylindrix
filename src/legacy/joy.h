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

#ifndef JOY_H

#define JOY_H


/* I N C L U D E S */

/*
#include <dos.h>
#include <bios.h>
*/

#include <stdio.h>
#include <math.h>
#include "types.h"

/* D E F I N E S  */


#define JOYPORT      0x201  /* joyport is at 201 hex */

#define BUTTON_1_A   0x10   /* joystick 1, button A */
#define BUTTON_1_B   0x20   /* joystick 1, button B */
#define BUTTON_2_A   0x40   /* joystick 2, button A */
#define BUTTON_2_B   0x80   /* joystick 2, button B */

#define JOYSTICK_1_X 0x01   /* joystick 1, x axis */
#define JOYSTICK_1_Y 0x02   /* joystick 1, y axis */
#define JOYSTICK_2_X 0x04   /* joystick 2, x axis */
#define JOYSTICK_2_Y 0x08   /* joystick 2, y axis */


#define JOY_1_CAL       1   /* command to calibrate joystick #1 */
#define JOY_2_CAL       2   /* command to calibrate joystick #2 */



#define JOYSTICK_THRESHOLD 15.0f
#define THROTTLE_THRESHOLD 10.0f

#define JOYSTICK_RANGE 0.70f  /* percentage off joystick that is sensitive to
                                position, values beyond this range are given
                                the maximum value (255) */

#define JOYSTICK_DELAY 5 /* The time we delay between joystick access */








/* Function prototypes */


unsigned char Buttons(unsigned char button);
unsigned int Joystick(unsigned char stick);

boolean Detect_Joystick( void );
boolean Detect_Flightstick( void );
void Joystick_Calibrate(int stick);
void Get_Joystick_Table( input_table table );
void Show_Calibration( int joystick_number );
void Calibrate_Joystick( void );
void Calibrate_Flightstick( void );
void Get_Flightstick_Table( input_table table );

/* functions used by menu system to calibrate joystick */

void calibrate_joystick_upper_left();
void calibrate_joystick_lower_right();
void calibrate_joystick_center();
void calibrate_flightstick_throttle();

void print_calibration_variables();

#endif

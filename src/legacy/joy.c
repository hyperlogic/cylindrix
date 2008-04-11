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

//#include <conio.h>
#include <string.h>
#include "joy.h"
#include "keys.h"



/* G L O B A L S */


extern int key_table[KEY_TABLE_SIZE]; /* From keys.c */
extern game_configuration_type game_configuration;

unsigned int joy_1_max_x,   /* global joystick calibration variables */
             joy_1_max_y,
             joy_1_min_x,
             joy_1_min_y,
             joy_1_cx,
             joy_1_cy,
             joy_2_max_x,
             joy_2_max_y,
             joy_2_min_x,
             joy_2_min_y,
             joy_2_cx,
             joy_2_cy;


unsigned int joy_1_down_threshold,
             joy_1_left_threshold,
             joy_1_right_threshold,
             joy_1_up_threshold,
             joy_2_up_threshold,
             joy_2_down_threshold,
             joy_2_left_threshold,
             joy_2_right_threshold;



/* F U N C T I O N S */


void print_calibration_variables()
{
    printf("joy_1_max_x = %d\n", joy_1_max_x );
    printf("joy_1_max_y = %d\n", joy_1_max_y );    
    printf("joy_1_min_y = %d\n", joy_1_min_y );
    printf("joy_1_min_y = %d\n", joy_1_min_y );
    printf("joy_1_cx = %d\n", joy_1_cx );
    printf("joy_1_cy = %d\n", joy_1_cy );
    printf("joy_2_max_x = %d\n", joy_2_max_x );
    printf("joy_2_max_y = %d\n", joy_2_max_y );
    printf("joy_2_min_x = %d\n", joy_2_min_x );
    printf("joy_2_min_y = %d\n", joy_2_min_y );
    printf("joy_2_cx = %d\n", joy_2_cx );
    printf("joy_2_cy = %d\n", joy_2_cy );
    
    printf("joy_1_up_threshold = %d\n", joy_1_up_threshold );
    printf("joy_1_down_threshold = %d\n", joy_1_down_threshold );
    printf("joy_1_left_threshold = %d\n", joy_1_left_threshold );
    printf("joy_1_right_threshold = %d\n", joy_1_right_threshold );
    printf("joy_2_up_threshold = %d\n", joy_2_up_threshold );
    printf("joy_2_down_threshold = %d\n", joy_2_down_threshold );
    printf("joy_2_left_threshold = %d\n", joy_2_left_threshold );
    printf("joy_2_right_threshold = %d\n", joy_2_right_threshold );
}

boolean Detect_Joystick( void ) {
	return(0); //Johnm 12/4/2001
/*
	int c;


     disable(); // Disable interrupts 

     outportb( JOYPORT, 0xff); // Trigger joystick 

     c = inportb( JOYPORT );
     c = inportb( JOYPORT );
     c = inportb( JOYPORT );

     delay( JOYSTICK_DELAY );

     c ^= inportb( JOYPORT ); // Any bits that changed state are now set 

     enable(); // Re-enable interrupts 

     if( c & 0x03 )
         return 1;
     else
         return 0;
*/

} // End of Detect_Joystick 


boolean Detect_Flightstick( void ) {
	return(0); //Johnm 12/4/2001
/*
	int c;


     disable(); // Disable interrupts 

     outportb( JOYPORT, 0xff); // Trigger joystick 

     c = inportb( JOYPORT );
     c = inportb( JOYPORT );
     c = inportb( JOYPORT );

     delay( JOYSTICK_DELAY );

     c ^= inportb( JOYPORT ); // Any bits that changed state are now set 

     enable(); // Re-enable interrupts 

     if( (c & 0x03) && (c&0x0C) )
         return 1;
     else
         return 0;
*/

} /* End of Detect_Flightstick */


unsigned char Buttons(unsigned char button) {
	return(0); //Johnm 12/4/2001
/*
//read the joystick buttons by peeking the port that the switches are attached to. 

outportb(JOYPORT,0); // clear the latch and request a sample 

// invert buttons then mask with request 

return( ~inportb(JOYPORT) & button);
*/
} /* end Buttons */



unsigned int Joystick(unsigned char stick)
{
	return(0); //Johnm 12/4/2001
// reads the joystick values manually by conting how long the capacitors take to charge/discharge
/*
   unsigned char test_byte;
   unsigned char done = 0;
   unsigned int count = 0;

   disable(); // Disable interrupts 

   outportb( JOYPORT, 0 );

   while( !done )
       {
        test_byte = inportb( JOYPORT );

        if( test_byte & stick )
            {
             count++;
            }
        else
            done = 1;
       }

   enable(); // Re-enable interrupts 

   return( count );
*/
} // end Joystick 




void Calibrate_Joystick( void )
{
  unsigned int joyx, joyy;


   joy_1_max_x=0;
   joy_1_max_y=0;
   joy_1_min_x=10000;
   joy_1_min_y=10000;


   joyx = joyy = 0;

   fprintf(stderr, "\nCalibrating Joystick: Rotate stick to all extremities, then leave in center and press fire");

     while( !Buttons(BUTTON_1_A) )
     {
     delay(JOYSTICK_DELAY);
     joyx  = Joystick(JOYSTICK_1_X);
     delay(JOYSTICK_DELAY);
     joyy  = Joystick(JOYSTICK_1_Y);


     printf("Joyx is    %d joyy is    %d \n", joyx, joyy );


      if( joyx > joy_1_max_x )
          joy_1_max_x=joyx;
      if( joyy > joy_1_max_y )
          joy_1_max_y=joyy;
      if( joyx < joy_1_min_x )
           joy_1_min_x=joyx;
      if( joyy < joy_1_min_y )
          joy_1_min_y=joyy;

     } /* End while not button press */

     joy_1_cx = joyx;
     joy_1_cy = joyy;


     /* Forgot why I put this here */
     if (Buttons(BUTTON_1_A))
         {
         ;
         }
     else if (Buttons(BUTTON_1_B))
         {
         ;
         }


         joy_1_up_threshold    = (unsigned int)(joy_1_cy - ((joy_1_cy - joy_1_min_y) / JOYSTICK_THRESHOLD));

         joy_1_down_threshold  = (unsigned int)(joy_1_cy + ((joy_1_max_y - joy_1_cy) / JOYSTICK_THRESHOLD));

         joy_1_left_threshold  = (unsigned int)(joy_1_cx - ((joy_1_cx - joy_1_min_x) / JOYSTICK_THRESHOLD));

         joy_1_right_threshold = (unsigned int)(joy_1_cx + ((joy_1_max_x - joy_1_cx) / JOYSTICK_THRESHOLD));


} /* end Calibrate_Joystick */



void Get_Joystick_Table( input_table table )
    {
     unsigned int joyx, joyy;
     boolean button1, button2, button3, button4;
     float temp_one, extrema;
     static boolean last_frame_mode_switch = FALSE;
     boolean strafe_on = FALSE;
     KeyboardConfig *config;

     config = &game_configuration.keyboard_config;
     
     delay(JOYSTICK_DELAY);
     joyx  = Joystick(JOYSTICK_1_X);
     delay(JOYSTICK_DELAY);
     joyy  = Joystick(JOYSTICK_1_Y);

     /* make sure that joyx isn't bigger then joy_1_max_x and
        isn't smaller than joy_1_min_x */

     if( joyx > joy_1_max_x ) {
         joyx = joy_1_max_x ;
     }
     else if( joyx < joy_1_min_x ) {
         joyx = joy_1_min_x;
     }

     /* make sure that joyy isn't bigger then joy_1_max_y and
        isn't smaller then joy_1_min_y */

     if( joyy > joy_1_max_y ) {
         joyy = joy_1_max_y;
     }
     else if( joyy < joy_1_min_y ) {
         joyy = joy_1_min_y;
     }

     button1 = Buttons(BUTTON_1_A);
     button2 = Buttons(BUTTON_1_B);
     button3 = Buttons(BUTTON_2_A);
     button4 = Buttons(BUTTON_2_B);

     if( joyx > joy_1_right_threshold )
         {

          extrema = ((joy_1_max_x - joy_1_cx) * JOYSTICK_RANGE) + joy_1_cx;

          if( joyx > extrema ) {
              joyx = (unsigned int)extrema;
          }

          temp_one = 255.0f / (extrema - joy_1_cx) * (joyx - joy_1_cx);

          if( temp_one > 255.0 ) {
              table[INDEX_RIGHT] = 255;
          }
          else {
              table[INDEX_RIGHT] = (unsigned char)temp_one;
          }

         }
     else if( joyx < joy_1_left_threshold )
         {

          extrema = joy_1_cx - ((joy_1_cx - joy_1_min_x) * JOYSTICK_RANGE);

          if( joyx < extrema ) {
              joyx = (unsigned int)extrema;
          }

          temp_one = 255.0f - ( 255.0f / (joy_1_cx - extrema) * (joyx - extrema) );

          if( temp_one > 255.0f ) {
              table[INDEX_LEFT] = 255;
          }
          else {
              table[INDEX_LEFT] = (unsigned char)temp_one;
          }

         }

     if( joyy > joy_1_down_threshold )
         {

          extrema = ((joy_1_max_x - joy_1_cx) * JOYSTICK_RANGE) + joy_1_cx;

          if( joyx > extrema ) {
              joyx = (unsigned int)extrema;
          }

          temp_one = 255.0f / (extrema - joy_1_cy) * (joyy - joy_1_cy);

          if( temp_one > 255.0f ) {
              table[INDEX_DOWN] = 255;
          }
          else {
              table[INDEX_DOWN] = (unsigned char)temp_one;
          }

         }
     else if( joyy < joy_1_up_threshold )
         {
          extrema = joy_1_cx - ((joy_1_cx - joy_1_min_x) * JOYSTICK_RANGE);

          if( joyx < extrema ) {
              joyx = (unsigned int)extrema;
          }

          temp_one = 255.0f - ( 255.0f / (joy_1_cy - extrema) * (joyy - extrema) );

          if( temp_one > 255.0f ) {
              table[INDEX_UP] = 255;
          }
          else {
              table[INDEX_UP] = (unsigned char)temp_one;
          }

         }


     if( button1 ) {
         if( game_configuration.joystick_config.button_1 == ShootLaser ) {
             table[INDEX_FIRE_GUN] = 1;
         }
         else if( game_configuration.joystick_config.button_1 == ShootMissile ) {
             table[INDEX_FIRE_MISSILE] = 1;
         }
         else if( game_configuration.joystick_config.button_1 == ActivateSpecial ) {
             table[INDEX_SPECIAL_WEAPON] = 1;
         }
         else if( game_configuration.joystick_config.button_1 == ActivateStrafe ) {
             strafe_on = TRUE;
         }
         else if( game_configuration.joystick_config.button_1 == ThrottleUp ) {
             table[INDEX_UP_THROTTLE] = 1;
         }
         else if( game_configuration.joystick_config.button_1 == ThrottleDown ) {
             table[INDEX_DOWN_THROTTLE] = 1;
         }
         else if( game_configuration.joystick_config.button_1 == SwitchModes ) {
             if( !last_frame_mode_switch ) {
                 table[INDEX_MODE_SWITCH] = 1;
             }
             last_frame_mode_switch = TRUE;
         }
         else if( game_configuration.joystick_config.button_1 == DoNothing ) {
             ;
         }
         else if( game_configuration.joystick_config.button_1 == StrafeUp ) {
             table[INDEX_SIDESTEP_UP] = 255;
             table[INDEX_UP] = 0;
         }
         else if( game_configuration.joystick_config.button_1 == StrafeDown ) {
             table[INDEX_SIDESTEP_DOWN] = 255;
             table[INDEX_DOWN] = 0;
         }
         else if( game_configuration.joystick_config.button_1 == StrafeLeft ) {
             table[INDEX_SIDESTEP_LEFT] = 255;
             table[INDEX_LEFT] = 0;
         }
         else if( game_configuration.joystick_config.button_1 == StrafeRight ) {
             table[INDEX_SIDESTEP_RIGHT] = 255;
             table[INDEX_RIGHT] = 0;
         }
     }
     else if( game_configuration.joystick_config.button_1 == SwitchModes ) {
         last_frame_mode_switch = FALSE;
     }



     if( button2 ) {
         if( game_configuration.joystick_config.button_2 == ShootLaser ) {
             table[INDEX_FIRE_GUN] = 1;
         }
         else if( game_configuration.joystick_config.button_2 == ShootMissile ) {
             table[INDEX_FIRE_MISSILE] = 1;
         }
         else if( game_configuration.joystick_config.button_2 == ActivateSpecial ) {
             table[INDEX_SPECIAL_WEAPON] = 1;
         }
         else if( game_configuration.joystick_config.button_2 == ActivateStrafe ) {
             strafe_on = TRUE;
         }
         else if( game_configuration.joystick_config.button_2 == ThrottleUp ) {
             table[INDEX_UP_THROTTLE] = 1;
         }
         else if( game_configuration.joystick_config.button_2 == ThrottleDown ) {
             table[INDEX_DOWN_THROTTLE] = 1;
         }
         else if( game_configuration.joystick_config.button_2 == SwitchModes ) {
             if( !last_frame_mode_switch ) {
                 table[INDEX_MODE_SWITCH] = 1;
             }
             last_frame_mode_switch = TRUE;
         }
         else if( game_configuration.joystick_config.button_2 == DoNothing ) {
             ;
         }
         else if( game_configuration.joystick_config.button_2 == StrafeUp ) {
             table[INDEX_SIDESTEP_UP] = 255;
             table[INDEX_UP] = 0;
         }
         else if( game_configuration.joystick_config.button_2 == StrafeDown ) {
             table[INDEX_SIDESTEP_DOWN] = 255;
             table[INDEX_DOWN] = 0;
         }
         else if( game_configuration.joystick_config.button_2 == StrafeLeft ) {
             table[INDEX_SIDESTEP_LEFT] = 255;
             table[INDEX_LEFT] = 0;
         }
         else if( game_configuration.joystick_config.button_2 == StrafeRight ) {
             table[INDEX_SIDESTEP_RIGHT] = 255;
             table[INDEX_RIGHT] = 0;
         }
     }
     else if( game_configuration.joystick_config.button_2 == SwitchModes ) {
         last_frame_mode_switch = FALSE;
     }



     if( button3 ) {
         if( game_configuration.joystick_config.button_3 == ShootLaser ) {
             table[INDEX_FIRE_GUN] = 1;
         }
         else if( game_configuration.joystick_config.button_3 == ShootMissile ) {
             table[INDEX_FIRE_MISSILE] = 1;
         }
         else if( game_configuration.joystick_config.button_3 == ActivateSpecial ) {
             table[INDEX_SPECIAL_WEAPON] = 1;
         }
         else if( game_configuration.joystick_config.button_3 == ActivateStrafe ) {
             strafe_on = TRUE;
         }
         else if( game_configuration.joystick_config.button_3 == ThrottleUp ) {
             table[INDEX_UP_THROTTLE] = 1;
         }
         else if( game_configuration.joystick_config.button_3 == ThrottleDown ) {
             table[INDEX_DOWN_THROTTLE] = 1;
         }
         else if( game_configuration.joystick_config.button_3 == SwitchModes ) {
             if( !last_frame_mode_switch ) {
                 table[INDEX_MODE_SWITCH] = 1;
             }
             last_frame_mode_switch = TRUE;
         }
         else if( game_configuration.joystick_config.button_3 == DoNothing ) {
             ;
         }
         else if( game_configuration.joystick_config.button_3 == StrafeUp ) {
             table[INDEX_SIDESTEP_UP] = 255;
             table[INDEX_UP] = 0;
         }
         else if( game_configuration.joystick_config.button_3 == StrafeDown ) {
             table[INDEX_SIDESTEP_DOWN] = 255;
             table[INDEX_DOWN] = 0;
         }
         else if( game_configuration.joystick_config.button_3 == StrafeLeft ) {
             table[INDEX_SIDESTEP_LEFT] = 255;
             table[INDEX_LEFT] = 0;
         }
         else if( game_configuration.joystick_config.button_3 == StrafeRight ) {
             table[INDEX_SIDESTEP_RIGHT] = 255;
             table[INDEX_RIGHT] = 0;
         }
     }
     else if( game_configuration.joystick_config.button_3 == SwitchModes ) {
         last_frame_mode_switch = FALSE;
     }


     if( button4 ) {
         if( game_configuration.joystick_config.button_4 == ShootLaser ) {
             table[INDEX_FIRE_GUN] = 1;
         }
         else if( game_configuration.joystick_config.button_4 == ShootMissile ) {
             table[INDEX_FIRE_MISSILE] = 1;
         }
         else if( game_configuration.joystick_config.button_4 == ActivateSpecial ) {
             table[INDEX_SPECIAL_WEAPON] = 1;
         }
         else if( game_configuration.joystick_config.button_4 == ActivateStrafe ) {
             strafe_on = TRUE;
         }
         else if( game_configuration.joystick_config.button_4 == ThrottleUp ) {
             table[INDEX_UP_THROTTLE] = 1;
         }
         else if( game_configuration.joystick_config.button_4 == ThrottleDown ) {
             table[INDEX_DOWN_THROTTLE] = 1;
         }
         else if( game_configuration.joystick_config.button_4 == SwitchModes ) {
             if( !last_frame_mode_switch ) {
                 table[INDEX_MODE_SWITCH] = 1;
             }
             last_frame_mode_switch = TRUE;
         }
         else if( game_configuration.joystick_config.button_4 == DoNothing ) {
             ;
         }
         else if( game_configuration.joystick_config.button_4 == StrafeUp ) {
             table[INDEX_SIDESTEP_UP] = 255;
             table[INDEX_UP] = 0;
         }
         else if( game_configuration.joystick_config.button_4 == StrafeDown ) {
             table[INDEX_SIDESTEP_DOWN] = 255;
             table[INDEX_DOWN] = 0;
         }
         else if( game_configuration.joystick_config.button_4 == StrafeLeft ) {
             table[INDEX_SIDESTEP_LEFT] = 255;
             table[INDEX_LEFT] = 0;
         }
         else if( game_configuration.joystick_config.button_4 == StrafeRight ) {
             table[INDEX_SIDESTEP_RIGHT] = 255;
             table[INDEX_RIGHT] = 0;
         }
     }
     else if( game_configuration.joystick_config.button_4 == SwitchModes ) {
         last_frame_mode_switch = FALSE;
     }


     if( (key_table[config->strafe] && table[INDEX_RIGHT]) ||
         (strafe_on && table[INDEX_RIGHT]) ) {

         table[INDEX_SIDESTEP_RIGHT] = table[INDEX_RIGHT];
         table[INDEX_RIGHT] = 0;

     }

     if( (key_table[config->strafe] && table[INDEX_UP]) ||
         (strafe_on && table[INDEX_UP]) ) {

         table[INDEX_SIDESTEP_UP] = table[INDEX_UP];
         table[INDEX_UP] = 0;

     }

     if( (key_table[config->strafe] && table[INDEX_DOWN]) ||
         (strafe_on && table[INDEX_DOWN]) ) {

         table[INDEX_SIDESTEP_DOWN] = table[INDEX_DOWN];
         table[INDEX_DOWN] = 0;

     }

     if( (key_table[config->strafe] && table[INDEX_LEFT]) ||
         (strafe_on && table[INDEX_LEFT]) ) {

         table[INDEX_SIDESTEP_LEFT] = table[INDEX_LEFT];
         table[INDEX_LEFT] = 0;

     }

} /* end Get_Joystick_Table */






/* Beginning of Flightstick functions */


void Calibrate_Flightstick( void )
{
  unsigned int joyx, joyy;
  unsigned int joy2y;


   joy_1_max_x=0;
   joy_1_max_y=0;
   joy_1_min_x=10000;
   joy_1_min_y=10000;

   joy_2_min_y = 10000;
   joy_2_max_y = 0;


   joyx = joyy = joy2y = 0;

   fprintf(stderr, "\nCalibrating Flightstick: Rotate stick to all extremities, \n move throttle to top and bottom \nthen leave in center and press fire");

     while( !Buttons(BUTTON_1_A) )
     {

     delay(JOYSTICK_DELAY);
     joyx  = Joystick(JOYSTICK_1_X);
     delay(JOYSTICK_DELAY);
     joyy  = Joystick(JOYSTICK_1_Y);
     delay(JOYSTICK_DELAY);
     joy2y = Joystick(JOYSTICK_2_Y);

     /*
     printf("\n");
     printf("Joyx is    %u joyy is    %u \n", joyx, joyy );
     printf("Maxx is %u Maxy is %u \n", joy_1_max_x, joy_1_max_y );
     */

      if( joyx > joy_1_max_x )
          joy_1_max_x=joyx;
      if( joyy > joy_1_max_y )
          joy_1_max_y=joyy;
      if( joyx < joy_1_min_x )
           joy_1_min_x=joyx;
      if( joyy < joy_1_min_y )
          joy_1_min_y=joyy;

      if( joy2y < joy_2_min_y )
          joy_2_min_y = joy2y;
      if( joy2y > joy_2_max_y )
          joy_2_max_y = joy2y;



     } /* End while not button press */

     joy_1_cx = joyx;
     joy_1_cy = joyy;

     joy_2_cy = (joy_2_max_y - joy_2_min_y) / 2;

     /* Forgot why I put this here */
     if (Buttons(BUTTON_1_A))
         {
         ;
         }
     else if (Buttons(BUTTON_1_B))
         {
         ;
         }


         joy_1_up_threshold    = (unsigned int)(joy_1_cy - ((joy_1_cy - joy_1_min_y) / JOYSTICK_THRESHOLD));

         joy_1_down_threshold  = (unsigned int)(joy_1_cy + ((joy_1_max_y - joy_1_cy) / JOYSTICK_THRESHOLD));

         joy_1_left_threshold  = (unsigned int)(joy_1_cx - ((joy_1_cx - joy_1_min_x) / JOYSTICK_THRESHOLD));

         joy_1_right_threshold = (unsigned int)(joy_1_cx + ((joy_1_max_x - joy_1_cx) / JOYSTICK_THRESHOLD));


         joy_2_up_threshold    = (unsigned int)(joy_2_cy - ((joy_2_cy - joy_2_min_y) / THROTTLE_THRESHOLD));
         joy_2_down_threshold  = (unsigned int)(joy_2_cy + ((joy_2_max_y - joy_2_cy) / THROTTLE_THRESHOLD));


} /* end Calibrate_Flightstick */


void Get_Flightstick_Table( input_table table )
    {
     unsigned int joyx, joyy, joy2y;
     boolean button1, button2, button3, button4, hat_up, hat_down, hat_left, hat_right;
     float temp_one, extrema; /* For math */
     boolean strafe_on = FALSE;
     static boolean last_frame_fire        = FALSE;
     static boolean last_frame_mode_switch = FALSE;
     KeyboardConfig *config;


     config = &game_configuration.keyboard_config;

     delay(JOYSTICK_DELAY);
     joyx  = Joystick(JOYSTICK_1_X);
     delay(JOYSTICK_DELAY);
     joyy  = Joystick(JOYSTICK_1_Y);
     delay(JOYSTICK_DELAY);
     joy2y  = Joystick(JOYSTICK_2_Y);

     /* make sure that joyx isn't bigger then joy_1_max_x and
        isn't smaller than joy_1_min_x */

     if( joyx > joy_1_max_x ) {
         joyx = joy_1_max_x ;
     }
     else if( joyx < joy_1_min_x ) {
         joyx = joy_1_min_x;
     }

     /* make sure that joyy isn't bigger then joy_1_max_y and
        isn't smaller then joy_1_min_y */

     if( joyy > joy_1_max_y ) {
         joyy = joy_1_max_y;
     }
     else if( joyy < joy_1_min_y ) {
         joyy = joy_1_min_y;
     }

     button1 = Buttons(BUTTON_1_A);
     button2 = Buttons(BUTTON_1_B);
     button3 = Buttons(BUTTON_2_A);
     button4 = Buttons(BUTTON_2_B);

     if( joyx > joy_1_right_threshold )
         {

          extrema = ((joy_1_max_x - joy_1_cx) * JOYSTICK_RANGE) + joy_1_cx;

          if( joyx > extrema ) {
              joyx = (unsigned int)extrema;
          }

          temp_one = 255.0f / (extrema - joy_1_cx) * (joyx - joy_1_cx);

          if( temp_one > 255.0f ) {
              table[INDEX_RIGHT] = 255;
          }
          else {
              table[INDEX_RIGHT] = (unsigned char)temp_one;
          }

         }
     else if( joyx < joy_1_left_threshold )
         {

          extrema = joy_1_cx - ((joy_1_cx - joy_1_min_x) * JOYSTICK_RANGE);

          if( joyx < extrema ) {
              joyx = (unsigned int)extrema;
          }

          temp_one = 255.0f - ( 255.0f / (joy_1_cx - extrema) * (joyx - extrema) );

          if( temp_one > 255.0f ) {
              table[INDEX_LEFT] = 255;
          }
          else {
              table[INDEX_LEFT] = (unsigned char)temp_one;
          }

         }

     if( joyy > joy_1_down_threshold )
         {

          extrema = ((joy_1_max_x - joy_1_cx) * JOYSTICK_RANGE) + joy_1_cx;

          if( joyx > extrema ) {
              joyx = (unsigned int)extrema;
          }

          temp_one = 255.0f / (extrema - joy_1_cy) * (joyy - joy_1_cy);

          if( temp_one > 255.0f ) {
              table[INDEX_DOWN] = 255;
          }
          else {
              table[INDEX_DOWN] = (unsigned char)temp_one;
          }

         }
     else if( joyy < joy_1_up_threshold )
         {
          extrema = joy_1_cx - ((joy_1_cx - joy_1_min_x) * JOYSTICK_RANGE);

          if( joyx < extrema ) {
              joyx = (unsigned int)extrema;
          }

          temp_one = 255.0f - ( 255.0f / (joy_1_cy - extrema) * (joyy - extrema) );

          if( temp_one > 255.0f ) {
              table[INDEX_UP] = 255;
          }
          else {
              table[INDEX_UP] = (unsigned char)temp_one;
          }

         }


     if( joy2y < joy_2_down_threshold )
         {
          table[INDEX_UP_THROTTLE] = TRUE;
         }
     else if( joy2y > joy_2_up_threshold )
         {
          table[INDEX_DOWN_THROTTLE] = TRUE;
         }

     hat_up = FALSE;
     hat_down = FALSE;
     hat_left = FALSE;
     hat_right = FALSE;

     if( button1 && button2 && button3 && button4 ) {
         hat_up = TRUE;
     }
     else if( button1 && button2 && button3 ) {
         hat_down = TRUE;
     }
     else if( button1 && button2 && button4 ) {
         hat_right = TRUE;
     }
     else if( button1 && button2 ) {
         hat_left = TRUE;
     }


     if( hat_up ) {

          if( game_configuration.joystick_config.hat_up == ShootLaser ) {
             table[INDEX_FIRE_GUN] = 1;
             last_frame_fire = TRUE;
          }
          else if( game_configuration.joystick_config.hat_up == ShootMissile ) {
             table[INDEX_FIRE_MISSILE] = 1;
          }
          else if( game_configuration.joystick_config.hat_up == ActivateSpecial ) {
             table[INDEX_SPECIAL_WEAPON] = 1;
          }
          else if( game_configuration.joystick_config.hat_up == ActivateStrafe ) {
             strafe_on = TRUE;
          }
          else if( game_configuration.joystick_config.hat_up == ThrottleUp ) {
             table[INDEX_UP_THROTTLE] = 1;
          }
          else if( game_configuration.joystick_config.hat_up == ThrottleDown ) {
             table[INDEX_DOWN_THROTTLE] = 1;
          }
          else if( game_configuration.joystick_config.hat_up == SwitchModes ) {
             if( !last_frame_mode_switch ) {
                 table[INDEX_MODE_SWITCH] = 1;
             }
             last_frame_mode_switch = TRUE;
          }
          else if( game_configuration.joystick_config.hat_up == DoNothing ) {
             ;
          }
          else if( game_configuration.joystick_config.hat_up == StrafeUp ) {
             table[INDEX_SIDESTEP_UP] = 255;
             table[INDEX_UP] = 0;
             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_up == StrafeDown ) {
             table[INDEX_SIDESTEP_DOWN] = 255;
             table[INDEX_DOWN] = 0;
             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_up == StrafeLeft ) {
             table[INDEX_SIDESTEP_LEFT] = 255;
             table[INDEX_LEFT] = 0;
             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_up == StrafeRight ) {
             table[INDEX_SIDESTEP_RIGHT] = 255;
             table[INDEX_RIGHT] = 0;
             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }

          if( last_frame_fire && (game_configuration.joystick_config.hat_up != ShootLaser ) ) {
              table[INDEX_FIRE_GUN] = 1;
          }
     }
     else if( game_configuration.joystick_config.hat_up == SwitchModes ) {
         last_frame_mode_switch = FALSE;
     }


     if( hat_down ) {

          if( game_configuration.joystick_config.hat_down == ShootLaser ) {
             table[INDEX_FIRE_GUN] = 1;
             last_frame_fire = TRUE;
          }
          else if( game_configuration.joystick_config.hat_down == ShootMissile ) {
             table[INDEX_FIRE_MISSILE] = 1;
          }
          else if( game_configuration.joystick_config.hat_down == ActivateSpecial ) {
             table[INDEX_SPECIAL_WEAPON] = 1;
          }
          else if( game_configuration.joystick_config.hat_down == ActivateStrafe ) {
             strafe_on = TRUE;
          }
          else if( game_configuration.joystick_config.hat_down == ThrottleUp ) {
             table[INDEX_UP_THROTTLE] = 1;
          }
          else if( game_configuration.joystick_config.hat_down == ThrottleDown ) {
             table[INDEX_DOWN_THROTTLE] = 1;
          }
          else if( game_configuration.joystick_config.hat_down == SwitchModes ) {
             if( !last_frame_mode_switch ) {
                 table[INDEX_MODE_SWITCH] = 1;
             }
             last_frame_mode_switch = TRUE;
          }
          else if( game_configuration.joystick_config.hat_down == DoNothing ) {
             ;
          }
          else if( game_configuration.joystick_config.hat_down == StrafeUp ) {
             table[INDEX_SIDESTEP_UP] = 255;
             table[INDEX_UP] = 0;
             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_down == StrafeDown ) {
             table[INDEX_SIDESTEP_DOWN] = 255;
             table[INDEX_DOWN] = 0;
             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_down == StrafeLeft ) {
             table[INDEX_SIDESTEP_LEFT] = 255;
             table[INDEX_LEFT] = 0;
             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_down == StrafeRight ) {
             table[INDEX_SIDESTEP_RIGHT] = 255;
             table[INDEX_RIGHT] = 0;
             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }

          if( last_frame_fire && (game_configuration.joystick_config.hat_down != ShootLaser ) ) {
              table[INDEX_FIRE_GUN] = 1;
          }

     }
     else if( game_configuration.joystick_config.hat_down == SwitchModes ) {
         last_frame_mode_switch = FALSE;
     }


     if( hat_right ) {

          if( game_configuration.joystick_config.hat_right == ShootLaser ) {
             table[INDEX_FIRE_GUN] = 1;
             last_frame_fire = TRUE;
          }
          else if( game_configuration.joystick_config.hat_right == ShootMissile ) {
             table[INDEX_FIRE_MISSILE] = 1;
          }
          else if( game_configuration.joystick_config.hat_right == ActivateSpecial ) {
             table[INDEX_SPECIAL_WEAPON] = 1;
          }
          else if( game_configuration.joystick_config.hat_right == ActivateStrafe ) {
             strafe_on = TRUE;
          }
          else if( game_configuration.joystick_config.hat_right == ThrottleUp ) {
             table[INDEX_UP_THROTTLE] = 1;
          }
          else if( game_configuration.joystick_config.hat_right == ThrottleDown ) {
             table[INDEX_DOWN_THROTTLE] = 1;
          }
          else if( game_configuration.joystick_config.hat_right == SwitchModes ) {
             if( !last_frame_mode_switch ) {
                 table[INDEX_MODE_SWITCH] = 1;
             }
             last_frame_mode_switch = TRUE;
          }
          else if( game_configuration.joystick_config.hat_right == DoNothing ) {
             ;
          }
          else if( game_configuration.joystick_config.hat_right == StrafeUp ) {
             table[INDEX_SIDESTEP_UP] = 255;
             table[INDEX_UP] = 0;

             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_right == StrafeDown ) {
             table[INDEX_SIDESTEP_DOWN] = 255;
             table[INDEX_DOWN] = 0;

             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_right == StrafeLeft ) {
             table[INDEX_SIDESTEP_LEFT] = 255;
             table[INDEX_LEFT] = 0;

             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }
          else if( game_configuration.joystick_config.hat_right == StrafeRight ) {
             table[INDEX_SIDESTEP_RIGHT] = 255;
             table[INDEX_RIGHT] = 0;

             if( last_frame_fire == TRUE ) {
                 table[INDEX_FIRE_GUN] = 1;
             }
          }

          if( last_frame_fire && (game_configuration.joystick_config.hat_right != ShootLaser ) ) {
              table[INDEX_FIRE_GUN] = 1;
          }

     }
     else if( game_configuration.joystick_config.hat_right == SwitchModes ) {
         last_frame_mode_switch = FALSE;
     }


     else if( hat_left ) {

          if( game_configuration.joystick_config.hat_left == ShootLaser ) {
             table[INDEX_FIRE_GUN] = 1;
             last_frame_fire = TRUE;
          }
          else if( game_configuration.joystick_config.hat_left == ShootMissile ) {
             table[INDEX_FIRE_MISSILE] = 1;
          }
          else if( game_configuration.joystick_config.hat_left == ActivateSpecial ) {
             table[INDEX_SPECIAL_WEAPON] = 1;
          }
          else if( game_configuration.joystick_config.hat_left == ActivateStrafe ) {
             strafe_on = TRUE;
          }
          else if( game_configuration.joystick_config.hat_left == ThrottleUp ) {
             table[INDEX_UP_THROTTLE] = 1;
          }
          else if( game_configuration.joystick_config.hat_left == ThrottleDown ) {
             table[INDEX_DOWN_THROTTLE] = 1;
          }
          else if( game_configuration.joystick_config.hat_left == SwitchModes ) {
             if( !last_frame_mode_switch ) {
                 table[INDEX_MODE_SWITCH] = 1;
             }
             last_frame_mode_switch = TRUE;
          }
          else if( game_configuration.joystick_config.hat_left == DoNothing ) {
             ;
          }
          else if( game_configuration.joystick_config.hat_left == StrafeUp ) {
             table[INDEX_SIDESTEP_UP] = 255;
             table[INDEX_UP] = 0;
          }
          else if( game_configuration.joystick_config.hat_left == StrafeDown ) {
             table[INDEX_SIDESTEP_DOWN] = 255;
             table[INDEX_DOWN] = 0;
          }
          else if( game_configuration.joystick_config.hat_left == StrafeLeft ) {
             table[INDEX_SIDESTEP_LEFT] = 255;
             table[INDEX_LEFT] = 0;
          }
          else if( game_configuration.joystick_config.hat_left == StrafeRight ) {
             table[INDEX_SIDESTEP_RIGHT] = 255;
             table[INDEX_RIGHT] = 0;
          }

          if( last_frame_fire && (game_configuration.joystick_config.hat_left != ShootLaser ) ) {
              table[INDEX_FIRE_GUN] = 1;
          }

     }
     else if( game_configuration.joystick_config.hat_left == SwitchModes ) {
         last_frame_mode_switch = FALSE;
     }


     if( !hat_up && !hat_down && !hat_right && !hat_left ) {

         if( button1 ) {
             if( game_configuration.joystick_config.button_1 == ShootLaser ) {
                 table[INDEX_FIRE_GUN] = 1;
                 last_frame_fire = TRUE;
             }
             else if( game_configuration.joystick_config.button_1 == ShootMissile ) {
                 table[INDEX_FIRE_MISSILE] = 1;
             }
             else if( game_configuration.joystick_config.button_1 == ActivateSpecial ) {
                 table[INDEX_SPECIAL_WEAPON] = 1;
             }
             else if( game_configuration.joystick_config.button_1 == ActivateStrafe ) {
                 strafe_on = TRUE;
             }
             else if( game_configuration.joystick_config.button_1 == ThrottleUp ) {
                 table[INDEX_UP_THROTTLE] = 1;
             }
             else if( game_configuration.joystick_config.button_1 == ThrottleDown ) {
                 table[INDEX_DOWN_THROTTLE] = 1;
             }
             else if( game_configuration.joystick_config.button_1 == SwitchModes ) {
                 if( !last_frame_mode_switch ) {
                     table[INDEX_MODE_SWITCH] = 1;
                 }
                 last_frame_mode_switch = TRUE;
             }
             else if( game_configuration.joystick_config.button_1 == DoNothing ) {
                 ;
             }
             else if( game_configuration.joystick_config.button_1 == StrafeUp ) {
                 table[INDEX_SIDESTEP_UP] = 255;
                 table[INDEX_UP] = 0;
             }
             else if( game_configuration.joystick_config.button_1 == StrafeDown ) {
                 table[INDEX_SIDESTEP_DOWN] = 255;
                 table[INDEX_DOWN] = 0;
             }
             else if( game_configuration.joystick_config.button_1 == StrafeLeft ) {
                 table[INDEX_SIDESTEP_LEFT] = 255;
                 table[INDEX_LEFT] = 0;
             }
             else if( game_configuration.joystick_config.button_1 == StrafeRight ) {
                 table[INDEX_SIDESTEP_RIGHT] = 255;
                 table[INDEX_RIGHT] = 0;
             }
         }
         else if( game_configuration.joystick_config.button_1 == SwitchModes ) {
             last_frame_mode_switch = FALSE;
         }
         else if( game_configuration.joystick_config.button_1 == ShootLaser ) {
             last_frame_fire = FALSE;
         }


         if( button2 ) {
             if( game_configuration.joystick_config.button_2 == ShootLaser ) {
                 table[INDEX_FIRE_GUN] = 1;
                 last_frame_fire = TRUE;
             }
             else if( game_configuration.joystick_config.button_2 == ShootMissile ) {
                 table[INDEX_FIRE_MISSILE] = 1;
             }
             else if( game_configuration.joystick_config.button_2 == ActivateSpecial ) {
                 table[INDEX_SPECIAL_WEAPON] = 1;
             }
             else if( game_configuration.joystick_config.button_2 == ActivateStrafe ) {
                 strafe_on = TRUE;
             }
             else if( game_configuration.joystick_config.button_2 == ThrottleUp ) {
                 table[INDEX_UP_THROTTLE] = 1;
             }
             else if( game_configuration.joystick_config.button_2 == ThrottleDown ) {
                 table[INDEX_DOWN_THROTTLE] = 1;
             }
             else if( game_configuration.joystick_config.button_2 == SwitchModes ) {
                 if( !last_frame_mode_switch ) {
                     table[INDEX_MODE_SWITCH] = 1;
                 }
                 last_frame_mode_switch = TRUE;
             }
             else if( game_configuration.joystick_config.button_2 == DoNothing ) {
                 ;
             }
             else if( game_configuration.joystick_config.button_2 == StrafeUp ) {
                 table[INDEX_SIDESTEP_UP] = 255;
                 table[INDEX_UP] = 0;
             }
             else if( game_configuration.joystick_config.button_2 == StrafeDown ) {
                 table[INDEX_SIDESTEP_DOWN] = 255;
                 table[INDEX_DOWN] = 0;
             }
             else if( game_configuration.joystick_config.button_2 == StrafeLeft ) {
                 table[INDEX_SIDESTEP_LEFT] = 255;
                 table[INDEX_LEFT] = 0;
             }
             else if( game_configuration.joystick_config.button_2 == StrafeRight ) {
                 table[INDEX_SIDESTEP_RIGHT] = 255;
                 table[INDEX_RIGHT] = 0;
             }
         }
         else if( game_configuration.joystick_config.button_2 == SwitchModes ) {
             last_frame_mode_switch = FALSE;
         }
         else if( game_configuration.joystick_config.button_2 == ShootLaser ) {
             last_frame_fire = FALSE;
         }



         if( button3 ) {
             if( game_configuration.joystick_config.button_3 == ShootLaser ) {
                 table[INDEX_FIRE_GUN] = 1;
                 last_frame_fire = TRUE;
             }
             else if( game_configuration.joystick_config.button_3 == ShootMissile ) {
                 table[INDEX_FIRE_MISSILE] = 1;
             }
             else if( game_configuration.joystick_config.button_3 == ActivateSpecial ) {
                 table[INDEX_SPECIAL_WEAPON] = 1;
             }
             else if( game_configuration.joystick_config.button_3 == ActivateStrafe ) {
                 strafe_on = TRUE;
             }
             else if( game_configuration.joystick_config.button_3 == ThrottleUp ) {
                 table[INDEX_UP_THROTTLE] = 1;
             }
             else if( game_configuration.joystick_config.button_3 == ThrottleDown ) {
                 table[INDEX_DOWN_THROTTLE] = 1;
             }
             else if( game_configuration.joystick_config.button_3 == SwitchModes ) {
                 if( !last_frame_mode_switch ) {
                     table[INDEX_MODE_SWITCH] = 1;
                 }
                 last_frame_mode_switch = TRUE;
             }
             else if( game_configuration.joystick_config.button_3 == DoNothing ) {
                 ;
             }
             else if( game_configuration.joystick_config.button_3 == StrafeUp ) {
                 table[INDEX_SIDESTEP_UP] = 255;
                 table[INDEX_UP] = 0;
             }
             else if( game_configuration.joystick_config.button_3 == StrafeDown ) {
                 table[INDEX_SIDESTEP_DOWN] = 255;
                 table[INDEX_DOWN] = 0;
             }
             else if( game_configuration.joystick_config.button_3 == StrafeLeft ) {
                 table[INDEX_SIDESTEP_LEFT] = 255;
                 table[INDEX_LEFT] = 0;
             }
             else if( game_configuration.joystick_config.button_3 == StrafeRight ) {
                 table[INDEX_SIDESTEP_RIGHT] = 255;
                 table[INDEX_RIGHT] = 0;
             }
         }
         else if( game_configuration.joystick_config.button_3 == SwitchModes ) {
             last_frame_mode_switch = FALSE;
         }
         else if( game_configuration.joystick_config.button_3 == ShootLaser ) {
             last_frame_fire = FALSE;
         }



         if( button4 ) {
             if( game_configuration.joystick_config.button_4 == ShootLaser ) {
                 table[INDEX_FIRE_GUN] = 1;
                 last_frame_fire = TRUE;
             }
             else if( game_configuration.joystick_config.button_4 == ShootMissile ) {
                 table[INDEX_FIRE_MISSILE] = 1;
             }
             else if( game_configuration.joystick_config.button_4 == ActivateSpecial ) {
                 table[INDEX_SPECIAL_WEAPON] = 1;
             }
             else if( game_configuration.joystick_config.button_4 == ActivateStrafe ) {
                 strafe_on = TRUE;
             }
             else if( game_configuration.joystick_config.button_4 == ThrottleUp ) {
                 table[INDEX_UP_THROTTLE] = 1;
             }
             else if( game_configuration.joystick_config.button_4 == ThrottleDown ) {
                 table[INDEX_DOWN_THROTTLE] = 1;
             }
             else if( game_configuration.joystick_config.button_4 == SwitchModes ) {
                 if( !last_frame_mode_switch ) {
                     table[INDEX_MODE_SWITCH] = 1;
                 }
                 last_frame_mode_switch = TRUE;
             }
             else if( game_configuration.joystick_config.button_4 == DoNothing ) {
                 ;
             }
             else if( game_configuration.joystick_config.button_4 == StrafeUp ) {
                 table[INDEX_SIDESTEP_UP] = 255;
                 table[INDEX_UP] = 0;
             }
             else if( game_configuration.joystick_config.button_4 == StrafeDown ) {
                 table[INDEX_SIDESTEP_DOWN] = 255;
                 table[INDEX_DOWN] = 0;
             }
             else if( game_configuration.joystick_config.button_4 == StrafeLeft ) {
                 table[INDEX_SIDESTEP_LEFT] = 255;
                 table[INDEX_LEFT] = 0;
             }
             else if( game_configuration.joystick_config.button_4 == StrafeRight ) {
                 table[INDEX_SIDESTEP_RIGHT] = 255;
                 table[INDEX_RIGHT] = 0;
             }
         }
         else if( game_configuration.joystick_config.button_4 == SwitchModes ) {
             last_frame_mode_switch = FALSE;
         }
         else if( game_configuration.joystick_config.button_4 == ShootLaser ) {
             last_frame_fire = FALSE;
         }
     }


     if( (key_table[config->strafe] && table[INDEX_RIGHT]) ||
         (strafe_on && table[INDEX_RIGHT]) ) {

         table[INDEX_SIDESTEP_RIGHT] = table[INDEX_RIGHT];
         table[INDEX_RIGHT] = 0;
     }

     if( (key_table[config->strafe] && table[INDEX_UP]) ||
         (strafe_on && table[INDEX_UP]) ) {

         table[INDEX_SIDESTEP_UP] = table[INDEX_UP];
         table[INDEX_UP] = 0;
     }

     if( (key_table[config->strafe] && table[INDEX_DOWN]) ||
         (strafe_on && table[INDEX_DOWN]) ) {

         table[INDEX_SIDESTEP_DOWN] = table[INDEX_DOWN];
         table[INDEX_DOWN] = 0;
     }

     if( (key_table[config->strafe] && table[INDEX_LEFT]) ||
         (strafe_on && table[INDEX_LEFT]) ) {

         table[INDEX_SIDESTEP_LEFT] = table[INDEX_LEFT];
         table[INDEX_LEFT] = 0;
     }

} /* end Get_Flightstick_Table */




void Show_Calibration( int joystick_number )
    {
     if( joystick_number == 1 )
         {
          printf("\nmax x=%u, max y=%u,min x=%u,min y=%u,cx=%u,cy=%u \n",joy_1_max_x,
                 joy_1_max_y, joy_1_min_x, joy_1_min_y, joy_1_cx, joy_1_cy);
         }
    else
         {
          printf("\nmax x=%u, max y=%u,min x=%u,min y=%u,cx=%u,cy=%u \n",joy_2_max_x,
                 joy_2_max_y, joy_2_min_x, joy_2_min_y, joy_2_cx, joy_2_cy);
         }

    } /* End of Show_Calibration */



void calibrate_joystick_upper_left()
{
    unsigned int joyx, joyy;

    joy_1_min_x=10000;
    joy_1_min_y=10000;

    joyx = joyy = 0;

    while( !Buttons(BUTTON_1_A) ) {

        delay(JOYSTICK_DELAY);
        joyx = Joystick(JOYSTICK_1_X);
        delay(JOYSTICK_DELAY);
        joyy = Joystick(JOYSTICK_1_Y);

        if( joyx < joy_1_min_x )
            joy_1_min_x=joyx;
        if( joyy < joy_1_min_y )
            joy_1_min_y=joyy;

    } /* End while not button press */

    /* loop until the button is up */

    while( Buttons(BUTTON_1_A) );

}

void calibrate_joystick_lower_right()
{
    unsigned int joyx, joyy;


   joy_1_max_x=0;
   joy_1_max_y=0;

   joyx = joyy = 0;

   while( !Buttons(BUTTON_1_A) ) {

       delay(JOYSTICK_DELAY);
       joyx = Joystick(JOYSTICK_1_X);
       delay(JOYSTICK_DELAY);
       joyy = Joystick(JOYSTICK_1_Y);

       if( joyx > joy_1_max_x )
           joy_1_max_x=joyx;
       if( joyy > joy_1_max_y )
           joy_1_max_y=joyy;

   } /* End while not button press */

   /* loop until the button is up */

   while( Buttons(BUTTON_1_A) );

}

void calibrate_joystick_center()
{
    unsigned int joyx, joyy;

    joyx = joyy = 0;

    while( !Buttons(BUTTON_1_A) ) {

        delay(JOYSTICK_DELAY);
        joyx = Joystick(JOYSTICK_1_X);
        delay(JOYSTICK_DELAY);
        joyy = Joystick(JOYSTICK_1_Y);

    } /* End while not button press */

    /* loop until the button is up */

    while( Buttons(BUTTON_1_A) );

    joy_1_cx = joyx;
    joy_1_cy = joyy;

    joy_1_up_threshold    = (unsigned int)(joy_1_cy - ((joy_1_cy - joy_1_min_y) / JOYSTICK_THRESHOLD));
    joy_1_down_threshold  = (unsigned int)(joy_1_cy + ((joy_1_max_y - joy_1_cy) / JOYSTICK_THRESHOLD));
    joy_1_left_threshold  = (unsigned int)(joy_1_cx - ((joy_1_cx - joy_1_min_x) / JOYSTICK_THRESHOLD));
    joy_1_right_threshold = (unsigned int)(joy_1_cx + ((joy_1_max_x - joy_1_cx) / JOYSTICK_THRESHOLD));

}

void calibrate_flightstick_throttle()
{
    unsigned int joyx, joyy;
    unsigned int joy2y;

    joy_2_min_y = 10000;
    joy_2_max_y = 0;

    joyx = joyy = joy2y = 0;

    while( !Buttons(BUTTON_1_A) ) {

        delay(JOYSTICK_DELAY);
        joyx = Joystick(JOYSTICK_1_X);
        delay(JOYSTICK_DELAY);
        joyy = Joystick(JOYSTICK_1_Y);
        delay(JOYSTICK_DELAY);
        joy2y = Joystick(JOYSTICK_2_Y);

        if( joy2y < joy_2_min_y )
            joy_2_min_y = joy2y;
        if( joy2y > joy_2_max_y )
            joy_2_max_y = joy2y;

    } /* End while not button press */

    /* loop until the button is up */

    while( Buttons(BUTTON_1_A) );

    joy_2_cy = (joy_2_max_y - joy_2_min_y) / 2;

    joy_2_up_threshold   = (unsigned int)(joy_2_cy - ((joy_2_cy - joy_2_min_y) / THROTTLE_THRESHOLD));
    joy_2_down_threshold = (unsigned int)(joy_2_cy + ((joy_2_max_y - joy_2_cy) / THROTTLE_THRESHOLD));

}

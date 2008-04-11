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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "keys.h"

extern game_configuration_type game_configuration;

// AJT: HACK, this wouldn't be necessary if we had proper keyboard tracking...
// If the user holds down the mode switch key the ship will keep
// Switching between air and ground rapidly, resulting in nothing happening
// This flag emulates the original Cylindrix behavior which was to only switch
// Modes on the keyup
int g_ModeSwitch = 0;

/* OK...now the input table and the key table are different...
   so we must change the input table based on the key table...
   eventually which keys map to what should me loaded from a
   file and configurable by the user.  For now it is hardcoded */
void Get_Key_Table( input_table table ) 
{
	int i;

	// Johnm 12/2/2001 - NEED TO COMPLETE FUNCTIONALITY HERE
	// AJT: TODO: need to hook up keyboard config.

	// clear input table.
	for( i = 0; i < INPUT_TABLE_SIZE; i++ )
		table[i] = 0;

	// AJT: HACK for switch mode key
 	if ( SYS_KeyDown(KEY_S) ) 
	{
		// Indicate key was pressed
		g_ModeSwitch = 1;
	}
	else 
	{
		// Indicate key was released
		if ( g_ModeSwitch == 1 ) 
		{
			table[INDEX_MODE_SWITCH] = 255;
			g_ModeSwitch = 0;
		}
	}

	if ( SYS_KeyDown(KEY_UP_ARROW) ) table[INDEX_UP] = 255;
	if ( SYS_KeyDown(KEY_DOWN_ARROW) ) table[INDEX_DOWN] = 255;
	if ( SYS_KeyDown(KEY_LEFT_ARROW) ) table[INDEX_LEFT] = 255;
	if ( SYS_KeyDown(KEY_RIGHT_ARROW) ) table[INDEX_RIGHT] = 255;
	if ( SYS_KeyDown(KEY_CONTROL) ) table[INDEX_FIRE_GUN] = 255;
	if ( SYS_KeyDown(KEY_SPACE) ) table[INDEX_FIRE_MISSILE] = 255;
	if ( SYS_KeyDown(KEY_X) ) table[INDEX_SPECIAL_WEAPON] = 255;
	if ( SYS_KeyDown(KEY_A) ) table[INDEX_UP_THROTTLE] = 255;
	if ( SYS_KeyDown(KEY_Z) ) table[INDEX_DOWN_THROTTLE] = 255;	

	// alt is the strafe modifyier
	if ( SYS_KeyDown(KEY_ALT) ) 
	{
		if ( SYS_KeyDown(KEY_RIGHT_ARROW) ) 
		{
			table[INDEX_RIGHT] = 0;
			table[INDEX_SIDESTEP_RIGHT] = 255;
		}
		if ( SYS_KeyDown(KEY_LEFT_ARROW) ) 
		{
			table[INDEX_LEFT] = 0;
			table[INDEX_SIDESTEP_LEFT] = 255;
		}
		if ( SYS_KeyDown(KEY_UP_ARROW) ) 
		{
			table[INDEX_UP] = 0;
			table[INDEX_SIDESTEP_UP] = 255;
		}
		if ( SYS_KeyDown(KEY_DOWN_ARROW) ) 
		{
			table[INDEX_DOWN] = 0;
			table[INDEX_SIDESTEP_DOWN] = 255;
		}
	}

	// original version of code that used keyboard config lookup.
	/*
     int i;
     KeyboardConfig *config; 

     
     config = &game_configuration.keyboard_config;


     for( i = 0; i < INPUT_TABLE_SIZE; i++ )
         table[i] = 0;

     if( key_table[config->up] )
         table[INDEX_UP] = 255;
     if( key_table[config->down] )
         table[INDEX_DOWN] = 255;
     if( key_table[config->left] )
         table[INDEX_LEFT] = 255;
     if( key_table[config->right] )
         table[INDEX_RIGHT] = 255;

     if( key_table[config->missile] )
         table[INDEX_FIRE_MISSILE] = 1;
     if( key_table[config->laser] )
         table[INDEX_FIRE_GUN] = 1;

     if( key_table[config->up_throttle] )
         table[INDEX_UP_THROTTLE] = 1;
     if( key_table[config->down_throttle] )
         table[INDEX_DOWN_THROTTLE] = 1;


     if( key_table[config->strafe] )
         {
          if( key_table[config->right] )
              {
               table[INDEX_RIGHT] = 0;
               table[INDEX_SIDESTEP_RIGHT] = 255;
              }
          if( key_table[config->left] )
              {
               table[INDEX_LEFT] = 0;
               table[INDEX_SIDESTEP_LEFT] = 255;
              }

          if( key_table[config->up] )
              {
               table[INDEX_UP] = 0;
               table[INDEX_SIDESTEP_UP] = 255;
              }
          if( key_table[config->down] )
              {
               table[INDEX_DOWN] = 0;
               table[INDEX_SIDESTEP_DOWN] = 255;
              }

         }

     if( key_table[config->special] )
         {
          table[INDEX_SPECIAL_WEAPON] = TRUE;
         }

     if( key_table[config->wing_1_pylon] )
           table[INDEX_L_COMMAND_PYLON] = TRUE;
     if( key_table[config->wing_1_attack] )
           table[INDEX_L_COMMAND_ATTACK] = TRUE;
     if( key_table[config->wing_1_attack_rb] )
           table[INDEX_L_COMMAND_ATTACK_RADAR] = TRUE;
     if( key_table[config->wing_1_defend_rb] )
           table[INDEX_L_COMMAND_DEFEND_RADAR] = TRUE;
     if( key_table[config->wing_1_group] )
           table[INDEX_L_COMMAND_GROUP] = TRUE;
     if( key_table[config->wing_1_cancel] )
           table[INDEX_L_COMMAND_CANCEL] = TRUE;
     if( key_table[config->wing_2_pylon] )
           table[INDEX_R_COMMAND_PYLON] = TRUE;
     if( key_table[config->wing_2_attack] )
           table[INDEX_R_COMMAND_ATTACK] = TRUE;
     if( key_table[config->wing_2_attack_rb] )
           table[INDEX_R_COMMAND_ATTACK_RADAR] = TRUE;
     if( key_table[config->wing_2_defend_rb] )
           table[INDEX_R_COMMAND_DEFEND_RADAR] = TRUE;
     if( key_table[config->wing_2_group] )
           table[INDEX_R_COMMAND_GROUP] = TRUE;
     if( key_table[config->wing_2_cancel] )
           table[INDEX_R_COMMAND_CANCEL] = TRUE;


     if( break_table[config->mode_switch] )
         {
          table[INDEX_MODE_SWITCH] = 1;
          break_table[config->mode_switch] = 0;
         }

*/    
} /* End of Get_Key_Table */

int Check_Input_Table( int index, input_table table ) 
{
     return table[ index ];
}

void Index_To_String( string scan_string, int index ) {
	switch( index )
	{
		case KEY_A:
			sprintf( scan_string, "A" );
			break;
		case KEY_B:
			sprintf( scan_string, "B" );
			break;
		case KEY_C:
			sprintf( scan_string, "C" );
			break;
		case KEY_D:
			sprintf( scan_string, "D" );
			break;
		case KEY_E:
			sprintf( scan_string, "E" );
			break;
		case KEY_F:
			sprintf( scan_string, "F" );
			break;
		case KEY_G:
			sprintf( scan_string, "G" );
			break;
		case KEY_H:
			sprintf( scan_string, "H" );
			break;
		case KEY_I:
			sprintf( scan_string, "I" );
			break;
		case KEY_J:
			sprintf( scan_string, "J" );
			break;
		case KEY_K:
			sprintf( scan_string, "K" );
			break;
		case KEY_L:
			sprintf( scan_string, "L" );
			break;
		case KEY_M:
			sprintf( scan_string, "M" );
			break;
		case KEY_N:
			sprintf( scan_string, "N" );
			break;
		case KEY_O:
			sprintf( scan_string, "O" );
			break;
		case KEY_P:
			sprintf( scan_string, "P" );
			break;
		case KEY_Q:
			sprintf( scan_string, "Q" );
			break;
		case KEY_R:
			sprintf( scan_string, "R" );
			break;
		case KEY_S:
			sprintf( scan_string, "S" );
			break;
		case KEY_T:
			sprintf( scan_string, "T" );
			break;
		case KEY_U:
			sprintf( scan_string, "U" );
			break;
		case KEY_V:
			sprintf( scan_string, "V" );
			break;
		case KEY_W:
			sprintf( scan_string, "W" );
			break;
		case KEY_X:
			sprintf( scan_string, "X" );
			break;
		case KEY_Y:
			sprintf( scan_string, "Y" );
			break;
		case KEY_Z:
			sprintf( scan_string, "Z" );
			break;
		case KEY_1:
			sprintf( scan_string, "1" );
			break;
		case KEY_2:
			sprintf( scan_string, "2" );
			break;
		case KEY_3:
			sprintf( scan_string, "3" );
			break;
		case KEY_4:
			sprintf( scan_string, "4" );
			break;
		case KEY_5:
			sprintf( scan_string, "5" );
			break;
		case KEY_6:
			sprintf( scan_string, "6" );
			break;
		case KEY_7:
			sprintf( scan_string, "7" );
			break;
		case KEY_8:
			sprintf( scan_string, "8" );
			break;
		case KEY_9:
			sprintf( scan_string, "9" );
			break;
		case KEY_0:
			sprintf( scan_string, "0" );
			break;
		case KEY_UP_ARROW:
			sprintf( scan_string, "UP ARROW" );
			break;
		case KEY_DOWN_ARROW:
			sprintf( scan_string, "DOWN ARROW" );
			break;
		case KEY_RIGHT_ARROW:
			sprintf( scan_string, "RIGHT ARROW" );
			break;
		case KEY_LEFT_ARROW:
			sprintf( scan_string, "LEFT ARROW" );
			break;
		case KEY_CONTROL:
			sprintf( scan_string, "CONTROL");
			break;
		case KEY_ALT:
			sprintf( scan_string, "ALT");
			break;
		case KEY_SPACE:
			sprintf( scan_string, "SPACE BAR");
			break;
		case KEY_MINUS:
			sprintf( scan_string, "MINUS");
			break;
		case KEY_EQUALS:
			sprintf( scan_string, "PLUS");
			break;
		case KEY_LEFT_BRACKET:
			sprintf( scan_string, "LEFT BRACKET");
			break;
		case KEY_RIGHT_BRACKET:
			sprintf( scan_string, "RIGHT BRACKET");
			break;
		case KEY_BACKSLASH:
			sprintf( scan_string, "BACKSLASH");
			break;
		case KEY_SEMICOLON:
			sprintf( scan_string, "SEMICOLON");
			break;
		case KEY_APOSTROPHE:
			sprintf( scan_string, "APOSTROPHE");
			break;
		case KEY_COMMA:
			sprintf( scan_string, "LESS THAN");
			break;
		case KEY_PERIOD:
			sprintf( scan_string, "GREATER THAN");
			break;
		case KEY_SLASH:
			sprintf( scan_string, "SLASH");
			break;
		case KEY_TILDE:
			sprintf( scan_string, "TILDE");
			break;
		case KEY_TAB:
			sprintf( scan_string, "TAB");
			break;
		case KEY_F1:
			sprintf( scan_string, "F1");
			break;
		case KEY_F2:
			sprintf( scan_string, "F2");
			break;
		case KEY_F3:
			sprintf( scan_string, "F3");
			break;
		case KEY_F4:
			sprintf( scan_string, "F4");
			break;
		case KEY_F5:
			sprintf( scan_string, "F5");
			break;
		case KEY_F6:
			sprintf( scan_string, "F6");
			break;
		case KEY_F7:
			sprintf( scan_string, "F7");
			break;
		case KEY_F8:
			sprintf( scan_string, "F8");
			break;
		case KEY_F9:
			sprintf( scan_string, "F9");
			break;
		case KEY_F10:
			sprintf( scan_string, "F10");
			break;
		case KEY_F11:
			sprintf( scan_string, "F11");
			break;
		case KEY_F12:
			sprintf( scan_string, "F12");
			break;
		case KEY_SHIFT:
			sprintf( scan_string, "SHIFT");
			break;
		case KEY_ENTER:
			sprintf( scan_string, "ENTER");
			break;
		case KEY_BACKSPACE:
			sprintf( scan_string, "BACKSPACE");
			break;
		default:
		  sprintf( scan_string, "NONE" );
		  break;
	}
}

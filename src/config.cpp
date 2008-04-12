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
#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "config.h"
#include "serial.h"

extern const char* g_DataPath;


/* Parses input for a pilot name that is inclosed in angle brakets */

void get_pilot_name( FILE *fp, char *str )
{
    char temp_str[80];
    char ch;
    int index = 0;
    
    /* find the open angle braket */
    
    while( fgetc( fp ) != '<' );
    
    /* read until we hit close angle braket */
    
    while( (ch = fgetc( fp )) != '>' ) {        
        temp_str[index] = ch;
        index++;
    }
    
    /* make sure we end the string with the null character */
    
    temp_str[index] = '\0';
    
    strcpy( str, temp_str );    
}


void Load_Game_Configuration( game_configuration_type *game_configuration )
    {
     FILE *fp;
     char temp_string[80]; /* To get the comments out of the file */     
     int test_number;
	char newfilename[512];

	sprintf(newfilename,"%sgamedata/game.cfg",g_DataPath);


     fp = fopen( newfilename, "r" );


     fscanf(fp, " %d ", &test_number ); /* Get com port */
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */


     switch( test_number )
         {
          case 1 :
              game_configuration->serial_com_port = COM_1;
              break;
          case 2 :
              game_configuration->serial_com_port = COM_2;
              break;
          case 3 :
              game_configuration->serial_com_port = COM_3;
              break;
          case 4 :
              game_configuration->serial_com_port = COM_4;
              break;

          default :
              printf("Error in com port loading \n");
              exit(0);
              break;

         }


     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */

     switch( test_number )
         {
          case 1 :
              game_configuration->serial_baud = SER_BAUD_9600;
              break;
          case 2 :
              game_configuration->serial_baud = SER_BAUD_19200;
              break;
          default :
              printf("Error loading serial baud \n");
              exit(0);

         }

     fscanf(fp, " %d ", &test_number ); /* Read the number for the modem port */
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */


     switch( test_number )
         {
          case 1 :
              game_configuration->modem_com_port = COM_1;
              break;
          case 2 :
              game_configuration->modem_com_port = COM_2;
              break;
          case 3 :
              game_configuration->modem_com_port = COM_3;
              break;
          case 4 :
              game_configuration->serial_com_port = COM_4;
              break;

          default :
              printf("Error in com port loading \n");
              exit(0);
              break;

         }


     fscanf(fp, " %d ", &test_number ); /* Read the modem baud */
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */


     switch( test_number )
         {
          case 1 :
              game_configuration->modem_baud = SER_BAUD_9600;
              break;
          case 2 :
              game_configuration->modem_baud = SER_BAUD_19200;
              break;
          default :
              printf("Error loading modem baud \n");
              exit(0);

         }

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->joystick = (JoystickType)test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->mouse_active = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->world_detail_level = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->tube_detail_level = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->hubs_on = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->sound_on = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->music_on = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->voices_on = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->sound_vol = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->music_vol = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->voice_vol = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->modem_game = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat the comment */
     game_configuration->serial_game = test_number;

     /* read in each players vehicle */

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->red0_vehicle = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->red1_vehicle = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->red2_vehicle = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->blue0_vehicle = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->blue1_vehicle = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->blue2_vehicle = test_number;

     /* read in each players ai */

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->red0_ai = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->red1_ai = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->red2_ai = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->blue0_ai = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->blue1_ai = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->blue2_ai = test_number;

     /* read in the level filename */

     fscanf(fp, " %s ", game_configuration->cylinder_filename );
     fscanf(fp, " %s ", temp_string );

     /* read in the keyboard_config structure */

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.laser = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.missile = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.special = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.mode_switch = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.up = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.down = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.left = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.right = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.up_throttle = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.down_throttle = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.strafe = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_1_pylon = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_1_attack = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_1_attack_rb = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_1_defend_rb = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_1_group = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_1_cancel = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_2_pylon = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_2_attack = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_2_attack_rb = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_2_defend_rb = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_2_group = test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->keyboard_config.wing_2_cancel = test_number;

     /* read in the joystick_config structure */

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->joystick_config.button_1 = (JoystickButtonAction)test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->joystick_config.button_2 = (JoystickButtonAction)test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->joystick_config.button_3 = (JoystickButtonAction)test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->joystick_config.button_4 = (JoystickButtonAction)test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->joystick_config.hat_up = (JoystickButtonAction)test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->joystick_config.hat_down = (JoystickButtonAction)test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->joystick_config.hat_left = (JoystickButtonAction)test_number;

     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string ); /* Eat comment */
     game_configuration->joystick_config.hat_right = (JoystickButtonAction)test_number;
     
     /* load in the pilot names */
     
     get_pilot_name( fp, game_configuration->pilot_names.slot0 );
     get_pilot_name( fp, game_configuration->pilot_names.slot1 );
     get_pilot_name( fp, game_configuration->pilot_names.slot2 );
     get_pilot_name( fp, game_configuration->pilot_names.slot3 );
     get_pilot_name( fp, game_configuration->pilot_names.slot4 );
     get_pilot_name( fp, game_configuration->pilot_names.slot5 );
     get_pilot_name( fp, game_configuration->pilot_names.slot6 );
     get_pilot_name( fp, game_configuration->pilot_names.slot7 );
     
     fscanf(fp, " %d ", &test_number );
     fscanf(fp, " %s ", temp_string );  /* Eat comment */
     game_configuration->animations_on = test_number;

     if( !feof(fp) ) {
	     fscanf(fp, " %d ", &test_number );
	     fscanf(fp, " %s ", temp_string );  /* Eat comment */
	     game_configuration->two_player = test_number;
     }
     if( !feof(fp) ) {
	     fscanf(fp, " %d ", &test_number );
	     fscanf(fp, " %s ", temp_string );  /* Eat comment */
	     game_configuration->fullscreen = test_number;
     }
 
     fclose( fp ); /* Close the file */

    } /* End of Load_Game_Configuration() */




void Save_Game_Configuration( game_configuration_type *game_configuration )
    {
     FILE *fp;
     char *write_strings[13] =
     { "Serial_Com_Port", "Serial_Baud", "Modem_Com_Port", "Modem_Baud",
       "Joystick", "Mouse_Active", "World_Detail_Level",
       "Tube_Detail_Level", "Hubs_On", "Sound_On", "Music_On", "Modem_Game",
       "Serial_Game" };

     int temp_number;

	char newfilename[512];

	sprintf(newfilename,"%sgamedata/game.cfg",g_DataPath);


     fp = fopen( newfilename, "r" );


     switch( game_configuration->serial_com_port )
         {
          case COM_1 :
              temp_number = 1;
              break;
          case COM_2 :
              temp_number = 2;
              break;
          case COM_3 :
              temp_number = 3;
              break;
          case COM_4 :
              temp_number = 4;
              break;
          default :
              printf("Error in com port saving \n");
              exit(0);
              break;

         }

     fprintf(fp, "%d ", temp_number );
     fprintf(fp, " %s \n", write_strings[0] ); /* Write the comment */


     switch( game_configuration->serial_baud )
         {
          case SER_BAUD_9600 :
              temp_number = 1;
              break;
          case SER_BAUD_19200 :
              temp_number = 2;
              break;
          default :
              printf("Error in serial baud saving \n");
              exit(0);
              break;
         }

     fprintf(fp, "%d ", temp_number );  /* Write the number for the serial baud */
     fprintf(fp," %s \n", write_strings[1] ); /* Write the comment */


     switch( game_configuration->modem_com_port )
         {
          case COM_1 :
              temp_number = 1;
              break;
          case COM_2 :
              temp_number = 2;
              break;
          case COM_3 :
              temp_number = 3;
              break;
          case COM_4 :
              temp_number = 4;
              break;
          default :
              printf("Error in modem com port saving \n");
              exit(0);
              break;
         }

     fprintf(fp, "%d ", temp_number );
     fprintf(fp, " %s \n", write_strings[2] ); /* Write the comment */



     switch( game_configuration->modem_baud )
         {
          case SER_BAUD_9600 :
              temp_number = 1;
              break;
          case SER_BAUD_19200 :
              temp_number = 2;
              break;
          default :
              printf("Error in modem baud saving \n");
              exit(0);
              break;
         }

     fprintf(fp, "%d ", temp_number );  /* Write the number for the serial baud */
     fprintf(fp, " %s \n", write_strings[3] ); /* Write the comment */


     fprintf(fp, "%d ", game_configuration->joystick );
     fprintf(fp, " %s \n", write_strings[4] ); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->mouse_active );
     fprintf(fp, " %s \n" , write_strings[5] ); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->world_detail_level );
     fprintf(fp, " %s \n", write_strings[6] ); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->tube_detail_level );
     fprintf(fp, " %s \n", write_strings[7] ); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->hubs_on );
     fprintf(fp, " %s \n", write_strings[8] );

     fprintf(fp, "%d ", game_configuration->sound_on );
     fprintf(fp, " %s \n", write_strings[9] ); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->music_on );
     fprintf(fp, " %s \n", write_strings[10] ); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->voices_on );
     fprintf(fp, " voices_on \n"); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->sound_vol );
     fprintf(fp, " sound_vol \n"); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->music_vol );
     fprintf(fp, " music_vol \n"); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->voice_vol );
     fprintf(fp, " voice_vol \n"); /* Write the comment */

     /* modem_game is always 0 */

     fprintf(fp, "0 " );
     fprintf(fp, " %s \n", write_strings[11] ); /* Write the comment */

     /* serial_game is always 0 */

     fprintf(fp, "0 " );
     fprintf(fp, " %s \n", write_strings[12] ); /* Write the comment */

     /* write each players vehicle selection */

     fprintf(fp, "%d ", game_configuration->red0_vehicle );
     fprintf(fp, " red0_vehicle\n" );

     fprintf(fp, "%d ", game_configuration->red1_vehicle );
     fprintf(fp, " red1_vehicle\n" );

     fprintf(fp, "%d ", game_configuration->red2_vehicle );
     fprintf(fp, " red2_vehicle\n" );

     fprintf(fp, "%d ", game_configuration->blue0_vehicle );
     fprintf(fp, " blue0_vehicle\n" );

     fprintf(fp, "%d ", game_configuration->blue1_vehicle );
     fprintf(fp, " blue1_vehicle\n" );

     fprintf(fp, "%d ", game_configuration->blue2_vehicle );
     fprintf(fp, " blue2_vehicle\n" );

     /* write each players ai selection */

     fprintf(fp, "%d ", game_configuration->red0_ai );
     fprintf(fp, " red0_ai\n" );

     fprintf(fp, "%d ", game_configuration->red1_ai );
     fprintf(fp, " red1_ai\n" );

     fprintf(fp, "%d ", game_configuration->red2_ai );
     fprintf(fp, " red2_ai\n" );

     fprintf(fp, "%d ", game_configuration->blue0_ai );
     fprintf(fp, " blue0_ai\n" );

     fprintf(fp, "%d ", game_configuration->blue1_ai );
     fprintf(fp, " blue1_ai\n" );

     fprintf(fp, "%d ", game_configuration->blue2_ai );
     fprintf(fp, " blue2_ai\n" );

     /* write name of level file */

     fprintf(fp, "%s ", game_configuration->cylinder_filename );
     fprintf(fp, " cylinder_filename\n" );

     /* write the keyboard_config structure */

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.laser );
     fprintf(fp, " laser\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.missile );
     fprintf(fp, " missile\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.special );
     fprintf(fp, " special\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.mode_switch );
     fprintf(fp, " mode_switch\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.up );
     fprintf(fp, " up\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.down );
     fprintf(fp, " down\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.left );
     fprintf(fp, " left\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.right );
     fprintf(fp, " right\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.up_throttle );
     fprintf(fp, " up_throttle\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.down_throttle );
     fprintf(fp, " down_throttle\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.strafe );
     fprintf(fp, " strafe\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_1_pylon );
     fprintf(fp, " wing_1_pylon\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_1_attack );
     fprintf(fp, " wing_1_attack\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_1_attack_rb );
     fprintf(fp, " wing_1_attack_rb\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_1_defend_rb );
     fprintf(fp, " wing_1_defend_rb\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_1_group );
     fprintf(fp, " wing_1_group\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_1_cancel );
     fprintf(fp, " wing_1_cancel\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_2_pylon );
     fprintf(fp, " wing_2_pylon\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_2_attack );
     fprintf(fp, " wing_2_attack\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_2_attack_rb );
     fprintf(fp, " wing_2_attack_rb\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_2_defend_rb );
     fprintf(fp, " wing_2_defend_rb\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_2_group );
     fprintf(fp, " wing_2_group\n" );

     fprintf(fp, "%d ", (int)game_configuration->keyboard_config.wing_2_cancel );
     fprintf(fp, " wing_2_cancel\n" );

     /* write the joystick config structure */

     fprintf(fp, "%d ", game_configuration->joystick_config.button_1 );
     fprintf(fp, " button_1\n" );

     fprintf(fp, "%d ", game_configuration->joystick_config.button_2 );
     fprintf(fp, " button_2\n" );

     fprintf(fp, "%d ", game_configuration->joystick_config.button_3 );
     fprintf(fp, " button_3\n" );

     fprintf(fp, "%d ", game_configuration->joystick_config.button_4 );
     fprintf(fp, " button_4\n" );

     fprintf(fp, "%d ", game_configuration->joystick_config.hat_up );
     fprintf(fp, " hat_up\n" );

     fprintf(fp, "%d ", game_configuration->joystick_config.hat_down );
     fprintf(fp, " hat_down\n" );

     fprintf(fp, "%d ", game_configuration->joystick_config.hat_left );
     fprintf(fp, " hat_left\n" );

     fprintf(fp, "%d ", game_configuration->joystick_config.hat_right );
     fprintf(fp, " hat_right\n" );
     
     /* save pilot names */
     
     fprintf(fp, "<%s>\n", game_configuration->pilot_names.slot0 );
     fprintf(fp, "<%s>\n", game_configuration->pilot_names.slot1 );
     fprintf(fp, "<%s>\n", game_configuration->pilot_names.slot2 );
     fprintf(fp, "<%s>\n", game_configuration->pilot_names.slot3 );
     fprintf(fp, "<%s>\n", game_configuration->pilot_names.slot4 );
     fprintf(fp, "<%s>\n", game_configuration->pilot_names.slot5 );
     fprintf(fp, "<%s>\n", game_configuration->pilot_names.slot6 );
     fprintf(fp, "<%s>\n", game_configuration->pilot_names.slot7 );
     
     fprintf(fp, "%d ", game_configuration->animations_on );
     fprintf(fp, " %s\n", "animations_on" ); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->two_player );
     fprintf(fp, " %s\n", "two_player" ); /* Write the comment */

     fprintf(fp, "%d ", game_configuration->fullscreen );
     fprintf(fp, " %s\n", "fullscreen" ); /* Write the comment */
     
     fclose( fp );

    } /* End of Save_Game_Configuration() */

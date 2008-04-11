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
#include "commands.h"
#include "voices.h"
#include "ai.h"
#include "keys.h"

command_handler_type red_commands, blue_commands;

extern player_events_type player_events[6]; /* From ai.c */
extern voice_handler_type voice_handlers[6]; /* From voices.c */
extern game_configuration_type game_configuration; /* From omega.c */
extern SYS_SOUNDHANDLE computer_sample[MAX_COMPUTER_SAMPLES]; /* From omega.c */



void Handle_Commands( WorldStuff *world_stuff, int leader )
    {
     short current_team;
     command_handler_type *command_handler;
     unsigned char *table;
     int i;
     int current_wingman = 0;
     boolean new_command[2]; /* Is there a new command in effect now? */


     new_command[0] = FALSE; /* Set both wingmans flags to false */
     new_command[1] = FALSE;

     current_team = world_stuff->player_array[leader].team;     
     table = world_stuff->player_array[leader].table;

     if( current_team == RED_TEAM )
         command_handler = &red_commands;
     else
         command_handler = &blue_commands;


     if( Check_Input_Table( INDEX_L_COMMAND_ATTACK, table ) )
         {
          if( command_handler->frames_left[0] <= 0 )
              {
               command_handler->frames_left[0] = 50;
               command_handler->command_active[0] = TRUE;
               command_handler->command[0] = ATTACK_COMMAND;
               new_command[0] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_L_COMMAND_PYLON, table ) )
         {
          if( command_handler->frames_left[0] <= 0 )
              {
               command_handler->frames_left[0] = 50;
               command_handler->command_active[0] = TRUE;
               command_handler->command[0] = GET_PYLONS_COMMAND;
               new_command[0] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_L_COMMAND_ATTACK_RADAR, table ) )
         {
          if( command_handler->frames_left[0] <= 0 )
              {
               command_handler->frames_left[0] = 50;
               command_handler->command_active[0] = TRUE;
               command_handler->command[0] = ATTACK_RADAR_COMMAND;
               new_command[0] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_L_COMMAND_DEFEND_RADAR, table ) )
         {
          if( command_handler->frames_left[0] <= 0 )
              {
               command_handler->frames_left[0] = 50;
               command_handler->command_active[0] = TRUE;
               command_handler->command[0] = DEFEND_RADAR_COMMAND;
               new_command[0] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_L_COMMAND_GROUP, table ) )
         {
          if( command_handler->frames_left[0] <= 0 )
              {
               command_handler->frames_left[0] = 50;
               command_handler->command_active[0] = TRUE;
               command_handler->command[0] = GROUP_COMMAND;
               new_command[0] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_L_COMMAND_CANCEL, table ) )
         {
          if( command_handler->frames_left[0] <= 0 )
              {
               command_handler->frames_left[0] = 50;
               command_handler->command_active[0] = FALSE;
               command_handler->command[0] = CANCEL_COMMANDS_COMMAND;
               new_command[0] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_R_COMMAND_ATTACK, table ) )
         {
          if( command_handler->frames_left[1] <= 0 )
              {
               command_handler->frames_left[1] = 50;
               command_handler->command_active[1] = TRUE;
               command_handler->command[1] = ATTACK_COMMAND;
               new_command[1] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_R_COMMAND_PYLON, table ) )
         {
          if( command_handler->frames_left[1] <= 0 )
              {
               command_handler->frames_left[1] = 50;
               command_handler->command_active[1] = TRUE;
               command_handler->command[1] = GET_PYLONS_COMMAND;
               new_command[1] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_R_COMMAND_ATTACK_RADAR, table ) )
         {
          if( command_handler->frames_left[1] <= 0 )
              {
               command_handler->frames_left[1] = 50;
               command_handler->command_active[1] = TRUE;
               command_handler->command[1] = ATTACK_RADAR_COMMAND;
               new_command[1] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_R_COMMAND_DEFEND_RADAR, table ) )
         {
          if( command_handler->frames_left[1] <= 0 )
              {
               command_handler->frames_left[1] = 50;
               command_handler->command_active[1] = TRUE;
               command_handler->command[1] = DEFEND_RADAR_COMMAND;
               new_command[1] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_R_COMMAND_GROUP, table ) )
         {
          if( command_handler->frames_left[1] <= 0 )
              {
               command_handler->frames_left[1] = 50;
               command_handler->command_active[1] = TRUE;
               command_handler->command[1] = GROUP_COMMAND;
               new_command[1] = TRUE;
              }
         } 
     else if( Check_Input_Table( INDEX_R_COMMAND_CANCEL, table ) )
         {
          if( command_handler->frames_left[1] <= 0 )
              {
               command_handler->frames_left[1] = 50;
               command_handler->command_active[1] = FALSE;
               command_handler->command[1] = CANCEL_COMMANDS_COMMAND;
               new_command[1] = TRUE;
              } 
         }


     for( i = 0; i < 6; i++ )
          if( world_stuff->player_array[i].leader == FALSE &&
              world_stuff->player_array[i].tank.team == current_team && 
              world_stuff->player_array[i].tank.alive )
              {
               if( current_wingman < 2 )
                   {
                    
                    if( new_command[current_wingman] ) /* player_events[i].memory.command != command_handler->command[current_wingman] ) */
                        {
                    
                         if( game_configuration.sound_on )
                             {
                              switch( command_handler->command[current_wingman] )
                                  {
                                   case ATTACK_COMMAND:
                                   SYS_TriggerSound( computer_sample[ATTACK_ENEMY] );
                                   break;
                                   case GET_PYLONS_COMMAND:
                                   SYS_TriggerSound( computer_sample[GRAB_PYLONS] );
                                   break;
                                   case ATTACK_RADAR_COMMAND:
                                   SYS_TriggerSound( computer_sample[ATTACK_RADAR] );
                                   break;
                                   case DEFEND_RADAR_COMMAND:
                                   SYS_TriggerSound( computer_sample[DEFEND_RADAR] );
                                   break;
                                   case GROUP_COMMAND:
                                   SYS_TriggerSound( computer_sample[GROUP_TOGETHER] );
                                   break;
                                   case CANCEL_COMMANDS_COMMAND:
                                   SYS_TriggerSound( computer_sample[CANCEL_ORDER] );
                                   break;
                                   default:
                                   break;
                                  } /* End case */
                             } /* End if sound on */
                         voice_handlers[i].affirmation_told = FALSE;
                         voice_handlers[i].negation_told = FALSE;
                      } 
                   
                    switch( command_handler->command[current_wingman] )
                        {
                         case ATTACK_COMMAND:
                         player_events[i].memory.command_state = ATTACK;
                         break;
                         case GET_PYLONS_COMMAND:
                         player_events[i].memory.command_state = GET_PYLONS;
                         break;
                         case ATTACK_RADAR_COMMAND:
                         player_events[i].memory.command_state = KILL_RADAR_BASE;
                         break;
                         case DEFEND_RADAR_COMMAND:
                         player_events[i].memory.command_state = PROTECT_RADAR_BASE;
                         break;
                         case GROUP_COMMAND:
                         player_events[i].memory.command_state = GROUP;
                         break;
                         case CANCEL_COMMANDS_COMMAND:
                         /* So they'll be the same */
                         player_events[i].memory.command_state = world_stuff->player_array[i].character.state;
                         break;
                         default:
                         player_events[i].memory.command_state = world_stuff->player_array[i].character.state;
                         break;
                        } /* End case */
                    player_events[i].memory.command_active = command_handler->command_active[current_wingman];
                    player_events[i].memory.command        = command_handler->command[current_wingman];
                    player_events[i].memory.command_frames_left = command_handler->frames_left[current_wingman];
                    current_wingman++;
                   
                   } /* End if current_wingman < 2 */
              } /* End if this is the right tank (for loop above it) */

     /* Decrement current number of frames */
     if( command_handler->frames_left[0] > 0 )
         command_handler->frames_left[0]--;
     if( command_handler->frames_left[1] > 0 )
         command_handler->frames_left[1]--;


    } /* End of Handle_Commands */


/* Utility function for debugging */
void Sprint_Command( char *command_string, command_type command )
    {
     switch( command )
         {
          case ATTACK_COMMAND :
              sprintf( command_string, "ATTACK" );
              break;
          case GET_PYLONS_COMMAND :
              sprintf( command_string, "GET PYLONS" );
              break;
          case ATTACK_RADAR_COMMAND :
              sprintf( command_string, "ATTACK RADAR" );
              break;
          case DEFEND_RADAR_COMMAND :
              sprintf( command_string, "DEFEND RADAR" );
              break;
          case GROUP_COMMAND :
              sprintf( command_string, "GROUP" );
              break;
          case CANCEL_COMMANDS_COMMAND :
              sprintf( command_string, "CANCEL");
              break;
         
          default :
              sprintf( command_string, "NONE ");
              break;

         } /* End switch */

    } /* End Sprint_Command  */


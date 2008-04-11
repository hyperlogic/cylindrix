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

#include "voices.h"
#include "ai.h"
#include "jonsb.h"

boolean winner_told   = FALSE; /* These keep track of the computer voice */
boolean transfer_told = FALSE; 
boolean radar_told    = FALSE;

extern game_configuration_type game_configuration; /* From omega.c */
extern SYS_SOUNDHANDLE computer_sample[MAX_COMPUTER_SAMPLES];  
extern player_events_type player_events[6];


voice_handler_type voice_handlers[6];


void Init_Voices( void )
    {
     int i;

     for( i = 0; i < 6; i++ )
         {
          voice_handlers[i].greeting_told = FALSE;
          voice_handlers[i].affirmation_told = FALSE;
          voice_handlers[i].negation_told = FALSE;
          voice_handlers[i].gloat_told = FALSE;
          voice_handlers[i].despair_told = FALSE;
          voice_handlers[i].death_told = FALSE;
          voice_handlers[i].victory_told = FALSE;
          voice_handlers[i].control_lost_told = FALSE;
          voice_handlers[i].control_regained_told = TRUE;

         }
    
     winner_told   = FALSE;
     transfer_told = FALSE; 
     radar_told    = FALSE;

    } /* End of Init_Voices */


/* Handle the gloat and despair voices */
void Gloat_Despair( WorldStuff *world_stuff, long view_vehicle )
    {
     int i;
     Player *player_array;

     player_array = world_stuff->player_array;

     for( i = 0; i < 6; i++ )
         {
          if( player_array[i].tank.alive && i != view_vehicle &&
              player_array[i].team == player_array[view_vehicle].team )
              {
               if( player_events[i].friends_alive < player_events[i].enemies_alive )
                   {
                    if( !voice_handlers[i].despair_told )
                        {
                         voice_handlers[i].despair_told = TRUE;
                         if( game_configuration.sound_on )
                             SYS_TriggerSound( world_stuff->player_array[i].character.samples[DESPAIR] );
                        }
                   }
               else if( player_events[i].friends_alive > player_events[i].enemies_alive )
                   {     
                    if( !voice_handlers[i].gloat_told )
                        {
                         voice_handlers[i].gloat_told = TRUE;
                         if( game_configuration.sound_on )
                             SYS_TriggerSound( world_stuff->player_array[i].character.samples[GLOAT] );                        
                        }
                   }
              }

         } /* End for */



    } /* End of Gloat_Despair */

void Handle_Voices( WorldStuff *world_stuff, long view_vehicle, unsigned char first_person_view, unsigned char teleporting )
    {
     int i;
     team_type current_team;


     current_team = world_stuff->player_array[view_vehicle].team;


     /* Check to see if there is a winner...if there is, tell the player */
     /*
     if( world_stuff->player_array[0].tank.alive == FALSE &&
         world_stuff->player_array[1].tank.alive == FALSE &&
         world_stuff->player_array[2].tank.alive == FALSE &&
         winner_told == FALSE )
             {
              winner_told = TRUE;
              if( game_configuration.sound_on )
                  {
                   Mix_Voice_Sample( computer_sample[BLUE_TEAM_WINS] );
                  }
             }
     if( world_stuff->player_array[3].tank.alive == FALSE &&
         world_stuff->player_array[4].tank.alive == FALSE &&
         world_stuff->player_array[5].tank.alive == FALSE &&
         winner_told == FALSE )
             {
              winner_told = TRUE;
              if( game_configuration.sound_on )
                  {
                   Mix_Voice_Sample( computer_sample[RED_TEAM_WINS] );
                  }
             }
     */

     /* If the radar base has been destroyed, tell the user */
     if( world_stuff->player_array[view_vehicle].tank.team == RED_TEAM )
         {
          if( world_stuff->red_radar_base.alive == FALSE &&
          radar_told == FALSE )
              {
               radar_told = TRUE;
               if( game_configuration.sound_on )
                   Mix_Voice_Sample( computer_sample[RADAR_DESTROYED] );
              }
         }
     else
         {
          if( world_stuff->blue_radar_base.alive == FALSE &&
          radar_told == FALSE )
              {
               radar_told = TRUE;
               if( game_configuration.sound_on )
                   Mix_Voice_Sample( computer_sample[RADAR_DESTROYED] );
              }
         }

        if( first_person_view && !teleporting ) {
            transfer_told = FALSE; /* John's goofy thing */
        }
        else if( !first_person_view && !teleporting ){

            transfer_told = FALSE; /* John's goofy thing */

        }
        else if( teleporting ) {

             if( world_stuff->player_array[view_vehicle].team == RED_TEAM )
                 i = 0;
             else
                 i = 3;

             /* If all vehicles are dead, dont say 'transfer' thing */
             if( world_stuff->player_array[i].tank.alive == FALSE &&
             world_stuff->player_array[i + 1].tank.alive == FALSE &&
             world_stuff->player_array[i + 2].tank.alive == FALSE )
                 transfer_told = TRUE;

        if( !transfer_told )
            {
             transfer_told = TRUE;
             if( game_configuration.sound_on )
                 {
                  Mix_Voice_Sample( computer_sample[TRANSFER] );
                 }
            }
        }

     voice_handlers[view_vehicle].greeting_told = TRUE;
     voice_handlers[view_vehicle].affirmation_told = TRUE;
     voice_handlers[view_vehicle].negation_told = TRUE;
     voice_handlers[view_vehicle].gloat_told = TRUE;
     voice_handlers[view_vehicle].despair_told = TRUE;
     voice_handlers[view_vehicle].death_told = TRUE;
     voice_handlers[view_vehicle].victory_told = TRUE;

     for( i = 0; i < 6; i++ )
         {
          if( world_stuff->player_array[i].controller == REMOTE_CONTROL )
              {
               voice_handlers[i].greeting_told = TRUE;
               voice_handlers[i].affirmation_told = TRUE;
               voice_handlers[i].negation_told = TRUE;
               voice_handlers[i].gloat_told = TRUE;
               voice_handlers[i].despair_told = TRUE;
               voice_handlers[i].death_told = TRUE;
               voice_handlers[i].victory_told = TRUE;
              }
         }

     for( i = 0; i < 6; i++ )
         { 
              
          if( !world_stuff->player_array[i].tank.alive &&
               world_stuff->player_array[i].tank.team == current_team )
              {
               if( i != view_vehicle )
                   {
                    if( !voice_handlers[i].death_told )
                        {
                         voice_handlers[i].death_told = TRUE;
                         Mix_Voice_Sample( world_stuff->player_array[i].character.samples[DEATH] );
                        } /* End if */
                   } /* End if */
              } /* End if */
         
          if( player_events[i].killed_someone && world_stuff->player_array[i].tank.alive &&
               world_stuff->player_array[i].tank.team == current_team ) 
              {
               if( i != view_vehicle ) 
                   Mix_Voice_Sample( world_stuff->player_array[i].character.samples[VICTORY] );

               player_events[i].killed_someone = FALSE;
              }


         if( world_stuff->player_array[i].character.state !=
             player_events[i].memory.command_state && 
             player_events[i].memory.command_active &&
             !voice_handlers[i].negation_told &&
             !voice_handlers[i].affirmation_told )
             {
              voice_handlers[i].negation_told = TRUE; 
              voice_handlers[i].affirmation_told = TRUE; 
              Mix_Voice_Sample( world_stuff->player_array[i].character.samples[NEGATION] );
             }
         else if( world_stuff->player_array[i].character.state ==
             player_events[i].memory.command_state && 
             player_events[i].memory.command_active &&
             !voice_handlers[i].negation_told &&
             !voice_handlers[i].affirmation_told )
             {
              voice_handlers[i].negation_told = TRUE; 
              voice_handlers[i].affirmation_told = TRUE; 
              Mix_Voice_Sample( world_stuff->player_array[i].character.samples[AFFIRMATION] );
             }

         
         } /* End for */

      
     Gloat_Despair( world_stuff, view_vehicle );


     if( world_stuff->player_array[view_vehicle].tank.controls_scrambled ||
         world_stuff->player_array[view_vehicle].tank.traitor_active )
         {
          if( !voice_handlers[view_vehicle].control_lost_told )
              {
               voice_handlers[view_vehicle].control_lost_told = TRUE;
               voice_handlers[view_vehicle].control_regained_told = FALSE;
               Mix_Voice_Sample( computer_sample[CONTROLS_LOST] );
              }                                 
         }
     else
         {
          if( !voice_handlers[view_vehicle].control_regained_told )
              {
               voice_handlers[view_vehicle].control_lost_told = FALSE;
               voice_handlers[view_vehicle].control_regained_told = TRUE;
               Mix_Voice_Sample( computer_sample[CONTROLS_REGAINED] );
              }
         }


    } /* End of Handle_Voices */


void Zero_Voice( int wingman_number )
    {
     voice_handlers[wingman_number].greeting_told = TRUE;
     voice_handlers[wingman_number].affirmation_told = TRUE;
     voice_handlers[wingman_number].negation_told = TRUE;
     voice_handlers[wingman_number].gloat_told = TRUE;
     voice_handlers[wingman_number].despair_told = TRUE;
     voice_handlers[wingman_number].death_told = TRUE;
     voice_handlers[wingman_number].victory_told = TRUE;
    } /* End of Zero_Voice() */

void No_Despair( int wingman_number )
    {
     voice_handlers[wingman_number].despair_told = TRUE;
    } /* End of No_Despair() */


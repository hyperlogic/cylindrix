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

#include "states.h"                  

extern player_events_type player_events[6]; /* From ai.c */


/* 
   OK this is an important function...here are the things we must
   keep in mind

   Our d vs enemy d
   our hitpoints
   number of enemies alive vs number of friends alive
   if our radar base is alive 
   if his radar base is alive 
   our distance from the energy square 

*/

state_type Find_State( WorldStuff *world_stuff, int index )
    {
     int state_index; 
     int state_weight[NUM_STATES]; /* Likelihood of each state occurring */
     int current_highest = 0; /* This is the current highest likelihood that a state will occurr */
     int i;
     character_type *character;  /* Alias to this character */
     player_events_type *events; /* Alias to this characters events */
     /* Alias's to this character's personality 
        so we don't have to do a million dereferences 
        negative is the first word, positive is the second */
     int passive_aggressive, bravery_cowardice,aerial_ground, 
         obedience_disobedience, pylon_grab, skill_level, 
         radar_kill, radar_protect;
     int surface, air; /* This is a separate thing from the general states */
     int modifier = 6; /* Arbitrary modifier number for now */
     int current_enemy;
     boolean ok_to_mode_switch = TRUE;
     boolean temp_boolean;


     character     = &world_stuff->player_array[index].character;
     events        = &player_events[index];
     current_enemy = events->memory.current_enemy;


     /* Make the aliases to this players personality */
     passive_aggressive     = 50 - (100 - character->passive_aggressive);
     bravery_cowardice      = 50 - (100 - character->bravery_cowardice);
     aerial_ground          = 50 - (100 - character->aerial_ground);
     obedience_disobedience = 50 - (100 - character->obedience_disobedience);
     pylon_grab             = 50 - (100 - character->pylon_grab);
     radar_kill             = 50 - (100 - character->radar_kill);
     radar_protect          = 50 - (100 - character->radar_protect); 
     skill_level            = character->skill_level;
     /* End of alias making */

     surface = air = 0;
     
     /* If all else fails we will stay in the same state */
     state_index = world_stuff->player_array[index].character.state;

     /* Zero out the likelihoods */
     for( i = 0; i < NUM_STATES; i++ )
         state_weight[i] = 0;
     
     /* Compute the different state weights */

     /* First we check to see if any commands are in effect...
        if they are, we add to that states weight */


     /* Process commands here */

     if( player_events[index].memory.command_active )
     {
     switch( player_events[index].memory.command )
         {
          case ATTACK_COMMAND:
          state_weight[ATTACK] += (modifier * 10) + obedience_disobedience;
          break;
          
          case ATTACK_RADAR_COMMAND:
          state_weight[KILL_RADAR_BASE] += (modifier * 10) + obedience_disobedience;
          break;
          
          case DEFEND_RADAR_COMMAND:
          state_weight[PROTECT_RADAR_BASE] += (modifier * 10) + obedience_disobedience;
          break;

          case GET_PYLONS_COMMAND:
          state_weight[GET_PYLONS] += (modifier * 10) + obedience_disobedience;
          break;
          
          case GROUP_COMMAND:
          state_weight[GROUPUP] += (modifier * 20) + obedience_disobedience;
          break;
          
          default:
          break;
         } /* End case */
     } /* End if */
     
     /* We add to our current state weight to avoid rapid flipping between
        two states */
     state_weight[state_index] += modifier;   
     
     
     if( events->my_D < events->enemy_D )
         {
          state_weight[GET_PYLONS]      +=  (modifier * (events->enemy_D - events->my_D)) + (pylon_grab * 2);
          state_weight[ATTACK]          -=  (modifier * (events->enemy_D - events->my_D)) - (passive_aggressive * 2);
          state_weight[KILL_RADAR_BASE] -=  (modifier * (events->enemy_D - events->my_D)) - (radar_kill * 2);
         } /* End if my d is less */
     else if( events->my_D > events->enemy_D )
         {
          state_weight[GET_PYLONS]      -=  (modifier * (events->my_D - events->enemy_D)) - (pylon_grab * 2);
          state_weight[ATTACK]          +=  (modifier * (events->my_D - events->enemy_D)) + (passive_aggressive * 2);
          state_weight[KILL_RADAR_BASE] +=  (modifier * (events->my_D - events->enemy_D)) + (radar_kill * 2);
         }
     
     if( events->friends_alive < events->enemies_alive )
         {
          state_weight[ATTACK]          -=  (modifier * ( (events->enemies_alive - events->friends_alive) *2) ) - (passive_aggressive * 2);
          state_weight[KILL_RADAR_BASE] +=  (modifier * ( (events->enemies_alive - events->friends_alive) *2) ) + (radar_kill * 2);
         }
     else if( events->friends_alive > events->enemies_alive )
         {
          state_weight[GET_PYLONS]      -=  (modifier * ( (events->friends_alive - events->enemies_alive) *2) ) - (pylon_grab * 2);
          state_weight[ATTACK]          +=  (modifier * 6) + (passive_aggressive * 2);
          if( events->hitpoints_high )
              state_weight[ATTACK] +=  ((modifier * 2) + (passive_aggressive * 2)); 
         }
     
     if( skill_level >= 3 && events->radar_hitpoints_low )
         {
          state_weight[PROTECT_RADAR_BASE] += modifier + (radar_protect * 3);
         }
     
     if( events->enemy_radar_alive && !events->my_radar_alive )
         {
          state_weight[ATTACK]          -=  ((modifier * 2) - (passive_aggressive * 2)); 
          state_weight[KILL_RADAR_BASE] +=  ((modifier * 2) + (radar_kill * 2)); 
          if( events->hitpoints_high )
              state_weight[KILL_RADAR_BASE] +=  ((modifier * 2) + (radar_kill * 2)); 
          state_weight[PROTECT_RADAR_BASE] -= 50000;
         }
     else if( !events->enemy_radar_alive && events->my_radar_alive )
         {
          state_weight[ATTACK]          +=  ((modifier * 2) + (passive_aggressive * 2)); 
          state_weight[KILL_RADAR_BASE] -=  50000; /* If his base is dead...don't attack it!! */
         }
     else if( !events->enemy_radar_alive && !events->my_radar_alive )
         {
          state_weight[KILL_RADAR_BASE] -=  50000; /* If his base is dead...don't attack it!! */
          state_weight[PROTECT_RADAR_BASE] -= 50000;
         } 
     
     
     
     if( world_stuff->player_array[index].tank.current_hitpoints == world_stuff->player_array[index].tank.max_hitpoints ) 
         {
          state_weight[GET_ENERGY] -= 300; /* We should pretty much always do this */
         }
     else
         {
          state_weight[GET_ENERGY] += (int)((world_stuff->player_array[index].tank.max_hitpoints - world_stuff->player_array[index].tank.current_hitpoints) * 1.5f);
         }
     
     
     if( events->hitpoints_high )
         {
          state_weight[ATTACK]           +=  ((modifier * 2) + (passive_aggressive * 2)); 
          state_weight[KILL_RADAR_BASE]  +=  ((modifier * 2) + (radar_kill * 2)); 
         }


     if( events->my_D == 0 )
         {
          state_weight[ATTACK] -= 10000;
          state_weight[KILL_RADAR_BASE] -= 10000;
         }
     if( events->my_D < 2 )
         {
          state_weight[ATTACK] -= modifier * 5;
          state_weight[KILL_RADAR_BASE] -= modifier * 10;
         }
     if( events->my_D > 3 )
         {
          state_weight[ATTACK] += modifier * events->my_D;
          state_weight[KILL_RADAR_BASE] += modifier * events->my_D;
         }
         
     if( events->my_D == 10 )
         state_weight[GET_PYLONS] -= 50000; /* If 'd' is 10...never get pylons */
     
     state_weight[GET_PYLONS] += 5 * (10 - events->my_D);
     state_weight[ATTACK]     += modifier + (passive_aggressive * 3);
     state_weight[KILL_RADAR_BASE] += modifier + (radar_kill * 3);
     state_weight[PROTECT_RADAR_BASE] += modifier + (radar_protect * 3);
     
     /* If we are not currently after anyone...let attack be zero..
        change this later */
     if( events->memory.current_enemy == 255 )
          state_weight[ATTACK] = 0;
     
     
     if( state_index == GET_ENERGY )
         {
          if( events->memory.recent_damage < 5 && events->angles.energy_dist_2D )
              state_weight[GET_ENERGY] += 100;
         }
         
     
     state_weight[EVADE]  += (events->memory.recent_damage * 5) - passive_aggressive;
     
     if( events->memory.recent_damage > 20 )
         {
          state_weight[EVADE] += modifier * 2;
          state_weight[ATTACK] += modifier;
          state_weight[GET_ENERGY] -= modifier * 5;
         }
     if( events->memory.recent_damage > 30 )     
         {
          state_weight[EVADE] += modifier * 3;
          state_weight[ATTACK] += modifier;
          state_weight[GET_ENERGY] -= modifier * 5;
         }
     
     /* Do a bubble sort to figure out the most desired state */
     for( i = 0; i < NUM_STATES; i++ )
         {
          if( current_highest < state_weight[i] )
              {
               current_highest = state_weight[i];
               state_index = i;
              }
         }


     if( world_stuff->player_array[index].tank.traitor_active )
         {    
          state_index = ATTACK;
         }

     
     /* Now we figure out if the AI wants to be on the surface or in the 
        air to do whatever it is doing */
     
     /* Depending on what state we are in now...factor in different things */

     switch( state_index )
         {
          case ATTACK :
              if( current_enemy < 6 ) {
                  if( world_stuff->player_array[events->memory.current_enemy].tank.vehicle_mode == Surface )
                      {
                       surface += modifier * 2 + (aerial_ground * 2);
                       air     -= modifier * 2 + (aerial_ground * 2);
                      }                                          
                  else if( world_stuff->player_array[events->memory.current_enemy].tank.vehicle_mode == Air )
                      {
                       surface -= modifier * 3 - (aerial_ground * 2);
                       air     += modifier * 3 - (aerial_ground * 2);
                      }                                          
	      }
              else
                      {
                       surface += aerial_ground * 2;       
                       air     -= aerial_ground * 2;
                      }
              break;
          case EVADE :
                  if( current_enemy < 6 ) {
                      if( world_stuff->player_array[events->memory.current_enemy].tank.vehicle_mode == Surface )
                          {
                           surface -= modifier * 3 - (aerial_ground * 2);
                           air     += modifier * 3 - (aerial_ground * 2);
                          }                                          
                      else if( world_stuff->player_array[events->memory.current_enemy].tank.vehicle_mode == Air )
                          {
                           surface += modifier + (aerial_ground * 2);
                           air     -= modifier + (aerial_ground * 2);
                          }                                          
		  }
                  else /* Enemy is a radar base */
                      {
                       if( world_stuff->player_array[index].tank.vehicle_mode == Surface )
                           {
                            surface += 10000;
                            air     -= 10000;
                           }
                       else
                           {
                            surface -= 10000;
                            air     += 10000;
                           }                 
                      }
              break;
          case GET_PYLONS :
              surface += aerial_ground * 2;       
              air     -= aerial_ground * 2;
              break;
          case GET_ENERGY:
              if( events->angles.energy_dist_2D < 0.3 )
                  {
                   surface -= 10000;
                   air     += 10000;
                  }
              else if( events->angles.energy_dist_2D < 6 )
                  {
                   surface += 10000;
                   air     -= 10000;
                  }
              else
                  {
                   surface -= 10000;
                   air     += 10000;
                  }
              if( !events->memory.surface )
                  {
                   if( events->angles.energy_dist_2D < 0.5 )
                       {
                        surface -= 10000;
                        air     += 10000;
                       }
                  }
              break;
          case KILL_RADAR_BASE: /* Kill the radar base on da ground dammit */
              surface += aerial_ground * 2;       
              air     -= aerial_ground * 2;
              break;      
          case GROUPUP:
              if( world_stuff->player_array[player_events[index].my_leader].tank.vehicle_mode == Surface )
                  {
                   surface += 10000;       
                   air     -= 10000;
                  } 
              else
                  {
                   surface -= 10000;       
                   air     += 10000;
                  }
              break;
          default:
              surface += aerial_ground * 2;      
              air     -= aerial_ground * 2;
              break;
              
         } /* End case */
         
     temp_boolean = player_events[index].memory.in_air_array[0];
     for( i = 0; i < 30; i++ )
         {
          if( player_events[index].memory.in_air_array[i] != temp_boolean )
              ok_to_mode_switch = FALSE;
         }

     if( ok_to_mode_switch ) {
         if( surface > air )
             {
              /* 2d and 3d paths are different! */
              if( !events->memory.surface ) 
                  events->memory.following_path = 0;
         
              events->memory.surface = 1;
             }
         else                               
             {
              /* 2d and 3d paths are different! */
              if( events->memory.surface )
                   events->memory.following_path = 0;
          
              events->memory.surface = 0;
             }
     }
     
     
     if( state_index != world_stuff->player_array[index].character.state )
         {
          events->memory.following_path = 0;
          events->memory.path_index_2D  = 0;
          events->memory.num_paths_2D   = 0;
         }
     
     return( (state_type)state_index );
     

    } /* End of Find_State */

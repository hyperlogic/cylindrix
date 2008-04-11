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

 #include "events.h" 

extern player_events_type player_events[6]; /* From ai.c */




/*

    short controls_scrambled;
    short frames_till_unscramble;
    short scramble_life;

    short traitor_life;
    short traitor_active;
    short frames_till_traitor_deactivate;

    short cloaking_active;
    short cloak_reload_time;
    short frames_till_cloak;
    short cloak_time;
    short frames_till_cloak_suck;

    short decoy_life;
    short decoy_reload_time;

*/




                    
/* Fill up the angles section of player_events */
void Update_Angles( WorldStuff *world_stuff, int index )
    {
     int i;
     Float_Point2d temp_point_one;
     Projectile *bullet; /* For the figuring of bullet angles */
     Orientation o; /* Temporary variable for bullet stuff */
     RadarBase *radar_base;
     Player *player;
     float temp_dist;

     /* Store the number of pylons */
     player_events[index].angles.num_pylons = world_stuff->pylons.pylons;

     /* Now we figure out everything in 3d and 2d no matter what */


     /* Figure out the distance, position, and 
        angle of the energy square in 3d */
     Copy_Point_3D( world_stuff->energy_square.center, player_events[index].angles.energy_pos_3D ); 
     player_events[index].angles.energy_dist_3D = Point_Distance_3D( world_stuff->player_array[index].tank.orient.position, player_events[index].angles.energy_pos_3D ); 
     player_events[index].angles.energy_angle_3D.horizontal_angle = Horizontal_Point_Angle_3D( player_events[index].angles.energy_pos_3D, &world_stuff->player_array[index].tank.orient );
     player_events[index].angles.energy_angle_3D.vertical_angle   = Vertical_Point_Angle_3D( player_events[index].angles.energy_pos_3D, &world_stuff->player_array[index].tank.orient );
          
          
     /* Do the same in 2d */
     /* Convert the position of the vehicle to 2d */
     Cylinder_x_y( world_stuff->player_array[index].tank.orient.position, temp_point_one ); 
     /* Convert the position of the energy square to 2d and store it */
     Cylinder_x_y( world_stuff->energy_square.center, player_events[index].angles.energy_pos_2D );
     /* Compute the 2d angle and distance from the energy square */
     player_events[index].angles.energy_dist_2D = Point_Distance_2D( temp_point_one, player_events[index].angles.energy_pos_2D );
     player_events[index].angles.energy_angle_2D = Point_Angle_2D( player_events[index].angles.energy_pos_2D, &world_stuff->player_array[index].tank.orient );
                                                        


     /* Figure out the distance, position, and 
     angle of the enemy radar base in 3d */
     if( world_stuff->player_array[index].team == RED_TEAM )
         radar_base = &world_stuff->blue_radar_base;
     else
         radar_base = &world_stuff->red_radar_base;
          
     Copy_Point_3D( radar_base->turret_orient.position, player_events[index].angles.enemy_radar_pos_3D ); 
     player_events[index].angles.enemy_radar_dist_3D                    = Point_Distance_3D( world_stuff->player_array[index].tank.orient.position, player_events[index].angles.enemy_radar_pos_3D ); 
     player_events[index].angles.enemy_radar_angle_3D.horizontal_angle  = Horizontal_Point_Angle_3D( player_events[index].angles.enemy_radar_pos_3D, &world_stuff->player_array[index].tank.orient );
     player_events[index].angles.enemy_radar_angle_3D.vertical_angle    = Vertical_Point_Angle_3D( player_events[index].angles.enemy_radar_pos_3D, &world_stuff->player_array[index].tank.orient );
     

     /* Figure out 2D enemy radar position, angle and distance */                                                        
     if( world_stuff->player_array[index].team == RED_TEAM )
         radar_base = &world_stuff->blue_radar_base;
     else
         radar_base = &world_stuff->red_radar_base;                                                        
          
     /* Convert the position of the radar to 2d and store it */                                                        
     Radar_Leg( &radar_base->base_orient, player_events[index].angles.enemy_radar_pos_2D );
          
     /* Compute the 2d angle and distance from the enemy radar base */
     player_events[index].angles.enemy_radar_dist_2D  = Point_Distance_2D( temp_point_one, player_events[index].angles.enemy_radar_pos_2D );
     player_events[index].angles.enemy_radar_angle_2D = Point_Angle_2D( player_events[index].angles.enemy_radar_pos_2D, &world_stuff->player_array[index].tank.orient );
          


     /* Loop through the pylons and figure out their angles and
        distances and positions in 3d */
     for( i = 0; i < world_stuff->pylons.pylons; i++ )
         {
          Pylon_Position_3D( &world_stuff->pylons.pylon[i], player_events[index].angles.pylon_pos_3D[i] );
          player_events[index].angles.pylon_dist_3D[i] = Point_Distance_3D( world_stuff->player_array[index].tank.orient.position, player_events[index].angles.pylon_pos_3D[i] ); 
          player_events[index].angles.pylon_angle_3D[i].horizontal_angle = Horizontal_Point_Angle_3D( player_events[index].angles.pylon_pos_3D[i], &world_stuff->player_array[index].tank.orient );
          player_events[index].angles.pylon_angle_3D[i].vertical_angle   = Vertical_Point_Angle_3D( player_events[index].angles.pylon_pos_3D[i], &world_stuff->player_array[index].tank.orient );
         }
         
                                                        
                                                        
     /* Loop through the pylons and figure out their angles and
        distances and positions in 2d */
     for( i = 0; i < world_stuff->pylons.pylons; i++ )
         {
          /* Convert the position of the pylon to 2d and store it */
          Pylon_Position_2D( &world_stuff->pylons.pylon[i], player_events[index].angles.pylon_pos_2D[i] );
               
          /* Compute the 2d angle and distance from the pylon */
          player_events[index].angles.pylon_dist_2D[i] = Point_Distance_2D( player_events[index].angles.pylon_pos_2D[i], temp_point_one ); 
          player_events[index].angles.pylon_angle_2D[i] = Point_Angle_2D( player_events[index].angles.pylon_pos_2D[i], &world_stuff->player_array[index].tank.orient );
         }


     /* Loop through the vehicles and figure out their angles and
        distances and positions in 3d and in 2d*/
     for( i = 0; i < 6; i++ )
         {
          /* Compute the 2d position of the other tank and store it */
          Cylinder_x_y( world_stuff->player_array[i].tank.orient.position, player_events[index].angles.tank_pos_2D[i] ); 
          /* Compute the 2d angle and distance from the tank */
          player_events[index].angles.tank_dist_2D[i] = (short)Point_Distance_2D( temp_point_one, player_events[index].angles.tank_pos_2D[i] );
          player_events[index].angles.tank_angle_2D[i] = Point_Angle_2D( player_events[index].angles.tank_pos_2D[i], &world_stuff->player_array[index].tank.orient );
     
          Copy_Point_3D( world_stuff->player_array[i].tank.orient.position, player_events[index].angles.tank_pos_3D[i] ); 
          player_events[index].angles.tank_dist_3D[i] = Point_Distance_3D( world_stuff->player_array[index].tank.orient.position, world_stuff->player_array[i].tank.orient.position );
          player_events[index].angles.tank_angle_3D[i].horizontal_angle = Horizontal_Point_Angle_3D( world_stuff->player_array[i].tank.orient.position, &world_stuff->player_array[index].tank.orient );
          player_events[index].angles.tank_angle_3D[i].vertical_angle   = Vertical_Point_Angle_3D( world_stuff->player_array[i].tank.orient.position, &world_stuff->player_array[index].tank.orient );
         }
     
     /* Loop through the pylons and update the owner variables */
     for( i = 0; i < world_stuff->pylons.pylons; i++ )
              {
               player_events[index].angles.pylon_owner[i] = world_stuff->pylons.pylon[i].owner;
              }
         



    /* OK now we loop through all of the enemy vehicles...if there are
       decoy vehicles...we will see if they are closer and fire at them
       instead of the actual vehicle */

    
    for( i = 0; i < 6; i++ )
        {
         player = &world_stuff->player_array[i];

         if( player->team != world_stuff->player_array[index].team )
             {
              bullet = player->tank.projectile_list;

              while( bullet != NULL )
                  {
                   if( bullet->type == DECOY_0 || bullet->type == DECOY_1 ||
                       bullet->type == DECOY_2 )
                       {
                        /* Convert the orientation from magic */
                        o.position[X] = mtof( bullet->orient.position[X] );
                        o.position[Y] = mtof( bullet->orient.position[Y] );
                        o.position[Z] = mtof( bullet->orient.position[Z] );
                        o.front[X]    = mtof( bullet->orient.front[X] );
                        o.front[Y]    = mtof( bullet->orient.front[Y] );
                        o.front[Z]    = mtof( bullet->orient.front[Z] );
                        o.up[X]       = mtof( bullet->orient.up[X] );
                        o.up[Y]       = mtof( bullet->orient.up[Y] );
                        o.up[Z]       = mtof( bullet->orient.up[Z] );

                        temp_dist = Point_Distance_3D( o.position, world_stuff->player_array[i].tank.orient.position );

                        if( temp_dist < player_events[index].angles.tank_dist_3D[i] )
                            {
                             
                             /* Replace vehicle's stuff with the decoy */

                             /* Compute the 2d position of the decoy and store it */
                             Cylinder_x_y( o.position, player_events[index].angles.tank_pos_2D[i] ); 
                             /* Compute the 2d angle and distance from the decoy */
                             player_events[index].angles.tank_dist_2D[i] = (short)Point_Distance_2D( temp_point_one, player_events[index].angles.tank_pos_2D[i] );
                             player_events[index].angles.tank_angle_2D[i] = Point_Angle_2D( player_events[index].angles.tank_pos_2D[i], &world_stuff->player_array[index].tank.orient );
     
                             Copy_Point_3D( o.position, player_events[index].angles.tank_pos_3D[i] ); 
                             player_events[index].angles.tank_dist_3D[i] = Point_Distance_3D( world_stuff->player_array[index].tank.orient.position, o.position );
                             player_events[index].angles.tank_angle_3D[i].horizontal_angle = Horizontal_Point_Angle_3D( o.position, &world_stuff->player_array[index].tank.orient );
                             player_events[index].angles.tank_angle_3D[i].vertical_angle   = Vertical_Point_Angle_3D( o.position, &world_stuff->player_array[index].tank.orient );

                            } /* End if dist is less */

                       } /* End if decoy */

                   bullet = bullet->next;

                  } /* End while */

             } /* End if enemy */

        } /* End for */


    } /* End of Update_Angles */





/* Fill up the memory section of player_events */
void Update_Memory( WorldStuff *world_stuff, int index )
    {
     int i, j;
     float nearest_dist = 50000;
     int nearest = 255; /* Indicates no enemy */
     float temp;
     short bullets_owned[8]; /* Number of bullets each and radar base hit us with */
     int most_hits = 0;
     int highest_hit_index = 255;
     Float_Point temp_point;
     boolean stuck = TRUE;


     /* Zero out owners of bullets */
     for( i = 0; i < 6; i++ )
         bullets_owned[i] = 0;
     
     /* Zero out number of bullets that have hit us recently */
     player_events[index].memory.num_bullet_hits = 0;
                 
     /* Loop through the bullets that have hit us in the last BULLET_THRESHOLD frames and
        figure out who the greatest threat is */
     for( i = 0; i < BULLET_THRESHOLD; i++ )
         {
          /* Loop through every vehicle and figure out which one this gunfire came from */
          if( player_events[index].memory.bullet_hit[i] == TRUE )
              {
               for( j = 0;j < 6; j++ )
                   {
                    if( player_events[index].memory.bullet_owner[i] == j )
                        {
                         bullets_owned[j]++;
                         player_events[index].memory.num_bullet_hits++;
                        }
                   
                   } /* End for loop through tanks */     
              } /* End if bullet hit this frame */
         } /* End for loop through bullets */
     
     /* Set the next bullet hit flag to false */    
     player_events[index].memory.bullet_index++;
     player_events[index].memory.bullet_index %= BULLET_THRESHOLD;
     player_events[index].memory.bullet_hit[player_events[index].memory.bullet_index] = FALSE;
         

     player_events[index].memory.recent_damage = Recent_Damage( index );

    
     /* Figure out which enemy we should attack */
     for( i = 0; i < 6; i++ )
         {
          
          if( world_stuff->player_array[i].tank.alive && world_stuff->player_array[index].team != world_stuff->player_array[i].team &&
              !world_stuff->player_array[i].tank.cloaking_active )
              {
               if( world_stuff->player_array[i].tank.vehicle_mode == Air || world_stuff->player_array[i].tank.vehicle_mode == Landing )
                   temp = player_events[index].angles.tank_dist_3D[i];
               else
                   temp = player_events[index].angles.tank_dist_2D[i];
          
              
               if( temp < nearest_dist )
                  {
                   nearest = i;
                   nearest_dist = temp;
                  }
              
              }   /* End if */
              
         }  /* End for */
        
     if( player_events[index].enemy_radar_alive )     
         {
          if( player_events[index].angles.enemy_radar_dist_3D < nearest_dist )
              {
               if( world_stuff->player_array[index].tank.team == RED_TEAM )
                    nearest = 7;
               else  
                    nearest = 6;
              }
         }

     /* Pick the current enemy based on nearest enemy or number of 
        hits each one got on you...depending on number of times hit */
     if( player_events[index].memory.num_bullet_hits < 10 )
         {
          player_events[index].memory.current_enemy = nearest;
         }
     else /* We've been hit a lot lately... */
         {
          for( i = 0; i < 8; i++ )
              {
               if( bullets_owned[i] > most_hits )
                   {
                    most_hits = bullets_owned[i];
                    highest_hit_index = i;
                   }
              }  
         
          player_events[index].memory.current_enemy = highest_hit_index;
         } 


     Copy_Point_3D( world_stuff->player_array[index].tank.orient.position, player_events[index].memory.my_positions[player_events[index].memory.my_position_index] );
     player_events[index].memory.my_position_index++;
     if( player_events[index].memory.my_position_index >= STUCK_THRESHOLD )
         player_events[index].memory.my_position_index = 0;

     Copy_Point_3D( world_stuff->player_array[index].tank.orient.position, temp_point );
     for( i = 0; i < STUCK_THRESHOLD; i++ )
         {
          if( temp_point[X] != player_events[index].memory.my_positions[i][X] ||
              temp_point[Y] != player_events[index].memory.my_positions[i][Y] ||
              temp_point[Z] != player_events[index].memory.my_positions[i][Z] )
              stuck = FALSE;
         }

     if( stuck )
         {
          player_events[index].memory.stuck = TRUE;
          player_events[index].memory.stuck_frames = STUCK_FRAMES;
         }

    } /* End of Update_Memory */

                         
                         
/* Fill up the player_events data structure and call update_angles and
   update_memory */
void Update_Player_Events( WorldStuff *world_stuff, int index )
    {
     int i, temp, first_enemy;
     RadarBase *radar_base, *enemy_radar_base; /* Pointers to radar bases */
     Float_Point2d temp_point_one, temp_point_two;


     /* One team is 0-2 and the other is 3-5 
        Figure out which enemy tanks to loop through 
        and alias a pointer to our radar base and the 
        enemy radar base */
     if( world_stuff->player_array[index].team == RED_TEAM ) 
         {                   
          first_enemy = 3;             
          radar_base = &world_stuff->red_radar_base;
          enemy_radar_base = &world_stuff->blue_radar_base;
         }
     else /* We are blue team */
         {
          first_enemy = 0;
          radar_base = &world_stuff->blue_radar_base;
          enemy_radar_base = &world_stuff->red_radar_base;
         }


     /* Update the radar base alive variables */
     player_events[index].enemy_radar_alive  = (unsigned char)enemy_radar_base->alive;
     player_events[index].my_radar_alive     = (unsigned char)radar_base->alive;
     
     if( radar_base->current_hitpoints < 40 ) /* 40 is arbitrary */
         player_events[index].radar_hitpoints_low = TRUE;
     else                           
         player_events[index].radar_hitpoints_low = FALSE;

     /* Reset values to zero so they can increment in the loop */
     player_events[index].enemies_alive = 0;
     player_events[index].friends_alive = 0;
     
     
          
     for( i = 0; i < 6; i++ ) 
         {
          player_events[index].tank_hitpoints[i]   = (unsigned char)world_stuff->player_array[i].tank.current_hitpoints;
          
          if( world_stuff->player_array[i].tank.alive && 
          (world_stuff->player_array[i].tank.team != world_stuff->player_array[index].tank.team ) )
              player_events[index].enemies_alive++;  
              
          if( world_stuff->player_array[i].tank.alive &&
          (world_stuff->player_array[i].tank.team == world_stuff->player_array[index].tank.team ) ) 
              player_events[index].friends_alive++;
              
          if( world_stuff->player_array[i].tank.vehicle_mode == Air )
              player_events[index].tank_in_air[i] = 1;
          else
              player_events[index].tank_in_air[i] = 0;
          
         }


     player_events[index].memory.in_air_array[player_events[index].memory.in_air_index] = player_events[index].tank_in_air[index];                                    
     if( player_events[index].memory.in_air_index++ >= 30 )
         player_events[index].memory.in_air_index = 0;


     player_events[index].my_D    = world_stuff->player_array[index].tank.laser_damage;
     player_events[index].enemy_D = world_stuff->player_array[first_enemy].tank.laser_damage;
     player_events[index].in_air  = world_stuff->player_array[index].table[ INDEX_MODE_SWITCH ];
     player_events[index].missiles_stored = world_stuff->player_array[index].tank.missiles_stored;
     
     if( player_events[index].missiles_stored > 0 )
         player_events[index].missiles_empty = FALSE;
     else
         player_events[index].missiles_empty = TRUE;

     
     /* Update the hitpoints high or low flags */
     temp = world_stuff->player_array[index].tank.max_hitpoints;
     if( world_stuff->player_array[index].tank.current_hitpoints < (temp / 4) )
         {
          player_events[index].hitpoints_low = TRUE;
          player_events[index].hitpoints_high = FALSE;
         }
     else if( world_stuff->player_array[index].tank.current_hitpoints > ( (temp / 4) * 3) )
         {
          player_events[index].hitpoints_low = FALSE;
          player_events[index].hitpoints_high = TRUE;
         }
     else
         {
          player_events[index].hitpoints_low = FALSE;
          player_events[index].hitpoints_high = FALSE;
         }

/* If we are in the air all things are computed in 3d...but if we are on the
   ground things in the are are computed in 3d and things on the ground are
   computed in 2d */
     if( world_stuff->player_array[index].tank.vehicle_mode == Surface )
         {
          Cylinder_x_y( world_stuff->player_array[index].tank.orient.position, temp_point_one );
          Cylinder_x_y( world_stuff->energy_square.center, temp_point_two );

          if( Is_Visible_2D( temp_point_one, temp_point_two, &world_stuff->pylons ) )
              player_events[index].energy_visible = TRUE;
          
          for( i = 0; i < 6; i++ ) 
             {
              if( world_stuff->player_array[i].tank.vehicle_mode == Surface )
                  {
                   Cylinder_x_y( world_stuff->player_array[i].tank.orient.position, temp_point_two );

                   if( Is_Visible_2D( temp_point_one, temp_point_two, &world_stuff->pylons ) )
                       player_events[index].tank_visible[i] = TRUE;
                  }
              else /* Enemy is in the air...do 3d */
                  {
                   if( Is_Visible_3D( world_stuff->player_array[index].tank.orient.position, world_stuff->player_array[i].tank.orient.position, &world_stuff->pylons ) )
                       player_events[index].tank_visible[i] = TRUE;
                  }
             } /* End for */
         
         }
     else /* We are in the air...calculate in 3-D */
         {
          for( i = 0; i < 6; i++ )
             {
              if( Is_Visible_3D( world_stuff->player_array[index].tank.orient.position, world_stuff->player_array[i].tank.orient.position, &world_stuff->pylons ) )
                   player_events[index].tank_visible[i] = TRUE;
             }
          if( Is_Visible_3D( world_stuff->player_array[index].tank.orient.position, world_stuff->energy_square.center, &world_stuff->pylons ) )
              {
               player_events[index].energy_visible = TRUE;
              }
         } /* End if we're on the ground */

     
     /* Are our missiles locked on the enemy? */
     if( world_stuff->player_array[index].tank.target != NULL )
          player_events[index].missiles_locked = TRUE;
     else
          player_events[index].missiles_locked = FALSE; 
     
     for( i = 0; i < 6; i++ )
         if( world_stuff->player_array[i].team == world_stuff->player_array[index].team &&
             world_stuff->player_array[i].leader )
              {
               player_events[index].my_leader = i;
              }

     Update_Angles( world_stuff, index );
     Update_Memory( world_stuff, index );
     
    } /* End of Update_Player_Events */




/* bullet shit 

          num_bullets = 0;
          for( i = 0; i < 6; i++ )
              {
               if( i != index )
                   {
                    bullet = world_stuff->player_array[i].tank.projectile_list;
                    while( bullet != NULL )
                        {
                         o.position[X] = mtof( bullet->orient.position[X] );
                         o.position[Y] = mtof( bullet->orient.position[Y] );
                         o.position[Z] = mtof( bullet->orient.position[Z] );
                         
                         o.front[X]    = mtof( bullet->orient.front[X] );
                         o.front[Y]    = mtof( bullet->orient.front[Y] );
                         o.front[Z]    = mtof( bullet->orient.front[Z] );
                         
                         o.up[X]       = mtof( bullet->orient.up[X] );
                         o.up[Y]       = mtof( bullet->orient.up[Y] );
                         o.up[Z]       = mtof( bullet->orient.up[Z] );
                        
                         player_events[index].angles.bullet_dist_3D[num_bullets] = Point_Distance_3D( o.position, world_stuff->player_array[index].tank.orient.position );
                         player_events[index].angles.bullet_angle_3D[num_bullets].horizontal_angle = Horizontal_Point_Angle_3D( o.position, &world_stuff->player_array[index].tank.orient );
                         player_events[index].angles.bullet_angle_3D[num_bullets].vertical_angle   = Vertical_Point_Angle_3D( o.position, &world_stuff->player_array[index].tank.orient );
                         player_events[index].angles.bullet_facing_angle_3D[num_bullets].horizontal_angle = Horizontal_Point_Angle_3D( world_stuff->player_array[index].tank.orient.position, &o );
                         player_events[index].angles.bullet_facing_angle_3D[num_bullets].vertical_angle   = Vertical_Point_Angle_3D( world_stuff->player_array[index].tank.orient.position, &o );
                        
                         num_bullets++;
                         bullet = bullet->next;
                        } 
                   
                   }  
                   
              }      
          
          

          for( i = 0; i < 2; i++ )
              {
               if( i == 0 )
                   radar_base = &world_stuff->red_radar_base;
               else
                   radar_base = &world_stuff->blue_radar_base;
                    
               bullet = radar_base->projectile_list;
               while( bullet != NULL )
                   {
                    o.position[X] = mtof( bullet->orient.position[X] );
                    o.position[Y] = mtof( bullet->orient.position[Y] );
                    o.position[Z] = mtof( bullet->orient.position[Z] );
                         
                    o.front[X]    = mtof( bullet->orient.front[X] );
                    o.front[Y]    = mtof( bullet->orient.front[Y] );
                    o.front[Z]    = mtof( bullet->orient.front[Z] );
                         
                    o.up[X]       = mtof( bullet->orient.up[X] );
                    o.up[Y]       = mtof( bullet->orient.up[Y] );
                    o.up[Z]       = mtof( bullet->orient.up[Z] );
                         
                        
                    player_events[index].angles.bullet_dist_3D[num_bullets] = Point_Distance_3D( o.position, world_stuff->player_array[index].tank.orient.position );
                    player_events[index].angles.bullet_angle_3D[num_bullets].horizontal_angle = Horizontal_Point_Angle_3D( o.position, &world_stuff->player_array[index].tank.orient );
                    player_events[index].angles.bullet_angle_3D[num_bullets].vertical_angle   = Vertical_Point_Angle_3D( o.position, &world_stuff->player_array[index].tank.orient );
                    player_events[index].angles.bullet_facing_angle_3D[num_bullets].horizontal_angle = Horizontal_Point_Angle_3D( world_stuff->player_array[index].tank.orient.position, &o );
                    player_events[index].angles.bullet_facing_angle_3D[num_bullets].vertical_angle   = Vertical_Point_Angle_3D( world_stuff->player_array[index].tank.orient.position, &o );
                        
                    num_bullets++;
                    bullet = bullet->next;
                   }
              
              }  


*/

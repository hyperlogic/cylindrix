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

#include "do_state.h"     
#include "path.h"

extern player_events_type player_events[6];     



void Get_Energy_2D( WorldStuff *world_stuff, int index )
    {
     short angle, enemy_angle;
     float dist, enemy_dist;
     player_events_type *events;
     int current_enemy;                           
     short skill_level;
     
     enemy_dist = enemy_angle = 0; /* To shut up compiler */                                
     
     events = &player_events[index];                               
     angle = events->angles.energy_angle_2D;
     dist  = events->angles.energy_dist_2D;
     skill_level = world_stuff->player_array[index].character.skill_level;
     
     current_enemy = player_events[index].memory.current_enemy;
     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          enemy_dist  = player_events[index].angles.tank_dist_2D[current_enemy];
          enemy_angle = player_events[index].angles.tank_angle_2D[current_enemy];
         }
     else
         {
          if( current_enemy == 6 || current_enemy == 7 )
              {
               enemy_dist  = player_events[index].angles.enemy_radar_dist_2D;
               enemy_angle = player_events[index].angles.enemy_radar_angle_2D;
              }
         }

     if( player_events[index].memory.following_path == 0 )
         {
          if( events->energy_visible )
              {
               if( skill_level == 0 )
                   {
                    Move_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                   }
               else if( skill_level == 1 )   
                   {
                    Move_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                   }
               else if( skill_level == 2 )   
                   {
                    Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                     
                     if( events->memory.current_enemy != 255 )
                         Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index ); 
                   }
               else /* Ground skill == 3 */
                   {
                    Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                    
                    if( events->memory.current_enemy != 255 )
                        Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index ); 
                    Missiles( world_stuff, index );                           
                    Projectile_Special_Weapons( world_stuff, index );
                   }
                   
              }
           else 
               {                                                               
                Make_Path_2D( world_stuff, index, events->angles.energy_pos_2D, -1 );
               }
         
         }
     else
          {
           if( events->energy_visible )
              {
               player_events[index].memory.following_path = 0;
               Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
              }
           else
               Follow_Path_2D( world_stuff, index ); 
          }  
     
     
         
    } /* End of Get_Energy_2D */
    
void Get_Energy_3D( WorldStuff *world_stuff, int index )
    {
     short horizontal_angle, vertical_angle;
     float dist;
     player_events_type *events;
     Float_Point point, destination;                           
                                
     events = &player_events[index];                               
     horizontal_angle = events->angles.energy_angle_3D.horizontal_angle;
     vertical_angle = events->angles.energy_angle_3D.vertical_angle;
     dist  = events->angles.energy_dist_3D;
     

     Copy_Point_3D( world_stuff->player_array[index].tank.orient.position, point );
     Copy_Point_3D( player_events[index].angles.energy_pos_3D, destination );

     if( events->angles.energy_dist_2D < 0.6 )
         {
          Aim_Gun_3D( world_stuff, index );       
          Missiles( world_stuff, index );     
          Projectile_Special_Weapons( world_stuff, index );
         }
     else
         {
          if( player_events[index].memory.following_path == 0 )
              {
               if( Is_Visible_3D( point, destination, &world_stuff->pylons) )
                   {
                    Move_Towards_3D( horizontal_angle, vertical_angle, dist, world_stuff->player_array[index].table, 2, 0.5 );
                   }
               else 
                   {
                    Make_Path_3D( world_stuff, index, destination, 255);
                   }
              }
          else
              {
               if( Is_Visible_3D( point, destination, &world_stuff->pylons ) )
                   {
                    player_events[index].memory.following_path = 0;
                    Move_Towards_3D( horizontal_angle, vertical_angle, dist, world_stuff->player_array[index].table, 2, 0.5 );
                   }
               else
                   Follow_Path_3D( world_stuff, index ); 
              }  
    
         }

    } /* End of Get_Energy_3D */


void Get_Pylons_2D( WorldStuff *world_stuff, int index )
    {
     short closest_pylon, angle, enemy_angle;
     float closest_dist, dist, enemy_dist;
     int i;
     int current_enemy;
     short current_pylon;
     short skill_level;
     Float_Point2d point, destination;

     
     enemy_dist = enemy_angle = 0; /* To shut up compiler */
     
     current_pylon = player_events[index].memory.current_pylon;
     
     closest_dist = 20000;
     closest_pylon = 0;
     current_enemy = player_events[index].memory.current_enemy;
     
     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          enemy_dist  = player_events[index].angles.tank_dist_2D[current_enemy];
          enemy_angle = player_events[index].angles.tank_angle_2D[current_enemy];
         }
     else
         {
          if( current_enemy == 6 || current_enemy == 7 )
              {
               enemy_dist  = player_events[index].angles.enemy_radar_dist_2D;
               enemy_angle = player_events[index].angles.enemy_radar_angle_2D;
              }
         }

     
     if( current_pylon == 255 || player_events[index].angles.pylon_owner[current_pylon] == world_stuff->player_array[index].team )
         {
          for( i = 0; i < player_events[index].angles.num_pylons; i++ )
              {
               if( player_events[index].angles.pylon_dist_2D[i] < closest_dist )
                   {
                    if( player_events[index].angles.pylon_owner[i] != world_stuff->player_array[index].team )
                        {
                         closest_dist = player_events[index].angles.pylon_dist_2D[i];
                         closest_pylon = i;
                        }
                   }
              }
         
          current_pylon = closest_pylon;
         }
         
         
     dist  = player_events[index].angles.pylon_dist_2D[ current_pylon ];
     angle = player_events[index].angles.pylon_angle_2D[ current_pylon ];
     
     
     player_events[index].memory.current_pylon = current_pylon; 
     
     
     skill_level = world_stuff->player_array[index].character.skill_level;
     
     
     
     Cylinder_x_y( world_stuff->player_array[index].tank.orient.position, point );
     Pylon_Position_2D( &world_stuff->pylons.pylon[current_pylon], destination );
     

     /* If the pylon we're after has changed recently */
     if( destination[X] != player_events[index].memory.final_destination_2D[X] ||
         destination[Y] != player_events[index].memory.final_destination_2D[Y] ) 
         { 
          Make_Path_2D( world_stuff, index, destination, current_pylon );
         }
         
     if( player_events[index].memory.following_path == 0 )
         {
          if( Is_Visible_2D_Pylon( point, destination, &world_stuff->pylons, current_pylon ) )
              {
               
               if( skill_level == 0 )
                   {
                    Move_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                   }
               else if( skill_level == 1 )   
                   {
                    Move_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                   }
               else if( skill_level == 2 )   
                   {
                    Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                    if( player_events[index].memory.current_enemy != 255 )
                        Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index );
                   }
               else /* Ground skill == 3 */
                   {
                    Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                    if( player_events[index].memory.current_enemy != 255 )
                        Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index );
                    Missiles( world_stuff, index );                           
                    Projectile_Special_Weapons( world_stuff, index );
                   }
              }
          else 
               {
                Make_Path_2D( world_stuff, index, destination, current_pylon );
               }
               
         }
     else
          {
           if( Is_Visible_2D_Pylon( point, destination, &world_stuff->pylons, current_pylon ) )
              {
               player_events[index].memory.following_path = 0;
               Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
              }
           else
               Follow_Path_2D( world_stuff, index ); 
          }  
         
    
    } /* End of Get_Pylons_2D */
                  

void Get_Pylons_3D( WorldStuff *world_stuff, int index )
    {
     short closest_pylon;
     short horizontal_angle, vertical_angle;
     float closest_dist, dist;
     int i;
     short current_pylon;
     Float_Point point, destination;
     
     current_pylon = player_events[index].memory.current_pylon;
     
     closest_dist = 20000;
     closest_pylon = 0;
     
     
     if( current_pylon == 255 || player_events[index].angles.pylon_owner[current_pylon] == world_stuff->player_array[index].team )
         {
          for( i = 0; i < player_events[index].angles.num_pylons; i++ )
              {
               if( player_events[index].angles.pylon_dist_3D[i] < closest_dist )
                   {
                    if( player_events[index].angles.pylon_owner[i] != world_stuff->player_array[index].team )
                        {
                         closest_dist = player_events[index].angles.pylon_dist_3D[i];
                         closest_pylon = i;
                        }
                   }
              }
          current_pylon = closest_pylon;
         }                                               
         
     player_events[index].memory.current_pylon = current_pylon; 
         
     dist  = player_events[index].angles.pylon_dist_3D[ current_pylon ];
     horizontal_angle = player_events[index].angles.pylon_angle_3D[ current_pylon ].horizontal_angle;
     vertical_angle = player_events[index].angles.pylon_angle_3D[ current_pylon ].vertical_angle;
     

     Copy_Point_3D( world_stuff->player_array[index].tank.orient.position, point );
     Pylon_Position_3D( &world_stuff->pylons.pylon[current_pylon], destination );
     

     if( player_events[index].memory.following_path == 0 )
         {
          if( Is_Visible_3D_Pylon( point, destination, &world_stuff->pylons, current_pylon ) )
              {
               Move_Towards_3D( horizontal_angle, vertical_angle, dist, world_stuff->player_array[index].table, 2, 0.5 );
              }
          else 
               {
                Make_Path_3D( world_stuff, index, destination, current_pylon );
               }
         }
     else
          {
           if( Is_Visible_3D_Pylon( point, destination, &world_stuff->pylons, current_pylon ) )
              {
               player_events[index].memory.following_path = 0;
               Move_Towards_3D( horizontal_angle, vertical_angle, dist, world_stuff->player_array[index].table, 2, 0.5 );
              }
           else
               Follow_Path_3D( world_stuff, index ); 
          }  


    } /* End of Get_Pylons_3D */

                  
                  
void Attack_2D( WorldStuff *world_stuff, int index )
    {
     unsigned char *table;
     float dist;
     short angle;
     team_type my_team;
     int current_enemy;
     short skill_level;
     player_events_type *events;
     Float_Point2d enemy_point; /* For is visible */
     
     dist = angle = 0; /* To shut up compiler */
     
     current_enemy = player_events[index].memory.current_enemy;
     skill_level = world_stuff->player_array[index].character.skill_level;
     events = &player_events[index];
     
     /* Store angle and distance to enemy vehicle for ease of use */   
     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          dist  = player_events[index].angles.tank_dist_2D[current_enemy];
          angle = player_events[index].angles.tank_angle_2D[current_enemy];

          enemy_point[X] = player_events[index].angles.tank_pos_2D[current_enemy][X];
          enemy_point[Y] = player_events[index].angles.tank_pos_2D[current_enemy][Y];
         }
     else
         {
          if( current_enemy == 6 || current_enemy == 7 )
              {
               dist  = player_events[index].angles.enemy_radar_dist_2D;
               angle = player_events[index].angles.enemy_radar_angle_2D;
               enemy_point[X] = player_events[index].angles.enemy_radar_pos_2D[X];
               enemy_point[Y] = player_events[index].angles.enemy_radar_pos_2D[Y];
              }
         }
     
     /* Record this players team for ease of reference */
     my_team = world_stuff->player_array[index].team;

     /* Reference pointer to input table */               
     table = world_stuff->player_array[index].table;
     
      
     if( player_events[index].memory.following_path == 0 )
         {
          if( Is_Visible_2D( enemy_point, player_events[index].angles.tank_pos_2D[index], &world_stuff->pylons ) )
              {
               if( skill_level == 0 )
                   {
                    Move_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                   }
               else if( skill_level == 1 )   
                   {
                    Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                    Aim_Gun_2D( world_stuff, angle, dist, index );
                   }
               else if( skill_level >= 2 )   
                   {
                    Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                    Aim_Gun_2D( world_stuff, angle, dist, index );
                    Missiles( world_stuff, index );                           
                    Projectile_Special_Weapons( world_stuff, index );
                   }
              }
          else 
              {                                                               
               Make_Path_2D( world_stuff, index, enemy_point, -1 );
              }
         
         }
     else
          {
           if( Is_Visible_2D( enemy_point, player_events[index].angles.tank_pos_2D[index], &world_stuff->pylons ) )
              {
               player_events[index].memory.following_path = 0;
               Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
              }
           else
               Follow_Path_2D( world_stuff, index ); 
          }  
     
              
    } /* end of Attack_2D */ 

    
                  
void Attack_3D( WorldStuff *world_stuff, int index )
    {
     unsigned char *table;
     float dist;
     angle_type_3D angle;
     int current_enemy;
     Float_Point enemy_point;


     current_enemy = player_events[index].memory.current_enemy;
     
     if( world_stuff->player_array[index].tank.vehicle_mode == Surface )
         Mode_Switch_Into_Air( world_stuff->player_array[index].table );
     
     /* Store angle and distance to enemy vehicle for ease of use */   
     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          dist                   = player_events[index].angles.tank_dist_3D[current_enemy];
          angle.horizontal_angle = player_events[index].angles.tank_angle_3D[current_enemy].horizontal_angle;
          angle.vertical_angle   = player_events[index].angles.tank_angle_3D[current_enemy].vertical_angle;
          Copy_Point_3D( world_stuff->player_array[current_enemy].tank.orient.position, enemy_point );
         }
     else /* current_enemy == 6 || current_enemy == 7  */
         {
          dist                   = player_events[index].angles.enemy_radar_dist_3D;
          angle.horizontal_angle = player_events[index].angles.enemy_radar_angle_3D.horizontal_angle;
          angle.vertical_angle   = player_events[index].angles.enemy_radar_angle_3D.vertical_angle;
          Copy_Point_3D( player_events[index].angles.enemy_radar_pos_3D, enemy_point );
         }
     
     /* Reference pointer to input table */               
     table = world_stuff->player_array[index].table;

     if( player_events[index].memory.following_path == 0 )
         {
          if( Is_Visible_3D( world_stuff->player_array[index].tank.orient.position, enemy_point, &world_stuff->pylons ) )
              {
               if( (angle.horizontal_angle < 10 || angle.horizontal_angle > 350) && (angle.vertical_angle < 10 || angle.vertical_angle > 350) )
                   {
                    if( world_stuff->player_array[index].character.skill_level >= 3 )
                        Aim_Gun_3D( world_stuff, index );

                    if( dist > 5 )
                        Up_Throttle( table );
                    else
                        Down_Throttle( table );

                    Fire_Gun( table );
                   }
               else
                   {
                    Move_Towards_3D( angle.horizontal_angle, angle.vertical_angle, dist, table, 3, 3 ); 
                   }

               if( world_stuff->player_array[index].character.skill_level >= 2 )
                   {
                    Missiles( world_stuff, index );     
                    Projectile_Special_Weapons( world_stuff, index );
                   }

              } /* End if visible */
          else 
               {
                Make_Path_3D( world_stuff, index, enemy_point, 255);
               }
     

         } /* End if not following path */
     else
          {
           if( Is_Visible_3D( world_stuff->player_array[index].tank.orient.position, enemy_point, &world_stuff->pylons ) )
              {
               player_events[index].memory.following_path = 0;
               Move_Towards_3D( angle.horizontal_angle, angle.vertical_angle, dist, world_stuff->player_array[index].table, 6, 6 );
              }
           else
               Follow_Path_3D( world_stuff, index ); 
          }  


    } /* end of Attack_3D */

                  
                  
void Kill_Radar_Base_2D( WorldStuff *world_stuff, int index ) 
    {
     short angle, aim_angle;
     float dist;
     player_events_type *events;
     unsigned char *table;
     Player *player;                          
     short skill_level;
     
                          
     events = &player_events[index];
     table = world_stuff->player_array[index].table; 
     player = &world_stuff->player_array[index];
     skill_level = world_stuff->player_array[index].character.skill_level;
     
     angle = events->angles.enemy_radar_angle_2D;
     dist  = events->angles.enemy_radar_dist_2D;
     
     aim_angle = angle;
     
     
     if( player_events[index].memory.following_path == 0 )
         {
          if( Is_Visible_2D( events->angles.tank_pos_2D[index], events->angles.enemy_radar_pos_2D, &world_stuff->pylons) )
              {
               if( skill_level == 0 )
                   {
                    Move_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                   }
               else if( skill_level == 1 )   
                   {
                    Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                    if( events->memory.current_enemy != 255 )
                        Aim_Gun_2D( world_stuff, aim_angle, dist, index );
                    Missiles( world_stuff, index );                           
                   }
               else /* Ground skill == 2 or 3 */
                   {
                    if( dist < 5 )
                        {
                         Rotate_Around_Clockwise( angle, dist, world_stuff->player_array[index].table );
                         Aim_Gun_2D( world_stuff, angle, dist, index );
                         Missiles( world_stuff, index );                           
                         /* If this is the locust, shoot off decoys to confuse the radar base */
                         if( world_stuff->player_array[index].tank.vtype == Locust )
                         Decoy( world_stuff, index );
                        }
                    else
                        {
                         Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                         Aim_Gun_2D( world_stuff, angle, dist, index );
                         Missiles( world_stuff, index );                           
                        }
                   }
              }
           else /* Not visible */
               {                                                               
                Make_Path_2D( world_stuff, index, events->angles.enemy_radar_pos_2D, -1 );
               }
         }
     else /* We are following path */
          {
           if( Is_Visible_2D( events->angles.tank_pos_2D[index], events->angles.enemy_radar_pos_2D, &world_stuff->pylons) )
              {
               player_events[index].memory.following_path = 0;
               Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
              }
           else
               Follow_Path_2D( world_stuff, index ); 
          }  
     
    
    } /* End of Kill_Radar_Base_2D */
                  
                                                      
void Kill_Radar_Base_3D( WorldStuff *world_stuff, int index )
    {
     short horizontal_angle, vertical_angle;
     float dist;
     player_events_type *events;
     Float_Point point, destination;


     events = &player_events[index];
     
     dist = events->angles.enemy_radar_dist_3D;
     horizontal_angle = events->angles.enemy_radar_angle_3D.horizontal_angle;
     vertical_angle = events->angles.enemy_radar_angle_3D.vertical_angle;
     
     Copy_Point_3D( world_stuff->player_array[index].tank.orient.position, point );
     Copy_Point_3D( player_events[index].angles.enemy_radar_pos_3D, destination );
     
     Missiles( world_stuff, index );     
     
     if( player_events[index].memory.following_path == 0 )
         {
          if( Is_Visible_3D( point, destination, &world_stuff->pylons ) )
              {
               Move_Towards_3D( horizontal_angle, vertical_angle, dist, world_stuff->player_array[index].table, 6, 6 ); 
               if( world_stuff->player_array[index].character.skill_level >= 2 )
                   if( (horizontal_angle < 5 || horizontal_angle > 355) &&
                       (vertical_angle < 5 || vertical_angle > 355 ) )
                       {
                        Aim_Gun_3D( world_stuff, index );       
                        Fire_Gun( world_stuff->player_array[index].table );
                       }
              }
          else 
               {
                Make_Path_3D( world_stuff, index, destination, 255);
               }
         }
     else
          {
           if( Is_Visible_3D( point, destination, &world_stuff->pylons ) )
              {
               player_events[index].memory.following_path = 0;
               Move_Towards_3D( horizontal_angle, vertical_angle, dist, world_stuff->player_array[index].table, 6, 6 );
              }
           else
               Follow_Path_3D( world_stuff, index ); 
          }  


    } /* End of Kill_Radar_Base_3D */
                                                      
                                                      
                               
void Protect_Radar_Base_2D( WorldStuff *world_stuff, int index )
    {
     RadarBase *radar_base;
     Float_Point2d point_one, point_two;
     Player *player;
     short angle, enemy_angle;
     float dist, enemy_dist;
     int current_enemy;
     
     enemy_dist = enemy_angle = 0; /* To shut up compiler */
     
     if( world_stuff->player_array[index].team == RED_TEAM )
         radar_base = &world_stuff->red_radar_base;
     else                                          
         radar_base = &world_stuff->red_radar_base;
     
     player = &world_stuff->player_array[index];
     current_enemy = player_events[index].memory.current_enemy;
     
     Cylinder_x_y( player->tank.orient.position, point_one );
     Cylinder_x_y( radar_base->base_orient.position, point_two );
     
     dist  = Point_Distance_2D( point_one, point_two );
     angle = Point_Angle_2D( point_two, &player->tank.orient );
     
     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          enemy_dist  = player_events[index].angles.tank_dist_2D[current_enemy];
          enemy_angle = player_events[index].angles.tank_angle_2D[current_enemy];
         }
     else
         {
          if( current_enemy == 6 || current_enemy == 7 )
              {
               enemy_dist  = player_events[index].angles.enemy_radar_dist_2D;
               enemy_angle = player_events[index].angles.enemy_radar_angle_2D;
              }
         }

     if( dist > 5 )
         {
          Sidestep_Towards_2D( angle, dist, player->table );
         }
     if( current_enemy != 255 )
         {
          Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index );     
          
          if( world_stuff->player_array[index].character.skill_level >= 2 )
              {
               Missiles( world_stuff, index );
               Projectile_Special_Weapons( world_stuff, index );

              }
         } 
          
    } /* End of Protect_Radar_Base_2D */                              
                               
                               
void Protect_Radar_Base_3D( WorldStuff *world_stuff, int index )
    {
     RadarBase *radar_base;          
     short horizontal_angle, vertical_angle;
     float dist;
     int current_enemy;
     Player *player;
     
     
     if( world_stuff->player_array[index].team == RED_TEAM )
         radar_base = &world_stuff->red_radar_base;
     else                                          
         radar_base = &world_stuff->blue_radar_base;

     
     player = &world_stuff->player_array[index];
     current_enemy = player_events[index].memory.current_enemy;
     
     
     dist = Point_Distance_3D( radar_base->base_orient.position, player->tank.orient.position );
     horizontal_angle = Horizontal_Point_Angle_3D( radar_base->base_orient.position, &player->tank.orient );
     vertical_angle = Vertical_Point_Angle_3D( radar_base->base_orient.position, &player->tank.orient );
     
     
     if( dist > 5 )
         Move_Towards_3D( horizontal_angle, vertical_angle, dist, player->table, 5, 5 );
     else
         {
          Down_Throttle( world_stuff->player_array[index].table );
          
          if( player_events[index].memory.current_enemy != 255 )
              {
               Attack_3D( world_stuff, index );
              }
         }

    } /* End of Protect_Radar_Base_3D */
                               

void Evade_2D( WorldStuff *world_stuff, int index )
    {                         
     short angle;
     float dist;
     short current_enemy;
     
     
     current_enemy = player_events[index].memory.current_enemy;
     dist = angle = 0; /* To shut up compiler */
     
     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          dist  = player_events[index].angles.tank_dist_2D[current_enemy];
          angle = player_events[index].angles.tank_angle_2D[current_enemy];
         }
     else
         {
          if( current_enemy == 6 || current_enemy == 7 )
              {
               dist  = player_events[index].angles.enemy_radar_dist_2D;
               angle = player_events[index].angles.enemy_radar_angle_2D;
              }
         }
     
     if( world_stuff->player_array[index].character.skill_level >= 2 )
         {
          Sidestep_Away_2D( angle, dist, world_stuff->player_array[index].table );
          if( current_enemy != 255 )
              {
               Aim_Gun_2D( world_stuff, angle, dist, index );     
               Missiles( world_stuff, index );
               Projectile_Special_Weapons( world_stuff, index );
              }
         }
     else
         {
          Sidestep_Away_2D( angle, dist, world_stuff->player_array[index].table );
         }
         
     /* If this is the locust, shoot off decoys to confuse the enemy */
     if( world_stuff->player_array[index].tank.vtype == Locust )
         Decoy( world_stuff, index );

    } /* End of Evade_2D */
    
void Evade_3D( WorldStuff *world_stuff, int index )
    {
     short horizontal_angle, vertical_angle;
     float dist;
     player_events_type *events;


     events = &player_events[index];
     horizontal_angle = events->angles.energy_angle_3D.horizontal_angle;
     vertical_angle = events->angles.energy_angle_3D.vertical_angle;
     dist  = events->angles.energy_dist_3D;

     New_Move_Towards_3D( horizontal_angle, vertical_angle, dist, world_stuff->player_array[index].table );


     /* If this is the locust, shoot off decoys to confuse the enemy */
     if( world_stuff->player_array[index].tank.vtype == Locust )
         Decoy( world_stuff, index );

    } /* End of Evade_3D */
                               
                               
void Group_2D( WorldStuff *world_stuff, int index )
    {
     short angle, enemy_angle;
     float dist, enemy_dist;
     short current_enemy;
     short leader;
     
     enemy_dist = enemy_angle = 0; /* To shut up compiler */

     leader = player_events[index].my_leader;
     current_enemy = player_events[index].memory.current_enemy;
     angle = player_events[index].angles.tank_angle_2D[leader];
     dist  = player_events[index].angles.tank_dist_2D[leader];
       
     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          enemy_dist  = player_events[index].angles.tank_dist_2D[current_enemy];
          enemy_angle = player_events[index].angles.tank_angle_2D[current_enemy];
         }
     else
         {
          if( current_enemy == 6 || current_enemy == 7 )
              {
               enemy_dist  = player_events[index].angles.enemy_radar_dist_2D;
               enemy_angle = player_events[index].angles.enemy_radar_angle_2D;
              }
         }
     
     if( player_events[index].memory.following_path == 0 )
         {
          if( player_events[index].tank_visible[leader] )
              {
               if( dist > 3 )
                   {
                    Sidestep_Towards_2D( angle, dist, world_stuff->player_array[index].table );
                    if( player_events[index].memory.current_enemy != 255 )
                        Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index ); 
                    Missiles( world_stuff, index );                           
                    Projectile_Special_Weapons( world_stuff, index );
                   }
               else
                   {
                    Rotate_Around_Clockwise( angle, dist, world_stuff->player_array[index].table );
                    if( player_events[index].memory.current_enemy != 255 )
                         Aim_Gun_2D( world_stuff, enemy_angle, enemy_dist, index ); 
                    Missiles( world_stuff, index );                           
                    Projectile_Special_Weapons( world_stuff, index );
                   }
         
              } /* End if visible */
          else      
              {                                                               
               Make_Path_2D( world_stuff, index, player_events[index].angles.tank_pos_2D[leader], -1 );
              }

         } /* End if following path */
     else
         {
          Follow_Path_2D( world_stuff, index ); 
         }

    } /* End of Group_2D */

void Group_3D( WorldStuff *world_stuff, int index )
    {
     Player *player;
     int current_enemy;
     short horizontal_angle, vertical_angle;
     float dist;
     int leader;


     player           = &world_stuff->player_array[index];
     current_enemy    = player_events[index].memory.current_enemy;
     leader           = player_events[index].my_leader;
     horizontal_angle = player_events[index].angles.tank_angle_3D[leader].horizontal_angle;
     vertical_angle   = player_events[index].angles.tank_angle_3D[leader].vertical_angle;
     dist             = player_events[index].angles.tank_dist_3D[leader];
     
     
     
     if( dist > 10 )
         Move_Towards_3D( horizontal_angle, vertical_angle, dist, player->table, 5, 5 );
     else
         {
          Down_Throttle( world_stuff->player_array[index].table );
          
          if( player_events[index].memory.current_enemy != 255 )
              {
               Attack_3D( world_stuff, index );
              }
         }
     
    } /* End of Group_3D */


/* These are the versions of the functions the AI calls...they 
   call the lower level 2-D and 3-D functions */                  
                  
void Attack( WorldStuff *world_stuff, int index )
    {
     int current_enemy;
     
     
     current_enemy = player_events[index].memory.current_enemy;


     if( current_enemy == 255 )
         {
          if( player_events[index].enemy_radar_alive )
              {
               Kill_Radar_Base( world_stuff, index );
               return;
              }
          else
              {
               Get_Pylons( world_stuff, index );
              }
             
         }

     if( player_events[index].memory.surface )
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Air)
              Mode_Switch_Onto_Ground( world_stuff->player_array[index].table );
          else if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
              Attack_2D( world_stuff, index );
         }
     else
         {  
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
              Mode_Switch_Into_Air( world_stuff->player_array[index].table );
          else if( world_stuff->player_array[index].tank.vehicle_mode == Air)
              Attack_3D( world_stuff, index );
         }               
         
    } /* End of Attack */     
         

                  
void Get_Energy( WorldStuff *world_stuff, int index )
    {                                            
    
     if( player_events[index].memory.surface )
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Air)
              Mode_Switch_Onto_Ground( world_stuff->player_array[index].table );
     
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
              Get_Energy_2D( world_stuff, index );
         }
     else
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
              Mode_Switch_Into_Air( world_stuff->player_array[index].table );
     
          if( world_stuff->player_array[index].tank.vehicle_mode == Air)
              Get_Energy_3D( world_stuff, index );
         }    
         
    } /* End of Get_Energy */                                                                     
    
    
void Get_Pylons( WorldStuff *world_stuff, int index )
    {
     if( player_events[index].memory.surface )
         {                                    
          if( world_stuff->player_array[index].tank.vehicle_mode == Air)
               Mode_Switch_Onto_Ground( world_stuff->player_array[index].table );
          else if( world_stuff->player_array[index].tank.vehicle_mode == Surface )
               Get_Pylons_2D( world_stuff, index );                             
         }      
     else                                         
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface )
               Mode_Switch_Into_Air( world_stuff->player_array[index].table );
          else if( world_stuff->player_array[index].tank.vehicle_mode == Air ) 
              {
               Get_Pylons_3D( world_stuff, index );    
              }
         }
              
    } /* End of Get_Pylons */
    

void Kill_Radar_Base( WorldStuff *world_stuff, int index )
    {                                         
     if( player_events[index].memory.surface )
          {                                    
           if( world_stuff->player_array[index].tank.vehicle_mode == Air)
               Mode_Switch_Onto_Ground( world_stuff->player_array[index].table );
           
           else if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
               Kill_Radar_Base_2D( world_stuff,index );
          }
     else                                         
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface )
               Mode_Switch_Into_Air( world_stuff->player_array[index].table );
          
          else if( world_stuff->player_array[index].tank.vehicle_mode == Air)
              Kill_Radar_Base_3D( world_stuff,index );
         }      
    } /* End of Kill_Radar_Base */    
    
    
void Protect_Radar_Base( WorldStuff *world_stuff, int index )
    {
     
     if( player_events[index].memory.surface )
         {                                    
          if( world_stuff->player_array[index].tank.vehicle_mode == Air)
               Mode_Switch_Onto_Ground( world_stuff->player_array[index].table );
          else if ( world_stuff->player_array[index].tank.vehicle_mode == Surface )
              Protect_Radar_Base_2D( world_stuff, index );
         }      
     else
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
              Mode_Switch_Into_Air( world_stuff->player_array[index].table );      
          else if ( world_stuff->player_array[index].tank.vehicle_mode == Air )
              Protect_Radar_Base_3D( world_stuff, index );
         }                    
         
    } /* Protect_Radar_Base */     
    
                                         
                                         
void Evade( WorldStuff *world_stuff, int index )
    {
     if( player_events[index].memory.surface ) 
         {                                    
          if( world_stuff->player_array[index].tank.vehicle_mode == Air) 
               Mode_Switch_Onto_Ground( world_stuff->player_array[index].table );
          else if ( world_stuff->player_array[index].tank.vehicle_mode == Surface )
              Evade_2D( world_stuff, index );
         }      
     else
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
              Mode_Switch_Into_Air( world_stuff->player_array[index].table );      
          else if ( world_stuff->player_array[index].tank.vehicle_mode == Air ) 
              Evade_3D( world_stuff, index );
         }
    
    } /* End of Evade */                                        
                                         
void Panic( WorldStuff *world_stuff, int index )
    {
     ;
    }
void Bezerk( WorldStuff *world_stuff, int index )
    {
     ;
    }
void Protect( WorldStuff *world_stuff, int index )
    {
     ;
    }
void Group( WorldStuff *world_stuff, int index )
    {
     if( player_events[index].memory.surface )
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Air)
              Mode_Switch_Onto_Ground( world_stuff->player_array[index].table );
     
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
              Group_2D( world_stuff, index );
         }
     else
         {
          if( world_stuff->player_array[index].tank.vehicle_mode == Surface)
              Mode_Switch_Into_Air( world_stuff->player_array[index].table );
     
          if( world_stuff->player_array[index].tank.vehicle_mode == Air)
              Group_3D( world_stuff, index );
         }    
     
    } /* End of Group */

void Hide( WorldStuff *world_stuff, int index )
    {
     ;
    }

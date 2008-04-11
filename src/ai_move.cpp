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

#include "ai_move.h"


extern player_events_type player_events[6];


void Move_Towards_2D( short angle, float dist, unsigned char *table )
    {
     if( angle <= 10 || angle >= 350 )
         {
          Move_Forward( table );
         }
     else if( angle > 5 && angle < 90 )
         {
          Move_Right( table );
          if( dist > 5 )
              Move_Forward( table );
         }
     else if( angle >= 90 && angle < 180 )
         {
          Move_Right( table );
         }
     else if( angle >= 180 && angle < 270 )
         {
          Move_Left( table );
         }
     else if( angle >= 270 && angle < 355 )
         {
          Move_Left( table );
          if( dist > 5 )
              Move_Forward(table );
         }
     else 
         {
          Move_Forward( table );
         }
     
    } /* End of Move_Towards_2D */


void Move_Towards_3D( short horizontal_angle, short vertical_angle, float dist, unsigned char *table, float accelerate_threshold, float decelerate_threshold )
    {
     if( horizontal_angle > 5 && horizontal_angle < 90 )
         { 
          Move_Right( table );
         }
     else if( horizontal_angle >= 90 && horizontal_angle < 180 )
         {
          Move_Left( table );
         }
     else if( horizontal_angle >= 185 && horizontal_angle < 270 )
         {
          Move_Right( table );
         }
     else if( horizontal_angle >=270 && horizontal_angle < 355 )
         {
          Move_Left( table );
         }


     if( vertical_angle > 5 && vertical_angle < 90 )
         { 
          Move_Back( table );
         }
     else if( vertical_angle >= 90 && vertical_angle < 180 )
         {
          Move_Back( table );
         }
     else if( vertical_angle >= 185 && vertical_angle < 270 )
         {
          Move_Forward( table );
         }
     else if( vertical_angle >=270 && vertical_angle < 355 )
         {
          Move_Forward( table );
         }

     
     if( ( vertical_angle <= 5 || vertical_angle >= 355) &&
         ( horizontal_angle <= 5 || horizontal_angle >= 355) )
         {    
          if( dist > accelerate_threshold ) /* 2 */
              Up_Throttle( table );
         } 

     /*
     if( ( vertical_angle >= 90 && vertical_angle <= 270 ) &&
         ( horizontal_angle >= 90 || horizontal_angle >= 270) )
         {
          Down_Throttle( table );
         }
     */

     /*
     else
         {
          Down_Throttle( table );
         }
     */

     if( dist < decelerate_threshold ) /* 0.5 */
         Down_Throttle( table );

    
    } /* End of Move_Towards_3D */


/* The difference between this one and move_towards_3d is that this one accellerates forward no
   matter what */
void New_Move_Towards_3D( short horizontal_angle, short vertical_angle, float dist, unsigned char *table )
    {
     if( horizontal_angle > 5 && horizontal_angle < 90 )
         { 
          Move_Right( table );
         }
     else if( horizontal_angle >= 90 && horizontal_angle < 180 )
         {
          Move_Left( table );
         }
     else if( horizontal_angle >= 185 && horizontal_angle < 270 )
         {
          Move_Right( table );
         }
     else if( horizontal_angle >=270 && horizontal_angle < 355 )
         {
          Move_Left( table );
         }


     if( vertical_angle > 5 && vertical_angle < 90 )
         { 
          Move_Back( table );
         }
     else if( vertical_angle >= 90 && vertical_angle < 180 )
         {
          Move_Back( table );
         }
     else if( vertical_angle >= 185 && vertical_angle < 270 )
         {
          Move_Forward( table );
         }
     else if( vertical_angle >=270 && vertical_angle < 355 )
         {
          Move_Forward( table );
         }
     
     
     if( dist > 5 )
              {
               Up_Throttle( table );
              }
     else if( (vertical_angle <= 5 || ( horizontal_angle >= 355) || horizontal_angle <= 5 || vertical_angle >= 355)  )
         {    
          if( dist > 1 )
              Up_Throttle( table );
          else if( dist < 0.5 )
              Down_Throttle( table );
         } 

    
    } /* End of New_Move_Towards_3D */


void Sidestep_Towards_3D( short horizontal_angle, short vertical_angle, float dist, unsigned char *table )
    {

     if( vertical_angle < 95 && vertical_angle > 85 )
         {
          Sidestep_Up( table ); /* This is like moving forward */
         }
     else if( vertical_angle >= 0 && vertical_angle <= 85 )
         {
          Move_Forward( table );
         }
     else if( vertical_angle <= 180 && vertical_angle >= 95 )
         {
          Move_Back( table );
         }
     else if( vertical_angle < 275 && vertical_angle > 265 )
         {
          Sidestep_Down( table );
         }
     else if( vertical_angle >= 275 && vertical_angle <= 360 )
         {
          Move_Back( table );
         }
     else if( vertical_angle <= 265 && vertical_angle > 180 )
         {
          Move_Forward( table );
         }
     if( horizontal_angle > 5 && horizontal_angle < 90 )
         { 
          Move_Right( table );
         }
     else if( horizontal_angle >= 90 && horizontal_angle < 180 )
         {
          Move_Left( table );
         }
     else if( horizontal_angle >= 185 && horizontal_angle < 270 )
         {
          Move_Right( table );
         }
     else if( horizontal_angle >=270 && horizontal_angle < 355 )
         {
          Move_Left( table );
         }

     /*
     if( (vertical_angle <= 5 || ( horizontal_angle >= 355) || horizontal_angle <= 5 || vertical_angle >= 355)  )
         {    
          if( dist > 2 )
              Up_Throttle( table );
          else if( dist < 0.5 )
              Down_Throttle( table );
         } 
     */
    
    } /* End of Sidestep_Towards_3D */



/* Move towards without rotating */
void Sidestep_Towards_2D( short angle, float dist, unsigned char *table )
    {

     
     if( angle > 5 && angle < 90 )
         {
          Sidestep_Right( table );
          Move_Forward( table );
         }
     else if( angle >= 90 && angle < 180 )
         {
          Sidestep_Right( table );
          Move_Back( table );
         }
     else if( angle < 355 && angle > 270 )
         {
          Sidestep_Left( table );
          Move_Forward( table );
         }
     else if( angle <= 270 && angle >= 180 )
         {
          Sidestep_Left( table );
          Move_Back( table );
         }
     else 
         {
          Move_Forward( table );
         }

    }  /* End of Sidestep_Towards_2D */

                            
                            
/* Move away without rotating */
void Sidestep_Away_2D( short angle, float dist, unsigned char *table )
    {
               
     if( angle > 5 && angle < 90 )
         {
          Sidestep_Left( table );
          Move_Back( table );
         }
     else if( angle >= 90 && angle < 180 )
         {
          Sidestep_Left( table );
          Move_Forward( table );
         }
     else if( angle < 355 && angle > 270 )
         {
          Sidestep_Right( table );
          Move_Back( table );
         }
     else if( angle <= 270 && angle >= 180 )
         {
          Sidestep_Right( table );
          Move_Forward( table );
         }
     else 
         {
          Move_Back( table );
         }

    }  /* End of Sidestep_Away_2D */


                            
 /* Aim the guns at the enemy, if moving, keep on moving */
void Aim_Gun_2D( WorldStuff *world_stuff, short angle, float dist, int index )
    {
     boolean ok = TRUE;
     int i;               
     short angle_two;
     float dist_two;
     unsigned char *table;
     team_type my_team;
     
     
     table = world_stuff->player_array[index].table; 
     my_team = world_stuff->player_array[index].team;
    
     /* Loop through the vehicles and make sure none of ours are in the way */
     for( i = 0; i < 6; i++ )
         {
          if( world_stuff->player_array[i].tank.alive && i != index && 
              world_stuff->player_array[i].team == my_team )
              {
               dist_two  = player_events[index].angles.tank_dist_2D[i];
               angle_two = player_events[index].angles.tank_angle_2D[i];
               
               /* If one of this players team is in the way, don't shoot */
               if( (angle_two < 10 || angle_two > 350) && dist_two < dist )
                   ok = FALSE;
               
              }      
         } /* End for */     
              
     if( angle < 10 || angle > 350 )
         {
          if( ok && dist < 8 )
              Fire_Gun( table ); 
         }
     else if( angle > 5 && angle < 90 )
         {
          Move_Right( table );
         }
     else if( angle >= 90 && angle < 180 )
         {
          Move_Right( table );
         }
     else if( angle < 355 && angle > 270 )
         {
          Move_Left( table );
         }
     else if( angle <= 270 && angle >= 180 )
         {
          Move_Left( table );
         }


    } /* End of Aim_Gun_2D */


    
                            
void Aim_Gun_3D( WorldStuff *world_stuff, int index )
    {
     short horizontal_angle, vertical_angle;
     player_events_type *events;
     unsigned char *table;
     short current_enemy;
     
     
     events = &player_events[index];
     table = world_stuff->player_array[index].table;
     current_enemy = events->memory.current_enemy;
     

     if( current_enemy >= 0 && current_enemy <= 5 )
         {
          horizontal_angle = events->angles.tank_angle_3D[current_enemy].horizontal_angle;
          vertical_angle   = events->angles.tank_angle_3D[current_enemy].vertical_angle;
         }
     else /* current_enemy == 6 || current_enemy == 7  */
         {
          horizontal_angle = player_events[index].angles.enemy_radar_angle_3D.horizontal_angle;
          vertical_angle = player_events[index].angles.enemy_radar_angle_3D.vertical_angle;
         }


     if( horizontal_angle > 5 && horizontal_angle < 90 )
         { 
          Move_Right( table );
         }
     else if( horizontal_angle >= 90 && horizontal_angle < 180 )
         {
          Move_Left( table );
         }
     else if( horizontal_angle >= 185 && horizontal_angle < 270 )
         {
          Move_Right( table );
         }
     else if( horizontal_angle >=270 && horizontal_angle < 355 )
         {
          Move_Left( table );
         }


     if( vertical_angle > 5 && vertical_angle < 90 )
         { 
          Move_Back( table );
         }
     else if( vertical_angle >= 90 && vertical_angle < 180 )
         {
          Move_Back( table );
         }
     else if( vertical_angle >= 185 && vertical_angle < 270 )
         {
          Move_Forward( table );
         }
     else if( vertical_angle >=270 && vertical_angle < 355 )
         {
          Move_Forward( table );
         }


     if( world_stuff->player_array[index].character.skill_level == 3 )
         {
          /* Fine 3d aiming */
          if( vertical_angle <= 5 && vertical_angle > 1 )
              Sidestep_Up( table );
          if( vertical_angle >= 355 && vertical_angle < 360 )
              Sidestep_Down( table );

          if( horizontal_angle <= 5 && horizontal_angle > 1 )
              Sidestep_Right( table );
          if( horizontal_angle >= 355 && horizontal_angle < 360 )
              Sidestep_Left( table );
         }
         
      
     if( (horizontal_angle < 10 || horizontal_angle > 350) && (vertical_angle < 10 || vertical_angle > 350) )
         {
          Fire_Gun( table );
         }
     Missiles( world_stuff, index );
                                      
    } /* End of Aim_Gun_3D */
    
                            
/* Check and see if we can hit with missiles...if we can...
   shoot one */
void Missiles( WorldStuff *world_stuff, int index )
    {
     player_events_type *events;
     float dist;
     short vertical_angle, horizontal_angle;
     Player *player;
     short current_enemy;


     
     player = &world_stuff->player_array[index];
     events = &player_events[index];
     current_enemy = events->memory.current_enemy;




     if( (player->character.skill_level >= 2 && events->missiles_stored < 2) &&
         (player->tank.vtype != Wasp || player->tank.vtype != Spider) )
         return; /* Save our last missile for special weapons if this vehicle needs it */
        
     if( events->missiles_locked )
         {
          
          if( !events->missiles_empty )
              {
               dist             = Point_Distance_3D( player->tank.target, player->tank.orient.position );
               vertical_angle   = Vertical_Point_Angle_3D( player->tank.target, &player->tank.orient );
               horizontal_angle = Horizontal_Point_Angle_3D( player->tank.target, &player->tank.orient );
          
          
               if( Is_Visible_3D( player->tank.target, player->tank.orient.position, &world_stuff->pylons ) )
                   if( horizontal_angle < 20 || horizontal_angle > 340 )
                       if( vertical_angle < 20 || vertical_angle > 340 )
                           Fire_Missile( player->table );
                  
              } /* End if not empty */
         
         } /* End if missiles locked */
     else if( !events->my_radar_alive )
         {

          if( current_enemy >= 0 && current_enemy <= 5 )
              {
               dist             = events->angles.tank_dist_3D[current_enemy];
               horizontal_angle = events->angles.tank_angle_3D[current_enemy].horizontal_angle;
               vertical_angle   = events->angles.tank_angle_3D[current_enemy].vertical_angle;
              }
          else /* current_enemy == 6 || current_enemy == 7  */
             {
              dist             = events->angles.enemy_radar_dist_3D;
              horizontal_angle = events->angles.enemy_radar_angle_3D.horizontal_angle;
              vertical_angle   = events->angles.enemy_radar_angle_3D.vertical_angle;
             }

          if( horizontal_angle < 5 || horizontal_angle > 355 )
              if( vertical_angle < 5 || vertical_angle > 355 )
                  if( dist < 5 )
                      Fire_Missile( player->table );

         } /* End if my radars dead */


    } /* End of missiles */


void Move_Towards_Point_3D( Player player[], int index, Float_Point dest_point )
    {
     float dist;
     short horizontal_angle, vertical_angle;
     unsigned char *table;

     table = player[index].table;


     dist = Point_Distance_3D( player[0].tank.orient.position, player[3].tank.orient.position );

     horizontal_angle = Horizontal_Point_Angle_3D( dest_point, &player[index].tank.orient );
     vertical_angle   = Vertical_Point_Angle_3D( dest_point, &player[index].tank.orient );

    
     Move_Towards_3D( horizontal_angle, vertical_angle, dist, table, 2, 0.5 );
    } /* End of Move_Towards_Point_3D */




/* Move towards a point in 2-D...used for surface mode stuff */
void Move_Towards_Point_2D( Player player[], int index, Float_Point2d dest_point )
    {
     short angle;
     float dist;
     unsigned char *table;
     float *point;
     Float_Point2d point_2d;


     /* Translate tanks position to 2-d */
     point       = player[index].tank.orient.position;
     Cylinder_x_y( point, point_2d );

     /* Reference pointer to input table */
     table = player[index].table;
     
     /* Store the distance and angle to the destination point */
     angle = Point_Angle_2D( dest_point, &player[index].tank.orient );
     dist  = Point_Distance_2D( point_2d, dest_point );    
     
     Move_Towards_2D( angle, dist, table );


    }  /* End of Move_Towards_Point_2D */

/* Rotate clockwise around something */
void Rotate_Around_Clockwise( short angle, float dist, unsigned char *table )
    {


     if( angle >= 0 && angle < 45 ) 
         {
          Move_Forward( table );
          Sidestep_Left( table );
         }
     if( angle >= 45 && angle < 90 )
         {
          Move_Forward( table );
         }
     else if( angle >= 90 && angle < 135 )
         {
          Move_Forward( table );
          Sidestep_Right( table );
         }
     else if( angle >= 135 && angle < 180 )
         {
          Sidestep_Right( table );
         } 
     else if( angle >= 180 && angle < 225 )   
         {
          Sidestep_Right( table );
          Move_Back( table );
         }
     else if( angle >= 225 && angle <= 270 )
         {
          Move_Back( table );
         }
     else if( angle >= 270 && angle < 315 )
         {
          Sidestep_Left( table );
          Move_Back( table );
         }
     else if( angle >= 315 && angle <= 360 )
         {
          Sidestep_Left( table );
         }

    } /* End Rotate_Around_Clockwise */


/* Rotate counterclockwise around something */
void Rotate_Around_Counterclockwise( short angle, float dist, unsigned char *table )
    {

     if( angle >= 0 && angle < 45 ) 
         {
          Sidestep_Right( table );
         }
     if( angle >= 45 && angle < 90 )
         {
          Move_Back( table );
          Sidestep_Right( table );
         }
     else if( angle >= 90 && angle < 135 )
         {
          Move_Back( table );
         }
     else if( angle >= 135 && angle < 180 )
         {
          Sidestep_Left( table );
          Move_Back( table );
         } 
     else if( angle >= 180 && angle < 225 )   
         {
          Sidestep_Left( table );
          Move_Forward( table );
         }
     else if( angle >= 225 && angle <= 270 )
         {
          Move_Forward( table );
          Sidestep_Left( table );
         }
     else if( angle >= 270 && angle < 315 )
         {
          Move_Forward( table );
         }
     else if( angle >= 315 && angle <= 360 )
         {
          Sidestep_Right( table );
          Move_Forward( table );
         }

    } /* End Rotate_Around_Counterclockwise */




void Projectile_Special_Weapons( WorldStuff *world_stuff, int index )
    {
     float dist;
     short horizontal_angle, vertical_angle;
     short current_enemy;
     unsigned char *table;


     table = world_stuff->player_array[index].table;
     current_enemy = player_events[index].memory.current_enemy;
     

     if( current_enemy > 5 ) /* If we're aiming at a radar base */
         return; /* Scramblers and such dont work on radar bases */


     dist             = player_events[index].angles.tank_dist_3D[current_enemy];
     horizontal_angle = player_events[index].angles.tank_angle_3D[current_enemy].horizontal_angle;
     vertical_angle   = player_events[index].angles.tank_angle_3D[current_enemy].vertical_angle;

     if( world_stuff->player_array[index].tank.vtype == Mosquito )
         {
          if( world_stuff->player_array[current_enemy].tank.traitor_active )
              return; /* Dont re hit traitor */
         }
     else if( world_stuff->player_array[index].tank.vtype == Beetle )
         {
          if( world_stuff->player_array[current_enemy].tank.controls_scrambled )
              return; /* Dont re hit already scrambled enemy */
         }
     else
         return; /* No other vehicles have projectile special weapons */


     if( horizontal_angle < 10 || horizontal_angle > 350 )
         if( vertical_angle < 10 || vertical_angle > 350 )
             if( dist < 5 )
                 if( !player_events[index].missiles_empty )
                         {
                          Special_Weapon( table );
                         }

    } /* End of Projectile_Special_Weapons */


void Decoy( WorldStuff *world_stuff, int index )
    {
     player_events_type *events;

     events = &player_events[index];

     if( !events->missiles_empty )
         if( Decoys_Active( world_stuff, index ) < 6 )
             Special_Weapon( world_stuff->player_array[index].table );

    } /* End of Decoy() */


void Cloak_Override( WorldStuff *world_stuff, int index )
    {
     unsigned char *table;
     player_events_type *events;


     table  = world_stuff->player_array[index].table;
     events = &player_events[index];


     if( world_stuff->player_array[index].character.state == EVADE )
         {
          if( !world_stuff->player_array[index].tank.cloaking_active && !events->missiles_empty )
              Special_Weapon( table );
         }
     else if( world_stuff->player_array[index].tank.cloaking_active )
         {
          if( world_stuff->player_array[index].character.state == ATTACK ||
              (world_stuff->player_array[index].character.state == KILL_RADAR_BASE &&
               events->angles.enemy_radar_dist_3D < 3) )
              Special_Weapon( table ); /* If we are not evading or getting energy dont cloak */
         }

     if( world_stuff->player_array[index].tank.cloaking_active ||
         (!world_stuff->player_array[index].tank.cloaking_active && table[INDEX_SPECIAL_WEAPON]) )
         {
          table[INDEX_FIRE_GUN] = FALSE;
          table[INDEX_FIRE_MISSILE] = FALSE;
         }

     if( events->missiles_stored < 3 )
         table[INDEX_FIRE_MISSILE] = FALSE;    

    } /* End of Cloak_Override() */


void Mines( WorldStuff *world_stuff, int index )
    {
     player_events_type *events;
     unsigned char *table;
     float dist;
     short current_enemy;
     boolean drop_mine = FALSE; 
     team_type my_team;
     short tanks_near = 0;
     int i;



     events = &player_events[index];
     table  = world_stuff->player_array[index].table;
     current_enemy = events->memory.current_enemy;
     my_team = world_stuff->player_array[index].tank.team;



     if( world_stuff->player_array[index].character.state == EVADE )
         {
          if( current_enemy == 6 || current_enemy == 7 )
              ; /* Don't drop mines to hurt radar */
          else
              {
               dist = events->angles.tank_dist_3D[current_enemy];
               if( dist < 5 )
                   drop_mine = TRUE;
              }

         }


     /* Loop through the tanks...if enough enemies are near drop a mine */
     for( i = 0; i < 6; i++ )
         {
          dist = events->angles.tank_dist_3D[i];

          if( my_team != world_stuff->player_array[i].tank.team &&
              dist < 5 )
              tanks_near++;
              
         }


     if( tanks_near >= (events->enemies_alive - 1) )
         {
          drop_mine = TRUE;
         }


     if( !events->missiles_empty && drop_mine )
         {
          Special_Weapon( table );
         }

    } /* End of Mines() */


void Stuck_Override( WorldStuff *world_stuff, int index )
    {
     unsigned char *table;



     table = world_stuff->player_array[index].table;


     if( world_stuff->player_array[index].tank.vehicle_mode == Surface )
         {
          Move_Forward( table );
         }
     else
         {
          Up_Throttle( table );
         }

     player_events[index].memory.stuck_frames--;
     if( player_events[index].memory.stuck_frames <= 0 )
         player_events[index].memory.stuck = FALSE;


    } /* End of Stuck_Override() */

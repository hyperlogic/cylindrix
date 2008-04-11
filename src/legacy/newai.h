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


#ifndef AI_H

#define AI_H

#include "types.h"

typedef struct
    {
     short vertical_angle;
     short horizontal_angle;
    } 3D_angle_type;


typedef enum { CHASE, EVADE, ATTACK, ENEMY_SEARCH,
               ENERGY_SEARCH, PANIC, BEZERK, HIDE,
               GROUP, GET_PYLONS, PROTECT,
               KILL_RADAR_BASE, PROTECT_RADAR_BASE } states;


typedef struct
    {
     Float_Point    destination;        /* This is the ai's current destination */
     Float_Point    last_position[6];   /* This is the last known position of all vehicles */
     
     Float_Point    my_positions[100];  /* Where this vehicle has been recently */
     short          my_position_index;  /* Current index into my_position */
     
     short          nearest_enemy;      /* Nearest enemy to this vehicle */
    } memory;


typedef struct
    {
     short 2D_bullet_angle[200];        /* 2-D Angle of each bullet from the player */
     short 2D_bullet_facing_angle[200]; /* 2-D Angle each bullet is facing relative to player */
     float 2D_bullet_dist[200];         /* 2-D Distance of each bullet from the player */
     short 2D_tank_angle[6];            /* 2-D Angle of each tank from the player */
     short 2D_tank_dist[6];             /* 2-D Distance of each tank from the player */
     Float_Point2d 2D_tank_location[6]; /* 2-D Location of each tank */
     short 2D_pylon_angle[100];
     float 2D_pylon_dist[100];
     Float_Point2d 2D_pylon_pos;

     3D_angle_type 3D_bullet_angle[200];        /* 3-D Angle of each bullet from the player */
     3D_angle_type 3D_bullet_facing_angle[200]; /* 3-D Angle each bullet is facing relative to player */
     float_3D bullet_dist[200];                 /* 3-D Distance of each bullet from the player */
     3D_angle_type 3D_tank_angle[6];            /* 3-D Angle of each tank from the player */
     float 3D_tank_dist[6];                     /* 3-D Distance of each tank from the player */
     Float_Point 3D_tank_location[6];                /* 3-D Location of each tank */
     
     3D_angle_type 3D_pylon_angle;
     float 3D_pylon_dist;
     Float_Point 3D_pylon_pos;

     short num_bullets;       /* Number of bullets on the screen */
     short num_pylons;

    } angles;


typedef struct
    {
     
     boolean     enemy_visible;        /* Is the enemy visible? */
     boolean     enemy_in_range;       /* Is the enemy in firing range? */
     boolean     enemy_radar_alive;
     short       enemy_D;              /* Enemies damage power */ 
     short       enemies_alive;        /* How many enemies are alive? 0-3 */
     short       enemy_hitpoints[3];   /* Hitpoints of each enemy */
     boolean     enemies_in_air[3];    /* Which enemies are in the air? */
     
     boolean     energy_visible;       /* Is an energy power up visible? */
     boolean     bullet_hit;           /* Did we take damage this frame? */
     boolean     hitpoints_low;        /* Are our hit points low? */
     boolean     hitpoints_high;       /* Are our hit points high? */
     boolean     missiles_empty;       /* Are our guns empty? */
    
     float       energy_distance;      /* Our distance from the energy square */
     Float_Point energy_position;      /* 3-D position of the energy square */

     short       friends_alive;        /* How many teammates are alive? */
     short       friends_hitpoints[3]; /* Hitpoints of our friends */
     boolean     friends_in_air[3];    /* How many teammates are in the air? */
     short       my_D;                 /* Our damage power */
     boolean     my_radar_alive;       /* Is my radar alive? */
     boolean     in_air;               /* Am i in the air or on the ground */
     short       num_missiles_ready;   /* How many missiles do I have? */
     
    } player_events;

typedef struct
    {
     char            name[40];
     char            filename[15]; /* Name of the pcx file */
     states          state;
     
     unsigned char   passive_aggressive;     /* 1-100...1-50 is passive 51-100 is aggressive */
     unsigned char   bravery_cowardice;      /* 1-100...same scheme */
     unsigned char   aerial_ground;          /* 1-100...same scheme */
     unsigned char   obedience_disobedience; /* 1-100...same scheme */

     unsigned char   strategy_skill;         /* 1-5  */
     
     unsigned char   air_skill;              /* 1-5  air and ground are  */ 
     unsigned char   ground_skill;           /* 1-5  motor skills...     */
    } character_type;


#endif


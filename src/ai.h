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

/* #include <conio.h> */ /* Need getch() */
#include "types.h"
#include "ai_util.h"
#include "motor.h"
#include "ai_move.h"
#include "events.h"
#include "do_state.h"
#include "path.h"
#include "commands.h"


/* Actual dimensions of the cylinder */
#define CYLINDER_WIDTH  63.0f
#define CYLINDER_LENGTH 60.0f 
#define CYLINDER_RADIUS 10.0f

#define MAX_DESTINATIONS 70 /* Maximum number of destinations in one path */

/* Dimensions of the path grids */
#define TUBE_COLUMNS     30 
#define TUBE_ROWS        29 
#define TUBE_HEIGHT      10 

#define BULLET_THRESHOLD 20 /* Dumber of frames we will look back for bullet hits */
#define IN_AIR_THRESHOLD 30 /* Number of frames I look back to see if I should go in the air */

#define STUCK_THRESHOLD  10 /* Number of frames we will check for being stuck */
#define STUCK_FRAMES     10 /* Number of frames we move if we are stuck */


typedef struct
    {
     short vertical_angle;
     short horizontal_angle;
    } angle_type_3D;


/* Things the AI needs to keep track of over time to give coherence
   to its actions */
typedef struct
    {
     Float_Point    last_position[6];   /* This is the last known position of all vehicles */
     
     short          current_pylon;      /* Index in pylon array of current pylon we are trying to get */
     
     Float_Point    my_positions[STUCK_THRESHOLD];  /* Where this vehicle has been recently */
     short          my_position_index;  /* Current index into my_position */
     boolean        stuck;
     int            stuck_frames;       /* How long we wiggle if we're stuck */

     short          surface;            /* Zero means stay on surface, one means stay in air */
     short          current_enemy;      /* This is the current enemy that this guy is fighting */
     
     short          bullet_hit[BULLET_THRESHOLD];   /* Keep an array of bullet hits for the last hundred frames */
     short          bullet_owner[BULLET_THRESHOLD]; /* Keep an array of who shot us in the last hundred frames */
     short          bullet_index;             /* Our index in the bullet array */
     short          current_threat;           /* Who's been shootin' our ass? */
     short          num_bullet_hits;          /* Number of times we've been hit by bullets recently */
     short          recent_damage;            /* How much damage we have taken recently */

     /* This is the ai's current 2D array of destinations for use on surface*/
     Float_Point2d  path_2D[MAX_DESTINATIONS];
     short          num_paths_2D;
     short          path_index_2D;
     Float_Point2d  final_destination_2D;
     
     /* This is the ai's current 3D array of destinations for use in air */
     Float_Point    path_3D[MAX_DESTINATIONS];
     short          num_paths_3D;
     short          path_index_3D;
     Float_Point    final_destination_3D; 
     
     boolean        following_path; /* Is the AI using paths right now? */

     boolean        command_active; 
     short          command_frames_left;
     command_type   command;        
     state_type     command_state; /* State_type the command should put us in */

     boolean        in_air_array[30]; /* To keep track of mode switching */
     int            in_air_index;
    } memory_type;


/* Position of everything in the world, angle and distance of everything
   relative to the player.  */
typedef struct
    {
     /* 2-D stuff */
     
     short bullet_angle_2D[200];        /* 2-D Angle of each bullet from the player */
     short bullet_facing_angle_2D[200]; /* 2-D Angle each bullet is facing relative to player */
     float bullet_dist_2D[200];         /* 2-D Distance of each bullet from the player */
     
     short tank_angle_2D[6];            /* 2-D Angle of each tank from the player */
     short tank_dist_2D[6];             /* 2-D Distance of each tank from the player */
     Float_Point2d tank_pos_2D[6];      /* 2-D position each tank */
     
     short pylon_angle_2D[100];         /* 2-D angle of each pylon from the player */
     float pylon_dist_2D[100];          /* 2-D distance of each pylon from the player */
     Float_Point2d pylon_pos_2D[100];   /* 2-D position of the pylons */

     short energy_angle_2D;             /* 2-D angle of the energy square from the player */
     float energy_dist_2D;              /* 2-D distance of the energy square from the player */
     Float_Point2d energy_pos_2D;       /* 2-D position of the energy square */
     
     short enemy_radar_angle_2D;        /* 2-D angle of the enemy radar from the player */
     float enemy_radar_dist_2D;         /* 2-D distance of the enemy radar from the player */
     Float_Point2d enemy_radar_pos_2D;  /* 2-D position of the enemy radar */
     
     
     /* 3-D stuff */

     angle_type_3D bullet_angle_3D[200];        /* 3-D Angle of each bullet from the player */
     angle_type_3D bullet_facing_angle_3D[200]; /* 3-D Angle each bullet is facing relative to player */
     float bullet_dist_3D[200];                 /* 3-D Distance of each bullet from the player */
     
     angle_type_3D tank_angle_3D[6];    /* 3-D Angle of each tank from the player */
     float tank_dist_3D[6];             /* 3-D Distance of each tank from the player */
     Float_Point tank_pos_3D[6];        /* 3-D Location of each tank */
     
     angle_type_3D pylon_angle_3D[100]; /* 3-D angle of each pylon from the player */
     float pylon_dist_3D[100];          /* 3-D distance of each pylon from the player */
     Float_Point pylon_pos_3D[100];     /* 3-D position of the pylons */
     
     angle_type_3D energy_angle_3D;     /* 3-D angle of the energy square from the player */
     float energy_dist_3D;              /* 3-D distance of the energy square from the player */
     Float_Point energy_pos_3D;         /* 3-D position of the energy square */
     
     angle_type_3D enemy_radar_angle_3D; /* 3-D angle of the enemy radar from the player */
     float enemy_radar_dist_3D;          /* 3-D distance of the enemy radar from the player */
     Float_Point enemy_radar_pos_3D;     /* 3-D position of the enemy radar*/
     
     /* General stuff */
     
     team_type pylon_owner[100]; /* Owner of each pylon */
     short num_bullets;          /* Number of bullets on the screen */
     short num_pylons;           /* Number of pylons in the game */

    } angles_type;


/* player_events_type is the AI's connection to the game world.
   Everything that it knows about the game is stored here. */
typedef struct
    {
     /* Enemy team status */
     
     boolean     enemy_radar_alive;
     short       enemy_D;              /* Enemies damage power */ 
     short       enemies_alive;        /* How many enemies are alive? 0-3 */
     short       enemy_leader;         /* Which one is the leader? */
     /* My status */
     boolean     bullet_hit;           /* Did we take damage this frame? */
     boolean     hitpoints_low;        /* Are our hit points low? */
     boolean     hitpoints_high;       /* Are our hit points high? */
     boolean     missiles_empty;       /* Are our guns empty? */
     short       missiles_stored;      /* How many missiles do I have? */
     boolean     missiles_locked;      /* Are our missiles locked on something? */
     boolean     in_air;               /* Am i in the air or on the ground */
     boolean     energy_visible;       /* Is an energy power up visible? */
     boolean     killed_someone;       /* Did I kill someone this frame? */
     short       my_leader;            /* Index of leader in player array */
     /* My teams status */
     
     short       friends_alive;        /* How many teammates are alive? */
     short       my_D;                 /* Our damage power */
     boolean     my_radar_alive;       /* Is my radar alive? */
     boolean     radar_hitpoints_low;  /* Is my radars hitpoints low? */
     
     boolean tank_visible[6];          /* Are the tanks visible to us? */
     boolean tank_in_air[6];           /* Are the tanks in the air? */
     boolean tank_locked[6];           /* Are our sights locked on the tanks? */
     boolean tank_in_range[6];         /* Are the tanks in range? */
     boolean tank_hitpoints[6];        
     
     angles_type angles;               /* Keep track of all the angles */
     memory_type memory;               /* Recent things that have happened */
    } player_events_type;


void Init_AI( WorldStuff *world_stuff );
void Load_AI( character_type *character, char *filename, int ai_number );
void AI_Control( WorldStuff *world_stuff, int vehicle_number );
void Clear_Ai( void );


#endif


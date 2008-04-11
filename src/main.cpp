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

#include "main.h"
#include "types.h"

#include "jonsb.h"     /* Stuff for deapth ququqauing of samples */
#include "util.h"      /* need exit_gracefully */
#include "tanks.h"     /* need Init_Tank & Load_Tank */
#include "ai.h"
#include "keys.h"
#include "object.h"
#include "pylon.h"
#include "energy.h"
#include "base.h"
#include "radar.h"
#include "prim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pylon.h"
#include "movement.h"
#include "base.h"
#include "project.h"
#include "explode.h"
#include "hud.h"
#include "text.h"
#include "ai.h"
#include "object.h"
#include "ai.h"
#include "ai_util.h"
#include "collide.h"
#include "stats.h"
#include "menu.h"
#include "stub.h"
#include <math.h>

#ifdef GLCYLINDRIX
#include "glcylindrix.h"
#endif

#define NO_LIVING_VEHICLES_FOUND 0
#define ARRIVED_AT_DESTINATION 1
#define STILL_TRANSPORTING 2

/* from omega.c */
//
//Johnm 9/5/2002 New boolean to specify that two people are playing in split screen mode
//From prim.c
extern int g_bRenderingSecondPlayer;
extern int g_bRenderingFirstPlayer;

#ifdef GLCYLINDRIX
// AJT: from omega.c, uses the g key to switch between gl rendering and the old software one.
extern boolean renderSoftwareBuffer;
#endif

extern int     level_warp;
extern boolean test_anim;
extern boolean show_readout;
extern boolean test_samples;
extern long exit_loop;  /* From omega.c */
extern long game_over;

extern boolean ceiling_on;
extern game_stats_type game_stats;

extern SYS_SOUNDHANDLE sample[MAX_WORLD_SAMPLES]; /* This should be in the world_sounds data structure */
extern SYS_SOUNDHANDLE computer_sample[MAX_COMPUTER_SAMPLES];
extern sb_mod_file *mod;
extern short num_playing; /* From jonsb.c */

extern game_configuration_type game_configuration;

extern player_events_type player_events[6];

extern level_type level; /* holds the values we load from the level file */

extern WorldStuff world_stuff;

extern MenuStuff menu_stuff;

extern int profile;

static string debug_string[6];

extern string temp_str2[6];  /* ditto */

extern char frame_rate_str[80]; /* frame rate to be blitted on the screen */

extern int temp_int;

Float_Point cylinder_center = { 0.0, 0.0, 0.0 }; /* used by anti_missile_update */

extern long keyboard_installed;
extern long timer_installed;
extern long ignore_sound_card;  /* True if we want to completely ignore the sound card */


long destination_vehicle_index;


void print_p_mem( char *str )
{
//    _go32_dpmi_meminfo info;      /* used to get memory information */

//    _go32_dpmi_get_free_memory_information( &info );

//    printf("%s, p_mem = %lu\n", str, _go32_dpmi_remaining_physical_memory() );
}


void print_all_input_tables( Player p[], long frame_counter )
{
    printf("player 0, frame %ld\n", frame_counter );
    print_player_info( p, frame_counter );

    printf("player 3, frame %ld\n", frame_counter );
    print_player_info( p, frame_counter );

    printf("\n");
}

void cylindrix_init_print( char str[] )
{
    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    string_blit( "CYLINDRIX INITIALIZATION", 30, 80,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, 2 );

    micro_string_blit( str, 5, 185,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 2 );

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}


void display_next_opponent()
{
    pcx_picture *pcx = NULL;
    long anchor_x = 0, anchor_y = 0;
    palette_type menu_palette;
    RGB_color *character_palette = NULL;
    long i;

    DB_Clear_Screen();    
    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

    Pop_Buffer( menu_stuff.wingman_menu_background.buffer );

    if( (game_configuration.blue0_ai == 8) &&
        (game_configuration.blue1_ai == 7) &&
        (game_configuration.blue2_ai == 2) ) {

        character_palette = menu_stuff.human_3.palette;
        pcx = &menu_stuff.human_3;
        anchor_x = 40;
        anchor_y = 23;

        string_blit( "HUMANS", 120, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 23) &&
             (game_configuration.blue1_ai == 24) &&
             (game_configuration.blue2_ai == 22) ) {

        character_palette = menu_stuff.overlords.palette;
        pcx = &menu_stuff.overlords;
        anchor_x = 36;
        anchor_y = 34;

        string_blit( "OVERLORDS", 100, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 25) &&
             (game_configuration.blue1_ai == 27) &&
             (game_configuration.blue2_ai == 26) ) {

        character_palette = menu_stuff.pharoahs.palette;
        pcx = &menu_stuff.pharoahs;
        anchor_x = 38;
        anchor_y = 32;

        string_blit( "PHAROAHS", 115, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 18) &&
             (game_configuration.blue1_ai == 17) &&
             (game_configuration.blue2_ai == 16) ) {

        character_palette = menu_stuff.scavengers.palette;
        pcx = &menu_stuff.scavengers;
        anchor_x = 37;
        anchor_y = 25;

        string_blit( "SCAVENGERS", 100, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 14) &&
             (game_configuration.blue1_ai == 13) &&
             (game_configuration.blue2_ai == 15) ) {

        character_palette = menu_stuff.slar.palette;
        pcx = &menu_stuff.slar;
        anchor_x = 38;
        anchor_y = 22;

        string_blit( "SLAR", 140, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 19) &&
             (game_configuration.blue1_ai == 20) &&
             (game_configuration.blue2_ai == 21) ) {

        character_palette = menu_stuff.succubi.palette;
        pcx = &menu_stuff.succubi;
        anchor_x = 46;
        anchor_y = 23;

        string_blit( "SUCCUBI", 107, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 28) &&
             (game_configuration.blue1_ai == 29) &&
             (game_configuration.blue2_ai == 30) ) {

        character_palette = menu_stuff.watchers.palette;
        pcx = &menu_stuff.watchers;
        anchor_x = 45;
        anchor_y = 29;

        string_blit( "WATCHERS", 115, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 10) &&
             (game_configuration.blue1_ai == 11) &&
             (game_configuration.blue2_ai == 12) ) {

        character_palette = menu_stuff.bok.palette;
        pcx = &menu_stuff.bok;
        anchor_x = 39;
        anchor_y = 31;

        string_blit( "BOK", 140, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 34) &&
             (game_configuration.blue1_ai == 35) &&
             (game_configuration.blue2_ai == 36) ) {

        character_palette = menu_stuff.biomech.palette;
        pcx = &menu_stuff.biomech;
        anchor_x = 42;
        anchor_y = 32;

        string_blit( "BIOMECHANOIDS", 90, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 32) &&
             (game_configuration.blue1_ai == 33) &&
             (game_configuration.blue2_ai == 31) ) {

        character_palette = menu_stuff.sentry.palette;
        pcx = &menu_stuff.sentry;
        anchor_x = 38;
        anchor_y = 37;

        string_blit( "SENTRY", 120, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }

    /* the first 69 colors are for the background */

    for( i = 0; i < 69; i++ ) {
        menu_palette[i] = menu_stuff.wingman_menu_background.palette[i];
    }

    for( i = 69; i < 256; i++ ) {  /* the rest of the colors are from the character screen */
        menu_palette[i] = character_palette[i];
    }

    Enable_Palette( menu_palette );

    if( pcx != NULL ) {
        character_picture_blit( anchor_x, anchor_y, pcx );
    }

    string_blit( "NEXT OPPONENT", 86, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, 2 );

	// AJT: disabled this, screw waiting.
	/*
    micro_string_blit( "PRESS ANY KEY TO CONTINUE", 5, 185,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 2 );
	*/

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

	 // AJT: disabled this, screw waiting.
    /* wait for the user to hit a key */	
//    while( !Jon_Kbhit() );
}


void tournament_init_print( char str[] )
{
    pcx_picture *pcx = NULL;
    long anchor_x = 0, anchor_y = 0;

    Pop_Buffer( menu_stuff.wingman_menu_background.buffer );

    if( (game_configuration.blue0_ai == 8) &&
        (game_configuration.blue1_ai == 7) &&
        (game_configuration.blue2_ai == 2) ) {

        pcx = &menu_stuff.human_3;
        anchor_x = 40;
        anchor_y = 23;

        string_blit( "HUMANS", 120, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 23) &&
             (game_configuration.blue1_ai == 24) &&
             (game_configuration.blue2_ai == 22) ) {

        pcx = &menu_stuff.overlords;
        anchor_x = 36;
        anchor_y = 34;

        string_blit( "OVERLORDS", 100, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 25) &&
             (game_configuration.blue1_ai == 27) &&
             (game_configuration.blue2_ai == 26) ) {

        pcx = &menu_stuff.pharoahs;
        anchor_x = 38;
        anchor_y = 32;

        string_blit( "PHAROAHS", 115, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 18) &&
             (game_configuration.blue1_ai == 17) &&
             (game_configuration.blue2_ai == 16) ) {

        pcx = &menu_stuff.scavengers;
        anchor_x = 37;
        anchor_y = 25;

        string_blit( "SCAVENGERS", 100, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );

    }
    else if( (game_configuration.blue0_ai == 14) &&
             (game_configuration.blue1_ai == 13) &&
             (game_configuration.blue2_ai == 15) ) {

        pcx = &menu_stuff.slar;
        anchor_x = 38;
        anchor_y = 22;

        string_blit( "SLAR", 140, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );
    }
    else if( (game_configuration.blue0_ai == 19) &&
             (game_configuration.blue1_ai == 20) &&
             (game_configuration.blue2_ai == 21) ) {

        pcx = &menu_stuff.succubi;
        anchor_x = 46;
        anchor_y = 23;

        string_blit( "SUCCUBI", 107, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );
    }
    else if( (game_configuration.blue0_ai == 28) &&
             (game_configuration.blue1_ai == 29) &&
             (game_configuration.blue2_ai == 30) ) {

        pcx = &menu_stuff.watchers;
        anchor_x = 45;
        anchor_y = 29;

        string_blit( "WATCHERS", 115, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );
    }
    else if( (game_configuration.blue0_ai == 10) &&
             (game_configuration.blue1_ai == 11) &&
             (game_configuration.blue2_ai == 12) ) {

        pcx = &menu_stuff.bok;
        anchor_x = 39;
        anchor_y = 31;

        string_blit( "BOK", 140, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );
    }
    else if( (game_configuration.blue0_ai == 34) &&
             (game_configuration.blue1_ai == 35) &&
             (game_configuration.blue2_ai == 36) ) {

        pcx = &menu_stuff.biomech;
        anchor_x = 42;
        anchor_y = 32;

        string_blit( "BIOMECHANOIDS", 90, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );
    }
    else if( (game_configuration.blue0_ai == 32) &&
             (game_configuration.blue1_ai == 33) &&
             (game_configuration.blue2_ai == 31) ) {

        pcx = &menu_stuff.sentry;
        anchor_x = 38;
        anchor_y = 37;

        string_blit( "SENTRY", 120, 120,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, 2 );
    }

    if( pcx != NULL ) {
        character_picture_blit( anchor_x, anchor_y, pcx );
    }

    string_blit( "NEXT OPPONENT", 86, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, 2 );

    micro_string_blit( str, 5, 185,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 2 );

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}


/* Loads all sound effect samples, loads
   & plays mod file. Note: reaches out for the global game_configuration &
   sample. */


void init_sounds( void )
{
      sample[CAP_COLLISION]          = SYS_LoadSound( "wav_data/world/pcollide.wav" );
      sample[CYLINDER_COLLISION]     = SYS_LoadSound( "wav_data/world/pcollide.wav" );
      sample[BULLET_FIRE]            = SYS_LoadSound( "wav_data/world/laser.wav" );
      sample[BULLET_HIT]             = SYS_LoadSound( "wav_data/world/vhit.wav" );
      sample[BULLET_MISS]            = SYS_LoadSound( "wav_data/world/lasermis.wav" );
      sample[MISSILE_FIRE]           = SYS_LoadSound( "wav_data/world/missile.wav" );
      sample[MISSILE_HIT]            = SYS_LoadSound( "wav_data/world/explode.wav" );
      sample[MISSILE_MISS]           = SYS_LoadSound( "wav_data/world/explode.wav" );
      sample[VEHICLE_EXPLOSION]      = SYS_LoadSound( "wav_data/world/mexplode.wav" );
      sample[MODE_SWITCH_INTO_AIR]   = SYS_LoadSound( "wav_data/world/takeoff.wav" );
      sample[MODE_SWITCH_ONTO_GROUND]= SYS_LoadSound( "wav_data/world/landing.wav" );
      sample[PYLON_COLLISION]        = SYS_LoadSound( "wav_data/world/pcollide.wav" );
      sample[VEHICLE_COLLISION]      = SYS_LoadSound( "wav_data/world/vcollide.wav" );
      sample[ENERGY_RECHARGE]        = SYS_LoadSound( "wav_data/world/energysq.wav" );
      sample[PYLON_TURN_BLUE]        = SYS_LoadSound( "wav_data/world/bluepyl.wav" );
      sample[PYLON_TURN_RED]         = SYS_LoadSound( "wav_data/world/redpyl.wav" );

      sample[VEHICLE_MOTION] = 0;

      computer_sample[BLUE_TEAM_WINS]  = SYS_LoadSound( "wav_data/computer/bluvic.wav" );
      computer_sample[RED_TEAM_WINS]   = SYS_LoadSound( "wav_data/computer/redvic.wav" );
      computer_sample[TRANSFER]        = SYS_LoadSound( "wav_data/computer/vetrans.wav" );
      computer_sample[RADAR_DESTROYED] = SYS_LoadSound( "wav_data/computer/raddes.wav" );

      computer_sample[ATTACK_ENEMY]    = SYS_LoadSound( "wav_data/computer/attack.wav" );
      computer_sample[ATTACK_RADAR]    = SYS_LoadSound( "wav_data/computer/rbattack.wav" );
      computer_sample[GROUP_TOGETHER]  = SYS_LoadSound( "wav_data/computer/group.wav" );
      computer_sample[GRAB_PYLONS]     = SYS_LoadSound( "wav_data/computer/gpylons.wav" );
      computer_sample[DEFEND_RADAR]    = SYS_LoadSound( "wav_data/computer/rbdefend.wav" );
      computer_sample[CANCEL_ORDER]    = SYS_LoadSound( "wav_data/computer/cancel.wav" );

      computer_sample[CONTROLS_LOST]     = SYS_LoadSound( "wav_data/computer/nres.wav" );
      computer_sample[CONTROLS_REGAINED] = SYS_LoadSound( "wav_data/computer/sregain.wav" );
      computer_sample[ROUND_ONE]         = SYS_LoadSound( "wav_data/computer/rone.wav" );
      computer_sample[ROUND_TWO]         = SYS_LoadSound( "wav_data/computer/rtwo.wav" );
      computer_sample[FINAL_ROUND]       = SYS_LoadSound( "wav_data/computer/finalr.wav" );

/*
      if( computer_sample[RADAR_DESTROYED] == NULL ||
      computer_sample[TRANSFER] == NULL || computer_sample[RED_TEAM_WINS]  == NULL ||
      computer_sample[BLUE_TEAM_WINS]  == NULL || computer_sample[ATTACK_ENEMY] == NULL ||
      computer_sample[ATTACK_RADAR] == NULL || computer_sample[GROUP_TOGETHER] == NULL ||
      computer_sample[GRAB_PYLONS] == NULL || computer_sample[DEFEND_RADAR] == NULL ||
      computer_sample[CANCEL_ORDER] == NULL || computer_sample[CONTROLS_LOST] == NULL ||
      computer_sample[CONTROLS_REGAINED] == NULL || computer_sample[ROUND_ONE] == NULL ||
      computer_sample[ROUND_TWO] == NULL || computer_sample[FINAL_ROUND] == NULL )
          {
           SYS_Error("Bad computer .wav filename \n");
          }
*/
} /* End init_sounds */

/* initializes the 3 vehicles on the red_team. Note: uses data from the
   global level & game_configuration data structures. */

void init_red_team( Player *p )
{
    /* initialize player 0 */

    p[0].tank.vtype = (VehicleType)game_configuration.red0_vehicle;
    p[0].team = RED_TEAM;
    p[0].leader = TRUE;

    Load_Tank( &p[0].tank, (VehicleType)game_configuration.red0_vehicle );

    /* initialize tanks orientation with values from the level file */

    p[0].tank.orient.position[X] = level.vehicle_orientations[0].position[X];
    p[0].tank.orient.position[Y] = level.vehicle_orientations[0].position[Y];
    p[0].tank.orient.position[Z] = level.vehicle_orientations[0].position[Z];

    p[0].tank.orient.front[X] = level.vehicle_orientations[0].front[X];
    p[0].tank.orient.front[Y] = level.vehicle_orientations[0].front[Y];
    p[0].tank.orient.front[Z] = level.vehicle_orientations[0].front[Z];

    p[0].tank.orient.up[X] = level.vehicle_orientations[0].up[X];
    p[0].tank.orient.up[Y] = level.vehicle_orientations[0].up[Y];
    p[0].tank.orient.up[Z] = level.vehicle_orientations[0].up[Z];

    Init_Tank( &p[0].tank, (VehicleType)game_configuration.red0_vehicle, p[0].team );

    /* initialize player 1 */

    p[1].tank.vtype = (VehicleType)game_configuration.red1_vehicle;
    p[1].team = RED_TEAM;
    p[1].leader = FALSE;

    Load_Tank( &p[1].tank, (VehicleType)game_configuration.red1_vehicle );

    /* initialize tanks orientation (later to be loaded from .lvl file) */

    p[1].tank.orient.position[X] = level.vehicle_orientations[1].position[X];
    p[1].tank.orient.position[Y] = level.vehicle_orientations[1].position[Y];
    p[1].tank.orient.position[Z] = level.vehicle_orientations[1].position[Z];

    p[1].tank.orient.front[X] = level.vehicle_orientations[1].front[X];
    p[1].tank.orient.front[Y] = level.vehicle_orientations[1].front[Y];
    p[1].tank.orient.front[Z] = level.vehicle_orientations[1].front[Z];

    p[1].tank.orient.up[X] = level.vehicle_orientations[1].up[X];
    p[1].tank.orient.up[Y] = level.vehicle_orientations[1].up[Y];
    p[1].tank.orient.up[Z] = level.vehicle_orientations[1].up[Z];

    Init_Tank( &p[1].tank, (VehicleType)game_configuration.red1_vehicle, p[1].team );

    /* initalize player 2 */

    p[2].tank.vtype = (VehicleType)game_configuration.red2_vehicle;
    p[2].team = RED_TEAM;
    p[2].leader = FALSE;

    Load_Tank( &p[2].tank, (VehicleType)game_configuration.red2_vehicle );

    /* initialize tanks orientation (later to be loaded from .lvl file) */

    p[2].tank.orient.position[X] = level.vehicle_orientations[2].position[X];
    p[2].tank.orient.position[Y] = level.vehicle_orientations[2].position[Y];
    p[2].tank.orient.position[Z] = level.vehicle_orientations[2].position[Z];

    p[2].tank.orient.front[X] = level.vehicle_orientations[2].front[X];
    p[2].tank.orient.front[Y] = level.vehicle_orientations[2].front[Y];
    p[2].tank.orient.front[Z] = level.vehicle_orientations[2].front[Z];

    p[2].tank.orient.up[X] = level.vehicle_orientations[2].up[X];
    p[2].tank.orient.up[Y] = level.vehicle_orientations[2].up[Y];
    p[2].tank.orient.up[Z] = level.vehicle_orientations[2].up[Z];

    Init_Tank( &p[2].tank, (VehicleType)game_configuration.red2_vehicle, p[2].team );

    /*
    p[1].tank.alive = FALSE;
    p[2].tank.alive = FALSE;
    */

} /* end of init_red_team */


/* Initializes the 3 vehicles on the blue_team. Note: uses data from the
   global level & game_configuration data structures. */

void init_blue_team( Player *p )
{

    /* initalize player 3 */

    p[3].tank.vtype = (VehicleType)game_configuration.blue0_vehicle;
    p[3].team = BLUE_TEAM;
    p[3].leader = TRUE;

    Load_Tank( &p[3].tank, (VehicleType)game_configuration.blue0_vehicle );

    /* initialize tanks orientation (later to be loaded from .lvl file) */

    p[3].tank.orient.position[X] = level.vehicle_orientations[3].position[X];
    p[3].tank.orient.position[Y] = level.vehicle_orientations[3].position[Y];
    p[3].tank.orient.position[Z] = level.vehicle_orientations[3].position[Z];

    p[3].tank.orient.front[X] = level.vehicle_orientations[3].front[X];
    p[3].tank.orient.front[Y] = level.vehicle_orientations[3].front[Y];
    p[3].tank.orient.front[Z] = level.vehicle_orientations[3].front[Z];

    p[3].tank.orient.up[X] = level.vehicle_orientations[3].up[X];
    p[3].tank.orient.up[Y] = level.vehicle_orientations[3].up[Y];
    p[3].tank.orient.up[Z] = level.vehicle_orientations[3].up[Z];

    Init_Tank( &p[3].tank, (VehicleType)game_configuration.blue0_vehicle, p[3].team );

    /* initialize player 4 */

    p[4].tank.vtype = (VehicleType)game_configuration.blue1_vehicle;
    p[4].team = BLUE_TEAM;
    p[4].leader = TRUE;

    Load_Tank( &p[4].tank, (VehicleType)game_configuration.blue1_vehicle );

    /* initialize tanks orientation (later to be loaded from .lvl file) */

    p[4].tank.orient.position[X] = level.vehicle_orientations[4].position[X];
    p[4].tank.orient.position[Y] = level.vehicle_orientations[4].position[Y];
    p[4].tank.orient.position[Z] = level.vehicle_orientations[4].position[Z];

    p[4].tank.orient.front[X] = level.vehicle_orientations[4].front[X];
    p[4].tank.orient.front[Y] = level.vehicle_orientations[4].front[Y];
    p[4].tank.orient.front[Z] = level.vehicle_orientations[4].front[Z];

    p[4].tank.orient.up[X] = level.vehicle_orientations[4].up[X];
    p[4].tank.orient.up[Y] = level.vehicle_orientations[4].up[Y];
    p[4].tank.orient.up[Z] = level.vehicle_orientations[4].up[Z];

    Init_Tank( &p[4].tank, (VehicleType)game_configuration.blue1_vehicle, p[4].team );

    /* initalize player 5 */

    p[5].tank.vtype = (VehicleType)game_configuration.blue2_vehicle;
    p[5].team = BLUE_TEAM;
    p[5].leader = TRUE;

    Load_Tank( &p[5].tank, (VehicleType)game_configuration.blue2_vehicle );

    /* initialize tanks orientation (later to be loaded from .lvl file) */

    p[5].tank.orient.position[X] = level.vehicle_orientations[5].position[X];
    p[5].tank.orient.position[Y] = level.vehicle_orientations[5].position[Y];
    p[5].tank.orient.position[Z] = level.vehicle_orientations[5].position[Z];

    p[5].tank.orient.front[X] = level.vehicle_orientations[5].front[X];
    p[5].tank.orient.front[Y] = level.vehicle_orientations[5].front[Y];
    p[5].tank.orient.front[Z] = level.vehicle_orientations[5].front[Z];

    p[5].tank.orient.up[X] = level.vehicle_orientations[5].up[X];
    p[5].tank.orient.up[Y] = level.vehicle_orientations[5].up[Y];
    p[5].tank.orient.up[Z] = level.vehicle_orientations[5].up[Z];

    Init_Tank( &p[5].tank, (VehicleType)game_configuration.blue2_vehicle, p[5].team );

    /* p[3].tank.alive = FALSE; */
    /* p[4].tank.alive = FALSE; */
    /* p[5].tank.alive = FALSE; */

} /* end of init_blue_team */

/* calls various initilization functions */

void init_stuff()
{
    /* Initialize sound stuff */

    if( game_configuration.sound_on )
        {
         if( game_configuration.game_type == TournamentGame ) {
             tournament_init_print( "INIT_SOUNDS() ENGAGED" );
         }
         else {
             cylindrix_init_print( "INIT_SOUNDS() ENGAGED" );
         }

         init_sounds();
         /* Init_Jon_Samples(); */ /* init the deapth queing stuff */

    } /* End if sound_on */

} /* End of Init_Stuff() */


/* initalize the color_info data structure, which holds important info about
   the palette */

void init_color_info( ColorInfo *color_info )
{
    long i;

    /* copy level.color_info into color_info */

    for( i = 0; i < NUM_GRADIENTS; i++ ) {
        color_info->gradient[i].active = level.color_info.gradient[i].active;
        color_info->gradient[i].first = level.color_info.gradient[i].first;
        color_info->gradient[i].last = level.color_info.gradient[i].last;
        color_info->gradient[i].num_colors = level.color_info.gradient[i].num_colors;
    }

    color_info->size = level.color_info.size;

} /* End of init_color_info */


void Init_Menu_Sounds()
{
	menu_stuff.menu_sounds.enter_sound  = SYS_LoadSound( "wav_data/world/vhit.wav" );
	menu_stuff.menu_sounds.esc_sound    = SYS_LoadSound( "wav_data/world/vhit.wav" );
	menu_stuff.menu_sounds.toggle_sound = SYS_LoadSound( "wav_data/world/lasermis.wav" );
	menu_stuff.menu_sounds.error_sound  = SYS_LoadSound( "wav_data/world/pcollide.wav" );
}

void Free_Menu_Sounds()
{
     SYS_ReleaseSound( menu_stuff.menu_sounds.enter_sound );
     menu_stuff.menu_sounds.enter_sound = 0;
     SYS_ReleaseSound( menu_stuff.menu_sounds.esc_sound );
     menu_stuff.menu_sounds.esc_sound = 0;
     SYS_ReleaseSound( menu_stuff.menu_sounds.toggle_sound );
     menu_stuff.menu_sounds.toggle_sound = 0;
     SYS_ReleaseSound( menu_stuff.menu_sounds.error_sound );
     menu_stuff.menu_sounds.error_sound = 0;
}

void Init_Menu_Voices()
{
    menu_stuff.vehicle_name_sounds.beetle_sound     = SYS_LoadSound("wav_data/computer/beetle.wav" );
    menu_stuff.vehicle_name_sounds.wasp_sound       = SYS_LoadSound("wav_data/computer/wasp.wav" );
    menu_stuff.vehicle_name_sounds.roach_sound      = SYS_LoadSound("wav_data/computer/roach.wav" );
    menu_stuff.vehicle_name_sounds.dragonfly_sound  = SYS_LoadSound("wav_data/computer/dragon.wav" );
    menu_stuff.vehicle_name_sounds.spider_sound     = SYS_LoadSound("wav_data/computer/spider.wav" );
    menu_stuff.vehicle_name_sounds.locust_sound     = SYS_LoadSound("wav_data/computer/locust.wav" );
    menu_stuff.vehicle_name_sounds.mosquito_sound   = SYS_LoadSound("wav_data/computer/mosquito.wav" );
    menu_stuff.vehicle_name_sounds.flea_sound       = SYS_LoadSound("wav_data/computer/flea.wav" );

    menu_stuff.character_sounds.cap_sound = SYS_LoadSound( "wav_data/races/human/capgrt.wav" );
    menu_stuff.character_sounds.maj_sound = SYS_LoadSound( "wav_data/races/human/majgrt.wav" );
    menu_stuff.character_sounds.ric_sound = SYS_LoadSound( "wav_data/races/human/ricgrt.wav" );
    menu_stuff.character_sounds.cin_sound = SYS_LoadSound( "wav_data/races/human/cingrt.wav" );
    menu_stuff.character_sounds.pre_sound = SYS_LoadSound( "wav_data/races/human/pregrt.wav" );
    menu_stuff.character_sounds.mrk_sound = SYS_LoadSound( "wav_data/races/human/mrkgrt.wav" );
    menu_stuff.character_sounds.kri_sound = SYS_LoadSound( "wav_data/races/human/krigrt.wav" );
    menu_stuff.character_sounds.sha_sound = SYS_LoadSound( "wav_data/races/human/shagrt.wav" );
    menu_stuff.character_sounds.kel_sound = SYS_LoadSound( "wav_data/races/human/kelgrt.wav" );
    menu_stuff.character_sounds.meg_sound = SYS_LoadSound( "wav_data/races/human/meggrt.wav" );
    menu_stuff.character_sounds.bok_sound = SYS_LoadSound( "wav_data/races/bok/bokgrt.wav" );
    menu_stuff.character_sounds.nbo_sound = SYS_LoadSound( "wav_data/races/bok/nbogrt.wav" );
    menu_stuff.character_sounds.tbo_sound = SYS_LoadSound( "wav_data/races/bok/tbogrt.wav" );
    menu_stuff.character_sounds.pub_sound = SYS_LoadSound( "wav_data/races/slar/pubgrt.wav" );
    menu_stuff.character_sounds.gal_sound = SYS_LoadSound( "wav_data/races/slar/galgrt.wav" );
    menu_stuff.character_sounds.sep_sound = SYS_LoadSound( "wav_data/races/slar/sepgrt.wav" );
    menu_stuff.character_sounds.hot_sound = SYS_LoadSound( "wav_data/races/scavenge/hotgrt.wav" );
    menu_stuff.character_sounds.cru_sound = SYS_LoadSound( "wav_data/races/scavenge/crugrt.wav" );
    menu_stuff.character_sounds.gre_sound = SYS_LoadSound( "wav_data/races/scavenge/gregrt.wav" );
    menu_stuff.character_sounds.cry_sound = SYS_LoadSound( "wav_data/races/succubi/crygrt.wav" );
    menu_stuff.character_sounds.gla_sound = SYS_LoadSound( "wav_data/races/succubi/glagrt.wav" );
    menu_stuff.character_sounds.azu_sound = SYS_LoadSound( "wav_data/races/succubi/azugrt.wav" );
    menu_stuff.character_sounds.baa_sound = SYS_LoadSound( "wav_data/races/overlord/baagrt.wav" );
    menu_stuff.character_sounds.mep_sound = SYS_LoadSound( "wav_data/races/overlord/mepgrt.wav" );
    menu_stuff.character_sounds.mal_sound = SYS_LoadSound( "wav_data/races/overlord/malgrt.wav" );
    menu_stuff.character_sounds.anu_sound = SYS_LoadSound( "wav_data/races/pharoah/anugrt.wav" );
    menu_stuff.character_sounds.bas_sound = SYS_LoadSound( "wav_data/races/pharoah/basgrt.wav" );
    menu_stuff.character_sounds.hor_sound = SYS_LoadSound( "wav_data/races/pharoah/horgrt.wav" );
    menu_stuff.character_sounds.neb_sound = SYS_LoadSound( "wav_data/races/watchers/nebgrt.wav" );
    menu_stuff.character_sounds.pol_sound = SYS_LoadSound( "wav_data/races/watchers/polgrt.wav" );
    menu_stuff.character_sounds.qua_sound = SYS_LoadSound( "wav_data/races/watchers/quagrt.wav" );

    menu_stuff.character_sounds.r_sound   = SYS_LoadSound( "wav_data/races/sentry/rgrt.wav" );
    menu_stuff.character_sounds.g_sound   = SYS_LoadSound( "wav_data/races/sentry/ggrt.wav" );
    menu_stuff.character_sounds.b_sound   = SYS_LoadSound( "wav_data/races/sentry/bgrt.wav" );
    menu_stuff.character_sounds.b00_sound = SYS_LoadSound( "wav_data/races/biomech/b00grt.wav" );
    menu_stuff.character_sounds.d23_sound = SYS_LoadSound( "wav_data/races/biomech/d23grt.wav" );
    menu_stuff.character_sounds.jld_sound = SYS_LoadSound( "wav_data/races/biomech/jldgrt.wav" );
}

void Free_Menu_Voices( void )
{
     SYS_ReleaseSound( menu_stuff.vehicle_name_sounds.beetle_sound );
     menu_stuff.vehicle_name_sounds.beetle_sound = 0;
     SYS_ReleaseSound( menu_stuff.vehicle_name_sounds.wasp_sound );
     menu_stuff.vehicle_name_sounds.wasp_sound = 0;
     SYS_ReleaseSound( menu_stuff.vehicle_name_sounds.roach_sound );
     menu_stuff.vehicle_name_sounds.roach_sound = 0;
     SYS_ReleaseSound( menu_stuff.vehicle_name_sounds.dragonfly_sound );
     menu_stuff.vehicle_name_sounds.dragonfly_sound = 0;
     SYS_ReleaseSound( menu_stuff.vehicle_name_sounds.spider_sound );
     menu_stuff.vehicle_name_sounds.spider_sound = 0;
     SYS_ReleaseSound( menu_stuff.vehicle_name_sounds.locust_sound );
     menu_stuff.vehicle_name_sounds.locust_sound = 0;
     SYS_ReleaseSound( menu_stuff.vehicle_name_sounds.mosquito_sound );
     menu_stuff.vehicle_name_sounds.mosquito_sound = 0;
     SYS_ReleaseSound( menu_stuff.vehicle_name_sounds.flea_sound );
     menu_stuff.vehicle_name_sounds.flea_sound = 0;

     SYS_ReleaseSound( menu_stuff.character_sounds.cap_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.maj_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.ric_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.cin_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.pre_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.mrk_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.kri_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.sha_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.kel_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.meg_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.bok_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.nbo_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.tbo_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.pub_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.gal_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.sep_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.hot_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.cru_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.gre_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.cry_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.gla_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.azu_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.baa_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.mep_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.mal_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.anu_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.bas_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.hor_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.neb_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.pol_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.qua_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.r_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.g_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.b_sound );

     SYS_ReleaseSound( menu_stuff.character_sounds.b00_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.d23_sound );
     SYS_ReleaseSound( menu_stuff.character_sounds.jld_sound );


     menu_stuff.character_sounds.cap_sound = 0;
     menu_stuff.character_sounds.maj_sound = 0;
     menu_stuff.character_sounds.ric_sound = 0;
     menu_stuff.character_sounds.cin_sound = 0;
     menu_stuff.character_sounds.pre_sound = 0;
     menu_stuff.character_sounds.mrk_sound = 0;
     menu_stuff.character_sounds.kri_sound = 0;
     menu_stuff.character_sounds.sha_sound = 0;
     menu_stuff.character_sounds.kel_sound = 0;
     menu_stuff.character_sounds.meg_sound = 0;
     menu_stuff.character_sounds.bok_sound = 0;
     menu_stuff.character_sounds.nbo_sound = 0;
     menu_stuff.character_sounds.tbo_sound = 0;
     menu_stuff.character_sounds.pub_sound = 0;
     menu_stuff.character_sounds.gal_sound = 0;
     menu_stuff.character_sounds.sep_sound = 0;
     menu_stuff.character_sounds.hot_sound = 0;
     menu_stuff.character_sounds.cru_sound = 0;
     menu_stuff.character_sounds.gre_sound = 0;
     menu_stuff.character_sounds.cry_sound = 0;
     menu_stuff.character_sounds.gla_sound = 0;
     menu_stuff.character_sounds.azu_sound = 0;
     menu_stuff.character_sounds.baa_sound = 0;
     menu_stuff.character_sounds.mep_sound = 0;
     menu_stuff.character_sounds.mal_sound = 0;
     menu_stuff.character_sounds.anu_sound = 0;
     menu_stuff.character_sounds.bas_sound = 0;
     menu_stuff.character_sounds.hor_sound = 0;
     menu_stuff.character_sounds.neb_sound = 0;
     menu_stuff.character_sounds.pol_sound = 0;
     menu_stuff.character_sounds.qua_sound = 0;
     menu_stuff.character_sounds.r_sound = 0;
     menu_stuff.character_sounds.g_sound = 0;
     menu_stuff.character_sounds.b_sound = 0;
     menu_stuff.character_sounds.b00_sound = 0;
     menu_stuff.character_sounds.d23_sound = 0;
     menu_stuff.character_sounds.jld_sound = 0;
}


void init_menu_stuff( MenuStuff *menu_stuff )
{

    /* malloc mem for the micro_text pcx */

    PCX_Allocate( &(menu_stuff->micro_text) );
    PCX_Load( "pcx_data/microfnt.pcx", &(menu_stuff->micro_text) );

    /* Load the menu_text bitmap */

    PCX_Allocate( &(menu_stuff->menu_text) );
    PCX_Load( "pcx_data/menutext.pcx", &(menu_stuff->menu_text) );

    /* Load the menu background */

    PCX_Allocate( &(menu_stuff->general_menu_background) );
    PCX_Load( "pcx_data/menuback.pcx", &(menu_stuff->general_menu_background) );

    /* Load the wingman menu background */

    PCX_Allocate( &(menu_stuff->wingman_menu_background) );
    PCX_Load( "pcx_data/wingback.pcx", &(menu_stuff->wingman_menu_background) );

    /* Load the vehicle_menu_background */

    PCX_Allocate( &(menu_stuff->vehicle_menu_background) );
    PCX_Load( "pcx_data/tankback.pcx", &(menu_stuff->vehicle_menu_background) );

    PCX_Allocate( &(menu_stuff->human_1) );
    PCX_Load( "pcx_data/human1.pcx", &(menu_stuff->human_1) );

    PCX_Allocate( &(menu_stuff->human_2) );
    PCX_Load( "pcx_data/human2.pcx", &(menu_stuff->human_2) );

    PCX_Allocate( &(menu_stuff->human_3) );
    PCX_Load( "pcx_data/human3.pcx", &(menu_stuff->human_3) );

    PCX_Allocate( &(menu_stuff->human_4) );
    PCX_Load( "pcx_data/human4.pcx", &(menu_stuff->human_4) );

    PCX_Allocate( &(menu_stuff->overlords) );
    PCX_Load( "pcx_data/overlord.pcx", &(menu_stuff->overlords) );

    PCX_Allocate( &(menu_stuff->pharoahs) );
    PCX_Load( "pcx_data/pharoahs.pcx", &(menu_stuff->pharoahs) );

    PCX_Allocate( &(menu_stuff->scavengers) );
    PCX_Load( "pcx_data/scaven.pcx", &(menu_stuff->scavengers) );

    PCX_Allocate( &(menu_stuff->slar) );
    PCX_Load( "pcx_data/slar.pcx", &(menu_stuff->slar) );

    PCX_Allocate( &(menu_stuff->succubi) );
    PCX_Load( "pcx_data/succubi.pcx", &(menu_stuff->succubi) );

    PCX_Allocate( &(menu_stuff->watchers) );
    PCX_Load( "pcx_data/watch.pcx", &(menu_stuff->watchers) );

    PCX_Allocate( &(menu_stuff->bok) );
    PCX_Load( "pcx_data/bok.pcx", &(menu_stuff->bok) );

    PCX_Allocate( &(menu_stuff->biomech) );
    PCX_Load( "pcx_data/biomech.pcx", &(menu_stuff->biomech) );

    PCX_Allocate( &(menu_stuff->sentry) );
    PCX_Load( "pcx_data/sentry.pcx", &(menu_stuff->sentry) );

    get_object( &(menu_stuff->wasp_obj), "3d_data/wasp.pfd" );
    get_object( &(menu_stuff->beetle_obj), "3d_data/beetle.pfd" );
    get_object( &(menu_stuff->flea_obj), "3d_data/flea.pfd" );
    get_object( &(menu_stuff->mosquito_obj), "3d_data/mosquito.pfd" );
    get_object( &(menu_stuff->spider_obj), "3d_data/spider.pfd" );
    get_object( &(menu_stuff->roach_obj), "3d_data/roach.pfd" );
    get_object( &(menu_stuff->locust_obj), "3d_data/locust.pfd" );
    get_object( &(menu_stuff->dragonfly_obj), "3d_data/dragon.pfd" );
}


void init_world_stuff( WorldStuff *world_stuff )
{
    long i;
    pcx_picture temp_pcx;

    /* make sure all pointers in worldstuff are NULL (to prevent the freeing
       of dangling pointers) */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "RESET DANGLEING POINTERS TO NULL" );
    }
    else {
        cylindrix_init_print( "RESET DANGLEING POINTERS TO NULL" );
    }

    world_stuff->tube.point = NULL;
    world_stuff->tube.points = 0;
    world_stuff->tube.face = NULL;
    world_stuff->tube.faces = 0;

    world_stuff->wire_tube.point = NULL;
    world_stuff->wire_tube.points = 0;
    world_stuff->wire_tube.edge = NULL;
    world_stuff->wire_tube.edges = 0;

    world_stuff->wire_tinytube.point = NULL;
    world_stuff->wire_tinytube.points = 0;
    world_stuff->wire_tinytube.edge = NULL;
    world_stuff->wire_tinytube.edges = 0;

    world_stuff->everything.point = NULL;
    world_stuff->everything.points = 0;
    world_stuff->everything.face = NULL;
    world_stuff->everything.faces = 0;

    world_stuff->laser_obj.point = NULL;
    world_stuff->laser_obj.points = 0;
    world_stuff->laser_obj.face = NULL;
    world_stuff->laser_obj.faces = 0;

    world_stuff->missile_obj.point = NULL;
    world_stuff->missile_obj.points = 0;
    world_stuff->missile_obj.face = NULL;
    world_stuff->missile_obj.faces = 0;

    world_stuff->mine_obj.point = NULL;
    world_stuff->mine_obj.points = 0;
    world_stuff->mine_obj.face = NULL;
    world_stuff->mine_obj.faces = 0;

    world_stuff->cs_missile_obj.point = NULL;
    world_stuff->cs_missile_obj.points = 0;
    world_stuff->cs_missile_obj.face = NULL;
    world_stuff->cs_missile_obj.faces = 0;

    world_stuff->traitor_missile_obj.point = NULL;
    world_stuff->traitor_missile_obj.points = 0;
    world_stuff->traitor_missile_obj.face = NULL;
    world_stuff->traitor_missile_obj.faces = 0;

    world_stuff->laser_debris_obj.point = NULL;
    world_stuff->laser_debris_obj.points = 0;
    world_stuff->laser_debris_obj.face = NULL;
    world_stuff->laser_debris_obj.faces = 0;

    world_stuff->missile_debris_obj.point = NULL;
    world_stuff->missile_debris_obj.points = 0;
    world_stuff->missile_debris_obj.face = NULL;
    world_stuff->missile_debris_obj.faces = 0;

    world_stuff->mine_debris_obj.point = NULL;
    world_stuff->mine_debris_obj.points = 0;
    world_stuff->mine_debris_obj.face = NULL;
    world_stuff->mine_debris_obj.faces = 0;

    world_stuff->cs_missile_debris_obj.point = NULL;
    world_stuff->cs_missile_debris_obj.points = 0;
    world_stuff->cs_missile_debris_obj.face = NULL;
    world_stuff->cs_missile_debris_obj.faces = 0;

    world_stuff->traitor_missile_debris_obj.point = NULL;
    world_stuff->traitor_missile_debris_obj.points = 0;
    world_stuff->traitor_missile_debris_obj.face = NULL;
    world_stuff->traitor_missile_debris_obj.faces = 0;

    world_stuff->ramming_debris_obj.point = NULL;
    world_stuff->ramming_debris_obj.points = 0;
    world_stuff->ramming_debris_obj.face = NULL;
    world_stuff->ramming_debris_obj.faces = 0;

    world_stuff->explosion_cloud_obj.point = NULL;
    world_stuff->explosion_cloud_obj.points = 0;
    world_stuff->explosion_cloud_obj.face = NULL;
    world_stuff->explosion_cloud_obj.faces = 0;

    world_stuff->red_radar_base.base_obj = NULL;
    world_stuff->red_radar_base.turret_obj = NULL;
    world_stuff->red_radar_base.world_base_obj = NULL;
    world_stuff->red_radar_base.world_turret_obj = NULL;
    world_stuff->red_radar_base.world_union_obj = NULL;

    world_stuff->blue_radar_base.base_obj = NULL;
    world_stuff->blue_radar_base.turret_obj = NULL;
    world_stuff->blue_radar_base.world_base_obj = NULL;
    world_stuff->blue_radar_base.world_turret_obj = NULL;
    world_stuff->blue_radar_base.world_union_obj = NULL;

    /* init actual players vehicle */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "INIT_RED_TEAM() ENERGIZED" );
    }
    else {
        cylindrix_init_print( "INIT_RED_TEAM() ENERGIZED" );
    }

    init_red_team( world_stuff->player_array );

    /* init dummy vehicle */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "INIT_RED_TEAM() POWERED UP" );
    }
    else {
        cylindrix_init_print( "INIT_RED_TEAM() POWERED UP" );
    }
    
    init_blue_team( world_stuff->player_array );

    /* load the laser object */
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( LASER_OBJ ) READY TO FLY" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( LASER_OBJ ) READY TO FLY" );
    }
    
    get_object( &(world_stuff->laser_obj), "3d_data/laser.pfd" );

    /* load the missile object */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( MISSILE_OBJ ) ENGAGED" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( MISSILE_OBJ ) ENGAGED" );
    }
    
    get_object( &(world_stuff->missile_obj), "3d_data/missile.pfd" );

    for( i = 0; i < world_stuff->missile_obj.faces; i++ ) {
        world_stuff->missile_obj.face[i].gradient = MissileGrad;
    }

    /* load the mine object */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( MINE_OBJ ) ACTIVE" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( MINE_OBJ ) ACTIVE" );
    }
    
    get_object( &(world_stuff->mine_obj), "3d_data/mine.pfd" );

    for( i = 0; i < world_stuff->mine_obj.faces; i++ ) {
        world_stuff->mine_obj.face[i].gradient = MissileGrad;
        world_stuff->mine_obj.face[i].transparent = TRUE;
    }
    
    /* load the cs_missile object */
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( CS_MISSILE_OBJ ) CHECK" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( CS_MISSILE_OBJ ) CHECK" );
    }
    
    get_object( &(world_stuff->cs_missile_obj), "3d_data/csmissil.pfd" );

    for( i = 0; i < world_stuff->cs_missile_obj.faces; i++ ) {
        world_stuff->cs_missile_obj.face[i].gradient = MissileGrad;
    }

    /* load the traitor_missile object */
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( TRAITOR_MISSILE_OBJ ) ACTIVE" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( TRAITOR_MISSILE_OBJ ) ACTIVE" );
    }
    
    get_object( &(world_stuff->traitor_missile_obj), "3d_data/traitor.pfd" );

    for( i = 0; i < world_stuff->traitor_missile_obj.faces; i++ ) {
        world_stuff->traitor_missile_obj.face[i].gradient = MissileGrad;
    }

    /* load the laser_debris_obj */

    if( game_configuration.game_type == TournamentGame ) {    
        tournament_init_print( "GET_OBJECT( LASER_DEBRIS_OBJ ) LIVING LARGE" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( LASER_DEBRIS_OBJ ) LIVING LARGE" );
    }
    
    get_object( &(world_stuff->laser_debris_obj), "3d_data/laserd.pfd" );

    /* load the missile_debris_obj */
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( MISSILE_DEBRIS_OBJ ) ON" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( LASER_DEBRIS_OBJ ) LIVING LARGE" );
    }

    get_object( &(world_stuff->missile_debris_obj), "3d_data/missiled.pfd" );

    for( i = 0; i < world_stuff->missile_debris_obj.faces; i++ ) {
        world_stuff->missile_debris_obj.face[i].gradient = ExplosionCloudGrad;
    }

    /* load the mine_debris_obj */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( MINE_DEBRIS_OBJ ) ENABLED" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( MINE_DEBRIS_OBJ ) ENABLED" );
    }

    get_object( &(world_stuff->mine_debris_obj), "3d_data/mined.pfd" );

    for( i = 0; i < world_stuff->mine_debris_obj.faces; i++ ) {
        world_stuff->mine_debris_obj.face[i].gradient = ExplosionCloudGrad;
    }

    /* load the cs_missile_debris_obj */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( CS_MISSILE_DEBRIS_OBJ )" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( CS_MISSILE_DEBRIS_OBJ )" );
    }

    get_object( &(world_stuff->cs_missile_debris_obj), "3d_data/csmissid.pfd" );

    for( i = 0; i < world_stuff->cs_missile_debris_obj.faces; i++ ) {
        world_stuff->cs_missile_debris_obj.face[i].gradient = ExplosionCloudGrad;
    }

    /* load the traitor_missile_debris_obj */
                  
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( CS_MISSILE_DEBRIS_OBJ )"  );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( CS_MISSILE_DEBRIS_OBJ )"  );
    }

    get_object( &(world_stuff->traitor_missile_debris_obj), "3d_data/traitord.pfd" );

    for( i = 0; i < world_stuff->traitor_missile_debris_obj.faces; i++ ) {
        world_stuff->traitor_missile_debris_obj.face[i].gradient = ExplosionCloudGrad;
    }

    /* load the ramming_debris_obj */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( RAMING_DEBRIS ) GENERATED" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( RAMING_DEBRIS ) GENERATED" );
    }

    get_object( &(world_stuff->ramming_debris_obj), "3d_data/ramd.pfd" );

    for( i = 0; i < world_stuff->ramming_debris_obj.faces; i++ ) {
        world_stuff->ramming_debris_obj.face[i].gradient = ExplosionCloudGrad;
    }

    /* load the explosion_cloud_obj */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( EXPLOSION_CLOUD_OBJ )" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( EXPLOSION_CLOUD_OBJ )" );
    }

    get_object( &(world_stuff->explosion_cloud_obj), "3d_data/cloud.pfd" );

    for( i = 0; i < world_stuff->explosion_cloud_obj.faces; i++ ) {
        world_stuff->explosion_cloud_obj.face[i].gradient = ExplosionCloudGrad;
    }

    /* init the hubs pointface data structures (to prevent freeing dangling
       pointers) */

    world_stuff->north_hub.point = NULL;
    world_stuff->north_hub.points = 0;
    world_stuff->north_hub.face = NULL;
    world_stuff->north_hub.faces = 0;

    world_stuff->south_hub.point = NULL;
    world_stuff->south_hub.points = 0;
    world_stuff->south_hub.face = NULL;
    world_stuff->south_hub.faces = 0;

    /* load hubs */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( NORTH_HUB ) ACTIVATED" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( NORTH_HUB ) ACTIVATED" );
    }

    get_object( &world_stuff->north_hub, level.north_hub_filename );

    for( i = 0; i < world_stuff->north_hub.faces; i++ ) {
        if( world_stuff->north_hub.face[i].gradient == 0 )
            world_stuff->north_hub.face[i].gradient = HubFirstGrad;
        else if( world_stuff->north_hub.face[i].gradient == 1 )
            world_stuff->north_hub.face[i].gradient = HubSecondGrad;
        else if( world_stuff->north_hub.face[i].gradient == 2 )
            world_stuff->north_hub.face[i].gradient = HubThirdGrad;
        else
            world_stuff->north_hub.face[i].gradient = HubFirstGrad;
    }

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( SOUTH_HUB ) ENERGIZED" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( SOUTH_HUB ) ENERGIZED" );
    }

    get_object( &world_stuff->south_hub, level.south_hub_filename );

    for( i = 0; i < world_stuff->south_hub.faces; i++ ) {
        if( world_stuff->south_hub.face[i].gradient == 0 )
            world_stuff->south_hub.face[i].gradient = HubFirstGrad;
        else if( world_stuff->south_hub.face[i].gradient == 1 )
            world_stuff->south_hub.face[i].gradient = HubSecondGrad;
        else if( world_stuff->south_hub.face[i].gradient == 2 )
            world_stuff->south_hub.face[i].gradient = HubThirdGrad;
        else
            world_stuff->south_hub.face[i].gradient = HubFirstGrad;
    }

    /* load the pylon configuration file and convert it into a PointFace */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_PYLONS() HYPER-LOGIC SIMULATION ON" );
    }
    else {
        cylindrix_init_print( "GET_PYLONS() HYPER-LOGIC SIMULATION ON" );
    }
    
    get_pylons( &(world_stuff->pylons), level.pylon_config_filename );

    /* load the tube */
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_OBJECT( TUBE ) ACTIVATING CYLINDRIX" );
    }
    else {
        cylindrix_init_print( "GET_OBJECT( TUBE ) ACTIVATING CYLINDRIX" );
    }
    
    get_object( &(world_stuff->tube), level.tube_filename );

    for( i = 0; i < world_stuff->tube.faces; i++ ) {
        if( world_stuff->tube.face[i].gradient == 0 )
            world_stuff->tube.face[i].gradient = TubeFirstGrad;
        else if( world_stuff->tube.face[i].gradient == 1 )
            world_stuff->tube.face[i].gradient = TubeSecondGrad;
        else if( world_stuff->tube.face[i].gradient == 2 )
            world_stuff->tube.face[i].gradient = TubeThirdGrad;
        else
            world_stuff->tube.face[i].gradient = TubeFirstGrad;
    }

    /* initialize energy power up */
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "INIT_ENERGY_POWER_UP() RE-VECTORED" );
    }
    else {
        cylindrix_init_print( "INIT_ENERGY_POWER_UP() RE-VECTORED" );
    }
    
    init_energy_power_up( &(world_stuff->energy_square) );

    /* initialize the wire_tube */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "GET_WIRE_OBJ( WIRE_TUBE ) PROCESSED" );
    }
    else {
        cylindrix_init_print( "GET_WIRE_OBJ( WIRE_TUBE ) PROCESSED" );
    }
    
    get_wire_object( &(world_stuff->wire_tube), "3d_data/tube.ped" );

    /* initialize the wire_tinytube used by the radar */
                  
    if( game_configuration.game_type == TournamentGame ) {                  
        tournament_init_print( "INIT_RADAR() KICKIN' ILL FUNK" );
    }
    else {
        cylindrix_init_print( "INIT_RADAR() KICKIN' ILL FUNK" );
    }
    
    init_radar( &(world_stuff->wire_tinytube) );

    /* initialize the red team's radar base */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "INIT_RED_RADAR_BASE() ILLUMINATED" );
    }
    else {
        cylindrix_init_print( "INIT_RED_RADAR_BASE() ILLUMINATED" );
    }
    
    init_red_radar_base( &(world_stuff->red_radar_base) );

    /* initialize the blue team's radar base */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "INIT_BLUE_RADAR_BASE() BASE I/O PORT 0X4FF" );
    }
    else {
        cylindrix_init_print( "INIT_BLUE_RADAR_BASE() BASE I/O PORT 0X4FF" );
    }
    
    init_blue_radar_base( &(world_stuff->blue_radar_base) );

    /* initalize the light position */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "LIGHT DELTA APPROXIMATION COMPLETE" );
    }
    else {
        cylindrix_init_print( "LIGHT DELTA APPROXIMATION COMPLETE" );
    }
    
    world_stuff->light[X] = 128;
    world_stuff->light[Y] = 0 << MEXP;
    world_stuff->light[Z] = -3 << MEXP;

    /* there aren't any explosions yet so explosion_list should be NULL */

    world_stuff->explosion_list = NULL;

    /* initialize the color_info data structure */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "INIT_COLOR_INFO() RECYCLED" );
    }
    else {
        cylindrix_init_print( "INIT_COLOR_INFO() RECYCLED" );
    }

    init_color_info( &(world_stuff->color_info) );

    /* set all the input tables to all zeros */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "REMOVE GARBAGE FROM INPUT TABLES" );
    }
    else {
        cylindrix_init_print( "REMOVE GARBAGE FROM INPUT TABLES" );
    }

    for( i = 0; i < 6; i++ ) {
        memset( world_stuff->player_array[i].table, FALSE, INPUT_TABLE_SIZE );
    }        
    
    /* Load the bitmaps for the letters & the color palette */
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "BLIT DATA OPTOMIZED" );
    }
    else {
        cylindrix_init_print( "BLIT DATA OPTOMIZED" );
    }

    PCX_Allocate( &(world_stuff->text) );

    PCX_Load( level.palette_filename, &world_stuff->text );

    /* Load the pcx for the heads up display */

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "COMPILED SPRITE ENGINE INIT FOR BEETLE" );
    }
    else {
        cylindrix_init_print( "COMPILED SPRITE ENGINE INIT FOR BEETLE" );
    }

    PCX_Allocate( &(temp_pcx) );

    PCX_Load( "pcx_data/beethud.pcx", &temp_pcx );

    init_pixmap( &world_stuff->beetle_hud, temp_pcx.buffer );
    
    free_pcx_picture( &temp_pcx );
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "COMPILED SPRITE ENGINE INIT FOR WASP" );
    }
    else {
        cylindrix_init_print( "COMPILED SPRITE ENGINE INIT FOR WASP" );
    }
    
    PCX_Allocate( &(temp_pcx) );                            
     
    PCX_Load( "pcx_data/wasphud.pcx", &temp_pcx );

    init_pixmap( &world_stuff->wasp_hud, temp_pcx.buffer );
    
    free_pcx_picture( &temp_pcx );
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "COMPILED SPRITE ENGINE INIT FOR FLEA" );
    }
    else {
        cylindrix_init_print( "COMPILED SPRITE ENGINE INIT FOR FLEA" );
    }
    
    PCX_Allocate( &(temp_pcx) );                            

    PCX_Load( "pcx_data/fleahud.pcx", &temp_pcx );

    init_pixmap( &world_stuff->flea_hud, temp_pcx.buffer );

    free_pcx_picture( &temp_pcx );

    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "COMPILED SPRITE ENGINE INIT FOR MOSQUITO" );
    }
    else {
        cylindrix_init_print( "COMPILED SPRITE ENGINE INIT FOR MOSQUITO" );
    }
    
    PCX_Allocate( &(temp_pcx) );                            

    PCX_Load( "pcx_data/mosqhud.pcx", &temp_pcx );

    init_pixmap( &world_stuff->mosquito_hud, temp_pcx.buffer );

    free_pcx_picture( &temp_pcx );

    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "COMPILED SPRITE ENGINE INIT FOR SPIDER" );
    }
    else {
        cylindrix_init_print( "COMPILED SPRITE ENGINE INIT FOR SPIDER" );
    }
    
    PCX_Allocate( &(temp_pcx) );                            

    PCX_Load( "pcx_data/spidhud.pcx", &temp_pcx );

    init_pixmap( &world_stuff->spider_hud, temp_pcx.buffer );

    free_pcx_picture( &temp_pcx );


    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "COMPILED SPRITE ENGINE INIT FOR DRAGONFLY" );
    }
    else {
        cylindrix_init_print( "COMPILED SPRITE ENGINE INIT FOR DRAGONFLY" );
    }
    
    PCX_Allocate( &(temp_pcx) );                            

    PCX_Load( "pcx_data/draghud.pcx", &temp_pcx );

    init_pixmap( &world_stuff->dragonfly_hud, temp_pcx.buffer );

    free_pcx_picture( &temp_pcx );
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "COMPILED SPRITE ENGINE INIT FOR ROACH" );
    }
    else {
        cylindrix_init_print( "COMPILED SPRITE ENGINE INIT FOR ROACH" );
    }
    
    PCX_Allocate( &(temp_pcx) );                            

    PCX_Load( "pcx_data/roachud.pcx", &temp_pcx );

    init_pixmap( &world_stuff->roach_hud, temp_pcx.buffer );

    free_pcx_picture( &temp_pcx );
    
    
    if( game_configuration.game_type == TournamentGame ) {
        tournament_init_print( "COMPILED SPRITE ENGINE INIT FOR LOCUST" );
    }
    else {
        cylindrix_init_print( "COMPILED SPRITE ENGINE INIT FOR LOCUST" );
    }
    
    PCX_Allocate( &(temp_pcx) );                            

    PCX_Load( "pcx_data/locuhud.pcx", &temp_pcx );

    init_pixmap( &world_stuff->locust_hud, temp_pcx.buffer );

    free_pcx_picture( &temp_pcx );

}

void check_command_line_args( int argc, const char *argv[],
                              unsigned char *ai_active )
{
    long i;

    /* check for command line argument flags */

    for( i = 0; i < argc; i ++ )
        {
         if( (argc > 1 ) && (strcmp( "-no_ai", argv[i] ) == 0 ) )
             {
              *ai_active = FALSE;
             }
         if( (argc > 1 ) && (strcmp( "-no_sound", argv[i] ) == 0 ) ) {
             ignore_sound_card = TRUE;
         }
         if( (argc > 1 ) && (strcmp( "-test_sound", argv[i] ) == 0 ) ) {
             test_samples = TRUE;
         }

         if( (argc > 1 ) && (strcmp( "-readout", argv[i] ) == 0 ) ) {
             show_readout = TRUE;
         }
         if( (argc > 1 ) && (strcmp( "-test_anim", argv[i] ) == 0 ) ) {
             test_anim = TRUE;
         }

         if( (argc > 1 ) && (strcmp( "-warp1", argv[i] ) == 0 ) ) {
             level_warp = 1;
         }
         if( (argc > 1 ) && (strcmp( "-warp2", argv[i] ) == 0 ) ) {
             level_warp = 2;
         }
         if( (argc > 1 ) && (strcmp( "-warp3", argv[i] ) == 0 ) ) {
             level_warp = 3;
         }
         if( (argc > 1 ) && (strcmp( "-warp4", argv[i] ) == 0 ) ) {
             level_warp = 4;
         }
         if( (argc > 1 ) && (strcmp( "-warp5", argv[i] ) == 0 ) ) {
             level_warp = 5;
         }
         if( (argc > 1 ) && (strcmp( "-warp6", argv[i] ) == 0 ) ) {
             level_warp = 6;
         }
         if( (argc > 1 ) && (strcmp( "-warp7", argv[i] ) == 0 ) ) {
             level_warp = 7;
         }
         if( (argc > 1 ) && (strcmp( "-warp8", argv[i] ) == 0 ) ) {
             level_warp = 8;
         }
         if( (argc > 1 ) && (strcmp( "-warp9", argv[i] ) == 0 ) ) {
             level_warp = 9;
         }
         if( (argc > 1 ) && (strcmp( "-warp10", argv[i] ) == 0 ) ) {
             level_warp = 10;
         }


        }  /* End of checking for args */

}

/* returns the index into the player_array of the local user */

int user_vehicle_index()
{
    int i;

    for( i = 0; i < 6; i++ ) {
        if( (world_stuff.player_array[i].controller == USER_CONTROL) ) {
            return i;
        }
    }
    return destination_vehicle_index;
}

/* tells what team the user is on */

void init_local_user_team( team_type *team )
{
    if( (user_vehicle_index() >= 0) && (user_vehicle_index() <= 2) ) {
        *team = RED_TEAM;
    }
    else if( (user_vehicle_index() >= 3) && (user_vehicle_index() <= 5) ) {
        *team = BLUE_TEAM;
    }
    else {
        *team = NO_TEAM;
    }
}

// AJT: networking is disabled.
#if 0
/* Receives data from the remote computer, about vehicles, wingman and the cylinder selected
   by the remote user.  Also it sends the same info about vehicles, wingman and the cylidner
   selected by the local user. */

void transfer_game_configuration_info( game_configuration_type *game_configuration, int master )
{
    game_info_type game_info;  /* packet that will hold data to be sent and data
                                  received from the remote computer */

    if( master ) {

        /* Initialize packet to send to the slave */

        game_info.vehicle_one   = game_configuration->red0_vehicle;
        game_info.vehicle_two   = game_configuration->red1_vehicle;
        game_info.vehicle_three = game_configuration->red2_vehicle;
        game_info.wingman_one   = game_configuration->red0_ai;
        game_info.wingman_two   = game_configuration->red1_ai;
        game_info.wingman_three = game_configuration->red2_ai;
        strcpy( game_info.cylinder_filename, game_configuration->cylinder_filename );

        /* fprintf( stderr, "Sending master game_info packet\n" ); */

        /* Send info packet to remote slave computer */
		
        Send_Game_Info_Packet( &game_info );

        /* fprintf( stderr, "Receiving slave game_info packet\n" ); */

        /* Get game info packet from remote slave computer */
		
        Get_Game_Info_Packet( &game_info );

        /* Copy the received packet into the game_configuration data struct */

        game_configuration->blue0_vehicle = game_info.vehicle_one;
        game_configuration->blue1_vehicle = game_info.vehicle_two;
        game_configuration->blue2_vehicle = game_info.vehicle_three;
        game_configuration->blue0_ai      = game_info.wingman_one;
        game_configuration->blue1_ai      = game_info.wingman_two;
        game_configuration->blue2_ai      = game_info.wingman_three;

        /* Note: Since the slave computer doesn't control which level file is to be loaded,
           we ignore the cylinder_filename field of game_info */
    }
    else {

        /* fprintf( stderr, "Receiving master game_info packet\n" ); */

        /* Get the master's game info packet */
		
        Get_Game_Info_Packet( &game_info );

        /* Copy master info packet data into the game_configuration struct */

        game_configuration->red0_vehicle = game_info.vehicle_one;
        game_configuration->red1_vehicle = game_info.vehicle_two;
        game_configuration->red2_vehicle = game_info.vehicle_three;
        game_configuration->red0_ai      = game_info.wingman_one;
        game_configuration->red1_ai      = game_info.wingman_two;
        game_configuration->red2_ai      = game_info.wingman_three;
        strcpy( game_configuration->cylinder_filename, game_info.cylinder_filename );


        /* Initialize the game_info packet that will be sent to the remote computer */

        game_info.vehicle_one   = game_configuration->blue0_vehicle;
        game_info.vehicle_two   = game_configuration->blue1_vehicle;
        game_info.vehicle_three = game_configuration->blue2_vehicle;
        game_info.wingman_one   = game_configuration->blue0_ai;
        game_info.wingman_two   = game_configuration->blue1_ai;
        game_info.wingman_three = game_configuration->blue2_ai;

        /* Note: since the slave cannot select the cylinder_filename we wont bother
           initializing the cylinder_filename field of game_info */

        /* fprintf(stderr, "Sending local game info packet \n"); */

        /* Send info packet to remote computer */
		
        Send_Game_Info_Packet( &game_info );
    }
}
#endif

void print_game_configuration_info( game_configuration_type *game_configuration, int master )
{

}

/* Depending on the type of game to be played this function will initialize the controller fields in
   each Player struct. */

void init_player_array_controller_info( Player p[], game_configuration_type *game_configuration,
                                        int master )
{

	p[0].controller = USER_CONTROL;
	p[1].controller = AI_CONTROL;
	p[2].controller = AI_CONTROL;
	p[3].controller = AI_CONTROL;
	p[4].controller = AI_CONTROL;
	p[5].controller = AI_CONTROL;
}



   /* get_all_input() initializes all of the players input_tables. If
   the player is USER_CONTROL the input_table comes from the joystick,
   mouse or keyboard.  If the user is AI_CONTROL then the input comes
   from the function AI_Control().  If the user is REMOTE_CONTROL then
   the input comes through the null-modem. */

void get_all_input( Player p[], int ai_active, int master )
{
    int i;

    for( i = 0; i < 6; i++ ) {
        if( p[i].controller == USER_CONTROL && !p[i].tank.traitor_active) {
            get_user_key_table( p[i].table );
        }
        else if( p[i].controller == AI_CONTROL || p[i].tank.traitor_active ) {
            if( ai_active && p[i].tank.alive && p[i].controller != REMOTE_CONTROL ) {
                AI_Control( &world_stuff, i );
            }
            else {
                memset( p[i].table, 0, INPUT_TABLE_SIZE );
            }
        }
        else if( p[i].controller == REMOTE_CONTROL ) {
            ;
        }
    }

} /* End of get_all_input() */

void get_only_ai_input( Player p[], int ai_active, int master )
{
    int i;

    for( i = 0; i < 6; i++ ) {
    
        if( p[i].controller == USER_CONTROL ) {
            memset( p[i].table, 0, INPUT_TABLE_SIZE );
        }
        else if( p[i].controller == AI_CONTROL ) {
            if( ai_active && p[i].tank.alive ) {
                AI_Control( &world_stuff, i );
            }
            else {
                memset( p[i].table, 0, INPUT_TABLE_SIZE );
            }
        }
        else if( p[i].controller == REMOTE_CONTROL ) {
            ;
        }    
    }

} /* End of get_only_ai_input() */


/* initialize the input_table for the local user. If the mouse is active
   then the input comes from the mouse. If the joystick is active then
   the input comes from joystick or flightstick. Otherwise the input
   is from the keyboard. */

void get_user_key_table( input_table table )
{
    Get_Key_Table( table );

// AJT: mouse & joystick is disabled.
#if 0
    if( game_configuration.mouse_active ) {
        Get_Mouse_Table( table );
    }

    if( game_configuration.joystick == StandardJoystick ) {
        Get_Joystick_Table( table );
    }
    else if( game_configuration.joystick == FlightStickPro ) {
        Get_Flightstick_Table( table );
    }
#endif
}

void anti_missile_update( WorldStuff *world_stuff )
{
    long i, j;
    Projectile *ptr;

    for( i = 0; i < 6; i++ ) {

        if( world_stuff->player_array[i].tank.anti_missile_active ) {

            /* find all enemy missiles locked on player[i] and
               re-direct them */

            for( j = 0; j < 6; j++ ) {

                if( j != i ) {

                    ptr = world_stuff->player_array[j].tank.projectile_list;

                    while( ptr != NULL ) {

                        if( world_stuff->player_array[i].tank.missiles_stored > 0 ) {

                            if( ptr->target == world_stuff->player_array[i].tank.orient.position ) {

                                ptr->target = cylinder_center;
                                world_stuff->player_array[i].tank.missiles_stored--;
                            }
                        }
                        ptr = ptr->next;
                    }
                }
            }
        }
    }
} //anti_missile_update

/* Move all the vehicles according to their input_tables one frame forward.
   This function handles all the collisions between vehicles, pylons & projectiles.
   Also creates and destroys projectiles, inflicts damage, scrables controls ....
   basicly all the main game logic and physics takes place here */

void move_everything_one_click()
{
    long i, j;
    unsigned char temp;

    for( i = 0; i < 6; i++ ) {

        if( world_stuff.player_array[i].tank.alive ) {

            /* scramble key table if necessary */

            if( world_stuff.player_array[i].tank.controls_scrambled ) {
                if( world_stuff.player_array[i].tank.frames_till_unscramble > 0 ) {

                    /* scramble key table */

                    temp = world_stuff.player_array[i].table[INDEX_LEFT];
                    world_stuff.player_array[i].table[INDEX_LEFT] = world_stuff.player_array[i].table[INDEX_UP];
                    world_stuff.player_array[i].table[INDEX_UP] = world_stuff.player_array[i].table[INDEX_RIGHT];
                    world_stuff.player_array[i].table[INDEX_RIGHT] = world_stuff.player_array[i].table[INDEX_DOWN];
                    world_stuff.player_array[i].table[INDEX_DOWN] = temp;

                    temp = world_stuff.player_array[i].table[INDEX_SIDESTEP_LEFT];
                    world_stuff.player_array[i].table[INDEX_SIDESTEP_LEFT] = world_stuff.player_array[i].table[INDEX_SIDESTEP_RIGHT];
                    world_stuff.player_array[i].table[INDEX_SIDESTEP_RIGHT] = temp;

                    world_stuff.player_array[i].tank.frames_till_unscramble--;

                }
                else {
                    world_stuff.player_array[i].tank.controls_scrambled = FALSE;
                }
            }

            /* handle traitor vehicles */

            if( world_stuff.player_array[i].tank.traitor_active ) {
                if( world_stuff.player_array[i].tank.frames_till_traitor_deactivate > 0 ) {
                    world_stuff.player_array[i].tank.frames_till_traitor_deactivate--;
                }
                else {
                    world_stuff.player_array[i].tank.traitor_active = FALSE;
                }
            }

            /* Move surface mode info from input_table to vehicle */

            if( Check_Input_Table( INDEX_MODE_SWITCH, world_stuff.player_array[i].table ) ) {
                if( world_stuff.player_array[i].tank.vehicle_mode == Air ) {
                    world_stuff.player_array[i].tank.air_forward_speed = 0.0;
                    world_stuff.player_array[i].tank.vehicle_mode = Landing;
                    if( game_configuration.sound_on ) {
                        Q_Jon_Sample( MODE_SWITCH_ONTO_GROUND, world_stuff.player_array[i].tank.orient.position );
                    }
                }
                else if( world_stuff.player_array[i].tank.vehicle_mode == Surface ) {
                    world_stuff.player_array[i].tank.vehicle_mode = Air;
                    world_stuff.player_array[i].tank.vel[X] = 0.35f * world_stuff.player_array[i].tank.orient.up[X];
                    world_stuff.player_array[i].tank.vel[Y] = 0.35f * world_stuff.player_array[i].tank.orient.up[Y];
                    world_stuff.player_array[i].tank.vel[Z] = 0.35f * world_stuff.player_array[i].tank.orient.up[Z];
                    if( game_configuration.sound_on ) {
                        Q_Jon_Sample( MODE_SWITCH_INTO_AIR, world_stuff.player_array[i].tank.orient.position );
                    }
                }
                else if( world_stuff.player_array[i].tank.vehicle_mode == Landing )
                    world_stuff.player_array[i].tank.vehicle_mode = Air;

            } /* End if mode switch */



            /* modifiy current velocity vector according to key presses */

            if( Check_Input_Table( INDEX_LEFT, world_stuff.player_array[i].table ) ) {
                update_vehicle_left_key( &(world_stuff.player_array[i].tank),
                                         world_stuff.player_array[i].table[INDEX_LEFT] );
            }
            if( Check_Input_Table( INDEX_RIGHT, world_stuff.player_array[i].table  ) ) {
                update_vehicle_right_key( &(world_stuff.player_array[i].tank),
                                         world_stuff.player_array[i].table[INDEX_RIGHT] );
            }
            if( Check_Input_Table( INDEX_UP, world_stuff.player_array[i].table  ) ) {
                update_vehicle_up_key( &(world_stuff.player_array[i].tank),
                                       world_stuff.player_array[i].table[INDEX_UP] );
            }
            if( Check_Input_Table( INDEX_DOWN, world_stuff.player_array[i].table ) ) {
                update_vehicle_down_key( &(world_stuff.player_array[i].tank),
                                         world_stuff.player_array[i].table[INDEX_DOWN] );
            }
            if( Check_Input_Table( INDEX_SIDESTEP_RIGHT, world_stuff.player_array[i].table  ) ) {
                update_vehicle_sidestep_right( &(world_stuff.player_array[i].tank),
                                               world_stuff.player_array[i].table[INDEX_SIDESTEP_RIGHT] );
            }
            if( Check_Input_Table( INDEX_SIDESTEP_LEFT, world_stuff.player_array[i].table  ) ) {
                update_vehicle_sidestep_left( &(world_stuff.player_array[i].tank),
                                              world_stuff.player_array[i].table[INDEX_SIDESTEP_LEFT] );
            }
            if( Check_Input_Table( INDEX_SIDESTEP_UP, world_stuff.player_array[i].table ) ) {
                if( world_stuff.player_array[i].tank.vehicle_mode == Surface ) {
                    update_vehicle_up_key( &(world_stuff.player_array[i].tank),
                                           world_stuff.player_array[i].table[INDEX_SIDESTEP_UP] );
                }
                else if( world_stuff.player_array[i].tank.vehicle_mode == Air ) {
                    update_vehicle_sidestep_up( &(world_stuff.player_array[i].tank),
                                                world_stuff.player_array[i].table[INDEX_SIDESTEP_UP] );
                }
            }
            if( Check_Input_Table( INDEX_SIDESTEP_DOWN, world_stuff.player_array[i].table ) ) {
                if( world_stuff.player_array[i].tank.vehicle_mode == Surface ) {
                    update_vehicle_down_key( &(world_stuff.player_array[i].tank),
                                             world_stuff.player_array[i].table[INDEX_SIDESTEP_DOWN] );
                }
                else if( world_stuff.player_array[i].tank.vehicle_mode == Air ) {
                    update_vehicle_sidestep_down( &(world_stuff.player_array[i].tank),
                                                  world_stuff.player_array[i].table[INDEX_SIDESTEP_DOWN] );
                }
            }

            /* Check for change in throttle speed. */

            if( Check_Input_Table( INDEX_UP_THROTTLE, world_stuff.player_array[i].table ) ) {
                if( world_stuff.player_array[i].tank.vehicle_mode == Air ) {
                    up_throttle( &(world_stuff.player_array[i].tank) );
                }
            }
            if( Check_Input_Table( INDEX_DOWN_THROTTLE, world_stuff.player_array[i].table ) ) {
                if( world_stuff.player_array[i].tank.vehicle_mode == Air ) {
                    down_throttle( &(world_stuff.player_array[i].tank) );
                }
            }

            /* update this vehicles target */

            update_missile_target( world_stuff.player_array, i, &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
        }
    }

#ifdef PROFILE
    if( profile ) {
        printf("move_vehicles(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* moves the vehicles and aims the radar_bases */

    move_vehicles( world_stuff.player_array, &(world_stuff.pylons), &(world_stuff.red_radar_base),
                   &(world_stuff.blue_radar_base), &(world_stuff.laser_obj) );

#ifdef PROFILE
    if( profile ) {
        printf("move all projectiles forward, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < 6; i++ ) {

        /* Move this vehicles projectiles forward.  If a collision with a
           pylon is detected or the projectiles life span is exceeded that
           projectile is deleted.  If a collision with a vehicle is detected,
           the vehicles hitpoints are decremented and the projectile is
           deleted. */

        move_projectiles_forward( &(world_stuff.player_array[i].tank), &(world_stuff.pylons), world_stuff.player_array, &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );

        if( world_stuff.player_array[i].tank.alive ) {

            /* If the vehicle fires its gun, create the new projectile.  The
               projectile position is a little bit ahead of the vehicle. */

            if( Check_Input_Table( INDEX_FIRE_GUN, world_stuff.player_array[i].table ) ) {
                if( world_stuff.player_array[i].tank.frames_till_fire_laser == 0 ) {
                    create_projectile( &(world_stuff.player_array[i].tank), &(world_stuff.laser_obj), LASER );
                    world_stuff.player_array[i].tank.frames_till_fire_laser = world_stuff.player_array[i].tank.laser_reload_time;

                    if( world_stuff.player_array[i].tank.cloaking_active == TRUE ) {

                        /* deactivate cloaking */

                        world_stuff.player_array[i].tank.cloaking_active = FALSE;
                        world_stuff.player_array[i].tank.frames_till_cloak = world_stuff.player_array[i].tank.cloak_reload_time;

                        /* make this tanks faces opaque */

                        for( j = 0; j < world_stuff.player_array[i].tank.obj->faces; j++ ) {
                            world_stuff.player_array[i].tank.obj->face[j].transparent = FALSE;
                        }

                        world_stuff.player_array[i].tank.frames_till_cloak_suck = 0;
                    }
                }
            }

            if( Check_Input_Table( INDEX_FIRE_MISSILE, world_stuff.player_array[i].table ) ) {
                if( world_stuff.player_array[i].tank.frames_till_fire_missile == 0 && ( world_stuff.player_array[i].tank.missiles_stored > 0 ) ) {

                    /* fire a missile */

                    create_projectile( &(world_stuff.player_array[i].tank), &(world_stuff.missile_obj), MISSILE );
                    world_stuff.player_array[i].tank.frames_till_fire_missile = world_stuff.player_array[i].tank.missile_reload_time;
                    world_stuff.player_array[i].tank.missiles_stored--;

                    if( world_stuff.player_array[i].tank.cloaking_active == TRUE ) {

                        /* deactivate cloaking */

                        world_stuff.player_array[i].tank.cloaking_active = FALSE;
                        world_stuff.player_array[i].tank.frames_till_cloak = world_stuff.player_array[i].tank.cloak_reload_time;

                        /* make this tanks faces opaque */

                        for( j = 0; j < world_stuff.player_array[i].tank.obj->faces; j++ ) {
                            world_stuff.player_array[i].tank.obj->face[j].transparent = FALSE;
                        }

                        world_stuff.player_array[i].tank.frames_till_cloak_suck = 0;
                    }
                }
            }
            if( Check_Input_Table( INDEX_SPECIAL_WEAPON, world_stuff.player_array[i].table ) ) {

                if( world_stuff.player_array[i].tank.frames_till_fire_missile == 0 && ( world_stuff.player_array[i].tank.missiles_stored > 0 ) ) {

                    if( world_stuff.player_array[i].tank.vtype == Mosquito ) {

                        /* fire a traitor_missile */

                        create_projectile( &(world_stuff.player_array[i].tank), &(world_stuff.traitor_missile_obj), TRAITOR_MISSILE );
                        world_stuff.player_array[i].tank.frames_till_fire_missile = world_stuff.player_array[i].tank.traitor_missile_reload_time;
                        world_stuff.player_array[i].tank.missiles_stored--;

                    }
                    else if( world_stuff.player_array[i].tank.vtype == Beetle ) {

                        /* fire a cs_missile */

                        create_projectile( &(world_stuff.player_array[i].tank), &(world_stuff.cs_missile_obj), CS_MISSILE );
                        world_stuff.player_array[i].tank.frames_till_fire_missile = world_stuff.player_array[i].tank.cs_missile_reload_time;
                        world_stuff.player_array[i].tank.missiles_stored--;
                    }
                    else if( world_stuff.player_array[i].tank.vtype == Roach ) {

                        /* launch a mine */

                        create_projectile( &(world_stuff.player_array[i].tank), &(world_stuff.mine_obj), MINE );
                        world_stuff.player_array[i].tank.frames_till_fire_missile = world_stuff.player_array[i].tank.mine_reload_time;
                        world_stuff.player_array[i].tank.missiles_stored--;
                    }
                    else if( world_stuff.player_array[i].tank.vtype == Locust ) {

                        /* launch 3 decoys */

                        create_decoy_projectile( &(world_stuff.player_array[i].tank), 0 );
                        create_decoy_projectile( &(world_stuff.player_array[i].tank), 1 );
                        create_decoy_projectile( &(world_stuff.player_array[i].tank), 2 );

                        world_stuff.player_array[i].tank.missiles_stored--;
                    }
                    else if( world_stuff.player_array[i].tank.vtype == Dragonfly ) {

                        if( (world_stuff.player_array[i].tank.frames_till_cloak == 0) &&
                            (world_stuff.player_array[i].tank.missiles_stored > 0) &&
                            (world_stuff.player_array[i].tank.cloaking_active == FALSE) ) {

                            /* activate cloaking */

                            world_stuff.player_array[i].tank.cloaking_active = TRUE;
                            world_stuff.player_array[i].tank.frames_till_cloak = world_stuff.player_array[i].tank.cloak_reload_time;

                            /* make this tanks faces transparent */

                            for( j = 0; j < world_stuff.player_array[i].tank.obj->faces; j++ ) {
                                world_stuff.player_array[i].tank.obj->face[j].transparent = TRUE;
                            }

                            /* suck a missile */

                            world_stuff.player_array[i].tank.missiles_stored--;

                            /* set frames_till_cloak_suck */

                            world_stuff.player_array[i].tank.frames_till_cloak_suck = world_stuff.player_array[i].tank.cloak_time;
                        }
                        else if( (world_stuff.player_array[i].tank.frames_till_cloak == 0) &&
                                 (world_stuff.player_array[i].tank.cloaking_active == TRUE) ) {

                            /* deactivate cloaking */

                            world_stuff.player_array[i].tank.cloaking_active = FALSE;
                            world_stuff.player_array[i].tank.frames_till_cloak = world_stuff.player_array[i].tank.cloak_reload_time;

                            /* make this tanks faces opaque */

                            for( j = 0; j < world_stuff.player_array[i].tank.obj->faces; j++ ) {
                                world_stuff.player_array[i].tank.obj->face[j].transparent = FALSE;
                            }

                            world_stuff.player_array[i].tank.frames_till_cloak_suck = 0;
                        }
                    }
                }
                else if( (world_stuff.player_array[i].tank.frames_till_cloak == 0) &&
                         (world_stuff.player_array[i].tank.cloaking_active == TRUE) ) {

                    /* deactivate cloaking */

                    world_stuff.player_array[i].tank.cloaking_active = FALSE;
                    world_stuff.player_array[i].tank.frames_till_cloak = world_stuff.player_array[i].tank.cloak_reload_time;

                    /* make this tanks faces opaque */

                    for( j = 0; j < world_stuff.player_array[i].tank.obj->faces; j++ ) {
                        world_stuff.player_array[i].tank.obj->face[j].transparent = FALSE;
                    }

                    world_stuff.player_array[i].tank.frames_till_cloak_suck = 0;
                }
            }

            /* regenerate missile stuff */

            if( world_stuff.player_array[i].tank.frames_till_fire_laser > 0 ) {
                world_stuff.player_array[i].tank.frames_till_fire_laser--;
            }
            if( world_stuff.player_array[i].tank.frames_till_fire_missile > 0 ) {
                world_stuff.player_array[i].tank.frames_till_fire_missile--;
            }

            /* turn off missile regeneration when cloaking is on */

            if( (world_stuff.player_array[i].tank.cloaking_active == FALSE) &&
                (world_stuff.player_array[i].tank.frames_till_new_missile > 0) ) {
                world_stuff.player_array[i].tank.frames_till_new_missile--;
            }
            if( world_stuff.player_array[i].tank.frames_till_new_missile == 0 ) {
                world_stuff.player_array[i].tank.frames_till_new_missile = world_stuff.player_array[i].tank.missile_generation_time;
                if( world_stuff.player_array[i].tank.missiles_stored < world_stuff.player_array[i].tank.max_missile_storage ) {
                    world_stuff.player_array[i].tank.missiles_stored++;
                }
            }

            /* decrement cloak stuff */

            if( (world_stuff.player_array[i].tank.frames_till_cloak > 0) ) {
                world_stuff.player_array[i].tank.frames_till_cloak--;
            }

            if( world_stuff.player_array[i].tank.cloaking_active &&
                (world_stuff.player_array[i].tank.frames_till_cloak_suck > 0) ) {
                world_stuff.player_array[i].tank.frames_till_cloak_suck--;
            }

            if( world_stuff.player_array[i].tank.cloaking_active &&
                (world_stuff.player_array[i].tank.frames_till_cloak_suck <= 0) &&
                (world_stuff.player_array[i].tank.missiles_stored > 0) ) {
                world_stuff.player_array[i].tank.missiles_stored--;
                world_stuff.player_array[i].tank.frames_till_cloak_suck = world_stuff.player_array[i].tank.cloak_time;
            }
            else if( world_stuff.player_array[i].tank.cloaking_active &&
                     (world_stuff.player_array[i].tank.frames_till_cloak_suck <= 0) &&
                     (world_stuff.player_array[i].tank.missiles_stored == 0) ) {

                /* deactivate cloaking */

                world_stuff.player_array[i].tank.cloaking_active = FALSE;
                world_stuff.player_array[i].tank.frames_till_cloak = world_stuff.player_array[i].tank.cloak_reload_time;

                /* make this tanks faces opaque */

                for( j = 0; j < world_stuff.player_array[i].tank.obj->faces; j++ ) {
                    world_stuff.player_array[i].tank.obj->face[j].transparent = FALSE;
                }
            }
        }
    }

    /* move the radar bases projectiles forward */

    move_base_projectiles_forward( &(world_stuff.red_radar_base), &(world_stuff.pylons), world_stuff.player_array );
    move_base_projectiles_forward( &(world_stuff.blue_radar_base), &(world_stuff.pylons), world_stuff.player_array );

    update_energy( &(world_stuff.energy_square), world_stuff.player_array );

    /* Figure out the strength of each team
       based on the number of pylons they own */

#ifdef PROFILE
    if( profile ) {
        printf("Tally_Pylons(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    Tally_Pylons( world_stuff.player_array, &(world_stuff.pylons) );

    anti_missile_update( &world_stuff );
} //move_everything_one_click


void Tony_Readout( void )
    {
	/*
     string temp_string;
     _go32_dpmi_meminfo info;      //used to get memory information 

     _go32_dpmi_get_free_memory_information( &info );

     sprintf( temp_string, "P_MEM=%lu,V_MEM=%lu",
              _go32_dpmi_remaining_physical_memory(),
              _go32_dpmi_remaining_virtual_memory() );

     micro_string_blit( temp_string, 97, 7, menu_stuff.micro_text.buffer,
                        menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );
*/
} /* ENd of Tony_Readout */

void John_Readout( void )
    {
	/*
     int i;
     string command_string[6];
     string state_string[6];
     string path_string[6];
     string leader_string;
     string frames_string;
     string temp_string;
     _go32_dpmi_meminfo info;      // used to get memory information 




     _go32_dpmi_get_free_memory_information( &info );

     
     sprintf( temp_string, "P_MEM=%lu,V_MEM=%lu",
              _go32_dpmi_remaining_physical_memory(),
              _go32_dpmi_remaining_virtual_memory() );

     micro_string_blit( temp_string, 97, 100, menu_stuff.micro_text.buffer,
                        menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );

     

     
     //sprintf( temp_string, "FIRED %d,HITS %d,MISSED %d,KILLS %d",
     //         game_stats.shots_fired, game_stats.enemy_hits, game_stats.misses, game_stats.kills );

     //micro_string_blit( temp_string, 1, 100, menu_stuff.micro_text.buffer,
     //                   menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );


     //sprintf(temp_string, "HIT %d,GRABBED %d,FRIENDLY%d,DEATHS %d",
     //        game_stats.times_hit, game_stats.pylons_grabbed, game_stats.friendly_hits, game_stats.times_killed );

     //micro_string_blit( temp_string, 1, 120, menu_stuff.micro_text.buffer,
     //                   menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );
     

     sprintf( temp_string, "NUM SAMPLES PLAYING %d",num_playing );

     micro_string_blit( temp_string, 97, 115, menu_stuff.micro_text.buffer,
                        menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );


    for( i = 0; i < 6; i++ )
        {
         if( world_stuff.player_array[i].tank.alive )
             Sprint_State( state_string[i], world_stuff.player_array[i].character.state, i );
         else
             sprintf(state_string[i], "DEAD! %d", i );

         sprintf(frames_string, "%d", player_events[i].memory.command_active );
         micro_string_blit( frames_string, 97, i * 7, menu_stuff.micro_text.buffer,
                            menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );

         if( world_stuff.player_array[i].leader )
             {
              sprintf(leader_string, "L" );
              micro_string_blit( leader_string, 92, i * 7, menu_stuff.micro_text.buffer,
                                 menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );

             }

         micro_string_blit( state_string[i], 1, i * 7, menu_stuff.micro_text.buffer,
             menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );
        }


    for( i = 0; i < 6; i++ )
         {
          Sprint_Command( command_string[i], player_events[i].memory.command );
          micro_string_blit( command_string[i], 110, i * 7, menu_stuff.micro_text.buffer,
                             menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );
         }



    for( i = 0; i < 6; i++ )
        {
         sprintf(path_string[i], "CE%d STK%d",  player_events[i].memory.current_enemy, player_events[i].memory.stuck);

         micro_string_blit( path_string[i], 10, i * 7 + 100, menu_stuff.micro_text.buffer,
             menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );

        }

    if( ceiling_on )
        {
         sprintf(path_string[0], "CEILING ON %ld CLICKS", Check_Timer());

         micro_string_blit( path_string[0], 10, 180, menu_stuff.micro_text.buffer,
             menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );

        }

    for( i = 0; i < 6; i++ )
        {

         if( player_events[i].memory.surface )
                 sprintf(path_string[i], "?%d #%d L%d S%d",  player_events[i].memory.following_path,
                         player_events[i].memory.num_paths_2D, player_events[i].memory.path_index_2D,
                         player_events[i].memory.surface);
         else
                 sprintf(path_string[i], "?%d #%d L%d S%d",  player_events[i].memory.following_path,
                         player_events[i].memory.num_paths_3D, player_events[i].memory.path_index_3D,
                         player_events[i].memory.surface );

         micro_string_blit( path_string[i], 220, i * 7, menu_stuff.micro_text.buffer,
             menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 191 );

        }

*/
} /* End of John_Readout */

void draw_fps()
{    
	char fps_str[64];
	static float time;
	float now, dt;

	now = SYS_GetTimeSeconds();
	dt = now - time;
	time = now;
	sprintf( fps_str, "FPS = %.1f", 1/dt );

	micro_string_blit( fps_str, 3, 3, menu_stuff.micro_text.buffer, 
		menu_stuff.micro_text.xpixels + 1, menu_stuff.micro_text.ypixels + 1, 2 );
}

void draw_everything( long view_vehicle, long status_bar,
                      long first_person_view, long transporting )
{
	long i;
	int nLoop;
	enum TubeType tubeType;	

#ifdef GLCYLINDRIX
	GL_Clear();			   // clear the screen.
#endif

	//This loop should loop once on 1 player, twice on two player 
	for( nLoop = 0; nLoop <= game_configuration.two_player; nLoop++ ) {

		if( nLoop == 0 ) {
			//Johnm - working on doing split screen rendering
			view_vehicle = user_vehicle_index();
		}
		else {
			//Johnm - working on doing split screen rendering
			view_vehicle = 3;
			world_stuff.view_orientation = world_stuff.player_array[view_vehicle].tank.orient;
		}
		//world_stuff.view_orientation = world_stuff.player_array[view_vehicle].tank.orient; //NOT quite working right! 
		//Johnm - end of new stuff

		/* Clear the double buffer */
		DB_Clear_Screen();

		animate_explosions();

#ifndef GLCYLINDRIX
		/* draw the tube into the double buffer */
		if( game_configuration.tube_detail_level == HIGH_DETAIL ||
		game_configuration.tube_detail_level == MEDIUM_DETAIL ) {
			if( level.wire_tube == FALSE ) {
				view_tube( &(world_stuff.tube), &(world_stuff.view_orientation), world_stuff.light );
			}
			else {
				view_wire_tube( &(world_stuff.wire_tube), &(world_stuff.view_orientation), world_stuff.light );
			}
		}
		else if( game_configuration.tube_detail_level == LOW_DETAIL ) {
			view_point_tube( world_stuff.tube.point, world_stuff.tube.points, &(world_stuff.view_orientation), world_stuff.light );
		}
#else
		if ( renderSoftwareBuffer )
		{
			/* draw the tube into the double buffer */
			if( game_configuration.tube_detail_level == HIGH_DETAIL ||
			game_configuration.tube_detail_level == MEDIUM_DETAIL ) {
				if( level.wire_tube == FALSE ) {
					view_tube( &(world_stuff.tube), &(world_stuff.view_orientation), world_stuff.light );
				}
				else {
					view_wire_tube( &(world_stuff.wire_tube), &(world_stuff.view_orientation), world_stuff.light );
				}
			}
			else if( game_configuration.tube_detail_level == LOW_DETAIL ) {
				view_point_tube( world_stuff.tube.point, world_stuff.tube.points, &(world_stuff.view_orientation), world_stuff.light );
			}
		}
		else
		{
			// figure out what type of tube we should draw.
			if ( game_configuration.tube_detail_level == HIGH_DETAIL ||
				 game_configuration.tube_detail_level == MEDIUM_DETAIL ) 
			{
				if ( level.wire_tube == FALSE ) 			
					tubeType = CheckerTube;			
				else
					tubeType = WireTube;
			}
			else if( game_configuration.tube_detail_level == LOW_DETAIL ) 
			{
				tubeType = PointTube;
			}

			// AJT: draw the tube into the GL renderwindow.
			GL_DrawTube( &(world_stuff), tubeType );
		}

#endif

		/* transform the vehicles and projectiles into world coords and
		combine them in result */

		free_object( &(world_stuff.everything) );

		/* insert energy square object into result */
		union_object( world_stuff.energy_square.world_obj, &(world_stuff.everything) );

		for( i = 0; i < 6; i++ ) {
			if( first_person_view ) {
				init_world_obj( &(world_stuff.player_array[i].tank) );
				if( world_stuff.player_array[i].tank.alive && (i != view_vehicle) ) {
					union_object( world_stuff.player_array[i].tank.world_obj, &(world_stuff.everything) );
				}
				projectile_transform_and_union( world_stuff.player_array[i].tank.projectile_list, &(world_stuff.everything) );
			}
			else {
				if( world_stuff.player_array[i].tank.alive ) {

				init_world_obj( &(world_stuff.player_array[i].tank) );
				union_object( world_stuff.player_array[i].tank.world_obj, &(world_stuff.everything) );

				}
				projectile_transform_and_union( world_stuff.player_array[i].tank.projectile_list, &(world_stuff.everything) );
			}
		}

		/* add all of the radar_base projectiles to result */
		projectile_transform_and_union( world_stuff.red_radar_base.projectile_list, &(world_stuff.everything) );
		projectile_transform_and_union( world_stuff.blue_radar_base.projectile_list, &(world_stuff.everything) );

		/* add all of the pylons to result */
		pylon_union( &(world_stuff.pylons), &(world_stuff.everything) );

		/* add the radar bases to result */
		if( world_stuff.red_radar_base.alive ) {
			union_object( world_stuff.red_radar_base.world_union_obj, &(world_stuff.everything) );
		}
		if( world_stuff.blue_radar_base.alive ) {
			union_object( world_stuff.blue_radar_base.world_union_obj, &(world_stuff.everything) );
		}

		/* put all the explosion objects into world_stuff.everything */
		explosion_union( &(world_stuff.everything) );

		/* insert the hubs into world_stuff.everything */
		if( game_configuration.hubs_on ) {
			union_object( &world_stuff.north_hub, &(world_stuff.everything) );
			union_object( &world_stuff.south_hub, &(world_stuff.everything) );
		}

#ifndef GLCYLINDRIX
		/* draw result in the double buffer (on top of the tube) */
		view_object( &(world_stuff.everything), &(world_stuff.view_orientation), world_stuff.light );
#else
		if ( renderSoftwareBuffer )
		{
			/* draw result in the double buffer (on top of the tube) */
			view_object( &(world_stuff.everything), &(world_stuff.view_orientation), world_stuff.light );
		}
		else
		{
			GL_DrawEverything( &world_stuff );
		}
#endif

		/* draw one pixel border round the screen to hide the clipping
		error problem :) */

		draw_border();
		/* draw hud */

		if( !transporting ) {
			if( world_stuff.player_array[view_vehicle].tank.vtype == Beetle && first_person_view ) {
				draw_hud( Beetle, &world_stuff.beetle_hud, world_stuff.player_array, view_vehicle,
				&(world_stuff.wire_tinytube), &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
			}
			else if( world_stuff.player_array[view_vehicle].tank.vtype == Wasp && first_person_view ) {
				draw_hud( Wasp, &world_stuff.wasp_hud, world_stuff.player_array, view_vehicle,
				&(world_stuff.wire_tinytube), &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
			}
			else if( world_stuff.player_array[view_vehicle].tank.vtype == Flea && first_person_view ) {
				draw_hud( Flea, &world_stuff.flea_hud, world_stuff.player_array, view_vehicle,
				&(world_stuff.wire_tinytube), &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
			}
			else if( world_stuff.player_array[view_vehicle].tank.vtype == Mosquito && first_person_view ) {
				draw_hud( Mosquito, &world_stuff.mosquito_hud, world_stuff.player_array, view_vehicle,
				&(world_stuff.wire_tinytube), &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
			}
			else if( world_stuff.player_array[view_vehicle].tank.vtype == Spider && first_person_view ) {
				draw_hud( Spider, &world_stuff.spider_hud, world_stuff.player_array, view_vehicle,
				&(world_stuff.wire_tinytube), &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
			}
			else if( world_stuff.player_array[view_vehicle].tank.vtype == Dragonfly && first_person_view ) {
				draw_hud( Dragonfly, &world_stuff.dragonfly_hud, world_stuff.player_array, view_vehicle,
				&(world_stuff.wire_tinytube), &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
			}
			else if( world_stuff.player_array[view_vehicle].tank.vtype == Roach && first_person_view ) {
				draw_hud( Roach, &world_stuff.roach_hud, world_stuff.player_array, view_vehicle,
				&(world_stuff.wire_tinytube), &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
			}
			else if( world_stuff.player_array[view_vehicle].tank.vtype == Locust && first_person_view ) {
				draw_hud( Locust, &world_stuff.locust_hud, world_stuff.player_array, view_vehicle,
				&(world_stuff.wire_tinytube), &(world_stuff.red_radar_base), &(world_stuff.blue_radar_base) );
			}
			else {
				/* !first_person_view */
				draw_third_person_hud( world_stuff.player_array, view_vehicle, &(world_stuff.red_radar_base),
				   &(world_stuff.blue_radar_base) );
			}
		} //!transporting

		draw_fps();

		if( show_readout )
			if( status_bar ) {
				John_Readout();
			}

		if( game_configuration.two_player && nLoop == 0 ) 
			g_bRenderingFirstPlayer = 1;
		if( game_configuration.two_player && nLoop == 1 ) 
			g_bRenderingSecondPlayer = 1;

#ifndef GLCYLINDRIX
		/* draw the contents of the double buffer into video mem */
		Swap_Buffer();     /* Copy the double buffer into the video buffer */
#else
		if ( renderSoftwareBuffer )
		{
			Swap_Buffer();
		}
		else
		{
			GL_SwapBuffers();
		}
#endif

		g_bRenderingFirstPlayer = 0;
		g_bRenderingSecondPlayer = 0;

		if( game_configuration.two_player ) { 
			//Johnm - put the view orientation back the way it was...weird stuff was happening...
			view_vehicle = user_vehicle_index();
			world_stuff.view_orientation = world_stuff.player_array[view_vehicle].tank.orient;
		}
	} //Johnm for

} //Draw_Everything



void free_pointface( PointFace *obj )
{
    free( obj->point );
    obj->point = NULL;

    free( obj->face );
    obj->face = NULL;

    obj->points = 0;
    obj->faces = 0;
}

void free_pointedge( PointEdge *obj )
{
    free( obj->point );
    obj->point = NULL;

    free( obj->edge );
    obj->edge = NULL;

    obj->points = 0;
    obj->edges = 0;
}

void free_projectile_list( Projectile *projectile_list )
{
    Projectile *ptr, *next;

    ptr = projectile_list;

    while( ptr != NULL ) {

       next = ptr->next;

       free_pointface( ptr->obj );
       free( ptr->obj );
       ptr->obj = NULL;

       free_pointface( ptr->world_obj );
       free( ptr->world_obj );
       ptr->world_obj = NULL;
       
       free( ptr->edge_table.edge );
       ptr->edge_table.edge = NULL;
       ptr->edge_table.edges = 0;

       free( ptr );

       ptr = next;
    }
}

void free_radar_base( RadarBase *base )
{
    free_pointface( base->base_obj );
    free( base->base_obj );
    base->base_obj = NULL;

    free_pointface( base->turret_obj );
    free( base->turret_obj );
    base->turret_obj = NULL;

    free_pointface( base->world_base_obj );
    free( base->world_base_obj );
    base->world_base_obj = NULL;

    free_pointface( base->world_turret_obj );
    free( base->world_turret_obj );
    base->world_turret_obj = NULL;

    free_pointface( base->world_union_obj );
    free( base->world_union_obj );
    base->world_union_obj = NULL;

    free( base->edge_table.edge );
    base->edge_table.edge = NULL;
    base->edge_table.edges = 0;

    free_projectile_list( base->projectile_list );
    base->projectile_list = NULL;
}

void free_pylons( Pylons *pylons )
{
    long i;

    for( i = 0; i < pylons->pylons; i++ ) {
        free_pointface( &(pylons->pylon[i].obj) );
    }

    free( pylons->pylon );
    pylons->pylons = 0;
}

void free_vehicle( Vehicle *v )
{
    free_pointface( v->obj );
    free( v->obj );
    v->obj = NULL;

    free_pointface( v->world_obj );
    free( v->world_obj );
    v->world_obj = NULL;

    free_pointface( v->collision_obj );
    free( v->collision_obj );
    v->collision_obj = NULL;

    free_pointface( v->world_collision_obj );
    free( v->world_collision_obj );
    v->world_collision_obj = NULL;

    free( v->collision_edges.edge );
    v->collision_edges.edge = NULL;
    v->collision_edges.edges = 0;

    free_projectile_list( v->projectile_list );
    v->projectile_list = NULL;
}

void free_player( Player *player )
{
    free_vehicle( &(player->tank) );
}

void free_energy( EnergyPowerUp *e )
{
    free_pointface( e->world_obj );
    free( e->world_obj );
    e->world_obj = NULL;
}

void free_explosions( Explosion *explosion_list )
{
    Explosion *ptr, *next;

    ptr = explosion_list;

    while( ptr != NULL ) {

       next = ptr->next;

       free_pointface( &ptr->debris );
       free_pointface( &ptr->world_debris );
       free_pointface( &ptr->cloud );
       free_pointface( &ptr->world_cloud );

       free( ptr );

       ptr = next;
    }
}

void free_pcx_picture( pcx_picture *pic )
{
	if( !pic ) {
		printf("null pic in free_pcx_picture\n");
		return;
	}
	if( !pic->buffer ) {
		printf("null pic->buffer in free_pcx_picture\n");
		return;
	}

    free( pic->buffer );
    pic->buffer = NULL;
}

void free_pixmap( PixMap *pixmap )
{
    free( pixmap->pixel );
    pixmap->pixel = NULL;
    pixmap->pixels = 0;
}

void free_world_stuff( WorldStuff *w )
{
    long i;

    reset_game_to_start( w, &level, &game_configuration, TRUE );

    free_pointface( &w->tube );
    free_pointedge( &w->wire_tube );
    free_pointedge( &w->wire_tinytube );
    free_pointface( &w->everything );
    free_pointface( &w->laser_obj );
    free_pointface( &w->missile_obj );
    free_pointface( &w->mine_obj );
    free_pointface( &w->cs_missile_obj );
    free_pointface( &w->traitor_missile_obj );
    free_pointface( &w->laser_debris_obj );
    free_pointface( &w->missile_debris_obj );
    free_pointface( &w->mine_debris_obj );
    free_pointface( &w->cs_missile_debris_obj );
    free_pointface( &w->traitor_missile_debris_obj );
    free_pointface( &w->ramming_debris_obj );
    free_pointface( &w->explosion_cloud_obj );
    free_pointface( &w->north_hub );
    free_pointface( &w->south_hub );

    free_radar_base( &w->red_radar_base );
    free_radar_base( &w->blue_radar_base );

    free_pylons( &w->pylons );

    for( i = 0; i < 6; i++ ) {
        free_player( &w->player_array[i] );
    }

    free_energy( &w->energy_square );

    free_explosions( w->explosion_list );
    w->explosion_list = NULL;
    
    free_pcx_picture( &w->text );

    free_pixmap( &w->beetle_hud );
    free_pixmap( &w->wasp_hud );
    free_pixmap( &w->spider_hud );
    free_pixmap( &w->flea_hud );
    free_pixmap( &w->roach_hud );
    free_pixmap( &w->locust_hud );
    free_pixmap( &w->dragonfly_hud );
    free_pixmap( &w->mosquito_hud );    
}

void free_menu_stuff( MenuStuff *menu_stuff )
{
printf("FIXME in main.c: not freeing pcx data in free_menu_stuff - was causing hang.\n");
return;

    free_pcx_picture( &menu_stuff->micro_text );
    free_pcx_picture( &menu_stuff->menu_text );
    free_pcx_picture( &menu_stuff->general_menu_background );
    free_pcx_picture( &menu_stuff->wingman_menu_background );
    free_pcx_picture( &menu_stuff->vehicle_menu_background );
    free_pcx_picture( &menu_stuff->human_1 );
    free_pcx_picture( &menu_stuff->human_2 );
    free_pcx_picture( &menu_stuff->human_3 );
    free_pcx_picture( &menu_stuff->human_4 );
    free_pcx_picture( &menu_stuff->overlords );
    free_pcx_picture( &menu_stuff->pharoahs );
    free_pcx_picture( &menu_stuff->scavengers );
    free_pcx_picture( &menu_stuff->slar );
    free_pcx_picture( &menu_stuff->succubi );
    free_pcx_picture( &menu_stuff->watchers );
    free_pcx_picture( &menu_stuff->bok );
    free_pcx_picture( &menu_stuff->biomech );
    free_pcx_picture( &menu_stuff->sentry );
}

void empty_snapshot_queue( SnapshotQueue *snapshot_queue )
{
    snapshot_queue->size = 0;
}

int num_vehicles_remaining( team_type team )
{
    int i;
    int count = 0;

    for( i = 0; i < 6; i++ ) {
        if( world_stuff.player_array[i].tank.alive && (world_stuff.player_array[i].tank.team == team) ) {
            count++;
        }
    }
    return count;
}

/* Move the current view_orientation in world_stuff toward the next vehicle of that team.
   It returns true when it reaches it's destination, and updates the view_vehicle to be
   the index of the player's whose vehicle we are commendeering. */

int transport_towards( long *view_vehicle, WorldStuff *world_stuff )
{
    Orientation *target;
    long target_vehicle_index = 0;
    long i;
    float len;
    Float_Vector temp;
    Float_Vector axis;
    float theta = 0;
    Float_Matrix trans;
    const float epsilon = 0.005f;
    const float translation_increment = 0.5f;
    const float rotation_increment = 0.349066f; /* 20 degrees */

    /* find the target orientation */

    target = NULL;

    for( i = 0; i < 6; i++ ) {
        if( world_stuff->player_array[i].team == world_stuff->player_array[*view_vehicle].team ) {
            if( world_stuff->player_array[i].tank.alive ) {
                target_vehicle_index = i;
                target = &(world_stuff->player_array[i].tank.orient);
                break;
            }
        }
    }

    if( target == NULL ) {

        /* no living vehicles found */

        return FALSE;
    }

    temp[X] = target->position[X] - world_stuff->view_orientation.position[X];
    temp[Y] = target->position[Y] - world_stuff->view_orientation.position[Y];
    temp[Z] = target->position[Z] - world_stuff->view_orientation.position[Z];

    if( (temp[X] < epsilon && temp[X] > -epsilon) &&
        (temp[Y] < epsilon && temp[Y] > -epsilon) &&
        (temp[Z] < epsilon && temp[Z] > -epsilon) ) {

        /* view_orientation has arived at destination */

        *view_vehicle = target_vehicle_index;

        return FALSE;
    }

    /* move view_orientation towards target */

    len = myvector_length( temp );

    if( fabs( temp[X] ) > translation_increment ) {
        world_stuff->view_orientation.position[X] += temp[X] / len * translation_increment;
    }
    else {
        world_stuff->view_orientation.position[X] = target->position[X];
    }

    if( fabs( temp[Y] ) > translation_increment ) {
        world_stuff->view_orientation.position[Y] += temp[Y] / len * translation_increment;
    }
    else {
        world_stuff->view_orientation.position[Y] = target->position[Y];
    }

    if( fabs( temp[Z] ) > translation_increment ) {
        world_stuff->view_orientation.position[Z] += temp[Z] / len * translation_increment;
    }
    else {
        world_stuff->view_orientation.position[Z] = target->position[Z];
    }

    /* rotate the view_orientation towards temp */

    normalize( temp );

    theta = angle_between( temp, world_stuff->view_orientation.front );

    if( theta > rotation_increment ) {
        theta = rotation_increment;
    }

    vector_cross_product( temp, world_stuff->view_orientation.front, axis );
    
    if( vector_dot_product( axis, axis ) == 0.0 ) {
        identity_matrix_init( trans );    
    }
    else {
        rotate_around_axis( theta, axis, trans );
    }

    transformation( trans, world_stuff->view_orientation.front );
    transformation( trans, world_stuff->view_orientation.up );

    temp[X] = target->position[X] - world_stuff->view_orientation.position[X];
    temp[Y] = target->position[Y] - world_stuff->view_orientation.position[Y];
    temp[Z] = target->position[Z] - world_stuff->view_orientation.position[Z];

    if( (temp[X] < epsilon && temp[X] > -epsilon) &&
        (temp[Y] < epsilon && temp[Y] > -epsilon) &&
        (temp[Z] < epsilon && temp[Z] > -epsilon) ) {

        /* view_orientation has arived at destination */

        *view_vehicle = target_vehicle_index;

        return FALSE;
    }
    else {
        return TRUE;
    }
} //transport_towards


void get_current_view_orient( Orientation *view_o, Orientation *old_o, team_type team, int *transporting,
                              int *first_person_view )
{

    if( (world_stuff.player_array[user_vehicle_index()].tank.alive == FALSE) && !(*transporting) ) {

        /* start to transport the vehicle */

        *transporting = TRUE;

    }
    else if( (world_stuff.player_array[user_vehicle_index()].tank.alive == FALSE) && (*transporting) ) {

        /* continue to transport the vehicle */

        (*first_person_view) = FALSE;

        *transporting = transport_towards( &destination_vehicle_index, &world_stuff );

        if( !(*transporting) ) {

            if( world_stuff.player_array[destination_vehicle_index].tank.alive ) {

                *first_person_view = TRUE;

                /* take away user control of the old dead vehicle */

                world_stuff.player_array[user_vehicle_index()].controller = NO_CONTROL;

                /* give user control to the new commendeered vehicle */

                world_stuff.player_array[destination_vehicle_index].controller = USER_CONTROL;
            }
        }
    }
    else {  /* user_vehicle_index() tank is alive */

        if( (*first_person_view) && !(*transporting) ) {

            /* copy the user_vehicle's orientation into the view_orient o */

            memcpy( view_o, &(world_stuff.player_array[user_vehicle_index()].tank.orient), sizeof( Orientation ) );

            memcpy( old_o, view_o, sizeof( Orientation ) );

            old_o->position[X] += 0.9f * view_o->up[X] - 1.7f * view_o->front[X];
            old_o->position[Y] += 0.9f * view_o->up[Y] - 1.7f * view_o->front[Y];
            old_o->position[Z] += 0.9f * view_o->up[Z] - 1.7f * view_o->front[Z];

            destination_vehicle_index = user_vehicle_index();

        }
        else if( !(*first_person_view) && !(*transporting) ) {

            /* when in third person view we use the old_o to buffer the view_orient one frame */

            memcpy( view_o, old_o, sizeof( Orientation ) );

            memcpy( old_o, &(world_stuff.player_array[user_vehicle_index()].tank.orient), sizeof( Orientation ) );

            old_o->position[X] += 0.9f * view_o->up[X] - 1.7f * view_o->front[X];
            old_o->position[Y] += 0.9f * view_o->up[Y] - 1.7f * view_o->front[Y];
            old_o->position[Z] += 0.9f * view_o->up[Z] - 1.7f * view_o->front[Z];

            destination_vehicle_index = user_vehicle_index();
        }
    }
} //get_current_view_orient





void get_current_view_orient_during_fadeout( Orientation *view_o, Orientation *old_o, team_type team, int *transporting,
                                             int *first_person_view )
{
    static long destination_vehicle_index;
    

    if( (*first_person_view) && !(*transporting) ) {

        /* copy the user_vehicle's orientation into the view_orient o */
        memcpy( view_o, &(world_stuff.player_array[user_vehicle_index()].tank.orient), sizeof( Orientation ) );

        memcpy( old_o, view_o, sizeof( Orientation ) );

        old_o->position[X] += 0.9f * view_o->up[X] - 1.7f * view_o->front[X];
        old_o->position[Y] += 0.9f * view_o->up[Y] - 1.7f * view_o->front[Y];
        old_o->position[Z] += 0.9f * view_o->up[Z] - 1.7f * view_o->front[Z];

        destination_vehicle_index = user_vehicle_index();

    }
    else if( !(*first_person_view) && !(*transporting) ) {

        /* when in third person view we use the old_o to buffer the view_orient one frame */
        memcpy( view_o, old_o, sizeof( Orientation ) );

        memcpy( old_o, &(world_stuff.player_array[user_vehicle_index()].tank.orient), sizeof( Orientation ) );

        old_o->position[X] += 0.9f * view_o->up[X] - 1.7f * view_o->front[X];
        old_o->position[Y] += 0.9f * view_o->up[Y] - 1.7f * view_o->front[Y];
        old_o->position[Z] += 0.9f * view_o->up[Z] - 1.7f * view_o->front[Z];

        destination_vehicle_index = user_vehicle_index();
    }
} //get_current_view_orient_during_fadeout


int is_round_over()
{
    if( (num_vehicles_remaining( RED_TEAM ) <= 0) ||
        (num_vehicles_remaining( BLUE_TEAM ) <= 0) ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/* This function resets all information in the player_array and both radar
   basesto get ready for another round of play.  It uses information from
   the game_configuration and level structs */

void reset_game_to_start( WorldStuff *world_stuff, level_type *level,
                          game_configuration_type *game_config, int master )
{
    int i, j;
    Player *p; /* alias to world_stuff->player_array */
    
    p = world_stuff->player_array;

    /* set all players controller fields */
    init_player_array_controller_info( p, game_config, master );

    /* clear all players ai */
    Clear_Ai();

    /* make sure the correct vehicles are leaders */
    update_leader( user_vehicle_index() );

    for( i = 0; i < 6; i++ ) {

        /* clear this players input table */
        memset( p[i].table, 0, INPUT_TABLE_SIZE );

        /* make sure this vehicle is on the surface */
        p[i].tank.vehicle_mode = Surface;

        /* make sure this vehicle is alive */
        p[i].tank.alive = TRUE;

        /* reset this vehicles orientation */
        p[i].tank.orient = level->vehicle_orientations[i];
        
        /* make sure all the faces of this vehicle are opaque */
        for( j = 0; j < p[i].tank.obj->faces; j++ ) {            
            p[i].tank.obj->face[j].transparent = FALSE;
        }

        /* init this vehicles world_obj according to the new orient */
        init_world_obj( &(p[i].tank) );

        /* init this vehicles world_collision_obj according to the new
           orientation */
        init_world_collision_obj( &(p[i].tank) );

        /* init this vehicles bounding box */
        init_bounding_box( &(p[i].tank) );

        /* init this vehicle velocity */
        p[i].tank.vel[X] = 0.0;
        p[i].tank.vel[Y] = 0.0;
        p[i].tank.vel[Z] = 0.0;

        /* make sure the missiles aren't locked on anything */
        p[i].tank.target = NULL;

        /* set it so they can fire their lasers in the first frame */
        p[i].tank.frames_till_fire_laser = 0;

        /* set it so they can fire a missile in the first frame */
        p[i].tank.frames_till_fire_missile = 0;

        /* set the time until a new missile is generated to the maximum */
        p[i].tank.frames_till_new_missile = p[i].tank.missile_generation_time;

        /* give them a full load of missiles */
        p[i].tank.missiles_stored = p[i].tank.max_missile_storage;

        /* free all active projectiles */
        free_projectile_list( p[i].tank.projectile_list );
        p[i].tank.projectile_list = NULL;

        /* give the vehicle full sheilds */
        p[i].tank.current_hitpoints = p[i].tank.max_hitpoints;

        /* make sure this vehicle controls aren't scrambled */
        p[i].tank.controls_scrambled = FALSE;
        p[i].tank.frames_till_unscramble = 0;

        /* make sure this vehicle isn't a traitor */
        p[i].tank.traitor_active = FALSE;
        p[i].tank.frames_till_traitor_deactivate = 0;

        /* make sure this vehicles isn't cloaked */
        p[i].tank.cloaking_active = FALSE;
        p[i].tank.frames_till_cloak_suck = p[i].tank.cloak_time;        
        
    }

    /* reset the radar bases orienations */
    world_stuff->red_radar_base.base_orient = level->base_orientations[0];
    world_stuff->blue_radar_base.base_orient = level->base_orientations[1];

    world_stuff->red_radar_base.turret_orient = level->turret_orientations[0];
    world_stuff->blue_radar_base.turret_orient = level->turret_orientations[1];

    /* init the world_base_obj, world_turret_obj, world_union_obj according
       to the new orientations */
    orient_object( world_stuff->red_radar_base.base_obj,
                   &(world_stuff->red_radar_base.base_orient),
                   world_stuff->red_radar_base.world_base_obj );

    orient_object( world_stuff->blue_radar_base.base_obj,
                   &(world_stuff->blue_radar_base.base_orient),
                   world_stuff->blue_radar_base.world_base_obj );

    orient_object( world_stuff->red_radar_base.turret_obj,
                   &(world_stuff->red_radar_base.turret_orient),
                   world_stuff->red_radar_base.world_turret_obj );

    orient_object( world_stuff->blue_radar_base.turret_obj,
                   &(world_stuff->blue_radar_base.turret_orient),
                   world_stuff->blue_radar_base.world_turret_obj );

    object_copy( world_stuff->red_radar_base.world_union_obj,
                 world_stuff->red_radar_base.world_base_obj );
    union_object( world_stuff->red_radar_base.world_turret_obj,
                  world_stuff->red_radar_base.world_union_obj );

    object_copy( world_stuff->blue_radar_base.world_union_obj,
                 world_stuff->blue_radar_base.world_base_obj );
    union_object( world_stuff->blue_radar_base.world_turret_obj,
                  world_stuff->blue_radar_base.world_union_obj );

    /* init the bounding box */
    find_bounding_box( world_stuff->red_radar_base.world_union_obj,
                       &(world_stuff->red_radar_base.box) );

    find_bounding_box( world_stuff->blue_radar_base.world_union_obj,
                       &(world_stuff->blue_radar_base.box) );

    /* give both radar bases full sheilds */
    world_stuff->red_radar_base.current_hitpoints = world_stuff->red_radar_base.max_hitpoints;
    world_stuff->blue_radar_base.current_hitpoints = world_stuff->blue_radar_base.max_hitpoints;

    /* make sure the radar bases are alive */
    world_stuff->red_radar_base.alive = TRUE;
    world_stuff->blue_radar_base.alive = TRUE;

    /* set the radar_base target to null (just in case later we make radar
       bases shoot missiles) */
    world_stuff->red_radar_base.target = NULL;
    world_stuff->blue_radar_base.target = NULL;

    /* free all the radar_bases projectiles */
    free_projectile_list( world_stuff->red_radar_base.projectile_list );
    world_stuff->red_radar_base.projectile_list = NULL;

    free_projectile_list( world_stuff->blue_radar_base.projectile_list );
    world_stuff->blue_radar_base.projectile_list = NULL;

    /* make sure the radar base can fire on the first frame */
    world_stuff->red_radar_base.frames_till_fire_laser = 0;
    world_stuff->blue_radar_base.frames_till_fire_laser = 0;

    /* free all current explosions */
    free_explosions( world_stuff->explosion_list );
    world_stuff->explosion_list = NULL;

    /* reset all the pylons to neutral */
    for( i = 0; i < world_stuff->pylons.pylons; i++ ) {

        /* reset this pylons team to NO_TEAM */
        world_stuff->pylons.pylon[i].owner = NO_TEAM;

        /* reset this pylons color to NeutralPylonGrad */
        change_pylon_color( &(world_stuff->pylons.pylon[i]), NeutralPylonGrad );

    }
} //reset_game_to_start

void free_all_samples( WorldStuff *world_stuff )
{
    long i, j;
    
    /* free all the computer samples */
    for( i = 0; i < MAX_COMPUTER_SAMPLES; i++ ) {
        if( computer_sample[i] != 0 ) {
            SYS_ReleaseSound( computer_sample[i] );
            computer_sample[i] = 0;
        }
    }
    
    /* free all of the world samples */
    for( i = 0; i < MAX_WORLD_SAMPLES; i++ ) {
        if( sample[i] != 0 ) {
            SYS_ReleaseSound( sample[i] );
            sample[i] = 0;
        }
    }
    
    /* free all the character samples */
    for( i = 0; i < 6; i++ ) {    
        for( j = 0; j < NUMBER_CHARACTER_SOUNDS; j++ ) {
            if( world_stuff->player_array[i].character.samples[j] != 0 ) {
                SYS_ReleaseSound( world_stuff->player_array[i].character.samples[j] );
                world_stuff->player_array[i].character.samples[j] = 0;
            }
        }
    }    
} //free_all_samples

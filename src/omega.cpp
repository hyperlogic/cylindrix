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

/*      
  omega.c : The main file for Cylindrix V1.0

  Programmers:     Anthony Thibault     (HyperLogic)
                   John R. McCawley III (Cap'n Hardgeus)

  History:

  June 18-20 : Modified code so that you can move on the inner surface of
               the cylinder.

  June 21-24 : Cleaned up code and moved all of the drawing crap out of
               main file and put it in object.h & object.c.

  June 25 : Improved data structures in types.h.

  July 1 - 18 : Added collision detection, multiple tanks, serial linkup,
                projectiles, pylons.

  July 18 - 31 : Added rudimentary AI, projectiles are deleted when they hit
                 pylons, projectiles inflict damage, can't fly out of cylinder,
                 can bounce of pylons, cylinder and other tanks. FLI player.
                 CD-Rom music (sort of)!!

  August 1 - 7 : Increased frame rate by speeding up clipping and projectile
                 movement.  AI dodges bullets. More robust CD audio.

  August 7 - 30 : heat seeking missles added, variable fireing rates, energy
                  power up square, cool radar.  More robust game play (not
                  as many random crashes).  Ported to djgpp V2.0 (beta).
                  Added cool sound blaster sample player.  New wireframe
                  cylinder.  Smarter AI. (it kinda looks like a game now)
                  CD-Rom audio stopped working.

  September 1 - 20 : Smoother switching modes, grabing pylons added, tanks
                     are now loaded from a .tbf file, primitive versions of
                     different tanks added. More robust FLI player. Contract
                     signed!! Yippie!

  September 20 - 28 : Radar turret added, mouse & joystick stuff working.

  October 1 - 5 : Neeto head up display complete with bars and target box,
                  more intuitive missile target selection, interpolative
                  shading added, more flexable color palette gradient stuff.
                  Null modem play uses primitive packets.  Basic modem stuff
                  working.

  October 5 - 18 : Angular friction thing added, Smoother movement, more
                   realistic collisions.  Slow modem play added.  110 degree
                   field of view.

  October 18 - 30 : Final (I hope) version of movement and collisions
                    finished.  15% speed up in collision detection. Collisions
                    now tested against a simple collision object rather then
                    the complex vehicle object.

  November 1 - 30 : Added level.dat files, game.cfg file, stereo depth
                    sensitive sound effects, mod player, killer 3d AI and
                    a funky text menu to set it up for its alpha version.
                    New features include: bad ass explosions, can't land in
                    pylons anymore, phat cylinder hubs, stylin trailing
                    camera view, spooky etherial transportation feature.
                    Inserted Josh's cyber sound effects & 4 mod files.
                    Added a bunch of different ai's.

  December 1 - January 26 :  Beta day came and went. Put in parts of the menu
                             system.  Added wingman selection screen & vehicle
                             selection screen.  CD rom is out of the way. Most
                             of the animations are done.  The music is almost
                             finished and the ad kicks ass. And lots of other
                             stuff. huh huh. Cleaned up main.
                             
  January 26 - Febuary 13 :  Duke Nuke'um 3D came out,.........2 weeks wasted.
                             We just received Goon's ill funk CD from DMI. Added Ipx game,
                             it's still kinda buggy though.  Null-Modem game works. Added cool
                             intro screens.  Got rid (I hope) of the div by zero bugs.  Made
                             the ships a little more balanced.  We should have the demo done by
                             today. (Wink Wink).
                            
  Febuary 13 - 17 : Demo completed, sent to CGW and PCGamer.  Cool order screens done by mike.
                    Tape of BioMechs and Sentry erased.  Ipx game fixed.  Win95 Timer problem
                    solved.  The Game should be complete by Monday.  Things are lookin good.
                    
  Febuary 17 - 28 : Needless to say the game was not complete by Monday, but soon after.
  		    Was sent off to DMI on Feb 23, received back on Feb 28.  Noticed several
  		    bugs: Worked fine under Win95 but crashed under DOS, Sometimes played the
  		    wrong menu song, sometimes cd music didnt work at all.
  		    
  Feburay 28 - March 3 : The Cd-rom now works on a variety of computers under windows95 & Dos.
                         It looks like this will be the first version 1.0. We'll send it of Monday!

  Last commented: March 3, 1996

*/

#include "types.h"     /* Basic data structures and defines for vector
                          graphics stuff */
#include "object.h"    /* stuff for manipulating objects and drawing them */
#include "movement.h"  /* functions for moving vehicles */
#include "collide.h"   /* collision and bounding box functions */
#include "project.h"   /* projectile creation and movement */
#include "prim.h"      /* graphics primitives */
#include "pylon.h"     /* functions dealing with pylons */
#include "keys.h"      /* functions dealing with keyboard interrupt */
#include "pcx.h"       /* pcx functions and data structures */
#include "text.h"      /* String blitter & letter blitter */

#include <stdio.h>     /* Standard Input and Output */
#include <string.h>    /* need strcmp */
#include <assert.h>
#include <ctype.h>     /* need toupper() */

#include "ai.h"        /* Stuff for ai control of players */
#include "voices.h"    /* Stuff for computer and wingmen voices */
#include "commands.h"  /* Stuff for wingmen commands */
#include "fli.h"       /* for playing .fli files */

#include "radar.h"     /* killer radar!! */
#include "base.h"      /* stuff dealing with the radar base */
#include "tanks.h"     /* fuctions for loading in tanks from a .tbf file */
#include "hud.h"       /* heads up display stuff */
#include "config.h"    /* Stuff to handle the game.cfg file */
#include "level.h"     /* Stuff for the level files */
#include "jonsb.h"     /* Stuff for deapth ququqauing of samples */
#include "explode.h"   /* Stuff for explosions */

#include "main.h"      /* stuff for main game loop */
#include "energy.h"    /* stuff for energy square */
#include "menu.h"      /* during the game menu */
#include "prim.h"      /* need to enable_palettes */
#include "fx.h"        /* need to dim_palettes */
#include "stats.h"     /* File for stat recording */

#include "stub.h"  // AJT: REMOVE

#ifdef GLCYLINDRIX
#include "glcylindrix.h"
#endif

#define INTROFADEFRAMES 15
#define OUTROFADEFRAMES 15

/* Global variables */

char copyright_string[] = "CYLINDRIX VERSION 2.0 COPYRIGHT 2002 GOLDTREE ENTERPRISES, Hotwarez LLC";
char version_string[] = "V2.02";

//Johnm 9/5/2002 Temporary global boolean to denote that we are rendering the second player's screen
int g_bRenderingFirstPlayer = 0;
int g_bRenderingSecondPlayer = 0;

extern long TIMER_CLICKS_PER_SECOND;
extern long GAME_CLICKS_PER_SECOND;

int     level_warp   = 0;
boolean test_anim    = FALSE;
boolean show_readout = FALSE;
boolean test_samples = FALSE; /* Johns debug samples thing */

boolean ceiling_on = TRUE;

#ifdef GLCYLINDRIX
boolean renderSoftwareBuffer = FALSE; 
#endif

/* Stat shit */

long program_over = FALSE;    /* TRUE when user wishes to leave the program */

game_stats_type game_stats;        /* Keep track of the current pilots stats for one game */
overall_stats_type overall_stats;  /* Keep track of the current pilots overall stats */

team_type local_user_team;    /* the team (red or blue) that the user is on */

int master = 0; /* = 0 to shut up compiler */

static string debug_string[6];
static string temp_str2[6];  /* ditto */

boolean ai_active = TRUE;

game_configuration_type game_configuration;

level_type level; /* This holds the values we load from the level file */

int debug = FALSE;        /* When true the program prints out a bunch of
                             debugging information */

long exit_loop = FALSE;        /* tells us to exit the main game loop */
long game_over = FALSE;        /* true if one team is victorious */

extern WingmanMenuData wingman_menu_data;

/* sample contains all sound effect samples */

/* This should be in the world_sounds data structure */

SYS_SOUNDHANDLE sample[MAX_WORLD_SAMPLES];
SYS_SOUNDHANDLE computer_sample[MAX_COMPUTER_SAMPLES];
SYS_SOUNDHANDLE music = 0;

/* mod contains all data for the mod file */

sb_mod_file *mod;

WorldStuff world_stuff;
MenuStuff menu_stuff;

long timer_installed = FALSE;
long keyboard_installed = FALSE;
long ignore_sound_card = FALSE;
long multiplayer_game_only = FALSE;

int temp_int;

char frame_rate_str[80];   /* frame rate to be blitted on the screen */

//#define MACOS_BUNDLE
#ifdef MACOS_BUNDLE
const char* g_DataPath = "./Cylindrix.app/Contents/MacOS/";
#else
const char* g_DataPath = "./";
#endif

/* init_game() loads the game.cfg file and initializes stuff that only needs to be
   initialized one time, like the double_buffer, some transformation matrices, sin
   and cosine lookup tables and the menu text and pictures. */

void init_game( int argc, const char *argv[] )
{
    /* init the double buffer */
    Init_Double_Buffer();
    
    /* load in all the stuff needed for the menus, like text, pcx pictures & 3d_objs */
    init_menu_stuff( &menu_stuff );

    /* load the game.cfg file */
	Load_Game_Configuration( &game_configuration );

    /* check the command line arguments */
    check_command_line_args( argc, argv, &ai_active );

    /* init the transformation matrices */
    projection_matrix_init();
    scale_matrix_init();
    wtov_matrix_init();

    /* Create lookup tables */
    init_sine_table();    
    init_arc_cos_table();

    keyboard_installed = TRUE;
    
    Clear_Game_Stats( &game_stats );
    Clear_Overall_Stats( &overall_stats );
    
	// play music
    if( game_configuration.music_on ) 
	{
		music = SYS_LoadSound(MENU_SONG);		
		SYS_PlaySoundVolume(music, true, game_configuration.music_vol);
    }
}

void setup_game()
{

    /* initalize everything and output each step */

    /* Load the level file */

    cylindrix_init_print( "LOAD_LEVEL() CYLINDRIX-OS V1.1" );

    if( !Load_Level( game_configuration.cylinder_filename, &level ) )
        SYS_Error("Error loading level file %s!!\n", game_configuration.cylinder_filename );

    /* init lookup tables, interrupts, matrices ... */

    cylindrix_init_print( "INIT_STUFF() HYPER-LOGIC ENGINE V1.31A" );

    init_stuff();

    /* initialize everything in the world_stuff data structure */

    cylindrix_init_print( "INIT_WORLD_STUFF() PUMPIN' PHAT SKILLS" );

    init_world_stuff( &world_stuff );

    /* initialize the ai */

    cylindrix_init_print( "GENETIC 3D REALITY PARSING CYBERFLUX AI" );

    Init_AI( &world_stuff );

    /* Init all the player stuff */

    reset_game_to_start( &world_stuff, &level, &game_configuration, master );

    /* This figures out which team the local user is on */

    init_local_user_team( &local_user_team );

    /* Clear the screen so it is black */

    DB_Clear_Screen();    
    Swap_Buffer();    
}

void tournament_game_setup()
{
    tournament_init_print( "LOAD_LEVEL() CYLINDRIX-OS V1.1" );

    if( !Load_Level( game_configuration.cylinder_filename, &level ) )
        SYS_Error("Error loading level file %s!!\n", game_configuration.cylinder_filename );

    /* init lookup tables, interrupts, matrices ... */

    tournament_init_print( "INIT_STUFF() HYPER-LOGIC ENGINE V1.31A" );

    init_stuff();

    /* initialize everything in the world_stuff data structure */

    tournament_init_print( "INIT_WORLD_STUFF() PUMPIN' PHAT SKILLS" );

    init_world_stuff( &world_stuff );

    /* initialize the ai */

    tournament_init_print( "GENETIC 3D REALITY PARSING CYBERFLUX AI" );
    
    Init_AI( &world_stuff );

    /* Init all the player stuff */

    reset_game_to_start( &world_stuff, &level, &game_configuration, master );

    /* This figures out which team the local user is on */

    init_local_user_team( &local_user_team );

    /* Clear the screen so it is black */

    DB_Clear_Screen();    
    Swap_Buffer();
    
} /* tournament_game_setup() */


// OLD GAME
/* plays one tournament game, returns 0 if the user lost, 1 if user won and 2 if the user quit */
#if 0
int play_one_tournament_game( int song_number, int wingman1_alive, int wingman2_alive, int bTournamentGame )
{
    long status_bar = FALSE;       /* Tells if the status bar is to be drawn. */
    Orientation old_orient;        /* holds the last frames view_orientation, used for the third person view */
    long first_person_view = TRUE; /* TRUE if camera is inside the cockpit */
    long transporting = FALSE;     /* TRUE if user is being transported to a new vehicle */
    long round_over = FALSE;       /* true if one team has won a round */
    long user_victories = 0;       /* number of user_victories */
    long enemy_victories = 0;      /* number of enemy_victoies */
    long frame_counter = 0;        /* number of frames processed so far, used for debugging */
    long round = 0;                /* indicates the round being played */
    long display_menu = FALSE;     /* if the user hits escape in the middle of the game this becomes true */
    int i;
    Float_Vector temp_vect;
    pcx_picture temp_pcx;
    long return_state = 2;             /* 0 if the user lost, 1 if the user won, 2 if the user quit */
    long color_cycle = FALSE;
    int bStoreTwoPlayer; //Tournament games set two_player to 0, need to store initial state to set back
	unsigned int loopStartTime;

    const long num_rounds = 3;     /* total number of rounds allowed */
     
    // Don't allow split screen on tournament games
    if( bTournamentGame ) {
		bStoreTwoPlayer = game_configuration.two_player;
		game_configuration.two_player = 0;
    }
    /* play the game!!! */

    round = 1;
    game_over = FALSE;
    exit_loop = FALSE;

    /* In tournament, USER GETS SHITTY ASS AI WINGMAN!!!! */
	if( bTournamentGame ) {
		world_stuff.player_array[1].character.skill_level = 0;
		world_stuff.player_array[2].character.skill_level = 0;
	}
    /* HEHEHEHEHEHEHEH                     */
	
    if( strcmp( game_configuration.cylinder_filename, "gamedata/level10.dat" ) == 0 )
        color_cycle = TRUE;
    else
        color_cycle = FALSE;    

    /* STAT SHIT */
    
    Clear_Game_Stats( &game_stats );
    
    strcpy( game_stats.name, game_configuration.pilot_name );

    while( !game_over ) {

        /* if the user was displaying the during_game menu then resume where you left off,
           other wise start the round. */

        if( !display_menu ) {

            if( game_configuration.music_on ) {
                Play_Song( song_number );
                Set_Cd_Volume( game_configuration.music_vol );
            }

            /* reset_player_array() will reset all of the players input_tables, orientations,
               hit_points, etc.  Will reset the radar bases, clear all explosions and projectiles */

            reset_game_to_start( &world_stuff, &level, &game_configuration, master );
            
            world_stuff.player_array[1].tank.alive = wingman1_alive;
            world_stuff.player_array[2].tank.alive = wingman2_alive;

            /* reset all the voices stuff */

            Init_Voices();

            /* If wingman is DEAD he shouldn't talk! */
            if( !wingman1_alive )
                {
                 Zero_Voice(1);
                 No_Despair(2);
                }
            if( !wingman2_alive )
                {
                 Zero_Voice(2);
                 No_Despair(1);
                }

            /* set the temp palette to black */

            memset( temp_pcx.palette, 0, sizeof( RGB_color ) * 256 );

            /* enable the black palette */

            Enable_Color_Palette( &temp_pcx );

            /* set the view_point */

            world_stuff.view_orientation = world_stuff.player_array[user_vehicle_index()].tank.orient;

            world_stuff.view_orientation.position[X] += 1.8f * world_stuff.view_orientation.up[X] - 2.85f * world_stuff.view_orientation.front[X];
            world_stuff.view_orientation.position[Y] += 1.8f * world_stuff.view_orientation.up[Y] - 2.85f * world_stuff.view_orientation.front[Y];
            world_stuff.view_orientation.position[Z] += 1.8f * world_stuff.view_orientation.up[Z] - 2.85f * world_stuff.view_orientation.front[Z];

            /* set transporting to true so that draw_everything doesnt draw the huds */

            transporting = TRUE;
            first_person_view = FALSE;

            /* fade in the palette */

			// AJT: FIX
			//New_Brighten_Palette( &world_stuff.text, 16 );
            for( i = 0; i < 16; i++ ) 
			{
                draw_everything( user_vehicle_index(), status_bar, first_person_view, transporting );                
            }

            /* play the round sample */

            if ( round == 1 )
				Play_Voice( computer_sample[ROUND_ONE] );
            else if ( round == 2 )
				Play_Voice( computer_sample[ROUND_TWO] );
            else
				Play_Voice( computer_sample[FINAL_ROUND] );

			// AJT: Fix me
			while ( !Is_Voice_Done() );

            while ( 1 ) 
			{

                /* move the view_orientation.position into the vehicle */

                if( distance_between( world_stuff.view_orientation.position, world_stuff.player_array[user_vehicle_index()].tank.orient.position ) < 1.0 ) {
                    world_stuff.view_orientation = world_stuff.player_array[user_vehicle_index()].tank.orient;
                    draw_everything( user_vehicle_index(), status_bar, first_person_view, transporting );
                    first_person_view = TRUE;
                    break;
                }
                else {

                    temp_vect[X] = world_stuff.player_array[user_vehicle_index()].tank.orient.position[X] - world_stuff.view_orientation.position[X];
                    temp_vect[Y] = world_stuff.player_array[user_vehicle_index()].tank.orient.position[Y] - world_stuff.view_orientation.position[Y];
                    temp_vect[Z] = world_stuff.player_array[user_vehicle_index()].tank.orient.position[Z] - world_stuff.view_orientation.position[Z];

                    normalize( temp_vect );

                    world_stuff.view_orientation.position[X] += (1.0f * temp_vect[X]);
                    world_stuff.view_orientation.position[Y] += (1.0f * temp_vect[Y]);
                    world_stuff.view_orientation.position[Z] += (1.0f * temp_vect[Z]);

                    draw_everything( user_vehicle_index(), status_bar, first_person_view, transporting );
                }
            }

            /* This is to prevent a crash when the user hits v during the first frame. */

            world_stuff.view_orientation = world_stuff.player_array[user_vehicle_index()].tank.orient;
            old_orient = world_stuff.player_array[user_vehicle_index()].tank.orient;

            transporting = FALSE;

            /* copy the game pallete into temp_pcx */

            memcpy( temp_pcx.palette, world_stuff.text.palette, sizeof( RGB_color ) * 256 );

            /* enable the game palette */

            Enable_Color_Palette( &world_stuff.text );
        }

        round_over = FALSE;
        exit_loop = FALSE;
        display_menu = FALSE;

        while( !exit_loop ) {            

            loopStartTime = SYS_GetTimeMS();

            /* update some of the game_stats */
                        
            game_stats.user_vehicle = user_vehicle_index();
            game_stats.total_frames++;
            game_stats.total_d = game_stats.total_d + world_stuff.player_array[game_stats.user_vehicle].tank.laser_damage;
            
            /* Get all the input tables for all the vehicles.
               Users get their input from the keyboard, joystick or mouse.
               Computer vehicles get their input from the AI.
               Remotely controlled get their input from the null_modem */

            get_all_input( world_stuff.player_array, ai_active, master );

            /* Move all the vehicles according to their input_tables one frame forward.
               This function handles all the collisions between vehicles, pylons & projectiles.
               Also creates and destroys projectiles, inflicts damage, scrambles controls ....
               basicly all the main game logic and physics takes place here */

            move_everything_one_click();

            /* check to see if the game is over or not */

            if( !round_over ) {

                if( (round_over = is_round_over()) ) {

                    if( num_vehicles_remaining( local_user_team ) > 0 ) {
                        user_victories++;
                    }
                    else {
                        enemy_victories++;
                    }

                    for( i = 0; i < 64; i++ ) {

                        /* fade the pallete to black */

//                        Dim_Palette( &temp_pcx, 1 );

//                        Enable_Color_Palette( &temp_pcx );

                        /* enable the pallete */

                        if( (num_vehicles_remaining( local_user_team ) > 0) ) {
                            get_current_view_orient( &world_stuff.view_orientation, &old_orient,
                                                     local_user_team, &transporting, &first_person_view );
                        }
                        else {
                            transporting = TRUE;
                        }

                        draw_everything( user_vehicle_index(), status_bar, first_person_view, transporting );

                        Play_Q_Samples( &world_stuff.view_orientation );

                        if( game_configuration.sound_on ) {
                            Handle_Voices( &world_stuff, user_vehicle_index(), (unsigned char)first_person_view, (unsigned char)transporting );
                        }

                        Handle_Commands( &world_stuff, user_vehicle_index() );

                        update_leader( user_vehicle_index() );

                        /* Frame rate ceiling */
						if ( ceiling_on )
						{
							while ( SYS_GetTimeMS() < loopStartTime + 33 );
						}						

                        get_only_ai_input( world_stuff.player_array, ai_active, master );

                        move_everything_one_click();

                    }

                    /* play the computer victory sample */

                    if( user_victories == 2 ) {
                        if( local_user_team == RED_TEAM ) {
                            if( sb_installed ) {
                                Play_Voice( computer_sample[RED_TEAM_WINS] );
                            }
                        }
                        else {
                            if( sb_installed ) {
                                Play_Voice( computer_sample[BLUE_TEAM_WINS] );
                            }
                        }
                    }
                    else if( enemy_victories == 2 ) {
                        if( local_user_team == RED_TEAM ) {
                            if( sb_installed ) {
                                Play_Voice( computer_sample[BLUE_TEAM_WINS] );
                            }
                        }
                        else {
                            if( sb_installed ) {
                                Play_Voice( computer_sample[RED_TEAM_WINS] );
                            }
                        }
                    }

                    if( sb_installed ) {
                        while( !Is_Voice_Done() );
                    }

                    /* play the wingman victory sample */

                    if( num_vehicles_remaining( local_user_team ) > 0 ) {
                        if( user_victories == 1 ) {
                            if( local_user_team == RED_TEAM ) {
                                if( sb_installed ) {
                                    Play_Voice( world_stuff.player_array[1].character.samples[VICTORY] );
                                }
                            }
                            else {
                                if( sb_installed ) {
                                    Play_Voice( world_stuff.player_array[4].character.samples[VICTORY] );
                                }
                            }
                        }
                        else {
                            if( local_user_team == RED_TEAM ) {
                                if( sb_installed ) {
                                    Play_Voice( world_stuff.player_array[2].character.samples[VICTORY] );
                                }
                            }
                            else {
                                if( sb_installed ) {
                                    Play_Voice( world_stuff.player_array[5].character.samples[VICTORY] );
                                }
                            }
                        }
                    }
                    else {
                        if( enemy_victories == 1 ) {
                            if( local_user_team == BLUE_TEAM ) {
                                if( sb_installed ) {
                                    Play_Voice( world_stuff.player_array[1].character.samples[VICTORY] );
                                }
                            }
                            else {
                                if( sb_installed ) {
                                    Play_Voice( world_stuff.player_array[4].character.samples[VICTORY] );
                                }
                            }
                        }
                        else {
                            if( local_user_team == BLUE_TEAM ) {
                                if( sb_installed ) {
                                    Play_Voice( world_stuff.player_array[2].character.samples[VICTORY] );
                                }
                            }
                            else {
                                if( sb_installed ) {
                                    Play_Voice( world_stuff.player_array[5].character.samples[VICTORY] );
                                }
                            }
                        }
                    }

                    if( sb_installed ) {
                        while( !Is_Voice_Done() );
                    }

                    break;
                }
            }

            /* figure out where the current view_orientation is */

            get_current_view_orient( &world_stuff.view_orientation, &old_orient,
                                     local_user_team, &transporting, &first_person_view );

            /* Draws the tube, pylons, vehicles and projectiles into the doubler_buffer.
               Also draws the heads up display and the status bar. */

            draw_everything( user_vehicle_index(), status_bar, first_person_view, transporting );
            
            if( color_cycle ) {
            
                /* Note: this will only look right if we are playing in level10.dat */
                
                Cycle_Palette_Section( 160, 20, &world_stuff.text );  /* cycle the purple gradient */
                Cycle_Palette_Section( 48, 95, &world_stuff.text );    /* cycle the red gradient */
                Cycle_Palette_Section( 112, 159, &world_stuff.text );  /* cycle the blue gradient */
                
                Enable_Color_Palette( &world_stuff.text );
            }


            /* Checks to see if the 'v', 'esc' and 'b' keys are pressed. If so
               they change the values of first_person_view, exit_loop and status_bar,
               respectively. */

            /* While there are still keyboard hits to process */                

            /* if 'v' is pressed change views */

            if ( SYS_KeyPressed( KEY_V ) ) 
			{
                first_person_view = !first_person_view;
            }

            /* if the user hits the esc key exit the loop and display the during_game menu */

            if ( SYS_KeyPressed( KEY_ESC ) ) 
			{
                exit_loop = TRUE;
                display_menu = TRUE;
            }

            /* if the user hits 'b' display the status bar (for debugging) */

            if ( show_readout )
			{
				if ( SYS_KeyPressed( KEY_B ) ) 
					status_bar = !status_bar;				
			}
    
            if ( SYS_KeyPressed( KEY_C ) )
                ceiling_on = !ceiling_on;
            
#ifdef GLCYLINDRIX
            if ( SYS_KeyPressed( KEY_G ) )
                renderSoftwareBuffer = !renderSoftwareBuffer;            
#endif


            if ( SYS_KeyPressed( KEY_SPACE ) )
			{
                if ( round_over )
                    exit_loop = TRUE;                
            }

            Play_Q_Samples( &world_stuff.view_orientation );

            if( game_configuration.sound_on ) {
                Handle_Voices( &world_stuff, user_vehicle_index(), (unsigned char)first_person_view, (unsigned char)transporting );
            }

            Handle_Commands( &world_stuff, user_vehicle_index() );

            update_leader( user_vehicle_index() );

            /* Frame rate ceiling */			
			if ( ceiling_on )
			{
				while ( SYS_GetTimeMS() < loopStartTime + 16 );
			}			

            frame_counter++;
        }

        if( display_menu ) {            
            game_over = during_game_menu_event_loop();            
            
            DB_Clear_Screen();            
            Swap_Buffer();    
            Enable_Color_Palette( &world_stuff.text );
            
            Figure_Game_Stats( &game_stats );
        }
        else {

            /* if one player has won over half of the total number of rounds
               then the game is over */

            if( (user_victories > (num_rounds / 2)) || (enemy_victories > (num_rounds / 2)) ) {
                game_over = TRUE;                
                
                if( user_victories > enemy_victories ) {                    
                    
                    /* STAT SHIT */
                    
                    game_stats.victory = 1;                    
    
                    Figure_Game_Stats( &game_stats );
    
                    Add_Game_Stats_To_Overall_Stats( &game_stats, &overall_stats );
                    
                    return_state = 1;

                }
                else {
                                    
                    /* STAT SHIT */
                    
                    game_stats.victory = 0;
                    
                    Figure_Game_Stats( &game_stats );
    
                    Add_Game_Stats_To_Overall_Stats( &game_stats, &overall_stats );
                    
                    return_state = 0;

                }
            }
            else {

                /* go on to the next round */
                
                Figure_Game_Stats( &game_stats );                

                round++;
            }
        }
    }

    DB_Clear_Screen();    
    Swap_Buffer();
    Enable_Color_Palette( &menu_stuff.general_menu_background );

    /* bring up custom_game_stat_menu */
    
    tournament_game_stat_menu_event_loop( return_state );
    
    DB_Clear_Screen();    
    Swap_Buffer();

    //Set two player flag back
    if( bTournamentGame ) {
          game_configuration.two_player = bStoreTwoPlayer ;
    }
 
    return return_state;

} /* play_one_tournament_game() */
#endif

#if 0
void play_tournament_game()
{
    int return_val;
    int tournament_over = FALSE;
    
    /* Play_Intro_Animation(); */
    
    if( level_warp <= 1 )
        goto game1;
    if( level_warp == 2 )
        goto game2;
    if( level_warp == 3 )
        goto game3;
    if( level_warp == 4 )
        goto game4;
    if( level_warp == 5 )
        goto game5;
    if( level_warp == 6 )
        goto game6;
    if( level_warp == 7 )
        goto game7;
    if( level_warp == 8 )
        goto game8;
    if( level_warp == 9 )
        goto game9;
    if( level_warp == 10 )
        goto game10;    

    game1:
    /* **************** game 1 ****************** */                                                    
                                                    
    /* play game 1 animation */

    /* display the scavengers */
    
    game_configuration.blue0_ai = 18;
    game_configuration.blue1_ai = 17;
    game_configuration.blue2_ai = 16;
    game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
    game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
    game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
    strcpy( game_configuration.cylinder_filename, "gamedata/level9.dat" );  

    if( game_configuration.animations_on ) {
        Play_Cut_One();
    }
    display_next_opponent();

    /* load all the shit */
        
    tournament_game_setup();

    /* start the game */
    
    return_val = play_one_tournament_game( SCAVENGER_SONG, TRUE, TRUE, TRUE );
    
    if( return_val == 0 ) {  /* user lost */
        tournament_over = TRUE;
    }
    else if( return_val == 1 ) {  /* user won */
    
        /* free everything */
        
        free_world_stuff( &world_stuff );
        free_all_samples( &world_stuff );
        
        erase_smart_heap();
        
    }
    else if( return_val == 2 ) {  /* user quit */
        tournament_over = TRUE;
    }
    else {  /* bad return_val */
        tournament_over = TRUE;
    }
    
    game2:
    
    /* **************** game 2 ****************** */

    
    if( !tournament_over ) {
    
        /* play game 2 animation */
    
        /* display the bok */
        
        game_configuration.blue0_ai = 10;
        game_configuration.blue1_ai = 11;
        game_configuration.blue2_ai = 12;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level10.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Two();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();

        /* start the game */
        
        return_val = play_one_tournament_game( BOK_SONG, TRUE, TRUE, TRUE );
        
        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
            /* free everything */
        
            free_world_stuff( &world_stuff );       
            free_all_samples( &world_stuff );
            
            erase_smart_heap();

        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }
    }    
    
    game3:
    
    /* **************** game 3 ****************** */
    
    
    if( !tournament_over ) {
    
        /* play game 3 animation */
    
        /* display the succubi */
        
        game_configuration.blue0_ai = 19;
        game_configuration.blue1_ai = 20;
        game_configuration.blue2_ai = 21;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level2.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Three();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();

        /* start the game */
        
        return_val = play_one_tournament_game( SUCCUBI_SONG, TRUE, TRUE, TRUE );
        
        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
            /* free everything */
        
            free_world_stuff( &world_stuff );       
            free_all_samples( &world_stuff );
            
            erase_smart_heap();

        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }               
    }    
    
    game4:
    
    /* **************** game 4 ****************** */
    
    
    if( !tournament_over ) {
    
        /* play game 4 animation */
    
        /* display the sentry */
        
        game_configuration.blue0_ai = 32;
        game_configuration.blue1_ai = 33;
        game_configuration.blue2_ai = 31;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level4.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Four();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();


        /* start the game */
        
        return_val = play_one_tournament_game( SENTRY_SONG, TRUE, TRUE, TRUE );
        
        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
            /* free everything */
            
            free_world_stuff( &world_stuff );       
            free_all_samples( &world_stuff );
            
            erase_smart_heap();        

        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }
    }    
    
    game5:
    
    /* **************** game 5 ****************** */
    
    
    if( !tournament_over ) {
    
        /* play game 5 animation */
    
        /* display the humans */
        
        game_configuration.blue0_ai = 8;
        game_configuration.blue1_ai = 7;
        game_configuration.blue2_ai = 2;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level1.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Five();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();

        /* start the game */
        
        return_val = play_one_tournament_game( HUMAN_SONG, TRUE, TRUE, TRUE );

        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
            /* free everything */
        
            free_world_stuff( &world_stuff );       
            free_all_samples( &world_stuff );
            
            erase_smart_heap();

        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }
    }    
    
    game6:
    
    /* **************** game 6 ****************** */
    
    
    if( !tournament_over ) {
    
        /* play game 6 animation */
    
        /* display the slar */
        
        game_configuration.blue0_ai = 14;
        game_configuration.blue1_ai = 13;
        game_configuration.blue2_ai = 15;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level3.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Six();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();


        /* start the game */
        
        return_val = play_one_tournament_game( SLAR_SONG, TRUE, TRUE, TRUE );

        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
           /* free everything */
           
           free_world_stuff( &world_stuff );       
           free_all_samples( &world_stuff );
           
           erase_smart_heap();

        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }
    }    
    
    game7:
    
    /* **************** game 7 ****************** */
    
    
    if( !tournament_over ) {
    
        /* play game 7 animation */
    
        /* display the biomechanoids */
        
        game_configuration.blue0_ai = 34;
        game_configuration.blue1_ai = 35;
        game_configuration.blue2_ai = 36;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level5.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Seven();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();

        
        /* start the game */
        
        return_val = play_one_tournament_game( BIOMECHANOID_SONG, TRUE, TRUE, TRUE );

        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
           /* free everything */
           
           free_world_stuff( &world_stuff );       
           free_all_samples( &world_stuff );
           
           erase_smart_heap();
        
        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }
    }    
    
    game8:
    
    /* **************** game 8 ****************** */
    
    
    if( !tournament_over ) {
    
        /* play game 8 animation */
    
        /* display the Pharoahs */
        
        game_configuration.blue0_ai = 25;
        game_configuration.blue1_ai = 27;
        game_configuration.blue2_ai = 26;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level8.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Eight();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();


        /* start the game */
        
        return_val = play_one_tournament_game( PHAROAH_SONG, TRUE, TRUE, TRUE );

        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
            /* free everything */
        
            free_world_stuff( &world_stuff );       
            free_all_samples( &world_stuff );
            
            erase_smart_heap();

        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }
    }    
    
    game9:
    
    /* **************** game 9 ****************** */
    
    
    if( !tournament_over ) {
    
        /* play game 2 animation */
    
        /* display the overlords */
        
        game_configuration.blue0_ai = 23;
        game_configuration.blue1_ai = 24;
        game_configuration.blue2_ai = 22;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level7.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Nine();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();


        /* start the game */
        
        return_val = play_one_tournament_game( OVERLORD_SONG, TRUE, FALSE, TRUE );

        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
            /* free everything */
        
            free_world_stuff( &world_stuff );       
            free_all_samples( &world_stuff );
            
            erase_smart_heap();

        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }
    }    
    
    game10:
    
    /* **************** game 10 ****************** */
    
    
    if( !tournament_over ) {
    
        /* play game 10 animation */
    
        /* display the watchers */
        
        game_configuration.blue0_ai = 28;
        game_configuration.blue1_ai = 29;
        game_configuration.blue2_ai = 30;
        game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
        game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
        game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;
        strcpy( game_configuration.cylinder_filename, "gamedata/level6.dat" );
        
        if( game_configuration.animations_on ) {
            Play_Cut_Ten();
        }
        display_next_opponent();
            
        /* load all the shit */
        
        tournament_game_setup();


        /* start the game */
        
        return_val = play_one_tournament_game( WATCHER_SONG, FALSE, FALSE, TRUE );

        if( return_val == 0 ) {  /* user lost */
            tournament_over = TRUE;
        }
        else if( return_val == 1 ) {  /* user won */
        
            /* dont free anything this time */
            /* the main loop will do it for me */
            ;
			
            Play_Ending_Animation(); 
        }
        else if( return_val == 2 ) {  /* user quit */
            tournament_over = TRUE;
        }
        else {  /* bad return_val */
            tournament_over = TRUE;
        }
    }
}
#endif
 
#if 0   
void play_outro( void )
{
	int i;
    pcx_picture order1_pcx;
/*    pcx_picture order2_pcx; */
    pcx_picture temp_pcx;    

    PCX_Load( "pcx_data/order1.pcx", &order1_pcx );
    /* PCX_Load( "pcx_data/order2.pcx", &order2_pcx ); */

    memset( temp_pcx.palette, 0, sizeof( RGB_color ) * 256 );
    Enable_Color_Palette( &temp_pcx );

    Pop_Buffer( order1_pcx.buffer );
    Swap_Buffer();
    
    /* fade in the palette for order1.pcx */

    New_Brighten_Palette( &order1_pcx, OUTROFADEFRAMES );

#ifdef GLCYLINDRIX
	 // keep rendering during the fade.
     for ( i = 0; i < OUTROFADEFRAMES; i++ )
		 GL_SubloadAndSwapBuffer();	 
#endif
    
    /* wait for the user to hit a key */
     
    while( !Jon_Kbhit() );
        
    /* after the user hit a key fade out the palette */
	New_Fade_Palette( OUTROFADEFRAMES );

#ifdef GLCYLINDRIX
	 // keep rendering during the fade.
     for ( i = 0; i < OUTROFADEFRAMES; i++ )
		 GL_SubloadAndSwapBuffer();	 
#endif

    /*
    memset( temp_pcx.palette, 0, sizeof( RGB_color ) * 256 );
    Enable_Color_Palette( &temp_pcx );

    Pop_Buffer( order2_pcx.buffer );
    Swap_Buffer();
    */
    /* fade in order2_pcx */
    /*
    for( i = 0; i < 64; i++ ) {
        Brighten_Palette( &temp_pcx, &order2_pcx, 1 );
        Enable_Color_Palette( &temp_pcx );
        delay(20);
    }
    */
    /* wait for the user to hit a key */
    /* 
    while( !Jon_Kbhit() );
    */
    /* fade out the palette */
    /*
    for( i = 0; i < 64; i++ ) {
        Dim_Palette( &temp_pcx, 1 );
        Enable_Color_Palette( &temp_pcx );
        delay(20);
    }

    DB_Clear_Screen();
    Swap_Buffer();
    */
    free( order1_pcx.buffer );
    /* free( order2_pcx.buffer ); */
     
} /* play_outro() */
#endif

#if 0
void MainLoop( int argc, char *argv[] ) {    
	int song_number = 7;

    /* init_game() loads the game.cfg file and initializes stuff that only needs to be
       initialized one time, like the double_buffer, some transformation matrices, sin
       and cosine lookup tables and the menu text and pictures. */
    
    init_game( argc, argv );

	//Commented out because palette fading is slow right now
    Play_Intro();

    Init_Menu_Sounds();
    
    init_all_main_menus();
    
    /* load all of the samples needed for the menu */
        
    Init_Menu_Voices();

    /* let the user initalize the game_configuration data structure */

    user_setup();
        
    Free_Menu_Voices();
   
    while( !program_over ) {

        erase_smart_heap();

        if( strcmp( game_configuration.cylinder_filename, "gamedata/level1.dat" ) == 0 ) {
            song_number = HUMAN_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level2.dat" ) == 0 ) {
            song_number = SUCCUBI_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level3.dat" ) == 0 ) {
            song_number = SLAR_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level4.dat" ) == 0 ) {
            song_number = SENTRY_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level5.dat" ) == 0 ) {
            song_number = BIOMECHANOID_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level6.dat" ) == 0 ) {
            song_number = WATCHER_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level7.dat" ) == 0 ) {
            song_number = OVERLORD_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level8.dat" ) == 0 ) {
            song_number = PHAROAH_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level9.dat" ) == 0 ) {
            song_number = SCAVENGER_SONG;
        }
        else if( strcmp( game_configuration.cylinder_filename, "gamedata/level10.dat" ) == 0 ) {
            song_number = BOK_SONG;
        }
        else {
            song_number = MENU_SONG; /* play the menu track */
        }

        if( game_configuration.game_type == CustomGame ){
        
            setup_game();
            //play_custom_game( song_number );            
			play_one_tournament_game(song_number, TRUE, TRUE, FALSE);
        }
        else if( game_configuration.game_type == TournamentGame ) {
            play_tournament_game();
        }

        free_world_stuff( &world_stuff );
        
        free_all_samples( &world_stuff );

        erase_smart_heap();

        /* load all of the samples needed for the menu */
        
        Init_Menu_Voices();

        /* let the user initalize the game_configuration data structure */

        if( game_configuration.music_on ) {
            Play_Song( MENU_SONG );
            Set_Cd_Volume( game_configuration.music_vol );
        }

        user_setup();
        
        Free_Menu_Voices();        
    }
    
    Save_Game_Configuration( &game_configuration );
        
    if( game_configuration.slot_num == 0 ) {
        Save_Overall_Stats_Binary( &overall_stats, "gamedata/xxx0.xxx" );
    }
    else if( game_configuration.slot_num == 1 ) {
        Save_Overall_Stats_Binary( &overall_stats, "gamedata/xxx1.xxx" );
    }
    else if( game_configuration.slot_num == 2 ) {
        Save_Overall_Stats_Binary( &overall_stats, "gamedata/xxx2.xxx" );
    }
    else if( game_configuration.slot_num == 3 ) {
        Save_Overall_Stats_Binary( &overall_stats, "gamedata/xxx3.xxx" );
    }    
    else if( game_configuration.slot_num == 4 ) {
        Save_Overall_Stats_Binary( &overall_stats, "gamedata/xxx4.xxx" );
    }
    else if( game_configuration.slot_num == 5 ) {
        Save_Overall_Stats_Binary( &overall_stats, "gamedata/xxx5.xxx" );
    }
    else if( game_configuration.slot_num == 6 ) {
        Save_Overall_Stats_Binary( &overall_stats, "gamedata/xxx6.xxx" );
    }
    else if( game_configuration.slot_num == 7 ) {
        Save_Overall_Stats_Binary( &overall_stats, "gamedata/xxx7.xxx" );
    }

    play_outro();

    Free_Menu_Sounds();
    
    free_menu_stuff( &menu_stuff );
    
    free_smart_heap(); 
    
    exit_gracefully();

}
#endif



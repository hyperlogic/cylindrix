#include <stdlib.h>
#include <assert.h>

#include "gameloop.h"
#include "omega.h"
#include "pcx.h"
#include "prim.h"
#include "main.h"
#include "menu.h"
#include "config.h"
#include "stats.h"
#include "glcylindrix.h"
#include "voices.h"
#include "util.h"
#include "fx.h"
#include "commands.h"
#include "menu.h"
#include "text.h"
#include "fli.h"

const char* gameName = "Cylindrix";

enum GameStateEnum
{	
	GAMESTATE_INTRO, 	
	GAMESTATE_MAINMENU,
	GAMESTATE_CUSTOMGAME,
	GAMESTATE_TOURNAMENTGAME,
	GAMESTATE_OUTRO,
	GAMESTATE_NUMSTATES,
};

enum GameStateEnum currentGameState = GAMESTATE_INTRO;

struct GameState
{
	void (*enter)();
	void (*process)();
	void (*exit)();
};

struct GameState gameStates[GAMESTATE_NUMSTATES] =
{
	{ IntroEnter, IntroProcess, IntroExit },
	{ MainMenuEnter, MainMenuProcess, MainMenuExit },
	{ PlayCustomGameEnter, PlayCustomGameProcess, PlayCustomGameExit },
	{ PlayTournamentGameEnter, PlayTournamentGameProcess, PlayTournamentGameExit },
	{ OutroEnter, OutroProcess, OutroExit },
};

static int quitGame = 0;

//
// GAMESTATE_INTRO
//

#define NUM_INTRO_IMAGES 2
static pcx_picture* intro_image[NUM_INTRO_IMAGES];
static int current_intro_image = 0;
static float intro_start_time;
static int intro_sub_state = 0;
static float intro_exit_fade;
static long status_bar = FALSE;

//
// GAMESTATE_TOURNAMENT
//

static int tournamentLevel = 0;

enum TournamentLevelEnum 
{
	Scavangers = 0,
	Bok = 1,
	Succubi = 2,
	Sentry = 3,
	Humans = 4,
	Slar = 5,
	Biomechanoids = 6,
	Pharoahs = 7,
	Overlords = 8,
	Watchers = 9,
	NumLevels = 10
};

struct TournamentLevelData
{
	int blueAI[3];
	char* cylinder_filename;
};

struct TournamentLevelData tournamentLevelData[ NumLevels ] =
{
	{ { 18, 17, 16 }, "gamedata/level9.dat" },
	{ { 10, 11, 12 }, "gamedata/level10.dat" },
	{ { 19, 20, 21 }, "gamedata/level2.dat" },
	{ { 32, 33, 31 }, "gamedata/level4.dat" },
	{ { 8, 7, 2 }, "gamedata/level1.dat" },
	{ { 14, 13, 15 }, "gamedata/level3.dat" },
	{ { 34, 35, 36 }, "gamedata/level4.dat" },
	{ { 25, 27, 26 }, "gamedata/level8.dat" },
	{ { 23, 24, 22 }, "gamedata/level7.dat" },
	{ { 28, 29, 30 }, "gamedata/level6.dat" }
};

//
// PLAYGAMESTATE
//

enum PlayGameStateEnum
{	
	PLAYGAMESTATE_INTRO,
	PLAYGAMESTATE_ENTERVEHICLE,
	PLAYGAMESTATE_FIGHT,
	PLAYGAMESTATE_OUTRO,
	PLAYGAMESTATE_MENU,
	PLAYGAMESTATE_STATS,
	PLAYGAMESTATE_NUMSTATES,
};

enum PlayGameStateEnum currentPlayGameState = PLAYGAMESTATE_INTRO;

struct GameState playGameStates[PLAYGAMESTATE_NUMSTATES] =
{
	{ PlayGameIntroEnter, PlayGameIntroProcess, PlayGameIntroExit },
	{ PlayGameEnterVehicleEnter, PlayGameEnterVehicleProcess, PlayGameEnterVehicleExit },
	{ PlayGameFightEnter, PlayGameFightProcess, PlayGameFightExit },
	{ PlayGameOutroEnter, PlayGameOutroProcess, PlayGameOutroExit },
	{ PlayGameMenuEnter, PlayGameMenuProcess, PlayGameMenuExit },
	{ PlayGameStatsEnter, PlayGameStatsProcess, PlayGameStatsExit },
};

const int num_rounds = 3;
static int round = 0;
static int color_cycle = FALSE;
static int transporting = FALSE;
static int first_person_view = FALSE;
static Orientation old_orient;	// holds the last frames view_orientation, used for the third person view
static int user_victories = 0;
static int enemy_victories = 0;
static int soundState = 0;

const char* levelFli[10] =
{
	"FLI/ONE.FLI",
	"FLI/TWO.FLI",
	"FLI/THREE.FLI",
	"FLI/FOUR.FLI",
	"FLI/FIVE.FLI",
	"FLI/SIX.FLI",
	"FLI/SEVEN.FLI",
	"FLI/EIGHT.FLI",
	"FLI/NINE.FLI",
	"FLI/TEN.FLI"
};

//
// GAMESTATE_INTRO
//

void IntroEnter()
{
	RGB_color black = { 0x0, 0x0, 0x0 };

	// load the two images.
	intro_image[0] = (pcx_picture*)malloc( sizeof( pcx_picture ) );
	PCX_Load( "pcx_data/gt.pcx", intro_image[0] );

	intro_image[1] = (pcx_picture*)malloc( sizeof( pcx_picture ) );
	PCX_Load( "pcx_data/hotware.pcx", intro_image[1] );
	
	Pop_Buffer( intro_image[0]->buffer );
	current_intro_image = 0;

	Enable_Color_Palette( intro_image[current_intro_image] );
	Set_Palette_Fade( 0.0f );

	Swap_Buffer();

	intro_start_time = SYS_GetTimeSeconds();
}

void IntroProcess()
{	 
	float timer = SYS_GetTimeSeconds() - intro_start_time;
	const float fade_time = 0.5f;
	const float wait_time = 2.0f;
	const float exit_fade_rate = 2.0f;
	float fade = 1.0f;		

	switch ( intro_sub_state )
	{
		// fade in
		case 0:
			Enable_Color_Palette( intro_image[current_intro_image] );
			if ( timer > fade_time )
			{
				intro_sub_state++;
				intro_start_time = SYS_GetTimeSeconds();
			}
			else			
				fade = timer / fade_time;
			break;

		// wait
		case 1:
			Enable_Color_Palette( intro_image[current_intro_image] );
			if ( timer > wait_time )
			{
				intro_sub_state++;
				intro_start_time = SYS_GetTimeSeconds();
			}
			else
				fade = 1.0f;
			break;

		// fade out
		case 2:
			Enable_Color_Palette( intro_image[current_intro_image] );
			if ( timer > fade_time )
			{				
				intro_sub_state++;
				intro_start_time = SYS_GetTimeSeconds();
				// switch images
				Pop_Buffer( intro_image[++current_intro_image]->buffer );
				Enable_Color_Palette( intro_image[current_intro_image] );
				fade = 0.0f;
			}
			else			
				fade = 1.0f - (timer / fade_time);
			break;

		// fade in
		case 3:
			Enable_Color_Palette( intro_image[current_intro_image] );
			if ( timer > fade_time )
			{
				intro_sub_state++;
				intro_start_time = SYS_GetTimeSeconds();
			}
			else			
				fade = timer / fade_time;
			break;

		// wait
		case 4:
			Enable_Color_Palette( intro_image[current_intro_image] );
			if ( timer > wait_time )
			{
				intro_sub_state++;
				intro_start_time = SYS_GetTimeSeconds();
			}
			else
				fade = 1.0f;
			break;

		// fade out
		case 5:
			Enable_Color_Palette( intro_image[current_intro_image] );
			if ( timer > fade_time )
			{
				intro_sub_state++;
				intro_start_time = SYS_GetTimeSeconds();
				fade = 0.0f;
			}
			else			
				fade = 1.0f - (timer / fade_time);
			break;

		case 6:
			intro_sub_state++;
			PlayFliEnter( "FLI/CYLINDRX.FLI" );
			break;

		case 7:
			if ( !PlayFliProcess() )
			{
				intro_sub_state = 99;
			}
			break;

		// fade out and jump to main menu.
		case 99:
			fade = intro_exit_fade - (exit_fade_rate * timer);
			if ( fade < 0.0f )
			{
				fade = 0.0f;
				currentGameState = GAMESTATE_MAINMENU;
			}
			break;

		default:
			assert( 0 ); // should never happen.
			break;
	}

	Set_Palette_Fade( fade );
	Swap_Buffer();	

	// check for keyboard input.
	if ( SYS_KeyPressed( KEY_ESC ) || SYS_KeyPressed( KEY_ENTER ) )
	{
		// skip the rest of the intro
		intro_sub_state = 99;
		intro_exit_fade = fade;
		intro_start_time = SYS_GetTimeSeconds();
	}
}

void IntroExit()
{
	int i = 0;

	DB_Clear_Screen();
	Swap_Buffer();

	for ( i = 0; i < NUM_INTRO_IMAGES; ++i )
	{
		free( intro_image[i]->buffer );
		intro_image[i]->buffer = 0;
		free( intro_image[i] );
		intro_image[i] = 0;
	}
}


//
// GAMESTATE_MAINMENU
//

void MainMenuEnter()
{	
	Init_Menu_Sounds();

    // load all of the samples needed for the menu
    Init_Menu_Voices();

    init_all_main_menus();

	menu_event_init();
   
	Enable_Color_Palette( &menu_stuff.general_menu_background );

	strcpy( game_configuration.cylinder_filename, "gamedata/level1.dat" );
}

void MainMenuProcess()
{
	int retVal;
	retVal = menu_event_loop();

	if ( retVal == 1 )
	{
		if ( game_configuration.game_type == TournamentGame )
		{
			currentGameState = GAMESTATE_TOURNAMENTGAME;
		}
		else // if ( game_configuration.game_type == CustomGame )
		{
			currentGameState = GAMESTATE_CUSTOMGAME;
		}
	}
	else if ( retVal == 2 )
	{
		currentGameState = GAMESTATE_OUTRO;
	}
}

void MainMenuExit()
{
	Save_Game_Configuration( &game_configuration );

	Free_Menu_Voices();
	Free_Menu_Sounds();
}


//
// GAMESTATE_CUSTOMGAME
//

void StartCustomGame()
{
	Init_Menu_Sounds();

	setup_game();

	user_victories = 0;
	enemy_victories = 0;
	round = 0;

    if( strcmp( game_configuration.cylinder_filename, "gamedata/level10.dat" ) == 0 )
        color_cycle = TRUE;
    else
        color_cycle = FALSE;    

    // init stats.
    Clear_Game_Stats( &game_stats );
    strcpy( game_stats.name, game_configuration.pilot_name );

	// AJT: TODO figure out which song to play
	/*
	if ( game_configuration.music_on ) 
	{
		Play_Song( song_number );
		Set_Cd_Volume( game_configuration.music_vol );
	}
	*/

	PlayGameIntroEnter();
	currentPlayGameState = PLAYGAMESTATE_INTRO;
}

void EndCustomGame()
{
	free_world_stuff( &world_stuff );
	free_all_samples( &world_stuff );
	Free_Menu_Sounds();
}

void StartTournamentGame()
{
	Init_Menu_Sounds();

	tournament_game_setup();

	user_victories = 0;
	enemy_victories = 0;
	round = 0;

    if( strcmp( game_configuration.cylinder_filename, "gamedata/level10.dat" ) == 0 )
        color_cycle = TRUE;
    else
        color_cycle = FALSE;    

    // init stats.
    Clear_Game_Stats( &game_stats );
    strcpy( game_stats.name, game_configuration.pilot_name );

	// AJT: TODO figure out which song to play
	/*
	if ( game_configuration.music_on ) 
	{
		Play_Song( song_number );
		Set_Cd_Volume( game_configuration.music_vol );
	}
	*/

    /* In tournament, USER GETS SHITTY ASS AI WINGMAN!!!! */
	world_stuff.player_array[1].character.skill_level = 0;
	world_stuff.player_array[2].character.skill_level = 0;
    /* HEHEHEHEHEHEHEH                     */


	PlayGameIntroEnter();
	currentPlayGameState = PLAYGAMESTATE_INTRO;
}

void EndTournamentGame()
{
	free_world_stuff( &world_stuff );
	free_all_samples( &world_stuff );
	Free_Menu_Sounds();    
}


void SetupLevel( int level )
{
	game_configuration.blue0_ai = tournamentLevelData[level].blueAI[0];
	game_configuration.blue1_ai = tournamentLevelData[level].blueAI[1];
	game_configuration.blue2_ai = tournamentLevelData[level].blueAI[2];
	game_configuration.blue0_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue0_ai )].preferred_vehicle;
	game_configuration.blue1_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue1_ai )].preferred_vehicle;
	game_configuration.blue2_vehicle = wingman_menu_data.info[ai_to_selection( game_configuration.blue2_ai )].preferred_vehicle;

	strcpy( game_configuration.cylinder_filename, tournamentLevelData[level].cylinder_filename );

	// play animations.
    if ( game_configuration.animations_on ) 
	{
        Play_Fli( levelFli[level] );
    }	

	// TODO: play music

	// set's up proper pallette.
	display_next_opponent();
}

void PlayCustomGameEnter()
{	
	StartCustomGame();
}

void PlayCustomGameProcess()
{
	enum PlayGameStateEnum previousPlayGameState;

	previousPlayGameState = currentPlayGameState;

	// process the current game state.
	playGameStates[currentPlayGameState].process();

	// if the game state has changed.
	if ( previousPlayGameState != currentPlayGameState )
	{
		// exit the previous state.
		playGameStates[previousPlayGameState].exit();

		// enter the new one.
		playGameStates[currentPlayGameState].enter();
	}
}

void PlayCustomGameExit()
{
	EndCustomGame();
}


//
// GAMESTATE_TOURNAMENTGAME
//

void PlayTournamentGameEnter()
{
	tournamentLevel = level_warp;

	SetupLevel( tournamentLevel );
	StartTournamentGame();
}

void PlayTournamentGameProcess()
{
	enum PlayGameStateEnum previousPlayGameState;
	enum GameStateEnum previousGameState;

	previousPlayGameState = currentPlayGameState;
	previousGameState = currentGameState;

	// process the current game state.
	playGameStates[currentPlayGameState].process();

	// if the play game state has changed.
	if ( previousPlayGameState != currentPlayGameState )
	{
		// exit the previous state.
		playGameStates[previousPlayGameState].exit();

		// enter the new one.
		playGameStates[currentPlayGameState].enter();
	}

	// if the game state has changed, and the player one, move onto the next level
	if ( (previousGameState != currentGameState) && game_stats.victory )
	{
		currentGameState = GAMESTATE_TOURNAMENTGAME;

		EndTournamentGame();
		SetupLevel( ++tournamentLevel );
		StartTournamentGame();
	}
}

void PlayTournamentGameExit()
{
	EndTournamentGame();
}

//
// GAMESTATE_OUTRO
//

void OutroEnter()
{
	// AJT: TODO implement logo fade out or something..
	quitGame = 1;
}

void OutroProcess()
{

}

void OutroExit()
{

}



//
// PLAYGAMESTATE_INTRO
//

void PlayGameIntroEnter()
{
    // reset_player_array() will reset all of the players input_tables, orientations,
    // hit_points, etc.  Will reset the radar bases, clear all explosions and projectiles
    reset_game_to_start( &world_stuff, &level, &game_configuration, master );
    world_stuff.player_array[1].tank.alive = TRUE;
    world_stuff.player_array[2].tank.alive = TRUE;

    // reset all the voices stuff
	Init_Voices();

    // init the view_point
    world_stuff.view_orientation = world_stuff.player_array[user_vehicle_index()].tank.orient;
    world_stuff.view_orientation.position[X] += 1.8f * world_stuff.view_orientation.up[X] - 2.85f * world_stuff.view_orientation.front[X];
    world_stuff.view_orientation.position[Y] += 1.8f * world_stuff.view_orientation.up[Y] - 2.85f * world_stuff.view_orientation.front[Y];
    world_stuff.view_orientation.position[Z] += 1.8f * world_stuff.view_orientation.up[Z] - 2.85f * world_stuff.view_orientation.front[Z];

    // set transporting to true so that draw_everything doesnt draw the huds
    transporting = TRUE;
    first_person_view = FALSE;

    // play the round sample
	switch ( round )
	{
		case 0:	SYS_TriggerSound( computer_sample[ROUND_ONE] ); break;
		case 1:	SYS_TriggerSound( computer_sample[ROUND_TWO] ); break;
		case 2:	SYS_TriggerSound( computer_sample[FINAL_ROUND] ); break;
	}

	intro_start_time = SYS_GetTimeSeconds();
}

void PlayGameIntroProcess()
{
	float timer = SYS_GetTimeSeconds() - intro_start_time;
	const float fade_time = 2.0f;	
	float fade;

	// enable the world's palette.
	Enable_Color_Palette( &world_stuff.text );

	// calc the fade value.
	fade = timer / fade_time;

	// we are done fading.
	if ( fade > 1.0f )
	{
		fade = 1.0f;

		// move to the enter vehicle state
		currentPlayGameState = PLAYGAMESTATE_ENTERVEHICLE;
	}

	Set_Palette_Fade( fade );

	draw_everything( user_vehicle_index(), status_bar, first_person_view, transporting );
}

void PlayGameIntroExit()
{
	;
}


//
// PLAYGAMESTATE_ENTERVEHICLE
//

void PlayGameEnterVehicleEnter()
{
	soundState = 0;
}

void PlayGameEnterVehicleProcess()
{
	Float_Vector temp_vect;

	// AJT: todo frame rate clamping.

    // are we close enough to the vehicle yet?
    if ( distance_between( world_stuff.view_orientation.position, world_stuff.player_array[user_vehicle_index()].tank.orient.position ) < 1.0 ) 
	{
        world_stuff.view_orientation = world_stuff.player_array[user_vehicle_index()].tank.orient;
        draw_everything( user_vehicle_index(), status_bar, first_person_view, transporting );
        first_person_view = TRUE;
		currentPlayGameState = PLAYGAMESTATE_FIGHT;        
    }
    else 
	{
		// move the view_orientation.position into the vehicle
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

void PlayGameEnterVehicleExit()
{
	// This is to prevent a crash when the user hits v during the first frame.
	world_stuff.view_orientation = world_stuff.player_array[user_vehicle_index()].tank.orient;
	old_orient = world_stuff.player_array[user_vehicle_index()].tank.orient;

	transporting = FALSE;
}


//
// PLAYGAMESTATE_FIGHT
//

void PlayGameFightEnter()
{
    ;
}

void PlayGameFightProcess()
{
	unsigned int loopStartTime = SYS_GetTimeMS();
	
#ifdef PROFILE
	ptime = SYS_GetTimeMicro();
#endif

	// update some of the game_stats
	game_stats.user_vehicle = user_vehicle_index();
	game_stats.total_frames++;
	game_stats.total_d = game_stats.total_d + world_stuff.player_array[game_stats.user_vehicle].tank.laser_damage;

	// Get all the input tables for all the vehicles.
	// Users get their input from the keyboard, joystick or mouse.
	// Computer vehicles get their input from the AI.
	// Remotely controlled get their input from the null_modem */
	get_all_input( world_stuff.player_array, ai_active, master );

	// Move all the vehicles according to their input_tables one frame forward.
	// This function handles all the collisions between vehicles, pylons & projectiles.
	// Also creates and destroys projectiles, inflicts damage, scrambles controls ....
	// basicaly all the main game logic and physics takes place here */
	move_everything_one_click();

	// check to see if the round is over or not
	if ( is_round_over() ) 
	{
        if ( num_vehicles_remaining( local_user_team ) > 0 ) 			
            user_victories++;            
        else
            enemy_victories++;

		currentPlayGameState = PLAYGAMESTATE_OUTRO;
	}    

    // figure out where the current view_orientation is
	get_current_view_orient( &world_stuff.view_orientation, &old_orient,
                             local_user_team, &transporting, &first_person_view );

	// Draws the tube, pylons, vehicles and projectiles into the doubler_buffer.
	// Also draws the heads up display and the status bar.
    draw_everything( user_vehicle_index(), status_bar, first_person_view, transporting );
    
    if ( color_cycle ) 
	{
        // Note: this will only look right if we are playing in level10.dat
        Cycle_Palette_Section( 160, 20, &world_stuff.text );	// cycle the purple gradient
        Cycle_Palette_Section( 48, 95, &world_stuff.text );		// cycle the red gradient
        Cycle_Palette_Section( 112, 159, &world_stuff.text );	// cycle the blue gradient
        Enable_Color_Palette( &world_stuff.text );
    }


    // if 'v' is pressed change views
    if ( SYS_KeyPressed( KEY_V ) ) 
	{
        first_person_view = !first_person_view;
    }

    // if the user hits the esc key exit the loop and display the during_game menu
    if ( SYS_KeyPressed( KEY_ESC ) )
	{
		currentPlayGameState = PLAYGAMESTATE_MENU;
    }

    // if the user hits 'b' display the status bar (for debugging)
    if ( show_readout )
	{
		if ( SYS_KeyPressed( KEY_B ) ) 
			status_bar = !status_bar;				
	}

	// enable or disable the frame rate ceiling.
    if ( SYS_KeyPressed( KEY_C ) )
        ceiling_on = !ceiling_on;
    
#ifdef GLCYLINDRIX
	// enable or disable software only rendering.
    if ( SYS_KeyPressed( KEY_G ) )
        renderSoftwareBuffer = !renderSoftwareBuffer;            
#endif

#ifdef PROFILE
    if ( SYS_KeyPressed( KEY_P ) )
        profile = !profile;
#endif

    Play_Q_Samples( &world_stuff.view_orientation );

    if( game_configuration.sound_on )
        Handle_Voices( &world_stuff, user_vehicle_index(), (unsigned char)first_person_view, (unsigned char)transporting );    

    Handle_Commands( &world_stuff, user_vehicle_index() );

    update_leader( user_vehicle_index() );

    // Frame rate ceiling
	if ( ceiling_on )
	{
		while ( SYS_GetTimeMS() < loopStartTime + 33 );
	}
}

void PlayGameFightExit()
{

}


//
// PLAYGAMESTATE_OUTRO
//

void PlayGameOutroEnter()
{
	;
}

void PlayGameOutroProcess()
{
	unsigned int loopStartTime = SYS_GetTimeMS();
	float timer = SYS_GetTimeSeconds() - intro_start_time;
	const float fade_time = 2.0f;
	float fade;

	// enable the world's palette.
	Enable_Color_Palette( &world_stuff.text );

	if ( soundState == 5 )
	{
		// calc the fade value.
		fade = 1.0f - (timer / fade_time);
	}
	else
	{
		fade = 1.0f;
	}

	if ( soundState == 4 )
	{
		intro_start_time = SYS_GetTimeSeconds();
		soundState = 5;
	}

	// we are done fading.
	if ( fade < 0.0f )
	{
		fade = 0.0f;

		// move to the intro state.
		currentPlayGameState = PLAYGAMESTATE_INTRO;
	}

	Set_Palette_Fade( fade );

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

    // Frame rate ceiling
	if ( ceiling_on )
	{
		while ( SYS_GetTimeMS() < loopStartTime + 33 );
	}

    get_only_ai_input( world_stuff.player_array, ai_active, master );

    move_everything_one_click();

    // play the win sample
	if ( soundState == 0 )
	{
		if ( user_victories == 2 ) 
		{
			soundState = 1;
			if ( local_user_team == RED_TEAM )
				Play_Voice( computer_sample[RED_TEAM_WINS] );
			else
				Play_Voice( computer_sample[BLUE_TEAM_WINS] );
		}
		else if( enemy_victories == 2 )
		{
			soundState = 1;
			if ( local_user_team == RED_TEAM )
				Play_Voice( computer_sample[BLUE_TEAM_WINS] );
			else
				Play_Voice( computer_sample[RED_TEAM_WINS] );
		}
		else
		{
			soundState = 2;
		}
	}

	if ( soundState == 2 )
	{
		// play the wingman victory sample
		if ( num_vehicles_remaining( local_user_team ) > 0 ) 
		{
			soundState = 3;
			if ( user_victories == 1 ) 
			{
				if ( local_user_team == RED_TEAM )
					Play_Voice( world_stuff.player_array[1].character.samples[VICTORY] );
				else
					Play_Voice( world_stuff.player_array[4].character.samples[VICTORY] );
			}
			else 
			{
				if ( local_user_team == RED_TEAM ) 
					Play_Voice( world_stuff.player_array[2].character.samples[VICTORY] );
				else
					Play_Voice( world_stuff.player_array[5].character.samples[VICTORY] );
			}
		}
		else 
		{
			soundState = 3;
			if ( enemy_victories == 1 )
			{
				if( local_user_team == BLUE_TEAM )
					Play_Voice( world_stuff.player_array[1].character.samples[VICTORY] );
				else
					Play_Voice( world_stuff.player_array[4].character.samples[VICTORY] );
			}
			else 
			{
				if ( local_user_team == BLUE_TEAM )
					Play_Voice( world_stuff.player_array[2].character.samples[VICTORY] );
				else
					Play_Voice( world_stuff.player_array[5].character.samples[VICTORY] );
			}
		}
	}

	if ( (soundState == 1) && Is_Voice_Done() )
	{
		soundState = 2;
	} 
	else if ( (soundState == 3) && Is_Voice_Done() )
	{
		soundState = 4;
	}	
	
//	if ( SYS_KeyPressed( KEY_SPACE ) )
//	{
//		currentPlayGameState = PLAYGAMESTATE_INTRO;
//	}

}

void PlayGameOutroExit()
{
    // if one player has won over half of the total number of rounds
    // then the game is over

    if ( (user_victories > (num_rounds / 2)) || (enemy_victories > (num_rounds / 2)) ) 
	{
		if ( user_victories > enemy_victories ) 					
			game_stats.victory = 1;  // user won!		
		else		
			game_stats.victory = 0;  // user lost!

		Figure_Game_Stats( &game_stats );
		Add_Game_Stats_To_Overall_Stats( &game_stats, &overall_stats );

		currentPlayGameState = PLAYGAMESTATE_STATS;
    }
    else 
	{
        // go on to the next round
        Figure_Game_Stats( &game_stats );
        round++;
    }
}


//
// PLAYGAMESTATE_MENU
//

void PlayGameMenuEnter()
{
	Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
    
    DB_Clear_Screen();    
    Swap_Buffer();
    Enable_Color_Palette( &menu_stuff.general_menu_background );

    during_game_menu_event_init();
}

void PlayGameMenuProcess()
{
	int retVal;
    retVal = during_game_menu_event_loop();

	if ( retVal == 2 )	
		currentGameState = GAMESTATE_MAINMENU;
	else if ( retVal == 1 )
		currentPlayGameState = PLAYGAMESTATE_FIGHT;
}

void PlayGameMenuExit()
{
    DB_Clear_Screen();
    Swap_Buffer();    
    Enable_Color_Palette( &world_stuff.text );
    
    Figure_Game_Stats( &game_stats );
}


//
// PLAYGAMESTATE_STATS
//

void PlayGameStatsEnter()
{
    DB_Clear_Screen();    
    Swap_Buffer();
    Enable_Color_Palette( &menu_stuff.general_menu_background );

	tournament_game_stat_menu_event_init();
}

void PlayGameStatsProcess()
{
    // bring up custom_game_stat_menu
    int retVal;
	retVal = tournament_game_stat_menu_event_loop( game_stats.victory );

	if ( retVal == 1 )
		currentGameState = GAMESTATE_MAINMENU;
}

void PlayGameStatsExit()
{

}


//
// GameInit
//

void GameInit( int argc, const char* argv[] )
{
	SYS_TimerInit();
	GL_Cylindrix_Init();
	SYS_InitSound();

	init_game( argc, argv );

	// jump into the intro state.
	IntroEnter();
	currentGameState = GAMESTATE_INTRO;
}


//
// GameLoop
//

int GameLoop()
{
	enum GameStateEnum previousGameState;

	previousGameState = currentGameState;

	// process the current game state.
	gameStates[currentGameState].process();

	// if the game state has changed.
	if ( previousGameState != currentGameState )
	{
		// exit the previous state.
		gameStates[previousGameState].exit();

		// enter the new one.
		gameStates[currentGameState].enter();
	}

	return quitGame;
}

//
// GameShutdown
//

void GameShutdown()
{
	SYS_ShutdownSound();
}


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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "menu.h"
#include "types.h"
#include "prim.h"
#include "text.h"
#include "main.h"
#include "pcx.h"
#include "hud.h"
#include "keys.h"
#include "object.h"
#include <stdlib.h>
#include <math.h>
#include "fli.h"

// AJT: REMOVE
#include "stub.h"

#include "jonsb.h"  /* to play menu sounds */
#include "stats.h"
#include "config.h" /* to save game config */

#ifdef GLCYLINDRIX
#include "glcylindrix.h"
#endif


#define SELECTED_TEXT_COLOR 2
#define UNSELECTED_TEXT_COLOR 3

/* externs */

//From main.c

extern long exit_loop;  /* From omega.c */
extern long game_over;

extern char version_string[];

extern long program_over;    /* TRUE when user wishes to leave the program */

extern MenuStuff menu_stuff;

extern game_configuration_type game_configuration;

extern overall_stats_type overall_stats;

extern level_type level;

extern int master;

extern long timer_installed;        /* true if the timer interrupt is installed */
extern long keyboard_installed;     /* true if the keyboard interrupt is installed */
extern long multiplayer_game_only;  /* true if the cylindrix disk aint in the drive */

long quit_game = FALSE;

extern game_stats_type game_stats;

extern const char* g_DataPath;

/* globals */

/* current_menu is the menu currently active */

MenuScreen current_menu;


/* all of the menus, data and method prototypes */


MenuScreen pilot_selection_menu;
GeneralMenuData pilot_selection_menu_data;

void pilot_selection_menu_up_arrow();
void pilot_selection_menu_down_arrow();
void pilot_selection_menu_right_arrow();
void pilot_selection_menu_left_arrow();
void pilot_selection_menu_return_key();
void pilot_selection_menu_escape_key();
void pilot_selection_menu_draw_menu();


MenuScreen pilot_input_menu;
GeneralMenuData pilot_input_menu_data;
char pilot_name[80] = "";

void pilot_input_menu_up_arrow();
void pilot_input_menu_down_arrow();
void pilot_input_menu_right_arrow();
void pilot_input_menu_left_arrow();
void pilot_input_menu_return_key();
void pilot_input_menu_escape_key();
void pilot_input_menu_draw_menu();


MenuScreen pilot_delete_menu;
GeneralMenuData pilot_delete_menu_data;

void pilot_delete_menu_up_arrow();
void pilot_delete_menu_down_arrow();
void pilot_delete_menu_right_arrow();
void pilot_delete_menu_left_arrow();
void pilot_delete_menu_return_key();
void pilot_delete_menu_escape_key();
void pilot_delete_menu_draw_menu();



MenuScreen main_menu;
GeneralMenuData main_menu_data;

void main_menu_up_arrow();
void main_menu_down_arrow();
void main_menu_right_arrow();
void main_menu_left_arrow();
void main_menu_return_key();
void main_menu_escape_key();
void main_menu_draw_menu();


MenuScreen options_menu;
GeneralMenuData options_menu_data;

void options_menu_up_arrow();
void options_menu_down_arrow();
void options_menu_right_arrow();
void options_menu_left_arrow();
void options_menu_return_key();
void options_menu_escape_key();
void options_menu_draw_menu();


MenuScreen detail_menu;
GeneralMenuData detail_menu_data;

void detail_menu_up_arrow();
void detail_menu_down_arrow();
void detail_menu_right_arrow();
void detail_menu_left_arrow();
void detail_menu_return_key();
void detail_menu_escape_key();
void detail_menu_draw_menu();


MenuScreen sound_menu;
GeneralMenuData sound_menu_data;

void sound_menu_up_arrow();
void sound_menu_down_arrow();
void sound_menu_right_arrow();
void sound_menu_left_arrow();
void sound_menu_return_key();
void sound_menu_escape_key();
void sound_menu_draw_menu();


MenuScreen keyboard_menu;
GeneralMenuData keyboard_menu_data;
int keyboard_menu_waitforkey = 0;

void keyboard_menu_up_arrow();
void keyboard_menu_down_arrow();
void keyboard_menu_right_arrow();
void keyboard_menu_left_arrow();
void keyboard_menu_return_key();
void keyboard_menu_escape_key();
void keyboard_menu_draw_menu();


MenuScreen joystick_menu;
GeneralMenuData joystick_menu_data;

void joystick_menu_up_arrow();
void joystick_menu_down_arrow();
void joystick_menu_right_arrow();
void joystick_menu_left_arrow();
void joystick_menu_return_key();
void joystick_menu_escape_key();
void joystick_menu_draw_menu();


MenuScreen calibrate_menu;

void calibrate_menu_up_arrow();
void calibrate_menu_down_arrow();
void calibrate_menu_right_arrow();
void calibrate_menu_left_arrow();
void calibrate_menu_return_key();
void calibrate_menu_escape_key();
void calibrate_menu_draw_menu();


MenuScreen joystick_config_menu;
GeneralMenuData joystick_config_menu_data;

void joystick_config_menu_up_arrow();
void joystick_config_menu_down_arrow();
void joystick_config_menu_right_arrow();
void joystick_config_menu_left_arrow();
void joystick_config_menu_return_key();
void joystick_config_menu_escape_key();
void joystick_config_menu_draw_menu();


MenuScreen game_menu;
GeneralMenuData game_menu_data;

void game_menu_up_arrow();
void game_menu_down_arrow();
void game_menu_right_arrow();
void game_menu_left_arrow();
void game_menu_return_key();
void game_menu_escape_key();
void game_menu_draw_menu();




MenuScreen custom_game_menu;
GeneralMenuData custom_game_menu_data;

void custom_game_menu_up_arrow();
void custom_game_menu_down_arrow();
void custom_game_menu_right_arrow();
void custom_game_menu_left_arrow();
void custom_game_menu_return_key();
void custom_game_menu_escape_key();
void custom_game_menu_draw_menu();


MenuScreen cylindrix_menu;
GeneralMenuData cylindrix_menu_data;

void cylindrix_menu_up_arrow();
void cylindrix_menu_down_arrow();
void cylindrix_menu_right_arrow();
void cylindrix_menu_left_arrow();
void cylindrix_menu_return_key();
void cylindrix_menu_escape_key();
void cylindrix_menu_draw_menu();


MenuScreen view_stats_menu;
GeneralMenuData view_stats_menu_data;

void view_stats_menu_up_arrow();
void view_stats_menu_down_arrow();
void view_stats_menu_right_arrow();
void view_stats_menu_left_arrow();
void view_stats_menu_return_key();
void view_stats_menu_escape_key();
void view_stats_menu_draw_menu();


MenuScreen custom_wingman1_menu;
MenuScreen custom_wingman2_menu;
MenuScreen custom_opponent1_menu;
MenuScreen custom_opponent2_menu;
MenuScreen custom_opponent3_menu;

WingmanMenuData wingman_menu_data;

void wingman_menu_up_arrow();
void wingman_menu_down_arrow();
void wingman_menu_right_arrow();
void wingman_menu_left_arrow();

void custom_wingman1_menu_return_key();
void custom_wingman2_menu_return_key();
void custom_opponent1_menu_return_key();
void custom_opponent2_menu_return_key();
void custom_opponent3_menu_return_key();

void custom_wingman1_menu_escape_key();
void custom_wingman2_menu_escape_key();
void custom_opponent1_menu_escape_key();
void custom_opponent2_menu_escape_key();
void custom_opponent3_menu_escape_key();

void wingman_menu_draw_menu();

void init_custom_wingman1_menu();
void init_custom_wingman2_menu();
void init_custom_opponent1_menu();
void init_custom_opponent2_menu();
void init_custom_opponent3_menu();

MenuScreen custom_vehicle0_menu;
MenuScreen custom_vehicle1_menu;
MenuScreen custom_vehicle2_menu;
MenuScreen custom_vehicle3_menu;
MenuScreen custom_vehicle4_menu;
MenuScreen custom_vehicle5_menu;

VehicleMenuData vehicle_menu_data;

void vehicle_menu_up_arrow();
void vehicle_menu_down_arrow();
void vehicle_menu_right_arrow();
void vehicle_menu_left_arrow();

void custom_vehicle0_menu_return_key();
void custom_vehicle1_menu_return_key();
void custom_vehicle2_menu_return_key();
void custom_vehicle3_menu_return_key();
void custom_vehicle4_menu_return_key();
void custom_vehicle5_menu_return_key();

void custom_vehicle0_menu_escape_key();
void custom_vehicle1_menu_escape_key();
void custom_vehicle2_menu_escape_key();
void custom_vehicle3_menu_escape_key();
void custom_vehicle4_menu_escape_key();
void custom_vehicle5_menu_escape_key();

void vehicle_menu_draw_menu();

void init_custom_vehicle0_menu();
void init_custom_vehicle1_menu();
void init_custom_vehicle2_menu();
void init_custom_vehicle3_menu();
void init_custom_vehicle4_menu();
void init_custom_vehicle5_menu();

MenuScreen during_game_menu;
GeneralMenuData during_game_menu_data;

void during_game_menu_up_arrow();
void during_game_menu_down_arrow();
void during_game_menu_right_arrow();
void during_game_menu_left_arrow();
void during_game_menu_return_key();
void during_game_menu_escape_key();
void during_game_menu_draw_menu();


MenuScreen during_game_options_menu;
GeneralMenuData during_game_options_menu_data;

void during_game_options_menu_up_arrow();
void during_game_options_menu_down_arrow();
void during_game_options_menu_right_arrow();
void during_game_options_menu_left_arrow();
void during_game_options_menu_return_key();
void during_game_options_menu_escape_key();
void during_game_options_menu_draw_menu();


MenuScreen during_game_detail_menu;
GeneralMenuData during_game_detail_menu_data;

void during_game_detail_menu_up_arrow();
void during_game_detail_menu_down_arrow();
void during_game_detail_menu_right_arrow();
void during_game_detail_menu_left_arrow();
void during_game_detail_menu_return_key();
void during_game_detail_menu_escape_key();
void during_game_detail_menu_draw_menu();


MenuScreen during_game_sound_menu;
GeneralMenuData during_game_sound_menu_data;

void during_game_sound_menu_up_arrow();
void during_game_sound_menu_down_arrow();
void during_game_sound_menu_right_arrow();
void during_game_sound_menu_left_arrow();
void during_game_sound_menu_return_key();
void during_game_sound_menu_escape_key();
void during_game_sound_menu_draw_menu();


MenuScreen during_game_keyboard_menu;
GeneralMenuData during_game_keyboard_menu_data;

void during_game_keyboard_menu_up_arrow();
void during_game_keyboard_menu_down_arrow();
void during_game_keyboard_menu_right_arrow();
void during_game_keyboard_menu_left_arrow();
void during_game_keyboard_menu_return_key();
void during_game_keyboard_menu_escape_key();
void during_game_keyboard_menu_draw_menu();


MenuScreen during_game_joystick_menu;
GeneralMenuData during_game_joystick_menu_data;

void during_game_joystick_menu_up_arrow();
void during_game_joystick_menu_down_arrow();
void during_game_joystick_menu_right_arrow();
void during_game_joystick_menu_left_arrow();
void during_game_joystick_menu_return_key();
void during_game_joystick_menu_escape_key();
void during_game_joystick_menu_draw_menu();


MenuScreen during_game_calibrate_menu;

void during_game_calibrate_menu_up_arrow();
void during_game_calibrate_menu_down_arrow();
void during_game_calibrate_menu_right_arrow();
void during_game_calibrate_menu_left_arrow();
void during_game_calibrate_menu_return_key();
void during_game_calibrate_menu_escape_key();
void during_game_calibrate_menu_draw_menu();


MenuScreen during_game_joystick_config_menu;
GeneralMenuData during_game_joystick_config_menu_data;

void during_game_joystick_config_menu_up_arrow();
void during_game_joystick_config_menu_down_arrow();
void during_game_joystick_config_menu_right_arrow();
void during_game_joystick_config_menu_left_arrow();
void during_game_joystick_config_menu_return_key();
void during_game_joystick_config_menu_escape_key();
void during_game_joystick_config_menu_draw_menu();


MenuScreen custom_game_stat_menu;
GeneralMenuData custom_game_stat_menu_data;

void custom_game_stat_menu_up_arrow();
void custom_game_stat_menu_down_arrow();
void custom_game_stat_menu_right_arrow();
void custom_game_stat_menu_left_arrow();
void custom_game_stat_menu_return_key();
void custom_game_stat_menu_escape_key();
void custom_game_stat_menu_draw_menu( int victory );


MenuScreen tournament_game_stat_menu;
GeneralMenuData tournament_game_stat_menu_data;

void tournament_game_stat_menu_up_arrow();
void tournament_game_stat_menu_down_arrow();
void tournament_game_stat_menu_right_arrow();
void tournament_game_stat_menu_left_arrow();
void tournament_game_stat_menu_return_key();
void tournament_game_stat_menu_escape_key();
void tournament_game_stat_menu_draw_menu( int victory );


MenuScreen tournament_game_setup_menu;
GeneralMenuData tournament_game_setup_menu_data;

void tournament_game_menu_up_arrow();
void tournament_game_menu_down_arrow();
void tournament_game_menu_right_arrow();
void tournament_game_menu_left_arrow();
void tournament_game_menu_return_key();
void tournament_game_menu_escape_key();
void tournament_game_menu_draw_menu();

MenuScreen tournament_vehicle0_menu;
MenuScreen tournament_vehicle1_menu;
MenuScreen tournament_vehicle2_menu;

void tournament_vehicle0_menu_return_key();
void tournament_vehicle1_menu_return_key();
void tournament_vehicle2_menu_return_key();

void tournament_vehicle0_menu_escape_key();
void tournament_vehicle1_menu_escape_key();
void tournament_vehicle2_menu_escape_key();

void init_tournament_vehicle0_menu();
void init_tournament_vehicle1_menu();
void init_tournament_vehicle2_menu();

MenuScreen tournament_wingman1_menu;
MenuScreen tournament_wingman2_menu;

void tournament_wingman1_menu_return_key();
void tournament_wingman2_menu_return_key();

void tournament_wingman1_menu_escape_key();
void tournament_wingman2_menu_escape_key();

void init_tournament_wingman1_menu();
void init_tournament_wingman2_menu();

void tournament_wingman_menu_up_arrow();
void tournament_wingman_menu_down_arrow();
void tournament_wingman_menu_right_arrow();
void tournament_wingman_menu_left_arrow();

/* exit_event_loop tells the event loop when to exit */

long exit_event_loop = FALSE;

void make_menu_current( MenuScreen *menu )
{
    current_menu.up_arrow = menu->up_arrow;
    current_menu.down_arrow = menu->down_arrow;
    current_menu.right_arrow = menu->right_arrow;
    current_menu.left_arrow = menu->left_arrow;
    current_menu.return_key = menu->return_key;
    current_menu.escape_key = menu->escape_key;
    current_menu.draw_menu = menu->draw_menu;
	current_menu.delay_input = 1;
}



/* methods for pilot_selection_menu */


int pilot_selection_menu_current_selection_valid()
{
    if( pilot_selection_menu_data.current_selection == 0 ) {

        if( strcmp( game_configuration.pilot_names.slot0, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_selection_menu_data.current_selection == 1 ) {

        if( strcmp( game_configuration.pilot_names.slot1, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_selection_menu_data.current_selection == 2 ) {

        if( strcmp( game_configuration.pilot_names.slot2, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_selection_menu_data.current_selection == 3 ) {

        if( strcmp( game_configuration.pilot_names.slot3, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_selection_menu_data.current_selection == 4 ) {

        if( strcmp( game_configuration.pilot_names.slot4, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_selection_menu_data.current_selection == 5 ) {

        if( strcmp( game_configuration.pilot_names.slot5, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_selection_menu_data.current_selection == 6 ) {

        if( strcmp( game_configuration.pilot_names.slot6, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_selection_menu_data.current_selection == 7 ) {

        if( strcmp( game_configuration.pilot_names.slot7, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_selection_menu_data.current_selection == 8 ) {
        return TRUE;
    }
    else if( pilot_selection_menu_data.current_selection == 9 ) {
        return TRUE;
    }
    else {
        return FALSE;
    }

}




void pilot_selection_menu_up_arrow()
{
    pilot_selection_menu_data.current_selection = (pilot_selection_menu_data.current_selection - 1) % pilot_selection_menu_data.num_items;

    if( pilot_selection_menu_data.current_selection < 0 ) {
        pilot_selection_menu_data.current_selection = pilot_selection_menu_data.num_items - 1;
    }

    /* make sure we find the next valid selection */

    while( !pilot_selection_menu_current_selection_valid() ) {

        pilot_selection_menu_data.current_selection = (pilot_selection_menu_data.current_selection - 1) % pilot_selection_menu_data.num_items;

        if( pilot_selection_menu_data.current_selection < 0 ) {
            pilot_selection_menu_data.current_selection = pilot_selection_menu_data.num_items - 1;
        }
    }

	Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void pilot_selection_menu_down_arrow()
{
    pilot_selection_menu_data.current_selection = (pilot_selection_menu_data.current_selection + 1) % pilot_selection_menu_data.num_items;

    /* make sure we find the next valid selection */

    while( !pilot_selection_menu_current_selection_valid() ) {
        pilot_selection_menu_data.current_selection = (pilot_selection_menu_data.current_selection + 1) % pilot_selection_menu_data.num_items;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void pilot_selection_menu_right_arrow()
{
    ;
}

void pilot_selection_menu_left_arrow()
{
    ;
}

void pilot_selection_menu_return_key()
{

    if( pilot_selection_menu_data.current_selection == 0 ) { /* slot0 */

        if( strcmp( game_configuration.pilot_names.slot0, "" ) == 0 ) {
            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* copy slot0 name into the username field of game_configuration */

            strcpy( game_configuration.pilot_name, game_configuration.pilot_names.slot0 );
            game_configuration.slot_num = 0;

            Load_Overall_Stats_Binary( &overall_stats, "gamedata/xxx0.xxx" );

            make_menu_current( &main_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }
    }
    else if( pilot_selection_menu_data.current_selection == 1 ) { /* slot1 */

         if( strcmp( game_configuration.pilot_names.slot1, "" ) == 0 ) {
			Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* copy slot1 name into the username field of game_configuration */

            strcpy( game_configuration.pilot_name, game_configuration.pilot_names.slot1 );
            game_configuration.slot_num = 1;

            Load_Overall_Stats_Binary( &overall_stats, "gamedata/xxx1.xxx" );

            make_menu_current( &main_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }


    }
    else if( pilot_selection_menu_data.current_selection == 2 ) { /* slot2 */

        if( strcmp( game_configuration.pilot_names.slot2, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* copy slot2 name into the username field of game_configuration */

            strcpy( game_configuration.pilot_name, game_configuration.pilot_names.slot2 );
            game_configuration.slot_num = 2;

            Load_Overall_Stats_Binary( &overall_stats, "gamedata/xxx2.xxx" );

            make_menu_current( &main_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_selection_menu_data.current_selection == 3 ) { /* slot3 */

        if( strcmp( game_configuration.pilot_names.slot3, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* copy slot3 name into the username field of game_configuration */

            strcpy( game_configuration.pilot_name, game_configuration.pilot_names.slot3 );
            game_configuration.slot_num = 3;

            Load_Overall_Stats_Binary( &overall_stats, "gamedata/xxx3.xxx" );

            make_menu_current( &main_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_selection_menu_data.current_selection == 4 ) { /* slot4 */

        if( strcmp( game_configuration.pilot_names.slot4, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* copy slot4 name into the username field of game_configuration */

            strcpy( game_configuration.pilot_name, game_configuration.pilot_names.slot4 );
            game_configuration.slot_num = 4;

            Load_Overall_Stats_Binary( &overall_stats, "gamedata/xxx4.xxx" );

            make_menu_current( &main_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_selection_menu_data.current_selection == 5 ) { /* slot5 */

        if( strcmp( game_configuration.pilot_names.slot5, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* copy slot5 name into the username field of game_configuration */

            strcpy( game_configuration.pilot_name, game_configuration.pilot_names.slot5 );
            game_configuration.slot_num = 5;

            Load_Overall_Stats_Binary( &overall_stats, "gamedata/xxx5.xxx" );

            make_menu_current( &main_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_selection_menu_data.current_selection == 6 ) { /* slot6 */

        if( strcmp( game_configuration.pilot_names.slot6, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* copy slot6 name into the username field of game_configuration */

            strcpy( game_configuration.pilot_name, game_configuration.pilot_names.slot6 );
            game_configuration.slot_num = 6;

            Load_Overall_Stats_Binary( &overall_stats, "gamedata/xxx6.xxx" );

            make_menu_current( &main_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_selection_menu_data.current_selection == 7 ) { /* slot7 */

        if( strcmp( game_configuration.pilot_names.slot7, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* copy slot7 name into the username field of game_configuration */

            strcpy( game_configuration.pilot_name, game_configuration.pilot_names.slot7 );
            game_configuration.slot_num = 7;

            Load_Overall_Stats_Binary( &overall_stats, "gamedata/xxx7.xxx" );

            make_menu_current( &main_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_selection_menu_data.current_selection == 8 ) { /* create new pilot */

        if( (strcmp( game_configuration.pilot_names.slot0, "" ) != 0) &&
            (strcmp( game_configuration.pilot_names.slot1, "" ) != 0) &&
            (strcmp( game_configuration.pilot_names.slot2, "" ) != 0) &&
            (strcmp( game_configuration.pilot_names.slot3, "" ) != 0) &&
            (strcmp( game_configuration.pilot_names.slot4, "" ) != 0) &&
            (strcmp( game_configuration.pilot_names.slot5, "" ) != 0) &&
            (strcmp( game_configuration.pilot_names.slot6, "" ) != 0) &&
            (strcmp( game_configuration.pilot_names.slot7, "" ) != 0) ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }
        else {

            make_menu_current( &pilot_input_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }
    }
    else if( pilot_selection_menu_data.current_selection == 9 ) { /* delete pilot */

        if( (strcmp( game_configuration.pilot_names.slot0, "" ) == 0) &&
            (strcmp( game_configuration.pilot_names.slot1, "" ) == 0) &&
            (strcmp( game_configuration.pilot_names.slot2, "" ) == 0) &&
            (strcmp( game_configuration.pilot_names.slot3, "" ) == 0) &&
            (strcmp( game_configuration.pilot_names.slot4, "" ) == 0) &&
            (strcmp( game_configuration.pilot_names.slot5, "" ) == 0) &&
            (strcmp( game_configuration.pilot_names.slot6, "" ) == 0) &&
            (strcmp( game_configuration.pilot_names.slot7, "" ) == 0) ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }
        else {

            make_menu_current( &pilot_delete_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }

    }


}

void pilot_selection_menu_escape_key()
{
    Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
}

void pilot_selection_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 80;
    const long y_start = 45;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    /* draw title */

    string_blit( "SELECT YOUR PILOT NAME", 35, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    x = x_start;
    y = y_start;

    for( i = 0; i < pilot_selection_menu_data.num_items; i++ ) {

        if( i == pilot_selection_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( pilot_selection_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }    

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}

void init_pilot_selection_menu()
{
    pilot_selection_menu.up_arrow = (Method)pilot_selection_menu_up_arrow;
    pilot_selection_menu.down_arrow = (Method)pilot_selection_menu_down_arrow;
    pilot_selection_menu.right_arrow = (Method)pilot_selection_menu_right_arrow;
    pilot_selection_menu.left_arrow = (Method)pilot_selection_menu_left_arrow;
    pilot_selection_menu.return_key = (Method)pilot_selection_menu_return_key;
    pilot_selection_menu.escape_key = (Method)pilot_selection_menu_escape_key;
    pilot_selection_menu.draw_menu = (Method)pilot_selection_menu_draw_menu;

    strcpy( pilot_selection_menu_data.item[0], game_configuration.pilot_names.slot0 );
    strcpy( pilot_selection_menu_data.item[1], game_configuration.pilot_names.slot1 );
    strcpy( pilot_selection_menu_data.item[2], game_configuration.pilot_names.slot2 );
    strcpy( pilot_selection_menu_data.item[3], game_configuration.pilot_names.slot3 );
    strcpy( pilot_selection_menu_data.item[4], game_configuration.pilot_names.slot4 );
    strcpy( pilot_selection_menu_data.item[5], game_configuration.pilot_names.slot5 );
    strcpy( pilot_selection_menu_data.item[6], game_configuration.pilot_names.slot6 );
    strcpy( pilot_selection_menu_data.item[7], game_configuration.pilot_names.slot7 );
    strcpy( pilot_selection_menu_data.item[8], "CREATE NEW PILOT" );
    strcpy( pilot_selection_menu_data.item[9], "DELETE PILOT" );

    pilot_selection_menu_data.num_items = 10;

    pilot_selection_menu_data.current_selection = 8;
}


/* methods for pilot_delete_menu */


int pilot_delete_menu_current_selection_valid()
{
    if( pilot_delete_menu_data.current_selection == 0 ) {

        if( strcmp( game_configuration.pilot_names.slot0, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_delete_menu_data.current_selection == 1 ) {

        if( strcmp( game_configuration.pilot_names.slot1, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_delete_menu_data.current_selection == 2 ) {

        if( strcmp( game_configuration.pilot_names.slot2, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_delete_menu_data.current_selection == 3 ) {

        if( strcmp( game_configuration.pilot_names.slot3, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_delete_menu_data.current_selection == 4 ) {

        if( strcmp( game_configuration.pilot_names.slot4, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_delete_menu_data.current_selection == 5 ) {

        if( strcmp( game_configuration.pilot_names.slot5, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_delete_menu_data.current_selection == 6 ) {

        if( strcmp( game_configuration.pilot_names.slot6, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if( pilot_delete_menu_data.current_selection == 7 ) {

        if( strcmp( game_configuration.pilot_names.slot7, "" ) == 0 ) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else {
        return FALSE;
    }

}



void pilot_delete_menu_up_arrow()
{
    pilot_delete_menu_data.current_selection = (pilot_delete_menu_data.current_selection - 1) % pilot_delete_menu_data.num_items;

    if( pilot_delete_menu_data.current_selection < 0 ) {
        pilot_delete_menu_data.current_selection = pilot_delete_menu_data.num_items - 1;
    }

    /* make sure we find the next valid selection */

    while( !pilot_delete_menu_current_selection_valid() ) {

        pilot_delete_menu_data.current_selection = (pilot_delete_menu_data.current_selection - 1) % pilot_delete_menu_data.num_items;

        if( pilot_delete_menu_data.current_selection < 0 ) {
            pilot_delete_menu_data.current_selection = pilot_delete_menu_data.num_items - 1;
        }
    }


    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void pilot_delete_menu_down_arrow()
{
    pilot_delete_menu_data.current_selection = (pilot_delete_menu_data.current_selection + 1) % pilot_delete_menu_data.num_items;

    /* make sure we find the next valid selection */

    while( !pilot_delete_menu_current_selection_valid() ) {
        pilot_delete_menu_data.current_selection = (pilot_delete_menu_data.current_selection + 1) % pilot_delete_menu_data.num_items;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void pilot_delete_menu_right_arrow()
{
    ;
}

void pilot_delete_menu_left_arrow()
{
    ;
}

void pilot_delete_menu_return_key()
{

    if( pilot_delete_menu_data.current_selection == 0 ) { /* slot0 */

        if( strcmp( game_configuration.pilot_names.slot0, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* delete this pilot and move the others up */

            strcpy( game_configuration.pilot_names.slot0, "" );

            strcpy( game_configuration.pilot_names.slot0, game_configuration.pilot_names.slot1 );

            strcpy( game_configuration.pilot_names.slot1, game_configuration.pilot_names.slot2 );

            strcpy( game_configuration.pilot_names.slot2, game_configuration.pilot_names.slot3 );

            strcpy( game_configuration.pilot_names.slot3, game_configuration.pilot_names.slot4 );

            strcpy( game_configuration.pilot_names.slot4, game_configuration.pilot_names.slot5 );

            strcpy( game_configuration.pilot_names.slot5, game_configuration.pilot_names.slot6 );

            strcpy( game_configuration.pilot_names.slot6, game_configuration.pilot_names.slot7 );

            strcpy( game_configuration.pilot_names.slot7, "" );

            /* zero out the stat file */

            make_menu_current( &pilot_selection_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }
    }
    else if( pilot_delete_menu_data.current_selection == 1 ) { /* slot1 */

         if( strcmp( game_configuration.pilot_names.slot1, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* delete this pilot and move the others up */

            strcpy( game_configuration.pilot_names.slot1, "" );

            strcpy( game_configuration.pilot_names.slot1, game_configuration.pilot_names.slot2 );

            strcpy( game_configuration.pilot_names.slot2, game_configuration.pilot_names.slot3 );

            strcpy( game_configuration.pilot_names.slot3, game_configuration.pilot_names.slot4 );

            strcpy( game_configuration.pilot_names.slot4, game_configuration.pilot_names.slot5 );

            strcpy( game_configuration.pilot_names.slot5, game_configuration.pilot_names.slot6 );

            strcpy( game_configuration.pilot_names.slot6, game_configuration.pilot_names.slot7 );

            strcpy( game_configuration.pilot_names.slot7, "" );

            /* zero out the stat file */

            make_menu_current( &pilot_selection_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }


    }
    else if( pilot_delete_menu_data.current_selection == 2 ) { /* slot2 */

        if( strcmp( game_configuration.pilot_names.slot2, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* delete this pilot and move the others up */

            strcpy( game_configuration.pilot_names.slot2, "" );

            strcpy( game_configuration.pilot_names.slot2, game_configuration.pilot_names.slot3 );

            strcpy( game_configuration.pilot_names.slot3, game_configuration.pilot_names.slot4 );

            strcpy( game_configuration.pilot_names.slot4, game_configuration.pilot_names.slot5 );

            strcpy( game_configuration.pilot_names.slot5, game_configuration.pilot_names.slot6 );

            strcpy( game_configuration.pilot_names.slot6, game_configuration.pilot_names.slot7 );

            strcpy( game_configuration.pilot_names.slot7, "" );

            /* zero out the stat file */

            make_menu_current( &pilot_selection_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_delete_menu_data.current_selection == 3 ) { /* slot3 */

        if( strcmp( game_configuration.pilot_names.slot3, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* delete this pilot and move the others up */

            strcpy( game_configuration.pilot_names.slot3, "" );

            strcpy( game_configuration.pilot_names.slot3, game_configuration.pilot_names.slot4 );

            strcpy( game_configuration.pilot_names.slot4, game_configuration.pilot_names.slot5 );

            strcpy( game_configuration.pilot_names.slot5, game_configuration.pilot_names.slot6 );

            strcpy( game_configuration.pilot_names.slot6, game_configuration.pilot_names.slot7 );

            strcpy( game_configuration.pilot_names.slot7, "" );

            /* zero out the stat file */

            make_menu_current( &pilot_selection_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_delete_menu_data.current_selection == 4 ) { /* slot4 */

        if( strcmp( game_configuration.pilot_names.slot4, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* delete this pilot and move the others up */

            strcpy( game_configuration.pilot_names.slot4, "" );

            strcpy( game_configuration.pilot_names.slot4, game_configuration.pilot_names.slot5 );

            strcpy( game_configuration.pilot_names.slot5, game_configuration.pilot_names.slot6 );

            strcpy( game_configuration.pilot_names.slot6, game_configuration.pilot_names.slot7 );

            strcpy( game_configuration.pilot_names.slot7, "" );

            /* zero out the stat file */

            make_menu_current( &pilot_selection_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_delete_menu_data.current_selection == 5 ) { /* slot5 */

        if( strcmp( game_configuration.pilot_names.slot5, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* delete this pilot and move the others up */

            strcpy( game_configuration.pilot_names.slot5, "" );

            strcpy( game_configuration.pilot_names.slot5, game_configuration.pilot_names.slot6 );

            strcpy( game_configuration.pilot_names.slot6, game_configuration.pilot_names.slot7 );

            strcpy( game_configuration.pilot_names.slot7, "" );

            /* zero out the stat file */

            make_menu_current( &pilot_selection_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_delete_menu_data.current_selection == 6 ) { /* slot6 */

        if( strcmp( game_configuration.pilot_names.slot6, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* delete this pilot and move the others up */

            strcpy( game_configuration.pilot_names.slot6, "" );

            strcpy( game_configuration.pilot_names.slot6, game_configuration.pilot_names.slot7 );

            strcpy( game_configuration.pilot_names.slot7, "" );

            /* zero out the stat file */

            make_menu_current( &pilot_selection_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( pilot_delete_menu_data.current_selection == 7 ) { /* slot7 */

        if( strcmp( game_configuration.pilot_names.slot7, "" ) == 0 ) {

            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            /* delete this pilot and move the others up */

            strcpy( game_configuration.pilot_names.slot7, "" );

            /* zero out the stat file */

            make_menu_current( &pilot_selection_menu );

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }

    strcpy( pilot_selection_menu_data.item[0], game_configuration.pilot_names.slot0 );
    strcpy( pilot_selection_menu_data.item[1], game_configuration.pilot_names.slot1 );
    strcpy( pilot_selection_menu_data.item[2], game_configuration.pilot_names.slot2 );
    strcpy( pilot_selection_menu_data.item[3], game_configuration.pilot_names.slot3 );
    strcpy( pilot_selection_menu_data.item[4], game_configuration.pilot_names.slot4 );
    strcpy( pilot_selection_menu_data.item[5], game_configuration.pilot_names.slot5 );
    strcpy( pilot_selection_menu_data.item[6], game_configuration.pilot_names.slot6 );
    strcpy( pilot_selection_menu_data.item[7], game_configuration.pilot_names.slot7 );
    strcpy( pilot_selection_menu_data.item[8], "CREATE NEW PILOT" );
    strcpy( pilot_selection_menu_data.item[9], "DELETE PILOT" );

    strcpy( pilot_delete_menu_data.item[0], game_configuration.pilot_names.slot0 );
    strcpy( pilot_delete_menu_data.item[1], game_configuration.pilot_names.slot1 );
    strcpy( pilot_delete_menu_data.item[2], game_configuration.pilot_names.slot2 );
    strcpy( pilot_delete_menu_data.item[3], game_configuration.pilot_names.slot3 );
    strcpy( pilot_delete_menu_data.item[4], game_configuration.pilot_names.slot4 );
    strcpy( pilot_delete_menu_data.item[5], game_configuration.pilot_names.slot5 );
    strcpy( pilot_delete_menu_data.item[6], game_configuration.pilot_names.slot6 );
    strcpy( pilot_delete_menu_data.item[7], game_configuration.pilot_names.slot7 );

}

void pilot_delete_menu_escape_key()
{
    make_menu_current( &pilot_selection_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void pilot_delete_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 80;
    const long y_start = 45;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    /* draw title */

    string_blit( "SELECT PILOT TO BE DELETED", 10, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    x = x_start;
    y = y_start;

    for( i = 0; i < pilot_delete_menu_data.num_items; i++ ) {

        if( i == pilot_delete_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( pilot_delete_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}

void init_pilot_delete_menu()
{
    pilot_delete_menu.up_arrow = (Method)pilot_delete_menu_up_arrow;
    pilot_delete_menu.down_arrow = (Method)pilot_delete_menu_down_arrow;
    pilot_delete_menu.right_arrow = (Method)pilot_delete_menu_right_arrow;
    pilot_delete_menu.left_arrow = (Method)pilot_delete_menu_left_arrow;
    pilot_delete_menu.return_key = (Method)pilot_delete_menu_return_key;
    pilot_delete_menu.escape_key = (Method)pilot_delete_menu_escape_key;
    pilot_delete_menu.draw_menu = (Method)pilot_delete_menu_draw_menu;

    strcpy( pilot_delete_menu_data.item[0], game_configuration.pilot_names.slot0 );
    strcpy( pilot_delete_menu_data.item[1], game_configuration.pilot_names.slot1 );
    strcpy( pilot_delete_menu_data.item[2], game_configuration.pilot_names.slot2 );
    strcpy( pilot_delete_menu_data.item[3], game_configuration.pilot_names.slot3 );
    strcpy( pilot_delete_menu_data.item[4], game_configuration.pilot_names.slot4 );
    strcpy( pilot_delete_menu_data.item[5], game_configuration.pilot_names.slot5 );
    strcpy( pilot_delete_menu_data.item[6], game_configuration.pilot_names.slot6 );
    strcpy( pilot_delete_menu_data.item[7], game_configuration.pilot_names.slot7 );

    pilot_delete_menu_data.num_items = 8;

    pilot_delete_menu_data.current_selection = 0;
}




/* methods for pilot_input_menu */



void pilot_input_menu_up_arrow()
{
    ;
}

void pilot_input_menu_down_arrow()
{
    ;
}

void pilot_input_menu_right_arrow()
{
    ;
}

void pilot_input_menu_left_arrow()
{
    ;
}

void pilot_input_menu_return_key()
{
    ;
}

void pilot_input_menu_escape_key()
{
    ;
}

void pilot_input_menu_draw_menu()
{
    const long x_start = 10;
    const long y_start = 90;    
    char prompt[80] = "PILOT NAME:";
    char temp_str[80] = "";
    char cursor[80] = "_";
    overall_stats_type temp_stats;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    /* draw title */

    string_blit( "ENTER PILOT NAME", 70, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    strcpy( temp_str, prompt );
    strcat( temp_str, pilot_name );
    strcat( temp_str, cursor );

    string_blit( temp_str, x_start, y_start,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );


    Swap_Buffer();

#ifdef GLCYLINDRIX		
	 GL_SubloadAndSwapBuffer();
#endif

    if( SYS_KeyPressed( KEY_A ) ) {
        strcat( pilot_name, "A" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_B ) ) {
        strcat( pilot_name, "B" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_C ) ) {
        strcat( pilot_name, "C" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_D ) ) {
        strcat( pilot_name, "D" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_E ) ) {
        strcat( pilot_name, "E" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_F ) ) {
        strcat( pilot_name, "F" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_G ) ) {
        strcat( pilot_name, "G" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_H ) ) {
        strcat( pilot_name, "H" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_I ) ) {
        strcat( pilot_name, "I" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_J ) ) {
        strcat( pilot_name, "J" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_K ) ) {
        strcat( pilot_name, "K" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_L ) ) {
        strcat( pilot_name, "L" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_M ) ) {
        strcat( pilot_name, "M" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_N ) ) {
        strcat( pilot_name, "N" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_O ) ) {
        strcat( pilot_name, "O" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_P ) ) {
        strcat( pilot_name, "P" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_Q ) ) {
        strcat( pilot_name, "Q" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_R ) ) {
        strcat( pilot_name, "R" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_S ) ) {
        strcat( pilot_name, "S" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_T ) ) {
        strcat( pilot_name, "T" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_U ) ) {
        strcat( pilot_name, "U" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_V ) ) {
        strcat( pilot_name, "V" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_W ) ) {
        strcat( pilot_name, "W" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_X ) ) {
        strcat( pilot_name, "X" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_Y ) ) {
        strcat( pilot_name, "Y" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_Z ) ) {
        strcat( pilot_name, "Z" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_SPACE ) ) {
        strcat( pilot_name, " " );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_1 ) ) {
        strcat( pilot_name, "1" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_2 ) ) {
        strcat( pilot_name, "2" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_3 ) ) {
        strcat( pilot_name, "3" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_4 ) ) {
        strcat( pilot_name, "4" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_5 ) ) {
        strcat( pilot_name, "5" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_6 ) ) {
        strcat( pilot_name, "6" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_7 ) ) {
        strcat( pilot_name, "7" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_8 ) ) {
        strcat( pilot_name, "8" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_9 ) ) {
        strcat( pilot_name, "9" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_0 ) ) {
        strcat( pilot_name, "0" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_MINUS ) ) {
        strcat( pilot_name, "-" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_EQUALS ) ) {
        strcat( pilot_name, "+" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_LEFT_BRACKET ) ) {
        strcat( pilot_name, "[" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_RIGHT_BRACKET ) ) {
        strcat( pilot_name, "]" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_BACKSLASH ) ) {
        strcat( pilot_name, "/" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_APOSTROPHE ) ) {
        strcat( pilot_name, "'" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_SEMICOLON ) ) {
        strcat( pilot_name, ";" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_SLASH ) ) {
        strcat( pilot_name, "/" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_TILDE ) ) {
        strcat( pilot_name, "~" );
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_BACKSPACE ) ) {
        if( strlen( pilot_name ) > 0 ) {
            pilot_name[strlen( pilot_name ) - 1] = '\0';
        }
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }
    else if( SYS_KeyPressed( KEY_ESC ) ) {
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        make_menu_current( &pilot_selection_menu );
		strcpy( pilot_name, "" );
    }
    else if( SYS_KeyPressed( KEY_ENTER ) ) {

        Clear_Overall_Stats( &temp_stats );
        strcpy( temp_stats.name, pilot_name );

        if( strcmp( game_configuration.pilot_names.slot0, "" ) == 0 ) {
            strcpy( game_configuration.pilot_names.slot0, pilot_name );
            Save_Overall_Stats_Binary( &temp_stats, "gamedata/xxx0.xxx" );
        }
        else if( strcmp( game_configuration.pilot_names.slot1, "" ) == 0 ) {
            strcpy( game_configuration.pilot_names.slot1, pilot_name );
            Save_Overall_Stats_Binary( &temp_stats, "gamedata/xxx1.xxx" );
        }
        else if( strcmp( game_configuration.pilot_names.slot2, "" ) == 0 ) {
            strcpy( game_configuration.pilot_names.slot2, pilot_name );
            Save_Overall_Stats_Binary( &temp_stats, "gamedata/xxx2.xxx" );
        }
        else if( strcmp( game_configuration.pilot_names.slot3, "" ) == 0 ) {
            strcpy( game_configuration.pilot_names.slot3, pilot_name );
            Save_Overall_Stats_Binary( &temp_stats, "gamedata/xxx3.xxx" );
        }
        else if( strcmp( game_configuration.pilot_names.slot4, "" ) == 0 ) {
            strcpy( game_configuration.pilot_names.slot4, pilot_name );
            Save_Overall_Stats_Binary( &temp_stats, "gamedata/xxx4.xxx" );
        }
        else if( strcmp( game_configuration.pilot_names.slot5, "" ) == 0 ) {
            strcpy( game_configuration.pilot_names.slot5, pilot_name );
            Save_Overall_Stats_Binary( &temp_stats, "gamedata/xxx5.xxx" );
        }
        else if( strcmp( game_configuration.pilot_names.slot6, "" ) == 0 ) {
            strcpy( game_configuration.pilot_names.slot6, pilot_name );
            Save_Overall_Stats_Binary( &temp_stats, "gamedata/xxx6.xxx" );
        }
        else if( strcmp( game_configuration.pilot_names.slot7, "" ) == 0 ) {
            strcpy( game_configuration.pilot_names.slot7, pilot_name );
            Save_Overall_Stats_Binary( &temp_stats, "gamedata/xxx7.xxx" );
        }

        strcpy( pilot_selection_menu_data.item[0], game_configuration.pilot_names.slot0 );
        strcpy( pilot_selection_menu_data.item[1], game_configuration.pilot_names.slot1 );
        strcpy( pilot_selection_menu_data.item[2], game_configuration.pilot_names.slot2 );
        strcpy( pilot_selection_menu_data.item[3], game_configuration.pilot_names.slot3 );
        strcpy( pilot_selection_menu_data.item[4], game_configuration.pilot_names.slot4 );
        strcpy( pilot_selection_menu_data.item[5], game_configuration.pilot_names.slot5 );
        strcpy( pilot_selection_menu_data.item[6], game_configuration.pilot_names.slot6 );
        strcpy( pilot_selection_menu_data.item[7], game_configuration.pilot_names.slot7 );
        strcpy( pilot_selection_menu_data.item[8], "CREATE NEW PILOT" );
        strcpy( pilot_selection_menu_data.item[9], "DELETE PILOT" );

        strcpy( pilot_delete_menu_data.item[0], game_configuration.pilot_names.slot0 );
        strcpy( pilot_delete_menu_data.item[1], game_configuration.pilot_names.slot1 );
        strcpy( pilot_delete_menu_data.item[2], game_configuration.pilot_names.slot2 );
        strcpy( pilot_delete_menu_data.item[3], game_configuration.pilot_names.slot3 );
        strcpy( pilot_delete_menu_data.item[4], game_configuration.pilot_names.slot4 );
        strcpy( pilot_delete_menu_data.item[5], game_configuration.pilot_names.slot5 );
        strcpy( pilot_delete_menu_data.item[6], game_configuration.pilot_names.slot6 );
        strcpy( pilot_delete_menu_data.item[7], game_configuration.pilot_names.slot7 );

        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        make_menu_current( &pilot_selection_menu );
		strcpy( pilot_name, "" );
    }
    else {
        ;
    }

    if( strlen( pilot_name ) > 20 ) {
        pilot_name[strlen( pilot_name ) - 1] = '\0';
    }
}

void init_pilot_input_menu()
{
    pilot_input_menu.up_arrow = (Method)pilot_input_menu_up_arrow;
    pilot_input_menu.down_arrow = (Method)pilot_input_menu_down_arrow;
    pilot_input_menu.right_arrow = (Method)pilot_input_menu_right_arrow;
    pilot_input_menu.left_arrow = (Method)pilot_input_menu_left_arrow;
    pilot_input_menu.return_key = (Method)pilot_input_menu_return_key;
    pilot_input_menu.escape_key = (Method)pilot_input_menu_escape_key;
    pilot_input_menu.draw_menu = (Method)pilot_input_menu_draw_menu;

    pilot_input_menu_data.num_items = 1;

    pilot_input_menu_data.current_selection = 0;
}




/* methods for main_menu */




void main_menu_up_arrow()
{
    main_menu_data.current_selection = (main_menu_data.current_selection - 1) % main_menu_data.num_items;
    if( main_menu_data.current_selection < 0 ) {
        main_menu_data.current_selection = main_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void main_menu_down_arrow()
{
    main_menu_data.current_selection = (main_menu_data.current_selection + 1) % main_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void main_menu_right_arrow()
{
    ;
}

void main_menu_left_arrow()
{
    ;
}

void main_menu_return_key()
{

    if( main_menu_data.current_selection == 0 ) { /* start game */

        make_menu_current( &game_menu );
        
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

    }
    else if( main_menu_data.current_selection == 1 ) { /* options */

        make_menu_current( &options_menu );
        
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

    }
    else if( main_menu_data.current_selection == 2 ) { /* view stats */

        make_menu_current( &view_stats_menu );
        
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

    }
    else if( main_menu_data.current_selection == 3 ) { /* change pilot */

        make_menu_current( &pilot_selection_menu );
        
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

    }
    else if( main_menu_data.current_selection == 4 ) { /* play intro */
    
        if( multiplayer_game_only ) {
        
            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        
        }
        else {
			
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

			Play_Fli( "FLI/HOUSE.FLI" );
        
            Enable_Color_Palette( &menu_stuff.general_menu_background );
        
/*	// TODO: play some music
            if( game_configuration.music_on ) {
                Play_Song( MENU_SONG );
                Set_Cd_Volume( game_configuration.music_vol );
            }
*/
        }

    }
    else if( main_menu_data.current_selection == 5 ) { /* quit */

        program_over = TRUE;

        exit_event_loop = TRUE;
        
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

    }

}

void main_menu_escape_key()
{
    ;
}

void main_menu_draw_menu()
{
    long i;
    unsigned char color = SELECTED_TEXT_COLOR;
    const long x_start = 75;
    const long y_start = 55;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < main_menu_data.num_items; i++ ) {

        if( i == main_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( main_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }
    
    micro_string_blit( version_string, 1, 189,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}

void init_main_menu()
{
    main_menu.up_arrow = (Method)main_menu_up_arrow;
    main_menu.down_arrow = (Method)main_menu_down_arrow;
    main_menu.right_arrow = (Method)main_menu_right_arrow;
    main_menu.left_arrow = (Method)main_menu_left_arrow;
    main_menu.return_key = (Method)main_menu_return_key;
    main_menu.escape_key = (Method)main_menu_escape_key;
    main_menu.draw_menu = (Method)main_menu_draw_menu;

    strcpy( main_menu_data.item[0], "START GAME" );
    strcpy( main_menu_data.item[1], "OPTIONS" );
    strcpy( main_menu_data.item[2], "VIEW STATS" );
    strcpy( main_menu_data.item[3], "CHANGE PILOT" );
    strcpy( main_menu_data.item[4], "PLAY INTRO" );
    strcpy( main_menu_data.item[5], "QUIT" );

    main_menu_data.num_items = 6;

    main_menu_data.current_selection = 0;
}




/* methods for options_menu */




void options_menu_up_arrow()
{
    options_menu_data.current_selection = (options_menu_data.current_selection - 1) % options_menu_data.num_items;
    if( options_menu_data.current_selection < 0 ) {
        options_menu_data.current_selection = options_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void options_menu_down_arrow()
{
    options_menu_data.current_selection = (options_menu_data.current_selection + 1) % options_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void options_menu_right_arrow()
{
    ;
}

void options_menu_left_arrow()
{
    ;
}

void options_menu_return_key()
{

    if( options_menu_data.current_selection == 0 ) { /* sound */

        make_menu_current( &sound_menu );

    }
    else if( options_menu_data.current_selection == 1 ) { /* detail options */

        make_menu_current( &detail_menu );

    }
    else if( options_menu_data.current_selection == 2 ) { /* keyboard */

        make_menu_current( &keyboard_menu );

    }
    else if( options_menu_data.current_selection == 3 ) { /* joystick */

        make_menu_current( &joystick_menu );

    }
    else if( options_menu_data.current_selection == 4 ) { /* animations: */
    
        game_configuration.animations_on = !game_configuration.animations_on;
    
        if( game_configuration.animations_on ) {
            strcpy( options_menu_data.item[4], "ANIMATIONS: ON" );
        }
        else {
            strcpy( options_menu_data.item[4], "ANIMATIONS: OFF" );
        }
    }
    else if( options_menu_data.current_selection == 5 ) { /* fullscreen: */
    
		// AJT: TODO make this work... haha.
        game_configuration.fullscreen = !game_configuration.fullscreen;
    
        if( game_configuration.fullscreen ) {
            strcpy( options_menu_data.item[5], "FULLSCREEN" );
        }
        else {
            strcpy( options_menu_data.item[5], "WINDOWED" );
        }
	//update_video_mode();
	Enable_Color_Palette( &menu_stuff.general_menu_background );
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

}

void options_menu_escape_key()
{
    make_menu_current( &main_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );

}

void options_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 75;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < options_menu_data.num_items; i++ ) {

        if( i == options_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( options_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}

void init_options_menu()
{
    options_menu.up_arrow = (Method)options_menu_up_arrow;
    options_menu.down_arrow = (Method)options_menu_down_arrow;
    options_menu.right_arrow = (Method)options_menu_right_arrow;
    options_menu.left_arrow = (Method)options_menu_left_arrow;
    options_menu.return_key = (Method)options_menu_return_key;
    options_menu.escape_key = (Method)options_menu_escape_key;
    options_menu.draw_menu = (Method)options_menu_draw_menu;

    strcpy( options_menu_data.item[0], "SOUND" );
    strcpy( options_menu_data.item[1], "DETAIL OPTIONS" );
    strcpy( options_menu_data.item[2], "KEYBOARD" );
    strcpy( options_menu_data.item[3], "JOYSTICK" );
    if( game_configuration.animations_on ) {
        strcpy( options_menu_data.item[4], "ANIMATIONS: ON" );
    }
    else {
        strcpy( options_menu_data.item[4], "ANIMATIONS: OFF" );
    }
    if( game_configuration.fullscreen ) {
        strcpy( options_menu_data.item[5], "FULLSCREEN" );
    }
    else {
        strcpy( options_menu_data.item[5], "WINDOWED" );
    }

    options_menu_data.num_items = 6;

    options_menu_data.current_selection = 0;
}



/* methods for detail_menu */




void detail_menu_up_arrow()
{
    detail_menu_data.current_selection = (detail_menu_data.current_selection - 1) % detail_menu_data.num_items;
    if( detail_menu_data.current_selection < 0 ) {
        detail_menu_data.current_selection = detail_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void detail_menu_down_arrow()
{
    detail_menu_data.current_selection = (detail_menu_data.current_selection + 1) % detail_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void detail_menu_right_arrow()
{
    ;
}

void detail_menu_left_arrow()
{
    ;
}

void detail_menu_return_key()
{

    if( detail_menu_data.current_selection == 0 ) { /* tube detail: */

        if( game_configuration.tube_detail_level == HIGH_DETAIL ) {
            game_configuration.tube_detail_level = LOW_DETAIL;
            strcpy( detail_menu_data.item[0], "TUBE DETAIL: LOW" );
        }
        else if( game_configuration.tube_detail_level == MEDIUM_DETAIL ) {
            game_configuration.tube_detail_level = HIGH_DETAIL;
            strcpy( detail_menu_data.item[0], "TUBE DETAIL: HIGH" );
        }
        else if( game_configuration.tube_detail_level == LOW_DETAIL ) {
            game_configuration.tube_detail_level = MEDIUM_DETAIL;
            strcpy( detail_menu_data.item[0], "TUBE DETAIL: MEDIUM" );
        }
        else {
            game_configuration.tube_detail_level = LOW_DETAIL;
            strcpy( detail_menu_data.item[0], "TUBE DETAIL: LOW" );
        }

    }
    else if( detail_menu_data.current_selection == 1 ) { /* world detail: */

        if( game_configuration.world_detail_level == HIGH_DETAIL ) {
            game_configuration.world_detail_level = LOW_DETAIL;
            strcpy( detail_menu_data.item[1], "WORLD DETAIL: LOW" );
        }
        else if( game_configuration.world_detail_level == LOW_DETAIL ) {
            game_configuration.world_detail_level = HIGH_DETAIL;
            strcpy( detail_menu_data.item[1], "WORLD DETAIL: HIGH" );
        }
        else {
            game_configuration.world_detail_level = LOW_DETAIL;
            strcpy( detail_menu_data.item[1], "WORLD DETAIL: LOW" );
        }

    }
    else if( detail_menu_data.current_selection == 2 ) { /* hubs: */

        if( game_configuration.hubs_on == TRUE ) {
            game_configuration.hubs_on = FALSE;
            strcpy( detail_menu_data.item[2], "HUBS: OFF" );
        }
        else {
            game_configuration.hubs_on = TRUE;
            strcpy( detail_menu_data.item[2], "HUBS: ON" );
        }
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void detail_menu_escape_key()
{
    make_menu_current( &options_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void detail_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 75;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < detail_menu_data.num_items; i++ ) {

        if( i == detail_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( detail_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}

void init_detail_menu()
{
    detail_menu.up_arrow = (Method)detail_menu_up_arrow;
    detail_menu.down_arrow = (Method)detail_menu_down_arrow;
    detail_menu.right_arrow = (Method)detail_menu_right_arrow;
    detail_menu.left_arrow = (Method)detail_menu_left_arrow;
    detail_menu.return_key = (Method)detail_menu_return_key;
    detail_menu.escape_key = (Method)detail_menu_escape_key;
    detail_menu.draw_menu = (Method)detail_menu_draw_menu;

    if( game_configuration.tube_detail_level == HIGH_DETAIL ) {
        strcpy( detail_menu_data.item[0], "TUBE DETAIL: HIGH" );
    }
    else if( game_configuration.tube_detail_level == MEDIUM_DETAIL ) {
        strcpy( detail_menu_data.item[0], "TUBE DETAIL: MEDIUM" );
    }
    else if( game_configuration.tube_detail_level == LOW_DETAIL ) {
        strcpy( detail_menu_data.item[0], "TUBE DETAIL: LOW" );
    }
    else {
        strcpy( detail_menu_data.item[0], "TUBE DETAIL: ????" );
    }

    if( game_configuration.world_detail_level == HIGH_DETAIL ) {
        strcpy( detail_menu_data.item[1], "WORLD DETAIL: HIGH" );
    }
    else if( game_configuration.world_detail_level == LOW_DETAIL ) {
        strcpy( detail_menu_data.item[1], "WORLD DETAIL: LOW" );
    }
    else {
        strcpy( detail_menu_data.item[1], "WORLD DETAIL: ????" );
    }

    if( game_configuration.hubs_on == TRUE ) {
        strcpy( detail_menu_data.item[2], "HUBS: ON" );
    }
    else {
        strcpy( detail_menu_data.item[2], "HUBS: OFF" );
    }

    detail_menu_data.num_items = 3;

    detail_menu_data.current_selection = 0;
}




/* methods for sound_menu */




void sound_menu_up_arrow()
{
    sound_menu_data.current_selection = (sound_menu_data.current_selection - 1) % sound_menu_data.num_items;
    if( sound_menu_data.current_selection < 0 ) {
        sound_menu_data.current_selection = sound_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void sound_menu_down_arrow()
{
    sound_menu_data.current_selection = (sound_menu_data.current_selection + 1) % sound_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void sound_menu_right_arrow()
{
    if( sound_menu_data.current_selection == 0 ) { /* cd volume.... */

        if( game_configuration.music_vol >= 225 ) {
            game_configuration.music_vol = 255;
        }
        else {
            if( game_configuration.music_vol == 224 ) {
                game_configuration.music_vol += 31;
            }
            else {
                game_configuration.music_vol += 32;
            }
        }

        Set_Cd_Volume( game_configuration.music_vol );

    }
    else if( sound_menu_data.current_selection == 1 ) { /* sound fx volume.. */

        if( game_configuration.sound_vol >= 225 ) {
            game_configuration.sound_vol = 255;
        }
        else {
            if( game_configuration.sound_vol == 224 ) {
                game_configuration.sound_vol += 31;
            }
            else {
                game_configuration.sound_vol += 32;
            }
        }

    }
    else if( sound_menu_data.current_selection == 2 ) { /* voice volume... */

        if( game_configuration.voice_vol >= 225 ) {
            game_configuration.voice_vol = 255;
        }
        else {
            if( game_configuration.voice_vol == 224 ) {
                game_configuration.voice_vol += 31;
            }
            else {
                game_configuration.voice_vol += 32;
            }
        }

    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void sound_menu_left_arrow()
{
    if( sound_menu_data.current_selection == 0 ) { /* cd volume.... */
        if( game_configuration.music_vol <= 32 ) {
            game_configuration.music_vol = 0;
        }
        else {
            game_configuration.music_vol -= 32;
        }

        Set_Cd_Volume( game_configuration.music_vol );

    }
    else if( sound_menu_data.current_selection == 1 ) { /* sound fx volume.. */
        if( game_configuration.sound_vol <= 32 ) {
            game_configuration.sound_vol = 0;
        }
        else {
            game_configuration.sound_vol -= 32;
        }

    }
    else if( sound_menu_data.current_selection == 2 ) { /* voice volume... */
        if( game_configuration.voice_vol <= 32 ) {
            game_configuration.voice_vol = 0;
        }
        else {
            game_configuration.voice_vol -= 32;
        }

    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void sound_menu_return_key()
{
    if( sound_menu_data.current_selection == 0 ) { /* cd volume.... */
        ;
    }
    else if( sound_menu_data.current_selection == 1 ) { /* sound fx volume.. */
        ;
    }
    else if( sound_menu_data.current_selection == 2 ) { /* voice volume... */
        ;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
}

void sound_menu_escape_key()
{
    make_menu_current( &options_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void sound_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 75;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;
    BarInfo bar_info;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < sound_menu_data.num_items; i++ ) {

        if( i == sound_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( sound_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        bar_info.upper_left[X] = x_start + 150;
        bar_info.upper_left[Y] = y + 1;
        bar_info.lower_right[X] = x_start + 200;
        bar_info.lower_right[Y] = y + 10;
        bar_info.min_val = 0;
        bar_info.max_val = 255;

        if( i == 0 ) {
            bar_info.val = game_configuration.music_vol;
        }
        else if( i == 1 ) {
            bar_info.val = game_configuration.sound_vol;
        }
        else if( i == 2 ) {
            bar_info.val = game_configuration.voice_vol;
        }

        bar_info.bar_color = color;
        bar_info.outline_color = 1;
        bar_info.type = HorizontalLeftJustified;

        draw_bar( &bar_info );

        y += y_spacing;
    }

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}

void init_sound_menu()
{
    sound_menu.up_arrow = (Method)sound_menu_up_arrow;
    sound_menu.down_arrow = (Method)sound_menu_down_arrow;
    sound_menu.right_arrow = (Method)sound_menu_right_arrow;
    sound_menu.left_arrow = (Method)sound_menu_left_arrow;
    sound_menu.return_key = (Method)sound_menu_return_key;
    sound_menu.escape_key = (Method)sound_menu_escape_key;
    sound_menu.draw_menu = (Method)sound_menu_draw_menu;

    strcpy( sound_menu_data.item[0], "CD VOLUME :" );
    strcpy( sound_menu_data.item[1], "FX VOLUME :" );
    strcpy( sound_menu_data.item[2], "VOICE VOLUME :" );

    sound_menu_data.num_items = 3;

    sound_menu_data.current_selection = 0;
}




/* methods for keyboard_menu */




void keyboard_menu_up_arrow()
{
    keyboard_menu_data.current_selection = (keyboard_menu_data.current_selection - 1) % keyboard_menu_data.num_items;
    if( keyboard_menu_data.current_selection < 0 ) {
        keyboard_menu_data.current_selection = keyboard_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void keyboard_menu_down_arrow()
{
    keyboard_menu_data.current_selection = (keyboard_menu_data.current_selection + 1) % keyboard_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void keyboard_menu_right_arrow()
{
    ;
}

void keyboard_menu_left_arrow()
{
    ;
}

void keyboard_menu_return_key()
{
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

	/* change color 2 to red */
	Set_One( 1, SELECTED_TEXT_COLOR ); /* 1 is nonsense right now */

	keyboard_menu_waitforkey = 1;

} /* End of keyboard_menu_return_key */

void keyboard_menu_escape_key()
{
    make_menu_current( &options_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void keyboard_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 10;
    const long y_start = 30;
    const long y_spacing = 7;
    long x, y;
    unsigned long current_scancode = 0;
    char temp_str[80];
	int found_key = 0;
	int key = 0;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    /* menu title */

    string_blit( "CONFIGURE KEYBOARD", 65, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    x = x_start;
    y = y_start;

    for( i = 0; i < keyboard_menu_data.num_items; i++ ) {

        if( i == keyboard_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        micro_string_blit( keyboard_menu_data.item[i], x, y,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, color );

        if( i == 0 )
            current_scancode = game_configuration.keyboard_config.laser;
        else if( i == 1 )
            current_scancode = game_configuration.keyboard_config.missile;
        else if( i == 2 )
            current_scancode = game_configuration.keyboard_config.special;
        else if( i == 3 )
            current_scancode = game_configuration.keyboard_config.mode_switch;
        else if( i == 4 )
            current_scancode = game_configuration.keyboard_config.up;
        else if( i == 5 )
            current_scancode = game_configuration.keyboard_config.down;
        else if( i == 6 )
            current_scancode = game_configuration.keyboard_config.left;
        else if( i == 7 )
            current_scancode = game_configuration.keyboard_config.right;
        else if( i == 8 )
            current_scancode = game_configuration.keyboard_config.up_throttle;
        else if( i == 9 )
            current_scancode = game_configuration.keyboard_config.down_throttle;
        else if( i == 10 )
            current_scancode = game_configuration.keyboard_config.strafe;
        else if( i == 11 )
            current_scancode = game_configuration.keyboard_config.wing_1_pylon;
        else if( i == 12 )
            current_scancode = game_configuration.keyboard_config.wing_1_attack;
        else if( i == 13 )
            current_scancode = game_configuration.keyboard_config.wing_1_attack_rb;
        else if( i == 14 )
            current_scancode = game_configuration.keyboard_config.wing_1_defend_rb;
        else if( i == 15 )
            current_scancode = game_configuration.keyboard_config.wing_1_group;
        else if( i == 16 )
            current_scancode = game_configuration.keyboard_config.wing_1_cancel;
        else if( i == 17 )
            current_scancode = game_configuration.keyboard_config.wing_2_pylon;
        else if( i == 18 )
            current_scancode = game_configuration.keyboard_config.wing_2_attack;
        else if( i == 19 )
            current_scancode = game_configuration.keyboard_config.wing_2_attack_rb;
        else if( i == 20 )
            current_scancode = game_configuration.keyboard_config.wing_2_defend_rb;
        else if( i == 21 )
            current_scancode = game_configuration.keyboard_config.wing_2_group;
        else if( i == 22 )
            current_scancode = game_configuration.keyboard_config.wing_2_cancel;


        /* sprintf( temp_str, "%ld", current_scancode ); */

        Index_To_String( temp_str, current_scancode );

        micro_string_blit( temp_str, x + 200, y,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, color );


        y += y_spacing;
    }

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

	if ( keyboard_menu_waitforkey )
	{
		current_menu.delay_input = 1;

		for ( i = 0; i < SIZEOFKEYTABLE; ++i )
		{
			if ( SYS_KeyPressed( (unsigned char)i ) )
			{
				found_key = 1;
				key = i;
				break;
			}
		}

		if ( found_key )
		{
			/* change color 2 back to green */

			Enable_Palette_Member( &menu_stuff.general_menu_background, SELECTED_TEXT_COLOR );

			if( keyboard_menu_data.current_selection == 0 ) { /* fire laser: */
				game_configuration.keyboard_config.laser = key;
			}
			else if( keyboard_menu_data.current_selection == 1 ) { /* fire missile: */
				game_configuration.keyboard_config.missile = key;
			}
			else if( keyboard_menu_data.current_selection == 2 ) { /* special weapon: */
				game_configuration.keyboard_config.special = key;
			}
			else if( keyboard_menu_data.current_selection == 3 ) { /* mode switch: */
				game_configuration.keyboard_config.mode_switch = key;
			}
			else if( keyboard_menu_data.current_selection == 4 ) { /* up: */
				game_configuration.keyboard_config.up = key;
			}
			else if( keyboard_menu_data.current_selection == 5 ) { /* down: */
				game_configuration.keyboard_config.down = key;
			}
			else if( keyboard_menu_data.current_selection == 6 ) { /* left: */
				game_configuration.keyboard_config.left = key;
			}
			else if( keyboard_menu_data.current_selection == 7 ) { /* right: */
				game_configuration.keyboard_config.right = key;
			}
			else if( keyboard_menu_data.current_selection == 8 ) { /* up throttle: */
				game_configuration.keyboard_config.up_throttle = key;
			}
			else if( keyboard_menu_data.current_selection == 9 ) { /* down throttle: */
				game_configuration.keyboard_config.down_throttle = key;
			}
			else if( keyboard_menu_data.current_selection == 10 ) { /* strafe: */
				game_configuration.keyboard_config.strafe = key;
			}
			else if( keyboard_menu_data.current_selection == 11 ) { /* wing 1 get pylons: */
				game_configuration.keyboard_config.wing_1_pylon = key;
			}
			else if( keyboard_menu_data.current_selection == 12 ) { /* wing 1 attack: */
				game_configuration.keyboard_config.wing_1_attack = key;
			}
			else if( keyboard_menu_data.current_selection == 13 ) { /* wing 1 attack radar base: */
				game_configuration.keyboard_config.wing_1_attack_rb = key;
			}
			else if( keyboard_menu_data.current_selection == 14 ) { /* wing 1 defend radar base: */
				game_configuration.keyboard_config.wing_1_defend_rb = key;
			}
			else if( keyboard_menu_data.current_selection == 15 ) { /* wing 1 group together: */
				game_configuration.keyboard_config.wing_1_group = key;
			}
			else if( keyboard_menu_data.current_selection == 16 ) { /* wing 1 cancel orders: */
				game_configuration.keyboard_config.wing_1_cancel = key;
			}
			else if( keyboard_menu_data.current_selection == 17 ) { /* wing 2 get pylons: */
				game_configuration.keyboard_config.wing_2_pylon = key;
			}
			else if( keyboard_menu_data.current_selection == 18 ) { /* wing 2 attack: */
				game_configuration.keyboard_config.wing_2_attack = key;
			}
			else if( keyboard_menu_data.current_selection == 19 ) { /* wing 2 attack radar base: */
				game_configuration.keyboard_config.wing_2_attack_rb = key;
			}
			else if( keyboard_menu_data.current_selection == 20 ) { /* wing 2 defend radar base: */
				game_configuration.keyboard_config.wing_2_defend_rb = key;
			}
			else if( keyboard_menu_data.current_selection == 21 ) { /* wing 2 group together: */
				game_configuration.keyboard_config.wing_2_group = key;
			}
			else if( keyboard_menu_data.current_selection == 22 ) { /* wing 2 cancel orders: */
				game_configuration.keyboard_config.wing_2_cancel = key;
			}

			Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

			keyboard_menu_waitforkey = 0;
		}
	 }
}

void init_keyboard_menu()
{
    keyboard_menu.up_arrow = (Method)keyboard_menu_up_arrow;
    keyboard_menu.down_arrow = (Method)keyboard_menu_down_arrow;
    keyboard_menu.right_arrow = (Method)keyboard_menu_right_arrow;
    keyboard_menu.left_arrow = (Method)keyboard_menu_left_arrow;
    keyboard_menu.return_key = (Method)keyboard_menu_return_key;
    keyboard_menu.escape_key = (Method)keyboard_menu_escape_key;
    keyboard_menu.draw_menu = (Method)keyboard_menu_draw_menu;

    strcpy( keyboard_menu_data.item[0], "FIRE LASER:" );
    strcpy( keyboard_menu_data.item[1], "FIRE MISSILE:" );
    strcpy( keyboard_menu_data.item[2], "SPECIAL WEAPON:" );
    strcpy( keyboard_menu_data.item[3], "MODE SWITCH:" );
    strcpy( keyboard_menu_data.item[4], "UP:" );
    strcpy( keyboard_menu_data.item[5], "DOWN:" );
    strcpy( keyboard_menu_data.item[6], "LEFT:" );
    strcpy( keyboard_menu_data.item[7], "RIGHT:" );
    strcpy( keyboard_menu_data.item[8], "UP THROTTLE:" );
    strcpy( keyboard_menu_data.item[9], "DOWN THROTTLE:" );
    strcpy( keyboard_menu_data.item[10], "STRAFE:" );

    strcpy( keyboard_menu_data.item[11], "WING 1 GET PYLONS:" );
    strcpy( keyboard_menu_data.item[12], "WING 1 ATTACK:" );
    strcpy( keyboard_menu_data.item[13], "WING 1 ATTACK RADAR BASE:" );
    strcpy( keyboard_menu_data.item[14], "WING 1 DEFEND RADAR BASE:" );
    strcpy( keyboard_menu_data.item[15], "WING 1 GROUP TOGETHER:" );
    strcpy( keyboard_menu_data.item[16], "WING 1 CANCEL ORDERS:" );

    strcpy( keyboard_menu_data.item[17], "WING 2 GET PYLONS:" );
    strcpy( keyboard_menu_data.item[18], "WING 2 ATTACK:" );
    strcpy( keyboard_menu_data.item[19], "WING 2 ATTACK RADAR BASE:" );
    strcpy( keyboard_menu_data.item[20], "WING 2 DEFEND RADAR BASE:" );
    strcpy( keyboard_menu_data.item[21], "WING 2 GROUP TOGETHER:" );
    strcpy( keyboard_menu_data.item[22], "WING 2 CANCEL ORDERS:" );

    keyboard_menu_data.num_items = 23;

    keyboard_menu_data.current_selection = 0;
}




/* methods for joystick_menu */




void joystick_menu_up_arrow()
{
    joystick_menu_data.current_selection = (joystick_menu_data.current_selection - 1) % joystick_menu_data.num_items;
    if( joystick_menu_data.current_selection < 0 ) {
        joystick_menu_data.current_selection = joystick_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void joystick_menu_down_arrow()
{
    joystick_menu_data.current_selection = (joystick_menu_data.current_selection + 1) % joystick_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void joystick_menu_right_arrow()
{
    ;
}

void joystick_menu_left_arrow()
{
    ;
}

void joystick_menu_return_key()
{

    if( joystick_menu_data.current_selection == 0 ) { /* joystick :*/

        game_configuration.joystick = (JoystickType)((game_configuration.joystick + 1) % 3);

        if( game_configuration.joystick == NoJoystick ) {
            strcpy( joystick_menu_data.item[0], "JOYSTICK : NONE" );
        }
        else if( game_configuration.joystick == StandardJoystick ) {
            strcpy( joystick_menu_data.item[0], "JOYSTICK : STANDARD" );
        }
        else if( game_configuration.joystick == FlightStickPro ) {
            strcpy( joystick_menu_data.item[0], "JOYSTICK : CH FLIGHT PRO" );
        }
    }
    else if( joystick_menu_data.current_selection == 1 ) { /* calibrate */

        make_menu_current( &calibrate_menu );

    }
    else if( joystick_menu_data.current_selection == 2 ) { /* configure */

        make_menu_current( &joystick_config_menu );

    }

    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void joystick_menu_escape_key()
{
    make_menu_current( &options_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void joystick_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 55;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < joystick_menu_data.num_items; i++ ) {

        if( i == joystick_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( joystick_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

}

void init_joystick_menu()
{
    joystick_menu.up_arrow = (Method)joystick_menu_up_arrow;
    joystick_menu.down_arrow = (Method)joystick_menu_down_arrow;
    joystick_menu.right_arrow = (Method)joystick_menu_right_arrow;
    joystick_menu.left_arrow = (Method)joystick_menu_left_arrow;
    joystick_menu.return_key = (Method)joystick_menu_return_key;
    joystick_menu.escape_key = (Method)joystick_menu_escape_key;
    joystick_menu.draw_menu = (Method)joystick_menu_draw_menu;

    if( game_configuration.joystick == NoJoystick ) {
        strcpy( joystick_menu_data.item[0], "JOYSTICK : NONE" );
    }
    else if( game_configuration.joystick == StandardJoystick ) {
        strcpy( joystick_menu_data.item[0], "JOYSTICK : STANDARD" );
    }
    else if( game_configuration.joystick == FlightStickPro ) {
        strcpy( joystick_menu_data.item[0], "JOYSTICK : CH FLIGHT PRO" );
    }

    strcpy( joystick_menu_data.item[1], "CALIBRATE" );
    strcpy( joystick_menu_data.item[2], "CONFIGURE" );

    joystick_menu_data.num_items = 3;

    joystick_menu_data.current_selection = 0;
}




/* methods for calibrate_menu */




void calibrate_menu_up_arrow()
{
    ;
}

void calibrate_menu_down_arrow()
{
    ;
}

void calibrate_menu_right_arrow()
{
    ;
}

void calibrate_menu_left_arrow()
{
    ;
}

void calibrate_menu_return_key()
{
    ;
}

void calibrate_menu_escape_key()
{
    ;
}

void calibrate_menu_draw_menu()
{

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    if( game_configuration.joystick == NoJoystick ) {

        /* no joystick selected */

        string_blit( "PLEASE SELECT JOYSTICK TYPE", 15, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

        Swap_Buffer();

#ifdef GLCYLINDRIX
	 GL_SubloadAndSwapBuffer();
#endif

    }
    else if( game_configuration.joystick == StandardJoystick ) {

        if( !Detect_Joystick() ) {

            /* no joystick detected */

            string_blit( "NO JOYSTICK DETECTED", 50, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			 GL_SubloadAndSwapBuffer();
#endif

        }
        else {

            /* calibrate joystick */

            string_blit( "MOVE JOYSTICK TO UPPER LEFT", 20, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			 GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_upper_left();

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE JOYSTICK TO LOWER RIGHT", 20, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			 GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_lower_right();

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE JOYSTICK TO CENTER", 30, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			 GL_SubloadAndSwapBuffer();
#endif


            calibrate_joystick_center();

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }
    }
    else if( game_configuration.joystick == FlightStickPro ) {

        if( !Detect_Flightstick() ) {

            /* no flightstick detected */

            string_blit( "NO FLIGHTSTICK DETECTED", 15, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			 GL_SubloadAndSwapBuffer();
#endif

        }
        else {

            /* calibrate flightstick */

            string_blit( "MOVE FLIGHTSTICK TO UPPER LEFT", 5, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_upper_left();

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE FLIGHTSTCK TO LOWER RIGHT", 5, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_lower_right();

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE FLIGHTSTICK TO CENTER", 20, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_center();

            Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE THROTTLE FROM MAX TO", 25, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "MIN THEN PRESS FIRE", 55, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
			GL_SubloadAndSwapBuffer();
#endif

            calibrate_flightstick_throttle();

            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }
    }

    make_menu_current( &joystick_menu );

}

void init_calibrate_menu()
{
    calibrate_menu.up_arrow = (Method)calibrate_menu_up_arrow;
    calibrate_menu.down_arrow = (Method)calibrate_menu_down_arrow;
    calibrate_menu.right_arrow = (Method)calibrate_menu_right_arrow;
    calibrate_menu.left_arrow = (Method)calibrate_menu_left_arrow;
    calibrate_menu.return_key = (Method)calibrate_menu_return_key;
    calibrate_menu.escape_key = (Method)calibrate_menu_escape_key;
    calibrate_menu.draw_menu = (Method)calibrate_menu_draw_menu;
}



/* methods for joystick_config_menu */




void joystick_config_menu_up_arrow()
{
    joystick_config_menu_data.current_selection = (joystick_config_menu_data.current_selection - 1) % joystick_config_menu_data.num_items;
    if( joystick_config_menu_data.current_selection < 0 ) {
        joystick_config_menu_data.current_selection = joystick_config_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void joystick_config_menu_down_arrow()
{
    joystick_config_menu_data.current_selection = (joystick_config_menu_data.current_selection + 1) % joystick_config_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void joystick_config_menu_right_arrow()
{
    ;
}

void joystick_config_menu_left_arrow()
{
    ;
}

void joystick_config_menu_return_key()
{

    if( joystick_config_menu_data.current_selection == 0 ) { /* button_1 */
        game_configuration.joystick_config.button_1 = (JoystickButtonAction)((game_configuration.joystick_config.button_1 + 1) % 12);
    }
    else if( joystick_config_menu_data.current_selection == 1 ) { /* button_2 */
        game_configuration.joystick_config.button_2 = (JoystickButtonAction)((game_configuration.joystick_config.button_2 + 1) % 12);
    }
    else if( joystick_config_menu_data.current_selection == 2 ) { /* button_3 */
        game_configuration.joystick_config.button_3 = (JoystickButtonAction)((game_configuration.joystick_config.button_3 + 1) % 12);
    }
    else if( joystick_config_menu_data.current_selection == 3 ) { /* button_4 */
        game_configuration.joystick_config.button_4 = (JoystickButtonAction)((game_configuration.joystick_config.button_4 + 1) % 12);
    }
    else if( joystick_config_menu_data.current_selection == 4 ) { /* hat_up */
        game_configuration.joystick_config.hat_up = (JoystickButtonAction)((game_configuration.joystick_config.hat_up + 1) % 12);
    }
    else if( joystick_config_menu_data.current_selection == 5 ) { /* hat_down */
        game_configuration.joystick_config.hat_down = (JoystickButtonAction)((game_configuration.joystick_config.hat_down + 1) % 12);
    }
    else if( joystick_config_menu_data.current_selection == 6 ) { /* hat_left */
        game_configuration.joystick_config.hat_left = (JoystickButtonAction)((game_configuration.joystick_config.hat_left + 1) % 12);
    }
    else if( joystick_config_menu_data.current_selection == 7 ) { /* hat_right */
        game_configuration.joystick_config.hat_right = (JoystickButtonAction)((game_configuration.joystick_config.hat_right + 1) % 12);
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void joystick_config_menu_escape_key()
{
    make_menu_current( &joystick_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void joystick_config_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 50;
    const long y_start = 50;
    const long y_spacing = 10;
    long x, y;
    unsigned long current_action = 0;
    char temp_str[80];

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    /* menu title */

    string_blit( "CONFIGURE JOYSTICK", 65, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    x = x_start;
    y = y_start;

    for( i = 0; i < joystick_config_menu_data.num_items; i++ ) {

        if( i == joystick_config_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        micro_string_blit( joystick_config_menu_data.item[i], x, y,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, color );

        if( i == 0 )
            current_action = game_configuration.joystick_config.button_1;
        else if( i == 1 )
            current_action = game_configuration.joystick_config.button_2;
        else if( i == 2 )
            current_action = game_configuration.joystick_config.button_3;
        else if( i == 3 )
            current_action = game_configuration.joystick_config.button_4;
        else if( i == 4 )
            current_action = game_configuration.joystick_config.hat_up;
        else if( i == 5 )
            current_action = game_configuration.joystick_config.hat_down;
        else if( i == 6 )
            current_action = game_configuration.joystick_config.hat_left;
        else if( i == 7 )
            current_action = game_configuration.joystick_config.hat_right;

        if( current_action == ShootLaser )
            strcpy( temp_str, "FIRE LASER" );
        else if( current_action == ShootMissile )
            strcpy( temp_str, "FIRE MISSILE" );
        else if( current_action == ActivateSpecial )
            strcpy( temp_str, "SPECIAL WEAPON" );
        else if( current_action == ActivateStrafe )
            strcpy( temp_str, "STRAFE ON" );
        else if( current_action == ThrottleUp )
            strcpy( temp_str, "THROTTLE UP" );
        else if( current_action == ThrottleDown )
            strcpy( temp_str, "THROTTLE DOWN" );
        else if( current_action == SwitchModes )
            strcpy( temp_str, "SWITCH MODES" );
        else if( current_action == DoNothing )
            strcpy( temp_str, "NOTHING" );
        else if( current_action == StrafeUp )
            strcpy( temp_str, "STRAFE UP" );
        else if( current_action == StrafeDown )
            strcpy( temp_str, "STRAFE DOWN" );
        else if( current_action == StrafeLeft )
            strcpy( temp_str, "STRAFE LEFT" );
        else if( current_action == StrafeRight )
            strcpy( temp_str, "STRAFE RIGHT" );

        micro_string_blit( temp_str, x + 100, y,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();

#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_joystick_config_menu()
{
    joystick_config_menu.up_arrow = (Method)joystick_config_menu_up_arrow;
    joystick_config_menu.down_arrow = (Method)joystick_config_menu_down_arrow;
    joystick_config_menu.right_arrow = (Method)joystick_config_menu_right_arrow;
    joystick_config_menu.left_arrow = (Method)joystick_config_menu_left_arrow;
    joystick_config_menu.return_key = (Method)joystick_config_menu_return_key;
    joystick_config_menu.escape_key = (Method)joystick_config_menu_escape_key;
    joystick_config_menu.draw_menu = (Method)joystick_config_menu_draw_menu;

    strcpy( joystick_config_menu_data.item[0], "BUTTON 1:" );
    strcpy( joystick_config_menu_data.item[1], "BUTTON 2:" );
    strcpy( joystick_config_menu_data.item[2], "BUTTON 3:" );
    strcpy( joystick_config_menu_data.item[3], "BUTTON 4:" );
    strcpy( joystick_config_menu_data.item[4], "HAT UP:" );
    strcpy( joystick_config_menu_data.item[5], "HAT DOWN:" );
    strcpy( joystick_config_menu_data.item[6], "HAT LEFT:" );
    strcpy( joystick_config_menu_data.item[7], "HAT RIGHT:" );

    joystick_config_menu_data.num_items = 8;

    joystick_config_menu_data.current_selection = 0;
}





/* methods for game_menu */




void game_menu_up_arrow()
{
    game_menu_data.current_selection = (game_menu_data.current_selection - 1) % game_menu_data.num_items;
    if( game_menu_data.current_selection < 0 ) {
        game_menu_data.current_selection = game_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void game_menu_down_arrow()
{
    game_menu_data.current_selection = (game_menu_data.current_selection + 1) % game_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void game_menu_right_arrow()
{
    ;
}

void game_menu_left_arrow()
{
    ;
}

void game_menu_return_key()
{

    if( game_menu_data.current_selection == 0 ) { /* tournament game */

        if( multiplayer_game_only ) {
        
            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
            
        }
        else {
            make_menu_current( &tournament_game_setup_menu );

            game_configuration.game_type = TournamentGame;
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }

    }
    else if( game_menu_data.current_selection == 1 ) { /* custom game */
    
        if( multiplayer_game_only ) {
            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {

            make_menu_current( &custom_game_menu );

            game_configuration.game_type = CustomGame;
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

        }
    }

}

void game_menu_escape_key()
{
    make_menu_current( &main_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void game_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 75;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < game_menu_data.num_items; i++ ) {

        if( i == game_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( game_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();

#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_game_menu()
{
    game_menu.up_arrow = (Method)game_menu_up_arrow;
    game_menu.down_arrow = (Method)game_menu_down_arrow;
    game_menu.right_arrow = (Method)game_menu_right_arrow;
    game_menu.left_arrow = (Method)game_menu_left_arrow;
    game_menu.return_key = (Method)game_menu_return_key;
    game_menu.escape_key = (Method)game_menu_escape_key;
    game_menu.draw_menu = (Method)game_menu_draw_menu;

    strcpy( game_menu_data.item[0], "TOURNAMENT GAME" );
    strcpy( game_menu_data.item[1], "CUSTOM GAME" );

    game_menu_data.num_items = 2;

    game_menu_data.current_selection = 0;
}


/* custom_game_menu */



void custom_game_menu_up_arrow()
{
    custom_game_menu_data.current_selection = (custom_game_menu_data.current_selection - 1) % custom_game_menu_data.num_items;
    if( custom_game_menu_data.current_selection < 0 ) {
        custom_game_menu_data.current_selection = custom_game_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void custom_game_menu_down_arrow()
{
    custom_game_menu_data.current_selection = (custom_game_menu_data.current_selection + 1) % custom_game_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void custom_game_menu_right_arrow()
{
    ;
}

void custom_game_menu_left_arrow()
{
    ;
}

void custom_game_menu_return_key()
{
    if( custom_game_menu_data.current_selection == 0 ) { /* start custom game */

        exit_event_loop = TRUE;
        game_configuration.game_type = CustomGame;
    }
    else if( custom_game_menu_data.current_selection == 1 ) { /* select your vehicle */

        init_custom_vehicle0_menu();
        make_menu_current( &custom_vehicle0_menu );
        DB_Clear_Screen();        
        Swap_Buffer();
#ifdef GLCYLINDRIX
		GL_SubloadAndSwapBuffer();
#endif
        Enable_Color_Palette( &menu_stuff.vehicle_menu_background );
    }
    else if( custom_game_menu_data.current_selection == 2 ) { /* select wingmen */

        init_custom_wingman1_menu();
        make_menu_current( &custom_wingman1_menu );
    }
    else if( custom_game_menu_data.current_selection == 3 ) { /* select wingmen vehicles */

        init_custom_vehicle1_menu();
        make_menu_current( &custom_vehicle1_menu );
        DB_Clear_Screen();        
        Swap_Buffer();
#ifdef GLCYLINDRIX
		GL_SubloadAndSwapBuffer();
#endif
        Enable_Color_Palette( &menu_stuff.vehicle_menu_background );
    }
    else if( custom_game_menu_data.current_selection == 4 ) { /* select opponent */

        init_custom_opponent1_menu();
        make_menu_current( &custom_opponent1_menu );
    }
    else if( custom_game_menu_data.current_selection == 5 ) { /* select opponents vehicles */

        init_custom_vehicle3_menu();
        make_menu_current( &custom_vehicle3_menu );
        DB_Clear_Screen();        
        Swap_Buffer();
#ifdef GLCYLINDRIX
		GL_SubloadAndSwapBuffer();
#endif
        Enable_Color_Palette( &menu_stuff.vehicle_menu_background );
    }
    else if( custom_game_menu_data.current_selection == 6 ) { /* select cylindrix */

        make_menu_current( &cylindrix_menu );

    }
    else if( custom_game_menu_data.current_selection == 7 ) { /* number of players */
	game_configuration.two_player = !game_configuration.two_player;
	if( game_configuration.two_player )
		strcpy( custom_game_menu_data.item[7], "TWO PLAYERS" );
	else
		strcpy( custom_game_menu_data.item[7], "ONE PLAYER" );

    }

    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void custom_game_menu_escape_key()
{
    make_menu_current( &game_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_game_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 25;
    const long y_start = 45;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < custom_game_menu_data.num_items; i++ ) {

        if( i == custom_game_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( custom_game_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }    

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_custom_game_menu()
{
	char szTemp[255];

	custom_game_menu.up_arrow = (Method)custom_game_menu_up_arrow;
	custom_game_menu.down_arrow = (Method)custom_game_menu_down_arrow;
	custom_game_menu.right_arrow = (Method)custom_game_menu_right_arrow;
	custom_game_menu.left_arrow = (Method)custom_game_menu_left_arrow;
	custom_game_menu.return_key = (Method)custom_game_menu_return_key;
	custom_game_menu.escape_key = (Method)custom_game_menu_escape_key;
	custom_game_menu.draw_menu = (Method)custom_game_menu_draw_menu;

	strcpy( custom_game_menu_data.item[0], "START CUSTOM GAME" );
	strcpy( custom_game_menu_data.item[1], "SELECT YOUR VEHICLE" );
	strcpy( custom_game_menu_data.item[2], "SELECT WINGMEN" );
	strcpy( custom_game_menu_data.item[3], "SELECT WINGMEN'S VEHICLES" );
	strcpy( custom_game_menu_data.item[4], "SELECT OPPONENT" );
	strcpy( custom_game_menu_data.item[5], "SELECT OPPONENT'S VEHICLES" );
	strcpy( custom_game_menu_data.item[6], "SELECT CYLINDRIX" );

	if( game_configuration.two_player )
		sprintf(szTemp, "TWO PLAYERS");
	else
		sprintf(szTemp, "ONE PLAYERS");
	strcpy( custom_game_menu_data.item[7], szTemp );

	custom_game_menu_data.num_items = 8;

	custom_game_menu_data.current_selection = 0;
}




/* methods for wingman_menu */


long selection_to_ai( long selection )
{
    if( selection == 0 ) {  /* Megan Hunter */
        return 9;
    }
    else if( selection == 1 ) { /* Kristen Masters */
        return 6;
    }
    else if( selection == 2 ) { /* Cindy Vicious */
        return 3;
    }
    else if( selection == 3 ) { /* Mr. Kent */
        return 5;
    }
    else if( selection == 4 ) { /* Major Flowers */
        return 1;
    }
    else if( selection == 5 ) { /* Preacher */
        return 4;
    }
    else if( selection == 6 ) { /* Kelly Sanders */
        return 8;
    }
    else if( selection == 7 ) { /* Shana Blake */
        return 7;
    }
    else if( selection == 8 ) { /* Richard Stryker */
        return 2;
    }
    else if( selection == 9 ) { /* Blank */
        return 0;
    }
    else if( selection == 10 ) { /* Capn Hardgeus */
        return 0;
    }
    else if( selection == 11 ) { /* Blank */
        return 0;
    }
    else if( selection == 12 ) { /* Mephisto */
        return 23;
    }
    else if( selection == 13 ) { /* Malfator */
        return 24;
    }
    else if( selection == 14 ) { /* Baal */
        return 22;
    }
    else if( selection == 15 ) { /* Anubis */
        return 25;
    }
    else if( selection == 16 ) { /* Horus */
        return 27;
    }
    else if( selection == 17 ) { /* Bast */
        return 26;
    }
    else if( selection == 18 ) { /* Greko */
        return 18;
    }
    else if( selection == 19 ) { /* Crusty */
        return 17;
    }
    else if( selection == 20 ) { /* Hotwire */
        return 16;
    }
    else if( selection == 21 ) { /* Gall */
        return 14;
    }
    else if( selection == 22 ) { /* Pubert */
        return 13;
    }
    else if( selection == 23 ) { /* Septic */
        return 15;
    }
    else if( selection == 24 ) { /* Crystal */
        return 19;
    }
    else if( selection == 25 ) { /* Glacia */
        return 20;
    }
    else if( selection == 26 ) { /* Azuria */
        return 21;
    }
    else if( selection == 27 ) { /* Nebula */
        return 28;
    }
    else if( selection == 28 ) { /* Polaris */
        return 29;
    }
    else if( selection == 29 ) { /* Quasar */
        return 30;
    }
    else if( selection == 30 ) { /* Bok */
        return 10;
    }
    else if( selection == 31 ) { /* N'Bok */
        return 11;
    }
    else if( selection == 32 ) { /* T'Bok */
        return 12;
    }
    else if( selection == 33 ) { /* B00B */
        return 34;
    }
    else if( selection == 34 ) { /* D23B6M */
        return 35;
    }
    else if( selection == 35 ) { /* JLDV8 */
        return 36;
    }
    else if( selection == 36 ) { /* G */
        return 32;
    }
    else if( selection == 37 ) { /* B */
        return 33;
    }
    else if( selection == 38 ) { /* R */
        return 31;
    }
    else {
        return 0;
    }
}

long ai_to_selection( long ai )
{
    if( ai == 9 ) {  /* Megan Hunter */
        return 0;
    }
    else if( ai == 6 ) { /* Kristen Masters */
        return 1;
    }
    else if( ai == 3 ) { /* Cindy Vicious */
        return 2;
    }
    else if( ai == 5 ) { /* Mr. Kent */
        return 3;
    }
    else if( ai == 1 ) { /* Major Flowers */
        return 4;
    }
    else if( ai == 4 ) { /* Preacher */
        return 5;
    }
    else if( ai == 8 ) { /* Kelly Sanders */
        return 6;
    }
    else if( ai == 7 ) { /* Shana Blake */
        return 7;
    }
    else if( ai == 2 ) { /* Richard Stryker */
        return 8;
    }
    else if( ai == 0 ) { /* Capn Hardgeus */
        return 10;
    }
    else if( ai == 23 ) { /* Mephisto */
        return 12;
    }
    else if( ai == 24 ) { /* Malfator */
        return 13;
    }
    else if( ai == 22 ) { /* Baal */
        return 14;
    }
    else if( ai == 25 ) { /* Anubis */
        return 15;
    }
    else if( ai == 27 ) { /* Horus */
        return 16;
    }
    else if( ai == 26 ) { /* Bast */
        return 17;
    }
    else if( ai == 18 ) { /* Greko */
        return 18;
    }
    else if( ai == 17 ) { /* Crusty */
        return 19;
    }
    else if( ai == 16 ) { /* Hotwire */
        return 20;
    }
    else if( ai == 14 ) { /* Gall */
        return 21;
    }
    else if( ai == 13 ) { /* Pubert */
        return 22;
    }
    else if( ai == 15 ) { /* Septic */
        return 23;
    }
    else if( ai == 19 ) { /* Crystal */
        return 24;
    }
    else if( ai == 20 ) { /* Glacia */
        return 25;
    }
    else if( ai == 21 ) { /* Azuria */
        return 26;
    }
    else if( ai == 28 ) { /* Nebula */
        return 27;
    }
    else if( ai == 29 ) { /* Polaris */
        return 28;
    }
    else if( ai == 30 ) { /* Quasar */
        return 29;
    }
    else if( ai == 10 ) { /* Bok */
        return 30;
    }
    else if( ai == 11 ) { /* N'Bok */
        return 31;
    }
    else if( ai == 12 ) { /* T'Bok */
        return 32;
    }
    else if( ai == 34 ) { /* B00B */
        return 33;
    }
    else if( ai == 35 ) { /* D23B6M */
        return 34;
    }
    else if( ai == 36 ) { /* JLDV8 */
        return 35;
    }
    else if( ai == 32 ) { /* G */
        return 36;
    }
    else if( ai == 33 ) { /* B */
        return 37;
    }
    else if( ai == 31 ) { /* R */
        return 38;
    }
    else {
        return 0;
    }
}


void wingman_menu_up_arrow()
{
    wingman_menu_data.current_row--;

    if( wingman_menu_data.current_row < 0 ) {
        wingman_menu_data.current_row = 0;
        Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
    }
    else {
        wingman_menu_data.palette_active = FALSE;
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }

    /* special case for Cap'n Hardgeus */

    if( wingman_menu_data.current_row == 3 ) {
        wingman_menu_data.current_col = 1;
    }
}

void wingman_menu_down_arrow()
{
    wingman_menu_data.current_row++;

    if( wingman_menu_data.current_row >= (wingman_menu_data.num_items / 3) ) {
        wingman_menu_data.current_row = (wingman_menu_data.num_items / 3) - 1;
        Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
    }
    else {
        wingman_menu_data.palette_active = FALSE;
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }

    /* special case for Cap'n Hardgeus */

    if( wingman_menu_data.current_row == 3 ) {
        wingman_menu_data.current_col = 1;
    }
}

void wingman_menu_right_arrow()
{
    wingman_menu_data.current_col++;

    if( wingman_menu_data.current_col > 2 ) {
        wingman_menu_data.current_col = 2;
        Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
    }
    else {

        /* special case for Cap'n Hardgeus */

        if( wingman_menu_data.current_row == 3 ) {
            wingman_menu_data.current_col = 1;
            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {
            Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
        }
    }
}

void wingman_menu_left_arrow()
{
    wingman_menu_data.current_col--;

    if( wingman_menu_data.current_col < 0 ) {
        wingman_menu_data.current_col = 0;
        Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
    }
    else {

        /* special case for Cap'n Hardgeus */

        if( wingman_menu_data.current_row == 3 ) {
            wingman_menu_data.current_col = 1;
            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {
            Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
        }
    }
}


void custom_wingman1_menu_return_key()
{
    long current_selection;

    current_selection = (wingman_menu_data.current_row * 3) + wingman_menu_data.current_col;

    /* play sample */


    Play_Voice( wingman_menu_data.info[current_selection].sample );
    while( !Is_Voice_Done() );

    game_configuration.red1_ai = selection_to_ai( current_selection );
    game_configuration.red1_vehicle = wingman_menu_data.info[current_selection].preferred_vehicle;

    /* move to select wingman1's vehicle */

    init_custom_wingman2_menu();
    make_menu_current( &custom_wingman2_menu );

    /* play wingman catch phrase */
}


void custom_wingman2_menu_return_key()
{
    long current_selection;

    current_selection = (wingman_menu_data.current_row * 3) + wingman_menu_data.current_col;

    /* play sample */
    
    Play_Voice( wingman_menu_data.info[current_selection].sample );
    while( !Is_Voice_Done() );

    game_configuration.red2_ai = selection_to_ai( current_selection );
    game_configuration.red2_vehicle = wingman_menu_data.info[current_selection].preferred_vehicle;

    /* move to select wingman2's vehicle */

    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
}


void custom_opponent1_menu_return_key()
{
    long current_selection;

    current_selection = (wingman_menu_data.current_row * 3) + wingman_menu_data.current_col;

    /* play sample */

    Play_Voice( wingman_menu_data.info[current_selection].sample );
    while( !Is_Voice_Done() );

    game_configuration.blue0_ai = selection_to_ai( current_selection );
    game_configuration.blue0_vehicle = wingman_menu_data.info[current_selection].preferred_vehicle;

    /* move to select opponent1's vehicle */

    init_custom_opponent2_menu();
    make_menu_current( &custom_opponent2_menu );

}

void custom_opponent2_menu_return_key()
{
    long current_selection;

    current_selection = (wingman_menu_data.current_row * 3) + wingman_menu_data.current_col;

    /* play sample */

    Play_Voice( wingman_menu_data.info[current_selection].sample );
    while( !Is_Voice_Done() );

    game_configuration.blue1_ai = selection_to_ai( current_selection );
    game_configuration.blue1_vehicle = wingman_menu_data.info[current_selection].preferred_vehicle;

    /* move to select opponent2's vehicle */

    init_custom_opponent3_menu();
    make_menu_current( &custom_opponent3_menu );

}

void custom_opponent3_menu_return_key()
{
    long current_selection;

    current_selection = (wingman_menu_data.current_row * 3) + wingman_menu_data.current_col;

    /* play sample */

    Play_Voice( wingman_menu_data.info[current_selection].sample );
    while( !Is_Voice_Done() );

    game_configuration.blue2_ai = selection_to_ai( current_selection );
    game_configuration.blue2_vehicle = wingman_menu_data.info[current_selection].preferred_vehicle;

    /* move to select opponent3's vehicle */

    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
}


void custom_wingman1_menu_escape_key()
{
    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_wingman2_menu_escape_key()
{
    init_custom_wingman1_menu();
    make_menu_current( &custom_wingman1_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_opponent1_menu_escape_key()
{
    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_opponent2_menu_escape_key()
{
    init_custom_opponent1_menu();
    make_menu_current( &custom_opponent1_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_opponent3_menu_escape_key()
{
    init_custom_opponent2_menu();
    make_menu_current( &custom_opponent2_menu );

    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void character_picture_blit( long anchor_x, long anchor_y, pcx_picture *pcx )
{

    long i = 0, x, y;

    for( y = anchor_y; y < anchor_y + pcx->ypixels + 1; y++ ) {
        for( x = anchor_x; x < anchor_x + pcx->xpixels + 1; x++ ) {
            if( pcx->buffer[i] != 0 ) {
                DB_Fast_Pixel( x, y, pcx->buffer[i] );
            }
            i++;
        }
    }
}

void wingman_menu_draw_menu()
{
    long current_selection, i;

    const long name_x = 60;
    const long name_y = 120;

    const long race_x = 220;
    const long race_y = 120;

    const long description_x = 110;
    const long description_x_2 = 30;
    const long description_y = 130;
    const long description_space = 10;

    const long vehicle_x = 150;
    const long vehicle_y = 170;

    RGB_color *character_palette = NULL;
    pcx_picture *pcx = NULL;

    long anchor_x = 0, anchor_y = 0;
    Point upper_left, lower_right;

    char temp_str[50];

    current_selection = (wingman_menu_data.current_row * 3) + wingman_menu_data.current_col;

    if( wingman_menu_data.palette_active == FALSE ) {

        DB_Clear_Screen();        
        Swap_Buffer();
#ifdef GLCYLINDRIX
		GL_SubloadAndSwapBuffer();
#endif

        for( i = 0; i < 69; i++ ) {  /* the first 69 colors are reserved for the background */
            wingman_menu_data.menu_palette[i] = menu_stuff.wingman_menu_background.palette[i];
        }

        if( wingman_menu_data.current_row == 0 ) {
            character_palette = menu_stuff.human_1.palette;
        }
        else if( wingman_menu_data.current_row == 1 ) {
            character_palette = menu_stuff.human_2.palette;
        }
        else if( wingman_menu_data.current_row == 2 ) {
            character_palette = menu_stuff.human_3.palette;
        }
        else if( wingman_menu_data.current_row == 3 ) {
            character_palette = menu_stuff.human_4.palette;
        }
        else if( wingman_menu_data.current_row == 4 ) {
            character_palette = menu_stuff.overlords.palette;
        }
        else if( wingman_menu_data.current_row == 5 ) {
            character_palette = menu_stuff.pharoahs.palette;
        }
        else if( wingman_menu_data.current_row == 6 ) {
            character_palette = menu_stuff.scavengers.palette;
        }
        else if( wingman_menu_data.current_row == 7 ) {
            character_palette = menu_stuff.slar.palette;
        }
        else if( wingman_menu_data.current_row == 8 ) {
            character_palette = menu_stuff.succubi.palette;
        }
        else if( wingman_menu_data.current_row == 9 ) {
            character_palette = menu_stuff.watchers.palette;
        }
        else if( wingman_menu_data.current_row == 10 ) {
            character_palette = menu_stuff.bok.palette;
        }
        else if( wingman_menu_data.current_row == 11 ) {
            character_palette = menu_stuff.biomech.palette;
        }
        else if( wingman_menu_data.current_row == 12 ) {
            character_palette = menu_stuff.sentry.palette;
        }

        for( i = 69; i < 256; i++ ) {  /* the rest of the colors are from the character screen */
            wingman_menu_data.menu_palette[i] = character_palette[i];
        }

        Enable_Palette( wingman_menu_data.menu_palette );

        wingman_menu_data.palette_active = TRUE;
    }

    /* draw the background */

    Pop_Buffer( menu_stuff.wingman_menu_background.buffer );

    /* draw the characters */

    if( wingman_menu_data.current_row == 0 ) {
        pcx = &menu_stuff.human_1;
        anchor_x = 40;
        anchor_y = 30;
    }
    else if( wingman_menu_data.current_row == 1 ) {
        pcx = &menu_stuff.human_2;
        anchor_x = 40;
        anchor_y = 25;
    }
    else if( wingman_menu_data.current_row == 2 ) {
        pcx = &menu_stuff.human_3;
        anchor_x = 40;
        anchor_y = 23;
    }
    else if( wingman_menu_data.current_row == 3 ) {
        pcx = &menu_stuff.human_4;
        anchor_x = 120;
        anchor_y = 28;
    }
    else if( wingman_menu_data.current_row == 4 ) {
        pcx = &menu_stuff.overlords;
        anchor_x = 36;
        anchor_y = 34;
    }
    else if( wingman_menu_data.current_row == 5 ) {
        pcx = &menu_stuff.pharoahs;
        anchor_x = 38;
        anchor_y = 32;
    }
    else if( wingman_menu_data.current_row == 6 ) {
        pcx = &menu_stuff.scavengers;
        anchor_x = 37;
        anchor_y = 25;
    }
    else if( wingman_menu_data.current_row == 7 ) {
        pcx = &menu_stuff.slar;
        anchor_x = 38;
        anchor_y = 22;
    }
    else if( wingman_menu_data.current_row == 8 ) {
        pcx = &menu_stuff.succubi;
        anchor_x = 46;
        anchor_y = 23;
    }
    else if( wingman_menu_data.current_row == 9 ) {
        pcx = &menu_stuff.watchers;
        anchor_x = 45;
        anchor_y = 29;
    }
    else if( wingman_menu_data.current_row == 10 ) {
        pcx = &menu_stuff.bok;
        anchor_x = 39;
        anchor_y = 31;
    }
    else if( wingman_menu_data.current_row == 11 ) {
        pcx = &menu_stuff.biomech;
        anchor_x = 42;
        anchor_y = 32;
    }
    else if( wingman_menu_data.current_row == 12 ) {
        pcx = &menu_stuff.sentry;
        anchor_x = 38;
        anchor_y = 37;
    }

    character_picture_blit( anchor_x, anchor_y, pcx );

    /* draw the title */

    string_blit( wingman_menu_data.menu_title, 65, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    /* draw "name:" */

    micro_string_blit( "NAME:", name_x - 40, name_y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );

    /* draw "race:" */

    micro_string_blit( "RACE:", race_x - 40, race_y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );

    /* draw "description:" */

    micro_string_blit( "DESCRIPTION:", name_x - 40, description_y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );

    /* draw "preferred vehicle:" */

    micro_string_blit( "PREFERRED VEHICLE:", vehicle_x - 130, vehicle_y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );

    /* draw name */

    micro_string_blit( wingman_menu_data.info[current_selection].name,
                       name_x, name_y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    /* draw race */

    micro_string_blit( wingman_menu_data.info[current_selection].race,
                       race_x, race_y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    /* draw description_1 */

    micro_string_blit( wingman_menu_data.info[current_selection].description_1,
                       description_x, description_y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    /* draw description_2 */

    micro_string_blit( wingman_menu_data.info[current_selection].description_2,
                       description_x_2, description_y + (description_space),
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    /* draw description_3 */

    micro_string_blit( wingman_menu_data.info[current_selection].description_3,
                       description_x_2, description_y + (2 * description_space),
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    /* draw description_4 */

    micro_string_blit( wingman_menu_data.info[current_selection].description_4,
                       description_x_2, description_y + (3 * description_space),
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    /* draw prefered vehicle */

    if( wingman_menu_data.info[current_selection].preferred_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( wingman_menu_data.info[current_selection].preferred_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( wingman_menu_data.info[current_selection].preferred_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( wingman_menu_data.info[current_selection].preferred_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( wingman_menu_data.info[current_selection].preferred_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( wingman_menu_data.info[current_selection].preferred_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( wingman_menu_data.info[current_selection].preferred_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }
    else if( wingman_menu_data.info[current_selection].preferred_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }

    micro_string_blit( temp_str, vehicle_x, vehicle_y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    if( wingman_menu_data.current_col == 0 ) {
        upper_left[X] = 33;
        upper_left[Y] = 24;
        lower_right[X] = 103;
        lower_right[Y] = 98;
    }
    else if( wingman_menu_data.current_col == 1 ) {
        upper_left[X] = 119;
        upper_left[Y] = 24;
        lower_right[X] = 189;
        lower_right[Y] = 98;
    }
    else {
        upper_left[X] = 205;
        upper_left[Y] = 24;
        lower_right[X] = 275;
        lower_right[Y] = 98;
    }

    draw_missile_box( upper_left, lower_right, SELECTED_TEXT_COLOR );


    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_custom_wingman1_menu()
{
    long selection;

    custom_wingman1_menu.up_arrow = (Method)wingman_menu_up_arrow;
    custom_wingman1_menu.down_arrow = (Method)wingman_menu_down_arrow;
    custom_wingman1_menu.right_arrow = (Method)wingman_menu_right_arrow;
    custom_wingman1_menu.left_arrow = (Method)wingman_menu_left_arrow;
    custom_wingman1_menu.return_key = (Method)custom_wingman1_menu_return_key;
    custom_wingman1_menu.escape_key = (Method)custom_wingman1_menu_escape_key;
    custom_wingman1_menu.draw_menu = (Method)wingman_menu_draw_menu;

    wingman_menu_data.num_items = 39;

    wingman_menu_data.wingman_index = 1;

    strcpy( wingman_menu_data.menu_title, "SELECT WINGMAN 1" );

    selection = ai_to_selection( game_configuration.red1_ai );

    wingman_menu_data.current_row = selection / 3;
    wingman_menu_data.current_col = selection % 3;

    wingman_menu_data.palette_active = FALSE;
}

void init_custom_wingman2_menu()
{
    long selection;

    custom_wingman2_menu.up_arrow = (Method)wingman_menu_up_arrow;
    custom_wingman2_menu.down_arrow = (Method)wingman_menu_down_arrow;
    custom_wingman2_menu.right_arrow = (Method)wingman_menu_right_arrow;
    custom_wingman2_menu.left_arrow = (Method)wingman_menu_left_arrow;
    custom_wingman2_menu.return_key = (Method)custom_wingman2_menu_return_key;
    custom_wingman2_menu.escape_key = (Method)custom_wingman2_menu_escape_key;
    custom_wingman2_menu.draw_menu = (Method)wingman_menu_draw_menu;

    wingman_menu_data.num_items = 39;

    wingman_menu_data.wingman_index = 1;

    strcpy( wingman_menu_data.menu_title, "SELECT WINGMAN 2" );

    selection = ai_to_selection( game_configuration.red2_ai );

    wingman_menu_data.current_row = selection / 3;
    wingman_menu_data.current_col = selection % 3;

    wingman_menu_data.palette_active = FALSE;
}

void init_custom_opponent1_menu()
{
    long selection;

    custom_opponent1_menu.up_arrow = (Method)wingman_menu_up_arrow;
    custom_opponent1_menu.down_arrow = (Method)wingman_menu_down_arrow;
    custom_opponent1_menu.right_arrow = (Method)wingman_menu_right_arrow;
    custom_opponent1_menu.left_arrow = (Method)wingman_menu_left_arrow;
    custom_opponent1_menu.return_key = (Method)custom_opponent1_menu_return_key;
    custom_opponent1_menu.escape_key = (Method)custom_opponent1_menu_escape_key;
    custom_opponent1_menu.draw_menu = (Method)wingman_menu_draw_menu;

    wingman_menu_data.num_items = 39;

    wingman_menu_data.wingman_index = 1;

    strcpy( wingman_menu_data.menu_title, "SELECT OPPONENT 1" );

    selection = ai_to_selection( game_configuration.blue0_ai );

    wingman_menu_data.current_row = selection / 3;
    wingman_menu_data.current_col = selection % 3;

    wingman_menu_data.palette_active = FALSE;
}

void init_custom_opponent2_menu()
{
    long selection;

    custom_opponent2_menu.up_arrow = (Method)wingman_menu_up_arrow;
    custom_opponent2_menu.down_arrow = (Method)wingman_menu_down_arrow;
    custom_opponent2_menu.right_arrow = (Method)wingman_menu_right_arrow;
    custom_opponent2_menu.left_arrow = (Method)wingman_menu_left_arrow;
    custom_opponent2_menu.return_key = (Method)custom_opponent2_menu_return_key;
    custom_opponent2_menu.escape_key = (Method)custom_opponent2_menu_escape_key;
    custom_opponent2_menu.draw_menu = (Method)wingman_menu_draw_menu;

    wingman_menu_data.num_items = 39;

    wingman_menu_data.wingman_index = 1;

    strcpy( wingman_menu_data.menu_title, "SELECT OPPONENT 2" );

    selection = ai_to_selection( game_configuration.blue1_ai );

    wingman_menu_data.current_row = selection / 3;
    wingman_menu_data.current_col = selection % 3;

    wingman_menu_data.palette_active = FALSE;
}

void init_custom_opponent3_menu()
{
    long selection;

    custom_opponent3_menu.up_arrow = (Method)wingman_menu_up_arrow;
    custom_opponent3_menu.down_arrow = (Method)wingman_menu_down_arrow;
    custom_opponent3_menu.right_arrow = (Method)wingman_menu_right_arrow;
    custom_opponent3_menu.left_arrow = (Method)wingman_menu_left_arrow;
    custom_opponent3_menu.return_key = (Method)custom_opponent3_menu_return_key;
    custom_opponent3_menu.escape_key = (Method)custom_opponent3_menu_escape_key;
    custom_opponent3_menu.draw_menu = (Method)wingman_menu_draw_menu;

    wingman_menu_data.num_items = 39;

    wingman_menu_data.wingman_index = 1;

    strcpy( wingman_menu_data.menu_title, "SELECT OPPONENT 3" );

    selection = ai_to_selection( game_configuration.blue2_ai );

    wingman_menu_data.current_row = selection / 3;
    wingman_menu_data.current_col = selection % 3;

    wingman_menu_data.palette_active = FALSE;
}


void init_wingman_data()
{
    /* note: description_1 is about 30 characters long,
             description_2 - 4 is about 41 characters each. */

    strcpy( wingman_menu_data.info[0].name, "MEGAN HUNTER" );
    strcpy( wingman_menu_data.info[0].race, "HUMAN" );
    strcpy( wingman_menu_data.info[0].description_1, "A PHD IN ASTROPHYSICS, STRONG" );
    strcpy( wingman_menu_data.info[0].description_2, "STRATEGIC ABILITIES BUT LACKS IN" );
    strcpy( wingman_menu_data.info[0].description_3, "PILOTING SKILLS. LIKES TO GRAB" );
    strcpy( wingman_menu_data.info[0].description_4, "PYLONS." );
    wingman_menu_data.info[0].preferred_vehicle = Flea;
    wingman_menu_data.info[0].sample = menu_stuff.character_sounds.meg_sound;

    strcpy( wingman_menu_data.info[1].name, "KRISTEN MASTERS" );
    strcpy( wingman_menu_data.info[1].race, "HUMAN" );
    strcpy( wingman_menu_data.info[1].description_1, "A PILOT OF 747 COMMERCIAL" );
    strcpy( wingman_menu_data.info[1].description_2, "AIRLINERS, A STRONG BUT SLOW PILOT,  HER" );
    strcpy( wingman_menu_data.info[1].description_3, "MAIN STRENGTH IS DESTROYING THE ENEMIES" );
    strcpy( wingman_menu_data.info[1].description_4, "RADAR BASES EARLY." );
    wingman_menu_data.info[1].preferred_vehicle = Locust;
    wingman_menu_data.info[1].sample = menu_stuff.character_sounds.kri_sound;

    strcpy( wingman_menu_data.info[2].name, "CINDY VICIOUS" );
    strcpy( wingman_menu_data.info[2].race, "HUMAN" );
    strcpy( wingman_menu_data.info[2].description_1, "AN EXTREMLY VIOLENT AND UN-" );
    strcpy( wingman_menu_data.info[2].description_2, "PLEASANT INDIVIDUAL.  SHE HAS BEEN" );
    strcpy( wingman_menu_data.info[2].description_3, "KICKED OUT OF ALL MAJOR MOTORCYCLE GANGS" );
    strcpy( wingman_menu_data.info[2].description_4, "IN THE U.S. FOR HER PSYCHOTIC NATURE." );
    wingman_menu_data.info[2].preferred_vehicle = Wasp;
    wingman_menu_data.info[2].sample = menu_stuff.character_sounds.cin_sound;

    strcpy( wingman_menu_data.info[3].name, "MR. KENT" );
    strcpy( wingman_menu_data.info[3].race, "HUMAN" );                                                     
    strcpy( wingman_menu_data.info[3].description_1, "C.I.A. REAL NAME: UNKNOWN," );
    strcpy( wingman_menu_data.info[3].description_2, "PERSONAL HISTORY: UNKNOWN. UNNATURALLY" );
    strcpy( wingman_menu_data.info[3].description_3, "CALM IN TENSE SITUATIONS" );
    strcpy( wingman_menu_data.info[3].description_4, "" );
    wingman_menu_data.info[3].preferred_vehicle = Mosquito;
    wingman_menu_data.info[3].sample = menu_stuff.character_sounds.mrk_sound;

    strcpy( wingman_menu_data.info[4].name, "MAJOR FLOWERS" );
    strcpy( wingman_menu_data.info[4].race, "HUMAN" );
    strcpy( wingman_menu_data.info[4].description_1, "A CRACK NAVY BOMBER PILOT," );
    strcpy( wingman_menu_data.info[4].description_2, "SLATED TO LEAVE FOR BOSNIA ON THE NIGHT" );
    strcpy( wingman_menu_data.info[4].description_3, "HE WAS ABDUCTED, A FIERCE PYLON GRABBER." );
    strcpy( wingman_menu_data.info[4].description_4, "" );
    wingman_menu_data.info[4].preferred_vehicle = Roach;
    wingman_menu_data.info[4].sample = menu_stuff.character_sounds.maj_sound;

    strcpy( wingman_menu_data.info[5].name, "PREACHER" );
    strcpy( wingman_menu_data.info[5].race, "HUMAN" );                                                     
    strcpy( wingman_menu_data.info[5].description_1, "A SELF PROCLAIMED UNDER-" );
    strcpy( wingman_menu_data.info[5].description_2, "ACHIEVER, WORLD RENOWNED FOR HIS BIZARRE" );
    strcpy( wingman_menu_data.info[5].description_3, "BONG COLLECTION. A LOSER." );
    strcpy( wingman_menu_data.info[5].description_4, "" );
    wingman_menu_data.info[5].preferred_vehicle = Dragonfly;
    wingman_menu_data.info[5].sample = menu_stuff.character_sounds.pre_sound;

    strcpy( wingman_menu_data.info[6].name, "KELLY SANDERS" );
    strcpy( wingman_menu_data.info[6].race, "HUMAN" );
    strcpy( wingman_menu_data.info[6].description_1, "AEROBICS INSTRUCTOR AND PART-" );
    strcpy( wingman_menu_data.info[6].description_2, "TIME MODEL. SHE IS SHY AND EVASIVE BUT" );
    strcpy( wingman_menu_data.info[6].description_3, "WILL MAKE SHORT WORK OF ENEMY RADAR" );
    strcpy( wingman_menu_data.info[6].description_4, "BASES." );
    wingman_menu_data.info[6].preferred_vehicle = Roach;
    wingman_menu_data.info[6].sample = menu_stuff.character_sounds.kel_sound;

    strcpy( wingman_menu_data.info[7].name, "SHANA BLAKE" );
    strcpy( wingman_menu_data.info[7].race, "HUMAN" );
    strcpy( wingman_menu_data.info[7].description_1, "A CHICAGO HOMICIDE DETECTIVE," );
    strcpy( wingman_menu_data.info[7].description_2, "KICKED OFF THE FORCE FOR MULTIPLE COUNTS" );
    strcpy( wingman_menu_data.info[7].description_3, "OF POLICE BRUTALITY." );
    strcpy( wingman_menu_data.info[7].description_4, "" );
    wingman_menu_data.info[7].preferred_vehicle = Dragonfly;
    wingman_menu_data.info[7].sample = menu_stuff.character_sounds.sha_sound;

    strcpy( wingman_menu_data.info[8].name, "RICHARD STRYKER" );
    strcpy( wingman_menu_data.info[8].race, "HUMAN" );
    strcpy( wingman_menu_data.info[8].description_1, "AN AIR FORCE FIGHTER PILOT," );
    strcpy( wingman_menu_data.info[8].description_2, "CODE NAME 'JOHNSON', KNOWN FOR HIS" );
    strcpy( wingman_menu_data.info[8].description_3, "ABILITY TO TAKE OUT ENEMY RADAR BASES." );
    strcpy( wingman_menu_data.info[8].description_4, "" );
    wingman_menu_data.info[8].preferred_vehicle = Dragonfly;
    wingman_menu_data.info[8].sample = menu_stuff.character_sounds.ric_sound;

    strcpy( wingman_menu_data.info[9].name, "BLANK" );
    strcpy( wingman_menu_data.info[9].race, "BLANK" );
    strcpy( wingman_menu_data.info[9].description_1, "BLANK" );
    strcpy( wingman_menu_data.info[9].description_2, "" );
    strcpy( wingman_menu_data.info[9].description_3, "" );
    strcpy( wingman_menu_data.info[9].description_4, "" );
    wingman_menu_data.info[9].preferred_vehicle = Mosquito;
    wingman_menu_data.info[9].sample = menu_stuff.menu_sounds.error_sound;

    strcpy( wingman_menu_data.info[10].name, "CAP'N HARDGEUS" );
    strcpy( wingman_menu_data.info[10].race, "HUMAN" );
    strcpy( wingman_menu_data.info[10].description_1, "A WELL BALENCED GROUND BASED" );
    strcpy( wingman_menu_data.info[10].description_2, "PILOT, KNOWN FOR HIS MASSIVE PORNO" );
    strcpy( wingman_menu_data.info[10].description_3, "COLLECTION AND GREAT HAND SPEED." );
    strcpy( wingman_menu_data.info[10].description_4, ""  );
    wingman_menu_data.info[10].preferred_vehicle = Flea;
    wingman_menu_data.info[10].sample = menu_stuff.character_sounds.cap_sound;

    strcpy( wingman_menu_data.info[11].name, "BLANK" );
    strcpy( wingman_menu_data.info[11].race, "BLANK" );
    strcpy( wingman_menu_data.info[11].description_1, "BLANK" );
    strcpy( wingman_menu_data.info[11].description_2, "" );
    strcpy( wingman_menu_data.info[11].description_3, "" );
    strcpy( wingman_menu_data.info[11].description_4, "" );
    wingman_menu_data.info[11].preferred_vehicle = Roach;
    wingman_menu_data.info[11].sample = menu_stuff.menu_sounds.error_sound;

    strcpy( wingman_menu_data.info[12].name, "MEPHISTO" );
    strcpy( wingman_menu_data.info[12].race, "OVERLORD" );
    strcpy( wingman_menu_data.info[12].description_1, "THE SLAYER OF SOULS, THE" );
    strcpy( wingman_menu_data.info[12].description_2, "DEVOURER OF WORLDS AND MASTER OF TUESDAY" );
    strcpy( wingman_menu_data.info[12].description_3, "NIGHT BINGO. A PYLON GRABBING HELL" );
    strcpy( wingman_menu_data.info[12].description_4, "FIEND." );
    wingman_menu_data.info[12].preferred_vehicle = Wasp;
    wingman_menu_data.info[12].sample = menu_stuff.character_sounds.mep_sound;

    strcpy( wingman_menu_data.info[13].name, "MALFATOR" );
    strcpy( wingman_menu_data.info[13].race, "OVERLORD" );                                                      
    strcpy( wingman_menu_data.info[13].description_1, "THE ROOT OF TEMPTATION AND" );
    strcpy( wingman_menu_data.info[13].description_2, "DAMNATION, THE FATHER OF MOSER AND ALL" );
    strcpy( wingman_menu_data.info[13].description_3, "THINGS WRETCHED, LIKES TO KILL RADAR" );
    strcpy( wingman_menu_data.info[13].description_4, "BASES." );
    wingman_menu_data.info[13].preferred_vehicle = Spider;
    wingman_menu_data.info[13].sample = menu_stuff.character_sounds.mal_sound;

    strcpy( wingman_menu_data.info[14].name, "BAAL" );
    strcpy( wingman_menu_data.info[14].race, "OVERLORD" );
    strcpy( wingman_menu_data.info[14].description_1, "ARCH-DEMON OF THE INFINITE" );
    strcpy( wingman_menu_data.info[14].description_2, "SADNESS. HOBBIES INCLUDE ROASTING SOULS" );
    strcpy( wingman_menu_data.info[14].description_3, "OF THE DAMNED AND FINGER PAINTING." );
    strcpy( wingman_menu_data.info[14].description_4, "" );
    wingman_menu_data.info[14].preferred_vehicle = Wasp;
    wingman_menu_data.info[14].sample = menu_stuff.character_sounds.baa_sound;

    strcpy( wingman_menu_data.info[15].name, "ANUBIS" );
    strcpy( wingman_menu_data.info[15].race, "PHAROAH" );
    strcpy( wingman_menu_data.info[15].description_1, "A SKILLED COMBATANT WHO KNOWS" );
    strcpy( wingman_menu_data.info[15].description_2, "WHEN TO FIGHT AND WHEN TO REGROUP." );
    strcpy( wingman_menu_data.info[15].description_3, "YOU'LL BE HARD PRESSED TO FIND A BETTER" );
    strcpy( wingman_menu_data.info[15].description_4, "WARRIOR." );
    wingman_menu_data.info[15].preferred_vehicle = Wasp;
    wingman_menu_data.info[15].sample = menu_stuff.character_sounds.anu_sound;

    strcpy( wingman_menu_data.info[16].name, "HORUS" );
    strcpy( wingman_menu_data.info[16].race, "PHAROAH" );
    strcpy( wingman_menu_data.info[16].description_1, "A WELL BALANCED FIGHTER WHO" );
    strcpy( wingman_menu_data.info[16].description_2, "EXCELS IN SNEAK ATTACKS.  SPENDS MUCH OF" );
    strcpy( wingman_menu_data.info[16].description_3, "HIS TIME CLOAKED AND REAPPEARS WITH A" );
    strcpy( wingman_menu_data.info[16].description_4, "VENGEANCE." );
    wingman_menu_data.info[16].preferred_vehicle = Dragonfly;
    wingman_menu_data.info[16].sample = menu_stuff.character_sounds.hor_sound;

    strcpy( wingman_menu_data.info[17].name, "BAST" );
    strcpy( wingman_menu_data.info[17].race, "PHAROAH" );
    strcpy( wingman_menu_data.info[17].description_1, "A WORTHY ADVERSARY WHO IS A" );
    strcpy( wingman_menu_data.info[17].description_2, "MASTER OF SCRAMBLING HER OPPONENTS" );
    strcpy( wingman_menu_data.info[17].description_3, "CONTROLS. SHE TAKES GREAT PLEASURE" );
    strcpy( wingman_menu_data.info[17].description_4, "IN THE AGONY OF HER ENEMIES." );
    wingman_menu_data.info[17].preferred_vehicle = Beetle;
    wingman_menu_data.info[17].sample = menu_stuff.character_sounds.bas_sound;

    strcpy( wingman_menu_data.info[18].name, "GREKO" );
    strcpy( wingman_menu_data.info[18].race, "SCAVENGER" );                                                      
    strcpy( wingman_menu_data.info[18].description_1, "A PATHOLOGICAL LIAR AND" );
    strcpy( wingman_menu_data.info[18].description_2, "MEGALOMANIAC WHOSE FAVORITE TACTIC IS" );
    strcpy( wingman_menu_data.info[18].description_3, "SITTING STILL AND DOING NOTHING." );
    strcpy( wingman_menu_data.info[18].description_4, "" );
    wingman_menu_data.info[18].preferred_vehicle = Roach;
    wingman_menu_data.info[18].sample = menu_stuff.character_sounds.gre_sound;

    strcpy( wingman_menu_data.info[19].name, "CRUSTY" );
    strcpy( wingman_menu_data.info[19].race, "SCAVENGER" );
    strcpy( wingman_menu_data.info[19].description_1, "AN INTERGALACTIC SHOPLIFTER" );
    strcpy( wingman_menu_data.info[19].description_2, "KNOWN FOR HIS EXTREME CLUMSINESS AND" );
    strcpy( wingman_menu_data.info[19].description_3, "BODY ODOR." );
    strcpy( wingman_menu_data.info[19].description_4, "" );
    wingman_menu_data.info[19].preferred_vehicle = Wasp;
    wingman_menu_data.info[19].sample = menu_stuff.character_sounds.cru_sound;

    strcpy( wingman_menu_data.info[20].name, "HOTWIRE" );
    strcpy( wingman_menu_data.info[20].race, "SCAVENGER" );
    strcpy( wingman_menu_data.info[20].description_1, "AN INEPT CYBORGANIC" );
    strcpy( wingman_menu_data.info[20].description_2, "TECHNOPHOBE WHO IS UNABLE TO PERFORM" );
    strcpy( wingman_menu_data.info[20].description_3, "THE MOST BASIC MANEUVERS." );
    strcpy( wingman_menu_data.info[20].description_4, "" );
    wingman_menu_data.info[20].preferred_vehicle = Spider;
    wingman_menu_data.info[20].sample = menu_stuff.character_sounds.hot_sound;

    strcpy( wingman_menu_data.info[21].name, "GALL" );
    strcpy( wingman_menu_data.info[21].race, "SLAR" );
    strcpy( wingman_menu_data.info[21].description_1, "HE IS CELEBRATED BY THE SLAR" );
    strcpy( wingman_menu_data.info[21].description_2, "FOR HIS SADISTIC MIND. ALSO HAS THE" );
    strcpy( wingman_menu_data.info[21].description_3, "UNCANNY ABILITY TO TURN HIMSELF INSIDE-" );
    strcpy( wingman_menu_data.info[21].description_4, "OUT." );
    wingman_menu_data.info[21].preferred_vehicle = Spider;
    wingman_menu_data.info[21].sample = menu_stuff.character_sounds.gal_sound;

    strcpy( wingman_menu_data.info[22].name, "PUBERT" );
    strcpy( wingman_menu_data.info[22].race, "SLAR" );
    strcpy( wingman_menu_data.info[22].description_1, "A CHAMPION SLUDGE WRESTLER" );
    strcpy( wingman_menu_data.info[22].description_2, "AND STENCH-BEAN FARMER. HE IS VERY" );
    strcpy( wingman_menu_data.info[22].description_3, "AGGRESIVE AND RATHER UNPLEASENT." );
    strcpy( wingman_menu_data.info[22].description_4, "" );
    wingman_menu_data.info[22].preferred_vehicle = Spider;
    wingman_menu_data.info[22].sample = menu_stuff.character_sounds.pub_sound;

    strcpy( wingman_menu_data.info[23].name, "SEPTIC" );
    strcpy( wingman_menu_data.info[23].race, "SLAR" );                                                      
    strcpy( wingman_menu_data.info[23].description_1, "THE ONLY TWO THINGS MORE" );
    strcpy( wingman_menu_data.info[23].description_2, "DISGUSTING THAN HIS NAME ARE HIS BODY" );
    strcpy( wingman_menu_data.info[23].description_3, "ODOR AND HIS MATING HABITS." );
    strcpy( wingman_menu_data.info[23].description_4, "" );
    wingman_menu_data.info[23].preferred_vehicle = Wasp;
    wingman_menu_data.info[23].sample = menu_stuff.character_sounds.sep_sound;

    strcpy( wingman_menu_data.info[24].name, "CRYSTAL" );
    strcpy( wingman_menu_data.info[24].race, "SUCCUBI" );
    strcpy( wingman_menu_data.info[24].description_1, "PSYCHOTIC AND HARSH BY HUMAN" );
    strcpy( wingman_menu_data.info[24].description_2, "STANDARDS SHE IS A MASTER OF SURFACE" );
    strcpy( wingman_menu_data.info[24].description_3, "NAVIGATION WHICH ENABLES HER TO AMASS A" );
    strcpy( wingman_menu_data.info[24].description_4, "GREAT DEAL OF ENERGY FOR HER TEAMMATES." );
    wingman_menu_data.info[24].preferred_vehicle = Dragonfly;
    wingman_menu_data.info[24].sample = menu_stuff.character_sounds.cry_sound;

    strcpy( wingman_menu_data.info[25].name, "GLACIA" );
    strcpy( wingman_menu_data.info[25].race, "SUCCUBI" );
    strcpy( wingman_menu_data.info[25].description_1, "THE LEADER OF THE SUCCUBI" );
    strcpy( wingman_menu_data.info[25].description_2, "TEAM, SHE USES HER AGGRESSIVE NATURE TO" );
    strcpy( wingman_menu_data.info[25].description_3, "PRESSURE HER OPPONENTS INTO SUBMISSION." );
    strcpy( wingman_menu_data.info[25].description_4, "" );
    wingman_menu_data.info[25].preferred_vehicle = Mosquito;
    wingman_menu_data.info[25].sample = menu_stuff.character_sounds.gla_sound;

    strcpy( wingman_menu_data.info[26].name, "AZURIA" );
    strcpy( wingman_menu_data.info[26].race, "SUCCUBI" );
    strcpy( wingman_menu_data.info[26].description_1, "SHE IS A SUPREME AERIAL" );
    strcpy( wingman_menu_data.info[26].description_2, "AGGRESSOR, SHE HAS BEEN TRAINED IN THE" );
    strcpy( wingman_menu_data.info[26].description_3, "BITING WINDS OF HER FRIGID HOME WORLD." );
    strcpy( wingman_menu_data.info[26].description_4, "" );
    wingman_menu_data.info[26].preferred_vehicle = Beetle;
    wingman_menu_data.info[26].sample = menu_stuff.character_sounds.azu_sound;

    strcpy( wingman_menu_data.info[27].name, "NEBULA" );
    strcpy( wingman_menu_data.info[27].race, "WATCHER" );
    strcpy( wingman_menu_data.info[27].description_1, "SPIRITUALLY IN TUNE WITH THE" );
    strcpy( wingman_menu_data.info[27].description_2, "STRATEGY OF THE CYLINDRIX. THE OLDEST" );
    strcpy( wingman_menu_data.info[27].description_3, "LIVING MEMBER OF THIS ANCIENT RACE." );
    strcpy( wingman_menu_data.info[27].description_4, "FOCUSES ON RADAR BASE DESTRUCTION." );
    wingman_menu_data.info[27].preferred_vehicle = Dragonfly;
    wingman_menu_data.info[27].sample = menu_stuff.character_sounds.neb_sound;

    strcpy( wingman_menu_data.info[28].name, "POLARIS" );
    strcpy( wingman_menu_data.info[28].race, "WATCHER" );
    strcpy( wingman_menu_data.info[28].description_1, "HIS TOTAL HARMONY WITH THE" );
    strcpy( wingman_menu_data.info[28].description_2, "UNIVERSE ALLOWS HIM TO FOCUS THE KNOW-" );
    strcpy( wingman_menu_data.info[28].description_3, "LEDGE OF THOUSANDS OF GENERATIONS ONTO" );
    strcpy( wingman_menu_data.info[28].description_4, "HIS SOLE PURPOSE. KICKING ASS." );
    wingman_menu_data.info[28].preferred_vehicle = Locust;
    wingman_menu_data.info[28].sample = menu_stuff.character_sounds.pol_sound;

    strcpy( wingman_menu_data.info[29].name, "QUASAR" );
    strcpy( wingman_menu_data.info[29].race, "WATCHER" );
    strcpy( wingman_menu_data.info[29].description_1, "MEDITATION AND INFINITE" );
    strcpy( wingman_menu_data.info[29].description_2, "PATIENCE ALLOW HIM TO CONTROL THE FLOW" );
    strcpy( wingman_menu_data.info[29].description_3, "OF A BATTLE BY CAPTURING PYLONS AND AND" );
    strcpy( wingman_menu_data.info[29].description_4, "STEERING CLEAR OF THE FRAY." );
    wingman_menu_data.info[29].preferred_vehicle = Flea;
    wingman_menu_data.info[29].sample = menu_stuff.character_sounds.qua_sound;

    strcpy( wingman_menu_data.info[30].name, "BOK" );
    strcpy( wingman_menu_data.info[30].race, "BOK" );
    strcpy( wingman_menu_data.info[30].description_1, "HIGH PRIEST OF THE 5TH MENTAL" );
    strcpy( wingman_menu_data.info[30].description_2, "ORDER. USES HIS TELEPATHIC ABILITY TO" );
    strcpy( wingman_menu_data.info[30].description_3, "PROVIDE HIS TEAMMATES WITH A STEADY" );
    strcpy( wingman_menu_data.info[30].description_4, "SUPPLY OF PYLONS" );
    wingman_menu_data.info[30].preferred_vehicle = Flea;
    wingman_menu_data.info[30].sample = menu_stuff.character_sounds.bok_sound;

    strcpy( wingman_menu_data.info[31].name, "N'BOK" );
    strcpy( wingman_menu_data.info[31].race, "BOK" );
    strcpy( wingman_menu_data.info[31].description_1, "AN ACOLYTE FOR THE TEMPLE OF" );
    strcpy( wingman_menu_data.info[31].description_2, "THE EXPANDED MIND. HE USES THE TEACHINGS" );
    strcpy( wingman_menu_data.info[31].description_3, "OF MASTER L'BOK TO GUIDE HIS THOUGHTS" );
    strcpy( wingman_menu_data.info[31].description_4, "DURING BATTLE." );
    wingman_menu_data.info[31].preferred_vehicle = Dragonfly;
    wingman_menu_data.info[31].sample = menu_stuff.character_sounds.nbo_sound;

    strcpy( wingman_menu_data.info[32].name, "T'BOK" );
    strcpy( wingman_menu_data.info[32].race, "BOK" );
    strcpy( wingman_menu_data.info[32].description_1, "THE GUARDIAN OF THE STONE OF" );
    strcpy( wingman_menu_data.info[32].description_2, "BOK, HOLDER OF THE SASH OF SAM'BOK AND" );
    strcpy( wingman_menu_data.info[32].description_3, "THE SEPTOR OF BILL'BOK. A PYLON GRABBER" );
    strcpy( wingman_menu_data.info[32].description_4, "AND A FIERCE COMPETITOR." );
    wingman_menu_data.info[32].preferred_vehicle = Locust;
    wingman_menu_data.info[32].sample = menu_stuff.character_sounds.tbo_sound;

    strcpy( wingman_menu_data.info[33].name, "B00B" );
    strcpy( wingman_menu_data.info[33].race, "BIOMECHANOID" );
    strcpy( wingman_menu_data.info[33].description_1, "OFFENSIVE ATTACK UNIT," );
    strcpy( wingman_menu_data.info[33].description_2, "PROGRAMMED TO THRIVE ON COMBAT AND THE" );
    strcpy( wingman_menu_data.info[33].description_3, "PAIN OF ORGANIC LIFE FORMS." );
    strcpy( wingman_menu_data.info[33].description_4, "USER-UNFRIENDLY." );
    wingman_menu_data.info[33].preferred_vehicle = Mosquito;
    wingman_menu_data.info[33].sample = menu_stuff.character_sounds.b00_sound;

    strcpy( wingman_menu_data.info[34].name, "D23B6M" );
    strcpy( wingman_menu_data.info[34].race, "BIOMECHANOID" );
    strcpy( wingman_menu_data.info[34].description_1, "TOTAL DESTRUCTION UNIT," );
    strcpy( wingman_menu_data.info[34].description_2, "DESIGNED FOR FOR OPTIMAL ENEMY DELETION" );
    strcpy( wingman_menu_data.info[34].description_3, "AND WIRED WITH RADAR DESTRUCTION SUB-" );
    strcpy( wingman_menu_data.info[34].description_4, "ROUTINES." );
    wingman_menu_data.info[34].preferred_vehicle = Spider;
    wingman_menu_data.info[34].sample = menu_stuff.character_sounds.d23_sound;

    strcpy( wingman_menu_data.info[35].name, "JLDV8" );
    strcpy( wingman_menu_data.info[35].race, "BIOMECHANOID" );                                                      
    strcpy( wingman_menu_data.info[35].description_1, "STRATEGY DROID, DESIGNED" );
    strcpy( wingman_menu_data.info[35].description_2, "FOR SPEED AND TACTICAL EFFICIENCY. HIGHLY" );
    strcpy( wingman_menu_data.info[35].description_3, "OPTOMIZED FOR PYLON ACQUISITION." );
    strcpy( wingman_menu_data.info[35].description_4, "" );
    wingman_menu_data.info[35].preferred_vehicle = Flea;
    wingman_menu_data.info[35].sample = menu_stuff.character_sounds.jld_sound;

    strcpy( wingman_menu_data.info[36].name, "G" );
    strcpy( wingman_menu_data.info[36].race, "SENTRY" );                                                      
    strcpy( wingman_menu_data.info[36].description_1, "USES HIS POWERFUL SENSOR" );
    strcpy( wingman_menu_data.info[36].description_2, "ARRAY TO QUICKLY IDENTIFY AND ELIMINATE" );
    strcpy( wingman_menu_data.info[36].description_3, "ENEMIES FROM THE AIR." );
    strcpy( wingman_menu_data.info[36].description_4, "" );
    wingman_menu_data.info[36].preferred_vehicle = Mosquito;
    wingman_menu_data.info[36].sample = menu_stuff.character_sounds.g_sound;

    strcpy( wingman_menu_data.info[37].name, "B" );
    strcpy( wingman_menu_data.info[37].race, "SENTRY" );
    strcpy( wingman_menu_data.info[37].description_1, "HIS HYPER-LOGIC COMPATIBLE" );
    strcpy( wingman_menu_data.info[37].description_2, "PYLON DETECTION SENSOR ALLOWS FOR QUICK" );
    strcpy( wingman_menu_data.info[37].description_3, "CLASSIFICATION AND ACQUISITION OF" );
    strcpy( wingman_menu_data.info[37].description_4, "PYLONS." );
    wingman_menu_data.info[37].preferred_vehicle = Roach;
    wingman_menu_data.info[37].sample = menu_stuff.character_sounds.b_sound;

    strcpy( wingman_menu_data.info[38].name, "R" );
    strcpy( wingman_menu_data.info[38].race, "SENTRY" );    			                              
    strcpy( wingman_menu_data.info[38].description_1, "HE IS CONSIDERED THE MOST" );
    strcpy( wingman_menu_data.info[38].description_2, "ADVANCED FUZZ BUSTER IN THE UNIVERSE DUE" );
    strcpy( wingman_menu_data.info[38].description_3, "TO HIS RADAR DETECTION SENSOR, WHICH" );
    strcpy( wingman_menu_data.info[38].description_4, "ALLOWS HIM TO DESTROY RADAR BASES EARLY." );
    wingman_menu_data.info[38].preferred_vehicle = Beetle;
    wingman_menu_data.info[38].sample = menu_stuff.character_sounds.r_sound;

} /* End of Init_Wingman_Data() */


void vehicle_menu_up_arrow()
{
    vehicle_menu_data.current_selection = (vehicle_menu_data.current_selection - 1) % vehicle_menu_data.num_items;
    if( vehicle_menu_data.current_selection < 0 ) {
        vehicle_menu_data.current_selection = vehicle_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void vehicle_menu_down_arrow()
{
    vehicle_menu_data.current_selection = (vehicle_menu_data.current_selection + 1) % vehicle_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void vehicle_menu_right_arrow()
{
    vehicle_menu_data.current_selection = (vehicle_menu_data.current_selection + 1) % vehicle_menu_data.num_items;

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void vehicle_menu_left_arrow()
{
    vehicle_menu_data.current_selection = (vehicle_menu_data.current_selection - 1) % vehicle_menu_data.num_items;
    if( vehicle_menu_data.current_selection < 0 ) {
        vehicle_menu_data.current_selection = vehicle_menu_data.num_items - 1;
    }

    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void custom_vehicle0_menu_return_key()
{
    game_configuration.red0_vehicle = vehicle_menu_data.current_selection;

    if( vehicle_menu_data.current_selection == Beetle ) {
		Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
		while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Wasp ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }
    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );

}

void custom_vehicle1_menu_return_key()
{
    game_configuration.red1_vehicle = vehicle_menu_data.current_selection;

    if( vehicle_menu_data.current_selection == Beetle ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Wasp ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );    
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }
    init_custom_vehicle2_menu();
    make_menu_current( &custom_vehicle2_menu );

}

void custom_vehicle2_menu_return_key()
{
    game_configuration.red2_vehicle = vehicle_menu_data.current_selection;
    
    if( vehicle_menu_data.current_selection == Beetle ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Wasp ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }    
    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();    
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );

}

void custom_vehicle3_menu_return_key()
{
    game_configuration.blue0_vehicle = vehicle_menu_data.current_selection;

    if( vehicle_menu_data.current_selection == Beetle ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Wasp ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }
    
    init_custom_vehicle4_menu();
    make_menu_current( &custom_vehicle4_menu );
}

void custom_vehicle4_menu_return_key()
{
    game_configuration.blue1_vehicle = vehicle_menu_data.current_selection;
    
    if( vehicle_menu_data.current_selection == Beetle ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Wasp ) {
		Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }
    init_custom_vehicle5_menu();
    make_menu_current( &custom_vehicle5_menu );
}

void custom_vehicle5_menu_return_key()
{
    game_configuration.blue2_vehicle = vehicle_menu_data.current_selection;
    
    if( vehicle_menu_data.current_selection == Beetle ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Wasp ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }
    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();    
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );

}

void custom_vehicle0_menu_escape_key()
{

    /* go back to the game_menu */

    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();    
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_vehicle1_menu_escape_key()
{
    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();    
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_vehicle2_menu_escape_key()
{
    init_custom_vehicle1_menu();
    make_menu_current( &custom_vehicle1_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_vehicle3_menu_escape_key()
{
    make_menu_current( &custom_game_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_vehicle4_menu_escape_key()
{
    init_custom_vehicle3_menu();
    make_menu_current( &custom_vehicle3_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void custom_vehicle5_menu_escape_key()
{
    init_custom_vehicle4_menu();
    make_menu_current( &custom_vehicle4_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void vehicle_menu_draw_menu()
{
    char temp_str[50];
    BarInfo bar_info;
    PointFace *pointface_ptr = NULL;
    Orientation view_orient;
    Point light;
    Float_Point new_front, new_up;
    const float x_rot = 0.017453f;  /* 1 deg */
    const float y_rot = 0.017453f;  /* 1 deg */
    const float z_rot = 0.008727f;  /* 0.5 deg */

    Pop_Buffer( menu_stuff.vehicle_menu_background.buffer );

    /* menu title */

    string_blit( vehicle_menu_data.menu_title, 75, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    /* print vehicle type */

    if( vehicle_menu_data.current_selection == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( vehicle_menu_data.current_selection == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    /* draw wingman */

    micro_string_blit( "PILOT:", 110, 20,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       4 );

    micro_string_blit( vehicle_menu_data.wingman_name, 159, 20,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       SELECTED_TEXT_COLOR );

    /* draw preferred vehicle */

    micro_string_blit( "PREFERRED VEHICLE:", 110, 30,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       4 );

    micro_string_blit( vehicle_menu_data.preferred_vehicle_name, 241, 30,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       SELECTED_TEXT_COLOR );

    /* draw vehicle */

    micro_string_blit( "VEHICLE:", 160, 140,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       4 );

    micro_string_blit( temp_str, 220, 140,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       SELECTED_TEXT_COLOR );

    /* draw the attribute titles */

    micro_string_blit( "SHEILDS:", 80, 155,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       4 );

    micro_string_blit( "FIREPOWER:", 66, 164,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       4 );

    micro_string_blit( "AIR SPEED:", 65, 173,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       4 );

    micro_string_blit( "SURFACE SPEED:", 37, 182,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       4 );

    micro_string_blit( "SPECIAL WEAPON:", 30, 191,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       4 );

    /* draw the attributes */

    /* shields */

    bar_info.upper_left[X] = 160;
    bar_info.upper_left[Y] = 154;
    bar_info.lower_right[X] = 210;
    bar_info.lower_right[Y] = 161;
    bar_info.min_val = 0;
    bar_info.max_val = 10;
    bar_info.val = vehicle_menu_data.info[vehicle_menu_data.current_selection].shields;
    bar_info.bar_color = SELECTED_TEXT_COLOR;
    bar_info.outline_color = 1;
    bar_info.type = HorizontalLeftJustified;
    draw_bar( &bar_info );

    /* firepower */

    bar_info.upper_left[X] = 160;
    bar_info.upper_left[Y] = 163;
    bar_info.lower_right[X] = 210;
    bar_info.lower_right[Y] = 170;
    bar_info.min_val = 0;
    bar_info.max_val = 10;
    bar_info.val = vehicle_menu_data.info[vehicle_menu_data.current_selection].firepower;
    bar_info.bar_color = SELECTED_TEXT_COLOR;
    bar_info.outline_color = 1;
    bar_info.type = HorizontalLeftJustified;
    draw_bar( &bar_info );

    /* air_speed */

    bar_info.upper_left[X] = 160;
    bar_info.upper_left[Y] = 172;
    bar_info.lower_right[X] = 210;
    bar_info.lower_right[Y] = 179;
    bar_info.min_val = 0;
    bar_info.max_val = 10;
    bar_info.val = vehicle_menu_data.info[vehicle_menu_data.current_selection].air_speed;
    bar_info.bar_color = SELECTED_TEXT_COLOR;
    bar_info.outline_color = 1;
    bar_info.type = HorizontalLeftJustified;
    draw_bar( &bar_info );

    /* surface_speed */

    bar_info.upper_left[X] = 160;
    bar_info.upper_left[Y] = 181;
    bar_info.lower_right[X] = 210;
    bar_info.lower_right[Y] = 188;
    bar_info.min_val = 0;
    bar_info.max_val = 10;
    bar_info.val = vehicle_menu_data.info[vehicle_menu_data.current_selection].surface_speed;
    bar_info.bar_color = SELECTED_TEXT_COLOR;
    bar_info.outline_color = 1;
    bar_info.type = HorizontalLeftJustified;
    draw_bar( &bar_info );

    /* special_weapon */

    micro_string_blit( vehicle_menu_data.info[vehicle_menu_data.current_selection].special_weapon,
                       160, 191,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1,
                       SELECTED_TEXT_COLOR );

    /* draw the spinning vehicle */

    if( vehicle_menu_data.current_selection == Wasp ) {
        pointface_ptr = &(menu_stuff.wasp_obj);
    }
    else if( vehicle_menu_data.current_selection == Beetle ) {
        pointface_ptr = &(menu_stuff.beetle_obj);
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        pointface_ptr = &(menu_stuff.flea_obj);
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        pointface_ptr = &(menu_stuff.mosquito_obj);
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        pointface_ptr = &(menu_stuff.spider_obj);
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        pointface_ptr = &(menu_stuff.roach_obj);
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        pointface_ptr = &(menu_stuff.locust_obj);
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        pointface_ptr = &(menu_stuff.dragonfly_obj);
    }

    /* rotate the orient around x-axis */

    new_front[X] = vehicle_menu_data.orient.front[X];
    new_front[Y] = (vehicle_menu_data.orient.front[Y] * (float)cos( x_rot )) -
                   (vehicle_menu_data.orient.front[Z] * (float)sin( x_rot ));
    new_front[Z] = (vehicle_menu_data.orient.front[Y] * (float)sin( x_rot )) +
                   (vehicle_menu_data.orient.front[Z] * (float)cos( x_rot ));

    new_up[X] = vehicle_menu_data.orient.up[X];
    new_up[Y] = (vehicle_menu_data.orient.up[Y] * (float)cos( x_rot )) -
                (vehicle_menu_data.orient.up[Z] * (float)sin( x_rot ));
    new_up[Z] = (vehicle_menu_data.orient.up[Y] * (float)sin( x_rot )) +
                (vehicle_menu_data.orient.up[Z] * (float)cos( x_rot ));

    vehicle_menu_data.orient.front[X] = new_front[X];
    vehicle_menu_data.orient.front[Y] = new_front[Y];
    vehicle_menu_data.orient.front[Z] = new_front[Z];

    vehicle_menu_data.orient.up[X] = new_up[X];
    vehicle_menu_data.orient.up[Y] = new_up[Y];
    vehicle_menu_data.orient.up[Z] = new_up[Z];

    /* rotate the orient around y_axis */

    new_front[X] = (vehicle_menu_data.orient.front[X] * (float)cos( y_rot )) +
                   (vehicle_menu_data.orient.front[Z] * (float)sin( y_rot ));
    new_front[Y] = vehicle_menu_data.orient.front[Y];
    new_front[Z] = (-vehicle_menu_data.orient.front[X] * (float)sin( y_rot )) +
                   (vehicle_menu_data.orient.front[Z] * (float)cos( y_rot ));

    new_up[X] = (vehicle_menu_data.orient.up[X] * (float)cos( y_rot )) +
                (vehicle_menu_data.orient.up[Z] * (float)sin( y_rot ));
    new_up[Y] = vehicle_menu_data.orient.up[Y];
    new_up[Z] = (-vehicle_menu_data.orient.up[X] * (float)sin( x_rot )) +
                (vehicle_menu_data.orient.up[Z] * (float)cos( x_rot ));

    vehicle_menu_data.orient.front[X] = new_front[X];
    vehicle_menu_data.orient.front[Y] = new_front[Y];
    vehicle_menu_data.orient.front[Z] = new_front[Z];

    vehicle_menu_data.orient.up[X] = new_up[X];
    vehicle_menu_data.orient.up[Y] = new_up[Y];
    vehicle_menu_data.orient.up[Z] = new_up[Z];

    /* rotate the orient around z_axis */

    new_front[X] = (vehicle_menu_data.orient.front[X] * (float)cos( z_rot )) -
                   (vehicle_menu_data.orient.front[Y] * (float)sin( z_rot ));
    new_front[Y] = (vehicle_menu_data.orient.front[X] * (float)sin( y_rot )) +
                   (vehicle_menu_data.orient.front[Y] * (float)cos( y_rot ));
    new_front[Z] = vehicle_menu_data.orient.front[Z];


    new_up[X] = (vehicle_menu_data.orient.up[X] * (float)cos( y_rot )) -
                (vehicle_menu_data.orient.up[Y] * (float)sin( y_rot ));
    new_up[Y] = (vehicle_menu_data.orient.up[X] * (float)sin( x_rot )) +
                (vehicle_menu_data.orient.up[Y] * (float)cos( x_rot ));
    new_up[Z] = vehicle_menu_data.orient.up[Z];

    vehicle_menu_data.orient.front[X] = new_front[X];
    vehicle_menu_data.orient.front[Y] = new_front[Y];
    vehicle_menu_data.orient.front[Z] = new_front[Z];

    vehicle_menu_data.orient.up[X] = new_up[X];
    vehicle_menu_data.orient.up[Y] = new_up[Y];
    vehicle_menu_data.orient.up[Z] = new_up[Z];

    /* orient the object */

    orient_object( pointface_ptr, &vehicle_menu_data.orient, &vehicle_menu_data.world_obj );

    /* init the view_orientation */

    view_orient.position[X] = 0.0f;
    view_orient.position[Y] = 0.0f;
    view_orient.position[Z] = -2.0f;

    view_orient.front[X] = 0.0f;
    view_orient.front[Y] = 0.0f;
    view_orient.front[Z] = 1.0f;

    view_orient.up[X] = 0.0f;
    view_orient.up[Y] = 1.0f;
    view_orient.up[Z] = 0.0f;

    light[X] = 0;
    light[Y] = 0;
    light[Z] = -2048;

    /* hack alert: because we haven't loaded the level file yet we
                   have to initialize its yon_clipping_plane field
                   so the clipping will work for super_view_object */

    level.yon_clipping_plane = -5120;

    super_view_object( &vehicle_menu_data.world_obj, &view_orient, light,
                       0, 0, 1024 );
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_custom_vehicle0_menu()
{
    string temp_str;

    custom_vehicle0_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    custom_vehicle0_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    custom_vehicle0_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    custom_vehicle0_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    custom_vehicle0_menu.return_key = (Method)custom_vehicle0_menu_return_key;
    custom_vehicle0_menu.escape_key = (Method)custom_vehicle0_menu_escape_key;
    custom_vehicle0_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.red0_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    strcpy( vehicle_menu_data.wingman_name, game_configuration.pilot_name );

    if( game_configuration.red0_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.red0_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.red0_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.red0_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.red0_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.red0_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.red0_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.red0_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );
}

void init_custom_vehicle1_menu()
{
    string temp_str;
    long character_index;

    custom_vehicle1_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    custom_vehicle1_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    custom_vehicle1_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    custom_vehicle1_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    custom_vehicle1_menu.return_key = (Method)custom_vehicle1_menu_return_key;
    custom_vehicle1_menu.escape_key = (Method)custom_vehicle1_menu_escape_key;
    custom_vehicle1_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.red1_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    character_index = ai_to_selection( game_configuration.red1_ai );

    strcpy( vehicle_menu_data.wingman_name, wingman_menu_data.info[character_index].name );

    if( game_configuration.red1_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.red1_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.red1_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.red1_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.red1_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.red1_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.red1_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.red1_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );
}

void init_custom_vehicle2_menu()
{
    string temp_str;
    long character_index;

    custom_vehicle2_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    custom_vehicle2_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    custom_vehicle2_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    custom_vehicle2_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    custom_vehicle2_menu.return_key = (Method)custom_vehicle2_menu_return_key;
    custom_vehicle2_menu.escape_key = (Method)custom_vehicle2_menu_escape_key;
    custom_vehicle2_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.red2_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    character_index = ai_to_selection( game_configuration.red2_ai );

    strcpy( vehicle_menu_data.wingman_name, wingman_menu_data.info[character_index].name );

    if( game_configuration.red2_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.red2_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.red2_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.red2_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.red2_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.red2_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.red2_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.red2_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );


}

void init_custom_vehicle3_menu()
{
    long character_index;
    string temp_str;

    custom_vehicle3_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    custom_vehicle3_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    custom_vehicle3_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    custom_vehicle3_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    custom_vehicle3_menu.return_key = (Method)custom_vehicle3_menu_return_key;
    custom_vehicle3_menu.escape_key = (Method)custom_vehicle3_menu_escape_key;
    custom_vehicle3_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.blue0_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    character_index = ai_to_selection( game_configuration.blue0_ai );

    strcpy( vehicle_menu_data.wingman_name, wingman_menu_data.info[character_index].name );

    if( game_configuration.blue0_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.blue0_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.blue0_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.blue0_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.blue0_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.blue0_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.blue0_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.blue0_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );

}

void init_custom_vehicle4_menu()
{
    string temp_str;
    long character_index;

    custom_vehicle4_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    custom_vehicle4_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    custom_vehicle4_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    custom_vehicle4_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    custom_vehicle4_menu.return_key = (Method)custom_vehicle4_menu_return_key;
    custom_vehicle4_menu.escape_key = (Method)custom_vehicle4_menu_escape_key;
    custom_vehicle4_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.blue1_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    character_index = ai_to_selection( game_configuration.blue1_ai );

    strcpy( vehicle_menu_data.wingman_name, wingman_menu_data.info[character_index].name );

    if( game_configuration.blue1_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.blue1_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.blue1_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.blue1_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.blue1_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.blue1_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.blue1_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.blue1_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );

}

void init_custom_vehicle5_menu()
{
    long character_index;
    string temp_str;

    custom_vehicle5_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    custom_vehicle5_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    custom_vehicle5_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    custom_vehicle5_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    custom_vehicle5_menu.return_key = (Method)custom_vehicle5_menu_return_key;
    custom_vehicle5_menu.escape_key = (Method)custom_vehicle5_menu_escape_key;
    custom_vehicle5_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.red2_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    character_index = ai_to_selection( game_configuration.blue2_ai );

    strcpy( vehicle_menu_data.wingman_name, wingman_menu_data.info[character_index].name );

    if( game_configuration.blue2_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.blue2_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.blue2_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.blue2_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.blue2_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.blue2_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.blue2_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.blue2_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );

}

void init_vehicle_data()
{
    vehicle_menu_data.info[Wasp].shields = 10;
    vehicle_menu_data.info[Wasp].firepower = 6;
    vehicle_menu_data.info[Wasp].air_speed = 6;
    vehicle_menu_data.info[Wasp].surface_speed = 6;
    strcpy( vehicle_menu_data.info[Wasp].special_weapon, "RAMMING" );

    vehicle_menu_data.info[Beetle].shields = 3;
    vehicle_menu_data.info[Beetle].firepower = 3;
    vehicle_menu_data.info[Beetle].air_speed = 6;
    vehicle_menu_data.info[Beetle].surface_speed = 6;
    strcpy( vehicle_menu_data.info[Beetle].special_weapon, "SCRAMBLE MISSILE" );

    vehicle_menu_data.info[Flea].shields = 3;
    vehicle_menu_data.info[Flea].firepower = 3;
    vehicle_menu_data.info[Flea].air_speed = 10;
    vehicle_menu_data.info[Flea].surface_speed = 8;
    strcpy( vehicle_menu_data.info[Flea].special_weapon, "ANTI MISSILE SYSTEM" );

    vehicle_menu_data.info[Spider].shields = 6;
    vehicle_menu_data.info[Spider].firepower = 10;
    vehicle_menu_data.info[Spider].air_speed = 3;
    vehicle_menu_data.info[Spider].surface_speed = 2;
    strcpy( vehicle_menu_data.info[Spider].special_weapon, "DOUBLE LASERS" );

    vehicle_menu_data.info[Mosquito].shields = 3;
    vehicle_menu_data.info[Mosquito].firepower = 3;
    vehicle_menu_data.info[Mosquito].air_speed = 6;
    vehicle_menu_data.info[Mosquito].surface_speed = 6;
    strcpy( vehicle_menu_data.info[Mosquito].special_weapon, "TRAITOR MISSILE" );

    vehicle_menu_data.info[Roach].shields = 6;
    vehicle_menu_data.info[Roach].firepower = 6;
    vehicle_menu_data.info[Roach].air_speed = 6;
    vehicle_menu_data.info[Roach].surface_speed = 6;
    strcpy( vehicle_menu_data.info[Roach].special_weapon, "CLOAKED MINES" );

    vehicle_menu_data.info[Dragonfly].shields = 3;
    vehicle_menu_data.info[Dragonfly].firepower = 3;
    vehicle_menu_data.info[Dragonfly].air_speed = 10;
    vehicle_menu_data.info[Dragonfly].surface_speed = 8;
    strcpy( vehicle_menu_data.info[Dragonfly].special_weapon, "CLOAKING" );

    vehicle_menu_data.info[Locust].shields = 6;
    vehicle_menu_data.info[Locust].firepower = 6;
    vehicle_menu_data.info[Locust].air_speed = 6;
    vehicle_menu_data.info[Locust].surface_speed = 6;
    strcpy( vehicle_menu_data.info[Locust].special_weapon, "HOLOGRAPHIC DECOYS" );
}



/* cylindrix_menu */



void cylindrix_menu_up_arrow()
{
    cylindrix_menu_data.current_selection = (cylindrix_menu_data.current_selection - 1) % cylindrix_menu_data.num_items;
    if( cylindrix_menu_data.current_selection < 0 ) {
        cylindrix_menu_data.current_selection = cylindrix_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void cylindrix_menu_down_arrow()
{
    cylindrix_menu_data.current_selection = (cylindrix_menu_data.current_selection + 1) % cylindrix_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void cylindrix_menu_right_arrow()
{
    ;
}

void cylindrix_menu_left_arrow()
{
    ;
}

void cylindrix_menu_return_key()
{
    if( cylindrix_menu_data.current_selection == 0 ) {       /* level1.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level1.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 1 ) {  /* level2.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level2.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 2 ) {  /* level3.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level3.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 3 ) {  /* level4.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level4.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 4 ) {  /* level5.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level5.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 5 ) {  /* level6.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level6.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 6 ) {  /* level7.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level7.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 7 ) {  /* level8.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level8.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 8 ) {  /* level9.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level9.dat" );
    }
    else if( cylindrix_menu_data.current_selection == 9 ) {  /* level10.dat */

        strcpy( game_configuration.cylinder_filename, "gamedata/level10.dat" );
    }

    make_menu_current( &custom_game_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void cylindrix_menu_escape_key()
{
    make_menu_current( &custom_game_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void cylindrix_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 60;
    const long y_start = 30;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < cylindrix_menu_data.num_items; i++ ) {

        if( i == cylindrix_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( cylindrix_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_cylindrix_menu()
{
    cylindrix_menu.up_arrow = (Method)cylindrix_menu_up_arrow;
    cylindrix_menu.down_arrow = (Method)cylindrix_menu_down_arrow;
    cylindrix_menu.right_arrow = (Method)cylindrix_menu_right_arrow;
    cylindrix_menu.left_arrow = (Method)cylindrix_menu_left_arrow;
    cylindrix_menu.return_key = (Method)cylindrix_menu_return_key;
    cylindrix_menu.escape_key = (Method)cylindrix_menu_escape_key;
    cylindrix_menu.draw_menu = (Method)cylindrix_menu_draw_menu;

    strcpy( cylindrix_menu_data.item[0], "EVERGREEN" );
    strcpy( cylindrix_menu_data.item[1], "CRYO" );
    strcpy( cylindrix_menu_data.item[2], "SLAR PIT" );
    strcpy( cylindrix_menu_data.item[3], "BIT FIELD" );
    strcpy( cylindrix_menu_data.item[4], "PERCEPTRON" );
    strcpy( cylindrix_menu_data.item[5], "VOID" );
    strcpy( cylindrix_menu_data.item[6], "ABYSS" );
    strcpy( cylindrix_menu_data.item[7], "OBELISK" );
    strcpy( cylindrix_menu_data.item[8], "PENAL TUBE" );
    strcpy( cylindrix_menu_data.item[9], "PULSE" );

    cylindrix_menu_data.num_items = 10;

    if( strcmp( game_configuration.cylinder_filename, "gamedata/level1.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 0;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level2.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 1;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level3.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 2;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level4.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 3;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level5.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 4;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level6.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 5;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level7.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 6;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level8.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 7;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level9.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 8;
    }
    else if( strcmp( game_configuration.cylinder_filename, "gamedata/level10.dat" ) == 0 ) {
        cylindrix_menu_data.current_selection = 9;
    }

}



/* funk */

void tournament_vehicle0_menu_return_key()
{
    game_configuration.red0_vehicle = vehicle_menu_data.current_selection;
    
    if( vehicle_menu_data.current_selection == Beetle ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Wasp ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }
    make_menu_current( &tournament_game_setup_menu );
    DB_Clear_Screen();    
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );

}

void tournament_vehicle1_menu_return_key()
{
    game_configuration.red1_vehicle = vehicle_menu_data.current_selection;
    
    if( vehicle_menu_data.current_selection == Beetle ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
        while( !Is_Voice_Done() );
    }

    else if( vehicle_menu_data.current_selection == Wasp ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }
    init_tournament_vehicle2_menu();
    make_menu_current( &tournament_vehicle2_menu );

}

void tournament_vehicle2_menu_return_key()
{
    game_configuration.red2_vehicle = vehicle_menu_data.current_selection;
    
    if( vehicle_menu_data.current_selection == Beetle ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.beetle_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Wasp ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.wasp_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Flea ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.flea_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Roach ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.roach_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Spider ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.spider_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Locust ) {
		Play_Voice( menu_stuff.vehicle_name_sounds.locust_sound );
		while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Dragonfly ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.dragonfly_sound );
        while( !Is_Voice_Done() );
    }
    else if( vehicle_menu_data.current_selection == Mosquito ) {
        Play_Voice( menu_stuff.vehicle_name_sounds.mosquito_sound );
        while( !Is_Voice_Done() );
    }    
    make_menu_current( &tournament_game_setup_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );

}


void tournament_vehicle0_menu_escape_key()
{

    /* go back to the game_menu */

    make_menu_current( &tournament_game_setup_menu );
    DB_Clear_Screen();    
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void tournament_vehicle1_menu_escape_key()
{
    make_menu_current( &tournament_game_setup_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void tournament_vehicle2_menu_escape_key()
{
    init_tournament_vehicle1_menu();
    make_menu_current( &tournament_vehicle1_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}


void init_tournament_vehicle0_menu()
{
    string temp_str;

    tournament_vehicle0_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    tournament_vehicle0_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    tournament_vehicle0_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    tournament_vehicle0_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    tournament_vehicle0_menu.return_key = (Method)tournament_vehicle0_menu_return_key;
    tournament_vehicle0_menu.escape_key = (Method)tournament_vehicle0_menu_escape_key;
    tournament_vehicle0_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.red0_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    strcpy( vehicle_menu_data.wingman_name, game_configuration.pilot_name );

    if( game_configuration.red0_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.red0_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.red0_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.red0_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.red0_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.red0_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.red0_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.red0_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );
}

void init_tournament_vehicle1_menu()
{
    string temp_str;
    long character_index;

    tournament_vehicle1_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    tournament_vehicle1_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    tournament_vehicle1_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    tournament_vehicle1_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    tournament_vehicle1_menu.return_key = (Method)tournament_vehicle1_menu_return_key;
    tournament_vehicle1_menu.escape_key = (Method)tournament_vehicle1_menu_escape_key;
    tournament_vehicle1_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.red1_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    character_index = ai_to_selection( game_configuration.red1_ai );

    strcpy( vehicle_menu_data.wingman_name, wingman_menu_data.info[character_index].name );

    if( game_configuration.red1_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.red1_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.red1_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.red1_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.red1_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.red1_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.red1_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.red1_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );
}

void init_tournament_vehicle2_menu()
{
    string temp_str;
    long character_index;

    tournament_vehicle2_menu.up_arrow = (Method)vehicle_menu_up_arrow;
    tournament_vehicle2_menu.down_arrow = (Method)vehicle_menu_down_arrow;
    tournament_vehicle2_menu.right_arrow = (Method)vehicle_menu_right_arrow;
    tournament_vehicle2_menu.left_arrow = (Method)vehicle_menu_left_arrow;
    tournament_vehicle2_menu.return_key = (Method)tournament_vehicle2_menu_return_key;
    tournament_vehicle2_menu.escape_key = (Method)tournament_vehicle2_menu_escape_key;
    tournament_vehicle2_menu.draw_menu = (Method)vehicle_menu_draw_menu;

    vehicle_menu_data.current_selection = game_configuration.red2_vehicle;
    strcpy( vehicle_menu_data.menu_title, "SELECT VEHICLE" );
    vehicle_menu_data.num_items = 8;

    vehicle_menu_data.world_obj.point = NULL;
    vehicle_menu_data.world_obj.points = 0;
    vehicle_menu_data.world_obj.face = NULL;
    vehicle_menu_data.world_obj.faces = 0;

    vehicle_menu_data.orient.front[X] = 0.0f;
    vehicle_menu_data.orient.front[Y] = 0.0f;
    vehicle_menu_data.orient.front[Z] = -1.0f;

    vehicle_menu_data.orient.up[X] = 0.0f;
    vehicle_menu_data.orient.up[Y] = 1.0f;
    vehicle_menu_data.orient.up[Z] = 0.0f;

    vehicle_menu_data.orient.position[X] = 0.7f;
    vehicle_menu_data.orient.position[Y] = 0.3f;
    vehicle_menu_data.orient.position[Z] = -0.7f;

    character_index = ai_to_selection( game_configuration.red2_ai );

    strcpy( vehicle_menu_data.wingman_name, wingman_menu_data.info[character_index].name );

    if( game_configuration.red2_vehicle == Wasp ) {
        strcpy( temp_str, "WASP" );
    }
    else if( game_configuration.red2_vehicle == Beetle ) {
        strcpy( temp_str, "BEETLE" );
    }
    else if( game_configuration.red2_vehicle == Flea ) {
        strcpy( temp_str, "FLEA" );
    }
    else if( game_configuration.red2_vehicle == Mosquito ) {
        strcpy( temp_str, "MOSQUITO" );
    }
    else if( game_configuration.red2_vehicle == Spider ) {
        strcpy( temp_str, "SPIDER" );
    }
    else if( game_configuration.red2_vehicle == Roach ) {
        strcpy( temp_str, "ROACH" );
    }
    else if( game_configuration.red2_vehicle == Locust ) {
        strcpy( temp_str, "LOCUST" );
    }
    else if( game_configuration.red2_vehicle == Dragonfly ) {
        strcpy( temp_str, "DRAGONFLY" );
    }

    strcpy( vehicle_menu_data.preferred_vehicle_name, temp_str );


}


void tournament_wingman_menu_up_arrow()
{
    wingman_menu_data.current_row--;

    if( wingman_menu_data.current_row < 0 ) {
        wingman_menu_data.current_row = 0;
        Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
    }
    else {
        wingman_menu_data.palette_active = FALSE;
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }

    /* special case for Cap'n Hardgeus */

    if( wingman_menu_data.current_row == 3 ) {
        wingman_menu_data.current_col = 1;
    }    
}

void tournament_wingman_menu_down_arrow()
{
    wingman_menu_data.current_row++;

    if( wingman_menu_data.current_row >= 4 ) {
        wingman_menu_data.current_row = 3;
        Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
    }
    else {
        wingman_menu_data.palette_active = FALSE;
        Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
    }

    /* special case for Cap'n Hardgeus */

    if( wingman_menu_data.current_row == 3 ) {
        wingman_menu_data.current_col = 1;
    }    
}

void tournament_wingman_menu_right_arrow()
{
    wingman_menu_data.current_col++;

    if( wingman_menu_data.current_col > 2 ) {
        wingman_menu_data.current_col = 2;
        Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
    }
    else {

        /* special case for Cap'n Hardgeus */

        if( wingman_menu_data.current_row == 3 ) {
            wingman_menu_data.current_col = 1;
            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {
            Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
        }
    }
}

void tournament_wingman_menu_left_arrow()
{
    wingman_menu_data.current_col--;

    if( wingman_menu_data.current_col < 0 ) {
        wingman_menu_data.current_col = 0;
        Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
    }
    else {

        /* special case for Cap'n Hardgeus */

        if( wingman_menu_data.current_row == 3 ) {
            wingman_menu_data.current_col = 1;
            Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
        }
        else {
            Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
        }
    }
}



void tournament_wingman1_menu_return_key()
{
    long current_selection;
    
    current_selection = (wingman_menu_data.current_row * 3) + wingman_menu_data.current_col;
    
    /* play sample */

    Play_Voice( wingman_menu_data.info[current_selection].sample );
    while( !Is_Voice_Done() );        

    game_configuration.red1_ai = selection_to_ai( current_selection );
    game_configuration.red1_vehicle = wingman_menu_data.info[current_selection].preferred_vehicle;

    /* move to select wingman1's vehicle */

    init_tournament_wingman2_menu();
    make_menu_current( &tournament_wingman2_menu );
    
    /* play wingman catch phrase */
}


void tournament_wingman2_menu_return_key()
{
    long current_selection;

    current_selection = (wingman_menu_data.current_row * 3) + wingman_menu_data.current_col;
        
    /* play sample */
    
    Play_Voice( wingman_menu_data.info[current_selection].sample );
    while( !Is_Voice_Done() );


    game_configuration.red2_ai = selection_to_ai( current_selection );
    game_configuration.red2_vehicle = wingman_menu_data.info[current_selection].preferred_vehicle;

    /* move to select wingman2's vehicle */

    make_menu_current( &tournament_game_setup_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
}


void tournament_wingman1_menu_escape_key()
{
    make_menu_current( &tournament_game_setup_menu );
    DB_Clear_Screen();
    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
    Enable_Color_Palette( &menu_stuff.general_menu_background );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void tournament_wingman2_menu_escape_key()
{
    init_tournament_wingman1_menu();
    make_menu_current( &tournament_wingman1_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}


void init_tournament_wingman1_menu()
{
    long selection;

    tournament_wingman1_menu.up_arrow = (Method)tournament_wingman_menu_up_arrow;
    tournament_wingman1_menu.down_arrow = (Method)tournament_wingman_menu_down_arrow;
    tournament_wingman1_menu.right_arrow = (Method)tournament_wingman_menu_right_arrow;
    tournament_wingman1_menu.left_arrow = (Method)tournament_wingman_menu_left_arrow;
    tournament_wingman1_menu.return_key = (Method)tournament_wingman1_menu_return_key;
    tournament_wingman1_menu.escape_key = (Method)tournament_wingman1_menu_escape_key;
    tournament_wingman1_menu.draw_menu = (Method)wingman_menu_draw_menu;

    wingman_menu_data.num_items = 39;

    wingman_menu_data.wingman_index = 1;

    strcpy( wingman_menu_data.menu_title, "SELECT WINGMAN 1" );

    selection = ai_to_selection( game_configuration.red1_ai );
    
    /* make sure that the selection is one of the humans */
    
    if( selection > 11 ) {
        selection = 0;
    }
    else if( selection == 11 ) {
        selection = 10;
    }
    else if( selection == 9 ) {
        selection = 10;
    }        

    wingman_menu_data.current_row = selection / 3;
    wingman_menu_data.current_col = selection % 3;

    wingman_menu_data.palette_active = FALSE;
}

void init_tournament_wingman2_menu()
{
    long selection;

    tournament_wingman2_menu.up_arrow = (Method)tournament_wingman_menu_up_arrow;
    tournament_wingman2_menu.down_arrow = (Method)tournament_wingman_menu_down_arrow;
    tournament_wingman2_menu.right_arrow = (Method)tournament_wingman_menu_right_arrow;
    tournament_wingman2_menu.left_arrow = (Method)tournament_wingman_menu_left_arrow;
    tournament_wingman2_menu.return_key = (Method)tournament_wingman2_menu_return_key;
    tournament_wingman2_menu.escape_key = (Method)tournament_wingman2_menu_escape_key;
    tournament_wingman2_menu.draw_menu = (Method)wingman_menu_draw_menu;

    wingman_menu_data.num_items = 39;

    wingman_menu_data.wingman_index = 1;

    strcpy( wingman_menu_data.menu_title, "SELECT WINGMAN 2" );

    selection = ai_to_selection( game_configuration.red2_ai );
    
    /* make sure that the selection is one of the humans */
    
    if( selection > 11 ) {
        selection = 0;
    }
    else if( selection == 11 ) {
        selection = 10;
    }
    else if( selection == 9 ) {
        selection = 10;
    }        


    wingman_menu_data.current_row = selection / 3;
    wingman_menu_data.current_col = selection % 3;

    wingman_menu_data.palette_active = FALSE;
}




/* methods for tournament_game_setup_menu */




void tournament_game_setup_menu_up_arrow()
{
    tournament_game_setup_menu_data.current_selection = (tournament_game_setup_menu_data.current_selection - 1) % tournament_game_setup_menu_data.num_items;
    if( tournament_game_setup_menu_data.current_selection < 0 ) {
        tournament_game_setup_menu_data.current_selection = tournament_game_setup_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void tournament_game_setup_menu_down_arrow()
{
    tournament_game_setup_menu_data.current_selection = (tournament_game_setup_menu_data.current_selection + 1) % tournament_game_setup_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void tournament_game_setup_menu_right_arrow()
{
    ;
}

void tournament_game_setup_menu_left_arrow()
{
    ;
}

void tournament_game_setup_menu_return_key()
{

    if( tournament_game_setup_menu_data.current_selection == 0 ) { /* start tournament */

        exit_event_loop = TRUE;
        
        if( (game_configuration.red1_ai >= 0) && (game_configuration.red1_ai <= 10) ) {
            ;
        }
        else {
            game_configuration.red1_ai = 9;
            game_configuration.red1_vehicle = Flea;
        }
        
        if( (game_configuration.red2_ai >= 0) && (game_configuration.red2_ai <= 10) ) {
            ;
        }
        else {
            game_configuration.red2_ai = 9;
            game_configuration.red2_vehicle = Flea;
        }

    }
    else if( tournament_game_setup_menu_data.current_selection == 1 ) { /* select your vehicle */
        
        init_tournament_vehicle0_menu();
        make_menu_current( &tournament_vehicle0_menu );
        DB_Clear_Screen();        
        Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
        Enable_Color_Palette( &menu_stuff.vehicle_menu_background );

    }
    else if( tournament_game_setup_menu_data.current_selection == 2 ) { /* select wingmen */
        
        init_tournament_wingman1_menu();
        make_menu_current( &tournament_wingman1_menu );

    }
    else if( tournament_game_setup_menu_data.current_selection == 3 ) { /* select wingmen's vehicles */

        init_tournament_vehicle1_menu();
        make_menu_current( &tournament_vehicle1_menu );
        DB_Clear_Screen();        
        Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
        Enable_Color_Palette( &menu_stuff.vehicle_menu_background );        

    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

}

void tournament_game_setup_menu_escape_key()
{
    make_menu_current( &game_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );

}

void tournament_game_setup_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 25;
    const long y_start = 55;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < tournament_game_setup_menu_data.num_items; i++ ) {

        if( i == tournament_game_setup_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( tournament_game_setup_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_tournament_game_setup_menu()
{
    tournament_game_setup_menu.up_arrow = (Method)tournament_game_setup_menu_up_arrow;
    tournament_game_setup_menu.down_arrow = (Method)tournament_game_setup_menu_down_arrow;
    tournament_game_setup_menu.right_arrow = (Method)tournament_game_setup_menu_right_arrow;
    tournament_game_setup_menu.left_arrow = (Method)tournament_game_setup_menu_left_arrow;
    tournament_game_setup_menu.return_key = (Method)tournament_game_setup_menu_return_key;
    tournament_game_setup_menu.escape_key = (Method)tournament_game_setup_menu_escape_key;
    tournament_game_setup_menu.draw_menu = (Method)tournament_game_setup_menu_draw_menu;    

    strcpy( tournament_game_setup_menu_data.item[0], "START TOURNAMENT GAME" );
    strcpy( tournament_game_setup_menu_data.item[1], "SELECT YOUR VEHICLE" );
    strcpy( tournament_game_setup_menu_data.item[2], "SELECT WINGMEN" );
    strcpy( tournament_game_setup_menu_data.item[3], "SELECT WINGMEN'S VEHICLES" );    

    tournament_game_setup_menu_data.num_items = 4;

    tournament_game_setup_menu_data.current_selection = 0;
}




/* methods for view_stats_menu */




void view_stats_menu_up_arrow()
{
    view_stats_menu_data.current_selection = (view_stats_menu_data.current_selection - 1) % view_stats_menu_data.num_items;
    if( view_stats_menu_data.current_selection < 0 ) {
        view_stats_menu_data.current_selection = view_stats_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void view_stats_menu_down_arrow()
{
    view_stats_menu_data.current_selection = (view_stats_menu_data.current_selection + 1) % view_stats_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void view_stats_menu_right_arrow()
{
    ;
}

void view_stats_menu_left_arrow()
{
    ;
}

void view_stats_menu_return_key()
{
    char filename[80];
    int i;
    FILE *fp;

    if( view_stats_menu_data.current_selection == 0 ) { /* output stats */
    
        /* find the next available filename */
        
        for( i = 0; i < 1000; i++ ) {
        
            sprintf( filename, "%sSTATS/OVER%03d.TXT", g_DataPath, i );
            
            if( (fp = fopen( filename, "r" ) ) == NULL ) {
            
                /* if this filename doesn't exist create it */

                Save_Overall_Stats_Text( &overall_stats, filename );
                
                break;
            }
            else {
                fclose( fp );
            }
        }
        
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        
        Pop_Buffer( menu_stuff.general_menu_background.buffer );
        
        string_blit( "STATS SAVED AS", 90, 65,                      
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
                     
        string_blit( filename, 75, 85,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
                     
        micro_string_blit( "PRESS ANY KEY TO CONTINUE", 10, 190,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                                   
        Swap_Buffer();
#ifdef GLCYLINDRIX
		GL_SubloadAndSwapBuffer();
#endif
        
        while( !Jon_Kbhit() );
        
        make_menu_current( &main_menu );

    }
    else if( view_stats_menu_data.current_selection == 1 ) { /* return to main */

        make_menu_current( &main_menu );

    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void view_stats_menu_escape_key()
{
    make_menu_current( &main_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void view_stats_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 5;
    const long y_start = 170;
    const long y_spacing = 15;
    const long stat_x_start = 200;
    const long stat_y_start = 35;
    const long stat_y_spacing = 9;
    long x, y;
    char str[80];

    Pop_Buffer( menu_stuff.general_menu_background.buffer );
    
    /* draw the title */
    
    string_blit( "OVERALL STATS", 85, 5,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    x = x_start;
    y = y_start;

    for( i = 0; i < view_stats_menu_data.num_items; i++ ) {

        if( i == view_stats_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( view_stats_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }    
    
    x = stat_x_start;
    y = stat_y_start;
    
    /* draw the pilot name */
    
    micro_string_blit( "PILOT:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    micro_string_blit( overall_stats.name, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
    
    /* draw the number of games */
    
    micro_string_blit( "TOTAL GAMES:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.number_of_games );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
    
    /* draw the victories */
    
    micro_string_blit( "VICTORIES:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.victories );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;

    /* draw the defeats */
    
    micro_string_blit( "DEFEATS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.defeats );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
    
    /* win_percentage */
    
    micro_string_blit( "WIN PERCENTAGE:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );

    sprintf( str, "%.3f", 100.0 * overall_stats.win_percentage );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                       
    /* draw the number of kills */
    
    micro_string_blit( "KILLS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.kills );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    y += stat_y_spacing;                       
    
    /* number of deaths */
    
    micro_string_blit( "DEATHS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.times_killed );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                       
    /* number of shots fired */
    
    micro_string_blit( "TOTAL SHOTS FIRED:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.shots_fired );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    y += stat_y_spacing;                       
    
    /* enemy hits */
    
    micro_string_blit( "ENEMY HITS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.enemy_hits );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );    
                       
    y += stat_y_spacing;
    
    /* friendly hits */
    
    micro_string_blit( "FREINDLY HITS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.friendly_hits );                    
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                               
    /* misses */

    micro_string_blit( "MISSED SHOTS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.misses );
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;                       
    
    /* hit miss ratio */
    
    micro_string_blit( "HIT PERCENTAGE:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%.3f", 100.0 * overall_stats.hit_percentage );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;                       
    
    /* pylons grabbed */
    
    micro_string_blit( "TOTAL PYLONS GRABBED:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.pylons_grabbed );
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    
    y += stat_y_spacing;
                       
    /* times hit by enemy */
                       
    micro_string_blit( "ENEMY HITS TAKEN:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)overall_stats.times_hit );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_view_stats_menu()
{
    view_stats_menu.up_arrow = (Method)view_stats_menu_up_arrow;
    view_stats_menu.down_arrow = (Method)view_stats_menu_down_arrow;
    view_stats_menu.right_arrow = (Method)view_stats_menu_right_arrow;
    view_stats_menu.left_arrow = (Method)view_stats_menu_left_arrow;
    view_stats_menu.return_key = (Method)view_stats_menu_return_key;
    view_stats_menu.escape_key = (Method)view_stats_menu_escape_key;
    view_stats_menu.draw_menu = (Method)view_stats_menu_draw_menu;

    strcpy( view_stats_menu_data.item[0], "OUTPUT STATS" );
    strcpy( view_stats_menu_data.item[1], "RETURN TO MAIN MENU" );

    view_stats_menu_data.num_items = 2;

    view_stats_menu_data.current_selection = 1;
}




void init_all_main_menus()
{
    /* init all the menus : the indenting shows the menu structure */
    
    init_pilot_selection_menu();
        init_pilot_delete_menu();
        init_pilot_input_menu();
    
    init_main_menu();
        init_game_menu();
            /* tournament_game */
            init_tournament_game_setup_menu();
            init_custom_game_menu();  /* custom game */
                init_cylindrix_menu();
        init_options_menu();
            init_sound_menu();
            init_detail_menu();
            init_keyboard_menu();
            init_joystick_menu();
                init_calibrate_menu();
                init_joystick_config_menu();
        /* view_stats */
        init_view_stats_menu();
        /* play_intro */

    /* set the current_menu to be main_menu */

    make_menu_current( &pilot_selection_menu );
}

void menu_event_init()
{
    init_wingman_data();  // initalizes all the characters names & descriptions
    init_vehicle_data();  // initalizes all the vehicles characteristics
}

int menu_event_loop()
{
    exit_event_loop = FALSE;

	// draw the current menu.
    (*current_menu.draw_menu)();

	if ( !current_menu.delay_input )
	{
		if ( SYS_KeyPressed( KEY_UP_ARROW ) )
		{
			(*current_menu.up_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_DOWN_ARROW ) ) 
		{
			(*current_menu.down_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_RIGHT_ARROW ) ) {
			(*current_menu.right_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_LEFT_ARROW ) ) 
		{
			(*current_menu.left_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_ENTER ) ) 
		{
			(*current_menu.return_key)();
		}
		else if ( SYS_KeyPressed( KEY_ESC ) ) 
		{
			(*current_menu.escape_key)();
		}
	}

	current_menu.delay_input = 0;

	if ( program_over )
		return 2;
	else
		return exit_event_loop;
}



/* methods for during_game_menu */




void during_game_menu_up_arrow()
{
    during_game_menu_data.current_selection = (during_game_menu_data.current_selection - 1) % during_game_menu_data.num_items;
    if( during_game_menu_data.current_selection < 0 ) {
        during_game_menu_data.current_selection = during_game_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_menu_down_arrow()
{
    during_game_menu_data.current_selection = (during_game_menu_data.current_selection + 1) % during_game_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_menu_right_arrow()
{
    ;
}

void during_game_menu_left_arrow()
{
    ;
}

void during_game_menu_return_key()
{

    if( during_game_menu_data.current_selection == 0 ) { /* resume game */        

        exit_event_loop = TRUE;

    }
    else if( during_game_menu_data.current_selection == 1 ) { /* options */

        make_menu_current( &during_game_options_menu );

    }
    else if( during_game_menu_data.current_selection == 2 ) { /* quit game */

        exit_event_loop = TRUE;
        quit_game = TRUE;
        
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void during_game_menu_escape_key()
{
    exit_event_loop = TRUE;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void during_game_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 75;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < during_game_menu_data.num_items; i++ ) {

        if( i == during_game_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( during_game_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_during_game_menu()
{
    during_game_menu.up_arrow = (Method)during_game_menu_up_arrow;
    during_game_menu.down_arrow = (Method)during_game_menu_down_arrow;
    during_game_menu.right_arrow = (Method)during_game_menu_right_arrow;
    during_game_menu.left_arrow = (Method)during_game_menu_left_arrow;
    during_game_menu.return_key = (Method)during_game_menu_return_key;
    during_game_menu.escape_key = (Method)during_game_menu_escape_key;
    during_game_menu.draw_menu = (Method)during_game_menu_draw_menu;

    strcpy( during_game_menu_data.item[0], "RESUME GAME" );
    strcpy( during_game_menu_data.item[1], "OPTIONS" );
    strcpy( during_game_menu_data.item[2], "QUIT GAME" );

    during_game_menu_data.num_items = 3;

    during_game_menu_data.current_selection = 0;
}




/* methods for during_game_options_menu */




void during_game_options_menu_up_arrow()
{
    during_game_options_menu_data.current_selection = (during_game_options_menu_data.current_selection - 1) % during_game_options_menu_data.num_items;
    if( during_game_options_menu_data.current_selection < 0 ) {
        during_game_options_menu_data.current_selection = during_game_options_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_options_menu_down_arrow()
{
    during_game_options_menu_data.current_selection = (during_game_options_menu_data.current_selection + 1) % during_game_options_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_options_menu_right_arrow()
{
    ;
}

void during_game_options_menu_left_arrow()
{
    ;
}

void during_game_options_menu_return_key()
{

    if( during_game_options_menu_data.current_selection == 0 ) { /* sound */

        make_menu_current( &during_game_sound_menu );

    }
    else if( during_game_options_menu_data.current_selection == 1 ) { /* detail during_game_options_menu */

        make_menu_current( &during_game_detail_menu );

    }
    else if( during_game_options_menu_data.current_selection == 2 ) { /* keyboard */

        make_menu_current( &during_game_keyboard_menu );

    }
    else if( during_game_options_menu_data.current_selection == 3 ) { /* joystick */

        make_menu_current( &during_game_joystick_menu );

    }
    else if( during_game_options_menu_data.current_selection == 4 ) { /* animations: */
    
        game_configuration.animations_on = !game_configuration.animations_on;
    
        if( game_configuration.animations_on ) {
            strcpy( during_game_options_menu_data.item[4], "ANIMATIONS: ON" );
        }
        else {
            strcpy( during_game_options_menu_data.item[4], "ANIMATIONS: OFF" );
        }
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void during_game_options_menu_escape_key()
{
    make_menu_current( &during_game_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void during_game_options_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 75;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < during_game_options_menu_data.num_items; i++ ) {

        if( i == during_game_options_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( during_game_options_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_during_game_options_menu()
{
    during_game_options_menu.up_arrow = (Method)during_game_options_menu_up_arrow;
    during_game_options_menu.down_arrow = (Method)during_game_options_menu_down_arrow;
    during_game_options_menu.right_arrow = (Method)during_game_options_menu_right_arrow;
    during_game_options_menu.left_arrow = (Method)during_game_options_menu_left_arrow;
    during_game_options_menu.return_key = (Method)during_game_options_menu_return_key;
    during_game_options_menu.escape_key = (Method)during_game_options_menu_escape_key;
    during_game_options_menu.draw_menu = (Method)during_game_options_menu_draw_menu;

    strcpy( during_game_options_menu_data.item[0], "SOUND" );
    strcpy( during_game_options_menu_data.item[1], "DETAIL OPTIONS" );
    strcpy( during_game_options_menu_data.item[2], "KEYBOARD" );
    strcpy( during_game_options_menu_data.item[3], "JOYSTICK" );
    if( game_configuration.animations_on ) {
        strcpy( during_game_options_menu_data.item[4], "ANIMATIONS: ON" );
    }
    else {
        strcpy( during_game_options_menu_data.item[4], "ANIMATIONS: OFF" );
    }

    during_game_options_menu_data.num_items = 5;

    during_game_options_menu_data.current_selection = 0;
}




/* methods for during_game_detail_menu */




void during_game_detail_menu_up_arrow()
{
    during_game_detail_menu_data.current_selection = (during_game_detail_menu_data.current_selection - 1) % during_game_detail_menu_data.num_items;
    if( during_game_detail_menu_data.current_selection < 0 ) {
        during_game_detail_menu_data.current_selection = during_game_detail_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_detail_menu_down_arrow()
{
    during_game_detail_menu_data.current_selection = (during_game_detail_menu_data.current_selection + 1) % during_game_detail_menu_data.num_items;
}

void during_game_detail_menu_right_arrow()
{
    ;
}

void during_game_detail_menu_left_arrow()
{
    ;
}

void during_game_detail_menu_return_key()
{

    if( during_game_detail_menu_data.current_selection == 0 ) { /* tube detail: */

        if( game_configuration.tube_detail_level == HIGH_DETAIL ) {
            game_configuration.tube_detail_level = LOW_DETAIL;
            strcpy( during_game_detail_menu_data.item[0], "TUBE DETAIL: LOW" );
        }
        else if( game_configuration.tube_detail_level == MEDIUM_DETAIL ) {
            game_configuration.tube_detail_level = HIGH_DETAIL;
            strcpy( during_game_detail_menu_data.item[0], "TUBE DETAIL: HIGH" );
        }
        else if( game_configuration.tube_detail_level == LOW_DETAIL ) {
            game_configuration.tube_detail_level = MEDIUM_DETAIL;
            strcpy( during_game_detail_menu_data.item[0], "TUBE DETAIL: MEDIUM" );
        }
        else {
            game_configuration.tube_detail_level = LOW_DETAIL;
            strcpy( during_game_detail_menu_data.item[0], "TUBE DETAIL: LOW" );
        }

    }
    else if( during_game_detail_menu_data.current_selection == 1 ) { /* world detail: */

        if( game_configuration.world_detail_level == HIGH_DETAIL ) {
            game_configuration.world_detail_level = LOW_DETAIL;
            strcpy( during_game_detail_menu_data.item[1], "WORLD DETAIL: LOW" );
        }
        else if( game_configuration.world_detail_level == LOW_DETAIL ) {
            game_configuration.world_detail_level = HIGH_DETAIL;
            strcpy( during_game_detail_menu_data.item[1], "WORLD DETAIL: HIGH" );
        }
        else {
            game_configuration.world_detail_level = LOW_DETAIL;
            strcpy( during_game_detail_menu_data.item[1], "WORLD DETAIL: LOW" );
        }

    }
    else if( during_game_detail_menu_data.current_selection == 2 ) { /* hubs: */

        if( game_configuration.hubs_on == TRUE ) {
            game_configuration.hubs_on = FALSE;
            strcpy( during_game_detail_menu_data.item[2], "HUBS: OFF" );
        }
        else {
            game_configuration.hubs_on = TRUE;
            strcpy( during_game_detail_menu_data.item[2], "HUBS: ON" );
        }

    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_detail_menu_escape_key()
{
    make_menu_current( &during_game_options_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void during_game_detail_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 75;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < during_game_detail_menu_data.num_items; i++ ) {

        if( i == during_game_detail_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( during_game_detail_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_during_game_detail_menu()
{
    during_game_detail_menu.up_arrow = (Method)during_game_detail_menu_up_arrow;
    during_game_detail_menu.down_arrow = (Method)during_game_detail_menu_down_arrow;
    during_game_detail_menu.right_arrow = (Method)during_game_detail_menu_right_arrow;
    during_game_detail_menu.left_arrow = (Method)during_game_detail_menu_left_arrow;
    during_game_detail_menu.return_key = (Method)during_game_detail_menu_return_key;
    during_game_detail_menu.escape_key = (Method)during_game_detail_menu_escape_key;
    during_game_detail_menu.draw_menu = (Method)during_game_detail_menu_draw_menu;

    if( game_configuration.tube_detail_level == HIGH_DETAIL ) {
        strcpy( during_game_detail_menu_data.item[0], "TUBE DETAIL: HIGH" );
    }
    else if( game_configuration.tube_detail_level == MEDIUM_DETAIL ) {
        strcpy( during_game_detail_menu_data.item[0], "TUBE DETAIL: MEDIUM" );
    }
    else if( game_configuration.tube_detail_level == LOW_DETAIL ) {
        strcpy( during_game_detail_menu_data.item[0], "TUBE DETAIL: LOW" );
    }
    else {
        strcpy( during_game_detail_menu_data.item[0], "TUBE DETAIL: ????" );
    }

    if( game_configuration.world_detail_level == HIGH_DETAIL ) {
        strcpy( during_game_detail_menu_data.item[1], "WORLD DETAIL: HIGH" );
    }
    else if( game_configuration.world_detail_level == LOW_DETAIL ) {
        strcpy( during_game_detail_menu_data.item[1], "WORLD DETAIL: LOW" );
    }
    else {
        strcpy( during_game_detail_menu_data.item[1], "WORLD DETAIL: ????" );
    }

    if( game_configuration.hubs_on == TRUE ) {
        strcpy( during_game_detail_menu_data.item[2], "HUBS: ON" );
    }
    else {
        strcpy( during_game_detail_menu_data.item[2], "HUBS: OFF" );
    }

    during_game_detail_menu_data.num_items = 3;

    during_game_detail_menu_data.current_selection = 0;
}




/* methods for during_game_sound_menu */




void during_game_sound_menu_up_arrow()
{
    during_game_sound_menu_data.current_selection = (during_game_sound_menu_data.current_selection - 1) % during_game_sound_menu_data.num_items;
    if( during_game_sound_menu_data.current_selection < 0 ) {
        during_game_sound_menu_data.current_selection = during_game_sound_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_sound_menu_down_arrow()
{
    during_game_sound_menu_data.current_selection = (during_game_sound_menu_data.current_selection + 1) % during_game_sound_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_sound_menu_right_arrow()
{
    if( during_game_sound_menu_data.current_selection == 0 ) { /* cd volume.... */
    
        if( game_configuration.music_vol >= 225 ) {
            game_configuration.music_vol = 255;
        }
        else {
            if( game_configuration.music_vol == 224 ) {
                game_configuration.music_vol += 31;
            }
            else {
                game_configuration.music_vol += 32;
            }
        }

        Set_Cd_Volume( game_configuration.music_vol );        
        
    }
    else if( during_game_sound_menu_data.current_selection == 1 ) { /* sound fx volume.. */
    
        if( game_configuration.sound_vol >= 225 ) {
            game_configuration.sound_vol = 255;
        }
        else {
            if( game_configuration.sound_vol == 224 ) {
                game_configuration.sound_vol += 31;
            }
            else {
                game_configuration.sound_vol += 32;
            }
        }
        
    }
    else if( during_game_sound_menu_data.current_selection == 2 ) { /* voice volume... */
    
        if( game_configuration.voice_vol >= 225 ) {
            game_configuration.voice_vol = 255;
        }
        else {
            if( game_configuration.voice_vol == 224 ) {
                game_configuration.voice_vol += 31;
            }
            else {
                game_configuration.voice_vol += 32;
            }
        }        
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void during_game_sound_menu_left_arrow()
{
    if( during_game_sound_menu_data.current_selection == 0 ) { /* cd volume.... */
        if( game_configuration.music_vol <= 32 ) {
            game_configuration.music_vol = 0;            
        }
        else {
            game_configuration.music_vol -= 32;
        }
        
        Set_Cd_Volume( game_configuration.music_vol );
        
    }
    else if( during_game_sound_menu_data.current_selection == 1 ) { /* sound fx volume.. */
        if( game_configuration.sound_vol <= 32 ) {
            game_configuration.sound_vol = 0;            
        }
        else {
            game_configuration.sound_vol -= 32;
        }

    }
    else if( during_game_sound_menu_data.current_selection == 2 ) { /* voice volume... */
        if( game_configuration.voice_vol <= 32 ) {
            game_configuration.voice_vol = 0;            
        }
        else {
            game_configuration.voice_vol -= 32;
        }

    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_sound_menu_return_key()
{
    if( during_game_sound_menu_data.current_selection == 0 ) { /* cd volume.... */
        ;
    }
    else if( during_game_sound_menu_data.current_selection == 1 ) { /* sound fx volume.. */
        ;
    }
    else if( during_game_sound_menu_data.current_selection == 2 ) { /* voice volume... */
        ;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );

}

void during_game_sound_menu_escape_key()
{
    make_menu_current( &during_game_options_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void during_game_sound_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 75;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;
    BarInfo bar_info;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < during_game_sound_menu_data.num_items; i++ ) {

        if( i == during_game_sound_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( during_game_sound_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        bar_info.upper_left[X] = x_start + 150;
        bar_info.upper_left[Y] = y + 1;
        bar_info.lower_right[X] = x_start + 200;
        bar_info.lower_right[Y] = y + 10;
        bar_info.min_val = 0;
        bar_info.max_val = 255;

        if( i == 0 ) {
            bar_info.val = game_configuration.music_vol;
        }
        else if( i == 1 ) {
            bar_info.val = game_configuration.sound_vol;
        }
        else if( i == 2 ) {
            bar_info.val = game_configuration.voice_vol;
        }

        bar_info.bar_color = color;
        bar_info.outline_color = 1;
        bar_info.type = HorizontalLeftJustified;

        draw_bar( &bar_info );

        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_during_game_sound_menu()
{
    during_game_sound_menu.up_arrow = (Method)during_game_sound_menu_up_arrow;
    during_game_sound_menu.down_arrow = (Method)during_game_sound_menu_down_arrow;
    during_game_sound_menu.right_arrow = (Method)during_game_sound_menu_right_arrow;
    during_game_sound_menu.left_arrow = (Method)during_game_sound_menu_left_arrow;
    during_game_sound_menu.return_key = (Method)during_game_sound_menu_return_key;
    during_game_sound_menu.escape_key = (Method)during_game_sound_menu_escape_key;
    during_game_sound_menu.draw_menu = (Method)during_game_sound_menu_draw_menu;

    strcpy( during_game_sound_menu_data.item[0], "CD VOLUME :" );
    strcpy( during_game_sound_menu_data.item[1], "FX VOLUME :" );
    strcpy( during_game_sound_menu_data.item[2], "VOICE VOLUME :" );

    during_game_sound_menu_data.num_items = 3;

    during_game_sound_menu_data.current_selection = 0;
}




/* methods for during_game_keyboard_menu */




void during_game_keyboard_menu_up_arrow()
{
    during_game_keyboard_menu_data.current_selection = (during_game_keyboard_menu_data.current_selection - 1) % during_game_keyboard_menu_data.num_items;
    if( during_game_keyboard_menu_data.current_selection < 0 ) {
        during_game_keyboard_menu_data.current_selection = during_game_keyboard_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_keyboard_menu_down_arrow()
{
    during_game_keyboard_menu_data.current_selection = (during_game_keyboard_menu_data.current_selection + 1) % during_game_keyboard_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_keyboard_menu_right_arrow()
{
    ;
}

void during_game_keyboard_menu_left_arrow()
{
    ;
}

void during_game_keyboard_menu_return_key()
{
    unsigned long scancode;

    /* change color 2 to red */
    
    Set_One( 1, SELECTED_TEXT_COLOR ); /* 1 is nonsense right now */

    while( !Jon_Kbhit() );

    scancode = Jon_Getkey();

    /* change color 2 back to green */
    
    Enable_Palette_Member( &menu_stuff.general_menu_background, SELECTED_TEXT_COLOR );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
    

    if( during_game_keyboard_menu_data.current_selection == 0 ) { /* fire laser: */

        game_configuration.keyboard_config.laser = scancode;

    }
    else if( during_game_keyboard_menu_data.current_selection == 1 ) { /* fire missile: */

             game_configuration.keyboard_config.missile = scancode;

    }
    else if( during_game_keyboard_menu_data.current_selection == 2 ) { /* special weapon: */

             game_configuration.keyboard_config.special = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 3 ) { /* mode switch: */

             game_configuration.keyboard_config.mode_switch = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 4 ) { /* up: */

             game_configuration.keyboard_config.up = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 5 ) { /* down: */

             game_configuration.keyboard_config.down = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 6 ) { /* left: */

             game_configuration.keyboard_config.left = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 7 ) { /* right: */

             game_configuration.keyboard_config.right = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 8 ) { /* up throttle: */

             game_configuration.keyboard_config.up_throttle = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 9 ) { /* down throttle: */

             game_configuration.keyboard_config.down_throttle = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 10 ) { /* strafe: */

             game_configuration.keyboard_config.strafe = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 11 ) { /* wing 1 get pylons: */

             game_configuration.keyboard_config.wing_1_pylon = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 12 ) { /* wing 1 attack: */

             game_configuration.keyboard_config.wing_1_attack = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 13 ) { /* wing 1 attack radar base: */

             game_configuration.keyboard_config.wing_1_attack_rb = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 14 ) { /* wing 1 defend radar base: */

             game_configuration.keyboard_config.wing_1_defend_rb = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 15 ) { /* wing 1 group together: */

             game_configuration.keyboard_config.wing_1_group = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 16 ) { /* wing 1 cancel orders: */

             game_configuration.keyboard_config.wing_1_cancel = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 17 ) { /* wing 2 get pylons: */

             game_configuration.keyboard_config.wing_2_pylon = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 18 ) { /* wing 2 attack: */

             game_configuration.keyboard_config.wing_2_attack = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 19 ) { /* wing 2 attack radar base: */

             game_configuration.keyboard_config.wing_2_attack_rb = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 20 ) { /* wing 2 defend radar base: */

             game_configuration.keyboard_config.wing_2_defend_rb = scancode;
    }
    else if( during_game_keyboard_menu_data.current_selection == 21 ) { /* wing 2 group together: */

             game_configuration.keyboard_config.wing_2_group = scancode;

    }
    else if( during_game_keyboard_menu_data.current_selection == 22 ) { /* wing 2 cancel orders: */

             game_configuration.keyboard_config.wing_2_cancel = scancode;
    }

} /* End of during_game_keyboard_menu_return_key */

void during_game_keyboard_menu_escape_key()
{
    make_menu_current( &during_game_options_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void during_game_keyboard_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 10;
    const long y_start = 30;
    const long y_spacing = 7;
    long x, y;
    unsigned long current_scancode = 0;
    char temp_str[80];

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    /* menu title */

    string_blit( "CONFIGURE KEYBOARD", 65, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    x = x_start;
    y = y_start;

    for( i = 0; i < during_game_keyboard_menu_data.num_items; i++ ) {

        if( i == during_game_keyboard_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        micro_string_blit( during_game_keyboard_menu_data.item[i], x, y,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, color );

        if( i == 0 )
            current_scancode = game_configuration.keyboard_config.laser;
        else if( i == 1 )
            current_scancode = game_configuration.keyboard_config.missile;
        else if( i == 2 )
            current_scancode = game_configuration.keyboard_config.special;
        else if( i == 3 )
            current_scancode = game_configuration.keyboard_config.mode_switch;
        else if( i == 4 )
            current_scancode = game_configuration.keyboard_config.up;
        else if( i == 5 )
            current_scancode = game_configuration.keyboard_config.down;
        else if( i == 6 )
            current_scancode = game_configuration.keyboard_config.left;
        else if( i == 7 )
            current_scancode = game_configuration.keyboard_config.right;
        else if( i == 8 )
            current_scancode = game_configuration.keyboard_config.up_throttle;
        else if( i == 9 )
            current_scancode = game_configuration.keyboard_config.down_throttle;
        else if( i == 10 )
            current_scancode = game_configuration.keyboard_config.strafe;
        else if( i == 11 )
            current_scancode = game_configuration.keyboard_config.wing_1_pylon;
        else if( i == 12 )
            current_scancode = game_configuration.keyboard_config.wing_1_attack;
        else if( i == 13 )
            current_scancode = game_configuration.keyboard_config.wing_1_attack_rb;
        else if( i == 14 )
            current_scancode = game_configuration.keyboard_config.wing_1_defend_rb;
        else if( i == 15 )
            current_scancode = game_configuration.keyboard_config.wing_1_group;
        else if( i == 16 )
            current_scancode = game_configuration.keyboard_config.wing_1_cancel;
        else if( i == 17 )
            current_scancode = game_configuration.keyboard_config.wing_2_pylon;
        else if( i == 18 )
            current_scancode = game_configuration.keyboard_config.wing_2_attack;
        else if( i == 19 )
            current_scancode = game_configuration.keyboard_config.wing_2_attack_rb;
        else if( i == 20 )
            current_scancode = game_configuration.keyboard_config.wing_2_defend_rb;
        else if( i == 21 )
            current_scancode = game_configuration.keyboard_config.wing_2_group;
        else if( i == 22 )
            current_scancode = game_configuration.keyboard_config.wing_2_cancel;


        /* sprintf( temp_str, "%ld", current_scancode ); */

        Index_To_String( temp_str, current_scancode );

        micro_string_blit( temp_str, x + 200, y,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, color );


        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_during_game_keyboard_menu()
{
    during_game_keyboard_menu.up_arrow = (Method)during_game_keyboard_menu_up_arrow;
    during_game_keyboard_menu.down_arrow = (Method)during_game_keyboard_menu_down_arrow;
    during_game_keyboard_menu.right_arrow = (Method)during_game_keyboard_menu_right_arrow;
    during_game_keyboard_menu.left_arrow = (Method)during_game_keyboard_menu_left_arrow;
    during_game_keyboard_menu.return_key = (Method)during_game_keyboard_menu_return_key;
    during_game_keyboard_menu.escape_key = (Method)during_game_keyboard_menu_escape_key;
    during_game_keyboard_menu.draw_menu = (Method)during_game_keyboard_menu_draw_menu;

    strcpy( during_game_keyboard_menu_data.item[0], "FIRE LASER:" );
    strcpy( during_game_keyboard_menu_data.item[1], "FIRE MISSILE:" );
    strcpy( during_game_keyboard_menu_data.item[2], "SPECIAL WEAPON:" );
    strcpy( during_game_keyboard_menu_data.item[3], "MODE SWITCH:" );
    strcpy( during_game_keyboard_menu_data.item[4], "UP:" );
    strcpy( during_game_keyboard_menu_data.item[5], "DOWN:" );
    strcpy( during_game_keyboard_menu_data.item[6], "LEFT:" );
    strcpy( during_game_keyboard_menu_data.item[7], "RIGHT:" );
    strcpy( during_game_keyboard_menu_data.item[8], "UP THROTTLE:" );
    strcpy( during_game_keyboard_menu_data.item[9], "DOWN THROTTLE:" );
    strcpy( during_game_keyboard_menu_data.item[10], "STRAFE:" );

    strcpy( during_game_keyboard_menu_data.item[11], "WING 1 GET PYLONS:" );
    strcpy( during_game_keyboard_menu_data.item[12], "WING 1 ATTACK:" );
    strcpy( during_game_keyboard_menu_data.item[13], "WING 1 ATTACK RADAR BASE:" );
    strcpy( during_game_keyboard_menu_data.item[14], "WING 1 DEFEND RADAR BASE:" );
    strcpy( during_game_keyboard_menu_data.item[15], "WING 1 GROUP TOGETHER:" );
    strcpy( during_game_keyboard_menu_data.item[16], "WING 1 CANCEL ORDERS:" );

    strcpy( during_game_keyboard_menu_data.item[17], "WING 2 GET PYLONS:" );
    strcpy( during_game_keyboard_menu_data.item[18], "WING 2 ATTACK:" );
    strcpy( during_game_keyboard_menu_data.item[19], "WING 2 ATTACK RADAR BASE:" );
    strcpy( during_game_keyboard_menu_data.item[20], "WING 2 DEFEND RADAR BASE:" );
    strcpy( during_game_keyboard_menu_data.item[21], "WING 2 GROUP TOGETHER:" );
    strcpy( during_game_keyboard_menu_data.item[22], "WING 2 CANCEL ORDERS:" );

    during_game_keyboard_menu_data.num_items = 23;

    during_game_keyboard_menu_data.current_selection = 0;
}




/* methods for during_game_joystick_menu */




void during_game_joystick_menu_up_arrow()
{
    during_game_joystick_menu_data.current_selection = (during_game_joystick_menu_data.current_selection - 1) % during_game_joystick_menu_data.num_items;
    if( during_game_joystick_menu_data.current_selection < 0 ) {
        during_game_joystick_menu_data.current_selection = during_game_joystick_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_joystick_menu_down_arrow()
{
    during_game_joystick_menu_data.current_selection = (during_game_joystick_menu_data.current_selection + 1) % during_game_joystick_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_joystick_menu_right_arrow()
{
    ;
}

void during_game_joystick_menu_left_arrow()
{
    ;
}

void during_game_joystick_menu_return_key()
{

    if( during_game_joystick_menu_data.current_selection == 0 ) { /* joystick :*/

        game_configuration.joystick = (JoystickType)((game_configuration.joystick + 1) % 3);

        if( game_configuration.joystick == NoJoystick ) {
            strcpy( during_game_joystick_menu_data.item[0], "JOYSTICK : NONE" );
        }
        else if( game_configuration.joystick == StandardJoystick ) {
            strcpy( during_game_joystick_menu_data.item[0], "JOYSTICK : STANDARD" );
        }
        else if( game_configuration.joystick == FlightStickPro ) {
            strcpy( during_game_joystick_menu_data.item[0], "JOYSTICK : CH FLIGHT PRO" );
        }
    }
    else if( during_game_joystick_menu_data.current_selection == 1 ) { /* calibrate */

        make_menu_current( &during_game_calibrate_menu );

    }
    else if( during_game_joystick_menu_data.current_selection == 2 ) { /* configure */

        make_menu_current( &during_game_joystick_config_menu );

    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void during_game_joystick_menu_escape_key()
{
    make_menu_current( &during_game_options_menu );
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void during_game_joystick_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 55;
    const long y_start = 70;
    const long y_spacing = 15;
    long x, y;

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    x = x_start;
    y = y_start;

    for( i = 0; i < during_game_joystick_menu_data.num_items; i++ ) {

        if( i == during_game_joystick_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( during_game_joystick_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_during_game_joystick_menu()
{
    during_game_joystick_menu.up_arrow = (Method)during_game_joystick_menu_up_arrow;
    during_game_joystick_menu.down_arrow = (Method)during_game_joystick_menu_down_arrow;
    during_game_joystick_menu.right_arrow = (Method)during_game_joystick_menu_right_arrow;
    during_game_joystick_menu.left_arrow = (Method)during_game_joystick_menu_left_arrow;
    during_game_joystick_menu.return_key = (Method)during_game_joystick_menu_return_key;
    during_game_joystick_menu.escape_key = (Method)during_game_joystick_menu_escape_key;
    during_game_joystick_menu.draw_menu = (Method)during_game_joystick_menu_draw_menu;

    if( game_configuration.joystick == NoJoystick ) {
        strcpy( during_game_joystick_menu_data.item[0], "JOYSTICK : NONE" );
    }
    else if( game_configuration.joystick == StandardJoystick ) {
        strcpy( during_game_joystick_menu_data.item[0], "JOYSTICK : STANDARD" );
    }
    else if( game_configuration.joystick == FlightStickPro ) {
        strcpy( during_game_joystick_menu_data.item[0], "JOYSTICK : CH FLIGHT PRO" );
    }

    strcpy( during_game_joystick_menu_data.item[1], "CALIBRATE" );
    strcpy( during_game_joystick_menu_data.item[2], "CONFIGURE" );

    during_game_joystick_menu_data.num_items = 3;

    during_game_joystick_menu_data.current_selection = 0;
}




/* methods for during_game_calibrate_menu */




void during_game_calibrate_menu_up_arrow()
{
    ;
}

void during_game_calibrate_menu_down_arrow()
{
    ;
}

void during_game_calibrate_menu_right_arrow()
{
    ;
}

void during_game_calibrate_menu_left_arrow()
{
    ;
}

void during_game_calibrate_menu_return_key()
{
    ;
}

void during_game_calibrate_menu_escape_key()
{
    ;
}

void during_game_calibrate_menu_draw_menu()
{

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    if( game_configuration.joystick == NoJoystick ) {

        /* no joystick selected */

        string_blit( "PLEASE SELECT JOYSTICK TYPE", 15, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

        Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

    }
    else if( game_configuration.joystick == StandardJoystick ) {

        if( !Detect_Joystick() ) {

            /* no joystick detected */

            string_blit( "NO JOYSTICK DETECTED", 50, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();

#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

        }
        else {

            /* calibrate joystick */

            string_blit( "MOVE JOYSTICK TO UPPER LEFT", 20, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_upper_left();
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE JOYSTICK TO LOWER RIGHT", 20, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_lower_right();
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE JOYSTICK TO CENTER", 30, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_center();
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }
    }
    else if( game_configuration.joystick == FlightStickPro ) {

        if( !Detect_Flightstick() ) {

            /* no flightstick detected */

            string_blit( "NO FLIGHTSTICK DETECTED", 15, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

        }
        else {

            /* calibrate flightstick */

            string_blit( "MOVE FLIGHTSTICK TO UPPER LEFT", 5, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_upper_left();
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE FLIGHTSTCK TO LOWER RIGHT", 5, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_lower_right();
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE FLIGHTSTICK TO CENTER", 20, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "THEN PRESS FIRE", 75, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

            calibrate_joystick_center();
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );

            Pop_Buffer( menu_stuff.general_menu_background.buffer );

            string_blit( "MOVE THROTTLE FROM MAX TO", 25, 75,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            string_blit( "MIN THEN PRESS FIRE", 55, 90,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

            Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif

            calibrate_flightstick_throttle();
            
            Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        }
    }

    make_menu_current( &during_game_joystick_menu );

}

void init_during_game_calibrate_menu()
{
    during_game_calibrate_menu.up_arrow = (Method)during_game_calibrate_menu_up_arrow;
    during_game_calibrate_menu.down_arrow = (Method)during_game_calibrate_menu_down_arrow;
    during_game_calibrate_menu.right_arrow = (Method)during_game_calibrate_menu_right_arrow;
    during_game_calibrate_menu.left_arrow = (Method)during_game_calibrate_menu_left_arrow;
    during_game_calibrate_menu.return_key = (Method)during_game_calibrate_menu_return_key;
    during_game_calibrate_menu.escape_key = (Method)during_game_calibrate_menu_escape_key;
    during_game_calibrate_menu.draw_menu = (Method)during_game_calibrate_menu_draw_menu;
}



/* methods for during_game_joystick_config_menu */




void during_game_joystick_config_menu_up_arrow()
{
    during_game_joystick_config_menu_data.current_selection = (during_game_joystick_config_menu_data.current_selection - 1) % during_game_joystick_config_menu_data.num_items;
    if( during_game_joystick_config_menu_data.current_selection < 0 ) {
        during_game_joystick_config_menu_data.current_selection = during_game_joystick_config_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_joystick_config_menu_down_arrow()
{
    during_game_joystick_config_menu_data.current_selection = (during_game_joystick_config_menu_data.current_selection + 1) % during_game_joystick_config_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void during_game_joystick_config_menu_right_arrow()
{
    ;
}

void during_game_joystick_config_menu_left_arrow()
{
    ;
}

void during_game_joystick_config_menu_return_key()
{

    if( during_game_joystick_config_menu_data.current_selection == 0 ) { /* button_1 */
        game_configuration.joystick_config.button_1 = (JoystickButtonAction)((game_configuration.joystick_config.button_1 + 1) % 12);
    }
    else if( during_game_joystick_config_menu_data.current_selection == 1 ) { /* button_2 */
        game_configuration.joystick_config.button_2 = (JoystickButtonAction)((game_configuration.joystick_config.button_2 + 1) % 12);
    }
    else if( during_game_joystick_config_menu_data.current_selection == 2 ) { /* button_3 */
        game_configuration.joystick_config.button_3 = (JoystickButtonAction)((game_configuration.joystick_config.button_3 + 1) % 12);
    }
    else if( during_game_joystick_config_menu_data.current_selection == 3 ) { /* button_4 */
        game_configuration.joystick_config.button_4 = (JoystickButtonAction)((game_configuration.joystick_config.button_4 + 1) % 12);
    }
    else if( during_game_joystick_config_menu_data.current_selection == 4 ) { /* hat_up */
        game_configuration.joystick_config.hat_up = (JoystickButtonAction)((game_configuration.joystick_config.hat_up + 1) % 12);
    }
    else if( during_game_joystick_config_menu_data.current_selection == 5 ) { /* hat_down */
        game_configuration.joystick_config.hat_down = (JoystickButtonAction)((game_configuration.joystick_config.hat_down + 1) % 12);
    }
    else if( during_game_joystick_config_menu_data.current_selection == 6 ) { /* hat_left */
        game_configuration.joystick_config.hat_left = (JoystickButtonAction)((game_configuration.joystick_config.hat_left + 1) % 12);
    }
    else if( during_game_joystick_config_menu_data.current_selection == 7 ) { /* hat_right */
        game_configuration.joystick_config.hat_right = (JoystickButtonAction)((game_configuration.joystick_config.hat_right + 1) % 12);
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void during_game_joystick_config_menu_escape_key()
{
    make_menu_current( &during_game_joystick_menu ); 
    
    Play_Menu_Sound( menu_stuff.menu_sounds.esc_sound );
}

void during_game_joystick_config_menu_draw_menu()
{
    long i;
    unsigned char color;
    const long x_start = 50;
    const long y_start = 50;
    const long y_spacing = 10;
    long x, y;
    unsigned long current_action = 0;
    char temp_str[80];

    Pop_Buffer( menu_stuff.general_menu_background.buffer );

    /* menu title */

    string_blit( "CONFIGURE JOYSTICK", 65, 3,
                 menu_stuff.menu_text.buffer,
                 menu_stuff.menu_text.xpixels + 1,
                 menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );

    x = x_start;
    y = y_start;

    for( i = 0; i < during_game_joystick_config_menu_data.num_items; i++ ) {

        if( i == during_game_joystick_config_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        micro_string_blit( during_game_joystick_config_menu_data.item[i], x, y,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, color );

        if( i == 0 )
            current_action = game_configuration.joystick_config.button_1;
        else if( i == 1 )
            current_action = game_configuration.joystick_config.button_2;
        else if( i == 2 )
            current_action = game_configuration.joystick_config.button_3;
        else if( i == 3 )
            current_action = game_configuration.joystick_config.button_4;
        else if( i == 4 )
            current_action = game_configuration.joystick_config.hat_up;
        else if( i == 5 )
            current_action = game_configuration.joystick_config.hat_down;
        else if( i == 6 )
            current_action = game_configuration.joystick_config.hat_left;
        else if( i == 7 )
            current_action = game_configuration.joystick_config.hat_right;

        if( current_action == ShootLaser )
            strcpy( temp_str, "FIRE LASER" );
        else if( current_action == ShootMissile )
            strcpy( temp_str, "FIRE MISSILE" );
        else if( current_action == ActivateSpecial )
            strcpy( temp_str, "SPECIAL WEAPON" );
        else if( current_action == ActivateStrafe )
            strcpy( temp_str, "STRAFE ON" );
        else if( current_action == ThrottleUp )
            strcpy( temp_str, "THROTTLE UP" );
        else if( current_action == ThrottleDown )
            strcpy( temp_str, "THROTTLE DOWN" );
        else if( current_action == SwitchModes )
            strcpy( temp_str, "SWITCH MODES" );
        else if( current_action == DoNothing )
            strcpy( temp_str, "NOTHING" );
        else if( current_action == StrafeUp )
            strcpy( temp_str, "STRAFE UP" );
        else if( current_action == StrafeDown )
            strcpy( temp_str, "STRAFE DOWN" );
        else if( current_action == StrafeLeft )
            strcpy( temp_str, "STRAFE LEFT" );
        else if( current_action == StrafeRight )
            strcpy( temp_str, "STRAFE RIGHT" );

        micro_string_blit( temp_str, x + 100, y,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, color );

        y += y_spacing;
    }

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_during_game_joystick_config_menu()
{
    during_game_joystick_config_menu.up_arrow = (Method)during_game_joystick_config_menu_up_arrow;
    during_game_joystick_config_menu.down_arrow = (Method)during_game_joystick_config_menu_down_arrow;
    during_game_joystick_config_menu.right_arrow = (Method)during_game_joystick_config_menu_right_arrow;
    during_game_joystick_config_menu.left_arrow = (Method)during_game_joystick_config_menu_left_arrow;
    during_game_joystick_config_menu.return_key = (Method)during_game_joystick_config_menu_return_key;
    during_game_joystick_config_menu.escape_key = (Method)during_game_joystick_config_menu_escape_key;
    during_game_joystick_config_menu.draw_menu = (Method)during_game_joystick_config_menu_draw_menu;

    strcpy( during_game_joystick_config_menu_data.item[0], "BUTTON 1:" );
    strcpy( during_game_joystick_config_menu_data.item[1], "BUTTON 2:" );
    strcpy( during_game_joystick_config_menu_data.item[2], "BUTTON 3:" );
    strcpy( during_game_joystick_config_menu_data.item[3], "BUTTON 4:" );
    strcpy( during_game_joystick_config_menu_data.item[4], "HAT UP:" );
    strcpy( during_game_joystick_config_menu_data.item[5], "HAT DOWN:" );
    strcpy( during_game_joystick_config_menu_data.item[6], "HAT LEFT:" );
    strcpy( during_game_joystick_config_menu_data.item[7], "HAT RIGHT:" );

    during_game_joystick_config_menu_data.num_items = 8;

    during_game_joystick_config_menu_data.current_selection = 0;
}

void during_game_menu_event_init()
{
	// init all the menus : the indenting shows the menu structure
    init_during_game_menu();
        init_during_game_options_menu();
            init_during_game_sound_menu();
            init_during_game_detail_menu();
            init_during_game_keyboard_menu();
            init_during_game_joystick_menu();
                init_during_game_calibrate_menu();
                init_during_game_joystick_config_menu();

    // set the current_menu to be during_game_menu
    make_menu_current( &during_game_menu );
}

// during_game_menu_event_loop returns 
// 0 if the user is still in the menu system.
// 1 if the user wishes to resume the game
// 2 if the user wishes to quit the game.
int during_game_menu_event_loop()
{
	quit_game = FALSE;
    exit_event_loop = FALSE;

	// draw the current menu.
    (*current_menu.draw_menu)();

	if ( !current_menu.delay_input )
	{
		if ( SYS_KeyPressed( KEY_UP_ARROW ) )
		{
			(*current_menu.up_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_DOWN_ARROW ) ) 
		{
			(*current_menu.down_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_RIGHT_ARROW ) ) {
			(*current_menu.right_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_LEFT_ARROW ) ) 
		{
			(*current_menu.left_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_ENTER ) ) 
		{
			(*current_menu.return_key)();
		}
		else if ( SYS_KeyPressed( KEY_ESC ) ) 
		{
			(*current_menu.escape_key)();
		}
	}

	current_menu.delay_input = 0;

	if ( exit_event_loop )
	{
		if ( quit_game )		
			return 2;
		else
			return 1;
	}
	else
		return 0;	
}




/* methods for custom_game_stat_menu */




void custom_game_stat_menu_up_arrow()
{
    custom_game_stat_menu_data.current_selection = (custom_game_stat_menu_data.current_selection - 1) % custom_game_stat_menu_data.num_items;
    if( custom_game_stat_menu_data.current_selection < 0 ) {
        custom_game_stat_menu_data.current_selection = custom_game_stat_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void custom_game_stat_menu_down_arrow()
{
    custom_game_stat_menu_data.current_selection = (custom_game_stat_menu_data.current_selection + 1) % custom_game_stat_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void custom_game_stat_menu_right_arrow()
{
    ;
}

void custom_game_stat_menu_left_arrow()
{
    ;
}

void custom_game_stat_menu_return_key()
{
    FILE *fp;
    int i;
    char filename[80];

    if( custom_game_stat_menu_data.current_selection == 0 ) { /* output stats */

        /* find the next available filename */
        
        for( i = 0; i < 1000; i++ ) {
                
            sprintf( filename, "%sSTATS/CUST%03d.TXT", g_DataPath, i );
            
            if( (fp = fopen( filename, "r" ) ) == NULL ) {
            
                /* if this filename doesn't exist create it */

                Save_Game_Stats( &game_stats, filename );
                
                break;
            }
            else {
                fclose( fp );
            }
        }
        
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        
        Pop_Buffer( menu_stuff.general_menu_background.buffer );
        
        string_blit( "STATS SAVED AS", 90, 65,                      
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
                     
        string_blit( filename, 75, 85,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
                     
        micro_string_blit( "PRESS ANY KEY TO CONTINUE", 10, 190,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
        
        Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
        
        while( !Jon_Kbhit() );
        
        exit_event_loop = TRUE;
        make_menu_current( &custom_game_menu );
    }
    else if( custom_game_stat_menu_data.current_selection == 1 ) { /* continue */

        exit_event_loop = TRUE;
        make_menu_current( &custom_game_menu );

    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void custom_game_stat_menu_escape_key()
{
    Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
}

void custom_game_stat_menu_draw_menu( int victory )
{
    long i;
    unsigned char color;
    const long x_start = 5;
    const long y_start = 170;
    const long y_spacing = 15;
    const long stat_x_start = 200;
    const long stat_y_start = 35;
    const long stat_y_spacing = 10;
    long x, y;
    char str[80];

    Pop_Buffer( menu_stuff.general_menu_background.buffer );
    
    /* draw the title */
    
    if( victory == 1 ) {    
        string_blit( "YOU WERE VICTORIOUS", 50, 5,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
    }
    else if( victory == 0 ) {
       string_blit( "YOU WERE DEFEATED", 60, 5,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
    }
    else {                
        string_blit( "GAME ABORTED", 90, 5,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
    }

    x = x_start;
    y = y_start;

    for( i = 0; i < custom_game_stat_menu_data.num_items; i++ ) {

        if( i == custom_game_stat_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( custom_game_stat_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }    
    
    x = stat_x_start;
    y = stat_y_start;
    
    /* draw the pilot name */
    
    micro_string_blit( "PILOT:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    micro_string_blit( game_stats.name, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                       
    /* draw the number of kills */
    
    micro_string_blit( "KILLS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.kills );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    y += stat_y_spacing;                       
    
    /* number of deaths */
    
    micro_string_blit( "DEATHS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.times_killed );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                       
    /* number of shots fired */
    
    micro_string_blit( "TOTAL SHOTS FIRED:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.shots_fired );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    y += stat_y_spacing;                       
    
    /* enemy hits */
    
    micro_string_blit( "ENEMY HITS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.enemy_hits );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );    
                       
    y += stat_y_spacing;
    
    /* friendly hits */
    
    micro_string_blit( "FREINDLY HITS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.friendly_hits );                       
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                               
    /* misses */

    micro_string_blit( "MISSED SHOTS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.misses );
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;                       
    
    /* hit miss ratio */
    
    micro_string_blit( "HIT PERCENTAGE:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%.3f", 100.0 * game_stats.hit_percentage );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;                       
    
    /* pylons grabbed */
    
    micro_string_blit( "TOTAL PYLONS GRABBED:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.pylons_grabbed );
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    
    y += stat_y_spacing;
                       
    /* average d */
    
    micro_string_blit( "AVERAGE D LEVEL:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%.3f", game_stats.average_d );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    
    y += stat_y_spacing;
                       
    /* times hit by enemy */
                       
    micro_string_blit( "ENEMY HITS TAKEN:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.times_hit );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_custom_game_stat_menu()
{
    custom_game_stat_menu.up_arrow = (Method)custom_game_stat_menu_up_arrow;
    custom_game_stat_menu.down_arrow = (Method)custom_game_stat_menu_down_arrow;
    custom_game_stat_menu.right_arrow = (Method)custom_game_stat_menu_right_arrow;
    custom_game_stat_menu.left_arrow = (Method)custom_game_stat_menu_left_arrow;
    custom_game_stat_menu.return_key = (Method)custom_game_stat_menu_return_key;
    custom_game_stat_menu.escape_key = (Method)custom_game_stat_menu_escape_key;
    custom_game_stat_menu.draw_menu = (Method)custom_game_stat_menu_draw_menu;	// HACK: casting from method2 to method

    strcpy( custom_game_stat_menu_data.item[0], "OUTPUT STATS" );
    strcpy( custom_game_stat_menu_data.item[1], "CONTINUE" );

    custom_game_stat_menu_data.num_items = 2;

    custom_game_stat_menu_data.current_selection = 1;
}

void custom_game_state_menu_event_init()
{
    // init all the menus : the indenting shows the menu structure
    init_custom_game_stat_menu();
    
    // set the current_menu to be during_game_menu
    make_menu_current( &custom_game_stat_menu );    
}

int custom_game_stat_menu_event_loop( int victory )
{
    exit_event_loop = FALSE;

	// draw the current menu.
    (*(Method2)(current_menu.draw_menu))( victory );  // HACK: casting from method to method2

	if ( !current_menu.delay_input )
	{
		if ( SYS_KeyPressed( KEY_UP_ARROW ) )
		{
			(*current_menu.up_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_DOWN_ARROW ) ) 
		{
			(*current_menu.down_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_RIGHT_ARROW ) ) {
			(*current_menu.right_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_LEFT_ARROW ) ) 
		{
			(*current_menu.left_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_ENTER ) ) 
		{
			(*current_menu.return_key)();
		}
		else if ( SYS_KeyPressed( KEY_ESC ) ) 
		{
			(*current_menu.escape_key)();
		}
	}

	current_menu.delay_input = 0;

	return exit_event_loop;
}




/* methods for tournament_game_stat_menu */




void tournament_game_stat_menu_up_arrow()
{
    tournament_game_stat_menu_data.current_selection = (tournament_game_stat_menu_data.current_selection - 1) % tournament_game_stat_menu_data.num_items;
    if( tournament_game_stat_menu_data.current_selection < 0 ) {
        tournament_game_stat_menu_data.current_selection = tournament_game_stat_menu_data.num_items - 1;
    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );
}

void tournament_game_stat_menu_down_arrow()
{
    tournament_game_stat_menu_data.current_selection = (tournament_game_stat_menu_data.current_selection + 1) % tournament_game_stat_menu_data.num_items;
    
    Play_Menu_Sound( menu_stuff.menu_sounds.toggle_sound );

}

void tournament_game_stat_menu_right_arrow()
{
    ;
}

void tournament_game_stat_menu_left_arrow()
{
    ;
}

void tournament_game_stat_menu_return_key()
{
    FILE *fp;
    int i;
    char filename[80];

    if( tournament_game_stat_menu_data.current_selection == 0 ) { /* output stats */

        /* find the next available filename */
        
        for( i = 0; i < 1000; i++ ) {
                
            sprintf( filename, "%sSTATS/TOUR%03d.TXT", g_DataPath, i );
            
            if( (fp = fopen( filename, "r" ) ) == NULL ) {
            
                /* if this filename doesn't exist create it */

                Save_Game_Stats( &game_stats, filename );
                
                break;
            }
            else {
                fclose( fp );
            }
        }
        
        Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
        
        Pop_Buffer( menu_stuff.general_menu_background.buffer );
        
        string_blit( "STATS SAVED AS", 90, 65,                      
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
                     
        string_blit( filename, 75, 85,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
                     
        micro_string_blit( "PRESS ANY KEY TO CONTINUE", 10, 190,
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

        Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
        
        while( !Jon_Kbhit() );
        
        exit_event_loop = TRUE;
        make_menu_current( &tournament_game_setup_menu );
    }
    else if( tournament_game_stat_menu_data.current_selection == 1 ) { /* continue */

        exit_event_loop = TRUE;
        make_menu_current( &tournament_game_setup_menu );

    }
    
    Play_Menu_Sound( menu_stuff.menu_sounds.enter_sound );
}

void tournament_game_stat_menu_escape_key()
{
    Play_Menu_Sound( menu_stuff.menu_sounds.error_sound );
}

void tournament_game_stat_menu_draw_menu( int victory )
{
    long i;
    unsigned char color;
    const long x_start = 5;
    const long y_start = 170;
    const long y_spacing = 15;
    const long stat_x_start = 200;
    const long stat_y_start = 35;
    const long stat_y_spacing = 10;
    long x, y;
    char str[80];

    Pop_Buffer( menu_stuff.general_menu_background.buffer );
    
    /* draw the title */
    
    if( victory == 1 ) {    
        string_blit( "YOU WERE VICTORIOUS", 50, 5,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
    }
    else if( victory == 0 ) {
       string_blit( "YOU WERE DEFEATED", 60, 5,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
    }
    else {
        string_blit( "GAME ABORTED", 90, 5,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, SELECTED_TEXT_COLOR );
    }

    x = x_start;
    y = y_start;

    for( i = 0; i < tournament_game_stat_menu_data.num_items; i++ ) {

        if( i == tournament_game_stat_menu_data.current_selection ) {
            color = SELECTED_TEXT_COLOR;
        }
        else {
            color = UNSELECTED_TEXT_COLOR;
        }

        string_blit( tournament_game_stat_menu_data.item[i], x, y,
                     menu_stuff.menu_text.buffer,
                     menu_stuff.menu_text.xpixels + 1,
                     menu_stuff.menu_text.ypixels + 1, color );

        y += y_spacing;
    }    
    
    x = stat_x_start;
    y = stat_y_start;
    
    /* draw the pilot name */
    
    micro_string_blit( "PILOT:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    micro_string_blit( game_stats.name, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                       
    /* draw the number of kills */
    
    micro_string_blit( "KILLS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.kills );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    y += stat_y_spacing;                       
    
    /* number of deaths */
    
    micro_string_blit( "DEATHS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.times_killed );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                       
    /* number of shots fired */
    
    micro_string_blit( "TOTAL SHOTS FIRED:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.shots_fired );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    y += stat_y_spacing;                       
    
    /* enemy hits */
    
    micro_string_blit( "ENEMY HITS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.enemy_hits );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );    
                       
    y += stat_y_spacing;
    
    /* friendly hits */
    
    micro_string_blit( "FREINDLY HITS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.friendly_hits );                       
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;
                               
    /* misses */

    micro_string_blit( "MISSED SHOTS:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.misses );
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;                       
    
    /* hit miss ratio */
    
    micro_string_blit( "HIT PERCENTAGE:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%.3f", 100.0 * game_stats.hit_percentage );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
                       
    y += stat_y_spacing;                       
    
    /* pylons grabbed */
    
    micro_string_blit( "TOTAL PYLONS GRABBED:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.pylons_grabbed );
    
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    
    y += stat_y_spacing;
                       
    /* average d */
    
    micro_string_blit( "AVERAGE D LEVEL:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%.3f", game_stats.average_d );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );
    
    y += stat_y_spacing;
                       
    /* times hit by enemy */
                       
    micro_string_blit( "ENEMY HITS TAKEN:", 10, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, 4 );
                       
    sprintf( str, "%d", (int)game_stats.times_hit );
                       
    micro_string_blit( str, x, y,
                       menu_stuff.micro_text.buffer,
                       menu_stuff.micro_text.xpixels + 1,
                       menu_stuff.micro_text.ypixels + 1, SELECTED_TEXT_COLOR );

    Swap_Buffer();
#ifdef GLCYLINDRIX
	GL_SubloadAndSwapBuffer();
#endif
}

void init_tournament_game_stat_menu()
{
    tournament_game_stat_menu.up_arrow = (Method)tournament_game_stat_menu_up_arrow;
    tournament_game_stat_menu.down_arrow = (Method)tournament_game_stat_menu_down_arrow;
    tournament_game_stat_menu.right_arrow = (Method)tournament_game_stat_menu_right_arrow;
    tournament_game_stat_menu.left_arrow = (Method)tournament_game_stat_menu_left_arrow;
    tournament_game_stat_menu.return_key = (Method)tournament_game_stat_menu_return_key;
    tournament_game_stat_menu.escape_key = (Method)tournament_game_stat_menu_escape_key;
    tournament_game_stat_menu.draw_menu = (Method)tournament_game_stat_menu_draw_menu;	// HACK: casting from method2 to method

    strcpy( tournament_game_stat_menu_data.item[0], "OUTPUT STATS" );
    strcpy( tournament_game_stat_menu_data.item[1], "CONTINUE" );

    tournament_game_stat_menu_data.num_items = 2;

    tournament_game_stat_menu_data.current_selection = 1;
}

void tournament_game_stat_menu_event_init()
{
    // init all the menus : the indenting shows the menu structure
    init_tournament_game_stat_menu();
    
    // set the current_menu to be during_game_menu
    make_menu_current( &tournament_game_stat_menu );
}

// returns 0 if user is still in menu, 1 if user exits
int tournament_game_stat_menu_event_loop( int victory )
{
    exit_event_loop = FALSE;

	// draw the current menu.
    (*(Method2)current_menu.draw_menu)( victory );	// HACK: casting from method to method2

	if ( !current_menu.delay_input )
	{
		if ( SYS_KeyPressed( KEY_UP_ARROW ) )
		{
			(*current_menu.up_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_DOWN_ARROW ) ) 
		{
			(*current_menu.down_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_RIGHT_ARROW ) ) {
			(*current_menu.right_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_LEFT_ARROW ) ) 
		{
			(*current_menu.left_arrow)();
		}
		else if ( SYS_KeyPressed( KEY_ENTER ) ) 
		{
			(*current_menu.return_key)();
		}
		else if ( SYS_KeyPressed( KEY_ESC ) ) 
		{
			(*current_menu.escape_key)();
		}
	}

	current_menu.delay_input = 0;

	return exit_event_loop;
}



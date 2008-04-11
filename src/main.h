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

#include "types.h"

#ifndef MAIN_H
#define MAIN_C

void init_sounds( void );
void init_red_team( Player *p );
void init_blue_team( Player *p );
void init_stuff();
void init_color_info( ColorInfo *color_info );
void init_world_stuff( WorldStuff *world_stuff );
void init_menu_stuff( MenuStuff *menu_stuff );

void check_command_line_args( int argc, const char *argv[],
                              unsigned char *ai_active );

void init_modem_connection( unsigned char master, long *movement_vehicle,
                            long *view_vehicle, long *remote_player );

void init_serial_game_state( unsigned char master, long *movement_vehicle,
                             long *view_vehicle, long *remote_player );

void get_user_key_table( input_table table );

void get_all_input( Player p[], int ai_active, int master );

void move_everything_one_click();

void draw_everything( long view_vehicle, long status_bar,
                      long first_person_view, long transporting );

void free_world_stuff( WorldStuff *w );

void free_menu_stuff( MenuStuff *menu_stuff );

void update_leader( long view_vehicle );

void free_pcx_picture( pcx_picture *pic );

void empty_snapshot_queue( SnapshotQueue *snapshot_queue );

void init_player_array_controller_info( Player p[], game_configuration_type *game_configuration,
                                        int master );
                                        
void transfer_game_configuration_info( game_configuration_type *game_configuration, int master );

int user_vehicle_index();

void init_local_user_team( team_type *team );

int num_vehicles_remaining( team_type team );

void get_current_view_orient( Orientation *o, Orientation *old_o, team_type team, int *transporting,
                              int *first_person_view );
                              
int is_round_over();

void reset_game_to_start( WorldStuff *world_stuff, level_type *level,
                          game_configuration_type *game_config, int master );

void free_all_samples( WorldStuff *world_stuff );

void get_only_ai_input( Player p[], int ai_active, int master );

void Init_Menu_Sounds( void );

void Free_Menu_Sounds( void );

void Init_Menu_Voices( void );

void Free_Menu_Voices( void );

void cylindrix_init_print( char str[] );

void print_game_configuration_info( game_configuration_type *game_configuration, int master );

void get_current_view_orient_during_fadeout( Orientation *view_o, Orientation *old_o, team_type team, int *transporting,
                                             int *first_person_view );

void print_all_input_tables( Player p[], long frame_counter );

void ipx_get_current_view_orient( Orientation *view_o, Orientation *old_o, team_type team, long *transporting,
                                  long *first_person_view );

void display_next_opponent();

void tournament_init_print( char str[] );

void print_p_mem( char *str );

#endif

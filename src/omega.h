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

#ifndef OMEGA_H
#define OMEGA_H

#include "types.h"
#include "stats.h"

extern MenuStuff menu_stuff;
extern game_configuration_type game_configuration;
extern game_stats_type game_stats;
extern overall_stats_type overall_stats;
extern WorldStuff world_stuff;
extern level_type level;
extern int master;
extern long sb_installed;
extern boolean ai_active;
extern team_type local_user_team;
extern boolean show_readout;
extern boolean ceiling_on;
extern int level_warp;

#ifdef GLCYLINDRIX
extern boolean renderSoftwareBuffer;
#endif


extern SYS_SOUNDHANDLE sample[MAX_WORLD_SAMPLES];
extern SYS_SOUNDHANDLE computer_sample[MAX_COMPUTER_SAMPLES];


void tournament_game_setup();
void setup_game();
void init_game( int argc, const char *argv[] );

#endif

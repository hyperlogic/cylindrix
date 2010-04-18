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

#include "ai.h"

extern const char* g_DataPath;

/* prototype for the initalization screen print out */

void cylindrix_init_print( char str[] );  /* from main.c */
void tournament_init_print( char str[] ); /* from main.c */

/* This is all six vehicles connection with the outside
   world for the ai to use */
player_events_type player_events[6];

/* From omega.c */
extern boolean test_samples;
extern game_configuration_type game_configuration;
/* From commands.c */
extern command_handler_type red_commands, blue_commands;

void Init_AI( WorldStuff *world_stuff )
{
    int j, i;
#if 0
    character_type character; /* Temporary */
#endif

    Init_Pylon_Grid(world_stuff);    /* For 2d paths */
    Init_Centers_Array(world_stuff); /* For 2d paths */

    if (game_configuration.game_type == TournamentGame )
        tournament_init_print("G3DRPCAIE : FUSING NEURONS FOR AI");
    else
        cylindrix_init_print("G3DRPCAIE : FUSING NEURONS FOR AI");

	/* Load up the ai from "new_people.dat" and initialize each player's character struct */
	int ai_indices[6];
	ai_indices[0] = game_configuration.red0_ai;
	ai_indices[1] = game_configuration.red1_ai;
	ai_indices[2] = game_configuration.red2_ai;
	ai_indices[3] = game_configuration.blue0_ai;
	ai_indices[4] = game_configuration.blue1_ai;
	ai_indices[5] = game_configuration.blue2_ai;
	Load_All_AI((Player*)world_stuff->player_array, "gamedata/new_people.dat", ai_indices);

    if (game_configuration.game_type == TournamentGame)
        tournament_init_print( "G3DRPCAIE : SYNAPSE PARSING ENABLED" );     
    else
        cylindrix_init_print( "G3DRPCAIE : SYNAPSE PARSING ENABLED" );     
 
    /* Zero everything in the AI out */
    
    for( i = 0; i < 6; i++ )
        {
         player_events[i].memory.following_path = 0;
        
        
         for( j = 0; j < BULLET_THRESHOLD; j++ )
             {
              player_events[i].memory.bullet_hit[j] = 0;
             }
         player_events[i].memory.stuck   = 0;
         player_events[i].memory.my_position_index = 0;
         player_events[i].memory.bullet_index = 0;
         player_events[i].memory.current_pylon = 255;
         player_events[i].memory.following_path = 0;
         player_events[i].killed_someone = FALSE;
         player_events[i].memory.command_active = 0;
         player_events[i].memory.command_frames_left = 0;
         player_events[i].memory.in_air_index = 0;
         for( j = 0; j < IN_AIR_THRESHOLD; j++ )
             {  /* We don't want them to switch modes right away */ 
              if( j % 2 ) 
                  player_events[i].memory.in_air_array[j] = 0;
              else
                  player_events[i].memory.in_air_array[j] = 1;
             }
        
        red_commands.command_active[0] = 0;
        red_commands.command_active[1] = 0;
        blue_commands.command_active[0] = 0;
        blue_commands.command_active[1] = 0;
        

        }  /* End for */
 
} /* End of Init_AI */


static const int NUM_LINES_PER_CHARACTER = 18;
static const int NUM_CHARACTERS = 50;
struct character_lines
{
	char* lines[NUM_LINES_PER_CHARACTER];
};

static void space_to_null(char* str)
{
	char* p = str;
	while (*p)
	{
		if (*p == ' ')
			*p = 0;
		p++;
	}
}

void Load_All_AI(Player* players, const char* filename, int* ai_indices)
{
	// append path to front of filename.
	char newfilename[512];
	sprintf(newfilename, "%s%s",g_DataPath, filename);
	FILE* fp = fopen(newfilename, "rb");
	if (!fp)
		SYS_Error("Error loading AI\n");

	struct character_lines* c_lines = (struct character_lines*)malloc(sizeof(struct character_lines) * NUM_CHARACTERS);

	// load in the WHOLE thing.
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	char* buffer = (char*)malloc(sizeof(char) * (size + 1));
	fseek(fp, 0, SEEK_SET);
	fread(buffer, sizeof(char), size, fp);

	int line_count = 0;
	int c_count = 0;
	char* line = strtok(buffer, "\n");
	while (line)
	{
		// skip comment lines
		if (line[0] == '#')
			continue;

		c_lines[c_count].lines[line_count++] = line;

		// move on to next character.
		if (line_count == NUM_LINES_PER_CHARACTER)
		{
			line_count = 0;
			c_count++;
		}
		line = strtok(0, "\n");
	}

	int i, j;
	for (i = 0; i < 6; i++)
	{
		character_type* c = &players[i].character;
		memset(c, 0, sizeof(character_type));
		space_to_null(c_lines[ai_indices[i]].lines[0]);
		strcpy(c->name, c_lines[ai_indices[i]].lines[0]);
		space_to_null(c_lines[ai_indices[i]].lines[1]);
		strcpy(c->filename, c_lines[ai_indices[i]].lines[1]);
		for (j = 0; j < NUMBER_CHARACTER_SOUNDS; ++j)
		{
			space_to_null(c_lines[ai_indices[i]].lines[j+2]);
			strcpy(c->sample_filenames[j], c_lines[ai_indices[i]].lines[j+2]);

			// actually load the sample here.
			c->samples[j] = SYS_LoadSound(c->sample_filenames[j]);
		}
		c->passive_aggressive = atoi(c_lines[ai_indices[i]].lines[9]);
		c->bravery_cowardice = atoi(c_lines[ai_indices[i]].lines[10]);
		c->aerial_ground = atoi(c_lines[ai_indices[i]].lines[11]);
		c->obedience_disobedience = atoi(c_lines[ai_indices[i]].lines[12]);
		c->pylon_grab = atoi(c_lines[ai_indices[i]].lines[13]);
		c->radar_kill = atoi(c_lines[ai_indices[i]].lines[14]);
		c->radar_protect = atoi(c_lines[ai_indices[i]].lines[15]);
		c->skill_level = atoi(c_lines[ai_indices[i]].lines[16]);
		c->preferred_vehicle = atoi(c_lines[ai_indices[i]].lines[17]);
	}
	
	free(c_lines);
	free(buffer);
}


void AI_Control( WorldStuff *world_stuff, int vehicle_number )
    {
     Player *player;
     team_type team, enemy_team;

     short frames_till_traitor_deactivate;
     short frames_till_unscramble;
     short scramble_life;
     short traitor_life;


     /* Alias pointer to this player */
     player = world_stuff->player_array;

     frames_till_traitor_deactivate = player[vehicle_number].tank.frames_till_traitor_deactivate;
     frames_till_unscramble         = player[vehicle_number].tank.frames_till_unscramble; 
     scramble_life                  = player[vehicle_number].tank.scramble_life;
     traitor_life                   = player[vehicle_number].tank.traitor_life;


     if( player[vehicle_number].tank.team == RED_TEAM )
         {
          team = RED_TEAM;
          enemy_team = BLUE_TEAM;
         }
     else
         {
          team = BLUE_TEAM;
          enemy_team = RED_TEAM;
         }



     if( player[vehicle_number].character.skill_level > 2 && player[vehicle_number].controller != USER_CONTROL )
         {
          if( player[vehicle_number].tank.traitor_active )
          if( frames_till_traitor_deactivate < (traitor_life - 40) )
              player[vehicle_number].tank.traitor_active = FALSE;

          /*
          if( player[vehicle_number].tank.controls_scrambled )
          if( frames_till_unscramble < (scramble_life - 40) )
              player[vehicle_number].tank.controls_scrambled = FALSE;
          */
         }


     if( player[vehicle_number].tank.traitor_active )
         {    
          player[vehicle_number].tank.team = enemy_team;
          player[vehicle_number].team = enemy_team;
         }

     
     /* Clear this players input table */
     Clear_Input_Table( player[vehicle_number].table );

     /* Fill up this players events data structure */
     Update_Player_Events( world_stuff, vehicle_number );

     /* Figure out what state we are in now */
     world_stuff->player_array[vehicle_number].character.state = Find_State( world_stuff, vehicle_number );


     if( player[vehicle_number].tank.traitor_active )
         {    
          world_stuff->player_array[vehicle_number].character.state = ATTACK;
         }

     /* Based on the state of the ai call appropriate control function */
     switch( world_stuff->player_array[vehicle_number].character.state )
         {
          case ATTACK:
              Attack( world_stuff, vehicle_number );
              break;
          case GET_ENERGY:
              Get_Energy( world_stuff, vehicle_number );
              break;
          case PANIC:
              Panic( world_stuff, vehicle_number );
              break;
          case BEZERK:
              Bezerk( world_stuff, vehicle_number );
              break;
          case HIDE:
              Hide( world_stuff, vehicle_number );
              break;
          case GROUPUP:
              Group( world_stuff, vehicle_number );
              break;
          case GET_PYLONS:
              Get_Pylons( world_stuff, vehicle_number );
              break;
          case PROTECT:
              Protect( world_stuff, vehicle_number );
              break;
          case KILL_RADAR_BASE:
              Kill_Radar_Base( world_stuff, vehicle_number );
              break;
          case PROTECT_RADAR_BASE:
              Protect_Radar_Base( world_stuff, vehicle_number );
              break;
          case EVADE:
              Evade( world_stuff, vehicle_number );
          default:
              break;

         } /* End case */

     if( player[vehicle_number].tank.traitor_active )
         {    
          player[vehicle_number].tank.team = team;
          player[vehicle_number].team = team;
         }

     if( player[vehicle_number].tank.vtype == Dragonfly )
         Cloak_Override( world_stuff, vehicle_number );

     if( player[vehicle_number].tank.vtype == Roach )
         Mines( world_stuff, vehicle_number );

     if( player_events[vehicle_number].memory.stuck )
         Stuck_Override( world_stuff, vehicle_number );

    } /* End of AI_Control */


void Clear_Ai( void )
    {
     int i, j;


    /* Zero everything in the AI out */
    for( i = 0; i < 6; i++ )
        {
         player_events[i].memory.following_path = 0;
        
        
         for( j = 0; j < BULLET_THRESHOLD; j++ )
             {
              player_events[i].memory.bullet_hit[j] = 0;
             }
         player_events[i].memory.stuck   = 0;
         player_events[i].memory.my_position_index = 0;
         player_events[i].memory.surface = 1;
         player_events[i].memory.bullet_index = 0;
         player_events[i].memory.current_pylon = 255;
         player_events[i].memory.following_path = 0;
         player_events[i].killed_someone = FALSE;
         player_events[i].memory.command_active = 0;
         player_events[i].memory.command_frames_left = 0;
         player_events[i].memory.in_air_index = 0;
         for( j = 0; j < IN_AIR_THRESHOLD; j++ )
             { /* We don't want them to switch modes right away */
              if( j % 2 ) 
                  player_events[i].memory.in_air_array[j] = 0;
              else
                  player_events[i].memory.in_air_array[j] = 1;
             }
        
        } /* End for */

        red_commands.command_active[0] = 0;
        red_commands.command_active[1] = 0;
        blue_commands.command_active[0] = 0;
        blue_commands.command_active[1] = 0;


    } /* End of Clear_Ai() */

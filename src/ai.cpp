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
#include <assert.h>
#include <yaml.h>

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
	Load_All_AI((Player*)world_stuff->player_array, "gamedata/people.yaml", ai_indices);

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

#define HANDLER_LIST													\
	DEF_HANDLER("name", strcpy(c->name, value);)						\
    DEF_HANDLER("pcx_file", strcpy(c->filename, value);)				\
    DEF_HANDLER("greeting", strcpy(c->sample_filenames[0], value);)		\
	DEF_HANDLER("affirmation", strcpy(c->sample_filenames[1], value);)	\
    DEF_HANDLER("negation", strcpy(c->sample_filenames[2], value);)		\
    DEF_HANDLER("gloat", strcpy(c->sample_filenames[3], value);)		\
    DEF_HANDLER("despair", strcpy(c->sample_filenames[4], value);)		\
    DEF_HANDLER("death", strcpy(c->sample_filenames[5], value);)		\
    DEF_HANDLER("victory", strcpy(c->sample_filenames[6], value);)		\
    DEF_HANDLER("passive_aggressive", c->passive_aggressive = atoi(value);) \
    DEF_HANDLER("bravery_cowardice", c->bravery_cowardice = atoi(value);) \
    DEF_HANDLER("aerial_ground", c->aerial_ground = atoi(value);)		\
    DEF_HANDLER("obedience_disobedience", c->obedience_disobedience = atoi(value);) \
    DEF_HANDLER("pylon_grab", c->pylon_grab = atoi(value);)				\
    DEF_HANDLER("radar_kill", c->radar_kill = atoi(value);)				\
	DEF_HANDLER("radar_protect", c->radar_protect = atoi(value);)		\
	DEF_HANDLER("skill_level", c->skill_level = atoi(value);)			\
	DEF_HANDLER("preferred_vehicle", c->preferred_vehicle = atoi(value);)

void Character_Set(character_type* c, const char* key, const char* value)
{
#define DEF_HANDLER(str, action)				\
	if (strcmp(str, key) == 0)					\
	{											\
	    action									\
		return;	    							\
	}

	HANDLER_LIST
#undef DEF_HANDLER
}

#define MAX_AIS 50

void Load_All_AI(Player* players, const char* filename, int* ai_indices)
{
	// append path to front of filename.
	char newfilename[512];
	sprintf(newfilename, "%s%s",g_DataPath, filename);
	FILE* fp = fopen(newfilename, "r");
	if (!fp)
		SYS_Error("Error loading AI\n");

	// pre allocate the maximum number of characters
	character_type* ais = (character_type*)malloc(sizeof(character_type) * MAX_AIS);
	memset(ais, 0, sizeof(character_type) * MAX_AIS);

	// open the YAML file
	yaml_parser_t parser;
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, fp);

	// NOTE: YAML file is expected to contain a sequence of maps.
	// One map per character.
	int done = 0;
	int ai_index = 0;
	char key[128];
	bool read_key = false;
	while (!done)
	{
		yaml_event_t event;

		// get the next event
		if (!yaml_parser_parse(&parser, &event))
		{
			SYS_Error("error parsing YAML file \"%s\"\n", newfilename);
			return;
		}

		// process the event
		switch (event.type)
		{
		case YAML_MAPPING_START_EVENT:
			read_key = true;
			break;

		case YAML_SCALAR_EVENT:
			if (read_key)
				strcpy(key, (char*)event.data.scalar.value);
			else
				Character_Set(&ais[ai_index], key, (char*)event.data.scalar.value);
			read_key = !read_key;
			break;

		case YAML_MAPPING_END_EVENT:
			assert(ai_index < MAX_AIS);  // too many ais in yaml file.
			ai_index++;
			break;

		case YAML_NO_EVENT:
		case YAML_STREAM_START_EVENT:
		case YAML_STREAM_END_EVENT:
		case YAML_DOCUMENT_START_EVENT:
		case YAML_DOCUMENT_END_EVENT:
		case YAML_ALIAS_EVENT:
		case YAML_SEQUENCE_START_EVENT:
		case YAML_SEQUENCE_END_EVENT:
		default:
			break;
		}

		done = (event.type == YAML_STREAM_END_EVENT);
		yaml_event_delete(&event);
	}

	yaml_parser_delete(&parser);

	// load each player's sound fx.
	for (int i = 0; i < 6; ++i)
	{
		character_type* c = &players[i].character;
		memcpy(c, &ais[ai_indices[i]], sizeof(character_type));
		for (int j = 0; j < NUMBER_CHARACTER_SOUNDS; ++j)
			c->samples[j] = SYS_LoadSound(c->sample_filenames[j]);
	}

	free(ais);
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

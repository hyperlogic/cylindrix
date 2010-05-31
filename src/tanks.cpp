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

#include <stdlib.h>
#include "tanks.h"
#include "object.h"
#include "collide.h"
#include "assert.h"
#include <yaml.h>

extern const char* g_DataPath;

#define HANDLER_LIST													\
	DEF_HANDLER(vtype, (VehicleType)atoi)								\
	DEF_HANDLER(surface_rad, atof)										\
	DEF_HANDLER(air_forward_speed,atof)									\
	DEF_HANDLER(air_max_forward_speed, atof)							\
	DEF_HANDLER(air_inc_forward_speed, atof)							\
	DEF_HANDLER(air_max_sidestep_speed, atof)							\
	DEF_HANDLER(air_inc_sidestep_speed, atof)							\
	DEF_HANDLER(air_rise_rot_speed, atof)								\
	DEF_HANDLER(air_spin_rot_speed, atof)								\
	DEF_HANDLER(air_inc_rot_speed, atof)								\
	DEF_HANDLER(air_max_rot_speed, atof)								\
	DEF_HANDLER(surface_max_speed, atof)								\
	DEF_HANDLER(surface_inc_speed, atof)								\
	DEF_HANDLER(surface_inc_sidestep_speed, atof)						\
	DEF_HANDLER(surface_rot_speed, atof)								\
	DEF_HANDLER(surface_inc_rot_speed, atof)							\
	DEF_HANDLER(surface_max_rot_speed, atof)							\
	DEF_HANDLER(laser_speed, atof)										\
	DEF_HANDLER(laser_life, atoi)										\
	DEF_HANDLER(laser_damage, atoi)										\
	DEF_HANDLER(laser_reload_time, atoi)								\
	DEF_HANDLER(frames_till_fire_laser, atoi)							\
	DEF_HANDLER(missile_speed, atof)									\
	DEF_HANDLER(turning_angle, atof)									\
	DEF_HANDLER(missile_life, atoi)										\
	DEF_HANDLER(missile_damage, atoi)									\
	DEF_HANDLER(missile_reload_time, atoi)								\
	DEF_HANDLER(frames_till_fire_missile, atoi)							\
	DEF_HANDLER(missile_generation_time, atoi)							\
	DEF_HANDLER(frames_till_new_missile, atoi)							\
	DEF_HANDLER(max_missile_storage, atoi)								\
	DEF_HANDLER(missiles_stored, atoi)									\
	DEF_HANDLER(max_projectiles, atoi)									\
	DEF_HANDLER(max_hitpoints, atoi)									\
	DEF_HANDLER(current_hitpoints, atoi)								\
	DEF_HANDLER(ramming_active, atoi)									\
	DEF_HANDLER(ramming_damage, atoi)									\
	DEF_HANDLER(double_lasers_active, atoi)								\
	DEF_HANDLER(mine_reload_time, atoi)									\
	DEF_HANDLER(mine_damage, atoi)										\
	DEF_HANDLER(mine_life, atoi)										\
	DEF_HANDLER(cs_missile_reload_time, atoi)							\
	DEF_HANDLER(cs_missile_life, atoi)									\
	DEF_HANDLER(cs_missile_speed, atof)									\
	DEF_HANDLER(controls_scrambled, atoi)								\
	DEF_HANDLER(frames_till_unscramble, atoi)							\
	DEF_HANDLER(scramble_life, atoi)									\
	DEF_HANDLER(traitor_missile_reload_time, atoi)						\
	DEF_HANDLER(traitor_missile_life, atoi)								\
	DEF_HANDLER(traitor_missile_speed, atof)							\
	DEF_HANDLER(traitor_life, atoi)										\
	DEF_HANDLER(traitor_active, atoi)									\
	DEF_HANDLER(frames_till_traitor_deactivate, atoi)					\
	DEF_HANDLER(anti_missile_active, atoi)								\
	DEF_HANDLER(cloaking_active, atoi)									\
	DEF_HANDLER(cloak_reload_time, atoi)								\
	DEF_HANDLER(frames_till_cloak, atoi)								\
	DEF_HANDLER(cloak_time, atoi)										\
	DEF_HANDLER(frames_till_cloak_suck, atoi)							\
	DEF_HANDLER(decoy_life, atoi)										\
	DEF_HANDLER(decoy_reload_time, atoi)

void Vehicle_Set(Vehicle* v, const char* key, const char* value)
{
#define DEF_HANDLER(field, func)					\
	if (strcmp(#field, key) == 0)					\
	{												\
		v->field = func(value);						\
		return;										\
	}

	HANDLER_LIST
}

#define MAX_TANKS 10

void Load_Tanks(Player* player, const game_configuration_type* gc)
{
	// append path to front of filename.
	char newfilename[512];
	sprintf(newfilename, "%sgamedata/tanks.yaml", g_DataPath);
	FILE* fp = fopen(newfilename, "rb");
	if (!fp)
		SYS_Error("Error loading Tanks!\n");

	// pre allocate the maximum number of tanks.
	Vehicle* tanks = (Vehicle*)malloc(sizeof(Vehicle) * MAX_TANKS);
	memset(tanks, 0, sizeof(Vehicle) * MAX_TANKS);

	yaml_parser_t parser;
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_file(&parser, fp);

	// NOTE: YAML file is expected to contain a sequence of maps.
	// One map per vehicle type.
	int done = 0;
	int tank_index = 0;
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
				Vehicle_Set(&tanks[tank_index], key, (char*)event.data.scalar.value);
			read_key = !read_key;
			break;

		case YAML_MAPPING_END_EVENT:
			assert(tank_index < MAX_TANKS);  // too many tanks in yaml file!
			tank_index++;
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

	for (int i = 0; i < 6; ++i)
	{
		int index;
		switch(i)
		{
		case 0: index = gc->red0_vehicle; break;
		case 1: index = gc->red1_vehicle; break;
		case 2: index = gc->red2_vehicle; break;
		case 3: index = gc->blue0_vehicle; break;
		case 4: index = gc->blue1_vehicle; break;
		case 5: index = gc->blue2_vehicle; break;
		}
		memcpy(&player[i].tank, &tanks[index], sizeof(Vehicle));
	}

	free(tanks);
}

void init_vehicle_gradient( Vehicle *tank )
{
    long i;

    if( tank->team == RED_TEAM ) {

        for( i = 0; i < tank->obj->faces; i++ ) {
            if( tank->obj->face[i].gradient == 0 )
                tank->obj->face[i].gradient = RedVehicleFirstGrad;
            else if( tank->obj->face[i].gradient == 1 )
                tank->obj->face[i].gradient = RedVehicleSecondGrad;
            else if( tank->obj->face[i].gradient == 2 )
                tank->obj->face[i].gradient = RedVehicleThirdGrad;
            else
                tank->obj->face[i].gradient = RedVehicleFirstGrad;
        }

        for( i = 0; i < tank->world_obj->faces; i++ ) {
            if( tank->world_obj->face[i].gradient == 0 )
                tank->world_obj->face[i].gradient = RedVehicleFirstGrad;
            else if( tank->world_obj->face[i].gradient == 1 )
                tank->world_obj->face[i].gradient = RedVehicleSecondGrad;
            else if( tank->world_obj->face[i].gradient == 2 )
                tank->world_obj->face[i].gradient = RedVehicleThirdGrad;
            else
                tank->world_obj->face[i].gradient = RedVehicleFirstGrad;
        }

    }
    else {

        for( i = 0; i < tank->obj->faces; i++ ) {
            if( tank->obj->face[i].gradient == 0 )
                tank->obj->face[i].gradient = BlueVehicleFirstGrad;
            else if( tank->obj->face[i].gradient == 1 )
                tank->obj->face[i].gradient = BlueVehicleSecondGrad;
            else if( tank->obj->face[i].gradient == 2 )
                tank->obj->face[i].gradient = BlueVehicleThirdGrad;
            else
                tank->obj->face[i].gradient = BlueVehicleFirstGrad;
        }

        for( i = 0; i < tank->world_obj->faces; i++ ) {
            if( tank->world_obj->face[i].gradient == 0 )
                tank->world_obj->face[i].gradient = BlueVehicleFirstGrad;
            else if( tank->world_obj->face[i].gradient == 1 )
                tank->world_obj->face[i].gradient = BlueVehicleSecondGrad;
            else if( tank->world_obj->face[i].gradient == 2 )
                tank->world_obj->face[i].gradient = BlueVehicleThirdGrad;
            else
                tank->world_obj->face[i].gradient = BlueVehicleFirstGrad;
        }
    }
}



void Init_Tank( Vehicle *tank, enum VehicleType tank_type, team_type team )
    {

    tank->vtype = tank_type;
    tank->team = team;

    if( (tank->obj = (PointFace *) malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("Init_Tank() : malloc failed\n");

    tank->obj->point = NULL;
    tank->obj->points = 0;
    tank->obj->face = NULL;
    tank->obj->faces = 0;

    if( (tank->collision_obj = (PointFace *) malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("Init_Tank() : malloc failed\n");

    tank->collision_obj->point = NULL;
    tank->collision_obj->points = 0;
    tank->collision_obj->face = NULL;
    tank->collision_obj->faces = 0;

    switch( tank_type )
        {
         case Wasp :
             get_object( tank->obj, "3d_data/wasp.pfd" );
             get_object( tank->collision_obj, "3d_data/cwasp.pfd" );
             break;
         case Beetle :
             get_object( tank->obj, "3d_data/beetle.pfd" );
             get_object( tank->collision_obj, "3d_data/cbeetle.pfd" );
             break;
         case Flea :
             get_object( tank->obj, "3d_data/flea.pfd" );
             get_object( tank->collision_obj, "3d_data/cflea.pfd" );
             break;
         case Mosquito :
             get_object( tank->obj, "3d_data/mosquito.pfd" );
             get_object( tank->collision_obj, "3d_data/cmosquit.pfd" );
             break;
         case Spider :
             get_object( tank->obj, "3d_data/spider.pfd" );
             get_object( tank->collision_obj, "3d_data/cspider.pfd" );
             break;
         case Dragonfly :
             get_object( tank->obj, "3d_data/dragon.pfd" );
             get_object( tank->collision_obj, "3d_data/cdragon.pfd" );
             break;
         case Roach :
             get_object( tank->obj, "3d_data/roach.pfd" );
             get_object( tank->collision_obj, "3d_data/croach.pfd" );
             break;
         case Locust :
             get_object( tank->obj, "3d_data/locust.pfd" );
             get_object( tank->collision_obj, "3d_data/clocust.pfd" );
             break;

         default :
             SYS_Error("Invalid tank number in Init_Tank \n");
             break;

        } /* End switch */

    if( ( tank->world_obj = (PointFace *) malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("Init_Tank() : malloc failed\n");

    tank->world_obj->point = NULL;
    tank->world_obj->points = 0;
    tank->world_obj->face = NULL;
    tank->world_obj->faces = 0;

    init_world_obj( tank );

    init_vehicle_gradient( tank );

    if( ( tank->world_collision_obj = (PointFace *) malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("Init_Tank() : malloc failed\n");

    tank->world_collision_obj->point = NULL;
    tank->world_collision_obj->points = 0;
    tank->world_collision_obj->face = NULL;
    tank->world_collision_obj->faces = 0;

    init_world_collision_obj( tank );

    init_bounding_box( tank );

    tank->collision_edges.edge = NULL;
    tank->collision_edges.edges = 0;

    init_edge_table( tank );

    tank->alive = TRUE;
    tank->target = NULL;
    tank->vehicle_mode = Surface;

} /* End of Init_Tank */


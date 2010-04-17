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

extern const char* g_DataPath;

static const int NUM_LINES_PER_TANK = 61;
static const int NUM_TANKS = 10;
struct tank_lines
{
	char* lines[NUM_LINES_PER_TANK];
};

void Load_Tanks(Player* player, const game_configuration_type* gc)
{
	// append path to front of filename.
	char newfilename[512];
	sprintf(newfilename, "%sgamedata/new_tanks.dat", g_DataPath);
	FILE* fp = fopen(newfilename, "rb");
	if (!fp)
		SYS_Error("Error loading Tanks!\n");

	struct tank_lines* t_lines = (struct tank_lines*)malloc(sizeof(struct tank_lines) * NUM_TANKS);

	// load in the WHOLE thing.
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	char* buffer = (char*)malloc(sizeof(char) * (size + 1));
	fseek(fp, 0, SEEK_SET);
	fread(buffer, sizeof(char), size, fp);

	int line_count = 0;
	int t_count = 0;
	char* line = strtok(buffer, "\n");
	while (line)
	{
		// skip comment lines
		if (line[0] == '#')
			continue;

		t_lines[t_count].lines[line_count++] = line;

		// move on to next tank
		if (line_count == NUM_LINES_PER_TANK)
		{
			line_count = 0;
			t_count++;
		}
		line = strtok(0, "\n");
	}

	int i, j;
	for (i = 0; i < 6; i++)
	{
		Vehicle* v = &player[i].tank;
		memset(v, 0, sizeof(Vehicle));
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
		int l = 0;
		v->vtype = (VehicleType)atoi(t_lines[index].lines[l++]);
		v->surface_rad = atof(t_lines[index].lines[l++]);

		v->air_forward_speed = atof(t_lines[index].lines[l++]);
		v->air_max_forward_speed = atof(t_lines[index].lines[l++]);
		v->air_inc_forward_speed = atof(t_lines[index].lines[l++]);
		v->air_max_sidestep_speed = atof(t_lines[index].lines[l++]);
		v->air_inc_sidestep_speed = atof(t_lines[index].lines[l++]);

		v->air_rise_rot_speed = atof(t_lines[index].lines[l++]);
		v->air_spin_rot_speed = atof(t_lines[index].lines[l++]);
		v->air_inc_rot_speed = atof(t_lines[index].lines[l++]);
		v->air_max_rot_speed = atof(t_lines[index].lines[l++]);
		v->surface_max_speed = atof(t_lines[index].lines[l++]);
		v->surface_inc_speed = atof(t_lines[index].lines[l++]);
		v->surface_inc_sidestep_speed = atof(t_lines[index].lines[l++]);

		v->surface_rot_speed = atof(t_lines[index].lines[l++]);
		v->surface_inc_rot_speed = atof(t_lines[index].lines[l++]);
		v->surface_max_rot_speed = atof(t_lines[index].lines[l++]);

		v->laser_speed = atof(t_lines[index].lines[l++]);
		v->laser_life = atoi(t_lines[index].lines[l++]);
		v->laser_damage = atoi(t_lines[index].lines[l++]);
		v->laser_reload_time = atoi(t_lines[index].lines[l++]);
		v->frames_till_fire_laser = atoi(t_lines[index].lines[l++]);

		v->missile_speed = atof(t_lines[index].lines[l++]);
		v->turning_angle = atof(t_lines[index].lines[l++]);
		v->missile_life = atoi(t_lines[index].lines[l++]);
		v->missile_damage = atoi(t_lines[index].lines[l++]);
		v->missile_reload_time = atoi(t_lines[index].lines[l++]);
		v->frames_till_fire_missile = atoi(t_lines[index].lines[l++]);
		v->missile_generation_time = atoi(t_lines[index].lines[l++]);
		v->frames_till_new_missile = atoi(t_lines[index].lines[l++]);
		v->max_missile_storage = atoi(t_lines[index].lines[l++]);
		v->missiles_stored = atoi(t_lines[index].lines[l++]);

		v->max_projectiles = atoi(t_lines[index].lines[l++]);

		v->max_hitpoints = atoi(t_lines[index].lines[l++]);
		v->current_hitpoints = atoi(t_lines[index].lines[l++]);

		v->ramming_active = atoi(t_lines[index].lines[l++]);
		v->ramming_damage = atoi(t_lines[index].lines[l++]);

		v->double_lasers_active = atoi(t_lines[index].lines[l++]);

		v->mine_reload_time = atoi(t_lines[index].lines[l++]);
		v->mine_damage = atoi(t_lines[index].lines[l++]);
		v->mine_life = atoi(t_lines[index].lines[l++]);

		v->cs_missile_reload_time = atoi(t_lines[index].lines[l++]);
		v->cs_missile_life = atoi(t_lines[index].lines[l++]);
		v->cs_missile_speed = atof(t_lines[index].lines[l++]);

		v->controls_scrambled = atoi(t_lines[index].lines[l++]);
		v->frames_till_unscramble = atoi(t_lines[index].lines[l++]);
		v->scramble_life = atoi(t_lines[index].lines[l++]);

		v->traitor_missile_reload_time = atoi(t_lines[index].lines[l++]);
		v->traitor_missile_life = atoi(t_lines[index].lines[l++]);
		v->traitor_missile_speed = atof(t_lines[index].lines[l++]);

		v->traitor_life = atoi(t_lines[index].lines[l++]);
		v->traitor_active = atoi(t_lines[index].lines[l++]);
		v->frames_till_traitor_deactivate = atoi(t_lines[index].lines[l++]);

		v->anti_missile_active = atoi(t_lines[index].lines[l++]);

		v->cloaking_active = atoi(t_lines[index].lines[l++]);
		v->cloak_reload_time = atoi(t_lines[index].lines[l++]);
		v->frames_till_cloak = atoi(t_lines[index].lines[l++]);
		v->cloak_time = atoi(t_lines[index].lines[l++]);
		v->frames_till_cloak_suck = atoi(t_lines[index].lines[l++]);

		v->decoy_life = atoi(t_lines[index].lines[l++]);
		v->decoy_reload_time = atoi(t_lines[index].lines[l++]);
	}
	
	free(t_lines);
	free(buffer);
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


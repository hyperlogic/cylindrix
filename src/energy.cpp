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
#include "energy.h"
#include "ai_util.h"
#include "util.h"
#include "object.h"
#include <stdio.h>
#include <stdlib.h>
#include "jonsb.h"

extern game_configuration_type game_configuration;

extern level_type level; /* This holds the values we load from the level file */

/* initializes the EnergyPowerUp data structure */

void init_energy_power_up( EnergyPowerUp *e )
{
    Float_Point temp;
    Float_Point2d temp2d;
    long i;

    e->refresh_rate = 0;
    e->frames_till_burst = 0;

    /* malloc space for the pointface */

    if( ( e->world_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
		SYS_Error("init_energy_power_up() : malloc failed\n");

    /* load the pointface */

    e->world_obj->point = NULL;
    e->world_obj->points = 0;
    e->world_obj->face = NULL;
    e->world_obj->faces = 0;

    get_object( e->world_obj, level.energy_square_filename );

    for( i = 0; i < e->world_obj->faces; i++ ) {
	e->world_obj->face[i].gradient = EnergySquareGrad;
    }

    /* initialize the energy power up struct */

    temp[X] = mtof( e->world_obj->point[0][X] );
    temp[Y] = mtof( e->world_obj->point[0][Y] );
    temp[Z] = mtof( e->world_obj->point[0][Z] );

    Cylinder_x_y( temp, temp2d );

    e->box.min_x = temp2d[X];
    e->box.max_y = temp2d[Y];

    temp[X] = mtof( e->world_obj->point[3][X] );
    temp[Y] = mtof( e->world_obj->point[3][Y] );
    temp[Z] = mtof( e->world_obj->point[3][Z] );

    Cylinder_x_y( temp, temp2d );

    e->box.max_x = temp2d[X];
    e->box.min_y = temp2d[Y];

    temp[X] = 0.0;
    temp[Y] = 0.0;
    temp[Z] = 0.0;

    for( i = 0; i < e->world_obj->points; i++ ) {
	temp[X] += mtof( e->world_obj->point[i][X] );
	temp[Y] += mtof( e->world_obj->point[i][Y] );
	temp[Z] += mtof( e->world_obj->point[i][Z] );
    }
    e->center[X] = temp[X] / e->world_obj->points;
    e->center[Y] = temp[Y] / e->world_obj->points;
    e->center[Z] = temp[Z] / e->world_obj->points;
}

/* give hitpoints to any vehicles that are sitting on the energy square */

void update_energy( EnergyPowerUp *e, Player *p )
{
    Float_Point2d temp;
    long i;

    for( i = 0; i < 6; i++ ) {

	if( p[i].tank.alive ) {
	    Cylinder_x_y( p[i].tank.orient.position, temp );

	    if( inside_bounding_box2d( temp, &(e->box) ) ) {
		if( e->frames_till_burst == 0 ) {
		    p[i].tank.current_hitpoints += 1;
		    if( game_configuration.sound_on )
			{
			/* sb_mix_sample(sample[ ENERGY_RECHARGE ]); */
			 Q_Jon_Sample( ENERGY_RECHARGE, p[i].tank.orient.position );
			}

		    if( p[i].tank.current_hitpoints > p[i].tank.max_hitpoints ) {
			p[i].tank.current_hitpoints = p[i].tank.max_hitpoints;
		    }
		}
	    }
	}
    }

    if( e->frames_till_burst == 0 ) {
	e->frames_till_burst = e->refresh_rate;
    }
    else {
	e->frames_till_burst--;
    }
}


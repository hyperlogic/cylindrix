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
#include "project.h"
#include "object.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "movement.h"
#include "pylon.h"
#include "collide.h"
#include "util.h"
#include "explode.h"
#include "jonsb.h"
#include "stats.h"
#include "keys.h"

/* updates one vehicles target field.  So that missiles will lock on the
   closest visible non-cloaked enemy vehicle */

/* player array 0, 1, 2 are assumed to be red_team and 3, 4, 5 are blue_team */

extern game_stats_type game_stats; /* From omega.c */

extern game_configuration_type game_configuration;
extern WorldStuff world_stuff;

void update_missile_target( Player *player_array, long index,
			    RadarBase *red_base, RadarBase *blue_base )
{
    long enemy_found, i;
    float len, temp, *enemy_ptr = NULL;
    Float_Point pos;
    Projectile *p;
    static Float_Point target_pos;

    if( player_array[index].tank.team == RED_TEAM ) {

	if( red_base->alive ) {

	    /* find the nearest living visible enemy */

	    enemy_found = FALSE;
	    len = 10000.0;

	    if( player_array[3].tank.alive && (!player_array[3].tank.cloaking_active) &&
		point_visible( &player_array[index].tank.orient,
		player_array[3].tank.orient.position ) ) {

		enemy_found = TRUE;
		enemy_ptr = player_array[3].tank.orient.position;
		len = distance_between(
			  player_array[index].tank.orient.position,
			  player_array[3].tank.orient.position );
	    }

	    if( player_array[4].tank.alive && (!player_array[4].tank.cloaking_active) &&
		point_visible( &player_array[index].tank.orient, player_array[4].tank.orient.position ) &&
		( len > (temp = distance_between( player_array[index].tank.orient.position, player_array[4].tank.orient.position ))) ) {

		enemy_found = TRUE;
		enemy_ptr = player_array[4].tank.orient.position;
		len = temp;
	    }

	    if( player_array[5].tank.alive && (!player_array[5].tank.cloaking_active) &&
		point_visible( &player_array[index].tank.orient,
		player_array[5].tank.orient.position ) &&
		( len > (temp = distance_between(
		player_array[index].tank.orient.position,
		player_array[5].tank.orient.position )))) {

		enemy_found = TRUE;
		enemy_ptr = player_array[5].tank.orient.position;
		len = temp;
	    }

	    if( blue_base->alive &&
		point_visible( &player_array[index].tank.orient,
		blue_base->base_orient.position ) &&
		( len > (temp = distance_between(
		player_array[index].tank.orient.position,
		blue_base->base_orient.position )))) {

		enemy_found = TRUE;
		enemy_ptr = blue_base->base_orient.position;
		len = temp;
	    }

            /* search for decoys */

            for( i = 3; i < 6; i++ ) {

                p = player_array[i].tank.projectile_list;

                while( p != NULL ) {

                    if( (p->type == DECOY_0) || (p->type == DECOY_1) || (p->type == DECOY_2) ) {

                        pos[X] = mtof( p->orient.position[X] );
                        pos[Y] = mtof( p->orient.position[Y] );
                        pos[Z] = mtof( p->orient.position[Z] );

                        if( point_visible( &player_array[index].tank.orient, pos ) &&
                            ( len > ( temp = distance_between( player_array[index].tank.orient.position, pos ) ) ) ) {

                            target_pos[X] = pos[X];
                            target_pos[Y] = pos[Y];
                            target_pos[Z] = pos[Z];

                            enemy_found = TRUE;
                            enemy_ptr = target_pos;
                            len = temp;
                        }
                    }
                    p = p->next;
                }
            }


	    if( enemy_found ) {
		player_array[index].tank.target = enemy_ptr;
	    }
	    else {
		player_array[index].tank.target = NULL;
	    }
	}
	else {
	    player_array[index].tank.target = NULL;
	}
    }
    else if( player_array[index].tank.team == BLUE_TEAM ) {

	if( blue_base->alive ) {

	    /* find the nearest living visible enemy */

	    enemy_found = FALSE;
	    len = 10000.0;

	    if( player_array[0].tank.alive && (!player_array[0].tank.cloaking_active) &&
		point_visible( &player_array[index].tank.orient,
		player_array[0].tank.orient.position ) ) {

		enemy_found = TRUE;
		enemy_ptr = player_array[0].tank.orient.position;
		len = distance_between(
			  player_array[index].tank.orient.position,
			  player_array[0].tank.orient.position );
	    }

	    if( player_array[1].tank.alive && (!player_array[1].tank.cloaking_active) &&
		point_visible( &player_array[index].tank.orient,
		player_array[1].tank.orient.position ) &&
		( len > (temp = distance_between(
		player_array[index].tank.orient.position,
		player_array[1].tank.orient.position )) ) ) {

		enemy_found = TRUE;
		enemy_ptr = player_array[1].tank.orient.position;
		len = temp;
	    }

	    if( player_array[2].tank.alive && (!player_array[2].tank.cloaking_active) &&
		point_visible( &player_array[index].tank.orient,
		player_array[2].tank.orient.position ) &&
		( len > (temp = distance_between(
		player_array[index].tank.orient.position,
		player_array[2].tank.orient.position )))) {

		enemy_found = TRUE;
		enemy_ptr = player_array[2].tank.orient.position;
		len = temp;
	    }

	    if( red_base->alive &&
		point_visible( &player_array[index].tank.orient,
		red_base->base_orient.position ) &&
		( len > (temp = distance_between(
		player_array[index].tank.orient.position,
		red_base->base_orient.position )))) {

		enemy_found = TRUE;
		enemy_ptr = red_base->base_orient.position;
		len = temp;
	    }

	    /* search for decoys */

	    for( i = 0; i < 3; i++ ) {

                p = player_array[i].tank.projectile_list;

                while( p != NULL ) {

                    if( (p->type == DECOY_0) || (p->type == DECOY_1) || (p->type == DECOY_2) ) {

                        pos[X] = mtof( p->orient.position[X] );
                        pos[Y] = mtof( p->orient.position[Y] );
                        pos[Z] = mtof( p->orient.position[Z] );

                        if( point_visible( &player_array[index].tank.orient, pos ) &&
                            ( len > ( temp = distance_between( player_array[index].tank.orient.position, pos ) ) ) ) {

                            target_pos[X] = pos[X];
                            target_pos[Y] = pos[Y];
                            target_pos[Z] = pos[Z];

                            enemy_found = TRUE;
                            enemy_ptr = target_pos;
                            len = temp;
                        }
                    }
                    p = p->next;
                }
            }

	    if( enemy_found ) {
		player_array[index].tank.target = enemy_ptr;
	    }
	    else {
		player_array[index].tank.target = NULL;
	    }
	}
	else {
	    player_array[index].tank.target = NULL;
	}
    }
}

void create_projectile( Vehicle *v, PointFace *obj, enum projectile_type type )
{
    Projectile *p;
    short count = 0;
    float projectile_speed;
    Gradient bullet_color = 0; /* = 0 to shut up compiler */
    float *target = v->target;
    Float_Vector left;
    int i;
    unsigned char owner = 99;

    /* find the owner of this projectile */

    for( i = 0; i < 6; i++ ) {
        if( v == &(world_stuff.player_array[i].tank) ) {
            owner = i;
            break;
        }
    }

    /* Based on the amount of damage a laser can do...we will
       change the color of the projectile */

    switch (v->laser_damage)
        {
         case 0 :
             bullet_color = LaserGrad0;
             break;
         case 1 :
             bullet_color = LaserGrad1;
             break;
         case 2 :
             bullet_color = LaserGrad2;
             break;
         case 3 :
             bullet_color = LaserGrad3;
             break;
         case 4 :
             bullet_color = LaserGrad4;
             break;
         case 5 :
             bullet_color = LaserGrad5;
             break;
         case 6 :
             bullet_color = LaserGrad6;
             break;
         case 7 :
             bullet_color = LaserGrad7;
             break;
         case 8 :
             bullet_color = LaserGrad8;
             break;
         case 9 :
             bullet_color = LaserGrad9;
             break;
         case 10 :
             bullet_color = LaserGrad10;
             break;
         default :
             break;
        } /* End switch */


    p = v->projectile_list;

    /* count the number of projectiles in the list */

    while( p != NULL ) {
        count++;
        p = p->next;
    }

    if( (count < v->max_projectiles) && v->double_lasers_active && (type == LASER) ) {

        if( game_configuration.sound_on ) {
            Q_Jon_Sample( BULLET_FIRE, v->orient.position );
        }


        /* STAT SHIT */
        if( owner == game_stats.user_vehicle )
            game_stats.shots_fired++;

        /* create 2 new laser projectiles */

        vector_cross_product( v->orient.up, v->orient.front, left );
        normalize( left );

        /* create a new projectile */

        if( count == 0 ) {
            v->projectile_list = (Projectile *)malloc( sizeof( Projectile ) );
            p = v->projectile_list;
        }
        else {
            p = v->projectile_list;

            while( p->next != NULL ) {
                p = p->next;
            }
            p->next = (Projectile *)malloc( sizeof( Projectile ) );
            p = p->next;
        }

        /* intialize it */

        p->next = NULL;
        p->type = type;
        p->owner = owner;

        /* malloc space for the PointFace */

        if( ( p->obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the point array */

        if( ( p->obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* copy the points from obj to p->obj */

        memcpy( p->obj->point, obj->point, sizeof( Point ) * obj->points );
        p->obj->points = obj->points;

        /* copy the faces from obj to p->obj */

        memcpy( p->obj->face, obj->face, sizeof( Face ) * obj->faces );
        p->obj->faces = obj->faces;

        if( type == LASER ) {
            p->frames_left = v->laser_life;
        }
        else if( type == MISSILE ) {
            p->frames_left = v->missile_life;
        }
        else if( type == MINE ) {
            p->frames_left = v->mine_life;
        }

        p->orient.front[X] = ftom( v->orient.front[X] );
        p->orient.front[Y] = ftom( v->orient.front[Y] );
        p->orient.front[Z] = ftom( v->orient.front[Z] );

        p->orient.up[X] = ftom( v->orient.up[X] );
        p->orient.up[Y] = ftom( v->orient.up[Y] );
        p->orient.up[Z] = ftom( v->orient.up[Z] );

        p->orient.position[X] = ftom( (v->orient.position[X] +
            ( (v->surface_rad - 9.85f) * v->orient.up[X])) + (0.25f * left[X]));
        p->orient.position[Y] = ftom( (v->orient.position[Y] +
            ( (v->surface_rad - 9.85f) * v->orient.up[Y])) + (0.25f * left[Y]));
        p->orient.position[Z] = ftom( (v->orient.position[Z] +
            ( (v->surface_rad - 9.85f) * v->orient.up[Z])) + (0.25f * left[Z]));

        /* move it forward a tad */

        projectile_speed = v->laser_speed;
        v->laser_speed = 0.6f;

        if( v->vehicle_mode == Surface ) {
            move_projectile_forward_surface( p, v );
        }
        else {
            move_projectile_forward_3d( p, v );
        }
        v->laser_speed = projectile_speed;

        /* malloc space for the pointface */

        if( ( p->world_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("create_projectile : malloc_failed\n");

        /* malloc space for the point array */

        if( ( p->world_obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->world_obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        p->edge_table.edge = NULL;
        init_projectile_edge_table( p );

        if( v->vehicle_mode == Surface )
            p->on_surface = TRUE;
        else
            p->on_surface = FALSE;

        /* Change the color of all of the lasers faces */

        if( type == LASER ) {
            for( i = 0; i < p->obj->faces; i++ ) {
                p->obj->face[i].gradient = bullet_color;
            }
        }

        count++;

        /* create a second new projectile */

        if( count == 0 ) {
            v->projectile_list = (Projectile *)malloc( sizeof( Projectile ) );
            p = v->projectile_list;
        }
        else {
            p = v->projectile_list;

            while( p->next != NULL ) {
                p = p->next;
            }
            p->next = (Projectile *)malloc( sizeof( Projectile ) );
            p = p->next;
        }

        /* intialize it */

        p->next = NULL;
        p->type = type;
        p->owner = owner;

        /* malloc space for the PointFace */

        if( ( p->obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the point array */

        if( ( p->obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* copy the points from obj to p->obj */

        memcpy( p->obj->point, obj->point, sizeof( Point ) * obj->points );
        p->obj->points = obj->points;

        /* copy the faces from obj to p->obj */

        memcpy( p->obj->face, obj->face, sizeof( Face ) * obj->faces );
        p->obj->faces = obj->faces;

        if( type == LASER ) {
            p->frames_left = v->laser_life;
        }
        else if( type == MISSILE ) {
            p->frames_left = v->missile_life;
        }
        else if( type == MINE ) {
            p->frames_left = v->mine_life;
        }

        p->orient.front[X] = ftom( v->orient.front[X] );
        p->orient.front[Y] = ftom( v->orient.front[Y] );
        p->orient.front[Z] = ftom( v->orient.front[Z] );

        p->orient.up[X] = ftom( v->orient.up[X] );
        p->orient.up[Y] = ftom( v->orient.up[Y] );
        p->orient.up[Z] = ftom( v->orient.up[Z] );

        p->orient.position[X] = ftom( (v->orient.position[X] +
            ( (v->surface_rad - 9.85f) * v->orient.up[X])) - (0.25f * left[X]));
        p->orient.position[Y] = ftom( (v->orient.position[Y] +
            ( (v->surface_rad - 9.85f) * v->orient.up[Y])) - (0.25f * left[Y]));
        p->orient.position[Z] = ftom( (v->orient.position[Z] +
            ( (v->surface_rad - 9.85f) * v->orient.up[Z])) - (0.25f * left[Z]));

        /* move it forward a tad */

        projectile_speed = v->laser_speed;
        v->laser_speed = 0.6f;

        if( v->vehicle_mode == Surface ) {
            move_projectile_forward_surface( p, v );
        }
        else {
            move_projectile_forward_3d( p, v );
        }
        v->laser_speed = projectile_speed;

        /* malloc space for the pointface */

        if( ( p->world_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("create_projectile : malloc_failed\n");

        /* malloc space for the point array */

        if( ( p->world_obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->world_obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        p->edge_table.edge = NULL;
        init_projectile_edge_table( p );

        if( v->vehicle_mode == Surface )
            p->on_surface = TRUE;
        else
            p->on_surface = FALSE;

        /* Change the color of all of the lasers faces */

        if( type == LASER ) {
            for( i = 0; i < p->obj->faces; i++ ) {
                p->obj->face[i].gradient = bullet_color;
            }
        }


    }
    else if( count < v->max_projectiles ) {

        /* create a new projectile */

        /* STAT SHIT */
        if( owner == game_stats.user_vehicle )
            game_stats.shots_fired++;


        if( count == 0 ) {
            v->projectile_list = (Projectile *)malloc( sizeof( Projectile ) );
            p = v->projectile_list;
        }
        else {
            p = v->projectile_list;

            while( p->next != NULL ) {
                p = p->next;
            }
            p->next = (Projectile *)malloc( sizeof( Projectile ) );
            p = p->next;
        }

        /* intialize it */

        p->next = NULL;
        p->type = type;
        p->owner = owner;

        /* malloc space for the PointFace */

        if( ( p->obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the point array */

        if( ( p->obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* copy the points from obj to p->obj */

        memcpy( p->obj->point, obj->point, sizeof( Point ) * obj->points );
        p->obj->points = obj->points;

        /* copy the faces from obj to p->obj */

        memcpy( p->obj->face, obj->face, sizeof( Face ) * obj->faces );
        p->obj->faces = obj->faces;

        if( type == LASER ) {
            p->frames_left = v->laser_life;
        }
        else if( type == MISSILE ) {
            p->frames_left = v->missile_life;
        }
        else if( type == MINE ) {
            p->frames_left = v->mine_life;
        }
        else if( type == CS_MISSILE ) {
            p->frames_left = v->cs_missile_life;
        }
        else if( type == TRAITOR_MISSILE ) {
            p->frames_left = v->traitor_missile_life;
        }

        p->orient.front[X] = ftom( v->orient.front[X] );
        p->orient.front[Y] = ftom( v->orient.front[Y] );
        p->orient.front[Z] = ftom( v->orient.front[Z] );

        p->orient.up[X] = ftom( v->orient.up[X] );
        p->orient.up[Y] = ftom( v->orient.up[Y] );
        p->orient.up[Z] = ftom( v->orient.up[Z] );

        if( type == LASER ) {

            if( game_configuration.sound_on ) {
                Q_Jon_Sample( BULLET_FIRE, v->orient.position );
            }

            p->orient.position[X] = ftom( v->orient.position[X] +
                ( (v->surface_rad - 9.85f) * v->orient.up[X]) );
            p->orient.position[Y] = ftom( v->orient.position[Y] +
                ( (v->surface_rad - 9.85f) * v->orient.up[Y]) );
            p->orient.position[Z] = ftom( v->orient.position[Z] +
                ( (v->surface_rad - 9.85f) * v->orient.up[Z]) );

            /* move it forward a tad */

            projectile_speed = v->laser_speed;
            v->laser_speed = 0.6f;

            if( v->vehicle_mode == Surface ) {
                move_projectile_forward_surface( p, v );
            }
            else {
                move_projectile_forward_3d( p, v );
            }
            v->laser_speed = projectile_speed;

        }
        else if( type == MISSILE ) {

            if( game_configuration.sound_on ) {
                Q_Jon_Sample( MISSILE_FIRE, v->orient.position );
            }

            p->orient.position[X] = ftom( v->orient.position[X] +
                ( (v->surface_rad - 9.6f) * v->orient.up[X]) );
            p->orient.position[Y] = ftom( v->orient.position[Y] +
                ( (v->surface_rad - 9.6f) * v->orient.up[Y]) );
            p->orient.position[Z] = ftom( v->orient.position[Z] +
                ( (v->surface_rad - 9.6f) * v->orient.up[Z]) );

            /* move it forward a tad */

            projectile_speed = v->missile_speed;
            v->missile_speed = 0.9f;

            move_projectile_forward_3d( p, v );

            v->missile_speed = projectile_speed;

            p->target = target;
        }
        else if( type == MINE ) {
            p->orient.position[X] = ftom( v->orient.position[X] - (0.5f * v->orient.front[X]) );
            p->orient.position[Y] = ftom( v->orient.position[Y] - (0.5f * v->orient.front[Y]) );
            p->orient.position[Z] = ftom( v->orient.position[Z] - (0.5f * v->orient.front[Z]) );
        }
        else if( type == CS_MISSILE ) {

            if( game_configuration.sound_on ) {
                Q_Jon_Sample( MISSILE_FIRE, v->orient.position );
            }

            p->orient.position[X] = ftom( v->orient.position[X] +
                ( (v->surface_rad - 9.6f) * v->orient.up[X]) );
            p->orient.position[Y] = ftom( v->orient.position[Y] +
                ( (v->surface_rad - 9.6f) * v->orient.up[Y]) );
            p->orient.position[Z] = ftom( v->orient.position[Z] +
                ( (v->surface_rad - 9.6f) * v->orient.up[Z]) );

            /* move it forward a tad */

            projectile_speed = v->cs_missile_speed;
            v->missile_speed = 0.9f;

            move_projectile_forward_3d( p, v );

            v->cs_missile_speed = projectile_speed;
        }
        else if( type == TRAITOR_MISSILE ) {

            if( game_configuration.sound_on ) {
                Q_Jon_Sample( MISSILE_FIRE, v->orient.position );
            }

            p->orient.position[X] = ftom( v->orient.position[X] +
                ( (v->surface_rad - 9.6f) * v->orient.up[X]) );
            p->orient.position[Y] = ftom( v->orient.position[Y] +
                ( (v->surface_rad - 9.6f) * v->orient.up[Y]) );
            p->orient.position[Z] = ftom( v->orient.position[Z] +
                ( (v->surface_rad - 9.6f) * v->orient.up[Z]) );

            /* move it forward a tad */

            projectile_speed = v->cs_missile_speed;
            v->missile_speed = 0.9f;

            move_projectile_forward_3d( p, v );

            v->cs_missile_speed = projectile_speed;
        }

        if( ( p->world_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("create_projectile : malloc_failed\n");

        /* malloc space for the point array */

        if( ( p->world_obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->world_obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("create_projectile() : malloc failed\n");

        p->edge_table.edge = NULL;
        init_projectile_edge_table( p );

        if( v->vehicle_mode == Surface )
            p->on_surface = TRUE;
        else
            p->on_surface = FALSE;

        /* Change the color of all of the lasers faces */

        if( type == LASER ) {
            for( i = 0; i < p->obj->faces; i++ ) {
                p->obj->face[i].gradient = bullet_color;
            }
        }
    }
}

void create_decoy_projectile( Vehicle *v, long type )
{
    Projectile *p;
    short count = 0;
    unsigned char i;
    unsigned char owner = 99;

    /* find the owner of this projectile */

    for( i = 0; i < 6; i++ ) {
        if( v == &(world_stuff.player_array[i].tank) ) {
            owner = i;
            break;
        }
    }

    p = v->projectile_list;

    /* count the number of projectiles in the list */

    while( p != NULL ) {
        count++;
        p = p->next;
    }

    /* create a new projectile */

    if( count == 0 ) {
        v->projectile_list = (Projectile *)malloc( sizeof( Projectile ) );
        p = v->projectile_list;
    }
    else {
        p = v->projectile_list;

        while( p->next != NULL ) {
            p = p->next;
        }
        p->next = (Projectile *)malloc( sizeof( Projectile ) );
        p = p->next;
    }

    /* intialize it */

    p->next = NULL;

    if( type == 0 ) {
        p->type = DECOY_0;
    }
    else if( type == 1 ) {
        p->type = DECOY_1;
    }
    else {
        p->type = DECOY_2;
    }

    /* malloc space for the PointFace */

    if( ( p->obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("create_decoy_projectile() : malloc failed\n");

    /* malloc space for the point array */

    if( ( p->obj->point = (Point *)malloc( sizeof( Point ) * v->obj->points ) ) == NULL )
        SYS_Error("create_decoy_projectile() : malloc failed\n");

    /* malloc space for the face array */

    if( ( p->obj->face = (Face *)malloc( sizeof( Face ) * v->obj->faces ) ) == NULL )
        SYS_Error("create_decoy_projectile() : malloc failed\n");

    /* copy the points from v->obj to p->obj */

    memcpy( p->obj->point, v->obj->point, sizeof( Point ) * v->obj->points );
    p->obj->points = v->obj->points;

    /* copy the faces from v->obj to p->obj */

    memcpy( p->obj->face, v->obj->face, sizeof( Face ) * v->obj->faces );
    p->obj->faces = v->obj->faces;

    /* initialize projectile life */

    p->frames_left = v->decoy_life;

    p->orient.front[X] = ftom( v->orient.front[X] );
    p->orient.front[Y] = ftom( v->orient.front[Y] );
    p->orient.front[Z] = ftom( v->orient.front[Z] );

    p->orient.up[X] = ftom( v->orient.up[X] );
    p->orient.up[Y] = ftom( v->orient.up[Y] );
    p->orient.up[Z] = ftom( v->orient.up[Z] );

    p->orient.position[X] = ftom( v->orient.position[X] );
    p->orient.position[Y] = ftom( v->orient.position[Y] );
    p->orient.position[Z] = ftom( v->orient.position[Z] );

    p->vel[X] = v->vel[X];
    p->vel[Y] = v->vel[Y];
    p->vel[Z] = v->vel[Z];

    p->air_rise_rot_speed = v->air_rise_rot_speed;
    p->air_spin_rot_speed = v->air_spin_rot_speed;
    p->surface_rot_speed = v->surface_rot_speed;
    p->owner = owner;

    /* play decoy sound */

    if( ( p->world_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("create_projectile : malloc_failed\n");

    /* malloc space for the point array */

    if( ( p->world_obj->point = (Point *)malloc( sizeof( Point ) * v->obj->points ) ) == NULL )
        SYS_Error("create_projectile() : malloc failed\n");

    /* malloc space for the face array */

    if( ( p->world_obj->face = (Face *)malloc( sizeof( Face ) * v->obj->faces ) ) == NULL )
        SYS_Error("create_decoy_projectile() : malloc failed\n");

    p->edge_table.edge = NULL;
    init_projectile_edge_table( p );

    if( v->vehicle_mode == Surface )
        p->on_surface = TRUE;
    else
        p->on_surface = FALSE;
}

void move_projectile_forward_3d( Projectile *p, Vehicle *v )
{
    if( p->type == LASER ) {
        p->orient.position[X] = p->orient.position[X] + ((p->orient.front[X] * ftom( v->laser_speed )) >> MEXP);
        p->orient.position[Y] = p->orient.position[Y] + ((p->orient.front[Y] * ftom( v->laser_speed )) >> MEXP);
        p->orient.position[Z] = p->orient.position[Z] + ((p->orient.front[Z] * ftom( v->laser_speed )) >> MEXP);
    }
    else if( p->type == MISSILE ) {
        p->orient.position[X] = p->orient.position[X] + ((p->orient.front[X] * ftom( v->missile_speed )) >> MEXP);
        p->orient.position[Y] = p->orient.position[Y] + ((p->orient.front[Y] * ftom( v->missile_speed )) >> MEXP);
        p->orient.position[Z] = p->orient.position[Z] + ((p->orient.front[Z] * ftom( v->missile_speed )) >> MEXP);
    }
    else if( p->type == MINE ) {
        /* do nothing mines don't move */
    }
    else if( p->type == CS_MISSILE ) {
        p->orient.position[X] = p->orient.position[X] + ((p->orient.front[X] * ftom( v->cs_missile_speed )) >> MEXP);
        p->orient.position[Y] = p->orient.position[Y] + ((p->orient.front[Y] * ftom( v->cs_missile_speed )) >> MEXP);
        p->orient.position[Z] = p->orient.position[Z] + ((p->orient.front[Z] * ftom( v->cs_missile_speed )) >> MEXP);
    }
    else if( p->type == TRAITOR_MISSILE ) {
        p->orient.position[X] = p->orient.position[X] + ((p->orient.front[X] * ftom( v->traitor_missile_speed )) >> MEXP);
        p->orient.position[Y] = p->orient.position[Y] + ((p->orient.front[Y] * ftom( v->traitor_missile_speed )) >> MEXP);
        p->orient.position[Z] = p->orient.position[Z] + ((p->orient.front[Z] * ftom( v->traitor_missile_speed )) >> MEXP);
    }
}

void move_projectile_forward_surface( Projectile *p, Vehicle *v )
{
    Point temp, new_front;
    long theta, phi;
    long len;
    const long surface_radius = 10086;
    const long two_pi = (6434);
    long proj_speed;

    if( p->type == LASER ) {
        proj_speed = ftom( v->laser_speed );
    }
    else if( p->type == MISSILE ) {
        proj_speed = ftom( v->missile_speed );
    }
    else if( p->type == CS_MISSILE ) {
        proj_speed = ftom( v->cs_missile_speed );
    }
    else if( p->type == TRAITOR_MISSILE ) {
        proj_speed = ftom( v->traitor_missile_speed );
    }
    else if( p->type == MINE ) {
        proj_speed = 0;
    }

    proj_speed = ftom( v->laser_speed );

    /* theta is the current angle on the cylinder */

    if( p->orient.position[Y] >= 0 ) {
        theta = iarccos( (p->orient.position[X] << 16) /
                         (surface_radius << 3) ) >> 3;
    }
    else {
        theta = two_pi - (iarccos( (p->orient.position[X] << 16) /
                                   (surface_radius << 3) ) >> 3);
    }

    /* temp is a vector from the origin to the tip of the front
       vector (which is tangent to the cylinder) */

    temp[X] = (((p->orient.front[X] * proj_speed) >> MEXP) + p->orient.position[X]);
    temp[Y] = (((p->orient.front[Y] * proj_speed) >> MEXP) + p->orient.position[Y]);
    temp[Z] = 0;

    /* normalize temp */

    len = ftom( (float)sqrt( mtof( ((temp[X] * temp[X]) >> MEXP) +
								   ((temp[Y] * temp[Y]) >> MEXP) ) ) );

    temp[X] = (temp[X] << 16) / (len << 3);
    temp[Y] = (temp[Y] << 16) / (len << 3);
    temp[Z] = 0;

    /* use temp to find the new angle on the cylinder, phi */

    if( temp[Y] >= 0 ) {
        phi = (iarccos( temp[X] ) >> 3);
    }
    else {
        phi = two_pi - (iarccos( temp[X] ) >> 3);
    }

    /* use phi to find the new_offset */

    p->orient.position[X] = ( icos( phi ) * surface_radius) >> MEXP;
    p->orient.position[Y] = ( isin( phi ) * surface_radius) >> MEXP;
    p->orient.position[Z] = ((p->orient.front[Z] * proj_speed) >> MEXP) + p->orient.position[Z];

    /* new_up is just the normalized vector from new_offset to the
       origin */

    p->orient.up[X] = (-p->orient.position[X] << MEXP) / surface_radius;
    p->orient.up[Y] = (-p->orient.position[Y] << MEXP) / surface_radius;
    p->orient.up[Z] = 0;

    /* rotate front_vector -theta around the z-axis */

    new_front[X] = ((p->orient.front[X] * icos( -theta ) ) >> MEXP) -
                   ((p->orient.front[Y] * isin( -theta ) ) >> MEXP);
    new_front[Y] = ((p->orient.front[Y] * icos( -theta ) ) >> MEXP) +
                   ((p->orient.front[X] * isin( -theta ) ) >> MEXP);
    new_front[Z] = 0;

    /* rotate front_vector to its new position tangent to the
       angle phi on the clyinder */

    p->orient.front[X] = (( new_front[X] * icos( phi ) ) >> MEXP) -
                         (( new_front[Y] * isin( phi ) ) >> MEXP);
    p->orient.front[Y] = (( new_front[Y] * icos( phi ) ) >> MEXP) +
                         (( new_front[Z] * isin( phi ) ) >> MEXP);
}

void old_move_missile_forward( Projectile *p, Vehicle *vehicle )
{
    Vector t, v, new_front, new_up;
    long zrot, xrot, theta, phi, diff, len;
    const long two_pi = (6434);
    const long pi = (3217);
    const long delta = 178;  /* max amount missile can rotate (10 degrees) */

    /* first figure out the amount to rotate missile around z-axis */

    /* project target onto xy_plane */

    t[X] = ftom( p->target[X] ) - p->orient.position[X];
    t[Y] = ftom( p->target[Y] ) - p->orient.position[Y];
    t[Z] = 0;

    /* normalize t with reference to the xy_plane */

    len = ftom( (float)sqrt( mtof( ((t[X] * t[X]) >> MEXP) +
							       ((t[Y] * t[Y]) >> MEXP) ) ) );

    /* printf("xy-plane, t len = %ld\n", len ); */

    if( len != 0 ) {

        t[X] = (t[X] << MEXP) / len;
        t[Y] = (t[Y] << MEXP) / len;
        t[Z] = 0;

        /* project front onto xy_plane */

        v[X] = p->orient.front[X];
        v[Y] = p->orient.front[Y];
        v[Z] = 0;

        /* normalize v */

        len = ftom( (float)sqrt( mtof( ((v[X] * v[X]) >> MEXP) +
								       ((v[Y] * v[Y]) >> MEXP) ) ) );

        /* printf("xy-plane, v len = %ld\n", len ); */


        if( len != 0 ) {

            v[X] = (v[X] << MEXP) / len;
            v[Y] = (v[Y] << MEXP) / len;
            v[Z] = 0;

            if( v[X] > 1024 ) {
                v[X] = 1024;
            }
            if( v[X] < -1024 ) {
                v[X] = -1024;
            }
            /* printf("xy_plane, v[X] = %ld\n", v[X] ); */

            /* figure out the angle v is from the x-axis (theta) */

            if( v[Y] >= 0 ) {
                theta = iarccos( (v[X] << 3) ) >> 3;
            }
            else {
                theta = two_pi - (iarccos( (v[X] << 3) ) >> 3);
            }

            /* figure out the angle t is from the x-axis (phi) */

            if( t[X] > 1024 ) {
                t[X] = 1024;
            }
            if( t[X] < -1024 ) {
                t[X] = -1024;
            }
            /* printf("xy_plane, t[X] = %ld\n", t[X] ); */

            if( t[Y] >= 0 ) {
                phi = iarccos( (t[X] << 3) ) >> 3;
            }
            else {
                phi = two_pi - (iarccos( (t[X] << 3) ) >> 3);
            }

            /* use (phi - theta) to figure out zrot */

            diff = phi - theta;

            /* make sure diff is between 0 and 2 pi */

            if( diff < 0 ) {
                diff = diff + two_pi;
            }
            if( diff >= two_pi ) {
                diff = diff - two_pi;
            }

            /* figure out zrot */

            if( diff < pi ) {

                /* zrot is counterclockwise */

                if( diff < delta ) {
                    zrot = diff;
                }
                else {
                    zrot = delta;
                }
            }
            else {

                /* zrot is clockwise */

                if( diff > (-delta + two_pi) ) {
                    zrot = -diff;
                }
                else {
                    zrot = -delta;
                }
            }
        }
        else {
            zrot = 0;
        }
    }
    else {
        zrot = 0;
    }

    /* now figure out xrot */

    /* project target onto yz_plane */

    t[X] = 0;
    t[Y] = ftom( p->target[Y] ) - p->orient.position[Y];
    t[Z] = ftom( p->target[Z] ) - p->orient.position[Z];

    /* normalize t with reference to the yz_plane */

    len = ftom( (float)sqrt( mtof( ((t[Y] * t[Y]) >> MEXP) +
						           ((t[Z] * t[Z]) >> MEXP) ) ) );

    /* printf("yz_plane, t len = %ld\n", len ); */

    if( len != 0 ) {

        t[X] = 0;
        t[Y] = (t[Y] << MEXP) / len;
        t[Z] = (t[Z] << MEXP) / len;

        /* project front onto yz_plane */

        v[X] = 0;
        v[Y] = p->orient.front[Y];
        v[Z] = p->orient.front[Z];

        /* normalize v */

        len = ftom( (float)sqrt( mtof( ((v[Y] * v[Y]) >> MEXP) +
							           ((v[Z] * v[Z]) >> MEXP) ) ) );

        /* printf("yz_plane, v len = %ld\n", len ); */

        if( len != 0 ) {

            v[X] = 0;
            v[Y] = (v[Y] << MEXP) / len;
            v[Z] = (v[Z] << MEXP) / len;

            /* figure out the angle v is from the x-axis (theta) */

            if( v[Y] > 1024 ) {
                v[Y] = 1024;
            }
            if( v[Y] < -1024 ) {
                v[Y] = -1024;
            }
            /* printf("yz_plane, v[Y] = %ld\n", v[Y] ); */

            if( v[Z] >= 0 ) {
                theta = iarccos( (v[Y] << 3) ) >> 3;
            }
            else {
                theta = two_pi - (iarccos( (v[Y] << 3) ) >> 3);
            }

            /* figure out the angle t is from the x-axis (phi) */

            if( t[Y] > 1024 ) {
                t[Y] = 1024;
            }
            if( t[Y] < -1024 ) {
                t[Y] = -1024;
            }
            /* printf("yz_plane, t[Y] = %ld\n", t[Y] ); */

            if( t[Z] >= 0 ) {
                phi = iarccos( (t[Y] << 3) ) >> 3;
            }
            else {
                phi = two_pi - (iarccos( (t[Y] << 3) ) >> 3);
            }

            /* use (phi - theta) to figure out xrot */

            diff = phi - theta;

            /* make sure diff is between 0 and 2 pi */

            if( diff < 0 ) {
                diff = diff + two_pi;
            }
            if( diff >= two_pi ) {
                diff = diff - two_pi;
            }

            /* figure out xrot */

            if( diff < pi ) {

                /* xrot is counterclockwise */

                if( diff < delta ) {
                    xrot = diff;
                }
                else {
                    xrot = delta;
                }
            }
            else {

                /* xrot is clockwise */

                if( diff > (-delta + two_pi) ) {
                    xrot = -diff;
                }
                else {
                    xrot = -delta;
                }
            }
        }
        else {
            xrot = 0;
        }
    }
    else {
        xrot = 0;
    }

    /* rotate front & up zrot around z-axis */

    new_front[X] = ((p->orient.front[X] * icos( zrot ) ) >> MEXP) -
                   ((p->orient.front[Y] * isin( zrot ) ) >> MEXP);
    new_front[Y] = ((p->orient.front[Y] * icos( zrot ) ) >> MEXP) +
                   ((p->orient.front[X] * isin( zrot ) ) >> MEXP);
    new_front[Z] = p->orient.front[Z];

    new_up[X] = ((p->orient.up[X] * icos( zrot ) ) >> MEXP) -
                ((p->orient.up[Y] * isin( zrot ) ) >> MEXP);
    new_up[Y] = ((p->orient.up[Y] * icos( zrot ) ) >> MEXP) +
                ((p->orient.up[X] * isin( zrot ) ) >> MEXP);
    new_up[Z] = p->orient.up[Z];

    /* rotate front xrot around x-axis */

    p->orient.front[X] = new_front[X];
    p->orient.front[Y] = ((new_front[Y] * icos( xrot ) ) >> MEXP) -
                         ((new_front[Z] * isin( xrot ) ) >> MEXP);
    p->orient.front[Z] = ((new_front[Y] * isin( xrot ) ) >> MEXP) +
                         ((new_front[Z] * icos( xrot ) ) >> MEXP);

    p->orient.up[X] = new_up[X];
    p->orient.up[Y] = ((new_up[Y] * icos( xrot ) ) >> MEXP) -
                      ((new_up[Z] * isin( xrot ) ) >> MEXP);
    p->orient.up[Z] = ((new_up[Y] * isin( xrot ) ) >> MEXP) +
                      ((new_up[Z] * icos( xrot ) ) >> MEXP);

    /* printf("target = (%.5f, %.5f, %.5f)\n", p->target[X], p->target[Y], p->target[Z] );
    printf("zrot = %ld, xrot = %ld\n", zrot, xrot);

    Get_Keypress(); */

    move_projectile_forward_3d( p, vehicle );
}

void move_missile_forward( Projectile *p, Vehicle *vehicle, short seeking_on )
{
    Float_Point diff, new_front, axis, front, up;
    float temp, theta;
    const float epsilon = 0.005f;
    Float_Matrix front_rotate;

    if( seeking_on ) {

        front[X] = mtof( p->orient.front[X] );
        front[Y] = mtof( p->orient.front[Y] );
        front[Z] = mtof( p->orient.front[Z] );

        up[X] = mtof( p->orient.up[X] );
        up[Y] = mtof( p->orient.up[Y] );
        up[Z] = mtof( p->orient.up[Z] );

        /* new_front is the vector from the missile to the target */

        new_front[X] = p->target[X] - mtof( p->orient.position[X] );
        new_front[Y] = p->target[Y] - mtof( p->orient.position[Y] );
        new_front[Z] = p->target[Z] - mtof( p->orient.position[Z] );

        normalize( new_front );
        normalize( front );
        normalize( up );

        /* Find the front_rotate matrix, which will rotate the front
        vector towards the new_front vector. */

        /* if front equals new_front then the front_rotate matrix should
        become the identity matrix */

        diff[X] = front[X] - new_front[X];
        diff[Y] = front[Y] - new_front[Y];
        diff[Z] = front[Z] - new_front[Z];

        if( ((diff[X] < epsilon) && (diff[X] > -epsilon)) &&
            ((diff[Y] < epsilon) && (diff[Y] > -epsilon)) &&
            ((diff[Z] < epsilon) && (diff[Z] > -epsilon)) ) {
            identity_matrix_init( front_rotate );
        }
        else {
            /* theta is the angle between front and new_front */

            temp = vector_dot_product( front, new_front );
            if( (temp > 1.0f) ) {
                /* printf("acos domain error!!!\n"); */
                temp = 1.0f;
                /* Get_Keypress(); */
            }
            else if( temp < -1.0f ) {
                /* printf("acos domain error!!!\n"); */
                temp = -1.0f;
            }

            theta = (float)acos( temp );

            /* axis is the axis that is used to rotate around */

            vector_cross_product( new_front, front, axis );

            /* make sure the length of axis equals one. */

            if( sqrt( vector_dot_product( axis, axis ) ) != 0.0 ) {
            
                normalize( axis );                
                
                if( (theta > 0) && (theta > vehicle->turning_angle) ) {
                    theta = vehicle->turning_angle;
                }

                /* assign to front_rotate the matrix that will rotate a point
                   theta radians around the vector axis */

                rotate_around_axis( theta, axis, front_rotate );

            }
            else {
                /* This can happen in two cases :
                   1) (new_front == 0) or (front == 0)
                   2) new_front == front
                   Both cases are extreamly unlikely but apperently do happen
                      occasionaly. */
                
                /* don't do any rotations */
                
                /* if new_front == 0 */
                
                if( ((new_front[X] < epsilon) && (new_front[X] > -epsilon)) &&
                    ((new_front[Y] < epsilon) && (new_front[Y] > -epsilon)) &&
                    ((new_front[Z] < epsilon) && (new_front[Z] > -epsilon)) ) {
                    
                    /* If this happens then something went very wrong somewere. */
                    /* so i guess we should quit landing this vehicle */
                    
                    return;
                }                
                else if( ((front[X] < epsilon) && (front[X] > -epsilon)) &&
                         ((front[Y] < epsilon) && (front[Y] > -epsilon)) &&
                         ((front[Z] < epsilon) && (front[Z] > -epsilon)) ) {
                    
                    /* If this happens then something went very wrong somewere. */
                    /* so i guess we should quit landing this vehicle */
                    
                    return;
                }
                else {
                    identity_matrix_init( front_rotate );
                }
                
                /* say a prayer and hope this works! Hail Mary, full of grace .... ,
                   Our Father who art in heaven .... , Please let this fucking thing work!!!! */
            }

        }

        transformation( front_rotate, front );
        transformation( front_rotate, up );

        p->orient.front[X] = ftom( front[X] );
        p->orient.front[Y] = ftom( front[Y] );
        p->orient.front[Z] = ftom( front[Z] );

        p->orient.up[X] = ftom( up[X] );
        p->orient.up[Y] = ftom( up[Y] );
        p->orient.up[Z] = ftom( up[Z] );

        move_projectile_forward_3d( p, vehicle );
    }
    else {
        if( p->on_surface ) {
            move_projectile_forward_surface( p, vehicle );
        }
        else {
            move_projectile_forward_3d( p, vehicle );
        }
    }
}

void move_decoy_projectile( Projectile *p, Vehicle *v, Player player_array[] )
{
    Vehicle temp_vehicle;
    long player_index;

    /* copy a bunch of shit from v into temp_vehicle so the movement functions
       will work */

    temp_vehicle.surface_rad = v->surface_rad;

    temp_vehicle.air_forward_speed = v->air_forward_speed;
    temp_vehicle.air_max_forward_speed = v->air_max_forward_speed;
    temp_vehicle.air_inc_forward_speed = v->air_inc_forward_speed;
    temp_vehicle.air_max_sidestep_speed = v->air_max_sidestep_speed;
    temp_vehicle.air_inc_sidestep_speed = v->air_inc_sidestep_speed;
    temp_vehicle.air_inc_rot_speed = v->air_inc_rot_speed;
    temp_vehicle.air_max_rot_speed = v->air_max_rot_speed;

    temp_vehicle.surface_max_speed = v->surface_max_speed;
    temp_vehicle.surface_inc_speed = v->surface_inc_speed;
    temp_vehicle.surface_inc_sidestep_speed = v->surface_inc_sidestep_speed;
    temp_vehicle.surface_inc_rot_speed = v->surface_inc_rot_speed;
    temp_vehicle.surface_max_rot_speed = v->surface_max_rot_speed;

    /* copy p->surface_mode into temp_vehicle */

    if( p->on_surface ) {
        temp_vehicle.vehicle_mode = Surface;
    }
    else {
        temp_vehicle.vehicle_mode = Air;
    }

    /* copy the orientation, vel & rotation speeds from p into temp_vehicle */

    temp_vehicle.orient.front[X] = mtof( p->orient.front[X] );
    temp_vehicle.orient.front[Y] = mtof( p->orient.front[Y] );
    temp_vehicle.orient.front[Z] = mtof( p->orient.front[Z] );

    temp_vehicle.orient.up[X] = mtof( p->orient.up[X] );
    temp_vehicle.orient.up[Y] = mtof( p->orient.up[Y] );
    temp_vehicle.orient.up[Z] = mtof( p->orient.up[Z] );

    temp_vehicle.orient.position[X] = mtof( p->orient.position[X] );
    temp_vehicle.orient.position[Y] = mtof( p->orient.position[Y] );
    temp_vehicle.orient.position[Z] = mtof( p->orient.position[Z] );

    temp_vehicle.vel[X] = p->vel[X];
    temp_vehicle.vel[Y] = p->vel[Y];
    temp_vehicle.vel[Z] = p->vel[Z];

    temp_vehicle.air_rise_rot_speed = p->air_rise_rot_speed;
    temp_vehicle.air_spin_rot_speed = p->air_spin_rot_speed;
    temp_vehicle.surface_rot_speed = p->surface_rot_speed;

    /* find this vehicles index into the player_array */

    for( player_index = 0; player_index < 6; player_index++ ) {
        if( v == &(player_array[player_index].tank) ) {
            break;
        }
    }

    /* update the orientations and velocity of temp_vehicle according to
       the key table */

    if( p->type == DECOY_0 ) {

        if( Check_Input_Table( INDEX_LEFT, player_array[player_index].table  ) ) {
            update_vehicle_up_key( &(temp_vehicle),
                                   player_array[player_index].table[INDEX_LEFT] );
	}
	if( Check_Input_Table( INDEX_RIGHT, player_array[player_index].table  ) ) {
            update_vehicle_down_key( &(temp_vehicle),
                                     player_array[player_index].table[INDEX_RIGHT] );
	}
	if( Check_Input_Table( INDEX_UP, player_array[player_index].table  ) ) {
            update_vehicle_right_key( &(temp_vehicle),
                                      player_array[player_index].table[INDEX_UP] );
	}
	if( Check_Input_Table( INDEX_DOWN, player_array[player_index].table ) ) {
            update_vehicle_left_key( &(temp_vehicle),
                                     player_array[player_index].table[INDEX_DOWN] );
	}
        if( Check_Input_Table( INDEX_SIDESTEP_RIGHT, player_array[player_index].table ) ) {

	    /* down */

	    if( player_array[player_index].tank.vehicle_mode == Surface ) {
                update_vehicle_down_key( &(temp_vehicle),
                                         player_array[player_index].table[INDEX_SIDESTEP_RIGHT] );
	    }
	    else if( player_array[player_index].tank.vehicle_mode == Air ) {
                update_vehicle_sidestep_down( &(temp_vehicle),
                                              player_array[player_index].table[INDEX_SIDESTEP_RIGHT] );
	    }
	}
	if( Check_Input_Table( INDEX_SIDESTEP_LEFT, player_array[player_index].table  ) ) {

	    /* up */

	    if( player_array[player_index].tank.vehicle_mode == Surface ) {
                update_vehicle_up_key( &(temp_vehicle),
                                       player_array[player_index].table[INDEX_SIDESTEP_LEFT] );
	    }
	    else if( player_array[player_index].tank.vehicle_mode == Air ) {
                update_vehicle_sidestep_up( &(temp_vehicle),
                                            player_array[player_index].table[INDEX_SIDESTEP_LEFT] );
	    }
	}
	if( Check_Input_Table( INDEX_SIDESTEP_UP, player_array[player_index].table ) ) {

	    /* right */

            update_vehicle_sidestep_right( &(temp_vehicle),
                                           player_array[player_index].table[INDEX_SIDESTEP_UP] );

	}
	if( Check_Input_Table( INDEX_SIDESTEP_DOWN, player_array[player_index].table ) ) {

	    /* left */

            update_vehicle_sidestep_left( &(temp_vehicle),
                                          player_array[player_index].table[INDEX_SIDESTEP_DOWN] );
	}
    }
    else if( p->type == DECOY_1 ) {

        if( Check_Input_Table( INDEX_LEFT, player_array[player_index].table  ) ) {
            update_vehicle_right_key( &(temp_vehicle),
                                      player_array[player_index].table[INDEX_LEFT] );
	}
	if( Check_Input_Table( INDEX_RIGHT, player_array[player_index].table  ) ) {
            update_vehicle_left_key( &(temp_vehicle),
                                     player_array[player_index].table[INDEX_RIGHT] );
	}
	if( Check_Input_Table( INDEX_UP, player_array[player_index].table  ) ) {
            update_vehicle_down_key( &(temp_vehicle),
                                     player_array[player_index].table[INDEX_UP] );
	}
	if( Check_Input_Table( INDEX_DOWN, player_array[player_index].table ) ) {
            update_vehicle_up_key( &(temp_vehicle),
                                   player_array[player_index].table[INDEX_DOWN] );
	}
        if( Check_Input_Table( INDEX_SIDESTEP_RIGHT, player_array[player_index].table ) ) {

	    /* left */

            update_vehicle_sidestep_left( &(temp_vehicle),
                                          player_array[player_index].table[INDEX_SIDESTEP_RIGHT] );

	}
	if( Check_Input_Table( INDEX_SIDESTEP_LEFT, player_array[player_index].table  ) ) {

	    /* right */

            update_vehicle_sidestep_right( &(temp_vehicle),
                                           player_array[player_index].table[INDEX_SIDESTEP_LEFT] );
	}
	if( Check_Input_Table( INDEX_SIDESTEP_UP, player_array[player_index].table ) ) {

	    /* down */

            if( player_array[player_index].tank.vehicle_mode == Surface ) {
                update_vehicle_down_key( &(temp_vehicle),
                                         player_array[player_index].table[INDEX_SIDESTEP_UP] );
	    }
	    else if( player_array[player_index].tank.vehicle_mode == Air ) {
                update_vehicle_sidestep_down( &(temp_vehicle),
                                              player_array[player_index].table[INDEX_SIDESTEP_UP] );
	    }

	}
	if( Check_Input_Table( INDEX_SIDESTEP_DOWN, player_array[player_index].table ) ) {

	    /* up */

	    if( player_array[player_index].tank.vehicle_mode == Surface ) {
                update_vehicle_up_key( &(temp_vehicle),
                                       player_array[player_index].table[INDEX_SIDESTEP_DOWN] );
	    }
	    else if( player_array[player_index].tank.vehicle_mode == Air ) {
                update_vehicle_sidestep_up( &(temp_vehicle),
                                            player_array[player_index].table[INDEX_SIDESTEP_DOWN] );
	    }
	}
    }
    else if( p->type == DECOY_2 ) {

        if( Check_Input_Table( INDEX_LEFT, player_array[player_index].table  ) ) {
            update_vehicle_down_key( &(temp_vehicle),
                                     player_array[player_index].table[INDEX_LEFT] );
	}
	if( Check_Input_Table( INDEX_RIGHT, player_array[player_index].table  ) ) {
            update_vehicle_up_key( &(temp_vehicle),
                                   player_array[player_index].table[INDEX_RIGHT] );
	}
	if( Check_Input_Table( INDEX_UP, player_array[player_index].table  ) ) {
            update_vehicle_left_key( &(temp_vehicle),
                                     player_array[player_index].table[INDEX_UP] );
	}
	if( Check_Input_Table( INDEX_DOWN, player_array[player_index].table ) ) {
            update_vehicle_right_key( &(temp_vehicle),
                                      player_array[player_index].table[INDEX_DOWN] );
	}
        if( Check_Input_Table( INDEX_SIDESTEP_RIGHT, player_array[player_index].table ) ) {

	    /* up */

	    if( player_array[player_index].tank.vehicle_mode == Surface ) {
                update_vehicle_up_key( &(temp_vehicle),
                                       player_array[player_index].table[INDEX_SIDESTEP_RIGHT] );
	    }
	    else if( player_array[player_index].tank.vehicle_mode == Air ) {
                update_vehicle_sidestep_up( &(temp_vehicle),
                                            player_array[player_index].table[INDEX_SIDESTEP_RIGHT] );
	    }

	}
	if( Check_Input_Table( INDEX_SIDESTEP_LEFT, player_array[player_index].table  ) ) {

	    /* down */

	    if( player_array[player_index].tank.vehicle_mode == Surface ) {
                update_vehicle_down_key( &(temp_vehicle),
                                         player_array[player_index].table[INDEX_SIDESTEP_LEFT] );
	    }
	    else if( player_array[player_index].tank.vehicle_mode == Air ) {
                update_vehicle_sidestep_down( &(temp_vehicle),
                                              player_array[player_index].table[INDEX_SIDESTEP_LEFT] );
	    }

	}
	if( Check_Input_Table( INDEX_SIDESTEP_UP, player_array[player_index].table ) ) {

	    /* left */

            update_vehicle_sidestep_left( &(temp_vehicle),
                                          player_array[player_index].table[INDEX_SIDESTEP_UP] );

	}
	if( Check_Input_Table( INDEX_SIDESTEP_DOWN, player_array[player_index].table ) ) {

	    /* right */

            update_vehicle_sidestep_right( &(temp_vehicle),
                                           player_array[player_index].table[INDEX_SIDESTEP_DOWN] );

	}
    }

    /* move temp_vehicle forward */

    move_vehicle_forward( &temp_vehicle );

    /* increase temp_vehicles forward thrust */

    if( temp_vehicle.vehicle_mode == Air ) {
        temp_vehicle.vel[X] += temp_vehicle.orient.front[X] * temp_vehicle.air_inc_forward_speed;
        temp_vehicle.vel[Y] += temp_vehicle.orient.front[Y] * temp_vehicle.air_inc_forward_speed;
        temp_vehicle.vel[Z] += temp_vehicle.orient.front[Z] * temp_vehicle.air_inc_forward_speed;
    }

    /* reduse vel by friction */

    reduce_by_friction( temp_vehicle.vehicle_mode, temp_vehicle.vel );

    /* copy the orient, vel & rotation stuff from temp_vehicle into p */

    p->orient.front[X] = rounding_ftom( temp_vehicle.orient.front[X] );
    p->orient.front[Y] = rounding_ftom( temp_vehicle.orient.front[Y] );
    p->orient.front[Z] = rounding_ftom( temp_vehicle.orient.front[Z] );

    p->orient.up[X] = rounding_ftom( temp_vehicle.orient.up[X] );
    p->orient.up[Y] = rounding_ftom( temp_vehicle.orient.up[Y] );
    p->orient.up[Z] = rounding_ftom( temp_vehicle.orient.up[Z] );

    p->orient.position[X] = rounding_ftom( temp_vehicle.orient.position[X] );
    p->orient.position[Y] = rounding_ftom( temp_vehicle.orient.position[Y] );
    p->orient.position[Z] = rounding_ftom( temp_vehicle.orient.position[Z] );

    p->vel[X] = temp_vehicle.vel[X];
    p->vel[Y] = temp_vehicle.vel[Y];
    p->vel[Z] = temp_vehicle.vel[Z];

    p->air_rise_rot_speed = temp_vehicle.air_rise_rot_speed;
    p->air_spin_rot_speed = temp_vehicle.air_spin_rot_speed;
    p->surface_rot_speed = temp_vehicle.surface_rot_speed;

}

void move_projectiles_forward( Vehicle *v, Pylons *pylons, Player *player,
                               RadarBase *red_base, RadarBase *blue_base )
{
    Projectile *p, *last;
    Orientation o;
    long face_index;

    last = NULL;
    p = v->projectile_list;

    while( p != NULL ) {

        /* move the projectile forward */

        if( p->type == LASER ) {
            if( p->on_surface ) {
                move_projectile_forward_surface( p, v );
                magic_orient_object( p->obj, &(p->orient), p->world_obj );
                init_projectile_bounding_box( p );
            }
            else {
                move_projectile_forward_3d( p, v );
                magic_orient_object( p->obj, &(p->orient), p->world_obj );
                init_projectile_bounding_box( p );
            }
        }
        else if( p->type == MISSILE ) {

            if( p->target == NULL ) {
                move_missile_forward( p, v, FALSE );
                magic_orient_object( p->obj, &(p->orient), p->world_obj );
                init_projectile_bounding_box( p );
            }
            else {
                move_missile_forward( p, v, TRUE );
                magic_orient_object( p->obj, &(p->orient), p->world_obj );
                init_projectile_bounding_box( p );
            }
        }
        else if( p->type == MINE ) {

            magic_orient_object( p->obj, &(p->orient), p->world_obj );
            init_projectile_bounding_box( p );
        }
        else if( p->type == CS_MISSILE ) {

            if( p->on_surface ) {
                move_projectile_forward_surface( p, v );
            }
            else {
                move_projectile_forward_3d( p, v );
            }

            magic_orient_object( p->obj, &(p->orient), p->world_obj );
            init_projectile_bounding_box( p );
        }
        else if( p->type == TRAITOR_MISSILE ) {

            if( p->on_surface ) {
                move_projectile_forward_surface( p, v );
            }
            else {
                move_projectile_forward_3d( p, v );
            }

            magic_orient_object( p->obj, &(p->orient), p->world_obj );
            init_projectile_bounding_box( p );
        }
        else if( (p->type == DECOY_0) || (p->type == DECOY_1) || (p->type == DECOY_2) ) {

            move_decoy_projectile( p, v, player );

            magic_orient_object( p->obj, &(p->orient), p->world_obj );
            init_projectile_bounding_box( p );

        }


        /* check for collisions */

        if( ((p->type == DECOY_0) || (p->type == DECOY_1) || (p->type == DECOY_2)) ) {

            if( ((p->frames_left == 0) || projectile_cylinder_collision( p ) ) ) {

                if( last == NULL ) {

                    p = p->next;
                    free( v->projectile_list->obj->point );
                    v->projectile_list->obj->point = NULL;
                    free( v->projectile_list->obj->face );
                    v->projectile_list->obj->face = NULL;
                    free( v->projectile_list->obj );
                    v->projectile_list->obj = NULL;
                    free( v->projectile_list->world_obj->point );
                    v->projectile_list->world_obj->point = NULL;
                    free( v->projectile_list->world_obj->face );
                    v->projectile_list->world_obj->face = NULL;
                    free( v->projectile_list->world_obj );
                    v->projectile_list->world_obj = NULL;
                    free( v->projectile_list->edge_table.edge );
                    v->projectile_list->edge_table.edge = NULL;
                    free( v->projectile_list );
                    v->projectile_list = NULL;
                    v->projectile_list = p;
                }
                else {

                    p = p->next;
                    free( last->next->obj->point );
                    last->next->obj->point = NULL;
                    free( last->next->obj->face );
                    last->next->obj->face = NULL;
                    free( last->next->obj );
                    last->next->obj = NULL;
                    free( last->next->world_obj->point );
                    last->next->world_obj->point = NULL;
                    free( last->next->world_obj->face );
                    last->next->world_obj->face = NULL;
                    free( last->next->world_obj );
                    last->next->world_obj = NULL;
                    free( last->next->edge_table.edge );
                    last->next->edge_table.edge = NULL;
                    free( last->next );
                    last->next = NULL;
                    last->next = p;
                }
            }
            else {
                p->frames_left--;
                last = p;
                p = p->next;
            }
        }
        else if( ( p->frames_left == 0 ) ||
                   pylon_projectile_collision( pylons, p, &face_index ) ||
                   player_projectile_collision( player, p, v ) ||
                   radar_base_projectile_collision( red_base, p, v ) ||
                   radar_base_projectile_collision( blue_base, p, v ) ||
                   projectile_cylinder_collision( p ) ) {

            if( last == NULL ) {

                /* add this explosion to the explosion list */

                o.position[X] = mtof( v->projectile_list->orient.position[X] );
                o.position[Y] = mtof( v->projectile_list->orient.position[Y] );
                o.position[Z] = mtof( v->projectile_list->orient.position[Z] );

                o.front[X] = -mtof( v->projectile_list->orient.front[X] );
                o.front[Y] = -mtof( v->projectile_list->orient.front[Y] );
                o.front[Z] = -mtof( v->projectile_list->orient.front[Z] );

                o.up[X] = mtof( v->projectile_list->orient.up[X] );
                o.up[Y] = mtof( v->projectile_list->orient.up[Y] );
                o.up[Z] = mtof( v->projectile_list->orient.up[Z] );

                if( v->projectile_list->type == LASER ) {
                    add_explosion( laser_explosion, &o, NULL, v->projectile_list->world_obj->face[0].gradient );
                }
                else if( v->projectile_list->type == MISSILE ) {
                    add_explosion( missile_explosion, &o, NULL, v->projectile_list->world_obj->face[0].gradient );
                }
                else if( v->projectile_list->type == MINE ) {
                    add_explosion( mine_explosion, &o, NULL, v->projectile_list->world_obj->face[0].gradient );
                }
                else if( v->projectile_list->type == CS_MISSILE ) {
                    add_explosion( cs_missile_explosion, &o, NULL, v->projectile_list->world_obj->face[0].gradient );
                }
                else if( v->projectile_list->type == TRAITOR_MISSILE ) {
                    add_explosion( traitor_missile_explosion, &o, NULL, v->projectile_list->world_obj->face[0].gradient );
                }

                p = p->next;
                free( v->projectile_list->obj->point );
                v->projectile_list->obj->point = NULL;
                free( v->projectile_list->obj->face );
                v->projectile_list->obj->face = NULL;
                free( v->projectile_list->obj );
                v->projectile_list->obj = NULL;
                free( v->projectile_list->world_obj->point );
                v->projectile_list->world_obj->point = NULL;
                free( v->projectile_list->world_obj->face );
                v->projectile_list->world_obj->face = NULL;
                free( v->projectile_list->world_obj );
                v->projectile_list->world_obj = NULL;
                free( v->projectile_list->edge_table.edge );
                v->projectile_list->edge_table.edge = NULL;
                free( v->projectile_list );
                v->projectile_list = NULL;
                v->projectile_list = p;
            }
            else {

                /* add this explosion to the explosion list */

                o.position[X] = mtof( last->next->orient.position[X] );
                o.position[Y] = mtof( last->next->orient.position[Y] );
                o.position[Z] = mtof( last->next->orient.position[Z] );

                o.front[X] = -mtof( last->next->orient.front[X] );
                o.front[Y] = -mtof( last->next->orient.front[Y] );
                o.front[Z] = -mtof( last->next->orient.front[Z] );

                o.up[X] = mtof( last->next->orient.up[X] );
                o.up[Y] = mtof( last->next->orient.up[Y] );
                o.up[Z] = mtof( last->next->orient.up[Z] );

                if( last->next->type == LASER ) {
                    add_explosion( laser_explosion, &o, NULL, last->next->world_obj->face[0].gradient );
                }
                else if( last->next->type == MISSILE ) {
                    add_explosion( missile_explosion, &o, NULL, last->next->world_obj->face[0].gradient );
                }
                else if( last->next->type == MINE ) {
                    add_explosion( missile_explosion, &o, NULL, last->next->world_obj->face[0].gradient );
                }
                else if( last->next->type == CS_MISSILE ) {
                    add_explosion( cs_missile_explosion, &o, NULL, last->next->world_obj->face[0].gradient );
                }
                else if( last->next->type == TRAITOR_MISSILE ) {
                    add_explosion( traitor_missile_explosion, &o, NULL, last->next->world_obj->face[0].gradient );
                }

                p = p->next;
                free( last->next->obj->point );
                last->next->obj->point = NULL;
                free( last->next->obj->face );
                last->next->obj->face = NULL;
                free( last->next->obj );
                last->next->obj = NULL;
                free( last->next->world_obj->point );
                last->next->world_obj->point = NULL;
                free( last->next->world_obj->face );
                last->next->world_obj->face = NULL;
                free( last->next->world_obj );
                last->next->world_obj = NULL;
                free( last->next->edge_table.edge );
                last->next->edge_table.edge = NULL;
                free( last->next );
                last->next = NULL;
                last->next = p;
            }
        }
        else {
            p->frames_left--;
            last = p;
            p = p->next;
        }
    }
}

void projectile_transform_and_union( Projectile *list, PointFace *result )
{
    while( list != NULL ) {
        magic_orient_object( list->obj, &(list->orient), list->world_obj );
        union_object( list->world_obj, result );
        list = list->next;
    }
}

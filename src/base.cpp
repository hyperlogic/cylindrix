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

#include <memory.h>

#include "base.h"
#include "object.h"
#include "stdio.h"
#include "stdlib.h"
#include "movement.h"
#include "collide.h"
#include "math.h"
#include "project.h"
#include "jonsb.h"
#include "explode.h"
#include "util.h"

extern game_configuration_type game_configuration;
extern SYS_SOUNDHANDLE sample[MAX_WORLD_SAMPLES]; /* From omega.c */
extern level_type level; /* Holds data loaded from level file in omega.c */


/* update_radar_base() uses the current base_orient and turret_orient to
   initalize the world_base_obj, world_turret_obj and world_union_object.
   It also updates the bounding box */

void update_radar_base( RadarBase *base )
{
    /* make sure base->world_base_obj & base->world_turret_obj are oriented */

    orient_object( base->base_obj, &(base->base_orient),
                   base->world_base_obj );

    orient_object( base->turret_obj, &(base->turret_orient),
                   base->world_turret_obj );

    /* union them into base->world_union_obj */

    object_copy( base->world_union_obj, base->world_base_obj );
    union_object( base->world_turret_obj, base->world_union_obj );

    /* intialize the bounding box */

    find_bounding_box( base->world_union_obj, &(base->box) );
}

void init_red_radar_base( RadarBase *red_base )
{
    long i;

    red_base->team = RED_TEAM;

    /* initalize laser info */

    red_base->max_projectiles = 30;
    red_base->laser_life = 90;
    red_base->laser_speed = 1.0;
    red_base->laser_reload_time = 2;
    red_base->frames_till_fire_laser = 0;
    red_base->laser_damage = 2;

    /* initilize hitpoint info */

    red_base->max_hitpoints = 100;
    red_base->current_hitpoints = 100;

    red_base->alive = TRUE;

    red_base->target = NULL;

    /* make sure all the pointers are NULL */

    red_base->base_obj = NULL;
    red_base->turret_obj = NULL;
    red_base->world_base_obj = NULL;
    red_base->world_turret_obj = NULL;
    red_base->world_union_obj = NULL;

    red_base->projectile_list = NULL;

    red_base->edge_table.edge = NULL;
    red_base->edge_table.edges = 0;

    /* initalize the orientation for the base */

    red_base->base_orient.front[X] = level.base_orientations[0].front[X];
    red_base->base_orient.front[Y] = level.base_orientations[0].front[Y];
    red_base->base_orient.front[Z] = level.base_orientations[0].front[Z];

    red_base->base_orient.up[X] = level.base_orientations[0].up[X];
    red_base->base_orient.up[Y] = level.base_orientations[0].up[Y];
    red_base->base_orient.up[Z] = level.base_orientations[0].up[Z];

    red_base->base_orient.position[X] = level.base_orientations[0].position[X];
    red_base->base_orient.position[Y] = level.base_orientations[0].position[Y];
    red_base->base_orient.position[Z] = level.base_orientations[0].position[Z];

    /* initalize the orientation for the turret */

    red_base->turret_orient.front[X] = level.turret_orientations[0].front[X];
    red_base->turret_orient.front[Y] = level.turret_orientations[0].front[Y];
    red_base->turret_orient.front[Z] = level.turret_orientations[0].front[Z];

    red_base->turret_orient.up[X] = level.turret_orientations[0].up[X];
    red_base->turret_orient.up[Y] = level.turret_orientations[0].up[Y];
    red_base->turret_orient.up[Z] = level.turret_orientations[0].up[Z];

    red_base->turret_orient.position[X] = level.turret_orientations[0].position[X];
    red_base->turret_orient.position[Y] = level.turret_orientations[0].position[Y];
    red_base->turret_orient.position[Z] = level.turret_orientations[0].position[Z];


    /* allocate space for base_obj */

    if( ( red_base->base_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("red_radar_base_init() : malloc failed\n");

    /* null out base_obj's fields */

    red_base->base_obj->point = NULL;
    red_base->base_obj->points = 0;
    red_base->base_obj->face = NULL;
    red_base->base_obj->faces = 0;

    /* load in base_obj */

    get_object( red_base->base_obj, level.base_filename );

    /* allocate space for turret_obj */

    if( ( red_base->turret_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("red_radar_base_init() : malloc failed\n");

    /* null out turret_obj's fields */

    red_base->turret_obj->point = NULL;
    red_base->turret_obj->points = 0;
    red_base->turret_obj->face = NULL;
    red_base->turret_obj->faces = 0;

    /* load in base_obj */

    get_object( red_base->turret_obj, level.turret_filename );

    /* initialize the gradients */

    for( i = 0; i < red_base->base_obj->faces; i++ ) {
        if( red_base->base_obj->face[i].gradient == 0 )
            red_base->base_obj->face[i].gradient = RedRadarFirstGrad;
        else if( red_base->base_obj->face[i].gradient == 1 )
            red_base->base_obj->face[i].gradient = RedRadarSecondGrad;
        else if( red_base->base_obj->face[i].gradient == 2 )
            red_base->base_obj->face[i].gradient = RedRadarThirdGrad;
        else
            red_base->base_obj->face[i].gradient = RedRadarFirstGrad;
    }

    for( i = 0; i < red_base->turret_obj->faces; i++ ) {
        if( red_base->turret_obj->face[i].gradient == 0 )
            red_base->turret_obj->face[i].gradient = RedRadarFirstGrad;
        else if( red_base->turret_obj->face[i].gradient == 1 )
            red_base->turret_obj->face[i].gradient = RedRadarSecondGrad;
        else if( red_base->turret_obj->face[i].gradient == 2 )
            red_base->turret_obj->face[i].gradient = RedRadarThirdGrad;
        else
            red_base->turret_obj->face[i].gradient = RedRadarFirstGrad;
    }

    /* allocate space for world_base_obj */

    if( ( red_base->world_base_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("red_radar_base_init() : malloc failed\n");

    /* null out world_base_obj's fields */

    red_base->world_base_obj->point = NULL;
    red_base->world_base_obj->points = 0;
    red_base->world_base_obj->face = NULL;
    red_base->world_base_obj->faces = 0;

    /* allocate space for world_union_obj */

    if( ( red_base->world_union_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("red_radar_base_init() : malloc failed\n");

    /* null out world_union_obj's fields */

    red_base->world_union_obj->point = NULL;
    red_base->world_union_obj->points = 0;
    red_base->world_union_obj->face = NULL;
    red_base->world_union_obj->faces = 0;

    /* Use base_orient to move base_obj into world coords. Store the result
       in world_base_obj. */

    orient_object( red_base->base_obj, &(red_base->base_orient),
                   red_base->world_base_obj );


    /* allocate space for world_turret_obj */

    if( ( red_base->world_turret_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("red_radar_base_init() : malloc failed\n");

    /* null out world_turret_obj's fields */

    red_base->world_turret_obj->point = NULL;
    red_base->world_turret_obj->points = 0;
    red_base->world_turret_obj->face = NULL;
    red_base->world_turret_obj->faces = 0;

    /* Use turret_orient to move turret_obj into world coords. Store the
       result in world_turret_obj. */

    orient_object( red_base->turret_obj, &(red_base->turret_orient),
                   red_base->world_turret_obj );


    /* union world_turret_obj and world_base_object into world_union_object */

    object_copy( red_base->world_union_obj, red_base->world_base_obj );
    union_object( red_base->world_turret_obj, red_base->world_union_obj );

    /* intialize the bounding box for world_union_object */

    find_bounding_box( red_base->world_union_obj, &(red_base->box) );

    /* intialize the edge table for world_union_object */

    create_edge_table( red_base->world_union_obj, &(red_base->edge_table) );
}

void init_blue_radar_base( RadarBase *blue_base )
{
    long i;

    blue_base->team = BLUE_TEAM;

    /* initalize laser info */

    blue_base->max_projectiles = 30;
    blue_base->laser_life = 90;
    blue_base->laser_speed = 1.0;
    blue_base->laser_reload_time = 2;
    blue_base->frames_till_fire_laser = 0;
    blue_base->laser_damage = 2;

    /* initilize hitpoint info */

    blue_base->max_hitpoints = 100;
    blue_base->current_hitpoints = 100;

    blue_base->alive = TRUE;

    blue_base->target = NULL;

    /* make sure all the pointers are NULL */

    blue_base->base_obj = NULL;
    blue_base->turret_obj = NULL;
    blue_base->world_base_obj = NULL;
    blue_base->world_turret_obj = NULL;
    blue_base->world_union_obj = NULL;

    blue_base->projectile_list = NULL;

    blue_base->edge_table.edge = NULL;
    blue_base->edge_table.edges = 0;

    /* initalize the orientation for the base */

    blue_base->base_orient.front[X] = level.base_orientations[1].front[X];
    blue_base->base_orient.front[Y] = level.base_orientations[1].front[Y];
    blue_base->base_orient.front[Z] = level.base_orientations[1].front[Z];

    blue_base->base_orient.up[X] = level.base_orientations[1].up[X];
    blue_base->base_orient.up[Y] = level.base_orientations[1].up[Y];
    blue_base->base_orient.up[Z] = level.base_orientations[1].up[Z];

    blue_base->base_orient.position[X] = level.base_orientations[1].position[X];
    blue_base->base_orient.position[Y] = level.base_orientations[1].position[Y];
    blue_base->base_orient.position[Z] = level.base_orientations[1].position[Z];

    /* initalize the orientation for the turret */

    blue_base->turret_orient.front[X] = level.turret_orientations[1].front[X];
    blue_base->turret_orient.front[Y] = level.turret_orientations[1].front[Y];
    blue_base->turret_orient.front[Z] = level.turret_orientations[1].front[Z];

    blue_base->turret_orient.up[X] = level.turret_orientations[1].up[X];
    blue_base->turret_orient.up[Y] = level.turret_orientations[1].up[Y];
    blue_base->turret_orient.up[Z] = level.turret_orientations[1].up[Z];

    blue_base->turret_orient.position[X] = level.turret_orientations[1].position[X];
    blue_base->turret_orient.position[Y] = level.turret_orientations[1].position[Y];
    blue_base->turret_orient.position[Z] = level.turret_orientations[1].position[Z];


    /* allocate space for base_obj */

    if( ( blue_base->base_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("blue_radar_base_init() : malloc failed\n");

    /* null out base_obj's fields */

    blue_base->base_obj->point = NULL;
    blue_base->base_obj->points = 0;
    blue_base->base_obj->face = NULL;
    blue_base->base_obj->faces = 0;

    /* load in base_obj */

    get_object( blue_base->base_obj, "3d_data/base.pfd");


    /* allocate space for turret_obj */

    if( ( blue_base->turret_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("blue_radar_base_init() : malloc failed\n");

    /* null out turret_obj's fields */

    blue_base->turret_obj->point = NULL;
    blue_base->turret_obj->points = 0;
    blue_base->turret_obj->face = NULL;
    blue_base->turret_obj->faces = 0;

    /* load in base_obj */

    get_object( blue_base->turret_obj, "3d_data/turret.pfd");

    /* initialize the gradients */

    for( i = 0; i < blue_base->base_obj->faces; i++ ) {
        if( blue_base->base_obj->face[i].gradient == 0 )
            blue_base->base_obj->face[i].gradient = BlueRadarFirstGrad;
        else if( blue_base->base_obj->face[i].gradient == 1 )
            blue_base->base_obj->face[i].gradient = BlueRadarSecondGrad;
        else if( blue_base->base_obj->face[i].gradient == 2 )
            blue_base->base_obj->face[i].gradient = BlueRadarThirdGrad;
        else
            blue_base->base_obj->face[i].gradient = BlueRadarFirstGrad;
    }

    for( i = 0; i < blue_base->turret_obj->faces; i++ ) {
        if( blue_base->turret_obj->face[i].gradient == 0 )
            blue_base->turret_obj->face[i].gradient = BlueRadarFirstGrad;
        else if( blue_base->turret_obj->face[i].gradient == 1 )
            blue_base->turret_obj->face[i].gradient = BlueRadarSecondGrad;
        else if( blue_base->turret_obj->face[i].gradient == 2 )
            blue_base->turret_obj->face[i].gradient = BlueRadarThirdGrad;
        else
            blue_base->turret_obj->face[i].gradient = BlueRadarFirstGrad;
    }

    /* allocate space for world_base_obj */

    if( ( blue_base->world_base_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("blue_radar_base_init() : malloc failed\n");

    /* null out world_base_obj's fields */

    blue_base->world_base_obj->point = NULL;
    blue_base->world_base_obj->points = 0;
    blue_base->world_base_obj->face = NULL;
    blue_base->world_base_obj->faces = 0;

    /* allocate space for world_union_obj */

    if( ( blue_base->world_union_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("blue_radar_base_init() : malloc failed\n");

    /* null out world_union_obj's fields */

    blue_base->world_union_obj->point = NULL;
    blue_base->world_union_obj->points = 0;
    blue_base->world_union_obj->face = NULL;
    blue_base->world_union_obj->faces = 0;

    /* Use base_orient to move base_obj into world coords. Store the result
       in world_base_obj. */

    orient_object( blue_base->base_obj, &(blue_base->base_orient),
                   blue_base->world_base_obj );


    /* allocate space for world_turret_obj */

    if( ( blue_base->world_turret_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
        SYS_Error("blue_radar_base_init() : malloc failed\n");

    /* null out world_turret_obj's fields */

    blue_base->world_turret_obj->point = NULL;
    blue_base->world_turret_obj->points = 0;
    blue_base->world_turret_obj->face = NULL;
    blue_base->world_turret_obj->faces = 0;

    /* Use turret_orient to move turret_obj into world coords. Store the
       result in world_turret_obj. */

    orient_object( blue_base->turret_obj, &(blue_base->turret_orient),
                   blue_base->world_turret_obj );


    /* union world_turret_obj and world_base_object into world_union_object */

    object_copy( blue_base->world_union_obj, blue_base->world_base_obj );
    union_object( blue_base->world_turret_obj, blue_base->world_union_obj );

    /* intialize the bounding box for world_union_object */

    find_bounding_box( blue_base->world_union_obj, &(blue_base->box) );

    /* intialize the edge table for world_union_object */

    create_edge_table( blue_base->world_union_obj, &(blue_base->edge_table) );
}

/* Hack Alert!!! */

void move_base_projectiles_forward( RadarBase *base, Pylons *pylons, Player *player )
{
    Projectile *p, *last;
    long face_index;
    Orientation o;
    Vehicle v; /* Hack Alert */

    /* copy fields of base into v */

    v.laser_damage = base->laser_damage;
    v.laser_speed = base->laser_speed;

    last = NULL;
    p = base->projectile_list;

    while( p != NULL ) {

        /* move the projectile forward */

        if( p->type == LASER ) {

            move_projectile_forward_3d( p, &v );
            magic_orient_object( p->obj, &(p->orient), p->world_obj );
            init_projectile_bounding_box( p );

        }
        /* else {
            move_missile_forward( p, v );
            magic_orient_object( p->obj, &(p->orient), p->world_obj );
            init_projectile_bounding_box( p );
        } */

        /* check for collisions */

        if( ( p->frames_left == 0 ) ||
            pylon_projectile_collision( pylons, p, &face_index ) ||
            player_projectile_collision( player, p, &v ) ||
            projectile_cylinder_collision( p ) ) {

            if( last == NULL ) {

                /* add this explosion to the explosion list */

                o.position[X] = mtof( base->projectile_list->orient.position[X] );
                o.position[Y] = mtof( base->projectile_list->orient.position[Y] );
                o.position[Z] = mtof( base->projectile_list->orient.position[Z] );

                o.front[X] = -mtof( base->projectile_list->orient.front[X] );
                o.front[Y] = -mtof( base->projectile_list->orient.front[Y] );
                o.front[Z] = -mtof( base->projectile_list->orient.front[Z] );

                o.up[X] = mtof( base->projectile_list->orient.up[X] );
                o.up[Y] = mtof( base->projectile_list->orient.up[Y] );
                o.up[Z] = mtof( base->projectile_list->orient.up[Z] );

                if( base->projectile_list->type == LASER ) {
                    add_explosion( laser_explosion, &o, NULL, base->projectile_list->world_obj->face[0].gradient );
                }
                else if( base->projectile_list->type == MISSILE ) {
                    add_explosion( missile_explosion, &o, NULL, base->projectile_list->world_obj->face[0].gradient );
                }

                p = p->next;
                free( base->projectile_list->obj->point );
                base->projectile_list->obj->point = NULL;
                free( base->projectile_list->obj->face );
                base->projectile_list->obj->face = NULL;
                free( base->projectile_list->obj );
                base->projectile_list->obj = NULL;
                free( base->projectile_list->world_obj->point );
                base->projectile_list->world_obj->point = NULL;
                free( base->projectile_list->world_obj->face );
                base->projectile_list->world_obj->face = NULL;
                free( base->projectile_list->world_obj );
                base->projectile_list->world_obj = NULL;
                free( base->projectile_list->edge_table.edge );
                base->projectile_list->edge_table.edge = NULL;
                free( base->projectile_list );
                base->projectile_list = NULL;
                base->projectile_list = p;
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
                last->next->world_obj = NULL;
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

void move_base_projectile_forward( Projectile *p, RadarBase *base )
{
    p->orient.position[X] = p->orient.position[X] + ((p->orient.front[X] * ftom( base->laser_speed )) >> MEXP);
    p->orient.position[Y] = p->orient.position[Y] + ((p->orient.front[Y] * ftom( base->laser_speed )) >> MEXP);
    p->orient.position[Z] = p->orient.position[Z] + ((p->orient.front[Z] * ftom( base->laser_speed )) >> MEXP);
}

void fire_radar_base( RadarBase *base, PointFace *obj )
{
    Projectile *p;
    short count = 0;
    float projectile_speed;
    Gradient bullet_color = 0; /* = 0 to shut up compiler */
    Float_Vector left;
    int i;

    /* Based on the amount of damage a laser can do...we will
       change the color of the projectile */

    switch (base->laser_damage)
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
             SYS_Error("fire_radar_base() : base->laser_damage out of range!!\n");
             break;
        } /* End switch */

    /* create the first projectile */

    p = base->projectile_list;

    /* count the number of projectiles in the list */

    while( p != NULL ) {
        count++;
        p = p->next;
    }

    if( count < base->max_projectiles ) {

        /* create a new projectile */

        if( count == 0 ) {
            base->projectile_list = (Projectile *)malloc( sizeof( Projectile ) );
            p = base->projectile_list;
        }
        else {
            p = base->projectile_list;

            while( p->next != NULL ) {
                p = p->next;
            }
            p->next = (Projectile *)malloc( sizeof( Projectile ) );
            p = p->next;
        }

        /* intialize it */

        p->next = NULL;
        p->type = LASER;
        
        if( base->team == RED_TEAM ) {
            p->owner = 6;
        }
        else if( base->team == BLUE_TEAM ) {
            p->owner = 7;
        }

        /* malloc space for the PointFace */

        if( ( p->obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        /* malloc space for the point array */

        if( ( p->obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        /* copy the points from obj to p->obj */

        memcpy( p->obj->point, obj->point, sizeof( Point ) * obj->points );
        p->obj->points = obj->points;

        /* copy the faces from obj to p->obj */

        memcpy( p->obj->face, obj->face, sizeof( Face ) * obj->faces );
        p->obj->faces = obj->faces;

        if( p->type == LASER ) {
            p->frames_left = base->laser_life;
        }

        /* else if( type == MISSILE ) {
            p->frames_left = base->missile_life;
        } */

        p->orient.front[X] = ftom( base->turret_orient.front[X] );
        p->orient.front[Y] = ftom( base->turret_orient.front[Y] );
        p->orient.front[Z] = ftom( base->turret_orient.front[Z] );

        p->orient.up[X] = ftom( base->turret_orient.up[X] );
        p->orient.up[Y] = ftom( base->turret_orient.up[Y] );
        p->orient.up[Z] = ftom( base->turret_orient.up[Z] );

        if( p->type == LASER ) {

            vector_cross_product( base->turret_orient.up, base->turret_orient.front, left );
            normalize( left );

            p->orient.position[X] = ftom( base->turret_orient.position[X] ) + ftom(0.125f * left[X]);

            p->orient.position[Y] = ftom( base->turret_orient.position[Y] ) + ftom(0.125f * left[Y]);

            p->orient.position[Z] = ftom( base->turret_orient.position[Z] ) + ftom(0.125f * left[Z]);


            /* move it forward a tad */

            projectile_speed = base->laser_speed;
            base->laser_speed = 1.0;

            move_base_projectile_forward( p, base );

            base->laser_speed = projectile_speed;
        }


        /* else if( type == MISSILE ) {
            p->orient.position[X] = ftom( v->orient.position[X] +
                ( (v->surface_rad - 9.6) * v->orient.up[X]) );
            p->orient.position[Y] = ftom( v->orient.position[Y] +
                ( (v->surface_rad - 9.6) * v->orient.up[Y]) );
            p->orient.position[Z] = ftom( v->orient.position[Z] +
                ( (v->surface_rad - 9.6) * v->orient.up[Z]) );
        */
            /* move it forward a tad */
        /*
            projectile_speed = v->missile_speed;
            v->missile_speed = 0.6;

            move_projectile_forward_3d( p, v );

            v->missile_speed = projectile_speed;

            p->target = target;
        } */



        if( ( p->world_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc_failed\n");

        /* malloc space for the point array */

        if( ( p->world_obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->world_obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        p->edge_table.edge = NULL;
        init_projectile_edge_table( p );

        p->on_surface = FALSE;

        /* Change the color of all of the lasers faces */

        if( p->type == LASER ) {
            for( i = 0; i < p->obj->faces; i++ ) {
                p->obj->face[i].gradient = bullet_color;
            }
        }
    }

    /* create the second projectile */

    p = base->projectile_list;

    /* count the number of projectiles in the list */

    while( p != NULL ) {
        count++;
        p = p->next;
    }

    if( count < base->max_projectiles ) {

        /* create a new projectile */

        if( count == 0 ) {
            base->projectile_list = (Projectile *)malloc( sizeof( Projectile ) );
            p = base->projectile_list;
        }
        else {
            p = base->projectile_list;

            while( p->next != NULL ) {
                p = p->next;
            }
            p->next = (Projectile *)malloc( sizeof( Projectile ) );
            p = p->next;
        }

        /* intialize it */

        p->next = NULL;
        p->type = LASER;
        
        if( base->team == RED_TEAM ) {
            p->owner = 6;
        }
        else if( base->team == BLUE_TEAM ) {
            p->owner = 7;
        }

        /* malloc space for the PointFace */

        if( ( p->obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        /* malloc space for the point array */

        if( ( p->obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        /* copy the points from obj to p->obj */

        memcpy( p->obj->point, obj->point, sizeof( Point ) * obj->points );
        p->obj->points = obj->points;

        /* copy the faces from obj to p->obj */

        memcpy( p->obj->face, obj->face, sizeof( Face ) * obj->faces );
        p->obj->faces = obj->faces;

        if( p->type == LASER ) {
            p->frames_left = base->laser_life;
        }

        /* else if( type == MISSILE ) {
            p->frames_left = base->missile_life;
        } */

        p->orient.front[X] = ftom( base->turret_orient.front[X] );
        p->orient.front[Y] = ftom( base->turret_orient.front[Y] );
        p->orient.front[Z] = ftom( base->turret_orient.front[Z] );

        p->orient.up[X] = ftom( base->turret_orient.up[X] );
        p->orient.up[Y] = ftom( base->turret_orient.up[Y] );
        p->orient.up[Z] = ftom( base->turret_orient.up[Z] );

        if( p->type == LASER ) {

            p->orient.position[X] = ftom( base->turret_orient.position[X] ) - ftom(0.125f * left[X]);

            p->orient.position[Y] = ftom( base->turret_orient.position[Y] ) - ftom(0.125f * left[Y]);

            p->orient.position[Z] = ftom( base->turret_orient.position[Z] ) - ftom(0.125f * left[Z]);


            /* move it forward a tad */

            projectile_speed = base->laser_speed;
            base->laser_speed = 1.0;

            move_base_projectile_forward( p, base );

            base->laser_speed = projectile_speed;
        }


        /* else if( type == MISSILE ) {
            p->orient.position[X] = ftom( v->orient.position[X] +
                ( (v->surface_rad - 9.6) * v->orient.up[X]) );
            p->orient.position[Y] = ftom( v->orient.position[Y] +
                ( (v->surface_rad - 9.6) * v->orient.up[Y]) );
            p->orient.position[Z] = ftom( v->orient.position[Z] +
                ( (v->surface_rad - 9.6) * v->orient.up[Z]) );
        */
            /* move it forward a tad */
        /*
            projectile_speed = v->missile_speed;
            v->missile_speed = 0.6;

            move_projectile_forward_3d( p, v );

            v->missile_speed = projectile_speed;

            p->target = target;
        } */



        if( ( p->world_obj = (PointFace *)malloc( sizeof( PointFace ) ) ) == NULL )
            SYS_Error("fire_radar_base : malloc_failed\n");

        /* malloc space for the point array */

        if( ( p->world_obj->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        /* malloc space for the face array */

        if( ( p->world_obj->face = (Face *)malloc( sizeof( Face ) * obj->faces ) ) == NULL )
            SYS_Error("fire_radar_base() : malloc failed\n");

        p->edge_table.edge = NULL;
        init_projectile_edge_table( p );

        p->on_surface = FALSE;

        /* Change the color of all of the lasers faces */

        if( p->type == LASER ) {
            for( i = 0; i < p->obj->faces; i++ ) {
                p->obj->face[i].gradient = bullet_color;
            }
        }
    }
}

void aim_radar_base( RadarBase *base, Player *player, PointFace *obj )
{
    Float_Vector new_front, axis, temp;
    float theta, len, min_len, foo;
    long i, valid_target;
    const float rot_inc = 0.174532f; /* 10 degrees in radians */
    const float max_attack_radius = 15.0f;
    Float_Matrix rot_matrix;
    Projectile *p;

    if( base->alive ) {

        /* find the closest enemy */

        valid_target = FALSE;

        min_len = 100000.0;
        temp[X] = 0.0;
        temp[Y] = 0.0;
        temp[Z] = 0.0;

        for( i = 0; i < 6; i++ ) {

            temp[X] = -base->base_orient.position[X] + player[i].tank.orient.position[X];
            temp[Y] = -base->base_orient.position[Y] + player[i].tank.orient.position[Y];
            temp[Z] = -base->base_orient.position[Z] + player[i].tank.orient.position[Z];

            len = (float)sqrt( vector_dot_product( temp, temp ) );

            if( (len < min_len) && (len < max_attack_radius) && player[i].tank.alive && (!player[i].tank.cloaking_active) ) {
                if( ( player[i].tank.team == RED_TEAM ) && ( base->team == BLUE_TEAM ) ) {

                    valid_target = TRUE;

                    new_front[X] = temp[X];
                    new_front[Y] = temp[Y];
                    new_front[Z] = temp[Z];

                    min_len = len;
                }
                else if( (player[i].tank.team == BLUE_TEAM) && (base->team == RED_TEAM) ) {

                    valid_target = TRUE;

                    new_front[X] = temp[X];
                    new_front[Y] = temp[Y];
                    new_front[Z] = temp[Z];

                    min_len = len;
                }
            }
        }
        
        /* check decoys too */
        
        for( i = 0; i < 6; i++ ) {
        
            p = player[i].tank.projectile_list;
            
            while( p != NULL ) {
                
                if( (p->type == DECOY_0) || (p->type == DECOY_1) || (p->type == DECOY_2) ) {
                
                    temp[X] = -base->base_orient.position[X] + mtof( p->orient.position[X] );
                    temp[Y] = -base->base_orient.position[Y] + mtof( p->orient.position[Y] );
                    temp[Z] = -base->base_orient.position[Z] + mtof( p->orient.position[Z] );

                    len = (float)sqrt( vector_dot_product( temp, temp ) );
                
                    if( (len < min_len) && (len < max_attack_radius) ) {
                    
                        if( ( player[i].tank.team == RED_TEAM ) && ( base->team == BLUE_TEAM ) ) {

                            valid_target = TRUE;

                            new_front[X] = temp[X];
                            new_front[Y] = temp[Y];
                            new_front[Z] = temp[Z];

                            min_len = len;
                        }
                        else if( (player[i].tank.team == BLUE_TEAM) && (base->team == RED_TEAM) ) {

                            valid_target = TRUE;

                            new_front[X] = temp[X];
                            new_front[Y] = temp[Y];
                            new_front[Z] = temp[Z];

                            min_len = len;
                        }
                    }
                }
                p = p->next;
            }
        }        

        /* incase base.position == player.position */

        if( (min_len == 0.0) && (base->team == RED_TEAM) ) {
            new_front[X] = 0.0;
            new_front[Y] = 0.0;
            new_front[Z] = 1.0;
        }
        else if( (min_len == 0.0) && (base->team == BLUE_TEAM) ) {
            new_front[X] = 0.0;
            new_front[Y] = 0.0;
            new_front[Z] = -1.0;
        }

        if( valid_target ) {

            /* new_front is a vector from base->position to target. normalize it */

            normalize( new_front );
            normalize( base->turret_orient.front );

            /* find the angle between turret_front and new_front */

            foo = vector_dot_product( base->turret_orient.front, new_front );

            if( (foo > 1.0) || (foo < -1.0) ) {
                if( foo > 0.0 )
                    foo = 1.0;
                else
                    foo = -1.0;
            }

            theta = (float)acos( foo );

            /* find the axis of rotation */

            vector_cross_product( new_front, base->turret_orient.front, axis );

            if( (axis[X] == 0.0) && (axis[Y] == 0.0) && (axis[Z] == 0.0) ) {

                /* either new_front == base->turret_orient.front or
                   or new_front = -base->turret_orient.front. So
                   we don't rotate. */

                identity_matrix_init( rot_matrix );

            }
            else {

                normalize( axis );

                if( theta > rot_inc ) {
                    theta = rot_inc;
                }

                /* store the transformation that rotates a point theta radians around
                   the axis vector into rot_matrix */

                rotate_around_axis( theta, axis, rot_matrix );

                transformation( rot_matrix, base->turret_orient.front );
            }

            /* now we need to find the new turret_up vector */

            vector_cross_product( base->turret_orient.front, base->base_orient.up, axis );

            if( (axis[X] == 0.0) && (axis[Y] == 0.0) && (axis[Z] == 0.0) ) {

                /* either base->turret_orient.front == base->base_orient.up
                   or base->turret_orient.front == -base->base_orient.up. So
                   we don't rotate. */

                identity_matrix_init( rot_matrix );

            }
            else {

                normalize( axis );

                rotate_around_axis( -PI / 2, axis, rot_matrix );

                base->turret_orient.up[X] = base->turret_orient.front[X];
                base->turret_orient.up[Y] = base->turret_orient.front[Y];
                base->turret_orient.up[Z] = base->turret_orient.front[Z];

                transformation( rot_matrix, base->turret_orient.up );
            }

            /* project turret_front onto the x-z plane */

            new_front[X] = base->turret_orient.front[X];
            new_front[Y] = 0.0;
            new_front[Z] = base->turret_orient.front[Z];

            if( (new_front[X] == 0.0) && (new_front[Y] == 0.0) && (new_front[Z] == 0.0) ) {
                new_front[X] = -base->turret_orient.up[X];
                new_front[Y] = 0.0;
                new_front[Z] = -base->turret_orient.up[Z];
            }

            normalize( new_front );
            normalize( base->base_orient.front );

            foo = vector_dot_product( base->base_orient.front, new_front );

            if( (foo > 1.0) || (foo < -1.0) ) {
                if( foo > 0.0 )
                    foo = 1.0;
                else
                    foo = -1.0;
            }

            theta = (float)acos( foo );

            vector_cross_product( new_front, base->base_orient.front, axis );

            if( (axis[X] == 0.0) && (axis[Y] == 0.0) && (axis[Z] == 0.0) ) {

                /* either new_front == base->base_orient.front
                   or new_front == -base->base_orient.front. So
                   we don't rotate. */

                identity_matrix_init( rot_matrix );

            }
            else {

                normalize( axis );

                rotate_around_axis( theta, axis, rot_matrix );

                transformation( rot_matrix, base->base_orient.front );
                transformation( rot_matrix, base->base_orient.up );
            }

            if( base->frames_till_fire_laser == 0 ) {
                fire_radar_base( base, obj );
                base->frames_till_fire_laser = base->laser_reload_time;
                if( game_configuration.sound_on )
                    {
                     /* sb_mix_sample(sample[BULLET_FIRE]); */
                     Q_Jon_Sample( BULLET_FIRE, base->base_orient.position );
                    }
            }
            else {
                base->frames_till_fire_laser--;
            }
        }
    }
}


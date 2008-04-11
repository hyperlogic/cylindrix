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
#include <stdlib.h>
#include <memory.h>
#include "object.h"
#include "types.h"
#include "explode.h"

extern WorldStuff world_stuff;

/* makes all the faces of obj independent of one another allowing
   each face to move independently. */

void split_pfd( PointFace *obj, PointFace *result )
{
    long num_points, i, j, point_index;

    /* figure out exactly how many points result will need */

    num_points = 0;

    for( i = 0; i < obj->faces; i++ ) {
        num_points += obj->face[i].size;
    }

    /* malloc all the points result will need */

    result->point = (Point *)malloc( sizeof( Point ) * num_points );
    result->points = num_points;

    /* malloc the faces for result */

    result->face = (Face *)malloc( sizeof( Face ) * obj->faces );
    result->faces = obj->faces;

    /* copy all the new points and faces into result */

    point_index = 0;

    for( i = 0; i < obj->faces; i++ ) {

        for( j = 0; j < obj->face[i].size; j++ ) {

            memcpy( result->point[point_index], obj->point[obj->face[i].index[j]], sizeof( Point ) );

            result->face[i].index[j] = point_index;

            point_index++;

        }

        result->face[i].size = obj->face[i].size;
        result->face[i].normal[X] = obj->face[i].normal[X];
        result->face[i].normal[Y] = obj->face[i].normal[Y];
        result->face[i].normal[Z] = obj->face[i].normal[Z];
        result->face[i].max_z = obj->face[i].max_z;
        result->face[i].gradient = obj->face[i].gradient;
        result->face[i].transparent = obj->face[i].gradient;
    }
}

/* adds an explosion to the explosion list */

void add_explosion( ExplosionType type, Orientation *o, PointFace *obj,
                    Gradient gradient )
{
    Explosion *ptr, *last;
    long i;

    last = world_stuff.explosion_list;
    ptr = world_stuff.explosion_list;

    /* make last point to the last node in the explosion list */

    while( ptr != NULL ) {
        last = ptr;
        ptr = ptr->next;
    }

    /* allocate the new node */

    ptr = (Explosion *)malloc( sizeof( Explosion ) );

    if( type == laser_explosion || type == missile_explosion ||
        type == mine_explosion || type == cs_missile_explosion ||
        type == ramming_explosion || type == traitor_missile_explosion ) {

        /* initialize the new node */

        /* init the type field */

        ptr->type = type;

        /* init the orient field */

        ptr->orient.position[X] = o->position[X];
        ptr->orient.position[Y] = o->position[Y];
        ptr->orient.position[Z] = o->position[Z];

        ptr->orient.front[X] = o->front[X];
        ptr->orient.front[Y] = o->front[Y];
        ptr->orient.front[Z] = o->front[Z];

        ptr->orient.up[X] = o->up[X];
        ptr->orient.up[Y] = o->up[Y];
        ptr->orient.up[Z] = o->up[Z];

        /* init the debris field */

        ptr->debris.point = NULL;
        ptr->debris.points = 0;
        ptr->debris.face = NULL;
        ptr->debris.faces = 0;

        if( type == laser_explosion ) {

            split_pfd( &(world_stuff.laser_debris_obj), &(ptr->debris) );

            /* change the object color to gradient */

            for( i = 0; i < ptr->debris.faces; i++ ) {
                ptr->debris.face[i].gradient = gradient;
            }

        }
        else if( type == missile_explosion ) {

            split_pfd( &(world_stuff.missile_debris_obj), &(ptr->debris) );

        }
        else if( type == mine_explosion ) {

            split_pfd( &(world_stuff.mine_debris_obj), &(ptr->debris) );

        }
        else if( type == cs_missile_explosion ) {

            split_pfd( &(world_stuff.cs_missile_debris_obj), &(ptr->debris) );

        }
        else if( type == ramming_explosion ) {

            split_pfd( &(world_stuff.ramming_debris_obj), &(ptr->debris) );

            /* change the color of the debris to gradient */

            for( i = 0; i < ptr->debris.faces; i++ ) {
                ptr->debris.face[i].gradient = gradient;
            }
        }
        else if( type == traitor_missile_explosion ) {

            split_pfd( &(world_stuff.traitor_missile_debris_obj), &(ptr->debris) );

        }
        
        /* make sure the explosions aren't transparent */
        
        for( i = 0; i < ptr->debris.faces; i++ ) {
            ptr->debris.face[i].transparent = FALSE;
        }


        /* init the world_debris field */

        ptr->world_debris.point = NULL;
        ptr->world_debris.points = 0;
        ptr->world_debris.face = NULL;
        ptr->world_debris.faces = 0;

        orient_object( &(ptr->debris), &(ptr->orient), &(ptr->world_debris) );

        /* init the cloud field */

        ptr->cloud.point = NULL;
        ptr->cloud.points = 0;
        ptr->cloud.face = NULL;
        ptr->cloud.faces = 0;

        /* init the world_cloud field */

        ptr->world_cloud.point = NULL;
        ptr->world_cloud.points = 0;
        ptr->world_cloud.face = NULL;
        ptr->world_cloud.faces = 0;

        /* init the frames_left field */

        if( ptr->type == laser_explosion ) {
            ptr->frames_left = 5;
        }
        else if( ptr->type == missile_explosion ) {
            ptr->frames_left = 10;
        }
        else if( ptr->type == mine_explosion ) {
            ptr->frames_left = 10;
        }
        else if( ptr->type == cs_missile_explosion ) {
            ptr->frames_left = 10;
        }
        else if( ptr->type == ramming_explosion ) {
            ptr->frames_left = 10;
        }
        else if( ptr->type == traitor_missile_explosion ) {
            ptr->frames_left = 10;
        }

        /* init the next field */

        ptr->next = NULL;

        /* make sure that something points to the new node */

        if( last == NULL ) {
            world_stuff.explosion_list = ptr;
        }
        else {
            last->next = ptr;
        }
    }
    else if( type == ship_explosion || type == radar_base_explosion ) {

        /* initialize the new node */

        /* init the type field */

        ptr->type = type;

        /* init the orient field */

        ptr->orient.position[X] = o->position[X];
        ptr->orient.position[Y] = o->position[Y];
        ptr->orient.position[Z] = o->position[Z];

        ptr->orient.front[X] = o->front[X];
        ptr->orient.front[Y] = o->front[Y];
        ptr->orient.front[Z] = o->front[Z];

        ptr->orient.up[X] = o->up[X];
        ptr->orient.up[Y] = o->up[Y];
        ptr->orient.up[Z] = o->up[Z];

        /* init the debris field */

        ptr->debris.point = NULL;
        ptr->debris.points = 0;
        ptr->debris.face = NULL;
        ptr->debris.faces = 0;

        split_pfd( obj, &(ptr->debris) );
        
        /* make sure the faces aren't transparent */
        
        for( i = 0; i < ptr->debris.faces; i++ ) {
            ptr->debris.face[i].transparent = FALSE;
        }

        /* init the world_debris field */

        ptr->world_debris.point = NULL;
        ptr->world_debris.points = 0;
        ptr->world_debris.face = NULL;
        ptr->world_debris.faces = 0;

        orient_object( &(ptr->debris), &(ptr->orient), &(ptr->world_debris) );

        /* init the cloud field */

        ptr->cloud.point = NULL;
        ptr->cloud.points = 0;
        ptr->cloud.face = NULL;
        ptr->cloud.faces = 0;

        split_pfd( &(world_stuff.explosion_cloud_obj), &(ptr->cloud) );
        
        /* make sure the faces aren't transparent */
        
        for( i = 0; i < ptr->cloud.faces; i++ ) {
            ptr->cloud.face[i].transparent = FALSE;
        }

        /* init the world_cloud field */

        ptr->world_cloud.point = NULL;
        ptr->world_cloud.points = 0;
        ptr->world_cloud.face = NULL;
        ptr->world_cloud.faces = 0;

        orient_object( &(ptr->cloud), &(ptr->orient), &(ptr->world_cloud) );

        /* init the frames_left field */

        ptr->frames_left = 30;

        /* init the next field */

        ptr->next = NULL;

        /* make sure that something points to the new node */

        if( last == NULL ) {
            world_stuff.explosion_list = ptr;
        }
        else {
            last->next = ptr;
        }
    }
}

/* adds all the points and faces of all the explosion objects to
   result */

void explosion_union( PointFace *result )
{
    Explosion *ptr;

    ptr = world_stuff.explosion_list;

    while( ptr != NULL ) {
        if( ptr->type == laser_explosion || ptr->type == missile_explosion ||
            ptr->type == mine_explosion || ptr->type == cs_missile_explosion ||
            ptr->type == ramming_explosion || ptr->type == traitor_missile_explosion ) {
            union_object( &(ptr->world_debris), result );
        }
        else if( ptr->type == ship_explosion || ptr->type == radar_base_explosion ) {
            union_object( &(ptr->world_debris), result );
            union_object( &(ptr->world_cloud), result );
        }
        ptr = ptr->next;
    }
}

void explode_object_fast( PointFace *obj )
{
    long i, j;

    for( i = 0; i < obj->faces; i++ ) {
        for( j = 0; j < obj->face[i].size; j++ ) {

            obj->point[obj->face[i].index[j]][X] += (obj->face[i].normal[X] >> 3);
            obj->point[obj->face[i].index[j]][Y] += (obj->face[i].normal[Y] >> 3);
            obj->point[obj->face[i].index[j]][Z] += (obj->face[i].normal[Z] >> 3);

        }
    }
}

void explode_object_slow( PointFace *obj )
{
    long i, j;

    for( i = 0; i < obj->faces; i++ ) {
        for( j = 0; j < obj->face[i].size; j++ ) {

            obj->point[obj->face[i].index[j]][X] += (obj->face[i].normal[X] >> 5);
            obj->point[obj->face[i].index[j]][Y] += (obj->face[i].normal[Y] >> 5);
            obj->point[obj->face[i].index[j]][Z] += (obj->face[i].normal[Z] >> 5);

        }
    }
}


void double_side_object( PointFace *obj )
{
    int i, j;

    /* reallocate more space for the new faces */

    obj->face = (Face *)realloc( obj->face, sizeof( Face ) * obj->faces * 2 );

    for( i = 0; i < obj->faces; i++ ) {
        for( j = 0; j < obj->face[i].size; j++ ) {
            obj->face[obj->faces + i].index[j] = obj->face[i].index[j];
        }
        obj->face[obj->faces + i].size = obj->face[i].size;
        obj->face[obj->faces + i].normal[X] = -obj->face[i].normal[X];
        obj->face[obj->faces + i].normal[Y] = -obj->face[i].normal[Y];
        obj->face[obj->faces + i].normal[Z] = -obj->face[i].normal[Z];
        obj->face[obj->faces + i].max_z = obj->face[i].max_z;
        obj->face[obj->faces + i].gradient = obj->face[i].gradient;
        obj->face[obj->faces + i].transparent = obj->face[i].transparent;
    }
    obj->faces = obj->faces * 2;
}

/* animates all the explosions one frame */

void animate_explosions()
{
    Explosion *ptr, *last;

    /* delete all explosions with no frames_left */

    last = NULL;
    ptr = world_stuff.explosion_list;

    while( ptr != NULL ) {

        if( ptr->frames_left == 0 ) {

            if( last == NULL ) {

                ptr = ptr->next;

                free( world_stuff.explosion_list->debris.point );
                free( world_stuff.explosion_list->debris.face );
                free( world_stuff.explosion_list->world_debris.point );
                free( world_stuff.explosion_list->world_debris.face );
                free( world_stuff.explosion_list->cloud.point );
                free( world_stuff.explosion_list->cloud.face );
                free( world_stuff.explosion_list->world_cloud.point );
                free( world_stuff.explosion_list->world_cloud.face );

                free( world_stuff.explosion_list );

                world_stuff.explosion_list = ptr;
            }
            else {

                ptr = ptr->next;

                free( last->next->debris.point );
                free( last->next->debris.face );
                free( last->next->world_debris.point );
                free( last->next->world_debris.face );
                free( last->next->cloud.point );
                free( last->next->cloud.face );
                free( last->next->world_cloud.point );
                free( last->next->world_cloud.face );

                free( last->next );

                last->next = ptr;
            }
        }
        else {
            ptr->frames_left--;
            last = ptr;
            ptr = ptr->next;
        }
    }

    /* animate all remaining explosions */

    ptr = world_stuff.explosion_list;

    while( ptr != NULL ) {

        if( ptr->type == laser_explosion || ptr->type == missile_explosion ||
            ptr->type == mine_explosion || ptr->type == cs_missile_explosion ||
            ptr->type == ramming_explosion || ptr->type == traitor_missile_explosion ) {

            /* animate the debris */

            explode_object_fast( &(ptr->debris) );
            orient_object( &(ptr->debris), &(ptr->orient), &(ptr->world_debris) );
            double_side_object( &(ptr->world_debris) );
        }
        else if( ptr->type == ship_explosion || ptr->type == radar_base_explosion ) {

            if( ptr->frames_left >= 10 ) {

                /* animate the debris */

                explode_object_fast( &(ptr->debris) );
                orient_object( &(ptr->debris), &(ptr->orient), &(ptr->world_debris) );
                double_side_object( &(ptr->world_debris) );

            }
            else {

                 /* delete the debris */

                 free( ptr->world_debris.point );
                 ptr->world_debris.point = NULL;
                 ptr->world_debris.points = 0;
                 free( ptr->world_debris.face );
                 ptr->world_debris.face = NULL;
                 ptr->world_debris.faces = 0;

            }

            /* animate the cloud */

            explode_object_slow( &(ptr->cloud) );
            orient_object( &(ptr->cloud), &(ptr->orient), &(ptr->world_cloud) );
            double_side_object( &(ptr->world_cloud) );
        }
        ptr = ptr->next;
    }
}


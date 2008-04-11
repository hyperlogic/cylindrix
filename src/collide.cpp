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

#include "collide.h"
#include <stdio.h>
#include <stdlib.h>
#include "object.h"

#include "types.h"
#include "explode.h"
#include "ai.h"
#include "ai_util.h"
#include "stats.h"
#include <math.h>
#include "jonsb.h"

extern game_stats_type game_stats; /* From omega.c */
extern int debug;
extern game_configuration_type game_configuration; /* From omega.c */
extern SYS_SOUNDHANDLE sample[MAX_WORLD_SAMPLES]; /* From omega.c */
extern player_events_type player_events[6]; /* From ai.c...i think */
extern WorldStuff world_stuff;

const float Cylinder_Length = 60.0; /* 60.0 */
const long Magic_Cylinder_Length = 61440; /* 61440 */

void print_bounding_box( BoundingBox *b )
{
    printf("BoundingBox (%.5f,%.5f,%.5f,%.5f,%.5f,%.5f)\n",
        b->min_x, b->max_x, b->min_y, b->max_y, b->min_z, b->max_z );
}

void print_magic_bounding_box( MagicBoundingBox *b )
{
    printf("BoundingBox (%ld,%ld,%ld,%ld,%ld,%ld)\n",
        b->min_x, b->max_x, b->min_y, b->max_y, b->min_z, b->max_z );
}


int x_overlap( BoundingBox *p, BoundingBox *v )
{
    if( p->max_x < v->min_x ) {
        return FALSE;
    }
    else if( v->max_x < p->min_x ) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

int y_overlap( BoundingBox *p, BoundingBox *v )
{
    if( p->max_y < v->min_y ) {
        return FALSE;
    }
    else if( v->max_y < p->min_y ) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

int z_overlap( BoundingBox *p, BoundingBox *v )
{
    if( p->max_z < v->min_z ) {
        return FALSE;
    }
    else if( v->max_z < p->min_z ) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

int bounding_box_overlap( BoundingBox *p, BoundingBox *q )
{
    if( p->max_x < q->min_x ) {
        return FALSE;
    }
    else if( q->max_x < p->min_x ) {
        return FALSE;
    }
    else {

        if( p->max_y < q->min_y ) {
            return FALSE;
        }
        else if( q->max_y < p->min_y ) {
            return FALSE;
        }
        else {

            if( p->max_z < q->min_z ) {
                return FALSE;
            }
            else if( q->max_z < p->min_z ) {
                return FALSE;
            }
            else {
                return TRUE;
            }
        }
    }
}

int bounding_box_overlap_2d( BoundingBox2d *p, BoundingBox2d *q )
{
    if( p->max_x < q->min_x ) {
        return FALSE;
    }
    else if( q->max_x < p->min_x ) {
        return FALSE;
    }
    else {

        if( p->max_y < q->min_y ) {
            return FALSE;
        }
        else if( q->max_y < p->min_y ) {
            return FALSE;
        }
        else {
             return TRUE;
        }
    }
}


int magic_x_overlap( MagicBoundingBox *p, MagicBoundingBox *v )
{
    if( p->max_x < v->min_x ) {
        return FALSE;
    }
    else if( v->max_x < p->min_x ) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

int magic_y_overlap( MagicBoundingBox *p, MagicBoundingBox *v )
{
    if( p->max_y < v->min_y ) {
        return FALSE;
    }
    else if( v->max_y < p->min_y ) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

int magic_z_overlap( MagicBoundingBox *p, MagicBoundingBox *v )
{
    if( p->max_z < v->min_z ) {
        return FALSE;
    }
    else if( v->max_z < p->min_z ) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

int magic_bounding_box_overlap( MagicBoundingBox *p, MagicBoundingBox *q )
{
    if( p->max_x < q->min_x ) {
        return FALSE;
    }
    else if( q->max_x < p->min_x ) {
        return FALSE;
    }
    else {

        if( p->max_y < q->min_y ) {
            return FALSE;
        }
        else if( q->max_y < p->min_y ) {
            return FALSE;
        }
        else {

            if( p->max_z < q->min_z ) {
                return FALSE;
            }
            else if( q->max_z < p->min_z ) {
                return FALSE;
            }
            else {
                return TRUE;
            }
        }
    }
}

int in_poly_xy( Face *f, Point *v, Point p )
{
    int i, j, c = 0;

    for( i = 0, j = f->size - 1; i < f->size; j = i++ ) {
        if( ( ( ( v[f->index[i]][Y] <= p[Y] ) &&
              ( p[Y] < v[f->index[j]][Y] ) ) ||
              ( ( v[f->index[j]][Y] <= p[Y] ) &&
              ( p[Y] < v[f->index[i]][Y] ) ) ) &&
              ( p[X] < (( v[f->index[j]][X] - v[f->index[i]][X] ) *
              ( p[Y] - v[f->index[i]][Y] )) / ( v[f->index[j]][Y] -
              v[f->index[i]][Y] ) + v[f->index[i]][X] ) )
            c = !c;
    }
    return c;
}

int in_poly_xz( Face *f, Point *v, Point p )
{
    int i, j, c = 0;

    for( i = 0, j = f->size - 1; i < f->size; j = i++ ) {
        if( ( ( ( v[f->index[i]][Z] <= p[Z] ) &&
              ( p[Z] < v[f->index[j]][Z] ) ) ||
              ( ( v[f->index[j]][Z] <= p[Z] ) &&
              ( p[Z] < v[f->index[i]][Z] ) ) ) &&
              ( p[X] < (( v[f->index[j]][X] - v[f->index[i]][X] ) *
              ( p[Z] - v[f->index[i]][Z] )) / ( v[f->index[j]][Z] -
              v[f->index[i]][Z] ) + v[f->index[i]][X] ) )
            c = !c;
    }
    return c;
}

int in_poly_yz( Face *f, Point *v, Point p )
{
    int i, j, c = 0;

    for( i = 0, j = f->size - 1; i < f->size; j = i++ ) {
        if( ( ( ( v[f->index[i]][Z] <= p[Z] ) &&
              ( p[Z] < v[f->index[j]][Z] ) ) ||
              ( ( v[f->index[j]][Z] <= p[Z] ) &&
              ( p[Z] < v[f->index[i]][Z] ) ) ) &&
              ( p[Y] < (( v[f->index[j]][Y] - v[f->index[i]][Y] ) *
              ( p[Z] - v[f->index[i]][Z] )) / ( v[f->index[j]][Z] -
              v[f->index[i]][Z] ) + v[f->index[i]][Y] ) )
            c = !c;
    }
    return c;
}


long absolute_val( long num )
{
    if( num < 0 ) {
        return -num;
    }
    else {
        return num;
    }
}

int edge_poly_intersect( Point *edge_vert, Edge edge,
                         Point *face_vert, Face *face )
{
    long A, B, C, D, t, max_coeff, t_denom;
    Point inter;
    Vector v;

    /*
    if( debug ) {
        printf("\nedge_poly_intersect()\n");
        printf("edge = ( %ld, %ld )\n", edge[0], edge[1] );
        printf("edge_vert[%ld] = ( %ld, %ld, %ld )\n", edge[0],
               edge_vert[edge[0]][X], edge_vert[edge[0]][Y],
               edge_vert[edge[0]][Z] );
        printf("edge_vert[%ld] = ( %ld, %ld, %ld )\n", edge[1],
               edge_vert[edge[1]][X], edge_vert[edge[1]][Y],
               edge_vert[edge[1]][Z] );

        printf("face = ( %ld, %ld, %ld, %ld )\n", face->index[0],
            face->index[1], face->index[2], face->index[3] );

        for( i = 0; i < 4; i++ ) {
            printf("face_vert[%ld] = ( %ld, %ld, %ld)\n", i,
                face_vert[face->index[i]][X],
                face_vert[face->index[i]][Y],
                face_vert[face->index[i]][Z] );
        }
    }
    */

    A = face->normal[X];
    B = face->normal[Y];
    C = face->normal[Z];
    D = - ( ( ( A * face_vert[face->index[0]][X] ) >> MEXP ) +
            ( ( B * face_vert[face->index[0]][Y] ) >> MEXP ) +
            ( ( C * face_vert[face->index[0]][Z] ) >> MEXP ) );

    /*
    if( debug ) {
        printf( " A = %.5f, B = %.5f, C = %.5f, D = %.5f\n",
                mtof( A ), mtof( B ), mtof( C ), mtof( D ) );
    }
    */

    v[X] = edge_vert[edge[1]][X] - edge_vert[edge[0]][X];
    v[Y] = edge_vert[edge[1]][Y] - edge_vert[edge[0]][Y];
    v[Z] = edge_vert[edge[1]][Z] - edge_vert[edge[0]][Z];

    /*
    if( debug ) {
        printf("v = ( %ld, %ld, %ld )\n", v[X], v[Y], v[Z] );
    }
    */

    t_denom = ( ((A * v[X]) >> MEXP) + ((B * v[Y]) >> MEXP) + ((C * v[Z]) >> MEXP) );

    /*
    if( debug ) {
        printf("t_denom = %.5f\n", mtof( t_denom ) );
    }
    */

    if( t_denom == 0 ) {
        return FALSE;
    }

    t = ( - (( ((A * edge_vert[edge[0]][X]) >> MEXP) +
               ((B * edge_vert[edge[0]][Y]) >> MEXP) +
               ((C * edge_vert[edge[0]][Z]) >> MEXP) + D) << MEXP ) ) / t_denom;

    /*
    if( debug ) {
        printf("t = %.5f\n", mtof( t ) );
    }
    */

    if( t < 0 || t > MAGIC ) {
        return FALSE;
    }

    inter[X] = ((v[X] * t) >> MEXP) + edge_vert[edge[0]][X];
    inter[Y] = ((v[Y] * t) >> MEXP) + edge_vert[edge[0]][Y];
    inter[Z] = ((v[Z] * t) >> MEXP) + edge_vert[edge[0]][Z];

    max_coeff = absolute_val( A );
    if( max_coeff < absolute_val( B ) )
        max_coeff = absolute_val( B );
    if( max_coeff < absolute_val( C ) )
        max_coeff = absolute_val( C );

    /*
    if( debug ) {
        printf("max_coeff = %.5f\n", mtof( max_coeff ) );
    }
    */

    if( max_coeff == absolute_val( A ) ) {  /* project against yz plane */
        /*
        if( debug ) {
            printf("project yz\n");
        }
        */
        if( in_poly_yz( face, face_vert, inter ) )
            return TRUE;
        else
            return FALSE;
    }
    else if( max_coeff == absolute_val( B ) ) {  /* project against xz plae */
        /*
        if( debug ) {
            printf("project xz\n");
        }
        */

        if( in_poly_xz( face, face_vert, inter ) )
            return TRUE;
        else
            return FALSE;
    }
    else {  /* project against xy plane */
        /*
        if( debug ) {
            printf("project xy\n");
        }
        */

        if( in_poly_xy( face, face_vert, inter ) )
            return TRUE;
        else
            return FALSE;
    }
}

int pointface_edge_intersect( PointFace *obj, Point *edge_vert, EdgeTable *et,
                              long *face_index )
{
    long i, j;

    for( i = 0; i < obj->faces; i++ ) {
        for( j = 0; j < et->edges; j++ ) {
            if( edge_poly_intersect( edge_vert, et->edge[j],
                obj->point, &(obj->face[i]) ) ) {
                *face_index = i;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* This function returns TRUE when the edge table et intersect one of the
   faces in obj, FALSE otherwise.  All the faces that intersect with et
   will have their normal vectors added to reflect. */

int super_pointface_edge_intersect( PointFace *obj, Point *edge_vert,
                                    EdgeTable *et, Float_Vector reflect )
{
    long i, j;
    int intersect = FALSE;

    for( i = 0; i < obj->faces; i++ ) {

        for( j = 0; j < et->edges; j++ ) {

            if( edge_poly_intersect( edge_vert, et->edge[j],
                obj->point, &(obj->face[i]) ) ) {

                /* add this faces normal vector to reflect */

                reflect[X] += mtof( obj->face[i].normal[X] );
                reflect[Y] += mtof( obj->face[i].normal[Y] );
                reflect[Z] += mtof( obj->face[i].normal[Z] );

                intersect = TRUE;
            }
        }
    }

    return intersect;
}

/* This function checks the edge_table of vehicle to see if it intersects
   with any of the pylons.  It returns TRUE if there are one or more
   collisions, FALSE if there are none. The normal vectors of all the faces
   that collide with the vehicle are added to the reflect vector (thats
   why its called super) */

int super_pylon_collision( Pylons *p, Vehicle *v, Float_Vector reflect )
{
    long i, collision = 0;

    for( i = 0; i < p->pylons; i++ ) {
        if( magic_bounding_box_overlap( &(p->pylon[i].mbox), &(v->mbox) ) &&
            super_pointface_edge_intersect( &(p->pylon[i].obj),
            v->world_collision_obj->point, &(v->collision_edges), reflect ) ) {

            if( (v->team == RED_TEAM) && (p->pylon[i].owner == BLUE_TEAM) ) {
                change_pylon_color( &(p->pylon[i]), RedCapturedPylonGrad );
                p->pylon[i].owner = RED_TEAM;

                if( Return_Vehicle_Index(v) == game_stats.user_vehicle )
                    game_stats.pylons_grabbed++;

                if( game_configuration.sound_on )
                    {
                    /* sb_mix_sample(sample[PYLON_TURN_RED]); */
                    Q_Jon_Sample( PYLON_TURN_RED, v->orient.position );
                    }
            }
            else if( (v->team == BLUE_TEAM) && (p->pylon[i].owner == RED_TEAM) ) {
                change_pylon_color( &(p->pylon[i]), BlueCapturedPylonGrad );
                p->pylon[i].owner = BLUE_TEAM;

                if( Return_Vehicle_Index(v) == game_stats.user_vehicle )
                    game_stats.pylons_grabbed++;

                if( game_configuration.sound_on )
                    {
                     /* sb_mix_sample(sample[PYLON_TURN_BLUE]); */
                     Q_Jon_Sample( PYLON_TURN_BLUE, v->orient.position );
                    }
            }
            else if( (v->team == RED_TEAM) && (p->pylon[i].owner == NO_TEAM) ) {
                change_pylon_color( &(p->pylon[i]), RedCapturedPylonGrad );
                p->pylon[i].owner = RED_TEAM;

                if( Return_Vehicle_Index(v) == game_stats.user_vehicle )
                    game_stats.pylons_grabbed++;

                if( game_configuration.sound_on )
                    {
                    /* sb_mix_sample(sample[PYLON_TURN_RED]); */
                    Q_Jon_Sample( PYLON_TURN_RED, v->orient.position );
                    }
            }
            else if( (v->team == BLUE_TEAM) && (p->pylon[i].owner == NO_TEAM) ) {
                change_pylon_color( &(p->pylon[i]), BlueCapturedPylonGrad );
                p->pylon[i].owner = BLUE_TEAM;

                if( Return_Vehicle_Index(v) == game_stats.user_vehicle )
                    game_stats.pylons_grabbed++;

                if( game_configuration.sound_on )
                    {
                     /* sb_mix_sample(sample[PYLON_TURN_BLUE]); */
                     Q_Jon_Sample( PYLON_TURN_BLUE, v->orient.position );
                    }
            }
            else {
                 if( game_configuration.sound_on )
                     {
                      /* sb_mix_sample(sample[PYLON_COLLISION]); */
                      Q_Jon_Sample( PYLON_COLLISION, v->orient.position );
                     }
            }
            collision++;
        }
    }
    if( collision == 0 ) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

int pylon_projectile_collision( Pylons *pylons, Projectile *p, long *face_index )
{
    long i;
    Float_Point point; /* To convert projectile position */


    for( i = 0; i < pylons->pylons; i++ ) {
        if( magic_bounding_box_overlap( &(pylons->pylon[i].mbox), &(p->box) ) &&
            pointface_edge_intersect( &(pylons->pylon[i].obj), p->world_obj->point,
                                      &(p->edge_table), face_index ) ) {
            if( game_configuration.sound_on )
                {
                 point[X] = mtof( p->orient.position[X] );
                 point[Y] = mtof( p->orient.position[Y] );
                 point[Z] = mtof( p->orient.position[Z] );
                 if( p->type == LASER )
                     Q_Jon_Sample( BULLET_MISS, point );
                 else if( p->type == MISSILE )
                     Q_Jon_Sample( MISSILE_MISS, point );
                }
            return TRUE;
         }
    }
    return FALSE;
}

int vehicle_projectile_collision( Vehicle *v, Projectile *p )
{
    long face_index;

    if( magic_bounding_box_overlap( &(p->box), &(v->mbox) ) &&
        pointface_edge_intersect( v->world_collision_obj, p->world_obj->point,
                                  &(p->edge_table), &face_index ) ) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

int super_player_collision( Player *player, Vehicle *v, Float_Vector reflect )
{
    int i, collision = 0;
    Orientation o;

    for( i = 0; i < 6; i++ ) {
        if( &player[i].tank != v ) {
            if( player[i].tank.alive && v->alive &&
                magic_bounding_box_overlap( &(v->mbox), &(player[i].tank.mbox)) &&
                super_pointface_edge_intersect( player[i].tank.world_collision_obj,
                v->world_collision_obj->point, &(v->collision_edges), reflect ) ) {

                if( v->ramming_active ) {
                    player[i].tank.current_hitpoints -= v->ramming_damage;

                    o.front[X] = -v->orient.front[X];
                    o.front[Y] = -v->orient.front[Y];
                    o.front[Z] = -v->orient.front[Z];

                    o.up[X] = v->orient.up[X];
                    o.up[Y] = v->orient.up[Y];
                    o.up[Z] = v->orient.up[Z];

                    o.position[X] = (v->orient.position[X] + player[i].tank.orient.position[X]) / 2;
                    o.position[Y] = (v->orient.position[Y] + player[i].tank.orient.position[Y]) / 2;
                    o.position[Z] = (v->orient.position[Z] + player[i].tank.orient.position[Z]) / 2;

                    add_explosion( ramming_explosion, &(o), &(world_stuff.ramming_debris_obj), player[i].tank.obj->face[0].gradient );
                }

                if( player[i].tank.current_hitpoints <= 0 ) {
                    player[i].tank.current_hitpoints = 0;
                    player[i].tank.alive = FALSE;

                    add_explosion( ship_explosion, &(player[i].tank.orient), player[i].tank.obj, 0 );

                    if( game_configuration.sound_on ) {
                        Q_Jon_Sample( VEHICLE_EXPLOSION, player[i].tank.orient.position );
                    }
                }

                collision++;
            }
        }
    }
    if( collision == 0 ) {
        return FALSE;
    }
    else {
        if( game_configuration.sound_on ) {
            Q_Jon_Sample( VEHICLE_COLLISION, v->orient.position );
        }
        return TRUE;
    }
}

/* vehicle_vehicle_collision() checks to see if v1 and v2 are currently
   colliding with one another.  The vector reflect will contain the sum of
   the normal vectors of v2's faces that v1 intersects with. */

int vehicle_vehicle_collision( Vehicle *v1, Vehicle *v2,
                               Float_Vector reflect )
{
    if( v1->alive && v2->alive ) {

        if( magic_bounding_box_overlap( &(v1->mbox), &(v2->mbox) ) ) {

            if( super_pointface_edge_intersect( v2->world_collision_obj,
                v1->world_collision_obj->point, &(v1->collision_edges), reflect ) ) {

                /* collision!! */

                return TRUE;
            }
        }
    }

    return FALSE;
}


int super_radar_base_collision( RadarBase *base, Vehicle *v, Float_Vector reflect )
{
    PointFace temp_obj;
    Orientation o;

    temp_obj.point = NULL;
    temp_obj.points = 0;
    temp_obj.face = NULL;
    temp_obj.faces = 0;

    if( base->alive ) {

        if( v->alive && bounding_box_overlap( &(v->box), &(base->box) ) &&
            super_pointface_edge_intersect( base->world_union_obj,
            v->world_collision_obj->point, &(v->collision_edges), reflect ) ) {

            if( v->ramming_active ) {
                base->current_hitpoints -= v->ramming_damage;

                o.front[X] = -v->orient.front[X];
                o.front[Y] = -v->orient.front[Y];
                o.front[Z] = -v->orient.front[Z];

                o.up[X] = v->orient.up[X];
                o.up[Y] = v->orient.up[Y];
                o.up[Z] = v->orient.up[Z];

                o.position[X] = (v->orient.position[X] + base->base_orient.position[X]) / 2;
                o.position[Y] = (v->orient.position[Y] + base->base_orient.position[Y]) / 2;
                o.position[Z] = (v->orient.position[Z] + base->base_orient.position[Z]) / 2;

                add_explosion( ramming_explosion, &(o), &(world_stuff.ramming_debris_obj), base->base_obj->face[0].gradient );
            }

            if( base->current_hitpoints <= 0 ) {
                base->current_hitpoints = 0;
                base->alive = FALSE;

                union_object( base->turret_obj, &(temp_obj) );
                union_object( base->base_obj, &(temp_obj) );

                add_explosion( radar_base_explosion, &(base->base_orient), &(temp_obj), 0 );

                if( game_configuration.sound_on ) {
                    Q_Jon_Sample( VEHICLE_EXPLOSION, base->base_orient.position );
                }
            }

            if( game_configuration.sound_on ) {
                Q_Jon_Sample( VEHICLE_COLLISION, v->orient.position );
            }

            free( temp_obj.point );
            free( temp_obj.face );

            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else {
        return FALSE;
    }
}

int player_projectile_collision( Player *player, Projectile *p, Vehicle *v )
{
    int i, temp_int;

    for( i = 0; i < 6; i++ ) {
        if( &player[i].tank != v ) {
            if( player[i].tank.alive &&
                vehicle_projectile_collision( &(player[i].tank), p ) ) {

                /* inflict damage */

                if( p->type == LASER )
                    {
                     player[i].tank.current_hitpoints -= v->laser_damage;
                     Update_Bullet_Hit( v->laser_damage, i, p );
                     if( game_configuration.sound_on )
                         {
                          /* sb_mix_sample(sample[BULLET_HIT]); */
                          Q_Jon_Sample( BULLET_HIT, player[i].tank.orient.position );
                         }
                    }
                else if( p->type == MISSILE )
                    {
                     player[i].tank.current_hitpoints -= v->missile_damage;
                     Update_Bullet_Hit( v->missile_damage, i, p );
                     if( game_configuration.sound_on )
                         {
                          /* sb_mix_sample(sample[MISSILE_HIT]); */
                          Q_Jon_Sample( MISSILE_HIT, player[i].tank.orient.position );
                         }
                    }
                else if( p->type == MINE ) {
                    player[i].tank.current_hitpoints -= v->mine_damage;
                    if( game_configuration.sound_on ) {
                        Q_Jon_Sample( MISSILE_HIT, player[i].tank.orient.position );
                    }
                }
                else if( p->type == CS_MISSILE ) {
                    player[i].tank.controls_scrambled = TRUE;
                    player[i].tank.frames_till_unscramble = player[i].tank.scramble_life;
                    if( game_configuration.sound_on ) {
                        Q_Jon_Sample( MISSILE_HIT, player[i].tank.orient.position );
                    }
                }
                else if( p->type == TRAITOR_MISSILE ) {
                    player[i].tank.traitor_active = TRUE;
                    player[i].tank.frames_till_traitor_deactivate = player[i].tank.traitor_life;
                    if( game_configuration.sound_on ) {
                        Q_Jon_Sample( MISSILE_HIT, player[i].tank.orient.position );
                    }
                }

                /* STAT SHIT */
                if( i == game_stats.user_vehicle )
                    game_stats.times_hit++;

                if( p->owner == game_stats.user_vehicle )
                    {
                     if( player[game_stats.user_vehicle].tank.team == player[i].tank.team )
                         game_stats.friendly_hits++;
                     else
                         game_stats.enemy_hits++;
                    } 


                if( player[i].tank.current_hitpoints <= 0 ) {

                    player[i].tank.current_hitpoints = 0;

                    player[i].tank.alive = FALSE;

                    /* STAT SHIT */
                    if( i == game_stats.user_vehicle )
                        game_stats.times_killed++;

                    add_explosion( ship_explosion, &(player[i].tank.orient), player[i].tank.obj, 0 );

                    if( (temp_int = Find_Bullet_Owner( p )) != -1 )
                         {
                         if( temp_int < 6 ) 
                             {
                              /* STAT SHIT */
                              if( temp_int == game_stats.user_vehicle )
                                  game_stats.kills++;

                              if( world_stuff.player_array[temp_int].tank.team != world_stuff.player_array[i].tank.team )
                                  {
                                   player_events[temp_int].killed_someone = TRUE;    
                                  }
                             }
                         }
                    if( game_configuration.sound_on )
                        {
                         Q_Jon_Sample( VEHICLE_EXPLOSION, player[i].tank.orient.position );
                        }
                }


                return TRUE;
            }
        }
    }
    return FALSE;
}

int radar_base_projectile_collision( RadarBase *base, Projectile *p, Vehicle *v )
{
    long face_index;
    MagicBoundingBox mbox;
    Float_Point point;
    PointFace temp_obj;

    if( base->alive ) {

        mbox.min_x = ftom( base->box.min_x );
        mbox.max_x = ftom( base->box.max_x );
        mbox.min_y = ftom( base->box.min_y );
        mbox.max_y = ftom( base->box.max_y );
        mbox.min_z = ftom( base->box.min_z );
        mbox.max_z = ftom( base->box.max_z );

        if( magic_bounding_box_overlap( &(p->box), &(mbox) ) &&
            pointface_edge_intersect( base->world_union_obj, p->world_obj->point,
                                  &(p->edge_table), &face_index ) ) {


            /* inflict damage */

            if( p->type == LASER ) {

                 /* STAT SHIT */
                 if( p->owner == game_stats.user_vehicle )
                     {
                      if( base->team == world_stuff.player_array[game_stats.user_vehicle].tank.team )
                          game_stats.friendly_hits++;
                      else
                          game_stats.enemy_hits++;
                     }

                 if( game_configuration.sound_on )
                    {
                     /* sb_mix_sample(sample[BULLET_HIT]); */
                     point[X] = mtof( p->orient.position[X] );
                     point[Y] = mtof( p->orient.position[Y] );
                     point[Z] = mtof( p->orient.position[Z] );
                     Q_Jon_Sample( BULLET_HIT, point );
                    }

                 base->current_hitpoints -= v->laser_damage;
            }
            else if( p->type == MISSILE ) {
            
                 /* STAT SHIT */
                 if( p->owner == game_stats.user_vehicle )
                     {
                      if( base->team == world_stuff.player_array[game_stats.user_vehicle].tank.team )
                          game_stats.friendly_hits++;
                      else
                          game_stats.enemy_hits++;
                     }
                
                 base->current_hitpoints -= v->missile_damage;
                 if( game_configuration.sound_on )
                    {
                     /* sb_mix_sample(sample[MISSILE_HIT]); */
                     point[X] = mtof( p->orient.position[X] );
                     point[Y] = mtof( p->orient.position[Y] );
                     point[Z] = mtof( p->orient.position[Z] );
                     Q_Jon_Sample( MISSILE_HIT, point );

                    }
            }
            if( base->current_hitpoints <= 0 ) {

                /* death of radar base */

                base->current_hitpoints = 0;
                base->alive = FALSE;

                temp_obj.point = NULL;
                temp_obj.points = 0;
                temp_obj.face = NULL;
                temp_obj.faces = 0;

                union_object( base->turret_obj, &(temp_obj) );
                union_object( base->base_obj, &(temp_obj) );

                add_explosion( radar_base_explosion, &(base->base_orient), &(temp_obj), 0 );

                if( game_configuration.sound_on ) {
                    Q_Jon_Sample( VEHICLE_EXPLOSION, base->base_orient.position );
                }

                free( temp_obj.point );
                free( temp_obj.face );
            }

            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else {
        return FALSE;
    }
}

void init_bounding_box( Vehicle *v )
{
    Point *ptr;
    long j, temp;

    if( (ptr = v->world_collision_obj->point) == NULL )
        SYS_Error("init_bounding_box(): null ptr in v->world_collision_obj->point\n");

    /* find min_x */

    temp = ptr[0][X];

    for( j = 1; j < v->world_collision_obj->points; j++ ) {
        if( ptr[j][X] < temp ) {
            temp = ptr[j][X];
        }
    }
    v->mbox.min_x = temp;
    v->box.min_x = mtof( temp );

    /* find max_x */

    temp = ptr[0][X];
    for( j = 1; j < v->world_collision_obj->points; j++ ) {
        if( ptr[j][X] > temp ) {
            temp = ptr[j][X];
        }
    }
    v->mbox.max_x = temp;
    v->box.max_x = mtof( temp );

    /* find min_y */

    temp = ptr[0][Y];
    for( j = 1; j < v->world_collision_obj->points; j++ ) {
        if( ptr[j][Y] < temp ) {
            temp = ptr[j][Y];
        }
    }
    v->mbox.min_y = temp;
    v->box.min_y = mtof( temp );

    /* find max_y */

    temp = ptr[0][Y];
    for( j = 1; j < v->world_collision_obj->points; j++ ) {
        if( ptr[j][Y] > temp ) {
            temp = ptr[j][Y];
        }
    }
    v->mbox.max_y = temp;
    v->box.max_y = mtof( temp );

    /* find min_z */

    temp = ptr[0][Z];
    for( j = 1; j < v->world_collision_obj->points; j++ ) {
        if( ptr[j][Z] < temp ) {
            temp = ptr[j][Z];
        }
    }
    v->mbox.min_z = temp;
    v->box.min_z = mtof( temp );

    /* find max_z */

    temp = ptr[0][Z];
    for( j = 1; j < v->world_collision_obj->points; j++ ) {
        if( ptr[j][Z] > temp ) {
            temp = ptr[j][Z];
        }
    }
    v->mbox.max_z = temp;
    v->box.max_z = mtof( temp );
}

void find_bounding_box( PointFace *obj, BoundingBox *box )
{
    Point *ptr;
    long j, temp;

    if( (ptr = obj->point) == NULL ) {
        printf("find_bounding_box(): null ptr in obj->point\n");
        exit(1);
    }

    /* find min_x */

    temp = ptr[0][X];

    for( j = 1; j < obj->points; j++ ) {
        if( ptr[j][X] < temp ) {
            temp = ptr[j][X];
        }
    }
    box->min_x = mtof( temp );

    /* find max_x */

    temp = ptr[0][X];
    for( j = 1; j < obj->points; j++ ) {
        if( ptr[j][X] > temp ) {
            temp = ptr[j][X];
        }
    }
    box->max_x = mtof( temp );

    /* find min_y */

    temp = ptr[0][Y];
    for( j = 1; j < obj->points; j++ ) {
        if( ptr[j][Y] < temp ) {
            temp = ptr[j][Y];
        }
    }
    box->min_y = mtof( temp );

    /* find max_y */

    temp = ptr[0][Y];
    for( j = 1; j < obj->points; j++ ) {
        if( ptr[j][Y] > temp ) {
            temp = ptr[j][Y];
        }
    }
    box->max_y = mtof( temp );

    /* find min_z */

    temp = ptr[0][Z];
    for( j = 1; j < obj->points; j++ ) {
        if( ptr[j][Z] < temp ) {
            temp = ptr[j][Z];
        }
    }
    box->min_z = mtof( temp );

    /* find max_z */

    temp = ptr[0][Z];
    for( j = 1; j < obj->points; j++ ) {
        if( ptr[j][Z] > temp ) {
            temp = ptr[j][Z];
        }
    }
    box->max_z = mtof( temp );
}


void init_projectile_bounding_box( Projectile *p )
{
    Point *ptr;
    long j, temp;

    if( (ptr = p->world_obj->point) == NULL ) {
        printf("init_projectile_bounding_box(): null ptr in p->world_obj->point\n");
        exit(1);
    }

    /* find min_x */

    temp = ptr[0][X];

    for( j = 1; j < p->world_obj->points; j++ ) {
        if( ptr[j][X] < temp ) {
            temp = ptr[j][X];
        }
    }
    p->box.min_x = temp;

    /* find max_x */

    temp = ptr[0][X];
    for( j = 1; j < p->world_obj->points; j++ ) {
        if( ptr[j][X] > temp ) {
            temp = ptr[j][X];
        }
    }
    p->box.max_x = temp;

    /* find min_y */

    temp = ptr[0][Y];
    for( j = 1; j < p->world_obj->points; j++ ) {
        if( ptr[j][Y] < temp ) {
            temp = ptr[j][Y];
        }
    }
    p->box.min_y = temp;

    /* find max_y */

    temp = ptr[0][Y];
    for( j = 1; j < p->world_obj->points; j++ ) {
        if( ptr[j][Y] > temp ) {
            temp = ptr[j][Y];
        }
    }
    p->box.max_y = temp;

    /* find min_z */

    temp = ptr[0][Z];
    for( j = 1; j < p->world_obj->points; j++ ) {
        if( ptr[j][Z] < temp ) {
            temp = ptr[j][Z];
        }
    }
    p->box.min_z = temp;

    /* find max_z */

    temp = ptr[0][Z];
    for( j = 1; j < p->world_obj->points; j++ ) {
        if( ptr[j][Z] > temp ) {
            temp = ptr[j][Z];
        }
    }
    p->box.max_z = temp;
}

int cap_collision( Vehicle *v, Float_Vector reflect )
{

    if( v->box.max_z >= (Cylinder_Length / 2) ) {
        reflect[X] += 0.0;
        reflect[Y] += 0.0;
        reflect[Z] += -1.0;
        if( game_configuration.sound_on )
            {
             /* sb_mix_sample(sample[CAP_COLLISION]); */
             Q_Jon_Sample( CAP_COLLISION, v->orient.position );
            }

        return TRUE;
    }
    else if( v->box.min_z <= -(Cylinder_Length / 2) ) {
        reflect[X] += 0.0;
        reflect[Y] += 0.0;
        reflect[Z] += 1.0;
        if( game_configuration.sound_on )
            {
             /* sb_mix_sample(sample[CAP_COLLISION]); */
             Q_Jon_Sample( CAP_COLLISION, v->orient.position );
            }

        return TRUE;
    }
    else {
        return FALSE;
    }
}

int projectile_cylinder_collision( Projectile *p )
{
    Vector v;
    long len;
    Float_Point point; /* For conversion of position */

    point[X] = mtof( p->orient.position[X] );
    point[Y] = mtof( p->orient.position[Y] );
    point[Z] = mtof( p->orient.position[Z] );


    if( p->on_surface && (p->type == LASER) ) {
        if( p->box.max_z >= (Magic_Cylinder_Length >> 1) ) {
            if( game_configuration.sound_on ) {
                if( p->type == LASER ) {
                    Q_Jon_Sample( BULLET_MISS, point );
		}
                else {
                    Q_Jon_Sample( MISSILE_MISS, point );
		}
	    }
            return TRUE;
        }
        else if( p->box.min_z <= -(Magic_Cylinder_Length >> 1) ) {
            if( game_configuration.sound_on ) {
                if( p->type == LASER ) {
                    Q_Jon_Sample( BULLET_MISS, point );
		}
                else {
                    Q_Jon_Sample( MISSILE_MISS, point );
		}
	    }
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else {
        if( p->box.max_z >= (Magic_Cylinder_Length >> 1) ) {
            if( game_configuration.sound_on ) {
                if( p->type == LASER ) {
                    Q_Jon_Sample( BULLET_MISS, point );
		}
                else {
                    Q_Jon_Sample( MISSILE_MISS, point );
		}
	    }

            return TRUE;
        }
        else if( p->box.min_z <= -(Magic_Cylinder_Length >> 1) ) {
            if( game_configuration.sound_on ) {
                if( p->type == LASER ) {
                    Q_Jon_Sample( BULLET_MISS, point );
		}
                else {
                    Q_Jon_Sample( MISSILE_MISS, point );
		}
	    }
            return TRUE;
        }
        else {
            v[X] = p->orient.position[X];
            v[Y] = p->orient.position[Y];
            v[Z] = 0;

            len = ftom( (float)sqrt( mtof( ((v[X] * v[X]) >> MEXP) +
										   ((v[Y] * v[Y]) >> MEXP) ) ) );

            if( len > 10138 ) { /* 9.9 */
		    if( game_configuration.sound_on ) {
			if( p->type == LASER ) {
			    Q_Jon_Sample( BULLET_MISS, point );
			}
			else {
			    Q_Jon_Sample( MISSILE_MISS, point );
			}
			return TRUE;
		    }
            }
            else {
                return FALSE;
            }
        }
    }

    return FALSE; //I assume this should be false?  was empty
}

int cylinder_cap_collision( Vehicle *v, Float_Vector reflect )
{
    int cap_collide = FALSE;
    Float_Vector vector;
    float len;

    if( v->box.max_z >= (Cylinder_Length / 2) ) {
        reflect[X] += 0.0;
        reflect[Y] += 0.0;
        reflect[Z] += -1.0;
        if( game_configuration.sound_on )
            {
             /* sb_mix_sample(sample[CAP_COLLISION]); */
             Q_Jon_Sample( CAP_COLLISION, v->orient.position );
            }

        cap_collide = TRUE;
    }
    else if( v->box.min_z <= -(Cylinder_Length / 2) ) {

        reflect[X] += 0.0;
        reflect[Y] += 0.0;
        reflect[Z] += 1.0;

        if( game_configuration.sound_on )
            {
             /* sb_mix_sample(sample[CAP_COLLISION]); */
             Q_Jon_Sample( CAP_COLLISION, v->orient.position );
            }

        cap_collide = TRUE;
    }

    vector[X] = v->orient.position[X];
    vector[Y] = v->orient.position[Y];
    vector[Z] = 0.0;

    if( (len = myvector_length( vector )) > 9.75 ) {

        reflect[X] -= vector[X] / len;
        reflect[Y] -= vector[Y] / len;

        if( game_configuration.sound_on )
            {
             /* sb_mix_sample(sample[CAP_COLLISION]); */
             Q_Jon_Sample( CAP_COLLISION, v->orient.position );
            }

           return TRUE;
    }

    return cap_collide;
}

/* vehicle_collision() checks to see if vehicle v is currently
   colliding with any pylons, radar bases, the cylinder or another vehicle.
   It also sends back reflect which is the sum of the normal vectors of all
   the faces that v is colliding with. */

int vehicle_collision( Vehicle *v, Pylons *pylons,
                       Player p[], RadarBase *red_base, RadarBase *blue_base,
                       Float_Vector reflect )
{
    int collision = 0;

    reflect[X] = 0.0;
    reflect[Y] = 0.0;
    reflect[Z] = 0.0;

    if( v->vehicle_mode == Surface ) {

        /* since v is on the surface we don't have to check for a collision
           with the entire cylinder only the caps. */

        collision += super_pylon_collision( pylons, v, reflect );
        collision += super_radar_base_collision( red_base, v, reflect );
        collision += super_player_collision( p, v, reflect );
        collision += super_radar_base_collision( blue_base, v, reflect );
        collision += cap_collision( v, reflect );

        if( collision > 0 ) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else if( v->vehicle_mode == Air ) {

        collision += super_pylon_collision( pylons, v, reflect );
        collision += super_radar_base_collision( red_base, v, reflect );
        collision += super_player_collision( p, v, reflect );
        collision += super_radar_base_collision( blue_base, v, reflect );
        collision += cylinder_cap_collision( v, reflect );

        if( collision > 0 ) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else if( v->vehicle_mode == Landing ) {

        collision += super_pylon_collision( pylons, v, reflect );
        collision += super_radar_base_collision( red_base, v, reflect );
        collision += super_player_collision( p, v, reflect );
        collision += super_radar_base_collision( blue_base, v, reflect );
        collision += cylinder_cap_collision( v, reflect );

        if( collision > 0 ) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else {
        return FALSE;
    }
}

void build_collision_table( CollisionTable c_table,
                            Player p[], Pylons *pylons, RadarBase *red_base,
                            RadarBase *blue_base )
{
    long i;

    for( i = 0; i < 6; i++ ) {
        c_table[i].collision = FALSE;
        c_table[i].reflect[X] = 0.0;
        c_table[i].reflect[Y] = 0.0;
        c_table[i].reflect[Z] = 0.0;
    }

    for( i = 0; i < 6; i++ ) {
        if( p[i].tank.alive ) {
            c_table[i].collision = vehicle_collision( &(p[i].tank), pylons, p,
                red_base, blue_base, c_table[i].reflect );
            if( c_table[i].collision ) {
                if( c_table[i].reflect[X] == 0.0 && c_table[i].reflect[Y] == 0.0 && c_table[i].reflect[Z] == 0.0 ) {
                    c_table[i].reflect[X] = -p[i].tank.orient.position[X];
                    c_table[i].reflect[Y] = -p[i].tank.orient.position[Y];
                    c_table[i].reflect[Z] = -p[i].tank.orient.position[Z];
                }
                normalize( c_table[i].reflect );
            }
        }
    }
}

void build_new_collision_table( CollisionTable c_table,
                                CollisionTable new_table, Player p[],
                                Pylons *pylons, RadarBase *red_base,
                                RadarBase *blue_base )
{
    long i;

    for( i = 0; i < 6; i++ ) {
        new_table[i].collision = FALSE;
        new_table[i].reflect[X] = 0.0;
        new_table[i].reflect[Y] = 0.0;
        new_table[i].reflect[Z] = 0.0;
    }

    for( i = 0; i < 6; i++ ) {
        if( c_table[i].collision ) {
            new_table[i].collision = vehicle_collision( &(p[i].tank), pylons,
            p, red_base, blue_base, new_table[i].reflect );
            if( new_table[i].collision ) {
                if( new_table[i].reflect[X] == 0.0 && new_table[i].reflect[Y] == 0.0 && new_table[i].reflect[Z] == 0.0 ) {
                    new_table[i].reflect[X] = -p[i].tank.orient.position[X];
                    new_table[i].reflect[Y] = -p[i].tank.orient.position[Y];
                    new_table[i].reflect[Z] = -p[i].tank.orient.position[Z];
                }
                normalize( new_table[i].reflect );
            }
        }
    }
}

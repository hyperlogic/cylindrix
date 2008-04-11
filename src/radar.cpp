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

#include "radar.h"
#include "object.h"
#include "prim.h"
#include "clipping.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Matrix rotation_matrix;
extern Matrix projection_matrix;      /* used to do perspective transform */
extern Matrix scale_matrix;           /* scale into cannonical view volume */
extern Matrix inv_scale_matrix;       /* inverse of scale_matrix */
extern Matrix wtov_matrix;            /* window to viewport mapping matrix */

extern long view_vertices[MAX_POINTS][3];   /* holds the vertices of obj after
                                        rotation */

extern long projected_vertices[MAX_POINTS][4]; /* vertices of obj after rotation &
                                           projection & scaling */

extern Edge global_edge_array[3000];

extern unsigned char clipped_away[MAX_POINTS];  /* tells if a point has been clipped
                                            away or not */

extern long new_hither;

/* initalizes the edgetable that is used by view_radar */

void init_radar( PointEdge *pe )
{
    get_wire_object( pe, "3d_data/tinytube.ped" );
}

/* it assumes that pe the cylider of radius 1.5 and length of 4 centered
   at the origin */

void view_radar( PointEdge *pe, Player *player_array, long view_vehicle, Point2d offset )
{
    long i;
    Point p;
    Matrix translation_matrix, temp_matrix;
    PointEdge clip_pe;
    unsigned char shade;
    
    /* these colors are in every level.pcx */
    
    const unsigned char blue = 3;
    const unsigned char yellow = 4;
    const unsigned char red = 1;

    /* copy the points from tube into view_vertices */

    memcpy( view_vertices, pe->point, sizeof( Point ) * pe->points );

    /* figure out matrix needed to translate, rotate and scale into
       cannonical view volume. Stick it in result_matrix */

    translation_matrix_init( translation_matrix, 0, 0, (6 << MEXP) );

    rot_matrix_init( rotation_matrix, player_array[view_vehicle].tank.orient.front,
        player_array[view_vehicle].tank.orient.up );

    matrix_mult( translation_matrix, rotation_matrix, temp_matrix );

    /* transform view_vertices into cannonical view volume (for clipping) */

    for( i = 0; i < pe->points; i++ ) {
        linear_transformation1( temp_matrix, view_vertices[i] );
    }

    /* initialize clip_pe for clipping */

    clip_pe.point = view_vertices;
    clip_pe.points = pe->points;
    clip_pe.edge = global_edge_array;
    memcpy( clip_pe.edge, pe->edge, sizeof( Edge ) * pe->edges );
    clip_pe.edges = pe->edges;

    /* transform everything */

    for( i = 0; i < clip_pe.points; i++ ) {

        /* copy from view_vertices into projectied_vertices */

        memcpy( projected_vertices[i], view_vertices[i], sizeof( Point ) );

        /* transform */

        linear_transformation( projection_matrix, projected_vertices[i] );
        wtov_transformation( wtov_matrix, projected_vertices[i] );
        projected_vertices[i][X] = ((1229 * (projected_vertices[i][X] - 163840)) >> MEXP) + 163840 + (offset[X] << MEXP);
        projected_vertices[i][Y] = ((1229 * (projected_vertices[i][Y] - 102400)) >> MEXP) + 102400 + (offset[Y] << MEXP);
    }

    for( i = 0; i < clip_pe.edges; i++ ) {
        if( clip_pe.edge[i][0] != NULL_INDEX ) {
            shade = radar_tube_shade( (view_vertices[clip_pe.edge[i][0]][Z]), WireFrameRadarGrad );
            DB_draw_edge( projected_vertices, clip_pe.edge[i], shade );
        }
    }

    /* now draw the positions */

    for( i = 0; i < 6; i++ ) {

        if( player_array[i].tank.alive ) {

            p[X] = ftom( 0.15f * player_array[i].tank.orient.position[X] );
            p[Y] = ftom( 0.15f * player_array[i].tank.orient.position[Y] );
            p[Z] = ftom( 0.066667f * player_array[i].tank.orient.position[Z] );

            linear_transformation1( temp_matrix, p );

            if( player_array[i].team == RED_TEAM ) {
                shade = red; /* red */
            }
            else {
                shade = blue; /* blue */
            }

            if( view_vehicle == i ) {
                shade = yellow; /* yellow */
            }

            linear_transformation( projection_matrix, p );
            wtov_transformation( wtov_matrix, p );

            p[X] = (((1229 * (p[X] - 163840)) >> MEXP) + 163840 + (offset[X] << MEXP)) >> MEXP;
            p[Y] = (((1229 * (p[Y] - 102400)) >> MEXP) + 102400 + (offset[Y] << MEXP)) >> MEXP;

            DB_Fast_Pixel( p[X], p[Y], shade );
            DB_Fast_Pixel( p[X] + 1, p[Y], shade );
            DB_Fast_Pixel( p[X] - 1, p[Y], shade );
            DB_Fast_Pixel( p[X], p[Y] + 1, shade );
            DB_Fast_Pixel( p[X], p[Y] - 1, shade );
            DB_Fast_Pixel( p[X] + 1, p[Y] + 1, shade );
            DB_Fast_Pixel( p[X] + 1, p[Y] - 1, shade );
            DB_Fast_Pixel( p[X] - 1, p[Y] + 1, shade );
            DB_Fast_Pixel( p[X] - 1, p[Y] - 1, shade );
        }
    }
    
    /* draws a yellow box around the currently targeted enemy */
    
    /*
    if( player_array[view_vehicle].tank.target != NULL ) {

        p[X] = ftom( 0.15 * player_array[view_vehicle].tank.target[X] );
        p[Y] = ftom( 0.15 * player_array[view_vehicle].tank.target[Y] );
        p[Z] = ftom( 0.066667 * player_array[view_vehicle].tank.target[Z] );

        linear_transformation1( temp_matrix, p );
        linear_transformation( projection_matrix, p );
        wtov_transformation( wtov_matrix, p );

        p[X] = (((1229 * (p[X] - 163840)) >> MEXP) + 163840 + (offset[X] << MEXP)) >> MEXP;
        p[Y] = (((1229 * (p[Y] - 102400)) >> MEXP) + 102400 + (offset[Y] << MEXP)) >> MEXP;

        shade = yellow;

        DB_Fast_Pixel( p[X] + 1, p[Y], shade );
        DB_Fast_Pixel( p[X] - 1, p[Y], shade );
        DB_Fast_Pixel( p[X], p[Y] + 1, shade );
        DB_Fast_Pixel( p[X], p[Y] - 1, shade );
        DB_Fast_Pixel( p[X] + 1, p[Y] + 1, shade );
        DB_Fast_Pixel( p[X] + 1, p[Y] - 1, shade );
        DB_Fast_Pixel( p[X] - 1, p[Y] + 1, shade );
        DB_Fast_Pixel( p[X] - 1, p[Y] - 1, shade );
    }
    */
}

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

#ifndef OBJECT_H
#define OBJECT_H

void orient_object( PointFace *obj, Orientation *o, PointFace *result );

void magic_orient_object( PointFace *obj, MagicOrientation *o, PointFace *result );

void print_edge_table( EdgeTable *et );

void init_projectile_edge_table( Projectile *p );

void init_edge_table( Vehicle *v );

void init_world_obj( Vehicle *v );

void projection_matrix_init();

void wtov_matrix_init();

void print_object( PointFace *obj );

void free_object( PointFace *obj );

void get_object( PointFace *obj, char *filename );

void wtov_transformation( long *wtov_matrix, long *vertex );

void projection_transformation( long *per_matrix, long *vertex );

void scale_transformation( long *mat, long *vertex );

void rotation_transformation( long *rot_matrix, long *vertex );

void translation_transformation( long tx, long ty, long tz, long *vertex );

long find_max_z( Face *f, Point vert[] );

int cmp( const void* p, const void* q );

void view_object( PointFace *obj, Orientation *view_orientation, Point light );

void super_view_object( PointFace *obj, Orientation *view_orientation, Point light,
                        long offsetx, long offsety, long scale );

void view_tube( PointFace *tube, Orientation *view_orientation,
                Point light );

void matrix_mult( Matrix l, Matrix r, Matrix result );

void rot_matrix_init( long *rotation_matrix, Float_Point front,
                      Float_Point up );

void trans_rot_matrix_init( long *rotation_matrix, Float_Point front,
                      Float_Point up );

void magic_trans_rot_matrix_init( long *rotation_matrix, Point front,
                                  Point up );

void scale_matrix_init();

void transform_and_union( Orientation *obj_orient, PointFace *obj,
                          PointFace *result );

void union_object( PointFace *obj, PointFace *result );

void view_wire_tube( PointEdge *tube, Orientation *view_orientation,
                     Point light );

void view_point_tube( Point tube_points[], long num_points,
                      Orientation *view_orientation, Point light );

void create_edge_table( PointFace *obj, EdgeTable *et );

void print_wire_object( PointEdge *pe );

void get_wire_object( PointEdge *pe, char *filename );

void translation_matrix_init( Matrix t, long tx, long ty, long tz );

void linear_transformation( long *m, long *v );

void linear_transformation1( long *m, long *v );

void object_copy( PointFace *result, PointFace *obj );

void init_wire_tube( PointFace *tube, PointEdge *wire_tube );

void init_world_collision_obj( Vehicle *v );

#endif


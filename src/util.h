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

#ifndef UTIL_H
#define UTIL_H

void init_sine_table();
void init_arc_cos_table();
long isin( long x );
long icos( long x );
long iarccos( long x );

unsigned char diffuse_shade( long dotprod, Gradient gradient );
unsigned char distance_diffuse_shade( long dotprod, long distance, Gradient gradient );
unsigned char vehicle_menu_distance_diffuse_shade( long dotprod, long distance, Gradient gradient );
unsigned char tube_shade( long distance, Gradient gradient );
unsigned char radar_tube_shade( long distance, Gradient gradient );


float mtof( long magic );
long ftom( float num );
long rounding_ftom( float num );
long rounding_fixed_multiply( long x, long y );
long rounding_fixed_to_long( long fixed );

/* floating point linear algabra functions */

float myvector_length( Float_Vector v );
float vector_dot_product( Float_Vector u, Float_Vector v );
void vector_cross_product( Float_Vector u, Float_Vector v, Float_Vector result );
void identity_matrix_init( Float_Matrix m );
void normalize( Float_Vector v );
void vector_project( Float_Vector u, Float_Vector v, Float_Vector result );
void matrix_multiply( Float_Matrix p, Float_Matrix q, Float_Matrix r );
void rotate_around_axis( float theta, Float_Vector axis, Float_Matrix m );
void transformation( Float_Matrix m, Float_Vector v );
void matrix_print( Float_Matrix m );
float distance_between( Float_Point u, Float_Point v );
float angle_between( Float_Vector u, Float_Vector v );

int point_visible( Orientation *o, Float_Point pos );

void print_vehicle_info( Vehicle *v );
void print_player_array_info( Player p[] );
void print_player_info( Player *p, long frame );

#ifdef PROFILE
unsigned long PROFILE_CheckTimer();
#endif

#endif


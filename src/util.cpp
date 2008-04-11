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

#include "util.h"
#include "keys.h"
#include "prim.h"

#include "object.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "clipping.h"  /* needed in point_visible */
#include "util.h"

extern long new_hither;     /* needed in point_visible */
extern Matrix scale_matrix; /* needed in point_visible */
extern game_configuration_type game_configuration; /* From omega.c */
extern level_type level; /* From omega.c...we need it for yon_clipping_plane */

extern WorldStuff world_stuff;


long sine_table[1609]; /* sin( 0.0 ) to sin( PI/2 ) */
long arc_cos_table[8193]; /* arccos( 0.0 ) to arccos( 1.0 ) in fixed-point
			     19.13 */
			     
extern long sb_installed;        /* True if the sound card driver is installed */
extern long keyboard_installed;  /* True if the keyboard interrupt is installed */
extern long timer_installed;     /* True if the timer interrrupt is installed */

void init_sine_table()
{
    long i;

    for( i = 0; i < 1609; i++ ) {
	sine_table[i] = ftom( (float)sin( mtof( i ) ) );
    }
}

void init_arc_cos_table()
{
     long i;

     for( i = 0; i < 8193; i++ ) {
	 arc_cos_table[i] = (long)(acos( ((float)i / 8192.0) ) * 8192.0);
     }
}

/* converts 22.10 fixed-point to float */

float mtof( long magic )
{
    return (float)((float)magic/(float)MAGIC);
}

/* converts float to 22.10 fixed-point */

long ftom( float num )
{
    return (long)(num * MAGIC);
}

long rounding_ftom( float num )
{
    double integer_part, fractional_part;

    fractional_part = modf( num * (float)MAGIC, &integer_part );

    if( ( fractional_part > 0.0 ) && ( fractional_part >= 0.5 ) ) {
	return (long)(integer_part + 1.0);
    }
    else if( ( fractional_part < 0.0 ) && ( -fractional_part >= 0.5 ) ) {
	return (long)(integer_part - 1.0);
    }
    else {
	return (long)(integer_part);
    }
}

long rounding_fixed_multiply( long x, long y )
{
    long temp;

    temp = (x * y) >> (MEXP - 1);

    if( (temp & 0x00000001) == 0 ) {
	return (temp >> 1);
    }
    else {
	return ((temp >> 1) + 1);
    }
}

long rounding_fixed_to_long( long fixed )
{
    fixed = fixed >> (MEXP - 1);

    if( (fixed & 0x00000001) == 0 ) {
	return (fixed >> 1);
    }
    else {
	return ((fixed >> 1) + 1);
    }
}

/* warning x is expected to be in 19.13 fixed-point not 22.10 */
/* the result is also in 19.13 fixed-point */

long iarccos( long x )
{
    if( x > 8192 ) {
	x = 8192;
    }
    else if( x < -8192 ) {
	x = -8192;
    }

    if( x < 0 ) {
	return -arc_cos_table[-x] + 25736; /* pi */
    }
    else {
	return arc_cos_table[x];
    }
}

/* gives you the sin of the angle x(radians) in fixed-point */

long isin( long x )
{
    long factor = 1;

    while( x < 0 ) {
	x += 6434;     /* 2 pi */
    }
    x %= 6434;         /* 2 pi */
    if( x > 3217 ) {   /* pi */
	x -= 3217;     /* pi */
	factor = -1;
    }
    if( x > 1608 )     /* pi / 2 */
	x = 3217 - x;  /* pi */

    return (sine_table[x] * factor);
}


/* gives you the cos of the angle x(radians) in fixed-point */

long icos( long x )
{
    return (isin(x+1608));
}

/* Finds the actual color of a pixel.  The variable dotprod is the dot product
   of this pixels normal vector and the light vector.  The variable gradient
   holds the index of the palette gradient that will be used to color this
   pixel.  This function finds the color based solely on the angle of the
   light source. */

unsigned char diffuse_shade( long dotprod, Gradient gradient )
{
    unsigned char num_colors = world_stuff.color_info.gradient[gradient].num_colors;
    unsigned char offset = world_stuff.color_info.gradient[gradient].first;

    if( num_colors == 1 ) {
	return offset;
    }
    else {

	long d_theta; /* number of radians each color in the gradient sweeps
			 through */

	long theta;   /* angle of face to light source (arccos(dotprod)) */
	long i;

	theta = iarccos( dotprod << 3 );

	d_theta = (1608 << 3) / num_colors;

	for( i = 1; i <= num_colors; i++ ) {

	    if( (i * d_theta) > theta ) {
		break;
	    }
	}
	return num_colors - (i - 1) + offset;
    }
}

/* Finds the actual color of a pixel.  The variable dotprod is the dot product
   of this pixels normal vector and the light vector.  The variable gradient
   holds the index of the palette gradient that will be used to color this
   pixel.  The variable distance is the distance of this pixel from the light
   source.  This function finds the color based the angle of the light souce
   and the distance from the light source. */

unsigned char distance_diffuse_shade( long dotprod, long distance, Gradient gradient )
{
    unsigned char num_colors = world_stuff.color_info.gradient[gradient].num_colors;
    unsigned char offset = world_stuff.color_info.gradient[gradient].first;
    long d_theta; /* number of radians each color in the gradient sweeps
		     through */
    long theta;   /* angle of face to light source (arccos(dotprod)) */
    long i;
    long max_distance = ((((25 << MEXP) << MEXP) / -5120) * level.yon_clipping_plane) >> MEXP;
    long factor = ((num_colors << MEXP) << MEXP) / max_distance; /* (num_colors / max_distance) */
    long shade;

    dotprod = (dotprod >> 1) + 512 + 50; /* shading hack to make things brighter */

    if( dotprod > 1024 ) {
	dotprod = 1024;
    }

    if( num_colors == 1 ) {
	return offset;
    }
    else {

	theta = iarccos( dotprod << 3 );

	assert( num_colors != 0 );

	d_theta = (1608 << 3) / num_colors;

	for( i = 1; i <= num_colors; i++ ) {

	    if( (i * d_theta) > theta ) {
		break;
	    }
	}
	shade = (offset + num_colors - (i - 1)) -
		((((distance + (3 << MEXP)) * factor) >> MEXP) >> MEXP);

	if( shade < offset ) {
	    shade = offset;
	}
	else if( shade > offset + (num_colors - 1) ) {
	    shade = offset + (num_colors - 1);
	}
	return (unsigned char)shade;
    }
}


unsigned char vehicle_menu_distance_diffuse_shade( long dotprod, long distance, Gradient gradient )
{
    unsigned char num_colors = 64;
    unsigned char offset = 5;
    long d_theta; /* number of radians each color in the gradient sweeps
		     through */
    long theta;   /* angle of face to light source (arccos(dotprod)) */
    long i;
    long max_distance = ((((25 << MEXP) << MEXP) / -5120) * level.yon_clipping_plane) >> MEXP;
    long factor = ((num_colors << MEXP) << MEXP) / max_distance; /* (num_colors / max_distance) */
    long shade;

    dotprod = (dotprod >> 1) + 512 + 50; /* shading hack to make things brighter */

    if( dotprod > 1024 ) {
	dotprod = 1024;
    }

    if( num_colors == 1 ) {
	return offset;
    }
    else {

	theta = iarccos( dotprod << 3 );

	assert( num_colors != 0 );

	d_theta = (1608 << 3) / num_colors;

	for( i = 1; i <= num_colors; i++ ) {

	    if( (i * d_theta) > theta ) {
		break;
	    }
	}
	shade = (offset + num_colors - (i - 1)) -
		((((distance + (3 << MEXP)) * factor) >> MEXP) >> MEXP);

	if( shade < offset ) {
	    shade = offset;
	}
	else if( shade > offset + (num_colors - 1) ) {
	    shade = offset + (num_colors - 1);
	}
	return (unsigned char)shade;
    }
}


/* Finds the actual color of a pixel that is some distance away from the
   light soruce. */

unsigned char tube_shade( long distance, Gradient gradient )
{
    unsigned char num_colors = world_stuff.color_info.gradient[gradient].num_colors;
    unsigned char offset = world_stuff.color_info.gradient[gradient].first;
    long shade;
    long factor = ((num_colors << MEXP) << MEXP) / level.yon_clipping_plane; /* (num_colors / max_distance) */

    if( num_colors == 1 ) {
	return offset;
    }
    else {
	shade = (offset + (num_colors - 1)) - (((distance * factor) >> MEXP) >> MEXP);

	if( shade < offset ) {
	    shade = offset;
	}
	else if( shade > offset + (num_colors - 1) ) {
	    shade = offset + (num_colors - 1);
	}
	
	return (unsigned char)shade;
    }
}

unsigned char radar_tube_shade( long distance, Gradient gradient )
{
    long num_colors = world_stuff.color_info.gradient[gradient].num_colors;
    long offset = world_stuff.color_info.gradient[gradient].first;
    long shade;
    long factor = (num_colors << MEXP) / 4;

    distance -= (3 << MEXP);

    if( num_colors == 1 ) {
	return (unsigned char)offset;
    }
    else {

	shade = (offset + (num_colors - 1)) - (((distance * factor) >> MEXP) >> MEXP);

	if( shade < offset ) {
	    shade = offset;
	}
	else if( shade > offset + (num_colors - 1) ) {
	    shade = offset + (num_colors - 1);
	}

	return (unsigned char)shade;
    }
}

//Renamed to avoid allegro conflict
float myvector_length( Float_Vector v )
{
    float dot_prod;

    dot_prod = (v[X] * v[X]) + (v[Y] * v[Y]) + (v[Z] * v[Z]);
    return (float)sqrt( dot_prod );
}

float vector_dot_product( Float_Vector u, Float_Vector v )
{
    return (u[X] * v[X]) + (u[Y] * v[Y]) + (u[Z] * v[Z]);
}

void vector_cross_product( Float_Vector u, Float_Vector v, Float_Vector result )
{
    result[X] = (u[Y] * v[Z]) - (u[Z] * v[Y]);
    result[Y] = (u[Z] * v[X]) - (u[X] * v[Z]);
    result[Z] = (u[X] * v[Y]) - (u[Y] * v[X]);
}

void identity_matrix_init( Float_Matrix m )
{
    m[0] = 1.0;
    m[1] = 0.0;
    m[2] = 0.0;
    m[3] = 0.0;

    m[4] = 0.0;
    m[5] = 1.0;
    m[6] = 0.0;
    m[7] = 0.0;

    m[8] = 0.0;
    m[9] = 0.0;
    m[10] = 1.0;
    m[11] = 0.0;

    m[12] = 0.0;
    m[13] = 0.0;
    m[14] = 0.0;
    m[15] = 1.0;
}

void normalize( Float_Vector v )
{
    float len;

    len = (float)sqrt( vector_dot_product( v, v ) );

    v[X] = v[X] / len;
    v[Y] = v[Y] / len;
    v[Z] = v[Z] / len;
}

/* result becomes the projection of u onto v */

void vector_project( Float_Vector u, Float_Vector v, Float_Vector result )
{
    float u_dot_v;
    float v_length;
    float scaler;

    u_dot_v = vector_dot_product( u, v );
    v_length = myvector_length( v );

    assert( v_length != 0.0f );

    scaler = u_dot_v / (v_length * v_length);

    result[X] = scaler * v[X];
    result[Y] = scaler * v[Y];
    result[Z] = scaler * v[Z];
}

void matrix_multiply( Float_Matrix p, Float_Matrix q, Float_Matrix r )
{
    /* multiply p and q into r */

    /* first row */

    r[0] = p[0] * q[0] + p[1] * q[4] +
	   p[2] * q[8] + p[3] * q[12];

    r[1] = p[0] * q[1] + p[1] * q[5] +
	   p[2] * q[9] + p[3] * q[13];

    r[2] = p[0] * q[2]  +  p[1]  *  q[6]  +
	   p[2] * q[10]  +  p[3]  *  q[14] ;

    r[3] = p[0] * q[3]  +  p[1]  *  q[7]  +
	   p[2] * q[11]  +  p[3]  *  q[15] ;

    /* second row */

    r[4] = p[4] * q[0]  +  p[5]  *  q[4]  +
	   p[6] * q[8]  +  p[7]  *  q[12] ;

    r[5] = p[4] * q[1]  +  p[5]  *  q[5]  +
	   p[6] * q[9]  +  p[7]  *  q[13] ;

    r[6] = p[4] * q[2]  +  p[5]  *  q[6]  +
	   p[6] * q[10]  +  p[7]  *  q[14] ;

    r[7] = p[4] * q[3]  +  p[5]  *  q[7]  +
	   p[6] * q[11]  +  p[7]  *  q[15] ;

    /* third row */

    r[8] = p[8] * q[0]  +  p[9]  *  q[4]  +
	   p[10] * q[8]  +  p[11]  *  q[12] ;

    r[9] = p[8] * q[1]  +  p[9]  *  q[5]  +
	   p[10] * q[9]  +  p[11]  *  q[13] ;

    r[10] = p[8] * q[2]  +  p[9]  *  q[6]  +
	    p[10] * q[10]  +  p[11]  *  q[14] ;

    r[11] = p[8] * q[3]  +  p[9]  *  q[7]  +
	    p[10] * q[11]  +  p[11]  *  q[15] ;

    /* forth row */

    r[12] = p[12] * q[0]  +  p[13]  *  q[4]  +
	    p[14] * q[8]  +  p[15]  *  q[12] ;

    r[13] = p[12] * q[1]  +  p[13]  *  q[5]  +
	    p[14] * q[9]  +  p[15]  *  q[13] ;

    r[14] = p[12] * q[2]  +  p[13]  *  q[6]  +
	    p[14] * q[10]  +  p[15]  *  q[14] ;

    r[15] = p[12] * q[3]  +  p[13]  *  q[7]  +
	    p[14] * q[11]  +  p[15]  *  q[15] ;
}

void rotate_around_axis( float theta, Float_Vector axis, Float_Matrix m )
{
    float v, w, one_over_v, one_over_w;
    Float_Matrix g, h, g_inverse, h_inverse, r, temp;
    const float epsilon = 0.005f;

    v = (float)sqrt( (axis[X] * axis[X]) + (axis[Y] * axis[Y]) );

    if( (v < epsilon) && (v > -epsilon) ) { /* if v close to 0.0 */
       identity_matrix_init( g );
       identity_matrix_init( g_inverse );
    }
    else {
	one_over_v = 1.0f / v;

	identity_matrix_init( g );

	g[0] = one_over_v * axis[X];
	g[1] = one_over_v * axis[Y];
	g[4] = one_over_v * -axis[Y];
	g[5] = one_over_v * axis[X];

	identity_matrix_init( g_inverse );

	g_inverse[0] = one_over_v * axis[X];
	g_inverse[1] = one_over_v * -axis[Y];
	g_inverse[4] = one_over_v * axis[Y];
	g_inverse[5] = one_over_v * axis[X];
    }

    w = (float)sqrt( (v * v) + (axis[Z] * axis[Z]) );
    assert( w != 0.0f );
    one_over_w = 1.0f / w;

    identity_matrix_init( h );

    h[0] = one_over_w * axis[Z];
    h[2] = one_over_w * -v;
    h[8] = one_over_w * v;
    h[10] = one_over_w * axis[Z];
    h[15] = one_over_w;

    identity_matrix_init( h_inverse );

    h_inverse[0] = one_over_w * axis[Z];
    h_inverse[2] = one_over_w * v;
    h_inverse[8] = one_over_w * -v;
    h_inverse[10] = one_over_w * axis[Z];
    h_inverse[15] = one_over_w;

    identity_matrix_init( r );

    r[0] = (float)cos( theta );
    r[1] = (float)sin( theta );
    r[4] = -(float)sin( theta );
    r[5] = (float)cos( theta );

    matrix_multiply( h, g, temp );
    matrix_multiply( r, temp, m );
    matrix_multiply( h_inverse, m, temp );
    matrix_multiply( g_inverse, temp, m );
}

void transformation( Float_Matrix m, Float_Vector v )
{
    Float_Point temp;

    temp[X] = (m[0] * v[X]) + (m[1] * v[Y]) + (m[2] * v[Z]) + m[3];
    temp[Y] = (m[4] * v[X]) + (m[5] * v[Y]) + (m[6] * v[Z]) + m[7];
    temp[Z] = (m[8] * v[X]) + (m[9] * v[Y]) + (m[10] * v[Z]) + m[11];

    v[X] = temp[X];
    v[Y] = temp[Y];
    v[Z] = temp[Z];
}

void matrix_print( Float_Matrix m )
{
    printf("| %.2f %.2f %.2f %.2f |\n",m[0],m[1],m[2],m[3]);
    printf("| %.2f %.2f %.2f %.2f |\n",m[4],m[5],m[6],m[7]);
    printf("| %.2f %.2f %.2f %.2f |\n",m[8],m[9],m[10],m[11]);
    printf("| %.2f %.2f %.2f %.2f |\n",m[12],m[13],m[14],m[15]);
}

/* returns the distance between point u and v */

float distance_between( Float_Point u, Float_Point v )
{
    Float_Point utov;

    utov[X] = v[X] - u[X];
    utov[Y] = v[Y] - u[Y];
    utov[Z] = v[Z] - u[Z];

    return (float)sqrt( vector_dot_product( utov, utov ) );
}

/* tells if point pos is visible from orientation o */

int point_visible( Orientation *o, Float_Point pos )
{
    Point p, point;
    Matrix translation_matrix, rotation_matrix, temp_matrix;

    /* transform the vertex into the view volume */

    p[X] = ftom( o->position[X] );
    p[Y] = ftom( o->position[Y] );
    p[Z] = ftom( o->position[Z] );

    point[X] = ftom( pos[X] );
    point[Y] = ftom( pos[Y] );
    point[Z] = ftom( pos[Z] );

    translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

    rot_matrix_init( rotation_matrix, o->front, o->up );

    matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

    /* transform the vertex into cannonical view volume */

    linear_transformation1( temp_matrix, point );
    scale_transformation( scale_matrix, point );

    /* check to see if it is in the view_volume */

    if( region( point, new_hither ) == 0 ) {
	return TRUE;
    }
    else {
	return FALSE;
    }
}

float angle_between( Float_Vector u, Float_Vector v )
{
    Float_Vector new_u, new_v;
    float temp;

    new_u[X] = u[X];
    new_u[Y] = u[Y];
    new_u[Z] = u[Z];

    new_v[X] = v[X];
    new_v[Y] = v[Y];
    new_v[Z] = v[Z];

    normalize( new_u );
    normalize( new_v );

    temp = vector_dot_product( new_u, new_v );

    if( temp > 1.0 ) {
	temp = 1.0;
    }
    else if( temp < -1.0 ) {
	temp = -1.0;
    }

    return (float)acos( temp );
}

void print_vehicle_info( Vehicle *v )
{
    printf("  position = (%.25f,%.25f,%.25f)\n", v->orient.position[X], v->orient.position[Y], v->orient.position[Z] );
    printf("  front    = (%.25f,%.25f,%.25f)\n", v->orient.front[X], v->orient.front[Y], v->orient.front[Z] );
    printf("  up       = (%.25f,%.25f,%.25f)\n", v->orient.up[X], v->orient.up[Y], v->orient.up[Z] );
    printf("  vel      = (%.25f,%.25f,%.25f)\n", v->vel[X], v->vel[Y], v->vel[Z] );
}

void print_player_info( Player *p, long frame )
{
    printf("%ld  position = (%.25f,%.25f,%.25f)\n", frame, p->tank.orient.position[X], p->tank.orient.position[Y], p->tank.orient.position[Z] );
    printf("%ld  front    = (%.25f,%.25f,%.25f)\n", frame, p->tank.orient.front[X], p->tank.orient.front[Y], p->tank.orient.front[Z] );
    printf("%ld  up       = (%.25f,%.25f,%.25f)\n", frame, p->tank.orient.up[X], p->tank.orient.up[Y], p->tank.orient.up[Z] );
    printf("%ld  vel      = (%.25f,%.25f,%.25f)\n", frame, p->tank.vel[X], p->tank.vel[Y], p->tank.vel[Z] );
    printf("%ld  input_table = (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)\n", frame, (int)p->table[0], (int)p->table[1],
        (int)p->table[2], (int)p->table[3], (int)p->table[4], (int)p->table[5], (int)p->table[6], (int)p->table[7],
        (int)p->table[8], (int)p->table[9], (int)p->table[10], (int)p->table[11], (int)p->table[12] );
}

void print_player_array_info( Player p[] )
{
    Vehicle *v;
    int i;

    for( i = 0; i < 6; i++ ) {

        v = &(p[i].tank);

        printf("%d  position = (%.25f,%.25f,%.25f)\n", i, v->orient.position[X], v->orient.position[Y], v->orient.position[Z] );
        printf("%d  front    = (%.25f,%.25f,%.25f)\n", i, v->orient.front[X], v->orient.front[Y], v->orient.front[Z] );
        printf("%d  up       = (%.25f,%.25f,%.25f)\n", i, v->orient.up[X], v->orient.up[Y], v->orient.up[Z] );
        printf("%d  vel      = (%.25f,%.25f,%.25f)\n", i, v->vel[X], v->vel[Y], v->vel[Z] );
    }
}

#ifdef PROFILE
unsigned long PROFILE_CheckTimer()
{
	unsigned long temp;
	unsigned long diff = (temp = SYS_GetTimeMicro()) - ptime;
	ptime = temp;
	return diff;
}
#endif
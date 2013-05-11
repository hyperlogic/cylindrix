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

/* file => object.c
   programmers => Anthony Thibault & John McCawley III

       This file contains functions used for object manipulation,
   (union_object, orient_object), object creation (load_object),
   linear algebra (matrix_mult, linear_transform) and efficiently
   drawing objects into the double buffer (draw_tube, draw_object).

   Last Commented: August 8, 1995
*/


#include "object.h"    /* function prototypes */

#include "types.h"     /* Basic data structures and defines for vector
			  graphics stuff */

#include "util.h"      /* contains useful functions like isin() and mtof() */

#include "prim.h"      /* Pixel plotters, set video mode, double buffering...
			  Primitives! */

#include "pcx.h"       /* Allocate memory for pcx file, load from file,
			  Enable color palette */

#include "clipping.h"  /* polygon clipper */

#include <math.h>      /* Need acos(), atan(), sin(), cos(), and sqrt() */

#include <stdio.h>     /* Standard Input and Output */

#include <stdlib.h>    /* need qsort */

#include <string.h>    /* need memset */

#include <assert.h>

extern const char* g_DataPath;

long vrpz = (-3 << MEXP);           /* z coord of the view reference point */
long xmin = (-4383);   /* -3.0 */   /* min x coord of window on view plane */
long xmax = (4383);    /* 3.0 */    /* max x coord of window on view plane */
long ymin = (-3159);   /* -2.5 */   /* min y coord of window on view plane */
long ymax = (3159);    /* 2.5 */    /* max y coord of window on view plane */
long yon = ( -1024 );  /*was 1024*/ /* back clipping plane (see HackAlert) */
long new_hither = ( -50 );          /* front clipping plane */

/* Hack Alert !!!!!!
   The variable yon is NOT the actual back clipping plane.  It is used so
   that the scale_matrix will not scale everything into the cannonical
   view volume, because the view volume is too small and will result in
   precision errors.  The actual value of yon is -11.0 and is stored in
   clipping.c */

Matrix rotation_matrix;        /* used to align objects into view coords */
Matrix trans_rotation_matrix;  /* used to align objects into world coords */
Matrix projection_matrix;      /* used to do perspective transform */
Matrix scale_matrix;           /* scale into cannonical view volume */
Matrix inv_scale_matrix;       /* inverse of scale_matrix */
Matrix wtov_matrix;            /* window to viewport mapping matrix */

long view_vertices[MAX_POINTS][3];   /* holds the vertices of obj after
					rotation */

long projected_vertices[MAX_POINTS][4]; /* vertices of obj after rotation &
					   projection & scaling */

/* Hack Alert !!!!!!!!!!!
    The projected_vertices array should be an array of Points (3 longs) not
    4 longs.  Each element of projected_vertices contains 4 longs because the
    forth long can contain the color of that vertex.  This is so the polygon
    scan converter can interpolate the colors of each vertex to produce
    smooth shading.
*/

Face f[MAX_FACES];  /* All faces to be sorted (hidden surface removal) */

Edge global_edge_array[3000];

unsigned char clipped_away[MAX_POINTS];  /* tells if a point has been clipped
					    away or not */

Face clipped_faces[MAX_FACES];  /* hold faces for clipping */

extern char temp_str[30];  /* declared in omega.c (used for debugging) */
extern int profile;        /* declared in omega.c (used for profiling) */


extern game_configuration_type game_configuration; /* From omega.c */


/* functions */

void init_wire_tube( PointFace *tube, PointEdge *wire_tube )
{
    EdgeTable temp_et;

    /* malloc wire_tube point array */

    if( (wire_tube->point = (Point *)malloc( sizeof( Point ) * tube->points )) == NULL ) {
	printf( "init_wire_tube() : malloc failed\n" );
    }

    /* copy tube's points into wire_tube */

    memcpy( wire_tube->point, tube->point, sizeof( Point ) * tube->points );
    wire_tube->points = tube->points;

    /* create an edge table from the pointface */

    temp_et.edge = NULL;
    temp_et.edges = 0;
    create_edge_table( tube, &temp_et );

    /* make wire_tube->edge point to temp_et.edge */

    wire_tube->edge = temp_et.edge;
    wire_tube->edges = temp_et.edges;

}

/* Prints out a matrix on the screen nice and pretty. (for debuging) */

void print_matrix( Matrix m )
{
    printf("| %-10ld %-10ld %-10ld %-10ld |\n",m[0],m[1],m[2],m[3]);
    printf("| %-10ld %-10ld %-10ld %-10ld |\n",m[4],m[5],m[6],m[7]);
    printf("| %-10ld %-10ld %-10ld %-10ld |\n",m[8],m[9],m[10],m[11]);
    printf("| %-10ld %-10ld %-10ld %-10ld |\n",m[12],m[13],m[14],m[15]);
}

/* Multiplies two 4x4 matrices, p & q, and sticks the result in r.
   Uses entirely fixed point math. */

void matrix_mult( Matrix p, Matrix q, Matrix r )
{
    /* first row */

    r[0] = ((p[0] * q[0]) >> MEXP) + ((p[1] * q[4]) >> MEXP) +
	   ((p[2] * q[8]) >> MEXP) + ((p[3] * q[12]) >> MEXP);

    r[1] = ((p[0] * q[1]) >> MEXP) + ((p[1] * q[5]) >> MEXP) +
	   ((p[2] * q[9]) >> MEXP) + ((p[3] * q[13]) >> MEXP);

    r[2] = ((p[0] * q[2]) >> MEXP) + ((p[1] * q[6]) >> MEXP) +
	   ((p[2] * q[10]) >> MEXP) + ((p[3] * q[14]) >> MEXP);

    r[3] = ((p[0] * q[3]) >> MEXP) + ((p[1] * q[7]) >> MEXP) +
	   ((p[2] * q[11]) >> MEXP) + ((p[3] * q[15]) >> MEXP);

    /* second row */

    r[4] = ((p[4] * q[0]) >> MEXP) + ((p[5] * q[4]) >> MEXP) +
	   ((p[6] * q[8]) >> MEXP) + ((p[7] * q[12]) >> MEXP);

    r[5] = ((p[4] * q[1]) >> MEXP) + ((p[5] * q[5]) >> MEXP) +
	   ((p[6] * q[9]) >> MEXP) + ((p[7] * q[13]) >> MEXP);

    r[6] = ((p[4] * q[2]) >> MEXP) + ((p[5] * q[6]) >> MEXP) +
	   ((p[6] * q[10]) >> MEXP) + ((p[7] * q[14]) >> MEXP);

    r[7] = ((p[4] * q[3]) >> MEXP) + ((p[5] * q[7]) >> MEXP) +
	   ((p[6] * q[11]) >> MEXP) + ((p[7] * q[15]) >> MEXP);

    /* third row */

    r[8] = ((p[8] * q[0]) >> MEXP) + ((p[9] * q[4]) >> MEXP) +
	   ((p[10] * q[8]) >> MEXP) + ((p[11] * q[12]) >> MEXP);

    r[9] = ((p[8] * q[1]) >> MEXP) + ((p[9] * q[5]) >> MEXP) +
	   ((p[10] * q[9]) >> MEXP) + ((p[11] * q[13]) >> MEXP);

    r[10] = ((p[8] * q[2]) >> MEXP) + ((p[9] * q[6]) >> MEXP) +
	    ((p[10] * q[10]) >> MEXP) + ((p[11] * q[14]) >> MEXP);

    r[11] = ((p[8] * q[3]) >> MEXP) + ((p[9] * q[7]) >> MEXP) +
	    ((p[10] * q[11]) >> MEXP) + ((p[11] * q[15]) >> MEXP);

    /* forth row */

    r[12] = ((p[12] * q[0]) >> MEXP) + ((p[13] * q[4]) >> MEXP) +
	   ((p[14] * q[8]) >> MEXP) + ((p[15] * q[12]) >> MEXP);

    r[13] = ((p[12] * q[1]) >> MEXP) + ((p[13] * q[5]) >> MEXP) +
	   ((p[14] * q[9]) >> MEXP) + ((p[15] * q[13]) >> MEXP);

    r[14] = ((p[12] * q[2]) >> MEXP) + ((p[13] * q[6]) >> MEXP) +
	    ((p[14] * q[10]) >> MEXP) + ((p[15] * q[14]) >> MEXP);

    r[15] = ((p[12] * q[3]) >> MEXP) + ((p[13] * q[7]) >> MEXP) +
	    ((p[14] * q[11]) >> MEXP) + ((p[15] * q[15]) >> MEXP);

}

/* Multiples 4x4 two matices, ip & iq, and sticks the result in result.
   Does the same as matrix_mult but uses floating point math to reduce
   the precision errors. */

void float_matrix_mult( Matrix ip, Matrix iq, Matrix result )
{
    float r[16];  /* temp float matrix, holds result */
    float p[16];  /* temp float matrix, holds ip */
    float q[16];  /* temp float matrix, holds iq */
    long i;

    /* copy ip into p */

    for( i = 0; i < 16; i++ ) {
	p[i] = mtof( ip[i] );
    }

    /* copy iq into q */

    for( i = 0; i < 16; i++ ) {
	q[i] = mtof( iq[i] );
    }

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

    /* copy r into result */

    result[0] = ftom( r[0] );
    result[1] = ftom( r[1] );
    result[2] = ftom( r[2] );
    result[3] = ftom( r[3] );
    result[4] = ftom( r[4] );
    result[5] = ftom( r[5] );
    result[6] = ftom( r[6] );
    result[7] = ftom( r[7] );
    result[8] = ftom( r[8] );
    result[9] = ftom( r[9] );
    result[10] = ftom( r[10] );
    result[11] = ftom( r[11] );
    result[12] = ftom( r[12] );
    result[13] = ftom( r[13] );
    result[14] = ftom( r[14] );
    result[15] = ftom( r[15] );
}

/* Prints out the indices of each edge in an EdgeTable. */

void print_edge_table( EdgeTable *et )
{
    long i;
    printf( "%ld\n", et->edges );

    for( i = 0; i < et->edges; i++ ) {
	printf( "%ld %ld\n", et->edge[i][0], et->edge[i][1] );
    }
}

void print_wire_object( PointEdge *pe )
{
    long i;

    printf("%ld %ld\n", pe->points, pe->edges );

    for( i = 0; i < pe->points; i++ ) {
	printf("%f %f %f\n", mtof( pe->point[i][X] ), mtof( pe->point[i][Y] ),
	    mtof( pe->point[i][Z] ) );
    }

    for( i = 0; i < pe->edges; i++ ) {
	printf("%ld %ld\n", pe->edge[i][0], pe->edge[i][1] );
    }
}

void get_wire_object( PointEdge *pe, const char *filename )
{
    FILE *fp;
    long points, edges, i;
    float x, y, z;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);


    /* free the pointedge */

    free( pe->point );
    pe->points = 0;
    free( pe->edge );
    pe->edge = 0;

    /* open up the file */

    if( (fp = fopen( newfilename, "r" )) <= 0 ) 	
        SYS_Error( "get_wire_object(): Invalid filename %s!\n", filename );    

    /* get the size of the object */

    fscanf( fp, " %ld %ld ", &points, &edges );
    pe->points = points;
    pe->edges = edges;

    /* malloc the points */

    if( ( pe->point = (Point *)malloc( sizeof( Point ) * points ) ) == NULL ) 	
		SYS_Error( "get_wire_object() : malloc failed\n" );

    /* malloc the edges */

    if( ( pe->edge = (Edge *)malloc( sizeof( Edge ) * edges ) ) == NULL ) 	
		SYS_Error( "get_wire_object() : malloc failed\n" );    

    /* scan in the points */

    for( i = 0; i < points; i++ ) {
	fscanf( fp, " %f %f %f ", &x, &y, &z );
	pe->point[i][X] = ftom( x );
	pe->point[i][Y] = ftom( y );
	pe->point[i][Z] = ftom( z );
    }

    /* scan in the edges */

    for( i = 0; i < edges; i++ ) {
	fscanf( fp, " %ld %ld ", &(pe->edge[i][0]), &(pe->edge[i][1]) );
    }

    fclose( fp );
}

/* Tells if a given edge is already in an EdgeTable.  Used to eliminate
   redundent edges in an EdgeTable. */

int in_edge_table( EdgeTable *et, Edge edge )
{
    long i;

    for( i = 0; i < et->edges; i++ ) {
	if( ( edge[0] == et->edge[i][0] ) && ( edge[1] == et->edge[i][1] ) ) {
	    return TRUE;
	}
	if( ( edge[1] == et->edge[i][0] ) && ( edge[0] == et->edge[i][1] ) ) {
	    return TRUE;
	}
    }
    return FALSE;
}

/* Takes a vehicles PointFace and builds an EdgeTable from it, and stores it
   in the vehicles data structure. */

/* Hack Alert !!!!!!!!
    This functions assumes that each face will have no more than 6 vertices
    in each face( on average ).  It will work fine as long as the vehicle
    PointFace adheres to this rule.
*/

void init_edge_table( Vehicle *v )
{
printf("init_edge_table start\n");

    long i, j;
    EdgeTable et;
    Edge edge;

    free( v->collision_edges.edge );
    v->collision_edges.edge = NULL;
    v->collision_edges.edges = 0;

    /* malloc the edges (see above Hack Alert) */

    if( ( et.edge = (Edge *) malloc( sizeof( Edge ) * 6 * v->collision_obj->faces ) ) == NULL )
		SYS_Error( "init_edge_table() : malloc failed\n" );

    et.edges = 0;

    /* insert edges into edge table, while not inserting redundent edges. */

    for( i = 0; i < v->collision_obj->faces; i++ ) {
	for( j = 0; j < v->collision_obj->face[i].size; j++ ) {
	    edge[0] = v->collision_obj->face[i].index[j];
	    edge[1] = v->collision_obj->face[i].index[(j + 1) % v->collision_obj->face[i].size];

	    if( !in_edge_table( &et, edge ) ) {
		et.edge[et.edges][0] = edge[0];
		et.edge[et.edges][1] = edge[1];
		et.edges++;
	    }
	}
    }

    /* realloc edges */
    if( ( et.edge = (Edge *) realloc( et.edge, sizeof( Edge ) * et.edges )) == NULL )
		SYS_Error( "init_edge_table() : realloc failed\n" );    

    v->collision_edges.edge = et.edge;
    v->collision_edges.edges = et.edges;
}

void create_edge_table( PointFace *obj, EdgeTable *et )
{
    long i, j;
    Edge edge;

    /* malloc the edges (see above Hack Alert) */

    if( ( et->edge = (Edge *) malloc( sizeof( Edge ) * 6 * obj->faces ) ) == NULL )
		SYS_Error( "create_edge_table() : malloc failed\n" );

    et->edges = 0;

    /* insert edges into edge table, while not inserting redundant edges. */

    for( i = 0; i < obj->faces; i++ ) {
	for( j = 0; j < obj->face[i].size; j++ ) {
	    edge[0] = obj->face[i].index[j];
	    edge[1] = obj->face[i].index[(j + 1) % obj->face[i].size];

	    if( !in_edge_table( et, edge ) ) {
		et->edge[et->edges][0] = edge[0];
		et->edge[et->edges][1] = edge[1];
		et->edges++;
	    }
	}
    }

    /* realloc edges */

    if( ( et->edge = (Edge *) realloc( et->edge, sizeof( Edge ) * et->edges )) == NULL )
		SYS_Error( "create_edge_table() : realloc failed\n" );
}

/* Basicaly does the same as init_edge_table but for a projectile instead of
   a vehicle. */

/* Hack Alert !!!!!!
    same as in init_edge_table
*/

void init_projectile_edge_table( Projectile *p )
{
    long i, j;
    EdgeTable et;
    Edge edge;

    /* malloc the edges ( see Hack Alert ) */

    if( ( et.edge = (Edge *) malloc( sizeof( Edge ) * 4 * p->obj->faces ) )	== NULL )
		SYS_Error( "init_projectile_edge_table() : malloc failed\n" );

    et.edges = 0;

    /* insert edges into edge table, while not inserting redundent edges. */

    for( i = 0; i < p->obj->faces; i++ ) {
	for( j = 0; j < p->obj->face[i].size; j++ ) {
	    edge[0] = p->obj->face[i].index[j];
	    edge[1] = p->obj->face[i].index[(j + 1) % p->obj->face[i].size];

	    if( !in_edge_table( &et, edge ) ) {
		et.edge[et.edges][0] = edge[0];
		et.edge[et.edges][1] = edge[1];
		et.edges++;
	    }
	}
    }

    /* realloc edges */

    if( ( et.edge = (Edge *) realloc( et.edge, sizeof( Edge ) * et.edges )) == NULL )
		SYS_Error( "init_projectile_edge_table() : realloc failed\n" );

    p->edge_table.edge = et.edge;
    p->edge_table.edges = et.edges;
}

/* Takes the PointFace obj, which is centered at the origin, and aligns its
   x-axis with the orientation's front vector, its y-axis with the
   orientation's up vector, its origin with the orientation's position. */

void orient_object( PointFace *obj, Orientation *o, PointFace *result )
{
    long i;
    Point p;
    Matrix trans_rotation_matrix;

    free_object( result );

    /* malloc point array */

    if( ( result->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
        SYS_Error( "orient_object() : malloc failed!\n" );

    /* malloc face array */

    if( ( result->face = (Face *)malloc( sizeof(Face) * obj->faces ) ) == NULL )
        SYS_Error( "orient_object() : malloc failed!!\n" );

    result->points = obj->points;
    result->faces = obj->faces;

    /* init the transformation matrix */

    p[X] = ftom( o->position[X] );
    p[Y] = ftom( o->position[Y] );
    p[Z] = ftom( o->position[Z] );

    trans_rot_matrix_init( trans_rotation_matrix, o->front, o->up );

    /* copy all the points from obj->point to result->point */

    memcpy( result->point, obj->point, sizeof( Point ) * obj->points );

    /* transform all the points from obj into world_obj */

    for( i = 0; i < obj->points; i++ ) {
	rotation_transformation( trans_rotation_matrix, result->point[i] );
	translation_transformation( p[X], p[Y], p[Z], result->point[i] );
    }

    /* copy all the faces from obj to result */

    memcpy( result->face, obj->face, sizeof( Face ) * obj->faces );

    /* rotate all the normal vectors */

    for( i = 0; i < obj->faces; i++ ) {
       rotation_transformation( trans_rotation_matrix, result->face[i].normal );
    }
}

/* Does the same as orient_object except that it expects the orientation
   structure to hold longs rather than floats */

void magic_orient_object( PointFace *obj, MagicOrientation *o, PointFace *result )
{
    long i;
    Matrix trans_rotation_matrix;

    free_object( result );

    /* malloc point array */

    if( ( result->point = (Point *)malloc( sizeof( Point ) * obj->points ) ) == NULL )
		SYS_Error( "transform_pfd() : malloc failed!\n" );

    /* malloc face array */

    if( ( result->face = (Face *)malloc( sizeof(Face) * obj->faces ) ) == NULL )
		SYS_Error( "transform_pfd() : malloc2 failed!!\n" );

    result->points = obj->points;
    result->faces = obj->faces;

    /* init the transformation matrix */

    magic_trans_rot_matrix_init( trans_rotation_matrix, o->front, o->up );

    /* copy all the points from obj into result */

    memcpy( result->point, obj->point, sizeof( Point ) * obj->points );

    /* transform all the points */

    for( i = 0; i < obj->points; i++ ) {
	rotation_transformation( trans_rotation_matrix, result->point[i] );
	translation_transformation( o->position[X], o->position[Y],
				    o->position[Z], result->point[i] );
    }

    /* copy all the faces from obj into result */

    memcpy( result->face, obj->face, sizeof( Face ) * obj->faces );

    /* rotate all the normal vectors */

    for( i = 0; i < obj->faces; i++ ) {
       rotation_transformation( trans_rotation_matrix, result->face[i].normal );
    }
}

/* Initializes the vehicles world_obj, given its current Orientation
   and obj. */

void init_world_obj( Vehicle *v )
{
    orient_object( v->obj, &(v->orient), v->world_obj );
}

void init_world_collision_obj( Vehicle *v )
{
    orient_object( v->collision_obj, &(v->orient), v->world_collision_obj );
}

/* Takes obj and moves its origin to the orientation's position point,
   aligns its x-axis with orientation's front vector, aligns its y-axis
   with orientation's up vector. Takes the result of this and unions it
   with result. I don't think I even use this anymore. */

void transform_and_union( Orientation *obj_orient, PointFace *obj,
			  PointFace *result )
{
    long i, j;
    Point p;
    Point temp;
    long point_start, face_start;

    /* realloc the extra space needed for the union */

    if( ( result->point = (Point*)realloc( result->point,
	( result->points + obj->points ) * sizeof( Point ) ) ) == NULL ) {
	printf("transform_and_union : realloc failed\n");
	exit(1);
    }

    point_start = result->points;
    result->points = result->points + obj->points;

    if( ( result->face = (Face*)realloc( result->face,
	( result->faces + obj->faces ) * sizeof( Face ) ) ) == NULL ) {
	printf("transform_and_union : realloc failed 2\n");
	exit(1);
    }

    face_start = result->faces;
    result->faces = result->faces + obj->faces;

    /* init the transformation matrix */

    p[X] = ftom( obj_orient->position[X] );
    p[Y] = ftom( obj_orient->position[Y] );
    p[Z] = ftom( obj_orient->position[Z] );

    trans_rot_matrix_init( trans_rotation_matrix, obj_orient->front,
			   obj_orient->up );

    /* transform all the points and union or something huh huh huh.*/

    for( i = 0; i < obj->points; i++ ) {

	temp[X] = obj->point[i][X];
	temp[Y] = obj->point[i][Y];
	temp[Z] = obj->point[i][Z];

	rotation_transformation( trans_rotation_matrix, temp );
	translation_transformation( p[X], p[Y], p[Z], temp );

	result->point[point_start + i][X] = temp[X];
	result->point[point_start + i][Y] = temp[Y];
	result->point[point_start + i][Z] = temp[Z];
    }

    for( i = 0; i < obj->faces; i++ ) {
       result->face[face_start + i].size = obj->face[i].size;
       for( j = 0; j < obj->face[i].size; j++ ) {
	   result->face[face_start + i].index[j] = obj->face[i].index[j] + point_start;
       }
       result->face[face_start + i].size = obj->face[i].size;
       result->face[face_start + i].gradient = obj->face[i].gradient;

       temp[X] = obj->face[i].normal[X];
       temp[Y] = obj->face[i].normal[Y];
       temp[Z] = obj->face[i].normal[Z];

       rotation_transformation( trans_rotation_matrix, temp );

       result->face[face_start + i].normal[X] = temp[X];
       result->face[face_start + i].normal[Y] = temp[Y];
       result->face[face_start + i].normal[Z] = temp[Z];

       result->face[face_start + i].transparent = obj->face[i].transparent;
    }
}

/* Takes the PointFace obj and combines it with result. Result will contain
   all of its original points and faces as well as the points and faces of
   obj. */

void union_object( PointFace *obj, PointFace *result )
{
    long i, j;
    long point_start, face_start;

    if( result->point == NULL || result->points == 0 ) {

	/* malloc result's point array */

	if( ( result->point = (Point *)malloc( obj->points * sizeof( Point ) ) ) == NULL ) {
	    printf("union_object() : malloc failed!!\n");
	    exit( 1 );
	}
	result->points = 0;
    }
    else {

	/* realloc result's point array */

	if( ( result->point = (Point *)realloc( result->point,
	    ( result->points + obj->points ) * sizeof( Point ) ) ) == NULL ) {
	    printf("union_object() : realloc failed\n");
	    exit(1);
	}
    }

    point_start = result->points;
    result->points = result->points + obj->points;

    if( result->face == NULL || result->faces == 0 ) {

	/* malloc result's face array */

	if( ( result->face = (Face *)malloc( obj->faces * sizeof( Face ) ) ) == NULL ) {
	    printf("union_object() : malloc failed\n");
	    exit( 1 );
	}
	result->faces = 0;
    }
    else {
	/* realloc result's face array */

	if( ( result->face = (Face*)realloc( result->face,
	    ( result->faces + obj->faces ) * sizeof( Face ) ) ) == NULL ) {
	    printf("union_object() : realloc failed 2\n");
	    exit(1);
	}
    }

    face_start = result->faces;
    result->faces = result->faces + obj->faces;

    /* copy all of obj's points onto the end of result's point array */

    memcpy( &result->point[point_start], obj->point, sizeof( Point ) *
	    obj->points );

    /* copy all of obj's face onto the end of result's face array */

    for( i = 0; i < obj->faces; i++ ) {
       for( j = 0; j < obj->face[i].size; j++ ) {
	   result->face[face_start + i].index[j] = obj->face[i].index[j] +
						   point_start;
       }
       result->face[face_start + i].size = obj->face[i].size;
       result->face[face_start + i].gradient = obj->face[i].gradient;

       result->face[face_start + i].normal[X] = obj->face[i].normal[X];
       result->face[face_start + i].normal[Y] = obj->face[i].normal[Y];
       result->face[face_start + i].normal[Z] = obj->face[i].normal[Z];

       result->face[face_start + i].transparent = obj->face[i].transparent;
    }
}

void object_copy( PointFace *result, PointFace *obj )
{
    if( result->point == NULL || result->points == 0 ) {

	/* malloc result's point array */

	if( ( result->point = (Point *)malloc( obj->points * sizeof( Point ) ) ) == NULL ) {
	    printf("object_copy() : malloc failed!!\n");
	    exit( 1 );
	}
	result->points = 0;
    }
    else {

	/* realloc result's point array */

	if( ( result->point = (Point *)realloc( result->point,
	    ( obj->points ) * sizeof( Point ) ) ) == NULL ) {
	    printf("object_copy() : realloc failed\n");
	    exit(1);
	}
    }

    if( result->face == NULL || result->faces == 0 ) {

	/* malloc result's face array */

	if( ( result->face = (Face *)malloc( obj->faces * sizeof( Face ) ) ) == NULL ) {
	    printf("object_copy() : malloc failed\n");
	    exit( 1 );
	}
	result->faces = 0;
    }
    else {
	/* realloc result's face array */

	if( ( result->face = (Face*)realloc( result->face,
	    ( obj->faces ) * sizeof( Face ) ) ) == NULL ) {
	    printf("object_copy() : realloc failed 2\n");
	    exit(1);
	}
    }

    /* copy obj's points into result */

    memcpy( result->point, obj->point, sizeof(Point) * obj->points );
    result->points = obj->points;

    /* copy obj's faces into result */

    memcpy( result->face, obj->face, sizeof(Face) * obj->faces );
    result->faces = obj->faces;
}

/* Initalizes the global projection matrix. Only needs to be called once
   at the begining of the game. */

void projection_matrix_init()
{
    /* Initalize the projection matrix. */

    projection_matrix[0] = MAGIC;
    projection_matrix[1] = 0;
    projection_matrix[2] = 0;
    projection_matrix[3] = 0;
    projection_matrix[4] = 0;
    projection_matrix[5] = MAGIC;
    projection_matrix[6] = 0;
    projection_matrix[7] = 0;
    projection_matrix[8] = 0;
    projection_matrix[9] = 0;
    projection_matrix[10] = 0;
    projection_matrix[11] = 0;
    projection_matrix[12] = 0;
    projection_matrix[13] = 0;
    projection_matrix[14] = rounding_ftom( 1.0f / mtof( -vrpz ) );
    projection_matrix[15] = MAGIC;
}

/* Initializes the global window to viewport mapping matrix, which takes a
   point who has already been projected onto the view plane and transforms
   it into screen coords (still in fixed-point). Needs to be called only
   once at start of game. */

void wtov_matrix_init()
{
    const float umin = -2.0;
    const float umax = 321.0;
    const float vmin = -2.0;
    const float vmax = 201.0;

    wtov_matrix[0] = -rounding_ftom( (umax - umin) / mtof(xmax - xmin) );
    wtov_matrix[1] = 0;
    wtov_matrix[2] = 0;
    wtov_matrix[3] = rounding_ftom( ( mtof(-xmin) * (umax - umin) / mtof(xmax - xmin) ) + umin );
    wtov_matrix[4] = 0;
    wtov_matrix[5] = rounding_ftom( (vmax - vmin) / mtof(ymax - ymin) );
    wtov_matrix[6] = 0;
    wtov_matrix[7] = rounding_ftom( ( mtof(-ymin) * (vmax - vmin) / mtof(ymax - ymin) ) + vmin );
    wtov_matrix[8] = 0;
    wtov_matrix[9] = 0;
    wtov_matrix[10] = MAGIC;
    wtov_matrix[11] = 0;
    wtov_matrix[12] = 0;
    wtov_matrix[13] = 0;
    wtov_matrix[14] = 0;
    wtov_matrix[15] = MAGIC;
}



/* prints all the vertices and faces in an object. For debuging. */

void print_object( PointFace *obj )
{
    int i, j;

    printf("points = %ld, faces = %ld\n", obj->points, obj->faces );

    for( i = 0; i < obj->points; i++ ) {
	printf("(%ld,%ld,%ld)\n", obj->point[i][X], obj->point[i][Y],
				  obj->point[i][Z]);
    }
    for( i = 0; i < obj->faces; i++ ) {
	for( j = 0; j < obj->face[i].size; j++ ) {
	    printf("%ld ", obj->face[i].index[j]);
	}
	printf("\n");
    }
}

/* frees the point array and the face array of obj */

void free_object( PointFace *obj )
{
    /* free all the points */

    free( obj->point );
    obj->point = NULL;
    obj->points = 0;

    /* free all the faces */

    free( obj->face );
    obj->point = NULL;
    obj->faces = 0;
}


/* Reads in all the points and faces and face colors from a file.  It also
   precomputes the normal vectors for each face. The file is assumed to be
   in the following format :

       The first line contains two ints, the first is the total number of
   points in the object, the second is the total number of faces in the
   object.

       The next n lines, where n is the number of points, contain 3 floats,
   the first is this vertex's x coordinate, the second is this vertex's y
   coordnate, the third is this vertex's z coordnate.

       The next m lines, where m is the number of faces, contains an int
   which indicates the number of indices in this face.  This is followed by
   p ints, where p is the number of indices in this face, each int represents
   one index of this face. Each index uniquely represents one point.  So the
   index 5 would mean that this face contains the fifth point listed. Indices
   start at 1.

       The next m tokens, define the color of each face.

       The visible faces are to be listed in a counter-clockwise order.  The
   object is assumed to be centered at the origin. No comments are allowed.

   example :

       3 2
       0.0 0.0 0.0
       1.0 1.0 1.0
       0.0 1.0 0.0
       -1.0 -1.0 -1.0
       3 1 2 3
       3 4 3 2
       2 2
*/

void get_object( PointFace *obj, const char *filename )
{
    FILE *fp;    /* input filepointer */
    int pts;     /* total number of points in the object */
    int polys;   /* total number of faces in the object */
    int i, j;
    float x, y, z;      /* holds values of vertices as they are read in */
    float uxv_dot_uxv;  /* the dot product of uxv with itself */
    float len;          /* the length of uxv */
    float u[3];         /* one edge of a polgon */
    float v[3];         /* another edge of a polgon */
    float uxv[3];       /* the normal vector of that polygon (u X v) */
    int grad;           /* used to read in the color of each face */
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);


    /* free the object */

    free_object( obj );

    /* open up the file */

    if( (fp = fopen( newfilename, "r" )) <= 0 )
		SYS_Error( "get_object(): Invalid filname '%s'!\n", newfilename );

    /* get the size of the object */

    fscanf( fp, " %d %d ", &pts, &polys );
    obj->points = pts;
    obj->faces = polys;

    /* malloc point array */

    if( ( obj->point = (Point *)malloc( sizeof( Point ) * pts ) ) == NULL )
		SYS_Error( "get_object() : malloc failed!\n" );

    /* malloc face array */

    if( (obj->face = (Face *)malloc( sizeof(Face) * polys ) ) == NULL )
		SYS_Error( "get_object() : malloc2 failed!!\n" );

    /* get each vertex */

    for( i = 0; i < pts; i++ ) {
	fscanf( fp, " %f %f %f ", &x, &y, &z );
	obj->point[i][X] = ftom( x );
	obj->point[i][Y] = ftom( y );
	obj->point[i][Z] = ftom( z );
    }

    /* get each face */

    for( i = 0; i < polys; i++ ) {
	fscanf( fp, " %d ", &pts );
	obj->face[i].size = pts;
	for( j = 0; j < pts; j++ ) {
	    fscanf( fp, " %ld ", &obj->face[i].index[j]);
	    obj->face[i].index[j]--; /* face indices start at 1 in file but */
	}                            /* start at 0 in C. */
    }

    /* get each faces color */

    for( i = 0; i < polys; i++ ) {
	fscanf( fp, " %d ", &(grad) );
	obj->face[i].gradient = grad;
    }


    fclose(fp);

    /* pre-compute normal vectors for each face  */
    /* ( so we don't have to figure them out every time we draw a face ) */

    for( i = 0; i < obj->faces; i++ ) {

	    /* u is a vector from the second vertex to the first */

	    u[X] = (float)(obj->point[obj->face[i].index[0]][X] - obj->point[obj->face[i].index[1]][X]);
	    u[Y] = (float)(obj->point[obj->face[i].index[0]][Y] - obj->point[obj->face[i].index[1]][Y]);
	    u[Z] = (float)(obj->point[obj->face[i].index[0]][Z] - obj->point[obj->face[i].index[1]][Z]);

	    /* v is a vector from the second vertex to the third */

	    v[X] = (float)(obj->point[obj->face[i].index[2]][X] - obj->point[obj->face[i].index[1]][X]);
	    v[Y] = (float)(obj->point[obj->face[i].index[2]][Y] - obj->point[obj->face[i].index[1]][Y]);
	    v[Z] = (float)(obj->point[obj->face[i].index[2]][Z] - obj->point[obj->face[i].index[1]][Z]);

	    /* uxv is the cross product of u and v, and therefore the
	       normal vector for that face.  But we still have to
	       normalize it */

	    uxv[X] = (u[Y] * v[Z]) - (u[Z] * v[Y] );
	    uxv[Y] = (u[Z] * v[X]) - (u[X] * v[Z] );
	    uxv[Z] = (u[X] * v[Y]) - (u[Y] * v[X] );

	    /* Dot uxv with itself */

	    uxv_dot_uxv = (uxv[X] * uxv[X]) + (uxv[Y] * uxv[Y]) + (uxv[Z] * uxv[Z] );

	    /* get the length of uxv */

	    len = (float)sqrt( uxv_dot_uxv );

	    /* normalize uxv */

	    uxv[X] = uxv[X] / len;
	    uxv[Y] = uxv[Y] / len;
	    uxv[Z] = uxv[Z] / len;

	    /* initialize the normal in face */

	    obj->face[i].normal[X] = ftom( uxv[X] );
	    obj->face[i].normal[Y] = ftom( uxv[Y] );
	    obj->face[i].normal[Z] = ftom( uxv[Z] );

	    /* initialize the transparent flag */

	    obj->face[i].transparent = FALSE;
    }
}

/* Transforms vertex according to wtov_matrix.  Optomized for the
   wtov_matrix. */

void wtov_transformation( long *wtov_matrix, long *vertex )
{
     vertex[X] = rounding_fixed_multiply(wtov_matrix[0], vertex[X]) + wtov_matrix[3];
     vertex[Y] = rounding_fixed_multiply(wtov_matrix[5], vertex[Y]) + wtov_matrix[7];
}


/* Matrix mult of the per_matrix and vertex. Optomized for the projection
   transformation. */

void projection_transformation( long *per_matrix, long *vertex )
{
    long homo;
    long temp_vertex[3];

    temp_vertex[X] = rounding_fixed_multiply( per_matrix[0], vertex[X] );
    temp_vertex[Y] = rounding_fixed_multiply( per_matrix[5], vertex[Y] );
    homo = rounding_fixed_multiply( per_matrix[14], vertex[Z] ) + per_matrix[15];

    if( homo == 0 ) {
	homo = 1;
    }

    vertex[X] = (temp_vertex[X] << MEXP) / homo;
    vertex[Y] = (temp_vertex[Y] << MEXP) / homo;
    vertex[Z] = 0;
}

/* transformation of vertex by mat, optomized for the scale and inv_scale
   matrices. */

void scale_transformation( long *mat, long *vertex )
{
    vertex[X] = rounding_fixed_multiply(vertex[X], mat[0]);
    vertex[Y] = rounding_fixed_multiply(vertex[Y], mat[5]);
    vertex[Z] = rounding_fixed_multiply(vertex[Z], mat[10]);
}

/* matrix multiplication of matix m and vector v
   optimized for the rotation transformation */

void rotation_transformation( long *rot_matrix, long *vertex )
{
    long temp_vertex[3];

    temp_vertex[X] = rounding_fixed_multiply(rot_matrix[0], vertex[X]) +
                     rounding_fixed_multiply(rot_matrix[1], vertex[Y]) +
                     rounding_fixed_multiply(rot_matrix[2], vertex[Z]);
    temp_vertex[Y] = rounding_fixed_multiply(rot_matrix[4], vertex[X]) +
                     rounding_fixed_multiply(rot_matrix[5], vertex[Y]) +
                     rounding_fixed_multiply(rot_matrix[6], vertex[Z]);
    temp_vertex[Z] = rounding_fixed_multiply(rot_matrix[8], vertex[X]) +
                     rounding_fixed_multiply(rot_matrix[9], vertex[Y]) +
                     rounding_fixed_multiply(rot_matrix[10], vertex[Z]);

    vertex[X] = temp_vertex[X];
    vertex[Y] = temp_vertex[Y];
    vertex[Z] = temp_vertex[Z];
}


/* moves vertex by tx along the x-axis, by ty along y-axis and by tz
   along z-axis */

void translation_transformation( long tx, long ty, long tz, long *vertex )
{
    vertex[X] = vertex[X] + tx;
    vertex[Y] = vertex[Y] + ty;
    vertex[Z] = vertex[Z] + tz;
}


/* returns the largest z value of face f */

long find_max_z( Face *f, Point vert[] )
{
    long max;
    int i;
    int size = f->size;
    long *index = f->index;

    max = vert[index[0]][Z];
    for( i = 1; i < size; i++ ) {
	if( max <= vert[index[i]][Z] )
	    max = vert[index[i]][Z];
    }
    return max;
}

/* comparison function for the qsort */

int cmp( const void* p, const void* q )
{
    long pmax, qmax;	

    pmax = static_cast<const Face*>(p)->max_z;
    qmax = static_cast<const Face*>(q)->max_z;

    if( pmax > qmax )
	return -1;
    else if( pmax < qmax )
	return 1;
    else
	return 0;
}

/* Initalizes the matrix t, to translate a point tx along the x-axis,
   ty along the y-axis and tz along the z-axis. */

void translation_matrix_init( Matrix t, long tx, long ty, long tz )
{
    t[0] = MAGIC;
    t[1] = 0;
    t[2] = 0;
    t[3] = tx;
    t[4] = 0;
    t[5] = MAGIC;
    t[6] = 0;
    t[7] = ty;
    t[8] = 0;
    t[9] = 0;
    t[10] = MAGIC;
    t[11] = tz;
    t[12] = 0;
    t[13] = 0;
    t[14] = 0;
    t[15] = MAGIC;
}

/* general transformation of point v by matrix m.  Works for ANY 4x4 matrix
   and ANY point (as long as there is no overflow).

   | a b c d |
   | e f g h |
   | i j k l |
   | m n o p | */

void linear_transformation( long *m, long *v )
{
    long tv[4];

    tv[X] = rounding_fixed_multiply(m[0], v[X]) +
            rounding_fixed_multiply(m[1], v[Y]) +
            rounding_fixed_multiply(m[2], v[Z]) + m[3];

    tv[Y] = rounding_fixed_multiply(m[4], v[X]) +
            rounding_fixed_multiply(m[5], v[Y]) +
            rounding_fixed_multiply(m[6], v[Z]) + m[7];

    tv[Z] = rounding_fixed_multiply(m[8], v[X]) +
            rounding_fixed_multiply(m[9], v[Y]) +
            rounding_fixed_multiply(m[10], v[Z]) + m[11];

    tv[3] = rounding_fixed_multiply(m[12], v[X]) +
            rounding_fixed_multiply(m[13], v[Y]) +
            rounding_fixed_multiply(m[14], v[Z]) + m[15];

    if( tv[3] != MAGIC && tv[3] != 0 ) {
	v[X] = (tv[X] << MEXP) / tv[3];
	v[Y] = (tv[Y] << MEXP) / tv[3];
	v[Z] = (tv[Z] << MEXP) / tv[3];
    }
    else {
	v[X] = tv[X];
	v[Y] = tv[Y];
	v[Z] = tv[Z];
    }
}

/* A less general version of linear_transformation, it will work on any
   4x4 matrix that has [0 0 0 1] for its bottom row.

   | a b c d |
   | e f g h |
   | i j k l |
   | 0 0 0 1 | */

void linear_transformation1( long *m, long *v )
{
    long tv[4];

    tv[X] = rounding_fixed_multiply(m[0], v[X]) +
            rounding_fixed_multiply(m[1], v[Y]) +
            rounding_fixed_multiply(m[2], v[Z]) + m[3];

    tv[Y] = rounding_fixed_multiply(m[4], v[X]) +
            rounding_fixed_multiply(m[5], v[Y]) +
            rounding_fixed_multiply(m[6], v[Z]) + m[7];

    tv[Z] = rounding_fixed_multiply(m[8], v[X]) +
            rounding_fixed_multiply(m[9], v[Y]) +
            rounding_fixed_multiply(m[10], v[Z]) + m[11];

    v[X] = tv[X];
    v[Y] = tv[Y];
    v[Z] = tv[Z];
}

/* an even less general version of linear_transformation, it works for any
   4x4 matrix in this form: which is suited to wtov trans and projection.

   | a b c d |
   | e f g h |
   | 0 0 1 0 |
   | 0 0 i 1 | */

void linear_transformation2( long *m, long *v )
{
    long tv[4];

    tv[X] = rounding_fixed_multiply(m[0], v[X]) +
            rounding_fixed_multiply(m[1], v[Y]) +
            rounding_fixed_multiply(m[2], v[Z]) + m[3];

    tv[Y] = rounding_fixed_multiply(m[4], v[X]) +
            rounding_fixed_multiply(m[5], v[Y]) +
            rounding_fixed_multiply(m[6], v[Z]) + m[7];

    tv[3] = rounding_fixed_multiply(m[14], v[Z]);

    v[X] = (tv[X] << MEXP) / tv[3];
    v[Y] = (tv[Y] << MEXP) / tv[3];
}

void view_point_tube( Point tube_points[], long num_points,
                      Orientation *view_orientation, Point light )
{
    long i;
    Point p;
    Matrix translation_matrix, temp_matrix, result_matrix;
    unsigned char shade = 0;

    /* copy the points from tube into view_vertices */

    memcpy( view_vertices, tube_points, sizeof( Point ) * num_points );

#ifdef PROFILE
    if( profile ) {
		printf("    translation_matrix_init(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* figure out matrix needed to translate, rotate and scale into
       cannonical view volume. Stick it in result_matrix */

    p[X] = ftom( view_orientation->position[X] );
    p[Y] = ftom( view_orientation->position[Y] );
    p[Z] = ftom( view_orientation->position[Z] );

    translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

#ifdef PROFILE
    if( profile ) {
		printf("    rot_matrix_init(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    rot_matrix_init( rotation_matrix, view_orientation->front,
		     view_orientation->up );

#ifdef PROFILE
    if( profile ) {
		printf("    matrix_mult(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

#ifdef PROFILE
    if( profile ) {
		printf("    transform view_vert for clipping, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* transform view_vertices into cannonical view volume (for clipping) */

    for( i = 0; i < num_points; i++ ) {
	linear_transformation1( temp_matrix, view_vertices[i] );
	scale_transformation( scale_matrix, view_vertices[i] );
    }

#ifdef PROFILE
    if( profile ) {
        printf("    clip the points, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* assume all points are clipped_away */

    memset( clipped_away, TRUE, MAX_POINTS );

    for( i = 0; i < num_points; i++ ) {
        if( region( view_vertices[i], new_hither ) == 0 ) { /* inside */
            clipped_away[i] = FALSE;
        }
    }

#ifdef PROFILE
    if( profile ) {
		printf("    translation_matrix_init, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* figure out matrix needed to unscale from cannonical view volume,
       translate by -vrp, project onto view plane, then scale and translate
       into screen coordinates.  Stick it in result_matrix */

    translation_matrix_init( translation_matrix, 0, 0, vrpz );

#ifdef PROFILE
    if( profile ) {
		printf("    2 matrix mults, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    matrix_mult( translation_matrix, inv_scale_matrix, temp_matrix );
    matrix_mult( projection_matrix, temp_matrix, result_matrix );

    /* transform everything */

#ifdef PROFILE
    if( profile ) {
		printf("    transform view_vert into projected_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < num_points; i++ ) {

	if( !clipped_away[i] ) {

	    /* copy from view_vertices into projected_vertices */

	    memcpy( projected_vertices[i], view_vertices[i], sizeof( Point ) );

	    /* transform */

	    linear_transformation( result_matrix, projected_vertices[i] );
	    wtov_transformation( wtov_matrix, projected_vertices[i] );
	}
    }

#ifdef PROFILE
    if( profile ) {
		printf("    shade and draw loop, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < num_points; i++ ) {
        if( !clipped_away[i] ) {
            shade = tube_shade( view_vertices[i][Z], TubeFirstGrad );
            DB_Scissor_Pixel( projected_vertices[i][X] >> MEXP, projected_vertices[i][Y] >> MEXP, shade );
        }
    }

#ifdef PROFILE
    if( profile ) {
        printf("    end of view_point_tube(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif
}


void view_wire_tube( PointEdge *wire_tube, Orientation *view_orientation,
		     Point light )
{
    long i, len, temp;
    Point p;
    Matrix translation_matrix, temp_matrix, result_matrix;
    PointEdge clip_pe;
    unsigned char shade = 0;
    Vector l;

    /* copy the points from tube into view_vertices */

    memcpy( view_vertices, wire_tube->point, sizeof( Point ) * wire_tube->points );

#ifdef PROFILE
    if( profile ) {
		printf("    translation_matrix_init(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* figure out matrix needed to translate, rotate and scale into
       cannonical view volume. Stick it in result_matrix */

    p[X] = ftom( view_orientation->position[X] );
    p[Y] = ftom( view_orientation->position[Y] );
    p[Z] = ftom( view_orientation->position[Z] );

    translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

#ifdef PROFILE
    if( profile ) {
		printf("    rot_matrix_init(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    rot_matrix_init( rotation_matrix, view_orientation->front,
		     view_orientation->up );

#ifdef PROFILE
    if( profile ) {
		printf("    matrix_mult(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

#ifdef PROFILE
    if( profile ) {
		printf("    transform view_vert for clipping, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* transform view_vertices into cannonical view volume (for clipping) */

    for( i = 0; i < wire_tube->points; i++ ) {
	linear_transformation1( temp_matrix, view_vertices[i] );
	scale_transformation( scale_matrix, view_vertices[i] );
    }

#ifdef PROFILE
    if( profile ) {
		printf("    initialize clip_pe for clipping, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* initialize clip_pe for clipping */

    clip_pe.point = view_vertices;
    clip_pe.points = wire_tube->points;
    clip_pe.edge = global_edge_array;
    memcpy( clip_pe.edge, wire_tube->edge, sizeof( Edge ) * wire_tube->edges );
    clip_pe.edges = wire_tube->edges;

#ifdef PROFILE
    if( profile ) {
        printf("    clip_edges(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* clip */

    clip_edges( &clip_pe, new_hither );

#ifdef PROFILE
    if( profile ) {
        printf( "init clipped_away array, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* loop through all the edges and flag the vertices that are used */

    memset( clipped_away, TRUE, MAX_POINTS );

    for( i = 0; i < clip_pe.edges; i++ ) {
	if( clip_pe.edge[i][0] != NULL_INDEX ) {
	    clipped_away[clip_pe.edge[i][0]] = FALSE;
	}
	if( clip_pe.edge[i][1] != NULL_INDEX ) {
	    clipped_away[clip_pe.edge[i][1]] = FALSE;
	}
    }

#ifdef PROFILE
    if( profile ) {
		printf("    translation_matrix_init, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* figure out matrix needed to unscale from cannonical view volume,
       translate by -vrp, project onto view plane, then scale and translate
       into screen coordinates.  Stick it in result_matrix */

    translation_matrix_init( translation_matrix, 0, 0, vrpz );

#ifdef PROFILE
    if( profile ) {
		printf("    2 matrix mults, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    matrix_mult( translation_matrix, inv_scale_matrix, temp_matrix );
    matrix_mult( projection_matrix, temp_matrix, result_matrix );

    /* transform everything */

#ifdef PROFILE
    if( profile ) {
		printf("    transform view_vert into projected_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < clip_pe.points; i++ ) {
	if( !clipped_away[i] ) {

	    /* copy from view_vertices into projected_vertices */

	    memcpy( projected_vertices[i], view_vertices[i], sizeof( Point ) );

	    /* transform */

	    linear_transformation( result_matrix, projected_vertices[i] );
	    wtov_transformation( wtov_matrix, projected_vertices[i] );
	}
    }

#ifdef PROFILE
    if( profile ) {
		printf("    shade and draw loop, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < clip_pe.edges; i++ ) {
        if( clip_pe.edge[i][0] != NULL_INDEX ) {

	    l[X] = view_vertices[clip_pe.edge[i][0]][X];
	    l[Y] = view_vertices[clip_pe.edge[i][0]][Y];
	    l[Z] = view_vertices[clip_pe.edge[i][0]][Z];

	    temp = ((l[X] * l[X]) >> MEXP) + ((l[Y] * l[Y]) >> MEXP) +
	           ((l[Z] * l[Z]) >> MEXP);

	    /* Get the length of l. */

            len = ftom((float)sqrt(mtof(temp)));

            if( len == 0 ) {
                len = 1;
            }

            len = -len;

            shade = tube_shade( len, TubeFirstGrad );
	    DB_draw_edge( projected_vertices, clip_pe.edge[i], shade );
	}
    }

#ifdef PROFILE
    if( profile ) {
		printf("    end of view_wire_tube(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif
}



/* Takes the tube in world coords, transforms it into the view_orientation,
   clips it and perspective transforms it, and draws its faces into the
   double buffer.  It uses depth shading.  All faces are assumed to be
   visible and not obscured by other faces. */

void view_tube( PointFace *tube, Orientation *view_orientation, Point light )
{
    int i, j;
    Point p;
    long f_size;   /* total number of faces */
    unsigned char shade = 0;  /* the final color sent to polygon scaner */
    PointFace clipped_obj;
    Window win;
    Matrix result_matrix;
    Matrix temp_matrix;
    Matrix translation_matrix;
    Point center;
    Vector l;
    long temp, len;
    Float_Vector f_temp_vect;

#ifdef PROFILE
    if( profile ) {
		printf("    initialize win, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* initalize 2d-clipping window */

    win.x0 = 0;
    win.y0 = 0;
    win.x1 = 320;
    win.y1 = 199;

#ifdef PROFILE
    if( profile ) {
		printf("    copy tube->point to view_vertices, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* copy the points from tube into view_vertices */

    memcpy( view_vertices, tube->point, sizeof( Point ) * tube->points );

#ifdef PROFILE
    if( profile ) {
		printf("    translation_matrix_init(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* figure out matrix needed to translate, rotate and scale into
       cannonical view volume. Stick it in result_matrix */

    p[X] = ftom( view_orientation->position[X] );
    p[Y] = ftom( view_orientation->position[Y] );
    p[Z] = ftom( view_orientation->position[Z] );

    translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

#ifdef PROFILE
    if( profile ) {
		printf("    rot_matrix_init(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    rot_matrix_init( rotation_matrix, view_orientation->front,
		     view_orientation->up );

#ifdef PROFILE
    if( profile ) {
		printf("    matrix_mult(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

#ifdef PROFILE
    if( profile ) {
		printf("    transform view_vert for clipping, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* transform view_vertices into cannonical view volume (for clipping) */

    for( i = 0; i < tube->points; i++ ) {
	linear_transformation1( temp_matrix, view_vertices[i] );
	scale_transformation( scale_matrix, view_vertices[i] );
    }

#ifdef PROFILE
    if( profile ) {
		printf("    copy into clipped_obj, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* copy the tubes transformed points and faces into clipped_obj */

    clipped_obj.point = view_vertices;
    clipped_obj.points = tube->points;
    clipped_obj.face = clipped_faces;
    memcpy( clipped_obj.face, tube->face, sizeof( Face ) * tube->faces );
    clipped_obj.faces = tube->faces;

#ifdef PROFILE
    if( profile ) {
		printf("    clip_obj(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* clip everything */

    clip_obj( &clipped_obj, new_hither );

#ifdef PROFILE
    if( profile ) {
		printf("    set clipped_away array, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* assume all points have been clipped away */

    memset( clipped_away, TRUE, MAX_POINTS );

    /* Intialize the array of faces. */

#ifdef PROFILE
    if( profile ) {
		printf("    copy clipped_obj's faces into f, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    f_size = 0;
    for( i = 0; i < clipped_obj.faces; i++ ) {

	/* if the face has less then 3 vertices, skip it */
	if( clipped_obj.face[i].size >= 3 ) {
	    for( j = 0; j < clipped_obj.face[i].size; j++ ) {
		f[f_size].index[j] = clipped_obj.face[i].index[j];
		clipped_away[ clipped_obj.face[i].index[j] ] = FALSE;
	    }
	    f[f_size].size = clipped_obj.face[i].size;
	    f[f_size].gradient = clipped_obj.face[i].gradient;
	    f[f_size].max_z = find_max_z( &f[f_size], clipped_obj.point );
	    f_size++;
	}
    }

#ifdef PROFILE
    if( profile ) {
		printf("    translation_matrix_init, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* figure out matrix needed to unscale from cannonical view volume,
       translate by -vrp, project onto view plane, then scale and translate
       into screen coordinates.  Stick it in result_matrix */

    translation_matrix_init( translation_matrix, 0, 0, vrpz );

#ifdef PROFILE
    if( profile ) {
		printf("    2 matrix mults, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    matrix_mult( translation_matrix, inv_scale_matrix, temp_matrix );
    matrix_mult( projection_matrix, temp_matrix, result_matrix );

    /* transform everything */

#ifdef PROFILE
    if( profile ) {
		printf("    transform view_vert into projected_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < clipped_obj.points; i++ ) {
	if( !clipped_away[i] ) {

	    /* copy from view_vertices into projectied_vertices */

	    memcpy( projected_vertices[i], view_vertices[i], sizeof( Point ) );

	    /* transform */

	    linear_transformation( result_matrix, projected_vertices[i] );
	    wtov_transformation( wtov_matrix, projected_vertices[i] );

	}
    }

#ifdef PROFILE
    if( profile ) {
		printf("    shade and draw loop, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < f_size; i++ ) {

        if( game_configuration.tube_detail_level == MEDIUM_DETAIL ) {

            center[X] = 0;
	    center[Y] = 0;
	    center[Z] = 0;

	    for( j = 0; j < f[i].size; j++ ) {
		center[X] += view_vertices[f[i].index[j]][X];
		center[Y] += view_vertices[f[i].index[j]][Y];
		center[Z] += view_vertices[f[i].index[j]][Z];
	    }

	    center[X] = center[X] / f[i].size;
	    center[Y] = center[Y] / f[i].size;
	    center[Z] = center[Z] / f[i].size;

	    l[X] = center[X];
	    l[Y] = center[Y];
	    l[Z] = center[Z];

	    temp = ((l[X] * l[X]) >> MEXP) + ((l[Y] * l[Y]) >> MEXP) +
	           ((l[Z] * l[Z]) >> MEXP);

	    /* if temp overflows use floats to find the length of l */

	    if( temp < 0 ) {

	        f_temp_vect[X] = mtof(l[X]);
	        f_temp_vect[Y] = mtof(l[Y]);
	        f_temp_vect[Z] = mtof(l[Z]);

	        len = rounding_ftom( myvector_length( f_temp_vect ) );
	    }
	    else {

	        /* Get the length of l. */

		len = ftom((float)sqrt(mtof(temp)));

		if( len == 0 ) {
		    len = 1;
		}
            }

            len = -len;

	    shade = tube_shade( len, f[i].gradient );

	    /* Now draw the i'th face */

	    fast_DB_poly_scan( &f[i], projected_vertices, &win, shade );
	}
        else if( game_configuration.tube_detail_level == HIGH_DETAIL ) {

	    for( j = 0; j < f[i].size; j++ ) {

		/* find shade for this vertex */

		l[X] = view_vertices[f[i].index[j]][X];
	        l[Y] = view_vertices[f[i].index[j]][Y];
	        l[Z] = view_vertices[f[i].index[j]][Z];

	        temp = ((l[X] * l[X]) >> MEXP) + ((l[Y] * l[Y]) >> MEXP) +
	               ((l[Z] * l[Z]) >> MEXP);

	        /* if temp overflows use floats to find the length of l */

	        if( temp < 0 ) {

	            f_temp_vect[X] = mtof(l[X]);
	            f_temp_vect[Y] = mtof(l[Y]);
	            f_temp_vect[Z] = mtof(l[Z]);

	            len = rounding_ftom( myvector_length( f_temp_vect ) );
	        }
	        else {

	            /* Get the length of l. */

		    len = ftom((float)sqrt(mtof(temp)));

		    if( len == 0 ) {
		        len = 1;
		    }
                }

                len = -len;

		projected_vertices[f[i].index[j]][3] = tube_shade( len, f[i].gradient ) << MEXP;
	    }

	    shade_DB_poly_scan( &f[i], projected_vertices, &win, shade );
	}
    }

#ifdef PROFILE
    if( profile ) {
		printf("    end of draw_tube(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif
}

/* Takes the PointFace obj and transforms it into the view_orientation,
   clips it, removes backfaces, depth sorts the faces and draws the diffuse
   shaded, light source attinuated faces into the double buffer. */

void super_view_object( PointFace *obj, Orientation *view_orientation, Point light,
                        long offsetx, long offsety, long scale )
{
    int i, j;
    Point p;
    int f_size;   /* total number of faces */
    long d = -vrpz;  /* distance from center of projection */
    long norm[3];      /* normal vector for a face */
    long e[3];        /* vector from vertex to center of projection */
    long dotprod;     /* holds the dotprod of uxv with itself */
    long l[3];        /* vector from vertex to light */
    long temp;        /* holds value of (e * e) then later (l * l) */
    long len = 0;         /* holds the lenght of a vector */
    long tempshade;   /* holds (uxv * l) = the cos of the angle between them */
    unsigned char shade = 0;  /* the final color sent to polygon scaner */
    PointFace clipped_obj;
    Window win;
    Matrix translation_matrix, temp_matrix;
    Float_Vector f_temp_vect;

#ifdef PROFILE
    if( profile ) {
	printf("    init win, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* initalize 2d-clipping window */

    win.x0 = 0;
    win.y0 = 0;
    win.x1 = 319;
    win.y1 = 199;

#ifdef PROFILE
    if( profile ) {
	printf("    copy obj->point into view_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* copy the points from tube into view_vertices */

    memcpy( view_vertices, obj->point, sizeof( Point ) * obj->points );

#ifdef PROFILE
    if( profile ) {
       printf("    setup matrices, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* transform the objects vertices into the view volume */

    p[X] = ftom( view_orientation->position[X] );
    p[Y] = ftom( view_orientation->position[Y] );
    p[Z] = ftom( view_orientation->position[Z] );

    translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

    rot_matrix_init( rotation_matrix, view_orientation->front,
		     view_orientation->up );

    matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

#ifdef PROFILE
    if( profile ) {
	printf("    transform view_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* transform view_vertices into cannonical view volume (for clipping) */

    for( i = 0; i < obj->points; i++ ) {
	linear_transformation1( temp_matrix, view_vertices[i] );
	scale_transformation( scale_matrix, view_vertices[i] );
    }

    /* copy the tubes transformed points and faces into clipped_obj */

#ifdef PROFILE
    if( profile ) {
	printf("    copy into clipped_obj, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    clipped_obj.point = view_vertices;
    clipped_obj.points = obj->points;
    clipped_obj.face = clipped_faces;
    memcpy( clipped_obj.face, obj->face, sizeof( Face ) * obj->faces );
    clipped_obj.faces = obj->faces;

#ifdef PROFILE
    if( profile ) {
	printf("    clip_obj(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* clip everything */

    clip_obj( &clipped_obj, new_hither );

#ifdef PROFILE
    if( profile ) {
	printf("    setup matrices, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* unscale all the vertices and translate by vrp */

    translation_matrix_init( translation_matrix, 0, 0, vrpz );

    matrix_mult( translation_matrix, inv_scale_matrix, temp_matrix );

#ifdef PROFILE
    if( profile ) {
	printf("    translate and inv_scale view_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < clipped_obj.points; i++ ) {
	linear_transformation( temp_matrix, view_vertices[i] );
    }

    /* Project the already rotated points onto view_plane, then do window
       to viewport mapping */

#ifdef PROFILE
    if( profile ) {
	printf("    project and wtov projected_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < clipped_obj.points; i++ ) {
	memcpy( projected_vertices[i], view_vertices[i], sizeof( Point ) );
	projection_transformation( projection_matrix, projected_vertices[i] );
	wtov_transformation( wtov_matrix, projected_vertices[i] );
    }

    /* Intialize the array of faces. */

#ifdef PROFILE
    if( profile ) {
	printf("    copy front_faces into f, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    f_size = 0;
    for( i = 0; i < clipped_obj.faces; i++ ) {

	/* copy front facing, non-empty faces into the f array */

	if( clipped_obj.face[i].size > 2 ) {

	    /* Figure out whether the i'th face is back-facing or not. */

	    /* e is the vector from the first vertex in the face to the
	       center of projection. (e stands for eye) */

	    e[X] = -(( view_vertices[clipped_obj.face[i].index[0]][X] ));
	    e[Y] = -(( view_vertices[clipped_obj.face[i].index[0]][Y] ));
	    e[Z] = -d - (( view_vertices[clipped_obj.face[i].index[0]][Z] ));

	    /* Now we need to normalize e by dividing it by its length. */

	    /* Take the dot product of e with itself. */

	    temp = ((e[X] * e[X]) >> MEXP) + ((e[Y] * e[Y]) >> MEXP) +
		   ((e[Z] * e[Z]) >> MEXP);

	    /* Now take the sqare root of that dot product, giving us the length
	       of e. */

	    /* if temp overflows do the computation using floats */

	    if( temp < 0 ) {
	        temp = rounding_ftom( (mtof(e[X]) * mtof(e[X])) + (mtof(e[Y]) * mtof(e[Y])) + (mtof(e[Z]) * mtof(e[Z])) );
	    }

	    len = ftom((float)sqrt(mtof(temp)));

	    /* Now divide e by its length to normalize it. (Make its length 1) */

	    if( len == 0 ) {
		len = 1;
	    }

	    e[X] = (e[X] << MEXP) / len;
	    e[Y] = (e[Y] << MEXP) / len;
	    e[Z] = (e[Z] << MEXP) / len;


	    /* Get the pre-computed normal for the face. */

	    norm[X] = clipped_obj.face[i].normal[X];
	    norm[Y] = clipped_obj.face[i].normal[Y];
	    norm[Z] = clipped_obj.face[i].normal[Z];

	    /* Rotate it so it's pointing in the right direction. */

	    rotation_transformation( rotation_matrix, norm );

	    /* Now take the dot product of e and the normal vector, which
	       gives you the cosine of the angle between them. */

	    dotprod = ((norm[X] * e[X]) >> MEXP) + ((norm[Y] * e[Y]) >> MEXP) +
		      ((norm[Z] * e[Z]) >> MEXP);

	    /* If the i'th face is front facing then stick it in the f array,
	       otherwise don't. */

	    if( dotprod >= 0 ) {

		for( j = 0; j < clipped_obj.face[i].size; j++ ) {
		    f[f_size].index[j] = clipped_obj.face[i].index[j];
		}
		f[f_size].size = clipped_obj.face[i].size;
		f[f_size].max_z = find_max_z( &f[f_size], clipped_obj.point );
		f[f_size].gradient = clipped_obj.face[i].gradient;
		f[f_size].normal[X] = norm[X];
		f[f_size].normal[Y] = norm[Y];
		f[f_size].normal[Z] = norm[Z];
		f[f_size].transparent = clipped_obj.face[i].transparent;
		f_size++;
	    }
	}
    }

#ifdef PROFILE
    if( profile ) {
	printf("    sort the faces, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* sort the array according to maxz (far...close) */

    qsort( (void *)f, (size_t)f_size, sizeof( Face ), cmp );

#ifdef PROFILE
    if( profile ) {
	printf("    shade and draw loop, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < f_size; i++ ) {

            if( f[i].transparent ) {

	        DB_transparent_poly_scan( &f[i], projected_vertices, &win, shade );

	    }
	    else {

	        norm[X] = f[i].normal[X];
	        norm[Y] = f[i].normal[Y];
	        norm[Z] = f[i].normal[Z];

	        for( j = 0; j < f[i].size; j++ ) {

		    /* figure out the shade of this vertex */

		    /* l is a vector that points from this vertex to the
		       light source. We need to normalize it. */

		    l[X] = light[X] - view_vertices[f[i].index[j]][X];
		    l[Y] = light[Y] - view_vertices[f[i].index[j]][Y];
		    l[Z] = light[Z] - view_vertices[f[i].index[j]][Z];

		    /* Dot l with itself. */

		    temp = ((l[X] * l[X]) >> MEXP) + ((l[Y] * l[Y]) >> MEXP) +
		           ((l[Z] * l[Z]) >> MEXP);

		    /* if temp overflows use floating point arithmitic to normalize l */

		    if( temp < 0 ) {
	                f_temp_vect[X] = mtof(l[X]);
	                f_temp_vect[Y] = mtof(l[Y]);
	                f_temp_vect[Z] = mtof(l[Z]);

	                normalize( f_temp_vect );

	                l[X] = rounding_ftom( f_temp_vect[X] );
	                l[Y] = rounding_ftom( f_temp_vect[Y] );
	                l[Z] = rounding_ftom( f_temp_vect[Z] );
	            }
	            else {

		        /* Get the length of l. */

		        len = ftom((float)sqrt(mtof(temp)));

		        /* Normalize l. */

		        if( len == 0 ) {
		            len = 1;
		        }

		        l[X] = (l[X] << MEXP) / len;
		        l[Y] = (l[Y] << MEXP) / len;
		        l[Z] = (l[Z] << MEXP) / len;
                    }

		    /* Take the dot product of norm and l.  Giving us the cosine of
		       the angle between them */

		    tempshade = ((norm[X]*l[X]) >> MEXP) + ((norm[Y]*l[Y]) >> MEXP) +
		      	        ((norm[Z]*l[Z]) >> MEXP);

		    if( tempshade < 0 ) {
		        tempshade = 0;
		    }
		    if( tempshade > 1024 ) {
		        tempshade = 1024;
		    }

		    /* use the shade_lookup table to figure out the actual color
		       to give this vertex */

                    projected_vertices[f[i].index[j]][3] = vehicle_menu_distance_diffuse_shade( tempshade, len, f[i].gradient ) << MEXP;
	        }

	        shade_DB_poly_scan( &f[i], projected_vertices, &win, shade );
	    }
	}

#ifdef PROFILE
    if( profile ) {
	printf("    end of draw_object(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif
}

/* Takes the PointFace obj and transforms it into the view_orientation,
   clips it, removes backfaces, depth sorts the faces and draws the diffuse
   shaded, light source attinuated faces into the double buffer.
   Also takes arguments offsetx, and offsety and scale. */

void view_object( PointFace *obj, Orientation *view_orientation, Point light )
{
    int i, j;
    Point p;
    int f_size;   /* total number of faces */
    long d = -vrpz;  /* distance from center of projection */
    long norm[3];      /* normal vector for a face */
    long e[3];        /* vector from vertex to center of projection */
    long dotprod;     /* holds the dotprod of uxv with itself */
    long l[3];        /* vector from vertex to light */
    long temp;        /* holds value of (e * e) then later (l * l) */
    long len = 0;         /* holds the lenght of a vector */
    long tempshade;   /* holds (uxv * l) = the cos of the angle between them */
    unsigned char shade = 0;  /* the final color sent to polygon scaner */
    Point center;
    PointFace clipped_obj;
    Window win;
    Matrix translation_matrix, temp_matrix;
    Float_Vector f_temp_vect;

#ifdef PROFILE
    if( profile ) {
	printf("    init win, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* initalize 2d-clipping window */

    win.x0 = 0;
    win.y0 = 0;
    win.x1 = 319;
    win.y1 = 199;

#ifdef PROFILE
    if( profile ) {
	printf("    copy obj->point into view_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* copy the points from tube into view_vertices */

    memcpy( view_vertices, obj->point, sizeof( Point ) * obj->points );

#ifdef PROFILE
    if( profile ) {
       printf("    setup matrices, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* transform the objects vertices into the view volume */

    p[X] = ftom( view_orientation->position[X] );
    p[Y] = ftom( view_orientation->position[Y] );
    p[Z] = ftom( view_orientation->position[Z] );

    translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

    rot_matrix_init( rotation_matrix, view_orientation->front,
		     view_orientation->up );

    matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

#ifdef PROFILE
    if( profile ) {
	printf("    transform view_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* transform view_vertices into cannonical view volume (for clipping) */

    for( i = 0; i < obj->points; i++ ) {
	linear_transformation1( temp_matrix, view_vertices[i] );
	scale_transformation( scale_matrix, view_vertices[i] );
    }

    /* copy the tubes transformed points and faces into clipped_obj */

#ifdef PROFILE
    if( profile ) {
	printf("    copy into clipped_obj, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    clipped_obj.point = view_vertices;
    clipped_obj.points = obj->points;
    clipped_obj.face = clipped_faces;
    memcpy( clipped_obj.face, obj->face, sizeof( Face ) * obj->faces );
    clipped_obj.faces = obj->faces;

#ifdef PROFILE
    if( profile ) {
	printf("    clip_obj(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* clip everything */

    clip_obj( &clipped_obj, new_hither );

#ifdef PROFILE
    if( profile ) {
	printf("    setup matrices, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* unscale all the vertices and translate by vrp */

    translation_matrix_init( translation_matrix, 0, 0, vrpz );

    matrix_mult( translation_matrix, inv_scale_matrix, temp_matrix );

#ifdef PROFILE
    if( profile ) {
	printf("    translate and inv_scale view_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < clipped_obj.points; i++ ) {
	linear_transformation( temp_matrix, view_vertices[i] );
    }

    /* Project the already rotated points onto view_plane, then do window
       to viewport mapping */

#ifdef PROFILE
    if( profile ) {
	printf("    project and wtov projected_vert, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < clipped_obj.points; i++ ) {
	memcpy( projected_vertices[i], view_vertices[i], sizeof( Point ) );
	projection_transformation( projection_matrix, projected_vertices[i] );
	wtov_transformation( wtov_matrix, projected_vertices[i] );
    }

    /* Intialize the array of faces. */

#ifdef PROFILE
    if( profile ) {
	printf("    copy front_faces into f, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    f_size = 0;
    for( i = 0; i < clipped_obj.faces; i++ ) {

	/* copy front facing, non-empty faces into the f array */

	if( clipped_obj.face[i].size > 2 ) {

	    /* Figure out whether the i'th face is back-facing or not. */

	    /* e is the vector from the first vertex in the face to the
	       center of projection. (e stands for eye) */

	    e[X] = -(( view_vertices[clipped_obj.face[i].index[0]][X] ));
	    e[Y] = -(( view_vertices[clipped_obj.face[i].index[0]][Y] ));
	    e[Z] = -d - (( view_vertices[clipped_obj.face[i].index[0]][Z] ));

	    /* Now we need to normalize e by dividing it by its length. */

	    /* Take the dot product of e with itself. */

	    temp = ((e[X] * e[X]) >> MEXP) + ((e[Y] * e[Y]) >> MEXP) +
		   ((e[Z] * e[Z]) >> MEXP);

	    /* Now take the sqare root of that dot product, giving us the length
	       of e. */

	    /* if temp overflows do the computation using floats */

	    if( temp < 0 ) {
	        temp = rounding_ftom( (mtof(e[X]) * mtof(e[X])) + (mtof(e[Y]) * mtof(e[Y])) + (mtof(e[Z]) * mtof(e[Z])) );
	    }

	    len = ftom((float)sqrt(mtof(temp)));

	    /* Now divide e by its length to normalize it. (Make its length 1) */

	    if( len == 0 ) {
		len = 1;
	    }

	    e[X] = (e[X] << MEXP) / len;
	    e[Y] = (e[Y] << MEXP) / len;
	    e[Z] = (e[Z] << MEXP) / len;


	    /* Get the pre-computed normal for the face. */

	    norm[X] = clipped_obj.face[i].normal[X];
	    norm[Y] = clipped_obj.face[i].normal[Y];
	    norm[Z] = clipped_obj.face[i].normal[Z];

	    /* Rotate it so it's pointing in the right direction. */

	    rotation_transformation( rotation_matrix, norm );

	    /* Now take the dot product of e and the normal vector, which
	       gives you the cosine of the angle between them. */

	    dotprod = ((norm[X] * e[X]) >> MEXP) + ((norm[Y] * e[Y]) >> MEXP) +
		      ((norm[Z] * e[Z]) >> MEXP);

	    /* If the i'th face is front facing then stick it in the f array,
	       otherwise don't. */

	    if( dotprod >= 0 ) {
		for( j = 0; j < clipped_obj.face[i].size; j++ ) {
		    f[f_size].index[j] = clipped_obj.face[i].index[j];
		}
		f[f_size].size = clipped_obj.face[i].size;
		f[f_size].max_z = find_max_z( &f[f_size], clipped_obj.point );
		f[f_size].gradient = clipped_obj.face[i].gradient;
		f[f_size].normal[X] = norm[X];
		f[f_size].normal[Y] = norm[Y];
		f[f_size].normal[Z] = norm[Z];
		f[f_size].transparent = clipped_obj.face[i].transparent;
		f_size++;
	    }
	}
    }

#ifdef PROFILE
    if( profile ) {
	printf("    sort the faces, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* sort the array according to maxz (far...close) */

    qsort( (void *)f, (size_t)f_size, sizeof( Face ), cmp );

#ifdef PROFILE
    if( profile ) {
	printf("    shade and draw loop, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < f_size; i++ ) {

	if( game_configuration.world_detail_level == LOW_DETAIL ) {

	    /* Figure out shading for the i'th face. */

	    if( f[i].transparent ) {

	        DB_transparent_poly_scan( &f[i], projected_vertices, &win, shade );

	    }
	    else {

	        /* Get the normal vector */

	        norm[X] = f[i].normal[X];
	        norm[Y] = f[i].normal[Y];
	        norm[Z] = f[i].normal[Z];

	        center[X] = 0;
	        center[Y] = 0;
	        center[Z] = 0;

	        for( j = 0; j < f[i].size; j++ ) {
		    center[X] += view_vertices[f[i].index[j]][X];
		    center[Y] += view_vertices[f[i].index[j]][Y];
		    center[Z] += view_vertices[f[i].index[j]][Z];
	        }

	        assert( f[i].size != 0 );

	        center[X] = center[X] / f[i].size;
	        center[Y] = center[Y] / f[i].size;
	        center[Z] = center[Z] / f[i].size;

	        /* l is a vector that points from the center of the polygon to the
	           light source. We need to normalize it. */

	        l[X] = light[X] - center[X];
	        l[Y] = light[Y] - center[Y];
	        l[Z] = light[Z] - center[Z];

	        /* Dot l with itself. */

	        temp = ((l[X] * l[X]) >> MEXP) + ((l[Y] * l[Y]) >> MEXP) +
		       ((l[Z] * l[Z]) >> MEXP);

                /* if temp overflows use floating point arithmitic to normalize l */

                if( temp < 0 ) {
	            f_temp_vect[X] = mtof(l[X]);
	            f_temp_vect[Y] = mtof(l[Y]);
	            f_temp_vect[Z] = mtof(l[Z]);

	            normalize( f_temp_vect );

	            l[X] = rounding_ftom( f_temp_vect[X] );
	            l[Y] = rounding_ftom( f_temp_vect[Y] );
	            l[Z] = rounding_ftom( f_temp_vect[Z] );
	        }
	        else {

	            /* Get the length of l. */

		    len = ftom((float)sqrt(mtof(temp)));

		    /* Normalize l. */

		    if( len == 0 ) {
		        len = 1;
		    }

		    l[X] = (l[X] << MEXP) / len;
		    l[Y] = (l[Y] << MEXP) / len;
		    l[Z] = (l[Z] << MEXP) / len;
                }

	        /* Take the dot product of norm and l.  Giving us the cosine of
	           the angle between them */

	        tempshade = ((norm[X]*l[X]) >> MEXP) + ((norm[Y]*l[Y]) >> MEXP) +
		            ((norm[Z]*l[Z]) >> MEXP);

	        if( tempshade < 0 ) {
		    tempshade = 0;
	        }
	        if( tempshade > 1024 ) {
		    tempshade = 1024;
	        }

	        /* use the shade_lookup table to figure out the actual color
	           to give this face */

	        shade = distance_diffuse_shade( tempshade, len, f[i].gradient );

	        /* Now draw the i'th face */

	        fast_DB_poly_scan( &f[i], projected_vertices, &win, shade );
	    }
	}
        else if( game_configuration.world_detail_level == SUPER_HIGH_DETAIL ||
                 game_configuration.world_detail_level == HIGH_DETAIL ||
                 game_configuration.world_detail_level == MEDIUM_DETAIL ) {

            if( f[i].transparent ) {

	        DB_transparent_poly_scan( &f[i], projected_vertices, &win, shade );

	    }
	    else {

	        norm[X] = f[i].normal[X];
	        norm[Y] = f[i].normal[Y];
	        norm[Z] = f[i].normal[Z];

	        for( j = 0; j < f[i].size; j++ ) {

		    /* figure out the shade of this vertex */

		    /* l is a vector that points from this vertex to the
		       light source. We need to normalize it. */

		    l[X] = light[X] - view_vertices[f[i].index[j]][X];
		    l[Y] = light[Y] - view_vertices[f[i].index[j]][Y];
		    l[Z] = light[Z] - view_vertices[f[i].index[j]][Z];

		    /* Dot l with itself. */

		    temp = ((l[X] * l[X]) >> MEXP) + ((l[Y] * l[Y]) >> MEXP) +
		           ((l[Z] * l[Z]) >> MEXP);

		    /* if temp overflows use floating point arithmitic to normalize l */

		    if( temp < 0 ) {
	                f_temp_vect[X] = mtof(l[X]);
	                f_temp_vect[Y] = mtof(l[Y]);
	                f_temp_vect[Z] = mtof(l[Z]);

	                normalize( f_temp_vect );

	                l[X] = rounding_ftom( f_temp_vect[X] );
	                l[Y] = rounding_ftom( f_temp_vect[Y] );
	                l[Z] = rounding_ftom( f_temp_vect[Z] );
	            }
	            else {

		        /* Get the length of l. */

		        len = ftom((float)sqrt(mtof(temp)));

		        /* Normalize l. */

		        if( len == 0 ) {
		            len = 1;
		        }

		        l[X] = (l[X] << MEXP) / len;
		        l[Y] = (l[Y] << MEXP) / len;
		        l[Z] = (l[Z] << MEXP) / len;
                    }

		    /* Take the dot product of norm and l.  Giving us the cosine of
		       the angle between them */

		    tempshade = ((norm[X]*l[X]) >> MEXP) + ((norm[Y]*l[Y]) >> MEXP) +
		      	        ((norm[Z]*l[Z]) >> MEXP);

		    if( tempshade < 0 ) {
		        tempshade = 0;
		    }
		    if( tempshade > 1024 ) {
		        tempshade = 1024;
		    }

		    /* use the shade_lookup table to figure out the actual color
		       to give this vertex */

		    projected_vertices[f[i].index[j]][3] = distance_diffuse_shade( tempshade, len, f[i].gradient ) << MEXP;

	        }
	        shade_DB_poly_scan( &f[i], projected_vertices, &win, shade );
	    }
	}
    }

#ifdef PROFILE
    if( profile ) {
	printf("    end of draw_object(), timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif
}


/* Defines the transformation which, transforms the front to the z-axis,
   and up to the -y-axis. It uses an orthoganal matrix, to transform between
   the coordinate systems. */

void rot_matrix_init( long *rotation_matrix, Float_Point front,
		      Float_Point up )
{
    Point x_axis;
    Point y_axis;
    Point z_axis;
    long temp, len;

    z_axis[X] = ftom( front[X] );
    z_axis[Y] = ftom( front[Y] );
    z_axis[Z] = ftom( front[Z] );

    y_axis[X] = ftom( -up[X] );
    y_axis[Y] = ftom( -up[Y] );
    y_axis[Z] = ftom( -up[Z] );

    /* find z_axis X y_axis, which is the x_axis */

    x_axis[X] = ((z_axis[Y] * y_axis[Z]) >> MEXP) - ((z_axis[Z] * y_axis[Y] ) >> MEXP);
    x_axis[Y] = ((z_axis[Z] * y_axis[X]) >> MEXP) - ((z_axis[X] * y_axis[Z] ) >> MEXP);
    x_axis[Z] = ((z_axis[X] * y_axis[Y]) >> MEXP) - ((z_axis[Y] * y_axis[X] ) >> MEXP);

    /* normalize x_axis just in case */

    temp = ((x_axis[X] * x_axis[X]) >> MEXP) + ((x_axis[Y] * x_axis[Y]) >> MEXP) + ((x_axis[Z] * x_axis[Z]) >> MEXP);

    len = ftom((float)sqrt(mtof(temp)));

    x_axis[X] = (x_axis[X] << MEXP) / len;
    x_axis[Y] = (x_axis[Y] << MEXP) / len;
    x_axis[Z] = (x_axis[Z] << MEXP) / len;

    /* z_axis X x_axis is the y_axis */

    y_axis[X] = ((x_axis[Y] * z_axis[Z]) >> MEXP) - ((x_axis[Z] * z_axis[Y] ) >> MEXP);
    y_axis[Y] = ((x_axis[Z] * z_axis[X]) >> MEXP) - ((x_axis[X] * z_axis[Z] ) >> MEXP);
    y_axis[Z] = ((x_axis[X] * z_axis[Y]) >> MEXP) - ((x_axis[Y] * z_axis[X] ) >> MEXP);

    /* initalize the matrix the rotation matrix */

    rotation_matrix[0] = x_axis[X];
    rotation_matrix[1] = x_axis[Y];
    rotation_matrix[2] = x_axis[Z];
    rotation_matrix[3] = 0;

    rotation_matrix[4] = y_axis[X];
    rotation_matrix[5] = y_axis[Y];
    rotation_matrix[6] = y_axis[Z];
    rotation_matrix[7] = 0;

    rotation_matrix[8] = z_axis[X];
    rotation_matrix[9] = z_axis[Y];
    rotation_matrix[10] = z_axis[Z];
    rotation_matrix[11] = 0;

    rotation_matrix[12] = 0;
    rotation_matrix[13] = 0;
    rotation_matrix[14] = 0;
    rotation_matrix[15] = MAGIC;
}

/* Defines the transformation which transforms z-axis to front, and the
   -y-axis to up. It uses an orthoganal matrix, to transform between
   the coordinate systems. Note: this matrix is just the transposition
   of the same matrix produced by rot_matrix_init. */

void trans_rot_matrix_init( long *rotation_matrix, Float_Point front,
			    Float_Point up )
{
    Point x_axis;
    Point y_axis;
    Point z_axis;
    float temp, len;
	long longTemp, longLen;

    /* normalize front_vector */

    temp = (front[X] * front[X]) +
	   (front[Y] * front[Y]) +
	   (front[Z] * front[Z]);

    len = (float)sqrt(temp);

    front[X] = front[X] / len;
    front[Y] = front[Y] / len;
    front[Z] = front[Z] / len;

    /* normalize up_vector */

    temp = (up[X] * up[X]) +
	   (up[Y] * up[Y]) +
	   (up[Z] * up[Z]);

    len = (float)sqrt(temp);

    up[X] = up[X] / len;
    up[Y] = up[Y] / len;
    up[Z] = up[Z] / len;

    z_axis[X] = ftom( front[X] );
    z_axis[Y] = ftom( front[Y] );
    z_axis[Z] = ftom( front[Z] );

    y_axis[X] = ftom( -up[X] );
    y_axis[Y] = ftom( -up[Y] );
    y_axis[Z] = ftom( -up[Z] );

    /* find z_axis X y_axis, which is the x_axis */

    x_axis[X] = ((z_axis[Y] * y_axis[Z]) >> MEXP) - ((z_axis[Z] * y_axis[Y] ) >> MEXP);
    x_axis[Y] = ((z_axis[Z] * y_axis[X]) >> MEXP) - ((z_axis[X] * y_axis[Z] ) >> MEXP);
    x_axis[Z] = ((z_axis[X] * y_axis[Y]) >> MEXP) - ((z_axis[Y] * y_axis[X] ) >> MEXP);

    /* normalize x_axis just in case */

    longTemp = ((x_axis[X] * x_axis[X]) >> MEXP) +
	   ((x_axis[Y] * x_axis[Y]) >> MEXP) +
	   ((x_axis[Z] * x_axis[Z]) >> MEXP);

    longLen = ftom((float)sqrt(mtof(longTemp)));

    x_axis[X] = (x_axis[X] << MEXP) / longLen;
    x_axis[Y] = (x_axis[Y] << MEXP) / longLen;
    x_axis[Z] = (x_axis[Z] << MEXP) / longLen;

    /* z_axis X x_axis is the y_axis */

    y_axis[X] = ((x_axis[Y] * z_axis[Z]) >> MEXP) - ((x_axis[Z] * z_axis[Y] ) >> MEXP);
    y_axis[Y] = ((x_axis[Z] * z_axis[X]) >> MEXP) - ((x_axis[X] * z_axis[Z] ) >> MEXP);
    y_axis[Z] = ((x_axis[X] * z_axis[Y]) >> MEXP) - ((x_axis[Y] * z_axis[X] ) >> MEXP);

    /* initalize the matrix the rotation matrix */

    rotation_matrix[0] = x_axis[X];
    rotation_matrix[1] = y_axis[X];
    rotation_matrix[2] = z_axis[X];
    rotation_matrix[3] = 0;

    rotation_matrix[4] = x_axis[Y];
    rotation_matrix[5] = y_axis[Y];
    rotation_matrix[6] = z_axis[Y];
    rotation_matrix[7] = 0;

    rotation_matrix[8] = x_axis[Z];
    rotation_matrix[9] = y_axis[Z];
    rotation_matrix[10] = z_axis[Z];
    rotation_matrix[11] = 0;

    rotation_matrix[12] = 0;
    rotation_matrix[13] = 0;
    rotation_matrix[14] = 0;
    rotation_matrix[15] = MAGIC;
}

/* Same as trans_rot_matrix_init except that it expects fixed-point vectors
   rather than floating-point ones. */

void magic_trans_rot_matrix_init( long *rotation_matrix, Vector front,
				  Vector up )
{
    Point x_axis;
    Point y_axis;
    Point z_axis;
    long temp, len;

    /* normalize front_vector */

    temp = ((front[X] * front[X]) >> MEXP) +
	   ((front[Y] * front[Y]) >> MEXP) +
	   ((front[Z] * front[Z]) >> MEXP);

    len = ftom( (float)sqrt( mtof( temp ) ) );

    front[X] = (front[X] << MEXP) / len;
    front[Y] = (front[Y] << MEXP) / len;
    front[Z] = (front[Z] << MEXP) / len;

    /* normalize up_vector */

    temp = ((up[X] * up[X]) >> MEXP) +
	   ((up[Y] * up[Y]) >> MEXP) +
	   ((up[Z] * up[Z]) >> MEXP);

    len = ftom( (float)sqrt( mtof( temp ) ) );

    up[X] = (up[X] << MEXP) / len;
    up[Y] = (up[Y] << MEXP) / len;

    z_axis[X] = front[X];
    z_axis[Y] = front[Y];
    z_axis[Z] = front[Z];

    y_axis[X] = -up[X];
    y_axis[Y] = -up[Y];
    y_axis[Z] = -up[Z];

    /* find z_axis X y_axis, which is the x_axis */

    x_axis[X] = ((z_axis[Y] * y_axis[Z]) >> MEXP) - ((z_axis[Z] * y_axis[Y] ) >> MEXP);
    x_axis[Y] = ((z_axis[Z] * y_axis[X]) >> MEXP) - ((z_axis[X] * y_axis[Z] ) >> MEXP);
    x_axis[Z] = ((z_axis[X] * y_axis[Y]) >> MEXP) - ((z_axis[Y] * y_axis[X] ) >> MEXP);

    /* normalize x_axis just in case */

    temp = ((x_axis[X] * x_axis[X]) >> MEXP) +
	   ((x_axis[Y] * x_axis[Y]) >> MEXP) +
	   ((x_axis[Z] * x_axis[Z]) >> MEXP);

    len = ftom((float)sqrt(mtof(temp)));

    x_axis[X] = (x_axis[X] << MEXP) / len;
    x_axis[Y] = (x_axis[Y] << MEXP) / len;
    x_axis[Z] = (x_axis[Z] << MEXP) / len;

    /* z_axis X x_axis is the y_axis */

    y_axis[X] = ((x_axis[Y] * z_axis[Z]) >> MEXP) - ((x_axis[Z] * z_axis[Y] ) >> MEXP);
    y_axis[Y] = ((x_axis[Z] * z_axis[X]) >> MEXP) - ((x_axis[X] * z_axis[Z] ) >> MEXP);
    y_axis[Z] = ((x_axis[X] * z_axis[Y]) >> MEXP) - ((x_axis[Y] * z_axis[X] ) >> MEXP);

    /* initalize the matrix the rotation matrix */

    rotation_matrix[0] = x_axis[X];
    rotation_matrix[1] = y_axis[X];
    rotation_matrix[2] = z_axis[X];
    rotation_matrix[3] = 0;

    rotation_matrix[4] = x_axis[Y];
    rotation_matrix[5] = y_axis[Y];
    rotation_matrix[6] = z_axis[Y];
    rotation_matrix[7] = 0;

    rotation_matrix[8] = x_axis[Z];
    rotation_matrix[9] = y_axis[Z];
    rotation_matrix[10] = z_axis[Z];
    rotation_matrix[11] = 0;

    rotation_matrix[12] = 0;
    rotation_matrix[13] = 0;
    rotation_matrix[14] = 0;
    rotation_matrix[15] = MAGIC;
}

/* Initializes the scale_matrix and the inv_scale_matrix that is used to
   transform everything into the cannonical view_volume for clipping.
   See Hack Alert on line 35. */

void scale_matrix_init()
{
    long sx, sy, sz, one_over_sx, one_over_sy, one_over_sz;
    const float fudge = 0.0;

    sx = rounding_ftom( (2.0f * mtof(vrpz)) / ((mtof(xmax) - mtof(xmin) + fudge) * (mtof(vrpz) + mtof(yon))) );
    sy = rounding_ftom( (2.0f * mtof(vrpz)) / ((mtof(ymax) - mtof(ymin) + fudge) * (mtof(vrpz) + mtof(yon))) );
    sz = rounding_ftom( 1.0f / (mtof(vrpz) + mtof(yon)) );

    one_over_sx = rounding_ftom( ((mtof(xmax) - mtof(xmin) + fudge) * (mtof(vrpz) + mtof(yon))) / (2.0f * mtof(vrpz)) );
    one_over_sy = rounding_ftom( ((mtof(ymax) - mtof(ymin) + fudge) * (mtof(vrpz) + mtof(yon))) / (2.0f * mtof(vrpz)) );
    one_over_sz = rounding_ftom( mtof(vrpz) + mtof(yon) );

    scale_matrix[0] = sx;
    scale_matrix[1] = 0;
    scale_matrix[2] = 0;
    scale_matrix[3] = 0;
    scale_matrix[4] = 0;
    scale_matrix[5] = sy;
    scale_matrix[6] = 0;
    scale_matrix[7] = 0;
    scale_matrix[8] = 0;
    scale_matrix[9] = 0;
    scale_matrix[10] = sz;
    scale_matrix[11] = 0;
    scale_matrix[12] = 0;
    scale_matrix[13] = 0;
    scale_matrix[14] = 0;
    scale_matrix[15] = MAGIC;

    inv_scale_matrix[0] = one_over_sx;
    inv_scale_matrix[1] = 0;
    inv_scale_matrix[2] = 0;
    inv_scale_matrix[3] = 0;
    inv_scale_matrix[4] = 0;
    inv_scale_matrix[5] = one_over_sy;
    inv_scale_matrix[6] = 0;
    inv_scale_matrix[7] = 0;
    inv_scale_matrix[8] = 0;
    inv_scale_matrix[9] = 0;
    inv_scale_matrix[10] = one_over_sz;
    inv_scale_matrix[11] = 0;
    inv_scale_matrix[12] = 0;
    inv_scale_matrix[13] = 0;
    inv_scale_matrix[14] = 0;
    inv_scale_matrix[15] = MAGIC;
}


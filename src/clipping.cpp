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

#include "types.h"     /* important defines & typedef's */
#include "util.h"
#include "clipping.h"


/* Sutherland-Hodgeman Polygon Clipping Algorithm    */
/* Certified Bug-Free !    <grin>                    */
/* CSCI 4631            Prof: Golden G. Richard III  */

#define INSIDE 0
#define TOP 1
#define RIGHT 2
#define BOTTOM 4
#define LEFT 8
#define FRONT 16
#define BACK 32

extern level_type level; /* From omega.c...we need it for yon_clipping_plane */
 

int region( Point p, long hither)
{
    int location = INSIDE;

    if (p[X] < p[Z]) {
	location = LEFT;
    }
    else if (p[X] > -p[Z]) {
	location = RIGHT;
    }

    if (p[Y] < p[Z]) {
	location = location | BOTTOM;
    }
    else if (p[Y] > -p[Z]) {
	location = location | TOP;
    }

    if ( p[Z] > hither) {
	location = location | FRONT;
    }
    else if (p[Z] < level.yon_clipping_plane ) {
	location = location | BACK;
    }

    return location;
}

int new_region( Point p, long hither)
{
    int location = INSIDE;

    if (p[X] < p[Z]) {
	location = location | LEFT;
    }
    if (p[X] > -p[Z]) {
	location = location | RIGHT;
    }
    if (p[Y] < p[Z]) {
	location = location | BOTTOM;
    }
    if (p[Y] > -p[Z]) {
	location = location | TOP;
    }
    if ( p[Z] > hither) {
	location = location | FRONT;
    }
    if (p[Z] < level.yon_clipping_plane ) {
	location = location | BACK;
    }
    return location;
}

void old_intersect( Point p1, Point p2, int plane, long hither, Point inter )
{
    long t;

    if (plane == RIGHT) {
	t = ((p1[X] + p1[Z]) << MEXP) / ((p1[Z] - p2[Z]) - (p2[X] - p1[X]));
	inter[X] = rounding_fixed_multiply((p2[X] - p1[X]), t) + p1[X];
	inter[Y] = rounding_fixed_multiply((p2[Y] - p1[Y]), t) + p1[Y];
	inter[Z] = -inter[X];
    }
    else if (plane == LEFT) {
	t = ((p1[X] - p1[Z]) << MEXP) / ((p2[Z] - p1[Z]) - (p2[X] - p1[X]));
	inter[X] = rounding_fixed_multiply((p2[X] - p1[X]), t) + p1[X];
	inter[Y] = rounding_fixed_multiply((p2[Y] - p1[Y]), t) + p1[Y];
	inter[Z] = inter[X];
    }
    else if (plane == TOP) {
	t = ((p1[Y] + p1[Z]) << MEXP) / ((p1[Z] - p2[Z]) - (p2[Y] - p1[Y]));
	inter[X] = rounding_fixed_multiply((p2[X] - p1[X]), t) + p1[X];
	inter[Y] = rounding_fixed_multiply((p2[Y] - p1[Y]), t) + p1[Y];
	inter[Z] = -inter[Y];
    }
    else if (plane == BOTTOM) {
	t = ((p1[Y] - p1[Z]) << MEXP) / ((p2[Z] - p1[Z]) - (p2[Y] - p1[Y]));
	inter[X] = rounding_fixed_multiply((p2[X] - p1[X]), t) + p1[X];
	inter[Y] = rounding_fixed_multiply((p2[Y] - p1[Y]), t) + p1[Y];
	inter[Z] = inter[Y];
    }
    else if (plane == FRONT) {
	t = ((hither - p1[Z]) << MEXP) / (p2[Z] - p1[Z]);
	inter[X] = rounding_fixed_multiply((p2[X] - p1[X]), t) + p1[X];
	inter[Y] = rounding_fixed_multiply((p2[Y] - p1[Y]), t) + p1[Y];
	inter[Z] = hither;
    }
    else if (plane == BACK) {
	t = ((level.yon_clipping_plane  - p1[Z]) << MEXP) / (p2[Z] - p1[Z]);
	inter[X] = rounding_fixed_multiply((p2[X] - p1[X]), t) + p1[X];
	inter[Y] = rounding_fixed_multiply((p2[Y] - p1[Y]), t) + p1[Y];
	inter[Z] = level.yon_clipping_plane;
    }
}

void intersect( Point p1, Point p2, int plane, long hither, Point inter )
{
    float t;
    Float_Point fp1, fp2, finter;

    /* uses floats to give more accuracy to the intersection calculations */

    fp1[X] = mtof( p1[X] );
    fp1[Y] = mtof( p1[Y] );
    fp1[Z] = mtof( p1[Z] );

    fp2[X] = mtof( p2[X] );
    fp2[Y] = mtof( p2[Y] );
    fp2[Z] = mtof( p2[Z] );

    if (plane == RIGHT) {
	t = (fp1[X] + fp1[Z]) / ((fp1[Z] - fp2[Z]) - (fp2[X] - fp1[X]));
	finter[X] = ((fp2[X] - fp1[X]) * t) + fp1[X];
	finter[Y] = ((fp2[Y] - fp1[Y]) * t) + fp1[Y];
	finter[Z] = -finter[X];
    }
    else if (plane == LEFT) {
	t = (fp1[X] - fp1[Z]) / ((fp2[Z] - fp1[Z]) - (fp2[X] - fp1[X]));
	finter[X] = ((fp2[X] - fp1[X]) * t) + fp1[X];
	finter[Y] = ((fp2[Y] - fp1[Y]) * t) + fp1[Y];
	finter[Z] = finter[X];
    }
    else if (plane == TOP) {
	t = (fp1[Y] + fp1[Z]) / ((fp1[Z] - fp2[Z]) - (fp2[Y] - fp1[Y]));
	finter[X] = ((fp2[X] - fp1[X]) * t) + fp1[X];
	finter[Y] = ((fp2[Y] - fp1[Y]) * t) + fp1[Y];
	finter[Z] = -finter[Y];
    }
    else if (plane == BOTTOM) {
	t = (fp1[Y] - fp1[Z]) / ((fp2[Z] - fp1[Z]) - (fp2[Y] - fp1[Y]));
	finter[X] = ((fp2[X] - fp1[X]) * t) + fp1[X];
	finter[Y] = ((fp2[Y] - fp1[Y]) * t) + fp1[Y];
	finter[Z] = finter[Y];
    }
    else if (plane == FRONT) {
	t = ( mtof( hither ) - fp1[Z]) / (fp2[Z] - fp1[Z]);
	finter[X] = ((fp2[X] - fp1[X]) * t) + fp1[X];
	finter[Y] = ((fp2[Y] - fp1[Y]) * t) + fp1[Y];
	finter[Z] = mtof( hither );
    }
    else if (plane == BACK) {
	t = ( mtof( level.yon_clipping_plane ) - fp1[Z]) / (fp2[Z] - fp1[Z]);
	finter[X] = ((fp2[X] - fp1[X]) * t) + fp1[X];
	finter[Y] = ((fp2[Y] - fp1[Y]) * t) + fp1[Y];
	finter[Z] = mtof( level.yon_clipping_plane );
    }

    inter[X] = rounding_ftom( finter[X] );
    inter[Y] = rounding_ftom( finter[Y] );
    inter[Z] = rounding_ftom( finter[Z] );
}


/* void poly_clip_plane( Point poly[], Point polyout[], int n, int *len,
			 long hither, int plane) */
void poly_clip_plane( PointFace *clip_obj, Face *face, long hither, int plane )
{
    Point v1, v2, inter;
    Face new_face;
    int j;

    new_face.size = 0;

    if( face->size > 0 ) {

	v1[X] = clip_obj->point[face->index[face->size - 1]][X];
	v1[Y] = clip_obj->point[face->index[face->size - 1]][Y];
	v1[Z] = clip_obj->point[face->index[face->size - 1]][Z];

	for ( j = 0; j < face->size; j++ ) {

	    /* vector_copy(&v2, poly[j]); */

	    v2[X] = clip_obj->point[face->index[j]][X];
	    v2[Y] = clip_obj->point[face->index[j]][Y];
	    v2[Z] = clip_obj->point[face->index[j]][Z];

	    if (! (region(v2, hither) & plane)) {

		if (! (region(v1, hither) & plane)) {

		    /* vector_copy(&polyout[(*len)++], v2); */

		    new_face.index[new_face.size] = face->index[j];
		    new_face.size++;
		}
		else {

		    intersect(v1, v2, plane, hither, inter);

		    /* vector_copy(&polyout[(*len)++], inter); */

		    /* create new vertex */

		    clip_obj->point[clip_obj->points][X] = inter[X];
		    clip_obj->point[clip_obj->points][Y] = inter[Y];
		    clip_obj->point[clip_obj->points][Z] = inter[Z];

		    new_face.index[new_face.size] = clip_obj->points;
		    new_face.size++;

		    clip_obj->points++;

		    /* vector_copy(&polyout[(*len)++], v2); */

		    new_face.index[new_face.size] = face->index[j];
		    new_face.size++;
		}
	    }
	    else {
		if (! (region(v1, hither) & plane)) {

		    intersect(v1, v2, plane, hither, inter);

		    /* vector_copy(&polyout[(*len)++], inter); */

		    /* add new vertex */

		    clip_obj->point[clip_obj->points][X] = inter[X];
		    clip_obj->point[clip_obj->points][Y] = inter[Y];
		    clip_obj->point[clip_obj->points][Z] = inter[Z];

		    new_face.index[new_face.size] = clip_obj->points;
		    new_face.size++;

		    clip_obj->points++;
		}
	    }

	    /* vector_copy(&v1, v2); */

	    v1[X] = v2[X];
	    v1[Y] = v2[Y];
	    v1[Z] = v2[Z];
	}

	/* copy new_face into *face */

	for( j = 0; j < new_face.size; j++ ) {
	    face->index[j] = new_face.index[j];
	}
	face->size = new_face.size;
    }
}

void poly_clip_3D( PointFace *clip_obj, Face *face, long hither )
{
    int outcode[MAX_FACE_SIZE], i, result0, result1;

    for( i = 0; i < face->size; i++ ) {
	outcode[i] = region( clip_obj->point[face->index[i]], hither );
    }
    result0 = outcode[0];
    result1 = outcode[0] == 0;
    for( i = 1; i < face->size; i++ ) {
	result0 = result0 & outcode[i];
	result1 = result1 & ( outcode[i] == 0 );
    }

    if( result0 != 0 ) {
	/* The entire face is clipped away */
	face->size = 0;
    }
    else if( result1 ) {
	/* The entire face is inside the clipping region, therefore it
	   doesnt need to be clipped. */
    }
    else {
	/* Must clip :( */

	poly_clip_plane( clip_obj, face, hither, BACK);
	poly_clip_plane( clip_obj, face, hither, FRONT);
	poly_clip_plane( clip_obj, face, hither, RIGHT);
	poly_clip_plane( clip_obj, face, hither, LEFT);
	poly_clip_plane( clip_obj, face, hither, TOP);
	poly_clip_plane( clip_obj, face, hither, BOTTOM);
    }
}

void clip_obj( PointFace *clip_obj, long hither )
{
    int i;

    for( i = 0; i < clip_obj->faces; i++ ) {
	poly_clip_3D( clip_obj, &(clip_obj->face[i]), hither );
    }
}

void edge_clip_3D( PointEdge *pe, Edge e, long hither )
{
    long accept, done;
    long outcode0, outcode1, outcodeOut;
    Edge out_edge;
    Point inter;

    out_edge[0] = e[0];
    out_edge[1] = e[1];

    accept = FALSE;
    done = FALSE;

    outcode0 = new_region( pe->point[out_edge[0]], hither );
    outcode1 = new_region( pe->point[out_edge[1]], hither );

    do {
	if( outcode0 == 0 && outcode1 == 0 ) {

	    /* trivial accept */

	    accept = TRUE;
	    done = TRUE;

	}
	else if( (outcode0 & outcode1) != 0 ) {

	    /* trivial reject */

	    done = TRUE;
	}
	else {
	    if( outcode0 != 0 ) {
		outcodeOut = outcode0;
	    }
	    else {
		outcodeOut = outcode1;
	    }

	    if( outcodeOut & TOP ) {

		/* find intersection against top plane */

		intersect( pe->point[out_edge[0]], pe->point[out_edge[1]],
			   TOP, hither, inter );

	    }
	    else if( outcodeOut & RIGHT ) {

		/* find intersection against right plane */

		intersect( pe->point[out_edge[0]], pe->point[out_edge[1]],
			   RIGHT, hither, inter );

	    }
	    else if( outcodeOut & BOTTOM ) {

		/* find intersection against bottom plane */

		intersect( pe->point[out_edge[0]], pe->point[out_edge[1]],
			   BOTTOM, hither, inter );

	    }
	    else if( outcodeOut & LEFT ) {

		/* find intersection against left plane */

		intersect( pe->point[out_edge[0]], pe->point[out_edge[1]],
			   LEFT, hither, inter );

	    }
	    else if( outcodeOut & FRONT ) {

		/* find intersection against front plane */

		intersect( pe->point[out_edge[0]], pe->point[out_edge[1]],
			   FRONT, hither, inter );

	    }
	    else if( outcodeOut & BACK ) {

		/* find intersection against back plane */

		intersect( pe->point[out_edge[0]], pe->point[out_edge[1]],
			   BACK, hither, inter );

	    }

	    if( outcodeOut == outcode0 ) {

		/* add inter to array of points */

		pe->point[pe->points][X] = inter[X];
		pe->point[pe->points][Y] = inter[Y];
		pe->point[pe->points][Z] = inter[Z];
		pe->points++;

		/* change endpoint of out_edge */

		out_edge[0] = (pe->points) - 1;

		/* compute new outcode */

		outcode0 = new_region( inter, hither );

	    }
	    else {

		/* add inter to array of points */

		pe->point[pe->points][X] = inter[X];
		pe->point[pe->points][Y] = inter[Y];
		pe->point[pe->points][Z] = inter[Z];
		pe->points++;

		/* change endpoint of out_edge */

		out_edge[1] = (pe->points) - 1;

		/* compute new outcode */

		outcode1 = new_region( inter, hither );

	    }
	}
    }
    while( !done );

    if( accept ) {

	/* if this edge is accepted copy it into e */

	e[0] = out_edge[0];
	e[1] = out_edge[1];
    }
    else {
	/* if this edge is rejected make e the null edge */

	e[0] = (long)NULL_INDEX;
	e[1] = (long)NULL_INDEX;
    }
}

void clip_edges( PointEdge *pe, long hither )
{
    long i;

    for( i = 0; i < pe->edges; i++ ) {
	edge_clip_3D( pe, pe->edge[i], hither );
    }
}

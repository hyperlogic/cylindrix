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
#include "ai_util.h"
#include "collide.h"
#include "ai.h"
#include <math.h>
#include <stdio.h>  /* need sprintf() */

#define MAGIC_CYLINDER_CENTER 32256
#define MAGIC_CYLINDER_WIDTH 64512

extern char temp_str[30];
extern player_events_type player_events[6];
extern WorldStuff world_stuff; /* From omega.c */

/* Returns the array index of the player nearest to the player
   whose index is passed to the function. Returns 255 if no
   enemies exist. */

unsigned char Find_Nearest_Enemy_2D( Player player[], unsigned char index )
{
    float min_distance, distance;
    Float_Point2d yourpos, enemypos;
    unsigned char i, min_index;	
    team_type your_team;

    min_distance = 2000.0;
    min_index = 255;

    Cylinder_x_y( player[index].tank.orient.position, yourpos );

    your_team = player[index].team;

    for( i = 0; i < 6; i++ ) {
        if( player[i].team != your_team && player[i].tank.alive ) {
            Cylinder_x_y( player[i].tank.orient.position, enemypos );
            distance = Point_Distance_2D( enemypos, yourpos );
            if( distance < min_distance ) {
                min_distance = distance;
                min_index = i;
            }
        }
    }
    return min_index;
}

/* Returns the index of the nearest visible enemy of the player whose index
   is passed. Returns 255 if no enemies are visible or no enemys exist.
   Note : uses the Is_Visible_2D() function. */

unsigned char Find_Nearest_Visible_Enemy_2D( Player player[],
                unsigned char index, Pylons *pylons )
{
    float min_distance, distance;
    Float_Point2d yourpos, enemypos;
    unsigned char i, min_index;	
    team_type your_team;

    min_distance = 2000.0;
    min_index = 255;

    Cylinder_x_y( player[index].tank.orient.position, yourpos );

    your_team = player[index].team;

    for( i = 0; i < 6; i++ ) {
        if( player[i].team != your_team && player[i].tank.alive ) {
            Cylinder_x_y( player[i].tank.orient.position, enemypos );
            distance = Point_Distance_2D( enemypos, yourpos );
            if( Is_Visible_2D( enemypos, yourpos, pylons ) ) {
                if( distance < min_distance ) {
                    min_distance = distance;
                    min_index = i;
                }
            }
        }
    }
    return min_index;
}

/* Returns the indes of the nearest visible enemy that is in the players
   field of view.  Returns 255 if no enemies are found. Note : uses the
   Is_Visible_2D() function. */

unsigned char Find_Nearest_Visible_FOV_Enemy_2d( Player player[],
                unsigned char index, Pylons *pylons )
{
    float min_distance, distance;
    Float_Point2d yourpos, enemypos;
    unsigned char i, min_index;
    team_type your_team;

    min_distance = 2000.0;
    min_index = 255;

    Cylinder_x_y( player[index].tank.orient.position, yourpos );

    your_team = player[index].team;

    for( i = 0; i < 6; i++ ) {
        if( player[i].team != your_team && player[i].tank.alive ) {
            Cylinder_x_y( player[i].tank.orient.position, enemypos );
            distance = Point_Distance_2D( enemypos, yourpos );
            if( point_visible( &(player[index].tank.orient), player[i].tank.orient.position ) ) {
                if( Is_Visible_2D( enemypos, yourpos, pylons ) ) {
                    if( distance < min_distance ) {
                        min_distance = distance;
                        min_index = i;
                    }
                }
            }
        }
    }
    return min_index;
}

/* Returns the angle (in degrees) between point and the player whose
   orientation structure is passed.  The angle is figured out in 2d as
   if the surface of the cylinder is a plane. */

short Point_Angle_2D( Float_Point2d point, Orientation *o )
{
    Point u_point3d, position;
    Point2d u_point2d, position2d, u, v, v_point2d;
    long dx, dy, len, result, u_theta, v_theta;
    float temp;

    position[X] = rounding_ftom( o->position[X] );
    position[Y] = rounding_ftom( o->position[Y] );
    position[Z] = rounding_ftom( o->position[Z] );

    /* find the u_vector (2d_front vector) */

    /* get the 3d u_point */

    u_point3d[X] = rounding_ftom( o->front[X] ) + position[X];
    u_point3d[Y] = rounding_ftom( o->front[Y] ) + position[Y];
    u_point3d[Z] = rounding_ftom( o->front[Z] ) + position[Z];

    iCylinder_x_y( u_point3d, u_point2d );
    iCylinder_x_y( position, position2d );

    /* fix the x wrap around problem */

    dx = u_point2d[X] - position2d[X];

    if( dx > MAGIC_CYLINDER_CENTER ) {
        u_point2d[X] -= MAGIC_CYLINDER_WIDTH;
    }
    else if( dx < -MAGIC_CYLINDER_CENTER ) {
        u_point2d[X] += MAGIC_CYLINDER_WIDTH;
    }

    dx = u_point2d[X] - position2d[X];
    dy = u_point2d[Y] - position2d[Y];

    /* find the len of the u vector */

    temp = mtof( ((dx * dx) >> MEXP) + ((dy * dy) >> MEXP) );

    if( temp >= 0.0 ) {
        len = rounding_ftom( (float)sqrt( temp ) );
    }
    else {
        len = rounding_ftom( (float)sqrt( (mtof(dx) * mtof(dx)) + (mtof(dy) * mtof(dy)) ) );
    }

    if( len == 0 ) {
        len = 1;
    }

    u[X] = (dx << MEXP) / len;
    u[Y] = (dy << MEXP) / len;

    /* find the v vector (vector from position to point) */

    v_point2d[X] = rounding_ftom( point[X] );
    v_point2d[Y] = rounding_ftom( point[Y] );

    dx = v_point2d[X] - position2d[X];

    /* fix the x wrap around */

    if( dx > MAGIC_CYLINDER_CENTER ) {
        v_point2d[X] -= MAGIC_CYLINDER_WIDTH;
    }
    else if( dx < -MAGIC_CYLINDER_CENTER ) {
        v_point2d[X] += MAGIC_CYLINDER_WIDTH;
    }

    dx = v_point2d[X] - position2d[X];
    dy = v_point2d[Y] - position2d[Y];

    temp = mtof(((dx * dx) >> MEXP) + ((dy * dy) >> MEXP) );

    if( temp >= 0.0 ) {
        len = rounding_ftom( (float)sqrt( temp ) );
    }
    else {
        len = rounding_ftom( (float)sqrt( (mtof(dx) * mtof(dx)) + (mtof(dy) * mtof(dy)) ) );
    }

    if( len == 0 ) {
        len = 1;
    }

    v[X] = (dx << MEXP) / len;
    v[Y] = (dy << MEXP) / len;

    if( u[Y] >= 0 ) {

        /* make sure that u[X] is in -1024..1024, to avoid iarccos domain
           error */

        if( u[X] > 1024 ) {
            u[X] = 1024;
        }
        else if( u[X] < -1024 ) {
            u[X] = -1024;
        }

        u_theta = (iarccos( u[X] << 3 ) >> 3);
    }
    else {

        /* make sure that u[X] is in -1024..1024, to avoid iarccos domain
           error */

        if( u[X] > 1024 ) {
            u[X] = 1024;
        }
        else if( u[X] < -1024 ) {
            u[X] = -1024;
        }

        u_theta = 6434 - (iarccos( u[X] << 3 ) >> 3);
    }

    if( v[Y] >= 0 ) {

        /* make sure that v[X] is in -1024..1024, to avoid iarccos domain
           error */

        if( v[X] > 1024 ) {
            v[X] = 1024;
        }
        else if( v[X] < -1024 ) {
            v[X] = -1024;
        }

        v_theta = (iarccos( v[X] << 3 ) >> 3);
    }
    else {

        /* make sure that v[X] is in -1024..1024, to avoid iarccos domain
           error */

        if( v[X] > 1024 ) {
            v[X] = 1024;
        }
        else if( v[X] < -1024 ) {
            v[X] = -1024;
        }

        v_theta = 6434 - (iarccos( v[X] << 3 ) >> 3);
    }

    result = v_theta - u_theta;

    if( result < 0 ) {
        return (short)rounding_fixed_to_long((((result + 6434) * 180) << MEXP) / 3216);
    }
    else {
        return (short)rounding_fixed_to_long(((180 * result) << MEXP) / 3216);
    }
}

/* this function applys the same transformation that would push the xy-point
   'center' to the x-coordinate center of the cylinder, to point.  The result
   of this transformation is put in new_point. */

void center( Point2d center, Point2d point, Point2d new_point )
{
    long x_shift;

    x_shift = point[X] + MAGIC_CYLINDER_CENTER - center[X];

    if( x_shift > MAGIC_CYLINDER_WIDTH ) {
        new_point[X] = x_shift - MAGIC_CYLINDER_WIDTH;
    }
    else if( x_shift < 0 ) {
        new_point[X] = x_shift + MAGIC_CYLINDER_WIDTH;
    }
    else {
        new_point[X] = x_shift;
    }
    new_point[Y] = point[Y];
}

/* Returns the distance between a point and the players position. */

float Point_Distance_2D( Float_Point2d point, Float_Point2d position )
{
    Point2d p, q, new_p, new_q;
    long x, y;
    float temp;

    p[X] = rounding_ftom( position[X] );
    p[Y] = rounding_ftom( position[Y] );

    q[X] = rounding_ftom( point[X] );
    q[Y] = rounding_ftom( point[Y] );

    new_p[X] = MAGIC_CYLINDER_CENTER;
    new_p[Y] = p[Y];

    center( p, q, new_q );

    x = new_p[X] - new_q[X];
    y = new_p[Y] - new_q[Y];

    /* printf("dx = %.4f, dy = %.4f\n", mtof( x ), mtof( y )); */

    temp = mtof( ((x * x) >> MEXP) + ((y * y) >> MEXP) );

    if( temp >= 0.0 ) {
        return (float)sqrt( temp );
    }
    else {
        return (float)sqrt( (mtof( x ) * mtof( x )) + (mtof( y ) * mtof( y )) );
    }
}

/* currently not used */

/* Tells if the 2d ray from p1 to p2 intersects with the 2d ray from
   q1 to q2. */

int ill2( Float_Point2d p1, Float_Point2d p2, Float_Point2d q1,
          Float_Point2d q2, Float_Point2d v )
{
    float mu, delta, lambda;
    Float_Point2d v1, v2, v3, v4;

    v[X] = 0.0;
    v[Y] = 0.0;

    v1[X] = p1[X];
    v1[Y] = p1[Y];

    v2[X] = p2[X] - p1[X];
    v2[Y] = p2[Y] - p1[Y];

    v3[X] = q1[X];
    v3[Y] = q1[Y];

    v4[X] = q2[X] - q1[X];
    v4[Y] = q2[Y] - q1[Y];

    delta = ( v2[X] * v4[Y] ) - ( v2[Y] * v4[X] );

    if( delta == 0.0 ) {
        return FALSE;
    }
    else {
        mu = ( ( v3[X] - v1[X] ) * v4[Y] - ( v3[Y] - v1[Y] ) * v4[X] ) / delta;
        v[X] = v1[X] + mu * v2[X];
        v[Y] = v1[Y] + mu * v2[Y];

        if( v4[X] != 0.0 )
            lambda = ( v[X] - v3[X] ) / v4[X];
        else
            lambda = ( v[Y] - v3[Y] ) / v4[Y];

        if( mu < 0.0 || mu > 1.0 )
            return FALSE;
        else if( lambda < 0.0 || lambda > 1.0 )
            return FALSE;
        else
            return TRUE;
    }
}

/* currently not used */

/* fixed point version of ill2 */

int fixed_ill2( Point2d p1, Point2d p2, Point2d q1, Point2d q2, Point2d v )
{
    long mu, delta, lambda;
    Point2d v1, v2, v3, v4;

    v[X] = 0;
    v[Y] = 0;

    v1[X] = p1[X];
    v1[Y] = p1[Y];

    v2[X] = p2[X] - p1[X];
    v2[Y] = p2[Y] - p1[Y];

    v3[X] = q1[X];
    v3[Y] = q1[Y];

    v4[X] = q2[X] - q1[X];
    v4[Y] = q2[Y] - q1[Y];

    delta = ((v2[X] * v4[Y]) >> MEXP) - ((v2[Y] * v4[X]) >> MEXP);

    if( delta == 0 ) {
        return FALSE;
    }
    else {
        mu = (((((v3[X] - v1[X]) * v4[Y]) >> MEXP) -
               (((v3[Y] - v1[Y]) * v4[X]) >> MEXP)) << MEXP ) / delta;

        v[X] = v1[X] + ((mu * v2[X]) >> MEXP);
        v[Y] = v1[Y] + ((mu * v2[Y]) >> MEXP);

        if( v4[X] != 0 )
            lambda = ((v[X] - v3[X]) << MEXP) / v4[X];
        else
            lambda = ((v[Y] - v3[Y]) << MEXP) / v4[Y];

        if( mu < 0 || mu > 1024 )
            return FALSE;
        else if( lambda < 0 || lambda > 1024 )
            return FALSE;
        else
            return TRUE;
    }
}

/* currently not used because it's too time intensive. But I'm keeping
   it around because it such a bitch to write and I might need it again. */

/* Tells if a given point is visible (not blocked by a pylon) from the
   players position. */

int Old_Is_Visible_2D( Float_Point2d point, Float_Point2d position, Pylons *pylons )
{
    long i, pos_x;
    Point2d p1, p2, inter, p, q;
    long x_shift, min_x, max_x;

    pos_x = rounding_ftom( position[X] );

    /* move position to the x_center of the 2d grid (x = 20.0) and
       stick it in p */

    p[X] = MAGIC_CYLINDER_CENTER;
    p[Y] = rounding_ftom( position[Y] );

    /* move point the same amount and stick it in q */

    x_shift = rounding_ftom( point[X] ) + MAGIC_CYLINDER_CENTER - pos_x;

    /* wrap it around */

    if( x_shift > MAGIC_CYLINDER_WIDTH ) {
        q[X] = x_shift - MAGIC_CYLINDER_WIDTH;
    }
    else if ( x_shift < 0 ) {
        q[X] = x_shift + MAGIC_CYLINDER_WIDTH;
    }
    else {
        q[X] = x_shift;
    }
    q[Y] = rounding_ftom( point[Y] );

    if( (q[X] - p[X]) > 18432 || (q[X] - p[X]) < -18432 ) {
        return FALSE;
    }
    if( (q[Y] - p[Y]) > 18432 || (q[Y] - p[Y]) < -18432 ) {
        return FALSE;
    }

    for( i = 0; i < pylons->pylons; i++ ) {

        /* move pylons->pylon[i].box2d.min_x to the x_center of the 2d grid
           (x = CYLINDER_WIDTH/2.0) and stick it in p. */

        x_shift = ftom( pylons->pylon[i].box2d.min_x ) + MAGIC_CYLINDER_CENTER - pos_x;

        /* wrap it around */

        if( x_shift > MAGIC_CYLINDER_WIDTH ) {
            min_x = x_shift - MAGIC_CYLINDER_WIDTH;
        }
        else if( x_shift < 0 ) {
            min_x = x_shift + MAGIC_CYLINDER_WIDTH;
        }
        else {
            min_x = x_shift;
        }

        /* find the x_width of the pylon bounding box. */

        x_shift = ftom( pylons->pylon[i].box2d.max_x - pylons->pylon[i].box2d.min_x );

        max_x = min_x + x_shift;

        /* check for collision against north face of pylon */

        p1[X] = min_x;
        p1[Y] = ftom( pylons->pylon[i].box2d.max_y );

        p2[X] = max_x;
        p2[Y] = ftom( pylons->pylon[i].box2d.max_y );

        if( fixed_ill2( q, p, p1, p2, inter ) ) {
            return FALSE;
        }

        /* check against east face */

        p1[X] = max_x;
        p1[Y] = ftom( pylons->pylon[i].box2d.max_y );

        p2[X] = max_x;
        p2[Y] = ftom( pylons->pylon[i].box2d.min_y );

        if( fixed_ill2( q, p, p1, p2, inter ) ) {
            return FALSE;
        }

        /* check against south face */

        p1[X] = max_x;
        p1[Y] = ftom( pylons->pylon[i].box2d.min_y );

        p2[X] = min_x;
        p2[Y] = ftom( pylons->pylon[i].box2d.min_y );

        if( fixed_ill2( q, p, p1, p2, inter ) ) {
            return FALSE;
        }

        /* check against west face */

        p1[X] = min_x;
        p1[Y] = ftom( pylons->pylon[i].box2d.min_y );

        p2[X] = min_x;
        p2[Y] = ftom( pylons->pylon[i].box2d.max_y );

        if( fixed_ill2( q, p, p1, p2, inter ) ) {
            return FALSE;
        }

    }
    return TRUE;
}

/* Used by ray_bounding_box_intersect2d to figure out the region which p
   lies in, according to the graph shown below.

         |      |
    1001 | 1000 | 1010             numbers are in binary
         |      |
   -------------------- <- max_y
         |      |
    0001 | 0000 | 0010
         |      |
   -------------------- <- min_y
         |      |
    0101 | 0100 | 0110
         |      |

         ^      ^
  min_x /        \ max_x

*/

long CompOutCode( Float_Point2d p, BoundingBox2d *box )
{
    long outcode;

    outcode = 0;

    if( p[Y] > box->max_y ) {
        outcode += 8;
    }
    else if( p[Y] < box->min_y ) {
        outcode += 4;
    }

    if( p[X] > box->max_x ) {
        outcode += 2;
    }
    else if( p[X] < box->min_x ) {
        outcode += 1;
    }
    return outcode;
}

/* Detects whether or not a ray and a 2d bounding box intersect.  It uses
   a hacked version of Cohen-Sutherland line clipping algorithm */

long ray_bounding_box_intersect_2d( Float_Point2d start, Float_Point2d end,
                                    BoundingBox2d *box )
{
    long accept, done;
    long outcode0, outcode1, outcodeOut;
    Float_Point2d p0, p1;
    float x = 0.0, y = 0.0;

    p0[X] = start[X];
    p0[Y] = start[Y];

    p1[X] = end[X];
    p1[Y] = end[Y];

    accept = FALSE;
    done = FALSE;
    outcode0 = CompOutCode( p0, box );
    outcode1 = CompOutCode( p1, box );

    do {
        if( (outcode0 == 0) && (outcode1 == 0) ) {
            accept = TRUE;
            done = TRUE;
        }
        else if( (outcode0 & outcode1) != 0 ) {
            done = TRUE;
        }
        else {

            if( outcode0 != 0 ) {
                outcodeOut = outcode0;
            }
            else {
                outcodeOut = outcode1;
            }

            if( outcodeOut & 0x8 ) {
                x = p0[X] + (p1[X] - p0[X]) * (box->max_y - p0[Y]) / (p1[Y] - p0[Y]);
                y = box->max_y;
            }
            else if( outcodeOut & 0x4 ) {
                x = p0[X] + (p1[X] - p0[X]) * (box->min_y - p0[Y]) / (p1[Y] - p0[Y]);
                y = box->min_y;
            }
            else if( outcodeOut & 0x2 ) {
                x = box->max_x;
                y = p0[Y] + (p1[Y] - p0[Y]) * (box->max_x - p0[X]) / (p1[X] - p0[X]);
            }
            else if( outcodeOut & 0x1 ) {
                x = box->min_x;
                y = p0[Y] + (p1[Y] - p0[Y]) * (box->min_x - p0[X]) / (p1[X] - p0[X]);
            }

            if( outcodeOut == outcode0 ) {
                p0[X] = x;
                p0[Y] = y;
                outcode0 = CompOutCode( p0, box );
            }
            else {
                p1[X] = x;
                p1[Y] = y;
                outcode1 = CompOutCode( p1, box );
            }
        }
    }
    while( !done );

    return accept;
}

/* Detects if there is a pylon between position and point. Treats the surface
   of the cylinder as a 2d_plane. */

int Is_Visible_2D( Float_Point2d point, Float_Point2d position, Pylons *pylons )
{
    Float_Point2d p, q;
    float x_shift, min_x, max_x;
    BoundingBox2d box, ray_box;
    long i;


    p[X] = 31.5;
    p[Y] = position[Y];

    /* move point the same amount and stick it in q */

    x_shift = point[X] + 31.5f - position[X];

    /* wrap it around */

    if( x_shift > 63.0f ) {
        q[X] = x_shift - 63.0f;
    }
    else if ( x_shift < 0.0 ) {
        q[X] = x_shift + 63.0f;
    }
    else {
        q[X] = x_shift;
    }
    q[Y] = point[Y];

    if( (q[X] - p[X]) > 18.0 || (q[X] - p[X]) < -18.0 ) {
        return FALSE;
    }
    if( (q[Y] - p[Y]) > 18.0 || (q[Y] - p[Y]) < -18.0 ) {
        return FALSE;
    }

    if( q[X] > p[X] ) {
        ray_box.min_x = p[X];
        ray_box.max_x = q[X];
    }
    else if( q[X] < p[X] ) {
        ray_box.min_x = q[X];
        ray_box.max_x = p[X];
    }
    else {
        ray_box.min_x = q[X];
        ray_box.max_x = q[X] + 0.005f;
    }

    if( q[Y] > p[Y] ) {
        ray_box.min_y = p[Y];
        ray_box.max_y = q[Y];
    }
    else if( q[Y] < p[Y] ) {
        ray_box.min_y = q[Y];
        ray_box.max_y = p[Y];
    }
    else {
        ray_box.min_y = q[Y];
        ray_box.max_y = q[Y] + 0.005f;
    }

    for( i = 0; i < pylons->pylons; i++ ) {

        /* move pylons->pylon[i].box2d.min_x to the x_center of the 2d grid
           (x = CYLINDER_WIDTH/2.0) and stick it in p. */

        x_shift = pylons->pylon[i].box2d.min_x + 31.5f - position[X];

        /* wrap it around */

        if( x_shift > 63.0 ) {
            min_x = x_shift - 63.0f;
        }
        else if( x_shift < 0.0 ) {
            min_x = x_shift + 63.0f;
        }
        else {
            min_x = x_shift;
        }

        /* find the x_width of the pylon bounding box. */

        x_shift = pylons->pylon[i].box2d.max_x - pylons->pylon[i].box2d.min_x;

        max_x = min_x + x_shift;

        box.min_x = min_x;
        box.max_x = max_x;
        box.min_y = pylons->pylon[i].box2d.min_y;
        box.max_y = pylons->pylon[i].box2d.max_y;

        if( bounding_box_overlap_2d( &box, &ray_box ) ) {
            if( ray_bounding_box_intersect_2d( p, q, &box ) ) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/* Detects if point is visible from an orientation by checking if there is
   a pylon in between them and if the point is in the orientations field of
   view */

int Is_Visible_FOV_2D( Float_Point point, Orientation *o, Pylons *pylons )
{
    Float_Point2d target_pos, orient_pos;

    Cylinder_x_y( point, target_pos );
    Cylinder_x_y( o->position, orient_pos );

    return point_visible( o, point ) && Is_Visible_2D( target_pos, orient_pos, pylons );
}

/* fixed point version of Cylinder_x_y */

void iCylinder_x_y( Point point, Point2d point2d )
{
    long theta, radius, temp;

    /* find the radius of point w.r.t. the x-y plane */

    radius = rounding_ftom( (float)sqrt( mtof(
                 ((point[X] * point[X]) >> MEXP) +
                 ((point[Y] * point[Y]) >> MEXP) )));

    /* theta is the current angle on the cylinder */

    if( radius != 0 ) {

        if( point[Y] >= 0 ) {

            /* make sure that temp is between -1.0..1.0 to prevent an
               arccos domain error */

            temp = (point[X] << MEXP) / radius;

            if( temp > 1024 ) {
                temp = 1024;
            }
            else if( temp < -1024 ) {
                temp = -1024;
            }

            theta = iarccos( temp << 3 ) >> 3;
        }
        else {

            /* make sure that temp is between -1.0..1.0 to prevent an
               arccos domain error */

            temp = (point[X] << MEXP) / radius;

            if( temp > 1024 ) {
                temp = 1024;
            }
            else if( temp < -1024 ) {
                temp = -1024;
            }

            theta = (6434) - (iarccos( temp << 3 ) >> 3);
        }
    }
    else {
        theta = 0;
    }

    point2d[X] = (theta * 64512) / 6434;
    point2d[Y] = point[Z];
}

/* Converts a Point (3d) to a Point2d by projecting it onto the surface of
   the cylinder. */

void Cylinder_x_y( Float_Point point, Float_Point2d point2d )
{
    Point2d ipoint2d;
    long theta, radius, temp;

    ipoint2d[X] = rounding_ftom( point[X] );
    ipoint2d[Y] = rounding_ftom( point[Y] );

    /* find the radius of point w.r.t. the x-y plane */

    radius = rounding_ftom( (float)sqrt( mtof(
                 ((ipoint2d[X] * ipoint2d[X]) >> MEXP) +
                 ((ipoint2d[Y] * ipoint2d[Y]) >> MEXP) )));

    /* theta is the current angle on the cylinder */

    if( radius != 0 ) {

        if( ipoint2d[Y] >= 0 ) {

            /* make sure that temp is between -1.0..1.0 to prevent an
               arccos domain error */

            temp = (ipoint2d[X] << MEXP) / radius;

            if( temp > 1024 ) {
                temp = 1024;
            }
            else if( temp < -1024 ) {
                temp = -1024;
            }

            theta = iarccos( temp << 3 ) >> 3;
        }
        else {

            /* make sure that temp is between -1.0..1.0 to prevent an
               arccos domain error */

            temp = (ipoint2d[X] << MEXP) / radius;

            if( temp > 1024 ) {
                temp = 1024;
            }
            else if( temp < -1024 ) {
                temp = -1024;
            }

            theta = (6434) - (iarccos( temp << 3 ) >> 3);
        }
    }
    else {
        theta = 0;
    }

    point2d[X] = mtof( (theta * 64512) / 6434 );
    point2d[Y] = point[Z];
}

unsigned char Find_Nearest_Enemy_3D( Player player[], unsigned char index )
{
    float min_distance, distance;
    unsigned char i, min_index;
    team_type your_team;

    min_distance = 2000.0;
    min_index = 255;

    your_team = player[index].team;

    for( i = 0; i < 6; i++ ) {
        if( player[i].team != your_team && player[i].tank.alive ) {
            distance = Point_Distance_3D( player[i].tank.orient.position, player[index].tank.orient.position );
            if( distance < min_distance ) {
                min_distance = distance;
                min_index = i;
            }
        }
    }
    return min_index;
}

unsigned char Find_Nearest_Visible_Enemy_3D( Player player[],
                unsigned char index, Pylons *pylons )
{
    float min_distance, distance;
    unsigned char i, min_index;
    team_type your_team;

    min_distance = 2000.0;
    min_index = 255;

    your_team = player[index].team;

    for( i = 0; i < 6; i++ ) {
        if( player[i].team != your_team && player[i].tank.alive ) {
            distance = Point_Distance_3D( player[i].tank.orient.position, player[index].tank.orient.position );
            if( Is_Visible_3D( player[i].tank.orient.position, player[index].tank.orient.position, pylons ) ) {
                if( distance < min_distance ) {
                    min_distance = distance;
                    min_index = i;
                }
            }
        }
    }
    return min_index;
}

unsigned char Find_Nearest_Visible_FOV_Enemy_3D( Player player[],
                  unsigned char index, Pylons *pylons )
{
    float min_distance, distance;
    unsigned char i, min_index;
    team_type your_team;

    min_distance = 2000.0;
    min_index = 255;

    your_team = player[index].team;

    for( i = 0; i < 6; i++ ) {
        if( player[i].team != your_team && player[i].tank.alive ) {
            distance = Point_Distance_3D( player[i].tank.orient.position, player[index].tank.orient.position );
            if( point_visible( &(player[index].tank.orient), player[i].tank.orient.position ) ) {
                if( Is_Visible_3D( player[i].tank.orient.position, player[index].tank.orient.position, pylons ) ) {
                    if( distance < min_distance ) {
                        min_distance = distance;
                        min_index = i;
                    }
                }
            }
        }
    }
    return min_index;
}

/* home */

int Is_Visible_3D( Float_Point point, Float_Point pos, Pylons *pylons )
{
    BoundingBox ray_box;
    long vert[2][3];
    EdgeTable et;
    long edge[1][2];
    long dummy, i;


    /* build the EdgeTable */

    vert[0][X] = rounding_ftom( point[X] );
    vert[0][Y] = rounding_ftom( point[Y] );
    vert[0][Z] = rounding_ftom( point[Z] );

    vert[1][X] = rounding_ftom( pos[X] );
    vert[1][Y] = rounding_ftom( pos[Y] );
    vert[1][Z] = rounding_ftom( pos[Z] );

    edge[0][0] = 0;
    edge[0][1] = 1;

    et.edge = edge;
    et.edges = 1;

    /* make the bounding box surrounding point and pos */

    if( point[X] > pos[X] ) {
        ray_box.max_x = point[X];
        ray_box.min_x = pos[X];
    }
    else if( point[X] < pos[X] ) {
        ray_box.max_x = pos[X];
        ray_box.min_x = point[X];
    }
    else {
        ray_box.min_x = pos[X];
        ray_box.max_x = pos[X] + 0.005f;
    }

    if( point[Y] > pos[Y] ) {
        ray_box.max_y = point[Y];
        ray_box.min_y = pos[Y];
    }
    else if( point[Y] < pos[Y] ) {
        ray_box.max_y = pos[Y];
        ray_box.min_y = point[Y];
    }
    else {
        ray_box.min_y = pos[Y];
        ray_box.max_y = pos[Y] + 0.005f;
    }

    if( point[Z] > pos[Z] ) {
        ray_box.max_z = point[Z];
        ray_box.min_z = pos[Z];
    }
    else if( point[Z] < pos[Z] ) {
        ray_box.max_z = pos[Z];
        ray_box.min_z = point[Z];
    }
    else {
        ray_box.min_z = pos[Z];
        ray_box.max_z = pos[Z] + 0.005f;
    }

    for( i = 0; i < pylons->pylons; i++ ) {

        if( bounding_box_overlap( &ray_box, &(pylons->pylon[i].box) ) ) {
            if( pointface_edge_intersect( &(pylons->pylon[i].obj), vert, &et, &dummy ) ) {
                return FALSE;
            }
        }
    }
    return TRUE;
}   /* End of Is_Visible_3D */

int Is_Visible_FOV_3D( Float_Point point, Orientation *o, Pylons *pylons )
{
    return point_visible( o, point ) && Is_Visible_3D( point, o->position, pylons );
}

float Point_Distance_3D( Float_Point u, Float_Point v )
{
    Float_Point u_minus_v;

    u_minus_v[X] = u[X] - v[X];
    u_minus_v[Y] = u[Y] - v[Y];
    u_minus_v[Z] = u[Z] - v[Z];

    return (float)sqrt( vector_dot_product( u_minus_v, u_minus_v ) );
}

short Horizontal_Point_Angle_3D( Float_Point point, Orientation *o )
{
    Float_Point new_point, left, proj_front, proj_left, new_pos;
    float theta;
    const float epsilon = 0.005f;

    new_point[X] = point[X] - o->position[X];
    new_point[Y] = point[Y] - o->position[Y];
    new_point[Z] = point[Z] - o->position[Z];

    /* make sure that point and o->position are not the same */

    if( new_point[X] < epsilon && new_point[X] > -epsilon ) {
        if( new_point[Y] < epsilon && new_point[Y] > -epsilon ) {
            if( new_point[Z] < epsilon && new_point[Z] > -epsilon ) {
                return 0;
            }
        }
    }

    vector_cross_product( o->up, o->front, left );

    vector_project( new_point, o->front, proj_front );
    vector_project( new_point, left, proj_left );

    /* new_pos is new_point projected on the plane formed by front & left */

    new_pos[X] = proj_front[X] + proj_left[X];
    new_pos[Y] = proj_front[Y] + proj_left[Y];
    new_pos[Z] = proj_front[Z] + proj_left[Z];

    /* make sure that new_pos doesn't get projected onto the origin.
       because if it is angle_between will crash! */

    if( new_pos[X] < epsilon && new_pos[X] > -epsilon ) {
        if( new_pos[Y] < epsilon && new_pos[Y] > -epsilon ) {
            if( new_pos[Z] < epsilon && new_pos[Z] > -epsilon ) {
                return 0;
            }
        }
    }

    theta = angle_between( o->front, new_pos );

    if( vector_dot_product( left, new_pos ) < 0 ) {
        return (short)(theta * (180.0f / PI));
    }
    else {
        return (short)(360.0f - ( theta * (180.0f / PI) ));
    }
}

short Vertical_Point_Angle_3D( Float_Point point, Orientation *o )
{
    Float_Point new_point, down, proj_front, proj_down, new_pos;
    float theta;
    const float epsilon = 0.005f;

    new_point[X] = point[X] - o->position[X];
    new_point[Y] = point[Y] - o->position[Y];
    new_point[Z] = point[Z] - o->position[Z];

    /* make sure that point and o->position aren't the same */

    if( new_point[X] < epsilon && new_point[X] > -epsilon ) {
        if( new_point[Y] < epsilon && new_point[Y] > -epsilon ) {
            if( new_point[Z] < epsilon && new_point[Z] > -epsilon ) {
                return 0;
            }
        }
    }

    down[X] = -o->up[X];
    down[Y] = -o->up[Y];
    down[Z] = -o->up[Z];

    vector_project( new_point, o->front, proj_front );
    vector_project( new_point, down, proj_down );

    /* new_pos is new_point projected on the plane formed by front & left */

    new_pos[X] = proj_front[X] + proj_down[X];
    new_pos[Y] = proj_front[Y] + proj_down[Y];
    new_pos[Z] = proj_front[Z] + proj_down[Z];

    /* make sure that new_pos doesn't get projected onto the origin.
       because if it is angle_between will crash! */

    if( new_pos[X] < epsilon && new_pos[X] > -epsilon ) {
        if( new_pos[Y] < epsilon && new_pos[Y] > -epsilon ) {
            if( new_pos[Z] < epsilon && new_pos[Z] > -epsilon ) {
                return 0;
            }
        }
    }

    theta = angle_between( o->front, new_pos );

    if( vector_dot_product( down, new_pos ) < 0 ) {
        return (short)(theta * (180.0f / PI));
    }
    else {
        return (short)(360.0f - ( theta * (180.0f / PI) ));
    }
}

void Sprint_State( char *state_string, state_type state, int index )
    {
     switch( state )
         {
          case ATTACK :
              sprintf( state_string, "ATTACK %d", index );
              break;
          case GET_ENERGY :
              sprintf( state_string, "GET_ENERGY %d", index );
              break;
          case PANIC :
              sprintf( state_string, "PANIC %d", index );
              break;
          case BEZERK :
              sprintf( state_string, "BEZERK %d", index );
              break;
          case HIDE :
              sprintf( state_string, "HIDE %d", index );
              break;
          case GROUPUP :
              sprintf( state_string, "GROUPUP %d", index );
              break;
          case KILL_RADAR_BASE :
              sprintf( state_string, "KILL_RADAR_BASE %d", index );
              break;
          case PROTECT_RADAR_BASE :
              sprintf( state_string, "PROTECT_RADAR_BASE %d", index );
              break;
          case GET_PYLONS :
              sprintf( state_string, "GET_PYLONS %d", index );
              break;
          case PROTECT :
              sprintf( state_string, "PROTECT %d", index );
              break;
          case EVADE :
              sprintf( state_string, "EVADE %d", index );
              break;
         
          default :
              sprintf( state_string, "NONE %d", index );
              break;

         } /* End switch */

    } /* End sprint state */

int inside_bounding_box2d( Float_Point2d point, BoundingBox2d *box )
{
    if( point[X] < box->max_x && point[X] > box->min_x ) {
        if( point[Y] < box->max_y && point[Y] > box->min_y ) {
            return TRUE;
        }
    }
    return FALSE;
}


/* Clear out the input table so movements from the last frame
   are not repeated */
void Clear_Input_Table( input_table table )
    {
     int i;

     for( i = 0; i < INPUT_TABLE_SIZE; i++ )
         {
          table[i] = 0;
         }

    } /* End Clear_Input_Table */


/* Copy from point one into point two */
void Copy_Point_3D( Float_Point point_one, Float_Point point_two )
    {
     
     point_two[X] = point_one[X];
     point_two[Y] = point_one[Y];
     point_two[Z] = point_one[Z];
    
    } /* End of Copy_Point_3D */


void Pylon_Position_3D( Pylon *pylon, Float_Point point )
    {
     
     point[X] = (pylon->box.max_x + pylon->box.min_x) / 2;
     point[Y] = (pylon->box.max_y + pylon->box.min_y) / 2;
     point[Z] = (pylon->box.max_z + pylon->box.min_z) / 2;
     
    } /* End of Pylon_Position_3D */

void Pylon_Position_2D( Pylon *pylon, Float_Point2d point )
    {
     Float_Point temp_point;     
     
     Pylon_Position_3D( pylon, temp_point );

     Cylinder_x_y( temp_point, point );

     /*
     point[X] = (pylon->box2d.min_x + pylon->box2d.max_x) / 2;
     point[Y] = (pylon->box2d.min_y + pylon->box2d.max_y) / 2;
     */
     
    } /* End of Pylon_Position_2D */
                                

/* Given the orientation of a radar base, return the position of its leg */                                
void Radar_Leg( Orientation *o, Float_Point2d point )
    {
     Float_Vector vector;
     Float_Point point_3D;
     
     
     /* Find the left vector */
     vector_cross_product( o->front, o->up, vector );
     
     vector[X] = vector[X] * 0.5f;
     vector[Y] = vector[Y] * 0.5f;
     vector[Z] = vector[Z] * 0.5f;
     
     point_3D[X] = o->position[X] + vector[X];
     point_3D[Y] = o->position[Y] + vector[Y];
     point_3D[Z] = o->position[Z] + vector[Z];
     
     Cylinder_x_y( point_3D, point );
     
    } /* End of Radar_Leg */

/* Detects if there is a pylon between position and point. Treats the surface
   of the cylinder as a 2d_plane. */

int Is_Visible_2D_Pylon( Float_Point2d point, Float_Point2d position, Pylons *pylons, int pylon_index )
{
    Float_Point2d p, q;
    float x_shift, min_x, max_x;
    BoundingBox2d box, ray_box;
    long i;


    p[X] = 31.5;
    p[Y] = position[Y];

    /* move point the same amount and stick it in q */

    x_shift = point[X] + 31.5f - position[X];

    /* wrap it around */

    if( x_shift > 63.0f ) {
        q[X] = x_shift - 63.0f;
    }
    else if ( x_shift < 0.0f ) {
        q[X] = x_shift + 63.0f;
    }
    else {
        q[X] = x_shift;
    }
    q[Y] = point[Y];

    if( (q[X] - p[X]) > 18.0f || (q[X] - p[X]) < -18.0f ) {
        return FALSE;
    }
    if( (q[Y] - p[Y]) > 18.0f || (q[Y] - p[Y]) < -18.0f ) {
        return FALSE;
    }

    if( q[X] > p[X] ) {
        ray_box.min_x = p[X];
        ray_box.max_x = q[X];
    }
    else if( q[X] < p[X] ) {
        ray_box.min_x = q[X];
        ray_box.max_x = p[X];
    }
    else {
        ray_box.min_x = q[X];
        ray_box.max_x = q[X] + 0.005f;
    }

    if( q[Y] > p[Y] ) {
        ray_box.min_y = p[Y];
        ray_box.max_y = q[Y];
    }
    else if( q[Y] < p[Y] ) {
        ray_box.min_y = q[Y];
        ray_box.max_y = p[Y];
    }
    else {
        ray_box.min_y = q[Y];
        ray_box.max_y = q[Y] + 0.005f;
    }

    for( i = 0; i < pylons->pylons; i++ ) {
        
        if( i != pylon_index )
            {
             /* move pylons->pylon[i].box2d.min_x to the x_center of the 2d grid
             (x = CYLINDER_WIDTH/2.0) and stick it in p. */

             x_shift = pylons->pylon[i].box2d.min_x + 31.5f - position[X];

             /* wrap it around */

             if( x_shift > 63.0f ) {
                min_x = x_shift - 63.0f;
             }
             else if( x_shift < 0.0 ) {
                min_x = x_shift + 63.0f;
             }
             else {
                min_x = x_shift;
             }

             /* find the x_width of the pylon bounding box. */

             x_shift = pylons->pylon[i].box2d.max_x - pylons->pylon[i].box2d.min_x;

            max_x = min_x + x_shift;

            box.min_x = min_x;
            box.max_x = max_x;
            box.min_y = pylons->pylon[i].box2d.min_y;
            box.max_y = pylons->pylon[i].box2d.max_y;

            if( bounding_box_overlap_2d( &box, &ray_box ) ) {
                if( ray_bounding_box_intersect_2d( p, q, &box ) ) {
                    return FALSE;
                }
            }
            
           } /* End of johns if */ 
           
    } /* End for */
    
    return TRUE;
} /* End of Is_Visible_2D_Pylon */


/* Tests for visibility ignoring the pylon index'th pylon.  255 is for ignore no pylons */
int Is_Visible_3D_Pylon( Float_Point point, Float_Point pos, Pylons *pylons, int pylon_index )
{
    BoundingBox ray_box;
    long vert[2][3];
    EdgeTable et;
    long edge[1][2];
    long dummy, i;


    /* build the EdgeTable */

    vert[0][X] = rounding_ftom( point[X] );
    vert[0][Y] = rounding_ftom( point[Y] );
    vert[0][Z] = rounding_ftom( point[Z] );

    vert[1][X] = rounding_ftom( pos[X] );
    vert[1][Y] = rounding_ftom( pos[Y] );
    vert[1][Z] = rounding_ftom( pos[Z] );

    edge[0][0] = 0;
    edge[0][1] = 1;

    et.edge = edge;
    et.edges = 1;

    /* make the bounding box surrounding point and pos */

    if( point[X] > pos[X] ) {
        ray_box.max_x = point[X];
        ray_box.min_x = pos[X];
    }
    else if( point[X] < pos[X] ) {
        ray_box.max_x = pos[X];
        ray_box.min_x = point[X];
    }
    else {
        ray_box.min_x = pos[X];
        ray_box.max_x = pos[X] + 0.005f;
    }

    if( point[Y] > pos[Y] ) {
        ray_box.max_y = point[Y];
        ray_box.min_y = pos[Y];
    }
    else if( point[Y] < pos[Y] ) {
        ray_box.max_y = pos[Y];
        ray_box.min_y = point[Y];
    }
    else {
        ray_box.min_y = pos[Y];
        ray_box.max_y = pos[Y] + 0.005f;
    }

    if( point[Z] > pos[Z] ) {
        ray_box.max_z = point[Z];
        ray_box.min_z = pos[Z];
    }
    else if( point[Z] < pos[Z] ) {
        ray_box.max_z = pos[Z];
        ray_box.min_z = point[Z];
    }
    else {
        ray_box.min_z = pos[Z];
        ray_box.max_z = pos[Z] + 0.005f;
    }

    for( i = 0; i < pylons->pylons; i++ ) {

        if( i != pylon_index )
            if( bounding_box_overlap( &ray_box, &(pylons->pylon[i].box) ) ) {
                if( pointface_edge_intersect( &(pylons->pylon[i].obj), vert, &et, &dummy ) ) {
                    return FALSE;
                }
            }
    }
    return TRUE;
} /* End of Is_Visible_3D_Pylon */




/* This function tells us which direction we are moving relative to our facing */
short Current_Direction_2D( WorldStuff *world_stuff, int index )
    {
     Player *player;
     Float_Point temp_point;
     Float_Point2d temp_point_2D;
     short angle;
     
     player = &world_stuff->player_array[index];
     
     if( (player->tank.vel[X] < 0.005 && player->tank.vel[X] > -0.005 )
      && (player->tank.vel[Y] < 0.005 && player->tank.vel[Y] > -0.005 ) 
      && (player->tank.vel[Z] < 0.005 && player->tank.vel[Z] > -0.005 ) )
          return(-1);
          
     
     temp_point[X] = player->tank.orient.position[X] + player->tank.vel[X];
     temp_point[Y] = player->tank.orient.position[Y] + player->tank.vel[Y];
     temp_point[Z] = player->tank.orient.position[Z] + player->tank.vel[Z];
     
     Cylinder_x_y( temp_point, temp_point_2D );
     
     angle = Point_Angle_2D( temp_point_2D, &player->tank.orient );
     
     
     return( angle );
     
    } /* End of Current_Direction_2D */
    
    
/* Given a Float_Point2d, store the row and column of the 
   square that that point lies on should be in ai_util.c */
void Point_Square_2D( Float_Point2d point, int *row, int *column )
    {
     float dx;
     float dy;
     
     dx = (float)CYLINDER_WIDTH / (float)TUBE_COLUMNS;
     dy = (float)CYLINDER_LENGTH / (float)TUBE_ROWS;
     
     
     *row    = (int)((point[Y] + (CYLINDER_LENGTH / 2)) / dy);
     *column = (int)(point[X] / dx);
     
     if( *row == TUBE_ROWS ) {
         *row = TUBE_ROWS - 1;
     }
     
     if( *column == TUBE_COLUMNS ) {
         *column = TUBE_COLUMNS - 1;
     }

    } /* End of Point_Square_2D */


/* Given the row and column...find the center of a square */
void Center_Of_2D( int row, int column, Float_Point2d point )
    {
     float dx, dy;
     
     dx = (float)CYLINDER_WIDTH / (float)TUBE_COLUMNS;
     dy = (float)CYLINDER_LENGTH / (float)TUBE_ROWS;
    
     point[X] = (column * dx) + (dx / 2);
     point[Y] = ((row * dy)    + (dy / 2)) - (CYLINDER_LENGTH / 2);
     
    } /* End of Center_Of_2D */


/* Figure out how much damage this vehicle has taken recently */
int Recent_Damage( int index )
    {
     int damage;
     int i;
     
     damage = 0;
     
     for( i = 0; i < BULLET_THRESHOLD; i++ )
         {
          damage += player_events[index].memory.bullet_hit[i];
         }

     return(damage);
     
    } /* End of Recent_Damage */


void Update_Bullet_Hit( short projectile_strength, int tank_number, Projectile *p )
    {
     int index, enemy_index;
     player_events_type *events;
     
     
     index       = tank_number;
     enemy_index = p->owner;

     events = &player_events[index];
     
     events->memory.bullet_hit[events->memory.bullet_index] = projectile_strength;
     events->memory.bullet_owner[events->memory.bullet_index] = (short)enemy_index;

     player_events[index].memory.bullet_index++;
     player_events[index].memory.bullet_index %= BULLET_THRESHOLD;
   
    } /* End of Update_Bullet_Hit */


/* Returns 0-5 for index of ship that shot us, or 6 or 7 for
   red and blue radar bases, respectively.  -1 is no ship */
int Find_Bullet_Owner( Projectile *p )
    {
     int i;
     Projectile *bullet;


     for( i = 0; i < 6; i++ )
         {
          bullet = world_stuff.player_array[i].tank.projectile_list;

          while( bullet != NULL )
              {
               if( bullet == p )          
                   return(i);
               bullet = bullet->next;
              }
         }  /* End loop */

     bullet = world_stuff.red_radar_base.projectile_list;
     while( bullet != NULL )
         {
          if( bullet == p )          
              return(6);
          bullet = bullet->next;
         }

     bullet = world_stuff.blue_radar_base.projectile_list;
     while( bullet != NULL )
         {
          if( bullet == p )          
              return(7);
          bullet = bullet->next;
         }

     SYS_Error("Bullet owner not found ! \n"); /* Take me out in final game */

     return(-1); /* To shut up compiler */

    } /* End of Find_Bullet_Owner */



/* Given the row and column and height...find the center of a square */
void Center_Of_3D( int row, int column, int height, Float_Point point )
    {
     float dr, dtheta, drow;
     float r;


     dr   = (float)CYLINDER_RADIUS / (float)TUBE_HEIGHT;
     drow = (float)CYLINDER_LENGTH / (float)TUBE_ROWS;
     r  = 10.0f - ((dr * height) + (dr / 2.0f));
     dtheta = (2 * PI) / TUBE_COLUMNS;


     point[X] = r * (float)cos((dtheta * column) + (dtheta / 2));
     point[Y] = r * (float)sin((dtheta * column) + (dtheta / 2));
     point[Z] = (row * drow) + (drow / 2) - (CYLINDER_LENGTH / 2);

     
    } /* End of Center_Of_3D */


/* Given a Float_Point, store the row and column and height of the 
   square that that point lies on */
void Point_Square_3D( Float_Point point, int *row, int *column, int *height )
    {
     float dx;
     float dy;
     float dz;
     Float_Point2d point_2d;
     Float_Point2d v;
     float actual_height;


     dx = (float)CYLINDER_WIDTH / (float)TUBE_COLUMNS;
     dy = (float)CYLINDER_LENGTH / (float)TUBE_ROWS;
     dz = (float)CYLINDER_RADIUS / (float)TUBE_HEIGHT;
     

     Cylinder_x_y( point, point_2d );

     *row    = (int)((point_2d[Y] + (CYLINDER_LENGTH / 2.0f)) / dy);
     *column = (int)(point_2d[X] / dx);
     
     v[X] = point[X];
     v[Y] = point[Y];

     actual_height = 10.0f - (float)sqrt( (v[X] * v[X]) + (v[Y] * v[Y]) );
     
     *height = (int)((actual_height / dz) + 0.5f);


     if( *row == TUBE_ROWS ) {
         *row = TUBE_ROWS - 1;
     }
     
     if( *column == TUBE_COLUMNS ) {
         *column = TUBE_COLUMNS - 1;
     }
     
     if( *column == TUBE_HEIGHT ) {
         *column = TUBE_HEIGHT - 1;
     }


    } /* End of Point_Square_3D */


void update_leader( long view_vehicle )
    {
     long i;
     boolean blue_leader_picked = FALSE;
     boolean red_leader_picked = FALSE;


     for( i = 0; i < 6; i++ )
          if( i == view_vehicle )
              {
               world_stuff.player_array[i].leader = TRUE;
               if( world_stuff.player_array[i].tank.team == BLUE_TEAM )
                   blue_leader_picked = TRUE;
               else
                   red_leader_picked = TRUE;

              }
          else
              {
               world_stuff.player_array[i].leader = FALSE;
              }

     for( i = 0; i < 6; i++ )
         if( world_stuff.player_array[i].tank.alive )
             {
              if( !world_stuff.player_array[i].leader ) {
		      if( world_stuff.player_array[i].tank.team == RED_TEAM )
			  {
			   if( !red_leader_picked )
			       {
				red_leader_picked = TRUE;
				world_stuff.player_array[i].leader = TRUE;
			       }
			   else
			       world_stuff.player_array[i].leader = FALSE;
			  }
	      }
              else /* Team == BLUE_TEAM */
                  {
                   if( !blue_leader_picked )
                       {
                        blue_leader_picked = TRUE;
                        world_stuff.player_array[i].leader = TRUE;
                       }
                   else
                       world_stuff.player_array[i].leader = FALSE;
                  }

             } /* End if alive */
         else
              world_stuff.player_array[i].leader = FALSE;

    } /* End of update_leader() */


/* Does this vehicle have any decoys flying around? */
int Decoys_Active( WorldStuff *world_stuff, int index )
    {
     Projectile *bullet;
     int num_decoys = 0;


     bullet = world_stuff->player_array[index].tank.projectile_list;

     while( bullet != NULL )
         {
          if( bullet->type == DECOY_0 || bullet->type == DECOY_1 ||
              bullet->type == DECOY_2 )
              num_decoys++;

          bullet = bullet->next;
         }

     return(num_decoys);

    } /* End of Decoys_Active */

int Return_Vehicle_Index( Vehicle *v )
    {
     int i;
     int owner;


     for( i = 0; i < 6; i++ ) {
        if( v == &(world_stuff.player_array[i].tank) ) {
            owner = i;
            break;
        }
     }
     return(i);
    } /* End of Return_Vehicle_Index */

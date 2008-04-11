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

/* file => movement.c
   programmers => Anthony Thibault & John McCawley
*/

#include "util.h"         /* useful utilities like mtof, ftom, iarccos... */
#include "object.h"       /* need object transformation stuff */
#include "movement.h"     /* function prototypes */
#include "collide.h"      /* need to check for collisions */
#include <math.h>         /* need sqrt, acos, sin & cos */
#include "stdio.h"
#include "base.h"         /* need to aim_radar bases */

extern int profile;
extern level_type level;

/* Will move the vehicle's position along its velocity vector, while keeping
   its front vector tangent to the cylinder and the up vector normal to the
   surface. */

void move_along_velocity_surface( Vehicle *v )
{
    Float_Point front_temp, vel_temp, new_front, new_up, new_position, new_vel;
    float len, vel_ccw = 0, front_ccw = 0, theta, phi, new_theta, sine, cosine;
    float front_len;
    float temp;

    vel_temp[X] = v->vel[X];
    vel_temp[Y] = v->vel[Y];

    len = (float)sqrt( ( vel_temp[X] * vel_temp[X] ) + ( vel_temp[Y] * vel_temp[Y] ) );

    front_len = (float)sqrt( ( v->orient.front[X] * v->orient.front[X] ) +
							 ( v->orient.front[Y] * v->orient.front[Y] ) );

    vel_temp[X] = v->vel[X] + v->orient.position[X];
    vel_temp[Y] = v->vel[Y] + v->orient.position[Y];

    front_temp[X] = v->orient.front[X] + v->orient.position[X];
    front_temp[Y] = v->orient.front[Y] + v->orient.position[Y];

    if( ( v->orient.position[Y] >= 0.0 ) &&
        ( v->orient.position[Y] > fabs( v->orient.position[X] ) ) ) {
        if( front_temp[X] < v->orient.position[X] )
            front_ccw = TRUE;
        else
            front_ccw = FALSE;
        if( vel_temp[X] < v->orient.position[X] )
            vel_ccw = TRUE;
        else
            vel_ccw = FALSE;
    }
    else if( ( v->orient.position[X] < 0.0 ) &&
        ( -v->orient.position[X] > fabs( v->orient.position[Y] ) ) ) {
        if( front_temp[Y] < v->orient.position[Y] )
            front_ccw = TRUE;
        else
            front_ccw = FALSE;
        if( vel_temp[Y] < v->orient.position[Y] )
            vel_ccw = TRUE;
        else
            vel_ccw = FALSE;
    }
    else if( ( v->orient.position[Y] < 0.0 ) &&
        ( -v->orient.position[Y] > fabs( v->orient.position[X] ) ) ) {
        if( front_temp[X] > v->orient.position[X] )
            front_ccw = TRUE;
        else
            front_ccw = FALSE;
        if( vel_temp[X] > v->orient.position[X] )
            vel_ccw = TRUE;
        else
            vel_ccw = FALSE;
    }
    else if( ( v->orient.position[X] >= 0.0 ) &&
        ( v->orient.position[X] > fabs( v->orient.position[Y] ) ) ) {
        if( front_temp[Y] > v->orient.position[Y] )
            front_ccw = TRUE;
        else
            front_ccw = FALSE;
        if( vel_temp[Y] > v->orient.position[Y] )
            vel_ccw = TRUE;
        else
            vel_ccw = FALSE;
    }

    if( vel_ccw )
        phi = len / v->surface_rad;
    else
        phi = -len / v->surface_rad;

    if( v->orient.position[Y] >= 0 ) {

        temp = v->orient.position[X] / v->surface_rad;

        if( temp > 1.0 ) {
            temp = 1.0;
        }
        else if( temp < -1.0 ) {
            temp = -1.0;
        }

        theta = (float)acos( temp );

    }
    else {
        temp = v->orient.position[X] / v->surface_rad;

        if( temp > 1.0 ) {
            temp = 1.0;
        }
        else if( temp < -1.0 ) {
            temp = -1.0;
        }

        theta = ( 2 * PI ) - (float)acos( temp );
    }

    new_theta = theta + phi;

    sine = (float)sin( new_theta );
    cosine = (float)cos( new_theta );

    if( front_ccw ) {
        new_front[X] = front_len * -sine;
        new_front[Y] = front_len * cosine;
        new_front[Z] = v->orient.front[Z];
    }
    else {
        new_front[X] = front_len * sine;
        new_front[Y] = front_len * -cosine;
        new_front[Z] = v->orient.front[Z];
    }

    new_up[X] = -cosine;
    new_up[Y] = -sine;
    new_up[Z] = 0.0;

    new_position[X] = v->surface_rad * cosine;
    new_position[Y] = v->surface_rad * sine;
    new_position[Z] = v->vel[Z] + v->orient.position[Z];

    if( vel_ccw ) {
        new_vel[X] = len * -sine;
        new_vel[Y] = len * cosine;
        new_vel[Z] = v->vel[Z];
    }
    else {
        new_vel[X] = len * sine;
        new_vel[Y] = len * -cosine;
        new_vel[Z] = v->vel[Z];
    }

    v->orient.front[X] = new_front[X];
    v->orient.front[Y] = new_front[Y];
    v->orient.front[Z] = new_front[Z];

    normalize( v->orient.front );

    v->orient.position[X] = new_position[X];
    v->orient.position[Y] = new_position[Y];
    v->orient.position[Z] = new_position[Z];

    v->orient.up[X] = new_up[X];
    v->orient.up[Y] = new_up[Y];
    v->orient.up[Z] = new_up[Z];

    normalize( v->orient.up );

    v->vel[X] = new_vel[X];
    v->vel[Y] = new_vel[Y];
    v->vel[Z] = new_vel[Z];
}

/* Move the vehicle position backwards against its velocity vector, while
   keeping the front vector tangent to the surface and keeping the up
   vector normal to the surface. */

void move_against_velocity_surface( Vehicle *v )
{
    v->vel[X] = -v->vel[X];
    v->vel[Y] = -v->vel[Y];
    v->vel[Z] = -v->vel[Z];

    move_along_velocity_surface( v );

    v->vel[X] = -v->vel[X];
    v->vel[Y] = -v->vel[Y];
    v->vel[Z] = -v->vel[Z];
}

/* Move the vehicle's position along its velocity vector, the
   front and up vectors remain unchanged */

void move_along_velocity_3d( Vehicle *v )
{
    v->orient.position[X] = v->vel[X] + v->orient.position[X];
    v->orient.position[Y] = v->vel[Y] + v->orient.position[Y];
    v->orient.position[Z] = v->vel[Z] + v->orient.position[Z];
}

/* Move the vehicle's position against its velocity vector, the
   front and up vectors remain unchanged */

void move_against_velocity_3d( Vehicle *v )
{
   v->orient.position[X] = v->orient.position[X] - v->vel[X];
   v->orient.position[Y] = v->orient.position[Y] - v->vel[Y];
   v->orient.position[Z] = v->orient.position[Z] - v->vel[Z];
}

/* This function is called when the up arrow key is pressed.  If the vehicle
   is on the surface it adds the (front_vector * surface_inc_speed) vector
   to the velocity vector.  If the vehicle is in the air it calls
   update_vehicle_dive(). */

void update_vehicle_up_key( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    if( v->vehicle_mode == Surface ) {

        /* add (front * surface_inc_speed) to velocity */

        /*
        printf("update_vehicle_up_key()\n");

        printf("  old_vel = (%.30f,%.30f,%.30f)\n",v->vel[X],v->vel[Y],v->vel[Z]);
        printf("  front = (%.30f,%.30f,%.30f)\n",v->orient.front[X],v->orient.front[Y],v->orient.front[Z]);
        printf("  v->surface_inc_speed = %.30f\n",v->surface_inc_speed);
        */

        v->vel[X] = ( v->orient.front[X] * v->surface_inc_speed * factor ) + v->vel[X];
        v->vel[Y] = ( v->orient.front[Y] * v->surface_inc_speed * factor ) + v->vel[Y];
        v->vel[Z] = ( v->orient.front[Z] * v->surface_inc_speed * factor ) + v->vel[Z];

        /*
        printf("  new_vel = (%.30f,%.30f,%.30f)\n",v->vel[X],v->vel[Y],v->vel[Z]);
        */
    }
    else if( v->vehicle_mode == Air ) {
        update_vehicle_dive( v, value );
    }
}

/* This function is called when the down arrow key is pressed.  If the
   vehicle is on the surface it adds the (front_vector * -surface_inc_speed)
   vector to the velocity vector.  If the vehicle is in the air it calls
   update_vehicle_climb(). */

void update_vehicle_down_key( Vehicle *v, unsigned char value )
{
    float factor = (float)value / 255.0f;

    if( v->vehicle_mode == Surface ) {

        v->vel[X] = ( v->orient.front[X] * -v->surface_inc_speed * factor ) + v->vel[X];
        v->vel[Y] = ( v->orient.front[Y] * -v->surface_inc_speed * factor ) + v->vel[Y];
        v->vel[Z] = ( v->orient.front[Z] * -v->surface_inc_speed * factor ) + v->vel[Z];
        /*
        printf("update_vehicle_down_key()\n");
        printf("  v->vel = (%.30f,%.30f,%.30f)\n",v->vel[X],v->vel[Y],v->vel[Z]);
        */
    }
    else if( v->vehicle_mode == Air ) {
        update_vehicle_climb( v, value );
    }
}

/* This function is called when the left arrow key is pressed.  If the
   vehicle is on the surface it calls update_vehicle_turn_counterclockwise().
   If the vehicle is in the air it calls
   update_vehicle_spin_counterclockwise(). */

void update_vehicle_left_key( Vehicle *v, unsigned char value )
{
    if( v->vehicle_mode == Surface ) {
        update_vehicle_turn_counterclockwise( v, value );
    }
    else if( v->vehicle_mode == Air ) {
        update_vehicle_spin_counterclockwise( v, value );
    }
}

/* This function is called when the right arrow key is pressed.  If the
   vehicle is on the surface it calls update_vehicle_turn_clockwise(). If
   the vehicle is in the air it calls update_vehicle_spin_clockwise(). */

void update_vehicle_right_key( Vehicle *v, unsigned char value )
{
    if( v->vehicle_mode == Surface ) {
        update_vehicle_turn_clockwise( v, value );
    }
    else if( v->vehicle_mode == Air ) {
        update_vehicle_spin_clockwise( v, value );
    }
}

/* This function is called when the user has pressed alt and left arrow
   at the same time.  If the vehicle is on the surface the vector (left *
   surface_inc_sidestep_speed) is added to velocity.  If the vehicle is in
   the air the vector (left * air_inc_sidestep_speed) is added to the
   velocity. */

void update_vehicle_sidestep_left( Vehicle *v, unsigned char value )
{
    Float_Vector left;
    float factor = value / 255.0f;

    if( v->vehicle_mode == Surface ) {

        vector_cross_product( v->orient.up, v->orient.front, left );

        v->vel[X] += ( left[X] * v->surface_inc_sidestep_speed * factor );
        v->vel[Y] += ( left[Y] * v->surface_inc_sidestep_speed * factor );
        v->vel[Z] += ( left[Z] * v->surface_inc_sidestep_speed * factor );

        /*
        printf("update_vehicle_sidestep_left()\n");
        printf("  v->vel = (%.30f,%.30f,%.30f)\n", v->vel[X],v->vel[Y],v->vel[Z]);
        */
    }
    else if( v->vehicle_mode == Air ) {

        vector_cross_product( v->orient.up, v->orient.front, left );

        v->vel[X] = ( left[X] * v->air_inc_sidestep_speed * factor ) + v->vel[X];
        v->vel[Y] = ( left[Y] * v->air_inc_sidestep_speed * factor ) + v->vel[Y];
        v->vel[Z] = ( left[Z] * v->air_inc_sidestep_speed * factor ) + v->vel[Z];
    }
}

/* This function is called when the user has pressed alt and right arrow
   at the same time.  If the vehicle is on the surface (right *
   surface_inc_sidestep_speed) is added to velocity.  If the vehicle is in
   the air (right * air_inc_sidestep_speed) is added to velocity. */

void update_vehicle_sidestep_right( Vehicle *v, unsigned char value )
{
    Float_Vector right;
    float factor = value / 255.0f;

    if( v->vehicle_mode == Surface ) {

        vector_cross_product( v->orient.front, v->orient.up, right );

        v->vel[X] = ( right[X] * v->surface_inc_sidestep_speed * factor) + v->vel[X];
        v->vel[Y] = ( right[Y] * v->surface_inc_sidestep_speed * factor) + v->vel[Y];
        v->vel[Z] = ( right[Z] * v->surface_inc_sidestep_speed * factor) + v->vel[Z];

        /*
        printf("update_vehicle_sidestep_right()\n");
        printf("  v->vel = (%.30f,%.30f,%.30f)\n", v->vel[X],v->vel[Y],v->vel[Z]);
        */
    }
    else if( v->vehicle_mode == Air ) {

        vector_cross_product( v->orient.front, v->orient.up, right );

        v->vel[X] = ( right[X] * v->air_inc_sidestep_speed * factor) + v->vel[X];
        v->vel[Y] = ( right[Y] * v->air_inc_sidestep_speed * factor) + v->vel[Y];
        v->vel[Z] = ( right[Z] * v->air_inc_sidestep_speed * factor) + v->vel[Z];
    }
}

void update_vehicle_sidestep_up( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    if( v->vehicle_mode == Surface ) {
        ;
    }
    else if( v->vehicle_mode == Air ) {
        v->vel[X] = ( v->orient.up[X] * v->air_inc_sidestep_speed * factor) + v->vel[X];
        v->vel[Y] = ( v->orient.up[Y] * v->air_inc_sidestep_speed * factor) + v->vel[Y];
        v->vel[Z] = ( v->orient.up[Z] * v->air_inc_sidestep_speed * factor) + v->vel[Z];
    }
}

void update_vehicle_sidestep_down( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    if( v->vehicle_mode == Surface ) {
        ;
    }
    else if( v->vehicle_mode == Air ) {
        v->vel[X] = ( -v->orient.up[X] * v->air_inc_sidestep_speed * factor) + v->vel[X];
        v->vel[Y] = ( -v->orient.up[Y] * v->air_inc_sidestep_speed * factor) + v->vel[Y];
        v->vel[Z] = ( -v->orient.up[Z] * v->air_inc_sidestep_speed * factor) + v->vel[Z];
    }
}


int old_push_vehicle_toward_surface( Vehicle *v )
{
    float len, proj_len;
    Float_Vector new_position, p, project;
    const float epsilon = 0.01f;
    const float inc_landing_speed = 0.067f;
    const float max_landing_speed = 0.35f;

    /* find new_position which is the destination position on the
       surface */

    /* Find the length of the position vector with reference to the x-y
       plane */

    len = (float)sqrt( (v->orient.position[X] * v->orient.position[X]) + (v->orient.position[Y] * v->orient.position[Y]) );

    /* New position is a just position extended in the x-y plane, so that
       its x-y length is equal to the surface_radius. */

    if( len != 0.0 ) {
        new_position[X] = (v->orient.position[X] / len) * v->surface_rad;
        new_position[Y] = (v->orient.position[Y] / len) * v->surface_rad;
        new_position[Z] = v->orient.position[Z];
    }
    else {
        new_position[X] = 0.0;
        new_position[Y] = v->surface_rad;
        new_position[Z] = v->orient.position[Z];
    }

    /* p is a vector from current position to the destination */

    p[X] = new_position[X] - v->orient.position[X];
    p[Y] = new_position[Y] - v->orient.position[Y];
    p[Z] = new_position[Z] - v->orient.position[Z];

    len = (float)sqrt( vector_dot_product( p, p ) );

    /* if we are close enough to the destination were done */

    if( (len < epsilon) && (len > -epsilon) ) {
        return TRUE;
    }

    vector_project( v->vel, p, project );

    /* if the distance to travel is less then the downward component of the
       velocity vector, we must change the vel vector so we don't overshoot
       the destination */

    proj_len = myvector_length( project );

    if( len < proj_len && (vector_dot_product(v->vel, p) < 0) ) {
        /* printf("arrrrghhhhhhhhhhhhhh!!!!!!!!!!\n"); */
        v->vel[X] = p[X];
        v->vel[Y] = p[Y];
        v->vel[Z] = p[Z];
    }
    else {
        v->vel[X] += (p[X] / len) * inc_landing_speed;
        v->vel[Y] += (p[Y] / len) * inc_landing_speed;
        v->vel[Z] += (p[Z] / len) * inc_landing_speed;
    }

    /* make sure that v->vel is not greater then max_landing_speed */

    len = myvector_length( v->vel );

    if( len > max_landing_speed ) {
        v->vel[X] = v->vel[X] / len * max_landing_speed;
        v->vel[Y] = v->vel[Y] / len * max_landing_speed;
        v->vel[Z] = v->vel[Z] / len * max_landing_speed;
    }

    move_along_velocity_3d( v );

    return FALSE;
}

int push_vehicle_toward_surface( Vehicle *v )
{
    const float epsilon = 0.005f;
    Float_Point destination;
    float len, destination_distance, downward_velocity_len, next_downward_velocity_len;
    Float_Vector down, downward_velocity;

    const float max_landing_speed = 0.35f;
    const float inc_landing_speed = 0.067f;

    /* find the destination point to land on */

    if( (v->orient.position[X] < epsilon) && (v->orient.position[X] > -epsilon) &&
        (v->orient.position[Y] < epsilon) && (v->orient.position[Y] > -epsilon) ) {

        destination[X] = 0.0;
        destination[Y] = v->surface_rad;
        destination[Z] = v->orient.position[Z];
    }
    else {
        len = (float)sqrt( (v->orient.position[X] * v->orient.position[X]) +
						   (v->orient.position[Y] * v->orient.position[Y]) );

        destination[X] = (v->orient.position[X] / len) * v->surface_rad;
        destination[Y] = (v->orient.position[Y] / len) * v->surface_rad;
        destination[Z] = v->orient.position[Z];
    }

    /* if the vehicle is already on the destination point then we are finished */

    /* find the distance to the destination point & the down vector */

    down[X] = destination[X] - v->orient.position[X];
    down[Y] = destination[Y] - v->orient.position[Y];
    down[Z] = destination[Z] - v->orient.position[Z];

    destination_distance = myvector_length( down );

    /* if the vehicle is already on the destination point then we are finished */

    if( destination_distance < epsilon ) {

        /* printf("finished landing!!!!\n"); */

        move_along_velocity_3d( v );

        return TRUE;
    }

    /* normalize the down vector */

    down[X] = down[X] / destination_distance;
    down[Y] = down[Y] / destination_distance;
    down[Z] = down[Z] / destination_distance;

    /* find the downward_velocity vector */

    if( vector_dot_product( down, v->vel ) < 0 ) {
        downward_velocity[X] = 0.0;
        downward_velocity[Y] = 0.0;
        downward_velocity[Z] = 0.0;
    }
    else {
        vector_project( v->vel, down, downward_velocity );
    }

    /* find the length of the downward_velocity vector */

    downward_velocity_len = myvector_length( downward_velocity );

    /* find the length of the downward_velocity with this frames acceleration
       added to it. */

    if( (downward_velocity_len + inc_landing_speed) > max_landing_speed ) {
        next_downward_velocity_len = max_landing_speed;
    }
    else {
        next_downward_velocity_len = downward_velocity_len + inc_landing_speed;
    }

    /* if we are going to hit the ground this frame */

    if( destination_distance < next_downward_velocity_len ) {

        /* printf("were landing this frame!!!!\n"); */

        v->vel[X] += down[X] * (destination_distance - downward_velocity_len);
        v->vel[Y] += down[Y] * (destination_distance - downward_velocity_len);
        v->vel[Z] += down[Z] * (destination_distance - downward_velocity_len);

        move_along_velocity_3d( v );

        return TRUE;

    }
    else {  /* we aren't going to land this frame */

        /* if the vehicle is moving in the up direction */

        if( vector_dot_product( v->vel, down ) < 0 ) {

            /* we don't have to worry about exceeding the max_landing_speed because
               we are going up not down */

            /* printf("were going up!!!!\n"); */

            v->vel[X] += (down[X] * inc_landing_speed);
            v->vel[Y] += (down[Y] * inc_landing_speed);
            v->vel[Z] += (down[Z] * inc_landing_speed);
        }
        else { /* the vehicle is moving down */

            /* if adding the downward increment results in a downward velocity faster then
               max_landing_speed then make the downward velocity equal the max_landing_speed,
               otherwise just add the downward increment. */

            if( (downward_velocity_len + inc_landing_speed) > max_landing_speed ) {

                /* printf("hit maximum speed!!!!\n"); */

                v->vel[X] += (down[X] * max_landing_speed) - (down[X] * downward_velocity_len);
                v->vel[Y] += (down[Y] * max_landing_speed) - (down[Y] * downward_velocity_len);
                v->vel[Z] += (down[Z] * max_landing_speed) - (down[Z] * downward_velocity_len);
            }
            else {

                /* printf("normal landing!!!\n"); */

                v->vel[X] += down[X] * inc_landing_speed;
                v->vel[Y] += down[Y] * inc_landing_speed;
                v->vel[Z] += down[Z] * inc_landing_speed;
            }
        }
    }

    move_along_velocity_3d( v );

    return FALSE;
}


/* align_vehicle_for_landing rotates the vehicle a small amount per frame
   until the vehicles front vector is tangent to the cylinder and its
   up vector is normal to it.  It returns TRUE when the vehicle is aligned */

int align_vehicle_for_landing( Vehicle *v )
{
    float len, theta, temp;
    Float_Point new_position, new_front, new_up, p, axis, i, j;
    Float_Matrix front_rotate, up_rotate, rotation;
    long done = FALSE;
    const float rotation_increment = 0.1745329f; /* 10 degrees in radians */
    const float epsilon = 0.005f;

    /* Find the destination orientation, where front is tagent to the
       cylinder, and up is normal to the cylinder at position, which
       lies on the cylinder */

    /* Find the length of the position vector with reference to the x-y
       plane */

    len = (float)sqrt( (v->orient.position[X] * v->orient.position[X]) + (v->orient.position[Y] * v->orient.position[Y]) );

    /* New position is a just position extended in the x-y plane, so that
       its x-y length is equal to the surface_radius. */

    if( len != 0.0 ) {
        new_position[X] = (v->orient.position[X] / len) * v->surface_rad;
        new_position[Y] = (v->orient.position[Y] / len) * v->surface_rad;
        new_position[Z] = v->orient.position[Z];
    }
    else {
        new_position[X] = 0.0;
        new_position[Y] = v->surface_rad;
        new_position[Z] = v->orient.position[Z];
    }

    /* new_up is just -new_position / || new_position || w.r.t. the x-y
       plane */


    if( len != 0.0 ) {
        new_up[X] = -v->orient.position[X] / len;
        new_up[Y] = -v->orient.position[Y] / len;
        new_up[Z] = 0.0;
    }
    else {
        new_up[X] = 0.0;
        new_up[Y] = -1.0;
        new_up[Z] = 0.0;
    }

    /* find the angle theta on the cylinder that new_position lies */

    if( v->orient.position[Y] >= 0.0 ) {
        temp = new_position[X] / v->surface_rad;

        if( temp > 1.0 ) {
            temp = 1.0;
        }
        else if( temp < -1.0 ) {
            temp = -1.0;
        }

        theta = (float)acos( temp );
    }
    else {
        temp = new_position[X] / v->surface_rad;

        if( temp > 1.0 ) {
            temp = 1.0;
        }
        else if( temp < -1.0 ) {
            temp = -1.0;
        }

        theta = (2 * PI) - (float)acos( temp );
    }

    /* i is a vector that is tangent to the cylinder and points in a counter
       clockwise direction.  j is just the x-axis. i and j form a plane. */

    i[X] = -(float)sin( theta );
    i[Y] = (float)cos( theta );
    i[Z] = 0.0;

    j[X] = 0.0;
    j[Y] = 0.0;
    j[Z] = 1.0;

    /* new_front is just the front vector projected onto the i-j plane */

    new_front[X] = (vector_dot_product( v->orient.front, i ) * i[X]) +
                   (vector_dot_product( v->orient.front, j ) * j[X]);
    new_front[Y] = (vector_dot_product( v->orient.front, i ) * i[Y]) +
                   (vector_dot_product( v->orient.front, j ) * j[Y]);
    new_front[Z] = (vector_dot_product( v->orient.front, i ) * i[Z]) +
                   (vector_dot_product( v->orient.front, j ) * j[Z]);

    /* normalize it so its length equals 1 */

    if( sqrt( vector_dot_product( new_front, new_front ) ) != 0.0 ) {
        normalize( new_front );
    }
    else {
        new_front[X] = 0.0;
        new_front[Y] = 0.0;
        new_front[Z] = 1.0;
    }

    /* Check to see of the new orientation equals the old one. If so then
       the vehicle is already on the surface and we're done. */

    p[X] = v->orient.front[X] - new_front[X];
    p[Y] = v->orient.front[Y] - new_front[Y];
    p[Z] = v->orient.front[Z] - new_front[Z];

    if( ((p[X] < epsilon) && (p[X] > -epsilon)) &&
        ((p[Y] < epsilon) && (p[Y] > -epsilon)) &&
        ((p[Z] < epsilon) && (p[Z] > -epsilon)) ) {

        p[X] = v->orient.up[X] - new_up[X];
        p[Y] = v->orient.up[Y] - new_up[Y];
        p[Z] = v->orient.up[Z] - new_up[Z];

        if( ((p[X] < epsilon) && (p[X] > -epsilon)) &&
            ((p[Y] < epsilon) && (p[Y] > -epsilon)) &&
            ((p[Z] < epsilon) && (p[Z] > -epsilon)) ) {

          /*  p[X] = v->orient.position[X] - new_position[X];
            p[Y] = v->orient.position[Y] - new_position[Y];
            p[Z] = v->orient.position[Z] - new_position[Z];

            if( ((p[X] < epsilon) && (p[X] > -epsilon)) &&
                ((p[Y] < epsilon) && (p[Y] > -epsilon)) &&
                ((p[Z] < epsilon) && (p[Z] > -epsilon)) ) {

                return TRUE;
            } */

            return TRUE;
        }
    }

    if( !done ) {

        /* Find the front_rotate matrix, which will rotate the front
           vector towards the new_front vector. */

        /* if front equals new_front then the front_rotate matrix should
           become the identity matrix */

        p[X] = v->orient.front[X] - new_front[X];
        p[Y] = v->orient.front[Y] - new_front[Y];
        p[Z] = v->orient.front[Z] - new_front[Z];

        if( ((p[X] < epsilon) && (p[X] > -epsilon)) &&
            ((p[Y] < epsilon) && (p[Y] > -epsilon)) &&
            ((p[Z] < epsilon) && (p[Z] > -epsilon)) ) {
            identity_matrix_init( front_rotate );
        }
        else {
            /* theta is the angle between front and new_front */

            temp = vector_dot_product( v->orient.front, new_front );

            if( temp > 1.0 ) {
                temp = 1.0;
            }
            else if( temp < -1.0 ) {
                temp = -1.0;
            }

            theta = (float)acos( temp );

            /* axis is the axis that is used to rotate around */

            vector_cross_product( new_front, v->orient.front, axis );

            /* make sure the length of axis equals one. */

            if( sqrt( vector_dot_product( axis, axis ) ) != 0.0 ) {
            
                normalize( axis );
                
                if( (theta > 0) && (theta > rotation_increment) ) {
                    theta = rotation_increment;
                }

                /* assign to front_rotate the matrix that will rotate a point
                   theta radians around the vector axis */

                rotate_around_axis( theta, axis, front_rotate );

            }
            else {
                /* This can happen in two cases :
                   1) (new_front == 0) or (v->orient.front == 0)
                   2) new_front == v->orient.front
                   Both cases are extreamly unlikely but apperently do happen
                      occasionaly. */
                
                /* don't do any rotations */
                
                /* if new_front == 0 */
                
                if( ((new_front[X] < epsilon) && (new_front[X] > -epsilon)) &&
                    ((new_front[Y] < epsilon) && (new_front[Y] > -epsilon)) &&
                    ((new_front[Z] < epsilon) && (new_front[Z] > -epsilon)) ) {
                    
                    /* If this happens then something went very wrong somewere. */
                    /* so i guess we should quit landing this vehicle */
                    
                    return TRUE;
                }                
                else if( ((v->orient.front[X] < epsilon) && (v->orient.front[X] > -epsilon)) &&
                    ((v->orient.front[Y] < epsilon) && (v->orient.front[Y] > -epsilon)) &&
                    ((v->orient.front[Z] < epsilon) && (v->orient.front[Z] > -epsilon)) ) {
                    
                    /* If this happens then something went very wrong somewere. */
                    /* so i guess we should quit landing this vehicle */
                    
                    return TRUE;
                }
                else {
                    /* manually set v->orient.front equal to new_front */
                    
                    v->orient.front[X] = new_front[X];
                    v->orient.front[Y] = new_front[Y];
                    v->orient.front[Z] = new_front[Z];
                    
                    identity_matrix_init( front_rotate );
                }
                
                /* say a prayer and hope this works! Hail Mary, full of grace .... ,
                   Our Father who art in heaven .... , Please let this fucking thing work!!!! */
            }
        }

        /* if front equals new_front then we can safely rotate the up
           vector */

        p[X] = v->orient.front[X] - new_front[X];
        p[Y] = v->orient.front[Y] - new_front[Y];
        p[Z] = v->orient.front[Z] - new_front[Z];

        if( ((p[X] < epsilon) && (p[X] > -epsilon)) &&
            ((p[Y] < epsilon) && (p[Y] > -epsilon)) &&
            ((p[Z] < epsilon) && (p[Z] > -epsilon)) ) {

            /* Find the matrix, up_rotate, that will rotate the up vector
               into the new_up vector. */

            /* if up equals new_up, up_rotate should be the identity matrix */

            p[X] = v->orient.up[X] - new_up[X];
            p[Y] = v->orient.up[Y] - new_up[Y];
            p[Z] = v->orient.up[Z] - new_up[Z];

            if( ((p[X] < epsilon) && (p[X] > -epsilon)) &&
                ((p[Y] < epsilon) && (p[Y] > -epsilon)) &&
                ((p[Z] < epsilon) && (p[Z] > -epsilon)) ) {

                identity_matrix_init( up_rotate );
            }
            else {

                /* theta is the angle between up and new_up */

                temp = vector_dot_product( v->orient.up, new_up );

                if( temp > 1.0 ) {
                    temp = 1.0;
                }
                else if( temp < -1.0 ) {
                    temp = -1.0;
                }

                theta = (float)acos( temp );

                /* axis is the axis to rotate around */

                vector_cross_product( new_up, v->orient.up, axis );

                /* make sure axis is normalized */

                if( sqrt( vector_dot_product( axis, axis ) ) != 0.0 ) {
                
                    normalize( axis );
                    
                    if( (theta > 0) && (theta > rotation_increment) ) {
                        theta = rotation_increment;
                    }

                    /* assign to up_rotate the matrix that will rotate a point
                       theta radians around axis */

                    rotate_around_axis( theta, axis, up_rotate );

                }
                else {
                    /* This can happen in two cases :
                       1) (new_up == 0) or (v->orient.up == 0)
                       2) new_up == v->orient.front
                       Both cases are extreamly unlikely but apperently do happen
                       occasionaly. */
                
                    /* don't do any rotations */
                
                    /* if new_up == 0 */
                
                    if( ((new_up[X] < epsilon) && (new_up[X] > -epsilon)) &&
                        ((new_up[Y] < epsilon) && (new_up[Y] > -epsilon)) &&
                        ((new_up[Z] < epsilon) && (new_up[Z] > -epsilon)) ) {
                    
                        /* If this happens then something went very wrong somewere. */
                        /* so i guess we should quit landing this vehicle */
                    
                        return TRUE;
                    }                
                    else if( ((v->orient.up[X] < epsilon) && (v->orient.up[X] > -epsilon)) &&
                             ((v->orient.up[Y] < epsilon) && (v->orient.up[Y] > -epsilon)) &&
                             ((v->orient.up[Z] < epsilon) && (v->orient.up[Z] > -epsilon)) ) {
                    
                        /* If this happens then something went very wrong somewere. */
                        /* so i guess we should quit landing this vehicle */
                    
                        return TRUE;
                    }
                    else {
                        /* manually set v->orient.front equal to new_front */
                    
                        v->orient.up[X] = new_up[X];
                        v->orient.up[Y] = new_up[Y];
                        v->orient.up[Z] = new_up[Z];
                        
                        identity_matrix_init( up_rotate );
                    }
                
                    /* say a prayer and hope this works! Hail Mary, full of grace .... ,
                       Our Father who art in heaven .... , Please let this fucking thing work!!!! */

                }

            }
        }
        else {
            identity_matrix_init( up_rotate );
        }

        /* combine both rotations into a single matrix */

        matrix_multiply( up_rotate, front_rotate, rotation );

        /* transform the front and up vectors of the vehicle */

        transformation( rotation, v->orient.front );
        transformation( rotation, v->orient.up );

        /* figure out the new position */

      /*  p[X] = new_position[X] - v->orient.position[X];
        p[Y] = new_position[Y] - v->orient.position[Y];
        p[Z] = new_position[Z] - v->orient.position[Z];

        len = sqrt( vector_dot_product( p, p ) );

        if( len < translation_increment ) {
            v->orient.position[X] = new_position[X];
            v->orient.position[Y] = new_position[Y];
            v->orient.position[Z] = new_position[Z];
        }
        else {
            v->orient.position[X] += (p[X] / len) * translation_increment;
            v->orient.position[Y] += (p[Y] / len) * translation_increment;
            v->orient.position[Z] += (p[Z] / len) * translation_increment;
        } */
    }

    return FALSE;
}

void update_vehicle_turn_counterclockwise( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    v->surface_rot_speed += v->surface_inc_rot_speed * factor;
}

void update_vehicle_turn_clockwise( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    v->surface_rot_speed -= v->surface_inc_rot_speed * factor;
}

void update_vehicle_spin_counterclockwise( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    v->air_spin_rot_speed += v->air_inc_rot_speed * factor;
}

void update_vehicle_spin_clockwise( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    v->air_spin_rot_speed -= v->air_inc_rot_speed * factor;
}

void update_vehicle_climb( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    v->air_rise_rot_speed += v->air_inc_rot_speed * factor;
}

void update_vehicle_dive( Vehicle *v, unsigned char value )
{
    float factor = value / 255.0f;

    v->air_rise_rot_speed -= v->air_inc_rot_speed * factor;
}

void rotate_vehicle( Vehicle *v )
{
    Float_Vector axis;
    Float_Matrix rotation_matrix, rotation_matrix2, result;
    float theta = 0.0;
    float angular_friction = level.angular_friction; 

    if( v->vehicle_mode == Surface ) {
        /*
        printf("rotate_vehicle()\n");

        printf("  old_front = (%.30f,%.30f,%.30f)\n",v->orient.front[X],v->orient.front[Y],v->orient.front[Z]);
        */

        /* reduce surface rotation speed by friction */

        if( (v->surface_rot_speed > 0.0) && (v->surface_rot_speed > angular_friction) ) {
            v->surface_rot_speed -= angular_friction;
        }
        else if( (v->surface_rot_speed < 0.0) && (v->surface_rot_speed < -angular_friction) ) {
            v->surface_rot_speed += angular_friction;
        }
        else {
            v->surface_rot_speed = 0.0;
        }

        /* truncate surface rotation speed if necessary */

        if( v->surface_rot_speed > v->surface_max_rot_speed ) {
            v->surface_rot_speed = v->surface_max_rot_speed;
        }
        else if( v->surface_rot_speed < -v->surface_max_rot_speed ) {
            v->surface_rot_speed = -v->surface_max_rot_speed;
        }

        /* make sure air rot speed is zero */

        v->air_rise_rot_speed = 0.0;
        v->air_spin_rot_speed = 0.0;

        /* find the axis to rotate around */

        axis[X] = -v->orient.up[X];
        axis[Y] = -v->orient.up[Y];
        axis[Z] = -v->orient.up[Z];

        /* find the amount to rotate around axis */

        theta = v->surface_rot_speed;

        /* find the rotation matrix */

        rotate_around_axis( theta, axis, rotation_matrix );

        /* rotate the front and up vectors */

        transformation( rotation_matrix, v->orient.front );
        transformation( rotation_matrix, v->orient.up );

        /*
        printf("  new_front = (%.30f,%.30f,%.30f)\n",v->orient.front[X],v->orient.front[Y],v->orient.front[Z]);
        */
    }
    else if( v->vehicle_mode == Air ) {

        /* reduce air_rot_speed by friction */

        if( (v->air_rise_rot_speed > 0.0) && (v->air_rise_rot_speed > angular_friction) ) {
            v->air_rise_rot_speed -= angular_friction;
        }
        else if( (v->air_rise_rot_speed < 0.0) && (v->air_rise_rot_speed < -angular_friction) ) {
            v->air_rise_rot_speed += angular_friction;
        }
        else {
            v->air_rise_rot_speed = 0.0;
        }

        if( (v->air_spin_rot_speed > 0.0) && (v->air_spin_rot_speed > angular_friction) ) {
            v->air_spin_rot_speed -= angular_friction;
        }
        else if( (v->air_spin_rot_speed < 0.0) && (v->air_spin_rot_speed < -angular_friction) ) {
            v->air_spin_rot_speed += angular_friction;
        }
        else {
            v->air_spin_rot_speed = 0.0;
        }

        /* truncate air rotation speed if necessary */

        if( v->air_rise_rot_speed > v->air_max_rot_speed ) {
            v->air_rise_rot_speed = v->air_max_rot_speed;
        }
        else if( v->air_rise_rot_speed < -v->air_max_rot_speed ) {
            v->air_rise_rot_speed = -v->air_max_rot_speed;
        }

        if( v->air_spin_rot_speed > v->air_max_rot_speed ) {
            v->air_spin_rot_speed = v->air_max_rot_speed;
        }
        else if( v->air_spin_rot_speed < -v->air_max_rot_speed ) {
            v->air_spin_rot_speed = -v->air_max_rot_speed;
        }

        /* make sure that surface_rot_speed is zero */

        v->surface_rot_speed = 0.0;

        /* find the axis to rotate around */

        vector_cross_product( v->orient.up, v->orient.front, axis );

        /* find the angle to rotate around axis */

        theta = v->air_rise_rot_speed;

        /* find the rotation matrix */

        rotate_around_axis( theta, axis, rotation_matrix );

        /* find the axis to rotate around */

        axis[X] = v->orient.front[X];
        axis[Y] = v->orient.front[Y];
        axis[Z] = v->orient.front[Z];

        /* find the angle to rotate around axis */

        theta = v->air_spin_rot_speed;

        /* find the rotation matrix */

        rotate_around_axis( theta, axis, rotation_matrix2 );

        /* combine rotation matrices */

        matrix_multiply( rotation_matrix2, rotation_matrix, result );

        /* rotate the front and up vectors */

        transformation( result, v->orient.front );
        transformation( result, v->orient.up );
    }
}


void up_throttle( Vehicle *v )
{
    v->air_forward_speed += 0.02f;

    if( v->air_forward_speed > v->air_max_forward_speed ) {
        v->air_forward_speed = v->air_max_forward_speed;
    }
}

void down_throttle( Vehicle *v )
{
    v->air_forward_speed -= 0.02f;

    if( v->air_forward_speed < 0.0 ) {
        v->air_forward_speed = 0.0;
    }
}

void move_vehicle_backward( Vehicle *v )
{
    if( v->vehicle_mode == Surface && v->alive ) {

        move_against_velocity_surface( v );

    }
    else if( v->vehicle_mode == Air && v->alive ) {

        move_against_velocity_3d( v );

    }
    else if( v->vehicle_mode == Landing && v->alive ) {

        move_against_velocity_3d( v );

    }
}

void move_vehicle_forward( Vehicle *v )
{
    Float_Vector left, projection;
    float length;
    long alignment_finished, movement_finished;

    if( v->vehicle_mode == Surface && v->alive ) {

        /* make sure the length of vel is less than or equal to
           surface_max_speed */

        if( (length = myvector_length( v->vel )) > v->surface_max_speed ) {
            v->vel[X] = (v->vel[X] / length) * v->surface_max_speed;
            v->vel[Y] = (v->vel[Y] / length) * v->surface_max_speed;
            v->vel[Z] = (v->vel[Z] / length) * v->surface_max_speed;
        }
        if( (v->vel[X] == 0.0) && (v->vel[Y] == 0.0) && (v->vel[Z] == 0.0) ) {
            rotate_vehicle( v );
        }
        else {
            move_along_velocity_surface( v );
            rotate_vehicle( v );
        }
    }
    else if( v->vehicle_mode == Air && v->alive ) {

        /* make sure that the sideways component of the vel vector is not
           greater than air_max_sidestep_speed. */

        vector_cross_product( v->orient.up, v->orient.front, left );

        vector_project( v->vel, left, projection );

        length = myvector_length( projection );

        if( vector_dot_product( projection, left ) > 0 ) {
            if( length > v->air_max_sidestep_speed ) {
                v->vel[X] = v->vel[X] + (left[X] * v->air_max_sidestep_speed) - projection[X];
                v->vel[Y] = v->vel[Y] + (left[Y] * v->air_max_sidestep_speed) - projection[Y];
                v->vel[Z] = v->vel[Z] + (left[Z] * v->air_max_sidestep_speed) - projection[Z];
            }
        }
        else {
            if( length > v->air_max_sidestep_speed ) {
                v->vel[X] = v->vel[X] + (-left[X] * v->air_max_sidestep_speed) - projection[X];
                v->vel[Y] = v->vel[Y] + (-left[Y] * v->air_max_sidestep_speed) - projection[Y];
                v->vel[Z] = v->vel[Z] + (-left[Z] * v->air_max_sidestep_speed) - projection[Z];
            }
        }

        /* make sure the up and down component of the vel vector is not
           greater then air_max_sidestep_speed. */

        vector_project( v->vel, v->orient.up, projection );

        length = myvector_length( projection );

        if( vector_dot_product( projection, v->orient.up ) > 0 ) {
            if( length > v->air_max_sidestep_speed ) {
                v->vel[X] = v->vel[X] + (v->orient.up[X] * v->air_max_sidestep_speed) - projection[X];
                v->vel[Y] = v->vel[Y] + (v->orient.up[Y] * v->air_max_sidestep_speed) - projection[Y];
                v->vel[Z] = v->vel[Z] + (v->orient.up[Z] * v->air_max_sidestep_speed) - projection[Z];
            }
        }
        else {
            if( length > v->air_max_sidestep_speed ) {
                v->vel[X] = v->vel[X] + (-v->orient.up[X] * v->air_max_sidestep_speed) - projection[X];
                v->vel[Y] = v->vel[Y] + (-v->orient.up[Y] * v->air_max_sidestep_speed) - projection[Y];
                v->vel[Z] = v->vel[Z] + (-v->orient.up[Z] * v->air_max_sidestep_speed) - projection[Z];
            }
        }

        /* make sure that forward thrust is not greater than
           air_forward_speed */

        vector_project( v->vel, v->orient.front, projection );

        /* if vel is pointing in the same direction as front */

        if( vector_dot_product( projection, v->orient.front ) > 0 ) {

            length = myvector_length( projection );

            if( length > v->air_forward_speed ) {
                v->vel[X] = v->vel[X] + (v->orient.front[X] * v->air_forward_speed) - projection[X];
                v->vel[Y] = v->vel[Y] + (v->orient.front[Y] * v->air_forward_speed) - projection[Y];
                v->vel[Z] = v->vel[Z] + (v->orient.front[Z] * v->air_forward_speed) - projection[Z];
            }
        }

        if( (v->vel[X] == 0.0) && (v->vel[Y] == 0.0) && (v->vel[Z] == 0.0) ) {
            rotate_vehicle( v );
        }
        else {
            move_along_velocity_3d( v );
            rotate_vehicle( v );
        }

    }
    else if( v->vehicle_mode == Landing && v->alive ) {

        alignment_finished = align_vehicle_for_landing( v );
        movement_finished = push_vehicle_toward_surface( v );

        if( alignment_finished && movement_finished ) {
            v->vehicle_mode = Surface;
        }
    }
}

void reduce_by_friction( enum VehicleMode mode, Float_Vector vel )
{
    const float surface_friction = level.surface_friction;
    const float air_friction = level.air_friction;
    float length;

    if( mode == Surface ) {

        /* now reduce vel by friction */

        length = myvector_length( vel );

        if( (length > 0.0) && (length > air_friction) ) {

            vel[X] = vel[X] - (vel[X] * (surface_friction / length));
            vel[Y] = vel[Y] - (vel[Y] * (surface_friction / length));
            vel[Z] = vel[Z] - (vel[Z] * (surface_friction / length));

        }
        else {
            vel[X] = 0.0;
            vel[Y] = 0.0;
            vel[Z] = 0.0;
        }
    }
    else if( mode == Air || mode == Landing ) {

        /* now reduce vel by friction */

        length = myvector_length( vel );

        if( (length > 0.0) && (length > air_friction) ) {

            vel[X] = vel[X] - (vel[X] * (air_friction / length));
            vel[Y] = vel[Y] - (vel[Y] * (air_friction / length));
            vel[Z] = vel[Z] - (vel[Z] * (air_friction / length));

        }
        else {
            vel[X] = 0.0;
            vel[Y] = 0.0;
            vel[Z] = 0.0;
        }
    }
}


void bounce_vehicle( Vehicle *v, Float_Vector reflect )
{
    Float_Vector bounce_vector, projection;
    const float surface_min_vel = 0.15f;
    const float air_min_vel = 0.2f;
    const float surface_elasticity = 0.65f;
    const float air_elasticity = 0.65f;
    float elasticity;
    float min_vel;

    if( v->vehicle_mode == Surface ) {
        elasticity = surface_elasticity;
        min_vel = surface_min_vel;
    }
    else if( v->vehicle_mode == Air || v->vehicle_mode == Landing ){
        elasticity = air_elasticity;
        min_vel = air_min_vel;
    }
    else {
        elasticity = air_elasticity;
        min_vel = air_min_vel;
    }

    /* find the bounce_vector */

    if( (myvector_length( v->vel ) * elasticity) > min_vel ) {

        /* reflect the velocity vector off of the plane specified
           by the reflect_vector. Call it the bounce vector. */

        vector_project( v->vel, reflect, projection );

        if( vector_dot_product( projection, reflect ) < 0 ) {
            bounce_vector[X] = (-2.0f * projection[X]) + v->vel[X];
            bounce_vector[Y] = (-2.0f * projection[Y]) + v->vel[Y];
            bounce_vector[Z] = (-2.0f * projection[Z]) + v->vel[Z];
        }
        else {
            bounce_vector[X] = projection[X] + v->vel[X];
            bounce_vector[Y] = projection[Y] + v->vel[Y];
            bounce_vector[Z] = projection[Z] + v->vel[Z];
        }
    }
    else {

        /* let the bounce vector equal the reflect vector * min_vel */

        bounce_vector[X] = min_vel * reflect[X];
        bounce_vector[Y] = min_vel * reflect[Y];
        bounce_vector[Z] = min_vel * reflect[Z];
    }

    /* reduce the bounce_vector depending on the elasticity
       of the collision */

    v->vel[X] = elasticity * bounce_vector[X];
    v->vel[Y] = elasticity * bounce_vector[Y];
    v->vel[Z] = elasticity * bounce_vector[Z];
}


void bounce_colliding_vehicles( CollisionTable c_table, Player p[] )
{
    long i;

    for( i = 0; i < 6; i++ ) {
        if( c_table[i].collision ) {
            bounce_vehicle( &(p[i].tank), c_table[i].reflect );
        }
    }
}

void force_bounce_vehicle( Vehicle *v, Float_Vector reflect )
{
    const float surface_elasticity = 0.5;
    const float air_elasticity = 0.5;
    const float landing_elasticity = 0.5;

    if( v->vehicle_mode == Surface ) {
        v->vel[X] = surface_elasticity * reflect[X];
        v->vel[Y] = surface_elasticity * reflect[Y];
        v->vel[Z] = surface_elasticity * reflect[Z];
    }
    else if( v->vehicle_mode == Air ) {
        v->vel[X] = air_elasticity * reflect[X];
        v->vel[Y] = air_elasticity * reflect[Y];
        v->vel[Z] = air_elasticity * reflect[Z];
    }
    else if( v->vehicle_mode == Landing ) {
        v->vel[X] = landing_elasticity * reflect[X];
        v->vel[Y] = landing_elasticity * reflect[Y];
        v->vel[Z] = landing_elasticity * reflect[Z];
    }
}

/* moves all the vehicles forward, bouncing them off stuff when there is
   a collision.  Note the radar_bases are aimed also, so that we only
   have to update each radar bases world_object and bounding box once
   per frame. */

void move_vehicles( Player p[], Pylons *pylons, RadarBase *red_base,
                    RadarBase *blue_base, PointFace *base_laser )
{
    long i;
    CollisionTable c_table, new_c_table, new_new_c_table;

    /* move all the vehicles along their velocity vectors */

#ifdef PROFILE
    if( profile ) {
        printf("    move each vehicle forward, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    for( i = 0; i < 6; i++ ) {
        move_vehicle_forward( &(p[i].tank) );
    }

#ifdef PROFILE
    if( profile ) {
        printf("    init vehicles bbox and world_obj, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* initalize every vehicles world_object and bounding box which
       are needed to check for collisions */

    for( i = 0; i < 6; i++ ) {
        init_world_collision_obj( &(p[i].tank) );
        init_bounding_box( &(p[i].tank) );
    }

#ifdef PROFILE
    if( profile ) {
        printf("    aim radar bases, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* aim the both radar_bases now so that we only have to orient their
       objects once per frame. */

    aim_radar_base( red_base, p, base_laser );
    aim_radar_base( blue_base, p, base_laser );

#ifdef PROFILE
    if( profile ) {
        printf("    update radar bases, timer = %ld\n", PROFILE_CheckTimer());
    }
#endif

    /* initalize both radar_bases world_union_object and bounding boxes which
       is needed to check for collisions */

    update_radar_base( red_base );
    update_radar_base( blue_base );

    /* find collisions */

#ifdef PROFILE
    if( profile ) {
        printf("    build_collision_table(), timer = %ld\n", PROFILE_CheckTimer());
    }
#endif

    build_collision_table( c_table, p, pylons, red_base, blue_base );

    /* move colliding vehicles backward */

#ifdef PROFILE
    if( profile ) {
        printf("    move colliding vehicle backward, timer = %ld\n", PROFILE_CheckTimer());
    }
#endif

    for( i = 0; i < 6; i++ ) {
        if( c_table[i].collision ) {
            move_vehicle_backward( &(p[i].tank) );
        }
    }

    /* add bouncing vectors to all colliding vehicles */

#ifdef PROFILE
    if( profile ) {
        printf("    bounce_colliding_vehicles(), timer = %ld\n", PROFILE_CheckTimer());
    }
#endif

    bounce_colliding_vehicles( c_table, p );

    /* move colliding vehicles along their velocity vectors */

#ifdef PROFILE
    if( profile ) {
        printf("    move colliding vehicles forward, timer = %ld\n", PROFILE_CheckTimer());
    }
#endif

    for( i = 0; i < 6; i++ ) {
        if( c_table[i].collision ) {
            move_vehicle_forward( &(p[i].tank) );
            init_world_collision_obj( &(p[i].tank) );
            init_bounding_box( &(p[i].tank) );
        }
    }

    /* find the collisions resulting from the movement of the previously
       colliding vehicles */

#ifdef PROFILE
    if( profile ) {
        printf("    build_new_collision_table, timer = %ld\n", PROFILE_CheckTimer());
    }
#endif

    build_new_collision_table( c_table, new_c_table, p, pylons, red_base,
                               blue_base );

    /* move the new colliding vehicles backward */

#ifdef PROFILE
    if( profile ) {
        printf("    move stuck vehicle backward, timer = %ld\n", PROFILE_CheckTimer());
    }
#endif

    for( i = 0; i < 6; i++ ) {
        if( new_c_table[i].collision ) {
            /* printf("pre_force_vel = (%.5f,%.5f,%.5f)\n", p[i].tank.vel[X], p[i].tank.vel[Y], p[i].tank.vel[Z] );
            move_vehicle_backward( &(p[i].tank) ); */
            force_bounce_vehicle( &(p[i].tank), new_c_table[i].reflect );
            /* printf("p[%ld] : Force\n", i );
            printf("post_force_vel = (%.5f,%.5f,%.5f)\n", p[i].tank.vel[X], p[i].tank.vel[Y], p[i].tank.vel[Z] ); */
            move_vehicle_forward( &(p[i].tank) );
            init_world_collision_obj( &(p[i].tank) );
            init_bounding_box( &(p[i].tank) );
        }
    }

    build_new_collision_table( new_c_table, new_new_c_table, p, pylons,
                               red_base, blue_base );

    for( i = 0; i < 6; i++ ) {
        if( new_new_c_table[i].collision ) {
            move_vehicle_backward( &(p[i].tank) );
            /* printf("p[%ld] : Stuck!!!\n", i );
            printf("   vel = (%.5f,%.5f,%.5f)\n", p[i].tank.vel[X], p[i].tank.vel[Y], p[i].tank.vel[Z] ); */
        }
    }

    /* add forward thrust to vel of airborne vehicles */

    for( i = 0; i < 6; i++ ) {
        if( p[i].tank.vehicle_mode == Air ) {
            p[i].tank.vel[X] += p[i].tank.orient.front[X] * p[i].tank.air_inc_forward_speed;
            p[i].tank.vel[Y] += p[i].tank.orient.front[Y] * p[i].tank.air_inc_forward_speed;
            p[i].tank.vel[Z] += p[i].tank.orient.front[Z] * p[i].tank.air_inc_forward_speed;
        }
    }

    /* reduce every vehicles velocity by friction */

#ifdef PROFILE
    if( profile ) {
        printf("    reduce_by_friction(), timer = %ld\n", PROFILE_CheckTimer());
    }
#endif

    for( i = 0; i < 6; i++ ) {
        reduce_by_friction( p[i].tank.vehicle_mode, p[i].tank.vel );
    }
}

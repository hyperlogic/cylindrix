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

#ifndef AI_UTIL_H
#define AI_UTIL_H


/* 2d functions */


unsigned char Find_Nearest_Enemy_2D( Player player[], unsigned char index );

unsigned char Find_Nearest_Visible_Enemy_2D( Player player[],
                unsigned char index, Pylons *pylons );

unsigned char Find_Nearest_Visible_FOV_Enemy_2d( Player player[],
                unsigned char index, Pylons *pylons );

short Point_Angle_2D( Float_Point2d point, Orientation *o );

float Point_Distance_2D( Float_Point2d point, Float_Point2d position );

int Is_Visible_2D( Float_Point2d point, Float_Point2d position, Pylons *pylons );
int Old_Is_Visible_2D( Float_Point2d point, Float_Point2d position, Pylons *pylons );

int Is_Visible_FOV_2D( Float_Point point, Orientation *o, Pylons *pylons );

void Cylinder_x_y( Float_Point point, Float_Point2d point2d );


/* 3d functions */


unsigned char Find_Nearest_Enemy_3D( Player player[], unsigned char index );

unsigned char Find_Nearest_Visible_Enemy_3D( Player player[],
                unsigned char index, Pylons *pylons );

unsigned char Find_Nearest_Visible_FOV_Enemy_3D( Player player[],
                  unsigned char index, Pylons *pylons );

int Is_Visible_3D( Float_Point point, Float_Point pos, Pylons *pylons );

int Is_Visible_FOV_3D( Float_Point point, Orientation *o, Pylons *pylons );

float Point_Distance_3D( Float_Point u, Float_Point v );

short Horizontal_Point_Angle_3D( Float_Point point, Orientation *o );

short Vertical_Point_Angle_3D( Float_Point point, Orientation *o );

/* misc. */

void Sprint_State( char *state_string, state_type state, int index );

int inside_bounding_box2d( Float_Point2d point, BoundingBox2d *box );

void center( Point2d center, Point2d point, Point2d new_point );

int ill2( Float_Point2d p1, Float_Point2d p2, Float_Point2d q1,
          Float_Point2d q2, Float_Point2d v );

int fixed_ill2( Point2d p1, Point2d p2, Point2d q1, Point2d q2, Point2d v );

void iCylinder_x_y( Point point, Point2d point2d );

void Clear_Input_Table( input_table table );

void Copy_Point_3D( Float_Point point_one, Float_Point point_two );

void Pylon_Position_3D( Pylon *pylon, Float_Point point );

void Pylon_Position_2D( Pylon *pylon, Float_Point2d point );

void Radar_Leg( Orientation *o, Float_Point2d point );

int Is_Visible_2D_Pylon( Float_Point2d point, Float_Point2d position, Pylons *pylons, int pylon_index );

int Is_Visible_3D_Pylon( Float_Point point, Float_Point pos, Pylons *pylons, int pylon_index );

short Current_Direction_2D( WorldStuff *world_stuff, int index );

void Point_Square_2D( Float_Point2d point, int *row, int *column );
 
void Center_Of_2D( int row, int column, Float_Point2d point );

void Center_Of_3D( int row, int column, int height, Float_Point point );

void Point_Square_3D( Float_Point point, int *row, int *column, int *height );

int Recent_Damage( int index );

void Update_Bullet_Hit( short projectile_strength, int tank_number, Projectile *p );
 
int Find_Bullet_Owner( Projectile *p );

void update_leader( long view_vehicle );

int Decoys_Active( WorldStuff *world_stuff, int index );

int Return_Vehicle_Index( Vehicle *v );

#endif

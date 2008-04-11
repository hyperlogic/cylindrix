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
#include "pylon.h"

#ifndef COLLIDE_H
#define COLLIDE_H

void find_bounding_box( PointFace *obj, BoundingBox *box );
void print_bounding_box( BoundingBox *b );
void print_magic_bounding_box( MagicBoundingBox *b );
int pylon_collision( Pylons *p, Vehicle *v, long *face_index );
int super_pylon_collision( Pylons *p, Vehicle *v, Float_Vector reflect );
int player_collision( Player *player, Vehicle *v );
int super_player_collision( Player *player, Vehicle *v, Float_Vector reflect );
int pylon_projectile_collision( Pylons *pylons, Projectile *p, long *face_index );
int vehicle_projectile_collision( Vehicle *v, Projectile *p );
int player_projectile_collision( Player *player, Projectile *p, Vehicle *v );
void init_bounding_box( Vehicle *v );
void init_projectile_bounding_box( Projectile *p );
int cap_collision( Vehicle *v, Float_Vector reflect );
int cylinder_cap_collision( Vehicle *v, Float_Vector reflect );
int projectile_cylinder_collision( Projectile *p );
int super_radar_base_collision( RadarBase *base, Vehicle *v, Float_Vector reflect );
int radar_base_projectile_collision( RadarBase *base, Projectile *p, Vehicle *v );

int vehicle_collision( Vehicle *v, Pylons *pylons,
                       Player p[], RadarBase *red_base, RadarBase *blue_base,
                       Float_Vector reflect );

void build_collision_table( CollisionTable c_table,
                            Player p[], Pylons *pylons, RadarBase *red_base,
                            RadarBase *blue_base );

void build_new_collision_table( CollisionTable c_table,
                                CollisionTable new_table, Player p[],
                                Pylons *pylons, RadarBase *red_base,
                                RadarBase *blue_base );

int bounding_box_overlap_2d( BoundingBox2d *p, BoundingBox2d *q );
int bounding_box_overlap( BoundingBox *p, BoundingBox *q );
int pointface_edge_intersect( PointFace *obj, Point *edge_vert, EdgeTable *et,
                              long *face_index );
#endif


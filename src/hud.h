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

#ifndef HUD_H
#define HUD_H

void draw_hud( enum VehicleType type, PixMap *pixmap,
               Player *player_array, long view_vehicle, PointEdge *wire_tube,
               RadarBase *red_base, RadarBase *blue_base );
               
void draw_third_person_hud( Player *player_array, long view_vehicle, 
                            RadarBase *red_base, RadarBase *blue_base );

void draw_missile_box( Point upper_left, Point lower_right, unsigned char color );

void draw_cross_box( Point upper_left, Point lower_right, unsigned char color );

void draw_bar( BarInfo *barinfo );

void draw_horizontal_left_justified_bar( BarInfo *barinfo );

void draw_horizontal_right_justified_bar( BarInfo *barinfo );

void draw_vertical_top_justified_bar( BarInfo *barinfo );

void draw_vertical_bottom_justified_bar( BarInfo *barinfo );

void blit_screen( unsigned char *buffer );

void blit_pixmap( PixMap *pixmap, unsigned char new_cross_hair_color );

void init_pixmap( PixMap *pixmap, unsigned char *buffer );

#endif


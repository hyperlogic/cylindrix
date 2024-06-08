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

#include <stdio.h>
#include <stdlib.h>
#include "types.h"   /* important types and defines */

#include "hud.h"     /* function prototypes */

#include "prim.h"    /* need DB_Fast_Pixel() and DB_Scissor_Pixel() */

#include "radar.h"   /* need view_radar() */

#include "object.h"  /* need translation_matrix_init(), matrix_mult(),
                        linear_transformation1(), scale_transformation(),
                        linear_transformation(), projection_transformation(),
                        wtov_transformation */

#include "util.h"     /* need ftom() */
#include "text.h"     /* need to blit text */ 
#include "commands.h" /* need to blit each wingman's orders */

/* This file contains functions to draw the heads up display.  The heads up
   display includes : a background pcx picture, the radar, gauges that relay
   important info, and box to be drawn around the view vehicles current
   missile target.
*/

#define TRANSPARENT_COLOR 0  /* this will indicate the invisible color */
#define CROSS_HAIR_COLOR 2   /* this is color of the pixels used in the cross hair */

/* The following externs are declared in object.c, and are needed inside of
   draw_hud. */

extern unsigned char *double_buffer;
extern Matrix scale_matrix;
extern Matrix inv_scale_matrix;
extern Matrix projection_matrix;
extern Matrix wtov_matrix;
extern int profile;

extern WorldStuff world_stuff;

extern MenuStuff menu_stuff;

extern command_handler_type red_commands;
extern command_handler_type blue_commands;

/* Draws the entire heads up display into the double_buffer.

    VehicleType type : type of vehicle who's hud we're drawing

    unsigned char *buffer : pointer to this hud's background picture

    Player *player_array : array containing all six players

    long view_vehicle : index into the player_array that indicates the vehicle
                        who's hud we're drawing

    PointEdge *wire_tube : pointer to the wire_tube object that is used when
                           the radar is drawn.

    RadarBase *red_base : pointer to the red_teams radar base.

    RadarBase *blue_base : pointer to the blue_teams radar base.
*/


void draw_hud( enum VehicleType type, PixMap *pixmap,
               Player *player_array, long view_vehicle, PointEdge *wire_tube,
               RadarBase *red_base, RadarBase *blue_base )
{
    Point p;                    /* the fixed-point version of the view_vehicles position */
    Point target;               /* used to transform the target into screen coords */
    Point upper_left;           /* the upper_left hand corner of the missile lock box */
    Point lower_right;          /* the lower_right hand corner of the missile lock box */

    Matrix translation_matrix;  /* These three matrices are used to transform */
    Matrix rotation_matrix;     /* the target and the corners of the missile */
    Matrix temp_matrix;         /* lock box into screen coords. */

    Point2d radar_offset;          /* used to place the radar on the screen */
    Point2d message_offset;        /* used to display messages */
    Point2d wing_one_order_offset; /* used to anchor the wingman order text */
    Point2d wing_two_order_offset; /* used to anchor the wingman order text */

    long wing_one_index;        /* the player_array index of wingman 1 */
    long wing_two_index;        /* the player_array index of wingman 2 */

    long i;

    long found_index_one;       /* tells us if we have found wingman 1 */

    BarInfo wing_one_bar;       /* needed to draw the hitpoints of wingman 1 */
    BarInfo wing_two_bar;       /* needed to draw the hitpoints of wingman 2 */
    BarInfo hitpoints_bar;      /* needed to draw this vehicles hitpoints */
    BarInfo missiles_bar;       /* needed to draw this vehicles missile storage */
    BarInfo damage_bar;         /* needed to draw this vehicles laser_damage */

    unsigned char missile_box_color;

	char temp_str[80];   /* used to print out orders */
    
    /* draw hud bitmap background on the screen */

#ifdef PROFILE
    if ( profile ) {
        printf("    blit_screen( pixmap ), timer = %ld\n", PROFILE_CheckTimer() );
		ptime = SYS_GetTimeMicro();
    }
#endif

    blit_pixmap( pixmap, 2 /* green for the cross-hair */ );

#ifdef PROFILE
    if( profile ) {
        printf("    find view_vehicles wingmen, timer = %ld\n", PROFILE_CheckTimer() );
		ptime = SYS_GetTimeMicro();
    }
#endif
    /* find view_vehicles wingmen */

    wing_one_index = 0;
    wing_two_index = 0;
    found_index_one = FALSE;

    for( i = 0; i < 6; i++ ) {
        if( (i != view_vehicle) && !found_index_one && (player_array[i].tank.team == player_array[view_vehicle].tank.team) ) {
            wing_one_index = i;
            found_index_one = TRUE;
        }
        else if( (i != view_vehicle) && found_index_one && (player_array[i].tank.team == player_array[view_vehicle].tank.team) ) {
            wing_two_index = i;
            break;
        }
    }

#ifdef PROFILE
    if( profile ) {
        printf("    initialize bars, timer = %ld\n", PROFILE_CheckTimer() );
		ptime = SYS_GetTimeMicro();
    }
#endif

    if( type == Beetle ) {

        /* initialize the wingman 1 bar */
        wing_one_bar.upper_left[X] = 295;
        wing_one_bar.upper_left[Y] = 28;
        wing_one_bar.lower_right[X] = 304;
        wing_one_bar.lower_right[Y] = 114;
        wing_one_bar.min_val = 0;
        wing_one_bar.max_val = player_array[wing_one_index].tank.max_hitpoints;
        wing_one_bar.val = player_array[wing_one_index].tank.current_hitpoints;
        wing_one_bar.bar_color = 160;
        wing_one_bar.outline_color = 79;
        wing_one_bar.type = VerticalBottomJustified;

        /* initialize the wingman 2 bar */

        wing_two_bar.upper_left[X] = 309;
        wing_two_bar.upper_left[Y] = 28;
        wing_two_bar.lower_right[X] = 318;
        wing_two_bar.lower_right[Y] = 114;
        wing_two_bar.min_val = 0;
        wing_two_bar.max_val = player_array[wing_two_index].tank.max_hitpoints;
        wing_two_bar.val = player_array[wing_two_index].tank.current_hitpoints;
        wing_two_bar.bar_color = 160;
        wing_two_bar.outline_color = 79;
        wing_two_bar.type = VerticalBottomJustified;

        /* initialize the hitpoints gauge */

        hitpoints_bar.upper_left[X] = 31;
        hitpoints_bar.upper_left[Y] = 182;
        hitpoints_bar.lower_right[X] = 204;
        hitpoints_bar.lower_right[Y] = 188;
        hitpoints_bar.min_val = 0;
        hitpoints_bar.max_val = player_array[view_vehicle].tank.max_hitpoints;
        hitpoints_bar.val = player_array[view_vehicle].tank.current_hitpoints;
        hitpoints_bar.bar_color = 160;
        hitpoints_bar.outline_color = 79;
        hitpoints_bar.type = HorizontalRightJustified;

        /* initialize missiles gauge */

        missiles_bar.upper_left[X] = 31;
        missiles_bar.upper_left[Y] = 192;
        missiles_bar.lower_right[X] = 204;
        missiles_bar.lower_right[Y] = 198;
        missiles_bar.min_val = 0;
        missiles_bar.max_val = player_array[view_vehicle].tank.max_missile_storage;
        missiles_bar.val = player_array[view_vehicle].tank.missiles_stored;
        missiles_bar.bar_color = 160;
        missiles_bar.outline_color = 79;
        missiles_bar.type = HorizontalRightJustified;

        /* initialize the laser_damage bar */

        damage_bar.upper_left[X] = 31;
        damage_bar.upper_left[Y] = 172;
        damage_bar.lower_right[X] = 204;
        damage_bar.lower_right[Y] = 178;
        damage_bar.min_val = 0;
        damage_bar.max_val = 10;
        damage_bar.val = player_array[view_vehicle].tank.laser_damage;
        damage_bar.bar_color = 160;
        damage_bar.outline_color = 79;
        damage_bar.type = HorizontalRightJustified;

        /* initialize the radar_offset */

        radar_offset[X] = 113;
        radar_offset[Y] = 67;
        
        /* initalize the message_offset */
        
        message_offset[X] = 170;
        message_offset[Y] = 2;
        
        /* initalize the wing_one_order_offset */
        
        wing_one_order_offset[X] = 20;
        wing_one_order_offset[Y] = 2;
        
        /* initalize the wing_two_order_offset */
        
        wing_two_order_offset[X] = 20;
        wing_two_order_offset[Y] = 13;
        
    }
    else if( type == Wasp ) {

        /* initialize the wingman 1 bar */

        wing_one_bar.upper_left[X] = 295;
        wing_one_bar.upper_left[Y] = 134;
        wing_one_bar.lower_right[X] = 304;
        wing_one_bar.lower_right[Y] = 188;
        wing_one_bar.min_val = 0;
        wing_one_bar.max_val = player_array[wing_one_index].tank.max_hitpoints;
        wing_one_bar.val = player_array[wing_one_index].tank.current_hitpoints;
        wing_one_bar.bar_color = 160;
        wing_one_bar.outline_color = 79;
        wing_one_bar.type = VerticalBottomJustified;

        /* initialize the wingman 2 bar */

        wing_two_bar.upper_left[X] = 309;
        wing_two_bar.upper_left[Y] = 134;
        wing_two_bar.lower_right[X] = 318;
        wing_two_bar.lower_right[Y] = 188;
        wing_two_bar.min_val = 0;
        wing_two_bar.max_val = player_array[wing_two_index].tank.max_hitpoints;
        wing_two_bar.val = player_array[wing_two_index].tank.current_hitpoints;
        wing_two_bar.bar_color = 160;
        wing_two_bar.outline_color = 79;
        wing_two_bar.type = VerticalBottomJustified;

        /* initialize the hitpoints gauge */

        hitpoints_bar.upper_left[X] = 267;
        hitpoints_bar.upper_left[Y] = 134;
        hitpoints_bar.lower_right[X] = 276;
        hitpoints_bar.lower_right[Y] = 188;
        hitpoints_bar.min_val = 0;
        hitpoints_bar.max_val = player_array[view_vehicle].tank.max_hitpoints;
        hitpoints_bar.val = player_array[view_vehicle].tank.current_hitpoints;
        hitpoints_bar.bar_color = 160;
        hitpoints_bar.outline_color = 79;
        hitpoints_bar.type = VerticalBottomJustified;

        /* initialize missiles gauge */

        missiles_bar.upper_left[X] = 281;
        missiles_bar.upper_left[Y] = 134;
        missiles_bar.lower_right[X] = 290;
        missiles_bar.lower_right[Y] = 188;
        missiles_bar.min_val = 0;
        missiles_bar.max_val = player_array[view_vehicle].tank.max_missile_storage;
        missiles_bar.val = player_array[view_vehicle].tank.missiles_stored;
        missiles_bar.bar_color = 160;
        missiles_bar.outline_color = 79;
        missiles_bar.type = VerticalBottomJustified;

        /* initialize the laser_damage bar */

        damage_bar.upper_left[X] = 253;
        damage_bar.upper_left[Y] = 134;
        damage_bar.lower_right[X] = 262;
        damage_bar.lower_right[Y] = 188;
        damage_bar.min_val = 0;
        damage_bar.max_val = 10;
        damage_bar.val = player_array[view_vehicle].tank.laser_damage;
        damage_bar.bar_color = 160;
        damage_bar.outline_color = 79;
        damage_bar.type = VerticalBottomJustified;

        /* initialize the radar_offset */

        radar_offset[X] = -113;
        radar_offset[Y] = 67;
        
        /* initalize the message_offset */
        
        message_offset[X] = 90;
        message_offset[Y] = 171;
        
        /* initalize the wing_one_order_offset */
        
        wing_one_order_offset[X] = 110;
        wing_one_order_offset[Y] = 182;
        
        /* initalize the wing_two_order_offset */
        
        wing_two_order_offset[X] = 110;
        wing_two_order_offset[Y] = 193;

    }
    else if( type == Flea ) {

        /* initialize the wingman 1 bar */

        wing_one_bar.upper_left[X] = 208;
        wing_one_bar.upper_left[Y] = 151;
        wing_one_bar.lower_right[X] = 217;
        wing_one_bar.lower_right[Y] = 188;
        wing_one_bar.min_val = 0;
        wing_one_bar.max_val = player_array[wing_one_index].tank.max_hitpoints;
        wing_one_bar.val = player_array[wing_one_index].tank.current_hitpoints;
        wing_one_bar.bar_color = 160;
        wing_one_bar.outline_color = 79;
        wing_one_bar.type = VerticalBottomJustified;

        /* initialize the wingman 2 bar */

        wing_two_bar.upper_left[X] = 222;
        wing_two_bar.upper_left[Y] = 151;
        wing_two_bar.lower_right[X] = 231;
        wing_two_bar.lower_right[Y] = 188;
        wing_two_bar.min_val = 0;
        wing_two_bar.max_val = player_array[wing_two_index].tank.max_hitpoints;
        wing_two_bar.val = player_array[wing_two_index].tank.current_hitpoints;
        wing_two_bar.bar_color = 160;
        wing_two_bar.outline_color = 79;
        wing_two_bar.type = VerticalBottomJustified;

        /* initialize the hitpoints gauge */

        hitpoints_bar.upper_left[X] = 20;
        hitpoints_bar.upper_left[Y] = 11;
        hitpoints_bar.lower_right[X] = 134;
        hitpoints_bar.lower_right[Y] = 17;
        hitpoints_bar.min_val = 0;
        hitpoints_bar.max_val = player_array[view_vehicle].tank.max_hitpoints;
        hitpoints_bar.val = player_array[view_vehicle].tank.current_hitpoints;
        hitpoints_bar.bar_color = 160;
        hitpoints_bar.outline_color = 79;
        hitpoints_bar.type = HorizontalLeftJustified;

        /* initialize missiles gauge */

        missiles_bar.upper_left[X] = 20;
        missiles_bar.upper_left[Y] = 21;
        missiles_bar.lower_right[X] = 84;
        missiles_bar.lower_right[Y] = 27;
        missiles_bar.min_val = 0;
        missiles_bar.max_val = player_array[view_vehicle].tank.max_missile_storage;
        missiles_bar.val = player_array[view_vehicle].tank.missiles_stored;
        missiles_bar.bar_color = 160;
        missiles_bar.outline_color = 79;
        missiles_bar.type = HorizontalLeftJustified;

        /* initialize the laser_damage bar */

        damage_bar.upper_left[X] = 20;
        damage_bar.upper_left[Y] = 1;
        damage_bar.lower_right[X] = 193;
        damage_bar.lower_right[Y] = 7;
        damage_bar.min_val = 0;
        damage_bar.max_val = 10;
        damage_bar.val = player_array[view_vehicle].tank.laser_damage;
        damage_bar.bar_color = 160;
        damage_bar.outline_color = 79;
        damage_bar.type = HorizontalLeftJustified;

        /* initialize the radar_offset */

        radar_offset[X] = 113;
        radar_offset[Y] = 67;
        
        /* initalize the message_offset */
        
        message_offset[X] = 2;
        message_offset[Y] = 170;
        
        /* initalize the wing_one_order_offset */
        
        wing_one_order_offset[X] = 20;
        wing_one_order_offset[Y] = 181;
        
        /* initalize the wing_two_order_offset */
        
        wing_two_order_offset[X] = 20;
        wing_two_order_offset[Y] = 192;
        
    }
    else if( type == Dragonfly ) {

        /* initialize the wingman 1 bar */

        wing_one_bar.upper_left[X] = 1;
        wing_one_bar.upper_left[Y] = 36;
        wing_one_bar.lower_right[X] = 10;
        wing_one_bar.lower_right[Y] = 122;
        wing_one_bar.min_val = 0;
        wing_one_bar.max_val = player_array[wing_one_index].tank.max_hitpoints;
        wing_one_bar.val = player_array[wing_one_index].tank.current_hitpoints;
        wing_one_bar.bar_color = 160;
        wing_one_bar.outline_color = 79;
        wing_one_bar.type = VerticalBottomJustified;

        /* initialize the wingman 2 bar */

        wing_two_bar.upper_left[X] = 15;
        wing_two_bar.upper_left[Y] = 36;
        wing_two_bar.lower_right[X] = 24;
        wing_two_bar.lower_right[Y] = 122;
        wing_two_bar.min_val = 0;
        wing_two_bar.max_val = player_array[wing_two_index].tank.max_hitpoints;
        wing_two_bar.val = player_array[wing_two_index].tank.current_hitpoints;
        wing_two_bar.bar_color = 160;
        wing_two_bar.outline_color = 79;
        wing_two_bar.type = VerticalBottomJustified;

        /* initialize the hitpoints gauge */

        hitpoints_bar.upper_left[X] = 107;
        hitpoints_bar.upper_left[Y] = 182;
        hitpoints_bar.lower_right[X] = 280;
        hitpoints_bar.lower_right[Y] = 188;
        hitpoints_bar.min_val = 0;
        hitpoints_bar.max_val = player_array[view_vehicle].tank.max_hitpoints;
        hitpoints_bar.val = player_array[view_vehicle].tank.current_hitpoints;
        hitpoints_bar.bar_color = 160;
        hitpoints_bar.outline_color = 79;
        hitpoints_bar.type = HorizontalLeftJustified;

        /* initialize missiles gauge */

        missiles_bar.upper_left[X] = 107;
        missiles_bar.upper_left[Y] = 192;
        missiles_bar.lower_right[X] = 280;
        missiles_bar.lower_right[Y] = 198;
        missiles_bar.min_val = 0;
        missiles_bar.max_val = player_array[view_vehicle].tank.max_missile_storage;
        missiles_bar.val = player_array[view_vehicle].tank.missiles_stored;
        missiles_bar.bar_color = 160;
        missiles_bar.outline_color = 79;
        missiles_bar.type = HorizontalLeftJustified;

        /* initialize the laser_damage bar */

        damage_bar.upper_left[X] = 107;
        damage_bar.upper_left[Y] = 172;
        damage_bar.lower_right[X] = 280;
        damage_bar.lower_right[Y] = 178;
        damage_bar.min_val = 0;
        damage_bar.max_val = 10;
        damage_bar.val = player_array[view_vehicle].tank.laser_damage;
        damage_bar.bar_color = 160;
        damage_bar.outline_color = 79;
        damage_bar.type = HorizontalLeftJustified;

        /* initialize the radar_offset */

        radar_offset[X] = -113;
        radar_offset[Y] = 67;
        
        /* initalize the message_offset */
        
        message_offset[X] = 50;
        message_offset[Y] = 2;
        
        /* initalize the wing_one_order_offset */
        
        wing_one_order_offset[X] = 230;
        wing_one_order_offset[Y] = 2;
        
        /* initalize the wing_two_order_offset */
        
        wing_two_order_offset[X] = 230;
        wing_two_order_offset[Y] = 13;

    }
    else if( type == Mosquito ) {

        /* initialize the wingman 1 bar */

        wing_one_bar.upper_left[X] = 295;
        wing_one_bar.upper_left[Y] = 34;
        wing_one_bar.lower_right[X] = 304;
        wing_one_bar.lower_right[Y] = 120;
        wing_one_bar.min_val = 0;
        wing_one_bar.max_val = player_array[wing_one_index].tank.max_hitpoints;
        wing_one_bar.val = player_array[wing_one_index].tank.current_hitpoints;
        wing_one_bar.bar_color = 160;
        wing_one_bar.outline_color = 79;
        wing_one_bar.type = VerticalBottomJustified;

        /* initialize the wingman 2 bar */

        wing_two_bar.upper_left[X] = 308;
        wing_two_bar.upper_left[Y] = 34;
        wing_two_bar.lower_right[X] = 317;
        wing_two_bar.lower_right[Y] = 129;
        wing_two_bar.min_val = 0;
        wing_two_bar.max_val = player_array[wing_two_index].tank.max_hitpoints;
        wing_two_bar.val = player_array[wing_two_index].tank.current_hitpoints;
        wing_two_bar.bar_color = 160;
        wing_two_bar.outline_color = 79;
        wing_two_bar.type = VerticalBottomJustified;

        /* initialize the hitpoints gauge */

        hitpoints_bar.upper_left[X] = 109;
        hitpoints_bar.upper_left[Y] = 181;
        hitpoints_bar.lower_right[X] = 220;
        hitpoints_bar.lower_right[Y] = 187;
        hitpoints_bar.min_val = 0;
        hitpoints_bar.max_val = player_array[view_vehicle].tank.max_hitpoints;
        hitpoints_bar.val = player_array[view_vehicle].tank.current_hitpoints;
        hitpoints_bar.bar_color = 160;
        hitpoints_bar.outline_color = 79;
        hitpoints_bar.type = HorizontalRightJustified;

        /* initialize missiles gauge */

        missiles_bar.upper_left[X] = 109;
        missiles_bar.upper_left[Y] = 192;
        missiles_bar.lower_right[X] = 228;
        missiles_bar.lower_right[Y] = 198;
        missiles_bar.min_val = 0;
        missiles_bar.max_val = player_array[view_vehicle].tank.max_missile_storage;
        missiles_bar.val = player_array[view_vehicle].tank.missiles_stored;
        missiles_bar.bar_color = 160;
        missiles_bar.outline_color = 79;
        missiles_bar.type = HorizontalRightJustified;

        /* initialize the laser_damage bar */

        damage_bar.upper_left[X] = 19;
        damage_bar.upper_left[Y] = 1;
        damage_bar.lower_right[X] = 192;
        damage_bar.lower_right[Y] = 7;
        damage_bar.min_val = 0;
        damage_bar.max_val = 10;
        damage_bar.val = player_array[view_vehicle].tank.laser_damage;
        damage_bar.bar_color = 160;
        damage_bar.outline_color = 79;
        damage_bar.type = HorizontalLeftJustified;

        /* initialize the radar_offset */

        radar_offset[X] = 113;
        radar_offset[Y] = 67;
        
        /* initalize the message_offset */
        
        message_offset[X] = 2;
        message_offset[Y] = 171;
        
        /* initalize the wing_one_order_offset */
        
        wing_one_order_offset[X] = 20;
        wing_one_order_offset[Y] = 182;
        
        /* initalize the wing_two_order_offset */
        
        wing_two_order_offset[X] = 20;
        wing_two_order_offset[Y] = 193;
        
    }
    else if( type == Roach ) {

        /* initialize the wingman 1 bar */

        wing_one_bar.upper_left[X] = 309;
        wing_one_bar.upper_left[Y] = 80;
        wing_one_bar.lower_right[X] = 318;
        wing_one_bar.lower_right[Y] = 138;
        wing_one_bar.min_val = 0;
        wing_one_bar.max_val = player_array[wing_one_index].tank.max_hitpoints;
        wing_one_bar.val = player_array[wing_one_index].tank.current_hitpoints;
        wing_one_bar.bar_color = 160;
        wing_one_bar.outline_color = 79;
        wing_one_bar.type = VerticalBottomJustified;

        /* initialize the wingman 2 bar */

        wing_two_bar.upper_left[X] = 309;
        wing_two_bar.upper_left[Y] = 2;
        wing_two_bar.lower_right[X] = 318;
        wing_two_bar.lower_right[Y] = 66;
        wing_two_bar.min_val = 0;
        wing_two_bar.max_val = player_array[wing_two_index].tank.max_hitpoints;
        wing_two_bar.val = player_array[wing_two_index].tank.current_hitpoints;
        wing_two_bar.bar_color = 160;
        wing_two_bar.outline_color = 79;
        wing_two_bar.type = VerticalBottomJustified;

        /* initialize the hitpoints gauge */

        hitpoints_bar.upper_left[X] = 105;
        hitpoints_bar.upper_left[Y] = 192;
        hitpoints_bar.lower_right[X] = 166;
        hitpoints_bar.lower_right[Y] = 198;
        hitpoints_bar.min_val = 0;
        hitpoints_bar.max_val = player_array[view_vehicle].tank.max_hitpoints;
        hitpoints_bar.val = player_array[view_vehicle].tank.current_hitpoints;
        hitpoints_bar.bar_color = 160;
        hitpoints_bar.outline_color = 79;
        hitpoints_bar.type = HorizontalLeftJustified;

        /* initialize missiles gauge */

        missiles_bar.upper_left[X] = 188;
        missiles_bar.upper_left[Y] = 192;
        missiles_bar.lower_right[X] = 249;
        missiles_bar.lower_right[Y] = 198;
        missiles_bar.min_val = 0;
        missiles_bar.max_val = player_array[view_vehicle].tank.max_missile_storage;
        missiles_bar.val = player_array[view_vehicle].tank.missiles_stored;
        missiles_bar.bar_color = 160;
        missiles_bar.outline_color = 79;
        missiles_bar.type = HorizontalLeftJustified;

        /* initialize the laser_damage bar */

        damage_bar.upper_left[X] = 19;
        damage_bar.upper_left[Y] = 192;
        damage_bar.lower_right[X] = 83;
        damage_bar.lower_right[Y] = 198;
        damage_bar.min_val = 0;
        damage_bar.max_val = 10;
        damage_bar.val = player_array[view_vehicle].tank.laser_damage;
        damage_bar.bar_color = 160;
        damage_bar.outline_color = 79;
        damage_bar.type = HorizontalLeftJustified;

        /* initialize the radar_offset */

        radar_offset[X] = 113;
        radar_offset[Y] = 67;
        
        /* initalize the message_offset */
        
        message_offset[X] = 2;
        message_offset[Y] = 182;
        
        /* initalize the wing_one_order_offset */
        
        wing_one_order_offset[X] = 20;
        wing_one_order_offset[Y] = 2;
        
        /* initalize the wing_two_order_offset */
        
        wing_two_order_offset[X] = 133;
        wing_two_order_offset[Y] = 2;
        
    }
    else if( type == Spider ) {

        /* initialize the wingman 1 bar */

        wing_one_bar.upper_left[X] = 95;
        wing_one_bar.upper_left[Y] = 134;
        wing_one_bar.lower_right[X] = 104;
        wing_one_bar.lower_right[Y] = 188;
        wing_one_bar.min_val = 0;
        wing_one_bar.max_val = player_array[wing_one_index].tank.max_hitpoints;
        wing_one_bar.val = player_array[wing_one_index].tank.current_hitpoints;
        wing_one_bar.bar_color = 160;
        wing_one_bar.outline_color = 79;
        wing_one_bar.type = VerticalBottomJustified;

        /* initialize the wingman 2 bar */

        wing_two_bar.upper_left[X] = 109;
        wing_two_bar.upper_left[Y] = 134;
        wing_two_bar.lower_right[X] = 118;
        wing_two_bar.lower_right[Y] = 188;
        wing_two_bar.min_val = 0;
        wing_two_bar.max_val = player_array[wing_two_index].tank.max_hitpoints;
        wing_two_bar.val = player_array[wing_two_index].tank.current_hitpoints;
        wing_two_bar.bar_color = 160;
        wing_two_bar.outline_color = 79;
        wing_two_bar.type = VerticalBottomJustified;

        /* initialize the hitpoints gauge */

        hitpoints_bar.upper_left[X] = 294;
        hitpoints_bar.upper_left[Y] = 134;
        hitpoints_bar.lower_right[X] = 303;
        hitpoints_bar.lower_right[Y] = 188;
        hitpoints_bar.min_val = 0;
        hitpoints_bar.max_val = player_array[view_vehicle].tank.max_hitpoints;
        hitpoints_bar.val = player_array[view_vehicle].tank.current_hitpoints;
        hitpoints_bar.bar_color = 160;
        hitpoints_bar.outline_color = 79;
        hitpoints_bar.type = VerticalBottomJustified;

        /* initialize missiles gauge */

        missiles_bar.upper_left[X] = 308;
        missiles_bar.upper_left[Y] = 134;
        missiles_bar.lower_right[X] = 317;
        missiles_bar.lower_right[Y] = 188;
        missiles_bar.min_val = 0;
        missiles_bar.max_val = player_array[view_vehicle].tank.max_missile_storage;
        missiles_bar.val = player_array[view_vehicle].tank.missiles_stored;
        missiles_bar.bar_color = 160;
        missiles_bar.outline_color = 79;
        missiles_bar.type = VerticalBottomJustified;

        /* initialize the laser_damage bar */

        damage_bar.upper_left[X] = 280;
        damage_bar.upper_left[Y] = 134;
        damage_bar.lower_right[X] = 289;
        damage_bar.lower_right[Y] = 188;
        damage_bar.min_val = 0;
        damage_bar.max_val = 10;
        damage_bar.val = player_array[view_vehicle].tank.laser_damage;
        damage_bar.bar_color = 160;
        damage_bar.outline_color = 79;
        damage_bar.type = VerticalBottomJustified;

        /* initialize the radar_offset */

        radar_offset[X] = -113;
        radar_offset[Y] = 67;
        
        /* initalize the message_offset */
        
        message_offset[X] = 125;
        message_offset[Y] = 171;
        
        /* initalize the wing_one_order_offset */
        
        wing_one_order_offset[X] = 141;
        wing_one_order_offset[Y] = 182;
        
        /* initalize the wing_two_order_offset */
        
        wing_two_order_offset[X] = 141;
        wing_two_order_offset[Y] = 193;
        
    }
    else if( type == Locust ) {

        /* initialize the wingman 1 bar */

        wing_one_bar.upper_left[X] = 295;
        wing_one_bar.upper_left[Y] = 149;
        wing_one_bar.lower_right[X] = 304;
        wing_one_bar.lower_right[Y] = 188;
        wing_one_bar.min_val = 0;
        wing_one_bar.max_val = player_array[wing_one_index].tank.max_hitpoints;
        wing_one_bar.val = player_array[wing_one_index].tank.current_hitpoints;
        wing_one_bar.bar_color = 160;
        wing_one_bar.outline_color = 79;
        wing_one_bar.type = VerticalBottomJustified;

        /* initialize the wingman 2 bar */

        wing_two_bar.upper_left[X] = 309;
        wing_two_bar.upper_left[Y] = 149;
        wing_two_bar.lower_right[X] = 318;
        wing_two_bar.lower_right[Y] = 188;
        wing_two_bar.min_val = 0;
        wing_two_bar.max_val = player_array[wing_two_index].tank.max_hitpoints;
        wing_two_bar.val = player_array[wing_two_index].tank.current_hitpoints;
        wing_two_bar.bar_color = 160;
        wing_two_bar.outline_color = 79;
        wing_two_bar.type = VerticalBottomJustified;

        /* initialize the hitpoints gauge */

        hitpoints_bar.upper_left[X] = 37;
        hitpoints_bar.upper_left[Y] = 192;
        hitpoints_bar.lower_right[X] = 125;
        hitpoints_bar.lower_right[Y] = 198;
        hitpoints_bar.min_val = 0;
        hitpoints_bar.max_val = player_array[view_vehicle].tank.max_hitpoints;
        hitpoints_bar.val = player_array[view_vehicle].tank.current_hitpoints;
        hitpoints_bar.bar_color = 160;
        hitpoints_bar.outline_color = 79;
        hitpoints_bar.type = HorizontalLeftJustified;

        /* initialize missiles gauge */

        missiles_bar.upper_left[X] = 148;
        missiles_bar.upper_left[Y] = 192;
        missiles_bar.lower_right[X] = 238;
        missiles_bar.lower_right[Y] = 198;
        missiles_bar.min_val = 0;
        missiles_bar.max_val = player_array[view_vehicle].tank.max_missile_storage;
        missiles_bar.val = player_array[view_vehicle].tank.missiles_stored;
        missiles_bar.bar_color = 160;
        missiles_bar.outline_color = 79;
        missiles_bar.type = HorizontalLeftJustified;

        /* initialize the laser_damage bar */

        damage_bar.upper_left[X] = 53;
        damage_bar.upper_left[Y] = 1;
        damage_bar.lower_right[X] = 160;
        damage_bar.lower_right[Y] = 7;
        damage_bar.min_val = 0;
        damage_bar.max_val = 10;
        damage_bar.val = player_array[view_vehicle].tank.laser_damage;
        damage_bar.bar_color = 160;
        damage_bar.outline_color = 79;
        damage_bar.type = HorizontalLeftJustified;

        /* initialize the radar_offset */

        radar_offset[X] = 113;
        radar_offset[Y] = -67;
        
        /* initalize the message_offset */
        
        message_offset[X] = 20;
        message_offset[Y] = 156;
        
        /* initalize the wing_one_order_offset */
        
        wing_one_order_offset[X] = 39;
        wing_one_order_offset[Y] = 167;
        
        /* initalize the wing_two_order_offset */
        
        wing_two_order_offset[X] = 39;
        wing_two_order_offset[Y] = 178;
        
    }

#ifdef PROFILE
    if( profile ) {
        printf("    draw_bar's, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* draw the bars */

    draw_bar( &wing_one_bar );
    draw_bar( &wing_two_bar );
    draw_bar( &hitpoints_bar );
    draw_bar( &missiles_bar );
    draw_bar( &damage_bar );

    /* draw the radar */

#ifdef PROFILE
    if( profile ) {
        printf("    view_radar, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif
    
    /* draw the message if needed */
    
    if( player_array[view_vehicle].tank.cloaking_active ) {
    
        micro_string_blit( "CLOAKING ACTIVE", message_offset[X], message_offset[Y],
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, 4 );
    }
    
    /* draw wingman's orders */    
    
    if( player_array[wing_one_index].tank.alive ) {
    
        /* print index 0 as wingman one's orders */
        
        if( player_array[wing_one_index].team == RED_TEAM ) {
            if( red_commands.command_active[0] ) {
                Sprint_Command( temp_str, red_commands.command[0] );
            }
            else {
                strcpy( temp_str, "NO ORDERS" );
            }
        }
        else {
            if( blue_commands.command_active[0] ) {
                Sprint_Command( temp_str, blue_commands.command[0] );
            }
            else {
                strcpy( temp_str, "NO ORDERS" );
            }        
        }
        
        micro_string_blit( temp_str, wing_one_order_offset[X], wing_one_order_offset[Y],
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, 4 );

        
        if( player_array[wing_two_index].tank.alive ) {
        
            /* print index 1 as wingman two's orders */
            
            if( player_array[wing_two_index].team == RED_TEAM ) {
                if( red_commands.command_active[1] ) {
                    Sprint_Command( temp_str, red_commands.command[1] );
                }
                else {
                    strcpy( temp_str, "NO ORDERS" );
                }
            }
            else {
                if( blue_commands.command_active[1] ) {
                    Sprint_Command( temp_str, blue_commands.command[1] );
                }
                else {
                    strcpy( temp_str, "NO ORDERS" );
                }        
            }
        
            micro_string_blit( temp_str, wing_two_order_offset[X], wing_two_order_offset[Y],
                               menu_stuff.micro_text.buffer,
                               menu_stuff.micro_text.xpixels + 1,
                               menu_stuff.micro_text.ypixels + 1, 4 );
            
        }
        else {
        
            /* print wingman two is dead */
            
            strcpy( temp_str, "DEAD" );
            
            micro_string_blit( temp_str, wing_two_order_offset[X], wing_two_order_offset[Y],
                               menu_stuff.micro_text.buffer,
                               menu_stuff.micro_text.xpixels + 1,
                               menu_stuff.micro_text.ypixels + 1, 4 );
            
        }
        
    }
    else if( player_array[wing_two_index].tank.alive ) {
    
        /* print wingman one is dead */
        
        strcpy( temp_str, "DEAD" );
            
        micro_string_blit( temp_str, wing_one_order_offset[X], wing_one_order_offset[Y],
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, 4 );
    
        /* print index 0 as wingman two's orders */
        
        if( player_array[wing_two_index].team == RED_TEAM ) {
            if( red_commands.command_active[0] ) {
                Sprint_Command( temp_str, red_commands.command[0] );
            }
            else {
                strcpy( temp_str, "NO ORDERS" );
            }
        }
        else {
            if( blue_commands.command_active[0] ) {
                Sprint_Command( temp_str, blue_commands.command[0] );
            }
            else {
                strcpy( temp_str, "NO ORDERS" );
            }        
        }
        
        micro_string_blit( temp_str, wing_two_order_offset[X], wing_two_order_offset[Y],
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, 4 );        
    }
    else {
    
        /* print wingman one is dead */
        
        strcpy( temp_str, "DEAD" );
            
        micro_string_blit( temp_str, wing_one_order_offset[X], wing_one_order_offset[Y],
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, 4 );

        
        /* print wingman two is dead */
        
        strcpy( temp_str, "DEAD" );
            
        micro_string_blit( temp_str, wing_two_order_offset[X], wing_two_order_offset[Y],
                           menu_stuff.micro_text.buffer,
                           menu_stuff.micro_text.xpixels + 1,
                           menu_stuff.micro_text.ypixels + 1, 4 );
        
    }
    
    /* draw the radar */

    if( (player_array[view_vehicle].tank.team == RED_TEAM) && (red_base->alive) ) 
	{
        view_radar( wire_tube, player_array, view_vehicle, radar_offset );
    }
    else if( (player_array[view_vehicle].tank.team == BLUE_TEAM) && (blue_base->alive) ) 
	{
        view_radar( wire_tube, player_array, view_vehicle, radar_offset );
    }

#ifdef PROFILE
    if( profile ) {
        printf("    draw target box, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif

    /* draw a box around the missile target */

    if( (player_array[view_vehicle].tank.frames_till_fire_missile > 0) ||
        (player_array[view_vehicle].tank.missiles_stored == 0) ) {
        missile_box_color = 1;
    }
    else {
        missile_box_color = 2;
    }

    if( (player_array[view_vehicle].tank.target != NULL) ) {

        /* set up the matrices to transform the target with reference to
           the view_vehicles orientation. */

        p[X] = ftom( player_array[view_vehicle].tank.orient.position[X] );
        p[Y] = ftom( player_array[view_vehicle].tank.orient.position[Y] );
        p[Z] = ftom( player_array[view_vehicle].tank.orient.position[Z] );

        target[X] = ftom( player_array[view_vehicle].tank.target[X] );
        target[Y] = ftom( player_array[view_vehicle].tank.target[Y] );
        target[Z] = ftom( player_array[view_vehicle].tank.target[Z] );

        translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

        rot_matrix_init( rotation_matrix,
            player_array[view_vehicle].tank.orient.front,
            player_array[view_vehicle].tank.orient.up );

        matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

        /* transform the target in reference to the view_vehicles
           orientation. */

        linear_transformation1( temp_matrix, target );

        /* setup translation so that the view_vehicles orientation is on the
           view point. */

        translation_matrix_init( translation_matrix, 0, 0, (-3 << MEXP) );

        /* translate so that the view_vehicles orientation is on the
           view point. */

        linear_transformation( translation_matrix, target );

        /* initialize the upper_left and lower_right corners of the box.
           the center of the box is the target. */

        upper_left[X] = target[X] + 307;
        upper_left[Y] = target[Y] - 307;
        upper_left[Z] = target[Z];

        lower_right[X] = target[X] - 307;
        lower_right[Y] = target[Y] + 307;
        lower_right[Z] = target[Z];

        /* project the upper_left corner on the view_plane and transform into
           screen coords. */

        projection_transformation( projection_matrix, upper_left );
        wtov_transformation( wtov_matrix, upper_left );

        /* project the lower_right corner of the view_plane and transform it
           into screen coords. */

        projection_transformation( projection_matrix, lower_right );
        wtov_transformation( wtov_matrix, lower_right );

        /* make sure that the corners aren't in fixed point */

        upper_left[X] = upper_left[X] >> MEXP;
        upper_left[Y] = upper_left[Y] >> MEXP;

        lower_right[X] = lower_right[X] >> MEXP;
        lower_right[Y] = lower_right[Y] >> MEXP;

        /* draw the box into the double buffer */

        draw_missile_box( upper_left, lower_right, missile_box_color );
    }

    /* draw sub target box */

    if( (player_array[view_vehicle].tank.target != NULL) ) {

        /* set up the matrices to transform the target with reference to
           the view_vehicles orientation. */

        p[X] = ftom( player_array[view_vehicle].tank.orient.position[X] );
        p[Y] = ftom( player_array[view_vehicle].tank.orient.position[Y] );
        p[Z] = ftom( player_array[view_vehicle].tank.orient.position[Z] );

        target[X] = ftom( player_array[view_vehicle].tank.target[X] );
        target[Y] = ftom( player_array[view_vehicle].tank.target[Y] );
        target[Z] = ftom( player_array[view_vehicle].tank.target[Z] );

        translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

        rot_matrix_init( rotation_matrix,
            player_array[view_vehicle].tank.orient.front,
            player_array[view_vehicle].tank.orient.up );

        matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

        /* transform the target in reference to the view_vehicles
           orientation. */

        linear_transformation1( temp_matrix, target );

        /* setup translation so that the view_vehicles orientation is on the
           view point. */

        translation_matrix_init( translation_matrix, 0, 0, (-3 << MEXP) );

        /* translate so that the view_vehicles orientation is on the
           view point. */

        linear_transformation( translation_matrix, target );

        /* initialize the upper_left and lower_right corners of the box.
           the center of the box is the target. */

        upper_left[X] = target[X] + 307;
        upper_left[Y] = target[Y] - 307;
        upper_left[Z] = target[Z];

        lower_right[X] = target[X] - 307;
        lower_right[Y] = target[Y] + 307;
        lower_right[Z] = target[Z];

        /* project the upper_left corner on the view_plane and transform into
           screen coords. */

        projection_transformation( projection_matrix, upper_left );
        wtov_transformation( wtov_matrix, upper_left );

        /* project the lower_right corner of the view_plane and transform it
           into screen coords. */

        projection_transformation( projection_matrix, lower_right );
        wtov_transformation( wtov_matrix, lower_right );

        /* half the coords fixed point */

        upper_left[X] = (((upper_left[X] - 163840) / 2) + 163840) >> MEXP;
        upper_left[Y] = (((upper_left[Y] - 102400) / 2) + 102400) >> MEXP;

        lower_right[X] = (((lower_right[X] - 163840) / 2) + 163840) >> MEXP;
        lower_right[Y] = (((lower_right[Y] - 102400) / 2) + 102400) >> MEXP;

        /* draw the box into the double buffer */

        draw_cross_box( upper_left, lower_right, missile_box_color );
    }

#ifdef PROFILE
    if( profile ) {
        printf("    end of draw_hud, timer = %ld\n", PROFILE_CheckTimer() );
    }
#endif
}

/* draws the cool looking box around the targeted enemy vehicle */

void draw_missile_box( Point upper_left, Point lower_right, unsigned char color )
{
    long i;
    long h_stub_len, v_stub_len;

    h_stub_len = (lower_right[X] - upper_left[X]) / 3;
    v_stub_len = (lower_right[Y] - upper_left[Y]) / 3;

    for( i = upper_left[X]; i <= upper_left[X] + h_stub_len; i++ ) {
        DB_Scissor_Pixel( i, upper_left[Y], color );
        DB_Scissor_Pixel( i, lower_right[Y], color );
    }
    for( i = lower_right[X] - h_stub_len; i <= lower_right[X]; i++ ) {
        DB_Scissor_Pixel( i, upper_left[Y], color );
        DB_Scissor_Pixel( i, lower_right[Y], color );
    }
    for( i = upper_left[Y]; i <= upper_left[Y] + v_stub_len; i++ ) {
        DB_Scissor_Pixel( upper_left[X], i, color );
        DB_Scissor_Pixel( lower_right[X], i, color );
    }
    for( i = lower_right[Y] - v_stub_len; i <= lower_right[Y]; i++ ) {
        DB_Scissor_Pixel( upper_left[X], i, color );
        DB_Scissor_Pixel( lower_right[X], i, color );
    }
}

void draw_cross_box( Point upper_left, Point lower_right, unsigned char color )
{
    long vert[2][4];
    Point2d top, left, right, bottom;
    Point2d new_upper_left, new_lower_right, center;
    long len;
    Edge e;

    center[X] = upper_left[X] + ((lower_right[X] - upper_left[X]) / 2);
    center[Y] = upper_left[Y] + ((lower_right[Y] - upper_left[Y]) / 2);

    new_upper_left[X] = center[X] - (lower_right[X] - upper_left[X]);
    new_upper_left[Y] = center[Y] - (lower_right[Y] - upper_left[Y]);

    new_lower_right[X] = center[X] + (lower_right[X] - upper_left[X]);
    new_lower_right[Y] = center[Y] + (lower_right[Y] - upper_left[Y]);

    e[0] = 0;
    e[1] = 1;

    len = ((new_lower_right[X] - new_upper_left[X]) / 2);

    if( len == 0 )
        len = 1;

    top[X] = new_upper_left[X] + len;
    top[Y] = new_upper_left[Y];

    bottom[X] = new_upper_left[X] + len;
    bottom[Y] = new_upper_left[Y] + len + len;

    left[X] = new_upper_left[X];
    left[Y] = new_upper_left[Y] + len;

    right[X] = new_upper_left[X] + len + len;
    right[Y] = new_upper_left[Y] + len;


    vert[0][X] = top[X] << MEXP;
    vert[0][Y] = top[Y] << MEXP;

    vert[1][X] = left[X] << MEXP;
    vert[1][Y] = left[Y] << MEXP;

    DB_draw_edge( vert, e, color );


    vert[0][X] = right[X] << MEXP;
    vert[0][Y] = right[Y] << MEXP;

    vert[1][X] = top[X] << MEXP;
    vert[1][Y] = top[Y] << MEXP;

    DB_draw_edge( vert, e, color );


    vert[0][X] = left[X] << MEXP;
    vert[0][Y] = left[Y] << MEXP;

    vert[1][X] = bottom[X] << MEXP;
    vert[1][Y] = bottom[Y] << MEXP;

    DB_draw_edge( vert, e, color );


    vert[0][X] = bottom[X] << MEXP;
    vert[0][Y] = bottom[Y] << MEXP;

    vert[1][X] = right[X] << MEXP;
    vert[1][Y] = right[Y] << MEXP;

    DB_draw_edge( vert, e, color );
}


/* Draws a any type of bar in the double buffer */

void draw_bar( BarInfo *barinfo )
{
    if( barinfo->type == HorizontalLeftJustified )
        draw_horizontal_left_justified_bar( barinfo );

    else if( barinfo->type == HorizontalRightJustified )
        draw_horizontal_right_justified_bar( barinfo );

    else if( barinfo->type == VerticalTopJustified )
        draw_vertical_top_justified_bar( barinfo );

    else if( barinfo->type == VerticalBottomJustified )
        draw_vertical_bottom_justified_bar( barinfo );
}

/* Draws only horizontal left justified bars into the double buffer */

void draw_horizontal_left_justified_bar( BarInfo *barinfo )
{
    long i, j;
    long bar_start_x, bar_end_x;

    /* draw the outline first */

    for( i = barinfo->upper_left[X]; i <= barinfo->lower_right[X]; i++ ) {
        DB_Fast_Pixel( i, barinfo->upper_left[Y], barinfo->outline_color );
        DB_Fast_Pixel( i, barinfo->lower_right[Y], barinfo->outline_color );
    }
    for( i = barinfo->upper_left[Y]; i <= barinfo->lower_right[Y]; i++ ) {
        DB_Fast_Pixel( barinfo->upper_left[X], i, barinfo->outline_color );
        DB_Fast_Pixel( barinfo->lower_right[X], i, barinfo->outline_color );
    }

    /* make sure value is in between max_value & min_value */

    if( barinfo->val > barinfo->max_val ) {
        barinfo->val = barinfo->max_val;
    }
    else if( barinfo->val < barinfo->min_val ) {
        barinfo->val = barinfo->min_val;
    }

    if( barinfo->val > barinfo->min_val ) {

        bar_start_x = barinfo->upper_left[X] + 1;
        bar_end_x = (long)(((float)((barinfo->lower_right[X] - 1) - (barinfo->upper_left[X] + 1)) / (float)(barinfo->max_val - barinfo->min_val)) * barinfo->val);

        bar_end_x += bar_start_x;

        /* draw the bar */

        for( j = barinfo->upper_left[Y] + 1; j < barinfo->lower_right[Y]; j++ ) {
            for( i = bar_start_x; i <= bar_end_x; i++ ) {
                DB_Fast_Pixel( i, j, barinfo->bar_color );
            }
        }
    }
}

/* Draws only horizontal right justified bars into the double buffer */

void draw_horizontal_right_justified_bar( BarInfo *barinfo )
{
    long i, j;
    long bar_start_x, bar_end_x, bar_length;

    /* draw the outline first */

    for( i = barinfo->upper_left[X]; i <= barinfo->lower_right[X]; i++ ) {
        DB_Fast_Pixel( i, barinfo->upper_left[Y], barinfo->outline_color );
        DB_Fast_Pixel( i, barinfo->lower_right[Y], barinfo->outline_color );
    }
    for( i = barinfo->upper_left[Y]; i <= barinfo->lower_right[Y]; i++ ) {
        DB_Fast_Pixel( barinfo->upper_left[X], i, barinfo->outline_color );
        DB_Fast_Pixel( barinfo->lower_right[X], i, barinfo->outline_color );
    }

    /* make sure value is in between max_value & min_value */

    if( barinfo->val > barinfo->max_val ) {
        barinfo->val = barinfo->max_val;
    }
    else if( barinfo->val < barinfo->min_val ) {
        barinfo->val = barinfo->min_val;
    }

    if( barinfo->val > barinfo->min_val ) {

        bar_length = (long)(((float)((barinfo->lower_right[X] - 1) - (barinfo->upper_left[X] + 1)) / (float)(barinfo->max_val - barinfo->min_val)) * barinfo->val);

        bar_end_x = barinfo->lower_right[X] - 1;
        bar_start_x = bar_end_x - bar_length;

        /* draw the bar */

        for( j = barinfo->upper_left[Y] + 1; j < barinfo->lower_right[Y]; j++ ) {
            for( i = bar_start_x; i <= bar_end_x; i++ ) {
                DB_Fast_Pixel( i, j, barinfo->bar_color );
            }
        }
    }
}

/* Draws only vertical top justified bars in the double buffer */

void draw_vertical_top_justified_bar( BarInfo *barinfo )
{
    long i, j;
    long bar_start_y, bar_end_y;

    /* draw the outline first */

    for( i = barinfo->upper_left[X]; i <= barinfo->lower_right[X]; i++ ) {
        DB_Fast_Pixel( i, barinfo->upper_left[Y], barinfo->outline_color );
        DB_Fast_Pixel( i, barinfo->lower_right[Y], barinfo->outline_color );
    }
    for( i = barinfo->upper_left[Y]; i <= barinfo->lower_right[Y]; i++ ) {
        DB_Fast_Pixel( barinfo->upper_left[X], i, barinfo->outline_color );
        DB_Fast_Pixel( barinfo->lower_right[X], i, barinfo->outline_color );
    }

    /* make sure value is in between max_value & min_value */

    if( barinfo->val > barinfo->max_val ) {
        barinfo->val = barinfo->max_val;
    }
    else if( barinfo->val < barinfo->min_val ) {
        barinfo->val = barinfo->min_val;
    }

    if( barinfo->val > barinfo->min_val ) {

        bar_start_y = barinfo->upper_left[Y] + 1;
        bar_end_y = (long)(((float)((barinfo->lower_right[Y] - 1) - (barinfo->upper_left[Y] + 1)) / (float)(barinfo->max_val - barinfo->min_val)) * barinfo->val);

        bar_end_y += bar_start_y;

        /* draw the bar */

        for( j = barinfo->upper_left[X] + 1; j < barinfo->lower_right[X]; j++ ) {
            for( i = bar_start_y; i <= bar_end_y; i++ ) {
                DB_Fast_Pixel( j, i, barinfo->bar_color );
            }
        }
    }
}

/* Draws only vertical bottom justified bars into the double buffer */

void draw_vertical_bottom_justified_bar( BarInfo *barinfo )
{
    long i, j;
    long bar_start_y, bar_end_y, bar_length;

    /* draw the outline first */

    for( i = barinfo->upper_left[X]; i <= barinfo->lower_right[X]; i++ ) {
        DB_Fast_Pixel( i, barinfo->upper_left[Y], barinfo->outline_color );
        DB_Fast_Pixel( i, barinfo->lower_right[Y], barinfo->outline_color );
    }
    for( i = barinfo->upper_left[Y]; i <= barinfo->lower_right[Y]; i++ ) {
        DB_Fast_Pixel( barinfo->upper_left[X], i, barinfo->outline_color );
        DB_Fast_Pixel( barinfo->lower_right[X], i, barinfo->outline_color );
    }

    /* make sure value is in between max_value & min_value */

    if( barinfo->val > barinfo->max_val ) {
        barinfo->val = barinfo->max_val;
    }
    else if( barinfo->val < barinfo->min_val ) {
        barinfo->val = barinfo->min_val;
    }

    if( barinfo->val > barinfo->min_val ) {

        bar_length = (long)(((float)((barinfo->lower_right[Y] - 1) - (barinfo->upper_left[Y] + 1)) / (float)(barinfo->max_val - barinfo->min_val)) * barinfo->val);

        bar_end_y = barinfo->lower_right[Y] - 1;
        bar_start_y = bar_end_y - bar_length;

        /* draw the bar */

        for( j = barinfo->upper_left[X] + 1; j < barinfo->lower_right[X]; j++ ) {
            for( i = bar_start_y; i <= bar_end_y; i++ ) {
                DB_Fast_Pixel( j, i, barinfo->bar_color );
            }
        }
    }
}

/* Blits a 320x200 buffer into the double buffer, pixels of TRANSPARENT_COLOR
   are not drawn. (that's why you can see through 'em.) */

void blit_screen( unsigned char *buffer )
{
    unsigned char *ptr = buffer;
    unsigned char *end = buffer + 64000;

    while( ptr < end ) {
        if( *ptr != TRANSPARENT_COLOR ) {
            *(double_buffer + (ptr - buffer)) = *ptr;
        }
        ptr++;
    }
}

/* takes an array of 64,000 pixels and compresses them into a pixmap by
   leaving out all transparent pixels */

void init_pixmap( PixMap *pixmap, unsigned char *buffer )
{
    long i, count;

    count = 0;

    for( i = 0; i < 64000; i++ ) {
        if( buffer[i] != TRANSPARENT_COLOR ) {
            count++;
        }
    }

    /* malloc the pixmap */

    pixmap->pixel = (Pixel *)malloc( sizeof( Pixel ) * count );
    pixmap->pixels = count;

    /* initialize the pixmap */

    count = 0;

    for( i = 0; i < 64000; i++ ) {
        if( buffer[i] != TRANSPARENT_COLOR ) {

            /* insert this pixel into the pixmap */

            pixmap->pixel[count].x = i % 320;
            pixmap->pixel[count].y = i / 320;
            pixmap->pixel[count].color = buffer[i];

            count++;
        }
    }
}

/* draws all the pixels of the pixmap into the double buffer */

void blit_pixmap( PixMap *pixmap, unsigned char new_cross_hair_color )
{
    long i;

    for( i = 0; i < pixmap->pixels; i++ ) {
        if( pixmap->pixel[i].color != CROSS_HAIR_COLOR ) {
            DB_Fast_Pixel( pixmap->pixel[i].x, pixmap->pixel[i].y, pixmap->pixel[i].color );
        }
        else {
            DB_Fast_Pixel( pixmap->pixel[i].x, pixmap->pixel[i].y, new_cross_hair_color );
        }
    }
}

void draw_third_person_hud( Player *player_array, long view_vehicle,
                            RadarBase *red_base, RadarBase *blue_base )
{
    long wing_one_index, wing_two_index, found_index_one, i;

    Point p;                    /* the fixed-point version of the view_vehicles position */
    Point target;               /* used to transform the target into screen coords */
    Point upper_left;           /* the upper_left hand corner of the missile lock box */
    Point lower_right;          /* the lower_right hand corner of the missile lock box */

    Matrix translation_matrix;  /* These three matrices are used to transform */
    Matrix rotation_matrix;     /* the target and the corners of the missile */
    Matrix temp_matrix;         /* lock box into screen coords. */

    char temp_string[80];
    unsigned char missile_box_color;

    wing_one_index = 0;
    wing_two_index = 0;
    found_index_one = FALSE;

    for( i = 0; i < 6; i++ ) {
        if( (i != view_vehicle) && !found_index_one && (player_array[i].tank.team == player_array[view_vehicle].tank.team) ) {
            wing_one_index = i;
            found_index_one = TRUE;
        }
        else if( (i != view_vehicle) && found_index_one && (player_array[i].tank.team == player_array[view_vehicle].tank.team) ) {
            wing_two_index = i;
            break;
        }
    }

    /* blit the text */

    sprintf( temp_string, "DAMAGE: %d", player_array[view_vehicle].tank.laser_damage );

    micro_string_blit( temp_string, 1, 175, menu_stuff.micro_text.buffer, menu_stuff.micro_text.xpixels+1,
                       menu_stuff.micro_text.ypixels+1, 1 );

    sprintf( temp_string, "SHEILDS: %d%%", (int)((((float)player_array[view_vehicle].tank.current_hitpoints) / ((float)player_array[view_vehicle].tank.max_hitpoints)) * 100.0) );

    micro_string_blit( temp_string, 1, 183, menu_stuff.micro_text.buffer, menu_stuff.micro_text.xpixels+1,
                       menu_stuff.micro_text.ypixels+1, 1 );

    sprintf( temp_string, "MISSILES: %d", player_array[view_vehicle].tank.missiles_stored );

    micro_string_blit( temp_string, 1, 191, menu_stuff.micro_text.buffer, menu_stuff.micro_text.xpixels+1,
                       menu_stuff.micro_text.ypixels+1, 1 );

    /* draw a box around the missile target */

    if( (player_array[view_vehicle].tank.frames_till_fire_missile > 0) ||
        (player_array[view_vehicle].tank.missiles_stored == 0) ) {
        missile_box_color = 1;
    }
    else {
        missile_box_color = 2;
    }

    if( (player_array[view_vehicle].tank.target != NULL) ) {

        /* set up the matrices to transform the target with reference to
           the view_vehicles orientation. */

        p[X] = ftom( world_stuff.view_orientation.position[X] );
        p[Y] = ftom( world_stuff.view_orientation.position[Y] );
        p[Z] = ftom( world_stuff.view_orientation.position[Z] );

        target[X] = ftom( player_array[view_vehicle].tank.target[X] );
        target[Y] = ftom( player_array[view_vehicle].tank.target[Y] );
        target[Z] = ftom( player_array[view_vehicle].tank.target[Z] );

        translation_matrix_init( translation_matrix, -p[X], -p[Y], -p[Z] );

        rot_matrix_init( rotation_matrix, world_stuff.view_orientation.front,
                         world_stuff.view_orientation.up );

        matrix_mult( rotation_matrix, translation_matrix, temp_matrix );

        /* transform the target in reference to the view_vehicles
           orientation. */

        linear_transformation1( temp_matrix, target );

        /* setup translation so that the view_vehicles orientation is on the
           view point. */

        translation_matrix_init( translation_matrix, 0, 0, (-3 << MEXP) );

        /* translate so that the view_vehicles orientation is on the
           view point. */

        linear_transformation( translation_matrix, target );

        /* initialize the upper_left and lower_right corners of the box.
           the center of the box is the target. */

        upper_left[X] = target[X] + 307;
        upper_left[Y] = target[Y] - 307;
        upper_left[Z] = target[Z];

        lower_right[X] = target[X] - 307;
        lower_right[Y] = target[Y] + 307;
        lower_right[Z] = target[Z];

        /* project the upper_left corner on the view_plane and transform into
           screen coords. */

        projection_transformation( projection_matrix, upper_left );
        wtov_transformation( wtov_matrix, upper_left );

        /* project the lower_right corner of the view_plane and transform it
           into screen coords. */

        projection_transformation( projection_matrix, lower_right );
        wtov_transformation( wtov_matrix, lower_right );

        /* make sure that the corners aren't in fixed point */

        upper_left[X] = upper_left[X] >> MEXP;
        upper_left[Y] = upper_left[Y] >> MEXP;

        lower_right[X] = lower_right[X] >> MEXP;
        lower_right[Y] = lower_right[Y] >> MEXP;

        /* draw the box into the double buffer */

        draw_missile_box( upper_left, lower_right, missile_box_color );
    }
}

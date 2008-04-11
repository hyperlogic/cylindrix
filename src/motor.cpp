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

#include "motor.h"
#include "movement.h"                        
#include "keys.h"

                        
extern player_events_type player_events[6];                        

void Move_Forward( input_table table )
    {
     table[ INDEX_UP ] = 255;
    }

void Move_Back( input_table table )
    {
     table[ INDEX_DOWN ] = 255; 
    }

void Move_Right( input_table table )
    {
     table[ INDEX_RIGHT ] = 255;
    }

void Move_Left( input_table table )
    {
     table[ INDEX_LEFT ] = 255; 
    }

void Sidestep_Right( input_table table )
    {
     table[ INDEX_SIDESTEP_RIGHT ] = 255;
    }

void Sidestep_Left( input_table table )
    {
     table[ INDEX_SIDESTEP_LEFT ] = 255;
    }

void Fire_Gun( input_table table )
    {
     table[ INDEX_FIRE_GUN ] = 1; 
    }

void Fire_Missile( input_table table )
    {
     table[ INDEX_FIRE_MISSILE ] = 1;
    }

void Mode_Switch_Into_Air( input_table table )
    {
     table[ INDEX_MODE_SWITCH ] = 1;
    }

void Mode_Switch_Onto_Ground( input_table table )
    {
     table[ INDEX_MODE_SWITCH ] = 1;
    }

void Up_Throttle( input_table table )
    {
     table[ INDEX_UP_THROTTLE ] = 255;
    }

void Down_Throttle( input_table table )
    {
     table[ INDEX_DOWN_THROTTLE ] = 255;
    }

void Sidestep_Up( input_table table )
    {
     table[INDEX_SIDESTEP_UP] = 255;
    }

void Sidestep_Down( input_table table )
    {
     table[INDEX_SIDESTEP_DOWN] = 255;
    }

void Special_Weapon( input_table table )
    {
     table[INDEX_SPECIAL_WEAPON] = TRUE;
    }
  
    

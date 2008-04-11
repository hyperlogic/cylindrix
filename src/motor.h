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

#ifndef MOTOR_H

#define MOTOR_H

#include "types.h"
#include "ai.h"
void Move_Forward( input_table table );
void Move_Back( input_table table );
void Move_Right( input_table table );
void Move_Left( input_table table );

void Sidestep_Right( input_table table );
void Sidestep_Left( input_table table );
void Sidestep_Up( input_table table );
void Sidestep_Down( input_table table );

void Up_Throttle( input_table table );
void Down_Throttle( input_table table );

void Fire_Gun( input_table table );
void Fire_Missile( input_table table );

void Mode_Switch_Into_Air( input_table table );
void Mode_Switch_Onto_Ground( input_table table );

void Special_Weapon( input_table table );

#endif


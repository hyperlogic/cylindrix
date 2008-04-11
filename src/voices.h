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


typedef struct
    {
     boolean greeting_told;
     boolean affirmation_told;
     boolean negation_told;
     boolean gloat_told;
     boolean despair_told;
     boolean death_told;
     boolean victory_told;
     boolean control_lost_told;
     boolean control_regained_told;
    } voice_handler_type;


void Init_Voices( void );
void Handle_Voices( WorldStuff *world_stuff, long view_vehicle, unsigned char first_person_view, unsigned char teleporting );
void Zero_Voice( int wingman_number );
void No_Despair( int wingman_number );

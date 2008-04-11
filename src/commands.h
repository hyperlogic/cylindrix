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

#ifndef COMMANDS_H

#define COMMANDS_H


#include "types.h"


typedef enum  
    {
     ATTACK_COMMAND, ATTACK_RADAR_COMMAND, DEFEND_RADAR_COMMAND,
     GET_PYLONS_COMMAND, GROUP_COMMAND, CANCEL_COMMANDS_COMMAND
    } command_type;

/* There is one boolean for wingman one, and one for wingman two */
typedef struct 
    {
     boolean        command_active[2]; 
     short          frames_left[2];       /* Number of frames command will be active */
     
     command_type   command[2];        /* What command is active? */
    } command_handler_type;


void Handle_Commands( WorldStuff *world_stuff, int leader );
void Sprint_Command( char *command_string, command_type command );

#endif


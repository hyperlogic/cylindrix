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
#include "keys.h"


/* Size of the circular buffer of keyboard inputs */
#define INPUT_BUFFER_SIZE 50



typedef struct
    {
     input_table table;
    } input_node_type;


typedef struct
    {
     input_node_type input_node[INPUT_BUFFER_SIZE];
     int current_node; /* Where we are now in input nodes */
     int first_node;   /* Last unprocessed input node */
    } input_buffer_type;


void Clear_Input_Nodes( void );
void Update_Input_Node( void ); /* Call from keyboard interrupt */
void Make_Snapshot_Queue( SnapshotQueue *snapshot_queue, int movement_vehicle );

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

#include "input.h"
#include "joy.h"
#include "main.h"
#include "packets.h"


extern int key_table[KEY_TABLE_SIZE]; /* From keys.c */
extern game_configuration_type game_configuration; /* From omega.c */
extern unsigned char master; /* From omega.c */
extern WorldStuff world_stuff; /* From omega.c */



input_buffer_type input_buffer;



void Clear_Input_Nodes( void )
    {
     int i, j;

     input_buffer.current_node = 0;
     input_buffer.first_node = 0;

     for( i = 0; i < INPUT_BUFFER_SIZE; i++ )
         {
          for( j = 0; j < INPUT_TABLE_SIZE; j++ )
              {
               input_buffer.input_node[i].table[j] = 0;
              }
         }
    } /* End of Clear_Input_Nodes */


/* Call from keyboard interrupt */
void Update_Input_Node( void )
    {
     
     get_user_key_table( input_buffer.input_node[input_buffer.current_node].table );
     

     input_buffer.current_node++;

     if( input_buffer.current_node >= INPUT_BUFFER_SIZE )
         input_buffer.current_node = 0;


    } /* End of Update_Input_Node */
     


void Make_Snapshot_Queue( SnapshotQueue *snapshot_queue, int movement_vehicle ) {

    int i, j;
    unsigned char *table, *table_two;     

     
    for( i = 0; i < 6; i++ ) {
        if( world_stuff.player_array[i].controller == USER_CONTROL ) {
            snapshot_queue->snapshot[snapshot_queue->size].input_package[i].type = USER_CONTROL;
            table = snapshot_queue->snapshot[snapshot_queue->size].input_package[i].table;
            table_two = world_stuff.player_array[i].table;
            
            for( j = 0; j < INPUT_TABLE_SIZE; j++ ) {                        
                table[j] = table_two[j];
            }
        }
        else if( world_stuff.player_array[i].controller == REMOTE_CONTROL ) {                   
            snapshot_queue->snapshot[snapshot_queue->size].input_package[i].type = REMOTE_CONTROL;
            table = snapshot_queue->snapshot[snapshot_queue->size].input_package[i].table;
            table_two = world_stuff.player_array[i].table;
            
            for( j = 0; j < INPUT_TABLE_SIZE; j++ ) {
                table[j] = table_two[j];
            }
        }
        else if( world_stuff.player_array[i].controller == AI_CONTROL ) {
            snapshot_queue->snapshot[snapshot_queue->size].input_package[i].type = AI_CONTROL;
        }
        else {
            fprintf(stderr, "ERROR IN SNAPSHOT QUEUEUE!!! \n");
        }
    } 


    input_buffer.first_node++;
    if( input_buffer.first_node >= INPUT_BUFFER_SIZE ) {
        input_buffer.first_node = 0;
    }
    snapshot_queue->size++;

} /* End of Make_Snapshot_Queue */

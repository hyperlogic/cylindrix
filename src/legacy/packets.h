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

#ifndef PACKETS_H
#define PACKETS_H

#include <stdio.h>
#include "types.h"


/* These are the defines for the different types of packets */

#define INPUT_TABLE_PACKET           0  /* Contains one vehicle's input table */
#define TEXT_MESSAGE_PACKET          1  /* Contains a typed message from one side to the other */
#define END_OF_PACKETS_PACKET        2  /* Lets the other side know that we are through sending packets */
#define RESYNCHRONIZATION_PACKET     3  /* Lets the other side know that we need to resynchronize everything This will happen if there are several error packets in a row */ 
#define ORIENTATION_PACKET           4  /* Contains one vehicles orientiation */
#define ERROR_PACKET                 5  /* Lets the other side know that we need the last batch of packets again */
#define TERMINATE_CONNECTION_PACKET  6  /* Lets the other side know that we are terminating the connection */
#define FILE_TRANSFER_PACKET         7
#define EXIT_GAME_PACKET             8  /* Lets the other side know we are quitting the current game without terminating the connection */
#define GAME_INFO_PACKET             9  /* Packet to send info about vehicles chosen and characters chosen */

#define MAX_MESSAGE_LENGTH 30


/*

up             1byte 
down           1byte
left           1byte
right          1byte
sidestep up    1byte
sidestep down  1byte
sidestep left  1byte
sidestep right 1byte

INDEX_UP_THROTTLE     |        
INDEX_DOWN_THROTTLE   |        
INDEX_FIRE_GUN        | 1byte        
INDEX_FIRE_MISSILE    |        
INDEX_SPECIAL_WEAPON  |        
INDEX_MODE_SWITCH     |        

sizeof encoded input table will be 9bytes
*/


#define ENCODED_INPUT_TABLE_SIZE 9

typedef unsigned char encoded_input_table_type[ENCODED_INPUT_TABLE_SIZE];

/* #define __LINUX__ */
/* Only use linux specific __attribute__ tag on Linux */
#ifndef PACKED_STRUCT 
	#ifdef __LINUX__
		#define PACKED_STRUCT __attribute__((packed))
	#else
		#define PACKED_STRUCT 
	#endif
#endif

typedef struct
    {
     unsigned char vehicle_one   PACKED_STRUCT;
     unsigned char vehicle_two   PACKED_STRUCT;
     unsigned char vehicle_three PACKED_STRUCT;
     unsigned char wingman_one   PACKED_STRUCT;
     unsigned char wingman_two   PACKED_STRUCT;
     unsigned char wingman_three PACKED_STRUCT;
     string   cylinder_filename  PACKED_STRUCT;
    } game_info_type;




void Encode_Input_Table( input_table table, encoded_input_table_type encoded_input_table );
void Decode_Input_Table( input_table table, encoded_input_table_type encoded_input_table );
void Get_Packets( Player player[] );
void Send_Input_Table_Packet( char index, input_table table );
void Get_Input_Table_Packet( Player player[] );
void Send_Text_Message_Packet( char *message );
void Get_Text_Message_Packet( void );
void Send_File_Packet( char *filename );
void Get_File_Packet( void );
void Send_Game_Info_Packet( game_info_type *game_info );
void Get_Game_Info_Packet( game_info_type *game_info );
void Get_Remote_Key_Table( input_table table );
void Send_Key_Table( input_table table );
void Send_End_Of_Packets_Packet( void );


#endif


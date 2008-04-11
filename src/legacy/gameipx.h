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

#ifndef GAMEIPX_H

#define GAMEIPX_H


#include "types.h"
#include "jonipx.h"


#define MAX_NUM_PLAYERS 6



typedef struct
    {
     char          name[20];
     boolean       user_controlled; /* Are they a player or an ai vehicle?? */
     unsigned char ai_index;        /* If ai controlled, which ai is controlling it */
     unsigned char vehicle_type;
    } player_info_type;



typedef struct
    {
     short             num_players; 
     node_address_type node_addresses[MAX_NUM_PLAYERS];    /* The node address of every player */
     unsigned char     indexes[MAX_NUM_PLAYERS];           /* The indexes into the player array of all players */
     boolean           server;                             /* Are we a client or a server? */
     short             my_number;                          /* What is my player number? 0 - (MAX_NUM_PLAYERS - 1) */
     unsigned char     index_into_player_array;            /* My index into the player array */
     team_type         team;                               /* What team am i on? */
     player_info_type  player_info[MAX_NUM_PLAYERS];       /* The indexes of this are the same as the indexes into the player array */
     boolean           my_responsibility[MAX_NUM_PLAYERS]; /* Am i responsible for the handling of this player */
    } IPX_game_info_type;




/*
and begin to wait for clients


if they are a client they send out a ready to play signal

when they are all connected they each get a menu screen...

when everyone clicks start, everyone waits till they recieve a
go ahead packet from the server...the server waits till it gets
a ready packet from every client

once the go ahead has been reached, every client broadcasts its
choices and everyone sychronizes the vehicles selected etc...

once the game starts, the server broadcasts its input tables and then
each computer in order broadcasts its input tables...loop till game over.
*/



int Wait_For_Nodes( IPX_game_info_type *ipx_game_info );
int Synchronize_Teams( IPX_game_info_type *ipx_game_info, team_type team );
int Synchronize_IPX_Game( IPX_game_info_type *ipx_game_info, game_configuration_type *game_configuration );
int Client_Send_Input_Tables( IPX_game_info_type *ipx_game_info, WorldStuff *world_stuff );
int Server_Send_Input_Tables( IPX_game_info_type *ipx_game_info, WorldStuff *world_stuff );
int Client_Get_Input_Tables( IPX_game_info_type *ipx_game_info, WorldStuff *world_stuff );
int Server_Get_Input_Tables( IPX_game_info_type *ipx_game_info, WorldStuff *world_stuff );
void Send_End_Game_Packet( IPX_game_info_type *ipx_game_info );


#endif



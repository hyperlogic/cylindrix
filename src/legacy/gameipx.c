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

#include "jonipx.h"
#include "gameipx.h"
#include "packets.h"
#include "keys.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h> /* For swab */

/*
#include <pc.h>
#include <dos.h>
*/

/* How long we delay before sending another packet */
#define PACKET_DELAY 10


node_address_type broadcast_node = { 0xff,0xff,0xff,0xff,0xff,0xff };


extern long exit_loop;  /* From omega.c */
extern long game_over;


extern local_address_type local_address;
extern boolean terminate_connection;

/*
typedef struct
    {
     short             num_players; 
     node_address_type node_addresses[MAX_NUM_PLAYERS];
     boolean           server;
     short             my_number;
    } IPX_game_info_type;
*/


int Wait_For_Nodes( IPX_game_info_type *ipx_game_info )
    {
     string_type send_string; /* For use as packets */
     game_packet_type game_packet; /* A REAL packet! (for recieve)*/
     int i, j;
     short num_ready = 1; /* The server is always ready! */
     int string_index; 
     unsigned short *short_ptr; /* Short alias to string buffer */
     int short_index = 0; /* Index into short_ptr thing */
     int process_node = 0;
     int done = 0;
     long game_over = FALSE;


     /* Clear out the send and recieve strings */
     for( i = 0; i < sizeof(string_type); i++ )
         {
          send_string[i] = 0;
          game_packet.string[i] = 0;
         }


     if( ipx_game_info->server )
         {
          /*
          Every second or so broadcast a 'waiting for game' packet
          with my node address in it. Every time I get a 'here i am'
          packet from a client i assign him the next available player
          number and send it back to him.  When all available slots are
          filled, i broadcast the game info packet that tells everyone
          how many players there are and what everyones node addresses
          are, then move on
          */

          /* Copy our node address into the first players node address */
          for( i = 0; i < sizeof(node_address_type); i++ )
              ipx_game_info->node_addresses[0][i] = local_address.node_address[i];

          /* Server is always player 0 */
          ipx_game_info->my_number = 0;


          /* While we haven't heard from everyone */
          while( num_ready < ipx_game_info->num_players )
              {
               process_node = 1;
               
               while( !Packet_Ready() )
                   {
                    if( Jon_Kbhit() )
                       if( Jon_Getkey() == INDEX_ESC )
                           return(0);
               
                    delay(10);
                    send_string[0] = WAITING_FOR_NODES_PACKET;
                    Jon_Send_Packet( send_string, 1, broadcast_node );
    
                   }


               /* fprintf(stderr, "Recieved packet\n"); */

               Pop_Packet( &game_packet );
               if( game_over )
                   return(0);
               if( terminate_connection ) /* Should put after every pop_packet */
                   return(0);

               for( i = 0; i < num_ready; i++ )
                   if( Compare_Nodes( game_packet.source_node, ipx_game_info->node_addresses[i] ) )
                       process_node = 0;
               
               if( process_node )
               {
               /* fprintf(stderr, "Got player %d! \n", num_ready); */

               /* Copy the senders node address into the next node address field */
               for( i = 0; i < sizeof(node_address_type); i++ )
                   ipx_game_info->node_addresses[num_ready][i] = game_packet.source_node[i];

               /* Give this guy a number and send it to him */
               send_string[0] = PLAYER_NUMBER_ASSIGNMENT_PACKET;
               send_string[1] = (char)num_ready;

               /* Jon_Send_Packet( send_string, sizeof(string_type), ipx_game_info->node_addresses[num_ready] ); */

               Jon_Send_Packet( send_string, sizeof(string_type), ipx_game_info->node_addresses[num_ready] );

               num_ready++;

               } /* End of if process nodes */

              } /* End of wait for nodes */


          delay(10);

          /* Now everyone has a number...let everyone know whats what */


          /* Send over info */
          send_string[0] = GAME_INIT_PACKET;
          send_string[1] = (char)ipx_game_info->num_players;
          string_index = 2;
          short_ptr = (unsigned short *)&send_string[string_index];

          for( i = 0; i < ipx_game_info->num_players; i++ )
              {
               for( j = 0; j < sizeof(node_address_type); j++ )
                   {
                    short_ptr[short_index] = ipx_game_info->node_addresses[i][j];
                    short_index++;
                   }
              }

          /* We dont send anything to ourselves...hence the start at one */
          for( i = 1; i < ipx_game_info->num_players; i++ )
              {
               Jon_Send_Packet( send_string, sizeof(string_type), ipx_game_info->node_addresses[i] );
               /* fprintf(stderr, "Sending player %d's game info \n", i); */
              }
          /* Jon_Send_Packet( send_string, sizeof(string_type), broadcast_node ); */
          
         }
     else /* If i am a client */
         {
          /*
          Listen to see if the server is there...
          if he is, record his node address and tell him i'm
          here...wait for his reply to tell me which number i am...
          then wait for him to send me the info on all players, then
          move on
          */

          /* fprintf(stderr, "Waiting for server...\n"); */

          done = 0;

          while( !done )
              {
               if( Jon_Kbhit() )
                   if( Jon_Getkey() == INDEX_ESC )
                       return(0);

               if( Packet_Ready() )
                   {
                    Pop_Packet( &game_packet );
               if( game_over )
                   return(0);

                    if( terminate_connection ) /* Should put after every pop_packet */
                        return(0);

                    if( game_packet.string[0] == WAITING_FOR_NODES_PACKET )
                        {

                         for( i = 0; i < sizeof(node_address_type); i++ )
                             ipx_game_info->node_addresses[0][i] = game_packet.source_node[i];
                         /* fprintf(stderr, "%x", ipx_game_info->node_addresses[0][i] ); */
                         done = 1;
                        }
                   }
              } /* End of while */
          /* fprintf(stderr,"\n"); */

          send_string[0] = HERE_I_AM_PACKET;
          Jon_Send_Packet( send_string, 1, ipx_game_info->node_addresses[0] );
          
          /* fprintf(stderr, "Server found, waiting for number assignment \n"); */

          done = 0;

          while( !done )
              {
               if( Jon_Kbhit() )
                   if( Jon_Getkey() == INDEX_ESC )
                       return(0);

               if( Packet_Ready() )
                   {
                    Pop_Packet( &game_packet );
                    if( game_over )
                        return(0);

                    if( terminate_connection ) /* Should put after every pop_packet */
                        return(0);

                    if( game_packet.string[0] == PLAYER_NUMBER_ASSIGNMENT_PACKET )
                        {
                         done = 1;
                        }
                   }
              } /* End of while */

          
          /*
          fprintf(stderr, "End of wait for server...\n");
          fprintf(stderr, "Got player number assignment packet \n");
          fprintf(stderr, "in the packet my number is %d \n", game_packet.string[1] );
          fprintf(stderr, "Waiting for game init packet \n");
          */

          ipx_game_info->my_number = game_packet.string[1];

          done = 0;

          while( !done )
              {
               if( Jon_Kbhit() )
                   if( Jon_Getkey() == INDEX_ESC )
                       return(0);

               if( Packet_Ready() )
                   {
                    Pop_Packet( &game_packet );
                    if( game_over )
                        return(0);

                    if( terminate_connection ) /* Should put after every pop_packet */
                        return(0);

                    if( game_packet.string[0] == GAME_INIT_PACKET )
                        {
                         ipx_game_info->num_players = game_packet.string[1];
                         string_index = 2;
                         short_ptr = (unsigned short *)&game_packet.string[string_index];
                         for( i = 0; i < ipx_game_info->num_players; i++ )
                             {
                              for( j = 0; j < sizeof(node_address_type); j++ )
                                  {
                                   ipx_game_info->node_addresses[i][j] = (char)short_ptr[short_index];
                                   short_index++;
                                  }
                              }
                         done = 1;

                        } /* End of for */
                   } /* End if packet ready */

              } /* End of while */


          /*
          fprintf(stderr, "Got game init packet \n");
          fprintf(stderr, "End of wait for game init packet \n");
          */

         } /* End of if i am a server */
       

    return(1);

    } /* End of Wait_For_Nodes */


int Synchronize_Teams( IPX_game_info_type *ipx_game_info, team_type team )
    {
     int i, j;
     string_type string;
     game_packet_type game_packet; /* A REAL packet! (for recieve)*/
     unsigned char current_index_into_red_team;
     unsigned char current_index_into_blue_team;
     boolean user_controlled[6] = { 0,0,0,0,0,0 }; /* Do we have a controller for this vehicle? */


     int done;
     unsigned char temp_team;

     

     current_index_into_red_team  = 0;
     current_index_into_blue_team = 3;


     if( ipx_game_info->server )
         {
          if( team == RED_TEAM )
              {
               ipx_game_info->team = RED_TEAM;
               ipx_game_info->indexes[0] = current_index_into_red_team;
               current_index_into_red_team++;
              }
          else
              {
               ipx_game_info->team = BLUE_TEAM;
               ipx_game_info->indexes[0] = current_index_into_blue_team;
               current_index_into_blue_team++;
              }

          /* Prompt everyone for their team request (except ourself!) */
          for( i = 1; i < ipx_game_info->num_players; i++ )
              {
               done = 0;

               string[0] = WAITING_FOR_TEAM_REQUEST_PACKET;
               Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[i] );

               while( !done )
                   {
                    if( Jon_Kbhit() )
                        if( Jon_Getkey() == INDEX_ESC )
                            return(0);

                    if( Packet_Ready() )
                        {
                         Pop_Packet( &game_packet ); 
                         if( game_over )
                             return(0);

                         if( terminate_connection ) /* Should put after every pop_packet */
                             return(0);

                         if( game_packet.string[0] == TEAM_REQUEST_PACKET )
                             {
                              done = 1;

                              temp_team = game_packet.string[1];
                              if( temp_team == RED_TEAM )
                                  {
                                   if( current_index_into_red_team < 3 )
                                        {
                                         ipx_game_info->indexes[i] = current_index_into_red_team;
                                         current_index_into_red_team++;
                                        }
                                   else
                                        {
                                         ipx_game_info->indexes[i] = current_index_into_blue_team;
                                         current_index_into_blue_team++;
                                        }
                                  }
                              else /* They want the blue team */
                                  {
                                   if( current_index_into_blue_team < 6 )
                                        {
                                         ipx_game_info->indexes[i] = current_index_into_blue_team;
                                         current_index_into_blue_team++;
                                        }
                                   else
                                        {
                                         ipx_game_info->indexes[i] = current_index_into_red_team;
                                         current_index_into_red_team++;
                                        }
                                  }

                             } /* End wait for packet */

                        } /* End if packet ready */

                   } /* End while not done */

              } /* End of for */

          /* Compile it all together and let everyone know what team they're on */

          string[0] = TEAM_ASSIGNMENT_PACKET;
          for( i = 0; i < ipx_game_info->num_players; i++ )
              {
               string[i+1] = ipx_game_info->indexes[i];
              }


          /* Don't send a packet to ourself! */
          for( i = 1; i < ipx_game_info->num_players; i++ )
              {
               Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[i] );
              }

         }
     else /* We are a client */
         {
          /* Wait for the prompt from the server */
          done = 0;

          while( !done )
              {
               if( Jon_Kbhit() )
                   if( Jon_Getkey() == INDEX_ESC )
                       return(0);

               if( Packet_Ready() )
                   {
                    Pop_Packet( &game_packet ); 
                    if( game_over )
                        return(0);

                    if( terminate_connection ) /* Should put after every pop_packet */
                        return(0);

                    if( game_packet.string[0] == WAITING_FOR_TEAM_REQUEST_PACKET )
                        {
                         done = 1;
                        }
                   }

              } /* End of while not done */

          /* Send the server the team i want to be on */
          string[0] = TEAM_REQUEST_PACKET;
          string[1] = team;

          Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[0] );

          done = 0;

          while( !done )
              {
               if( Jon_Kbhit() )
                   if( Jon_Getkey() == INDEX_ESC )
                       return(0);

               if( Packet_Ready() )
                   {
                    Pop_Packet( &game_packet ); 
                    if( game_over )
                        return(0);

                    if( terminate_connection ) /* Should put after every pop_packet */
                        return(0);

                    if( game_packet.string[0] == TEAM_ASSIGNMENT_PACKET )
                        {
                         done = 1;

                         for( i = 0; i < ipx_game_info->num_players; i++ )
                             {
                              ipx_game_info->indexes[i] = game_packet.string[i + 1];
                             }
                        }

                   } /* End if packet ready */

              } /* End while */

          if( ipx_game_info->indexes[ipx_game_info->my_number] == 0 ||
              ipx_game_info->indexes[ipx_game_info->my_number] == 1 ||
              ipx_game_info->indexes[ipx_game_info->my_number] == 2 )
              {
               ipx_game_info->team = RED_TEAM;
              }
          else
              {
               ipx_game_info->team = BLUE_TEAM;
              }


         } /* End if we are a client or server */
               

     for( i = 0; i < 6; i++ )
          ipx_game_info->my_responsibility[i] = 0;

     ipx_game_info->index_into_player_array = ipx_game_info->indexes[ipx_game_info->my_number];

     for( i = 0; i < 6; i++ )
          for( j = 0; j < ipx_game_info->num_players; j++ )
              if( ipx_game_info->indexes[j] == i )
                  user_controlled[i] = TRUE;


     if( ipx_game_info->server )
         {
          ipx_game_info->my_responsibility[ipx_game_info->index_into_player_array] = TRUE;

          if( ipx_game_info->index_into_player_array == 0 )
              {
               if( !user_controlled[1] )
                   ipx_game_info->my_responsibility[1] = TRUE;
               if( !user_controlled[2] )
                   ipx_game_info->my_responsibility[2] = TRUE;
              }
          else if( ipx_game_info->index_into_player_array == 3 )
              {
               if( !user_controlled[4] )
                   ipx_game_info->my_responsibility[4] = TRUE;
               if( !user_controlled[5] )
                   ipx_game_info->my_responsibility[5] = TRUE;
              }

         if( !user_controlled[0] && !user_controlled[1] && !user_controlled[2] )
             {
              ipx_game_info->my_responsibility[0] = TRUE;
              ipx_game_info->my_responsibility[1] = TRUE;
              ipx_game_info->my_responsibility[2] = TRUE;
             }

         if( !user_controlled[3] && !user_controlled[4] && !user_controlled[5] )
             {
              ipx_game_info->my_responsibility[3] = TRUE;
              ipx_game_info->my_responsibility[4] = TRUE;
              ipx_game_info->my_responsibility[5] = TRUE;
             }

         }
     else
         {
          ipx_game_info->my_responsibility[ipx_game_info->index_into_player_array] = TRUE;

          if( ipx_game_info->index_into_player_array == 0 )
              {
               if( !user_controlled[1] )
                   ipx_game_info->my_responsibility[1] = TRUE;
               if( !user_controlled[2] )
                   ipx_game_info->my_responsibility[2] = TRUE;
              }
          else if( ipx_game_info->index_into_player_array == 3 )
              {
               if( !user_controlled[4] )
                   ipx_game_info->my_responsibility[4] = TRUE;
               if( !user_controlled[5] )
                   ipx_game_info->my_responsibility[5] = TRUE;
              }

         } /* End if we're the server */



     /*
     for( i = 0; i < 6; i++ )
         {
          if( ipx_game_info->my_responsibility[i] )
              fprintf(stderr, "I am responsible for tank %d \n", i );
         }
     */

     /* Get_Keypress(); */

     delay(IPX_DELAY); /* A hack to make it work on luke's pentium */
     return(1);

    } /* End of Synchronize_Teams() */



/* format of player info packet
byte     : type of packet
byte     : index into player array
byte     : type of vehicle
char[20] : name of character
*/

/* Format of cylinder packet
byte     : type of packet
char[20] : filename of cylinder
*/


int Synchronize_IPX_Game( IPX_game_info_type *ipx_game_info, game_configuration_type *game_configuration )
    {
     int i, j;
     game_packet_type game_packet;
     string_type string;
     /* So I can access the vehicles in game.cfg like an array ;) */
     int vehicles[6];
     int num_to_recieve = 0; /* Number of remote shite the server will recieve */
     int done;


     vehicles[0] = game_configuration->red0_vehicle;
     vehicles[1] = game_configuration->red1_vehicle;
     vehicles[2] = game_configuration->red2_vehicle;
     vehicles[3] = game_configuration->blue0_vehicle;
     vehicles[4] = game_configuration->blue1_vehicle;
     vehicles[5] = game_configuration->blue2_vehicle;


     for( i = 0; i < 6; i++ )
         if( !ipx_game_info->my_responsibility[i] )
             num_to_recieve++;

     if( ipx_game_info->server )
         {
           /* fprintf(stderr, "Server waiting for player info \n"); */

           while( num_to_recieve > 0 )
               {
                if( Jon_Kbhit() )
                    if( Jon_Getkey() == INDEX_ESC )
                        return(0);

                if( Packet_Ready() )
                    {
                     Pop_Packet( &game_packet ); 
                     if( game_over )
                         return(0);

                     if( terminate_connection ) /* Should put after every pop_packet */
                         return(0);

                     if( game_packet.string[0] == PLAYER_INFO_PACKET )
                         {
                          /* fprintf(stderr, "Got one players info \n"); */
                          num_to_recieve--;
                          vehicles[(int)game_packet.string[1]] = game_packet.string[2];
                         }
                    } /* End if packet ready */

               } /* End while num_to_recieve */

         /* fprintf(stderr, "Got everyones info...sending \n"); */

         /* Send info to all of the players */
         for( i = 0; i < ipx_game_info->num_players; i++ )
             {
              string[0] = CYLINDER_FILENAME_PACKET;
              for( j = 0; j < 20; j++ )
                  string[j+1] = game_configuration->cylinder_filename[j];

              Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[i] );

              delay( IPX_DELAY );

              for( j = 0; j < 6; j++ )
                  {
                   string[0] = PLAYER_INFO_PACKET;
                   string[1] = j;
                   string[2] = vehicles[j];

                   /* we aren't sending names yet 
                   buffer_pos = 3;
                   for( j = 0; j < 20; j++ )
                   string[buffer_pos] = */
                   Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[i] );
                   delay( IPX_DELAY );
                  }

             } /* End for */

         }
     else /* If i am a client */
         {
          /*
             Send the server however many of the above packets
             we are responsible for, then wait for the server
             to send us all six of the packets
          */
          /* fprintf(stderr, "Client sending info \n"); */

          for( i = 0; i < 6; i++ )
               {

                if( ipx_game_info->my_responsibility[i] )
                    {
                     /* fprintf(stderr, "Player %d is mine...sending \n", i ); */
                     string[0] = PLAYER_INFO_PACKET;
                     string[1] = i;
                     string[2] = vehicles[i];

                     /* we aren't sending names yet 
                     buffer_pos = 3;
                     for( j = 0; j < 20; j++ )
                     string[buffer_pos] = 
                     */
                     /* Send dis shit to the server */
                     Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[0] );
                     delay( IPX_DELAY );
                    } /* End if my responsibility */

               } /* End for */

           /* fprintf(stderr, "Done sending my guys...waiting for cylinder filename \n"); */

           done = 0;
           while( !done )
               {
                if( Jon_Kbhit() )
                    if( Jon_Getkey() == INDEX_ESC )
                        return(0);

                if( Packet_Ready() )
                    {
                     Pop_Packet( &game_packet ); 
                     if( game_over )
                        return(0);

                     if( terminate_connection ) /* Should put after every pop_packet */
                         return(0);

                     if( game_packet.string[0] == CYLINDER_FILENAME_PACKET )
                         {
                          /* fprintf(stderr, "Got cylinder filename \n"); */
                          done = 1;
                          for( i = 0; i < 20; i++ )
                              game_configuration->cylinder_filename[i] = game_packet.string[i+1];

                         }
                    }

               } /* End while not done */

           /* fprintf(stderr, "Waiting for all six player info packets \n"); */

           for( i = 0; i < 6; i++ )
               {
                /* Wait for all six packets */
                done = 0;
                while( !done )
                    {
                     if( Jon_Kbhit() )
                        if( Jon_Getkey() == INDEX_ESC )
                            return(0);

                     if( Packet_Ready() )
                         {
                          Pop_Packet( &game_packet ); 
                          if( game_over )
                              return(0);

                          if( terminate_connection ) /* Should put after every pop_packet */
                              return(0);

                          if( game_packet.string[0] == PLAYER_INFO_PACKET )
                              {
                               done = 1;
                               vehicles[(int)game_packet.string[1]] = game_packet.string[2];
                              }
                         } /* End if packet ready */
                    }

               } /* End for */

         } /* End if server */


     game_configuration->red0_vehicle  = vehicles[0];
     game_configuration->red1_vehicle  = vehicles[1];
     game_configuration->red2_vehicle  = vehicles[2];
     game_configuration->blue0_vehicle = vehicles[3];
     game_configuration->blue1_vehicle = vehicles[4];
     game_configuration->blue2_vehicle = vehicles[5];



     /* fprintf(stderr, "%s \n", game_configuration->cylinder_filename ); */
     /*
     for( i = 0; i < 6; i++ )
         fprintf(stderr, "Player %d's ship is %d \n", i, vehicles[i] );
     */
     /* Get_Keypress(); */


     return(1);

    } /* End of Synchronize_IPX_Game() */

/*
 format of a client input table packet

 byte : type of packet
 byte : number of tables being sent

 and for each table being sent...

 byte : index into player array
 ENCODED_INPUT_TABLE_SIZE : encoded input table
*/



int Client_Send_Input_Tables( IPX_game_info_type *ipx_game_info, WorldStuff *world_stuff )
    {
     encoded_input_table_type encoded_input_table;
     string_type string;
     short buffer_pos = 2;
     char num_to_send = 1;
     int i, j;
     game_packet_type game_packet;
     int done;

     done = 0;
     num_to_send = 0;


     while( !done )
         {
          if( Packet_Ready() )
              {
               Pop_Packet( &game_packet ); 
               if( game_over )
                   return(0);

               if( terminate_connection ) /* Should put after every pop_packet */
                   return(0);

               if( game_packet.string[0] == WAITING_FOR_INPUT_TABLE_PACKET )
                   {
                    done = 1;
                   }
              }
         }


     for( i = 0; i < 6; i++ )
         if( ipx_game_info->my_responsibility[i] )
             num_to_send++;

     string[0] = CLIENT_INPUT_TABLE_PACKET;
     string[1] = num_to_send;
     buffer_pos = 2;

     for( i = 0; i < 6; i++ )
         {
          if( ipx_game_info->my_responsibility[i] )
              {
               string[buffer_pos] = i;
               buffer_pos++;

               Encode_Input_Table( world_stuff->player_array[i].table, encoded_input_table );
               for( j = 0; j < ENCODED_INPUT_TABLE_SIZE; j++ )
                   {
                    string[buffer_pos] = encoded_input_table[j];
                    buffer_pos++;
                   }

              } /* End if */

         } /* End for */

     Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[0] );
     return(1);
    } /* End of Client_Send_Input_Tables() */



int Server_Send_Input_Tables( IPX_game_info_type *ipx_game_info, WorldStuff *world_stuff )
    {
     encoded_input_table_type encoded_input_table;
     string_type string;
     unsigned short buffer_pos = 1;
     int i, j;


     string[0] = SERVER_INPUT_TABLE_PACKET;
     buffer_pos = 1;

     for( i = 0; i < 6; i++ )
         {
          Encode_Input_Table( world_stuff->player_array[i].table, encoded_input_table );

          for( j = 0; j < ENCODED_INPUT_TABLE_SIZE; j++ )
              {
               string[buffer_pos] = encoded_input_table[j];
               buffer_pos++;
              }

         }

     /* Start at 1...we don't send a packet to ourselves */
     for( i = 1; i < ipx_game_info->num_players; i++ )
          Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[i] );

     return(1);
    } /* End of Server_Send_Input_Tables() */



int Client_Get_Input_Tables( IPX_game_info_type *ipx_game_info, WorldStuff *world_stuff )
    {
     short buffer_pos = 1;
     int i, j;
     encoded_input_table_type encoded_input_table;
     game_packet_type game_packet;
     int done = 0;


     while( !done )
         {
          if( Packet_Ready() )
              {
               Pop_Packet( &game_packet ); 
               if( game_over )
                   return(0);

               if( terminate_connection ) /* Should put after every pop_packet */
                   return(0);

               if( game_packet.string[0] == SERVER_INPUT_TABLE_PACKET )
                   {
                    done = 1;
                   }
              }
         }


     buffer_pos = 1;

     for( i = 0; i < 6; i++ )
         {
          for( j = 0; j < ENCODED_INPUT_TABLE_SIZE; j++ )
             {
              encoded_input_table[j] = game_packet.string[buffer_pos];
              buffer_pos++;
             }
          Decode_Input_Table( world_stuff->player_array[i].table, encoded_input_table );
         }

     return(1);
    } /* End of Client_Get_Input_Tables() */


/*
 format of a client input table packet

 byte : type of packet
 byte : number of tables being sent

 and for each table being sent...

 byte : index into player array
 ENCODED_INPUT_TABLE_SIZE : encoded input table
*/


int Server_Get_Input_Tables( IPX_game_info_type *ipx_game_info, WorldStuff *world_stuff )
    {
     int i, j, buffer_pos, number_to_decode, index;
     game_packet_type game_packet;
     string_type string;
     int done = 0;
     encoded_input_table_type encoded_input_table;


     string[0] = WAITING_FOR_INPUT_TABLE_PACKET;

     for( i = 1; i < ipx_game_info->num_players; i++ )
         {
          Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[i] );
          done = 0;

          while( !done )
              {
               if( Packet_Ready() )
                   {
                    Pop_Packet( &game_packet ); 
                    if( game_over )
                        return(0);

                    if( terminate_connection ) /* Should put after every pop_packet */
                        return(0);

                    if( game_packet.string[0] == CLIENT_INPUT_TABLE_PACKET )
                        {
                         done = 1;
                         number_to_decode = game_packet.string[1];
                         buffer_pos = 2;

                         while( number_to_decode > 0 )
                             {
                              index = game_packet.string[buffer_pos];
                              buffer_pos++;

                              for( j = 0; j < ENCODED_INPUT_TABLE_SIZE; j++ )
                                  {
                                   encoded_input_table[j] = game_packet.string[buffer_pos];
                                   buffer_pos++;
                                  }
                              Decode_Input_Table( world_stuff->player_array[index].table, encoded_input_table );
                              number_to_decode--;
                             }

                        }
                   }

              } /* End of while */


         } /* End of for */

     return(1);
    } /* End of Server_Get_Input_Table() */


void Send_End_Game_Packet( IPX_game_info_type *ipx_game_info )
    {
     string_type string;

     string[0] = END_IPX_GAME_PACKET;

     /*
     for( i = 1; i < ipx_game_info->num_players; i++ )
         {
          Jon_Send_Packet( string, sizeof(string_type), ipx_game_info->node_addresses[i] );
         }
     */
     Jon_Send_Packet( string, sizeof(string_type), broadcast_node );
    } /* End of Send_End_Game_Packet() */

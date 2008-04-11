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

#ifndef JONIPX_H

#define JONIPX_H

//#include <dos.h> /* For delay */

/* #define __LINUX__ */
/* Only use linux specific __attribute__ tag on Linux */
#ifndef PACKED_STRUCT 
	#ifdef __LINUX__
		#define PACKED_STRUCT __attribute__((packed))
	#else
		#define PACKED_STRUCT 
	#endif
#endif

#define MY_SOCKET 0x869C

#define MAX_PACKETS_IN_BUFFER 100

/* Amount we delay between sending packets */
#define IPX_DELAY                        3

#define WAITING_FOR_NODES_PACKET         10
#define PLAYER_NUMBER_ASSIGNMENT_PACKET  11
#define HERE_I_AM_PACKET                 12
#define GAME_INIT_PACKET                 13
#define BIG_ASS_GAME_INFO_PACKET         14

#define READY_PACKET                     15  /* Indicates we are ready to move on */

#define SERVER_INPUT_TABLE_PACKET        16
#define CLIENT_INPUT_TABLE_PACKET        17
#define WAITING_FOR_INPUT_TABLE_PACKET   18

#define TEAM_REQUEST_PACKET              19
#define TEAM_ASSIGNMENT_PACKET           20
#define WAITING_FOR_TEAM_REQUEST_PACKET  21


#define PLAYER_INFO_PACKET               22
#define CYLINDER_FILENAME_PACKET         23

#define TERMINATE_IPX_CONNECTION_PACKET  24
#define END_IPX_GAME_PACKET              25



/* This is the socket we will socket we will use for both reading and
   writing...i use this particular one because its the one they used for
   DOOM...'nuff said */


typedef unsigned char net_type[4];
typedef unsigned char node_address_type[6];

typedef char string_type[80];

typedef unsigned short address_type[2];

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
     net_type          net           PACKED_STRUCT;  /* Network address */
     node_address_type node_address  PACKED_STRUCT;  /* Node address */
     unsigned short    socket        PACKED_STRUCT;  /* Big endian socket number */
    } net_address_type;

typedef struct
    {
     net_type           net          PACKED_STRUCT; /* My network address */
     node_address_type  node_address PACKED_STRUCT; /* My node address */
    } local_address_type;

typedef struct
    {
     address_type      link              PACKED_STRUCT; /* Pointer to next ECB */
     unsigned long     ESR               PACKED_STRUCT; /* Event service routine 00000000h if none */
     unsigned char     in_use            PACKED_STRUCT; /* In use flag */
     unsigned char     complete          PACKED_STRUCT; /* Completing flag */
     unsigned short    socket            PACKED_STRUCT; /* Big endian socket number */
     unsigned char     IPX_work[4]       PACKED_STRUCT; /* IPX work space */
     unsigned char     D_work[12]        PACKED_STRUCT; /* Driver work space */
     node_address_type immediate_address PACKED_STRUCT; /* Immediate local node address */
     unsigned short    fragment_count    PACKED_STRUCT; /* Fragment count */
     unsigned long     fragment_data     PACKED_STRUCT; /* Pointer to data fragment */
     unsigned short    fragment_size     PACKED_STRUCT; /* Size of data fragment */
    } ECB_type;

typedef struct
    {
     unsigned short   checksum          PACKED_STRUCT;   /* Big endian checksum */
     unsigned short   length            PACKED_STRUCT;   /* Big endian length in bytes */
     unsigned char    transport_control PACKED_STRUCT;   /* Transport control */
     unsigned char    packet_type       PACKED_STRUCT;   /* Packet type */
     net_address_type destination       PACKED_STRUCT;   /* Destination network address */
     net_address_type source            PACKED_STRUCT;   /* Source network address */
    } IPX_header_type;



typedef struct
    {
     ECB_type        ecb         PACKED_STRUCT;
     IPX_header_type ipx_header  PACKED_STRUCT;
     string_type     string      PACKED_STRUCT;
    } packet_type;



typedef struct
    {
     string_type       string;
     node_address_type source_node;
    } game_packet_type;

typedef struct
    {
     game_packet_type    packets[MAX_PACKETS_IN_BUFFER]; /* Array of packets */
     unsigned short      buffer_start; /* The last packet we read */
     unsigned short      buffer_pos;   /* The most recent packet */
    } packet_buffer_type;


/* Don't forget to define constants */


int Compare_Nodes( node_address_type node_one, node_address_type node_two );
unsigned short Endian_Swap( unsigned short old_short );
int IPX_Open_Socket( unsigned char longetivity, unsigned short *socket_number );
void IPX_Close_Socket( unsigned short *socket_number );
void Get_Local_Address();
void IPX_Send_Packet( ECB_type *ecb );
int IPX_Listen_For_Packet( ECB_type *ecb );
void Im_Idle( void );
void Init_Send_Packet( ECB_type *ecb, IPX_header_type *ipx_header, 
                       unsigned short size, unsigned short sock );
void Init_Recieve_Packet( ECB_type *ecb, IPX_header_type *ipx_header, 
                          unsigned short size, unsigned short sock );
int Init_IPX( void );
int Init_Jonipx( void );
int Jon_Get_Packet( string_type string, node_address_type source_node );
int Jon_Send_Packet( string_type string, int length, node_address_type dest_node );
void Close_Jonipx( void );
int Packet_Ready( void );
int Pop_Packet( game_packet_type *packet );

#endif



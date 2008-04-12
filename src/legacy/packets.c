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

#include "packets.h"
#include "serial.h"
#include "util.h"
#include "menu.h" /* For the exit game cludge */

extern const char* g_DataPath;

char recieve_buffer[30]; /* in omega.c */
int recieve_length;
int frames_till_message_end;

/* From omega.c */
extern game_configuration_type game_configuration; 

extern MenuScreen main_menu; /* From menu.c */

extern long exit_loop;  /* From omega.c */
extern long game_over;


void Encode_Input_Table( input_table table, encoded_input_table_type encoded_input_table )
    {
     unsigned char encoded_byte = 0;
     int i;



     /* We copy the up down left right and sidestep shit straight */
     for( i = 0; i < 8; i++ )
         encoded_input_table[i] = table[i];

     /*
     for( i = 0; i < 8; i++ )
         {
          if( table[i] >= 1 )
              encoded_byte = encoded_byte | 01;    

          if( i < 7 )
              encoded_byte = encoded_byte << 1;
         }

     encoded_input_table[0] = encoded_byte;
     */
     encoded_byte = 0;

     for( i = 8; i < 15; i++ )
         {
          if( table[i] >= 1 )
             encoded_byte = encoded_byte | 01;    

          if( i < 14 )
              encoded_byte = encoded_byte << 1;
         }
     
     encoded_input_table[8] = encoded_byte;

    } /* End of encode input_table */

void Decode_Input_Table( input_table table, encoded_input_table_type encoded_input_table )
    {
     char encoded_byte;
     int i;

     encoded_byte = encoded_input_table[8];

     /*
     for( i = 7; i >= 0; i-- )
         {
          if( encoded_byte & 01 )
              table[i] = 1;
          else
              table[i] = 0;
          
          if( i > 0 )
              encoded_byte = encoded_byte >> 1;
         }

     encoded_byte = encoded_input_table[1];
     */

     for( i = 14; i >= 8; i-- )
         {
          if( encoded_byte & 01 )
              table[i] = 1;
          else
              table[i] = 0;
          
          if( i > 8 )
              encoded_byte = encoded_byte >> 1;
         }

     /* encoded_input_table[1] = encoded_byte; */
     for( i = 0; i < 8; i++ )
         table[i] = encoded_input_table[i];

    } /* End of decode_input_table */


void Get_Packets( Player player[] )
    {
     boolean exit = FALSE;
     char test_char;



     while( !exit )
         {
          Get_One_Character( &test_char );

          switch( test_char )
              {
               case INPUT_TABLE_PACKET :
                   Get_Input_Table_Packet( player );
                   break;

               case TEXT_MESSAGE_PACKET :
                   Get_Text_Message_Packet();
                   break;

               case END_OF_PACKETS_PACKET :
                  /* printf("Recieved end of packets packet \n");  */
                   exit = TRUE; /* We are done reading packets */
                   break;

               case RESYNCHRONIZATION_PACKET :
                   /* printf("Resynchronize packet...exiting\n"); */
                   Serial_Write( TERMINATE_CONNECTION_PACKET );
                   exit_gracefully(); 
                   break;

               case ORIENTATION_PACKET :
                   /* printf("Orientation packet...exiting\n"); */
                   Serial_Write( TERMINATE_CONNECTION_PACKET );
                   exit_gracefully(); 
                   break;

               case ERROR_PACKET :
                   /* printf("Error packet...exiting\n"); */
                   Serial_Write( TERMINATE_CONNECTION_PACKET );
                   exit_gracefully(); 
                   break;

               case TERMINATE_CONNECTION_PACKET :
                   /* printf("Terminate connection packet...exiting\n"); */
                   Serial_Write( TERMINATE_CONNECTION_PACKET );
                   exit_gracefully(); 
                   break;

               case EXIT_GAME_PACKET :
                   exit_loop = TRUE;
                   game_over = TRUE;
                   exit = TRUE; /* We are done reading packets */
                   break;
                   

               case FILE_TRANSFER_PACKET :
                   Get_File_Packet();
                   break;
               
               /* For game info packet we need a pointer to game_info type */
               /*
               case GAME_INFO_PACKET :            
                   Get_Game_Info_Packet();
                   break;
               */

               default :
                   /* printf("Invalid packet...exiting\n"); */
                   Serial_Write( TERMINATE_CONNECTION_PACKET );
                   exit_gracefully(); 
                   break;

              } /* End switch */

         

         } /* End while !exit */

    } /* End of get_packets */           


/* format of input table packet :
   char packet_type;
   char index;        * index into player array...can be 1-6  *
   input_table table; * array 1..INPUT_TABLE_SIZE of unsigned char *
*/

void Send_Input_Table_Packet( char index, input_table table )
    {
     int i;
     encoded_input_table_type encoded_input_table;

     Encode_Input_Table( table, encoded_input_table ); 
     
     Serial_Write( INPUT_TABLE_PACKET );
     Serial_Write( index );
     
     /*
     for( i = 0; i < INPUT_TABLE_SIZE; i++ )
         Serial_Write( table[i] );
     */
     
     for( i = 0; i < ENCODED_INPUT_TABLE_SIZE; i++ )
         Serial_Write( encoded_input_table[i] );
     
    } /* End of Send_Input_Table_Packet() */


void Get_Input_Table_Packet( Player player[] )
    {
     char index; /* Index into the player array */
     int i;
     encoded_input_table_type encoded_input_table;


     /* The packet_type byte has already been read by get_packets
        so we don't have to read it */

     Get_One_Character( &index ); /* Find out which one to update */

     /* fprintf( stderr, "Updating player %d \n", index ); */

        
        
     /*
     for( i = 0; i < INPUT_TABLE_SIZE; i++ )
         {
          Get_One_Character( &player[index].table[i] ); 
         } 
     */   

      
      for( i = 0; i < ENCODED_INPUT_TABLE_SIZE; i++ )
          Get_One_Character( &encoded_input_table[i] );

      Decode_Input_Table( player[(int)index].table, encoded_input_table );
      

    } /* End Get_Input_Table_Packet */


/* Format of a text message packet :
   char packet_type;
   char message_length;   * Number of characters in message *
   char message[length];
*/

void Send_Text_Message_Packet( char *message ) 
    {
     char message_length = 0;
     int i;
     int done = 0;

     Serial_Write( TEXT_MESSAGE_PACKET );

     while( !done ) /* Figure out length of message */
         {
          if( message[(int)(message_length++)] == '\0' )
              done = 1;    
         }     

     Serial_Write( message_length ); /* Tell remote machine the length of message */
     
     for( i = 0; i < message_length; i++ )
         {
          Serial_Write( message[i] );
         }
          
    } /* End of Send_Text_Message_Packet */


void Get_Text_Message_Packet( void )
    {
     char message_length;
     int i;

     frames_till_message_end = 30;

     Get_One_Character( &message_length ); /* Find out length of message */

     for( i = 0; i < message_length; i++ )
         {
          Get_One_Character( &recieve_buffer[i] ); /* Get one character of message */
         } 

     recieve_buffer[i] = '\0'; 
     
     recieve_length = message_length;

    } /* End Get_Text_Message_Packet */



void Send_Data_Chunk( char *send_buffer, char chunk_size )
    {
     char checksum = 0;
     int i;
     int exit = 0;
     char ch;
     char packet_num = 0; /* = 0 to shut up compiler */

     for( i = 0; i < chunk_size; i++ )
         { 
          checksum = checksum + send_buffer[i];
         }

     while( !exit )
         {
          Serial_Write( packet_num );
          Serial_Write( chunk_size );

          for( i = 0; i < chunk_size; i++ )
              Serial_Write( send_buffer[i] );

          Get_One_Character( &ch );
          
          if( ch == checksum )
              exit = 1;
         }

    } /* End of Send data chunk */
          

void Get_Data_Chunk( char *send_buffer )
    {
     char checksum = 0;
     int i;
     int exit = 0;
     char ch;
     char packet_num = 0; /* = 0 to shut up compiler */
     int chunk_size  = 0;

     for( i = 0; i < chunk_size; i++ )
         { 
          checksum = checksum + send_buffer[i];
         }

     while( !exit )
         {
          Serial_Write( packet_num );
          Serial_Write( (char)chunk_size );

          for( i = 0; i < chunk_size; i++ )
              Serial_Write( send_buffer[i] );

          Get_One_Character( &ch );
          
          if( ch == checksum )
              exit = 1;
         }

    } /* End of Get_Data_Chunk */



void Send_File_Packet( char *filename )
    {
     int exit = 0;
     FILE * fp;
     unsigned long file_size;
     unsigned long i = 0;
     char filename_length = 0;
     char *temp_ptr; /* For sending integers */
     char ch;
     int send_count;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);


     while( !exit )
         {
          if( filename[(int)(filename_length++)] == '\0' )
              exit = 1;
         }
     exit = 0;
     
     
     fp = fopen( newfilename, "rb" ); 

     fseek( fp, 0, SEEK_END ); /* Move to end of file */
     /* This should be the size of the file */
     file_size = ftell( fp ); 

     fseek( fp, 0, SEEK_SET ); /* Move back to beginning of file */


     printf("Sending packet %s size is %ld \n", filename, file_size );


     Serial_Write( FILE_TRANSFER_PACKET ); /* Identify type of packet */
     
     Serial_Write( filename_length ); /* Give filename length to remote */  

     for( i = 0; i < (unsigned long)filename_length; i++ ) /* Give filename to remote */
         Serial_Write( filename[i] );

     
     temp_ptr = (char *)&file_size; /* alias pointer to file_size */
     Serial_Write( temp_ptr[0] );
     Serial_Write( temp_ptr[1] ); /* Send the 32 bit size of the file */
     Serial_Write( temp_ptr[2] );
     Serial_Write( temp_ptr[3] );
     

     send_count = 0;
     for( i = 0; i < file_size; i++ )
         {
          fread( &ch, 1, 1, fp );
          Serial_Write( ch );
          /* printf("Pos is %d \n", i ); */
         
          if( send_count++ == 9 )
              {
               printf("Pausing %d \n", i);
               Get_One_Character( &ch ); 
               send_count = 0;
              }
         
         }
 
     
     fclose( fp ); 


    } /* End of send_file packet */
  

void Get_File_Packet( void )
    {
     char filename_length, ch;
     char filename[20];
     unsigned long i;
     unsigned long file_size;
     char *temp_ptr;
     FILE *fp;
     int send_count;
	char newfilename[512];




     /* Get length of filename */          
     Get_One_Character( &filename_length );
     
     for( i = 0; i < (unsigned long)filename_length; i++ )
         {
          Get_One_Character( &filename[i] );
         }
     filename[i] = '\0';

          
     temp_ptr = (char *)&file_size;

     Get_One_Character( &temp_ptr[0] ); /* Read the 32 bit */
     Get_One_Character( &temp_ptr[1] ); /* File size */
     Get_One_Character( &temp_ptr[2] ); 
     Get_One_Character( &temp_ptr[3] ); 


     printf("Got packet %s size is %ld \n", filename, file_size );

	sprintf(newfilename,"%s%s",g_DataPath,filename);
     fp = fopen( newfilename, "wb" );

     send_count = 0;
     for( i = 0; i < file_size; i++ )
         {
          Get_One_Character( &ch );
          fwrite( &ch, 1, 1, fp );
          /* printf("Pos is %d \n", i ); */
          
          if( send_count++ == 9 )
              {
               printf("Sending %d \n", i);
               Serial_Write( (char)0xff );
               send_count = 0;
              }
         }

     fclose( fp );     
     

    } /* end of Get_File_Packet */



void Send_Game_Info_Packet( game_info_type *game_info )
    {
     char *ch;
     int i;

     ch = (char *)game_info; /* Alias pointer to structure */
                 
     /* printf("Sending game info \n"); */
     for( i = 0; i < sizeof(game_info_type); i++ )
         {
          Serial_Write( ch[i] );
         } /* end of for */
    } /* End of Send_Game_Info_Packet() */

void Get_Game_Info_Packet( game_info_type *game_info )
    {
     char *ch;
     int i;

     ch = (char *)game_info; /* Alias pointer to structure */
     /* printf("Recieving game info \n"); */
     for( i = 0; i < sizeof(game_info_type); i++ )
         {
          Get_One_Character( &ch[i] );
         } /* end of for */    

    } /* End of Send_Game_Info_Packet() */


void Get_Remote_Key_Table( input_table table )
    {
     int i;

     for( i = 0; i < INPUT_TABLE_SIZE; i++ )
         {
          while( Serial_Read( &table[i] ) == -1 )
              {}
         }      

    } /* End of get_remote_key_table */

void Send_Key_Table( input_table table )     
    {
     int i;


     for( i = 0; i < INPUT_TABLE_SIZE; i++ )
         {
          Serial_Write( table[i] );
         }      
    
    }


void Send_End_Of_Packets_Packet( void ) 
    {
      Serial_Write( END_OF_PACKETS_PACKET ); 
    }





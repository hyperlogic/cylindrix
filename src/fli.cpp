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

#include <stdio.h>
#include "fli.h"
#include "util.h" /* For exit_gracefully() */
#include "keys.h"

extern const char* g_DataPath;

extern long TIMER_CLICKS_PER_SECOND; /* from omega.c */
extern long GAME_CLICKS_PER_SECOND;  /* */
extern boolean ceiling_on;           /* */


int current_frame;

uint32_t first_frame = 0;
uint32_t file_length = 0;

/* Delta chunk is the most common type of chunk...
   it contains the changes in the image from the last
   frame */
void Delta_Chunk( FILE *fp )
    {
     uint32_t line_index;       /* Number of lines processed  */
     uint32_t pixel_index = 0;  /* Number of pixels processed */
     uint32_t pos;              /* Position in file           */
     
     int16_t lines_to_skip;           /* Number of lines to skip    */
     uint16_t number_lines;   /* Number of lines encoded for frame */
     
     uint8_t num_packets;     /* Number of packets in line  */
     
     uint32_t packet_index;     /* Index for loop             */
     uint8_t skip_count;      /* Number of xpixels to skip  */
     int8_t packet_type;              /* Type of delta encoding     */
     
     uint8_t pixel_data;      /* One pixel to put on screen */
     uint32_t byte_count;       /* Index for loop             */
     int32_t x, y;


     pos = ftell( fp ); /* Record our position in the file */

     /* Get the y position we start on */ 
     fread( &lines_to_skip, sizeof(int16_t), 1, fp );  
     /* Get the number of lines encoded */
     fread( &number_lines,  sizeof(int16_t), 1, fp );
     
     x = y = 0;

     y += lines_to_skip; /* Move to line lines_to_skip */
     

     for( line_index = 0; line_index < number_lines; line_index++ )
         {
          fread( &num_packets, sizeof( unsigned char ), 1, fp );

          for( packet_index = 0; packet_index < num_packets; packet_index++ )
              {
               /* Get number of x pixels to skip on this packet */
               fread( &skip_count, sizeof( unsigned char ), 1, fp );           
               /* Get type of packet encoding */
               fread( &packet_type, sizeof( char ), 1, fp );
               
               x += skip_count; /* Add the skip count to our x value */
               
               /* If packet type is positive, copy packet_type
                  number of pixels from the file to the screen... */
               if( packet_type > 0 )
                   {
                    for( byte_count = 0; byte_count < (uint32_t)packet_type; byte_count++ ) 
                        {
                         fread( &pixel_data, sizeof( char ), 1, fp );
                         pixel_index++;
                         DB_Fast_Pixel( x, y, pixel_data ); 
                         x++;
                        }
                   }
               /* If packet type is negative, take its absolute value,
                  and replicate one byte from the file packet_type 
                  times */
               else
                   {
                    packet_type = -packet_type;
                    fread( &pixel_data, sizeof(char), 1, fp );

                    for( byte_count = 0; byte_count < (uint32_t)packet_type; byte_count++ )
                        {
                         pixel_index++;
                         DB_Fast_Pixel( x, y, pixel_data ); 
                         x++;
                        }

                   } /* End if else */
    
              } /* End for packet index */

         y++;   /* Move to next line */
         x = 0; /* Move to beginning of line */ 
         
         } /* End for line index */    
    
    } /* End of delta_chunk */


void Color_Chunk( FILE *fp )
    {
     uint16_t num_packets; 
     RGB_color color;

     /* Packet */
     uint8_t skip_count;
     uint8_t color_count;
     uint8_t color_index = 0; /* Index into 256 palette */
     uint8_t r,g,b;
     int32_t i, j;
     
     fread( &num_packets, sizeof(uint16_t), 1, fp );     


     for( i = 0; i < num_packets; i++ )
         {
          fread( &skip_count, sizeof( unsigned char ), 1, fp );
          fread( &color_count, sizeof( unsigned char ), 1, fp );    

          /* if skip count is zero, then we are to update every
             palette register */
          if( skip_count == 0 )
              {

               for( j = 0; j < 256; j++ )
                   {
                    fread( &r, sizeof( unsigned char ), 1, fp );
                    fread( &g, sizeof( unsigned char ), 1, fp );
                    fread( &b, sizeof( unsigned char ), 1, fp );
                    color.red   = r;
                    color.green = g;
                    color.blue  = b;   
                    Set_Palette_Register( color_index, &color );
                    color_index++;
                   }
              
              }
          else
              {
               color_index += skip_count;
               
               for( j = 0; j < color_count; j++ )
                   {
                    fread( &r, sizeof( unsigned char ), 1, fp );
                    fread( &g, sizeof( unsigned char ), 1, fp );
                    fread( &b, sizeof( unsigned char ), 1, fp );
                    color_index++;
                   }  
              
               fread( &color_count, sizeof( unsigned char ), 1, fp );    
              } /* End if/else */
         
         } /* End for packets */
    
    } /* End of color_chunk */
 

void Copy_Chunk( FILE *fp )  
    {
     uint32_t x, y;
     unsigned char pixel_data;

     /* A copy chunk just has 64000 bytes of data to pop
        straight to video memory */

     for( y = 0; y < 200; y++ )
         {
          for( x = 0; x < 320; x++ )
              {
               fread( &pixel_data, sizeof( unsigned char ), 1, fp );
               DB_Fast_Pixel( x, y, pixel_data );
              }
         }
    }



void Byte_Run_Chunk( FILE *fp )
    {
     int32_t i, index;
     char temp_char, temp_char_two;
     int32_t x, y;

     x = y = 0;
     index = 0;
     
     while( index < 64000 )
         {
          fread( &temp_char, sizeof( char ), 1, fp ); /* Skip first char */
             
          while( x < 320 )
              {
               fread( &temp_char, sizeof( char ), 1, fp );
               
               /* If the character is negative, we read -temp_char
                  number of pixels from the file to the video memory */
               if( temp_char < 0 )
                   {
                    for( i = 0; i < -temp_char; i++ )
                        {
                         fread( &temp_char_two, sizeof( char ), 1, fp );       
                         DB_Fast_Pixel( x, y, temp_char_two );
                         x++;
                         index++;
                        }
                   }
               /* else temp_char > 0, we copy one byte from the 
                  file to video memory temp_char times  */
               else 
                   {
                    fread( &temp_char_two, sizeof( char ), 1, fp );  
                    
                    for( i = 0; i < temp_char; i++ )
                        {
                         DB_Fast_Pixel( x, y, temp_char_two );
                         x++;
                         index++;
                        }
                   
                   } /* End if/else */

              } /* End while */
               
          y++;    /* At the end of a line increment the y */
          x = 0;  /* And reset x to zero */

         } /* End while index < 64000 */

    } /* End of byte_run */


/* Read a sub chunk and branch to the appropriate
   function for each type of chunk */
int Read_Sub_Chunk( FILE *fp ) {
     sub_chunk_header header;
     
     fread( &header.chunk_size, sizeof( uint32_t ), 1, fp );     
     fread( &header.chunk_type, sizeof( uint16_t ), 1, fp );

     if( header.chunk_type == 0xC )
         {
          Delta_Chunk( fp ); 
         }
     else if( header.chunk_type == 0x0B )
         { 
          first_frame = -1; /* Set first frame flag */
          Color_Chunk( fp );
         }
     else if( header.chunk_type == 0x0F )
         {
          Byte_Run_Chunk( fp );
         }
     else if( header.chunk_type == 0x10 )
         {
          Copy_Chunk( fp );
         }
     else
         {
          fprintf(stderr, "Subchunk is bad type  %x\n", header.chunk_type); 
          
		  return(0); //Johnm 12/2/20001 - made return 0 on error
         }
	return(1);
}

/* Process one frame chunk */
long Read_Chunk( FILE *fp )
    {
     chunk_header header;
     int32_t i, pos;

     /* Read in frame chunk header */
     fread( &header, sizeof( chunk_header ), 1, fp );

     if( feof( fp ) )
         return(0);

     pos = ftell( fp ); /* Record our position in file */
     
     /* If there are subchunks */
     if( header.number_of_chunks > 0 )
         {   
          for( i = 0; i < header.number_of_chunks; i++ )
              {
               if( !Read_Sub_Chunk( fp ) )
				   return(0);
              } 
         } /* End if */
     
    
     fseek( fp, pos, SEEK_SET );

     /* Skip to the end of this chunk */ 
     fseek( fp, header.chunk_size - sizeof( chunk_header ), SEEK_CUR ); 
     

     /* If the first_frame flag (-1) is set, record our current 
        position as the first frame */
     if( first_frame == -1 )
         {
          first_frame = ftell( fp );
         }

     return(1);

    } /* End of Read_Chunk */


void Play_Fli( const char *filename )
{
	flic_header header;     /* Header for the entire file  */
	FILE *fp;               /* Pointer to opened .fli file */
	unsigned char exit = 0; /* Flag for the loop           */
	char newfilename[512];
	uint32_t time;

	sprintf(newfilename,"%s%s",g_DataPath,filename);

	fp = fopen( newfilename, "rb" );  /* Open file  */

	fread( &header, sizeof( flic_header ), 1, fp ); /* Get the flic header */

	fseek( fp, 128, SEEK_SET ); /* Move to end of header */

	DB_Clear_Screen();  /* Clear the double buffer */ 

	time = SYS_GetTimeMS();

	while ( !feof( fp ) && !exit )
	{
		if ( Read_Chunk( fp ) == 0 ) /* If we reach the last chunk */
			exit = 1;

		if( exit == 1 ) /* Reset to first frame (set in color_chunk) */
		{
			fseek( fp, first_frame, SEEK_SET );  
			/*    exit = 0; */
		}

		Swap_Buffer(); /* Swap the double buffer into video memory */

		while ( SYS_GetTimeMS() < (time + 33) );
		time = SYS_GetTimeMS();

// AJT:
//		if( Jon_Kbhit() ) /* If the user presses a key, exit */
//		{
//			exit = 1;
//		}
	} 
     
	fclose( fp ); /* Close the file */
}



/* Beginning of Ram based fli reading */



void Delta_Chunk_Ram( uint8_t *file_buffer, uint32_t *file_pos )
    {
     unsigned long line_index;       /* Number of lines processed  */
     unsigned long pixel_index = 0;  /* Number of pixels processed */
     
     short lines_to_skip;           /* Number of lines to skip    */
     unsigned short number_lines;   /* Number of lines encoded for frame */
     
     unsigned char num_packets;     /* Number of packets in line  */
     
     unsigned long packet_index;     /* Index for loop             */
     unsigned char skip_count;      /* Number of xpixels to skip  */
     char packet_type;              /* Type of delta encoding     */
     
     unsigned char pixel_data;      /* One pixel to put on screen */
     unsigned long byte_count;       /* Index for loop             */
     long x, y, i;
     unsigned char *temp_buffer;


     /* Get the y position we start on */ 
     temp_buffer = (unsigned char *)&lines_to_skip;
     for( i = 0; i < sizeof( short ); i++ )
         temp_buffer[i] = file_buffer[ (*file_pos)++ ];
     /* Get the number of lines encoded */
     temp_buffer = (unsigned char *)&number_lines;
     for( i = 0; i < sizeof( short ); i++ )
         temp_buffer[i] = file_buffer[ (*file_pos)++ ];
     

     x = y = 0;

     y += lines_to_skip; /* Move to line lines_to_skip */
     

     for( line_index = 0; line_index < number_lines; line_index++ )
         {
          num_packets = file_buffer[ (*file_pos)++ ];

          for( packet_index = 0; packet_index < num_packets; packet_index++ )
              {
               /* Get number of x pixels to skip on this packet */
               skip_count  = file_buffer[ (*file_pos)++ ];   

               /* Get type of packet encoding */
               packet_type = file_buffer[ (*file_pos)++ ];   

               x += skip_count; /* Add the skip count to our x value */


               /* If packet type is positive, copy packet_type
                  number of pixels from the file to the screen... */
               if( packet_type > 0 )
                   {
                    for( byte_count = 0; byte_count < (unsigned long)packet_type; byte_count++ ) 
                        {
                         pixel_data = file_buffer[ (*file_pos)++ ];   
                         pixel_index++;
                         DB_Fast_Pixel( x, y, pixel_data );
                         x++;
                        }
                   }
               /* If packet type is negative, take its absolute value,
                  and replicate one byte from the file packet_type 
                  times */
               else
                   {
                    packet_type = -packet_type;
                    pixel_data = file_buffer[ (*file_pos)++ ];   
                    for( byte_count = 0; byte_count < (unsigned long)packet_type; byte_count++ )
                        {
                         pixel_index++;
                         DB_Fast_Pixel( x, y, pixel_data ); 
                         x++;
                        }

                   } /* End if else */
    
              } /* End for packet index */

         y++;   /* Move to next line */
         x = 0; /* Move to beginning of line */ 
         
         } /* End for line index */    
    
    } /* End of Delta_Chunk_Ram() */


void Color_Chunk_Ram( uint8_t *file_buffer, uint32_t *file_pos )
    {
     unsigned short num_packets; 
     RGB_color color;
     
     unsigned char *temp_buffer;

     /* Packet */
     unsigned char skip_count;
     unsigned char color_count;
     unsigned char color_index = 0; /* Index into 256 palette */
     unsigned char r,g,b;
     long i, j;
     

     temp_buffer = (unsigned char *)&num_packets;
     for( i = 0; i < sizeof( uint16_t ); i++ )
         {
          temp_buffer[i] = file_buffer[ (*file_pos)++ ];
         }

     for( i = 0; i < num_packets; i++ )
         {
          skip_count = file_buffer[ (*file_pos)++ ];
          color_count = file_buffer[ (*file_pos)++ ];     
          
          
          /* if skip count is zero, then we are to update every
             palette register */
          if( skip_count == 0 )
              {

               for( j = 0; j < 256; j++ )
                   {
                    r = file_buffer[ (*file_pos)++ ];    
                    g = file_buffer[ (*file_pos)++ ];    
                    b = file_buffer[ (*file_pos)++ ];   
                    color.red   = r;
                    color.green = g;
                    color.blue  = b;   
                    Set_Palette_Register( color_index, &color );
                    color_index++;
                   }
              
              }
          else
              {
               color_index += skip_count;
               
               for( j = 0; j < color_count; j++ )
                   {
                    r = file_buffer[ (*file_pos)++ ];    
                    g = file_buffer[ (*file_pos)++ ];    
                    b = file_buffer[ (*file_pos)++ ];   
                    color_index++;
                   }  
              
              color_count = file_buffer[ (*file_pos)++ ];  
              } /* End if/else */
         
         } /* End for packets */
    
    } /* End of Color_Chunk_Ram() */
 

void Copy_Chunk_Ram( uint8_t *file_buffer, uint32_t *file_pos  )  
    {
     long x, y;
     unsigned char pixel_data;

     /* A copy chunk just has 64000 bytes of data to pop
        straight to video memory */

     for( y = 0; y < 200; y++ )
         {
          for( x = 0; x < 320; x++ )
              {
               pixel_data = file_buffer[ (*file_pos)++ ];
               DB_Fast_Pixel( x, y, pixel_data );
              }
         }
    } /* End of Copy_Chunk_Ram() */



void Byte_Run_Chunk_Ram( uint8_t *file_buffer, uint32_t *file_pos )
    {
     long i, index;
     char temp_char, temp_char_two;
     long x, y;
     

     x = y = 0;
     index = 0;
     
     while( index < 64000 )
         {
          /* fread( &temp_char, sizeof( char ), 1, fp ); */ /* Skip first char */
                       
          temp_char = file_buffer[ (*file_pos)++ ];

          while( x < 320 )
              {
              /* fread( &temp_char, sizeof( char ), 1, fp ); */
               temp_char = file_buffer[ (*file_pos)++ ];   
               
               /* If the character is negative, we read -temp_char
                  number of pixels from the file to the video memory */
               if( temp_char < 0 )
                   {
                    for( i = 0; i < -temp_char; i++ )
                        {
                         /* fread( &temp_char_two, sizeof( char ), 1, fp );   */
                         temp_char_two = file_buffer[ (*file_pos)++ ];   
                         DB_Fast_Pixel( x, y, temp_char_two );
                         x++;
                         index++;
                        }
                   }
               /* else temp_char > 0, we copy one byte from the 
                  file to video memory temp_char times  */
               else 
                   {
                    /* fread( &temp_char_two, sizeof( char ), 1, fp );  */
                    temp_char_two = file_buffer[ (*file_pos)++ ];   
                    
                    for( i = 0; i < temp_char; i++ )
                        {
                         DB_Fast_Pixel( x, y, temp_char_two );
                         x++;
                         index++;
                        }
                   
                   } /* End if/else */

              } /* End while */
               
          y++;    /* At the end of a line increment the y */
          x = 0;  /* And reset x to zero */

         } /* End while index < 64000 */

    } /* End of Byte_Run_Chunk_Ram() */


/* Read a sub chunk and branch to the appropriate
   function for each type of chunk */
int Read_Sub_Chunk_Ram( uint8_t *file_buffer, uint32_t *file_pos )
    {
     sub_chunk_header header;
     long i;
     unsigned char *temp_buffer;


     temp_buffer = (unsigned char *)&header;

     for( i = 0; i < sizeof(sub_chunk_header); i++ )
         {
          temp_buffer[i] = file_buffer[ (*file_pos)++ ];
         }

     if( header.chunk_type == 0xC )
         {
          current_frame++;
          Delta_Chunk_Ram( file_buffer, file_pos ); 
         }
     else if( header.chunk_type == 0x0B )
         { 
          first_frame = -1; /* Set first frame flag */
          Color_Chunk_Ram( file_buffer, file_pos );
         }
     else if( header.chunk_type == 0x0F )
         {
          Byte_Run_Chunk_Ram( file_buffer, file_pos  );
          current_frame++;
         }
     else if( header.chunk_type == 0x10 )
         {
          Copy_Chunk_Ram( file_buffer, file_pos );
          current_frame++;
         }
     else
         {
          fprintf(stderr, "Subchunk is bad type  %x\n", header.chunk_type); 

		  return(0); //Johnm 12/2/2001 - don't exit whole program on bad .fli
// AJT
//          exit_gracefully();
         }

	 return(1);
} /* End of Read_Sub_Chunk_Ram() */

/* Process one frame chunk */
long Read_Chunk_Ram( uint8_t *file_buffer, uint32_t *file_pos )
    {
     chunk_header header;
     long i;
     unsigned long pos;
     unsigned char *temp_buffer;


     /* Alias pointer to chunk header */
     temp_buffer = (unsigned char *)&header;

     if( *file_pos >= file_length )
         return 0;

     pos = *file_pos;

     /* Get chunk header from array */
     for( i = 0; i < sizeof( chunk_header ); i++ )
         {
          temp_buffer[i] = file_buffer[ (*file_pos)++ ]; 
         }

     /* If there are subchunks */
     if( header.number_of_chunks > 0 )
         {

          for( i = 0; i < header.number_of_chunks; i++ )
              {
               if( !Read_Sub_Chunk_Ram( file_buffer, file_pos ) )
				   return(0);
              } 
         } /* End if */

     *file_pos = pos + header.chunk_size;
     
     /* If the first_frame flag (-1) is set, record our current 
        position as the first frame */
     if( first_frame == -1 )
         {
          first_frame = *file_pos;
         }

     return(1);

    } /* End of Read_Chunk_Ram */


void Play_Fli_Ram( char *filename )
    {
     flic_header header;     /* Header for the entire file  */
     FILE *fp;               /* Pointer to opened .fli file */
     unsigned char exit = 0; /* Flag for the loop           */
     unsigned char *file_buffer;
     uint32_t file_pos = 0;
     long temp;
     long i;
     unsigned char *temp_buffer;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);



     fp = fopen( newfilename, "rb" );  /* Open file  */

     /* Allocate space in Ram for fli file */
     fseek( fp, 0, SEEK_END ); /* Move to end of file */
    
     /* This should be the size of the file */
     file_buffer = (unsigned char*)malloc( temp = ftell( fp ) );

     fseek( fp, 0, SEEK_SET ); /* Move back to beginning of file */
     fread( file_buffer, temp, 1, fp ); /* Read whole file into ram */

     fclose( fp ); /* Close the file */     
     
     
     /* Alias pointer to header */
     temp_buffer = (unsigned char *)&header;

     for( i = 0; i < 128; i++ ) /* Read in the header from ram */
         {
          temp_buffer[i] = file_buffer[i];
          file_pos++;
         }
     
     file_length = header.file_size;
     
     DB_Clear_Screen();  /* Clear the double buffer */ 

     while( !exit )
        {
         
         if ( Read_Chunk_Ram( file_buffer, &file_pos ) == 0 ) /* If we reach the last chunk */
             exit = 1;

         if( exit == 1 ) /* Reset to first frame (set in color_chunk) */
             {
              file_pos = first_frame;
              exit = 0;  
             }

         Swap_Buffer(); /* Swap the double buffer into video memory */

// AJT         
//         if( Check_Raw() == 1 || Jon_Kbhit() ) /* If the user presses a key, exit */
//             { 
//              if( Jon_Kbhit() ) /* Eat the keypress */
//                  Jon_Getkey();
//
//              while( Check_Raw() == 1 ) {}
//              exit = 1;
//             }
     } 
    
     free( file_buffer );
     DB_Clear_Screen();

     Swap_Buffer();

    } /* End of play_fli_ram */


/*
typedef struct
    {
     unsigned char *file_buffer;
     unsigned long file_length;
     unsigned long file_pos;
     unsigned long first_frame;
    } fli_file_type;
*/

void Load_Fli( char *filename, fli_file_type *fli_file  )
    {
     flic_header header;     /* Header for the entire file  */
     FILE *fp;               /* Pointer to opened .fli file */
     long temp;
     long i;
     unsigned char *temp_buffer;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);


     
     fli_file->file_length = 0;
     fli_file->file_pos    = 0;
     fli_file->first_frame = 0;


     fp = fopen( newfilename, "rb" );  /* Open file  */

     /* Allocate space in Ram for fli file */
     fseek( fp, 0, SEEK_END ); /* Move to end of file */
    
     /* This should be the size of the file */
     fli_file->file_buffer = (unsigned char*)malloc( temp = ftell( fp ) );

     fseek( fp, 0, SEEK_SET ); /* Move back to beginning of file */
     fread( fli_file->file_buffer, temp, 1, fp ); /* Read whole file into ram */

     fclose( fp ); /* Close the file */     
     
     
     /* Alias pointer to header */
     temp_buffer = (unsigned char *)&header;

     for( i = 0; i < 128; i++ ) /* Read in the header from ram */
         {
          temp_buffer[i] = fli_file->file_buffer[i];
          fli_file->file_pos++;
         }
     
     fli_file->file_length = header.file_size;


    } /* End of Load_Fli */



void Play_Loaded_Fli( fli_file_type *fli_file )
    {
     unsigned char exit = 0; /* Flag for the loop           */
     

     file_length = fli_file->file_length; /* Update global */


     DB_Clear_Screen();  /* Clear the double buffer */ 

     while( !exit )
        {
         
         if ( Read_Chunk_Ram( fli_file->file_buffer, &fli_file->file_pos ) == 0 ) /* If we reach the last chunk */
             exit = 1;

         if( exit == 1 ) /* Reset to first frame (set in color_chunk) */
             {
              fli_file->file_pos = first_frame;
              exit = 0;  
             }

         Swap_Buffer(); /* Swap the double buffer into video memory */
 
// AJT		 
//         if( Check_Raw() == 1 || Jon_Kbhit() ) /* If the user presses a key, exit */
//             { 
//              if( Jon_Kbhit() ) /* Eat the keypress */
//                  Jon_Getkey();
//
//              while( Check_Raw() == 1 ) {}
//              exit = 1;
//            }
        } 
    
     DB_Clear_Screen();

     Swap_Buffer();

     fli_file->file_pos = 128;


    } /* End of play_fli_ram */


void Free_Fli( fli_file_type *fli_file )
    {
     free( fli_file->file_buffer );
    }


void One_Frame( fli_file_type *fli_file )
    {
     long done = 0;

     
     file_length = fli_file->file_length; /* Update global */
     fli_file->first_frame = 128;

     
     if ( Read_Chunk_Ram( fli_file->file_buffer, &fli_file->file_pos ) == 0 ) /* If we reach the last chunk */
         done = 1;

     if( done == 1 ) /* Reset to first frame (set in color_chunk) */
         {
          fli_file->file_pos = fli_file->first_frame;
         }

     Swap_Buffer(); /* Swap the double buffer into video memory */

    } /* End of One_Frame */




/* Size of buffer we load chunks of fli file into */
#define FLI_BUFFER_SIZE 66000
/*
typedef struct
    {
     unsigned char *file_buffer;
     unsigned long file_length;
     unsigned long file_pos;
     unsigned long first_frame;
    } fli_file_type;
*/

void Play_Fli_Buffered( char *filename )
    {
     unsigned char done = 0; /* Flag for the loop           */
     FILE *fp;
     unsigned char *buffer_one, *buffer_two, *current_buffer, *old_buffer;
     flic_header header;
     uint32_t file_pos = 0;
     int flip = 0;
     unsigned long bytes_left = 0;
     unsigned long end_file_pos, current_file_pos;
     int done_reading = 0; /* Flag to say we're done reading file */
     long i, temp;
   	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);

 

     DB_Clear_Screen(); /* Clear screen before enabling palette */
     Swap_Buffer();

     /* Open the damned file */
     fp = fopen( newfilename, "rb" );

	 if( !fp )
		 return;

     /* Read the damm header */
     fread( &header, sizeof( header ), 1, fp);

     current_frame = 0;


     /*
     fprintf(stderr, "Number of frames is %d \n", header.number_of_frames );
     Get_Keypress();
     */

     /* Prime the buffer */
     if( (buffer_one = (unsigned char*)malloc( FLI_BUFFER_SIZE )) == NULL )
         {
          fprintf(stderr, "Error mallocing memory in FLI FILE reader \n");
          return;
         }
     if( (buffer_two = (unsigned char*)malloc( FLI_BUFFER_SIZE )) == NULL )
         {
          fprintf(stderr, "Error mallocing memory in FLI FILE reader \n");
          return;
         }

     fread( buffer_one, FLI_BUFFER_SIZE, 1, fp );
     current_buffer = buffer_one;
     old_buffer     = buffer_two;
     
     file_length = FLI_BUFFER_SIZE; /* Update global */

     //Set_Timer(0);  // AJT:

     while( done == 0 )
         {
          if( Read_Chunk_Ram( current_buffer, &file_pos ) == 0 ) /* If we reach the last chunk */
              done = 1;

          if( current_frame > header.number_of_frames - 2 )
              done = 1;

		  // AJT:
          /*
          if( Jon_Kbhit() )
              if( Jon_Getkey() == INDEX_ESC )
                  done = 1;
          */

// AJT
//          if( ceiling_on )
//              while( Check_Timer() < ((float)TIMER_CLICKS_PER_SECOND / (float)25) );

//          Set_Timer(0);    

//          Wait_For_Vsync();
          if( !done )
		  {
              Swap_Buffer(); /* Swap the double buffer into video memory */
		  }


                    
          /* If we have read in enough shizit load summore */
          if( (file_pos > (1)) && !done_reading )
              {
               current_file_pos = ftell( fp );
               fseek( fp, 0, SEEK_END );
               end_file_pos = ftell( fp );
               fseek( fp, current_file_pos, SEEK_SET );
               bytes_left = end_file_pos - current_file_pos;
               /* fprintf(stderr, "bytes left is %ld \n", bytes_left );  */
               
               if( bytes_left > 0 )
                   {
                    if( !flip )
                        {
                         flip = !flip;
                         old_buffer     = buffer_one;
                         current_buffer = buffer_two;
                        }
                    else
                        {
                         flip = !flip;
                         old_buffer     = buffer_two;
                         current_buffer = buffer_one;
                        }
                    
                    temp = 0;
                    for( i = file_pos; i < FLI_BUFFER_SIZE; i++ )
                         {
                          current_buffer[temp++] = old_buffer[i];
                         } /* End for */
                    
                    file_pos = 0;
                    if( bytes_left >= (unsigned long)(FLI_BUFFER_SIZE - temp) )
                        {
                         fread( &current_buffer[temp], (FLI_BUFFER_SIZE - temp), 1, fp ); 
                        }
                    else    
                        {
                         fread( &current_buffer[temp], bytes_left, 1, fp );
                         file_length = temp + bytes_left;
                        }
                   
                   }
               else
                   {
                    done_reading = 1;
                   }
              } /* End if time to load shizit */

         } /* End while not done */

     DB_Clear_Screen();
     Swap_Buffer();

     free( buffer_one ); /* Free da memory */
     free( buffer_two );
    
    } /* End of Play_Fli_Buffered */

static flic_header gCurrentHeader;
static FILE* gCurrentFile;
static unsigned long gFrameTime;

void PlayFliEnter( const char* filename )
{
	char newfilename[512];
	
	sprintf( newfilename, "%s%s", g_DataPath, filename );

	// open the fli
	gCurrentFile = fopen( newfilename, "rb" );
	if ( !gCurrentFile )
	{
		SYS_Error( "could not open FLI file!\n" );
		return;
	}

	fread( &gCurrentHeader, sizeof( flic_header ), 1, gCurrentFile ); /* Get the flic header */

	fseek( gCurrentFile, 128, SEEK_SET ); /* Move to end of header */

	DB_Clear_Screen();  /* Clear the double buffer */ 

	gFrameTime = SYS_GetTimeMS();
}

int PlayFliProcess()
{	
	int exit = 0;

	if ( feof( gCurrentFile ) )
		return 0;

	if ( SYS_GetTimeMS() < (gFrameTime + 33) )
		return 1;

	gFrameTime = SYS_GetTimeMS();

	if ( Read_Chunk( gCurrentFile ) == 0 ) /* If we reach the last chunk */
		return 0;

	Swap_Buffer(); /* Swap the double buffer into video memory */

    if ( SYS_KeyDown( KEY_ESC ) )
        return 0;

	return 1;
}

void PlayerFliExit()
{
	fclose( gCurrentFile ); /* Close the file */
	gCurrentFile = 0;
}

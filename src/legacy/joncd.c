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

#include "joncd.h" 
#include "dosbuff.h"

//#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

/*
#include <dpmi.h>
#include <go32.h>
#include <dos.h>
#include <pc.h>
#include <sys/farptr.h>
*/

#include <string.h>


#define CD_DELAY 1500


/* Globals */

extern char cdrom_drive_letter; /* From cuts.c */

cdrom_data_type cdrom_data;

extern unsigned short segment_of_dos_buffer, offset_of_dos_buffer;
extern unsigned long address_of_dos_buffer;
extern unsigned long linear_address_of_dos_buffer;

extern unsigned short segment_of_dos_buffer_two, offset_of_dos_buffer_two;
extern unsigned long address_of_dos_buffer_two;
extern unsigned long linear_address_of_dos_buffer_two;


/* End of Globals */


/*

void Allocate_Dos_Buffers( void )
    {
     _go32_dpmi_seginfo info;
     unsigned int address, page; 
     unsigned int new_segment, new_offset;

     info.size = (200 + 15) / 16;
     
     
     if ( _go32_dpmi_allocate_dos_memory( &info ) ) 
	 {
	  printf("ERROR Allocate dos buffer Alloc failed \n");
	  exit(0);
	 }

     address = info.rm_segment << 4; 
     
     page = address & 0xffff;
     if ((page + 100) > 0xffff)
	 address = (address - page) + 0x10000;
	 
     new_segment = address / 16;
     new_offset  = address % 16;

     segment_of_dos_buffer = new_segment;
     offset_of_dos_buffer  = new_offset;

     linear_address_of_dos_buffer = address;
     address_of_dos_buffer = ((unsigned long)new_segment << 16) | new_offset;
     

     memset( &info, 0, sizeof(info) );

     info.size = (200 + 15) / 16;
     
     
     if ( _go32_dpmi_allocate_dos_memory( &info ) ) 
	 {
	  printf("ERROR Allocate dos buffer Alloc failed \n");
	  exit(0);
	 }


     address = info.rm_segment << 4; 
     
     page = address & 0xffff;
     if ((page + 100) > 0xffff)
	 address = (address - page) + 0x10000;
	 
     new_segment = address / 16;
     new_offset  = address % 16;

     segment_of_dos_buffer_two = new_segment;
     offset_of_dos_buffer_two  = new_offset;

     linear_address_of_dos_buffer_two = address;
     address_of_dos_buffer_two = ((unsigned long)new_segment << 16) | new_offset;


    } 


void Copy_Into_Dos_Buffer( void *block, short length ) 
    {
    dosmemput( block, length, linear_address_of_dos_buffer );
    }  
    
void Copy_From_Dos_Buffer( void *block, short length )
    {
    dosmemget( linear_address_of_dos_buffer, length, block );  
    }

void Copy_Into_Dos_Buffer_Two( void *block, short length ) 
    {
    dosmemput( block, length, linear_address_of_dos_buffer_two );
    }  
    
void Copy_From_Dos_Buffer_Two( void *block, short length )
    {
    dosmemget( linear_address_of_dos_buffer_two, length, block );  
    }

void Test_Dos_Buffers( void )
    {
     int i;
     unsigned char buffer1[100], buffer2[100];

     for( i = 0; i < 100; i++ )
	 {
	  buffer1[i] = i;
	  buffer2[i] = 0;
	 }

     Copy_Into_Dos_Buffer( buffer1, 100 );
     Copy_From_Dos_Buffer( buffer2, 100 );

     for( i = 0; i < 100; i++ )
	 printf("%d \n", buffer2[i] );
     getch();

     for( i = 0; i < 100; i++ )
	 {
	  buffer1[i] = i;
	  buffer2[i] = 0;
	 }

     Copy_Into_Dos_Buffer_Two( buffer1, 100 );
     Copy_From_Dos_Buffer_Two( buffer2, 100 );

     for( i = 0; i < 100; i++ )
	 printf("%d \n", buffer2[i] );
     getch();


    }


*/

void red_book (unsigned long value, unsigned char *min, unsigned char *sec, unsigned char *frame)
{
	/*
  *frame = value & 0x000000ff;
  *sec = (value & 0x0000ff00) >> 8;
  *min = (value & 0x00ff0000) >> 16;
	*/
}


unsigned long hsg (unsigned long value)
{
  unsigned char min, sec, frame;

  red_book (value, &min, &sec, &frame);
  value = (unsigned long)min * 4500;
  value += (short)sec * 75;
  value += frame - 150;
  return value;
}




void Device_Request( void *block ) {
	/*
     unsigned char *length;
     _go32_dpmi_registers r;


     // The first byte of every block is the length of the block 
     length = (unsigned char *)block;


     memset( &r, 0, sizeof(r) ); // Clear out the registers 

     r.x.ax = 0x1510; // Subfunction 0x1510, send device driver request 
     r.x.cx = cdrom_data.drive_number; // Make sure we know this first! 
     r.x.es = segment_of_dos_buffer_two;
     r.x.bx = offset_of_dos_buffer_two;


     r.x.ss = _go32_info_block.linear_address_of_transfer_buffer & 0x0f;
     r.x.sp = (_go32_info_block.linear_address_of_transfer_buffer >> 4) & 0xffff;

     // Copy device request into dos buffer two 
     Copy_Into_Dos_Buffer_Two( block, length[0] );


     if( _go32_dpmi_simulate_int( 0x2f, &r ) == -1 )
	 {
          // printf("Error in device_request \n"); 
	 }


     Copy_From_Dos_Buffer_Two( block, length[0] );

     if( r.x.ax != 0x1510 );
         // printf("Error code is 0x%x \n", r.x.ax ); 

*/
} // End of Device_Request() 



int Cdrom_Installed( void ) {
	return(1); /* Johnm 12/1/2001 */

	/*
     _go32_dpmi_registers r;
     char drive_letter;
     FILE *fp;
     int return_value = 1;


     memset( &r, 0, sizeof(r) );

     r.x.ax = 0x1500; // Subfunction 0x1500..Cdrom installation check 
     r.x.bx = 0;

     if( _go32_dpmi_simulate_int( 0x2f, &r ) == -1 )
	 {
          // printf("Error in device_request \n"); 
	 }

     cdrom_data.num_drives   = r.x.bx;

     if( cdrom_data.num_drives < 1 )
	 {
          // printf("No Cdrom detected (MSCDEX not loaded \n"); 
          return_value = 0;
	 }

     cdrom_data.drive_number = r.x.cx;



     // Here we determine where animations are loaded from 
     if( (fp = fopen( "override.txt", "r")) != NULL )
         {
          fscanf(fp, "%c", &drive_letter );
          fclose(fp);
         }
     else
         {
          drive_letter = 'a' + cdrom_data.drive_number;
         }

     cdrom_drive_letter = drive_letter;

     
     //fp = fopen( "poo.dat", "w" );
     //fprintf(fp, "Number of drives is %d and first drive is %d \n", cdrom_data.num_drives,
     //cdrom_data.drive_number );
     //fprintf(fp, "Drive letter is %c \n", drive_letter );
     //fclose(fp);

     if( return_value != 0 )
         {
          // Cd_Status(); 
          
           // Cd_Command( RESET ); 

          Get_Audio_Info();
         }

     return(return_value);
*/
} // End of Cdrom_Installed 


/*

typedef struct
    {
     unsigned char  length       __attribute__((packed));
     unsigned char  subunit      __attribute__((packed));
     unsigned char  comcode      __attribute__((packed));
     unsigned short status       __attribute__((packed));
     unsigned char  unused[8]    __attribute__((packed));
     unsigned char  media        __attribute__((packed));
     unsigned long  address      __attribute__((packed));
     unsigned short num_bytes    __attribute__((packed));
    } IOCTL_header_type;
*/


/* Used for ejecting tray, closing tray...etc */
void Cd_Command( unsigned char comcode ) {
/*
	IOCTL_header_type header;

     header.length    = sizeof( header );
     header.subunit   = 0;                     // Why? 
     header.comcode   = 0x0C;                  // Com code for IOCTL output 
     header.status    = 0;                     // Should it be zero? 
     header.media     = 0;                     // Should it be zero? 
     header.num_bytes = sizeof(comcode);       // Num bytes we are writing 
     header.address   = address_of_dos_buffer; // Where to find command 


     Copy_Into_Dos_Buffer( &comcode, sizeof( comcode ) );
     Device_Request( &header );
  */   
} /* End of Cd_Command */


/* Used for ejecting tray, closing tray...etc */
void Cd_Lock( unsigned char lock_mode ) {
	/*
     IOCTL_header_type header;
     unsigned char_buffer[2];


     header.length    = sizeof( header );
     header.subunit   = 0;                     // Why? 
     header.comcode   = 0x0C;                  // Com code for IOCTL output 
     header.status    = 0;                     // Should it be zero? 
     header.media     = 0;                     // Should it be zero? 
     header.num_bytes = 2;                     // Num bytes we are writing 
     header.address   = address_of_dos_buffer; // Where to find command 

     char_buffer[0] = 0x1;
     char_buffer[1] = lock_mode;

     Copy_Into_Dos_Buffer( &char_buffer, 2 );
     Device_Request( &header );
*/     
} /* End of Cd_Lock */



/*
typedef struct
    {
     unsigned char  length       __attribute__((packed));
     unsigned char  subunit      __attribute__((packed));
     unsigned char  comcode      __attribute__((packed));
     unsigned short status       __attribute__((packed));
     unsigned char  unused[8]    __attribute__((packed));
     unsigned char  media        __attribute__((packed));
     unsigned long  address      __attribute__((packed));
     unsigned short num_bytes    __attribute__((packed));
     unsigned short sector __attribute__((packed));  // Take these out
     unsigned long  volid  __attribute__((packed));  
    } status_header_type;

     
typedef struct
    {
     unsigned char  mode     __attribute__((packed));
     unsigned short status   __attribute__((packed));
     unsigned short unused   __attribute__((packed));
    } status_data_type;
*/


void Cd_Status( void ) {
	/*
     status_header_type header;
     status_data_type status_data;
     unsigned short test_short;


     header.length    = sizeof( header );
     header.subunit   = 0; // Why? 
     header.comcode   = 0x03; // IOCTL Input 
     header.status    = 0; // Why? 
     header.media     = header.volid = header.sector = 0; // Why? 
     header.address   = address_of_dos_buffer; // Where we are going to store command 
     header.num_bytes = sizeof(status_data) ;  


     status_data.mode = 6; // 6 is request for status command 

     // Copy buffer and command into dos buffer
     Copy_Into_Dos_Buffer( &status_data, sizeof(status_data) );
     Device_Request( &header ); 
     // Contents of dos buffer should now have info we need 
     Copy_From_Dos_Buffer( &status_data, sizeof(status_data) );


     cdrom_data.status = status_data.status;

     test_short = status_data.status;

*/
} /* End of Cd_Status */

/*
typedef struct
     {
      unsigned char mode    __attribute__((packed));
      unsigned char lowest  __attribute__((packed));
      unsigned char highest __attribute__((packed));
      unsigned long address __attribute__((packed));
     } track_data_type;

*/

void Get_Audio_Info( void ) {
	/*
     IOCTL_header_type header;
     track_data_type track_data;


     header.length    = sizeof( header );
     header.subunit   = 0; // Why? 
     header.comcode   = 0x03; // IOCTL Input 
     header.status    = 0; // Why? 
     header.media     = 0; // Why? 
     header.address   = address_of_dos_buffer; // Where we are going to store command 
     header.num_bytes = sizeof( track_data ); 


     track_data.mode    = 0x0A;
     track_data.lowest  = 0;
     track_data.highest = 0;
     track_data.address = 0;


     Copy_Into_Dos_Buffer( &track_data, sizeof( track_data ) );
     Device_Request( &header );
     Copy_From_Dos_Buffer( &track_data, sizeof( track_data ) );


     cdrom_data.first_track  = track_data.lowest;
     cdrom_data.last_track   = track_data.highest;
     cdrom_data.lead_address = track_data.address;


     
     //printf("First track is %d and last track is %d, address is %ld \n",
     //cdrom_data.first_track, cdrom_data.last_track, cdrom_data.lead_address );
     //while( !Jon_Kbhit() );
     
*/
} /* End of Get_Audio_Info */


/*
typedef struct
    {
     unsigned char  mode            __attribute__((packed));
     unsigned char  track_num       __attribute__((packed));
     unsigned long  track_start     __attribute__((packed)); // In red book 
     unsigned char  track_control   __attribute__((packed)); // I ignore this 
    } track_info_type;
*/

unsigned long Track_Pos( int tracknum ) {
     return(1); //Johnm 12/1/2001
/*
	 IOCTL_header_type header;
     track_info_type track_info;
     unsigned char min, sec, frame;

     if( tracknum > cdrom_data.last_track )
	 {
          // printf("Invalid track \n"); 
	  return -1;
	 }

     header.length    = sizeof( header );
     header.subunit   = 0; // Why? 
     header.comcode   = 0x03; // IOCTL Input 
     header.status    = 0; // Why? 
     header.media     = 0; // Why? 
     header.address   = address_of_dos_buffer; // Where we are going to store command 
     header.num_bytes = sizeof( track_info ); 


     memset( &track_info, 0, sizeof( track_info ) );

     track_info.mode      = 0x0B; // Subfunction 0x0B, Audio track info 
     track_info.track_num = tracknum;

     Copy_Into_Dos_Buffer( &track_info, sizeof( track_info ) );
     Device_Request( &header );
     Copy_From_Dos_Buffer( &track_info, sizeof( track_info ) );

     red_book( track_info.track_start, &min, &sec, &frame );

     return( hsg( track_info.track_start ) );
*/
} /* End of Track_Pos */



/*
typedef struct
    {
     unsigned char  length       __attribute__((packed));
     unsigned char  subunit      __attribute__((packed));
     unsigned char  comcode      __attribute__((packed));
     unsigned short status       __attribute__((packed));
     unsigned char  unused[8]    __attribute__((packed));
     unsigned char  mode         __attribute__((packed)); // Addressing mode 
     unsigned long  start        __attribute__((packed));
     unsigned long  play_length  __attribute__((packed)); 
    } play_header_type;
*/

void Play_Song( int tracknum ) {
	/*
     play_header_type header;
     unsigned long temp_pos;



     if( tracknum > cdrom_data.last_track )
         return;

     delay( CD_DELAY );

     header.length    = sizeof( header );
     header.subunit   = 0;     // Why? 
     header.comcode   = 0x84;  // Play audio 
     header.status    = 0;     // Why? 

     header.mode      = 0; // Hsg adressing 

     // Cd_Command(RESET);  
     Stop_Audio();

     header.start     = Track_Pos( tracknum );

     if( (tracknum + 1) > cdrom_data.last_track )
	 {
	  temp_pos = cdrom_data.end_of_disk;
	 }
     else
	 {
	  temp_pos = Track_Pos( tracknum + 1 );
	 }

     header.play_length = (temp_pos - header.start);

     Device_Request( &header );

*/
} /* End of Play_Song() */

/*
typedef struct
    {
     unsigned char mode    __attribute__((packed));
     unsigned char input0  __attribute__((packed));
     unsigned char volume0 __attribute__((packed));
     unsigned char input1  __attribute__((packed));
     unsigned char volume1 __attribute__((packed));
     unsigned char input2  __attribute__((packed));
     unsigned char volume2 __attribute__((packed));
     unsigned char input3  __attribute__((packed));
     unsigned char volume3 __attribute__((packed));
    } volume_info_type;
*/

/* Set cdrom output volume...0-255 */
void Set_Cd_Volume( int volume ) {
	/*
     IOCTL_header_type header;
     volume_info_type volume_info;


     header.length    = sizeof( header );
     header.subunit   = 0;                     // Why? 
     header.comcode   = 0x0C;                  // Com code for IOCTL output 
     header.status    = 0;                     // Should it be zero? 
     header.media     = 0;                     // Should it be zero? 
     header.num_bytes = sizeof(volume_info);   // Num bytes we are writing 
     header.address   = address_of_dos_buffer; // Where to find command 


     volume_info.mode = 0x03; // Subfunction 0x03, control audio channel 

     volume_info.input0 = 0; // I don't know what goes in these 
     volume_info.input1 = 1; // 
     volume_info.input2 = 2; // 
     volume_info.input3 = 3; // 

     volume_info.volume0 = volume;
     volume_info.volume1 = volume;
     volume_info.volume2 = volume;
     volume_info.volume3 = volume;


     Copy_Into_Dos_Buffer( &volume_info, sizeof( volume_info ) );


     Device_Request( &header );
*/

} /* End of Set_Cd_Volume */

/*
typedef struct
    {
     unsigned char  length       __attribute__((packed));
     unsigned char  subunit      __attribute__((packed));
     unsigned char  comcode      __attribute__((packed));
     unsigned short status       __attribute__((packed));
     unsigned char  unused[8]    __attribute__((packed));
    } request_header_type; // This is a generic device request header 
*/

void Stop_Audio( void ) {
	/*
     request_header_type header;


     header.comcode   = 0x85; // Subfunction 0x85, stop audio 
     header.length    = sizeof( header );
     header.subunit   = 0;                     // Why? 
     header.status    = 0;                     // Should it be zero? 


     Device_Request( &header );
*/
} /* End of Stop_Audio() */

int Is_Audio_Playing( void ) {
	return(1); //Johnm 12/1/2001

	/*
     short test_short;
     int i;


     Cd_Status();

     test_short = cdrom_data.status;

     for( i = 0; i < 14; i++ )
	 {
	  test_short = test_short >> 1;
	 }

     if( test_short & 0x01 )
	 return(1);
     else
	 return(0);
*/     

} /* End of Is_Audio_Playing() */



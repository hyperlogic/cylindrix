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

#ifndef FLI_H
#define FLI_H

#include <stdlib.h>
#include "prim.h"
/* #include "pc.h" */
#include "keys.h"

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
     unsigned long  file_size        PACKED_STRUCT; /* Total size of the file       */
     unsigned short file_id          PACKED_STRUCT; /* File format indicator        */
     unsigned short number_of_frames PACKED_STRUCT; /* Total number of frames       */
     unsigned short width            PACKED_STRUCT; /* Screen width in pixels       */
     unsigned short height           PACKED_STRUCT; /* Screen height in pixels      */
     unsigned short pixel_depth      PACKED_STRUCT; /* Number of bits per pixel (8) */
     unsigned short flags            PACKED_STRUCT; /* Set to 0x03                  */
     unsigned long  frame_delay      PACKED_STRUCT; /* Time delay between frames    */
     unsigned short reserved1        PACKED_STRUCT; /* Not used ( set to 0x00 )     */
  
     /* The following fields are set to zero in a .fli file */
     
     unsigned long  date_created  PACKED_STRUCT; /* Time/date file was created      */
     unsigned long  creator_sn    PACKED_STRUCT; /* Serial number of the program    */
     unsigned long  last_updated  PACKED_STRUCT; /* Time/date file was last changed */
     unsigned long  updater_sn    PACKED_STRUCT; /* Serial number of updater prog   */
     unsigned short x_aspect      PACKED_STRUCT; /* X of display aspect ratio       */
     unsigned short y_aspect      PACKED_STRUCT; /* Y of display aspect ratio       */
     char           reserved2[38] PACKED_STRUCT; /* Not used (set to 0x00)          */
     unsigned long  frame1_offset PACKED_STRUCT; /* Offset of first frame           */
     unsigned long  frame2_offset PACKED_STRUCT; /* Offset of second frame          */
     char           reserved3[40] PACKED_STRUCT; /* Not used (set to 0x00)          */
    } flic_header;


typedef struct
    {
     unsigned long  chunk_size       PACKED_STRUCT; /* Total size of the chunk */
     unsigned short chunk_type       PACKED_STRUCT; /* Chunk identifier */
     unsigned short number_of_chunks PACKED_STRUCT; /* Number of subchunks in this chunk */
     char           reserved[8]      PACKED_STRUCT; /* Not used (set to 0x00 ) */
    } chunk_header;

typedef struct
    {
     unsigned long  chunk_size PACKED_STRUCT; /* Total size of the chunk */
     unsigned short chunk_type PACKED_STRUCT; /* Chunk identifier */
    } sub_chunk_header;


typedef struct
    {
     unsigned char *file_buffer;
     unsigned long file_length;
     unsigned long file_pos;
     unsigned long first_frame;
    } fli_file_type;

void PlayFliEnter( const char* filename );
int PlayFliProcess();
void PlayerFliExit();

/* Play a fli file straigt off of a disk */
void Play_Fli( const char *filename );

/* Load a fli file into ram and play it */
//void Play_Fli_Ram( char *filename );

/* Load a fli file into ram */
//void Load_Fli( char *filename, fli_file_type *fli_file  );

/* Play a fli file that has been loaded into ram */
//void Play_Loaded_Fli( fli_file_type *fli_file );

/* Free a fli file */
//void Free_Fli( fli_file_type *fli_file );

/* Play one frame from a loaded fli file...if we reach the end, start at the
   beginning */
//void One_Frame( fli_file_type *fli_file );

//void Play_Fli_Buffered( char *filename );

#endif


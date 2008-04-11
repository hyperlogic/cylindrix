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

#ifndef CDROM_H
#define CDROM_H


#define EJECT_TRAY 0
#define RESET 2
#define CLOSE_TRAY 5
#define DATA_TRACK 64
#define LOCK 1
#define UNLOCK 0


  /* The __attribute__ ((PACKED)) thing is to
     tell DJGPP not to use padding in the structure,
     since dos is not expecting it */

/* #define __LINUX__ */
/* Only use linux specific __attribute__ tag on Linux */
#ifndef PACKED_STRUCT 
	#ifdef __LINUX__
		#define PACKED_STRUCT __attribute__((packed))
	#else
		#define PACKED_STRUCT 
	#endif
#endif


typedef struct playinfo {
  unsigned char control PACKED_STRUCT;
  unsigned char adr     PACKED_STRUCT;
  unsigned char track   PACKED_STRUCT;
  unsigned char index   PACKED_STRUCT;
  unsigned char min     PACKED_STRUCT;
  unsigned char sec     PACKED_STRUCT;
  unsigned char frame   PACKED_STRUCT;
  unsigned char zero    PACKED_STRUCT;
  unsigned char amin    PACKED_STRUCT;
  unsigned char asec    PACKED_STRUCT;
  unsigned char aframe  PACKED_STRUCT;
} playinfo_type;

typedef struct volumeinfo {
    unsigned char mode    PACKED_STRUCT;
    unsigned char input0  PACKED_STRUCT;
    unsigned char volume0 PACKED_STRUCT;
    unsigned char input1  PACKED_STRUCT;
    unsigned char volume1 PACKED_STRUCT;
    unsigned char input2  PACKED_STRUCT;
    unsigned char volume2 PACKED_STRUCT;
    unsigned char input3  PACKED_STRUCT;
    unsigned char volume3 PACKED_STRUCT;
} volumeinfo_type;

typedef struct {
  unsigned short drives             PACKED_STRUCT;
  unsigned char  first_drive        PACKED_STRUCT;
  unsigned short current_track      PACKED_STRUCT;
  unsigned long  track_position     PACKED_STRUCT;
  unsigned char  track_type         PACKED_STRUCT;
  unsigned char  low_audio          PACKED_STRUCT;
  unsigned char  high_audio         PACKED_STRUCT;
  unsigned char  disk_length_min    PACKED_STRUCT;
  unsigned char  disk_length_sec    PACKED_STRUCT;
  unsigned char  disk_length_frames PACKED_STRUCT;
  unsigned long  endofdisk          PACKED_STRUCT;
  unsigned char  upc[7]             PACKED_STRUCT;
  unsigned char  diskid[6]          PACKED_STRUCT;
  unsigned long  status             PACKED_STRUCT;
  unsigned short error              PACKED_STRUCT;
} cdrom_data_type;



void Allocate_Dos_Buffer( void );
void Copy_Into_Dos_Buffer( void *block, short length );
void Copy_From_Dos_Buffer( void *block, short length );
void cd_get_volume (struct volumeinfo *vol);
void cd_set_volume (struct volumeinfo *vol);
void cd_get_audio_info (void);
void cd_set_track (short tracknum);
void cd_status (void);
void cd_seek (unsigned long location);
void cd_play_audio (unsigned long begin, unsigned long end);
void cd_stop_audio (void);
void cd_resume_audio (void);
void cd_cmd (unsigned char mode);
short cdrom_installed (void);
short cd_done_play (void);
void cd_lock (unsigned char doormode);
void Set_CD_Volume( unsigned char volume );

#endif


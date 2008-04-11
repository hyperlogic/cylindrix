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

#ifndef NEWCD_H





/*

2F 0802 Format of device driver request header:


Table 1382
Format of device driver request header:
Offset  Size    Description     
 00h    BYTE    length of request header
 01h    BYTE    subunit within device driver
 02h    BYTE    command code (see #1380)
 03h    WORD    status (filled in by device driver) (see #1381)
---DOS---
 05h  4 BYTEs   reserved (unused in DOS 2.x and 3.x)
 09h    DWORD   (European MS-DOS 4.0 only) pointer to next request header in
			  device's request queue
		(other versions) reserved (unused in DOS 2.x and 3.x)


---command code 00h---
 0Dh    BYTE    (ret) number of units

 0Eh    DWORD   (call) pointer to DOS device helper function (see #1384)
			  (European MS-DOS 4.0 only)
		(call) pointer past end of memory available to driver (DOS 5+)
		(ret) address of first free byte following driver
 12h    DWORD   (call) pointer to commandline arguments
		(ret) pointer to BPB array (block drivers) or
			  0000h:0000h (character drivers)
 16h    BYTE    (DOS 3.0+) drive number for first unit of block driver (0=A)
   ---European MS-DOS 4.0---
 17h    DWORD   pointer to function to save registers on stack
   ---DOS 5+ ---
 17h    WORD    (ret) error-message flag
		0001h MS-DOS should display error msg on init failure

---command code 01h---
 0Dh    BYTE    media descriptor
 0Eh    BYTE    (ret) media status
		00h don't know
		01h media has not changed
		FFh media has been changed
 0Fh    DWORD   (ret, DOS 3.0+) pointer to previous volume ID if the
		  OPEN/CLOSE/RM bit in device header is set and disk changed

---command code 02h---
 0Dh    BYTE    media descriptor
 0Eh    DWORD   transfer address
		-> scratch sector if NON-IBM FORMAT bit in device header set
		-> first FAT sector otherwise
 12h    DWORD   pointer to BPB (set by driver) (see #0875 at INT 21/AH=53h)

---command codes 03h,0Ch--- (see also INT 21/AX=4402h,INT 21/AX=4403h)
 0Dh    BYTE    media descriptor (block devices only)

 0Eh    DWORD   transfer address
 12h    WORD    (call) number of bytes to read/write
		(ret) actual number of bytes read or written

---command codes 04h,08h,09h (except Compaq DOS 3.31, DR-DOS 6)---
 0Dh    BYTE    media descriptor (block devices only)
 0Eh    DWORD   transfer address
 12h    WORD    byte count (character devices) or sector count (block devices)
 14h    WORD    starting sector number (block devices only)
 16h    DWORD   (DOS 3.0+) pointer to volume ID if error 0Fh returned
 1Ah    DWORD   (DOS 4.0+) 32-bit starting sector number (block devices with
		  device attribute word bit 1 set only) if starting sector
		  number above is FFFFh (see INT 21/AH=52h)

---command code 05h---
 0Dh    BYTE    byte read from device if BUSY bit clear on return

---command codes 06h,07h,0Ah,0Bh,0Dh,0Eh,0Fh---
 no further fields

---command code 10h---
 0Dh    BYTE    unused
 0Eh    DWORD   transfer address
 12h    WORD    (call) number of bytes to write
		(ret) actual number of bytes written

---command codes 11h,12h---
 0Dh    BYTE    reserved

---command code 15h---
 no further fields

---command codes 13h,19h---
 0Dh    BYTE    category code
		00h unknown
		01h COMn:
		03h CON
		05h LPTn:
		07h mouse (European MS-DOS 4.0)
		08h disk
		9Eh (STARLITE) Media Access Control driver
 0Eh    BYTE    function code
		00h (STARLITE) MAC Bind request
 0Fh    WORD    copy of DS at time of IOCTL call (apparently unused in DOS 3.3)
		SI contents (European MS-DOS 4.0)
 11h    WORD    offset of device driver header
		DI contents (European MS-DOS 4.0)

 13h    DWORD   pointer to parameter block from INT 21/AX=440Ch or AX=440Dh

---command codes 80h,82h---
 0Dh    BYTE    addressing mode
		00h HSG (default)
		01h Phillips/Sony Red Book
 0Eh    DWORD   transfer address (ignored for command 82h)
 12h    WORD    number of sectors to read
		(if 0 for command 82h, request is an advisory seek)
 14h    DWORD   starting sector number
		logical sector number in HSG mode
		frame/second/minute/unused in Red Book mode
		(HSG sector = minute * 4500 + second * 75 + frame - 150)
 18h    BYTE    data read mode
		00h cooked (2048 bytes per frame)
		01h raw (2352 bytes per frame, including EDC/ECC)
 19h    BYTE    interleave size (number of sectors stored consecutively)

 1Ah    BYTE    interleave skip factor
		(number of sectors between consecutive portions)

---command code 83h---
 0Dh    BYTE    addressing mode (see above)
 0Eh    DWORD   transfer address (ignored)
 12h    WORD    number of sectors to read (ignored)
 14h    DWORD   starting sector number (see also above)

---command code 84h---
 0Dh    BYTE    addressing mode (see above)
 0Eh    DWORD   starting sector number (see also above)
 12h    DWORD   number of sectors to play

---command codes 85h,88h---
 no further fields

---command codes 86h,87h---
 0Dh    BYTE    addressing mode (see above)
 0Eh    DWORD   transfer address (ignored in write mode 0)
 12h    WORD    number of sectors to write
 14h    DWORD   starting sector number (see also above)

 18h    BYTE    write mode
		00h mode 0 (write all zeros)
		01h mode 1 (default) (2048 bytes per sector)
		02h mode 2 form 1 (2048 bytes per sector)
		03h mode 2 form 2 (2336 bytes per sector)
 19h    BYTE    interleave size (number of sectors stored consecutively)
 1Ah    BYTE    interleave skip factor
		(number of sectors between consecutive portions)

INT 2F 0802
Copied from Ralf Brown's Interrupt List

*/


#define EJECT_DISK   0x00
#define CLOSE_DISK   0x05
#define RESET        0x02
#define LOCK         0x01
#define UNLOCK       0x00


typedef struct
    {
     unsigned char  num_drives;    /* How many cdrom drives are present? */
     unsigned char  drive_number;  /* First cdrom drive # 0=A 1=B etc */
     unsigned char  first_track;   /* First track on the cd */
     unsigned char  last_track;    /* Last track on the cd */
     unsigned long  lead_address;  /* Address of lead out track in red book format */
     unsigned long  end_of_disk;   /* End of disk in hsg */
     unsigned short status;        /* Bit encode status */
    } cdrom_data_type;



/*
void Allocate_Dos_Buffers( void );
void Copy_Into_Dos_Buffer( void *block, short length );
void Copy_From_Dos_Buffer( void *block, short length );
void Copy_Into_Dos_Buffer_Two( void *block, short length );
void Copy_From_Dos_Buffer_two( void *block, short length );
void Test_Dos_Buffers( void );
*/

int Cdrom_Installed( void );
void Cd_Command( unsigned char comcode );
void Cd_Lock( unsigned char lock_mode );
void Cd_Status( void );
void Get_Audio_Info( void );
unsigned long Track_Pos( int tracknum );
void Play_Song( int tracknum );
void Set_Cd_Volume( int volume );
void Stop_Audio( void );
int Is_Audio_Playing( void );

#endif

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

#ifndef DOSBUFF_H

#define DOSBUFF_H

#define DOS_BUFFER_SIZE 200 /* 300 bytes */
#define DOS_BUFFER_TEST 100 /* The amount we REALLY need for the dos buffer */


void Allocate_Dos_Buffers( void );
void Copy_Into_Dos_Buffer( void *block, short length );
void Copy_From_Dos_Buffer( void *block, short length );
void Copy_Into_Dos_Buffer_Two( void *block, short length ) ;
void Copy_From_Dos_Buffer_Two( void *block, short length );
void Copy_Into_Send_Buffer( void *block, short length );
void Copy_From_Send_Buffer( void *block, short length );
void Copy_Into_Recieve_Buffer( void *block, short length );
void Copy_From_Recieve_Buffer( void *block, short length );
void Copy_Into_Ecb_Send_Buffer( void *block, short length );
void Copy_From_Ecb_Send_Buffer( void *block, short length );
void Copy_Into_Ecb_Recieve_Buffer( void *block, short length );
void Copy_From_Ecb_Recieve_Buffer( void *block, short length );
void Test_Dos_Buffers( void );
unsigned long Make_Far_Pointer( unsigned short segment, unsigned short offset );

#endif


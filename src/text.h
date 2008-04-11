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

#ifndef TEXT_H
#define TEXT_H

void letter_blit( char ch, int x, int y, unsigned char *packed_array,
                  int length, int height, unsigned char color );
void string_blit( char *str, int x, int y, unsigned char *packed_array,
                  int length, int height, unsigned char color );
void micro_letter_blit( char ch, int x, int y, unsigned char *packed_array,
                        int length, int height, unsigned char color );
void micro_string_blit( char *str, int x, int y, unsigned char *packed_array,
                        int length, int height, unsigned char color );
#endif

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
#include <string.h>

#include "prim.h"

#define CHAR_LENGTH 11  /* length of one character in pixels */
#define CHAR_HEIGHT 12  /* height of one character in pixels */
#define TRANSPARENT 255 /* the color that is considered transparent */
#define SPACING 5       /* the number of pixels between each character */

#define MICRO_CHAR_LENGTH 7
#define MICRO_CHAR_HEIGHT 6
#define MICRO_SPACING 1

/*  letter blitter */

void letter_blit( char ch, int x, int y, unsigned char *packed_array,
                  int length, int height, unsigned char color )
{
    int screen_x, screen_y;
    int pa_start = (ch - '!') * CHAR_LENGTH;
    int pa_end = pa_start + CHAR_LENGTH;
    unsigned char temp_color;
    int i,j;

    if( ch >= 'a' && ch <= 'z' ) {
        ch = ch - 32;
    }

    if( ch >= '!' && ch <= '`' ) {
        screen_y = y;
        for( j = 0; j < CHAR_HEIGHT; j++ ) {
            screen_x = x;
            for( i = pa_start; i < pa_end; i++ ) {
                if( (temp_color = packed_array[ (j * length) + i]) != TRANSPARENT ) {
                    if( temp_color == 2 ) {
                        DB_Fast_Pixel( screen_x, screen_y, color  );
                    }
                    else {
                        DB_Fast_Pixel( screen_x, screen_y, temp_color );
                    }
                }
                screen_x++;
            }
            screen_y++;
        }
    }
    else {
        ;
    }
}

void micro_letter_blit( char ch, int x, int y, unsigned char *packed_array,
                        int length, int height, unsigned char color )
{
    int screen_x, screen_y;
    int pa_start = (ch - '!') * MICRO_CHAR_LENGTH;
    int pa_end = pa_start + MICRO_CHAR_LENGTH;
    int i,j;

    if( ch >= 'a' && ch <= 'z' ) {
        ch = ch - 32;
    }

    if( ch >= '!' && ch <= '`' ) {
        screen_y = y;
        for( j = 0; j < MICRO_CHAR_HEIGHT; j++ ) {
            screen_x = x;
            for( i = pa_start; i < pa_end; i++ ) {
                if( packed_array[ (j * length) + i] != TRANSPARENT )
                    DB_Fast_Pixel( screen_x, screen_y, color  ); /* packed_array[ (j * length) + i]  */
                screen_x++;
            }
            screen_y++;
        }
    }
    else {
        ;
    }
}


/*  string blitter */

void string_blit( const char *str, int x, int y, unsigned char *packed_array,
                  int length, int height, unsigned char color )
{
    int str_len = strlen( str );
    int screen_x = x;
    int i;

    for( i = 0; i < str_len; i++ ) {
        letter_blit( str[i], screen_x, y, packed_array, length, height, color );
        if( str[i] == ' ' ) {
            screen_x += SPACING;
        }
        else {
            screen_x += CHAR_LENGTH;
        }
    }
}

/* micro string blitter */

void micro_string_blit( const char *str, int x, int y, unsigned char *packed_array,
                        int length, int height, unsigned char color )
{
    int str_len = strlen( str );
    int screen_x = x;
    int i;

    for( i = 0; i < str_len; i++ ) {
        micro_letter_blit( str[i], screen_x, y, packed_array, length, height, color );
        if( str[i] == ' ' ) {
            screen_x += MICRO_SPACING;
        }
        screen_x += MICRO_CHAR_LENGTH;
    }
}


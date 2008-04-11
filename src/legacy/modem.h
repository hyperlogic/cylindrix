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

#ifndef MODEM_H

#define MODEM_H

#include "serial.h"
#include "keys.h"

#define MODEM_RESET                "atz\0"
#define MODEM_SPEAKER              "atm1\0"
#define MODEM_DIAL                 "atdt\0"
#define MODEM_HANGUP               "ath0\0"
#define MODEM_ANSWER               "ats0=1\0"
#define MODEM_ESCAPE               "+++\0"
/*
#define MODEM_INIT_STRING          "ATB0N0\\N0 %C0 &K0 S37=9\0"
*/
#define MODEM_INIT_STRING          "ATZS27=32S15=16S13=64&B0&H2&I1&N6&K0&M0\0"

void Init_Modem( short port_base, short baud, short configuration );
void Close_Modem( short port_base );
void Write_String_To_Modem( char *write_string );
void Write_String_To_Modem_With_CR( char *write_string );
int Wait_Till_Connected( void );
int Dial_Modem_Till_Connected( char *phone_number );
int Answer_Modem_Till_Connected( void );
void dtr(int i, short base );
void rts(int i, short base );

#endif



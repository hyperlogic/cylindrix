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

#ifndef SERIAL_H

#define SERIAL_H

/* I N C L U D E S /////////////////////////////////////////////////////////// */

//#include <dos.h>

/* #include <bios.h> */

#include <stdio.h>
#include <math.h>

/*
#include <dpmi.h>
#include <pc.h>
*/

#include "types.h"
/* D E F I N E S  //////////////////////////////////////////////////////////// */

/* registers in UART */

#define SER_RBF        0    /* the read buffer */
#define SER_THR        0    /* the write buffer */
#define SER_IER        1    /* the int. enable register */
#define SER_IIR        2    /* the int. identification register */
#define SER_LCR        3    /* control data config. and divisor latch */
#define SER_MCR        4    /* modem control reg. */
#define SER_LSR        5    /* line status reg. */
#define SER_MSR        6    /* modem status of cts, ring etc. */
#define SER_DLL        0    /* the low byte of baud rate divisor */
#define SER_DLH        1    /* the hi byte of divisor latch */

/* bit patterns for control registers */

#define SER_BAUD_1200  96   /* baud rate divisors for 1200 baud - 19200 */
#define SER_BAUD_2400  48
#define SER_BAUD_9600  12
#define SER_BAUD_19200  6

#define SER_GP02        8     /* enable interrupt */


#define COM_1           0x3F8 /* base port address of port 0 */
#define COM_2           0x2F8 /* base port address of port 1 */
#define COM_3           0x3E8 /* base port address of port 2 */
#define COM_4           0x2E8 /* base port address of port 3 */


#define SER_STOP_1      0     /* 1 stop bit per character */
#define SER_STOP_2      4     /* 2 stop bits per character */

#define SER_BITS_5      0     /* send 5 bit characters */
#define SER_BITS_6      1     /* send 6 bit characters */
#define SER_BITS_7      2     /* send 7 bit characters */
#define SER_BITS_8      3     /* send 8 bit characters */

#define SER_PARITY_NONE 0     /* no parity */
#define SER_PARITY_ODD  8     /* odd parity */
#define SER_PARITY_EVEN 24    /* even parity */

#define SER_DIV_LATCH_ON 128  /* used to turn reg 0,1 into divisor latch */

#define PIC_IMR    0x21   /* pic's interrupt mask reg. */
#define PIC_ICR    0x20   /* pic's interupt control reg. */


#define INT_SER_PORT_0    0x0C  /* port 0 interrupt com 1 & 3 */
#define INT_SER_PORT_1    0x0B  /* port 0 interrupt com 2 & 4 */

#define SERIAL_BUFF_SIZE 300    /* current size of circulating receive buffer */




/* NEW */

#ifdef __GNUC__
#define far
#define peek(a,b) (*(unsigned short *)(0xe0000000 + (a)*16 + (b)))
#define disable() asm("cli")
#define enable() asm("sti")
#endif




/* END OF NEW */

void Flush_Buffer( void );
short Ready_Serial();
short Serial_Read( char *send_char );
void Serial_Write(char ch);
void Open_Serial(short port_base, short baud, short configuration);
void Close_Serial(short port_base);
int Handshake( void );
int Get_One_Character( char *ch );


#endif

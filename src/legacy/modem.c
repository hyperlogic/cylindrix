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
#include <ctype.h>
#include <string.h>
//#include <conio.h>
#include "modem.h"


void Init_Modem( short port_base, short baud, short configuration )
    {

     Open_Serial( port_base, baud, configuration );
     
     dtr(1, port_base ); /* New */
     rts(1, port_base ); /* New */

     Write_String_To_Modem_With_CR( MODEM_INIT_STRING ); 

    } /* End of Init_Modem */


void Close_Modem( short port_base )
    {
     /* Write_String_To_Modem( MODEM_ESCAPE ); */

     Write_String_To_Modem_With_CR( MODEM_HANGUP );
     Close_Serial( port_base );
    }


void Write_String_To_Modem( char *write_string )
    {
     int i = 0;

     while( write_string[i] != '\0' )
         {
          delay(100);
          Serial_Write( write_string[i++] );
          delay(100);
         }

    } /* end of Write_String_To_Modem */

void Write_String_To_Modem_With_CR( char *write_string )
    {
     int i = 0;

     while( write_string[i] != '\0' )
         {
          delay(100);
          fprintf(stderr, "%c", write_string[i] );
          Serial_Write( write_string[i++] );
          delay(100);
         }
     
     fprintf(stderr, "\n" );

     Serial_Write( '\015' ); /* send enter to modem */
     Serial_Write( '\012' ); /* Send linefeed to modem */
    } /* end of Write_String_To_Modem */



int Wait_Till_Connected( void )
    {
     char buffer[20];
     int i = 0;
     int done = 0;
     int index;
     char ch;
     char *test_string = "CONNECT";
                   


     delay( 500 );

     for( index = 0; index < 50; index++ )
         {
          if( Serial_Read( &ch ) != -1 )
              fprintf(stderr, "%c", ch );
         }


     buffer[0] = '\0';
     buffer[1] = '\0';

     while( (i < 20) && !done )
         {
          if( Jon_Kbhit() )
              if( Jon_Getkey() == 27 )
                  done = 1;

          if( Ready_Serial() )
              {
               Serial_Read( &buffer[i++] );
               buffer[i] = '\0';

               fprintf(stderr, "%c", buffer[i - 1] );

               if( test_string[i - 1] != (char)toupper( buffer[i - 1] ) )
                   {
                    buffer[i - 1] = '\0'; 
                    i--;
                   }

              } /* End if ready_serial */

          if( !strcmp( buffer, test_string ) )
              {
               delay( 500 );
               for( index = 0; index < 50; index++ ) /* Flush the buffer */
                   Serial_Read( &ch );
               return( 1 );
              }


         } /* End while */

     delay( 500 );
     for( index = 0; index < 50; index++ ) /* Flush the buffer */
         Serial_Read( &ch );
     return( 0 );
        
    } /* End of Wait_Till_Connected */


int Dial_Modem_Till_Connected( char *phone_number )
    {
     int i = 0;

     Write_String_To_Modem( MODEM_DIAL );

     fprintf(stderr, "dialing \n");
     while( phone_number[i] != '\0' )
         {
          fprintf(stderr, "%c", phone_number[i] );
          Serial_Write( phone_number[i] );
          i++;
         }

     Serial_Write( '\015' ); /* send enter to modem */

     return( Wait_Till_Connected() );

    } /* End of Dial_Modem */


int Answer_Modem_Till_Connected( void )
    {
     delay(500);
     Write_String_To_Modem_With_CR( MODEM_ANSWER );


     return( Wait_Till_Connected() ); 

    }



/* Beginning of new stuff */

/* Set the DTR pin to the status given */
void dtr(int i, short base ) {
	return; //Johnm 12/4/2001
  /*
	short i1;

  i1 = inportb(base + 4) & 0x00FE;
  outportb(base + 4, (i || 1) ? (i1 + 1) : i1); // First one could be zero 
*/
}

/* Set the RTS pin to the status given */
void rts(int i, short base ) {
/*
	short i1;

  i1 = inportb(base + 4) & 0x00FD;
  outportb(base + 4, (i) ? (i1 + 2) : i1);
*/
}



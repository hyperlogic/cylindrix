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
#include "level.h"

extern char g_DataPath[255];

int Load_Level( char *filename, level_type *level )
    {
     FILE *fp;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,filename);


     if( (fp = fopen( newfilename, "rb" )) == NULL )
         return 0;

     fread( level, sizeof(level_type), 1, fp );

     fclose( fp );
    
     return(1); /* Function was successful */
    }

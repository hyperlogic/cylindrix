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

#include "types.h"
//#include "textmenu.h"
#include "user.h"
#include "keys.h"
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "sb_stub.h"

/*
#include <pc.h>
#include <keys.h>
*/

//#include <conio.h>
#include "prim.h"
#include "menu.h"


extern game_configuration_type game_configuration;

extern level_type level; /* This holds the values we load from the level file */

extern WorldStuff world_stuff;

extern MenuStuff menu_stuff;

/* lets the user set up the game using silly little text menus */

void user_setup()
{
    /* display the new background image */

    
    delay(10);
    menu_event_loop();

    Save_Game_Configuration( &game_configuration );

}

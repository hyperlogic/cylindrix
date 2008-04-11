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

//Johnm 12/2/2001 - these are just stubs to make Cylindrix compile without
//Sound library

#include "sb_lib.h"
/*
void sb_free_mod_file(sb_mod_file *pmod);
void sb_mod_play(sb_mod_file *pmod);
void sb_mod_pause(void); 
void sb_free_sample(sb_sample *sample);
sb_status sb_queue_sample(sb_sample *sample);
sb_sample *sb_load_sample(char *pFoo, int nFoo);
sb_mod_file *sb_load_mod_file(char *pFoo);
void sb_mix_sample(sb_sample *psample);
void sb_uninstall_driver(void); 
sb_status sb_install_driver(int x); 
*/

void New_Play_Menu_Sound( SBBYTE *data ); 
sb_sample *jon_sb_load_sample(char *fname, int format); 
void jon_sb_free_sample(sb_sample *ss); 
void Hack_Update_Queue( void );
int getkey(); 
int _conio_kbhit(); 
void gotoxy( int x, int y ); 
void textcolor( int nColor ); 
void textbackground( int nColor); 
void New_Brighten_Palette( pcx_picture_ptr image, int frames ); 
void New_Enable_Palette( palette_type palette ); 
void New_Fade_Palette( int frames ); 
void delay( int nFoo ); 
void update_video_mode( void );

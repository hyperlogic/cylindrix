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

#ifndef __SB_MOD
#define __SB_MOD
#ifdef __cplusplus
extern "C" {
#endif

sb_mod_file *sb_load_mod_file(char *);
void sb_free_mod_file(sb_mod_file *);
void sb_mod_initialize_player(void);
void sb_mod_play(sb_mod_file *);
void sb_mod_pause(void);
void sb_mod_resume(void);
void sb_mod_parse_file(void);
void sb_mod_parse_file_mono(void);

void sb_mod_module_lock_your_memory(void);

void sb_mod_show_info(void);

extern char sb_mod_error[64];
extern int sb_mod_active;
extern int sb_mod_volume;

#ifdef __cplusplus
}
#endif
#endif

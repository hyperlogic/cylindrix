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

#ifndef __SB_DIGI
#define __SB_DIGI
#ifdef __cplusplus
extern "C" {
#endif

/* If a function in this module returns an error, look here for a description
   of what went wrong.                                                        */
extern char sb_digi_error[64];

/* The mixing buffers.                                                         */
extern int *sb_mixing_buffer;
extern int *sb_left_buffer;
extern int *sb_right_buffer;

/* A multiplication table to speed up performance-critical inner loops.       */
extern int sb_multTable[64][256];

/* Pass: Pointer to file-name string; format of file (see sb_defs.h)          */
/* Returns: Pointer to allocated sb_sample structure                          */
sb_sample *sb_load_sample(char *, int);

/* Pass: Pointer to sb_sample structure                                       */
/* Returns:                                                                   */
void sb_free_sample(sb_sample *);

/* Pass:                                                                      */
/* Returns: SB_SUCCESS if successful, SB_FAILURE otherwise (see sb_defs.h)    */
sb_status sb_digi_initialize(void);

/* Pass: Pointer to sb_sample structure created by sb_load_sample()           */
/* Returns: status of attempt as an sb_status enum.                           */
sb_status sb_queue_sample(sb_sample *);

/* Pass: Pointer to sb_sample structure created by sb_load_sample()           */
/* Returns:                                                                   */
void sb_mix_sample(sb_sample *);

sb_sample *sb_convert_frequency(sb_sample *, int, int);
void sb_mix_stereo_samples(void);
void sb_mix_mono_samples(void);
void sb_digi_module_lock_your_memory(void);


sb_sample *jon_sb_load_sample(char *fname, int format);
void jon_sb_free_sample(sb_sample *ss);


#ifdef __cplusplus
}
#endif
#endif

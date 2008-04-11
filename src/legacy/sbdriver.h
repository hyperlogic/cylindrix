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

#ifndef __SB_DRIVER
#define __SB_DRIVER
#ifdef __cplusplus
extern "C" {
#endif

/* If one of the functions in this module returns an error, look here for a
   description of what happened.                                              */
extern char sb_driver_error[80];
extern int sb_sample_frequency;

/* Pass: Frequency (Hz) all your samples will be running at.                  */
/* Returns: sb_status enum indicating what happened (see sb_defs.h)           */
sb_status sb_install_driver(int);

/* Pass:                                                                      */
/* Returns:                                                                   */
void sb_uninstall_driver(void);

#ifdef __cplusplus
}
#endif
#endif

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


#include <allegro.h>
#include <alleggl.h>
#include <memory.h>
#include <direct.h>

#ifdef GLCYLINDRIX
#include <winalleg.h>
#endif

#include "pcx.h"
#include "jonsb.h"

#include "types.h"
#include "config.h"

#ifdef GLCYLINDRIX
#include "glcylindrix.h"
#endif

/* if game is to be installed globally, change this to the games data direcoty */
#define CONFIG_PATH "./"

// AJT: i had some problems linking without this.
int errno = 0;

extern game_configuration_type game_configuration;

//Johnm 9/5/2002 - Beginning to move to a configurable resolution
#ifndef GLCYLINDRIX
const int g_XResolution = 640;
const int g_YResolution = 480;
#else
const int g_XResolution = 1024;
const int g_YResolution = 768;
#endif

extern long program_over;    /* TRUE when user wishes to leave the program */

/* Base path of data files */
char g_DataPath[255];

void MainLoop( int argc, char *argv[] );

void update_video_mode( void ) 
{
#ifdef GLCYLINDRIX

	allegro_gl_set(AGL_Z_DEPTH, 8);
	allegro_gl_set(AGL_COLOR_DEPTH, 32);
	allegro_gl_set(AGL_SUGGEST, AGL_Z_DEPTH | AGL_COLOR_DEPTH);	
	
	if( game_configuration.fullscreen ) {
		set_gfx_mode(GFX_OPENGL_FULLSCREEN,g_XResolution,g_YResolution,0,0);
	}
	else {
		set_gfx_mode(GFX_OPENGL_WINDOWED,g_XResolution,g_YResolution,0,0);
	}

#else

	set_color_depth(8);

	if( game_configuration.fullscreen ) {
		set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,g_XResolution,g_YResolution,0,0);
	}
	else {
		set_gfx_mode(GFX_AUTODETECT_WINDOWED,g_XResolution,g_YResolution,0,0);
	}
#endif

}

int main(  int argc, char *argv[]  ) {
	
	/* Modify this line to wherever the appropriate hard-coded data path should be */
	sprintf(g_DataPath, "./");

	// AJT: I had some problems linking with this. 
	//chdir(CONFIG_PATH);


	allegro_init();
#ifdef GLCYLINDRIX
	install_allegro_gl();
#endif
	
	Load_Game_Configuration( &game_configuration );

	update_video_mode();
	
	install_keyboard();
	install_timer();

#ifdef GLCYLINDRIX
	// AJT: creates the GL renderwindow & a gl context
	GL_Cylindrix_Init();
#endif

	MainLoop(  argc, argv );

	remove_timer();
	remove_keyboard();
	allegro_exit();

	return(0);
} //WinMain

END_OF_MAIN()

//Johnm 12/2/2001 - these are just stubs to make Cylindrix compile without
//Sound library

void sb_free_mod_file(sb_mod_file *pmod) {
	
}

void sb_mod_play(sb_mod_file *pmod) {

}

void sb_mod_pause(void) {

}


/* Pass: Pointer to sb_sample structure                                       */
/* Returns:                                                                   */
void sb_free_sample(sb_sample *sample) {
	jon_sb_free_sample(sample);
}


extern boolean voice_done;
SAMPLE *g_SampleQueue[MAX_VOICE_SAMPLES];
int g_nCurrentQueue = 0;
int g_nCurrentVoice = -1;

//Hack function that's called from the buffer swapping to 
//Update Queue'd samples...
void Hack_Update_Queue( void ) {
	if( g_nCurrentVoice == -1 )
		voice_done = 1;
	else if( !voice_check(g_nCurrentVoice) ) {
		if( g_nCurrentQueue > 0 ) {
			g_nCurrentQueue--;
			g_nCurrentVoice = allocate_voice(g_SampleQueue[g_nCurrentQueue]);
			if( g_nCurrentVoice != -1 ) {
				voice_start(g_nCurrentVoice);
				voice_set_priority(g_nCurrentVoice, 255);
				release_voice(g_nCurrentVoice);
			}
		}
		else {
			voice_done = 1;
			g_nCurrentVoice = -1;
		}
	}
	
}

/* Pass: Pointer to sb_sample structure created by sb_load_sample()           */
/* Returns: status of attempt as an sb_status enum.                           */
sb_status sb_queue_sample(sb_sample *sample) {
	int bAdd = 0;

	if( g_nCurrentVoice == -1 ) {
		bAdd = 1;
	}
	else if( !voice_check(g_nCurrentVoice ) ) {
		bAdd = 1;			
	}

	if( bAdd ) {
		g_nCurrentVoice = allocate_voice((SAMPLE *)sample->data);
		voice_start(g_nCurrentVoice);
		voice_set_priority(g_nCurrentVoice, 255);
		release_voice(g_nCurrentVoice);
	}
	else {
		g_SampleQueue[g_nCurrentQueue++] = (SAMPLE *)sample->data;
	}

	return(SB_SUCCESS);
}


/* Pass: Pointer to file-name string; format of file (see sb_defs.h)          */
/* Returns: Pointer to allocated sb_sample structure                          */
sb_sample *sb_load_sample(char *pFoo, int nFoo) {
	sb_sample *pSample;
	char newfilename[512];

	sprintf(newfilename,"%s%s",g_DataPath,pFoo);


	pSample = 0;

	pSample = malloc(sizeof(sb_sample));
	memset(pSample, 0, sizeof(sb_sample));

	pSample->data = (char *)load_sample(newfilename); //(SAMPLE *)load_sample(pFoo);

	return(pSample);
}

sb_mod_file *sb_load_mod_file(char *pFoo) {
	return(0);
}


/* Pass: Pointer to sb_sample structure created by sb_load_sample()           */
/* Returns:                                                                   */
void sb_mix_sample(sb_sample *psample) {
	//Need to change this to support panning
	int nVolume = (psample->left_volume + psample->right_volume) / 2;
	

	play_sample((SAMPLE *)(psample->data), nVolume, 120, 1000, 0);

}

void New_Play_Menu_Sound( BYTE *data ) {
	play_sample((SAMPLE *)data, 255, 120, 1000, 0);
}

sb_sample *jon_sb_load_sample(char *fname, int format) {
	return(sb_load_sample(fname, format));
}

void jon_sb_free_sample(sb_sample *ss) {
	if( ss ) {
		if( ss->data )
			destroy_sample((SAMPLE *)ss->data);
		free(ss);
	}
}

char sb_driver_error[80];


/* Pass:                                                                      */
/* Returns:                                                                   */
void sb_uninstall_driver(void) {
	remove_sound();
}

/* Pass: Frequency (Hz) all your samples will be running at.                  */
/* Returns: sb_status enum indicating what happened (see sb_defs.h)           */
sb_status sb_install_driver(int x) {

   if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
	  return(1);
   }

	return(SB_SUCCESS);
}

int getkey() {
	return(0);
}
int _conio_kbhit() {
	return(0);
}

void gotoxy( int x, int y ) {

}

void textcolor( int nColor ) {

}

void textbackground( int nColor) {

}



void New_Brighten_Palette( pcx_picture_ptr image, int frames ) {
	PALETTE obPalette;
	int i;

	for( i = 0; i < 256; i++ ) {
		obPalette[i].r = image->palette[i].red;
		obPalette[i].g = image->palette[i].green;
		obPalette[i].b = image->palette[i].blue;
	}

	fade_in(obPalette, frames);

#ifdef GLCYLINDRIX
	GL_SetFrameBufferPalette( obPalette );
	GL_FadeInFrameBuffer( frames );
#endif

}

void New_Enable_Palette( palette_type palette  ) {
	PALETTE obPalette;
	int i;

	for( i = 0; i < 256; i++ ) {
		obPalette[i].r = palette[i].red;
		obPalette[i].g = palette[i].green;
		obPalette[i].b = palette[i].blue;
	}

	set_palette(obPalette);

#ifdef GLCYLINDRIX	
	GL_SetFrameBufferPalette( obPalette );
#endif


}

void New_Fade_Palette( int frames ) {
	fade_out( frames );
#ifdef GLCYLINDRIX
	GL_FadeOutFrameBuffer( frames );
#endif
}


//THIS IS GOING TO AFFECT EVERYTHING!!!
void delay( int nFoo ) {
	rest(nFoo);
}

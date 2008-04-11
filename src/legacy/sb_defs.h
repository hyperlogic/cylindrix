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

#ifndef __SB_DEF
#define __SB_DEF
#ifdef __cplusplus
extern "C" {
#endif

#ifndef SBBYTE
#define SBBYTE                    unsigned char
#endif
#ifndef SBWORD
#define SBWORD                    unsigned short
#endif

#ifndef SBDWORD
#define SBDWORD                   unsigned long
#endif

/* Formats to pass to the sb_load_sample() function.                          */
#define _SB_RAW                 0
#define _SB_WAV                 1
#define _SB_VOC                 2

/* Maximum samples that can be mixed or queued at a time.                     */
#define _SB_MAX_MIX             16
#define _SB_MAX_QUEUE           16

/* Size of the DMA transfer buffer.  Smaller sizes give better resolution in  */
/* MIDI files, at the cost of speed.                                          */
#define _SB_BUFFER_SIZE         96
#define _SB_BLOCK_SIZE          (_SB_BUFFER_SIZE/2)

/* struct to store sample info.                                               */
typedef struct {
  SBBYTE *data;
  int length;
  int stereo;
  int bits;
  int left_volume;
  int right_volume;
  void (*callback)();
} sb_sample;

/* struct to keep track of FM voice allocation.                               */
typedef struct {
  int ownerTrack;
  int patchNum;
  unsigned char ownerChannel;
  char isActive;
  unsigned char note;
  unsigned char volume;
} sb_fmVoice;

/* struct to store MIDI file info.                                            */
typedef struct {
  SBWORD deltaTicksPerQuarter;
  SBWORD usecPerDeltaTick;
  SBDWORD usecPerQuarter;

  int numTracks;
  SBBYTE **track;
  int *trackPointer;
  SBDWORD *deltaTicksTillNextEvent;
  SBBYTE *lastEvent;
  int *trackIsActive;
  sb_fmVoice voiceInfo[9];
} sb_midi_file;

/* enum for return-values of many of the sb_ functions.                       */
typedef enum {
  SB_SUCCESS,
  SB_FAILURE,
  SB_BAD_BLASTER,
  SB_BAD_ADDRESS,
  SB_BAD_IRQ,
  SB_BAD_DMA,
  SB_BAD_FILE,
  SB_BUSY,
  SB_BAD_POINTER
} sb_status;

typedef struct {
  char *sample;
  char name[22];
  SBWORD length;
  char fineTune;
  SBBYTE loudness;
  SBWORD repeatStart;
  SBWORD repeatLength;
} sb_mod_instrument;

typedef struct {
  char title[20];
  sb_mod_instrument *instrument;
  SBBYTE songLength;
  SBBYTE CIAA;
  SBBYTE arrangement[128];
  SBBYTE id[4];
  SBBYTE *patternBuffer;
  int modSamples;
  int modChannels;
} sb_mod_file;

#ifdef __cplusplus
}
#endif
#endif

#include "assert.h"
#include "../system.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <SDL_Audio.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <mpg123.h>

struct SYS_Wave
{
	struct SYS_Wave* next;
	struct SYS_Wave* prev;
	ALuint buffer;
	ALuint source;  // used for single instance sounds.
	const char* filename;		// for debugging.
};

#define SIZEOFWAVEPOOL 500
struct SYS_Wave wavePool[SIZEOFWAVEPOOL];
struct SYS_Wave* freeList = 0;
struct SYS_Wave* usedList = 0;

// used for dynamic sounds (fire & forget, play once)
#define SIZEOFSOURCEPOOL 50
ALuint sourcePool[SIZEOFSOURCEPOOL];

// for testing
void DumpSDLAudioSpec(const SDL_AudioSpec* spec)
{
	printf("freq = %d\n", spec->freq);
	printf("format = %d\n", spec->format);
	printf("silence = %d\n", spec->silence);
	printf("channels = %d\n", spec->channels);
}

void CheckForErrors()
{
	ALenum error = alGetError();
	if ( error != AL_NO_ERROR )
	{
		const char* msg;
		switch (error)
		{
		case AL_INVALID_NAME: msg = "Invalid Name"; break;
		case AL_INVALID_ENUM: msg = "Invalid Enum"; break;
		case AL_INVALID_VALUE: msg = "Invalid Value"; break;
		case AL_INVALID_OPERATION: msg = "Invalid Operation"; break;
		case AL_OUT_OF_MEMORY: msg = "Out of Memory"; break;
		default: msg = "???"; break;
		}
		printf("OpenAL ERROR! %d, %s\n", error, msg );
	}
}

void SYS_InitSound()
{
	/*
	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const char* devicesStr = alcGetString(0, ALC_DEVICE_SPECIFIER);
		const char* defaultDeviceStr = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
		printf("OpenAL default device = %s\n", defaultDeviceStr);
		
		printf("All devices:\n");
		const char* p = devicesStr;
		while( *p != 0 )
		{
			printf("%s\n",p);
			p += (strlen(p) + 1);
		}
	}
	*/
			
	// open device
	ALCdevice* device = alcOpenDevice(0);
	if ( !device )
	{
		SYS_Error("Error opening OpenAL device\n");
		return;
	}
	
	/*
	const char* extStr = alcGetString(device, ALC_EXTENSIONS);
	printf("OpenAL extentions : %s\n", extStr);
	*/

	alGetError(); // clear error code

	// create context
	ALCcontext* context = alcCreateContext(device,0);
	alcMakeContextCurrent(context);
	CheckForErrors();
	
	// use linear distance model & disable doppler.
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alDopplerFactor(0.0f);

	// init wave pools
	memset(wavePool, 0, sizeof(struct SYS_Wave) * SIZEOFWAVEPOOL);
	freeList = wavePool;
	usedList = 0;
	for ( int i = 0; i < SIZEOFWAVEPOOL; ++i )
	{
		freeList[i].prev = (i == 0) ? 0 : &wavePool[i-1];
		freeList[i].next = (i == SIZEOFWAVEPOOL-1) ? 0 : &wavePool[i+1];
	}

	// init source pool
	memset(sourcePool, 0xff, sizeof(ALuint) * SIZEOFSOURCEPOOL);
	CheckForErrors();
	
	int error = mpg123_init();
	if (error != MPG123_OK)
	{
		printf("Error mpg123_init() : %s\n", mpg123_plain_strerror(error));
	}
	
	SYS_SOUNDHANDLE music = SYS_LoadSound( "music/track02.mp3");
	SYS_PlaySound(music, true);
}

void SYS_ShutdownSound()
{
	mpg123_exit();
	
	ALCcontext* context = alcGetCurrentContext();
	ALCdevice* device = alcGetContextsDevice(context);
	alcMakeContextCurrent(0);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

static SYS_Wave* GetWaveFromFreePool()
{
	SYS_Wave* wave;
	
	if ( freeList == 0 )
		SYS_Error( "AllocWaveFromPool() out of waves!\n" );

	// grab a wave off of the free list
	wave = freeList;
	freeList = wave->next;
	if ( freeList )
		freeList->prev = 0;	

	// and stick it in the used list
	wave->next = usedList;
	if ( usedList )
		usedList->prev = wave;
	usedList = wave;

	// init the resources.
	wave->buffer = 0;
	wave->source = -1;
	wave->filename = 0;

#ifdef SYS_SOUNDDEBUG
	soundsLoaded++;
	soundsFree--;
#endif

	return wave;
}

static void ReturnWaveToPool( SYS_Wave* wave )
{
	assert( wave );

	// remove wave from usedList.
	if ( wave->prev )
		wave->prev->next = wave->next;
	if ( wave->next )
		wave->next->prev = wave->prev;
	if ( usedList == wave )
		usedList = wave->next;
	
	// add wave to head of freeList
	wave->next = freeList;
	if ( freeList )
		freeList->prev = wave;
	freeList = wave;

#ifdef SYS_SOUNDDEBUG
	soundsLoaded--;
	soundsFree++;
#endif

}

static int SDLFormatToOpenALFormat(Uint16 format,Uint8 channels)
{
	if ( (format == AUDIO_S8) || (format == AUDIO_U8) )
		return channels == 1 ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
	else if ( (format == AUDIO_S16SYS) || (format == AUDIO_U16SYS) )
		return channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	else
	{
		assert(0);
		return AL_FORMAT_MONO8;	// Unsupported audio format, brace your ears for NOISE!
	}
}

static SYS_SOUNDHANDLE SYS_LoadWAVFile(char* filename)
{
	// use SDL to load a wave file.
	SDL_AudioSpec wav_spec;
	Uint32 wav_length;
	Uint8 *wav_buffer;

	// Get PCM data from a WAV file
	if (SDL_LoadWAV(filename, &wav_spec, &wav_buffer, &wav_length) == 0)
	{
		fprintf(stderr, "Could not open wav %s: %s\n", filename, SDL_GetError());
		return 0;
	}
	//DumpSDLAudioSpec(&wav_spec);

	SYS_Wave* wave = GetWaveFromFreePool();
	wave->filename = filename;
	alGenBuffers(1, &wave->buffer);

	// copy PCM data into buffer
	int format = SDLFormatToOpenALFormat(wav_spec.format, wav_spec.channels);
	alBufferData(wave->buffer, format, wav_buffer, wav_length, wav_spec.freq);
	CheckForErrors();

	// free PCM data
	SDL_FreeWAV(wav_buffer);

#ifdef SYS_SOUNDDEBUG
	fprintf( soundLog, "SYS_LoadSound( \"%s\" ), %d loaded, %d free\n", wave->filename, soundsLoaded, soundsFree );
#endif

	return (SYS_SOUNDHANDLE)wave;
}

static SYS_SOUNDHANDLE SYS_LoadMP3File(char* filename)
{
	mpg123_handle* mh = 0;
	int error = MPG123_OK;
	
	mh = mpg123_new(0, &error);
	if ((error != MPG123_OK) || (mh == 0))
	{
		printf("error mpg123_new()\n");
		return 0;
	}
	
	error = mpg123_open(mh, filename);
	if (error != MPG123_OK)
	{
		printf("error mpg123_open()\n");
		mpg123_delete(mh);
		return 0;
	}
	
	int channels, encoding;
	long rate;
	error = mpg123_getformat(mh, &rate, &channels, &encoding);
	if (error != MPG123_OK)
	{
		printf("error mpg123_getformat()\n");
		mpg123_delete(mh);
		return 0;
	}
	
	if (encoding != MPG123_ENC_SIGNED_16)
	{ 
		// Signed 16 is the default output format anyways; it would actually by only different if we forced it.
	  	// So this check is here just for this explanation.
		mpg123_delete(mh);
		printf( "Bad encoding: 0x%x!\n", encoding);
		return 0;
	}
	
	// Ensure that this output format will not change (it could, when we allow it).
	mpg123_format_none(mh);
	mpg123_format(mh, rate, channels, encoding);

	size_t block_size = mpg123_outblock( mh );
	size_t buffer_size = 1024 * 1024 * 100;  // 100 megs
	unsigned char* buffer = (unsigned char*)malloc( buffer_size );
	size_t bytes_read = 0;
	size_t total_bytes_read = 0;
	unsigned char* b = buffer;

	do
	{
		error = mpg123_read(mh, b, block_size, &bytes_read);
		b += bytes_read;
		total_bytes_read += bytes_read;
		
		printf( "read %d bytes! total = %d\n", (unsigned int)bytes_read, (unsigned int)total_bytes_read);
		
		// We are not in feeder mode, so MPG123_OK, MPG123_ERR and MPG123_NEW_FORMAT are the only possibilities.
		//   We do not handle a new format, MPG123_DONE is the end... so abort on anything not MPG123_OK.
	} while (error == MPG123_OK);
	
	printf("bytes_read = %d\n", (unsigned int)total_bytes_read);

	SYS_Wave* wave = GetWaveFromFreePool();
	wave->filename = filename;
	alGenBuffers(1, &wave->buffer);

	// copy PCM data into OpenAL buffer
	alBufferData(wave->buffer, AL_FORMAT_STEREO16, buffer, total_bytes_read, rate);
	CheckForErrors();

	free(buffer);
	mpg123_delete(mh);
	
	return (SYS_SOUNDHANDLE)wave;
}

SYS_SOUNDHANDLE SYS_LoadSound(char* filename)
{
	int len = strlen(filename);
	char ext[4];
	ext[0] = tolower(filename[len-3]);
	ext[1] = tolower(filename[len-2]);
	ext[2] = tolower(filename[len-1]);
	ext[3] = 0;
	
	if (strcmp("wav", ext) == 0)
	{
		return SYS_LoadWAVFile(filename);
	}
	else if (strcmp("mp3", ext) == 0)
	{
		return SYS_LoadMP3File(filename);
	}
	else
	{
		SYS_Error("Could not open %s\n", filename);
		return 0;
	}
}

void SYS_ReleaseSound( SYS_SOUNDHANDLE soundHandle )
{
	SYS_Wave* wave = (SYS_Wave*)soundHandle;

#ifdef SYS_SOUNDDEBUG
	fprintf( soundLog, "SYS_ReleaseSound( \"%s\" ), %d loaded, %d free\n", wave->fileName, soundsLoaded-1, soundsFree+1 );
#endif

	assert(wave);
	alDeleteBuffers(1, &wave->buffer);

	if (wave->source)
		alDeleteSources(1, &wave->source);

	ReturnWaveToPool(wave);
}

// plays this sound once, (fire and forget)
void SYS_TriggerSound( SYS_SOUNDHANDLE soundHandle )
{
	SYS_TriggerSoundVolume( soundHandle, 255 );
}

void SYS_TriggerSoundVolume( SYS_SOUNDHANDLE soundHandle, unsigned char volume )
{
	int i;
	SYS_Wave* wave = (SYS_Wave*)soundHandle;
	ALint val = 0;
	assert(wave);

	// find a stopped or unallocated source in the pool
	for ( i = 0; i < SIZEOFSOURCEPOOL; ++i )
	{
		if ( sourcePool[i] == 0xffffffff )
		{
			// allocate a new source
			alGenSources(1, &sourcePool[i]);
			break;
		}
		else
		{
			alGetSourcei(sourcePool[i], AL_SOURCE_STATE, &val);
   			if ( val != AL_PLAYING )
			{
				alDeleteSources(1, &sourcePool[i]);
				break;
			}
		}
	}

	// if we've found a free surface
	if ( i < SIZEOFSOURCEPOOL )
	{
		alGenSources(1,&sourcePool[i]);
		alSourcei(sourcePool[i], AL_BUFFER, wave->buffer);
		alSourcef(sourcePool[i], AL_GAIN, volume/255.0f);
		alSourcei(sourcePool[i], AL_SOURCE_RELATIVE, 1);
		alSourcePlay(sourcePool[i]);
	}
}

// play this sound but only one instance of this sound is allowed to be playing at once.
void SYS_PlaySound( SYS_SOUNDHANDLE soundHandle, int looping )
{
	SYS_PlaySoundVolume( soundHandle, looping, 255 );
}

void SYS_PlaySoundVolume( SYS_SOUNDHANDLE soundHandle, int looping, unsigned char volume )
{
	SYS_Wave* wave = (SYS_Wave*)soundHandle;
	assert(wave);

	if (wave->source)
		alDeleteSources(1, &wave->source);
	
	alGenSources(1, &wave->source);
	alSourcei(wave->source, AL_BUFFER, wave->buffer);
	alSourcef(wave->source, AL_GAIN, volume/255.0f);
	alSourcei(wave->source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
	alSourcei(wave->source, AL_SOURCE_RELATIVE, 1);
	alSourcePlay(wave->source);
}

void SYS_SetListenerOrientation( float position[3], float forward[3], float up[3] )
{
	alListenerf(AL_GAIN, 1.0f);
	alListenerfv(AL_POSITION, position);
		
	// OpenAL expects 6 floats
	float o[6];
	o[0] = forward[0]; 
	o[1] = forward[1];
	o[2] = forward[2];
	o[3] = up[0]; 
	o[4] = up[1];
	o[5] = up[2];
	
	alListenerfv(AL_ORIENTATION, o);
}

void SYS_TriggerSound3D( SYS_SOUNDHANDLE soundHandle, float position[3], unsigned char volume )
{
	int i;
	SYS_Wave* wave = (SYS_Wave*)soundHandle;
	ALint val = 0;
	assert(wave);

	// find a stopped or unallocated source in the pool
	for ( i = 0; i < SIZEOFSOURCEPOOL; ++i )
	{
		if ( sourcePool[i] == 0xffffffff )
		{
			// allocate a new source
			alGenSources(1, &sourcePool[i]);
			break;
		}
		else
		{
			alGetSourcei(sourcePool[i], AL_SOURCE_STATE, &val);
   			if ( val != AL_PLAYING )
			{
				alSourcef(sourcePool[i], AL_GAIN, 0.0f );
				alDeleteSources(1, &sourcePool[i]);
				break;
			}
		}
	}

	// if we've found a free surface
	if ( i < SIZEOFSOURCEPOOL )
	{
		alGenSources(1,&sourcePool[i]);
		alSourcei(sourcePool[i], AL_BUFFER, wave->buffer);
		alSourcef(sourcePool[i], AL_GAIN, volume/255.0f);
		alSourcefv(sourcePool[i], AL_POSITION, position);
		
		// set falloff constants.
		const float kRefDistance = 2.0f;
		const float kMaxDistance = 30.0f;	// half the length of the cylinder.
		const float kRolloffFactor = 1.0f;

		alSourcef(sourcePool[i], AL_REFERENCE_DISTANCE, kRefDistance);
		alSourcef(sourcePool[i], AL_MAX_DISTANCE, kMaxDistance);
		alSourcef(sourcePool[i], AL_ROLLOFF_FACTOR, kRolloffFactor);
		
		alSourcePlay(sourcePool[i]);
	}
}

void SYS_StopSound( SYS_SOUNDHANDLE soundHandle )
{
	SYS_Wave* wave = (SYS_Wave*)soundHandle;
	assert(wave);

	if (wave->source)
	{
		alSourceStop(wave->source);
	}
}

int SYS_IsSoundPlaying( SYS_SOUNDHANDLE soundHandle )
{
	SYS_Wave* wave = (SYS_Wave*)soundHandle;
	assert(wave);

	if (wave->source)
	{
		ALint val;
		alGetSourcei(wave->source, AL_SOURCE_STATE, &val);
		return val == AL_PLAYING;
	}
	return 0;
}

#include "assert.h"
#include "../system.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <SDL/SDL_audio.h>
#ifdef DARWIN
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include <mpg123.h>

ALCcontext* g_context = 0;
ALCdevice* g_device = 0;

//#define SYS_SOUNDDEBUG

struct SYS_Wave
{
	struct SYS_Wave* next;
	struct SYS_Wave* prev;
	ALuint buffer;
	ALuint source;  		// used for single instance sounds.
	
	mpg123_handle* mh;		// only used for streaming mp3 sounds.
	ALuint mp3_buffers[2];	// only used for streaming mp3 sounds.
	int bufferIndex;
	int channels;			// only used for streaming mp3 sounds.
	long rate;				// only used for streaming mp3 sounds.
	size_t bufferSize;		// size in bytes of streaming mp3 buffers.
	const char* filename;	// for debugging.
};

#define SIZEOFWAVEPOOL 500
struct SYS_Wave wavePool[SIZEOFWAVEPOOL];
struct SYS_Wave* freeList = 0;
struct SYS_Wave* usedList = 0;

// used for dynamic sounds (fire & forget, play once)
#define SIZEOFSOURCEPOOL 50
ALuint sourcePool[SIZEOFSOURCEPOOL];
extern char* g_DataPath;

#ifdef SYS_SOUNDDEBUG
FILE* soundLog;
const char* soundLogFileName = "sound.log";
int soundsLoaded = 0;
int soundsFree = SIZEOFWAVEPOOL;
#endif

#ifdef SYS_SOUNDDEBUG
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
#else
void CheckForErrors() {}
#endif

void SYS_InitSound()
{
	
#ifdef SYS_SOUNDDEBUG
	soundsLoaded = 0;
	soundsFree = SIZEOFWAVEPOOL;
	soundLog = fopen( soundLogFileName, "w" );
	fprintf(soundLog, "SYS_InitSound() - OpenAL SKU - %d sounds in pool\n", soundsFree);
	
	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const char* devicesStr = alcGetString(0, ALC_DEVICE_SPECIFIER);
		const char* defaultDeviceStr = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
		fprintf(soundLog, "OpenAL default device = %s\n", defaultDeviceStr);
		
		fprintf(soundLog, "All devices:\n");
		const char* p = devicesStr;
		while( *p )
		{
			fprintf(soundLog, "%s\n", p);
			p += (strlen(p) + 1);
		}
	}
#endif

	// open device
	ALCdevice* device = alcOpenDevice(0);
	if ( !device )
	{
		SYS_Error("Error opening OpenAL device\n");
		return;
	}
	
#ifdef SYS_SOUNDDEBUG
	const char* extStr = alcGetString(device, ALC_EXTENSIONS);
	fprintf(soundLog, "OpenAL extentions : %s\n", extStr);
#endif

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
	memset(sourcePool, 0, sizeof(ALuint) * SIZEOFSOURCEPOOL);
	CheckForErrors();
	
	int error = mpg123_init();
	if (error != MPG123_OK)
	{
		printf("Error mpg123_init() : %s\n", mpg123_plain_strerror(error));
	}
}

void SYS_ShutdownSound()
{
	SYS_StopAllSounds();

#ifdef SYS_SOUNDDEBUG
	fprintf( soundLog, "SYS_ShutdownSound(), %d loaded, %d free\n", soundsLoaded, soundsFree );	
#endif

	int i;
	for (i = 0; i < SIZEOFSOURCEPOOL; ++i)
	{
		if (sourcePool[i])
		{
			alDeleteSources(1, &sourcePool[i]);
			CheckForErrors();
		}
	}
	
	while ( usedList )
	{		
		SYS_ReleaseSound( (SYS_SOUNDHANDLE)usedList );
	}
	
	
#ifdef SYS_SOUNDDEBUG
	fprintf( soundLog, "Cleaned up used sounds, %d loaded, %d free\n", soundsLoaded, soundsFree );
	fclose( soundLog );
#endif
	
	mpg123_exit();
	
	ALCcontext* context = alcGetCurrentContext();
	CheckForErrors();
	ALCdevice* device = alcGetContextsDevice(context);
	CheckForErrors();
	alcMakeContextCurrent(0);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

static void SYS_ProcessStreaming();

void SYS_ProcessSound()
{
	SYS_ProcessStreaming();
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
	wave->source = 0;
	wave->mp3_buffers[0] = 0;
	wave->mp3_buffers[1] = 0;
	wave->filename = 0;
	wave->mh = 0;

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
	if ( wave->mh )
		mpg123_delete(wave->mh);
	
	// add wave to head of freeList
	wave->next = freeList;
	wave->prev = 0;
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

static SYS_SOUNDHANDLE SYS_LoadWAVFile(const char* filename)
{
	// use SDL to load a wave file.
	SDL_AudioSpec wav_spec;
	Uint32 wav_length;
	Uint8 *wav_buffer;
	
	char newfilename[512];
	sprintf(newfilename,"%s%s",g_DataPath,filename);

	// Get PCM data from a WAV file
	if (SDL_LoadWAV(newfilename, &wav_spec, &wav_buffer, &wav_length) == 0)
	{
		fprintf(stderr, "Could not open wav %s: %s\n", filename, SDL_GetError());
		return 0;
	}
	//DumpSDLAudioSpec(&wav_spec);


	SYS_Wave* wave = GetWaveFromFreePool();
	wave->filename = filename;

	alGenBuffers(1, &wave->buffer);
	CheckForErrors();

	// copy PCM data into buffer
	int format = SDLFormatToOpenALFormat(wav_spec.format, wav_spec.channels);
	alBufferData(wave->buffer, format, wav_buffer, wav_length, wav_spec.freq);
	CheckForErrors();

	// free PCM data
	SDL_FreeWAV(wav_buffer);

	return (SYS_SOUNDHANDLE)wave;
}

static SYS_SOUNDHANDLE SYS_LoadMP3File(const char* filename)
{
	mpg123_handle* mh = 0;
	int error = MPG123_OK;
	
	mh = mpg123_new(0, &error);
	if ((error != MPG123_OK) || (mh == 0))
	{
		printf("error mpg123_new()\n");
		return 0;
	}
	
	char newfilename[512];
	sprintf(newfilename,"%s%s",g_DataPath,filename);
	
	error = mpg123_open(mh, newfilename);
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
	
	// create streaming OpenAL buffers.
	SYS_Wave* wave = GetWaveFromFreePool();
	wave->filename = filename;

	alGenBuffers(2, wave->mp3_buffers);
	CheckForErrors();
	
	wave->bufferIndex = 0;
	wave->bufferSize = mpg123_outblock( mh ) * 64;
	wave->rate = rate;
	wave->channels = channels;
	wave->mh = mh;
	
	return (SYS_SOUNDHANDLE)wave;
}

static bool SYS_StreamNextBuffer(int bufferIndex, SYS_Wave* wave)
{
	assert(wave->mh);
	
	// TODO: should probably use a static buffer
	unsigned char* buffer = (unsigned char*)malloc( wave->bufferSize );
	size_t bytes_read = 0;
	
	int error = mpg123_read(wave->mh, buffer, wave->bufferSize, &bytes_read);
	
	// We are not in feeder mode, so MPG123_OK, MPG123_ERR and MPG123_NEW_FORMAT are the only possibilities.
	//   We do not handle a new format, MPG123_DONE is the end... so abort on anything not MPG123_OK.
	//} while (error == MPG123_OK);
	
	if ( error != MPG123_OK )
	{
		// we hit the end.
		return false;	// TODO: loop or stop the sound...
	}

#ifdef SYS_SOUNDDEBUG		
	printf("bytes_read = %d\n", (unsigned int)bytes_read);
#endif

	// copy PCM data into OpenAL buffer
	alBufferData(wave->mp3_buffers[bufferIndex], AL_FORMAT_STEREO16, buffer, bytes_read, wave->rate);
	CheckForErrors();

	free(buffer);
	
	return true;
}

static void SYS_ProcessStreaming()
{	
	// loop over the used wave list.
	SYS_Wave* wave = usedList;	
	while (wave)
	{		
		// if this is a streaming sound and it's currently playing
		if (wave->mh && SYS_IsSoundPlaying((SYS_SOUNDHANDLE)wave))
		{
			// if we need to fill the next buffer
			ALint val;
			alGetSourcei(wave->source, AL_BUFFERS_PROCESSED, &val);
			CheckForErrors();
			
			if (val)	// val is not zero
			{
#ifdef SYS_SOUNDDEBUG
				printf("processStreaming() for %s\n", wave->filename);
#endif				
				alSourceUnqueueBuffers(wave->source,1,&wave->mp3_buffers[wave->bufferIndex]);
				CheckForErrors();
				
				bool success = SYS_StreamNextBuffer(wave->bufferIndex, wave);
				if (success)
				{
					alSourceQueueBuffers(wave->source,1,&wave->mp3_buffers[wave->bufferIndex]);
					CheckForErrors();
					wave->bufferIndex = (wave->bufferIndex + 1) % 2;
				}
				else
				{
					// TODO: loop instead of stopping
					SYS_StopSound((SYS_SOUNDHANDLE)wave);
				}
			}
		}
		wave = wave->next;
	}
}

SYS_SOUNDHANDLE SYS_LoadSound(const char* filename)
{

	int len = strlen(filename);
	char ext[4];
	ext[0] = tolower(filename[len-3]);
	ext[1] = tolower(filename[len-2]);
	ext[2] = tolower(filename[len-1]);
	ext[3] = 0;

	SYS_SOUNDHANDLE result;
	if (strcmp("wav", ext) == 0)
	{
		result = SYS_LoadWAVFile(filename);
	}
	else if (strcmp("mp3", ext) == 0)
	{
		result = SYS_LoadMP3File(filename);
	}
	else
	{
		SYS_Error("Could not open %s\n", filename);
		result = 0;
	}

#ifdef SYS_SOUNDDEBUG
	SYS_Wave* wave = (SYS_Wave*)result;
	fprintf( soundLog, "SYS_LoadSound( \"%s\" ), %d loaded, %d free\n", filename, soundsLoaded, soundsFree );
	fprintf(soundLog, "    buffer = %d, source = %d, mp3_buffers = %d, %d\n", wave->buffer, wave->source, wave->mp3_buffers[0], wave->mp3_buffers[1]);
#endif

	return result;
}

void SYS_ReleaseSound( SYS_SOUNDHANDLE soundHandle )
{
	SYS_Wave* wave = (SYS_Wave*)soundHandle;

#ifdef SYS_SOUNDDEBUG
	fprintf(soundLog, "SYS_ReleaseSound( \"%s\" ), %d loaded, %d free\n", wave->filename, soundsLoaded-1, soundsFree+1);
	fprintf(soundLog, "    buffer = %d, source = %d, mp3_buffers = %d, %d\n", wave->buffer, wave->source, wave->mp3_buffers[0], wave->mp3_buffers[1]);
	fflush(soundLog);
#endif

	assert(wave);

	if (wave->source)
	{
		alDeleteSources(1, &wave->source);
		CheckForErrors();
	}

	// 


	if (wave->buffer)
	{
		alDeleteBuffers(1, &wave->buffer);
		CheckForErrors();
	}

	if (wave->mp3_buffers[0])
	{
		alDeleteBuffers(2, wave->mp3_buffers);
		CheckForErrors();
	}

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
		if ( sourcePool[i] == 0 )
		{
			break;
		}
		else
		{
			alGetSourcei(sourcePool[i], AL_SOURCE_STATE, &val);
			CheckForErrors();
   			if ( val != AL_PLAYING )
			{
				alDeleteSources(1, &sourcePool[i]);
				CheckForErrors();
				break;
			}
		}
	}

	// if we've found a free surface
	if ( i < SIZEOFSOURCEPOOL )
	{
		alGenSources(1,&sourcePool[i]);
		CheckForErrors();
		alSourcei(sourcePool[i], AL_BUFFER, wave->buffer);
		CheckForErrors();
		alSourcef(sourcePool[i], AL_GAIN, volume/255.0f);
		CheckForErrors();
		alSourcei(sourcePool[i], AL_SOURCE_RELATIVE, 1);
		CheckForErrors();
		alSourcePlay(sourcePool[i]);
		CheckForErrors();
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

	if (wave->mh)
	{
		if (wave->source)
		{
			alDeleteSources(1, &wave->source);
			CheckForErrors();
		}
	
		alGenSources(1, &wave->source);
		CheckForErrors();
		SYS_StreamNextBuffer(0, wave);		
		SYS_StreamNextBuffer(1, wave);		
		alSourceQueueBuffers(wave->source, 2, wave->mp3_buffers);
		CheckForErrors();
		alSourcef(wave->source, AL_GAIN, volume/255.0f);
		CheckForErrors();
		//alSourcei(wave->source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
		alSourcei(wave->source, AL_SOURCE_RELATIVE, 1);		
		CheckForErrors();
		alSourcePlay(wave->source);
		CheckForErrors();
#ifdef SYS_SOUNDDEBUG		
		printf("playing stream %s\n", wave->filename );
#endif
	}
	else
	{
		if (wave->source)
		{
			alDeleteSources(1, &wave->source);
			CheckForErrors();
		}
	
		alGenSources(1, &wave->source);
		CheckForErrors();
		alSourcei(wave->source, AL_BUFFER, wave->buffer);
		CheckForErrors();
		alSourcef(wave->source, AL_GAIN, volume/255.0f);
		CheckForErrors();
		alSourcei(wave->source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
		CheckForErrors();
		alSourcei(wave->source, AL_SOURCE_RELATIVE, 1);
		CheckForErrors();
		alSourcePlay(wave->source);
		CheckForErrors();
	}
}

void SYS_SetListenerOrientation( float position[3], float forward[3], float up[3] )
{
	alListenerf(AL_GAIN, 1.0f);
	CheckForErrors();
	alListenerfv(AL_POSITION, position);
	CheckForErrors();
		
	// OpenAL expects 6 floats
	float o[6];
	o[0] = forward[0]; 
	o[1] = forward[1];
	o[2] = forward[2];
	o[3] = up[0]; 
	o[4] = up[1];
	o[5] = up[2];
	
	alListenerfv(AL_ORIENTATION, o);
	CheckForErrors();
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
		if ( sourcePool[i] == 0 )
		{
			break;
		}
		else
		{
			alGetSourcei(sourcePool[i], AL_SOURCE_STATE, &val);
			CheckForErrors();
   			if ( val != AL_PLAYING )
			{
				alSourcef(sourcePool[i], AL_GAIN, 0.0f );
				CheckForErrors();
				alDeleteSources(1, &sourcePool[i]);
				CheckForErrors();
				break;
			}
		}
	}

	// if we've found a free surface
	if ( i < SIZEOFSOURCEPOOL )
	{
		alGenSources(1,&sourcePool[i]);
		CheckForErrors();
		alSourcei(sourcePool[i], AL_BUFFER, wave->buffer);
		CheckForErrors();
		alSourcef(sourcePool[i], AL_GAIN, volume/255.0f);
		CheckForErrors();
		alSourcefv(sourcePool[i], AL_POSITION, position);
		CheckForErrors();
		
		// set falloff constants.
		const float kRefDistance = 2.0f;
		const float kMaxDistance = 30.0f;	// half the length of the cylinder.
		const float kRolloffFactor = 1.0f;

		alSourcef(sourcePool[i], AL_REFERENCE_DISTANCE, kRefDistance);
		CheckForErrors();
		alSourcef(sourcePool[i], AL_MAX_DISTANCE, kMaxDistance);
		CheckForErrors();
		alSourcef(sourcePool[i], AL_ROLLOFF_FACTOR, kRolloffFactor);
		CheckForErrors();
		
		alSourcePlay(sourcePool[i]);
		CheckForErrors();
	}
}

void SYS_StopSound( SYS_SOUNDHANDLE soundHandle )
{
	SYS_Wave* wave = (SYS_Wave*)soundHandle;
	assert(wave);

	if (wave->source)
	{
		alSourceStop(wave->source);
		CheckForErrors();
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
		CheckForErrors();
		return val == AL_PLAYING;
	}
	return 0;
}

void SYS_StopAllSounds()
{
	int i;
	for (i = 0; i < SIZEOFSOURCEPOOL; ++i)
	{
		if (sourcePool[i])
		{
			alDeleteSources(1, &sourcePool[i]);
			CheckForErrors();
			sourcePool[i] = 0;
		}
	}

	SYS_Wave* p = usedList;
	while (p)
	{
		if (p->source)
		{
			alDeleteSources(1, &p->source);
			p->source = 0;
		}
		p = p->next;
	}
}

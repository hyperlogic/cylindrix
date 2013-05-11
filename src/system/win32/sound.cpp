#define INITGUID
#include <windows.h>
#include <dsound.h>
#include <cguid.h>

#include <assert.h>
#include "..\system.h"
#include <math.h>
#include <stdio.h>

extern HWND hwnd;

LPDIRECTSOUND8 ds = 0;

struct SYS_Wave
{
	struct SYS_Wave* next;
	struct SYS_Wave* prev;
	HMMIO hmmio;				// open mmio file. (need to close)
	MMCKINFO ckRiff;			// wave chunk
	MMCKINFO ckIn;				// format chunk
	MMCKINFO ck;				// data chunk
	WAVEFORMATEX* pwfx;			// Pointer to WAVEFORMATEX structure  (need to free)
	LPDIRECTSOUNDBUFFER dsb;	// ds buffer, (need to release)
	const char* fileName;		// for debugging.
	DWORD dsBufferSize;
};

#define SIZEOFWAVEPOOL 500
struct SYS_Wave wavePool[SIZEOFWAVEPOOL];
struct SYS_Wave* freeList = 0;
struct SYS_Wave* usedList = 0;

#define SIZEOFDSBUFFERPOOL 50
LPDIRECTSOUNDBUFFER dsBufferPool[SIZEOFDSBUFFERPOOL];

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

int wave_to_dsound_volume[256];
int wave_to_dsound_pan[256];

//#define SYS_SOUNDDEBUG

#ifdef SYS_SOUNDDEBUG
FILE* soundLog;
char* soundLogFileName = "sound.log";
int soundsLoaded = 0;
int soundsFree = SIZEOFWAVEPOOL;
#endif


void SYS_InitSound()
{
	HRESULT hr;
	LPDIRECTSOUNDBUFFER pb = 0;
	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;
	int i;

	const DWORD NumChannels = 2;
	const DWORD SamplesPerSec = 22050;
	const DWORD BitsPerSample = 16;

	if ( ds )	
		SYS_Error( "SYS_InitSound() called twice!" );
	
	// create the directsound interface.
	hr = DirectSoundCreate8( NULL, &ds, NULL );
	if ( FAILED(hr) )
		SYS_Error( "DirectSoundCreate8 failed!\n" );

	// set priority	
	hr = ds->SetCooperativeLevel( hwnd, DSSCL_PRIORITY );
	if ( FAILED(hr) )
		SYS_Error( "SetCooperativeLevel failed!\n" );

	// create a primary buffer.	
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;

    hr = ds->CreateSoundBuffer( &dsbd, &pb, NULL );
	if ( FAILED(hr) )
        SYS_Error( "CreateSoundBuffer failed!\n" );

	// set the format of the primary buffer    
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) NumChannels; 
    wfx.nSamplesPerSec  = (DWORD) SamplesPerSec; 
    wfx.wBitsPerSample  = (WORD) BitsPerSample; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);

    hr = pb->SetFormat( &wfx );
	if ( FAILED(hr) )
		SYS_Error( "SetFormat failed!\n" );

	pb->Release();

	// init the wave pool
	memset( wavePool, 0, sizeof( struct SYS_Wave ) * SIZEOFWAVEPOOL );

	freeList = wavePool;	

	usedList = 0;
	for ( i = 0; i < SIZEOFWAVEPOOL; ++i )
	{
		freeList[i].prev = (i == 0) ? 0 : &wavePool[i-1];
		freeList[i].next = (i == SIZEOFWAVEPOOL-1) ? 0 : &wavePool[i+1];
	}

	// init the ds buffer pool
	for ( i = 0; i < SIZEOFDSBUFFERPOOL; ++i )
		dsBufferPool[i] = 0;

	// setup volume lookup table
	wave_to_dsound_volume[0] = DSBVOLUME_MIN;
	for ( i = 1; i < 256; i++ )
		wave_to_dsound_volume[i] = (int)MAX(DSBVOLUME_MIN, DSBVOLUME_MAX + 2000.0*log10(i/255.0));

	// setup pan lookup table
	wave_to_dsound_pan[0] = DSBPAN_LEFT;
	for (i = 1; i < 128; i++)
		wave_to_dsound_pan[i] = (int)MAX(DSBPAN_LEFT, DSBPAN_CENTER + 2000.0*log10(i/127.0));

	wave_to_dsound_pan[255] = DSBPAN_RIGHT;
	for (i = 128; i < 255; i++)
		wave_to_dsound_pan[i] = (int)MIN(DSBPAN_RIGHT, DSBPAN_CENTER - 2000.0*log10((255.0-i)/127.0));

#ifdef SYS_SOUNDDEBUG
	soundsLoaded = 0;
	soundsFree = SIZEOFWAVEPOOL;

	soundLog = fopen( soundLogFileName, "w" );
	fprintf( soundLog, "SYS_InitSound() - win32 SKU - %d sounds in pool\n", soundsFree );
#endif
}

void SYS_ShutdownSound()
{
	int i;

#ifdef SYS_SOUNDDEBUG
	fprintf( soundLog, "SYS_ShutdownSound(), %d loaded, %d free\n", soundsLoaded, soundsFree );	
#endif

	// release all buffers in the dsBufferPool.
	for ( i = 0; i < SIZEOFDSBUFFERPOOL; ++i )
	{
		if ( dsBufferPool[i] )
			dsBufferPool[i]->Release();
	}

	// release all buffers in the usedPool.
	while ( usedList )
	{
		SYS_ReleaseSound( (SYS_SOUNDHANDLE)usedList );
	}

#ifdef SYS_SOUNDDEBUG
	fprintf( soundLog, "Cleaned up used sounds, %d loaded, %d free\n", soundsLoaded, soundsFree );
	fclose( soundLog );
#endif

	// release directSound
	ds->Release();
	ds = 0;
}

static struct SYS_Wave* AllocWaveFromPool()
{
	struct SYS_Wave* wave;
	
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
	wave->hmmio = 0;
	wave->pwfx = 0;
	wave->dsb = 0;
	wave->fileName = 0;

#ifdef SYS_SOUNDDEBUG
	soundsLoaded++;
	soundsFree--;
#endif

	return wave;
}

static void ReturnWaveToPool( struct SYS_Wave* wave )
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

void InitWaveFromFile( struct SYS_Wave* wave, char* fileName )
{
	PCMWAVEFORMAT pcmWaveFormat;  // Temp PCM structure to load in.
	WORD cbExtraBytes = 0;

	// first step is to load the sound data.
	wave->hmmio = mmioOpen( fileName, NULL, MMIO_ALLOCBUF | MMIO_READ );

	if ( wave->hmmio == NULL )
		SYS_Error( "could not open file %s\n", fileName );

	if ( ( mmioDescend( wave->hmmio, &wave->ckRiff, NULL, 0 ) != 0 ) )
		SYS_Error( "mmioDescend Failed!\n" );

    // Check to make sure this is a valid wave file
    if( (wave->ckRiff.ckid != FOURCC_RIFF) ||
        (wave->ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E') ) )
        SYS_Error( "not a valid .WAV file\n" );

    // Search the input file for for the 'fmt ' chunk.
    wave->ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if ( mmioDescend( wave->hmmio, &wave->ckIn, &wave->ckRiff, MMIO_FINDCHUNK ) != 0 )
        SYS_Error( "couldnt get fmt\n" );

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
	if ( wave->ckIn.cksize < (LONG) sizeof(PCMWAVEFORMAT) )
		SYS_Error( "fmt chunk smaller then pcmwaveformat struct!\n" );

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( wave->hmmio, (HPSTR) &pcmWaveFormat, 
                  sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat) )
        SYS_Error( "error reading PCMWAVEFORMAT!\n" );

    if ( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {		
        wave->pwfx = (WAVEFORMATEX*) malloc( sizeof(WAVEFORMATEX) );
        if ( wave->pwfx == 0 )
            SYS_Error( "malloc of WAVEFORMATEX failed!\n" );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( wave->pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        wave->pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.   
        if ( mmioRead( wave->hmmio, (CHAR*)&cbExtraBytes, sizeof(WORD)) != sizeof(WORD) )
            SYS_Error( "read of num extra bytes failed!\n" );

        wave->pwfx = (WAVEFORMATEX*) malloc( sizeof(WAVEFORMATEX) + cbExtraBytes );
        if ( wave->pwfx == 0 )
            SYS_Error( "malloc of WAVEFORMATEX failed!\n" );

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( wave->pwfx, &pcmWaveFormat, sizeof(pcmWaveFormat) );
        wave->pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if ( mmioRead( wave->hmmio, (CHAR*)(((BYTE*)&(wave->pwfx->cbSize))+sizeof(WORD)),
                      cbExtraBytes ) != cbExtraBytes )
            SYS_Error( "read of extra bytes failed!\n" );
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if ( mmioAscend( wave->hmmio, &wave->ckIn, 0 ) != 0 )
        SYS_Error( "mmioAscend failed!\n" );

    // Seek to the data
    if ( mmioSeek( wave->hmmio, wave->ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET ) == -1 )
        SYS_Error( "mmioSeek failed!\n" );

    // Search the input file for the 'data' chunk.
    wave->ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if ( mmioDescend( wave->hmmio, &wave->ck, &wave->ckRiff, MMIO_FINDCHUNK ) != 0 )
		SYS_Error( "mmioDescend into data chuck failed!\n" );

	wave->dsBufferSize = wave->ck.cksize;
	wave->fileName = fileName;
}

void FillDSBuffer( struct SYS_Wave* wave )
{
	void* lockedBuffer;
	DWORD lockedBufferSize;
	MMIOINFO mmioinfoIn;
	DWORD cT;
	HRESULT hr;

	// lock the ds buffer
    hr = wave->dsb->Lock( 0, wave->dsBufferSize, &lockedBuffer, &lockedBufferSize, NULL, NULL, 0L );
	if ( FAILED(hr) )
        SYS_Error( "Buffer lock failed!\n" );

	// read in the wav data into the locked direct sound buffer.    
    if ( mmioGetInfo( wave->hmmio, &mmioinfoIn, 0 ) != 0 )
		SYS_Error( "mmioGetInfo failed!\n" );

    for ( cT = 0; cT < wave->dsBufferSize; cT++ )
    {
        // Copy the bytes from the io to the buffer.
        if ( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
        {
            if ( mmioAdvance( wave->hmmio, &mmioinfoIn, MMIO_READ ) != 0 )
                SYS_Error( "mmioAdvanceFailed!\n" );

            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
                SYS_Error( "???, hit end?\n" );
        }

        // Actual copy.
        *((BYTE*)lockedBuffer+cT) = *((BYTE*)mmioinfoIn.pchNext);
        mmioinfoIn.pchNext++;
    }

    if ( mmioSetInfo( wave->hmmio, &mmioinfoIn, 0 ) != 0 )
        SYS_Error( "mmioSetInfo failed!\n" );

    // Unlock the buffer, we don't need it anymore.
    hr = wave->dsb->Unlock( lockedBuffer, lockedBufferSize, NULL, 0 );
	if ( FAILED(hr) )
		SYS_Error( "Unlock failed!\n" );

}

SYS_SOUNDHANDLE SYS_LoadSound( const char* fileName )
{
	int size = strlen( fileName );

    // TODO: streaming mp3s.
	if ( fileName[size - 3] == 'm' && fileName[size - 2] == 'p' && fileName[size - 1] == '3' )
        return 0;

	DSBUFFERDESC dsbd;
	HRESULT hr;
	struct SYS_Wave* wave;

	wave = AllocWaveFromPool();
	InitWaveFromFile( wave, fileName );	

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated    
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
    dsbd.dwBufferBytes   = wave->dsBufferSize;
    dsbd.guid3DAlgorithm = GUID_NULL;
    dsbd.lpwfxFormat     = wave->pwfx;
    
    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
    hr = ds->CreateSoundBuffer( &dsbd, &wave->dsb, NULL );

	if ( FAILED(hr) )
		SYS_Error( "CreateSoundBuffer failed!\n" );

//	hr = RestoreBuffer( dsb, NULL );
//	if ( FAILED(hr) )
//		SYS_Error( "RestoreBuffer failed!\n" );

	FillDSBuffer( wave );

#ifdef SYS_SOUNDDEBUG
	fprintf( soundLog, "SYS_LoadSound( \"%s\" ), %d loaded, %d free\n", wave->fileName, soundsLoaded, soundsFree );
#endif

	return (SYS_SOUNDHANDLE)wave;
}

void SYS_ReleaseSound( SYS_SOUNDHANDLE soundHandle )
{
	struct SYS_Wave* wave = (struct SYS_Wave*)soundHandle;

#ifdef SYS_SOUNDDEBUG
	fprintf( soundLog, "SYS_ReleaseSound( \"%s\" ), %d loaded, %d free\n", wave->fileName, soundsLoaded-1, soundsFree+1 );
#endif

	assert( wave );

	if ( wave->hmmio )
	{
		mmioClose( wave->hmmio, 0 );
		wave->hmmio = 0;
	}

	if ( wave->pwfx )
	{
		free( wave->pwfx );
		wave->pwfx = 0;
	}

	if ( wave->dsb )
	{
		wave->dsb->Release();
		wave->dsb = 0;
	}

	ReturnWaveToPool( wave );
}

void SYS_TriggerSoundVolumeWithPan( SYS_SOUNDHANDLE soundHandle, unsigned char volume, unsigned char pan );

// plays this sound once, (fire and forget)
void SYS_TriggerSound( SYS_SOUNDHANDLE soundHandle )
{
	SYS_TriggerSoundVolumeWithPan( soundHandle, 255, 128 );
}

void SYS_TriggerSoundVolume( SYS_SOUNDHANDLE soundHandle, unsigned char volume )
{
	SYS_TriggerSoundVolumeWithPan( soundHandle, volume, 128 );
}

void SYS_SetListenerOrientation( float position[3], float forward[3], float up[3] )
{
	// TODO: spatial sound
}

void SYS_TriggerSound3D( SYS_SOUNDHANDLE soundHandle, float position[3], unsigned char volume )
{
	// TODO: spatial sound
    SYS_TriggerSoundVolumeWithPan( soundHandle, volume, 128 );
}

static void SYS_TriggerSoundVolumeWithPan( SYS_SOUNDHANDLE soundHandle, unsigned char volume, unsigned char pan )
{
	int i;
	struct SYS_Wave* wave = (struct SYS_Wave*)soundHandle;
	DWORD status;

	assert( wave );

	// find an unused surface in the ds buffer pool
	for ( i = 0; i < SIZEOFDSBUFFERPOOL; ++i )
	{
		if ( dsBufferPool[i] )
		{
		    dsBufferPool[i]->GetStatus( &status );
			if ( !(status & DSBSTATUS_PLAYING) )
			{
				// release this surface
				dsBufferPool[i]->Release();
				dsBufferPool[i] = 0;
				break;
			}
		}
		else
		{
			break;
		}
	}

	// if we've found a free surface
	if ( i < SIZEOFDSBUFFERPOOL )
	{
		ds->DuplicateSoundBuffer( wave->dsb, &dsBufferPool[i] );

		dsBufferPool[i]->SetVolume( wave_to_dsound_volume[volume] );
		dsBufferPool[i]->SetPan( wave_to_dsound_pan[pan] );

		dsBufferPool[i]->Play( 0, 0, 0 );
	}
}

// play this sound but only one instance of this sound is allowed to be playing at once.
void SYS_PlaySound( SYS_SOUNDHANDLE soundHandle, int looping )
{
	SYS_PlaySoundVolume( soundHandle, looping, 255 );
}

void SYS_PlaySoundVolume( SYS_SOUNDHANDLE soundHandle, int looping, unsigned char volume )
{
	struct SYS_Wave* wave = (struct SYS_Wave*)soundHandle;

	// TODO: FIXME HACK AJT
	if (wave == 0)
		return;

	assert( wave );


	wave->dsb->SetVolume( wave_to_dsound_volume[volume] );

	wave->dsb->Play( 0, 0, looping ? DSBPLAY_LOOPING : 0 );
}

void SYS_StopSound( SYS_SOUNDHANDLE soundHandle )
{
	struct SYS_Wave* wave = (struct SYS_Wave*)soundHandle;

	// TODO: FIXME HACK AJT
	if (wave == 0)
		return;

	assert( wave );

	wave->dsb->Stop();
}

int SYS_IsSoundPlaying( SYS_SOUNDHANDLE soundHandle )
{
	struct SYS_Wave* wave = (struct SYS_Wave*)soundHandle;
	DWORD status;
	assert( wave );

    wave->dsb->GetStatus( &status );
	return ( status & DSBSTATUS_PLAYING ) != 0;
}

void SYS_StopAllSounds()
{

}

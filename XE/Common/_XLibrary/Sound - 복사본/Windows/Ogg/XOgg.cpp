#include "stdafx.h"
#include "XOgg.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 일단 간단하게 구현하기 위해 통짜로 로딩하여 플레이하는 방식을 했고
// 아이폰에서 쓸때는 스트림형태로 로딩하는걸 구현해야 할듯. 아이폰용 ogg디코더는 어디에?
XOgg *OGG = NULL;

// Variables
LPOVCLEAR			fn_ov_clear;
LPOVREAD			fn_ov_read;
LPOVPCMTOTAL		fn_ov_pcm_total;
LPOVINFO			fn_ov_info;
LPOVCOMMENT			fn_ov_comment;
LPOVOPENCALLBACKS	fn_ov_open_callbacks;

void XOgg::Init( void )
{
	m_bVorbisInit = false;
	// Variables
	fn_ov_clear = NULL;
	fn_ov_read = NULL;
	fn_ov_pcm_total = NULL;
	fn_ov_info = NULL;
	fn_ov_comment = NULL;
	fn_ov_open_callbacks = NULL;

	InitVorbisFile();
	if (!m_bVorbisInit)
	{
		XLOG_ALERT( "Failed to find OggVorbis DLLs (vorbisfile.dll, ogg.dll, or vorbis.dll)" );
		ALFWShutdown();
		return;
	}
}

void XOgg::Destroy( void )
{
	ShutdownVorbisFile();
}

void XOgg::CloseStream( OggVorbis_File &sOggVorbisFile )
{
	// Close OggVorbis stream
	fn_ov_clear(&sOggVorbisFile);
}

void XOgg::InitVorbisFile()
{	
	if (m_bVorbisInit)
		return;

	// Try and load Vorbis DLLs (VorbisFile.dll will load ogg.dll and vorbis.dll)
	m_hVorbisFileDLL = LoadLibraryA("vorbisfile.dll");
	if (m_hVorbisFileDLL)
	{
		fn_ov_clear = (LPOVCLEAR)GetProcAddress(m_hVorbisFileDLL, "ov_clear");
		fn_ov_read = (LPOVREAD)GetProcAddress(m_hVorbisFileDLL, "ov_read");
		fn_ov_pcm_total = (LPOVPCMTOTAL)GetProcAddress(m_hVorbisFileDLL, "ov_pcm_total");
		fn_ov_info = (LPOVINFO)GetProcAddress(m_hVorbisFileDLL, "ov_info");
		fn_ov_comment = (LPOVCOMMENT)GetProcAddress(m_hVorbisFileDLL, "ov_comment");
		fn_ov_open_callbacks = (LPOVOPENCALLBACKS)GetProcAddress(m_hVorbisFileDLL, "ov_open_callbacks");

		if (fn_ov_clear && fn_ov_read && fn_ov_pcm_total && fn_ov_info &&
			fn_ov_comment && fn_ov_open_callbacks)
		{
			m_bVorbisInit = true;
		}
	}
}

void XOgg::ShutdownVorbisFile()
{
	if (m_hVorbisFileDLL)
	{
		FreeLibrary(m_hVorbisFileDLL);
		m_hVorbisFileDLL = NULL;
	}
	m_bVorbisInit = false;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//		Ogg File obj
//
////////////////////////////////////////////////////////////////////////////////////////////////
void XOggObj::Init( void )
{
	ulFrequency = 0;
	ulFormat = 0;
	ulChannels = 0;
	ulBufferSize = 0;
	ulBytesWritten = 0;
	pDecodeBuffer = NULL;
	XCLEAR_ARRAY( uiBuffers );
	uiSource = 0;
	uiBuffer = 0;
	m_Error = 0;
}

void XOggObj::Stop( void )
{
	// Stop the Source and clear the Queue
	alSourceStop(uiSource);
}

void XOggObj::Destroy( void )
{
	// Stop the Source and clear the Queue
	alSourceStop(uiSource);
	alSourcei(uiSource, AL_BUFFER, 0);

	if (pDecodeBuffer)
		SAFE_DELETE_ARRAY( pDecodeBuffer );

	// Clean up buffers and sources
	alDeleteSources( 1, &uiSource );
	alDeleteBuffers( NUMBUFFERS, uiBuffers );
	OGG->CloseStream( sOggVorbisFile );
}

#define	SERVICE_UPDATE_PERIOD	(20)

void Swap(short &s1, short &s2);
size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int ov_seek_func(void *datasource, ogg_int64_t offset, int whence);
int ov_close_func(void *datasource);
long ov_tell_func(void *datasource);


BOOL XOggObj::Load( LPCSTR szOgg )
{
//	ALint			iState;
	ALint			iLoop;
//	ALint			iBuffersProcessed, iTotalBuffersProcessed, iQueuedBuffers;

	// Open Ogg Stream
	ov_callbacks	sCallbacks;
	vorbis_info		*psVorbisInfo;

	sCallbacks.read_func = ov_read_func;
	sCallbacks.seek_func = ov_seek_func;
	sCallbacks.close_func = ov_close_func;
	sCallbacks.tell_func = ov_tell_func;

	// Open the OggVorbis file
	FILE *pOggVorbisFile = NULL;
	fopen_s( &pOggVorbisFile, szOgg, "rb");
	if (!pOggVorbisFile)
	{
		XLOG_ALERT("Could not find %s\n", Convert_char_To_TCHAR( szOgg ) );
//		ShutdownVorbisFile();
		return FALSE;
	}

	// Create an OggVorbis file stream
	if (fn_ov_open_callbacks(pOggVorbisFile, &sOggVorbisFile, NULL, 0, sCallbacks) == 0)
	{
		// Get some information about the file (Channels, Format, and Frequency)
		psVorbisInfo = fn_ov_info(&sOggVorbisFile, -1);
		if (psVorbisInfo)
		{
			ulFrequency = psVorbisInfo->rate;
			ulChannels = psVorbisInfo->channels;
			ulBufferSize = 1024 * 1024 * 64;		// 최대 64메가
			if (psVorbisInfo->channels == 1)
			{
				ulFormat = AL_FORMAT_MONO16;
				// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
//				ulBufferSize = ulFrequency >> 1;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				ulBufferSize -= (ulBufferSize % 2);
			}
			else if (psVorbisInfo->channels == 2)
			{
				ulFormat = AL_FORMAT_STEREO16;
				// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
//				ulBufferSize = ulFrequency;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				ulBufferSize -= (ulBufferSize % 4);
			}
			else if (psVorbisInfo->channels == 4)
			{
				ulFormat = alGetEnumValue("AL_FORMAT_QUAD16");
				// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
//				ulBufferSize = ulFrequency * 2;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				ulBufferSize -= (ulBufferSize % 8);
			}
			else if (psVorbisInfo->channels == 6)
			{
				ulFormat = alGetEnumValue("AL_FORMAT_51CHN16");
				// Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
//				ulBufferSize = ulFrequency * 3;
				// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
				ulBufferSize -= (ulBufferSize % 12);
			}
		}

		if (ulFormat != 0)
		{
			// Allocate a buffer to be used to store decoded data for all Buffers
			pDecodeBuffer = new char[ ulBufferSize ];
			if (!pDecodeBuffer)
			{
				XLOG_ALERT("Failed to allocate memory for decoded OggVorbis data\n");
				fn_ov_clear(&sOggVorbisFile);
//				ShutdownVorbisFile();
				return FALSE;
			}

			// Generate some AL Buffers for streaming
			alGenBuffers( NUMBUFFERS, uiBuffers );

			// Generate a Source to playback the Buffers
			alGenSources( 1, &uiSource );

			// Fill all the Buffers with decoded audio data from the OggVorbis file
//			for (iLoop = 0; iLoop < NUMBUFFERS; iLoop++)
			iLoop = 0;
			{
				ulBytesWritten = DecodeOggVorbis(&sOggVorbisFile, pDecodeBuffer, ulBufferSize, ulChannels);
				if (ulBytesWritten)
				{
					char *pData = new char[ ulBytesWritten ];
					memcpy( pData, pDecodeBuffer, ulBytesWritten );
					SAFE_DELETE_ARRAY( pDecodeBuffer );
					pDecodeBuffer = pData;
					alBufferData(uiBuffers[iLoop], ulFormat, pData, ulBytesWritten, ulFrequency);
					alSourceQueueBuffers(uiSource, 1, &uiBuffers[iLoop]);
				}
			}
		}
	} else
	{
//		ShutdownVorbisFile();
		return FALSE;
	}
	return TRUE;
}

void XOggObj::Play( void )
{
	// Start playing source
	alSourcePlay(uiSource);
}

void XOggObj::SetRepeat( BOOL bRepeat )
{
	alSourcei( uiSource, AL_LOOPING, bRepeat );
}

void XOggObj::SetVolume( float fVolume )
{
	alSourcef( uiSource, AL_GAIN, fVolume );
}


unsigned long XOggObj::DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels)
{
	int current_section;
	long lDecodeSize;
	unsigned long ulSamples;
	short *pSamples;

	unsigned long ulBytesDone = 0;
	while (1)
	{
		lDecodeSize = fn_ov_read(psOggVorbisFile, pDecodeBuffer + ulBytesDone, ulBufferSize - ulBytesDone, 0, 2, 1, &current_section);
		if (lDecodeSize > 0)
		{
			ulBytesDone += lDecodeSize;

			if (ulBytesDone >= ulBufferSize)
				break;
		}
		else
		{
			break;
		}
	}

	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	if (ulChannels == 6)
	{		
		pSamples = (short*)pDecodeBuffer;
		for (ulSamples = 0; ulSamples < (ulBufferSize>>1); ulSamples+=6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			Swap(pSamples[ulSamples+1], pSamples[ulSamples+2]);
			Swap(pSamples[ulSamples+3], pSamples[ulSamples+5]);
			Swap(pSamples[ulSamples+4], pSamples[ulSamples+5]);
		}
	}

	return ulBytesDone;
}

////////////////////////////////////////////////////////////////////////////////////////////
void Swap(short &s1, short &s2)
{
	short sTemp = s1;
	s1 = s2;
	s2 = sTemp;
}

size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread(ptr, size, nmemb, (FILE*)datasource);
}

int ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	return fseek((FILE*)datasource, (long)offset, whence);
}

int ov_close_func(void *datasource)
{
   return fclose((FILE*)datasource);
}

long ov_tell_func(void *datasource)
{
	return ftell((FILE*)datasource);
}

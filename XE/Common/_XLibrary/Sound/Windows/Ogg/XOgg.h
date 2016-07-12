#pragma once
#include "sound/windows/OpenAL/Framework.h"
#include "Vorbis\vorbisfile.h"

// Function pointers
typedef int (*LPOVCLEAR)(OggVorbis_File *vf);
typedef long (*LPOVREAD)(OggVorbis_File *vf,char *buffer,int length,int bigendianp,int word,int sgned,int *bitstream);
typedef ogg_int64_t (*LPOVPCMTOTAL)(OggVorbis_File *vf,int i);
typedef vorbis_info * (*LPOVINFO)(OggVorbis_File *vf,int link);
typedef vorbis_comment * (*LPOVCOMMENT)(OggVorbis_File *vf,int link);
typedef int (*LPOVOPENCALLBACKS)(void *datasource, OggVorbis_File *vf,char *initial, long ibytes, ov_callbacks callbacks);

class XOgg
{
	bool m_bVorbisInit;
	HINSTANCE m_hVorbisFileDLL;	// Ogg Voribis DLL Handle

	void Init();
	void Destroy();
public:
	XOgg() { Init(); }
	virtual ~XOgg() { Destroy(); }

	void Play( void );

	void InitVorbisFile();
	void ShutdownVorbisFile();
	unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);
	void CloseStream( OggVorbis_File &sOggVorbisFile );
};

extern XOgg *OGG;

#define NUMBUFFERS              (4)

class XOggObj
{
	ALuint		    uiBuffers[NUMBUFFERS];
	ALuint		    uiSource;
	ALuint			uiBuffer;
	unsigned long	ulFrequency;
	unsigned long	ulFormat;
	unsigned long	ulChannels;
	unsigned long	ulBufferSize;
	unsigned long	ulBytesWritten;
	char			*pDecodeBuffer;
	OggVorbis_File	sOggVorbisFile;
	int m_Error;

	void Init();
	void Destroy();
public:
	XOggObj( LPCSTR szOgg ) { 
		Init(); 
		if( Load( szOgg ) == FALSE )
			m_Error = 1;
	}
	virtual ~XOggObj() { Destroy(); }
	//
	GET_ACCESSOR( int, Error );
	//
	BOOL Load( LPCSTR szOgg );
	void Play( void );
	void Stop( void );
	void SetRepeat( BOOL bRepeat );
	void SetVolume( float fVolume );
	unsigned long DecodeOggVorbis(OggVorbis_File *psOggVorbisFile, char *pDecodeBuffer, unsigned long ulBufferSize, unsigned long ulChannels);
};

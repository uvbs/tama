#include "stdafx.h"
#include "XOpenAL2.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

bool XOpenAL2::sInit()
{	
	ALFWInit();
	if (!ALFWInitOpenAL()) {
		XLOG_ALERT( "Failed to initialize OpenAL" );
		ALFWShutdown();
		return false;
	}
	return true;
}

void XOpenAL2::sDestroy()
{
	ALFWShutdownOpenAL();
	ALFWShutdown();
}
//////////////////////////////////////////////////////////////////////////
bool XOALDat::Load( const char* cFile )
{
	// Create some OpenAL Buffer Objects
	alGenBuffers( 1, &m_alBuffer );
	XBREAK( m_alBuffer == 0 );
	// Load Wave file into OpenAL Buffer
	if( !ALFWLoadWaveToBuffer( cFile, m_alBuffer ) ) {
		return false;
	}
	alGetError(); // Clear the error
	const auto error = alGetError();
	return ( XASSERT(error == AL_NO_ERROR) );
}

void XOALDat::Destroy()
{
	// Delete the Buffers
	alDeleteBuffers( 1, &m_alBuffer );
	m_alBuffer = 0;
}

//////////////////////////////////////////////////////////////////////////
void XOALStream::Destroy()
{
	// Delete the Sources
	alDeleteSources( 1, &source );
}

bool XOALStream::Load( const char* cFile )
{
	m_spOALDat = XOpenAL2::Load( cFile );
	if( XBREAK(m_spOALDat == nullptr) ) {
		return false;
	}

	return true;
}


XSndStream sndStrm = SOUNDMNG->PlaySound( "butt_click.wav" );
m_sndStrm = sndStrm;

{
	SetVolume( sndStrm, 0.5f );
}

XSoundMng::Process()
{
	for( stream : m_ listStream ) {
		if( stream.시간이 오래된건가 )
			삭제( stream );
	}
}

XStream XSoundMng::PlaySound( const char* cKey )
{
	XSndDat exist = Find( cKey );
	if( exist == nullptr ) {
		exist.Load( cKey );
		m_listDat.Add( dat );
	}
	XStream stream( exist );
	m_listStream.Add( stream );
	return stream;
}

//////////////////////////////////////////////////////////////////////////
void XOALObj::Destroy()
{
	// Delete the Sources
    alDeleteSources(1, &source);
	// Delete the Buffers
    alDeleteBuffers(1, &buffer);
//	if( data )
//		free( data );	// 애플 예제엔 이게 없는데 이거 없으면 릭 난다 ㅅㅂ애플
}

BOOL XOALObj::Load( LPCSTR szFilename )
{
//	ALenum			error;
	// Create some OpenAL Buffer Objects
	alGenBuffers(1, &buffer);
	
	// Load Wave file into OpenAL Buffer
	if( !ALFWLoadWaveToBuffer( szFilename, buffer ) )
	{
//		XLOG_ALERT( "Failed to load %s", szFilename );
		return FALSE;
	}

	// Create some OpenAL Source Objects
	alGenSources(1, &source);

	{
		ALenum error = AL_NO_ERROR;
		alGetError(); // Clear the error
		
		// Turn Looping ON
//			alSourcei(source, AL_LOOPING, AL_TRUE);
		alSourcei(source, AL_LOOPING, AL_FALSE);
		
//		CGPoint					sourcePos;
//		sourcePos = CGPointMake(0., -70.);
		// Set Source Position
//		float sourcePosAL[] = {sourcePos.x, 25.0, sourcePos.y};
//		alSourcefv(source, AL_POSITION, sourcePosAL);
		
		// Set Source Reference Distance
//		alSourcef(source, AL_REFERENCE_DISTANCE, 50.0f);
		
		// attach OpenAL Buffer to OpenAL Source
		alSourcei(source, AL_BUFFER, buffer);
		
		if((error = alGetError()) != AL_NO_ERROR) {
//			NSLog(@"Error attaching buffer to source: %x\n", error);
//			exit(1);
			XBREAK(1);
		}	
	}
	return TRUE;
}

int XOALObj::CreateSouce( int _alBuffer )
{
	ALuint alBuffer = (ALuint)_alBuffer;
	ALuint alSrc = 0;
	// Create some OpenAL Source Objects
	alGenSources( 1, &alSrc );
	ALenum error = AL_NO_ERROR;
	alGetError(); // Clear the error
	// Turn Looping ON
	alSourcei( alSrc, AL_LOOPING, AL_FALSE );
	// attach OpenAL Buffer to OpenAL Source
	alSourcei( alSrc, AL_BUFFER, alBuffer );

	if( ( error = alGetError() ) != AL_NO_ERROR ) {
		XBREAK( 1 );
	}
	return alSrc;
}

void XOALObj::SetRepeat( BOOL bRepeat )
{
	alSourcei(source, AL_LOOPING, bRepeat );
}

void XOALObj::SetVolume( float fVolume )
{
	alSourcef(source, AL_GAIN, fVolume );
}

void XOALObj::Play2( int alSrc )
{
	alSourcePlay( alSrc );
	auto error = alGetError();
	if( error != AL_NO_ERROR ) {
		XBREAKF( 1, "alSourcePlay error code=%d", (int)error );
	} else {
	}
}


void XOALObj::Play()
{
	ALenum error;
	
	// Begin playing our source file
	alSourcePlay(source);
	if((error = alGetError()) != AL_NO_ERROR) {
//		NSLog(@"error starting source: %x\n", error);
//		XALERT( "alSourcePlay error code=%d", (int)error );
		CONSOLE( "alSourcePlay error code=%d", (int)error );
	} else {
		// Mark our state as playing (the view looks at this)
	}
}

void XOALObj::Stop()
{
	ALenum error;
	
	// Stop playing our source file
	alSourceStop(source);
	if((error = alGetError()) != AL_NO_ERROR) {
//		NSLog(@"error stopping source: %x\n", error);
		CONSOLE( "error stopping source: %x\n", (int)error );
	} else {
		// Mark our state as not playing (the view looks at this)
	}
}

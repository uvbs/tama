/*
 *  OpenAL.cpp
 *  Test1
 *
 *  Created by xuzhu on 10. 6. 7..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "stdafx.h"
#include "XOpenAL.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

BOOL XOpenAL::Init( void )
{	
	ALFWInit();
	if (!ALFWInitOpenAL()) {
		XBREAKF( 1, "Failed to initialize OpenAL" );
		ALFWShutdown();
		return FALSE;
	}
	return TRUE;
}

void XOpenAL::Destroy( void )
{
	ALFWShutdownOpenAL();
	ALFWShutdown();
}

//////////////////////////////////////////////////////////////////////////
void XOALObj::Destroy( void )
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


void XOALObj::Play( void )
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

void XOALObj::Stop( void )
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

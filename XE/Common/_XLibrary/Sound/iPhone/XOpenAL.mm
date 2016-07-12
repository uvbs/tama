/*
 *  OpenAL.cpp
 *  Test1
 *
 *  Created by xuzhu on 10. 6. 7..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <UIKit/UIKit.h>
#include "XOpenAL.h"
#include "MyOpenALSupport.h"


BOOL XOpenAL::Init( void )
{	
	// Create a new OpenAL Device
	// Pass NULL to specify the system’s default output device
	device = alcOpenDevice(NULL);
	if (device != NULL)
	{
		// Create a new OpenAL Context
		// The new context will render to the OpenAL Device just created 
		context = alcCreateContext(device, 0);
		if (context != NULL)
		{
			// Make the new context the Current OpenAL Context
			alcMakeContextCurrent(context);
		}
	}
	// clear any errors
	alGetError();
	return TRUE;
}

void XOpenAL::Destroy( void )
{
    //Release context
    alcDestroyContext(context);
    //Close device
    alcCloseDevice(device);
}

void XOALObj::Destroy( void )
{
	// Delete the Sources
    alDeleteSources(1, &source);
	// Delete the Buffers
    alDeleteBuffers(1, &buffer);
	free( data );	// 애플 예제엔 이게 없는데 이거 없으면 릭 난다 ㅅㅂ애플
}

BOOL XOALObj::Load( LPCTSTR szFilename )
{
	ALenum			error;

	// Create some OpenAL Buffer Objects
	alGenBuffers(1, &buffer);
	if((error = alGetError()) != AL_NO_ERROR) {
		XALERT("Error Generating Buffers: %x", error);
		exit(1);
	}
	
	// Create some OpenAL Source Objects
	alGenSources(1, &source);
	if(alGetError() != AL_NO_ERROR) 
	{
		XALERT("Error generating sources! %x\n", error);
		exit(1);
	}
	{
		ALenum  error = AL_NO_ERROR;
		ALenum  format;
		ALsizei size;
		ALsizei freq;
		
		NSBundle*				bundle = [NSBundle mainBundle];
		
		NSString *strPath = [NSString stringWithUTF8String:szFilename];
		// get some audio data from a wave file
		CFURLRef fileURL = (__bridge_retained CFURLRef)[NSURL fileURLWithPath:strPath];
//		CFURLRef fileURL = (CFURLRef)[[NSURL fileURLWithPath:[bundle pathForResource:@"Jump" ofType:@"wav"]] retain];
		
		if (fileURL)
		{	
			data = MyGetOpenALAudioData(fileURL, &size, &format, &freq);
			CFRelease(fileURL);
			
			if((error = alGetError()) != AL_NO_ERROR) {
				XALERT("error loading sound: %x\n", error);
				exit(1);
			}
			
			// use the static buffer data API
			alBufferDataStaticProc(buffer, format, data, size, freq);
			
			if((error = alGetError()) != AL_NO_ERROR) {
				XALERT("error attaching audio to buffer: %x\n", error);
			}		
		}
		else
			XALERT("Could not find file!\n");
	}
	
	{
		ALenum error = AL_NO_ERROR;
		alGetError(); // Clear the error
		
		// Turn Looping ON
		//			alSourcei(source, AL_LOOPING, AL_TRUE);
		alSourcei(source, AL_LOOPING, AL_FALSE);
		
		CGPoint					sourcePos;
		sourcePos = CGPointMake(0., -70.);
		// Set Source Position
		float sourcePosAL[] = {sourcePos.x, 25.0, sourcePos.y};
		alSourcefv(source, AL_POSITION, sourcePosAL);
		
		// Set Source Reference Distance
		alSourcef(source, AL_REFERENCE_DISTANCE, 50.0f);
		
		// attach OpenAL Buffer to OpenAL Source
		alSourcei(source, AL_BUFFER, buffer);
		
		if((error = alGetError()) != AL_NO_ERROR) {
			XALERT("Error attaching buffer to source: %x\n", error);
			exit(1);
		}	
	}
	return TRUE;
}

void XOALObj::SetRepeat( BOOL bRepeat )
{
	alSourcei(source, AL_LOOPING, bRepeat );
}

void XOALObj::SetVolume( float fVolume )
{
	alSourcef(source, AL_GAIN, fVolume );
}

void XOALObj::Play( void )
{
	ALenum error;
	
	// Begin playing our source file
	alSourcePlay(source);
	if((error = alGetError()) != AL_NO_ERROR) {
		XALERT("error starting source: %x\n", error);
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
		XALERT("error stopping source: %x\n", error);
	} else {
		// Mark our state as not playing (the view looks at this)
	}
}

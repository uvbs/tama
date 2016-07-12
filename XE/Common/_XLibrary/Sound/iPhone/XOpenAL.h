/*
 *  OpenAL.h
 *  Test1
 *
 *  Created by xuzhu on 10. 6. 7..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __XOPENAL_H__
#define __XOPENAL_H__

#ifdef _VER_IOS
#import <OpenAL/al.h>
#import <OpenAL/alc.h>

class XOpenAL
{
	ALCcontext*				context;
	ALCdevice*				device;
public:
	XOpenAL() { Init(); }
	~XOpenAL() { Destroy(); }
	
	BOOL Init( void );	
	void Destroy( void );
};

//#define LPCTSTR const char *


class XOALObj
{
	ALuint					source;
	ALuint					buffer;
	void*					data;
public:
	XOALObj() {}
	XOALObj( LPCTSTR szFilename ) { Load(szFilename); }
	~XOALObj() { Destroy(); }
	
	void Destroy( void );
	BOOL Load( LPCTSTR szFilename );
	void Play( void );
	void Stop( void );
	void SetRepeat( BOOL bRepeat );
	void SetVolume( float fVolume );
};
#endif // _VER_IOS

#endif // __XOPENAL_H__

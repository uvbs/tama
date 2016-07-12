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
#include "sound/windows/OpenAL/Framework.h"

class XOpenAL
{
public:
	XOpenAL() { Init(); }
	~XOpenAL() { Destroy(); }
	
	BOOL Init( void );	
	void Destroy( void );
};
//////////////////////////////////////////////////////////////////////////
class XOALObj
{
	ALuint					source;
	ALuint					buffer;
//	void*					data;
	void Init() {
		source = 0;
		buffer = 0;
//		data = NULL;
	}
public:
	XOALObj() { 	Init();	}
//	XOALObj( LPCSTR szFilename ) { Init(); }
	virtual ~XOALObj() { Destroy(); }
	///< 
	bool IsLoaded() const {
		return buffer != 0;
	}
	void Destroy( void );
	BOOL Load( LPCSTR szFilename );
	void Play( void );
	void Stop( void );
	void SetRepeat( BOOL bRepeat );
	void SetVolume( float fVolume );
	static void Play2( int alSrc );
	static int CreateSouce( int _alBuffer );
	int GetalBuffer() const {
		return (int)buffer;
	}
};

#endif // __XOPENAL_H__

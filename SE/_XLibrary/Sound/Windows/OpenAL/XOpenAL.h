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
#include "OpenAL\\Framework.h"

class XOpenAL
{
public:
	XOpenAL() { Init(); }
	~XOpenAL() { Destroy(); }
	
	BOOL Init();	
	void Destroy();
};

class XOALObj
{
	ALuint					source;
	ALuint					buffer;
//	void*					data;
	void Init() {
		source = 0;
		buffer = 0;
//		data = nullptr;
	}
public:
	XOALObj() { 	Init();	}
	XOALObj( LPCSTR szFilename ) { Init(); Load(szFilename); }
	virtual ~XOALObj() { Destroy(); }
	
	void Destroy();
	BOOL Load( LPCSTR szFilename );
	void Play();
	void Stop();
	void SetRepeat( BOOL bRepeat );
	void SetVolume( float fVolume );
};

#endif // __XOPENAL_H__

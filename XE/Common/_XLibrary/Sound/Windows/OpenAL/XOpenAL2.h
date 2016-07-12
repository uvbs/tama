#pragma once

#include "sound/windows/OpenAL/Framework.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/19 15:17
*****************************************************************/
class XOpenAL2
{
	XOpenAL2();
public:
	~XOpenAL2() {}
	//
private:
	static bool sInit();
	static void sDestroy();
}; // class XOpenAL2

class XOALDat;
typedef std::shared_ptr<XOALDat> XSPOALDat;
typedef std::shared_ptr<const XOALDat> XSPOALDatConst;

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/19 15:26
*****************************************************************/
class XOALDat
{
public:
	XOALDat() { Init(); }
	virtual ~XOALDat() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( ALuint, alBuffer );
	// public member
	bool Load( const char* cFile );
	bool IsLoaded() const {
		return m_alBuffer != 0;
	}
private:
	// private member
	ALuint m_alBuffer = 0;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XOALDat

//////////////////////////////////////////////////////////////////////////
/****************************************************************
* @brief 소리 스트림. play가 호출될때마다 생성된다. 플레이중인 스트림을 제어하려면 이 객체를 보관한다.
* @author xuzhu
* @date	2016/04/19 15:32
*****************************************************************/
class XOALStream
{
public:
	XOALStream( XSndDat spDat )
		: XStream( spDat ) {
		Init(); 
		m_spOALDat = (XOALDat)spDat;
	}
	virtual ~XOALStream() { Destroy(); }
	// get/setter
	// public member
//	bool Load( const char* cFile );
	void SetVolume( float vol ) {
		m_Volume = vol;
		alSourcePlay( m_alSrc, AL_GAIN, vol );
	}
	void SetRepeat( bool bRepeat ) {
		m_bRepeat = bRepeat;
		alSourcei( m_alSrc, AL_LOOPING, (bRepeat)? 1 : 0 );
	}
	void Stop() {
		// Stop playing our source file
		alSourceStop( m_alSrc );
		const auto err = alGetError();

	}
private:
	// private member
	XSPOALDat m_spOALDat;
	ALuint m_alSrc = 0;
	float m_Volume = 1.f;
	bool m_bRepeat = false;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XOALObj2

////////////////////////////////////////////////////////////////
XOALStream::XOALStream()
{
	Init();
}

void XOALStream::Destroy()
{
}


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



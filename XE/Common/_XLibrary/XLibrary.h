#pragma once
#include <stdio.h>
//#include "XSystem.h"
#include "XLock.h"

// XE엔진중 최하위 레이어에 위치한 저수준 라이브러리.
// XLibrary는 싱글톤이어야 한다.
// XLibrary내의 모든 함수는 스레드 세이프 하다.
class XLibrary //: public XLock
{
public:
private:
//	_XNoRecursiveInstance _m_nrInstance;			// 상호호출 방지 인스턴스.
//	static XSharedObj<DWORD> s_shoNextForRandom;	// 랜덤 시드.
//	DWORD m_idMainThread;		// 메인스레드 아이디
	bool m_bEntered = false;		// 실수로 Console()에서 중복진입을 막기위한 장치
	XTHREAD_ID m_idMainThread;
	XRandom2 m_Random;
	void Init() {
//		m_idMainThread = 0;
	}
	void Destroy();	
// 	static DWORD sRand();
// 	// 32비트 랜덤값을 생성
// 	static ID sGenerateID();
protected:
	// 앱에서 DlgConsole에 메시지를 뿌릴수 있도록한다. __xLog류 함수 호출금지
	virtual void ConsoleMessage( LPCTSTR szMsg ) = 0;
public:
	XLibrary();
	virtual ~XLibrary() { Destroy(); }
	//
	GET_READONLY_ACCESSOR( XTHREAD_ID, idMainThread );
	//
	void Consolef( LPCTSTR szFormat, ... );
	void Console( LPCTSTR szMsg );
	//////////////////////////////////////////////////////////////////////////
	// 스레드 세이프한 버전의 랜덤 함수들.
// 	DWORD xRandom( DWORD nRange );
// 	DWORD xRandom( DWORD min, DWORD max );
// 	// 최대 256.0까지 range를 가질수 있음
// 	float xRandomF( float fRange );
// 	float xRandomF( float start, float end );
// 	void xSRand( DWORD seed );
// 	DWORD xGetSeed();
	//////////////////////////////////////////////////////////////////////////
};


extern XLibrary *XLIB;


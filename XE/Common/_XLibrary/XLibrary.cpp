#include "stdafx.h"
#include "XLibrary.h"
#include "etc/Debug.h"
#include "etc/xUtil.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


// 여차하면 이게 #define XLIB		GetXLibrary() 가 될수도 있음.
XLibrary *XLIB = NULL;

//XSharedObj<DWORD> XLibrary::s_shoNextForRandom;	// 랜덤 시드.

//////////////////////////////////////////////////////////////////////////
// static
// ID XLibrary::sGenerateID() 
// {
// 	DWORD idRand;
// 	do {
// 		idRand = sRand();		// 공유되는 자원은 xRand내부의 seed값인데 랜덤쪽에서 락되고 있음.
// 		// 0은 생성하지 못하도록
// 	} while( idRand == 0 );
// 	return idRand;
// }
// 
// DWORD XLibrary::sRand( void ) 
// {
// 	// Lock
// 	DWORD* ps_next = &s_shoNextForRandom.GetSharedObj();
// 	//
// 	DWORD next = 0;
// 	int num = random( 16 );	// m_shoNextForRandom이걸로 스레드세이프해질려나.
// 	next |= num;
// 	for( int i = 0; i < 7; ++i ) {
// 		next <<= 4;
// 		int num = random(16);	// m_shoNextForRandom이걸로 스레드세이프해질려나.
// 		next |= num;
// 	}
// //	DWORD next = *ps_next * 1103515245 + 12345;
// //	*ps_next = next;
// 	// Unlock
// 	s_shoNextForRandom.ReleaseSharedObj();
// 	return next;
// }
// DWORD XLibrary::xRandom( DWORD nRange )
// {
// 	if( XBREAK(nRange == 0) )	return 0;
// 	return xRand() % nRange;
// }
// DWORD XLibrary::xRandom( DWORD min, DWORD max )
// {
// 	if( max < min )
// 		SWAP( min, max );
// 	return min + xRandom( max - min );
// }
// // 최대 256.0까지 range를 가질수 있음
// float XLibrary::xRandomF( float fRange )
// {
// 	return (float)(((double)xRand() / 0xFFFFFFFF) * fRange);
// }
// float XLibrary::xRandomF( float start, float end ) 
// {
// 	float f = start + xRandomF( end-start );
// 	return f;
// }
// static
//////////////////////////////////////////////////////////////////////////


XLibrary::XLibrary() 
{ 
	Init();
	m_idMainThread = GET_CURR_THREAD_ID();
#ifdef WIN32
//	m_idMainThread = GetCurrentThreadId();
#endif
}

void XLibrary::Destroy()
{
}

void XLibrary::Consolef( LPCTSTR szFormat, ... ) 
{
	if( XBREAK( m_bEntered ) )		// 중복호출됨. 잘못된 호출
		return;
	m_bEntered = true;
//	XLOCK_OBJ;
// 	_XNoRecursiveStack _nrStack( &_m_nrInstance );
// 	// ConsoleMessage내부에서 재호출을 금지시킴
// 	if( _nrStack.IsLock() == TRUE ) {
// 		XBREAK_POINT();		// 재호출은 이제 완전 금지라서 브레이크 걸어둠.
// 		return;
// 	}
// 	XBREAK( _nrStack.IsLock() == TRUE );	
// 	_nrStack.Lock();
	//
	TCHAR buff[1024*10];
	va_list	vl;
	va_start(vl, szFormat);

	_vstprintf_s(buff, szFormat, vl);
	va_end(vl);
	ConsoleMessage( buff );		// virtual
	m_bEntered = false;
}

void XLibrary::Console( LPCTSTR szMsg )
{
	if( XBREAK(m_bEntered) )		// 중복호출됨. 잘못된 호출
		return;
	m_bEntered = true;
	//
	ConsoleMessage( szMsg );		// virtual
	//
	m_bEntered = false;
}

// void XLibrary::ConsoleMessage( LPCTSTR szMsg )
// {
// 	//__xLog( XLOGTYPE_LOG, szMsg );	<< 이렇게 쓰지 말것 szMsg에 20%증가합니다. 이런거 오면 뻑남.
// 	__xLog( XLOGTYPE_LOG, _T("%s"), szMsg );
// }

// void XLibrary::xSRand( DWORD seed ) 
// {
// 	// seed값이 있는 실제 데이타 포인터
// 	DWORD *pseed = &s_shoNextForRandom.GetSharedObj();
// 	*pseed = seed;
// 	s_shoNextForRandom.ReleaseSharedObj();
// }
// 
// DWORD XLibrary::xGetSeed() 
// {
// 	DWORD *pseed = &s_shoNextForRandom.GetSharedObj();
// 	s_shoNextForRandom.ReleaseSharedObj();
// 	return *pseed;
// }
// 

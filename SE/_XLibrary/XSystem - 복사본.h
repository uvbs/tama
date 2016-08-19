#pragma once
#include "etc/Types.h"
//#include "XLibrary.h"

#define XUNIX_PATH( STR )		XSYSTEM::strReplace( STR, '\\', '/' )
#ifdef WIN32
  #define XWIN32_PATH( STR )	XSYSTEM::strReplace( STR, '/', '\\' )
#else
  #define XWIN32_PATH( STR ) (0)
#endif
#ifdef WIN32
  #define XPLATFORM_PATH( STR )	XSYSTEM::strReplace( STR, '/', '\\' )
#else
  #define XPLATFORM_PATH( STR )	XSYSTEM::strReplace( STR, '\\', '/' )
#endif


namespace XSYSTEM {
	BOOL MakeDir( LPCTSTR szPath );
	BOOL MakeRecursiveDir( LPCTSTR szPath );
	BOOL RenameFile( const char* cPathOld, const char* cPathNew );
	BOOL RemoveFile( const char* cPath );
	void strReplace( TCHAR *szBuff, TCHAR szSrc, TCHAR szDst );
	unsigned long long CalcCheckSum( LPCTSTR szFile );
	void xSleep( unsigned int sec );
	void SetAutoSleepMode( BOOL bFlag );
	void RestoreAutoSleepMode( void );
	void xDebugBreak( void );
//	BOOL CheckCDMA( void );		// JniHelper.cpp가 프로젝트 독립된 라이브러리로 떨어져나오기 전까진 엔진으로 들어올수 없다.
//	BOOL CheckWiFi( void );
	DWORD GetFileSize( LPCTSTR szFullpath );
	float StrToNumber( const char *cStr );
#ifdef WIN32
	void strReplace( char *cBuff, char cSrc, char cDst );
	BOOL MakeDir( const char* cPath );
	BOOL MakeRecursiveDir( const char* cPath );
	DWORD GetFileSize( const char *cFullpath );
#endif
	int CreateCompressMem( BYTE** ppOutDst, DWORD *pOutSizeDst, BYTE *pSrc, DWORD sizeSrc );
	int UnCompressMem( BYTE **ppOutDst, DWORD sizeDst,BYTE *pSrc, DWORD sizeSrc );
};

/**
 상호호출을 막아주는 객체
 ex)
 _XNoRecursiveInstance nrFunc1;		// func1용 상호호출금지 객체
 // 상호호출할지도 모르는 함수1
 func1()
 {
	_XNoRecuciveStack nrStack( &nrFunc1 )		// 이블럭을 빠져나갈때 Lock상태를 이전상태로 되돌려 놓는다.
	if( nrFunc1.IsLock() )		// 이미 락이 걸린상태로 왔으면
		return;			// 그냥 리턴
	nrFunc1.Lock();				// 아니면 여기서 락을 건다.

	test1()
 }
 
 _XNoRecursiveInstance nrTest1;		// test1용 상호호출금지 객체
  // 상호호출할지도 모르는 함수2
 test1()
 {
	_XNoRecuciveStack nrStack( &nrTest1 )
	if( nrTest1.IsLock() )
		return;
	nrTest1.Lock();

	func1();
 }
 */
class _XNoRecursiveInstance
{
	int m_refCnt;
	BOOL m_Lock;
public:
	_XNoRecursiveInstance() { 
		m_Lock = FALSE; 
		m_refCnt = 0;
	}
	virtual ~_XNoRecursiveInstance();
	BOOL IsLock( void ) { return m_Lock; }
	SET_ACCESSOR( BOOL, Lock );
	void SetRetain( void ) { ++m_refCnt; }
	void SetRelease( void ) { --m_refCnt; }
};

/**
 이 클래스로 객체를 함수 첫시작부에 만들면 
 블럭을 빠져나갈때 자동으로 Lock상태를 이전상태로 되돌려 놓는다.
*/
class _XNoRecursiveStack
{
	_XNoRecursiveInstance *m_pNoRecursiveInstance;
	BOOL m_oldLock;
public:
	_XNoRecursiveStack( _XNoRecursiveInstance *pNoRecursiveInstance ) { 
		m_pNoRecursiveInstance = pNoRecursiveInstance;
		m_oldLock = pNoRecursiveInstance->IsLock();
		pNoRecursiveInstance->SetRetain();
	}
	virtual ~_XNoRecursiveStack() { 
		m_pNoRecursiveInstance->SetLock( m_oldLock );
		m_pNoRecursiveInstance->SetRelease();
	}
	void Lock( void ) {
		m_pNoRecursiveInstance->SetLock( TRUE );
	}
	BOOL IsLock( void ) {
		return m_pNoRecursiveInstance->IsLock();
	}

};


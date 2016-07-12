#pragma once
#include "etc/Types.h"

// #define XUNIX_PATH( STR )		XSYSTEM::strReplace( STR, '\\', '/' )
// #define XWIN32_PATH( STR ) XSYSTEM::strReplace( STR, '\\', '/' )
// #define XPLATFORM_PATH( STR )	XSYSTEM::strReplace( STR, '\\', '/' )
// #define PATH_SLASH '/'

//#define XBREAK_POINT()		XSYSTEM::xDebugBreak()
// inline volatile void voidFunc() {}
// //#define XBREAK_POINT()		XSYSTEM::xDebugBreak()
// // 함수로하지 않고 매크로로 만든이유는 브레이크가 걸렸을때 해당 코드화면에서 바로 정지하도록 하기 위함.
// #if defined(WIN32)
//   #if defined(_XNOT_BREAK)
// 		#define XBREAK_POINT()		voidFunc()	// assert매크로에서 브레이크의 리턴값으로 분기가 잘못되는걸 막기위해 void로 함.
//   #else
// 	#ifdef _DEBUG
// 		#if defined(_AFX) || defined(_AFXDLL)
// 			#define XBREAK_POINT() 		AfxDebugBreak()
// 		#else
// 			#define XBREAK_POINT() 		DebugBreak()
// 		#endif
// 	#else
// 		#define XBREAK_POINT() 		voidFunc()			// 이걸따로둔 이유. release에선 AfxDebugBreak/DebugBreak함수가 아예 없는함수인것처럼 동작한다. 만약 (BREAK_POINT(), XXX)와 같은식으로 썼을때 ( , xxx)로 변환되어 컴팔에러남. 그래서 빈 함수를 만듬.
// 	#endif // _DEBUG
//   #endif
// #elif defined(_VER_IOS)
// 	#define XBREAK_POINT()		XSYSTEM::xDebugBreak()
// #else
// 	#define XBREAK_POINT()		voidFunc()	
// #endif 


namespace XSYSTEM {
	BOOL MakeDir( LPCTSTR szPath );
	BOOL MakeRecursiveDir( LPCTSTR szPath );
	BOOL RenameFile( const char* cPathOld, const char* cPathNew );
	BOOL RemoveFile( const char* cPath );
	inline BOOL RemoveFile( const std::string& str ) {
		return RemoveFile( str.c_str() );
	}
	void strReplace( TCHAR *szBuff, TCHAR szSrc, TCHAR szDst );
	void strReplace( _tstring& strSrc, TCHAR tcSrc, TCHAR tcDst );
	unsigned long long CalcCheckSum( LPCTSTR szFile );
	void xSleep( unsigned int sec );
	void SetAutoSleepMode( BOOL bFlag );
	void RestoreAutoSleepMode( void );
	XE::xtAlertRet xMessageBox( LPCTSTR szMsg, XE::xtAlert type );
// #ifdef WIN32
// 	inline void xDebugBreak( void ) {}
// #else
#if defined(_VER_IOS)	// 
	void _xDebugBreak( void );
#endif
//#endif
//	BOOL CheckCDMA( void );		// JniHelper.cpp가 프로젝트 독립된 라이브러리로 떨어져나오기 전까진 엔진으로 들어올수 없다.
//	BOOL CheckWiFi( void );
	DWORD GetFileSize( LPCTSTR szFullpath );
	float StrToNumber( const char *cStr );
#ifdef WIN32
	void strReplace( char *cBuff, char cSrc, char cDst );
	void strReplace( std::string& strSrc, char cSrc, char cDst );
	BOOL MakeDir( const char* cPath );
	BOOL MakeRecursiveDir( const char* cPath );
	DWORD GetFileSize( const char *cFullpath );
	XE::xtERROR IsReadOnly( LPCTSTR szFilePath );
#endif
	int CreateCompressMem( BYTE** ppOutDst, DWORD *pOutSizeDst, const BYTE *pSrc, const DWORD sizeSrc );
	inline int CreateCompressMem( BYTE** ppOutDst, int *pOutSizeDst, const BYTE *pSrc, const int sizeSrc ) {
		return CreateCompressMem( ppOutDst, (DWORD*)pOutSizeDst, pSrc, (DWORD)sizeSrc );
	}
	int UnCompressMem( BYTE** ppOutDst, DWORD sizeDst,BYTE *pSrc, DWORD sizeSrc );
	int UnCompressMem( BYTE* pOutDst, const int sizeOutBuff
										, const DWORD sizeUncompressed
										, const BYTE *pSrc, const DWORD sizeSrc );
	template<int _Size>
	inline int UnCompressMem( BYTE (&pOutDst)[_Size]
													, const DWORD sizeUncompressed
													, const BYTE *pSrc, const DWORD sizeSrc ) {
		return UnCompressMem( pOutDst, _Size, sizeUncompressed, pSrc, sizeSrc );
	}
	BOOL IsAlphabet( LPCTSTR szStr );
	BOOL IsAlphabet( TCHAR c );
	BOOL IsNumberChar( TCHAR c );
	BOOL CheckCDMA( void );
	BOOL CheckWiFi( void );
	XE::xtDOW GetDayOfWeek( void );
	int GetHour( void );
	void GetHourMinSec( int *pHourOut, int *pMinOut, int *pSecOut );
	void GetHourMinSec( xSec secTotal, int *pHourOut, int *pMinOut, int *pSecOut );
	void GetDayHourMinSec( xSec secTotal, int *pDayOut, int *pHourOut, int *pMinOut, int *pSecOut );
	_tstring GetstrTimeHMS( xSec secTotal );
	int GetsecToday();
	int GetDay( void );
//	int GetDistDay( int dayStart, int dayEnd );
	void GetYearMonthDay( int *pYearOut, int *pMonthOut, int *pDayOut );
	int GetYMDToDays( int year, int month, int day );
  char* base64_encode( const void* buf, size_t size );
  void* base64_decode(const char* s, size_t* data_len_ptr);
#ifdef WIN32
	int GetnumChar( LPCTSTR szText );
#endif // WIN32
	int GetnumChar( const char* utf8Text );
	BOOL CopyFileX( LPCTSTR szSrcPath, LPCTSTR szDstPath );
	BOOL DeleteFile( const char* cFilePath );
#ifdef WIN32
	BOOL CopyFileX( const char* cSrcPath, const char* cDstPath );
#endif
	_tstring GetDeviceModel();
}; // xsystem

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


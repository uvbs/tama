#include "stdafx.h"
#include "path.h"
#include "xUtil.h"
#include "debug.h"
#ifdef _CLIENT
#include <fstream>
#include "zip_support/unzip.h"
using namespace cocos2d;
#ifndef WIN32
#include "xString.h"
#endif
#endif //_CLIENT
#include "etc/Timer.h"
#include "etc/xMath.h"	// GetIntersect류 함수때문에 포함함.

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#if defined(_VER_ANDROID) || defined(_VER_IOS)
#ifndef _XUTF8
#error "안드로이드와 ios는 XUTF8을 붙여야 한다."
#endif
#endif

using namespace std;

namespace XE
{
	XRandom2 RANDOM_OBJ;		// 전역 랜덤객체
}

////////////////////////////////////////////////////////////////
//static DWORD s_next;
DWORD xRand()
{
	return XE::RANDOM_OBJ.GetRand();
}
DWORD xRandom( DWORD nRange )
{
	return XE::RANDOM_OBJ.GetRandom( nRange );
}
int xRandom( int min, int max )
{
	return XE::RANDOM_OBJ.GetRandom( min, max );
}
// 최대 256.0까지 range를 가질수 있음
float xRandomF( float fRange )
{
	return XE::RANDOM_OBJ.GetRandomF( fRange );
}
float xRandomF( float start, float end ) 
{
	return XE::RANDOM_OBJ.GetRandomF( start, end );
}
// void xSRand64( XINT64 seed )
// {
// 	XE::RANDOM_OBJ.SetSeed( seed );
// }
void xSRand( DWORD seed )
{
	XE::RANDOM_OBJ.SetSeed( seed );
}
// DWORD xGetSeed()
// {
// 	return XE::RANDOM_OBJ.GetSeed();
// }

int random( short min, short max )
{
	if( max < min )
		std::swap( min, max );
	return min + random( max - min + 1 );
}


DWORD xGenerateSeed() {
	return XE::RANDOM_OBJ.GenerateSeed();
}
// XUINT64 xGenerateSeed64() {
// 	return XE::RANDOM_OBJ.GenerateSeed64();
// }

////////////////////////////////////////////////////////////////
void XFps::Process( int idx )
{
	XAUTO_LOCK2( this );
	if( m_dwStart == 0 )
		m_dwStart = GetTickCount();
//	++m_cntFrame;
	++m_aryCntFrame[idx];
	const auto dwPass = GetTickCount() - m_dwStart;
	if( dwPass > 1000 ) {
//		m_Fps = (int)(m_cntFrame / (dwPass / 1000.f));
		// 1초마다 정산.
		for( int i = 0; i < 3; ++i ) {
			m_aryFps[i] = (int)(m_aryCntFrame[i] / (dwPass / 1000.f));
			m_aryCntFrame[i] = 0;
		}
//		m_cntFrame = 0;
		m_dwStart = GetTickCount();
	}
}

void XFps::AddCnt( int size )
{
	XAUTO_LOCK2( this );
	if( m_dwStart == 0 )
		m_dwStart = GetTickCount();
	m_aryCntFrame[0] += size;
	const auto dwPass = GetTickCount() - m_dwStart;
	if( dwPass > 1000 ) {
		m_aryFps[0] = (int)( m_aryCntFrame[0] / ( dwPass / 1000.f ) );
		m_aryCntFrame[0] = 0;
		m_dwStart = GetTickCount();
	}
}


int CalcFPS()
{
	static float dt = 0;
	static int frame = 0;
	static int frameRate = 0;
	static DWORD dwPrev = GetTickCount();
	DWORD dwTick = GetTickCount();
	float t = (float)(dwTick - dwPrev) / 1000;
	dwPrev = dwTick;
	dt += t;
	frame ++;
	if( dt > 1.0f )
	{
		frameRate = (int)(frame / dt);
		frame = 0;
		dt = 0;
	}

	return frameRate;
}

////////////////////////////////////////////////////////////
// 파일타이틀과 확장자를 리턴  ex) test.wri
static TCHAR _szTempBuff[2048];
#ifdef WIN32
static char _cTempBuff[2048];
#endif
namespace XE
{
	//	_XNoRecursiveInstance _nr__xLog;		// 상호호출 방지 인스턴스.
	// <-구체적으로 어떤자원에 대한 락이라는건지 모호함.
	// 현재로선 debug.cpp의 모든 함수의 진입과 탈출에 대해서 사용.
	static XLock x_Lock;
}

LPCTSTR XE::Format( LPCTSTR szFormat, ... )
{
	TCHAR szTemp[ 2048 ];
	XLockObj lockObj( &XE::x_Lock, __TFUNC__ );		// 다른 스레드에서 로그 쓰려고 하면 잠시 기다리게 함.
	va_list vl;
// 	try {
		va_start( vl, szFormat );
		_vstprintf_s( szTemp, szFormat, vl );
// 	}
// 	catch( std::exception& e ) {
// 		XBREAKF(1, "exception: %s ", e.what() );
// 	}
	int len = _tcslen( szTemp );
	int size = (sizeof(szTemp) / sizeof(szTemp[0]));
	XBREAK( len >= size );
	va_end( vl );
	_tcscpy_s( _szTempBuff, szTemp );		// 안전을 위해
	return _szTempBuff;
}
#ifdef WIN32
const char* XE::Format( const char *cFormat, ... )
{
	char cTemp[ 2048 ];
	va_list vl;
	va_start( vl, cFormat );
	vsprintf_s( cTemp, cFormat, vl );
	va_end( vl );
	strcpy_s( _cTempBuff, cTemp );
	return _cTempBuff;
}
#endif // win32

LPCTSTR XE::StrTrimLeft( LPCTSTR szStr, int n )
{
	XBREAK( n+2 >= MAX_PATH );
	_tcsncpy_s( _szTempBuff, szStr, n );
	return _szTempBuff;
}

// strFullPath에서 패스부분만 szBuff로 옮긴다 끝에 \도 살린다.
LPCTSTR XE::GetFilePath( TCHAR *pOut, int sizeOut, LPCTSTR _szSrc, TCHAR tSlash )
{
	if( !_szSrc )
		return _T("");
	TCHAR szSrc[ 2048 ];
	_tcscpy_s( szSrc, _szSrc );
	XBREAK( _tcslen(szSrc) >= MAX_PATH );
	_tcscpy_s( pOut, sizeOut, szSrc );
	LPTSTR p = _tcsrchr( pOut, '/' );
	if( p == nullptr ) {
		p = _tcsrchr( pOut, '/' );
		if( p == nullptr )
			return _T("");
	}
	*(p+1) = 0;		// \뒤에 널이 들어가도록
	return pOut;
}
LPCTSTR XE::GetFilePath( LPCTSTR szSrc, TCHAR tSlash )
{
	return GetFilePath( _szTempBuff, sizeof(_szTempBuff) / sizeof(_szTempBuff[0]), szSrc, tSlash );
}
#ifdef WIN32
const char * XE::GetFilePath( char *pOut, int sizeOut, const char* _cSrc, char cSlash )
{
	if( !_cSrc )
		return "";
	char cSrc[2048];
	strcpy_s( cSrc, _cSrc );
	XBREAK( strlen(cSrc) >= MAX_PATH );
	strcpy_s( pOut, sizeOut, cSrc );
//	char *p = strrchr( pOut, cSlash );
	char *p = strrchr( pOut, '/' );
	if( p == nullptr ) {
		p = strrchr( pOut, '/' );
		if( p == nullptr )
			return "";
	}
	*(p+1) = 0;		// \뒤에 널이 들어가도록
	return pOut;
}
const char * XE::GetFilePath( const char * cSrc, char cSlash )
{
	return GetFilePath( _cTempBuff, sizeof(_cTempBuff) / sizeof(_cTempBuff[0]), cSrc, cSlash );
}
#endif // win32

// 파일명만 추출.
LPCTSTR	XE::GetFileName( TCHAR *pOut, int sizeOut, LPCTSTR _szSrc )
{
	if( !_szSrc )
		return _T("");
	TCHAR szSrc[2048];
	_tcscpy_s( szSrc, _szSrc );
	int		len, i;
	if( XE::IsEmpty(szSrc) )
		return _T("");
	memset( pOut, 0, sizeOut * sizeof(TCHAR) );
	len = _tcslen( szSrc );		// 풀네임 길이 읽음
	i = len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 \가 나오는곳을 찾는다.
#ifdef WIN32
		if( szSrc[i] == '\\' || szSrc[i] == '/' )	// ::GetModuleFileName같은건 \로 나오기땜에 이 루틴은 살려놔야함.
#else
		if( szSrc[i] == '/' )		// 이제 윈도우 환경도 모두 /로 통일한다
 #endif
		{
			_tcscpy_s( pOut, sizeOut, szSrc + i + 1 );	// \가 나온 다음 칸부터 파일명으로 씀
			break;
		}
		if( i == 0 )		// 끝까지 검색했는데도 \가 나오지 않았으면 그대로 씀
			_tcscpy_s( pOut, sizeOut, szSrc );
	}

	return pOut;
}
LPCTSTR	XE::GetFileName( LPCTSTR szSrc )
{
	return GetFileName( _szTempBuff, szSrc );
}
#ifdef WIN32
const char* XE::GetFileName( char *pOut, int sizeOut, const char* _cSrc )
{
	int		len, i;
	if( XE::IsEmpty( _cSrc ) )
		return "";
	char cSrc[2048];
	strcpy_s( cSrc, _cSrc );
// 파일명만 추출.
	memset( pOut, 0, sizeOut * sizeof(char) );
	len = strlen( cSrc );		// 풀네임 길이 읽음
	i = len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 \가 나오는곳을 찾는다.
#ifdef WIN32
		if( cSrc[i] == '\\' || cSrc[i] == '/' )	// ::GetModuleFileName같은건 \로 나오기땜에 이 루틴은 살려놔야함.
#else
		if( cSrc[i] == '/' )
#endif 
		{
			strcpy_s( pOut, sizeOut, cSrc + i + 1 );	// \가 나온 다음 칸부터 파일명으로 씀
			break;
		}
		if( i == 0 )		// 끝까지 검색했는데도 \가 나오지 않았으면 그대로 씀
			strcpy_s( pOut, sizeOut, cSrc );
	}

	return pOut;
}
const char* XE::GetFileName( const char* cSrc )
{
	return GetFileName( _cTempBuff, cSrc );
}
#endif // win32
// 파일타이틀만 임시버퍼에 저장	ex) test
LPCTSTR	XE::GetFileTitle( LPCTSTR _szSrc )
{
	TCHAR szSrc[2048];
	_tcscpy_s( szSrc, _szSrc );
	int		len, i;
	LPCTSTR szName;
	TCHAR buff[256];
	szName = GetFileName( szSrc );		// 패스는 떼네고 파일명의 시작 포인터를 리턴
	_tcscpy_s( buff, szName );
//	szSrc = buff;		// xuzhu.txt
	_tcscpy_s( szSrc, buff );
	// 파일명만 추출.
	len = _tcslen( szSrc );		// 풀네임 길이 읽음
	for( i = 0; i < len; i ++ )	{
		if( szSrc[i] == '.' )	break;
		_szTempBuff[i] = szSrc[i];
	}
	_szTempBuff[i] = '\0';

	return _szTempBuff;
}
LPCTSTR	XE::GetFileTitleX( LPCTSTR _szSrc )
{
	TCHAR szSrc[2048];
	_tcscpy_s( szSrc, _szSrc );
	int		len, i;
	LPCTSTR szName;
	TCHAR buff[ 256 ];
	szName = GetFileName( szSrc );		// 패스는 떼네고 파일명의 시작 포인터를 리턴
	_tcscpy_s( buff, szName );
//	szSrc = buff;		// xuzhu.txt
	_tcscpy_s( szSrc, buff );

	// 파일명만 추출.
	len = _tcslen( szSrc );		// 풀네임 길이 읽음
	for( i = 0; i < len; i++ )
	{
		if( szSrc[ i ] == '.' )	break;
		_szTempBuff[ i ] = szSrc[ i ];
	}
	_szTempBuff[ i ] = '\0';

	return _szTempBuff;
}

#ifdef WIN32
LPCTSTR XE::GetFileExt( LPCTSTR szSrc )
{
	LPCTSTR p = _tcsrchr( szSrc, '.' );
	if( p == NULL )
		return _T("");
	return p + 1;
}
#endif

const char* XE::GetFileExt( const char *cSrc )
{
	const char *p = strchr( cSrc, '.' );
	if( p == NULL )
		return "";
	return p + 1;
}

// XObj::Process 이런 문자열에서 클래스 이름만 빼온다.
LPCTSTR XE::GetClassName( LPCTSTR _szSrc )
{
	TCHAR szSrc[2048];
	_tcscpy_s( szSrc, _szSrc );
	int len = _tcslen( szSrc );
	int i;
	for( i = 0; i < len; ++i )	{
		if( szSrc[i] == _T(':') )		{
			_szTempBuff[i] = 0;
			return _szTempBuff;
		}
		_szTempBuff[i] = szSrc[i];
	}
	_szTempBuff[i] = 0;
	return _szTempBuff;
}

///////////////////////////////////////////////
#ifndef WIN32
#define CC_BREAK_IF(cond)            if(cond) break

BOOL XE::IsExistFileFromZip( const char* pszZipFilePath, const char* pszFileName )
{
	const std::string strFile = XE::GetFileName( pszFileName );
	XBREAK( strFile.empty() );
	BOOL bRet = FALSE;
	unzFile pFile = NULL;
	do
	{
		CC_BREAK_IF(!pszZipFilePath || !pszFileName);
		CC_BREAK_IF(strlen(pszZipFilePath) == 0);

		pFile = unzOpen(pszZipFilePath);
		CC_BREAK_IF(!pFile);

		int nRet = unzLocateFile(pFile, pszFileName, 1);
		CC_BREAK_IF(UNZ_OK != nRet);
		
		char szFilePathA[260];
		unz_file_info FileInfo;
		nRet = unzGetCurrentFileInfo(pFile, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0);
		CC_BREAK_IF(UNZ_OK != nRet);

		nRet = unzOpenCurrentFile(pFile);
		CC_BREAK_IF(UNZ_OK != nRet);
		//
		bRet = TRUE;
		//
		unzCloseCurrentFile(pFile);
	} while (0);

	if (pFile)
	{
		unzClose(pFile);
	}
	return bRet;
}

time_t XE::GetFileModifyTimeFromZip( const char* pszZipFilePath, const char* pszFileName )
{
	BOOL bRet = FALSE;
	unzFile pFile = NULL;
	time_t mtime = 0;
	do
	{
		CC_BREAK_IF(!pszZipFilePath || !pszFileName);
		CC_BREAK_IF(strlen(pszZipFilePath) == 0);

		pFile = unzOpen(pszZipFilePath);
		CC_BREAK_IF(!pFile);

		int nRet = unzLocateFile(pFile, pszFileName, 1);
		CC_BREAK_IF(UNZ_OK != nRet);
		
		char szFilePathA[260];
		unz_file_info FileInfo;
		nRet = unzGetCurrentFileInfo(pFile, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0);
		CC_BREAK_IF(UNZ_OK != nRet);
		mtime = FileInfo.dosDate;

		//
		bRet = TRUE;
	} while (0);

	if (pFile)
	{
		unzClose(pFile);
	}
	return mtime;
}

static void SetTm( struct tm *pTmDst, tm_unz& tmsrc )
{
	pTmDst->tm_year = tmsrc.tm_year;
	pTmDst->tm_mon = tmsrc.tm_mon;
	pTmDst->tm_mday = tmsrc.tm_mday;
	pTmDst->tm_hour = tmsrc.tm_hour;
	pTmDst->tm_min = tmsrc.tm_min;
	pTmDst->tm_sec = tmsrc.tm_sec;
	pTmDst->tm_isdst = 0;
}

unsigned char* XE::GetFileDataFromZip( const char* pszZipFilePath, const char* pszFileName, unsigned long *pOutSize )
{
		unsigned char * pBuffer = NULL;
		unzFile pFile = NULL;
		*pOutSize = 0;
		
		do
		{
				CC_BREAK_IF(!pszZipFilePath || !pszFileName);
				CC_BREAK_IF(strlen(pszZipFilePath) == 0);
				
				pFile = unzOpen(pszZipFilePath);
				CC_BREAK_IF(!pFile);
				
				int nRet = unzLocateFile(pFile, pszFileName, 1);
				CC_BREAK_IF(UNZ_OK != nRet);
				
				char szFilePathA[260];
				unz_file_info FileInfo;
				nRet = unzGetCurrentFileInfo(pFile, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0);
// 		XTRACE("in Apk:%s\nunzip:%s", pszZipFilePath, pszFileName );
// 		XTRACE("dosDate:%d\n%d/%d/%d(%d:%d:%d)", FileInfo.dosDate, 
// 				FileInfo.tmu_date.tm_year,
// 				FileInfo.tmu_date.tm_mon,
// 				FileInfo.tmu_date.tm_mday,
// 				FileInfo.tmu_date.tm_hour,
// 				FileInfo.tmu_date.tm_min,
// 				FileInfo.tmu_date.tm_sec
// 				);
// 		struct tm user_stime;
// 		time_t user_time;
// 		SetTm( &user_stime, FileInfo.tmu_date );
// 		user_time = mktime( &user_stime );
// 		XTRACE("time_t=%d", user_time);
				CC_BREAK_IF(UNZ_OK != nRet);
				
				nRet = unzOpenCurrentFile(pFile);
				CC_BREAK_IF(UNZ_OK != nRet);
				
				pBuffer = new unsigned char[FileInfo.uncompressed_size];
				int nSize = 0;
				nSize = unzReadCurrentFile(pFile, pBuffer, FileInfo.uncompressed_size);
				XASSERTF(nSize == 0 || nSize == (int)FileInfo.uncompressed_size, "the file size is wrong");
				
				*pOutSize = FileInfo.uncompressed_size;
				unzCloseCurrentFile(pFile);
		} while (0);
		
		if (pFile)
		{
				unzClose(pFile);
		}
		
		return pBuffer;
}

#endif // not win32

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//      Image Processing
//
//////////////////////////////////////////////////////////////////////////////////////////////////
// #ifdef _VER_OPENGL
// 	#define XCOLOR_ARGB4444(a,r,g,b)	\
// 		((XCOLORW)( (((a)>>4) << 12) | (((r)>>4) << 8) | (((g)>>4) << 4) | ((b)>>4) ))
// #else
// 	#define XCOLOR_ARGB4444(a,r,g,b)	\
// 		((XCOLORW)( (((a)>>1) << 12) | (((b)>>1) << 8) | (((g)>>1) << 4) | ((r)>>1) ))
// #endif
#ifdef WIN32
// 윈도환경외에 사용할일 없음.
/**
 @brief argb8888 -> argb4444
*/
void XE::ConvertBlockARGB8888ToARGB4444( WORD *pDst, int wDst, int hDst
																			, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE a = XCOLOR_A_FROM_ARGB( pixelSrc );
			BYTE r = XCOLOR_R_FROM_ARGB( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ARGB( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ARGB( pixelSrc );
			*pDst++ = XCOLOR_TO_ARGB4444( a, r, g, b );
		}
		pDst += ( wDst - wSrc );
	}
}
/**
 @brief ARGB8888포맷의 pSrc이미지를 RGB565포맷으로 바꿔준다.
 pDst의 초기 클리어는 호출하는측에서 책임진다.
*/
void XE::ConvertBlockARGB8888ToRGB565( WORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE r = XCOLOR_R_FROM_ARGB( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ARGB( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ARGB( pixelSrc );
			*pDst++ = XCOLOR_TO_RGB565( r, g, b );
		}
		pDst += ( wDst - wSrc );
	}
}
#endif // WIN32

#ifdef _VER_OPENGL
// dx모드에선 사용할일이 없음.
/**
 @brief argb8888 -> abgr4444
*/
// void XE::ConvertBlockARGB8888ToABGR4444( WORD *pDst, int wDst, int hDst
// 																			, DWORD *pSrc, int wSrc, int hSrc )
// {
// 	for( int i = 0; i < hSrc; i++ ) {
// 		for( int k = 0; k < wSrc; k++ ) {
// 			DWORD pixelSrc = *pSrc++;
// 			BYTE a = XCOLOR_A_FROM_ARGB( pixelSrc );
// 			BYTE r = XCOLOR_R_FROM_ARGB( pixelSrc );
// 			BYTE g = XCOLOR_G_FROM_ARGB( pixelSrc );
// 			BYTE b = XCOLOR_B_FROM_ARGB( pixelSrc );
// 			*pDst++ = XCOLOR_TO_ABGR4444( a, b, g, r );
// 		}
// 		pDst += ( wDst - wSrc );
// 	}
// }
// abgr8 -> bgr4
void XE::ConvertBlockABGR8888ToABGR4444( WORD *pDst, int wDst, int hDst
																			, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE a = XCOLOR_A_FROM_ABGR( pixelSrc );
			BYTE r = XCOLOR_R_FROM_ABGR( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ABGR( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ABGR( pixelSrc );
			*pDst++ = XCOLOR_TO_ABGR4444( a, b, g, r );
		}
		pDst += ( wDst - wSrc );
	}
}
void XE::ConvertBlockABGR8888ToRGBA4444( WORD *pDst, int wDst, int hDst
																			, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE a = XCOLOR_A_FROM_ABGR( pixelSrc );
			BYTE r = XCOLOR_R_FROM_ABGR( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ABGR( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ABGR( pixelSrc );
			*pDst++ = XCOLOR_TO_RGBA4444( r, g, b, a );
		}
		pDst += ( wDst - wSrc );
	}
}
#define XCOLOR_TO_RGBA1555(r,g,b,a)	\
	((XCOLORW)( (((r)>>3) << 11) | (((g)>>3) << 6) | (((b)>>3) << 1) | ((a)>>7) ))

void XE::ConvertBlockABGR8888ToRGBA1555( WORD *pDst, int wDst, int hDst
																			, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE a = XCOLOR_A_FROM_ABGR( pixelSrc );
			BYTE r = XCOLOR_R_FROM_ABGR( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ABGR( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ABGR( pixelSrc );
			*pDst++ = XCOLOR_TO_RGBA1555( r, g, b, a );
		}
		pDst += ( wDst - wSrc );
	}
}
void XE::ConvertBlockARGB8888ToABGR8888( DWORD *pDst, int wDst, int hDst
																			, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE a = XCOLOR_A_FROM_ARGB( pixelSrc );
			BYTE r = XCOLOR_R_FROM_ARGB( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ARGB( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ARGB( pixelSrc );
			*pDst++ = XCOLOR_ABGR( r, g, b, a );
		}
		pDst += ( wDst - wSrc );
	}
}
// void XE::ConvertBlockABGR8888ToABGR4444_2( WORD *pDst, int wDst, int hDst
// 																			, DWORD *pSrc, int wSrc, int hSrc )
// {
// 	for( int i = 0; i < hSrc; i++ ) {
// 		for( int k = 0; k < wSrc; k++ ) {
// 			DWORD pixelSrc = *pSrc++;
// 			BYTE a = XCOLOR_A_FROM_ABGR( pixelSrc );
// 			BYTE r = XCOLOR_R_FROM_ABGR( pixelSrc );
// 			BYTE g = XCOLOR_G_FROM_ABGR( pixelSrc );
// 			BYTE b = XCOLOR_B_FROM_ABGR( pixelSrc );
// 			*pDst++ = XCOLOR_TO_ARGB4444( r, g, b, a );
// 		}
// 		pDst += ( wDst - wSrc );
// 	}
// }
/**
 @brief ARGB8888포맷의 pSrc이미지를 BGR565포맷으로 바꿔준다.
 pDst의 초기 클리어는 호출하는측에서 책임진다.
*/
void XE::ConvertBlockARGB8888ToBGR565( WORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE r = XCOLOR_R_FROM_ARGB( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ARGB( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ARGB( pixelSrc );
			*pDst++ = XCOLOR_TO_BGR565( b, g, r );
		}
		pDst += ( wDst - wSrc );
	}
}
void XE::ConvertBlockABGR8888ToBGR565( WORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE r = XCOLOR_R_FROM_ABGR( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ABGR( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ABGR( pixelSrc );
			*pDst++ = XCOLOR_TO_BGR565( b, g, r );
		}
		pDst += ( wDst - wSrc );
	}
}
void XE::ConvertBlockABGR8888ToRGB565( WORD *pDst, int wDst, int hDst
																		, DWORD *pSrc, int wSrc, int hSrc )
{
	for( int i = 0; i < hSrc; i++ ) {
		for( int k = 0; k < wSrc; k++ ) {
			DWORD pixelSrc = *pSrc++;
			BYTE r = XCOLOR_R_FROM_ABGR( pixelSrc );
			BYTE g = XCOLOR_G_FROM_ABGR( pixelSrc );
			BYTE b = XCOLOR_B_FROM_ABGR( pixelSrc );
			*pDst++ = XCOLOR_TO_RGB565( r, g, b );
		}
		pDst += ( wDst - wSrc );
	}
}
#endif // _VER_OPENGL


void XE::NumberToMoneyString( TCHAR acDest[], int nNumber )
{    
	TCHAR acNumberString[64];    
	TCHAR *pReverse;    
	TCHAR *pMoneyString;    
	int nCount, nLength;         
	_stprintf_s( acNumberString, _T("%d"), ((nNumber < 0) ? -nNumber : nNumber));    
	nLength = _tcslen( acNumberString );         // pReverse를 acNumberString문자열의 마지막 문자에 위치시킵니다. .    
	pReverse = acNumberString + (nLength - 1);    
	pMoneyString = acDest;           // 뒤에서 부터 앞으로 포인터를 이동하면서 복사    
	nCount = 0;    
	while( nCount < nLength )    
	{        
		if( nCount !=0 && nCount % 3 == 0 ) 
			*pMoneyString++ = ',';        
		*pMoneyString++ = *pReverse--;        
		nCount++;    
	}         
	// 음수 처리     
	if ( nNumber < 0 ) 
		*pMoneyString++ = '-';    
	*pMoneyString = '\0';             
	// 문자열 뒤집기      
	_tcsrev( acDest );
}
/**
 @brief 숫자 단위가 크면 천단위는 K로 표시한다.
 모든 숫자는 최대 4자리까지만 표시되게 한다. 10,000 => 10K    9,999 => 9,999
 10,000 => 10K
 10,000,000 => 10M
 10,000,000,000 => 10B
*/
void XE::NumberToMoneyStringK( TCHAR acDest[], int nNumber )
{
	TCHAR symbol = 0;
	int numK = nNumber;
	if( numK >= 10000000000) {	// => 10B
		numK /= 1000000000;
		symbol = 'B';
	}	else 
	if( numK >= 10000000 ) {		// => 10M
		numK /= 1000000;
		symbol = 'M';
	}	else 
	if( numK >= 10000 ) {
		numK /= 1000;			// => 10K
		symbol = 'K';
	}
	NumberToMoneyString( acDest, numK );
	if( symbol ) {
		const auto len = _tcslen( acDest );
		acDest[len] = symbol;
		acDest[len+1] = 0;
	}
}
LPCTSTR XE::NumberToMoneyString( int number )
{
	static TCHAR szBuff[1024];
	NumberToMoneyString( szBuff, number );
	return szBuff;
}
LPCTSTR XE::NumberToMoneyStringK( int number )
{
	static TCHAR szBuff[ 1024 ];
	NumberToMoneyStringK( szBuff, number );
	return szBuff;
}

LPCTSTR XE::GetTimeString()
{
	return GetTimeString( _szTempBuff );
}

LPCTSTR XE::GetTimeString( TCHAR *pBuff, int len )
{
	SYSTEMTIME		time;
	GetLocalTime( &time );
	_tcscpy_s( pBuff, len, XE::Format( _T("%d/%2d/%2d   %02d:%02d:%02d"), 
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond ) );
	return pBuff;
}

#ifdef WIN32
DWORD XE::GetHDDSerial()
{
	char Buffer[ 256 ];
	char C[ 256 ];
	DWORD A, B;
	DWORD SerialNum;
	if( GetVolumeInformationA("C:\\", Buffer, 256, &SerialNum, &A, &B, C, 256) ) 
		return SerialNum;
	return 0;
}
#endif

#ifdef WIN32
unsigned long long XE::GetCheckSum( LPCTSTR szStr )
{
	TCHAR c;
	unsigned long long dwCheckSum = 0;
	BYTE cnt = 1;
	while((c = *szStr++ ))
	{
		dwCheckSum += (unsigned long long)c * cnt;
		cnt += 100;
	}
	return dwCheckSum;
}
#endif // WIN32

unsigned long long XE::GetCheckSum( const char* cStr )
{
	char c;
	unsigned long long dwCheckSum = 0;
	BYTE cnt = 1;
	while( ( c = *cStr++ ) ) {
		dwCheckSum += ( unsigned long long )c * cnt;
		cnt += 100;
	}
	return dwCheckSum;
}

unsigned long long XE::GetCheckSum( const BYTE *pMem, int size )
{
	BYTE c;
	unsigned long long llCheckSum = 0;
	BYTE cnt = 1;
	while( size-- )
	{
		c = *pMem++;
		llCheckSum += (unsigned long long)c * cnt;
		cnt += 100;
	}
	return llCheckSum;
}

// \n 문자열을 실제 리턴문자로 교체한다.
void XE::ReplaceReturnChar( TCHAR *szDst, int sizeDst, LPCTSTR szSrc )
{
	TCHAR c;
	TCHAR *pDst = szDst;
	TCHAR *pSrc = (TCHAR*)szSrc;
	while( ( c = *pSrc++ ) ) {
		if( c == '\\' ) {
			if( *pSrc++ == 'n' )
				*pDst++ = '\n';
		} else
			*pDst++ = c;
	}
	*pDst = 0;
}
//szSrcZip = c:/work/a/abc.zip
//	szBuff = c:/work/a/
//TCHAR szBuff[ 1024 ];
//_tcscpy_s( szBuff, XE::GetFilePath( szSrcZip ) );
//XE::GetFileName()	->_abc.zip
//XE::GetFileTitle()	-> abc

//szSrcZip Zip 파일의 풀패스, szSrcFile 압축 해제할 파일의 이름, szDstFile 해제후 파일의 풀패스
int XE::UnZipFile(LPCTSTR szSrcZip, LPCTSTR szSrcFile, LPCTSTR szDstFile)
{
	BOOL bResult = FALSE;
#ifdef _CLIENT
	unzFile uf = NULL;
	do 
	{
		XBREAK_IF(XE::IsEmpty(szSrcZip) == TRUE );
		XBREAK_IF(XE::IsEmpty(szSrcFile) == TRUE );
		//압축파일 오픈
		std::string strSrcZip = SZ2C(szSrcZip);
		uf = unzOpen(strSrcZip.c_str());
		XBREAK_IF( uf == NULL );
		string strSrcFile = SZ2C(szSrcFile);
		int ret = unzLocateFile(uf, strSrcFile.c_str(), 1);
		XBREAK_IF(UNZ_OK != ret);

		CHAR filename[MAX_PATH];       
		unz_file_info info;
		ret = unzGetCurrentFileInfo(uf, &info, filename, MAX_PATH, NULL, 0, NULL, 0);
		XBREAK_IF(UNZ_OK != ret);

		ret = unzOpenCurrentFile(uf);
		XBREAK_IF(UNZ_OK != ret);

		{
			// 일정덩어리씩 압축 해제함
			const int BUF = 0xffff;
			Bytef in[BUF];
			int readsize(0);
			std::ofstream op;
			//압축 해제
			if( szDstFile != NULL )
				op.open(szDstFile, std::ios_base::binary);
			else
			{
				TCHAR szFileBuff[MAX_PATH];
				_tcscpy_s( szFileBuff, XE::GetFilePath( szSrcZip ) );
				_tcscat_s( szFileBuff, szSrcFile );
				op.open( SZ2C(szFileBuff), std::ios_base::binary );
			}
			while(( readsize = unzReadCurrentFile(uf,(void*) in, BUF) ))
				op.write( (const  char*) in, readsize);

			op.close();
			unzCloseCurrentFile(uf);
		} 
		bResult = TRUE;
	} while(0);
	if( uf )
		unzClose(uf);
#else
	XBREAK(1);
#endif // CLIENT
	return bResult;
}
//////////////////////////////////////////////////////////////////////////
// 만들다 만 클래스
class XStringUTF8
{
	std::string m_strUtf8;
	void Init() {}
	void Destroy() {}
public:
#ifdef WIN32
	XStringUTF8( LPCWSTR wStr ) { 
		Init(); 
		char utf8[ 4096 ];
		ConvertUTF16toUTF8( wStr, utf8 );
		m_strUtf8 = utf8;
	}
#endif 
	XStringUTF8( const char *utf8 ) { 
		Init(); 
		m_strUtf8 = utf8;
	}
	virtual ~XStringUTF8() { Destroy(); }
	//
	GET_ACCESSOR( const std::string&, strUtf8 );
	//
	BOOL operator == ( const char *utf8 ) {
		return XE::IsSame( m_strUtf8.c_str(), utf8 );
	}
};

void XE::GetUUID( char *cOut, int len )
{
	char cBuff[ 48 ];
	char cBuff2[ 48 ];
	UINT rnd;
	rnd = xRand();
	sprintf_s( cBuff2, "%08x", rnd );
	strcpy_s( cBuff, cBuff2 );
	rnd = xRand();
	sprintf_s( cBuff2, "%08x", rnd );
	strcat_s( cBuff, cBuff2 );
	rnd = xRand();
	sprintf_s( cBuff2, "%08x", rnd );
	strcat_s( cBuff, cBuff2 );
	rnd = xRand();
	sprintf_s( cBuff2, "%08x", rnd );
	strcat_s( cBuff, cBuff2 );
	rnd = xRand();
	sprintf_s( cBuff2, "%08x", rnd );
	strcat_s( cBuff, cBuff2 );
	strcpy_s( cOut, len, cBuff );
}
#ifdef WIN32
bool XE::GetGUID( std::string* pOutStr )
{
	GUID guid;
	auto hr = CoCreateGuid( &guid );
	if( hr != S_OK )
		return false;
	*pOutStr = XE::Format("%08X%04X%04X"
										, guid.Data1
										, guid.Data2
										, guid.Data3 );
	for( int i = 0; i < XNUM_ARRAY(guid.Data4); ++i ) {
		*pOutStr += XE::Format("%02X", guid.Data4[i] );
	}
	return true;
}
#endif // WIN32

/**
 @brief 문자열에서 {을/를} 형태의 스트링을 찾아서 돌려준다.
 @param idx 검색할 시작 인덱스.
*/
// _tstring XE::GetStrJosa( LPCTSTR szStr, int& idx )
// {
// 	TCHAR josaString[1024];		// {단어,을/를} 의 포맷문자열
// 	josaString[0] = 0;
// 	int idxF = 0;
// 	bool bOpen = false, bClose = false;
// 	while(1)
// 	{
// 		TCHAR c = szStr[idx++];
// 		if( bOpen && c != '[' )
// 		{
// 			bOpen = false;
// 			idxF = 0;
// 			josaString[ 0 ] = 0;
// 		} else
// 		if( bClose && c != '}' )
// 			bClose = false;
// 		if( c == '{' )
// 		{
// 			bOpen = true;
// 		} else
// 		if( c == ']' )
// 		{
// 			bClose = true;
// 		} else
// 		if( c == '}' )
// 		{
// 			if( bClose ) {
// 				josaString[idxF++] = c;
// 				josaString[idxF++] = 0;
// 				XBREAK( idxF >= XARRAY_LENGTH(josaString) );
// 				break;
// 			}
// 		}
// 		if( bOpen )
// 		{
// 			josaString[ idxF++ ] = c;
// 		}
// 	}
// 	//
// 	return _tstring( josaString );
// }

/**
 @brief wChar가 받침이 있는지 없는지 조사한다.
*/
bool XE::IsBachim( WCHAR wChar )
{
	if( wChar >= '0' && wChar <= '9')
	{
		if( std::wcschr( L"013678", wChar ) )
			return true;
		else 
			return false;
	} else
	if( (wChar >= 'a' && wChar <= 'z') || (wChar >= 'A' && wChar <= 'Z') )
	{
		if( std::wcschr( L"aeiourvwyzAEIOURVWYZ", wChar ) )
			return false;		// ex:(hera, tie, xi, woo, you, winner, cov, warez)를
		else
			return true;
	} else
	{
		// 한글의 경우는 코드를 변환한다.
		int code = wChar - 44032;
		// 	int cho = 19, jung = 21, jong=28;
		if( code < 0 || code > 11171 )
			return false;
		if( ( code % 28 ) == 0 )
			return false;
		else
			return true;
	}
}


/**
 @brief 문자열에서 {을/를} 형태의 스트링을 만나면 마지막글자를 분석해서 둘중 하나를 선택한다.
 받침있는조사/받침없는조사 형태로 써야 한다.
*/
void XE::ConvertJosaStr( TCHAR *szDst, int lenDst, LPCTSTR szStr )
{
	TCHAR josa1[32];	// 받침글자의 조사
	TCHAR josa2[32];	// 노받침글자의 조사
#ifdef _XUTF8
	char utf8Char[3] = {0,};	// lastChar의 utf8한글
	int idxUtf8 = 0;
#endif
	WCHAR lastChar = 0;		// 마지막으로 읽은 utf16포맷의 글자
	int idxSrc = 0, idxDst = 0;
	int idxJosa1 = 0, idxJosa2 = 0;
	int open = 0;	// 0:조사블럭아님 1:첫번째조사 2:두번째조사
	while( 1 )
	{
		TCHAR c = szStr[ idxSrc++ ];
		if( c == 0 )
			break;
		else
		if( c == '{' )
		{
			open = 1;
			continue;
		}
		else
		if( c == '/' && open == 1 )
		{
			open = 2;	// 두번째 조사
			continue;
		} else
		if( c == '}' )
		{
			open = 0;
			idxJosa1 = idxJosa2 = 0;
			if( IsBachim(lastChar) ) {
				_tcscat_s( szDst, lenDst, josa1 );
				idxDst += _tcslen( josa1 );
			}
			else {
				_tcscat_s( szDst, lenDst, josa2 );
				idxDst += _tcslen( josa2 );
			}
			continue;;
		}
		if( open )
		{
			if( open == 1 ) {
				josa1[idxJosa1++] = c;
				XBREAK( idxJosa1 >= lenDst );
				josa1[idxJosa1] = 0;
			}
			else {
				josa2[idxJosa2++] = c;
				XBREAK( idxJosa2 >= lenDst );
				josa2[idxJosa2] = 0;
			}

		} else {
			szDst[idxDst++] = c;
			szDst[idxDst] = 0;
#ifdef _XUTF8
			if( IsHangul( c ) ) {
				utf8Char[ idxUtf8++ ] = c;
				if( idxUtf8 >= 3 ) {
					idxUtf8 = 0;
					lastChar = (WCHAR)Utf8HangulToUtf16( utf8Char );
//  					int i0 = lastChar;
//  					XLOGXNA("3.%d=0x%x", i0, i0 );
// 
// 					lastChar = (WCHAR)(i0 & 0x0000ffff);
				}
			} else
				lastChar = (WCHAR)c;
#else
			if( IsAsciiChar( c ) || IsHangul( c ) )
				lastChar = c;
#endif 
		}
	}
}

#ifdef WIN32
void XE::ConvertJosaStr( char *szDst, int lenDst, const char *szStr )
{
	char josa1[32];	// 받침글자의 조사
	char josa2[32];	// 노받침글자의 조사
	char utf8Char[3] = {0,};	// lastChar의 utf8한글
	int idxUtf8 = 0;
	WCHAR lastChar = 0;		// 마지막으로 읽은 utf16포맷의 글자
	int idxSrc = 0, idxDst = 0;
	int idxJosa1 = 0, idxJosa2 = 0;
	int open = 0;	// 0:조사블럭아님 1:첫번째조사 2:두번째조사
	while( 1 )
	{
		char c = szStr[ idxSrc++ ];
		if( c == 0 )
			break;
		else
		if( c == '{' )
		{
			open = 1;
			continue;
		}
		else
		if( c == '/' && open == 1 )
		{
			open = 2;	// 두번째 조사
			continue;
		} else
		if( c == '}' )
		{
			open = 0;
			idxJosa1 = idxJosa2 = 0;
			if( IsBachim(lastChar) ) {
				strcat_s( szDst, lenDst, josa1 );
				idxDst += strlen( josa1 );
			}
			else {
				strcat_s( szDst, lenDst, josa2 );
				idxDst += strlen( josa2 );
			}
			continue;;
		}
		if( open )
		{
			if( open == 1 ) {
				josa1[idxJosa1++] = c;
				XBREAK( idxJosa1 >= lenDst );
				josa1[idxJosa1] = 0;
			}
			else {
				josa2[idxJosa2++] = c;
				XBREAK( idxJosa2 >= lenDst );
				josa2[idxJosa2] = 0;
			}

		} else {
			szDst[idxDst++] = c;
			szDst[idxDst] = 0;
			if( IsHangul( c ) ) {
				utf8Char[ idxUtf8++ ] = c;
				if( idxUtf8 >= 3 ) {
					idxUtf8 = 0;
					lastChar = Utf8HangulToUtf16( utf8Char );
				}
			} else
				lastChar = (WCHAR)c;
		}
	}
}
#endif // WIN32

/**
 @brief strSrc에서 szFind를 찾아 szReplace로 대체한다.
*/
bool XE::ReplaceStr( _tstring& strSrc, LPCTSTR szFind, LPCTSTR szReplace )
{
	bool bFind = false;
	int pos;
	while( ( pos = strSrc.find( szFind ) ) != string::npos )
	{
		bFind = true;
		int len = _tcslen( szFind );
		strSrc.erase( pos, len );
		strSrc.insert( pos, szReplace );
	}
	return bFind;
}

/**
 @brief strFile 파일명을 조사해서 확장자가 없으면 szExt를 붙여준다.
 만일 strFile이 비어있다면 szDefaultName을 파일명으로 넣어준다.
*/
void XE::MakeExt( _tstring& strFile, LPCTSTR szExt, LPCTSTR szDefaultName )
{
	if( strFile.empty() )
	{
		if( szDefaultName )
			strFile = szDefaultName;
	} else
	{
		_tstring strIconExt = XE::GetFileExt( strFile.c_str() );
		if( strIconExt.empty() && szExt )
			strFile += szExt;
	}
}

int XE::GetNumCR( LPCTSTR szStr )
{
	int numCr = 0;
	TCHAR c;
	while( (c = *szStr++) )
	{
		if( c == '\n' )
			++numCr;
	}
	return numCr;
}
/**
 @brief 문자열이 알파벳이나 숫자로만 이뤄졌는지 검사한다
*/
bool XE::IsAsciiStr( LPCTSTR szStr )
{
	int len = _tcslen( szStr );
	for( int i = 0; i < len; ++i )
		if( !IsAsciiChar( szStr[i] ) )
			return false;
	return true;
}

XSeed::~XSeed() 
{
	// 기존시드값을 받아두는건 아무 의미 없음. 
	// rand()함수는 해당이 안되기때문. 끝나고나면 그냥 랜덤시드값으로 다시 맞추는게 맞음.
	const auto seedNew = ::xGenerateSeed();
	::xSRand( seedNew );
	//		::xSRand( ::timeGetTime() );
}

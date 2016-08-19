#include "stdafx.h"
#include "XSystem.h"
#include "etc/path.h"
#include "zip_support/zlib/zlib.h"
#ifdef _VER_IOS
//#include "civmm.h"
#include "objc/xe_ios.h"
#include <unistd.h>
#endif
#ifdef _VER_ANDROID
#include <unistd.h>
#include "android/com_mtricks_xe_Cocos2dxHelper.h"
#include "android/JniHelper.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _XNOT_BREAK
#pragma message("defined _XNOT_BREAK..........................................")
#endif


namespace XSYSTEM
{
//////////////////////////////////////////////////////////////////////////
// base64 Encode/Decode
static int POS(char c)
{
	if (c>='A' && c<='Z') return c - 'A';
	if (c>='a' && c<='z') return c - 'a' + 26;
	if (c>='0' && c<='9') return c - '0' + 52;
	if (c == '+') return 62;
	if (c == '/') return 63;
	if (c == '=') return -1;
	return 0;
}

char* base64_encode(const void* buf, size_t size)
{
	static const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//    static const char base64[] = "kk66XJZqFDh0IerxDnaTwH/XJ+fpf/qpD/Xx2+X+GOxfxDCl/awSr2AVOn0a+E+/";


	char* str = (char*) malloc((size+3)*4/3 + 1);

	char* p = str;
	unsigned char* q = (unsigned char*) buf;
	size_t i = 0;

	while(i < size) {
		int c = q[i++];
		c *= 256;
		if (i < size) c += q[i];
		i++;

		c *= 256;
		if (i < size) c += q[i];
		i++;

		*p++ = base64[(c & 0x00fc0000) >> 18];
		*p++ = base64[(c & 0x0003f000) >> 12];

		if (i > size + 1)
			*p++ = '=';
		else
			*p++ = base64[(c & 0x00000fc0) >> 6];

		if (i > size)
			*p++ = '=';
		else
			*p++ = base64[c & 0x0000003f];
	}

	*p = 0;

	return str;
}

void* base64_decode(const char* s, size_t* data_len_ptr)
{
	size_t len = strlen(s);

	if( XBREAK(len % 4) )
	{
		CONSOLE("Invalid input in base64_decode, %dis an invalid length for an input string for BASE64 decoding", len );
		return nullptr;
	}

	unsigned char* data = (unsigned char*) malloc(len/4*3);
	memset( data, 0, len/4*3 );

	int n[4];
	unsigned char* q = (unsigned char*) data;

	for(const char*p=s; *p; )
	{
		n[0] = POS(*p++);
		n[1] = POS(*p++);
		n[2] = POS(*p++);
		n[3] = POS(*p++);

		if (n[0]==-1 || n[1]==-1)
		{
			CONSOLE("Invalid input in base64_decode. Invalid BASE64 encoding");
			free(data);
			return nullptr;
		}

		if (n[2]==-1 && n[3]!=-1)
		{
			free(data);
			CONSOLE("Invalid input in base64_decode. Invalid BASE64 encoding");
			return nullptr;
		}

		q[0] = (n[0] << 2) + (n[1] >> 4);
		if (n[2] != -1) q[1] = ((n[1] & 15) << 4) + (n[2] >> 2);
		if (n[3] != -1) q[2] = ((n[2] & 3) << 6) + n[3];
		q += 3;
	}

	// make sure that data_len_ptr is not null
	if (!data_len_ptr)
	{
		CONSOLE("Invalid input in base64_decode. Invalid destination for output string length");
		return nullptr;
	}

	*data_len_ptr = q-data - (n[2]==-1) - (n[3]==-1);

	return data;
}
} // namespace XSYSTEM
// base64 Encode/Decode
//////////////////////////////////////////////////////////////////////////};

// szPath를 만든다. 만약 상위폴더가 없다면 실패하며 MakeRecursiveDir을 사용해야 한다.
BOOL XSYSTEM::MakeDir( LPCTSTR szPath )
{
	TCHAR szBuff[ 1024 ];
	_tcscpy_s( szBuff, szPath );
	if( _tmkdir( szBuff ) == 0 )
		return TRUE;
	return FALSE;
}
// szPath는 파일명이 포함되선 안되며 풀패스형태로 와야한다.
// ex) c:/spr/ko/
BOOL XSYSTEM::MakeRecursiveDir( LPCTSTR szPath )
{
	if( szPath[0] == 0 )
		return TRUE;
	if( MakeDir( szPath ) == FALSE )
	{
		// szPath의 상위폴더명을 얻는다.
		TCHAR szUpPath[ 1024 ];
		XE::MakeHighPath( szUpPath, 1024, szPath );
		MakeRecursiveDir( szUpPath );
		return MakeDir( szPath );
	}
	return TRUE;
}

#ifdef WIN32
#include <direct.h>
BOOL XSYSTEM::MakeDir( const char* cPath )
{
	char cBuff[ 1024 ];
	strcpy_s( cBuff, cPath );
	if( _mkdir( cBuff ) == 0 )
		return TRUE;
	return FALSE;
}
// cPath는 파일명이 포함되선 안되며 풀패스형태로 와야한다.
// ex) c:/spr/ko/
BOOL XSYSTEM::MakeRecursiveDir( const char* cPath )
{
	if( cPath[0] == 0 )
		return TRUE;
	if( MakeDir( cPath ) == FALSE )
	{
		// cPath의 상위폴더명을 얻는다.
		char cUpPath[ 1024 ];
		XE::MakeHighPath( cUpPath, 1024, cPath );
		MakeRecursiveDir( cUpPath );
		return MakeDir( cPath );
	}
	return TRUE;
}
#endif // win32

// 파일명 변환
BOOL XSYSTEM::RenameFile( const char* cPathOld, const char* cPathNew )
{
	return rename( cPathOld, cPathNew ) == 0;
}

BOOL XSYSTEM::RemoveFile( const char* _cPath )
{
	char cPath[ 1024 ];
	strcpy_s( cPath, _cPath );
	XPLATFORM_PATH( cPath );
	return remove( cPath ) == 0;
}

void XSYSTEM::strReplace( TCHAR *szBuff, TCHAR szSrc, TCHAR szDst )
{
	int len = _tcslen( szBuff );
	for( int i = 0; i < len; ++i )
	{
		if( szBuff[ i ] == szSrc )
			szBuff[ i ] = szDst;
	}
}

void XSYSTEM::strReplace( _tstring& strSrc, TCHAR tcSrc, TCHAR tcDst )
{
	std::replace_if( strSrc.begin(), strSrc.end(),
		[tcSrc]( char c )->bool{
			return ( c == tcSrc );
	}, tcDst );
}

#ifdef WIN32
void XSYSTEM::strReplace( std::string& strSrc, char cSrc, char cDst )
{
	std::replace_if( strSrc.begin(), strSrc.end(),
		[cSrc]( char c )->bool{
			return ( c == cSrc );
	}, cDst );
}
#endif // WIN32


DWORD XSYSTEM::GetFileSize( LPCTSTR szFullpath )
{
	XResFile file;
	if( file.Open( szFullpath, XBaseRes::xREAD ) == 0 )
		return 0;
	return (DWORD)file.Size();
}

#ifdef WIN32
DWORD XSYSTEM::GetFileSize( const char *cFullpath )
{
	XResFile file;
	if( file.Open( cFullpath, XBaseRes::xREAD ) == 0 )
		return 0;
	return (DWORD)file.Size();
}
#endif

/**
 @brief 파일이 리드온리파일인지 검사한다. 
 @return ERR_READONLY 리드온리 파일
 @return ERR_SUCCESS 파일이 있고 쓰기모드로 여는데 문제가 없음.
 @return 기타에러
*/
bool XSYSTEM::IsReadOnly( LPCTSTR szFilePath )
{
	XResFile file;
	file.Open( szFilePath, XBaseRes::xWRITE);
	auto err = file.GetLastError();
	return err == XE::ERR_READONLY || err == XE::ERR_PERMISSION_DENIED;
}
// szFile은 풀패스
unsigned long long XSYSTEM::CalcCheckSum( LPCTSTR szFile )
{
	XResFile file;
	if( file.Open( szFile, XBaseRes::xREAD ) == 0 )
		return 0;
	int size = file.Size();
	BYTE *pMem = new BYTE[ size ];
	file.Read( pMem, size );
	BYTE cnt = 1;
	unsigned long long checksum = 0;
	BYTE *p = pMem;
	for( int i = 0; i < size; ++i )
	{
		BYTE b1 = *p++;
		checksum += (unsigned long long)b1 * cnt;
		cnt += 100;
	}
	// 파일용량이 큰 경우 checksum값이 오버플로우가 되긴 하지만 체크섬기능엔 
	// 문제가 없는듯 하여 그대로 쓰기로 한다.
	SAFE_DELETE_ARRAY( pMem );
	return checksum;
}
#ifdef WIN32
void XSYSTEM::strReplace( char *cBuff, char cSrc, char cDst )
{
	int len = strlen( cBuff );
	for( int i = 0; i < len; ++i )
	{
		if( cBuff[ i ] == cSrc )
			cBuff[ i ] = cDst;
	}
}
#endif // win32

void XSYSTEM::xSleep( unsigned int sec )
{
#ifdef WIN32
	Sleep( (DWORD)(sec * 1000) );
#else
	sleep( sec );
#endif
}

void XSYSTEM::SetAutoSleepMode( BOOL bFlag )
{
#ifdef _VER_IOS
	IOS::SetAutoSleepMode( bFlag );
#endif
}

void XSYSTEM::RestoreAutoSleepMode()
{
#ifdef _VER_IOS
	IOS::RestoreAutoSleepMode();
#endif
}


XE::xtAlertRet XSYSTEM::xMessageBox( LPCTSTR szMsg, XE::xtAlert type )
{
	int retv = 1;
#ifdef WIN32
	UINT typeWin = MB_OK;
	switch( type )
	{
	case XE::xMB_OK:	typeWin = MB_OK;	break;
	case XE::xMB_YESNO:	typeWin = MB_YESNO;	break;
	case XE::xMB_OKCANCEL:	typeWin = MB_OKCANCEL;	break;
	}
  #if defined(_AFX) || defined(_AFXDLL)
	retv = AfxMessageBox( szMsg, typeWin );
  #else
	retv = MessageBox( nullptr, szMsg, _T("Alert"), (UINT) typeWin );
  #endif
	if( type == XE::xMB_YESNO )
		if( retv == IDNO )
			exit(1);
#endif
#ifdef _VER_IOS
	XALERT("%s", szMsg);
#endif
#ifdef _VER_ANDROID
	XLOG( "%s", szMsg );
#endif
	return (XE::xtAlertRet)retv;
}
//
#ifndef _WIN32
void XSYSTEM::xDebugBreak()
{
#ifdef WIN32
  #if defined(_XNOT_BREAK)
	return;
  #endif
  #if defined(_AFX) || defined(_AFXDLL)
 	AfxDebugBreak();
  #else
	DebugBreak();  
  #endif
#else
	volatile int nBreak = 0;
	nBreak = 1;
#endif
}
#endif // not win32

float XSYSTEM::StrToNumber( const char *cStr )
{
	if( XE::IsEmpty(cStr) )
		return 0.f;
	char cBuff[1024] = { 0, };
	char *cSrc = (char*)cStr;
	char c = 0;
	int i = 0;
	int len = strlen(cStr);
	if( len >= 1024 )
		return 0.f;

	while(( c = *cSrc++ ))
	{
		if( (c >= '0' && c <= '9') || c == '.' )
			cBuff[i++] = c;
	}
	float num = (float)atof(cBuff);
	return num;
}

/**
 pSrc메모리를 압축해서 pOutDst에 메모리를 할당해 돌려준다. pOutSizeDst는
 pOutDst의 사이즈. 
 1이 리턴되면 성공. 0이 리턴되면 실패.
*/
int XSYSTEM::CreateCompressMem( BYTE** ppOutDst, DWORD *pOutSizeDst, 
								BYTE *pSrc, DWORD sizeSrc )
{
	*ppOutDst = nullptr;
	*pOutSizeDst = 0;
	//  Upon entry, destLen is the total size of the destination buffer,
	// which must be at least 0.1% larger than sourceLen plus 12 bytes.
	DWORD sizeOutDst = (DWORD)((sizeSrc+12) * 1.001f) + 2;	// 스트링으로 쓰일수도 있으므로 +2(wchar)을 해줌.
	BYTE *pTemp = new BYTE[ sizeOutDst ];	// 압축크기의 최대가능성으로 잡는다.
	memset( pTemp, 0, sizeOutDst );
	int result = compress( (Bytef*)pTemp, (uLongf*)&sizeOutDst, 
							(const Bytef*)pSrc, (uLong)sizeSrc );
	if( result == Z_OK )
	{
		if( XBREAK( sizeOutDst > (DWORD)(sizeSrc * 1.5f) ) )	// 이런경우는 없을껄?
			return 0;
		*ppOutDst = new BYTE[ sizeOutDst ];
		*pOutSizeDst = sizeOutDst;
		memcpy_s( *ppOutDst, sizeOutDst, pTemp, sizeOutDst );
		SAFE_DELETE_ARRAY( pTemp );
		return 1;
	} else
	if( result == Z_MEM_ERROR )
	{
		// 메모리가 충분하지 않음.
		XBREAKF(1, "not enough memory for compress");
	} else
	if( result == Z_BUF_ERROR )
	{
		// 내보낼 버퍼의 크기가 충분하지 않음.
		XBREAKF(1, "dest not enough memory for compress");
	} else
	{
		XBREAKF(1, "unknown compress error:%d", result);
	}
	SAFE_DELETE_ARRAY( pTemp );
	return 0;
}

/**
 압축된 메모리 pSrc(크기sizeSrc)를 풀어서 ppOutDst(크기sizeDst)
 에 푼다.
*/
int XSYSTEM::UnCompressMem( BYTE **ppOutDst, DWORD sizeDst,
							BYTE *pSrc, DWORD sizeSrc )
{
	*ppOutDst = nullptr;
	if( XBREAK( sizeDst == 0 ) )
		return 0;
	if( XBREAK( sizeSrc == 0 ) )
		return 0;
	// 압축이 풀릴 메모리
	BYTE *pDst = new BYTE[ sizeDst ];
	*ppOutDst = pDst;
	DWORD sizeOutDst = sizeDst;
	int result = uncompress( (Bytef*)pDst, (uLongf*)&sizeOutDst, 
								(const Bytef*)pSrc, (uLong) sizeSrc );
	if( result == Z_OK )
	{
		XBREAK( sizeDst != sizeOutDst );
		return 1;
	} else
	if( result == Z_MEM_ERROR ) {
		XBREAKF(1, "not enough memory for uncompress");
	} else
	if( result == Z_BUF_ERROR ) {
		XBREAKF(1, "dest not enough memory for compress");
	} else
	if( result == Z_DATA_ERROR ) {
		XBREAKF(1, "error compress data");
	} else {
		XBREAKF(1, "unknown uncompress error:%d", result);
	}
	return 0;

}

BOOL XSYSTEM::IsAlphabet( LPCTSTR szStr )
{
	LPCTSTR p = szStr;
	TCHAR c = 0;
	while(( c = *p++ ))
	{
		if( IsAlphabet( c ) == FALSE )
			return FALSE;
	};
	return TRUE;

}

BOOL XSYSTEM::IsAlphabet( TCHAR c )
{
	if( (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') )
		return TRUE;
	return FALSE;
}

BOOL XSYSTEM::IsNumberChar( TCHAR c )
{
	if( (c >= '0' && c <= '9') )
		return TRUE;
	return FALSE;
}

// 3G, 4G상태냐
BOOL XSYSTEM::CheckCDMA()
{
#ifdef _VER_ANDROID
	return JniHelper::Check3G();
#elif defined(WIN32)
	return FALSE;
#endif
	return FALSE;
}

BOOL XSYSTEM::CheckWiFi()
{
#ifdef _VER_ANDROID
	return JniHelper::CheckWiFi();
#elif defined(WIN32)
	return TRUE;
#endif
	return TRUE;
}

/**
 현재 요일을 돌려준다. 0~6
 0:일욜 xDOW_SUNDAY
 1:월욜 xDOW_MONDAY
 2:화욜 나머지는 인텔리센스 참조ㅋ
 3:수욜
 4:목욜
 5:금욜
 6:토욜
*/
XE::xtDOW XSYSTEM::GetDayOfWeek()
{
	SYSTEMTIME time;
	GetLocalTime( &time );
	return (XE::xtDOW)time.wDayOfWeek;
}

/**
 현재 시(hour)를 돌려준다. 0~23
*/
int XSYSTEM::GetHour()
{
	SYSTEMTIME time;
	GetLocalTime( &time );
	return (int)time.wHour;
}

/**
 현재시간을 시/분/초로 만들어 돌려준다.
*/
void XSYSTEM::GetHourMinSec( int *pHourOut, int *pMinOut, int *pSecOut )
{
	SYSTEMTIME time;
	GetLocalTime( &time );
	*pHourOut = time.wHour;
	if( pMinOut )
		*pMinOut = time.wMinute;
	if( pSecOut )
		*pSecOut = time.wSecond;
}
/**
 @brief 오늘중에서 현재 몇초가 흘러갔는지 계산.
*/
int XSYSTEM::GetsecToday()
{
	SYSTEMTIME time;
	GetLocalTime( &time );
	int sec = time.wHour * 60 * 60;
	sec += time.wMinute * 60;
	sec += time.wSecond;
	return sec;
}
/**
 @brief secTotal초를 시/분/초로 나눠준다.
*/
void XSYSTEM::GetHourMinSec( xSec secTotal, int *pHourOut, int *pMinOut, int *pSecOut )
{
	xSec m = secTotal / 60;
	XBREAK( m > 0xffffffff );
	xSec h = m / 60;
	m %= 60;
	XBREAK( m > 0xffffffff );
// 	xSec sechm = ( ( h * 60 * 60 ) + ( m * 60 ) );	// 시:분 을 초로환산.
	xSec s = 0;
//	if( sechm > 0 )
		s = secTotal % 60;

	if( pHourOut )
		*pHourOut = (int)h;
	if( pMinOut )
		*pMinOut = (int)m;
	if( pSecOut )
		*pSecOut = (int)s;
}

/**
 @brief 초를 일/시/분/초로 환산한다.
*/
void XSYSTEM::GetDayHourMinSec( xSec secTotal, int *pDayOut, int *pHourOut, int *pMinOut, int *pSecOut )
{
	xSec m = secTotal / 60;
	XBREAK( m > 0xffffffff );
	xSec h = m / 60;
	m %= 60;
	XBREAK( m > 0xffffffff );
	xSec s = 0;
	s = secTotal % 60;
	int d = (int)(h / 24);
	h %= 24;

	if( pDayOut )
		*pDayOut = d;
	if( pHourOut )
		*pHourOut = (int)h;
	if( pMinOut )
		*pMinOut = (int)m;
	if( pSecOut )
		*pSecOut = (int)s;
}


/**
 오늘이 몇일인지를 리턴한다.
*/
int XSYSTEM::GetDay()
{
	SYSTEMTIME time;
	GetLocalTime( &time );
	return (int)time.wDay;
}

/**
 오늘의 년/월/일을 알아낸다.
*/
void XSYSTEM::GetYearMonthDay( int *pYearOut, int *pMonthOut, int *pDayOut )
{
	SYSTEMTIME time;
	GetLocalTime( &time );
	*pYearOut = time.wYear;
	if( pMonthOut )
		*pMonthOut = time.wMonth;
	if( pDayOut )
		*pDayOut = time.wDay;
}
/**
 시작일과 끝일을 주면 그사이가 몇일인지를 계산한다.
*/
/*
int XSYSTEM::GetDistDay( int year1, int month1, int day1, 
						int year2, int month2, int day2 )
{
	BOOL bYoon1, bYoon2;
	int numDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if( (year1 % 4) == 0 )
	{
		bYoon1 = TRUE;
		if( (year1 % 100) == 0 )
		{
			if( (year1 % 400) == 0 )
				bYoon1 = TRUE;
			else
				bYoon1 = FALSE;
		}
	}
	if( bYoon1 )
		numDay[1] = 29;
}
*/

/**
 2012년으로부터 주어진 날짜까지 일로 환산하면 몇일인지 계산한다.
*/
int XSYSTEM::GetYMDToDays( int year, int month, int day )
{
	BOOL bYoon = FALSE;	// 윤년
	const int yearBase = 2012;
	int numDay[ 12 ] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if( ( year % 4 ) == 0 )
	{
		bYoon = TRUE;
		if( ( year % 100 ) == 0 )
		{
			if( ( year % 400 ) == 0 )
				bYoon = TRUE;
			else
				bYoon = FALSE;
		}
	}
	if( bYoon )
		numDay[ 1 ] = 29;
	if( year < yearBase )
		return 0;		// 계산할 수 없음.
	int yearDist = year - yearBase;
	int numYoon = yearDist / 4 + 1;		// 그사이 기간동안의 윤년 횟수.
	int days = yearDist * 365 + numYoon;
	if( month > 1 )
	{
		for( int i = 0; i < month-1; ++i )
			days += numDay[ i ];
	}
	days += day;
	return days;
}
//////////////////////////////////////////////////////////////////////////
_XNoRecursiveInstance::~_XNoRecursiveInstance() 
{
	// 아직 해제되지 않은 리커시브방지 객체가 있다.
	XBREAKF( m_refCnt != 0, "is not release _XNoRecursiveInstance" );
}

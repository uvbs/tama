#include "stdafx.h"
#include "XSystem.h"
#include "zip_support/zlib/zlib.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

namespace XSYSTEM
{

};

// szPath를 만든다. 만약 상위폴더가 없다면 실패하며 MakeRecursiveDir을 사용해야 한다.
BOOL XSYSTEM::MakeDir( LPCTSTR szPath )
{
	TCHAR szBuff[ 1024 ];
	_tcscpy_s( szBuff, szPath );
	if( _tmkdir( szBuff ) == 0 )
		return TRUE;
	return FALSE;
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

void XSYSTEM::RestoreAutoSleepMode( void )
{
#ifdef _VER_IOS
	IOS::RestoreAutoSleepMode();
#endif
}



//
void XSYSTEM::xDebugBreak( void )
{
#ifdef WIN32
  #if defined(_SERVER) && !defined(_XNOT_BREAK)
	#if defined(_AFX) || defined(_AFXDLL)
 		AfxDebugBreak();
    #else
		DebugBreak();  
    #endif
  #endif // server && not break;
#else
	volatile int nBreak = 0;
	nBreak = 1;
#endif
}

float XSYSTEM::StrToNumber( const char *cStr )
{
	char cBuff[1024] = { 0, };
	char *cSrc = (char*)cStr;
	char c = 0;
	int i = 0;
	int len = strlen(cStr);
	if( len >= 1024 )
		return 0.f;

	while( c = *cSrc++ )
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
	*ppOutDst = NULL;
	*pOutSizeDst = 0;
	//  Upon entry, destLen is the total size of the destination buffer,
	// which must be at least 0.1% larger than sourceLen plus 12 bytes.
	DWORD sizeOutDst = (DWORD)((sizeSrc+12) * 1.001f) + 2;	// 스트링으로 쓰일수도 있으므로 +2(wchar)을 해줌.
	BYTE *pTemp = new BYTE[ sizeOutDst ];	// 압축크기의 최대가능성으로 잡는다.
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
	*ppOutDst = NULL;
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
	if( result == Z_MEM_ERROR )
		XBREAKF(1, "not enough memory for uncompress");
	else
	if( result == Z_BUF_ERROR )
		XBREAKF(1, "dest not enough memory for compress");
	else
	if( result == Z_DATA_ERROR )
		XBREAKF(1, "error compress data");
	else
		XBREAKF(1, "unknown uncompress error:%d", result);
	return 0;

}

//////////////////////////////////////////////////////////////////////////
_XNoRecursiveInstance::~_XNoRecursiveInstance() 
{
	// 아직 해제되지 않은 리커시브방지 객체가 있다.
	XBREAKF( m_refCnt != 0, "is not release _XNoRecursiveInstance" );
}


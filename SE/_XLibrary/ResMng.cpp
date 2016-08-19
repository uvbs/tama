/*
 *  Resource.cpp
 *  Game
 *
 *  Created by xuzhu on 10. 12. 8..
 *  Copyright 2010 LINKS CO.,LTD. All rights reserved.
 *
 */
#include "stdafx.h"
#include "ResMng.h"
//#include "XLua.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XBaseRes::xtERROR XBaseRes::s_err = XBaseRes::ERR_SUCCESS;
//TCHAR XBaseRes::s_szResName[1024] = { 0, };
#ifdef WIN32
//char XBaseRes::s_cResName[1024] = { 0, };
#endif

// static
/*
루아가 붙어서 ResMng의 독립성이 안좋아져서 안에서 뺌. 필요하면 밖에서 Register해서 쓸것.
void XBaseRes::RegisterLua( XLua *pLua )
{
	pLua->Register_Class<XBaseRes>( "XBaseRes" );
	pLua->RegisterCPPFunc<XBaseRes>( "Write", &XBaseRes::LuaWrite );
	pLua->RegisterCPPFunc<XBaseRes>( "Read", &XBaseRes::LuaRead );
//	RegisterCPPFunc<XBaseRes>( "getx", &XObj::Getx );
}
//--------------------------------------------------------------------------------------------------------
// static
void XResFile::RegisterLua( XLua *pLua )
{
	//
	XBaseRes::RegisterLua( pLua );
	//
	pLua->Register_Class<XResFile>( "XResFile" );
	pLua->Register_ClassInh<XResFile, XBaseRes>();
	pLua->RegisterCPPFunc<XResFile>( "Write", &XResFile::LuaWrite );
	pLua->RegisterCPPFunc<XResFile>( "Read", &XResFile::LuaRead );
//	pLua->RegisterCPPFunc<XResFile>( "Open", &XResFile::LuaOpen );
//	pLua->RegisterVar( "READ", FALSE );
//	pLua->RegisterVar( "WRITE", TRUE );
}
*/
int XResFile::Open( LPCTSTR szFilename, xRWTYPE mode ) 
{
	SetError( ERR_SUCCESS );
#ifdef WIN32
	errno_t err = 0;
#else
	int err = 0;
#endif
	if( mode == xREAD )
		err = _tfopen_s( &m_fp, szFilename, _T("rb") ); 
	else
	if( mode == xWRITE ) 
		err = _tfopen_s( &m_fp, szFilename, _T("wb") );
	else
		XBREAK(1);
	m_Mode = mode;
	SetResName( szFilename );		// 파일로딩에 성공하든 실패하든 마지막으로 억세스한 리소스이름을 받도록 바뀜.
	if( m_fp ) 
	{
		SetError( ERR_SUCCESS );
		//
		if( mode == xWRITE )
		{
			if( XCrypto::GetbEncryption() )		// 암호화 옵션이 켜져있는가.
			{
				XCrypto::WriteHeader( m_fp );		// 암호화에 필요한 헤더를 저장함.
			}
		} else
		if( mode == xREAD )
		{
			if( XCrypto::IsEncryption( m_fp ) )
			{
				if( XCrypto::CheckCheckSum( m_fp ) == FALSE )
				{
					XALERT_OKCANCEL( "%s file is damaged", szFilename );
#ifdef _VER_IPHONE
                    CONSOLE( "%s file is damaged.", XBaseRes::GetszResName() ); 
#endif
					exit(1);
				}
				XCrypto::ReadHeader( m_fp );		// 헤더를 읽고 초기화함.
			}
		}
	}
	else
	{
#ifdef WIN32
		switch( err )
		{
		case EACCES: SetError( ERR_PERMISSION_DENIED ); break;
		case EROFS:	SetError( ERR_READONLY );	break;
		case ENOENT: SetError( ERR_FILE_NOT_FOUND );	break;
		default:	SetError( ERR_FAILED );		break;				// 그외 일반적인 에러
		}
#else
        SetError( ERR_FILE_NOT_FOUND );
#endif
	}
	return (m_fp)? 1: 0;		// 성공실패는 1,0으로만 리턴하고 구체적인 에러를 보려면 GetError()을 사용해야 함
}

#ifdef WIN32
int XResFile::Open( const char *szFilename, xRWTYPE mode ) 
{ 
	SetError( ERR_SUCCESS );
#ifdef WIN32
	errno_t err = 0;
#else
	int err = 0;
#endif
	if( mode == xREAD )
		err = fopen_s( &m_fp, szFilename, "rb" ); 
	else
	if( mode == xWRITE ) 
		err = fopen_s( &m_fp, szFilename, "wb" );
	else
		XBREAK(1);
	m_Mode = mode;
	// Convert_char_To_TCHAR모듈이 링크되어 일단 뺌
//	SetResName( Convert_char_To_TCHAR( szFilename ) );		// 파일로딩에 성공하든 실패하든 마지막으로 억세스한 리소스이름을 받도록 바뀜.
	SetcResName( szFilename );
	if( m_fp ) 
	{
		SetError( ERR_SUCCESS );
		//
		if( mode == xWRITE )
		{
			if( XCrypto::GetbEncryption() )		// 암호화 옵션이 켜져있는가.
			{
				XCrypto::WriteHeader( m_fp );		// 암호화에 필요한 헤더를 저장함.
			}
		} else
		if( mode == xREAD )
		{
			if( XCrypto::IsEncryption( m_fp ) )
			{
				if( XCrypto::CheckCheckSum( m_fp ) == FALSE )		// 체크섬 검사
				{
#ifdef WIN32
					char buff[1024];
					sprintf_s( buff, "%s file is damaged", szFilename );
					MessageBoxA( NULL, buff, "error", MB_OK );
#endif
					exit(1);
				}
				XCrypto::ReadHeader( m_fp );		// 헤더를 읽고 초기화함.
			}
		}
	}
	else
	{
#ifdef WIN32
		switch( err )
		{
		case EACCES: SetError( ERR_PERMISSION_DENIED ); break;
		case EROFS:	SetError( ERR_READONLY );	break;
		case ENOENT: SetError( ERR_FILE_NOT_FOUND );	break;
		default:	SetError( ERR_FAILED );		break;				// 그외 일반적인 에러
		}
#else
        SetError( ERR_FILE_NOT_FOUND );
#endif
	}
	return (m_fp)? 1: 0;		// 성공실패는 1,0으로만 리턴하고 구체적인 에러를 보려면 GetError()을 사용해야 함
}
#endif // WIN32

void XResFile::Close() 
{ 
	if( m_fp ) 
	{
		if( m_Mode == XBaseRes::xWRITE )
		{
			// 쓰기모드면 파일을 닫기전에 마지막위치에 체크섬 집어넣음.
			if( GetbEncryption() )		// 암호화 파일이면
			{
				XCrypto::WriteCheckSum( m_dwCheckSum, m_fp );
/*				DWORD dwCheckSum;
				XCrypto::Encrypt( (BYTE *)&dwCheckSum, 4, (BYTE *)&m_dwCheckSum, 4 );	
				fwrite( &dwCheckSum, 4, 1, m_fp );		// checksum 저장. 일단 기존파일들 호환성 문제도 있고 하니 암호화파일만 체크섬 저장하도록 하자.
				*/
				XCrypto::WriteTailer( m_fp );		// 끝날때 정보 한번더 쓰고 끝낸다.
			}
		}
/*		else
		if( m_Mode == XBaseRes::xREAD )
		{
			if( GetbEncryption() )		// 암호화 파일이면
			{
				DWORD dwCheckSum;
				fread( &dwCheckSum, 4, 1, m_fp );
				XCrypto::Decrypt( (BYTE *)&dwCheckSum, 4 );		// ptr버퍼의 total길이만큼 해독해서 다시 넣는다.
				if( dwCheckSum != m_dwCheckSum )
				{
					if( XE::IsHave( XBaseRes::GetszResName() ) )
						XALERT( "%s file is damaged", XBaseRes::GetszResName() );
#ifdef WIN32
					else
					{
						char buff[1024];
						sprintf_s( buff, "%s file is damaged", XBaseRes::GetcResName() );
						MessageBoxA( NULL, buff, "error", MB_OK );
//						MessageBoxA( NULL, XE::Format( "%s 파일이 손상되었습니다.", XBaseRes::GetcResName() ), "알림", MB_OK );
					}
#endif
#ifdef _VER_IPHONE
                    CONSOLE( "%s file is damaged.", XBaseRes::GetszResName() ); 
#endif
					exit(1);
				}
			}
		} */
		fclose( m_fp ); 
	}
}

int XResFile::Read( void *ptr, size_t size, size_t n ) 
{ 
	XBREAK( (int)size < 0 );  
	int ret = fread( ptr, size, n, m_fp);	// 일단 통째로 다 읽는다.
	// 암호화 된 파일이면 해독해서 버퍼에 바꿔넣는다.
	if( GetbEncryption() )
	{
		int total = size * n;		// read한 전체 크기
		XCrypto::Decrypt( (BYTE *)ptr, total );		// ptr버퍼의 total길이만큼 해독해서 다시 넣는다.
		// 이것은 버퍼를 외부에서 지정해주기때문에 한번에 해독해도 된다.
		// 체크섬 계산.
		{
			int total = size * n;				// write해야할 전체 사이즈
			BYTE *pbyte = (BYTE *)ptr;
			for( int i = 0; i < total; ++i )
				m_dwCheckSum += (DWORD)(*pbyte++);		// 각 바이트를 모두 더함.
		}
	}
	return ret;
}

int XResFile::Write( void *ptr, size_t size, size_t n ) 
{ 
	XBREAK( (int)size < 0 );  
	int total = size * n;				// write해야할 전체 사이즈
	// 체크섬 계산.
	{
		BYTE *pbyte = (BYTE *)ptr;
		for( int i = 0; i < total; ++i )
			m_dwCheckSum += (DWORD)(*pbyte++);		// 각 바이트를 모두 더함.
	}

	if( GetbEncryption() )
	{
		// 암호화 쓰기모드
		// 버퍼를 4k단위로 끊어서 암호화 한다음 write한다.
		BYTE buffer[ 4096 ];
		int num = total / 4096;
		int remain = total % 4096;
		BYTE *_ptr = (BYTE *)ptr;
		for( int i = 0; i < num; ++i )
		{
			// 4k단위로 ptr을 암호화 해서 buffer에 넣는다.
			XCrypto::Encrypt( buffer, sizeof(buffer), _ptr, 4096 );	
			fwrite( buffer, 4096, 1, m_fp );
			_ptr += 4096;
		}
		// 4k단위로 끊고 나머지 부분을 마저 다 암호화 한다.
		if( remain )
		{
			XCrypto::Encrypt( buffer, sizeof(buffer), (BYTE*)_ptr, remain );	
			fwrite( buffer, remain, 1, m_fp );
		}
		return total;
	} else
		return fwrite( ptr, size, n, m_fp); 
}

long XResFile::Size() 
{ 
	if( GetbEncryption() )
	{
		long offset = ftell( m_fp );
		fseek( m_fp, 0, SEEK_END );
		int size = ftell( m_fp );			// file size
		size -= 4 + 4 + 28 + 4;			// identifier + ver + keyTable(28byte) + checksum
		fseek( m_fp, offset, SEEK_SET );
		return size;
	} else
	{
		long offset = ftell( m_fp );
		fseek( m_fp, 0, SEEK_END );
		int size = ftell( m_fp );			// file size
		fseek( m_fp, offset, SEEK_SET );
		return size;
	}
}

//
int XResFile::Printf( LPCTSTR szFormat, ... ) 
{ 
#ifdef WIN32
    TCHAR szBuff[1024];	
    va_list         vl;
	
    va_start(vl, szFormat);
    _vstprintf_s(szBuff, szFormat, vl);
    va_end(vl);

	return _ftprintf( m_fp, _T("%s"), szBuff ); 
#else
	return 0;
#endif
}

// 스트링의 사이즈와 스트링을 저장한다.
int XResFile::WriteString( TCHAR *szStr )
{
	XBREAK( szStr == NULL );
	int size = ((_tcslen( szStr ) + 1) * sizeof(TCHAR));		// 널포함
	XBREAK( size <= 0 );
	XBREAK( size > 0xffff );		// 스트링이 지나치게 길어도 뭔가 문제있는거다.
	Write( &size, 4 );
	Write( szStr, size );
	return size;
}

int XResFile::ReadString( TCHAR *pOutStr )
{
	XBREAK( pOutStr == NULL );
	int size;
	Read( &size, 4 );
	XBREAK( size <= 0 );
	XBREAK( size > 0xffff );
	Read( pOutStr, size );
	return size;
}

int XResFile::WriteStringA( char* cStr )
{
	XBREAK( cStr == nullptr );
	int len = strlen( cStr );
	int size = ( len + sizeof( char ) ) * sizeof( char );
	XBREAK( size <= 0 );
	XBREAK( size > 0xffff );		// 스트링이 지나치게 길어도 뭔가 문제있는거다.
	if( len > 0 )
		Write( &size, 4 );		// 널포함 사이즈
	else
		Write( &len, 4 );		// 널포함 사이즈
	if( len > 0 )
		Write( cStr, size );		// 널 포함.
	return size + sizeof( char );
}

int XResFile::ReadStringA( char* pOutStr )
{
	XBREAK( pOutStr == nullptr );
	int size;
	Read( &size, 4 );
	XBREAK( size < 0 );
	XBREAK( size > 0xffff );
	if( size > 0 )
		Read( pOutStr, size );
	else
		pOutStr[0] = 0;
	return size;
}


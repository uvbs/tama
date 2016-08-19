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
��ư� �پ ResMng�� �������� ���������� �ȿ��� ��. �ʿ��ϸ� �ۿ��� Register�ؼ� ����.
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
	SetResName( szFilename );		// ���Ϸε��� �����ϵ� �����ϵ� ���������� �＼���� ���ҽ��̸��� �޵��� �ٲ�.
	if( m_fp ) 
	{
		SetError( ERR_SUCCESS );
		//
		if( mode == xWRITE )
		{
			if( XCrypto::GetbEncryption() )		// ��ȣȭ �ɼ��� �����ִ°�.
			{
				XCrypto::WriteHeader( m_fp );		// ��ȣȭ�� �ʿ��� ����� ������.
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
				XCrypto::ReadHeader( m_fp );		// ����� �а� �ʱ�ȭ��.
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
		default:	SetError( ERR_FAILED );		break;				// �׿� �Ϲ����� ����
		}
#else
        SetError( ERR_FILE_NOT_FOUND );
#endif
	}
	return (m_fp)? 1: 0;		// �������д� 1,0���θ� �����ϰ� ��ü���� ������ ������ GetError()�� ����ؾ� ��
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
	// Convert_char_To_TCHAR����� ��ũ�Ǿ� �ϴ� ��
//	SetResName( Convert_char_To_TCHAR( szFilename ) );		// ���Ϸε��� �����ϵ� �����ϵ� ���������� �＼���� ���ҽ��̸��� �޵��� �ٲ�.
	SetcResName( szFilename );
	if( m_fp ) 
	{
		SetError( ERR_SUCCESS );
		//
		if( mode == xWRITE )
		{
			if( XCrypto::GetbEncryption() )		// ��ȣȭ �ɼ��� �����ִ°�.
			{
				XCrypto::WriteHeader( m_fp );		// ��ȣȭ�� �ʿ��� ����� ������.
			}
		} else
		if( mode == xREAD )
		{
			if( XCrypto::IsEncryption( m_fp ) )
			{
				if( XCrypto::CheckCheckSum( m_fp ) == FALSE )		// üũ�� �˻�
				{
#ifdef WIN32
					char buff[1024];
					sprintf_s( buff, "%s file is damaged", szFilename );
					MessageBoxA( NULL, buff, "error", MB_OK );
#endif
					exit(1);
				}
				XCrypto::ReadHeader( m_fp );		// ����� �а� �ʱ�ȭ��.
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
		default:	SetError( ERR_FAILED );		break;				// �׿� �Ϲ����� ����
		}
#else
        SetError( ERR_FILE_NOT_FOUND );
#endif
	}
	return (m_fp)? 1: 0;		// �������д� 1,0���θ� �����ϰ� ��ü���� ������ ������ GetError()�� ����ؾ� ��
}
#endif // WIN32

void XResFile::Close() 
{ 
	if( m_fp ) 
	{
		if( m_Mode == XBaseRes::xWRITE )
		{
			// ������� ������ �ݱ����� ��������ġ�� üũ�� �������.
			if( GetbEncryption() )		// ��ȣȭ �����̸�
			{
				XCrypto::WriteCheckSum( m_dwCheckSum, m_fp );
/*				DWORD dwCheckSum;
				XCrypto::Encrypt( (BYTE *)&dwCheckSum, 4, (BYTE *)&m_dwCheckSum, 4 );	
				fwrite( &dwCheckSum, 4, 1, m_fp );		// checksum ����. �ϴ� �������ϵ� ȣȯ�� ������ �ְ� �ϴ� ��ȣȭ���ϸ� üũ�� �����ϵ��� ����.
				*/
				XCrypto::WriteTailer( m_fp );		// ������ ���� �ѹ��� ���� ������.
			}
		}
/*		else
		if( m_Mode == XBaseRes::xREAD )
		{
			if( GetbEncryption() )		// ��ȣȭ �����̸�
			{
				DWORD dwCheckSum;
				fread( &dwCheckSum, 4, 1, m_fp );
				XCrypto::Decrypt( (BYTE *)&dwCheckSum, 4 );		// ptr������ total���̸�ŭ �ص��ؼ� �ٽ� �ִ´�.
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
//						MessageBoxA( NULL, XE::Format( "%s ������ �ջ�Ǿ����ϴ�.", XBaseRes::GetcResName() ), "�˸�", MB_OK );
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
	int ret = fread( ptr, size, n, m_fp);	// �ϴ� ��°�� �� �д´�.
	// ��ȣȭ �� �����̸� �ص��ؼ� ���ۿ� �ٲ�ִ´�.
	if( GetbEncryption() )
	{
		int total = size * n;		// read�� ��ü ũ��
		XCrypto::Decrypt( (BYTE *)ptr, total );		// ptr������ total���̸�ŭ �ص��ؼ� �ٽ� �ִ´�.
		// �̰��� ���۸� �ܺο��� �������ֱ⶧���� �ѹ��� �ص��ص� �ȴ�.
		// üũ�� ���.
		{
			int total = size * n;				// write�ؾ��� ��ü ������
			BYTE *pbyte = (BYTE *)ptr;
			for( int i = 0; i < total; ++i )
				m_dwCheckSum += (DWORD)(*pbyte++);		// �� ����Ʈ�� ��� ����.
		}
	}
	return ret;
}

int XResFile::Write( void *ptr, size_t size, size_t n ) 
{ 
	XBREAK( (int)size < 0 );  
	int total = size * n;				// write�ؾ��� ��ü ������
	// üũ�� ���.
	{
		BYTE *pbyte = (BYTE *)ptr;
		for( int i = 0; i < total; ++i )
			m_dwCheckSum += (DWORD)(*pbyte++);		// �� ����Ʈ�� ��� ����.
	}

	if( GetbEncryption() )
	{
		// ��ȣȭ ������
		// ���۸� 4k������ ��� ��ȣȭ �Ѵ��� write�Ѵ�.
		BYTE buffer[ 4096 ];
		int num = total / 4096;
		int remain = total % 4096;
		BYTE *_ptr = (BYTE *)ptr;
		for( int i = 0; i < num; ++i )
		{
			// 4k������ ptr�� ��ȣȭ �ؼ� buffer�� �ִ´�.
			XCrypto::Encrypt( buffer, sizeof(buffer), _ptr, 4096 );	
			fwrite( buffer, 4096, 1, m_fp );
			_ptr += 4096;
		}
		// 4k������ ���� ������ �κ��� ���� �� ��ȣȭ �Ѵ�.
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

// ��Ʈ���� ������� ��Ʈ���� �����Ѵ�.
int XResFile::WriteString( TCHAR *szStr )
{
	XBREAK( szStr == NULL );
	int size = ((_tcslen( szStr ) + 1) * sizeof(TCHAR));		// ������
	XBREAK( size <= 0 );
	XBREAK( size > 0xffff );		// ��Ʈ���� ����ġ�� �� ���� �����ִ°Ŵ�.
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
	XBREAK( size > 0xffff );		// ��Ʈ���� ����ġ�� �� ���� �����ִ°Ŵ�.
	if( len > 0 )
		Write( &size, 4 );		// ������ ������
	else
		Write( &len, 4 );		// ������ ������
	if( len > 0 )
		Write( cStr, size );		// �� ����.
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


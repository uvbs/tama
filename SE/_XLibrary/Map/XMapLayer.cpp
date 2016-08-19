#include "stdafx.h"
#include "XMapLayer.h"


void XMapLayer::Load( XResFile *pRes, LPCTSTR szPath )
{
	DWORD type;
	pRes->Read( &type, 4 );
	m_Type = (xtType)( type );
	pRes->Read( &m_idLayer, 4 );
	int size;
	pRes->Read( &size, 4 );
	UNICHAR szBuff[ 32 ];
	XBREAK( size <= 0 );
	XBREAK( size >= sizeof( szBuff ) );
	pRes->Read( szBuff, size );
//#ifdef WIN32
//	_tcscpy_s( m_szName, szBuff );
//#else
//	_tcscpy_s( m_szName, _ConvertUTF16ToUTF8( szBuff ) );
//#endif
//	if( XE::IsEmpty( m_szName ) )
//		_tcscpy_s( m_szName, _T("Layer 1") );		// �̸��� ������ ����Ʈ �̸�.
	pRes->Read( &m_vDrawOffset.x, 4 );
	pRes->Read( &m_vDrawOffset.y, 4 );
	BYTE b;
	pRes->Read( &b, 1 );		m_bVisible = b;
	pRes->Read( &b, 1 );		m_bShowMask = b;
	pRes->Read( &b, 1 );
	pRes->Read( &b, 1 );
	DWORD reserved[4];
	pRes->Read( reserved, sizeof(DWORD) * 4 );
}
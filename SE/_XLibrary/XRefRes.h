#pragma once
#include "global.h"

class XRefRes
{
	int m_nRefCnt;		// ���۷��� ī��Ʈ
	TCHAR m_szName[ 64 ];	// ���ϸ� ������
	void Init() {
		m_nRefCnt = 0;
		XCLEAR_ARRAY( m_szName );
	}
	void Destroy() {}
public:
	XRefRes( LPCTSTR szName ) { 
		Init(); 
		_tcscpy_s( m_szName, szName );
	}
	virtual ~XRefRes() { Destroy(); }

	GET_ACCESSOR( int, nRefCnt );
	void IncRefCnt( void ) { ++m_nRefCnt; }
	void DecRefCnt( void ) { --m_nRefCnt; }
	GET_ACCESSOR( LPCTSTR, szName );
//	void SetszFile( LPCTSTR szName ) {
//		_tcscpy_s( m_szName, szName );
//	}
};


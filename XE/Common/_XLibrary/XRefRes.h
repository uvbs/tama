#pragma once
#include "etc/global.h"

class XRefRes
{
	int m_nRefCnt;		// 레퍼런스 카운트
	_tstring m_strRes;	// 로컬 패스+파일명
	XUINT64 m_id64Res;
	void Init() {
		m_nRefCnt = 0;
		m_id64Res = 0;
	}
	void Destroy() {}
protected:
	int DecRefCnt( void ) { return --m_nRefCnt; }
public:
	XRefRes() { Init(); }
	XRefRes( LPCTSTR szRes ) {
		Init();
		SetstrRes( szRes );
	}
	virtual ~XRefRes() { Destroy(); }
	// 특수한 상황에서 강제로 레퍼런스 카운트를 클리어 시킨다.
	void _ClearRefCnt() {
		m_nRefCnt = 0;
	}
	void IncRefCnt( void ) { ++m_nRefCnt; }
	GET_ACCESSOR( int, nRefCnt );
	// 엔진제작자외엔 사용하지 말것.
	void _SetrefCnt( int refCnt ) {
		m_nRefCnt = refCnt;
	}
	GET_ACCESSOR( const _tstring&, strRes );
	GET_SET_ACCESSOR( XUINT64, id64Res );
	void SetstrRes( LPCTSTR szRes ) {
		if( szRes ) {
			TCHAR szTemp[ 256 ];
			_tcscpy_s( szTemp, szRes );
			_tcslwr_s( szTemp );
			m_strRes = szRes;
			// 소문자로 바꿔서 체크섬을 계산한다.
			m_id64Res = XE::GetCheckSum( szTemp );
		}
	}
	BOOL IsHavestrRes( void ) {
		return (m_strRes.empty() == false);
	}
	// m_strRes중에 파일명만 리턴한다.
	_tstring GetFilename( void ) {
		_tstring strFilename = XE::GetFileName( m_strRes.c_str() );
		return strFilename;
	}
friend class XImageMng;
friend class XFontMng;
};


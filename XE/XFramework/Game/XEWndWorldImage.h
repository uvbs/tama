#pragma once
#include "XEWndWorld.h"

////////////////////////////////////////////////////////////////
/**
월드배경 통짜이미지 버전
*/
class XEWndWorldImage : public XEWndWorld
{
	XSurface *m_psfcBg;		// 배경 통짜이미지
	_tstring m_strResBg;	// 서브폴더이름을 포함한 배경파일명
	void Init() {
		m_psfcBg = NULL;
	}

	void Destroy();
public:
	XEWndWorldImage( LPCTSTR resImg );
	virtual ~XEWndWorldImage() { Destroy(); }
	//
	virtual BOOL OnCreate( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	/**
	 이 클래스를 상속받는 클래스는 월드생성 함수를 반드시 구현해야 한다.
	 vwSize = 배경이미지의 논리적 크기
	*/
	virtual XEWorld* OnCreateWorld( const XE::VEC2& vwSize ) = 0;
	XSurface* LoadImg( LPCTSTR resImg );
};

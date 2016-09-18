#pragma once
#include "XEWndWorld.h"

class XScroll;
class XEWorldCamera;
////////////////////////////////////////////////////////////////
/**
월드배경 전용레이어(통짜이미지 버전)
*/
class XEWndWorldImage : public XWnd // public XEWndWorld
{
	XSurface *m_psfcBg;		// 배경 통짜이미지
//	_tstring m_strResBg;	// 서브폴더이름을 포함한 배경파일명
	XSPWorld m_spWorld;
	std::shared_ptr<XEWorldCamera> m_spCamera;		// 바인딩된 카메라
//	XRenderCmdMng* m_pRenderer = nullptr;
//	XTextureAtlas* m_pAtlas = nullptr;
	XScroll* m_pScroller = nullptr;
	void Init() {
		m_psfcBg = NULL;
	}

	void Destroy();
public:
	XEWndWorldImage( XSurface* psfcBG, XSPWorld spWorld );
	virtual ~XEWndWorldImage() { Destroy(); }
	//
	GET_SET_ACCESSOR( std::shared_ptr<XEWorldCamera>, spCamera );
	void Draw() override;
	/**
	 이 클래스를 상속받는 클래스는 월드생성 함수를 반드시 구현해야 한다.
	 vwSize = 배경이미지의 논리적 크기
	*/
//	virtual XEWorld* OnCreateWorld( const XE::VEC2& vwSize ) = 0;
	XE::VEC2 GetvwSizeWorld() const;
private:
	XSurface* LoadImg( LPCTSTR resImg );
};

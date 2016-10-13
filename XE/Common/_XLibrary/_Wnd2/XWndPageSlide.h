/********************************************************************
	@date:	2014/11/17 17:23
	@file: 	C:\xuzhu_work\Project\iPhone\XE\Common\_XLibrary\_Wnd2\XWndPageSlide.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XWnd.h"
#include "XFramework/client/XLayout.h"

//////////////////////////////////////////////////////////////////////////
class XWndPageForSlide : public XWnd
{
	int m_idxPage;			// 페이지 인덱스
	void Init() {
		m_idxPage = 0;
	}
	void Destroy() {}
public:
	XWndPageForSlide( int idxPage, const XE::VEC2& vPos, const XE::VEC2& vSize ) 
	: XWnd( vPos.x, vPos.y, vSize.w, vSize.h ) { 
		Init(); 
		m_idxPage = idxPage;
	}
	virtual ~XWndPageForSlide() { Destroy(); }
	//
	GET_ACCESSOR( int, idxPage );
};

//////////////////////////////////////////////////////////////////////////
class XWndPageSlideWithXMLDelegate : public XBaseDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XWndPageSlideWithXMLDelegate() { Init(); }
	virtual ~XWndPageSlideWithXMLDelegate() { Destroy(); }
	//
	virtual int GetNumPagesForSlide( const XLayout::xATTR_ALL& attr ) { return 0; }
	// 시작페이지를 몇페이지로 할것인가.(0~)
	virtual int GetStartPageIndexForSlide( const XLayout::xATTR_ALL& attr ) { return 0; }
	virtual void OnFinishCreatePageLayout( XWndPageForSlide *pPage ) {}
};

class XWndPageSlideWithXML : public XWnd
{
	BOOL m_bModeXML;		
	XLayout *m_pLayout;
	TiXmlElement *m_elemSlide;
	XWndPageForSlide *m_pLeft;
	XWndPageForSlide *m_pCurr;
	XWndPageForSlide *m_pRight;
	XList<XWndPageForSlide*> m_listLeft;	//그리지 않는 왼쪽 페이지
	XList<XWndPageForSlide*> m_listRight;	//그리지 않는 오른쪽 페이지
//	XList<string> m_listPageKey;		// 페이지 문자열
	XE::xtDir m_AutoScrollDir;			// 자동스크롤 방향
	XE::VEC2 m_vStartScroll;	// 자동스크롤 시작위치
	XE::VEC2 m_vPrev;			
	BOOL m_bTouchDown;		// 
	XInterpolationObjDeAccel m_itpScroll;
	XSurface *m_sfcPointOn;
	XSurface *m_sfcPointOff;
	XE::VEC2 m_posPagePoint;		// 페이지포인트 시작위치
	float m_distPagePoint;		// 페이지포인트 간격
	std::string m_strPageLayout;	// 페이지 레이아웃 노드 이름.
	int m_numPages;				// 전체 페이지 수.
	BOOL m_bScrollExclusive;		// 스크롤 독점모드.
	BOOL m_bLockSlide;			// 슬라이드 잠금(버튼을 이용한 슬라이드 만들때 사용)
	XWndPageSlideWithXMLDelegate *m_pDelegate;
	void Init() {
		m_bModeXML = FALSE;
		m_pLayout = NULL;
		m_elemSlide = NULL;
		m_pLeft = m_pCurr = m_pRight = NULL;
		m_AutoScrollDir = XE::xDIR_NONE;
		m_bTouchDown = FALSE;
		m_sfcPointOn = m_sfcPointOff = NULL;
		m_distPagePoint = 25.f;
		m_numPages = 0;
		m_bScrollExclusive = FALSE;
		m_pDelegate = NULL;
		m_bLockSlide = FALSE;
	}
	void Destroy();
public:
	/**
	szImgPoint: 현재 페이지의 위치를 보여주는 포인트이미지 파일명. 파일명.png의 형식
					으로 넣으면 파일명_on/off.png형태로 자동으로 바꿔준다.
	*/
	XWndPageSlideWithXML( const XE::VEC2& vPos, 
							const XE::VEC2& vSize, 
							XLayout* pLayout, 
							TiXmlElement *elemSlide,
							LPCTSTR szImgPoint=NULL );
	virtual ~XWndPageSlideWithXML() { Destroy(); }
	//
	GET_SET_ACCESSOR( BOOL, bModeXML );
	GET_SET_ACCESSOR( BOOL, bLockSlide );
	GET_SET_ACCESSOR( const std::string&, strPageLayout );
	GET_ACCESSOR( int, numPages );
	void SetnumPages( int numPages );
	GET_SET_ACCESSOR( XWndPageSlideWithXMLDelegate*, pDelegate );
	int GetNextPage( int idxBase );
	int GetPrevPage( int idxBase );
	XWndPageForSlide* GetCurrPage( void ) {
		return m_pCurr;
	}
	XWndPageForSlide* GetRightPage( int idx = 0 );
	XWndPageForSlide* GetLeftPage( int idx = 0 );

//	int AddPageKey( const char *cKeyPage );
//	void _SetCurrPageForXML( int idxPage );
	void SetCurrPage( int idxPage );

	XWndPageForSlide* _CreatePageForXML( int idxPage );
//	XWndPageForSlide* CreatePageForNormal( int idxPage );
	XWndPageForSlide* CreatePage( int idxPage );
/*	XWndPageForSlide* CreatePage( int idxPage ) {
		if( m_bModeXML )
			return CreatePageForXML( idxPage );

		return CreatePageForNormal( idxPage );
	}*/
	void DoAutoScroll( XE::xtDir dir, const XE::VEC2& vStart );
	//
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnLButtonDownCancel( void );
	virtual int Process( float dt );
	virtual void Draw( void );
};


#pragma once
#include "XWndView.h"

//////////////////////////////////////////////////////////////////////////
class XWndPopup : public XWndView
{
	XE::VEC2 m_vLTMargin;	// 왼쪽, 위쪽 마진
	XE::VEC2 m_vRBMargin;	// 오른쪽 아래 마진
	XList4<XWndButton*> m_listButtons;
	XLayoutObj* m_pLayoutObj = nullptr;
	bool m_bNCTouch = false;
	void Init() {
		XWnd::SetwtWnd( XE::WT_POPUP );
		m_vLTMargin = XE::VEC2( 17, 6 );
		m_vRBMargin = XE::VEC2( 17, 0 );
		SetbModal( TRUE );
	}
	void Destroy();
public:
	XWndPopup() { 
		Init(); 
		SetEnableNcEvent( TRUE );
	}
	// szFrame: 파일명만 읽는다. 자동으로 UI폴더에서 읽는다.
	XWndPopup( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szFrame ) 
	: XWndView( vPos.x, vPos.y, vSize.w, vSize.h, szFrame ) { 
		Init(); 
		SetEnableNcEvent( TRUE );
	}
	XWndPopup( const XE::VEC2& vSize, LPCTSTR szResFrame ) 
		: XWndView( 0.f, 0.f, vSize.w, vSize.h, szResFrame ) { 
			Init(); 
			XE::VEC2 vSizeAligned = GetSizeLocal();	// vSize와  vSizeAligned는 다를수 있다. 타일단위로 정렬된 크기이기 때문이다.
			XE::VEC2 vPos = XE::GetGameSize() / 2.f - vSizeAligned /2.f;
			SetPosLocal( vPos );
			SetEnableNcEvent( TRUE );
	}
	XWndPopup( XLayout *pLayout, const char *cKey, const char *cGroup=nullptr );
	XWndPopup( LPCTSTR szLayout, const char *cNodeName, XWnd *pParent = nullptr );
	XWndPopup( const _tstring& strXml, const std::string& strcNode, XWnd *pParent = nullptr )
		: XWndPopup( strXml.c_str(), strcNode.c_str(), pParent ) {}
	XWndPopup( float x, float y, LPCTSTR szImg )
		: XWndView( x, y, szImg ) {
		Init();
		XE::VEC2 vSizeParent = XE::GetGameSize();
		XE::VEC2 vSizePopup = GetSizeLocal();
		XE::VEC2 vLT = ( vSizeParent / 2.f ) - ( vSizePopup / 2.f );
		SetPosLocal( vLT );
		SetEnableNcEvent( TRUE );
	}
	virtual ~XWndPopup() { Destroy(); }
	//
	GET_SET_ACCESSOR( const XE::VEC2&, vLTMargin );
	GET_SET_ACCESSOR( const XE::VEC2&, vRBMargin );
	XLayout* GetpLayout();
	/**
	 버튼과 아래쪽 프레임간의 간격을 지정한다. 타일방식 프레임일경우 타일의 모양에 따라 더 여유가 생길수 있으니 유념할것.
	*/
	void SetBottomMargin( float margin ) {
		m_vRBMargin.h = margin;
	}
	void SetLeftMargin( float margin ) {
		m_vLTMargin.w = margin;
	}
	void SetRightMargin( float margin ) {
		m_vRBMargin.w = margin;
	}
/*
	int OnOk() {
		SetbDestroy( TRUE );
		return 1;
	}
*/
	virtual int OnOk( XWnd *pWnd, DWORD p1, DWORD p2 );
	virtual int OnCancel( XWnd *pWnd, DWORD p1, DWORD p2 );
	virtual int OnYes( XWnd *pWnd, DWORD p1, DWORD p2 );
	virtual int OnNo( XWnd *pWnd, DWORD p1, DWORD p2 );
	BOOL OnKeyUp( int keyCode );
	XWndButton* AddButtonOk( LPCTSTR szText, LPCTSTR szButtPng ) {
		return AddButton( XWnd::xOK, szText, szButtPng );
	}
	XWndButton* AddButtonCancel( LPCTSTR szText, LPCTSTR szButtPng ) {
		return AddButton( XWnd::xCANCEL, szText, szButtPng );
	}
	XWndButtonString* AddButton( const char *cIdentifier,
								LPCTSTR szLabel,
								LPCTSTR szFont,
								float sizeFont, 
								LPCTSTR szPngHeader );
	XWndButtonString* AddButton( XWnd::xtAlert type, 
								LPCTSTR szLabel,
								LPCTSTR szFont,
								float sizeFont,
								LPCTSTR szPngHeader );
	XWndButton* AddButton( XWnd::xtAlert type, 
								LPCTSTR szLabel,
								LPCTSTR szButtPng );
	XWndButtonString* AddButtonImg( const char *cIdentifier, 
								LPCTSTR szLabel, 
								LPCTSTR szFont,
								float sizeFont, 
								LPCTSTR szImg );
	void AdjustButtons();
	XWndButton* GetButton( const char *csid );
	XWndButton* SetCancelButton( const char *cKey );
	XWndButton* SetOkButton( const char *cKey );
	void OnNCLButtonDown( float lx, float ly ) override;
	void OnNCLButtonUp( float lx, float ly ) override;
	void SetLayout( const _tstring& strtXml, const std::string& strcNode, XWnd* pParent );
};


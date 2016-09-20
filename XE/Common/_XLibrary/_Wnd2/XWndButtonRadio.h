#pragma once
#include "XWndButton.h"
// #include "XFontSpr.h"
// #include "XFontMng.h"
// #include "sprite/SprObj.h"
//#include "XWndText.h"
//#include "XWndCtrls.h"

#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

////////////////////////////////////////////////////////////////////////
// 라디오 버튼
class XWndButtonRadio : /*virtual */public XWndButton
{
//	XWndRadioGroup *m_pGroup = nullptr;
//	XWndTextString *m_pText = nullptr;
	void Init() {
	}
	void Destroy() {
	}
public:
// 	XWndButtonRadio( float x, float y, LPCTSTR szSpr, ID idUp, ID idDown, ID idDisable=0 ) 
// 		: XWnd( x, y ),
// 		XWndButton( x, y, szSpr, idUp, idDown, idDisable ) {
// 		Init();
// 	}
	XWndButtonRadio( float x, float y, LPCTSTR szImgUp, LPCTSTR szImgDown, LPCTSTR szImgDisable=NULL ) 
		: XWnd( x, y ),
		XWndButton( x, y, szImgUp, szImgDown, szImgDisable ) {
		Init();
	}
	XWndButtonRadio( ID idGroup, const XE::VEC2& vPos )
		: XWnd( vPos )
		, XWndButton( vPos ) {
		Init();
		SetidGroup( idGroup );
	}
// 	XWndButtonRadio( float x, float y, 
// 					LPCTSTR szText, 
// 					XCOLOR colText, 
// 					XBaseFontDat *pFontDat, 
// 					LPCTSTR szImgUp, 
// 					LPCTSTR szImgDown, 
// 					LPCTSTR szImgDisable=NULL );
	XWndButtonRadio( ID idGroup,
					float x, float y, 
					LPCTSTR szImgUp, 
					LPCTSTR szImgDown, 
					LPCTSTR szImgDisable=NULL ) 
		: XWnd( x, y ),
		XWndButton( x, y, szImgUp, szImgDown, szImgDisable ) {
			m_idGroup = idGroup;
			Init();
	}
	XWndButtonRadio( ID idGroup,
									 const XE::VEC2& vPos,
									 LPCTSTR szImgUp,
									 LPCTSTR szImgDown,
									 LPCTSTR szImgDisable = NULL )
		: XWndButtonRadio( idGroup, vPos.x, vPos.y, szImgUp, szImgDown, szImgDisable ) {}
// 	XWndButtonRadio( ID idGroup,		// 라디오 그룹 아이디. 같은 그룹내 라디오버튼들은 동일한 아이디를 부여해야 한다.
// 					float x, float y, 
// 					LPCTSTR szText, 
// 					XCOLOR colText, 
// 					LPCTSTR szFont,
// 					float sizeFont,
// 					LPCTSTR szImgUp, 
// 					LPCTSTR szImgDown, 
// 					LPCTSTR szImgDisable=NULL );
	virtual ~XWndButtonRadio() { Destroy(); }
//	SET_ACCESSOR( XWndRadioGroup*, pGroup );
//	GET_ACCESSOR( XWndTextStringArea*, pTextArea );
// 	void SetStyle( xFONT::xtStyle style ) {
// 		if( m_pText )
// 			m_pText->SetStyle( style );
// 	}
	//
	virtual void OnMouseMove( float lx, float ly ) {}
	virtual void OnLButtonDown( float lx, float ly ) { 
		m_bFirstPush = TRUE;
	}
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly ) {}	// 상위 함수를 처리하지 못하게함
	virtual void OnNCLButtonUp( float lx, float ly ) ;
	virtual void Draw();
};

#pragma warning ( default : 4250 )


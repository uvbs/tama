/********************************************************************
	@date:	2015/01/19 18:24
	@file: 	C:\xuzhu_work\Project\iPhone\XE\Common\_XLibrary\_Wnd2\XWndView.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XWnd.h"
//#include "XFramework/client/XLayoutObj.h"

class XWndList;
class XWndDarkBg;
class XLayoutObj;
//-------------------------------------------------------------------------------------
class XWndView : public XWnd
{
public:
// 	static XWndDarkBg *s_pWndDarkBg;
private:
	XLua *m_prefLua;
	char m_cDrawHandler[ 32 ];	// lua draw handler
	float m_dt;
	BOOL m_bDrawFrame;
	CTimer m_timerAppear;	// 나타날때 타이머
	XE::VEC2 m_vLT;				// 스케일 1.0일때 좌상귀 좌표
	XCOLOR m_colBg;
	XCOLOR m_colBorder;
	bool m_bNcClose = false;		// 영역 밖을 누르면 자동으로 this가 닫힌다.
	bool m_bNcTouched = false;
#ifdef _CHEAT
	CTimer m_timerSpeed;		// 로딩속도 테스트용
	DWORD m_timeLoad;
#endif
	void Init() {
		XWnd::SetwtWnd( XE::WT_VIEW );
		m_prefLua = NULL;
		XCLEAR_ARRAY( m_cDrawHandler );
		strcpy_s( m_cDrawHandler, "OnDrawView" );		// default handler name
		m_dt = 1.0f;
		m_bDrawFrame = FALSE;
		//		m_timerAppear.SetRealTimer( TRUE );
		m_nStateAppear = 0;
		m_nBlockSize = 0;
		m_psfcFrame = NULL;
		m_colBg = 0;
		m_colBorder = 0;
		SetbAnimationEnterLeave( TRUE );		// 기본적으론 등장 애니메이션 있음.
#ifdef _CHEAT
		m_timeLoad = 0;
#endif
	}
	void Destroy();
protected:
	int m_nBlockSize;
	XWND_RES_FRAME m_frameImage;
	int m_nStateAppear;		// 나타날때 처리중 다용도 상태
	XSurface *m_psfcFrame;	// 뷰 바탕 이미지가 따로 있을때 지정
public:
	XWndView() { Init(); }
	XWndView( float x, float y, float w, float h, const char *cDrawHandler, XLua *pLua )
		: XWnd( x, y, w, h ) {
		Init();
		if( cDrawHandler )
			strcpy_s( m_cDrawHandler, cDrawHandler );
		m_prefLua = pLua;
	}
	// szFrame: 파일명만 넣는다. 자동으로 UI폴더에서 읽는다.
	XWndView( float x, float y, float w, float h, LPCTSTR szFrame )
		: XWnd( x, y, w, h ) {
		Init();
		if( XE::IsHave(szFrame) )
			LoadRes( szFrame );
	}
	XWndView( float x, float y, LPCTSTR szImg )
		: XWnd( x, y ) {
		Init();
		SetBgImg( szImg );
	}
	XWndView( LPCTSTR szImg );
	XWndView( const XE::VEC2& vPos, const XE::VEC2& vSize, XCOLOR colBg, XCOLOR colBorder = XCOLOR_WHITE )
		: XWnd( vPos.x, vPos.y, vSize.w, vSize.h ) {
		Init();
		m_colBg = colBg;
		m_colBorder = colBorder;
	}
	XWndView( XLayout *pLayout, const char *cKey, const char *cGroup );
	XWndView( XLayout *pLayout, const char *cNodeName );
	XWndView( const XE::VEC2& vPos, const XE::VEC2& vSize )
		: XWnd( vPos, vSize ) { 
		Init(); 
	};
//	XWndView( LPCTSTR szLayout, const char *cNodeName );
	virtual ~XWndView() { Destroy(); }
	GET_SET_ACCESSOR( BOOL, bDrawFrame );
	void SetbNcClose( bool bFlag ) {
		SetEnableNcEvent( bFlag );
		m_bNcClose = bFlag;
	}
	bool IsAnimation() override { 
		return m_nStateAppear != 1; 
	}
	float GetSizeFrameBlock() {
		return (float)m_frameImage.nBlockSize;
	}
	//
	BOOL LoadRes( LPCTSTR szRes );
	/**
	 @brief view의 배경이미지를 설정한다.
	*/
	void SetBgImg( LPCTSTR szImg, XE::xtPixelFormat formatSurface );
	inline void SetBgImg( LPCTSTR szImg ) {
		SetBgImg( szImg, XE::xPF_ARGB4444 );
	}
	virtual int Process( float dt );
	virtual void Draw();
	virtual void ProcessAnimationLerp( BOOL bEnter, float timeLerp, float aniLerp );
	virtual BOOL ProcessEnterLeaveAnimation( float dt );
	void OnDestroy() override;
	//	virtual void OnAddWnd();
	void OnFinishAppear() override;
	BOOL OnCreate() override;
	// vSizeOrig을 32단위로 grid화 시켜서 this의 사이즈로 세팅한다.
	XE::VEC2 SetSizeByGrid( const XE::VEC2& vSizeOrig ) {
		const auto vAligned = vSizeOrig.RoundUpDiv( 32 );
		SetSizeLocal( vAligned );
		return vAligned;
	}
	void OnFinishCreatedChildLayout( XLayout *pLayout ) override;
	void Update() override;
	void OnNCLButtonDown( float lx, float ly ) {
		if( m_bNcClose )
			m_bNcTouched = true;
		XWnd::OnNCLButtonDown( lx, ly );
	}
	void OnNCLButtonUp( float lx, float ly ) {
		if( m_bNcClose && m_bNcTouched )
			SetbDestroy( true );
		XWnd::OnNCLButtonUp( lx, ly );
	}
};	// XWndView

//}
class XWndDialog : public XWndView
{
public:
private:
	void Init() {
		m_pFontDat = NULL;
		m_pFont = NULL;
	}
	void Destroy() {
		SAFE_RELEASE2( FONTMNG, m_pFontDat );
		SAFE_DELETE( m_pFont );
	}
protected:
	XBaseFontDat *m_pFontDat;
	XBaseFontObj *m_pFont;
public:
	XWndDialog() {}
	template<typename T>
	XWndDialog( T x, T y, T width, T height, LPCTSTR szRes, LPCTSTR szFont )
		: XWndView( x, y, width, height ) {
		Init();
		LoadRes( szRes );
		m_pFontDat = FONTMNG->Load( szFont, 20.f );
		m_pFont = m_pFontDat->CreateFontObj();
		SetDialogSize( width, height );
	}
	template<typename T>
	XWndDialog( T width, T height, LPCTSTR szRes, LPCTSTR szFont )
		: XWndView() {
		Init();
		LoadRes( szRes );
		m_pFontDat = FONTMNG->Load( szFont, 20.f );
		m_pFont = m_pFontDat->CreateFontObj();
		SetDialogSize( width, height );

	}
	XWndDialog( LPCTSTR szRes, LPCTSTR szFont )
		: XWndView() {
		Init();
		LoadRes( szRes );
		m_pFontDat = FONTMNG->Load( szFont, 20.f );
		m_pFont = m_pFontDat->CreateFontObj();
	}
	XWndDialog( LPCTSTR szImg );
	virtual ~XWndDialog() { Destroy(); }
	// get/set
	template<typename T>
	void SetDialogSize( T w, T h ) {
		float nw = (float)w, nh = (float)h;
		if( m_nBlockSize > 0 )
		{
			nw = (float)( m_nBlockSize * ( (int)w / m_nBlockSize ) );
			nh = (float)( m_nBlockSize * ( (int)h / m_nBlockSize ) );	// 블럭사이즈에 떨어지도록 크기를 조정한다
		}
		SetSizeLocal( nw, nh );
		SetPosLocal( XE::GetGameWidth() / 2 - (int)nw / 2, XE::GetGameHeight() / 2 - (int)nh / 2 );
	}
	//
	//    BOOL LoadRes( LPCTSTR szRes );
	//	XWndButton* AddButton( ID id, float x, float y, LPCTSTR szText, LPCTSTR szSpr, ID idActUp, ID idActDown=0 );
	//	XWndButton* AddButton( ID id, const XE::VEC2& vPos, const XE::VEC2& size, LPCTSTR szText );
//	XWndButton* AddButton( ID id, float x, float y, LPCTSTR szSpr, ID idActUp, ID idActDown );
	XWnd* AddStaticText( ID id, float x, float y, float w, float h, LPCTSTR szText, XBaseFontDat *pFontDat, XE::xAlign align, XCOLOR col = XCOLOR_WHITE, xFONT::xtStyle style = xFONT::xSTYLE_NORMAL );
	XWnd* AddStaticText( float x, float y, LPCTSTR szText, XBaseFontDat *pFontDat, XE::xAlign align, XCOLOR col = XCOLOR_WHITE, xFONT::xtStyle style = xFONT::xSTYLE_NORMAL ) {
		return AddStaticText( XE::GenerateID(), x, y, 0, 0, szText, pFontDat, align, col, style );
	}
	XWnd* AddButton( XWnd::xtAlert type, XWnd *pButt ) {
		XWnd *pAdd = Add( (ID)type, pButt );
		if( type == XWnd::xOK )
			pButt->SetEvent( XWM_CLICKED, this, &XWndDialog::OnOk );
		else if( type == XWnd::xCANCEL )
			pButt->SetEvent( XWM_CLICKED, this, &XWndDialog::OnCancel );
		return pAdd;
	}
	//
	virtual int Process( float dt );
	virtual void Draw();
	virtual void OnLButtonUp( float lx, float ly );
	virtual int OnOk( XWnd*, DWORD, DWORD );
	virtual int OnCancel( XWnd*, DWORD, DWORD );
};	// XWndDialog

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/07/05 14:59
*****************************************************************/
class XWndDarkBg : public XWnd
{
public:
public:
	XWndDarkBg() : XWnd(XE::VEC2(0), XE::GetGameSize() ) { 
		Init(); 
	}
	virtual ~XWndDarkBg() { Destroy(); }
	// get/setter
	GET_ACCESSOR( float, Alpha );
	// public member
	void DoDarken() {
		m_State = 0;
		m_Alpha = 0.f;
	}
	void DoLighten() {
		m_State = 2;
		m_Alpha = c_maxAlpha;
	}
	void AddRefCnt() {
		++m_cntRef;
	}
	int DecRefCnt() {
		return --m_cntRef;
	}

	// private member
private:
	CTimer m_timerAlpha;
	int m_State = 0;	// 0:나타나는상태 1:나타나있는상태 2:사라지는상태 3.완전히사라진상태
	float m_Alpha = 0.f;
	int m_cntRef = 0;
	const float c_maxAlpha = 0.8f;
	// private method
private:
	void Init() {}
	void Destroy() {}
	virtual int Process( float dt );
	virtual void Draw() override {
		BYTE a = (BYTE)( m_Alpha * 255 );
		GRAPHICS->FillRectSize( XE::VEC2( 0 ), XE::GetGameSize(), XCOLOR_RGBA( 0, 0, 0, a ) );
	}
}; // class XWndDarkBg

#pragma once
#include "XWnd.h"
#include "XFontSpr.h"
#include "XFontMng.h"
#include "SprObj.h"
#include "XFactory.h"

#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

class XWndMng;
class XSprObj;
////////////////////////////////////////////////////////////////////////
class XWndButton : virtual public XWnd
{
public:
	static XWndButton* Find( XWndMng *pWndMng, ID idWnd );
private:
	XSprObj *m_pSprObj;
	void Init( void ) {
		m_idActDisable = m_idActDown = m_idActUp = m_idActLock = 0;
		m_bPush = m_bEdge = m_bFirstPush = FALSE;
		m_bSprNum = FALSE;
		m_pSprObj = NULL;	
		m_bLock = FALSE;
		m_bPixelCheck = FALSE;
	}
	void Destroy( void ) { 
		SAFE_DELETE( m_pSprObj );
	}
protected:
	BOOL m_bPush, m_bEdge, m_bFirstPush;
	DWORD m_idActUp;		// 안눌려졌을때 액션번호
	DWORD m_idActDown;		// 눌려졌을때 액션번호
	DWORD m_idActDisable;	// 비활성화 됐을때 액션번호
	DWORD m_idActLock;		// 잠겨있을때 액션번호
	BOOL m_bSprNum;
	BOOL m_bLock;				// 현재 버튼이 잠겨있는가.
	BOOL m_bPixelCheck;		// 버튼이 눌릴때 이미지픽셀단위로 검사한다.
public:
	XWndButton() {}
	// 템플릿으로 하려했더니 헤더에 넣어야 하고 그럼  SprObj.h도 인클루드 해야해서 포기
	XWndButton( float x, float y, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, DWORD idActDisable = 0, BOOL bSprNum = FALSE, XToolTip *pToolTip=NULL );
	XWndButton( float x, float y, XSprObj *pSprObj, DWORD idActUp, DWORD idActDown = 0, DWORD idActDisable = 0 );			// KeepSrc된 SprObj가 필요할때 외부에서 생성시킨걸 쓴다
	XWndButton( float x, float y, float w, float h, XToolTip *pToolTip=NULL );
	void Create( float x, float y, XSprObj *pSprObj, DWORD idActUp, DWORD idActDown, DWORD idActDisable ) ;
	virtual ~XWndButton() { Destroy(); }


	void SetPush( BOOL bFlag ) { m_bPush = bFlag; }
	BOOL GetPush( void ) { return m_bPush; }
	GET_ACCESSOR( DWORD, idActUp );
 	GET_ACCESSOR( DWORD, idActDown );
	GET_ACCESSOR( DWORD, idActDisable );
	GET_ACCESSOR( BOOL, bSprNum );
	GET_ACCESSOR( XSprObj*, pSprObj );
	GET_SET_ACCESSOR( BOOL, bPixelCheck );
	void SetidActLock( DWORD idActLock ) {
		m_idActLock = idActLock;
	}
	GET_ACCESSOR( BOOL, bLock );
	void SetbLock( BOOL bFlag ) {
		m_bLock = bFlag;
		if( bFlag )
		{
			XBREAK( m_idActLock == 0 );		// id가 미리 지정되어 있어야 한다.
			SetbEnable( FALSE );		// 락이 걸리면 자동으로 disable이 된다.
		}
		else
			SetbEnable( TRUE );			// 락이 풀리면 자동으로 enable이 된다.
	}

	void CancelPush( void ) { m_bPush = m_bFirstPush = FALSE; } // OnLButtonDown()에서 한 눌림처리를 취소시킨다. 버튼 안눌린걸로 해야하는데 XWndButton::OnLButtonDown()은 불러야 할때 사용
	// virtual
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual BOOL IsWndAreaIn( float lx, float ly );
	// handler
	virtual XWnd* OnLButtonDown( float lx, float ly );
	virtual XWnd* OnMouseMove( float lx, float ly );
	virtual ID		  OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly );
	virtual void OnNCLButtonUp( float lx, float ly );
	virtual void OnPlaySound( ID id ) { 
		SOUNDMNG->SetdistListener( 0 );
		SOUNDMNG->OpenPlaySound( id ); 	
	}
protected:
	virtual void OnPushDown( void ) {}
}; // class XWndButton
////////////////////////////////////////////////////////////////////////
class XWndDynamicButton : public XWndButton
{
private:
	void Init( void ) {	}
	void Destroy( void ) { 	}
protected:
public:
	// 템플릿으로 하려했더니 헤더에 넣어야 하고 그럼  SprObj.h도 인클루드 해야해서 포기
	XWndDynamicButton( float x, float y, float w, float h, XToolTip *pToolTip=NULL ) 
		: XWnd( NULL, x, y, w, h ), 
		XWndButton( x, y, w, h, pToolTip )
	{
		Init();
	}
	virtual ~XWndDynamicButton() { Destroy(); }

	// virtual
	virtual void Draw( void );
}; // class XWndDynamicButton
////////////////////////////////////////////////////////////////////////
// check button
class XWndButtonCheck : virtual public XWndButton
{
	void Init( void ) {}
	void Destroy( void ) { 	}
public:
	template<typename T>
	XWndButtonCheck( T x, T y, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, BOOL bPush=FALSE ) 
		: XWnd( NULL, x, y ), XWndButton( x, y, szSpr, idActUp, idActDown )  {
		Init();
		m_bPush = bPush;
	}
	virtual ~XWndButtonCheck() { Destroy(); }
	//
	BOOL GetCheck( void ) { return GetPush(); }
	void SetCheck( BOOL bFlag ) { SetPush( bFlag ); }
	//
	virtual XWnd* OnMouseMove( float lx, float ly );
	virtual XWnd* OnLButtonDown( float lx, float ly );
	virtual ID OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly ) {}	// 상위 함수를 처리하지 못하게함
	virtual void OnNCLButtonUp( float lx, float ly ) ;
};


////////////////////////////////////////////////////////////////////////
class XWndTextNumber : virtual public XWnd
{
private:
	XE::VEC2 m_vOffset;		// 기준좌표에서 얼만큼 떨어져 찍힐건지 좌표
	float *m_pNumber;		// 숫자가 있는 변수의 주소
	XBaseFontObj *m_pFont;
	XBaseFontDat *m_pFontDat;
	void Init( void ) {
		m_pNumber = NULL;
		m_pFont = NULL;
		m_pFontDat = NULL;
	}
	void Destroy( void ) { 	
		SAFE_DELETE( m_pFont );
		SAFE_RELEASE2( FONTMNG, m_pFontDat );
	}
protected:
public:
	XWndTextNumber() { Init(); }
	XWndTextNumber( float *pNumber, XBaseFontDat *pDat ) {
		Init();
		m_pNumber = pNumber;
		m_pFont = FACTORY->CreateFontObj( pDat );
		SetOffset( GetWidthLocal() / 2.f, GetHeightLocal() / 2.f );	// default offset
	}
	virtual ~XWndTextNumber() { Destroy(); }
	// 
	void SetOffset( float x, float y ) { m_vOffset.x = x; m_vOffset.y = y; }
	//
	virtual void Draw( void );
};

////////////////////////////////////////////////////////////////////////
class XWndTextString : virtual public XWnd
{
private:
	TCHAR m_szString[1024];		// 
	XBaseFontObj *m_pFont;
	XE::VEC2 m_vAdjust;
	void Init( void ) {
		memset( m_szString, 0, sizeof(m_szString) );
		m_pFont = NULL;
	}
	void Destroy( void ) { 
		SAFE_DELETE( m_pFont );
	}
protected:
public:
	XWndTextString() { Init(); }
//	XWndTextString( LPCTSTR szString, LPCTSTR szFont, float sizeFont, XCOLOR col=XCOLOR_WHITE );
	template<typename T>
	XWndTextString( T x, T y, LPCTSTR szString, XBaseFontDat *pFontDat, XCOLOR col ) 
		: XWnd( NULL, x, y ) 
	{
		Init();
		XBREAK( szString == NULL );
		SetText( szString );
		m_pFont = FACTORY->CreateFontObj( pFontDat );
		m_pFont->SetColor( col );
	}
	virtual ~XWndTextString() { Destroy(); }
	//
	void SetText( LPCTSTR szString ) { _tcscpy_s( m_szString, szString ); }
//	void SetAdjust( float adjx, float adjy ) { m_vAdjust.Set( adjx, adjy ); }
	void SetAlign( xAlign align ) {
//        m_pFont->SetStaticText( m_szString );   // FTGL버전땜에 필요함
        switch( align )
        {
        case xALIGN_CENTER:
            {
                XE::VEC2 vSize = m_pFont->GetLayoutSize( m_szString );
                m_vAdjust -= vSize / 2.f;
            }
            break;
        }
//		m_pFont->SetAlign( align );
	}
	void SetColorText( XCOLOR col ) {
		m_pFont->SetColor( col );
	}
	//
//	void SetLineLength( float len ) {
//		m_pFont->SetLineLength( len );
//	}
	void SetStyle( xFONT::xtStyle style ) {
		m_pFont->SetStyle( style );
	}
	GET_ACCESSOR( XBaseFontObj*, pFont );
	//
	virtual void Draw( void );
};

////////////////////////////////////////////////////////////////////////
// 버튼위에 지정한 숫자가 출력되는 버튼
class XWndButtonNumber : public XWndButton, public XWndTextNumber
{
private:
	void Init( void ) {	}
	void Destroy( void ) { 	}
protected:
public:
	XWndButtonNumber( float x, float y, float *pNumber, XBaseFontDat *pDat, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0 ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szSpr, idActUp, idActDown ), 
		XWndTextNumber( pNumber, pDat ) 
	{
		Init();
	}
	virtual ~XWndButtonNumber() { Destroy(); }
	
	virtual void Draw( void ) {
		XWndButton::Draw(); 
		XWndTextNumber::Draw();
	}
};
////////////////////////////////////////////////////////////////////////
// 스트링을 버튼위에 그리는 버튼
class XWndButtonString : public XWndButton, public XWndTextString
{
private:
	void Init( void ) {	}
	void Destroy( void ) { 	}
protected:
public:
	XWndButtonString( float x, float y, LPCTSTR szString, XBaseFontDat *pFontDat, LPCTSTR szSpr, ID idActUp, ID idActDown=0, ID idActDisable=0 ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szSpr, idActUp, idActDown, idActDisable ), 
		XWndTextString( 0, 0, szString, pFontDat, XCOLOR_BLACK ) {
		Init();
	}
	virtual ~XWndButtonString() { Destroy(); }
	
	virtual void Draw( void ) { XWndButton::Draw(); XWndTextString::Draw(); }
	// lua
//	void LuaSetAdjustText( float adjx, float adjy ) { XWndTextString::SetAdjust( adjx, adjy ); }
//	void LuaSetAlign( xAlign align ){ XWndTextString::SetAlign( align ); }
};

////////////////////////////////////////////////////////////////////////
// 크기조절되는 버튼-스트링버전
class XWndDynamicButtonString : public XWndDynamicButton, public XWndTextString
{
private:
	void Init( void ) {	}
	void Destroy( void ) { 	}
protected:
public:
	XWndDynamicButtonString( float x, float y, float w, float h, LPCTSTR szString, XBaseFontDat *pFontDat ) 
		: XWnd( NULL, x, y, w, h ), 
		XWndDynamicButton( x, y, w, h ), 
		XWndTextString( 0, 0, szString, pFontDat, XCOLOR_BLACK ) {
		Init();
	}
	virtual ~XWndDynamicButtonString() { Destroy(); }
	
	virtual void Draw( void ) { XWndDynamicButton::Draw(); XWndTextString::Draw(); }
};

////////////////////////////////////////////////////////////////////////
// 체크버튼위에 숫자 표시하는거
class XWndButtonCheckNumber : public XWndButtonCheck, public XWndTextNumber
{
private:
	void Init( void ) {	}
	void Destroy( void ) { 	}
protected:
public:
	XWndButtonCheckNumber( float x, float y, float *pNumber, XBaseFontDat *pFontDat, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, BOOL bPush=FALSE ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szSpr, idActUp, idActDown ),
		XWndButtonCheck( x, y, szSpr, idActUp, idActDown, bPush ), 
		XWndTextNumber( pNumber, pFontDat ) {
		Init();
	}
	virtual ~XWndButtonCheckNumber() { Destroy(); }
	
	virtual void Draw( void ) {
		XWndButtonCheck::Draw();
		XWndTextNumber::Draw();
	}

};

////////////////////////////////////////////////////////////////////////
// 체크버튼위에 스트링 표시하는 버전
class XWndButtonCheckString : public XWndButtonCheck, public XWndTextString
{
private:
	void Init( void ) {	}
	void Destroy( void ) { 	}
protected:
public:
	XWndButtonCheckString( float x, float y, LPCTSTR szString, XBaseFontDat *pFontDat, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, BOOL bPush=FALSE ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szSpr, idActUp, idActDown, 0 ),
		XWndButtonCheck( x, y, szSpr, idActUp, idActDown, bPush ), 
		XWndTextString( 0, 0, szString, pFontDat, XCOLOR_BLACK ) {
		Init();
	}
	virtual ~XWndButtonCheckString() { Destroy(); }
	
	virtual void Draw( void ) {
		XWndButtonCheck::Draw();
		XWndTextString::Draw();
	}
}; // class XWndButtonCheckString 

////////////////////////////////////////////////////////////////////////
// 이펙트 되는 버튼
class XWndButtonEffect;
class XWndButtonEffect : public XWndButton, public XWndEffectGradually
{
public:
	XWndButtonEffect( float x, float y, LPCTSTR szSpr, ID idActUp, ID idActDown=0, ID idActDisable=0 ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szSpr, idActUp, idActDown, idActDisable ),
		XWndEffectGradually() {
		
	}
	virtual ~XWndButtonEffect() {}
	// virtual
	virtual int Process( float dt ) { int retv = XWndButton::Process( dt ); XWndEffectGradually::Process( dt ); return retv; }
	// lua
	void LuaSetStart( BOOL bToBig, float x1, float y1, float x2, float y2, float sec ) {	
		SetStart( bToBig, XE::VEC2(x1,y1), XE::VEC2(x2,y2), sec );
	}
//	void LuaSetStart( BOOL bToBig, XE::VEC2 vStart, XE::VEC2 vEnd, float sec ) {	
//		SetStart( bToBig, vStart, vEnd, sec );
//	}

};
////////////////////////////////////////////////////////////////////////
// 라디오 버튼
class XWndRadioGroup;
class XWndButtonRadio : virtual public XWndButton
{
	XWndRadioGroup *m_pGroup;
	void Init() {
		m_pGroup = NULL;
	}
public:
	XWndButtonRadio( float x, float y, LPCTSTR szSpr, ID idUp, ID idDown, ID idDisable=0 ) 
		: XWnd( NULL, x, y ),
		XWndButton( x, y, szSpr, idUp, idDown, idDisable ) {
		Init();
	}
	virtual ~XWndButtonRadio() {}
	SET_ACCESSOR( XWndRadioGroup*, pGroup );
	//
	virtual XWnd* OnMouseMove( float lx, float ly ) { return this; }
	virtual XWnd* OnLButtonDown( float lx, float ly ) { m_bFirstPush = TRUE;	 return this;	}
	virtual ID OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly ) {}	// 상위 함수를 처리하지 못하게함
	virtual void OnNCLButtonUp( float lx, float ly ) ;

};
class XWndRadioGroup : public XWnd
{
public:
	XWndRadioGroup() {
		SetSizeLocal( -1, -1 );
	}
	virtual ~XWndRadioGroup() {}

	// 라디오버튼을 그룹에 추가시킨다
	XWndButtonRadio* Add( ID idWnd, XWndButtonRadio *pButt ) {
		XWnd::Add( idWnd, pButt );
		pButt->SetpGroup( this );
		return pButt;
	}
	// 그룹내 모든 라디오버튼의 눌림상태를 off로 만든다
	void Clear( void ) {
		LIST_LOOP( m_listItems, XWnd*, itor, pButt )
		{
			XWndButton *p = dynamic_cast<XWndButton*>( pButt );
			if( XASSERT( p ) )
				p->SetPush( FALSE );
		} END_LOOP;
	}
};

#pragma warning ( default : 4250 )


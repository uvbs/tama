#pragma once
#include "XWnd.h"
#include "XFontSpr.h"
#include "XFontMng.h"
#include "SprObj.h"
//#include "XFactory.h"

#pragma warning( disable : 4250 )	//  warning C4250: 'XWndButtonHexNumber' : 우위에 따라 'XWndButton::XWndButton::Process'을(를) 상속합니다.

#define xfALIGN_LEFT			0x01
#define xfALIGN_RIGHT		0x02
#define xfALIGN_HCENTER	0x04
#define xfALIGN_VCENTER		0x08
#define xfALIGN_TOP			0x10
#define xfALIGN_BOTTOM		0x20
#define xfALIGN_ALL			0x3F

class XWndMng;
class XSprObj;
////////////////////////////////////////////////////////////////////////
class XWndButton : virtual public XWnd
{
public:
//	static XWndButton* Find( XWndMng *pWndMng, ID idWnd );
	// fuck
	static XWND_MESSAGE_MAP s_mmSoundDown;		// 디폴트 메시지. 사운드.
	static XWND_MESSAGE_MAP s_mmCreate;				// 디폴트 메시지. 생성
	static XWND_MESSAGE_MAP s_mmQuestClicked;		// 디폴트 메시지. 버튼이 눌리면 퀘스트 객체로 이벤트 전달.
	template<typename T>
	static void SetDefaultEvent( DWORD msg, XWnd *pOwner, T func, DWORD param1=0, DWORD param2=0 ) {
		typedef int (XWnd::*CALLBACK_FUNC)( XWnd *, DWORD dwParam1, DWORD dwParam2 );
		XWND_MESSAGE_MAP *pMsgMap = NULL;
		switch( msg ) {
		case XWM_SOUND_DOWN:	pMsgMap = &s_mmSoundDown;	break;
		case XWM_CREATE:				pMsgMap = &s_mmCreate;		break;
		case XWM_QUEST_CLICKED:	pMsgMap = &s_mmQuestClicked;	break;
		}
		if( pMsgMap )	{
			pMsgMap->msg = msg;
			pMsgMap->param = param1;
			pMsgMap->pOwner = pOwner;
			pMsgMap->pHandler = static_cast<CALLBACK_FUNC>( func );
		}
	}
private:
	XSprObj *m_pSprObj;
	XSurface *m_pSurface[3];		// 버튼이미지를 surface(png)로 쓰는 버전용
	void Init( void ) {
		m_idActDisable = m_idActDown = m_idActUp = 0;
		m_bPush = m_bEdge = m_bFirstPush = FALSE;
//		m_bSprNum = FALSE;
		m_pSprObj = NULL;	
		XCLEAR_ARRAY( m_pSurface );
		// fuck
		if( s_mmSoundDown.pOwner )
			SetEvent( XWM_SOUND_DOWN, s_mmSoundDown.pOwner, s_mmSoundDown.pHandler );
		if( s_mmCreate.pOwner )
			SetEvent( XWM_CREATE, s_mmCreate.pOwner, s_mmCreate.pHandler );
		if( s_mmQuestClicked.pOwner )
			SetEvent( XWM_QUEST_CLICKED, s_mmQuestClicked.pOwner, s_mmQuestClicked.pHandler );
	}
	void Destroy( void ) { 
		SAFE_RELEASE2(IMAGE_MNG, m_pSurface[0] );
		SAFE_RELEASE2(IMAGE_MNG, m_pSurface[1] );
		SAFE_RELEASE2(IMAGE_MNG, m_pSurface[2] );
		SAFE_DELETE( m_pSprObj );
	}
protected:
	BOOL m_bPush, m_bEdge, m_bFirstPush;
	DWORD m_idActUp;		// 안눌려졌을때 액션번호
	DWORD m_idActDown;		// 눌려졌을때 액션번호
	DWORD m_idActDisable;	// 비활성화 됐을때 액션번호
	DWORD m_xfFlag;			// 다용도 플래그
//	BOOL m_bSprNum;
public:
	XWndButton() {}
	// 템플릿으로 하려했더니 헤더에 넣어야 하고 그럼  SprObj.h도 인클루드 해야해서 포기
//	XWndButton( float x, float y, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, DWORD idActDisable = 0, BOOL bSprNum = FALSE, XToolTip *pToolTip=NULL );
	XWndButton( float x, float y, LPCTSTR szSpr, DWORD idActUp, DWORD idActDown = 0, DWORD idActDisable = 0, XToolTip *pToolTip=NULL );
	XWndButton( float x, float y, XSprObj *pSprObj, DWORD idActUp, DWORD idActDown = 0, DWORD idActDisable = 0 );			// KeepSrc된 SprObj가 필요할때 외부에서 생성시킨걸 쓴다
	XWndButton( float x, float y, float w, float h, XToolTip *pToolTip=NULL );
	XWndButton( float x, float y, LPCTSTR szImgUp, LPCTSTR szImgDown, LPCTSTR szImgDisable=NULL, XToolTip *pToolTip=NULL, DWORD xfFlag=0 );
	void Create( float x, float y, XSprObj *pSprObj, DWORD idActUp, DWORD idActDown, DWORD idActDisable ) ;
	virtual ~XWndButton() { Destroy(); }


	void SetPush( BOOL bFlag ) { m_bPush = bFlag; }
	BOOL GetPush( void ) { return m_bPush; }
	GET_ACCESSOR( DWORD, idActUp );
 	GET_ACCESSOR( DWORD, idActDown );
	GET_ACCESSOR( DWORD, idActDisable );
//	GET_ACCESSOR( BOOL, bSprNum );
	GET_ACCESSOR( XSprObj*, pSprObj );
	GET_ACCESSOR( DWORD, xfFlag );
	void SetFlag( DWORD xfFlag, XWnd *pParent=NULL );

	void CancelPush( void ) { m_bPush = m_bFirstPush = FALSE; } // OnLButtonDown()에서 한 눌림처리를 취소시킨다. 버튼 안눌린걸로 해야하는데 XWndButton::OnLButtonDown()은 불러야 할때 사용
	void DrawNoSpr( void );
	void SetSprObj( LPCTSTR szSpr, ID idActUp=0, ID idActDown=0 );
	void SetpSurface( int idx, LPCTSTR szImg );
	void DrawFromSurface( const XE::VEC2& vPos );
	// virtual
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void Draw( const XE::VEC2& vPos );
	virtual BOOL IsWndAreaIn( float lx, float ly );
	// handler
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly );
	virtual void OnNCLButtonUp( float lx, float ly );
	virtual void OnPlaySound( ID id ) { 
		SOUNDMNG->SetdistListener( 0 );
		SOUNDMNG->OpenPlaySound( id ); 	
	}
	virtual void OnAddWnd( void );
protected:
	virtual void OnPushDown( void ) {}
}; // class XWndButton
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
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
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
		m_pFont = pDat->CreateFontObj();
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
	XBaseFontDat *m_pFontDat;
	XBaseFontObj *m_pFont;
	XE::VEC2 m_vAdjust;
	XCOLOR m_Color, m_colDisable;	// 활성화/비활성화 텍스트 색
	XCOLOR m_colBackground;
	XE::VEC2 m_sizeString;
	BOOL m_bRetain;
	void Init( void ) {
		m_szString[0] = 0;
		m_pFont = NULL;
		m_pFontDat = NULL;
		m_Color = XCOLOR_WHITE;
		m_colDisable = XCOLOR_GRAY;
		m_colBackground = 0;
		m_bRetain = FALSE;
	}
	void Destroy( void ) { 
		SAFE_DELETE( m_pFont );
		if( m_bRetain )
			SAFE_RELEASE2( FONTMNG, m_pFontDat );
	}
protected:
public:
	XWndTextString() { Init(); }
//	XWndTextString( LPCTSTR szString, LPCTSTR szFont, float sizeFont, XCOLOR col=XCOLOR_WHITE );
	template<typename T>
	XWndTextString( T x, T y, LPCTSTR szString, XBaseFontDat *pFontDat, XCOLOR col=XCOLOR_WHITE ) 
		: XWnd( NULL, x, y ) {
		Init();
		if( XE::IsHave( szString ) )
			SetText( szString );
		m_pFontDat = pFontDat;
		m_pFont = pFontDat->CreateFontObj();
		m_pFont->SetColor( col );
		m_Color = col;
		m_pFont->SetLineLength( (float)GRAPHICS->GetScreenWidth() );
	}
	XWndTextString( const XE::VEC2& vSize, LPCTSTR szString, XBaseFontDat *pFontDat, XCOLOR col=XCOLOR_WHITE )
		: XWnd( NULL, 0, 0 ) {
			Init();
			SetText( szString );
			m_pFontDat = pFontDat;
			m_pFont = pFontDat->CreateFontObj();
			m_pFont->SetColor( col );
			m_Color = col;
			m_pFont->SetLineLength( (float)GRAPHICS->GetScreenWidth() );
			SetSizeLocal( vSize.w, vSize.h );
	}
	XWndTextString( const XE::VEC2& vPos, LPCTSTR szString, 
							LPCTSTR szFont, float sizeFont, 
							XCOLOR col=XCOLOR_WHITE ) 
		: XWnd( NULL, vPos.x, vPos.y ) {
			Init();
			if( szString )
				SetText( szString );
			m_pFontDat = FONTMNG->Load( szFont, sizeFont );
			if( XBREAK( m_pFontDat == NULL ) )
				return;
			m_bRetain = TRUE;
			m_pFont = m_pFontDat->CreateFontObj();
			m_pFont->SetColor( col );
			m_Color = col;
			m_pFont->SetLineLength( (float)GRAPHICS->GetScreenWidth() );
	}
	virtual ~XWndTextString() { Destroy(); }
	//
	void SetText( LPCTSTR szString ) {
		_tcscpy_s( m_szString, szString );
		if( m_pFont && m_pFont->GetAlign() == XE::xALIGN_VCENTER )
			SetAlign( XE::xALIGN_VCENTER );
	}
	//
	void ChangeText( LPCTSTR szString ) {
		float lineLen = 0;
		if( m_pFont )
			lineLen = m_pFont->GetLineLength();
		SAFE_DELETE( m_pFont );
		m_pFont = m_pFontDat->CreateFontObj();
		m_pFont->SetColor( m_Color );
		if( lineLen == 0 )
			m_pFont->SetLineLength( (float)GRAPHICS->GetScreenWidth() );
		else
			m_pFont->SetLineLength( lineLen );
		_tcscpy_s( m_szString, szString );
	}

//	void SetAdjust( float adjx, float adjy ) { m_vAdjust.Set( adjx, adjy ); }
	void SetAlign( XE::xAlign align ) {
//        m_pFont->SetStaticText( m_szString );   // FTGL버전땜에 필요함
		m_pFont->SetAlign( align );
		if( align & XE::xALIGN_VCENTER )
		{
			XE::VEC2 vSize = m_pFont->GetLayoutSize( m_szString );
			m_vAdjust.y = -(vSize.h / 2.f);
		}
	}
	void SetColorText( XCOLOR col ) {
		m_Color = col;
		m_pFont->SetColor( col );
	}
	void SetColorBackground( XCOLOR col ) {
		m_colBackground = col;
		m_sizeString = m_pFont->GetLayoutSize( m_szString );
	}
	// 비활성화 됬을때 색
	void SetColorTextDisable( XCOLOR col ) {
		m_colDisable = col;
	}
	//
	void SetLineLength( float len ) {
		m_pFont->SetLineLength( len );
	}
	void SetStyle( xFONT::xtStyle style ) {
		m_pFont->SetStyle( style );
	}
	GET_ACCESSOR( XBaseFontObj*, pFont );
	XE::VEC2 GetLayoutSize( void ) {
		return m_pFont->GetLayoutSize( m_szString );
	}
	XE::VEC2 GetLayoutPos( void ) {
		return m_pFont->GetLayoutPos( m_szString );
	}
	//
	virtual void Draw( void );
	virtual void Draw( const XE::VEC2& vPos );
};

////////////////////////////////////////////////////////////////////////
class XWndTextStringArea : virtual public XWnd
{
private:
	TCHAR m_szString[1024];		// 
	XBaseFontDat *m_pFontDat;
	XBaseFontObj *m_pFont;
	XE::VEC2 m_sizeString;		// 스트링 부분만의 크기
	XCOLOR m_Color;
	BOOL m_bRetain;			// m_pFontDat를 this가 리테인함.
	void Init( void ) {
		m_szString[0] = 0;
		m_pFont = NULL;
		m_pFontDat = NULL;
		m_Color = XCOLOR_WHITE;
		m_bRetain = FALSE;
	}
	void Destroy( void ) { 
		SAFE_DELETE( m_pFont );
		if( m_bRetain )
			SAFE_RELEASE2( FONTMNG, m_pFontDat );
	}
protected:
public:
	XWndTextStringArea() { Init(); }
	XWndTextStringArea( float x, float y, float w, float h, LPCTSTR szString, 
								XBaseFontDat *pFontDat, XCOLOR col ) 
		: XWnd( NULL, x, y, w, h ) {
		Init();
		m_pFontDat = pFontDat;
		m_Color = col;
		if( szString )
		{
			m_pFont = pFontDat->CreateFontObj();
			m_pFont->SetColor( col );
			m_pFont->SetLineLength( w );
#ifdef WIN32
			m_pFont->SetArea( XE::VEC2( w, h ) );
			m_pFont->SetAlign( XE::xALIGN_CENTER );
#endif
			SetText( szString );
		}
	}
	XWndTextStringArea( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szString, 
								XBaseFontDat *pFontDat, XCOLOR col )
		: XWnd( NULL, vPos.x, vPos.y, vSize.w, vSize.h ) {
		Init();
		m_pFontDat = pFontDat;
		m_Color = col;
		if( szString )
		{
			m_pFont = pFontDat->CreateFontObj();
			m_pFont->SetColor( col );
			m_pFont->SetLineLength( vSize.w );
#ifdef WIN32
			m_pFont->SetArea( vSize );
			m_pFont->SetAlign( XE::xALIGN_CENTER );
#endif
			SetText( szString );
		}
	}
	XWndTextStringArea( float x, float y, XBaseFontDat *pFontDat )
		: XWnd( NULL, x, y, 0.1f, 0.1f ) {
		Init();
		m_pFontDat = pFontDat;
		m_pFont = pFontDat->CreateFontObj();
#ifdef WIN32
			m_pFont->SetAlign( XE::xALIGN_CENTER );
#endif
	}
	XWndTextStringArea( float x, float y, LPCTSTR szFont, float sizeFont )
		: XWnd( NULL, x, y, 0.1f, 0.1f ) {
			Init();
			m_pFontDat = FONTMNG->Load( szFont, sizeFont );
			if( XBREAK( m_pFontDat == NULL ) )
				return;
			m_bRetain = TRUE;
			m_pFont = m_pFontDat->CreateFontObj();
#ifdef WIN32
			m_pFont->SetAlign( XE::xALIGN_CENTER );
#endif
	}
	XWndTextStringArea( const XE::VEC2& vPos, const XE::VEC2& vSize, 
								LPCTSTR szText,
								LPCTSTR szFont, float sizeFont, XCOLOR colText=XCOLOR_WHITE )
		: XWnd( NULL, vPos.x, vPos.y, vSize.w, vSize.h ) {
			Init();
			m_pFontDat = FONTMNG->Load( szFont, sizeFont );
			if( XBREAK( m_pFontDat == NULL ) )
				return;
			m_bRetain = TRUE;
			m_pFont = m_pFontDat->CreateFontObj();
			SetColorText( colText );
			m_pFont->SetLineLength( vSize.w );
#ifdef WIN32
			m_pFont->SetArea( vSize );
			m_pFont->SetAlign( XE::xALIGN_CENTER );
#endif
			if( XE::IsHave( szText ) )
				SetText( szText );
	}
	virtual ~XWndTextStringArea() { Destroy(); }
	//
	void SetText( LPCTSTR szString ) { 
		if( szString )	{
			if( m_pFont == NULL ) {
				XBREAK( m_pFontDat == NULL );
				m_pFont = m_pFontDat->CreateFontObj();
				m_pFont->SetColor( m_Color );
#ifdef WIN32
				m_pFont->SetArea( GetSizeFinal() );
				m_pFont->SetAlign( XE::xALIGN_CENTER );
#endif
				XE::VEC2 vSize = GetSizeFinal();
				m_pFont->SetLineLength( vSize.w );
			}
			_tcscpy_s( m_szString, szString ); 
			m_sizeString = m_pFont->GetLayoutSize( m_szString );
		} else {
			m_szString[0] = 0;
			m_sizeString = XE::VEC2(0);
		}
	}
	void SetAlign( XE::xAlign align ) {
		XBREAK( align == XE::xALIGN_CENTER );
//        m_pFont->SetStaticText( m_szString );   // FTGL버전땜에 필요함
//		m_pFont->SetAlign( align );
	}
	void SetColorText( XCOLOR col ) {
		m_Color = col;
		m_pFont->SetColor( col );
	}
	XCOLOR GetColorText( void ) {
		return m_pFont->GetColor();
	}
	//
	void SetLineLength( float len ) {
		m_pFont->SetLineLength( len );
	}
	void SetArea( const XE::VEC2& vSize ) {
		m_pFont->SetArea( vSize );
		SetSizeLocal( vSize );
	}
	void SetStyle( xFONT::xtStyle style ) {
		m_pFont->SetStyle( style );
	}
	GET_ACCESSOR( XBaseFontObj*, pFont );
	//
	virtual void Draw( const XE::VEC2& vPos );
	virtual void Draw( void );
};
template<typename RET>
class XWndTextDynamic : public XWndTextString
{
	RET (XWnd::*m_pFunc)( DWORD param1 );
	_tstring m_strFormat;
	XWnd *m_pOwner;
	DWORD m_Param1;
	void Init() {
		m_pFunc = NULL;
		m_pOwner = NULL;
		m_Param1 = -1;
	}
	void Destroy() {}
public:
	template<typename T>
	XWndTextDynamic( float x, float y, XWnd* pOwner, T func, LPCTSTR szFormat, XBaseFontDat *pFontDat, XCOLOR col=XCOLOR_WHITE ) 
		: XWnd( NULL, x, y ), XWndTextString( x, y, NULL, pFontDat, col )
	{ 
		typedef RET (XWnd::*CALLBACK_FUNC)( DWORD param1 );
		Init(); 
		m_pOwner = pOwner;
		m_pFunc = static_cast<CALLBACK_FUNC>( func );
		m_strFormat = szFormat;
	}
	template<typename T>
	XWndTextDynamic( float x, float y, XWnd* pOwner, T func, DWORD param, LPCTSTR szFormat, XBaseFontDat *pFontDat, XCOLOR col=XCOLOR_WHITE ) 
		: XWnd( NULL, x, y ), XWndTextString( x, y, NULL, pFontDat, col )
	{ 
		typedef RET (XWnd::*CALLBACK_FUNC)( DWORD param1 );
		Init(); 
		m_pOwner = pOwner;
		m_pFunc = static_cast<CALLBACK_FUNC>( func );
		m_strFormat = szFormat;
		m_Param1 = param;
	}
	virtual ~XWndTextDynamic() { Destroy(); }
	void Draw( void ) {
		RET val = (m_pOwner->*m_pFunc)( m_Param1 );
		XWndTextString::SetText( XE::Format( m_strFormat.c_str(), val ) );
		XWndTextString::Draw();
	}
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
class XWndButtonString : public XWndButton, public XWndTextStringArea
{
private:
	XCOLOR m_Color;
	void Init( void ) {	
		m_Color = XCOLOR_WHITE;
	}
	void Destroy( void ) { 	}
protected:
public:
	XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
							XBaseFontDat *pFontDat, 
							LPCTSTR szSpr, ID idActUp, ID idActDown=0, ID idActDisable=0 ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szSpr, idActUp, idActDown, idActDisable ), 
		XWndTextStringArea( x, y, pFontDat ) {
		Init();
		m_Color = col;
		if( szString )
		{
			XWndTextStringArea::SetColorText( col );
			XWndTextStringArea::SetLineLength( GetSizeFinal().w );
			XWndTextStringArea::SetText( szString );
		}
//		if( GetpFont() )
//			GetpFont()->SetStyle( xFONT::xSTYLE_STROKE );
#ifdef WIN32
		XWndTextStringArea::SetArea( GetSizeFinal() );
#endif
	}
	XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
							LPCTSTR szFont, float sizeFont,
							LPCTSTR szSpr, ID idActUp, ID idActDown=0, ID idActDisable=0 ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szSpr, idActUp, idActDown, idActDisable ), 
		XWndTextStringArea( x, y, szFont, sizeFont ) {
			Init();
			m_Color = col;
			if( szString )
			{
				XWndTextStringArea::SetColorText( col );
				XWndTextStringArea::SetLineLength( GetSizeFinal().w );
				XWndTextStringArea::SetText( szString );
			}
//			if( GetpFont() )
//				GetpFont()->SetStyle( xFONT::xSTYLE_STROKE );
#ifdef WIN32
			XWndTextStringArea::SetArea( GetSizeFinal() );
#endif
	}
	XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, XBaseFontDat *pFontDat, 
							LPCTSTR szImgUp, LPCTSTR szImgDown=NULL, LPCTSTR szImgDisable=NULL ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szImgUp, szImgDown, szImgDisable ), 
		XWndTextStringArea( x, y, pFontDat ) {
			Init();
			m_Color = col;
			if( szString )
			{
				XWndTextStringArea::SetColorText( col );
				XWndTextStringArea::SetLineLength( GetSizeFinal().w );
				XWndTextStringArea::SetText( szString );
			}
//			if( GetpFont() )
//				GetpFont()->SetStyle( xFONT::xSTYLE_STROKE );
#ifdef WIN32
			XWndTextStringArea::SetArea( GetSizeFinal() );
#endif
	}
	XWndButtonString( float x, float y, LPCTSTR szString, XCOLOR col, 
							LPCTSTR szFont, float sizeFont,
							LPCTSTR szImgUp, LPCTSTR szImgDown=NULL, LPCTSTR szImgDisable=NULL ) 
		: XWnd( NULL, x, y ), 
		XWndButton( x, y, szImgUp, szImgDown, szImgDisable ), 
		XWndTextStringArea( x, y, szFont, sizeFont ) {
			Init();
			m_Color = col;
			if( szString )
			{
				XWndTextStringArea::SetColorText( col );
				XWndTextStringArea::SetLineLength( GetSizeFinal().w );
				XWndTextStringArea::SetText( szString );
			}
//			if( GetpFont() )
//				GetpFont()->SetStyle( xFONT::xSTYLE_STROKE );
#ifdef WIN32
			XWndTextStringArea::SetArea( GetSizeFinal() );
#endif
	}
	virtual ~XWndButtonString() { Destroy(); }
	//
	void SetString( LPCTSTR szString ) {
		XWndTextStringArea::SetColorText( m_Color );
		XWndTextStringArea::SetLineLength( GetSizeFinal().w );
		XWndTextStringArea::SetText( szString );
	}
	//
	virtual void Draw( void );
	virtual void Draw( const XE::VEC2& vPos ) {
		XWndButton::Draw( vPos );
		XWndTextStringArea::Draw( vPos );
	}
};
////////////////////////////////////////////////////////////////////////
// 리소스가 필요없는 디버깅용 심플한 버튼
class XWndButtonDebug : public XWndButton
{
	XBaseFontObj *m_pFontObj;
	TCHAR m_szText[ 64 ];
	void Init() {
		m_pFontObj = NULL;
		m_szText[0] = 0;
	}
	void Destroy() {
		SAFE_DELETE( m_pFontObj );
	}
public:
	XWndButtonDebug( float x, float y, float w, float h, LPCTSTR szText, XBaseFontDat *pFontDat )
		: XWndButton( x, y, w, h ), XWnd( NULL, x, y, w, h )	{
		Init();
		m_pFontObj = pFontDat->CreateFontObj();;
        m_pFontObj->SetLineLength( w );
		m_pFontObj->SetAlign( XE::xALIGN_CENTER );
		m_pFontObj->SetArea( XE::VEC2( w, h ) );
		if( szText )
			_tcscpy_s( m_szText, szText );
	}
	virtual ~XWndButtonDebug() { Destroy(); }
	BOOL IsWndAreaIn( float lx, float ly );
	int Process( float dt );
	void Draw( void );
};
////////////////////////////////////////////////////////////////////////
// 크기조절되는 버튼-스트링버전
/*class XWndDynamicButtonString : public XWndDynamicButton, public XWndTextString
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
*/
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
	virtual void Draw( const XE::VEC2& vPos ) {
		XWndTextString::Draw( vPos );
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
	XWndTextStringArea *m_pTextArea;
	void Init() {
		m_pGroup = NULL;
		m_pTextArea = NULL;
	}
	void Destroy() {
		SAFE_DELETE( m_pTextArea );
	}
public:
	XWndButtonRadio( float x, float y, LPCTSTR szSpr, ID idUp, ID idDown, ID idDisable=0 ) 
		: XWnd( NULL, x, y ),
		XWndButton( x, y, szSpr, idUp, idDown, idDisable ) {
		Init();
	}
	XWndButtonRadio( float x, float y, LPCTSTR szImgUp, LPCTSTR szImgDown, LPCTSTR szImgDisable=NULL ) 
		: XWnd( NULL, x, y ),
		XWndButton( x, y, szImgUp, szImgDown, szImgDisable ) {
		Init();
	}
	XWndButtonRadio( float x, float y, 
							LPCTSTR szText, 
							XCOLOR colText, 
							XBaseFontDat *pFontDat, 
							LPCTSTR szImgUp, 
							LPCTSTR szImgDown, 
							LPCTSTR szImgDisable=NULL ) 
		: XWnd( NULL, x, y ),
		XWndButton( x, y, szImgUp, szImgDown, szImgDisable ) {
			Init();
			XE::VEC2 vSize = GetSizeLocal();
			m_pTextArea = new XWndTextStringArea( XE::VEC2(0), vSize, szText, pFontDat, colText );
	}
	virtual ~XWndButtonRadio() { Destroy(); }
	SET_ACCESSOR( XWndRadioGroup*, pGroup );
	GET_ACCESSOR( XWndTextStringArea*, pTextArea );
	//
	virtual void OnMouseMove( float lx, float ly ) {}
	virtual void OnLButtonDown( float lx, float ly ) { 
		m_bFirstPush = TRUE;
	}
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly ) {}	// 상위 함수를 처리하지 못하게함
	virtual void OnNCLButtonUp( float lx, float ly ) ;
	virtual void Draw( void );
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
	// 라디오버튼을 그룹에 추가시킨다
	XWndButtonRadio* Add( XWndButtonRadio *pButt ) {
		XWnd::Add( pButt );
		pButt->SetpGroup( this );
		return pButt;
	}
	// 그룹내 모든 라디오버튼의 눌림상태를 off로 만든다
	void Clear( void ) {
		XLIST_LOOP( m_listItems, XWnd*, pButt )
		{
			XWndButton *p = dynamic_cast<XWndButton*>( pButt );
			if( XASSERT( p ) )
				p->SetPush( FALSE );
		} END_LOOP;
	}
};

#pragma warning ( default : 4250 )


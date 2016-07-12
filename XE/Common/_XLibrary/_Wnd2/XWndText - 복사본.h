#pragma once
#include "XWnd.h"
#include "XFontSpr.h"
#include "XFontMng.h"

class XWndTextString;
////////////////////////////////////////////////////////////////////////
class XWndTextString : /*virtual */public XWnd
{
public:
	static XWndTextString* sUpdateCtrl( XWnd *pRoot
																		, const char *cIdentifier
																		, const XE::VEC2& vPos
																		, LPCTSTR szFont
																		, int sizeFont );
	static XWndTextString* sUpdateCtrl( XWnd *pRoot
																		, const XE::VEC2& vPos 
																		, const char *cIdentifier ) {
		return sUpdateCtrl( pRoot, cIdentifier, vPos, FONT_NANUM, 20 );
	}
	static XWndTextString* sUpdateCtrl( XWnd *pRoot
																		, const char *cIdentifier
																		, const XE::VEC2& vPos ) {
		return sUpdateCtrl( pRoot, cIdentifier, vPos, FONT_NANUM, 20 );
	}
private:
#ifdef _XNEW_WND_TEXT
	TCHAR *m_szString;
	int m_maxSize;		// 버퍼 최대 크기
#else
	TCHAR m_szString[1024];		// _tstring으로 하면 스트링이 바뀔때마다 동적할당으로 메모리위치가 바껴서 FTGL이 메모리주소 캐시로 인한 오동작을 한다.
#endif
	XBaseFontDat *m_pFontDat;
	XBaseFontObj *m_pFont;
//	XE::VEC2 m_vAdjust;
	XCOLOR m_Color, m_colDisable;	// 활성화/비활성화 텍스트 색
	XCOLOR m_colBackground;
	XE::VEC2 m_sizeString;		// 화면상에 표시될 사이즈. 최적화를 위해 값이 없을수도 있다.
	BOOL m_bRetain;
	bool m_bSizeFollowByParent = false;		// 자동으로 부모윈도우의 사이즈를 따른다.(땜빵코드)
	XE::xAlign m_Align = XE::xALIGN_LEFT;	// FTGL때문에 XBaseFontObj의 Align과는 별도로쓴다.
	bool m_bAlignParent = false;		// align을 부모크기를 기준으로 실시간 정렬한다.
	void Init( void ) {
#ifdef _XNEW_WND_TEXT
		m_szString = NULL;
		m_maxSize = 0;
#else
		m_szString[0] = 0;
#endif
		m_pFont = NULL;
		m_pFontDat = NULL;
		m_Color = XCOLOR_WHITE;
		m_colDisable = XCOLOR_GRAY;
		m_colBackground = 0;
		m_bRetain = FALSE;
		m_strDebug = _T("text");
		// 이제부턴 텍스터 컨트롤은 제로사이즈가 되어선 안된다.
#ifdef _XWIN_ZERO_SIZE
		if( !GetSizeLocal().IsValid() )
			SetSizeLocal(1,1);
#endif
	}
	void Destroy( void ) { 
#ifdef _XNEW_WND_TEXT
		SAFE_DELETE_ARRAY( m_szString );
#endif
		DestroyFont();
	}
	void DestroyFont( void ) { 
		SAFE_DELETE( m_pFont );
		if( m_bRetain )
			SAFE_RELEASE2( FONTMNG, m_pFontDat );
	}
protected:
public:
	XWndTextString();
// 	template<typename T1, typename T2>
// 	XWndTextString( T1 x, T2 y, LPCTSTR szString, XBaseFontDat *pFontDat = nullptr, XCOLOR col=XCOLOR_WHITE ) 
// 		: XWnd( x, y ) {
// 		Init();
// 		Create( szString, pFontDat, col );
// 	}
// 	template<typename T1, typename T2>
// 	XWndTextString( T1 x, T2 y, const _tstring& str, XBaseFontDat *pFontDat = nullptr, XCOLOR col=XCOLOR_WHITE ) 
// 		: XWnd( x, y ) {
// 		Init();
// 		Create( str.c_str(), pFontDat, col );
// 	}
// 	XWndTextString( const XE::VEC2& vSize, LPCTSTR szString, XBaseFontDat *pFontDat = nullptr, XCOLOR col=XCOLOR_WHITE )
// 		: XWnd( 0, 0 ) {
// 			Init();
// 			Create( szString, pFontDat, col );
// 			SetSizeLocal( vSize.w, vSize.h );
// 	}
	XWndTextString( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szString, XBaseFontDat *pFontDat = nullptr, XCOLOR col = XCOLOR_WHITE )
		: XWnd( vPos, vSize ) {
		Init();
		Create( szString, pFontDat, col );
		if( m_pFont )
			m_pFont->SetLineLength( vSize.w );
	}
	XWndTextString( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szString, 
							LPCTSTR szFont, float sizeFont, 
							XCOLOR col=XCOLOR_WHITE ); 
	XWndTextString( float x, float y, LPCTSTR szString,
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE ) 
							: XWndTextString( XE::VEC2(x,y), szString, szFont, sizeFont, col ) {}
	XWndTextString( float x, int y, LPCTSTR szString,
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE ) 
							: XWndTextString( XE::VEC2(x,y), szString, szFont, sizeFont, col ) {}
	XWndTextString( int x, float y, LPCTSTR szString,
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE ) 
							: XWndTextString( XE::VEC2(x,y), szString, szFont, sizeFont, col ) {}
	XWndTextString( int x, int y, LPCTSTR szString,
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE ) 
							: XWndTextString( XE::VEC2(x,y), szString, szFont, sizeFont, col ) {}
	XWndTextString( float x, float y, const _tstring& str,
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE ) 
							: XWndTextString( XE::VEC2(x,y), str.c_str(), szFont, sizeFont, col ) {}
	XWndTextString( float x, int y, const _tstring& str,
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE ) 
							: XWndTextString( XE::VEC2(x,y), str.c_str(), szFont, sizeFont, col ) {}
	XWndTextString( int x, float y, const _tstring& str,
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE ) 
							: XWndTextString( XE::VEC2(x,y), str.c_str(), szFont, sizeFont, col ) {}
	XWndTextString( int x, int y, const _tstring& str,
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE ) 
							: XWndTextString( XE::VEC2(x,y), str.c_str(), szFont, sizeFont, col ) {}
	XWndTextString( const XE::VEC2& vPos, LPCTSTR szString, 
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE );
	XWndTextString( const XE::VEC2& vPos, const _tstring& strString, 
							LPCTSTR szFont = FONT_SYSTEM, float sizeFont = FONT_SIZE_DEFAULT, 
							XCOLOR col=XCOLOR_WHITE );
	XWndTextString( const XE::VEC2& vPos, const XE::VEC2& vSize, const _tstring& strString,
					LPCTSTR szFont, float sizeFont,
					XCOLOR col = XCOLOR_WHITE );
	XWndTextString( const XE::VEC2& vPos, float sizeFont = FONT_SIZE_DEFAULT )
		: XWndTextString( vPos, _T(""), FONT_SYSTEM, sizeFont, XCOLOR_WHITE ) {
	}
	/**
	 @brief 부모의 크기에 맞게 자동으로 중앙정렬된다.
	*/
	XWndTextString( LPCTSTR szString,
					LPCTSTR szFont, float sizeFont,
					XCOLOR col = XCOLOR_WHITE );
	XWndTextString( const _tstring& str,
					LPCTSTR szFont, float sizeFont,
					XCOLOR col = XCOLOR_WHITE ) 
					: XWndTextString( str.c_str(), szFont, sizeFont, col ) {}

	template<typename T1, typename T2>
	XWndTextString( T1 x, T2 y, LPCTSTR szString, 
							LPCTSTR szFont, float sizeFont, 
							XCOLOR col=XCOLOR_WHITE ) 
// 		: XWnd( x, y ),
		: XWndTextString( XE::VEC2(x,y), szString, szFont, sizeFont, col ) {}
	template<typename T1, typename T2>
	XWndTextString( T1 x, T2 y, const _tstring& strString,
					LPCTSTR szFont, float sizeFont,
					XCOLOR col = XCOLOR_WHITE ) 
// 		: XWnd( x, y ),
		: XWndTextString( x, y, strString.c_str(), szFont, sizeFont, col ) {}
	virtual ~XWndTextString() { Destroy(); }
	//
	virtual XE::VEC2 GetSizeForceFinal( void );
	///< 
	void Create( LPCTSTR szString, XBaseFontDat *pFontDat = nullptr, XCOLOR col=XCOLOR_WHITE );
	GET_SET_ACCESSOR( bool, bAlignParent );
	void SetText( const _tstring& strText ) {
		SetText( strText.c_str() );
	}
	void SetText( LPCTSTR szString );
	GET_ACCESSOR( LPCTSTR, szString );
//	GET_SET_ACCESSOR( const XE::VEC2&, vAdjust );
	//
	void SetFont( LPCTSTR szFont, float size );
	void ChangeText( LPCTSTR szString );

	void SetAlign( XE::xAlign align );
	inline void SetAlignHCenter() {
		SetAlign( XE::xALIGN_HCENTER );
	}
	inline void SetAlignVCenter() {
		SetAlign( XE::xALIGN_VCENTER );
	}
	inline void SetAlignCenter() {
		SetAlign( XE::xALIGN_CENTER );
	}
	inline void SetAlignRight() {
		SetAlign( XE::xALIGN_RIGHT );
	}
	void SetColorText( XCOLOR col ) {
		m_Color = col;
		m_pFont->SetColor( col );
	}
	void SetColorBackground( XCOLOR col ) {
		m_colBackground = col;
		if( m_sizeString.IsZero() == FALSE )
			PreCalcLayoutSize();
	}
	// 비활성화 됬을때 색
	void SetColorTextDisable( XCOLOR col ) {
		m_colDisable = col;
	}
	//
	void SetLineLength( float len ) {
		int lenLine = (int)m_pFont->GetLineLength();
		// 레이아웃 사이즈가 구해져있었고 레이아웃 폭이 바뀌었다면 레이아웃 사이즈 재계산
		if( m_sizeString.IsZero() == FALSE && lenLine != (int)len )
			PreCalcLayoutSize();
		m_pFont->SetLineLength( len );
	}
	void SetStyle( xFONT::xtStyle style ) {
		m_pFont->SetStyle( style );
	}
	void SetStyleNormal() {
		m_pFont->SetStyle( xFONT::xSTYLE_NORMAL );
	}
	void SetStyleShadow() {
		m_pFont->SetStyle( xFONT::xSTYLE_SHADOW );
	}
	void SetStyleStroke() {
		m_pFont->SetStyle( xFONT::xSTYLE_STROKE );
	}
	GET_ACCESSOR( XBaseFontObj*, pFont );
	XE::VEC2 GetLayoutSize( void ) {
		if( m_sizeString.IsZero() )
			PreCalcLayoutSize();
		return m_sizeString;
	}
	XE::VEC2 GetLayoutPos( void ) {
#ifdef _XNEW_WND_TEXT
		XBREAK( m_szString == NULL );
#endif
		return m_pFont->GetLayoutPos( m_szString );
	}
	// 최적화를 위해 화면에 표시될 텍스트의 사이즈를 미리 구해놓는다.
	virtual void PreCalcLayoutSize( void );
	virtual BOOL IsWndAreaIn( float lx, float ly ) {
		// 스트링이 있고 사이즈가 안구해졌을때만 사이즈를 계산한다.
		// 만약 사이즈가 있었는데 스트링이 ""로 바꼈으면 이전사이즈를 사용한다.
		if( XE::IsHave( m_szString ) && m_sizeString.IsZero() )	
			PreCalcLayoutSize();
		return XWnd::IsWndAreaIn( lx, ly );
	}
	//
	virtual void Draw( void );
	virtual void Draw( const XE::VEC2& vPos );
	virtual XE::xRECT GetBoundBox( void ) {
		XE::xRECT rect = XWnd::GetBoundBox();
		rect.vRB = rect.vLT + m_sizeString;
		return rect;
	}
	virtual void AutoLayoutHCenter( void ) {
		PreCalcLayoutSize();
		XBREAK( m_pParent == nullptr );		// 이제 경고를 내도록 바뀜.
		XWnd::AutoLayoutHCenter( m_pParent );
	}
	virtual void AutoLayoutVCenter( void ) {
		PreCalcLayoutSize();
		XBREAK( m_pParent == nullptr );		// 이제 경고를 내도록 바뀜.
		XWnd::AutoLayoutVCenter( m_pParent );
	}
	virtual void AutoLayoutCenter( void ) override {
		PreCalcLayoutSize();
		XBREAK( m_pParent == nullptr );		// 이제 경고를 내도록 바뀜.
		XWnd::AutoLayoutCenter( m_pParent );
	}
	XE::VEC2 GetSizeWindow( void ) override {
		return GetLayoutSize();
	}
 	void Update() override;
	// this의 m_vSize와 같다.
// 	void SetSize( const XE::VEC2& vSize ) {
// 		m_pFont->SetArea( vSize );
// 		SetSizeLocal( vSize );
// 	}
	void UpdateSize();

};

////////////////////////////////////////////////////////////////////////
// class XWndTextStringArea : virtual public XWnd
// {
// private:
// 	TCHAR m_szString[1024];		// 
// 	XBaseFontDat *m_pFontDat;
// 	XBaseFontObj *m_pFont;
// 	XE::VEC2 m_sizeString;		// 스트링 부분만의 크기
// 	XCOLOR m_Color;
// 	BOOL m_bRetain;			// m_pFontDat를 this가 리테인함.
// //	XE::xAlign m_Align = XE::xALIGN_NONE;
// 	void Init( void ) {
// 		m_szString[0] = 0;
// 		m_pFont = NULL;
// 		m_pFontDat = NULL;
// 		m_Color = XCOLOR_WHITE;
// 		m_bRetain = FALSE;
// 	}
// 	void Destroy( void ) { 
// 		SAFE_DELETE( m_pFont );
// 		if( m_bRetain )
// 			SAFE_RELEASE2( FONTMNG, m_pFontDat );
// 	}
// protected:
// public:
// 	XWndTextStringArea() { Init(); }
// 	XWndTextStringArea( float x, float y, float w, float h, LPCTSTR szString, 
// 								XBaseFontDat *pFontDat = nullptr, XCOLOR col = XCOLOR_WHITE) 
// 		: XWnd( x, y, w, h ) {
// 		Init();
// 		Create( w, h, szString, pFontDat, col );
// 	}
// 	XWndTextStringArea( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szString, 
// 								XBaseFontDat *pFontDat = nullptr, XCOLOR col = XCOLOR_WHITE )
// 		: XWnd( vPos.x, vPos.y, vSize.w, vSize.h ) {
// 		Init();
// 		Create( vSize.w, vSize.h, szString, pFontDat, col );
// 	}
// 	XWndTextStringArea( float x, float y, XBaseFontDat *pFontDat = nullptr )
// 		: XWnd( x, y, 0.1f, 0.1f ) {
// 		Init();
// 		Create( pFontDat );
// 	}
// 	XWndTextStringArea( float x, float y, LPCTSTR szFont, float sizeFont )
// 		: XWnd( x, y, 0.1f, 0.1f ) {
// 			Init();
// 			XBaseFontDat *pFontDat = FONTMNG->Load( szFont, sizeFont );
// 			XBREAK( pFontDat == NULL );
// 			Create( pFontDat );
// 			m_bRetain = TRUE;
// 	}
// 	XWndTextStringArea( const XE::VEC2& vPos, const XE::VEC2& vSize, 
// 								LPCTSTR szText,
// 								LPCTSTR szFont, float sizeFont, XCOLOR colText=XCOLOR_WHITE )
// 		: XWnd( vPos.x, vPos.y, vSize.w, vSize.h ) {
// 			Init();
// 			XBaseFontDat *pFontDat = FONTMNG->Load( szFont, sizeFont );
// 			XBREAK( pFontDat == NULL );
// 			Create( vSize.w, vSize.h, szText, pFontDat, colText );
// 			m_bRetain = TRUE;
// 	}
// 	// 부모의 크기에 맞춰서 자동정렬되는 생성자.
// 	XWndTextStringArea( LPCTSTR szText, LPCTSTR szFont, float sizeFont, XCOLOR colText=XCOLOR_WHITE )
// 		: XWnd( 0, 0, 1, 1 ) {
// 			Init();
// 			_tstring strText = szText;
// 			XBaseFontDat *pFontDat = FONTMNG->Load( szFont, sizeFont );
// 			XBREAK( pFontDat == NULL );
// 			Create( strText.c_str(), pFontDat, colText );
// 			m_bRetain = TRUE;
// 			SetbUpdate( true );
// 	}
// 	virtual ~XWndTextStringArea() { Destroy(); }
// 	//
// private:
// 	void Create( float w, float h, LPCTSTR szString, XBaseFontDat *pFontDat = nullptr, XCOLOR col=XCOLOR_WHITE );
// 	void Create( LPCTSTR szString, XBaseFontDat *pFontDat = nullptr, XCOLOR col = XCOLOR_WHITE );
// 	void Create( XBaseFontDat *pFontDat/* = nullptr*/ );
// public:
// 	void Update() override;
// 	void UpdateAlign() {
// 		if( m_pFont == nullptr )
// 			return;
// 		// 텍스트의 사이즈를 다시 구한다.
// 		m_sizeString = m_pFont->GetLayoutSize( m_szString );
// 		XE::VEC2 vSize = GetSizeFinal();	// sizeString과 SizeLocal은 다른것임. sizelocal은 윈도우전체 크기고, sizeString은 텍스트부분 크기만이다.
// #ifdef WIN32
// 		m_pFont->SetArea( vSize );
// 		m_pFont->SetAlign( XE::xALIGN_CENTER );
// #endif
// 		m_pFont->SetLineLength( vSize.w );
// 	}
// 	void SetText( LPCTSTR szString ) { 
// 		if( szString )	{
// 			if( m_pFont == NULL ) {
// 				XBREAK( m_pFontDat == NULL );
// 				m_pFont = m_pFontDat->CreateFontObj();
// 				m_pFont->SetColor( m_Color );
// // 				XE::VEC2 vSize = GetSizeFinal();
// // #ifdef WIN32
// // 				m_pFont->SetArea( vSize );
// // 				m_pFont->SetAlign( XE::xALIGN_CENTER );
// // #endif
// // 				m_pFont->SetLineLength( vSize.w );
// 			}
// 			// 텍스트가 이전과 달라졌을때만 size갱신한다.
// 			if( XE::IsSame( m_szString, szString ) == FALSE )
// 			{
//  				_tcscpy_s( m_szString, szString ); 
// 				UpdateAlign();
// 			}
// // 			// 텍스트가 이전과 달라졌을때만 size갱신한다.
// // 			if( XE::IsSame( m_szString, szString ) == FALSE )
// // 			{
// // 				_tcscpy_s( m_szString, szString ); 
// // 				m_sizeString = m_pFont->GetLayoutSize( m_szString );
// // 			}
// 		} else {
// 			m_szString[0] = 0;
// 			m_sizeString = XE::VEC2(0);
// 		}
// 	}
// 	void SetAlign( XE::xAlign align ) {
// 		XBREAK( align == XE::xALIGN_CENTER );
// //        m_pFont->SetStaticText( m_szString );   // FTGL버전땜에 필요함
// //		m_pFont->SetAlign( align );
// 	}
// 	void SetColorText( XCOLOR col ) {
// 		m_Color = col;
// 		m_pFont->SetColor( col );
// 	}
// 	XCOLOR GetColorText( void ) {
// 		return m_pFont->GetColor();
// 	}
// 	//
// 	void SetLineLength( float len ) {
// 		m_pFont->SetLineLength( len );
// 	}
// 	void SetArea( const XE::VEC2& vSize ) {
// 		m_pFont->SetArea( vSize );
// 		SetSizeLocal( vSize );
// 	}
// 	void SetStyle( xFONT::xtStyle style ) {
// 		m_pFont->SetStyle( style );
// 	}
// 	GET_ACCESSOR( XBaseFontObj*, pFont );
// 	//
// 	virtual void Draw( const XE::VEC2& vPos );
// 	virtual void Draw( void );
// };
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
		: XWnd( x, y ), XWndTextString( x, y, NULL, pFontDat, col )
	{ 
		typedef RET (XWnd::*CALLBACK_FUNC)( DWORD param1 );
		Init(); 
		m_pOwner = pOwner;
		m_pFunc = static_cast<CALLBACK_FUNC>( func );
		m_strFormat = szFormat;
	}
	template<typename T>
	XWndTextDynamic( float x, float y, XWnd* pOwner, T func, DWORD param, LPCTSTR szFormat, XBaseFontDat *pFontDat, XCOLOR col=XCOLOR_WHITE ) 
		: XWnd( x, y ), XWndTextString( x, y, NULL, pFontDat, col )
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

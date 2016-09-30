#pragma once
#include "XWnd.h"
#include "XFontSpr.h"
#include "XFontMng.h"
#include "XFramework/Game/XEComponentMng.h"

/**
 @brief 텍스트 표시 컨트롤
 @정책: 정책이 왔다리 갔다리 하니까 텍스트쪽에 버그가 엄청나게 생긴다. 확실한 정책을 지정해놓고 방향을 잃지 않도록 하자.
	  .윈도우와 렌더텍스트를 분명히 구분해서 사용하며 보통은 윈도우가 렌더텍스트를 내부에 숨기고 있어야 한다.
	  .텍스트 윈도우는 기본적으로 텍스트가 표시될 영역의 크기를 가질 수 있다.
		.따라서 윈도우의 크기와 실제 렌더된 텍스트의 크기는 다를수 있다.
		.만약 크기를 지정하지 않고 객체를 생성했다면 윈도우의 크기는 0이된다. 
			-이렇게 한 이유는 FTGL의 특성상 렌더된 텍스트의 레이아웃 크기를 구하는게 매우 느려서다. 그래서 보통의 왼쪽정렬 텍스트는 사이즈 없이 렌더만 되도록 사용한다.
			-또한 UI를 제작할때 텍스트윈도우의 크기를 일일히 지정해줘야하는 수고를 없애기 위해서다.
			-크기가 0인것은 auto의 의미다. 자동으로 m_vSize가 채워진다. 텍스트가 정렬속성이 있다거나 left정렬이지만 크기를 구하려 시도했으면 사이즈가 세팅된다. 그냥 일반적인 left정렬 텍스트는 사이즈가 계속 0이된다.
		.텍스트윈도우는 디폴트로 untouchable이다. 클릭 이벤트를 원한다면 명시적으로 touchable로 바꿔줘야 한다.
		.한번 지정된 윈도우크기는 직접 바꾸지 않는 한, 텍스트 레이아웃이 변해도 자동으로 바뀌지 않는다.
		.레이아웃이 변하는 경우는 정렬방식을 바꿨거나 문단가로폭, 폰트크기 등이 있다.
		.중앙정렬이나 우측정렬은 FTGL에서 지원하지만 세로정렬은 지원하지 않으므로 일관성을 위해서 윈도우가 직접 정렬좌표를 계산하도록 한다. 따라서 렌더링할때는 항상 LEFT정렬상태로 그리고 레이아웃크기 계산을 따로 한다.
		.만약 동적으로 텍스트 레이아웃의 크기에 따라 다른 윈도우들의 위치가 조정되어야 하는경우(e:퀘 안내창)
		컨트롤에서 직접 렌더된텍스트의 레이아웃크기와 좌상귀 위치를 얻어서 계산해야 한다.
		.이에따라 컨트롤은 텍스트레이아웃 크기(m_sizeString)과 더불어 중앙정렬의 경우 좌측 여백의 위치도 있으므로
		레이아웃 좌표(m_posString)도 보관하고 있어야 한다.
		.바뀐 레이아웃 크기를, 원한다면 윈도우크기로 바꿔써도 상관은 없다. 단 프로그래머가 명시적으로 지정해서 써야한다.
		.문단가로폭(m_lineLength)이 0인경우는 렌더링전 화면 해상도의 최대폭으로 자동적으로 사용되도록 하며 강제로 다른값으로 바꿔넣지 않는다(ex:640)
		.Layout.xml에서 pos:center를 하면 자동으로 SetAlign(center)가 되고 Draw시 텍스트컨트롤의 사이즈가 따로 지정되지 않았으면 실시간으로 부모의 크기를 따른다.
		.LEFT정렬은 기본적으로 레이아웃 크기를 구하지 않지만 명시적 요청이 들어오면 계산을 해야한다. 그러나 left정렬 상태에서 텍스트만 바뀌면 이전 크기는 클리어되고 다시 명시적 호출이 있을때까지 재 계산하지 않는다.
		.layout에서 length를 명시적으로 지정하지 않는한 윈도크기에 맞춰 자동으로 문단폭이 정해지면 안되는거 아닐까?
		.
*/
class XWndTextString;
class XEComponent;
////////////////////////////////////////////////////////////////////////
class XWndTextString : public XWnd
{
	enum xtBitFlag {
		xFLAG_NONE = 0,
		xFLAG_COMMA = 0x01,
		xFLAG_PASSWORD = 0x02,
	};
public:
	static XWndTextString* sUpdateCtrl( XWnd *pRoot
																		, const char *cIdentifier
																		, const XE::VEC2& vPos
																		, LPCTSTR szFont
																		, int sizeFont
																		, bool bShow = true );
	static XWndTextString* sUpdateCtrl( XWnd *pRoot
																		, const XE::VEC2& vPos 
																		, const char *cIdentifier
																		, bool bShow = true ) {
		return sUpdateCtrl( pRoot, cIdentifier, vPos, FONT_NANUM, 20, bShow );
	}
	static XWndTextString* sUpdateCtrl( XWnd *pRoot
																		, const char *cIdentifier
																		, const XE::VEC2& vPos
																		, bool bShow = true ) {
		return sUpdateCtrl( pRoot, cIdentifier, vPos, FONT_NANUM, 20, bShow );
	}
	static XWndTextString* sUpdateCtrl( XWnd* pParent
																		, const std::string& ids
																		, LPCTSTR szFont
																		, int sizeFont
																		, bool bShow );
private:
#ifdef _XNEW_WND_TEXT
	TCHAR *m_szString;
	int m_maxSize;		// 버퍼 최대 크기
#else
	TCHAR m_szString[1024];		// _tstring으로 하면 스트링이 바뀔때마다 동적할당으로 메모리위치가 바껴서 FTGL이 메모리주소 캐시로 인한 오동작을 한다.
#endif
	XBaseFontDat *m_pFontDat;
	XBaseFontObj *m_pFont;
	XCOLOR m_Color, m_colDisable;	// 활성화/비활성화 텍스트 색
	XCOLOR m_colBackground;
	XE::VEC2 m_sizeString;		// 화면상에 표시되는 순수한 텍스트만의 사이즈. 최적화를 위해 값이 없을수도 있다. m_vScale의 트랜스폼을 받지 않은 순수크기
	XE::VEC2 m_posString;			// 중앙이나 우측정렬일경우 렌더되는 텍스트의 실제 좌상귀 오프셋. 왼쪽정렬이면 0이다. 수직정렬의 경우도 마찬가지
	xFONT::xtStyle m_Style = xFONT::xSTYLE_NORMAL;
	BOOL m_bRetain;		// 삭제할것.
	XEComponentMng m_compMngByAlpha;
	float m_lineLength = 0;			// 디폴트값은 0이며 0은 FTGL에 넘겨질때 최대 해상도 값으로 넘겨진다.
	DWORD m_dwFlag = 0;
	bool m_bUpdateSizeAndPos = false;		// 사이즈계산을 한곳에서 일괄적으로 하기위함. SetAlign이나 SetLineLength가 변하면 즉시 계산하지 말고 플래그만 받아뒀다가 size가 필요한순간(GetLayoutSize)에 계산한다. 이 플래그가 true가 되면 무조건 재계산을 하는것이므로 중복계산을 피하려면 이것을 사용하는측에서 중복검사를 해야한다.
	bool m_bUpdateSizeIfChangeText = false;		// true가 되면 SetText()로 텍스트가 바꼈을때 사이즈를 재계산하고 부모에게 알린다.
//	bool m_bBatch = false;
	void Init() {
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
		SetbTouchable( false );		// 이제 텍스트도 크기를 가질수 있으므로 디폴트로 터치불가로 해야한다.
//		// 이제부턴 텍스터 컨트롤은 제로사이즈가 되어선 안된다.
	}
	void Destroy() { 
#ifdef _XNEW_WND_TEXT
		SAFE_DELETE_ARRAY( m_szString );
#endif
		DestroyFont();
	}
	void DestroyFont() { 
		SAFE_DELETE( m_pFont );
		if( m_bRetain )
			SAFE_RELEASE2( FONTMNG, m_pFontDat );
	}
protected:
public:
	XWndTextString();
	XWndTextString( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szString, XBaseFontDat *pFontDat = nullptr, XCOLOR col = XCOLOR_WHITE );
	XWndTextString( const XE::VEC2& vPos, const XE::VEC2& vSize, const _tstring& strString, XBaseFontDat *pFontDat = nullptr, XCOLOR col = XCOLOR_WHITE ) 
	: XWndTextString( vPos, vSize, strString.c_str(), pFontDat, col ) {	}
	// 윈도우의 크기는 지정하지 않는 버전
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
							XCOLOR col=XCOLOR_WHITE ) : XWndTextString( vPos, strString.c_str(), szFont, sizeFont, col ) {}
	XWndTextString( const XE::VEC2& vPos, float sizeFont = FONT_SIZE_DEFAULT )
		: XWndTextString( vPos, _T(""), FONT_SYSTEM, sizeFont, XCOLOR_WHITE ) { }
	// 윈도우의 크기를 명시적으로 지정하는 버전
	XWndTextString( const XE::VEC2& vPos, const XE::VEC2& vSize, const _tstring& strString,
									LPCTSTR szFont, float sizeFont,
									XCOLOR col = XCOLOR_WHITE ) 
									: XWndTextString( vPos, vSize, strString.c_str(), szFont, sizeFont, col )	{}
		
	// 윈도우의 좌표와 크기를 지정하지 않는버전. 좌표는 디폴트로 0이되고 크기는 0이된다. 이런경우 보통 부모의 위치와 크기에 따른다.
	XWndTextString( LPCTSTR szString,
									LPCTSTR szFont, float sizeFont,
									XCOLOR col = XCOLOR_WHITE );
	XWndTextString( const _tstring& str,
									LPCTSTR szFont, float sizeFont,
									XCOLOR col = XCOLOR_WHITE ) 
									: XWndTextString( str.c_str(), szFont, sizeFont, col ) {}

	virtual ~XWndTextString() { Destroy(); }
	//
//	virtual XE::VEC2 GetSizeForceFinal();
	///< 
	GET_ACCESSOR( XEComponentMng&, compMngByAlpha );
	inline void SetText( const _tstring& strText ) {
		SetText( strText.c_str() );
	}
	void SetText( LPCTSTR szString );
	void SetNumberText( int num );
	GET_ACCESSOR_CONST( LPCTSTR, szString );
	void SetbBatch( bool bFlag );
	//
	void SetFont( LPCTSTR szFont, float size );
	void SetAlign( XE::xAlign align ) override;
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
	inline void SetbComma( bool bFlag ) {
		if( bFlag )
			m_dwFlag |= xFLAG_COMMA;
		else
			m_dwFlag &= ~(xFLAG_COMMA);
	}
	inline bool IsComma() const {
		return (m_dwFlag & xFLAG_COMMA) != 0;
	}
	inline void SetbPassword( bool bFlag ) {
		if( bFlag )
			m_dwFlag |= xFLAG_PASSWORD;
		else
			m_dwFlag &= ~( xFLAG_PASSWORD );
	}
	inline bool IsPassword() const {
		return ( m_dwFlag & xFLAG_PASSWORD ) != 0;
	}
	inline void SetColorText( XCOLOR col ) {
		m_Color = col;
// 		m_pFont->SetColor( col );		// 최종 렌더 직전에 하도록 변경
	}
	inline void SetColorBackground( XCOLOR col ) {
		m_colBackground = col;
	}
	// 비활성화 됬을때 색
	inline void SetColorTextDisable( XCOLOR col ) {
		m_colDisable = col;
	}
	//
	void SetLineLength( float len );
	inline void SetStyle( xFONT::xtStyle style ) {
//		m_pFont->SetStyle( style );
		m_Style = style;
	}
	inline void SetStyleNormal() {
		m_Style = xFONT::xSTYLE_NORMAL;
//		m_pFont->SetStyle( xFONT::xSTYLE_NORMAL );
	}
	inline void SetStyleShadow() {
		m_Style = xFONT::xSTYLE_SHADOW;
// 		m_pFont->SetStyle( xFONT::xSTYLE_SHADOW );
	}
	inline void SetStyleStroke() {
		m_Style = xFONT::xSTYLE_STROKE;
// 		m_pFont->SetStyle( xFONT::xSTYLE_STROKE );
	}
	GET_ACCESSOR_CONST( XBaseFontObj*, pFont );
	//
	virtual void Draw() override;
	virtual void Draw( const XE::VEC2& vPos ) override;
	XE::xRECT GetBoundBoxByVisibleNoTrans() override {
		return XE::xRECT( GetLayoutPosNoTrans(), GetLayoutSizeNoTrans() );
	}
	/**
	 @brief 텍스트 레이아웃과 자식들을 포함한 최대 크기를 돌려준다. 
	 텍스트가 중앙정렬이 되어있어도 시작점은 0,0기준이며 윈도우의 폭이나 텍스트레이아웃의 폭중 큰값을 사용한다.
	*/
	XE::VEC2 GetSizeNoTransLayout() override;
 	void Update() override;
private:
	void Create( LPCTSTR szString, XBaseFontDat *pFontDat = nullptr, XCOLOR col = XCOLOR_WHITE );
	void Create( const XE::VEC2& vSize, LPCTSTR szString, LPCTSTR szFont, float sizeFont, XCOLOR col );
	int Process( float dt ) override;
	bool IsUpdateLayoutSize();
	bool IsUpdateLayoutPos();
	// 강제로 m_sizeString을 계산해 낸다. 이것은 LEFT정렬이라도 계산을 해야한다.
	void ForceCalcLayoutSize();
	void ForceCalcLayoutPos();
	// 이제 GetLayoutSize 시리즈는 외부에서 쓰지 못한다. GetLayoutSize만 하고 Pos는 안하는 실수를 방지하기 위해서이다. GetBoundBoxByVisibleNoTrans를 쓰기바란다.
	// 렌더된 텍스트의 실제 크기를 구한다. 이것은 윈도우 트랜스폼의 영향을 받지 않는다. 크기를 요청받은것이므로 LEFT정렬이라도 계산을 해야한다.
	// 단, 이미 계산되어있다면 다시 구하지 않는다.
	XE::VEC2 GetLayoutSizeNoTrans();
	inline XE::VEC2 GetLayoutSizeLocal(){
		return GetLayoutSizeNoTrans() * GetScaleLocal();
	}
	inline XE::VEC2 GetLayoutSizeFinal() {
		return GetLayoutSizeLocal() * GetScaleFinal();
	}
	// 레이아웃 좌상귀 좌표를 얻는다. 이것은 트랜스폼의 영향을 받지 않는다.
	XE::VEC2 GetLayoutPosNoTrans();
	// 레이아웃 좌상귀 좌표를 얻는다. 트랜스폼의 영향을 받는다.
	inline XE::VEC2 GetLayoutPosLocal(){
		return GetLayoutPosNoTrans() * GetScaleLocal();
	}
	inline XE::VEC2 GetLayoutPosFinal(){
		return GetLayoutPosNoTrans() * GetScaleFinal();
	}
	void AutoLayoutByAlign( const XWnd *pParent, XE::xAlign align ) override;
}; // XWndTextString




//////////////////////////////////////////////////////////////////////////
/**
 @brief 
*/
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
	void Draw() {
		RET val = (m_pOwner->*m_pFunc)( m_Param1 );
		XWndTextString::SetText( XE::Format( m_strFormat.c_str(), val ) );
		XWndTextString::Draw();
	}
};

XWndTextString* xGET_TEXT_CTRL( XWnd *pRoot, const char *cKey );
/**
 @brief 
*/
XWndTextString* xSET_TEXT( XWnd *pRoot, const char *cKey, LPCTSTR szText );
inline XWndTextString* xSET_TEXT( XWnd *pRoot, const char *cKey, const _tstring& strText ) {
	return xSET_TEXT( pRoot, cKey, strText.c_str() );
}

XWndTextString* xSET_TEXT( XWnd *pRoot, const char *cKey, int num );

#define xSET_TEXT_RET( RET, ROOT, KEY, TXT )  \
	XWndTextString *RET = dynamic_cast<XWndTextString*>( ROOT->Find( KEY ) ); \
	if( RET ) \
		RET->SetText( TXT ); \
	else { \
		XLOGXNA( "xSET_TEXT_RET: %s not found", KEY ); } 

XWndTextString* xSET_TEXTF( XWnd *pRoot, LPCTSTR text, const char *cKeyFormat, ... );
XWndTextString* xSET_TEXTF( XWnd *pRoot, const _tstring& text, const char *cKeyFormat, ... );
XWndTextString* xGET_TEXT_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... );
XWndTextString* xSET_TEXT_FORMAT( XWnd *pRoot, const char *cKey, LPCTSTR strFormat, ... );

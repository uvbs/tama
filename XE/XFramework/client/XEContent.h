#pragma once
#include "XApp.h"
//#include "XWindow.h"
#include "_Wnd2/XWnd.h"
#include "XClientMain.h"
#include "XFramework/XReceiverCallbackH.h"
// #include "XFramework/client/XWndBatchRender.h"

#define XGAME_SIZE		XE::GetGameSize()

namespace xnProfile {
struct xRESULT;
}
class XEBaseScene;
class XEContent;
class XWndView;
class XWndDarkBg;
class XRenderCmdMng;
class XTextureAtlas;
//
class XEContent : public XWnd
{
public:
	static XEContent* sGet() { return s_pInstance; }
	static XVector<xnWnd::xClickEvent> s_aryTooltip;
private:
	static XEContent* s_pInstance;
	XBaseFontDat *m_pfdSystem;		// 기본 시스템 폰트
	XBaseFontObj *m_pfoSystem;
	XBaseFontDat *m_pfdSystemSmall = nullptr;		// 기본 시스템 폰트(작은거)
	bool m_bTouched = false;		// 터치가 되어있는 상태인가
	bool m_bDraging = false;		// 드래깅중인가.
	XRenderCmdMng* m_pRenderer = nullptr;
	XTextureAtlas* m_pAtlas = nullptr;
	void Init() {
		m_pfdSystem = NULL;
		m_pfoSystem = NULL;
	}
	void Destroy();
protected:
public:
	XEContent();
	virtual ~XEContent() {
		Destroy();
	}
	GET_ACCESSOR( XBaseFontDat*, pfdSystem );
	GET_ACCESSOR( XBaseFontObj*, pfoSystem );
	GET_ACCESSOR( XBaseFontDat*, pfdSystemSmall );
	GET_BOOL_ACCESSOR( bDraging );
	GET_BOOL_ACCESSOR( bTouched );
	GET_ACCESSOR( XTextureAtlas*, pAtlas );
	GET_ACCESSOR( XRenderCmdMng*, pRenderer );
	//
	BOOL OnCreate();
	virtual void DidFinishCreated() = 0;
	BOOL CreateSystemFont();
	void DestroySystemFont();
	// handler
	//     virtual XBaseScene* GetpScene() { return NULL; }
	virtual XLua* GetpLua() { return NULL; }
	virtual void OnCheatMode() {}
	virtual void OnEndEditBox( int idEditField, const char *cStr ) {}
	virtual void OnPurchaseFinishedConsume( BOOL bGoogle, const char *cJson, const char *cidProduct, const char *cToken, const char *cPrice, const char *cOrderId, const char *cTransactDate ) {}
	virtual void OnPurchaseError( const char *cErr ) {}
	virtual void OnPurchaseStart( const char *cSku ) {}
	// pImg는 새로 할당받아서 넘기므로 받는측에서 삭제해야한다.
	virtual void OnRecvProfileImageByFacebook( const std::string& strFbUserId, DWORD* pImg, int w, int h, int bpp ) {}
	// 홈에서 다시 되돌아올때 호출됨.
	virtual void OnResume() {}
	// 홈버튼으로 나가기전 호출됨
	virtual void OnPause() {}
	virtual void OnReload() {
		CONSOLE( "OnReload" );
	}
	virtual void OnResultLoginAuthen( XE::xRESULT_AUTHEN *pResultAuthen ) {}
	virtual void OnFinishProfiling( XArrayLinear<xnProfile::xRESULT>& aryResult ) {}
	/// 모든 버튼UI는 눌리면 이게 호출된다.
	virtual void OnClickWnd( XWnd *pWnd ) {}
	BOOL RestoreDevice() override;
	XWnd* GetpRootScene();
	virtual void OnFinishAppearPopup( XWndView *pView ) {}
	virtual void OnClosePopup( const std::string& idsWnd ) {}
	virtual void OnBeforeDrawByPopup( XWndView *pView );
	virtual XWndDarkBg* CreateDarkBg();
	virtual void OnFinishLoadedByScene( XEBaseScene *pLoadedScene ) {}
	virtual bool OnReceiveCallbackData( const xnReceiverCallback::xData& data );
	// 하위 클래스는 게임에서 사용할 국가 코드(키)문자열을 리턴해야한다. lang.txt
	virtual const std::string OnSelectLanguageKey() = 0;
	int __OnClickTooltip( XWnd* pWnd, DWORD p1, DWORD p2 );
	virtual void OnClickTooltip( XWnd* pWnd ) {}
	virtual int Process( float dt ) override;
// 	void SetCurrRenderer();
// 	void ClearCurrRenderer();
// 	void SetCurrAtlas();
// 	void ClearCurrAtlas();
	virtual void OnLButtonDown( float lx, float ly ) override;
	virtual void OnLButtonUp( float lx, float ly ) override;
	virtual void OnMouseMove( float lx, float ly ) override;
};

#ifdef WIN32
inline void PUT_STRING_SMALL( const XE::VEC2& vPos, XCOLOR col, const _tstring& str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( vPos, str, col );
}

inline void PUT_STRING_SMALL( float x, float y, XCOLOR col, const _tstring& str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( x, y, str, col );
}

inline void PUT_STRING_SMALL( const XE::VEC2& vPos, XCOLOR col, LPCTSTR str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( vPos, str, col );
}

inline void PUT_STRING_SMALL( float x, float y, XCOLOR col, LPCTSTR str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( x, y, str, col );
}

inline void PUT_STRINGF_SMALL( float x, float y, XCOLOR col, LPCTSTR szFormat, ... ) {
	TCHAR szBuff[256];
	va_list vl;
	va_start( vl, szFormat );
	_vstprintf_s( szBuff, szFormat, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( x, y, szBuff, col );
}

inline void PUT_STRINGF_SMALL( const XE::VEC2& vPos, XCOLOR col, LPCTSTR szFormat, ... ) {
	TCHAR szBuff[256];
	va_list vl;
	va_start( vl, szFormat );
	_vstprintf_s( szBuff, szFormat, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( vPos, szBuff, col );
}
#endif // WIN32

inline void PUT_STRING_SMALL( const XE::VEC2& vPos, XCOLOR col, const std::string& str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( vPos, str, col );
}

inline void PUT_STRING_SMALL( float x, float y, XCOLOR col, const std::string& str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( x, y, str, col );
}

inline void PUT_STRING_SMALL( const XE::VEC2& vPos, XCOLOR col, const char* str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( vPos, str, col );
}

inline void PUT_STRING_SMALL( float x, float y, XCOLOR col, const char* str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( x, y, str, col );
}

inline void PUT_STRINGF_SMALL( float x, float y, XCOLOR col, const char* format, ... ) {
	char cBuff[256];
	va_list vl;
	va_start( vl, format );
	vsprintf_s( cBuff, format, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( x, y, cBuff, col );
}

inline void PUT_STRINGF_SMALL( const XE::VEC2& vPos, XCOLOR col, const char* format, ... ) {
	char cBuff[256];
	va_list vl;
	va_start( vl, format );
	vsprintf_s( cBuff, format, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystemSmall()->DrawString( vPos, cBuff, col );
}

#define PUT_STRINGFV( V, COL, F, ... ) \
	if( XE::GetMain()->GetpGame() ) \
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( V.x, V.y, COL, _T(F), ##__VA_ARGS__ );
#define PUT_STRINGFV_STYLE( V, COL, STYLE, F, ... ) \
	if( XE::GetMain()->GetpGame() ) \
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawStringStyle( V.x, V.y, COL, STYLE, _T(F), ##__VA_ARGS__ );
// 
inline void PUT_STRINGF2( float x, float y, XCOLOR col, LPCTSTR szFormat, ... ) {
	TCHAR szBuff[ 256 ];
	va_list vl;
	va_start( vl, szFormat );
	_vstprintf_s( szBuff, szFormat, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( x, y, szBuff, col );
}
inline void PUT_STRINGF2( const XE::VEC2& v, XCOLOR col, LPCTSTR szFormat, ... ) {
	TCHAR szBuff[ 256 ];
	va_list vl;
	va_start( vl, szFormat );
	_vstprintf_s( szBuff, szFormat, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( v.x, v.y, szBuff, col );
}
inline void PUT_STRINGF2_STROKE( const XE::VEC2& v, XCOLOR col, LPCTSTR szFormat, ... ) {
	TCHAR szBuff[ 256 ];
	va_list vl;
	va_start( vl, szFormat );
	_vstprintf_s( szBuff, szFormat, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( v.x, v.y, szBuff, col, xFONT::xSTYLE_STROKE );
}
inline void PUT_STRINGF2_SHADOW( const XE::VEC2& v, XCOLOR col, LPCTSTR szFormat, ... ) {
	TCHAR szBuff[256];
	va_list vl;
	va_start( vl, szFormat );
	_vstprintf_s( szBuff, szFormat, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( v.x, v.y, szBuff, col, xFONT::xSTYLE_SHADOW );
}
inline void PUT_STRING2_STROKE( const XE::VEC2& v, LPCTSTR szText, XCOLOR col = XCOLOR_WHITE ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( v.x, v.y, szText, col, xFONT::xSTYLE_STROKE );
}
inline void PUT_STRING2_STROKE( const XE::VEC2& v, const _tstring& strText, XCOLOR col = XCOLOR_WHITE ) {
	PUT_STRING2_STROKE( v, strText.c_str(), col );
}

inline void PUT_STRING2_SHADOW( const XE::VEC2& v, LPCTSTR szText, XCOLOR col ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( v.x, v.y, szText, col, xFONT::xSTYLE_SHADOW );
}
#define PUT_STRINGF_STYLE( X, Y, COL, STYLE, F, ... ) \
	if( XE::GetMain()->GetpGame() ) \
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawStringStyle( (float)X, (float)Y, COL, STYLE, _T(F), ##__VA_ARGS__ );

#define PUT_STRING_STYLE( X, Y, COL, STYLE, STR ) \
	if( XE::GetMain()->GetpGame() ) \
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( (float)X, (float)Y, STR, COL, STYLE );

#define PUT_STRINGF_SHADOW( X, Y, COL, F, ... ) \
	if( XE::GetMain()->GetpGame() ) \
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawStringStyle( (float)X, (float)Y, COL, xFONT::xSTYLE_SHADOW, _T(F), ##__VA_ARGS__ );

#define PUT_STRING_SHADOW( X, Y, COL, STR ) \
	if( XE::GetMain()->GetpGame() ) \
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( (float)X, (float)Y, STR, COL, xFONT::xSTYLE_SHADOW );

#define PUT_STRINGF_STROKE( X, Y, COL, F, ... ) \
	if( XE::GetMain()->GetpGame() ) \
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawStringStyle( (float)X, (float)Y, COL, xFONT::xSTYLE_STROKE, _T(F), ##__VA_ARGS__ );

#define PUT_STRING_STROKE( X, Y, COL, STR ) \
	if( XE::GetMain()->GetpGame() ) \
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( (float)X, (float)Y, STR, COL, xFONT::xSTYLE_STROKE );


#ifdef WIN32
inline void PUT_STRING( float x, float y, XCOLOR col, LPCTSTR szStr ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( x, y, szStr, col );
}

inline void PUT_STRING( const XE::VEC2& vPos, XCOLOR col, LPCTSTR szStr ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( vPos, szStr, col );
}
inline void PUT_STRING( float x, float y, XCOLOR col, const _tstring& str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( x, y, str, col );
}

inline void PUT_STRING( const XE::VEC2& vPos, XCOLOR col, const _tstring& str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( vPos, str, col );
}

inline void PUT_STRINGF( float x, float y, XCOLOR col, LPCTSTR format, ... ) {
	TCHAR szBuff[256];
	va_list vl;
	va_start( vl, format );
	_vstprintf_s( szBuff, format, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( x, y, szBuff, col );
}

inline void PUT_STRINGF( const XE::VEC2& vPos, XCOLOR col, LPCTSTR format, ... ) {
	TCHAR szBuff[256];
	va_list vl;
	va_start( vl, format );
	_vstprintf_s( szBuff, format, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( vPos, szBuff, col );
}
#endif // WIN32

// euckr버전
inline void PUT_STRING( float x, float y, XCOLOR col, const char* str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( x, y, str, col );
}

inline void PUT_STRING( const XE::VEC2& vPos, XCOLOR col, const char* str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( vPos, str, col );
}
inline void PUT_STRING( float x, float y, XCOLOR col, const std::string& str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( x, y, str, col );
}

inline void PUT_STRING( const XE::VEC2& vPos, XCOLOR col, const std::string& str ) {
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( vPos, str, col );
}

inline void PUT_STRINGF( float x, float y, XCOLOR col, const char* _format, ... ) {
	TCHAR format[64];
	_tcscpy_s( format, C2SZ( _format ) );
	TCHAR szBuff[256];
	va_list vl;
	va_start( vl, _format );
	_vstprintf_s( szBuff, format, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( x, y, szBuff, col );
}

inline void PUT_STRINGF( const XE::VEC2& vPos, XCOLOR col, const char* _format, ... ) {
	TCHAR format[64];
	_tcscpy_s( format, C2SZ( _format ) );
	TCHAR szBuff[256];
	va_list vl;
	va_start( vl, format );
	_vstprintf_s( szBuff, format, vl );
	va_end( vl );
	if( XE::GetMain()->GetpGame() )
		XE::GetMain()->GetpGame()->GetpfdSystem()->DrawString( vPos, szBuff, col );
}

#pragma once
#include "etc/XSurface.h"
#include "XWnd.h"
#include "etc/xMath.h"
#include "XImage.h"
#include "XFramework/Game/XEComponentMng.h"

class XEComponent;

//////////////////////////////////////////////////////////////////////////

class XWndImage : public XWnd
{
public:
	static XWndImage* sUpdateCtrl( XWnd *pRoot, const char *cIdentifier );
	static XWndImage* sUpdateCtrl( XWnd *pRoot, const std::string& strIdentifier ) {
		return sUpdateCtrl( pRoot, strIdentifier.c_str() );
	}
	static XWndImage* sUpdateCtrl( XWnd *pRoot
															, const XE::VEC2& vPos
															, LPCTSTR resImg
															, XE::xtPixelFormat formatSurface
															, const char *cIdentifier );
	static XWndImage* sUpdateCtrl( XWnd *pRoot
															, const XE::VEC2& vPos
															, LPCTSTR resImg
															, const char *cIdentifier ) {
		return sUpdateCtrl( pRoot, vPos, resImg, XE::xPF_ARGB1555, cIdentifier );
	}
	static XWndImage* sUpdateCtrl( XWnd *pRoot
															, const XE::VEC2& vPos
															, LPCTSTR resImg
															, const std::string& ids ) {
		return sUpdateCtrl( pRoot, vPos, resImg, XE::xPF_ARGB1555, ids.c_str() );
	}
	static XWndImage* sUpdateCtrl( XWnd *pRoot
															, const XE::VEC2& vPos
															, LPCTSTR resImg
															, bool bAlpha
															, const char *cIdentifier ) {
		return sUpdateCtrl( pRoot, vPos, resImg, XE::xPF_ARGB4444, cIdentifier );
	}
	static XWndImage* sUpdateCtrl( XWnd *pRoot
															, const XE::VEC2& vPos
															, const _tstring& resImg
															, bool bAlpha
															, const char *cIdentifier ) {
		return sUpdateCtrl( pRoot, vPos, resImg.c_str(), XE::xPF_ARGB4444, cIdentifier );
	}
	static XWndImage* sUpdateCtrl( XWnd *pRoot
															, const XE::VEC2& vPos
															, LPCTSTR resImg
															, bool bAlpha
															, const std::string& strIdentifier ) {
		return sUpdateCtrl( pRoot, vPos, resImg, XE::xPF_ARGB4444, strIdentifier.c_str() );
	}
	static XWndImage* sUpdateCtrl( XWnd *pRoot
															, const XE::VEC2& vPos
															, const _tstring& resImg
															, bool bAlpha
															, const std::string& strIdentifier ) {
		return sUpdateCtrl( pRoot, vPos, resImg.c_str(), XE::xPF_ARGB4444, strIdentifier.c_str() );
	}
private:
//	BOOL m_bCreate;			// 자체 생성한거라 파괴의 책임도 this에 있음을 알림
	XSurface *m_pSurface;
	XE::VEC2 m_vAdjust;
	XE::VEC2 m_vAdjustAxis;		// 회전/확축의 축 설정
	float m_dAngY;
	float m_dAngZ;
	BOOL m_bPixelPicking;		// 픽셀단위로 터치를 체크할것인가.
	XCOLOR m_Color = XCOLOR_WHITE;
	int m_Priority = 0;
//	XVector<XEComponent*> m_aryAlphaComponent;
	XEComponentMng m_compMngByAlpha;
	void Init() {
		m_pSurface = NULL;
		m_dAngY = 0;
		m_dAngZ = 0;
		m_strDebug = _T( "img" );
		m_bPixelPicking = FALSE;
	}
	void Destroy();
public:
	// 이제 이거 대신 XWndImage( LPCTSTR szRes, T x, T y ) 를 사용할것.
	template<typename T>
	XWndImage( BOOL bHighReso, LPCTSTR szRes, T x, T y )
		: XWnd( x, y ) {
		Init();
		if( XE::IsHave( szRes ) ) {
			Create( bHighReso, szRes );

			if( m_pSurface )
				SetSizeLocal( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
		}
	}
	template<typename T>
	XWndImage( LPCTSTR szRes, T x, T y, bool bMakeMask = false )
		: XWnd( x, y )	{
		Init();
		if( XE::IsHave( szRes ) ) {
			Create( TRUE, szRes, bMakeMask );
			if( m_pSurface )
				SetSizeLocal( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
			// 마스킹을 만들면 디폴트로 픽셀픽킹모드로 된다.
			if( bMakeMask )
				SetbPixelPicking( TRUE );
		}
	}
	template<typename T>
	XWndImage( const _tstring& strRes, T x, T y, bool bMakeMask = false )
		: XWnd( x, y )	{
		Init();
		if( strRes.empty() == false ) {
			Create( TRUE, strRes.c_str(), bMakeMask );
			if( m_pSurface )
				SetSizeLocal( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
			// 마스킹을 만들면 디폴트로 픽셀픽킹모드로 된다.
			if( bMakeMask )
				SetbPixelPicking( TRUE );
		}
	}
	XWndImage( const _tstring& strRes, const XE::VEC2& vPos, bool bMakeMask = false )
		: XWndImage( strRes, vPos.x, vPos.y, bMakeMask ) {}
	XWndImage( LPCTSTR szRes, const XE::VEC2& vPos, bool bMakeMask = false )
		: XWndImage( szRes, vPos.x, vPos.y, bMakeMask ) {}
	XWndImage( const _tstring& strRes )
		: XWndImage( strRes, 0.f, 0.f, false ) {}
	XWndImage( LPCTSTR szRes )
		: XWndImage( szRes, 0.f, 0.f, false ) {}
	XWndImage( BOOL bHighReso, float x, float y, float w, float h )
		: XWnd( x, y ) {
		Init();
		SetSizeLocal( w, h );
	}
	XWndImage( XSurface *pSurface, float x, float y )
		: XWnd( x, y ) {
		Init();
		m_pSurface = pSurface;
		if( pSurface ) {
			m_pSurface->IncRefCnt();
			SetSizeLocal( pSurface->GetWidth(), pSurface->GetHeight() );
		}
	}
	template<typename T>
	XWndImage( LPCTSTR szRes, T x, T y, const XE::VEC2& sizeTexture ) 
		: XWnd( x, y ) {
		Init();
		if( XE::IsHave( szRes ) ) {
			Create( TRUE, szRes, FALSE, sizeTexture );
			if( m_pSurface )
				SetSizeLocal( m_pSurface->GetWidth(), m_pSurface->GetHeight() );
// 			// 마스킹을 만들면 디폴트로 픽셀픽킹모드로 된다.
// 			if( bMakeMask )
// 				SetbPixelPicking( TRUE );
		}
	}
	// pixelFormat지정 버전
	XWndImage( LPCTSTR szRes, XE::xtPixelFormat formatSurface, float x, float y, bool bMakeMask = false );
	XWndImage( LPCTSTR szRes, XE::xtPixelFormat formatSurface, const XE::VEC2& vPos, bool bMakeMask = false )
		: XWndImage( szRes, formatSurface, vPos.x, vPos.y, bMakeMask ) {}
	XWndImage( const _tstring& strRes, XE::xtPixelFormat formatSurface, float x, float y, bool bMakeMask = false )
		: XWndImage( strRes.c_str(), formatSurface, x, y, bMakeMask ) {}
	XWndImage( const _tstring& strRes, XE::xtPixelFormat formatSurface, const XE::VEC2& vPos, bool bMakeMask = false )
		: XWndImage( strRes.c_str(), formatSurface, vPos.x, vPos.y, bMakeMask ) {}
	XWndImage( const XE::VEC2& vPos ) : XWnd( vPos ) {
		Init();
	}
	// batch모드용
	XWndImage( const _tstring& strRes, 
						 bool bBatch,
						 XE::xtPixelFormat formatSurface, 
						 const XE::VEC2& vPos );
	XWndImage() {
		Init();
	}
	virtual ~XWndImage() { Destroy(); }
	//
	virtual BOOL IsWndAreaIn( float lx, float ly );
	GET_ACCESSOR( BOOL, bPixelPicking );
protected:
	GET_SET_ACCESSOR_CONST( const XE::VEC2&, vAdjust );
public:
	GET_SET_ACCESSOR_CONST( int, Priority );
// 	GET_SET_ACCESSOR( xDM_TYPE, Effect );
// 	inline void SetBlendFunc( XE::xtBlendFunc func ) {
// 		m_Effect = XE::ConvertBlendFuncDMTypeDmType( func );
// 	}
	void SetbPixelPicking( BOOL bPixelPicking ) {
		// 이미지 로딩시 마스킹 생성 옵션으로 로딩하지 않으면 픽셀픽킹은 못함.
		XBREAK( m_pSurface && m_pSurface->GetpMask() == NULL );
		m_bPixelPicking = bPixelPicking;
	}
	void SetAdjustAxis( const XE::VEC2& vAxis ) {
		m_vAdjustAxis = vAxis;
	}
private:
	void Create( BOOL bHighReso, LPCTSTR szImg, bool bMakeMask = false, const XE::VEC2& sizeTexture = XE::VEC2() );
	void Create( LPCTSTR szRes, XE::xtPixelFormat format, bool bBatch, bool bMakeMask );
public:
	GET_SET_ACCESSOR_CONST( XCOLOR, Color );
	void Draw() override;
	virtual void Draw( XE::VEC2& vPos, xDM_TYPE drawMode/*=xDM_NORMAL*/ );
	void Draw( const XE::VEC2& vPos ) override;
	void GetMatrix( MATRIX *pOut ) override;
	void GetDebugString( _tstring& strOut ) override;
	MATRIX* GetMatrix( MATRIX *pOut, float lx, float ly );
	// bAutoSizeParent: this의 크기를 포함해서 부모의 전체 바운드박스영역을 재 조정한다.
	void SetSurface( LPCTSTR szRes, XE::xtPixelFormat formatSurface, bool bAutoSizeParent = false );
	void SetSurface( LPCTSTR szRes, bool bAutoSizeParent = false );
	void SetSurface( const _tstring& strRes, bool bAutoSizeParent = false ) {
		SetSurface( strRes.c_str(), bAutoSizeParent );
	}
	void SetSurface( XSurface *pSurface );
	void SetSurface( LPCTSTR szRes, bool bBatch, XE::xtPixelFormat formatSurface, bool bAutoSizeParent );
	inline void SetSurfacePtr( XSurface *pSurface ) {
		SetSurface( pSurface );
	}
	inline void SetSurfaceWithpSurface( XSurface *pSurface ) {
		SetSurface( pSurface );
	}
	inline void SetSurfaceRes( LPCTSTR resImg ) {
		SetSurface( resImg );
	}
	inline void SetSurfaceRes( const _tstring& strRes ) {
		SetSurfaceRes( strRes.c_str() );
	}
	inline void SetSurfaceRes( const _tstring& strRes, XE::xtPixelFormat format ) {
		SetSurface( strRes.c_str(), format );
	}
	inline void SetSurfaceRes( const _tstring& strRes, XE::xtPixelFormat format, bool bBatch ) {
		SetSurface( strRes.c_str(), bBatch, format, bBatch );
	}
	void SetSurface( const char* cKey, DWORD *pImg, int w, int h, bool bAutoSizeParent );
	inline void SetSurface( const char* cKey, const XE::xImage& imgRaw, bool bAutoSizeParent = false ) {
		SetSurface( cKey, imgRaw.m_pImg, imgRaw.m_ptSize.w, imgRaw.m_ptSize.h, bAutoSizeParent );
	}
	inline void SetSurfaceWithRaw( const char* cKey, const XE::xImage& imgRaw, bool bAutoSizeParent = false ) {
		SetSurface( cKey, imgRaw.m_pImg, imgRaw.m_ptSize.w, imgRaw.m_ptSize.h, bAutoSizeParent );
	}
	inline void SetSurfaceWithRaw( const std::string& strKey, const XE::xImage& imgRaw, bool bAutoSizeParent = false ) {
		SetSurface( strKey.c_str(), imgRaw.m_pImg, imgRaw.m_ptSize.w, imgRaw.m_ptSize.h, bAutoSizeParent );
	}
	XSurface* GetpSurface()	{
		return m_pSurface;
	}
// 	void SetAlign( XE::xAlign align ) {
// 		switch( align ) {
// 		case XE::xALIGN_LEFT:		m_vAdjust = XE::VEC2( 0 );	break;
// 		case XE::xALIGN_RIGHT:	m_vAdjust.w -= GetSizeFinal().w;		break;
// 		case XE::xALIGN_CENTER:	m_vAdjust -= GetSizeFinal() / 2.f;		break;
// 		case XE::xALIGN_BOTTOM:	m_vAdjust.y = -GetSizeFinal().h;	break;
// 		}
// 	}
	// 돌리려면 AdjustAxis로 축도 지정해줘야 한다.
	void SetRotateY( float dAng ) {
		m_dAngY = dAng;
	}
	void SetRotateZ( float dAng ) {
		m_dAngZ = dAng;
	}
	//
	GET_ACCESSOR( XEComponentMng&, compMngByAlpha );
	int Process( float dt ) override;
	XEComponent* AddComponentWave( const std::string& ids
																, float secDelay = 1.f
																, float secFade = 1.f
																, float minValue = 0.f
																, float maxValue = 1.f );
//	void Update() override;	
private:
//	void DoAlign();
//	void UpdateAlign();
}; // WndImage

inline XWndImage* xSET_IMG( XWnd *pRoot, const char *cKey, LPCTSTR szRes, XE::xtPixelFormat format = XE::xPF_ARGB1555 ) {
	XBREAK( pRoot == NULL );
	auto pImg = SafeCast<XWndImage*>( pRoot->Find( cKey ) );
	if( pImg )
		pImg->SetSurfaceRes( szRes, format );
	else
		XLOGXN( "xSET_IMG: %s not found. img file=%s", C2SZ( cKey ), szRes );
	return pImg;
};

inline XWndImage* xSET_IMG( XWnd *pRoot, const char *cKey, const _tstring& strRes, XE::xtPixelFormat format = XE::xPF_ARGB1555 ) {
	return xSET_IMG( pRoot, cKey, strRes.c_str(), format );
}

inline XWndImage* xSET_IMGF( XWnd *pRoot, LPCTSTR szRes, const char *cKeyFormat, ... ) {
	XBREAK( pRoot == NULL );
	char cKey[256];
	va_list vl;
	va_start( vl, cKeyFormat );
	vsprintf_s( cKey, cKeyFormat, vl );
	va_end( vl );
	auto pImg = SafeCast<XWndImage*>( pRoot->Find( cKey ) );
	if( pImg )
		pImg->SetSurfaceRes( szRes );
	else
		XLOGXN( "xSET_IMGF: %s not found. img file=%s", C2SZ( cKey ), szRes );
	return pImg;
};

#define xSET_IMG2	xSET_IMG_RET
#define xSET_IMG_RET( RET, ROOT, KEY, RES ) \
	XBREAK( ROOT == NULL ); \
	XWndImage *RET = SafeCast<XWndImage*, XWnd*>( ROOT->Find( KEY ) ); \
	if( RET ) \
		RET->SetSurface( RES ); \
		else \
		XLOGXN( "xSET_IMG2: %s not found. img file=%s", C2SZ( KEY ), RES ); \

XWndImage* xGET_IMAGE_CTRLF( XWnd *pRoot, const char *cKeyFormat, ... );
XWndImage* xGET_IMAGE_CTRL( XWnd *pRoot, const char *cKey );

#define xSET_IMG_SEQ( ROOT, KEY, SEQ, RES ) { \
	XBREAK( ROOT == NULL ); \
	char __cBuff[ 256 ]; \
	sprintf_s( __cBuff, "%s%d", KEY, SEQ ); \
	XWndImage *_pImg = SafeCast<XWndImage*, XWnd*>( ROOT->Find( __cBuff ) ); \
	if( _pImg ) \
		_pImg->SetSurfaceRes( RES ); \
				else \
		XLOGXN( "xSET_IMG_SEQ: %s not found. img file=%s", C2SZ( __cBuff ), RES ); \
}


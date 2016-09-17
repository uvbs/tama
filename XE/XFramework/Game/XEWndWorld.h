#pragma once
#include "_Wnd2/XWnd.h"
//#include "XFramework/XScroll.h"
//#include "XEWorld.h"
//#include "XFramework/client/XWndBatchRender.h"

////////////////////////////////////////////////////////////////
class XEWorld;
class XBaseFontDat;
class XEBaseWorldObj;
class XRenderCmdMng;
class XTextureAtlas;
class XEWorldCamera;
/**
 XEWorld보다 좀더 구체적으로 화면에 월드를 그리는 추상화 객체
*/
class XEWndWorld : public XWnd//, public XScroll
{
public:
private:
	static XEWndWorld* s_pInstance;
	XSPWorld m_spWorld;
	std::shared_ptr<XEWorldCamera> m_spCamera;		// 바인딩된 카메라
// 	XE::VEC2 m_vwCamera;		// 카메라 위치. z는 카메라와 월드판과의 거리. -1이 100% 상태 -2는 줌아웃50%
// 	float m_scaleCamera;		// 카메라 배율
// 	float m_scaleMin, m_scaleMax;	// 카메라 배율 최소/최대
	XRenderCmdMng* m_pRenderer = nullptr;
	XTextureAtlas* m_pAtlas = nullptr;
	void Init() {
// 		m_scaleCamera = 1.0f;
// 		m_scaleMin = m_scaleMax = 1.0f;
	}
	void Destroy();
// 	SET_ACCESSOR( const XE::VEC2&, vwCamera );	// 카메라 위치를 바꾸려면 
protected:
	SET_ACCESSOR( XSPWorld, spWorld );
//	SET_ACCESSOR( float, scaleCamera );
public:
//	XEWndWorld();
	XEWndWorld( XSPWorld spWorld );
	virtual ~XEWndWorld() { Destroy(); }
	//
	virtual void Release() override;
	virtual BOOL OnCreate() override;
	GET_ACCESSOR( XSPWorld, spWorld );
// 	GET_ACCESSOR( const XE::VEC2&, vwCamera );
// 	GET_ACCESSOR( float, scaleCamera );
	//GET_ACCESSOR( XRenderCmdMng*, pBatchRenderer );
	GET_ACCESSOR( XTextureAtlas*, pAtlas );
// 	GET_SET_ACCESSOR_CONST( float, scaleMin );
// 	GET_SET_ACCESSOR_CONST( float, scaleMax );
// 	void SetScaleCamera( float scale ) {
// 		XScroll::SetScale( scale );
// 		m_scaleCamera = scale;
// 	}
// 	// 카메라의 중앙을 설정한다.
// 	void SetFocus( const XE::VEC2& vFocus ) {
// 		XScroll::SetFocus( vFocus );
// 		SetvwCamera( XScroll::GetvCenter() );
// 	}
	//
	GET_SET_ACCESSOR( std::shared_ptr<XEWorldCamera>, spCamera );
	ID AddObj( const XSPWorldObj& spObj );
//	void DrawDebugInfo( float x, float y, XCOLOR col, XBaseFontDat *pFontDat ) override;
	BOOL IsOutBoundary( const XE::xRECT& rectBB );
	//
	virtual void OnLButtonDown( float lx, float ly ) override {
		XWnd::OnLButtonDown( lx, ly );
//		XScroll::OnLButtonDown( lx, ly );
	}
	virtual void OnMouseMove( float lx, float ly ) override {
		XWnd::OnMouseMove( lx, ly );
//		XScroll::OnMouseMove( lx, ly );
	}
	virtual void OnLButtonUp( float lx, float ly ) override {
		XWnd::OnLButtonUp( lx, ly );
//		XScroll::OnLButtonUp( lx, ly );
	}
	// 확대/축소 동작
// 	virtual void OnZoom( float scale, float lx, float ly ) override;
	virtual int Process( float dt ) override;
	virtual void Draw() override;
	// 월드좌표를 스크린전체 좌표로 변환한다.
	XE::VEC2 GetPosWorldToScreen( const XE::VEC3& vwPos, float *pOutScale=NULL ) {
		return GetPosWorldToWindow( vwPos, pOutScale ) + GetPosLocal();
	}
	// 월드좌표를 윈도우내 로컬좌표로 변환한다.
	XE::VEC2 GetPosWorldToWindow( const XE::VEC3& vwPos, float *pOutScale );
	XE::VEC2 GetPosWindowToWorld( const XE::VEC2& vlsPos );
private:
	void SetCurrRenderer();
	void ClearCurrRenderer();
	void SetCurrAtlas();
	void ClearCurrAtlas();
};


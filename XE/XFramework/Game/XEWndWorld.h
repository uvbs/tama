#pragma once
#include "_Wnd2/XWnd.h"
#include "XFramework/XScroll.h"
#include "XEWorld.h"

////////////////////////////////////////////////////////////////
class XEWorld;
class XBaseFontDat;
class XEBaseWorldObj;
/**
 XEWorld보다 좀더 구체적으로 화면에 월드를 그리는 추상화 객체
*/
class XEWndWorld : public XWnd, public XScroll
{
	XEWorld *m_pWorld;
	XE::VEC2 m_vwCamera;		// 카메라 위치. z는 카메라와 월드판과의 거리. -1이 100% 상태 -2는 줌아웃50%
	float m_scaleCamera;		// 카메라 배율
	float m_scaleMin, m_scaleMax;	// 카메라 배율 최소/최대
	void Init() {
		m_pWorld = NULL;
		m_scaleCamera = 1.0f;
		m_scaleMin = m_scaleMax = 1.0f;
	}
	void Destroy();
	SET_ACCESSOR( const XE::VEC2&, vwCamera );	// 카메라 위치를 바꾸려면 
protected:
	SET_ACCESSOR( XEWorld*, pWorld );
	GET_SET_ACCESSOR( float, scaleMin );
	GET_SET_ACCESSOR( float, scaleMax );
//	SET_ACCESSOR( float, scaleCamera );
public:
	XEWndWorld();
	XEWndWorld( XEWorld *pWorld );
	virtual ~XEWndWorld() { Destroy(); }
	//
	virtual void Release( void );
	GET_ACCESSOR( XEWorld*, pWorld );
	GET_ACCESSOR( const XE::VEC2&, vwCamera );
	GET_ACCESSOR( float, scaleCamera );
	void SetScaleCamera( float scale ) {
		XScroll::SetScale( scale );
		m_scaleCamera = scale;
	}
	// 카메라의 중앙을 설정한다.
	void SetFocus( const XE::VEC2& vFocus ) {
		XScroll::SetFocus( vFocus );
		SetvwCamera( XScroll::GetvCenter() );
	}
	//
	ID AddObj( const WorldObjPtr& spObj ) {
		return m_pWorld->AddObj( spObj );
	}
	void DrawDebugInfo( float x, float y, XCOLOR col, XBaseFontDat *pFontDat ) override;
	BOOL IsOutBoundary( const XE::xRECT& rectBB );
	//
	virtual void OnLButtonDown( float lx, float ly ) {
		XWnd::OnLButtonDown( lx, ly );
		XScroll::OnLButtonDown( lx, ly );
	}
	virtual void OnMouseMove( float lx, float ly ) {
		XWnd::OnMouseMove( lx, ly );
		XScroll::OnMouseMove( lx, ly );
	}
	virtual void OnLButtonUp( float lx, float ly ) {
		XWnd::OnLButtonUp( lx, ly );
		XScroll::OnLButtonUp( lx, ly );
	}
	// 확대/축소 동작
	virtual void OnZoom( float scale, float lx, float ly );
	virtual int Process( float dt );
	virtual void Draw( void );
	// 월드좌표를 스크린전체 좌표로 변환한다.
	virtual XE::VEC2 GetPosWorldToScreen( const XE::VEC3& vwPos, float *pOutScale=NULL ) {
		return GetPosWorldToWindow( vwPos, pOutScale ) + GetPosLocal();
	}
	// 월드좌표를 윈도우내 로컬좌표로 변환한다.
	virtual XE::VEC2 GetPosWorldToWindow( const XE::VEC3& vwPos, float *pOutScale=NULL );
	virtual XE::VEC2 GetPosWindowToWorld( const XE::VEC2& vlsPos );
	
};


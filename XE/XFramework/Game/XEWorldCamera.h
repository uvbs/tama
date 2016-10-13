/********************************************************************
	@date:	2016/09/17 15:37
	@file: 	C:\p4v\iPhone_zero\XE\XFramework\Game\XEWorldCamera.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XFramework/XScroll.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/09/17 15:37
*****************************************************************/
class XEWorldCamera : public XScroll
{
public:
	XEWorldCamera( const XE::VEC2& sizeWorld, const XE::VEC2& sizeView, float scaleCamera );
	~XEWorldCamera() {
		Destroy();
	}
	// get/setter
	GET_ACCESSOR_CONST( const XE::VEC2&, vwCamera );
	GET_ACCESSOR_CONST( float, scaleCamera );
	GET_SET_ACCESSOR_CONST( float, scaleMin );
	GET_SET_ACCESSOR_CONST( float, scaleMax );
	// public member
	void SetScaleCamera( float scale ) {
		XScroll::SetScale( scale );
		m_scaleCamera = scale;
	}
	// 카메라의 중앙을 설정한다.
	void SetFocus( const XE::VEC2& vFocus ) {
		XScroll::SetFocus( vFocus );
		SetvwCamera( XScroll::GetvCenter() );
	}
	void OnLButtonDown( float lx, float ly ) {
		XScroll::OnLButtonDown( lx, ly );
	}
	void OnMouseMove( float lx, float ly ) {
		XScroll::OnMouseMove( lx, ly );
	}
	void OnLButtonUp( float lx, float ly ) {
		XScroll::OnLButtonUp( lx, ly );
	}
	void Process( float dt );
	XE::VEC2 GetPosWorldToWindow( const XE::VEC3& vwPos, float *pOutScale );
	XE::VEC2 GetPosWindowToWorld( const XE::VEC2& vlsPos );
	void OnZoom( float scale, float lx, float ly );
private:
	// private member
	XE::VEC2 m_vwCamera;		// 카메라 위치. z는 카메라와 월드판과의 거리. -1이 100% 상태 -2는 줌아웃50%
	float m_scaleCamera;		// 카메라 배율
	float m_scaleMin, m_scaleMax;	// 카메라 배율 최소/최대
private:
	// private method
	void Init() {}
	void Destroy() {}
	SET_ACCESSOR( const XE::VEC2&, vwCamera );	// 카메라 위치를 바꾸려면 
}; // class XEWorldCamera

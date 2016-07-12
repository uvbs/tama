/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XWnd.h"
//#include "XFramework/client/XLayoutObj.h"
#include "XFramework/XScroll.h"

class XWndList;
class XWndDarkBg;
class XLayoutObj;

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/25 16:11
*****************************************************************/
class XDelegateScrollView
{
public:
	XDelegateScrollView() {}
	virtual ~XDelegateScrollView() {}
	// get/setter
	// public member
	virtual void DelegateStopScroll( const XE::VEC2& vCurr ) {}
}; // class XDelegateScrollView

	 ////////////////////////////////////////////////////////////////
class XWndScrollView : public XWnd, public XScroll
{
	XE::VEC2 m_vAdjScroll;
	BOOL m_bScroll;
	XE::VEC2 m_vTotalSize;			// 스크롤뷰의 실제 크기(창크기 말고)
	XE::VEC2 m_vScrollLast;
	XE::VEC2 m_vMouse;
	XE::VEC2 m_vTouch;	// 최초 터치한 위치
	XE::VEC2 m_vPrev;		// 이전 마우스 위치
	float m_Alpha;
	CTimer m_Timer;
	XWndScrollView *m_pAttach;		// this스크롤뷰가 스크롤될때 똑같이 스크롤될 스크롤뷰.
	bool m_bToggle = false;
	XE::VEC2 m_vStopScrollPrev;		// StopScroll이벤트를 위해 마지막으로 업데이트가 되었던 좌표
	XDelegateScrollView* m_pDelegate = nullptr;
	void Init() {
		m_bScroll = FALSE;
		m_Alpha = 0;
		m_pAttach = NULL;
	}
	SET_ACCESSOR( const XE::VEC2&, vAdjScroll );
protected:
	GET_ACCESSOR_CONST( BOOL, bScroll );	// protectecd로 한 특별한 이유는 없음.
public:
	XWndScrollView( float x, float y, float w, float h );
	XWndScrollView( const XE::VEC2& vPos, const XE::VEC2& vSize );
	virtual ~XWndScrollView() {}
	//
	GET_ACCESSOR_CONST( const XE::VEC2&, vAdjScroll );
	GET_SET_ACCESSOR( XWndScrollView*, pAttach );
	GET_SET_ACCESSOR_CONST( XDelegateScrollView*, pDelegate );
	void Clear() {
		m_vAdjScroll.Set( 0 );
	}
	/**
	 전체 스크롤뷰의 크기를 세팅
	*/
	void SetViewSize( const XE::VEC2& vSize );
	const XE::VEC2& GetViewSize() {
		return m_vTotalSize;
	}
	XE::VEC2 GetPosFinal() const override { 
		return m_vAdjScroll;
	}
	virtual XE::VEC2 GetPosScreen() {
		if( m_pParent ) {
			return m_pParent->GetPosScreen() + ( GetPosLocal() + m_vAdjScroll ) * m_pParent->GetScaleLocal(); // child일때만 좌표까지 스케일해주고. 부모의 스케일과 내 스케일을 곱해준걸 써야한다 
		}
		else
			return GetPosLocal();
	}
	XE::VEC2 GetPosAdjust() override {
		return m_vAdjScroll;
	}
	// 뚫린창의 좌상귀 화면 좌표를 계산한다.
	XE::VEC2 GetPosWindow() override {
		return XWnd::GetPosFinal();
	}
	void SetFocusView( float wx, float wy );
	inline void SetFocusView( const XE::VEC2& vCenter ) {
		return SetFocusView( vCenter.x, vCenter.y );
	}
	XE::VEC2 GetFocusView() {
		return XScroll::GetvCenter();
	}

	void Scroll();
	void DrawScrollBar();
protected:
public:
	XE::VEC2 SetScrollViewAutoSize();	// 이제 이거 안불러도 됨.
	virtual int Process( float dt );
	virtual void Draw();
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnLButtonDownCancel();
	virtual void OnNCLButtonUp( float lx, float ly );
	virtual void OnNCMouseMove( float lx, float ly );
	virtual void OnFinishCreatedChildLayout( XLayout *pLayout );
	virtual void OnScroll();
//friend class XWndScrollView;
}; // XWndScrollView

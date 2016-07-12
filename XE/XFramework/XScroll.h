#pragma once
#include "etc/Types.h"
#include "etc/Global.h"
#include "xVector.h"
/**
 마우스 입력을 받고 어떤 숫자와 숫자 사이를 스크롤 시키는 추상적인 객체
 모든 스크롤류는 이 객체를 상속받는다.
*/
class XScroll
{
public:
	// 스크롤 허용방향
	enum xtType {
		xTP_NONE,
		xTP_HORIZ,
		xTP_VERT,
		xTP_ALL,
	};
private:
	BOOL m_bTouchDown;
	BOOL m_bTouchUp;
	BOOL m_bScrolling;
	XE::VEC2 m_vScroll;			// 이번턴에 스크롤되어야 하는 양.
//	XE::VEC2 m_vScrollLast;		
	XE::VEC2 m_vScrollAvg;		// 최근 스크롤속도의 평균값
	XE::VEC2 m_vCurr;			// 현재 스크롤 위치(좌상귀)
	XE::VEC2 m_vCenter;			// m_vCurr이 갱신될때 중앙값이 미리 구해둬야한다(외부에서 scale값이 먼저 변해버리면 잘못 계산됨)
	XE::VEC2 m_vMouse;
	XE::VEC2 m_vMousePrev;
	XE::VEC2 m_sizeView;		// 스크롤되는 뷰(?)의 크기
	XE::VEC2 m_sizeScroll;		// 스크롤되는 영역의 전체 크기(자동계산)
	XE::VEC2 m_vStartScroll;	// 스크롤되는 시작 숫자(좌상귀 시작좌표)
	XE::VEC2 m_vEndScroll;		// 스크롤되는 끝 숫자(우하귀 끝좌표)
	xtType m_typeScroll;
	float m_Scale;				// 확축되는 스크롤에 사용
//	CTimer m_Timer;
	BOOL m_bZoom;
	bool m_bScrlLock = false;		// 일시적으로 스크롤을 막는다.
	bool m_bFirstUpdate = false;		// 최초 업데이트 한번부르기 위해 땜빵함.
	void Init() {
		m_bTouchDown = FALSE;
		m_bTouchUp = FALSE;
		m_typeScroll = xTP_NONE;
		m_bScrolling = FALSE;
		m_bZoom = FALSE;
		m_Scale = 1.0f;
	}
	void Destroy() {}
protected:
	GET_ACCESSOR( BOOL, bTouchDown );
	GET_ACCESSOR( BOOL, bTouchUp );
	GET_ACCESSOR( xtType, typeScroll );
public:
	XScroll() { Init(); }
	XScroll( const XE::VEC2& vStartScroll, 
			  const XE::VEC2& vEndScroll,
			  const XE::VEC2& vCurrent,
			  const XE::VEC2& sizeView, 
			  XScroll::xtType typeScroll );
	virtual ~XScroll() { Destroy(); }
	//
	void SetLock( bool flag ) {
		if( flag ) {
			m_bTouchDown = FALSE;
			m_bScrolling = FALSE;
			m_bZoom = FALSE;
		}
	}
	bool IsLock() {
		return m_bScrlLock;
	}
	// 스크롤객체가 작동중이냐
	BOOL IsActive() {
		return (m_typeScroll != xTP_NONE);		// 스크롤객체의 작동중임은 m_typeScroll의 초기화 여부로 판단.
	}
	GET_ACCESSOR( const XE::VEC2&, vCurr );
	XE::VEC2& GetvCurrLT() {
		return m_vCurr;
	}
	void SetvLT( const XE::VEC2& vCurr );
	void SetvCenter( const XE::VEC2& vCenter ) {
		SetFocus( vCenter );
	}
	GET_ACCESSOR( const XE::VEC2&, sizeView );
	GET_ACCESSOR( float, Scale );
	GET_ACCESSOR( const XE::VEC2&, vCenter );
	GET_ACCESSOR( const XE::VEC2&, sizeScroll );
	BOOL IsScroll( void ) {
		return m_vScroll.IsZero() == FALSE;
	}
	BOOL IsVertOnly( void ) {
		return m_typeScroll == xTP_VERT;
	}
	BOOL IsHorizOnly( void ) {
		return m_typeScroll == xTP_HORIZ;
	}
	BOOL IsFreeScroll( void ) {
		return m_typeScroll == xTP_ALL;
	}
	void SetVertOnly( void ) {
		m_typeScroll = xTP_VERT;
	}
	inline void SetScrollVertOnly() {
		SetVertOnly();
	}
	void SetHorizOnly( void ) {
		m_typeScroll = xTP_HORIZ;
	}
	inline void SetScrollHorizOnly() {
		SetHorizOnly();
	}
	void SetFreeScroll( void ) {
		m_typeScroll = xTP_ALL;
	}
	void SetScrollAllDir() {
		SetFreeScroll();
	}
	// 스크롤 허용방향을 지정한다.
	void SetScrollDir( xtType typeScroll ) {
		m_typeScroll = typeScroll;
	}
	void SetScrollDir( XE::xtCrossDir dir ) {
		switch( dir ) {
		case XE::xVERT:		SetScrollVertOnly();	break;
		case XE::xHORIZ:	SetScrollHorizOnly();	break;
		case XE::xVERT_HORIZ_ALL:	SetScrollAllDir(); break;
		default:
			XBREAK(1);
			break;
		}
	}
	void SetScale( float scale ) {
		m_Scale = scale;
		FitScroll();
	}
	void SetFocus( float wx, float wy );
	void SetFocus( const XE::VEC2& vWin ) {
		SetFocus( vWin.x, vWin.y );
	}
	void CancelTouchDown( void ) {
		m_bTouchDown = FALSE;
		m_bScrolling = FALSE;
	}
	//
protected:
	void InitScroll( const XE::VEC2& vStartScroll, 
						const XE::VEC2& vEndScroll,
						const XE::VEC2& vCurrent,
						const XE::VEC2& sizeView, 
						XScroll::xtType typeScroll );
public:
	void UninitScroll( void ) {
		Init();
		m_vScroll.Set(0);
		m_vScrollAvg.Set(0);
//		m_vScrollLast.Set(0);
		m_vCurr.Set(0);
		m_vMouse.Set(0);
		m_vMousePrev.Set(0);
		m_sizeScroll.Set(0);
//		m_Timer.Off();
	}
	void FitScroll( void );
	int Process( float dt );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnLButtonDownCancel( void ) {
		m_bTouchDown = FALSE;
		m_bTouchUp = FALSE;
		m_bScrolling = FALSE;
	}
	void OnZoom( float lx, float ly, float scale );
	virtual void OnScroll( void ) {}
	virtual void OnStopScroll() {}
	void SetFocusTop();
	void SetFocusBottom();
	void SetFocusLeft();
	void SetFocusRight();
}; // XScroll


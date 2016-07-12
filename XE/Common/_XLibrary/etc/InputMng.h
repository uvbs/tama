#pragma once

#include "xe.h"
#include "etc/global.h"
#ifdef _CHEAT
#include "etc/Timer.h"
#endif

namespace XE {
    enum xtDevice {
        DEVICE_UNKNOWN,
        DEVICE_IPHONE,
        DEVICE_IPAD,
        DEVICE_IPOD,
        DEVICE_ANDROID,
		DEVICE_WINDOWS,
    };
	enum xtKey {
		KEY_NONE,
		KEY_BACK=1000,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_UP,
		KEY_DOWN,
		KEY_DEL,
	};
  LPCTSTR GetDeviceString( xtDevice device );
};
class XInputMng
{
public:
	static XInputMng *s_pInputMng;
    static XE::xtDevice s_Device;
	enum xtState {
		xTS_NONE,
		xTS_TOUCHED,	// 터치중이다.
	};
protected:
	XE::VEC2 m_vPos;		// 마우스 좌표
	XE::VEC2 m_vPrevPos;	// 이전 마우스 좌표
	xtState m_State;
	int m_cntTouches;		// 터치한 손가락 수
#ifdef _CHEAT
	CTimer m_timerPush;		// 터치중인 시간
#endif
	
private:
	void _Init( void ) {
		m_State = xTS_NONE;
		m_cntTouches = 0;
	}
	void _Destroy( void ) {}
public:
	XInputMng() { _Init(); }
	virtual ~XInputMng() { _Destroy(); }
	//
    GET_ACCESSOR(XE::VEC2&, vPrevPos);
	GET_ACCESSOR(xtState, State);
	GET_SET_ACCESSOR( int, cntTouches );
/*
	void SetState( xtState state ) {
#ifdef _CHEAT
		if( m_State == xTS_NONE && state == xTS_TOUCHED )	// 누른순간
			m_timerPush.Set(0);
		else
		if( m_State == xTS_TOUCHED && state == xTS_NONE )	// 뗀순간
			m_timerPush.Off();
#endif
		m_State = state;
	}
*/
#ifdef _CHEAT
	GET_ACCESSOR(CTimer&, timerPush);
#endif
	const XE::VEC2& GetMousePos( void ) { return m_vPos; }
	void SetMousePos( float x, float y ) {
        m_vPrevPos = m_vPos;
        m_vPos.x = x;
        m_vPos.y = y;
    }
    void SetMousePos( XE::VEC2& vPos ) {
        m_vPrevPos = m_vPos;
        m_vPos = vPos;
    }
//	virtual void SetMousePos( int x, int y ) { m_vPos.x = (float)x; m_vPos.y = (float)y; }
//	void SetPrevPos( int x, int y ) { m_vPrevPos.x = (float)x; m_vPrevPos.y = (float)y; }
//	void ClearUpMsg( void );
};

#define INPUTMNG	XInputMng::s_pInputMng

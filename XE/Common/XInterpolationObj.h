#pragma once
#include "etc/Timer.h"
#include "etc/xMath.h"

namespace XE {
	enum xtITP {
		xITP_NONE,
		xITP_LINEAR,
		xITP_SIN,
		xITP_DEACCEL,
		xITP_ACCEL,
		xITP_CATMULLROM,
		xITP_PULSE,
	};
};
class XInterpolationObj
{
public:
private:
	CTimer m_Timer;
	XE::xtITP m_itpFunc;
	BOOL m_bLoop;
	float m_timeLerp;
	void Init() {
		m_itpFunc = XE::xITP_NONE;
		m_bLoop = FALSE;
		m_timeLerp = 0;
	}
	void Destroy() {}
protected:
	void Set( float sec, XE::xtITP itpFunc, BOOL bLoop ) {
		m_Timer.Set( sec );
		m_itpFunc = itpFunc;
		m_bLoop = bLoop;
	}
public:
	XInterpolationObj() { Init(); }
	virtual ~XInterpolationObj() { Destroy(); }
	//
	void Off( void ) {
		Init();
		m_Timer.Off();
	}
	BOOL IsOn( void ) {
		return (m_itpFunc != XE::xITP_NONE);
	}
	BOOL IsOff( void ) {
		return (m_itpFunc == XE::xITP_NONE);
	}
	float GetInterpolationValue( void ) {
		// 0초~세팅된초 까지를 0~1.0으로 보고 현재 시간이 어디까지 왔는지 알아낸다.
		float timeLerp = m_Timer.GetSlerp();
		if( timeLerp > 1.0f )
			timeLerp = 1.0f;
		m_timeLerp = timeLerp;
		// 각자 보간계산법에 의해 보간값을 계산한다.
		float itpValue = GetVirtualInterpolation( timeLerp );
		// 시간이 끝났고 반복속성이면 시간을 리셋한다.
		if( timeLerp >= 1.0f && m_bLoop )
		{
			m_Timer.Reset();
			// m_timeLerp는 여기서 초기화 하지 않는다. 이번턴에서 timeLerp가 1.0인 조건을 써야 할때가 있기때문에,.
		}
		return itpValue;
	}
	BOOL IsFinish( void ) {
		return( m_timeLerp >= 1.0f );
	}
	float GetTimeLerp( void ) {
		return m_Timer.GetSlerp();
	}
	//
	virtual float GetVirtualInterpolation( float timeLerp )=0;
}; // class XInterpolationObj
//////////////////////////////////////////////////////////////////////////
// Sin Interpolation
class XInterpolationObjSin : public XInterpolationObj
{
		float m_min;
		float m_max;
		float m_slope;
	void Init() {
		m_min = m_max = m_slope = 0;
	}
	void Destroy() {}
public:
	XInterpolationObjSin() { Init(); }
	virtual ~XInterpolationObjSin() { Destroy(); }
	//
	void Set( float sec, float min, float max, float slope, BOOL bLoop ) {
		XInterpolationObj::Set( sec, XE::xITP_SIN, bLoop );
		m_min = min;
		m_max = max;
		m_slope = slope;
	}
	// 초기화(Set)와 동시에 보간값을 읽고 임의로 종료시키기 전까지 초기화 시키지 않는다.
	float GetInterpolationValueWithSet( float sec, float min, float max, float slope, BOOL bLoop=FALSE ) {
		if( XInterpolationObj::IsOff() ) {
			Set( sec, min, max, slope, bLoop );
			return 0;
		} else {
			return GetInterpolationValue();
		}
	}
	virtual float GetVirtualInterpolation( float timeLerp ) {
		float itpValue = XE::xiSin( timeLerp, m_min, m_max, m_slope );
		return itpValue;
	}
}; // sin 

//////////////////////////////////////////////////////////////////////////
// Linear Interpolation
class XInterpolationObjLinear : public XInterpolationObj
{
	float m_min;
	float m_max;
	void Init() {
		m_min = m_max = 0;
	}
	void Destroy() {}
public:
	XInterpolationObjLinear() { Init(); }
	virtual ~XInterpolationObjLinear() { Destroy(); }
	//
	void Set( float sec, float min, float max, BOOL bLoop ) {
		XInterpolationObj::Set( sec, XE::xITP_LINEAR, bLoop );
		m_min = min;
		m_max = max;
	}
	// 초기화(Set)와 동시에 보간값을 읽고 임의로 종료시키기 전까지 초기화 시키지 않는다.
	float GetInterpolationValueWithSet( float sec, float start, float end, BOOL bLoop=FALSE ) {
		if( XInterpolationObj::IsOff() ) {
			Set( sec, start, end, bLoop );
			return 0;
		} else {
			return GetInterpolationValue();
		}
	}
	virtual float GetVirtualInterpolation( float timeLerp ) {
		float size = m_max - m_min;
		float itpValue = m_min + XE::xiLinearLerp( timeLerp ) * size;
		return itpValue;
	}
}; // linear

//////////////////////////////////////////////////////////////////////////
// DeAccel(감속) Interpolation
class XInterpolationObjDeAccel : public XInterpolationObj
{
	float m_A;
	float m_B;
	void Init() {
		m_A = m_B = 0;
	}
	void Destroy() {}
public:
	XInterpolationObjDeAccel() { Init(); }
	virtual ~XInterpolationObjDeAccel() { Destroy(); }
	//
	void Set( float sec, BOOL bLoop ) {
		XInterpolationObj::Set( sec, XE::xITP_DEACCEL, bLoop );
		m_A = 1.0f;
		m_B = 0;
	}
	// 초기화(Set)와 동시에 보간값을 읽고 임의로 종료시키기 전까지 초기화 시키지 않는다.
	float GetInterpolationValueWithSet( float sec, BOOL bLoop=FALSE ) {
		if( XInterpolationObj::IsOff() ) {
			Set( sec, bLoop );
			return 0;
		} else {
			return GetInterpolationValue();
		}
	}
	virtual float GetVirtualInterpolation( float timeLerp ) {
		float itpValue = XE::xiHigherPowerDeAccel( timeLerp, m_A, m_B );
		return itpValue;
	}
}; // deAccel

//////////////////////////////////////////////////////////////////////////
// Accel(가속) Interpolation
class XInterpolationObjAccel : public XInterpolationObj
{
	float m_A;
	float m_B;
	void Init() {
		m_A = m_B = 0;
	}
	void Destroy() {}
public:
	XInterpolationObjAccel() { Init(); }
	virtual ~XInterpolationObjAccel() { Destroy(); }
	//
	void Set( float sec, BOOL bLoop ) {
		XInterpolationObj::Set( sec, XE::xITP_ACCEL, bLoop );
		m_A = 1.0f;
		m_B = 0;
	}
	// 초기화(Set)와 동시에 보간값을 읽고 임의로 종료시키기 전까지 초기화 시키지 않는다.
	float GetInterpolationValueWithSet( float sec, BOOL bLoop=FALSE ) {
		if( XInterpolationObj::IsOff() ) {
			Set( sec, bLoop );
			return 0;
		} else {
			return GetInterpolationValue();
		}
	}
	virtual float GetVirtualInterpolation( float timeLerp ) {
		float itpValue = XE::xiHigherPowerAccel( timeLerp, m_A, m_B );
		return itpValue;
	}
}; // Accel

//////////////////////////////////////////////////////////////////////////
// Catmullrom(곡선) Interpolation
class XInterpolationObjCatmullrom : public XInterpolationObj
{
	float m_p[4];
	void Init() {
		XCLEAR_ARRAY( m_p );
	}
	void Destroy() {}
public:
	XInterpolationObjCatmullrom() { Init(); }
	virtual ~XInterpolationObjCatmullrom() { Destroy(); }
	//
	void Set( float sec, float p0, float p1, float p2, float p3, BOOL bLoop ) {
		XInterpolationObj::Set( sec, XE::xITP_CATMULLROM, bLoop );
		m_p[0] = p0;
		m_p[1] = p1;
		m_p[2] = p2;
		m_p[3] = p3;
	}
	// 초기화(Set)와 동시에 보간값을 읽고 임의로 종료시키기 전까지 초기화 시키지 않는다.
	// -10.0f, 0, 1, 1
	float GetInterpolationValueWithSet( float sec, float p0=-10.f, float p1=0, float p2=1.f, float p3=1.f, BOOL bLoop=FALSE ) {
		if( XInterpolationObj::IsOff() ) {
			Set( sec, p0, p1, p2, p3, bLoop );
			return 0;
		} else {
			return GetInterpolationValue();
		}
	}
	virtual float GetVirtualInterpolation( float timeLerp ) {
		float itpValue = XE::xiCatmullrom( timeLerp, m_p[0], m_p[1], m_p[2], m_p[3] );
		return itpValue;
	}
}; // Catmullrom

//////////////////////////////////////////////////////////////////////////
// Pulse(박동) Interpolation
class XInterpolationObjPulse : public XInterpolationObj
{
	float m_max;
	float m_endLerp;
	void Init() {
		m_max = 0;
		m_endLerp = 0;
	}
	void Destroy() {}
public:
	XInterpolationObjPulse() { Init(); }
	virtual ~XInterpolationObjPulse() { Destroy(); }
	//
	void Set( float sec, float max, float endLerp, BOOL bLoop ) {
		XInterpolationObj::Set( sec, XE::xITP_PULSE, bLoop );
		m_max = max;
		m_endLerp = endLerp;
	}
	// 초기화(Set)와 동시에 보간값을 읽고 임의로 종료시키기 전까지 초기화 시키지 않는다.
	float GetInterpolationValueWithSet( float sec, float max, float endLerp, BOOL bLoop=FALSE ) {
		if( XInterpolationObj::IsOff() ) {
			Set( sec, max, endLerp, bLoop );
			return 0;
		} else {
			return GetInterpolationValue();
		}
	}
	virtual float GetVirtualInterpolation( float timeLerp ) {
		float itpValue = XE::xiPulse( timeLerp, m_max, m_endLerp );
		return itpValue;
	}
}; // Pulse

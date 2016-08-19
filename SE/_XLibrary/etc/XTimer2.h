#pragma once
#include "debug.h"

class XTimer2
{
private:
	float m_Timer;
	float m_WaitTime;
	float m_Speed;		// 타이머스피드 배수. 2면 시간이 두배로 빨리 간다는것.
	BOOL m_bActive;
public:
	XTimer2() {
		m_Timer = 0;
		m_WaitTime = 0;
		m_Speed = 1.0f;
		m_bActive = FALSE;
	}
	XTimer2( float sec ) {
		m_Timer = 0;
		m_WaitTime = 0;
		m_Speed = 1.0f;
		m_bActive = FALSE;
		Set( sec );
	}
	virtual ~XTimer2() {}
	//
	void _Set( float timer, float waitTime, float speed ) {
		m_Timer = timer;
		m_WaitTime = waitTime;
		m_Speed = speed;
		m_bActive = TRUE;
	}
	BOOL		IsOn( void ) { return m_bActive; }
	BOOL		IsOff( void ) { return !m_bActive; }
	void		Off( void ) { m_bActive = FALSE; }
	float GetWaitTime( void ) { return m_WaitTime; }
	float _GetTimer( void ) { return m_Timer; }
	void	Set( float fWaitTime );
	void	Reset( void );
	float	GetPassTime( void );
	float GetRemainTime( void )	{ return m_Timer; }	// 남은 시간
	BOOL		IsOver( void );
	BOOL		IsOver( float fOverTime );
	float GetSlerp( void ) {		// 세팅된시간중에 몇퍼센트나 흘러갔는지..0~1의 값으로 돌려준다. WaitTime이 지정되어있어야 한다
		float slerp = GetPassTime() / GetWaitTime();
		if( slerp > 1.0f )		slerp = 1.0f;
		return slerp;
	}		
	GET_ACCESSOR( float, Speed );
	void SetSpeed( float scale ) {
		m_Speed = scale;
	}
	void Update( float dt );
};




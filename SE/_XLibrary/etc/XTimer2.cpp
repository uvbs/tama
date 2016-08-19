#include "stdafx.h"
#include "XTimer2.h"

//
//
void	XTimer2::Set( float fWaitTime )
{
	m_bActive = TRUE;
	m_Timer = fWaitTime;
	m_WaitTime = fWaitTime;
}

void	XTimer2::Reset( void )
{
	m_bActive = TRUE;
	m_Timer = m_WaitTime;
}

void XTimer2::Update( float dt )
{
	m_Timer -= ((1/60.f) * dt) * m_Speed;
}
// 지나간 시간
float XTimer2::GetPassTime( void )
{
	return m_WaitTime - m_Timer;
}
BOOL	XTimer2::IsOver( void  )
{
	if( IsOff() )	return FALSE;
	return m_Timer <= 0;
}

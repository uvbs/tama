#include "stdafx.h"
#include <time.h>
#include "XTimer2.h"
#include "XArchive.h"
#include "Timer.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// 1970/1/1 0시부터 흐른 현재까지의 초를 구한다.
xSec XTimer2::sGetTime() 
{ 
	return (DWORD)time(NULL); 
}

//
//
/**
 secWaitTime초 후에 IsOver()를 TRUE로 만든다.
*/
void	XTimer2::Set( float secWaitTime )
{
	m_bActive = TRUE;
	m_secTimer = 0;
	m_secWaitTime = secWaitTime;
}

void	XTimer2::Reset()
{
	m_bActive = TRUE;
	m_secTimer = 0;
}

void XTimer2::Update( float dt )
{
	if( IsOn() )
		m_secTimer += (((double)1/60.0) * (double)dt) * (double)m_Speed;
}
// 지나간 시간
float XTimer2::GetsecPassTime() const
{
	return (float)m_secTimer;
}

double XTimer2::GetsecPassTime64() const
{
	return m_secTimer;
}

BOOL XTimer2::IsOver() const
{
	if( IsOff() )	
		return FALSE;
	// 타이머가 세팅한 시간을 넘어서면 TRUE를 리턴한다.
	if( m_secTimer > (double)m_secWaitTime && m_secWaitTime != 0 )
		return TRUE;
	return FALSE;
}

BOOL XTimer2::IsOver( float secWait ) const
{
	if( IsOff() )	
		return FALSE;
	// 타이머가 세팅한 시간을 넘어서면 TRUE를 리턴한다.
	if( m_secTimer > (double)secWait )
		return TRUE;
	return FALSE;
}

/**
 @brief 현재 흘러간시간에 secAdd를 더했을때 시간이 오버되는지 검사
*/
BOOL XTimer2::IsAddOver( double secAdd ) const
{
	if( m_secTimer + secAdd > (double)m_secWaitTime )
		return TRUE;
	return FALSE;
}

void XTimer2::Serialize( XArchive& ar ) const
{
	XBREAK( xVER_TIMER2 > 0xff );
	ar << (BYTE)xVER_TIMER2;
	ar << (BYTE)( m_Speed * 100 );
	ar << (BYTE)m_bActive;
	ar << (BYTE)0;
	ar << m_secTimer;
	ar << (int)m_secWaitTime;
		
}

/**
 @brief m_secTimer는 0초 기반이어서 받는측에서 별다른 처리없이 시간동기화 된다.
*/
BOOL XTimer2::DeSerialize( XArchive& ar, int _ver )
{
	BYTE b0;
	int ver, sec;
	if( _ver == 0 ) {
		ar >> m_secTimer;
		ar >> b0; m_Speed = (float)b0 / 100.f;
		ar >> b0;	m_bActive = (BOOL)b0;
		ar >> b0;	m_secWaitTime = (float)b0;
		ar >> b0;
	} else {
		ar >> b0;	ver = b0;
		ar >> b0; m_Speed = (float)b0 / 100.f;
		ar >> b0;	m_bActive = (BOOL)b0;
		ar >> b0;
		ar >> m_secTimer;
		ar >> sec;	m_secWaitTime = (float)sec;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//xSec XTimerTiny::s_secSaved = 0;

int XTimerTiny::Serialize( XArchive& ar ) const
{
	XBREAK( xVER_TIMER_TINY > 0xff );
	XBREAK( (DWORD)m_secWait > 0x00ffffff );
	DWORD dw0;
	dw0 = (xVER_TIMER_TINY << 24);
	dw0 |= ((DWORD)m_secWait & 0x00ffffff);
//	ar << xVER_TIMER_TINY;
	ar << dw0;		// ver과 secWait값이 함께 들어있음.
	if( m_secStart == 0 ) {
		ar << (DWORD)0;
		ar << (DWORD)-1;
	} else {
		DWORD secCurr = XTimer2::sGetTime();
		DWORD secPass = secCurr - m_secStart;
		if( (int)secPass < 0 )
			secPass = 0;
		ar << secCurr;	// 저장할당시의 시간을 함께 저장한다.a
		ar << secPass;
//		XTRACE("Se:sGetTime:%d secPass=%d\n", secCurr, secPass );
	}
	return 1;
}
/**
 @brief 
 @param ver 내부에 전용 ver을 가지게 바껴서 ver값은 무시함.
*/
int XTimerTiny::DeSerialize( XArchive& ar, int ) 
{
	DWORD dw0;
	int ver;
	ar >> dw0;
	if( dw0 == 1 ) {
		ver = (int)dw0;
	} else {
		ver = (int)((dw0 >> 24) & 0xff);
		m_secWait = (int)(dw0 & 0x00ffffff);		// 헤더4바이트중 3바이트를 사용.
		XBREAK( ver <= 0 || ver > xVER_TIMER_TINY );
	}
//	ar >> ver;
	DWORD secPass, secSaved = 0;
	ar >> secSaved;
	ar >> secPass;
	if( secPass == 0xffffffff ) {
		m_secStart = 0;
	} else {
#ifdef _CLIENT
		// 서버/클라 시간 동기용. 클라와 서버는 시간이 차이가 날 수 있다. 그러므로 서버에선 실제 지나간 시간만을 저장해서 클라에게 던져주고 클라에선 클라자신의 현재시간에서 흘러간시간을 빼서 서버와 동기화를 시킨다.
		const xSec secServerToClient = 1;		// 서버에서 클라로 오는 시간도 있으므로 1초를 보정함.
		m_secStart = XTimer2::sGetTime() - secPass + secServerToClient;
#else
		// (DB용)저장한후로부터 지나간 시간을 얻는다. 오프라인시간
		const auto secCurr = XTimer2::sGetTime();
		xSec secOffline = 0;
		if( secSaved )
			secOffline = secCurr - secSaved;		// 저장한후 오프라인동안의 시간
		///< note: 저장한 서버와 로딩한 서버는 시간대가 같다는 가정이어야 한다. 만약 한국에서 저장한 데이타를 미국에서 로딩했다고 가정하면 시간대가 다르므로 오프라인동안 흘러간시간을 제대로 파악할 수 없다.
		m_secStart = secCurr - secPass - secOffline;
#endif 
// 		XTRACE( "De:sGetTime:%d secPass=%d secSaved=%d secPassSaved=%d secStart=%d\n", 
// 			secCurr, secPass, secSaved, secPassSaved, m_secStart );

	}
	return 1;
}

void XTimerTiny::RestoreTimer( xSec secSaved, xSec secPass ) 
{
	ClearTimer();
	if( secPass == 0xffffffff )
		return;
	const auto secCurr = XTimer2::sGetTime();
	const auto secOffline = secCurr - secSaved;
	m_secStart = secCurr - secPass - secOffline;
}

bool XTimerTiny::IsOver( xSec secMax ) const 
{
	if( m_secStart == 0 )
		return false;
	xSec secPass = XTimer2::sGetTime() - m_secStart;
//	XBREAK( (int)secPass < 0 );
	if( (int)secPass < 0 )
		secPass = 0;
	if( secPass >= secMax )
		return true;
	return false;
}

xSec XTimerTiny::GetsecPass() const 
{
	if( m_secStart == 0 )
		return 0xffffffff;
	const auto secPass = XTimer2::sGetTime() - m_secStart;
	return ((int)secPass < 0)? 0 : secPass;
}

int XTimerTiny::GetsecRemain() const 
{
	const auto secPass = (int)GetsecPass();
	if( secPass == 0xffffffff )
		return 0xffffffff;
	XBREAK( m_secWait == 0 );
	int secRemain = m_secWait - secPass;
	if( secRemain < 0 )
		secRemain = 0;
	return secRemain;
}

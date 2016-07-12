#include "stdafx.h"
#include "etc/Timer.h"
#include "XArchive.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//double CTimer::s_dTimer = 0;			// 실제시간이 아닌 pause가능한 시간으로 쓰기위해 필요.
DWORD CTimer::s_dTimer = 0;			// 실제시간이 아닌 pause가능한 시간으로 쓰기위해 필요.
//BOOL CTimer::s_bRealTimer = 1;			// 실제시간모드가 디폴트
unsigned long long CTimer::s_llFreqPerSec = 0;	// 초당 틱수

#pragma comment( lib, "winmm" )		// winmm.lib 추가
//
//
//
void	CTimer::Set( float secWaitTime, BOOL bRealTimer )
{
//	XBREAK( fWaitTime == 0 );
	m_bActive = TRUE;
	m_bRealTimer = bRealTimer;
	m_dwWaitTime = (DWORD)(secWaitTime * TICKS_PER_SEC);
	DWORD dwCurrTime = GetTime();
//	m_dwPrevTime = dwCurrTime;
	m_dwStartTime = dwCurrTime;
	m_dwPassTime = 0;
}

void	CTimer::Reset( void )
{
	m_bActive = TRUE;
	DWORD dwCurrTime = GetTime();
//	m_dwPrevTime = dwCurrTime;
	m_dwStartTime = dwCurrTime;
	m_dwPassTime = 0;
}

// 시간을 저장했다 로딩해 쓸때 사용. 지나간시간을 적용하여 셋한다.
void CTimer::SetContinue( DWORD msTimeWait, DWORD msTimePass, BOOL bRealTimer )
{
	m_bActive = TRUE;
	m_bRealTimer = bRealTimer;
	m_dwWaitTime = msTimeWait;
	DWORD dwCurrTime = GetTime();
	m_dwStartTime = dwCurrTime - msTimePass;
//	m_dwPrevTime = dwCurrTime;
}
// 지나간 시간
DWORD CTimer::GetPassTime( void ) const
{
	if( m_bActive == FALSE )
		return 0;
	DWORD dwCurrTime = GetTime();
	return dwCurrTime - m_dwStartTime + m_dwPassTime;
}

void CTimer::SetPassSec( float sec ) 
{
	m_dwPassTime = (DWORD)(sec * TICKS_PER_SEC);
}

BOOL	CTimer::IsOver( float secOverTime ) const
{
	if( IsOff() )	return FALSE;
	DWORD dwCurrTime = GetTime();
	if( dwCurrTime - m_dwStartTime + m_dwPassTime >= (DWORD)(secOverTime * TICKS_PER_SEC) )
		return TRUE;
	return FALSE;
}
BOOL	CTimer::IsOver( void  ) const
{
	if( IsOff() )	return FALSE;
	DWORD dwCurrTime = GetTime();
	if( dwCurrTime - m_dwStartTime + m_dwPassTime >= m_dwWaitTime )
		return TRUE;
	
	return FALSE;
}
// dwEveryTime마다 TRUE를 리턴
// Reset()한번 해주는걸로 준비는 끝난다.
/*BOOL	CTimer::IsEvery( float fEveryTime )
{
	if( IsOn() == FALSE )		return FALSE;
	DWORD dwCurrTime = GetTime();
	if( dwCurrTime - m_dwPrevTime >= (DWORD)(fEveryTime * TICKS_PER_SEC) )
	{
		m_dwPrevTime = dwCurrTime;
		return TRUE;
	}
	return FALSE;
}*/

/**
 @brief 
 @note 리얼타임이 아니므로 시리얼라이즈후에 다시 풀때는 그 사이시간이 적용되지 않는다.
 리얼타임으로 쓰려면 XTimer2::sGetTime()을 사용해야 한다.
*/
int CTimer::Serialize( XArchive& ar ) const
{
	DWORD dw1 = 0;
	if( m_bActive )
		dw1 |= 0x01;
	if( m_bOverSlerp )
		dw1 |= 0x02;
	ar << dw1;
	if( m_bActive )
	{
		ar << m_dwWaitTime;
		// 지나간 시간을 넣음.
		DWORD tickPass = GetPassTime();
		ar << tickPass;
	}
	return 1;
}
int CTimer::DeSerialize( XArchive& ar )
{
	DWORD dw1;
	ar >> dw1;
	if( dw1 & 0x01 )
		m_bActive = TRUE;
	else
		m_bActive = FALSE;
	if( m_bActive )
	{
		if( dw1 & 0x02 )
			m_bOverSlerp = TRUE;
		else
			m_bOverSlerp = FALSE;
		ar >> m_dwWaitTime;
		DWORD tickPass;
		ar >> tickPass;		// 지나간 틱을 꺼내서
		m_dwStartTime = GetTime() - tickPass;	// 현재시간에서 지나간시간을 빼주면 시작한시간을 알수 있다.
	}
	return 1;
}

// 현재시간:h24Curr가 start~end시간 범위내에 있는지 검사한다.
BOOL CTimer::sIsHourIn( int h24Start, int h24End, int h24Curr )
{
	int start = h24Start;
	int end = h24End;
	if( start == 24 )
		start = 0;
	if( end == 0 )
		end = 24;
	if( end <= start )	// 20시~08시 같은경우 20시~32시 형태로 바꿈
		end += 24;
	// 1차 검사. 현재시간이 start~end범위에 있는지 검사
	if( h24Curr >= start && h24Curr <= end )
		return TRUE;
	// 없으면 2차검사를 위해 현재시간에 24시간을 더함
	h24Curr += 24;
	if( h24Curr >= start && h24Curr <= end )
		return TRUE;
	return FALSE;
}

//#ifdef _VER_IOS
#ifndef WIN32
#include <sys/time.h>
#include <time.h>

DWORD timeGetTime( void )
{
	struct timeval now;
	gettimeofday( &now, NULL );
	// sec: 초단위
	// usec: 마이크로초단위(백만분의1)
	DWORD nMillisec = (now.tv_sec * 1000) + (now.tv_usec / 1000 );
	return nMillisec;
}
/*
// 현재주파수 얻기. 현재시간을 마이크로세컨드 단위로 변환한다.
void QueryPerformanceCounter( LARGE_INTEGER *pllTimer )
{
	struct timeval now;
	gettimeofday( &now, NULL);
	
	pllTimer->QuadPart = (now.tv_sec * 1000000) + now.tv_usec;
}

// 초당주파수 얻기.
void QueryPerformanceFrequency( LARGE_INTEGER *pllFreq )	// for MAC
{
	pllFreq->QuadPart = 1000000;
}
*/

unsigned long long XE::GetFreqTime( void ) 
{
	struct timeval now;
	gettimeofday( &now, NULL );
	// sec: 초단위
	// usec: 마이크로초단위(백만분의1)
	DWORD nMillisec = (now.tv_sec * CTimer::GetFreqPerSec()) + now.tv_usec;
	return nMillisec;
}

void GetLocalTime( SYSTEMTIME *stTime )
{
	struct tm temp;
	struct tm *t = &temp;
	memset( &temp, 0, sizeof(temp) );
	const time_t ttt = time(NULL);  // 아이폰에선 이 함수가 안되네
	t = localtime( &ttt );
	
	stTime->wYear = t->tm_year;
	stTime->wMonth = t->tm_mon;
	stTime->wDayOfWeek = t->tm_wday;
	stTime->wDay = t->tm_mday;
	stTime->wHour = t->tm_hour;
	stTime->wMinute = t->tm_min;
	stTime->wSecond = t->tm_sec;
	stTime->wMilliseconds = 0;
}
// not win32
#else
// win32
namespace XE
{
	unsigned long long GetFreqTime( void ) 
	{
		LARGE_INTEGER llCnt;
		QueryPerformanceCounter( &llCnt );
		auto sec = (double)llCnt.QuadPart / (double)CTimer::GetFreqPerSec();
		auto llTime = (XUINT64)( sec * 1000000);
		return llTime;
//		return llCnt.QuadPart;
	}

} // XE
// win32
#endif 


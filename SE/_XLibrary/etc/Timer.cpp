#include "stdafx.h"
#include "Timer.h"

//double CTimer::s_dTimer = 0;			// 실제시간이 아닌 pause가능한 시간으로 쓰기위해 필요.
DWORD CTimer::s_dTimer = 0;			// 실제시간이 아닌 pause가능한 시간으로 쓰기위해 필요.
//BOOL CTimer::s_bRealTimer = 1;			// 실제시간모드가 디폴트

#pragma comment( lib, "winmm" )		// winmm.lib 추가
//
//
//
void	CTimer::Set( float fWaitTime, BOOL bRealTimer )
{
//	XBREAK( fWaitTime == 0 );
	m_bActive = TRUE;
	m_bRealTimer = bRealTimer;
	m_dwWaitTime = (DWORD)(fWaitTime * TICKS_PER_SEC);
	DWORD dwCurrTime = GetTime();
	m_dwPrevTime = dwCurrTime;
	m_dwStartTime = dwCurrTime;
}

void	CTimer::Reset()
{
	m_bActive = TRUE;
	DWORD dwCurrTime = GetTime();
	m_dwPrevTime = dwCurrTime;
	m_dwStartTime = dwCurrTime;
}

// 지나간 시간
DWORD CTimer::GetPassTime()
{
	DWORD dwCurrTime = GetTime();
	return dwCurrTime - m_dwStartTime;
}
//DWORD	dwTemp;

BOOL	CTimer::IsOver( float fOverTime )
{
	if( IsOff() )	return FALSE;
	DWORD dwCurrTime = GetTime();
	if( dwCurrTime - m_dwStartTime >= (DWORD)(fOverTime * TICKS_PER_SEC) )
		return TRUE;
	return FALSE;
}
BOOL	CTimer::IsOver( void  )
{
	if( IsOff() )	return FALSE;
	DWORD dwCurrTime = GetTime();
	if( dwCurrTime - m_dwStartTime >= m_dwWaitTime )
		return TRUE;
	
	return FALSE;
}
// dwEveryTime마다 TRUE를 리턴
// Reset()한번 해주는걸로 준비는 끝난다.
BOOL	CTimer::IsEvery( float fEveryTime )
{
	if( IsOn() == FALSE )		return FALSE;
	DWORD dwCurrTime = GetTime();
	if( dwCurrTime - m_dwPrevTime >= (DWORD)(fEveryTime * TICKS_PER_SEC) )
	{
		m_dwPrevTime = dwCurrTime;
		return TRUE;
	}
	return FALSE;
}

#ifdef _VER_IPHONE
#include <sys/time.h>
#include <time.h>

DWORD timeGetTime()
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
void GetLocalTime( SYSTEMTIME *time )
{
	struct tm temp;
	struct tm *t = &temp;
	memset( &temp, 0, sizeof(temp) );
//	const time_t ttt = time(NULL);  // 아이폰에선 이 함수가 안되네
//	t = localtime( &ttt );
	
	time->wYear = t->tm_year;
	time->wMonth = t->tm_mon;
	time->wDayOfWeek = t->tm_wday;
	time->wDay = t->tm_mday;
	time->wHour = t->tm_hour;
	time->wMinute = t->tm_min;
	time->wSecond = t->tm_sec;
	time->wMilliseconds = 0;
}
#endif // verIphone

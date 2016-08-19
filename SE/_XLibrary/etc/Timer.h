#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef _WIN32
#include "mmsystem.h"
#endif
#include "debug.h"

#define	TICKS_PER_SEC		1000.0f
#define	XSEC(S)		((DWORD)(TICKS_PER_SEC * (float)S))

#ifdef UNDER_CE
#define timeGetTime	GetTickCount
#endif

#ifdef _VER_IPHONE
typedef struct _SYSTEMTIME {
	WORD wYear;     
	WORD wMonth;     
	WORD wDayOfWeek;    
	WORD wDay;     
	WORD wHour;     
	WORD wMinute;     
	WORD wSecond;     
	WORD wMilliseconds; 
} SYSTEMTIME; 
DWORD timeGetTime();
//void QueryPerformanceCounter( LARGE_INTEGER *pllTimer );		// for MAC
//void QueryPerformanceFrequency( LARGE_INTEGER *pllFreq );	// for MAC
void GetLocalTime( SYSTEMTIME *time );
#define GetTickCount timeGetTime
#endif 

class CTimer
{
public:

//	static double s_dTimer;			// 실제시간이 아닌 pause가능한 시간으로 쓰기위해 필요
	static DWORD s_dTimer;
	static void Initialize() { s_dTimer = 1; }
	static void UpdateTimer( float dt ) { 
		s_dTimer += (DWORD)(((double)TICKS_PER_SEC/60.0) * (double)dt); 
		XBREAKF( s_dTimer < 0, "심각한오류: 타이머가 오버플로우 되었습니다. s_dTimer를 unsigned로 바꾸거나 방법을 찾으십시요" );
	}
//	static DWORD	GetTime() { 	return (DWORD)s_dTimer;	}
private:
	BOOL m_bRealTimer;		// 실제시간 타이머인가 pause가능한 타이머인가
//	DWORD	m_dwCurrTime;
	DWORD	m_dwPrevTime;
	DWORD	m_dwStartTime;
	DWORD	m_dwWaitTime;
	BOOL m_bActive;
public:

	CTimer() {
		m_dwWaitTime = 0;
		m_dwPrevTime = 0;
		m_dwStartTime = 0;
//		m_dwCurrTime = 0;
		m_bActive = FALSE;
		m_bRealTimer = FALSE;
	}
	CTimer( float sec ) {
		m_dwWaitTime = 0;
		m_dwPrevTime = 0;
		m_dwStartTime = 0;
//		m_dwCurrTime = 0;
		m_bActive = FALSE;
		m_bRealTimer = FALSE;
		Set( sec );
	}
	~CTimer() {}

	DWORD	GetTime() { 
		if( m_bRealTimer )
			return timeGetTime(); 
		return (DWORD)s_dTimer;
	}
//	void SetRealTimer( BOOL bRealTimer ) { m_bRealTimer = bRealTimer; }
	BOOL		IsOn() { return m_bActive; }
	BOOL		IsOff() { return !m_bActive; }
	void		Off() { m_bActive = FALSE; }
	void		On() { 
		m_bActive = TRUE; 
		m_dwWaitTime = 0;
		DWORD dwCurrTime = GetTime();
		m_dwPrevTime = dwCurrTime;
		m_dwStartTime = dwCurrTime;
	}
	DWORD GetWaitTime() { return m_dwWaitTime; }
	DWORD GetStartTime() { return m_dwStartTime; }
	void _SetWaitTime( DWORD waitTime ) { m_dwWaitTime = waitTime; }
	void _SetStartTime( DWORD startTime ) { m_dwStartTime = startTime; }
	void	Set( float fWaitTime, BOOL bRealTimer=FALSE );
	void	Reset();
	DWORD GetPassTime();
	DWORD GetRemainTime()	{ return m_dwWaitTime - GetPassTime(); }	// 남은 시간
	BOOL		IsOver();
	BOOL		IsOver( float fOverTime );
	BOOL		IsEvery( float fEveryTime );
	BOOL		Waiting() { return !(IsOver()); }
	float GetSlerp() {		// 세팅된시간중에 몇퍼센트나 흘러갔는지..0~1의 값으로 돌려준다. WaitTime이 지정되어있어야 한다
		float slerp = (float)GetPassTime() / GetWaitTime();
		if( slerp > 1.0f )		slerp = 1.0f;
		return slerp;
	}		
	// lua
	BOOL LuaIsOver() { 
		BOOL b = IsOver();
		if( b )
			Off();		// 루아버전은 코딩의 편의상 자동으로 꺼준다
		return b;
	}
};



#endif // __TIMER_H__

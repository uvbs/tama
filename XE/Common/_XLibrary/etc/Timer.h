#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef _WIN32
#include <mmsystem.h>
#endif
#include "debug.h"

#ifdef UNDER_CE
#define timeGetTime	GetTickCount
#endif

class XArchive;
//#ifdef _VER_IOS
#ifndef WIN32
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
DWORD timeGetTime( void );
//void QueryPerformanceCounter( LARGE_INTEGER *pllTimer );		// for MAC
//void QueryPerformanceFrequency( LARGE_INTEGER *pllFreq );	// for MAC
void GetLocalTime( SYSTEMTIME *time );
#define GetTickCount timeGetTime
#endif 

namespace XE {
	unsigned long long GetFreqTime( void ) ;
};

/**
 @brief 범용 시간체크 타이머
 x초를 기다렸다가 뭔가를 해야할때 사용한다.
 실제 타이머가 아니고 process(dt)기반이기때문에 dt가 0이되면 시간도 함께 멈춘다. 따라서 pause상태가 필요한 캐릭터 애니메이션등에 주로 쓰인다.
 process(dt)가 돌지 않을땐(홈버튼으로 나갔을때 같은) 시간도 가지 않으니 서버와 연동해야 하는 타이머가 필요할땐 쓰지 않는게 좋다.
*/
class CTimer
{
public:
	static DWORD s_dTimer;
	static void Initialize( void ) { 
		s_dTimer = 1; 
#ifdef WIN32
		// 타이머 주파수를 구해둔다.
		LARGE_INTEGER llFreq;
		QueryPerformanceFrequency( &llFreq );
		s_llFreqPerSec = llFreq.QuadPart;
#else
		s_llFreqPerSec = 1000000;
#endif
	}
	static void UpdateTimer( float dt ) { 
		s_dTimer += (DWORD)(((double)TICKS_PER_SEC/60.0) * (double)dt); 
//		XBREAKF( s_dTimer < 0, "심각한오류: 타이머가 오버플로우 되었습니다. s_dTimer를 unsigned로 바꾸거나 방법을 찾으십시요" );
	}
	static unsigned long long GetFreqPerSec( void ) {
		return s_llFreqPerSec;
	}
	static DWORD sGetTime() {
		return (DWORD)s_dTimer;
	}
//	static DWORD	GetTime( void ) { 	return (DWORD)s_dTimer;	}
	static BOOL sIsHourIn( int h24Start, int h24End, int h24Curr );
private:
	static unsigned long long s_llFreqPerSec;	// 초당 틱수
	BOOL m_bRealTimer;		// 실제시간 타이머인가 pause가능한 타이머인가
//	DWORD	m_dwCurrTime;
//	DWORD	m_dwPrevTime;
	DWORD	m_dwStartTime;
	DWORD	m_dwWaitTime;
	BOOL m_bActive;
	BOOL m_bOverSlerp;		// GetSlerp()시 1.0을 넘어갈수 있도록 하는 옵션
	DWORD m_dwPassTime;	// 이미 일정시간이 지나간상태인것을 구현하려면 이것을 사용
	void Init() {
		m_dwWaitTime = 0;
//		m_dwPrevTime = 0;
		m_dwStartTime = 0;
//		m_dwCurrTime = 0;
		m_bActive = FALSE;
		m_bRealTimer = FALSE;
		m_bOverSlerp = FALSE;
		m_dwPassTime = 0;
	}
public:

	CTimer() { Init(); 	}
	CTimer( float sec ) {
		Init();
		Set( sec );
	}
	~CTimer() {}

	GET_SET_ACCESSOR( BOOL, bOverSlerp );
	DWORD GetTime( void ) const { 
		if( m_bRealTimer )
			return timeGetTime(); 
		return (DWORD)s_dTimer;
	}
//	void SetRealTimer( BOOL bRealTimer ) { m_bRealTimer = bRealTimer; }
	BOOL IsOn( void ) const { return m_bActive; }
	BOOL IsOff( void ) const { return !m_bActive; }
	void Off( void ) { 
		Init();
	}
	void On( void ) { 
		m_bActive = TRUE; 
		m_dwWaitTime = 0;
		DWORD dwCurrTime = GetTime();
//		m_dwPrevTime = dwCurrTime;
		m_dwStartTime = dwCurrTime;
		m_dwPassTime = 0;
	}
	DWORD GetWaitTime( void ) const { return m_dwWaitTime; }
	DWORD GetStartTime( void ) const { return m_dwStartTime; }
	float GetWaitSec( void ) const { return (float)GetWaitTime() / TICKS_PER_SEC; }
	void _SetWaitTime( DWORD msWaitTime ) { m_dwWaitTime = msWaitTime; }
	void _SetStartTime( DWORD msStartTime ) { m_dwStartTime = msStartTime; }
	void Set( float secWaitTime, BOOL bRealTimer=FALSE );
	inline void Set( int secWaitTime, BOOL bRealTimer = FALSE ) {
		Set( (float)secWaitTime, bRealTimer );
	}
	void Reset( void );
	// 지나간시간을 1/1000초로 돌려준다.
	DWORD GetPassTime( void ) const;
	void SetPassSec( float sec ) ;
	// 지나간시간을 초로 환산하여 돌려준다.
	float GetPassSec( void ) const {
		return (float)GetPassTime() / TICKS_PER_SEC;
	}
	// 남은 시간(1/1000초)
	DWORD GetRemainTime( void )	const { 
		if( m_bActive == FALSE )
			return 0;
		return m_dwWaitTime - GetPassTime(); 
	}	
	// 남은시간(초)
	float GetRemainSec( void ) const {
		if( m_bActive == FALSE )
			return 0;
		return GetRemainTime() / TICKS_PER_SEC;
	}
	BOOL IsOver( void ) const;
	BOOL IsOver( float secOverTime ) const;
//	BOOL IsEvery( float fEveryTime );
	BOOL Waiting( void ) const { return !(IsOver()); }
	float GetSlerp( void ) const {		// 세팅된시간중에 몇퍼센트나 흘러갔는지..0~1의 값으로 돌려준다. WaitTime이 지정되어있어야 한다
		if( IsOff() )
			return 0;
		float slerp = (float)GetPassTime() / GetWaitTime();
		if( slerp > 1.0f && m_bOverSlerp == FALSE )
			slerp = 1.0f;
		return slerp;
	}		
	void SetContinue( DWORD msTimeWait, DWORD msTimePass, BOOL bRealTimer );
	// lua
	BOOL LuaIsOver( void ) { 
		BOOL b = IsOver();
		if( b )
			Off();		// 루아버전은 코딩의 편의상 자동으로 꺼준다
		return b;
	}
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar );
	void Update( float dt ) {}		// XTimer2와의 호환성을 유지하기 위해 만든 빈함수
};



#endif // __TIMER_H__

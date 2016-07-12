#pragma once
#include "debug.h"

class XArchive;
/**
 실제시간이 아닌 제어 가능한 타이머 객체
 이 타이머는 원할때 멈췄다가 다시 진행 할 수 있다.
 또한 시간을 빠르게 가거나 느리게 가게 할 수도 있다.
 XTimer2객체를 사용하려면 매프레임 Update(dt)를 갱신해야 한다.
 process(dt)가 돌지 않을땐(홈버튼으로 나갔을때 같은) 시간도 가지 않으니 서버와 연동해야 하는 타이머가 필요할땐 쓰지 않는게 좋다.
*/

//typedef time_t xSec;
class XTimer2
{
public:
	enum { xVER_TIMER2 = 1 };
	static xSec sGetTime( void );
	/**
	 주어진 sec(초)를 시/분/초로 분해해 준다.
	*/
	static void sGetHourMinSec( int sec, int *pHourOut, int *pMinOut=NULL, int *pSecOut=NULL ) {
		*pHourOut = sec / 60 / 60;
		if( pMinOut )
			*pMinOut = (sec / 60) % 60;
		if( pSecOut )
			*pSecOut = sec % 60;
	}
private:
	double m_secTimer;
	float m_secWaitTime;
	float m_Speed;		// 타이머스피드 배수. 2면 시간이 두배로 빨리 간다는것.
	BOOL m_bActive;
	void Init() {
		m_secTimer = 0;
		m_secWaitTime = 0;
		m_Speed = 1.0f;
		m_bActive = FALSE;
	}
public:
	XTimer2() { Init();	}
	XTimer2( float sec ) {
		Init();
		Set( sec );
	}
	virtual ~XTimer2() {}
	//
	void _Set( double dbsecTimer, float secWaitTime, float speedMultiply ) {
		m_secTimer = dbsecTimer;
		m_secWaitTime = secWaitTime;
		m_Speed = speedMultiply;
		m_bActive = TRUE;
	}
	BOOL IsOn( void ) const { return m_bActive; }
	BOOL IsOff( void ) const { return !m_bActive; }
	void Off( void ) { 
		m_bActive = FALSE; 
		Init();
	}
	void On( void ) { 
		m_bActive = TRUE; 
	}
	float GetsecWaitTime( void ) const { return m_secWaitTime; }
	DWORD GettickWaitTime( void ) const { return (DWORD)(m_secWaitTime * TICKS_PER_SEC); }
	double _GetTimer( void ) const { return m_secTimer; }
	void	Set( float secWaitTime );
	void	Reset( void );
	float	GetsecPassTime( void ) const;
	double GetsecPassTime64( void ) const;
	// 이미 시간을 sec초만큼 지나간 상태로 만든다.
private:
	// 웬만하면 AddTime을 쓸것.
	void SetPassTime( float sec ) {
		m_secTimer = (double)sec;
	}
	void SetPassTime( xSec sec ) {
		m_secTimer = (double)sec;
	}
	void SetPassTime64( XINT64 sec ) {
		m_secTimer = (double)sec;
	}
public:
	// 타이머가 흐르도록 한다. 제한시간이 없을때 사용한다.
	void Start( void ) {
		m_bActive = TRUE;
		m_secTimer = 0;
		m_secWaitTime = 0;
	}
	// 시간을 잠시 멈춤
	void Pause( void ) {
		m_bActive = FALSE;
	}
	// Pause된 시간을 다시 흐르게 함.
	void Resume( void ) {
		m_bActive = TRUE;
	}
	/**
	 @brief 기다려야 하는 시간을 늘인다.
	*/
	float AddWaitSec( float sec ) {
		return m_secWaitTime += sec;
	}
	// 남은 시간
	float GetsecRemainTime( void ) const { 
		if( m_bActive )
			return m_secWaitTime - (float)m_secTimer; 
		return 0;
	}	
	BOOL IsOver( void ) const;
	BOOL IsOver( float secWait ) const;
	BOOL IsAddOver( double secAdd ) const;
	float GetSlerp( void ) const {		// 세팅된시간중에 몇퍼센트나 흘러갔는지..0~1의 값으로 돌려준다. WaitTime이 지정되어있어야 한다
		float slerp = GetsecPassTime() / GetsecWaitTime();
//		if( slerp > 1.0f )		slerp = 1.0f;	// 1이상으로 넘어가는걸 써먹을데가 있을거 같아 주석처리함.
		return slerp;
	}		
	GET_ACCESSOR( float, Speed );
	void SetSpeed( float scale ) {
		m_Speed = scale;
	}
	void AddTime( float sec ) {
		if( IsOff() )
			return;
		m_secTimer += (double)sec;
	}
	void AddTime( xSec sec ) {
		if( IsOff() )
			return;
		m_secTimer += (double)sec;
	}
	void AddTime( int sec ) {
		if( IsOff() )
			return;
		m_secTimer += (double)sec;
	}
	void Update( float dt );
	void Serialize( XArchive& ar ) const;
	BOOL DeSerialize( XArchive& ar, int _ver );
};


/****************************************************************
* @brief 실제시간 기반 타이머
* @author xuzhu
* @date	2015/03/30 17:20
* 서버와 연동해서ㅑ 실제시간기반으로 일정 시간을 기다려서 처리해야 하는류의 작업에 사용한다.
*****************************************************************/
class XTimerTiny
{
	enum { xVER_TIMER_TINY = 2 };
public:
// 	int sSerializeGlobalTimer( XArchive& ar );
// 	int sDeSerializeGlobalTimer( XArchive& ar );
private:
//	static xSec s_secSaved;
	xSec m_secStart = 0;
	int m_secWait = 0;		// 기다려야 하는시간(있다면)
	int m_cntFail = 0;		// 타임오버 확인 실패카운드
	void Init() {}
	void Destroy() {}
public:
	XTimerTiny() { Init(); }
	virtual ~XTimerTiny() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( int, cntFail );
	GET_ACCESSOR_CONST( xSec, secStart );
	GET_ACCESSOR_CONST( int, secWait );
	void Set( int secWait ) {
		DoStart();
		m_secWait = secWait;
		XBREAK( secWait == 0 );
	}
	int AddCntFail( int add = 1 ) {
		return m_cntFail += add;
	}
	void DoStart() {
		m_secStart = XTimer2::sGetTime();
		m_cntFail = 0;
		m_secWait = 0;
	}
	void DoStart( xSec secStart ) {
		m_secStart = secStart;
		m_cntFail = 0;
		m_secWait = 0;
	}
	void DoStop() {
		ClearTimer();
	}
	void On() {
		DoStart();
	}
	void Off() {
		DoStop();
	}
	void ResetTimer() {
		DoStart();
	}
	inline void ClearTimer() {
		m_secStart = 0;
		m_cntFail = 0;
		m_secWait = 0;
	}
	void RetryTimer( xSec secAfter ) {
		// 10초후에 다시 시도하게 하기 위해 타이머를 리셋하고 이미 50초가 지난것처럼 만듬.
		m_secStart = XTimer2::sGetTime() - (60 - secAfter);
	}
	// 현재 타이머가 시작되었는가.
	bool IsOn() const {
		return m_secStart != 0;
	}
	bool IsOff() const {
		return m_secStart == 0;
	}
	bool IsOver( xSec secMax ) const;
	inline bool IsOver() const {
		XBREAK( m_secWait == 0 );			// 0인걸쓰려면 DoStart()/IsOver( secMax )를 쓸것.
		return IsOver( m_secWait );
	}
	// 타이머가 시작한후로 지나간 시간을 얻는다.
	xSec GetsecPass() const;
	int GetsecRemain() const;
	// 시간을 addSec초만큼 더 가게 만든다.
	void AddSec( int addSec ) {
		m_secStart -= (xSec)addSec;		// 시작시간을 뒤로 당겨주면 시간이 더흘러간 효과
	}
	// 시간을 subSec초만큼 뒤로 돌린다.
	void SubSec( int subSec ) {
		m_secStart += (xSec)subSec;		// 시작시간을 앞으로 당겨주면 시간이 덜흘러간 효과
	}
	/**
	 @brief 저장되어있던 시간을 꺼내에 오프라인시간까지 추가하여 타이머를 복구시킨다.
	 @param secSaved DB에 저장했던 당시 시간
	 @param secPass 저장할때 시점까지 지나갔던 시간
	*/
	void RestoreTimer( xSec secSaved, xSec secPass );
	int Serialize( XArchive& ar ) const;
	int DeSerialize( XArchive& ar, int _ver = 0 );
}; // class XTimerTiny

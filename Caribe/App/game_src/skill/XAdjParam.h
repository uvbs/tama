#pragma once
#include "SkillDef.h"

////////////////////////////////////////////////////////////////
XE_NAMESPACE_START( XSKILL )

/**
 @brief 다양한 파라메터를 처리하기위해 ADJ_PARAM방식은 더이상 한계
 보정치를 구할때 모든 버프/아이템객체를 실시간으로 순회하며 증가치를 더해서 최종적으로 기본치와 곱하거나 더한다.
 건네줄땐 #,%,!방식의 보정치를 모두 건네야 한다.
*/

struct ADJ_PARAM
{
	// 최적화. valxxxx변수들 union방식으로 하는거 고려
	float valPercent;		// 퍼센트 타입으로 기술된 값의 누적
	float valImm;			// 즉치로 기술된 값의 누적
	float valFixedImm;	// 고정치값
	DWORD dwParam[4];		// 다용도 파라메터
	void Init() {
		valPercent = 0;					// +10%를 원할때 기존엔 %1.1을 했는데 %10으로 바꿨다 그래서 초기치도 0이다
		valImm = 0;
		valFixedImm = -1;				// 고정치값은 0을 쓸수도 있으므로 -1이 디폴트다
		XCLEAR_ARRAY( dwParam );
	}
	ADJ_PARAM() { Init(); }
	bool IsClear() const {		// 값이 초기화 되어있는 상태인지?
		return( valPercent == 0 && valImm == 0 && valFixedImm == -1 );
	}
// 	void ClearDebug() {
// 		*((DWORD*)(&valPercent)) = 0xfefefefe;
// 	}
	float GetValPercent() const {	return valPercent;	}
	float GetValImm() const {	return valImm; }
	float GetValFixedImm() const {return valFixedImm;	}
};

/**
 @brief 
*/
class XAdjParam
{
	struct xState {
		float resist;	// 상태 저항력(0~100)
		bool bActive;		// 상태 ON/OFF
		xState() {
			resist = 0.f;
			bActive = false;
		}
	};
	struct xArys {
		XVector<ADJ_PARAM> m_adjParam;		// 파라메터 보정치
		XVector<xState> m_States;			// 상태(축복/저주/기절 등)
	};
	XVector<xArys> m_Arys;					// 더블버퍼. Write류는 백버퍼에 Read류는 Curr버퍼로.
	xArys* m_pCurrAry = nullptr;		// 더블버퍼중 현재 버퍼의 포인터(Read)
	xArys* m_pBackAry = nullptr;		// 더블버퍼중 후면 버퍼의 포인터(Write)
// 	XVector<ADJ_PARAM> m_adjParam;		// 파라메터 보정치
// 	XVector<xState> m_States;			// 상태(축복/저주/기절 등)
	void Init() {}
	void Destroy() {}
public:
	XAdjParam( int maxParam, int maxState ) 
	: m_Arys( 2 ) { 
		for( auto& buff : m_Arys ) {
			buff.m_adjParam.resize( maxParam );
			buff.m_States.resize( maxState );
		}
		m_pCurrAry = &m_Arys[0];
		m_pBackAry = &m_Arys[1];

	}
	virtual ~XAdjParam() { Destroy(); }
	inline void Swap() {
		std::swap( m_pCurrAry, m_pBackAry );
	}
	/**
	 @brief 파라메터 어레이의 크기를 구한다
	*/
	inline int GetMaxParam() const {
		return m_pBackAry->m_adjParam.Size();
	}
	inline int GetMaxState() const {
		return m_pBackAry->m_States.Size();
	}
	/**
	 @brief 파라메터가 보정파라메터인지 비보정파라메터인지 구분한다.
	 @return 양수값은 보정파라메터 음수값은 비보정파라메터 0은 invalid파라메터다.
	*/
	inline int IsAdjParam( int parameter ) const {
		if ( parameter > 0 )
			return 1;
		else if( parameter < 0 )
			return -1;
		return 0;
	}
	/**
	 @brief parameter가 비보정 파라메터인가
	*/
	inline BOOL IsNotAdjParam( int parameter ) const {
		return IsAdjParam( parameter ) < 0;
	}
	/**
	 @brief 보정치를 더한다.
	*/
protected:
	void AddAdjParam( int adjParam, xtValType valType, float adj );
public:
	virtual void AddAdjParamMsg( int adjParam, xtValType valType, float adj ) = 0;
	/**
	 @brief 현재 보정치를 돌려준다.(Get은 후면버퍼를 사용하고 Set이나 Add는 전면버퍼에 넣는다.)
	*/
	inline const ADJ_PARAM* GetAdjParam( int adjParam ) const {
		return &(m_pCurrAry->m_adjParam[ adjParam ]);
	}
	_tstring GetstrAdjParam( int adjParam ) const;
	/**
	 @brief val값을 Adj파라메터로 보정해서 돌려준다.
	*/
	float CalcAdjParam( float val, int adjParam, float addAdjRatio=0.f, float addAdjVal=0.f ) const;
	float GetAdjValue( float val, int adjParam );
	float GetAdjValue( int adjParam );
	// 매 게임루프마다 객체의 process루프를 돌기전에 clearAdjParam을 하는 루프를 먼저 돌리도록 한다.
	void ClearAdjParam();
	// 매 게임루프마다 객체의 process를 돌기전에 adjParam값을 모두 초기화 시켜야한다.
	// 하지만 프로그래머의 실수로 빼먹을 수 도 있으니 체크루틴을 삽입했다.
	// 만약 스킬프로세스를 시작하려는데 adjParam값이 클리어가 안되어있다면 에러를 assert가 걸리도록 했다.
//	void ClearDebugAdjParam();
	bool IsHaveAdjParam( int adjParam ) const {
		return !(m_pCurrAry->m_adjParam[ adjParam ].IsClear());
	}
	// adjparam을 클리어 했는지 검사. 디버그모드용
// 	BOOL IsClearAdjParam() {
// 		if( *((DWORD*)(&m_adjParam[0].valPercent)) == 0xfefefefe )
// 			return FALSE;
// 		return TRUE;
// 	}
	/**
	 @brief idxState상태에 걸렸는가
	*/
	inline BOOL IsState( int idxState ) const {
		return m_pCurrAry->m_States[idxState].bActive;
	}
protected:
	virtual void SetStateMsg( int idxState, bool bFlag ) {}
	inline void SetState( int idxState, bool bFlag ) {
		m_pBackAry->m_States[idxState].bActive = bFlag;
	}
public:
	inline float GetResistState( int idxState ) const {
		return m_pCurrAry->m_States[idxState].resist;
	}
};
XE_NAMESPACE_END


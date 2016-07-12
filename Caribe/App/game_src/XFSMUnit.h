#pragma once
#include "XEBaseFSM.h"
//#include "XBaseUnit.h"

class XBaseUnit;
class XEWorld;
class XFSMChase;
///////////////////////////////////////////////////////////////////
class XFSMBase : public XEBaseFSM
{
public:
	/**
	 @brief FSM 아이디
	*/
	enum xtFSM {
		xFSM_NONE,
		xFSM_IDLE,		///< 대기모드
		xFSM_CHASE,		///< 추적모드
		xFSM_NORMAL_ATTACK,		///< 일반공격중 모드
		xFSM_USE_SKILL,			///< 스킬사용중 모드
		xFSM_STUN,				///< 스턴 및 이동/공격불가 상태
		xFSM_DIE,				///< 죽는중 모드
	};
	static LPCTSTR sGetStrFSM( ID idFsm ) {
		switch( idFsm ) {
		case xFSM_NONE:	return _T("xFSM_NONE");
		case xFSM_IDLE:	return _T( "xFSM_IDLE" );
		case xFSM_CHASE:	return _T( "xFSM_CHASE" );
		case xFSM_NORMAL_ATTACK:	return _T( "xFSM_NORMAL_ATTACK" );
		case xFSM_USE_SKILL:	return _T( "xFSM_USE_SKILL" );
		case xFSM_DIE:	return _T( "xFSM_DIE" );
		default: return _T("FSM_ERROR !!!!!!!!!!!!!!!!!!!!!!");
		}
	}
private:
	//
	CTimer m_timerLocalMove;		///< 분대내에서의 로컬위치로 뛰어갈때 사용하는 보간용 타이머
	void Init() {
		m_pUnit = nullptr;
	}
	void Destroy();
protected:
	/**
	 @brief FSM은 유닛객체 외부에서 호출될일이 없으므로 레퍼런스객체로 하지 않았다.
	*/
	XBaseUnit *m_pUnit;
public:
	XFSMBase( ID idFSM, XBaseUnit *pUnit );
	virtual ~XFSMBase() { Destroy(); }
	//
	GET_ACCESSOR( CTimer&, timerLocalMove );
	virtual void Release() override {}
	//
	XEWorld* GetWorld( void );
	/// 타겟이 this보다 오른쪽에 있는가
	BOOL IsTargetRight( const XE::VEC3& vwDst );
	XFSMChase* DoChase( const UnitPtr& spTarget, XFSMBase::xtFSM fsmNext );
	XFSMChase* DoChase( const XE::VEC3& vwTarget );
	virtual void OnDamage( void ) {}

};

///////////////////////////////////////////////////////////////////
/**
 @brief 대기모드
*/
class XFSMIdle : public XFSMBase
{
private:

	//
	void _Init() {}
	void Destroy();
public:
	XFSMIdle( XBaseUnit *pUnit );
	virtual ~XFSMIdle() { Destroy(); }

	void Init( void );
	void Release() override;
	int FrameMove( float dt );
};

/**
 @brief 추적모드
*/
class XFSMChase : public XFSMBase
{
//	XE::VEC3 m_vTarget;			///< 목표가 좌표형태일 경우
	XE::VEC3 m_vwDstTarget;		///< 현재 이동목표 지점
	XE::VEC3 m_vwLocalMoveStart;	///< 분대 내 자리까지 이동할때 시작위치
	CTimer m_timerDelay;		///< 달려가기전 오브젝트마다 랜덤값의 대기시간이 있다. 무리가 뛰어갈때 좀더 자연스럽게 보이기위해.
	int m_modeMoveToTarget;		///< 0:부대이동모드 1:바인드좌표이동모드(근접) 2:타겟추적모드(원거리)
	float m_speedAccel;			///< 타겟근처에서 대쉬할때 속도 배수
	BOOL m_bDash;				///< 타겟까지 대쉬로 접근하는 모드
	bool m_bArrive = false;				///< 타겟앞에 서면 true. 추적때문에 이동하면 false.
	void _Init() {
		m_modeMoveToTarget = 0;
		m_speedAccel = 1.f;
		m_bDash = FALSE;
	}
	void Destroy();
public:
	XFSMChase( XBaseUnit *pUnit );
	virtual ~XFSMChase() { Destroy(); }
	//
	void Init( void );
	void Init( BOOL bDash );
	void Uninit( void );
	int FrameMove( float dt );
	int ProcessTraceTarget( float dt );
	int ProcessTraceBind( float dt );
	virtual void AddDebugStr( _tstring& str ) {
		str += XE::Format(_T("m=%d\n"),m_modeMoveToTarget );
	}
	/// 새 타겟을 잡으면 발생된다. 전투시작 최초 달려갈때는 발생하지 않으니 주의.
	void OnFirstNewTarget() {
		m_bArrive = false;
	}
	void Release() override;
	/**
	 @brief 상태 초기화 함수
	*/
//	void Init( UnitPtr& spUnit );
};

/****************************************************************
* @brief 일반공격 상태
* @author xuzhu
* @date	2014/09/15 18:59
*****************************************************************/
class XFSMNormalAttack : public XFSMBase
{
	XE::VEC3 m_vTarget;			///< 목표가 좌표형태일 경우
//	CTimer m_timerAttack;	///< 공속타이머
//	XGAME::xtMelee m_typeCurrMeleeType;	///< 현재 어떤 공격중이었는가.
	SquadPtr m_spLastTargetSquad;		///< 마지막으로 공격하고 있던 타겟부대
	void _Init() {

	}
	void Destroy();
public:
	XFSMNormalAttack( XBaseUnit *pUnit );
	virtual ~XFSMNormalAttack() { Destroy(); }
	//
	void Init( void );
	void Uninit( void );
	int FrameMove( float dt );
	void Release() override;
	/**
	 @brief 상태 초기화 함수
	*/
	void Init( UnitPtr& spUnit );
	void DoAttackMotion( void );
	virtual void AddDebugStr( _tstring& str ) {
		str += XE::Format( _T( "NAttack\n" ) );
	}
};

/**
 @brief 이동불가 공격불가
*/
class XFSMStun : public XFSMBase
{
private:
	CTimer m_timerDelay;	// 쓰러진지 몇초가 지나면 사라지는가
	//
	void _Init() {}
	void Destroy();
public:
	XFSMStun( XBaseUnit *pUnit );
	virtual ~XFSMStun() { Destroy(); }

	void Init( void );
	void Init( float secStun );
	void Release() override;
	virtual void Uninit( void ) { }
	int FrameMove( float dt );
	virtual void OnDamage( void );
};


/**
 @brief 죽는상태
*/
class XFSMDie : public XFSMBase
{
private:
	CTimer m_timerDelay;	// 쓰러진지 몇초가 지나면 사라지는가
	CTimer m_timerAppear;	// 사라질때 타이머
	int m_State = 0;		// 0:죽은상태(시체) 1:사라지는중
	//
	void _Init() {}
	void Destroy();
public:
	XFSMDie( XBaseUnit *pUnit );
	virtual ~XFSMDie() { Destroy(); }

	void Init( void );
	void Release() override;
	virtual void Uninit( void ) 
	{
		XBREAK( GetidNextFSM() != 0 );;	// 죽은유닛한테 명령을 내려 죽음이 풀린상황.
	}
	int FrameMove( float dt );
};


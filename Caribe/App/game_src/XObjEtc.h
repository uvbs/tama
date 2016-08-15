/********************************************************************
	@date:	2014/09/23 16:01
	@file: 	D:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XObjEtc.h
	@author:	xuzhu
	
	@brief:	여러가지 잡 오브젝트들의 클래스
*********************************************************************/
#pragma once
#include "XFramework/Game/XEBaseWorldObj.h"
#include "skill/XSkillReceiver.h"
#include "skill/XSkillUser.h"
#include "XFramework/Game/XEComponents.h"
#include "XFont.h"
#include "XPool.h"
#include "XStruct.h"

class XWndBattleField;
class XDelegateObjBullet;
class XBaseUnit;
class XCompObjMove;
class XCompObjFont;
namespace XGAME {
struct xRES_NUM;
};


//////////////////////////////////////////////////////////////////////////
class XObjBullet : public XEBaseWorldObj
{
	ID m_idBullet;		// 발사체 아이디
	XE::VEC3 m_vOld;	// 이전 프레임에서의 위치
	void Init() {
		m_secLife = 0;
//		m_idArriveSfx = 0;
		m_idBullet = 0;
		m_idActArrive = 0;
		m_Damage = 0;
	}
	void Destroy();
protected:
	XE::VEC3 m_vDst;
	XE::VEC3 m_vSrc;
	CTimer m_timerLife;
	float m_secLife;			// 날아가는 시간
	XSPUnit m_spOwner;
	XSPUnit m_spTarget;
	float m_Damage;			///< 발사체가 도달해서 타겟에게 전해줄 데미지
	float m_meterRadius = 0.f;			///< damage가 범위공격이라면 그 범위. 0은 단일공격
	float m_ratioDamageSplash = 0.f;	///< 광역데미지의 데미지율(m_Damage에 따른)
//	ID m_idArriveSfx;		// 도착하면 생성될 오브젝트 아이디
	_tstring m_sprArrive;	// 도착하면 생성될 스프라이트
	ID m_idActArrive;
	XDelegateObjBullet *m_pDelegate;
//	float m_offsetZ;		///< 목표의 정중앙에서 랜덤하게 벗어난 값을 미리 갖는다.
	XE::VEC3 m_vOffset;		///< 타겟위치에 약간씩 변화를 주고싶을때 오프셋을 준다.
	_tstring m_strIdentifier;		///< OnArriveBullet등에서 사용하는 총알 고유 식별자.
//	XArrayLinearN<_tstring,4> m_aryInvokeSkill;		///< 발사체가 목표에 도달한 후 발동될 발동스킬. 발동스킬을 여러개가 한꺼번에 붙을수 있으므로 복수로 했다.
	XVector<_tstring> m_aryInvokeSkill;		//더이상 이방식으로 궁수특성을 구현하지 않음. 일반적인 발동에 사용하기 위해 남겨둠.
	bool m_bCritical = false;
	//
	GET_ACCESSOR_CONST( const CTimer&, timerLife );
	GET_ACCESSOR( XSPUnit&, spOwner );
	GET_ACCESSOR_CONST( const XE::VEC3&, vSrc );
	GET_ACCESSOR_CONST( float, Damage );
public:
	XObjBullet( ID idBullet,
							const XSPUnit& spOwner,
							const XSPUnit spTarget,
							const XE::VEC3& vwSrc,
							const XE::VEC3& vwDst,
							float damage,
							bool bCritical,
							LPCTSTR szSpr, ID idAct,
							float secFly = 0.3f );
	virtual ~XObjBullet() { Destroy(); }
	//
	virtual void Release() {
		m_spOwner.reset();
		m_spTarget.reset();
	}
	GET_ACCESSOR_CONST( ID, idBullet );
	GET_SET_ACCESSOR( XDelegateObjBullet*, pDelegate );
	GET_SET_ACCESSOR_CONST( const _tstring&, strIdentifier );
	GET_ACCESSOR_CONST( const XE::VEC3&, vDst );
	GET_SET_ACCESSOR_CONST( const XE::VEC3&, vOffset );
	GET_SET_ACCESSOR_CONST( float, meterRadius );
	GET_SET_ACCESSOR_CONST( float, ratioDamageSplash );
/*
	void SetArriveSfx( ID idObj ) {
		m_idArriveSfx = idObj;
	}
*/
	// 발사체가 목표에 도착하면 생성될 sfx
	void SetArriveSfx( LPCTSTR sprArrive, ID idAct ) {
		m_sprArrive = sprArrive;
		m_idActArrive = idAct;
	}
	void SetSecFly( float secFly ) {
		m_timerLife.Set( secFly );
	}
	float GetSecFly() const {
		return m_timerLife.GetWaitSec();
	}
	/**
	 @brief 광역데미지 속성일경우 그 정보. 
	 @param meterRadius 반지름(미터)
	 @param ratioDamage 스플래시데미지의 데미지율. m_Damage에 대한 비율임
	*/
	void SetSplash( float meterRadius, float ratioDamage ) {
		m_meterRadius = meterRadius;
		m_ratioDamageSplash = ratioDamage;
	}
	//
	virtual void FrameMove( float dt );
	virtual void OnArriveBullet( DWORD dwParam );
	virtual XE::VEC3 OnInterpolation( const XE::VEC3& vSrc, const XE::VEC3& vDst, float lerpTime );
	int AddInvokeSkill( const _tstring& strInvokeSkill );
};

class XDelegateObjBullet
{
	void Init() {}
	void Destroy() {}
public:
	XDelegateObjBullet() { Init(); }
	virtual ~XDelegateObjBullet() { Destroy(); }
	//
	virtual void OnArriveBullet( XObjBullet *pBullet,
															 XSPUnit spAttacker,
															 XSPUnit spTarget,
															 const XE::VEC3& vwDst,
															 float damage,
															 bool bCritical,
															 LPCTSTR sprArrive, ID idActArrive,
															 DWORD dwParam = 0 ) {}

};


/****************************************************************
* @brief 화살
* @author xuzhu
* @date	2014/09/23 16:01
*****************************************************************/
class XObjArrow : public XObjBullet
#ifdef _XMEM_POOL
				, public XMemPool<XObjArrow>
#endif
{
	BOOL m_bArrive;
	XSKILL::xtMoving m_MoveType = XSKILL::xMT_ARC;
	struct xCALLBACK {
		XBaseUnit *pOwner;
		std::function<void(XBaseUnit*, XObjArrow*)> funcCallback;
		xCALLBACK() {
			pOwner = nullptr;
		}
	};
	xCALLBACK m_Callback;
	void Init() {
		m_bArrive = FALSE;
//		m_psoStuck = NULL;
	}
	void Destroy();
public:
	XObjArrow( XEWndWorld *pWndWorld,
						 const XSPUnit& spOwner,
						 const XSPUnit& spTarget,
						 const XE::VEC3& vwSrc,
						 const XE::VEC3& vwDst,
						 float damage,
						 bool bCritical,
						 LPCTSTR szSpr, ID idAct,
						 float factorSpeed = 8.f );	// 숫자가 높을수록 빨라짐
	virtual ~XObjArrow() { Destroy(); }
	///< 
	GET_SET_ACCESSOR_CONST( XSKILL::xtMoving, MoveType );
	virtual void FrameMove( float dt );
	virtual void Draw( const XE::VEC2& vPos, float scale = 1.f, float alpha=1.f );
	XE::VEC3 OnInterpolation( const XE::VEC3& vSrc, const XE::VEC3& vDst, float lerpTime ) override;
	void OnArriveBullet( DWORD dwParam ) override;
	template<typename F>
	void RegisterCallback( XBaseUnit* pOwner, F func, float damage ) {
		m_Callback.pOwner = pOwner;
		m_Callback.funcCallback = std::bind( func, std::placeholders::_1, std::placeholders::_2, damage );
	}
	virtual void CallCallbackFunc() {
		if( m_Callback.funcCallback )
			m_Callback.funcCallback( m_Callback.pOwner, this );
	}
};


/****************************************************************
* @brief 멀티샷같은 스킬 발사체 객체
* @author xuzhu
* @date	2014/11/25 12:47
*****************************************************************/
class XSkillShootObj;
class XSkillShootObj : public XObjArrow
{
	struct xCALLBACK {
		XBaseUnit *pOwner;
		std::function<void( XBaseUnit*, XSkillShootObj*)> funcCallback;
		xCALLBACK() {
			pOwner = nullptr;
		}
	};
	xCALLBACK m_Callback;
	void Init() {}
	void Destroy();
public:
	XSkillShootObj( XEWndWorld *pWndWorld,
									const XSPUnit& spOwner,
									const XSPUnit& spTarget,
									const XE::VEC3& vwSrc,
									const XE::VEC3& vwDst,
									float damage,
									LPCTSTR szSpr, ID idAct,
									float factorSpeed = 8.f );
	virtual ~XSkillShootObj() { Destroy(); }
	//
	template<typename F, typename T1, typename T2, typename T3/*, typename T4, typename T5*/>
	void RegisterCallback( XBaseUnit* pOwner, F func, T1 p1, T2 p2, T3 p3/*, T4 p4, T5 p5*/ ) {
		m_Callback.pOwner = pOwner;
		m_Callback.funcCallback = std::bind( func,
																				 std::placeholders::_1,
																				 std::placeholders::_2,
																				 p1, p2, p3/*, p4, p5*/ );
	}
	virtual void CallCallbackFunc();
//	XE::VEC3 OnInterpolation( const XE::VEC3& vSrc, const XE::VEC3& vDst, float lerpTime ) override;
}; // class XSkillShootObj
/**
 @brief 바위
*/
class XObjRock : public XObjBullet
{
	BOOL m_bArrive;
	XSurface *m_psfcShadow;
	int m_maxElastic = 0;		// 탄성이 일어날 횟수.
	int m_cntElastic = 0;
	float m_AddDamage = 0.f;			// 탄성으로 새로 튀는 바위의 데미지
	float m_factorSpline = 1.f;		// 포물선궤적을 더 구부리고 싶다면 1이상을 준다.
	void Init() {
		m_bArrive = FALSE;
		m_psfcShadow = NULL;
	}
	void Destroy();
public:
	XObjRock( XEWndWorld *pWndWorld,
		const XSPUnit& spOwner,
		const XSPUnit spTarget,
		const XE::VEC3& vwSrc,
		const XE::VEC3& vwDst,
		float damage,
		bool bCritical,
		LPCTSTR szSpr, ID idAct );
	virtual ~XObjRock() { Destroy(); }
	//
	void SetElastic( float addDamage, int maxElastic, int cntElastic = 0 ) {
		if( (int)addDamage > 0 ) {
			m_maxElastic = maxElastic;
			m_cntElastic = cntElastic;
			m_AddDamage = addDamage;
		} else
			m_maxElastic = 0;
	}
	GET_ACCESSOR( int, cntElastic );
	GET_ACCESSOR( float, AddDamage );
	GET_SET_ACCESSOR( float, factorSpline );
	///< 
	virtual void FrameMove( float dt );
	virtual void Draw( const XE::VEC2& vPos, float scale = 1.f, float alpha=1.f );
	XE::VEC3 OnInterpolation( const XE::VEC3& vSrc, const XE::VEC3& vDst, float lerpTime ) override;
	void OnArriveBullet( DWORD dwParam ) override;
};

/****************************************************************
* @brief 사이클롭스용 레이저
* @author xuzhu
* @date	2014/10/06 14:24
*****************************************************************/
class XObjLaser : public XEBaseWorldObj
{
	XE::VEC3 m_vwStart;
	XE::VEC3 m_vwEnd;
	CTimer m_timerLife;
	XE::VEC3 m_vDelta;
	CTimer m_timerLaser;
	void Init() {}
	void Destroy();
public:
	XObjLaser( LPCTSTR szSpr, const XE::VEC3& vwStart, const XE::VEC3& vwEnd );
	virtual ~XObjLaser() { Destroy(); }
	//
	void FrameMove( float dt );
	void Draw( const XE::VEC2& vPos, float scale = 1.f, float alpha=1.f );
	virtual void Release();
};

/****************************************************************
* @brief 일정시간동안 반복되는 이펙트류 일반화
* @author xuzhu
* @date	2014/10/06 15:47
*****************************************************************/
class XObjLoop : public XEBaseWorldObj, public XSKILL::XSkillSfx
{
	CTimer m_timerLife;
	float m_secLife;
	XE::xtHorizDir m_Dir;
	XSPUnit m_spRefUnit;		// 따라다녀야할 오브젝트가 있다면
	BOOL m_bTraceRefObj;		///< 레퍼런스 객체를 따라다녀야 한다면 TRUE
	XE::VEC3 m_vAdjust;			///< this의 위치 보정치
	int m_State = 0;
	std::shared_ptr<XCompObjMove> m_spCompMove;		// 이동을 제어하는 컴포넌트
	void Init() {
		m_secLife = 0.f;	// 0은 한번만 플레이하고 중지. -1은 무한루프
		m_Dir = XE::HDIR_NONE;
		m_bTraceRefObj = FALSE;
	}
	void Destroy();
public:
	XObjLoop( const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct, float secLife=0.f );
	XObjLoop( int typeObj, const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct, float secLife=0.f );
	XObjLoop( int typeObj, const XSPUnit& spTrace, LPCTSTR szSpr, ID idAct, float secLife = 0.f );
	XObjLoop( int typeObj, const XSPUnit& spRefObj, const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct, float secLife = 0.f );
	virtual ~XObjLoop() { Destroy(); }
	///< 
	SET_ACCESSOR( XE::xtHorizDir, Dir );
	SET_ACCESSOR( const XE::VEC3&, vAdjust );
	GET_ACCESSOR( std::shared_ptr<XCompObjMove>, spCompMove );
	void SetvAdjust( float x, float y, float z ) {
		m_vAdjust.Set( x, y, z );
	}
	void SetBounce( float power, float dAngZ, float gravity = 1.f );
	inline void SetBounce( const XE::VEC2& vrPower, const XE::VEC2& vrdAngZ, float gravity = 1.f ) {
		SetBounce( xRandomF( vrPower ), xRandomF( vrdAngZ ), gravity );
	}
	//
	void Release() {
		m_spRefUnit.reset();
	}
	void FrameMove( float dt );
	void OnEventSprObj( XSprObj *pSprObj, XKeyEvent *pKey, float lx, float ly, ID idEvent, float fAngle, float fOverSec ) override;
	//
};

/****************************************************************
* @brief 버프를 가질수 있는 스킬sfx객체.
* 보통 바닥에 놓는 지속스킬에 사용한다.
* @author xuzhu
* @date	2014/11/20 19:30
*****************************************************************/
class XSkillSfxReceiver : public XEBaseWorldObj, public XSKILL::XSkillReceiver
{
	XECompCamp m_Camp;
	CTimer m_timerLife;
	void Init() {}
	void Destroy();
public:
	XSkillSfxReceiver( BIT bitCamp, const XE::VEC3& vwPos, float sec );
	~XSkillSfxReceiver() { Destroy(); }
	ID GetId() override {
		return GetsnObj();
	}
	void Release() override {}
	int OnApplyEffectNotAdjParam( XSKILL::XSkillUser *pCaster, XSKILL::XSkillDat* pSkillDat, const XSKILL::EFFECT *pEffect, float abilMin ) override { return 0; }
	const XECompCamp& GetCamp() const override {		///< this의 진영을 리턴
		return m_Camp;
	}
	void AddAdjParamMsg( int adjParam, XSKILL::xtValType valType, float adj ) override { XBREAK(1); }

	void FrameMove( float dt ) override;
}; // class XSkillSfxReceiver


/****************************************************************
* @brief 
* @author xuzhu
* @date	2014/11/27 11:30
*****************************************************************/
namespace XGAME {
	enum xtHit {
		xHT_NONE,
		xHT_HIT,		// 일반적중
		xHT_CRITICAL,	// 치명타
		xHT_EVADE,		// 회피
		xHT_MISS,		// 빗나감
		xHT_VORPAL,		// 즉사
		xHT_STUN,		// 기절
		xHT_IMMUNE,  // 면역
		xHT_STATE,	// 상태이상
		xHT_CUSTOM,	// 사용자 정의 문자열
	};
	enum xtbitHit {
// 		xBHT_NONE = 0,
		xBHT_HIT = 0x01,				// (초기화값)1:적중 0:빗맞음
		xBHT_CRITICAL = 0x02,		// 1:크리티컬 0:일반타격
		xBHT_BY_SKILL = 0x04,		// 1:스킬(혹은 특성)에 의한 타격
		xBHT_POISON = 0x08,			// 독속성 타격
		xBHT_EVADE = 0x10,			// 피격자측에서 회피
		xBHT_VORPAL = 0x20,			// 즉사
		xBHT_IMMUNE = 0x40,			// 피해면역
		xBHT_THORNS_DAMAGE = 0x80,		// 반사데미지
	};
};
class XObjDmgNum : public XEBaseWorldObj

{
public:
	static _tstring s_strFont;
private:
	int m_Number;
	_tstring m_strNumber;
	XGAME::xtHit m_typeHit = XGAME::xHT_HIT;
	XE::VEC3 m_vDelta;
	XBaseFontDat *m_pfdNumber;
//	CTimer m_timerLife;
	int m_State;
	XCOLOR m_Col = 0;
	std::shared_ptr<XCompObjMove> m_spCompMove;
// 	XCompObjBounce m_compBounce;
	//
	void Init() {
		m_Number = 0;
		m_pfdNumber = nullptr;
		m_State = 0;
	}
	void Destroy();
public:
	XObjDmgNum( float num
							, XGAME::xtHit typeHit, int paramHit
							, const XE::VEC3& vwPos, XCOLOR col = 0 );
	XObjDmgNum( LPCTSTR szStr
							, const XE::VEC3& vwPos, XCOLOR col = 0 );
	XObjDmgNum( const _tstring& str
							, const XE::VEC3& vwPos, XCOLOR col = 0 )
							: XObjDmgNum( str.c_str(), vwPos, col ) {}
	~XObjDmgNum() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( XCOLOR, Col );
//	GET_ACCESSOR_CONST( std::shared_ptr<XCompObjBounce>, spCompBounce );
	//
	void Release() {}
	void FrameMove( float dt );
	void Draw( const XE::VEC2& vPos, float scale/* =1.f */, float alpha/* =1.f */ );
private:
	void InitEffect();
	void SetBounce( float power, float dAngZ, float gravity );
	inline void SetBounce( const XE::VEC2& vrPower, const XE::VEC2& vrdAngZ, float gravity = 1.f ) {
		SetBounce( xRandomF( vrPower ), xRandomF( vrdAngZ ), gravity );
	}
}; // class XObjDamageNumber

/**
 @brief 
*/
class XObjYellSkill : public XEBaseWorldObj
{
public:
private:
	_tstring m_strText;
	XE::VEC3 m_vDelta;
//	XBaseFontDat *m_pFontDat = nullptr;
	XSPUnit m_spOwner;
	XBaseFontObj *m_pFontObj = nullptr;
	CTimer m_timerLife;
	int m_State = 0;
	XCOLOR m_Col = 0;
	XSurface *m_psfcBg = nullptr;
	void Init() {
	}
	void Destroy();
public:
	XObjYellSkill( LPCTSTR szText, 
								const XSPUnit& spOwner,
								const XE::VEC3& vwPos, 
								XCOLOR col );
	~XObjYellSkill() { Destroy(); }
	//
	GET_SET_ACCESSOR( XCOLOR, Col );
	//
	void Release() override {
		m_spOwner.reset();
	}
	void FrameMove( float dt ) override;
	void Draw( const XE::VEC2& vPos, float scale/* =1.f */, float alpha/* =1.f */ ) override;
}; // class XObjYellSkill
/****************************************************************
* @brief 바닥에 놓이는 화염구덩이
* @author xuzhu
* @date	2015/01/27 18:52
*****************************************************************/
class XObjFlame : public XEBaseWorldObj
{
	float m_Radius = 0.f;		// 데미지 반경
	CTimer m_timerDOT;			// 주기적으로 데미지를 주기위한.
	CTimer m_timerLife;			// 지속시간
	BIT m_bitCamp = 0;				// 데미지를 줄 대상의 진영
	float m_Damage = 0;			// 데미지
	XSPUnit m_spAttacker;		// 공격자.
	void Init() {}
	void Destroy();
public:
	XObjFlame( const XSPUnit& spAttacker, const XE::VEC3& vwPos, float damage, float radius, float secLife, BIT bitCampTarget, LPCTSTR szSpr, ID idAct );
	~XObjFlame() { Destroy(); }
	//
	GET_ACCESSOR( CTimer&, timerDOT );
	//
	void Release() override {
		m_spAttacker.reset();
	}
// 	void SetLifeTime( float sec ) {
// 		XBREAK( sec > 0 );
// 		m_timerLife.Set( sec );
// 	}
	void FrameMove( float dt ) override;
}; // class XObjFlame

/**
 @brief 전투중 떨어지는 자원객체. 생성되면 살짝 튀었다 떨어져서 일정시간 생존하다 사라진다.
 아군 유닛이 닿으면 획득 애니메이션이 나오고 사라진다.
*/
class XObjRes : public XEBaseWorldObj {
	XSurface* m_psfcShadow = nullptr;
	CTimer m_timerAlpha;
	XVector<XGAME::xRES_NUM> m_aryLoots;
	XGAME::xtResource m_resType = XGAME::xRES_NONE;
	int m_numRes = 0;
	std::shared_ptr<XCompObjFont> m_spCompFont;
	std::shared_ptr<XCompObjMove> m_spCompMove;
public:
	XObjRes( const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct, const XVector<XGAME::xRES_NUM>& aryLoots );
	XObjRes( const XE::VEC3& vwPos, XGAME::xtResource resType, int num );
	~XObjRes();
	//
	void SetBounce( float power, float dAngZ, float gravity = 1.f );
	inline void SetBounce( const XE::VEC2& vrPower, const XE::VEC2& vrdAngZ, float gravity = 1.f ) {
		SetBounce( xRandomF( vrPower ), xRandomF( vrdAngZ ), gravity );
	}
	void Release() {}
	void FrameMove( float dt ) override;
	void Draw( const XE::VEC2& vPos, float scale, float alpha ) override;
};
// class XObjRes : public XEBaseWorldObj {
// 	XSurface* m_psfcShadow = nullptr;
// 	CTimer m_timerAlpha;
// 	XVector<XGAME::xRES_NUM> m_aryLoots;
// 	XGAME::xtResource m_resType = XGAME::xRES_NONE;
// 	int m_numRes = 0;
// 	XCompObjFont m_compFont;
// 	XCompObjBounce m_compBounce;
// public:
// 	XObjRes( const XE::VEC3& vwPos, LPCTSTR szSpr, ID idAct, const XVector<XGAME::xRES_NUM>& aryLoots );
// 	XObjRes( const XE::VEC3& vwPos, XGAME::xtResource resType, int num );
// 	~XObjRes();
// 	//
// 	void Release() {}
// 	void FrameMove( float dt ) override;
// 	void Draw( const XE::VEC2& vPos, float scale, float alpha ) override;
// };

/****************************************************************
* @brief 자원아이콘과 개수숫자를 그린다. UI타입으로 가장 상위 레이어에 찍힌다. 
  생성된 위치에서 서서히 위로 올라가 사라진다.
* @author xuzhu
* @date	2016/05/28 16:40
*****************************************************************/
class XObjResNum : public XEBaseWorldObj
{
public:
	XObjResNum( const XE::VEC3& vwPos, XGAME::xtResource resType, int num );
	~XObjResNum() { Destroy(); }
	// get/setter
	// public member
private:
	// private member
	int m_State = 0;		// 0:튀어오름 1:바닥에 떨어져있음 2:사라지는중
	XE::VEC3 m_vwDelta;
	CTimer m_timerAlpha;
	std::shared_ptr<XCompObjFont> m_spCompFont;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Release() {}
	void FrameMove( float dt ) override;
	void Draw( const XE::VEC2& vPos, float scale, float alpha ) override;
}; // class XObjResNum

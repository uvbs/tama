/********************************************************************
	@date:	2014/10/14 9:49
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XUnitHero.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XBaseUnit.h"
#include "XPropHero.h"

class XHero;
/****************************************************************
* @brief 영웅 유닛
* @author xuzhu
* @date	2014/10/14 9:49
*****************************************************************/
class XUnitHero : public XBaseUnit
{
	XHero *m_pHero;
	XPropHero::xPROP *m_pProp;
// 	XPropUnit::xPROP *m_pPropUnit;		// 현재 인솔중인 유닛의 프로퍼티
	CTimer m_timerCool;			// 스킬 쿨타임
	BOOL m_bUseSkill;			// 스킬사용이 예약되었음
	XSKILL::XSkillObj *m_pSkillActive;
	XSKILL::XSkillObj *m_pSkillPassive;
	XSKILL::XSkillUser::xUseSkill m_infoUseSkill;
	bool m_bYell = false;
	void Init() {
		m_pHero = NULL;
		m_pProp = nullptr;
// 		m_pPropUnit = nullptr;
		m_bUseSkill = FALSE;
		m_pSkillPassive = nullptr;
		m_pSkillActive = nullptr;
	}
	void Destroy();
public:
	XUnitHero( XSquadObj *pSquadObj, XHero *pHero, ID idPropUnit, BIT bitSide, const XE::VEC3& vPos, float multipleAbility );
	virtual ~XUnitHero() { Destroy(); }
	virtual void OnCreate( void );
	///< 
	GET_ACCESSOR( CTimer&, timerCool );
	//////////////////////////////////////////////////////////////////////////
	// 스탯
//	float GetAttackMeleePower( void ) override;
//	float GetAttackRangePower( void ) override;
// 	float GetAttackMeleeDamage( UnitPtr spTarget ) override;
// 	float GetAttackRangeDamage( UnitPtr spTarget ) override;
//	float GetSpeedMovePerSec( void ) override;
//	float GetMovePixelPerFrame( float movPixelPerFrame ) override;
// 	float GetAttackRadiusByPixel( void ) override;
// 	float GetSpeedAttackBase() override {
// 		return m_pPropUnit->atkSpeed;
// 	}
//	int GetMaxHp( void ) override;
// 	float GetDefensePower( void ) override;
//	float GetAdjDamage( float damage ) override;
//	float GetCriticalRatio( void ) override;
//	float GetCriticalPower( void ) override;
//	float GetEvadeRatio( XSKILL::xtDamage typeDamage ) override;
//	float GetPenetrationRatio( void ) override;
	// 스탯
	//////////////////////////////////////////////////////////////////////////
	/// 공격사거리(픽셀)
	LPCTSTR GetszSpr( void ) override {
		return m_pProp->strSpr.c_str();
	}
// 	virtual BOOL IsRange( void ) {
// 		return m_pProp->IsRange();
// 	}
// 	virtual XGAME::xtSize GetUnitSize( void );
	float GetPropScale( void ) override {
		return m_pProp->scale;
	}
// 	virtual XGAME::xtAttack GetTypeAtk( void ) {
// 		if( IsRange() )
// 			return XGAME::xAT_RANGE;
// 		if( m_pProp->classJob == XGAME::xCLS_TANKER )
// 			return XGAME::xAT_TANKER;
// 		if( m_pProp->classJob == XGAME::xCLS_HORSE )
// 			return XGAME::xAT_SPEED;
// 		XBREAK(1);
// 		return XGAME::xAT_NONE;
// 	}
// 	virtual int GetMovSpeedNormal( void ) {
// 		return m_pProp->movSpeedNormal;
// 	}
	void FrameMove( float dt ) override; 
	void OnDebugStr( _tstring& str );
	int OnEndNormalAttackMotion( void );
	int OnBeforeAttackMotion( void );
	void OnHitEventSkill( const XE::VEC3& vwSrc );
// 	int OnApplyEffectNotAdjParam( XSKILL::XSkillUser *pCaster, XSKILL::XSkillDat* pSkillDat, XSKILL::EFFECT *pEffect, int level ) override;
	virtual void OnArriveBullet( XObjBullet *pBullet,
								const UnitPtr& spAttacker,
								const UnitPtr& spTarget,
								const XE::VEC3& vwDst,
								float damage,
								bool bCritical,
								LPCTSTR sprArrive, ID idActArrive,
								DWORD dwParam ) override;
// 	virtual BOOL IsHero( void ) {
// 		return TRUE;
// 	}
	int DoUseSkill( void );
	int DoSkillMotion( void );
	BOOL IsSkillCool( void ) {
		return m_timerCool.IsOn() && m_timerCool.IsOver() == FALSE;
	}
	virtual int GetSkillLevel( XSKILL::XSkillObj* pSkillObj );
	void OnEventSprObj( XSprObj *pSprObj, XKeyEvent *pKey, float lx, float ly, WORD idEvent, float fAngle, float fOverSec ) override;
	void ShootRangeAttack( UnitPtr& spTarget,
												const XE::VEC3& vwSrc,
												const XE::VEC3& vwDst,
												float damage,
												bool bCritical ) override;
	bool IsLeader() override;
	bool OnEventApplyInvokeEffect( XSKILL::XSkillUser* pCaster,
																	XSKILL::XBuffObj *pBuffObj,
																	XSKILL::XSkillDat *pSkillDat,
																	XSKILL::EFFECT *pEffect,
																	int level ) override;
	XSKILL::xtInvokeTarget
	OnGetInvokeTarget( XSKILL::XSkillDat *pDat,
										XSKILL::EFFECT *pEffect,
										XSKILL::xtInvokeTarget invokeTarget ) override;
};

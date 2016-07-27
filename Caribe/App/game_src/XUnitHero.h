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
	XBaseFontDat *m_pfdName = nullptr;	// 이름 표시용 폰트.
	XBaseFontObj *m_pfoName = nullptr;
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
	/// 공격사거리(픽셀)
	LPCTSTR GetszSpr( void ) override {
		return m_pProp->strSpr.c_str();
	}
	XE::VEC3 GetHSL() override { 
		return m_pProp->vHSL;
	}
	XE::xHSL GetHSL2() override {
		XE::xHSL hsl;
		hsl.m_vHSL = m_pProp->vHSL;
		return hsl;
	}
	float GetPropScale( void ) override {
		return m_pProp->scale;
	}
	void FrameMove( float dt ) override; 
	void OnDebugStr( _tstring& str );
	int OnEndNormalAttackMotion( void );
	int OnBeforeAttackMotion( void );
	void OnHitEventSkill( const XE::VEC3& vwSrc );
// 	int OnApplyEffectNotAdjParam( XSKILL::XSkillUser *pCaster, XSKILL::XSkillDat* pSkillDat, XSKILL::EFFECT *pEffect, int level ) override;
	void OnArriveBullet( XObjBullet *pBullet,
								UnitPtr spAttacker,
								UnitPtr spTarget,
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
//	void OnEventSprObj( XSprObj *pSprObj, XKeyEvent *pKey, float lx, float ly, ID idEvent, float fAngle, float fOverSec ) override;
	void OnEventHit( const xSpr::xEvent& event ) override;
	void ShootRangeAttack( UnitPtr& spTarget,
												const XE::VEC3& vwSrc,
												const XE::VEC3& vwDst,
												float damage,
												bool bCritical,
												const std::string& strType,
												const _tstring& strSpr ) override;
	bool IsLeader() override;
	bool OnEventApplyInvokeEffect( XSKILL::XSkillUser* pCaster,
																	XSKILL::XBuffObj *pBuffObj,
																	XSKILL::XSkillDat *pSkillDat,
																	const XSKILL::EFFECT *pEffect,
																	int level ) override;
	XSKILL::xtInvokeTarget
	OnGetInvokeTarget( XSKILL::XSkillDat *pDat,
										const XSKILL::EFFECT *pEffect,
										XSKILL::xtInvokeTarget invokeTarget ) override;
	void OnArriveTarget( UnitPtr spUnit, const XE::VEC3& vwDst ) override;
	void OnAISet( bool bSet ) override;
	XE::VEC2 DrawName( const XE::VEC2& vPos, float scaleFactor, float scale, const XE::VEC2& vDrawHp ) override;
	_tstring GetstrIds() override;
};

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
	CTimer m_timerCool;			// 스킬 쿨타임
	BOOL m_bUseSkill;			// 스킬사용이 예약되었음
	XSKILL::XSkillDat* m_pSkillActive;
	XSKILL::XSkillDat* m_pSkillPassive;
	XSKILL::XSkillUser::xUseSkill m_infoUseSkill;
	bool m_bYell = false;
	XBaseFontDat *m_pfdName = nullptr;	// 이름 표시용 폰트.
	XBaseFontObj *m_pfoName = nullptr;
	void Init() {
		m_pHero = NULL;
		m_pProp = nullptr;
		m_bUseSkill = FALSE;
		m_pSkillPassive = nullptr;
		m_pSkillActive = nullptr;
	}
	void Destroy();
public:
	XUnitHero( XSPSquad spSquadObj, XHero *pHero, ID idPropUnit, BIT bitSide, const XE::VEC3& vPos, float multipleAbility );
	virtual ~XUnitHero() { Destroy(); }
	virtual void OnCreate();
	///< 
	GET_ACCESSOR( CTimer&, timerCool );
	/// 공격사거리(픽셀)
	LPCTSTR GetszSpr() override {
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
	float GetPropScale() override {
		return m_pProp->scale;
	}
	void FrameMove( float dt ) override; 
	void OnDebugStr( _tstring& str );
	int OnEndNormalAttackMotion();
	int OnBeforeAttackMotion();
	void OnHitEventSkill( const XE::VEC3& vwSrc );
	void OnArriveBullet( XObjBullet *pBullet,
											 UnitPtr spAttacker,
											 UnitPtr spTarget,
											 const XE::VEC3& vwDst,
											 float damage,
											 bool bCritical,
											 LPCTSTR sprArrive, ID idActArrive,
											 DWORD dwParam ) override;
	int DoUseSkill();
	int DoSkillMotion();
	BOOL IsSkillCool() const {
		return m_timerCool.IsOn() && m_timerCool.IsOver() == FALSE;
	}
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

#pragma once

#include "XFramework/Game/XEWorld.h"
#include "Skill/xSkill.h"
#include "XObjEtc.h"

#define WORLD	XBattleField::sGet()

class XLegionObj;
class XWndBattleField;
class XLegion;
class XAccount;
////////////////////////////////////////////////////////////////
class XBattleField : public XEWorld, 
						public XDelegateObjMng
{
public:
	static XBattleField* sGet() {
		return s_pInstance;
	}
private:
	static XBattleField *s_pInstance;
	XGAME::xtBattle m_typeBattle = XGAME::xBT_NONE;
	XVector<XGAME::xRES_NUM> m_aryLoots;
	void Init() {
	}
	void Destroy();
public:
	XBattleField( const XE::VEC2& vSize );
	virtual ~XBattleField() { Destroy(); }
	//
	GET_SET_ACCESSOR( XGAME::xtBattle, typeBattle );
	XSPLegionObj GetLegionObj( int idx ) const;
	///< 
	void Release() override;
	void DestroyAllObj() override;
	// 월드좌표를 스크린전체 좌표로 변환한다.
// 	virtual XE::VEC2 GetPosWorldToScreen( const XE::VEC3& vwPos ) {
// 		return XE::VEC2(0);
// 	}
// 	// 월드좌표를 윈도우내 로컬좌표로 변환한다.
// 	virtual XE::VEC2 GetPosWorldToWindow( const XE::VEC3& vwPos ) {
// 		return XE::VEC2( 0 );
// 	}
	int Process( XEWndWorld *pWndWorld, float dt ) override;
	void Draw( XEWndWorld *pWndWorld ) override;
	//////////////////////////////////////////////////////////////////////////
	// SKILL
// 	XSKILL::XSkillReceiver* GetTarget( ID snObj );
	/**
	 @brief snObj오브젝트의 포인터를 구해준다. 그 오브젝트는 XSkillUser를 상속받은 객체여야 한다.
	*/
// 	virtual XSKILL::XSkillUser* GetCaster( ID snObj );
	// SKILL
	//////////////////////////////////////////////////////////////////////////
	XSPSquadObj FindNearSquadEnemy( XSquadObj *pFinder );
	void OnStartBattle();
	void OnDieLegion( XSPLegionObj spLegion );
	void OnDieSquad( XSPSquadObj spSquadObj );
	void OnDelegateFrameMoveEachObj( float dt, ID idEvent, XSPWorldObj spObj );
	XSPSquadObj GetPickSquad( const XE::VEC3& vwPick, BIT bitCamp, ID snExclude = 0 );
	XSPLegionObj GetEnemyLegionObj( XSquadObj *pSrc );
	XSPLegionObj GetAILegionObj() const {
		return GetLegionObj( 1 );
	}
	XSPLegionObj GetPlayerLegionObj() const {
		return GetLegionObj( 0 );
	}
	int GetNearSquadEnemy( XSquadObj *pSquadSrc,
					XArrayLinearN<XSPSquadObj, 64> *pOutAry,
					float radius );
	int GetNearSquad( XSquadObj *pSquadSrc,
					XArrayLinearN<XSPSquadObj, 64> *pOutAry,
					float radius );
	XSPSquadObj GetSquadBySN( XGAME::xtLegionIdx idxLegion, ID snSquad );
	void SpawnSquadByCheat( const XE::VEC3& vwPos, XGAME::xtUnit unit, bool bEnemy, XSPAcc spAcc );
  void SetAI( bool bFlag );
	void DrawLegionBar( const XE::VEC2& vPos, int idxLegion ) const;
	XSPUnit GetHeroUnit( ID idProp );
	XSPWorldObj AddpObj( XEBaseWorldObj *pNewObj );
	void SetLootRes( const XVector<XGAME::xRES_NUM>& aryLoots );
	void Clear();
private:
};



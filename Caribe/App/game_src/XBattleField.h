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
//	XVector<XSPLegionObj> m_aryLegion;	// 0:아군 1:적군
//	LegionPtr m_spLegionEnemy;
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
// 	inline XSPLegionObj GetLegionObj( int idx ) const {
// 		return m_aryLegion[idx];
// 	}
	XSPLegionObj GetLegionObj( int idx ) const;
	///< 
	virtual void Release( void );
	virtual void DestroyAllObj( void );
	// 월드좌표를 스크린전체 좌표로 변환한다.
	virtual XE::VEC2 GetPosWorldToScreen( const XE::VEC3& vwPos ) {
		return XE::VEC2(0);
	}
	// 월드좌표를 윈도우내 로컬좌표로 변환한다.
	virtual XE::VEC2 GetPosWorldToWindow( const XE::VEC3& vwPos ) {
		return XE::VEC2( 0 );
	}
// 	BOOL CreateLegionObj( LegionPtr& spLegion, BIT bitSide, BOOL bDestroyLegion );
	virtual int Process( XEWndWorld *pWndWorld, float dt );
	virtual void Draw( XEWndWorld *pWndWorld );
	//////////////////////////////////////////////////////////////////////////
	// SKILL
	XSKILL::XSkillReceiver* GetTarget( ID snObj );
	/**
	 @brief snObj오브젝트의 포인터를 구해준다. 그 오브젝트는 XSkillUser를 상속받은 객체여야 한다.
	*/
	virtual XSKILL::XSkillUser* GetCaster( ID snObj );
	// SKILL
	//////////////////////////////////////////////////////////////////////////
	SquadPtr FindNearSquadEnemy( XSquadObj *pFinder );
#if defined(_XSINGLE) || !defined(_CLIENT)
// 	void ResetLegionObj( /*XWndBattleField *pWndWorld*/ );
// 	void RecreateLegionObj( /*XWndBattleField *pWndWorld*/ );
#endif // defined(_XSINGLE) || !defined(_CLIENT)
//	void CreateSquadObj( XWndBattleField *pWndWorld );
	void OnStartBattle();
	void OnDieLegion( XSPLegionObj spLegion );
	void OnDieSquad( XSquadObj* pSquadObj );
	void OnDelegateFrameMoveEachObj( float dt, ID idEvent, WorldObjPtr spObj );
	SquadPtr GetPickSquad( const XE::VEC3& vwPick, BIT bitCamp, ID snExclude = 0 );
	XSPLegionObj GetEnemyLegionObj( XSquadObj *pSrc );
	XSPLegionObj GetAILegionObj() const {
		return GetLegionObj( 1 );
	}
	XSPLegionObj GetPlayerLegionObj() const {
		return GetLegionObj( 0 );
	}
	int GetNearSquadEnemy( XSquadObj *pSquadSrc,
					XArrayLinearN<SquadPtr, 64> *pOutAry,
					float radius );
	int GetNearSquad( XSquadObj *pSquadSrc,
					XArrayLinearN<SquadPtr, 64> *pOutAry,
					float radius );
	SquadPtr GetSquadBySN( XGAME::xtLegionIdx idxLegion, ID snSquad );
	void SpawnSquadByCheat( const XE::VEC3& vwPos, XGAME::xtUnit unit, bool bEnemy );
  void SetAI( bool bFlag );
	void DrawLegionBar( const XE::VEC2& vPos, int idxLegion );
	UnitPtr GetHeroUnit( ID idProp );
	WorldObjPtr AddpObj( XEBaseWorldObj *pNewObj );
	void SetLootRes( const XVector<XGAME::xRES_NUM>& aryLoots );
	void Clear();
};



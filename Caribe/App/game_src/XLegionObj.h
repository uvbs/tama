#pragma once
//#include "XPropUnit.h"
#include "XFramework/Game/XEComponents.h"
//#include "Skill/xSkill.h"
//#include "XHero.h"
////////////////////////////////////////////////////////////////
namespace XSKILL {
class XSkillReceiver;
enum xtJuncture : int;
}
class XBaseUnit;
class XHero;
class XSquadron;
class XWndBattleField;
class XSquadObj;
class XLegionObj;
class XEWndWorld;
class XSquadObj;
class XStatistic;
//#include "XBaseUnit.h"

////////////////////////////////////////////////////////////////
class XLegion;
class XWndBattleField;
class XLegionObj : public std::enable_shared_from_this <XLegionObj>
{
public:
	static int sGetnumObj() {
		return s_numObj;
	}
	static void sClearnumObj() {
		s_numObj = 0;
	}
private:
	static int s_numObj;		// 메모리 릭 추적용
	XSPLegionObj GetThis() {
		return shared_from_this();
	}
	XSPLegion m_spLegion;
	XList4<XSPSquadObj> m_listSquad;
	XECompCamp m_Camp;
	int m_cntLive;				///< 현재 살아있는 부대수. 부대가 전멸할때마다 하나씩 카운팅 된다.
	XStatistic *m_pStatObj = nullptr;		///< 전투통계 객체
  float m_Def = 0.f;      // 군단에 걸려있는 추가 방어력
	void Init() {
		m_cntLive = 0;
		++s_numObj;
	}
	void Destroy();
public:
	XLegionObj( XSPLegion& spLegion, BIT bitCamp, BOOL bDestroyLegion = FALSE );
	virtual ~XLegionObj() { Destroy(); --s_numObj; }
	//
	void Release();
	GET_ACCESSOR2( XSPLegion, spLegion );
	GET_ACCESSOR_CONST( const XECompCamp&, Camp );
	GET_ACCESSOR( int, cntLive );
	XStatistic* GetpStatObj() const {
		return const_cast<XStatistic*>( m_pStatObj );
	}
	GET_ACCESSOR_CONST( const XList4<XSPSquadObj>&, listSquad );
  GET_SET_ACCESSOR( float, Def );
	XSPHero GetpLeader();
	BOOL IsPlayer() const {
		return m_Camp == XGAME::xSIDE_PLAYER;
	}
	BOOL IsOther() const {
		return m_Camp == XGAME::xSIDE_OTHER;
	}
	//
	BOOL CreateLegion( XWndBattleField *pWndWorld, const XE::VEC3& vwStart, XGAME::xtLegionOption bitOption );
#ifdef _CHEAT
	BOOL CreateLegionDebug( XWndBattleField *pWndWorld, XE::VEC3 vwStart, XGAME::xtLegionOption bitOption );
	void MakeResource();
#endif
	XSquadObj* AddSquad( const XSPSquadObj& spSquad );
	/// 부대가 생성되거나 전멸할때 하나씩 카운팅 해준다.
	int AddCntLive( int add ) {
		m_cntLive += add;
		XBREAK( m_cntLive < 0 || m_cntLive > XGAME::MAX_SQUAD );
		return m_cntLive;
	}
	void SetAI( BOOL bFlag );
	void OnSkillEvent( XSKILL::xtJuncture event );
	XSPSquadObj FindNearSquad( XSquadObj *pFinder );
	XSPSquadObj FindNearSquad( XSquadObj *pFinder, bool( *pFunc )( XSPSquadObj& ) );
	XSPUnit FindNearUnit( const XE::VEC3& vwSrc, float meterRadius ) const;
//	XSPUnit FindAttackTargetInSquad( XSquadObj *pFinder, const XSPSquad& spAt, XSPSquad *pOutSquad );
	void FrameMove( float dt );
	void Draw( XEWndWorld *pWndWorld );
	void OnDieSuqad( XSPSquadObj spSquad );
	BOOL IsNearestSquad( const XSPSquadObj& spBase, XSPSquadObj& spOut );
	XSPSquadObj GetPickSquad( const XE::VEC3& vwPick, BIT bitCamp, ID snExclude = 0 );
	int GetNearSquad( XSquadObj *pSquadSrc,
					XArrayLinearN<XSPSquadObj, 64> *pOutAry,
					float radius );
	XSPSquadObj GetSquadBySN( ID snSquad );
	void KillAllUnit();
	XSPSquadObj FindNearSquadLeastHp( XSquadObj *pFinder,
																 float pixelRadius,
																 BIT bitCampFind,
																 BOOL bHighest );
	XSPSquadObj FindSquadRandom( XSquadObj *pFinder,
														float pixelRadius,
														bool bIncludeFinder );
	void OnStartBattle();
	void OnAfterStartBattle();
	int GetAllUnit( XVector<XSKILL::XSkillReceiver*> *pOutAry );
	int GetAllUnit( XVector<XBaseUnit*> *pOutAry );
	XSPUnit GetHeroUnit( ID idHero );
	void DoFullHp();
	XSPSquadObj FindSquadTakeTargetAndClosed( XSquadObj *pFinder, XSquadObj *pTarget, XGAME::xtUnit unitFilter = XGAME::xUNIT_NONE );
	float GetSumHpAllSquad();
	float GetMaxHpAllSquad();
	int SerializeForGuildRaid( XArchive& ar );
	void DrawSquadsBar( const XE::VEC2& vPos );
	void GetArySquadByResource( XVector<XSPSquadObj>* pOut );
	XSPSquadObj CreateSquadObj( int i, int j, 
													 const XE::VEC3& vwBase, 
													 XGAME::xtLegionOption bitOption, 
													 XWndBattleField* pWndWorld ) const;
	static XSPSquadObj sCreateSquadObj( XSPLegionObjConst spLegionObj,
																	 XSPSquadron pSquad,
																	 const XE::VEC3& vwPos,
																	 XGAME::xtLegionOption bitOption,
																	 XWndBattleField* pWndWorld );
	bool CreateSquadObjList( XWndBattleField *pWndWorld, const XE::VEC3& vwStart, XGAME::xtLegionOption bitOption );
	XSPSquadObj GetspSquadObjByIdx( int idx );
	inline XSPSquadObj GetspSquadObjBySN( ID snSquad ) {
		return GetSquadBySN( snSquad );
	}
	void DoDamage();
};

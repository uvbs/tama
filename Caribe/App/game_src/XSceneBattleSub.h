#pragma once
#include "constGame.h"
#include "etc/Global.h"
#include "XFramework/XESceneMng.h"

class XWndBattleField;

XE_NAMESPACE_START( XGAME )
struct xSceneBattleParam : public xSceneParamBase {
	ID m_idEnemy = 0;			// 상대가 pc의 경우.
	xtSpot m_typeSpot = XGAME::xSPOT_NONE;	// 전투가벌어지는 스팟의 타입(에러확인용)
	ID m_idSpot = 0;					// 전투가 벌어지는 스팟(0인경우도 있음)
	int m_Level = 0;					// 상대의 레벨
	_tstring m_strName;				// 상대이름
	XVector<XSPLegion> m_spLegion;	// 0:아군 1:적군
	xtBattle m_typeBattle = XGAME::xBT_NONE;
	int m_Defense = 0;				// 방어도(보석광산용)
	int m_idxStage = -1;				///< 캠페인의 경우 스테이지 인덱스
	int m_idxFloor = 0;
	xSceneBattleParam( ID idEnemy,
										 xtSpot typeSpot,
										 ID idSpot,
										 int level,
										 const _tstring& strName,
										 const XVector<XSPLegion>& aryLegion,
										 xtBattle typeBattle,
										 int def, int idxStage, int idxFloor )
		: m_idEnemy( idEnemy )
		, m_typeSpot( typeSpot )
		, m_idSpot( idSpot )
		, m_Level( level )
		, m_strName( strName )
		, m_spLegion( aryLegion )
		, m_typeBattle( typeBattle )
		, m_Defense( def )
		, m_idxStage( idxStage )
		, m_idxFloor( idxFloor ) {	}
	// npc와 전투하는것인가.
	bool IsVsNpc() const {
		return m_idEnemy == 0;
	}
	inline bool IsPrivateRaid() const {
		return m_typeBattle == XGAME::xBT_PRIVATE_RAID;
	}
	inline bool IsNormal() const {
		return m_typeBattle == XGAME::xBT_NORMAL;
	}
	bool IsValid() const override;
// 	inline bool IsInvalid() const {
// 		return !IsValid();
// 	}
	inline void Release() {
		m_spLegion[0].reset();
		m_spLegion[1].reset();
	}
}; // xSceneBattleParam

// 전투시 양측 진영의 모든 정보.
struct xsCamp	{
	xtSide m_bitSide = xSIDE_NONE;
	XSPAcc m_spAcc;
	ID m_idAcc = 0;				// 이 진영이 pc일경우.
	int m_Level = 0;			// 군단(군주)레벨( m_spAcc는 앞으로 안써야하기때문)
	_tstring m_strName;		// 군단(군주) 이름.
	XSPLegion m_spLegion;
	XSPLegionObj m_spLegionObj;
	xtBattle m_bitOption = XGAME::xBT_NONE;
	XSurface* m_psfcProfile = nullptr;
#ifdef _XSINGLE
	std::string m_idsLegion;		// XPropLegion의 군단ids
// 	void ReCreateLegion( XWndBattleField* pWndWorld );
	void CreateLegion( const std::string& idsLegion, xtSide bitSide );
#endif // _XSINGLE
	void CreateLegionObj();
	void CreateSquadObjs( XWndBattleField* pWndWorld, xtBattle typeBattle );
	void SetAccInfo( xtSide bitSide, ID idAcc, int lvAcc, const _tstring& strName ) {
		m_bitSide = bitSide;
		m_idAcc = idAcc;
		m_Level = lvAcc;
		m_strName = strName;
	}
	void Release();
};
	
XE_NAMESPACE_END;

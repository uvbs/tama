﻿#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XArchive.h"
#include "XSpot.h"
#include "XParticleMng.h"

class XGame;
class XLegion;
class XSpot;
class XSpotSulfur;
class XWndBattleField;
class XLegionObj;
class XSceneProcess;
class XUnitHero;
class XBaseItem;
class XWndProgressBar2;
class XWndFaceInBattle;
class XSurface;
class XWndBatchRender;

XE_NAMESPACE_START( XGAME )
struct xSceneBattleParam : public xSceneParamBase {
	ID m_idEnemy = 0;			// 상대가 pc의 경우.
#ifndef _XSINGLE
	xtSpot m_typeSpot = XGAME::xSPOT_NONE;	// 전투가벌어지는 스팟의 타입(에러확인용)
	ID m_idSpot = 0;					// 전투가 벌어지는 스팟(0인경우도 있음)
#endif // not _XSINGLE
	int m_Level = 0;					// 상대의 레벨
	_tstring m_strName;				// 상대이름
	XVector<XSPLegion> m_spLegion;	// 0:아군 1:적군
	xtBattle m_typeBattle = XGAME::xBT_NONE;
	int m_Defense = 0;				// 방어도(보석광산용)
	int m_idxStage = -1;				///< 캠페인의 경우 스테이지 인덱스
	int m_idxFloor = 0;
	xSceneBattleParam( ID idEnemy,
#ifndef _XSINGLE
										 xtSpot typeSpot,
										 ID idSpot,
#endif // not _XSINGLE
										 int level,
										 const _tstring& strName,
										 XVector<XSPLegion> aryLegion,
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
	bool IsValid() const;
	inline bool IsInvalid() const {
		return !IsValid();
	}
// 	inline void Release() {
// 		m_spLegion[0].reset();
// 		m_spLegion[1].reset();
// 	}
};

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
	void CreateSquadObj( XWndBattleField* pWndWorld, xtBattle typeBattle );
	void SetAccInfo( xtSide bitSide, ID idAcc, int lvAcc, const _tstring& strName ) {
		m_bitSide = bitSide;
		m_idAcc = idAcc;
		m_Level = lvAcc;
		m_strName = strName;
	}
	void Release();
};
	
XE_NAMESPACE_END;
/**
 @brief 
*/
class XSceneBattle : public XSceneBase, public XParticleDelegate
{
	friend class XSceneProcessBattle;
	friend class XSceneProcessReady;
public:
	static bool sIsHaveBattleStart() {
		return s_BattleStart.IsValid();
	}
	static bool sIsEmptyBattleStart() {
		return !sIsHaveBattleStart();
	}
	static const XGAME::xBattleStart& sGetBattleStart() {
		return s_BattleStart;
	}
	static void sSetBattleStart( const XGAME::xBattleStart& bs ) {
		// 복사로 전달.
		s_BattleStart = bs;
	}
private:
	// 전투시작시 필요한 정보들을 외부에서 적재한후 배틀씬을 부른다.
	static XGAME::xBattleStart s_BattleStart;
private:
	XLayoutObj m_Layout;
	/*
	전투시 적 군단데이터는 무조건 복사본을 가지고 있는걸로 한다.
	NPC스팟의 군단의 경우 스팟의 포인터를 가져와 쓴다고 해도 만약 배틀에서 졌을경우는 스팟의 군단이 지워져선 안되기 때문에
	그냥 일관되게 복사본을 사용해서 전투가 끝나면 모두 지우는걸로 하는게 좋겠다.
	*/
	XSPSceneParam m_spSceneParam;
	XWndBattleField *m_pWndWorld;		// 오브젝트들이 배치될 월드공간
	XSceneProcess *m_pProcess;		// 씬의 흐름을 컨트롤하는 객체
	XVector<XWndProgressBar2*> m_aryBar;		// 0:아군 1:적군
	float m_hpMaxLegion[2];		// 0:아군 1:적군
	CTimer m_timerPlay;
	bool m_bFinish = false;			// 땜빵. 
	XGAME::xFocusMng m_FocusMng;		// 카메라 관리자.
	int m_mulPlay = 1;		// 플레이 배속
	XGAME::xBattleResult m_resultBattle;		// 전투결과
	XSpot* m_pSpot = nullptr;
	CTimer m_timerResult;
	XVector<XGAME::xsCamp> m_aryCamp;			// 양측 진영의 모든 정보들.
	//
	void Init()  {
		m_pWndWorld = nullptr;
		m_pProcess = nullptr;
	}
	void Destroy();
protected:
public:
	XSceneBattle( XGame *pGame, XSPSceneParam& spBaseParam );
	virtual ~XSceneBattle(void) { Destroy(); }
	//
	inline const XGAME::xsCamp& GetCamp( XGAME::xtSideIndex idxSide ) const {
		return m_aryCamp[ idxSide ];
	}
	inline XSPLegionObj GetspLegionObjMutable( XGAME::xtSideIndex idxSide ) {
		return m_aryCamp[idxSide].m_spLegionObj;
	}
	inline XSPLegionObjConst GetspLegionObj( XGAME::xtSideIndex idxSide ) const {
		return m_aryCamp[idxSide].m_spLegionObj;
	}
	inline XSPLegionObj GetspLegionObjMutable( XGAME::xtSide bitSide ) {
		return ((bitSide == XGAME::xSIDE_PLAYER)? 
						m_aryCamp[XGAME::xSI_PLAYER].m_spLegionObj :
						m_aryCamp[XGAME::xSI_OTHER].m_spLegionObj);
	}
	inline XSPLegionObjConst GetspLegionObj( XGAME::xtSide bitSide ) const {
		return (( bitSide == XGAME::xSIDE_PLAYER ) ?
						m_aryCamp[XGAME::xSI_PLAYER].m_spLegionObj :
						m_aryCamp[XGAME::xSI_OTHER].m_spLegionObj);
	}
	//
	void CreateSquadObj( void );
	// virtual
	virtual BOOL OnCreate();
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	void GetSquadInfoToAry( XSPSquad spSquad, XBaseUnit* pUnit, XVector<_tstring>* pOut ) const;
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	int OnEnterScene( XWnd*, DWORD, DWORD ) override;		// 씬이 모두 밝아지고 호출됨
	//
	void OnRecvBattleResult( XGAME::xBattleResult& result );
	void DoPopupBattleResult( XGAME::xBattleResult& result );
	// 	void OnRecvBattleResult( XGAME::xtSpot typeSpot, bool bWin, int addScore,
//                             XArrayLinearN<ItemBox, 256>& aryDrops,
//                             XArrayLinearN<XGAME::xRES_NUM,XGAME::xRES_MAX> *pAryLoot=nullptr );
	int OnOkBattleResult( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnReconOk( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnRecvBattleResultSulfurEncounter( XSpotSulfur *pSpot, const XGAME::xBattleStartInfo& info, std::shared_ptr<xSceneBattleParam> spParam );
	int OnOkBattleResultSulfurEncounter( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnEndSceneProcess( XSceneProcess *pProcess );
// 	int OnDebugRetry( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnDebugRecreate( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnDebugProfile( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnUseSkillByButton( XWnd* pWnd, DWORD p1, DWORD p2 );
	XWndFaceInBattle* GetpWndFace( ID snHero );
	void OnUseSkill( XSPSquad spSquadObj, const _tstring& strText );
	void OnSelectSquad( const XSPSquad& squadSelect );
	void OnStartSkillMotion( XUnitHero *pUnitHero, float secCool );
	int OnCancelSelect( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnFinishBattle( XGAME::xtSide bitCampWin, bool bRetreatSulfur );
	void CreateDebugButtons( void );
	int OnDebugAllKill( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnOkHerosExp( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnDebugAbil( XWnd* pWnd, DWORD p1, DWORD p2 );

	int OnSurrrender(XWnd* pWnd, DWORD p1, DWORD p2);		// 항복 버튼
	void Update() override;
	// 헬퍼 함수
	void CreateBattleUI();
	//
	int OnSulfurRetreat( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnCheat( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickStatistic( XWnd* pWnd, DWORD p1, DWORD p2 );
	void SendFinishBattle( XGAME::xtExitBattle ebCode, XGAME::xtSide bitWinner, int idxStgae, bool bRetreatSulfur );
	XE::VEC2 OnDelegateProjection2( const XE::VEC3& vPos );
	bool OnDelegateIsInScreen( const XE::VEC2& vScr ) override;
	float OnDelegateGetScaleViewport() override;
	void OnReload() override;
	void CreateParticleSfx();
	void OnControlSquad( const XHero *pHero );
//	int OnClickOkByTutorialPopup( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnEndCutScene( const std::string& idsEndSeq ) override;
	bool IsDialogMsg( DWORD idSeqPopup );
	void DoMoveCamera( const XE::VEC2& vwDstCenter, float secMove = 0.f);
	XE::VEC2 GetvwCamera();
	void OnCreateOrderDialog( ID idHero ) override;
	XHero* GetpHero( ID idHero ) override;
	void OnDieSquad( XSPSquad spSquadObj );
#ifdef _XSINGLE
	static void sSetAbilHeroes();
	static void sSetAbilHero( XHero *pHero, XGAME::xtUnit unit, LPCTSTR idsAbil, int point );
	static XSPAcc sCreateAcc();
	void SaveSingle();
	void SetAI( bool bFlag );
#endif // _XSINGLE

private:
	void OnEnterBattle();
	void CreateProcessReady();
	int OnClickPlay( XWnd* pWnd, DWORD p1, DWORD p2 );
	XSPSquad GetspSquadObj( ID snSquad );
	int OnTouchHeroFace( XWnd* pWnd, DWORD snSquad, DWORD );
	void CheckLeak();
	static void sSetBattleParamForSingle();
	void CreateCamps();
	void Release() override;
	void CreateHeroesFace();
	int OnDebugButton( XWnd* pWnd, DWORD p1, DWORD p2 );
	XWndBatchRender* GetpLayerUI();
	XWndBatchRender* GetpLayerFaces();
};

extern XSceneBattle *SCENE_BATTLE;

namespace XGAME {
	extern XAccount *x_pAccEnemy;	// 임시. 
};

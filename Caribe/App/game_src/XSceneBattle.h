#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XArchive.h"
#include "XSpot.h"
#include "XParticleMng.h"
#include "XSceneBattleSub.h"

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

#define MAX_SQUAD_PVRAID		30

namespace XGAME {
struct xSceneBattleParam;
struct xsCamp;
}
/**
 @brief 
*/
class XSceneBattle : public XSceneBase, public XParticleDelegate
{
	friend class XSceneProcessBattle;
	friend class XSceneProcessReady;
public:
	static std::shared_ptr<XGAME::xSceneBattleParam> sSetBattleParam();
private:
private:
	XLayoutObj m_Layout;
	/*
	전투시 적 군단데이터는 무조건 복사본을 가지고 있는걸로 한다.
	NPC스팟의 군단의 경우 스팟의 포인터를 가져와 쓴다고 해도 만약 배틀에서 졌을경우는 스팟의 군단이 지워져선 안되기 때문에
	그냥 일관되게 복사본을 사용해서 전투가 끝나면 모두 지우는걸로 하는게 좋겠다.
	*/
	std::shared_ptr<XGAME::xSceneBattleParam> m_spSceneParam;
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
	GET_ACCESSOR_CONST( std::shared_ptr<XGAME::xSceneBattleParam>, spSceneParam );
	GET_ACCESSOR_PTR( XWndBattleField*, pWndWorld );
	XSceneBattle( XGame *pGame, XGAME::xtScene scene, XSPSceneParam spBaseParam );
public:
	XSceneBattle( XGame *pGame, XSPSceneParam spBaseParam )
		: XSceneBattle( pGame, XGAME::xSC_INGAME, spBaseParam ) {}
	virtual ~XSceneBattle( void ) {
		Destroy();
	}
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
	void GetSquadInfoToAry( XSPSquadObj spSquad, XBaseUnit* pUnit, XVector<_tstring>* pOut ) const;
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
	void OnRecvBattleResultSulfurEncounter( XSpotSulfur *pSpot, const XGAME::xBattleStartInfo& info, std::shared_ptr<XGAME::xSceneBattleParam> spParam );
// 	int OnOkBattleResultSulfurEncounter( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnEndSceneProcess( XSceneProcess *pProcess );
	virtual int GetsecTimeOver();
	//virtual void OnStartProcessBattle();
	// 	int OnDebugRetry( XWnd* pWnd, DWORD p1, DWORD p2 );
// 	int OnDebugRecreate( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnDebugProfile( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnUseSkillByButton( XWnd* pWnd, DWORD p1, DWORD p2 );
	XWndFaceInBattle* GetpWndFace( ID snHero );
	void OnUseSkill( XSPSquadObj spSquadObj, const _tstring& strText );
	void OnSelectSquad( const XSPSquadObj& squadSelect );
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
//	int OnSulfurRetreat( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnCheat( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickStatistic( XWnd* pWnd, DWORD p1, DWORD p2 );
	void SendFinishBattle( XGAME::xtExitBattle ebCode, XGAME::xtSide bitWinner, int idxStgae, bool bRetreatSulfur );
	XE::VEC2 OnDelegateProjection2( const XE::VEC3& vPos );
	bool OnDelegateIsInScreen( const XE::VEC2& vScr ) override;
	float OnDelegateGetScaleViewport() override;
	void OnReload() override;
	void CreateParticleSfx();
	void OnControlSquad( XSPHeroConst pHero );
//	int OnClickOkByTutorialPopup( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnEndCutScene( const std::string& idsEndSeq ) override;
	bool IsDialogMsg( DWORD idSeqPopup );
	void DoMoveCamera( const XE::VEC2& vwDstCenter, float secMove = 0.f);
	XE::VEC2 GetvwCamera();
	void OnCreateOrderDialog( ID idHero ) override;
	XSPHero GetpHero( ID idHero ) override;
	virtual void OnDieSquad( XSPSquadObj spSquadObj );
#ifdef _XSINGLE
	static void sSetAbilHeroes();
	static void sSetAbilHero( XSPHero pHero, XGAME::xtUnit unit, LPCTSTR idsAbil, int point );
	static XSPAcc sCreateAcc();
	void SaveSingle();
	void SetAI( bool bFlag );
#endif // _XSINGLE
protected:
	XSPWorldConst GetspWorld() const;
	void AddObj( XSPWorldObj spObj );
private:
	void OnEnterBattle();
	void CreateProcessReady();
	int OnClickPlay( XWnd* pWnd, DWORD p1, DWORD p2 );
	XSPSquadObj GetspSquadObj( ID snSquad );
	int OnTouchHeroFace( XWnd* pWnd, DWORD snSquad, DWORD );
	void CheckLeak();
	void CreateCamps();
	void Release() override;
	void CreateHeroesFace();
	virtual void GetSquadObjToAry( int idxSide, XSPLegionObjConst spLegionObj, XVector<XSPSquadObjConst>* pOut );
	void UpdateHeroesFace();
	virtual void OnCreateFaces( XSPLegionObj spLegionObj, int idxSide, XWnd* pWndLayer );
	void ArrangeFaces( const XVector<XVector<XWnd*>>& aryFaces, XWnd* pWndLayer, int idxSide );
	void ArrangeAry( XVector<XVector<XWnd*>>* pOut, XWnd* pWndLayer );
	void CreateFaceWnds( const XLegionObj* pLegionObj, int idxSide, XWnd* pWndLayer );
	int OnDebugButton( XWnd* pWnd, DWORD p1, DWORD p2 );
	XWndBatchRender* GetpLayerUI();
	XWndBatchRender* GetpLayerFaces();
	XSPWorld GetspmWorld();
	void OnDieSquadPrivateRaid( XSPSquadObj spSquadObj );
	int OnDebugTest( XWnd* pWnd, DWORD p1, DWORD p2 );
};

extern XSceneBattle *SCENE_BATTLE;

namespace XGAME {
	extern XAccount *x_pAccEnemy;	// 임시. 
};

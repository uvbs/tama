#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
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
class XSceneBattle : public XSceneBase, public XParticleDelegate
{
//	static int s_mulPlay = 1;			// 플레이 배속
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
	XWndBattleField *m_pWorld;		// 오브젝트들이 배치될 월드공간
	XSceneProcess *m_pProcess;		// 씬의 흐름을 컨트롤하는 객체
	XWndProgressBar *m_pAllyBar;
	XWndProgressBar *m_pEnemyBar;
	float m_hpMaxLegion[2];
	CTimer m_timerPlay;
	bool m_bFinish = false;			// 땜빵. 
	XGAME::xFocusMng m_FocusMng;		// 카메라 관리자.
	int m_mulPlay = 1;		// 플레이 배속
	XGAME::xBattleResult m_resultBattle;		// 전투결과
	//
	void Init()  {
		m_pWorld = nullptr;
		m_pProcess = nullptr;
		m_pAllyBar = nullptr;
		m_pEnemyBar = nullptr;
	}
	void Destroy();
protected:
public:
	XSceneBattle( XGame *pGame/*, SceneParamPtr& pParam*/ );
	virtual ~XSceneBattle(void) { Destroy(); }
	//
//	BOOL CreateLegionObj( XLegion *pLegion, BOOL bDestroyLegion );
	void CreateSquadObj( void );
	// virtual
	virtual BOOL OnCreate();
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	int OnEnterScene( XWnd*, DWORD, DWORD ) override;		// 씬이 모두 밝아지고 호출됨
	//
	void OnRecvBattleResult( XGAME::xBattleResult& result );
// 	void OnRecvBattleResult( XGAME::xtSpot typeSpot, bool bWin, int addScore,
//                             XArrayLinearN<ItemBox, 256>& aryDrops,
//                             XArrayLinearN<XGAME::xRES_NUM,XGAME::xRES_MAX> *pAryLoot=nullptr );
	int OnOkBattleResult( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnReconOk( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnRecvBattleResultSulfurEncounter( XSpotSulfur *pSpot, const XGAME::xBattleStartInfo& info );
	int OnOkBattleResultSulfurEncounter( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnEndSceneProcess( XSceneProcess *pProcess );
	int OnDebugRetry( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnDebugRecreate( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnDebugProfile( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnUseSkill( XWnd* pWnd, DWORD p1, DWORD p2 );
	void OnSelectSquad( const SquadPtr& squadSelect );
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
	int OnClickStatistic( XWnd* pWnd, DWORD p1, DWORD p2 );
	void SendFinishBattle( XGAME::xtExitBattle ebCode, XGAME::xtSide bitWinner, int idxStgae, bool bRetreatSulfur );
	XE::VEC2 OnDelegateProjection2( const XE::VEC3& vPos );
	bool OnDelegateIsInScreen( const XE::VEC2& vScr ) override;
	float OnDelegateGetScaleViewport() override;
	void OnReload() override;
	void CreateParticleSfx();
	void OnControlSquad( XHero *pHero );
//	int OnClickOkByTutorialPopup( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnEndCutScene( const std::string& idsEndSeq ) override;
	bool IsDialogMsg( DWORD idSeqPopup );
	void DoMoveCamera( const XE::VEC2& vwDstCenter, float secMove = 0.f);
	XE::VEC2 GetvwCamera();
	void OnCreateOrderDialog( ID idHero ) override;
	XHero* GetpHero( ID idHero ) override;
	void OnDieSquad( XSquadObj* pSquadObj );
#ifdef _XSINGLE
	static void sSetAbilHeroes();
	static void sSetAbilHero( XHero *pHero, XGAME::xtUnit unit, LPCTSTR idsAbil, int point );
#endif // _XSINGLE

private:
	void OnEnterBattle();
	void CreateProcessReady();
	int OnClickPlay( XWnd* pWnd, DWORD p1, DWORD p2 );
friend class XSceneProcessBattle;
friend class XSceneProcessReady;
};

extern XSceneBattle *SCENE_BATTLE;

namespace XGAME {
	extern XAccount *x_pAccEnemy;	// 임시. 
};

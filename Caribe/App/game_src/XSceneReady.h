/********************************************************************
	@date:	2015/02/23 14:38
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XSceneReady.h
	@author:	xuzhu
	
	@brief:	인게임 전투배치씬
*********************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XSceneBattle.h"

class XGame;
class XWndSquadInLegion;
class XWndSelectHeroesInReady;
class XSceneReady : public XSceneBase
{
	XLayoutObj m_Layout;
private:
  CTimer m_timerLimit;
	bool m_bEditMode = false;
	std::shared_ptr<XGAME::xSceneBattleParam> m_spParam;
	//
	void Init()  {
	}
	void Destroy();
protected:
public:
	XSceneReady( XGame *pGame, XSPSceneParam& spBaseParam );
	virtual ~XSceneReady(void) { Destroy(); }
	//
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	void Release() override;
	int OnClickStart( XWnd* pWnd, DWORD p1, DWORD p2 );
	void Update() override;
	int OnClickMySquad( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickEnemySquad( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnDropSquad( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickCancel( XWnd* pWnd, DWORD p1, DWORD p2 );
	void MoveSquadInLegion( int idxSrc, int idxDst, ID snHeroSrc, ID snHeroDst );
	XWndSquadInLegion* GetWndSquadSlot( int idxSlot );
	void RecvBattleStart();
	int OnClickTutorial( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnEnterScene( XWnd*, DWORD, DWORD );
	void OnAutoUpdate() override;
	int OnClickEdit( XWnd* pWnd, DWORD p1, DWORD p2 );
private:
	void UpdateMyLegion();
	XWndSelectHeroesInReady* UpdateWndSelectHeroes();
};

extern XSceneReady *SCENE_READY;


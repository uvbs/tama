/********************************************************************
	@date:	2014/10/28 16:08
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XSceneLegion.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"

class XGame;
class XWndInvenHeroElem;
class XHero;
class XLegion;
class XWndSquadInLegion;
class XSceneLegion : public XSceneBase
{
	XLayoutObj m_Layout;
	XWndInvenHeroElem *m_pSelHero;
	XWndList *m_pHeroList;
	XLegion* m_pLegion;
private:
	//
	void Init()  {
		m_pSelHero = nullptr;
		m_pHeroList = nullptr;
		m_pLegion = nullptr;
	}
	void Destroy();
protected:
public:
	XSceneLegion( XGame *pGame );
	virtual ~XSceneLegion(void) { Destroy(); }
	//
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	int OnBack( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickSquad( XWnd* pWnd, DWORD p1, DWORD p2 );
	int OnClickPopupClose(XWnd *pWnd, DWORD p1, DWORD p2); //팝업 종료
	int OnClickSetLeader(XWnd *pWnd, DWORD p1, DWORD p2); //군단장 지정
	int OnClickDecide(XWnd* pWnd, DWORD p1, DWORD p2);
	int OnDropSquad(XWnd* pWnd, DWORD p1, DWORD p2);
	void CreateSquadToLegion( XSPHero pHero, XLegion *pLegion, int idxSlot );
	XWndSquadInLegion* GetWndSquadSlot( int idxSlot );
	void MoveSquadInLegion( int idxSrc, int idxDst, ID snHeroSrc, ID snHeroDst );
	BOOL IsAbleAddHero( void );
	void Update() override;
	void CopyLegionObj();
	void RecvChangeSquad(int success = 1);
	int OnClickLegionInfo( XWnd* pWnd, DWORD p1, DWORD p2 );
};

extern XSceneLegion *SCENE_LEGION;


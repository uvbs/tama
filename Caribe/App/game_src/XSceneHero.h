/********************************************************************
	@date:	2014/10/28 11:56
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XSceneHero.h
	@author:	xuzhu
	
	@brief:	영웅 소환씬
*********************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"

class XGame;

// 영웅고용 씬
class XSceneHero : public XSceneBase
{
	XLayoutObj m_Layout;
private:
	// 소환시 사용되는 임시 변수들
	bool _m_bPiece = false;		
	ID _m_idHero = 0;			// 
	ID _m_snHero = 0;
	int _m_numPiece = 0;
	XGAME::xtGatha _m_typeGatha =XGAME::xGA_NONE;
	void Init()  { }
	void Destroy();

protected:

public:
	XSceneHero( XGame *pGame );
	virtual ~XSceneHero(void) { Destroy(); }

	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );

	int OnBack(XWnd* pWnd, DWORD p1, DWORD p2);

	int OnSummonLowTemp(XWnd* pWnd, DWORD p1, DWORD p2);		// 일반 영웅 소환 버튼
	int OnSummonHighTemp(XWnd* pWnd, DWORD p1, DWORD p2);		// 고급 영웅 소환 버튼

	int OnSummonPopupOK(XWnd* pWnd, DWORD p1, DWORD p2);		// 일반 영웅 소환 확인 버튼

	void OnRecvSummon( bool bPiece, ID idPropHero, ID snHero, XGAME::xtGatha typeSummon, int num);			// 영웅 소환 받기
	int OnSummonFinish(XWnd* pWnd, DWORD p1, DWORD p2);		// 영웅 소환 완료
	int OnClose(XWnd* pWnd, DWORD p1, DWORD p2);
	int OnSummonHeroAnimation(XWnd* pWnd, DWORD p1, DWORD p2);
	void Update() override;
};

extern XSceneHero *SCENE_HERO;
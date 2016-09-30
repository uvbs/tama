/********************************************************************
	@date:	2016/10/01 0:15
	@file: 	C:\p4v\iPhone_zero\Caribe\App\game_src\XScenePrivateRaid.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XSceneBattle.h"
#include "XFramework/client/XLayoutObj.h"

XE_NAMESPACE_START( XGAME )
//
struct xPrivateRaidParam : public xSceneBattleParam {

};


//
XE_NAMESPACE_END; // XGAME


class XGame;
class XScenePrivateRaid : public XSceneBattle
{
public:
	XScenePrivateRaid* sGet();
public:
	XScenePrivateRaid( XGame *pGame, XSPSceneParam& spParam );
	virtual ~XScenePrivateRaid() { Destroy(); }
	//
	// virtual
	void Create() override;
	int Process( float dt ) override;
	void Draw() override;
	void OnLButtonDown( float lx, float ly ) override;
	void OnLButtonUp( float lx, float ly ) override;
	void OnMouseMove( float lx, float ly ) override;
	void Update() override;
protected:
private:
	static XScenePrivateRaid* s_pSingleton;
	XLayoutObj m_Layout;
	XSPSceneParam m_spParamForNext;		// 다음 연결될 씬을 위해 받아둠.
private:
	void Init()  {}
	void Destroy();
};


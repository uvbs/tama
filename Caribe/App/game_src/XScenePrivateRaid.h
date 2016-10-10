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
/** //////////////////////////////////////////////////////////////////
@brief
*/
struct xPrivateRaidParam : public xSceneBattleParam {
	XVector<XList4<XSPHero>> m_aryEnter;		// 출전영웅 리스트
	xPrivateRaidParam( ID idEnemy,
										 xtSpot typeSpot,
										 ID idSpot,
										 int level,
										 const _tstring& strName,
										 const XVector<XSPLegion>& aryLegion )
		: xSceneBattleParam( idEnemy, typeSpot, idSpot, level, strName, aryLegion,
												 XGAME::xBT_PRIVATE_RAID, 0, -1, 0 )
		, m_aryEnter( 2 ) {}
};
//
XE_NAMESPACE_END; // XGAME


class XGame;
class XScenePrivateRaid : public XSceneBattle
{
public:
	XScenePrivateRaid* sGet();
	static std::shared_ptr<XGAME::xPrivateRaidParam> sSetPrivateRaidParam();
public:
	XScenePrivateRaid( XGame *pGame, XSPSceneParam spParam );
	~XScenePrivateRaid() { Destroy(); }
	//
	// virtual
// 	void Create() override;
// 	int Process( float dt ) override;
// 	void Draw() override;
// 	void OnLButtonDown( float lx, float ly ) override;
// 	void OnLButtonUp( float lx, float ly ) override;
// 	void OnMouseMove( float lx, float ly ) override;
	void Update() override;
	int GetsecTimeOver() override;
protected:
private:
	static XScenePrivateRaid* s_pSingleton;
	XLayoutObj m_Layout;
	XSPSceneParam m_spParamForNext;		// 다음 연결될 씬을 위해 받아둠.
private:
	void Init()  {}
	void Destroy();
	void OnDieSquad( XSPSquadObj spSquadObj );
	void OnDieSquadPrivateRaid( XSPSquadObj spSquadObj );
	void GetSquadObjToAry( int idxSide, XSPLegionObjConst spLegionObj, XVector<XSPSquadObjConst>* pOut ) override;
//	void OnCreateFaces( XSPLegionObj spLegionObj, int idxSide, XWnd* pWndLayer ) override;
};


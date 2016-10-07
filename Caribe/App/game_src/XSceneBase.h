/********************************************************************
	@date:	2015/06/26 16:16
	@file: 	C:\xuzhu_work\Project\iPhone_may\Caribe\App\game_src\XSceneBase.h
	@author:	xuzhu
	
	@brief:	카리브씬의 베이스
*********************************************************************/
#pragma once
#include "XFramework/XEBaseScene.h"
#ifdef _xIN_TOOL
#include "XTool.h"
#endif // _xIN_TOOL
#include "constGame.h"
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/26 16:16
*****************************************************************/
namespace XGAME {
enum xtParamSync : int;
};
class XGame;
class XGameWndAlert;
class XSceneBase : public XEBaseScene
#ifdef _xIN_TOOL
									, public XDelegateTool
#endif // _xIN_TOOL
{
public:
	XSceneBase( XGame *pGame, XGAME::xtScene typeScene );
	virtual ~XSceneBase() { Destroy(); }
	// get/setter
	// public member
	virtual int OnEnterScene( XWnd*, DWORD, DWORD ) override;
	/// 컷씬이 종료된 후 서버로 가서 승인받고 다시 클라로 내려와서 xAE_END_SEQ이벤트로 컷씬을 다시 찾을때 더이상 컷씬이 없으면 호출됨.
	virtual int OnEndCutScene( const std::string& idsEndSeq ) { return 0; }
	XGameWndAlert* WndAlert( LPCTSTR format, ... );
	XGameWndAlert* WndAlertOkCancel( const char *csid, LPCTSTR format, ... );
	XGameWndAlert* WndAlertParam( LPCTSTR szTitle, XWnd::xtAlert type, XCOLOR col, LPCTSTR format, ... );
	XGameWndAlert* WndAlertYesNo( const char *ids, LPCTSTR format, ... );
	XGameWndAlert* WndAlertWithType( const char *ids, XWnd::xtAlert type, LPCTSTR format, ... );
	virtual void OnCloseDialog( const std::string& idsWnd ) {}
	virtual void OnCreateOrderDialog( ID idHero ) {}
	/**
	 @brief 씬에게 idHero의 XHero객체가 있는지 요청이 들어온다면 각 씬의 구조에 맞춰 XHero객체를 찾아서 돌려준다.
	*/
	virtual XSPHero GetpHero( ID idHero ) { return nullptr; }
	virtual void OnRecvSyncAcc( XGAME::xtParamSync type ) {}
// private member
private:
// private method
private:
	void Init() {}
	void Destroy() { }
}; // class XSceneBase

XE_NAMESPACE_START( XGAME )
XSceneBase* xGetpScene();
XE_NAMESPACE_END;

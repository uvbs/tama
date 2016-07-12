#pragma once

#include "_Wnd2/XWnd.h"

XE_NAMESPACE_START( XGAME )
//
struct xSPM_BASE {
	ID idParam;
	std::string m_strParam;
	virtual ~xSPM_BASE() {
		idParam = 0;
	}
};
//
struct xSceneInfo {
	ID m_idScene = 0;
//	std::string m_ids;
//	bool m_bDeferDestroyPrevScene = false;		// this를 호출하는 이전씬은 this의 로딩이 끝나기 전까지 페이드 아웃을 보류한다.
	ID m_idNeedBridgePrev = 0;			// 이 씬을 위해 앞에서 연결씬(브릿지/로딩씬)이 필요하다면 그 씬의 아이디를.
	xSceneInfo() {}
	xSceneInfo( ID idScene, ID idNeedBridgePrev ) {
		m_idScene = idScene;
		m_idNeedBridgePrev = idNeedBridgePrev;
	}
};
XE_NAMESPACE_END; // XGAME


class XEBaseScene;
class XESceneMng;
class XEContent;

typedef std::shared_ptr<XGAME::xSPM_BASE>	SceneParamPtr;

////////////////////////////////////////////////////////////////
class XEDelegateSceneMng
{
	void Init() {}
	void Destroy() {}
public:
	XEDelegateSceneMng() { Init(); }
	virtual ~XEDelegateSceneMng() { Destroy(); }
	//
	virtual XEBaseScene* DelegateCreateScene( XESceneMng *pSceneMng, ID idScene, SceneParamPtr& spParam ) = 0;
	virtual void DelegateOnDestroy( XEBaseScene *pScene ) {}
	virtual void DelegateOnDestroyAfter( ID idSceneDestroy, ID idSceneNext, SceneParamPtr spParam ) {}
};
////////////////////////////////////////////////////////////////
class XESceneMng : public XWnd
{
public:
private:
	XEContent *m_pGame;
	XEDelegateSceneMng *m_pDelegate;
	XEBaseScene *m_pScene;
	ID m_idNextScene;		// 다음에 넘어가야할 씬
	ID m_idLastScene;		// 이전에 파괴된 씬
	SceneParamPtr m_spParam;		// 씬간에 전달되는 파라메터
	XVector<XGAME::xSceneInfo> m_arySceneInfos;		// 각 씬들의 정보.
	void Init() {
		m_pGame = NULL;
		m_pDelegate = NULL;
		m_pScene = NULL;
		m_idNextScene = 0;
		m_idLastScene = 0;
//		m_pParam = NULL;
	}
	void Destroy();
public:
	XESceneMng( XEContent *pGame, XEDelegateSceneMng *pDelegate );
	virtual ~XESceneMng() { Destroy(); }
	//
	GET_SET_ACCESSOR( ID, idNextScene );
	GET_ACCESSOR( ID, idLastScene );
	GET_SET_ACCESSOR( XEBaseScene*, pScene );
	GET_SET_ACCESSOR( SceneParamPtr&, spParam );
	void OnCheatMode( void );
	void AddSceneInfo( const XGAME::xSceneInfo& info ) {
		m_arySceneInfos.Add( info );
	}
	void AddSceneInfo( ID idScene, ID idNeedBridgePrev ) {
		const XGAME::xSceneInfo info( idScene, idNeedBridgePrev );
		m_arySceneInfos.Add( info );
	}
	const XGAME::xSceneInfo GetSceneInfo( ID idScene );
	int Process( float dt ) override;
	void Release() override;
	XEBaseScene* CreatePreNextScene( ID idNextScene, SceneParamPtr& spParam );
	void OnDestroyBridge( XEBaseScene *pScenePreCreated );
	void DestroyCurrScene();
};

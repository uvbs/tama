/****************************************************************
* @brief XSceneLoading class
* @author xuzhu
* @date	2016/01/22 19:57
*****************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"

class XGame;
class XSceneLoading : public XSceneBase
{
public:
	XSceneLoading* sGet();
public:
	XSceneLoading( XGame *pGame, SceneParamPtr& spParam );
	virtual ~XSceneLoading() { Destroy(); }
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
	static XSceneLoading* s_pSingleton;
	XLayoutObj m_Layout;
	int m_cntProcess = 0;
	SceneParamPtr m_spParamForNext;		// 다음 연결될 씬을 위해 받아둠.
	XEBaseScene *m_pSceneNextPreCreated = nullptr;		// 브릿지씬을 위해 미리생성시켜둔 브릿지 다음씬.
private:
	void Init()  {}
	void Destroy();
	// // 씬 파괴되기 직전
	void OnDestroySceneBefore() override;	
};


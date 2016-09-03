/****************************************************************
* @brief XSceneSample class
* @author xuzhu
* @date	2016/01/22 19:57
*****************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayoutObj.h"

class XGame;
class XSceneTest : public XSceneBase
{
public:
	XSceneTest* sGet();
public:
	XSceneTest( XGame *pGame, SceneParamPtr& spParam );
	virtual ~XSceneTest() { Destroy(); }
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
	static XSceneTest* s_pSingleton;
	XLayoutObj m_Layout;
	SceneParamPtr m_spParamForNext;		// 다음 연결될 씬을 위해 받아둠.
private:
	void Init()  {}
	void Destroy();
};


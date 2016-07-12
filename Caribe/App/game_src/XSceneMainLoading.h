/****************************************************************
* @brief XSceneMainLoading class
* @author xuzhu
* @date	2016/01/22 19:57
*****************************************************************/
#pragma once
#if 0
#include "XSceneBase.h"
#include "client/XLayout.h"

class XGame;
class XSceneMainLoading : public XSceneBase
{
public:
	XSceneMainLoading* sGet();
	static bool sIsLoaded() { return s_bLoaded; }
public:
	XSceneMainLoading( XGame *pGame );
	virtual ~XSceneMainLoading() { Destroy(); }
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
	static XSceneMainLoading* s_pSingleton;
	static bool s_bLoaded;
	XLayoutObj m_Layout;
	SceneParamPtr m_spParamForNext;		// 다음 연결될 씬을 위해 받아둠.
	int m_cntProcess = 0;
private:
	void Init()  {}
	void Destroy();
};

#endif // 0
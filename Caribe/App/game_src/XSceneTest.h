/****************************************************************
* @brief XSceneSample class
* @author xuzhu
* @date	2016/01/22 19:57
*****************************************************************/
#pragma once
#ifdef _XTEST
#include "XSceneBase.h"
#include "XFramework/client/XLayoutObj.h"
#include "OpenGL2/XGraphicsOpenGL.h"

#define MAX_SPR1  1
#define MAX_SPR2  1

class XSprObj;
class XWndStoragyItemElem;
namespace xSplit {
class XNode;
}
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
//	void ReplaceTexture( DWORD* pAtlas, const XE::xRECT& rect, int wMax );
	void OnRButtonUp( float lx, float ly ) override;
	void Update() override;
protected:
private:
	static XSceneTest* s_pSingleton;
	XLayoutObj m_Layout;
	SceneParamPtr m_spParamForNext;		// 다음 연결될 씬을 위해 받아둠.
//	DWORD* m_pAtlas = nullptr;
	GLuint m_glTexture = 0;
	xSplit::XNode* m_pRoot = nullptr;
	ID m_idCurr = 0;
	XSprObj* m_psoTest[MAX_SPR1 * MAX_SPR2];
	XVector<XWndStoragyItemElem*> m_aryCtrl;
private:
	void Init()  {}
	void Destroy();
};

#endif // _XTEST

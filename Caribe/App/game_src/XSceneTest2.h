/****************************************************************
* @brief XSceneSample class
* @author xuzhu
* @date	2016/01/22 19:57
*****************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayoutObj.h"
#include "OpenGL2/XGraphicsOpenGL.h"

#define MAX_SPR1  1
#define MAX_SPR2  1

class XSprObj;
namespace xSplit {
class XNode;
}
class XGame;
class XSceneTest2 : public XSceneBase
{
public:
	XSceneTest2* sGet();
public:
	XSceneTest2( XGame *pGame, XSPSceneParam& spParam );
	virtual ~XSceneTest2() { Destroy(); }
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
	static XSceneTest2* s_pSingleton;
	XLayoutObj m_Layout;
	XSPSceneParam m_spParamForNext;		// 다음 연결될 씬을 위해 받아둠.
	DWORD* m_pAtlas = nullptr;
	GLuint m_glTexture = 0;
	xSplit::XNode* m_pRoot = nullptr;
	ID m_idCurr = 0;
private:
	void Init()  {}
	void Destroy();
};


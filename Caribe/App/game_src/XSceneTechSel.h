/********************************************************************
	@date:	2014/12/10 16:44
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XSceneTechSel.h
	@author:	
	
	@brief:	기술연구 유닛선택씬
*********************************************************************/
#pragma once
#include "XSceneBase.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"

class XGame;
class XSceneTechSel : public XSceneBase
{
	XLayoutObj m_Layout;
private:
	//
	void Init()  {
	}
	void Destroy();
protected:
public:
	XSceneTechSel( XGame *pGame );
	virtual ~XSceneTechSel(void) { Destroy(); }
	//
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	int OnBack( XWnd *pWnd, DWORD p1, DWORD p2 );
};

extern XSceneTechSel *SCENE_TECH_SEL;


#pragma once
#include "XEBaseScene.h"
//#include "XAppLayout.h"

class XGame;
class XSceneTitle : public XEBaseScene
{
	//XLayoutObj m_Layout;
private:
	//
	void Init()  {
	}
	void Destroy();
protected:
public:
	XSceneTitle( XGame *pGame );
	virtual ~XSceneTitle(void) { Destroy(); }
	//
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
};

extern XSceneTitle *SCENE_WORLD;


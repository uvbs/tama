#pragma once
#include "XEBaseScene.h"
//#include "XAppLayout.h"

class XGame;
class XSceneWorld : public XEBaseScene
{
	//XLayoutObj m_Layout;
private:
	//
	void Init()  {
	}
	void Destroy();
protected:
public:
	XSceneWorld( XGame *pGame );
	virtual ~XSceneWorld(void) { Destroy(); }
	//
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
};

extern XSceneWorld *SCENE_WORLD;


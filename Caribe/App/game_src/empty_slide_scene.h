/********************************************************************
	@date:	
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XESceneSlide.h"
//#include "XLayout.h"

class XGame;
class XSceneSample : public XESceneSlide
{
	//XLayoutObj m_Layout;
private:
	//
	void Init()  {
	}
	void Destroy();
protected:
public:
	XSceneSample( XGame *pGame );
	virtual ~XSceneSample(void) { Destroy(); }
	//
	XWnd* CreateScene( int idx, DWORD p1, DWORD p2 );
	// virtual
	virtual void Create( void );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
};

extern XSceneSample *SCENE_SAMPLE;


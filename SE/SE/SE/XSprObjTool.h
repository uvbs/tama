#pragma once
#include "sprobj.h"
//#include "XObjAct.h"

// class XObjActTool;
// class XObjActTool : public XObjAct
// {
// 	void Init() {
// 	}
// 	void Destroy() {}
// public:
// 	XObjActTool() { Init(); }
// 	XObjActTool( SPAction spAction ) : XObjAct( spAction ) { Init(); }
// 	virtual ~XObjActTool() { Destroy(); }
// 	// virtual
// 	virtual void DrawLayerLabel( float left, float top, SPBaseLayer spPushLayer = nullptr );
// 	virtual XLayerDummy *AddDummyLayer();
// };

class XSprObjTool :	public XSprObj
{
	void Init() {
	}
	void Destroy() {}
public:
	XSprObjTool() { Init(); }
	XSprObjTool( ID idSprObj ) : XSprObj( idSprObj ) { Init(); }
	virtual ~XSprObjTool() { Destroy(); }
	//
	XBaseKey* GetInsideKey( const XE::VEC2& vPos, float fViewScale ) const;
	//
//	virtual XObjAct* CreateObjAct( SPAction spAction ) { return new XObjActTool( spAction ); }
};


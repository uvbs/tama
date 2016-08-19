#pragma once
#include "XLayerBase.h"

////////////////////////////////////////////////////
class XLayerEvent : public XBaseLayer
{
	void _Init() {}
	void _Destroy() {}
public:
	static xSpr::xtLayer sGetType() {
		return xSpr::xLT_EVENT;
	}
	XLayerEvent() {
		_Init();
		SetbitType( xSpr::xLT_EVENT );
		SetstrLabel( CString( _T( "Event" ) ) );
		SetbAbleOpen( FALSE );
	}
	virtual ~XLayerEvent() { _Destroy(); }
//	virtual XSPBaseLayer CopyDeep() override;
	//
	XSPBaseLayer CreateCopy( XSPActObjConst spActObjSrc ) const override;
private:
	XSPLayerEvent GetThis() {
		return std::static_pointer_cast<XLayerEvent>( XBaseLayer::GetThis() );
	}
	void SetNewInstanceInSameAction( ID idLayer, int nLayer ) override {};
	void SetNewInstanceInOtherAction() override {};
};


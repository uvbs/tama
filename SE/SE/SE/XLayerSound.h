#pragma once
#include "XLayerBase.h"

class XLayerSound : public XBaseLayer
{
	void Init() {
	}
	void Destroy() {}
public:
	static xSpr::xtLayer sGetType() {
		return xSpr::xLT_SOUND;
	}
	XLayerSound() {
		Init();
		SetbitType( xSpr::xLT_SOUND );
		SetstrLabel( CString( _T( "Play Sound" ) ) );
		SetbAbleOpen( FALSE );
	}
	virtual ~XLayerSound() {}
	virtual void DrawLabel( float left, float top, XCOLOR colFont = XCOLOR_WHITE );
//	virtual XSPBaseLayer CopyDeep() override;
	virtual XBaseKey* DoCreateKeyDialog( XSPAction spAction, float fFrame, xSpr::xtLayerSub subType ) override;
	//
	XSPBaseLayer CreateCopy( XSPActObjConst spActObjSrc ) const override;
private:
	XSPLayerSound GetThis() {
		return std::static_pointer_cast<XLayerSound>( XBaseLayer::GetThis() );
	}
	void SetNewInstanceInSameAction( ID idLayer, int nLayer ) override {};
	void SetNewInstanceInOtherAction() override {};
};

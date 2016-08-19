#pragma once
#include "XLayerMove.h"

class XMILayerCrossDir;
////////////////////////////////////////////////////
// 다용도 더미 레이어
class XLayerDummy;
class XLayerDummy : public XLayerMove
{
public:
	static xSpr::xtLayer sGetType() {
		return xSpr::xLT_DUMMY;
	}
private:
	BOOL m_bActive;
	XMILayerCrossDir *m_pMICrossDir;		// 이것도 XLayerDummyTool을 상속받아 써야할듯
	void Init() {
		m_bActive = FALSE;
		m_pMICrossDir = NULL;
	}
	void Destroy();
public:
	XLayerDummy();
	XLayerDummy( BOOL bCreateMI )
		: XLayerMove( xSpr::xLT_DUMMY, CString( _T( "Dummy" ) ), TRUE ) {		// MI를만들지 않는 버전
		Init();
	}
	XLayerDummy( const XLayerDummy& rhs );
	virtual ~XLayerDummy() { Destroy(); }
	//
	bool OnCreate() override;
	XSPBaseLayer CreateCopy( XSPActObjConst spActObjSrc ) const override;
	// get/set
	GET_SET_ACCESSOR( BOOL, bActive );
	GET_ACCESSOR( XMILayerCrossDir*, pMICrossDir );
	//
	void ClearMICrossDir();
	void RegisterMI();		// AnimationView에 십자선 인터페이스를 등록한다
	void UnRegisterMI();		// AnimationView에 십자선 인터페이스를 등록해제한다
	//
	void Draw( float x, float y, const D3DXMATRIX &m, XSprObj *pParentSprObj ) override;
//	XSPBaseLayer CopyDeep() override;
	void OnSelected( int nEvent ) override {
		if( nEvent )
			RegisterMI();
		else
			UnRegisterMI();
	}
	XBaseKey* DoCreateKeyDialog( XSPAction spAction, float fFrame, xSpr::xtLayerSub subType ) override;
// 	XSPBaseLayer CreateCopyTest( std::shared_ptr<const XLayerDummy> spLayerSrc ) const;
// 	XSPBaseLayer CreateCopyTest2( std::shared_ptr<const XLayerDummy> spLayerSrc ) const;
private:
	XSPLayerDummy GetThis() {
		return std::static_pointer_cast<XLayerDummy>( XBaseLayer::GetThis() );
	}
	void SetNewInstanceInSameAction( ID idLayer, int nLayer ) override;
	void SetNewInstanceInOtherAction() override;
};
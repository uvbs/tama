#pragma once
#include "XLayerMove.h"

class XSprite;
class XLayerImage : public XLayerMove
{
	XSprite *m_pSpriteCurr;
	void Init() {
		m_pSpriteCurr = NULL;
	}
public:
	static xSpr::xtLayer sGetType() {
		return xSpr::xLT_IMAGE;
	}
	XLayerImage() {
		Init();
		SetbitType( xSpr::xLT_IMAGE );
		SetstrLabel( CString( _T( "Image" ) ) );
		SetbAbleOpen( TRUE );
	}
	virtual ~XLayerImage() {}
	// 
	XSPBaseLayer CreateCopy( XSPActObjConst spActObjSrc ) const override;
	GET_SET_ACCESSOR( XSprite*, pSpriteCurr );
//	void UpdateBoundBox( XSprObj *pSprObj );
	void Clear() override { 
		XLayerMove::Clear(); Init(); 
	}
	void Draw( float x, float y, const D3DXMATRIX &m, XSprObj *pParentSprObj ) override;
	void FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj ) override;
	DWORD GetPixel( float cx, float cy,
									float mx, float my,
									const D3DXMATRIX &m,
									BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL ) const override;
	DWORD GetPixel( const D3DXMATRIX &mCamera, const D3DXVECTOR2& mv, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const;
//	virtual XSPBaseLayer CopyDeep() override;
//	void CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj ) override;
	XE::xRECT GetBoundBox( const D3DXMATRIX& mParent ) const override;
private:
	xSPLayerImage GetThis() {
		return std::static_pointer_cast<XLayerImage>( XBaseLayer::GetThis() );
	}
	void SetNewInstanceInSameAction( ID idLayer, int nLayer ) override {};
	void SetNewInstanceInOtherAction() override {};
};
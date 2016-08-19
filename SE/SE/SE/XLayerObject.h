#pragma once
#include "XLayerMove.h"

class XSprObj;
////////////////////////////////////////////////////
class XLayerObject : public XLayerMove
{
	XSprObj *m_pSprObjCurr;
	void Init() {
		m_pSprObjCurr = NULL;
	}
	float m_maxFrame = 0;		// m_pSprObjCurr의 maxFrame
public:
	static xSpr::xtLayer sGetType() {
		return xSpr::xLT_OBJECT;
	}
	XLayerObject() {
		Init();
		SetbitType( xSpr::xLT_OBJECT );
		SetstrLabel( CString( _T( "CreateObj" ) ) );
		SetbAbleOpen( TRUE );
	}
	virtual ~XLayerObject() { }
	//
	XSPBaseLayer CreateCopy( XSPActObjConst spActObjSrc ) const override;
	//
	GET_ACCESSOR( XSprObj*, pSprObjCurr );
	GET_ACCESSOR_CONST( float, maxFrame );
	void SetpSprObjCurr( XSprObj* pSprObj );
	void UpdateBoundBox( XSprObj *pSprObj );
	void Clear() override { XLayerMove::Clear(); Init(); }
	void FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj ) override;
	void MoveFrame( float fFrmCurr, XSPActObjConst spActObj ) override;
	void Draw( float x, float y, const D3DXMATRIX &m, XSprObj *pParentSprObj ) override;
	DWORD GetPixel( float cx, float cy,
													float mx, float my,
													const D3DXMATRIX &m,
													BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL ) const override;
	DWORD GetPixel( const D3DXMATRIX &mCamera, const D3DXVECTOR2& mv, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL ) const override;
//	XSPBaseLayer CopyDeep() override;
//	void CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj ) override;
	XE::xRECT GetBoundBox( const D3DXMATRIX& mParent ) const override;
private:
	XSPLayerObject GetThis() {
		return std::static_pointer_cast<XLayerObject>( XBaseLayer::GetThis() );
	}
	void SetNewInstanceInSameAction( ID idLayer, int nLayer ) override {};
	void SetNewInstanceInOtherAction() override {};
};

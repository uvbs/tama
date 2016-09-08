#pragma once
#include "Sprdef.h"
#include "etc/xMath.h"

class XArchive;
class XSprObj;
class XBaseLayer;
class XActDat;

struct LAYER_INFO;
struct XEFFECT_PARAM;

//////////////////////////////////////////////////////////////////////////
// XSprObj에서 쓰는 액션정보
class XActObj {
	XSprObj *m_pSprObj;
	int m_nNumLayers;
	XBaseLayer **m_ppLayers;		// 레이어 리스트
	XActDat *m_pAction;			// 이 노드가 가리키는 SprDat에서의 액션 포인터. read only
	void Init() {
		m_pAction = NULL;
		m_nNumLayers = 0;
		m_ppLayers = NULL;
		m_pSprObj = NULL;
	}
	void Destroy();
	void DestroyLayer();
public:
	XActObj( XSprObj *pSprObj ) {
		Init(); m_pSprObj = pSprObj;
	}
	XActObj( XSprObj *pSprObj, XActDat *pAction );
	~XActObj() {
		Destroy();
	}
	// get/set
	GET_SET_ACCESSOR_CONST( XActDat*, pAction );
	GET_ACCESSOR_CONST( int, nNumLayers );

	DWORD GetPixel( float cx, float cy, float mx, float my, const MATRIX& m, BYTE *pa, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL );
	XBaseLayer *GetLayerInPixel( float lx, float ly );
	// etc
	void FrameMove( float dt, float fFrmCurr );
#ifdef _VER_OPENGL
	void Draw( float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam );
#else
	void Draw( float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam );
#endif
	int Serialize( XArchive& ar );
	int DeSerialize( XArchive& ar );
	XBaseLayer *GetLayer( xSpr::xtLayer type, int nLayer );
	XBaseLayer *CreateLayer( int idx, xSpr::xtLayer type = xSpr::xLT_NONE, int nLayer = -1, float fAdjAxisX = 0, float fAdjAxisY = 0 );
	XBaseLayer* GetpLayerByidLocalInLayer( ID idLocalInLayer ) const;
	XBaseLayer *CreateLayer( int idx, LAYER_INFO* pLayerInfo );
	void ClearLayer();
private:
};
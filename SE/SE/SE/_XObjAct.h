#pragma once

#if 0
//#include "Key.h"
#include "Layer.h"
//#include "SprDat.h"
#include "Sprdef.h"
//#include "XLuaSprObj.h"
// 이 액션에서 로드될 XSprDat들은 미리 모두 로드된다
// XSprObj에서 쓰는 액션정보
class XObjAct 
{
	XBaseLayer_Itor m_itorLayer;
	std::list<SPBaseLayer> m_listLayer;		// 레이어 리스트
	int m_nMaxLayer[ XBaseLayer::xMAX_LAYER ];		// 툴에서만 사용. 레이어를 새로 추가할때 자동으로 레이어번호를 지정해주기 위한...
	XAniAction *m_pAction;			// 이 노드가 가리키는 SprDat에서의 액션 포인터. read only
	void Init() {
		memset( m_nMaxLayer, 0, sizeof(m_nMaxLayer) );
		m_pAction = nullptr;
	}
	void Destroy();
	XBaseLayer *CreateLayer( LAYER_INFO *pLayerInfo );
	XBaseLayer *CreateLayer( ID idLayer, XBaseLayer::xTYPE type/* = xSpr::xNONE_LAYER*/, int nLayer = -1, float fAdjAxisX = 0, float fAdjAxisY = 0 );
	void DelLayer( SPBaseLayer spLayer, BOOL bDelLayerOnly = FALSE );
public:
	XObjAct() { Init(); }
	XObjAct( SPAction spAction ) { Init(); m_pAction = spAction; }
	~XObjAct() { Destroy(); }
	// get/set
	GET_SET_ACCESSOR( XAniAction*, spAction );
	// layer
	int GetNumLayer() { return m_listLayer.size(); }
	void GetNextLayerClear() { m_itorLayer = m_listLayer.begin(); }
	SPBaseLayer GetNextLayer() {
		if( m_itorLayer == m_listLayer.end() )
			return nullptr;
		return (*m_itorLayer++);
	}
	SPBaseLayer FindLayer( SPBaseLayer spLayer ) {
		return (*find( m_listLayer.begin(), m_listLayer.end(), spLayer ));
	}
	SPBaseLayer FindLayer( XBaseLayer::xTYPE type, int nLayer ) {
		return GetLayer( type, nLayer );
	}
	SPBaseLayer FindLayer( ID idSerial ) {
		return GetLayer( idSerial );
	}
	void InsertLayer( SPBaseLayer spDst, SPBaseLayer spSrc );
	void RenumberLayer();
//	void RemapNextLayer();
	void SortLayer();
	int GenerateMaxLayer( XBaseLayer::xTYPE type ) { 	return m_nMaxLayer[ (int)type ]++;	}
	void DestroyLayer();
	XBaseLayer *CheckPushLayer( float lx, float ly, float h = 0, XBaseLayer::xSubType *pOutSubType=nullptr );
	XBaseLayer *CheckPushLayer( const XE::VEC2& vInKeyWin, float h = 0, XBaseLayer::xSubType *pOutSubType = nullptr ) {
		return CheckPushLayer( vInKeyWin.x, vInKeyWin.y, h, pOutSubType );
	}
	void UpdateLayersPos();
	XBaseLayer *GetLayer( XBaseLayer::xTYPE type, int nLayer );
	XBaseLayer *GetLayer( ID idSerial );
	void ClearLayer();
	void SetMaxLayer();
//protected:
	void AddLayer( SPBaseLayer spNewLayer, BOOL bCalcPriority=TRUE );
//public:
	XBaseLayer *AddImgLayer();
	XBaseLayer *AddObjLayer();
	XBaseLayer *AddSndLayer();
	XBaseLayer *AddEventLayer();
	virtual XLayerDummy *AddDummyLayer();
	SPBaseLayer GetLastLayer();
	SPBaseLayer GetFirstLayer();
	DWORD GetPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb );
	XBaseLayer *GetLayerInPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m );
	XBaseLayer *GetLayerInPixel( const D3DXMATRIX& mCamera, const D3DXVECTOR2& mv );
	void DoDelEmptyLayer();
	void SendLayerEvent_OnSelected( int nEvent );
	void CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj );
	// etc
	void FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj );
	void MoveFrame( float fFrmCurr );
	void Draw( const D3DXMATRIX &m );
	virtual void DrawLayerLabel( float left, float top, SPBaseLayer spPushLayer = nullptr ) { XBREAK(1); }

	friend class XSprObj;
} ;

typedef std::list<XObjAct *>			XObjAct_List;
typedef std::list<XObjAct *>::iterator	XObjAct_Itor;
#endif

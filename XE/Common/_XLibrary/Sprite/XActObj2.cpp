#include "stdafx.h"
#include "XActObj2.h"
#include "XActDat.h"
#include "SprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


#define LAYER_LOOP( K )	int K##i;		\
						XBaseLayer **ppLayers = m_ppLayers;	\
						K##i = m_nNumLayers;		\
						while( K##i-- )			\
						{						\
							XBaseLayer *K = *ppLayers++;	\
							if( K == NULL )	continue;				// 이벤트레이어같은건 널이기때문에 이게 필요하다
#define LAYER_LOOP_REV( K )	int K##i;		\
							XBaseLayer **ppLayers = m_ppLayers + (m_nNumLayers - 1);	\
							K##i = m_nNumLayers;		\
							while( K##i-- )			\
							{						\
								XBaseLayer *K = *ppLayers--;	\
								if( K == NULL )	continue;				// 이벤트레이어같은건 널이기때문에 이게 필요하다
#define LOOP_END		}

///////////////////////////////////////////////////////////////
//
// XObjAct
//
////////////////////////////////////////////////////////////////
XActObj::XActObj( XSprObj *pSprObj, const XActDat *pAction )
	: m_pAction( pAction )
{
	m_pAction = pAction;
	m_pSprObj = pSprObj;
	m_nNumLayers = pAction->GetnNumLayerInfo();
	m_ppLayers = new XBaseLayer*[m_nNumLayers];
	memset( m_ppLayers, 0, sizeof( XBaseLayer* ) * m_nNumLayers );
}
void XActObj::Destroy( void )
{
	// layer 삭제
	DestroyLayer();
}

void XActObj::ClearLayer( void )
{
	LAYER_LOOP( pLayer )
		pLayer->Clear();
	LOOP_END
}
void XActObj::FrameMove( float dt, float fFrmCurr )
{
	LAYER_LOOP( pLayer )
		pLayer->FrameMove( m_pSprObj, dt, fFrmCurr );
	LOOP_END
}
#ifdef _VER_OPENGL
void XActObj::Draw( float x, float y, const MATRIX &m, XEFFECT_PARAM *pEffectParam )
{
	LAYER_LOOP( pLayer )
		pLayer->Draw( m_pSprObj, x, y, m, pEffectParam );
	LOOP_END
}
#else
void XActObj::Draw( float x, float y, const D3DXMATRIX &m, XEFFECT_PARAM *pEffectParam )
{
	LAYER_LOOP( pLayer )
		pLayer->Draw( m_pSprObj, x, y, m, pEffectParam );
	LOOP_END
}
#endif
void XActObj::DestroyLayer( void )
{
	int i;
	for( i = 0; i < m_nNumLayers; i++ )
		SAFE_DELETE( m_ppLayers[i] );
	SAFE_DELETE_ARRAY( m_ppLayers );
}
XBaseLayer *XActObj::CreateLayer( int idx, const LAYER_INFO* pLayerInfo )
{
	auto pLayer = CreateLayer( idx
														 , pLayerInfo->type
														 , pLayerInfo->nLayer
														 , pLayerInfo->fAdjustAxisX
														 , pLayerInfo->fAdjustAxisY );		// 실제 레이어를 만들어준다
	if( pLayer ) {
		XBREAK( pLayerInfo->idLayer == 0 );
		pLayer->SetidLayer( pLayerInfo->idLayer );
	}
	return pLayer;
}
// pLayer가 널이면 레이어를 초기화 상태로 만든다.
XBaseLayer *XActObj::CreateLayer( int idx, xSpr::xtLayer type, int nLayer, float fAdjAxisX, float fAdjAxisY )
{
	const XE::VEC2 vAdjAxis( fAdjAxisX, fAdjAxisY );
#ifdef _XDEBUG
	if( idx >= m_nNumLayers ) {
		XLOG( "%s id=%d ACT %s(%d) idx(%d) >= m_nNumLayers(%d)", m_pSprObj->GetSprFilename(), m_pSprObj->GetdwID(),
					m_pAction->GetszActName(), m_pAction->GetID(), idx, m_nNumLayers );
		return NULL;
	}

#endif

	XBaseLayer *pLayer = nullptr;
	{
		// 이거 나중에 XBaseLayer의스태틱 함수로 집어넣자. 안그러면 레이어종류가 추가될때마다 일일히 이런곳 찾아서 수정해줘야 한다
		switch( type ) {
		case xSpr::xLT_IMAGE:
			pLayer = new XLayerImage;
			((XLayerImage *)pLayer)->SetvAdjAxis( vAdjAxis );
			break;
		case xSpr::xLT_OBJ:
			pLayer = new XLayerObject;
			((XLayerObject *)pLayer)->SetvAdjAxis( vAdjAxis );
			break;
		case xSpr::xLT_SOUND:
			pLayer = new XLayerSound;
			break;
		case xSpr::xLT_EVENT:
			// 이벤트레이어는 게임에서 생성시킬 필요 없다
			break;
		case xSpr::xLT_DUMMY:
			pLayer = new XLayerDummy;
			((XLayerMove *)pLayer)->SetvAdjAxis( vAdjAxis );
			break;
		default:
			pLayer = NULL;
			XERROR( "layer 생성타입이 잘못되었음. %d", (int)type );
			break;
		}
		if( pLayer ) {
			pLayer->SetnLayer( nLayer );								// 지정한 레이어번호로 생성
		}
		m_ppLayers[idx] = pLayer;		// 널이 들어갈수도 있다
	}
	return pLayer;
}
XBaseLayer *XActObj::GetLayer( xSpr::xtLayer type, int nLayer )
{
	LAYER_LOOP( pLayer )
		if( pLayer->GetType() == type && pLayer->GetnLayer() == nLayer )
			return pLayer;

	LOOP_END
		return NULL;
}

// lx, ly는 0,0센터 기준 로컬좌표
DWORD XActObj::GetPixel( float cx, float cy, float mx, float my, const MATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb )
{
	DWORD pixel;
	LAYER_LOOP_REV( pLayer )
	{
		pixel = pLayer->GetPixel( cx, cy, mx, my, m, pa, pr, pg, pb );
		if( *pa > 0 )
			return pixel;			// 투명색이 아니면 픽셀이 있는것으로 간주하고 그 픽셀을 리턴한다
	} LOOP_END;
	return 0;
}

int XActObj::Serialize( XArchive& ar )
{
	LAYER_LOOP( pLayer )
	{
		pLayer->Serialize( ar, m_pSprObj );
	}
	LOOP_END;
	return 1;
}

int XActObj::DeSerialize( XArchive& ar )
{
	LAYER_LOOP( pLayer )
	{
		pLayer->DeSerialize( ar, m_pSprObj );
	}
	LOOP_END;
	return 1;
}

XBaseLayer* XActObj::GetpLayerByidLocalInLayer( ID idLocalInLayer ) const
{
	XBaseLayer **ppLayers = m_ppLayers;
	int num = m_nNumLayers;
	while( num-- ) {
		XBaseLayer *pLayer = *ppLayers++;
		if( pLayer == nullptr )	continue;				// 이벤트레이어같은건 널이기때문에 이게 필요하다
		if( pLayer->GetidLayer() == idLocalInLayer )
			return pLayer;
	}
	return nullptr;
}
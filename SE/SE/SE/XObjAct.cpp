#include "stdafx.h"
#if 0
#include "XObjAct.h"
#include "SprObj.h"
#include "XGraphicsD3DTool.h"
#include "FrameView.h"
#include "AnimationView.h"
#include "SprMng.h"
using namespace XE;

///////////////////////////////////////////////////////////////
//
// XObjAct
//
////////////////////////////////////////////////////////////////
#define LAYER_LOOP( I )				\
								XBaseLayer_Itor I; \
								for( I = m_listLayer.begin(); I != m_listLayer.end(); I ++ ) \
		
#define LAYER_MANUAL_LOOP( I )				\
								XBaseLayer_Itor I; \
								for( I = m_listLayer.begin(); I != m_listLayer.end(); ) 

void XObjAct::Destroy()
{
	// layer 삭제
	DestroyLayer();
}

void XObjAct::ClearLayer()
{
	for( auto& spLayer : m_listLayer )
		(*itor)->Clear();
}
void XObjAct::FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj )
{
	for( auto& spLayer : m_listLayer )
		(*itor)->FrameMove( dt, fFrmCurr, pSprObj );
}
void XObjAct::MoveFrame( float fFrmCurr )
{
	for( auto& spLayer : m_listLayer )
		(*itor)->MoveFrame( fFrmCurr );
}
void XObjAct::Draw( const D3DXMATRIX &m )
{
	LIST_LOOP( m_listLayer, SPBaseLayer, itor, spLayer )
	{
		if( spLayer->GetbShow() )	// show상태에서만 레이어를 그린다
			spLayer->Draw( 0, 0, m );
	} END_LOOP;
	// draw Bound box
/*	D3DXVECTOR2 vbbLT = m_pAction->GetBoundBoxLT();
	D3DXVECTOR2 vbbRB = m_pAction->GetBoundBoxRB();
	D3DXVECTOR2 vtLT, vtRB;
	D3DXVec2TransformCoord( &vtLT, &vbbLT, &m );
	D3DXVec2TransformCoord( &vtRB, &vbbRB, &m );
	XE::VEC2 xvLT( vtLT );
	XE::VEC2 xvRB( vtRB );
	GRAPHICS->DrawRect( xvLT, xvRB, XCOLOR_BLUE );
	*/
}
void XObjAct::DestroyLayer()
{
	LAYER_MANUAL_LOOP( itor )
	{
		SPBaseLayer spLayer = (*itor);
		SAFE_DELETE( spLayer );
		m_listLayer.erase( itor++ );
	}
}
SPBaseLayer XObjAct::CreateLayer( LAYER_INFO *pLayerInfo )
{
	if( pLayerInfo )
		return CreateLayer( pLayerInfo->idLayer, pLayerInfo->type, pLayerInfo->nLayer, pLayerInfo->fAdjustAxisX, pLayerInfo->fAdjustAxisY );
	else
		return CreateLayer( 0, xSpr::xNONE_LAYER );
}

// spLayer가 널이면 레이어를 초기화 상태로 만든다.
XBaseLayer *XObjAct::CreateLayer( ID idLayer, XBaseLayer::xTYPE type, int nLayer, float fAdjAxisX, float fAdjAxisY )
{
	SPBaseLayer spLayer = nullptr, *pPrev = nullptr;
	if( type == xSpr::xNONE_LAYER )
	{	// 기본 레이어들을 생성시켜준다
		spLayer = new XLayerImage;
		spLayer->SetnLayer( GenerateMaxLayer( XBaseLayer::xIMAGE_LAYER ) );
		AddLayer( spLayer );
		LAYER_INFO *pLayerInfo = m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 );
		spLayer->SetidLayer( pLayerInfo->idLayer );
	} else
	{
		// 이거 나중에 XBaseLayer의스태틱 함수로 집어넣자. 안그러면 레이어종류가 추가될때마다 일일히 이런곳 찾아서 수정해줘야 한다
		switch( type )
		{
			case XBaseLayer::xIMAGE_LAYER:
				spLayer = new XLayerImage;
				((XLayerImage *)spLayer)->SetAdjustAxis( fAdjAxisX, fAdjAxisY );
				break;
			case XBaseLayer::xOBJ_LAYER:
				spLayer = new XLayerObject;
				((XLayerObject *)spLayer)->SetAdjustAxis( fAdjAxisX, fAdjAxisY );
				break;
			case XBaseLayer::xSOUND_LAYER:
				spLayer = new XLayerSound;
				break;
			case XBaseLayer::xEVENT_LAYER:
				spLayer = new XLayerEvent;
				break;
			case XBaseLayer::xDUMMY_LAYER:
				spLayer = new XLayerDummy;
				((XLayerMove *)spLayer)->SetAdjustAxis( fAdjAxisX, fAdjAxisY );
				break;
			default:
				spLayer = nullptr;
				XALERT( "XAniAction::CreateLayer 생성타입이 이상함. %d", (int)type );
				break;
		}
		if( nLayer == -1 )
			spLayer->SetnLayer( GenerateMaxLayer( type ) );		// 레이어번호 자동생성
		else
			spLayer->SetnLayer( nLayer );								// 지정한 레이어번호로 생성
		AddLayer( spLayer );
		spLayer->SetidLayer( idLayer );
	}

	return spLayer;
}
void XObjAct::AddLayer( SPBaseLayer spNewLayer, BOOL bCalcPriority/* = TRUE */)	
{
	if( !m_listLayer.empty() ) {
		XBaseLayer_Itor itorEnd = m_listLayer.end();
		SPBaseLayer spLast = ( *--itorEnd );
		if( bCalcPriority )		// 프라이오리티를 건드리지 않는 옵션
			pNewLayer->SetfPriority( pLast->GetfPriority() + 1.0f );
	} else {
		if( bCalcPriority )
			pNewLayer->SetfPriority( 1.0f );
	}
	m_listLayer.push_back( pNewLayer );
}

bool CompLayer( XBaseLayer *layer1, XBaseLayer *layer2 )
{
	if( layer1->GetfPriority() < layer2->GetfPriority() )
		return TRUE;
	return FALSE;
/*	if( layer1->GetType() < layer2->GetType() )
		return TRUE;
	else 
	if( layer1->GetType() == layer2->GetType() )
	{
		return layer1->GetnLayer() < layer2->GetnLayer();
	} else
		return FALSE; */
}
void XObjAct::SortLayer()
{
	m_listLayer.sort( CompLayer );
	// 레이어 번호를 다시 세팅한다
//	RenumberLayer();
	UpdateLayersPos();
}
// 레이어리스트들을순회하며 레이어번호를 새로매긴다
void XObjAct::RenumberLayer()
{
	SPBaseLayer spPrev = nullptr;
	int nImgLayer = 0, nObjLayer = 0, nSndLayer = 0;
	for( auto& spLayer : m_listLayer )
	{
		// 레이어 번호를 다시 세팅한다
		switch( (*itor)->GetType() )
		{
		case XBaseLayer::xIMAGE_LAYER:
			(*itor)->SetnLayer( nImgLayer++ );
			break;
		case XBaseLayer::xOBJ_LAYER:
			(*itor)->SetnLayer( nObjLayer++ );
			break;
		case XBaseLayer::xSOUND_LAYER:
			(*itor)->SetnLayer( nSndLayer++ );
			break;
		}
	}
}
XBaseLayer *XObjAct::GetLayer( XBaseLayer::xTYPE type, int nLayer )
{
	for( auto& spLayer : m_listLayer )
	{
		if( (*itor)->GetType() == type && (*itor)->GetnLayer() == nLayer )
			return (*itor);
	}
	return nullptr;
}
XBaseLayer *XObjAct::GetLayer( ID idSerial )
{
	for( auto& spLayer : m_listLayer )
	{
		if( (*itor)->GetidSerial() == idSerial )
			return (*itor);
	}
	return nullptr;
}

void XObjAct::DelLayer( SPBaseLayer spLayer, BOOL bDelLayerOnly )
{
	BOOL bErase = FALSE;
	LAYER_MANUAL_LOOP( itor )
	{
		if( (*itor) == spLayer )
		{
			if( bDelLayerOnly == FALSE )		// 알맹이까지 삭제해야한다면
			{
				// 레이어인포를 먼저 삭제 - 이걸 이안에다 넣은이유는 레이어순서바꿀때 노드만 지웠다가 다시 insert하는것이라서 만약 레이어인포를 지워버리면 인서트 할때 다시 살리기가 난감해진다. 대신 DelLayer옵션으로 레이어만 삭제하는 기능이 들어간다면 레이어인포와 레이어수가 다른문제가 생길거다. 경고를 띄워줘야 할듯
				m_pAction->DelLayerInfo( spLayer );
				bErase = m_pAction->DelLayerLinkKey( spLayer );		// 이 레이어에 속해있는 모든키를 삭제해야 한다
				delete (*itor);			// 알맹이 레이어도 삭제
				// 레이어 순서바꾸기에서는 레이어노드만 삭제하고 다시 삽입하기때문에 레이어를 삭제하면 안된다. bDelLayerOnly안에서 삭제하는게 맞음
			}
			m_listLayer.erase( itor++ );			// 레이어 노드 삭제
			break;
		} else
			++itor;
	}
	if( bErase )			// 키를 삭제했는가
	{
//		SetAction();		
		// 레이어 내용 초기화
		ClearLayer();
	}
}
// pSrc를 pDst가 있는 노드앞에 삽입한다.
// pDst가 널이면 맨뒤에 추가한다
void XObjAct::InsertLayer( SPBaseLayer spDst, SPBaseLayer spSrc )
{
	if( pSrc == nullptr )	return;
	for( auto& spLayer : m_listLayer )
	{
		if( (*itor) == pDst ) {
			if( m_listLayer.begin() == itor )
			{
				pSrc->SetfPriority( (*itor)->GetfPriority() / 2.0f );	// 맨앞에 레이어면 Priority값의 절반을 새레이어의 priority값으로 쓴다
			} else
			{
				XBaseLayer_Itor itorPrev = itor;
				itorPrev--;
				pSrc->SetfPriority( (*itorPrev)->GetfPriority() + ((*itor)->GetfPriority() - (*itorPrev)->GetfPriority()) / 2.0f );		// 다음키와 pDst키 사이의 중간값을 계산해서 삽입될 노드의 우선순위로 쓴다
			}
			m_listLayer.insert( itor, pSrc );
			return;
		}
	}
	if( itor == m_listLayer.end() )		// pDst를 찾지못했으면 맨뒤에다가 추가시킨다
		AddLayer( pSrc );
//		m_listLayer.push_back( pSrc );

//	RenumberLayer();		// 바뀐순서대로 레이어번호 재부여
	UpdateLayersPos();
}

// 액션을 클릭해서 선택하면 각 레이어에 진입/나옴 이벤트를 보낸다
void XObjAct::SendLayerEvent_OnSelected( int nEvent )
{
	for( auto& spLayer : m_listLayer )
	{
		(*itor)->OnSelected( nEvent );
	}
}

SPBaseLayer XObjAct::AddImgLayer() 
{
	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xIMAGE_LAYER );
	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
	return spLayer;
}
SPBaseLayer XObjAct::AddObjLayer() 
{
	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xOBJ_LAYER );
	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
	return spLayer;
}
SPBaseLayer XObjAct::AddSndLayer() 
{
	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xSOUND_LAYER );
	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
	return spLayer;
}
SPBaseLayer XObjAct::AddEventLayer() 
{
	SPBaseLayer spLayer = CreateLayer( 0, XBaseLayer::xEVENT_LAYER );
	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
	return spLayer;
}
XLayerDummy* XObjAct::AddDummyLayer() 
{
	XLayerDummy *spLayer = SafeCast<XLayerDummy*, SPBaseLayer>( CreateLayer( 0, XBaseLayer::xDUMMY_LAYER ) );
	spLayer->SetidLayer( m_pAction->AddLayerInfo( spLayer->GetType(), spLayer->GetnLayer(), 0 )->idLayer );
	return spLayer;
}
SPBaseLayer XObjAct::GetLastLayer() 
{
	if( m_listLayer.size() == 0 )
		return nullptr;
	XBaseLayer_Itor itor = m_listLayer.end();
	return ( *--itor );
}
SPBaseLayer XObjAct::GetFirstLayer() 
{
	if( m_listLayer.size() == 0 )
		return nullptr;
	XBaseLayer_Itor itor = m_listLayer.begin();
	return ( *itor );
}
#endif
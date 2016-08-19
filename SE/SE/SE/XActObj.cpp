#include "stdafx.h"
#include "XActObj.h"
#include "XAniAction.h"
#include "XKeyAll.h"
#include "XLayerAll.h"
#include "SprObj.h"
#include "SprDat.h"
#include "Tool.h"
#include "FrameView.h"
#include "AnimationView.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xSpr;

/**
 @brief this액션을 카피한다.
*/
// XSPActObj XActObj::CreateCopy() 
// {
// 	return XSPActObj( new XActObj( *this ) );
// 		// 액션 카피본 생성
// 		auto spNewAct = m_spAction->CreateCopy( m_pSprObj->GetpSprDat() );
// 		if( XASSERT( spNewAct ) ) {
// 			spNewAct->SetNewForAfterCopy();
// 			// 카피본 액션으로 새 액션객체 생성
// 			auto spNewActObj = XSPActObj( new XActObj( *this ) );
// 			spNewActObj->m_spAction = spNewAct;
// 			for( auto spLayer : m_listLayer ) {
// 				auto spNewLayer = spLayer->CreateCopy( GetThis() );
// 				spNewActObj->m_listLayer.Add( spNewLayer );
// 			}
// 			return spNewActObj;
// 		}
// 		return nullptr;
// }

/**
 @brief this액션을 복사해서 별도의 아이디를 가진 새로운 인스턴스로 만든다.
*/
XSPActObj XActObj::CreateNewInstance() const
{
	// 액션 카피본 생성
	auto spNewAct = m_spAction->CreateNewInstance();
	if( XASSERT( spNewAct ) ) {
		// 카피본 액션으로 새 액션객체 생성
		auto spNewActObj = XSPActObj( new XActObj( *this ) );
		spNewActObj->m_spAction = spNewAct;
		spNewActObj->m_listLayer.clear();
		for( auto spLayer : m_listLayer ) {
			XSPBaseLayer spNewLayer = spLayer->CreateCopy( spNewActObj );
			spNewLayer->SetNewInstanceInOtherAction();
			spNewActObj->m_listLayer.Add( spNewLayer );
		}
		return spNewActObj;
	}
	return nullptr;
}


/**
 @brief spLayerSrc의 카피본을 만들어서 새 레이어로 추가한다.
*/
XSPBaseLayer XActObj::CreateAddCopyLayer( XSPBaseLayerConst spLayerSrc )
{
	auto spLayerNew = spLayerSrc->CreateCopy( GetThis() );
	ID idLayer = m_spAction->GenerateIDLayer();
	int nLayer = m_spAction->GenerateMaxLayer( spLayerNew->GetbitType() );
	spLayerNew->SetidLayer( idLayer );
	spLayerNew->SetnLayer( nLayer );
	m_listLayer.Add( spLayerNew );
	SortLayer( CFrameView::sGet()->GetSizeKey() );
//	SyncLayerInfoToAction();
	// 소스측 레이어에 있던 키 카피해서 추가
	m_spAction->CopyKeyByLayer( spLayerSrc->GetidLayer(), spLayerNew->GetidLayer() );
	SortKey();
	return spLayerNew;
}


////////////////////////////////////////////////////////////////////////
bool CompLayer(XSPBaseLayer spLayer1, XSPBaseLayer spLayer2)
{
	if (spLayer1->GetfPriority() < spLayer2->GetfPriority())
		return TRUE;
	return FALSE;
}

////////////////////////////////////////////////////////////////
// XActObj::XActObj( XSprObj* pSprObj )
// {
// 	Init();
// 	m_pSprObj = nullptr;
// }
/**
 @brief 빈 액션을 만들고 aryLayerInfo정보로 레이어를 생성한다.
*/
XActObj::XActObj( XSprObj* pSprObj, XSPAction spAction, const std::vector<xLayerInfo>& aryLayerInfo )
{
	Init();
	XBREAK( pSprObj == nullptr );
	m_pSprObj = pSprObj;
	XBREAK( spAction == nullptr );
	m_spAction = spAction;
	// 동적 레이어객체를 만든다.
	if( m_spAction )
		MakeLayerObj( m_spAction, aryLayerInfo );
}

/**
 @brief 빈 액션을 만든다.
*/
XActObj::XActObj( XSprObj* pSprObj, XSPAction spAction )
{
	Init();
	XBREAK( pSprObj == nullptr );
	m_pSprObj = pSprObj;
	XBREAK( spAction == nullptr );
	m_spAction = spAction;
// 	// 동적 레이어객체를 만든다.
// 	if( m_spAction )
// 		MakeLayerObj( m_spAction );
}

void XActObj::Destroy()
{
	// XActObj가 파괴되면 SprDat에서 XAniAction도 자동으로 파괴된다.
// 	if( m_pSprObj->GetpSprDat() )
// 		m_pSprObj->GetpSprDat()->DelAction( m_spAction->GetidAct() );
}

ID XActObj::GetidAct() const 
{
	return m_spAction->GetidAct();
}

/**
 @brief this액션의 레이어들의 세이브정보를 만든다.
*/
void XActObj::MakeLayerInfo( std::vector<xLayerInfo>* pOutAry )
{
	for( auto spLayer : m_listLayer ) {
		ID idLayer = spLayer->GetidLayer();
		auto type = spLayer->GetbitType();
		int nLayer = spLayer->GetnLayer();
		float fLastFrame = spLayer->GetfLastFrame();
		XE::VEC2 vAdjAxis;
		if( spLayer->IsTypeLayerMove() ) {
			auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );
			if( XASSERT( spLayerMove ) ) {
				vAdjAxis = spLayerMove->GetvAdjustAxis();
			}
		}
		xLayerInfo layerInfo( idLayer, type, nLayer, fLastFrame, vAdjAxis );
		pOutAry->push_back( layerInfo );
	}
}

/**
 @brief 액션내의 레이어들중 키가 하나도 없는 레이어를 삭제한다
*/
void XActObj::DoDelEmptyLayer()
{
	for( auto itor = m_listLayer.begin(); itor != m_listLayer.end(); ) {
		const auto spLayer = (*itor);
		if( XASSERT(spLayer) ) {
			if( IsEmptyLayer( spLayer )	) {
				m_listLayer.erase( itor++ );
			} else
				++itor;
		} else
			++itor;
	}
}
/**
 @brief spLayer에 속한 키가 하나도 없는가.
*/
bool XActObj::IsEmptyLayer( const XSPBaseLayer& spLayer )
{
	for( const auto pKey : m_spAction->GetlistKey() ) {
		if( pKey->GetidLayer() == spLayer->GetidLayer() )
			return false;
	}
	return true;
}

/**
 @brief spAction으로부터 동적 레이어객체를 만든다.
*/
void XActObj::MakeLayerObj( XSPAction spAction, const std::vector<xLayerInfo>& aryLayerInfo )
{
	if( XBREAK( spAction == nullptr ) )
		return;
	XASSERT( m_listLayer.empty() );
	for( const auto& layerInfo : aryLayerInfo ) {
		auto spLayer = XBaseLayer::sCreate( layerInfo );
		if( XASSERT( spLayer ) ) {
			XBREAK( layerInfo.m_idLayer == 0 );
			//			m_listLayer.Add( spLayer );
			AddLayer( spLayer, TRUE );
		}
	}
}
// void XActObj::MakeLayerObj( XSPAction spAction )
// {
// 	if( XBREAK(spAction == nullptr) )
// 		return;
// 	XASSERT( m_listLayer.empty() );
// 	for( const auto& layerInfo : spAction->GetlistLayerInfo() ) {
// 		auto spLayer = XBaseLayer::sCreate( layerInfo );
// 		if( XASSERT(spLayer) ) {
// 			XBREAK( layerInfo.m_idLayer == 0 );
// //			m_listLayer.Add( spLayer );
// 			AddLayer( spLayer, TRUE );
// 		}
// 	}
// }

// void XActObj::SetLayerInfo( const XList4<xLayerInfo>& listLayerInfo )
// {
// 	m_spAction->SetLayerInfo( listLayerInfo );
// }
/**
 @brief 패스로 연결된 키들의 앞뒤 순서를 다시 맞춘다.
*/
void XActObj::SortKey()
{
	// XAniAction의 레이어정보 갱신.
// 	SyncLayerInfoToAction();
	std::vector<xLayerInfo> aryLayerInfo;
	MakeLayerInfo( &aryLayerInfo );
	m_spAction->SortKeyForXActObj( aryLayerInfo );
}

/**
 @brief 레이어를 업데이트 한다.
*/
void XActObj::FrameMove( float dt, float fFrmCurr )
{
	for( auto& spLayer : m_listLayer )
		spLayer->FrameMove( dt, fFrmCurr, m_pSprObj );
}

/**
 @brief 레이어를 모두 지우고 초기화 시킨다.
*/
void XActObj::ClearLayer()
{
	for( auto& spLayer : m_listLayer )
		spLayer->Clear();
}

void XActObj::MoveFrame( float fFrmCurr )
{
	for( auto& spLayer : m_listLayer )
		spLayer->MoveFrame( fFrmCurr, GetThis() );
}

float XActObj::GetMaxFrame() const 
{
	return m_spAction->GetfMaxFrame();
}

void XActObj::Draw( const D3DXMATRIX &m )
{
	for( auto& spLayer : m_listLayer ) {
		if( spLayer->GetbShow() )	{// show상태에서만 레이어를 그린다
			GetAnimationView()->DrawLayerBefore( spLayer );
			spLayer->Draw( 0, 0, m, m_pSprObj );
		}
	}
}


/**
 @brief 
 lx, ly는 0,0센터 기준 로컬좌표
*/
DWORD XActObj::GetPixel( float cx, float cy
												, float mx, float my
												, const D3DXMATRIX &m
												, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const
{
	DWORD pixel;
	auto itor = m_listLayer.end();
	--itor;
	while( 1 ) {
		auto& spLayer = (*itor);
		pixel = spLayer->GetPixel( cx, cy, mx, my, m, pa, pr, pg, pb );
		if( *pa > 0 )
			return pixel;			// 투명색이 아니면 픽셀이 있는것으로 간주하고 그 픽셀을 리턴한다
		if( itor == m_listLayer.begin() )
			break;
		--itor;
	}
	return 0;
}

/**
 @brief 
 lx, ly는 0,0센터 기준 로컬좌표
 해당좌표에 색깔이 있는 레이어가 있는지 찾아낸다
*/
XSPBaseLayer XActObj::GetLayerInPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m ) const
{
	auto itor = m_listLayer.end();
	--itor;
	while( 1 ) {
		auto& spLayer = ( *itor );
		BYTE a = 0;
		spLayer->GetPixel( cx, cy, mx, my, m, &a );
		if( a > 0 )
			return ( *itor );
		if( itor == m_listLayer.begin() )
			break;
		--itor;
	}
	return nullptr;
}

/**
 @brief 
*/
XSPBaseLayer XActObj::GetLayerInPixel( const D3DXMATRIX& mCamera, const D3DXVECTOR2& mv ) const
{
	auto itor = m_listLayer.end();
	--itor;
	while( 1 ) {
		auto& spLayer = ( *itor );
		BYTE a = 0;
		if( spLayer->GetbShow() ) {
			spLayer->GetPixel( mCamera, mv, &a );
			if( a > 0 )
				return ( *itor );
		}
		if( itor == m_listLayer.begin() )
			break;
		--itor;
	}
	return nullptr;
}

/**
 @brief this액션의 바운딩박스를 계산해서 액션에 저장한다.
 한액션의 처음부터 끝까지를 모두 반영하여 가장큰 바운딩 박스를 저장한다.
*/
void XActObj::PreCalcBoundBox()
{
	m_spAction->InitBoundBox();
	// 모든 레이어를 상대로 모든 키가 실행됨
	const auto mAct = m_spAction->GetLocalMatrix();		// 
	const float maxFrame = m_spAction->GetfMaxFrame();
	auto itorKey = m_spAction->GetKeyItorBegin();
	for( float frm = 0.f; frm <= maxFrame; frm += 1.f ) {
		// frm프레임까지의 모든레이어의 키를 실행시킴
		itorKey = m_pSprObj->ExecuteKeyByFrame( GetThis(), itorKey, frm );
		// 각 레이어의 바운드박스의 최대값을 구해서 액션에 업데이트
		for( auto& spLayer : m_listLayer ) {
			// 레이어 보간
			spLayer->FrameMove( 1.f, frm, m_pSprObj );
			XE::xRECT rectBB = spLayer->GetBoundBox( mAct );
			// 액션의 바운딩박스 크기를 업데이트.
			m_spAction->UpdateBoundBox( rectBB.vLT );
			m_spAction->UpdateBoundBox( rectBB.vRB );
		}
	}
// 
// 	for( auto& spLayer : m_listLayer ) {
// 		// 각 레이어의 바운딩박스를 구해서 액션에 업데이트
// 		float frame = 0;
// 		const float maxFrame = m_spAction->GetfMaxFrame();
// 		auto itorKey = m_spAction->GetKeyItorBegin();
// 		for( float frm = 0.f; frm <= maxFrame; frm += 1.f ) {
// 			// frm프레임의 키를 실행
// 			itorKey = m_pSprObj->ExecuteKeyByFrame( m_spAction, itorKey, frm );
// 			// 레이어 업데이트
// 			spLayer->FrameMove( 1.f, frame, m_pSprObj );
// 			// 레이어로부터 바운딩박스 크기를 구함.
// 			const auto mAct = m_spAction->GetLocalMatrix();		// 
// 			XE::xRECT rectBB = spLayer->GetBoundBox( mAct );
// 			// 액션의 바운딩박스 크기를 업데이트.
// 			m_spAction->UpdateBoundBox( rectBB.vLT );
// 			m_spAction->UpdateBoundBox( rectBB.vRB );
// 		} // for
// 	}
}

XBaseKey_Itor XActObj::GetKeyItorBegin() const 
{
	if( XASSERT(m_spAction) )
		return m_spAction->GetKeyItorBegin();
	return XBaseKey_Itor();
}

void XActObj::SetSpeed( float speed )
{
	if( XASSERT(m_spAction) )
		m_spAction->SetSpeed( speed );
}

/**
 @brief spLayerDel레이어를 삭제한다. 
 @param bDelLayerOnly true이면 속해있는 키는 지우지 않고 레이어만 지운다.
*/
void XActObj::DelLayer( XSPBaseLayer spLayerDel, BOOL bDelLayerOnly /*= FALSE*/ )
{
	BOOL bErase = FALSE;
	ID idLayer = spLayerDel->GetidLayer();
	for( auto itor = m_listLayer.begin(); itor != m_listLayer.end(); ) {
		auto spLayer = (*itor);
		if( spLayer->getid() == spLayerDel->getid() ) {
			// 알맹이까지 삭제해야한다면
			if( !bDelLayerOnly ) {		
				bErase = m_spAction->DelLayerLinkKey( spLayer );		// 이 레이어에 속해있는 모든키를 삭제해야 한다
				//delete ( *itor );			// 알맹이 레이어도 삭제
				// 레이어 순서바꾸기에서는 레이어노드만 삭제하고 다시 삽입하기때문에 레이어를 삭제하면 안된다. bDelLayerOnly안에서 삭제하는게 맞음
			}
			// 레이어순서바꾸기에선 기존엔 리스트에서만 빼고 다시 새위치에 삽입했지만
			// 지금은 shared_ptr이므로 erase하면 바로 삭제가 되어버리므로 삭제/삽업 순서를 바꿔야 한다.
			// 삽입을 먼저하고 삭제를 시켜야함.
			XBREAK( bDelLayerOnly );	// 이경우 기존코드에선 레이어가 파괴되지 않았는데 지금은 shared_ptr이라 자동파괴되므로 파괴안되도록 수정할것.
			m_listLayer.erase( itor++ );			// 레이어 노드 삭제
//			m_spAction->DelLayerInfo( idLayer );
			break;
		} else
			++itor;
	}
	if( bErase ) {			// 키를 삭제했는가
		// 레이어 내용 초기화
		ClearLayer();
	}
}

/**
 각 레이어의 y좌표를 미리 계산한다.
*/
void XActObj::UpdateLayersPos( const XE::VEC2& sizeKey )
{
	float y = 0;
	LIST_LOOP_AUTO( m_listLayer, itor, spLayer ) {
		spLayer->Setscry( y );
		y += sizeKey.h;
		if( spLayer->GetbOpen() ) {
			y += sizeKey.h;				// 기본적으로 pos채널은 열린다
			// 트랜스폼 가능한 레이어일경우
			if( spLayer->IsTypeLayerMove() )	{
				// 회전 채널을 쓰는 키가 있으면 한줄 더 내린다
				if( m_spAction->FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_ROT ) )	{
					y += sizeKey.h;
					spLayer->SetBitDrawChannel( CHA_ROT );		
				} else
					spLayer->ClearBitDrawChannel( CHA_ROT );		
				// 확축 채널을 쓰는 키가 있으면 한줄 더 내린다
				if( m_spAction->FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_SCALE ) ) {
					y += sizeKey.h;
					spLayer->SetBitDrawChannel( CHA_SCALE );		
				} else
					spLayer->ClearBitDrawChannel( CHA_SCALE );		
				// 이펙트 채널을 쓰는 키가 있으면 한줄 더 내린다
				if( m_spAction->FindKey( spLayer, xSpr::xKT_NONE, xSpr::xKTS_EFFECT ) ) {
					y += sizeKey.h;
					spLayer->SetBitDrawChannel( CHA_EFFECT );		// 이펙트 채널을 그려야 함을 알림
				} else
					spLayer->ClearBitDrawChannel( CHA_EFFECT );		// 이펙트 채널을 그려야 함을 알림
			}
		}
	} END_LOOP;
}


bool XActObj::SwapLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2, const XE::VEC2& sizeKey )
{
	if( spLayer1 == nullptr || spLayer2 == nullptr )	
		return false;
	if( XBREAK( m_spAction == nullptr ) )
		return false;
	SwapLayer( spLayer1, spLayer2 );
	UpdateLayersPos( sizeKey );
	return true;
}

bool XActObj::SwapLayer( XSPBaseLayer spLayer1, XSPBaseLayer spLayer2 )
{
	bool bRet = true;
	auto& itor1 =std::find( m_listLayer.begin(), m_listLayer.end(), spLayer1 );
	auto& itor2 = std::find( m_listLayer.begin(), m_listLayer.end(), spLayer2 );
	std::swap( *itor1, *itor2 );
	
	return bRet;
}

XSPBaseLayer XActObj::GetspLayer( xSpr::xtLayer type, int nLayer ) const
{
	for( auto spLayer : m_listLayer ) {
		if( spLayer->IsSameType( type ) && spLayer->GetnLayer() == nLayer )
			return spLayer;
	}
	return nullptr;
}
XSPBaseLayer XActObj::GetspLayerMutable( xSpr::xtLayer type, int nLayer ) const
{
	for( auto spLayer : m_listLayer ) {
		if( spLayer->IsSameType( type ) && spLayer->GetnLayer() == nLayer )
			return spLayer;
	}
	return nullptr;
}

XSPBaseLayer XActObj::GetspLayerByidLayer( ID idLayer ) const
{
	for( auto spLayer : m_listLayer ) {
		if( spLayer->GetidLayer() == idLayer )
			return spLayer;
	}
	return nullptr;
}

// XSPBaseLayer XActObj::GetspLayerBynLayer( int nLayer ) const
// {
// 	for( auto spLayer : m_listLayer ) {
// 		if( spLayer->GetnLayer() == nLayer )
// 			return spLayer;
// 	}
// 	return nullptr;
// }

XSPBaseLayer XActObj::GetspLayer( XBaseKey* pKey )
{
	if( pKey == nullptr )
		return nullptr;;
	return GetspLayerByidLayer( pKey->GetidLayer() );
}

XSPBaseLayer XActObj::GetspLayerMutable( ID idLayer ) const
{
	for( auto spLayer : m_listLayer ) {
		if( spLayer->GetidLayer() == idLayer )
			return spLayer;
	}
	return nullptr;
}

XSPBaseLayer XActObj::GetspLayer( xSpr::xtLayer typeLayer, ID idLayer ) const
{
	for( auto& spLayer : m_listLayer ) {
		if( spLayer->GetidLayer() == idLayer )
			if( XASSERT(spLayer->IsSameType( typeLayer) ) )
				return spLayer;
	}
	return nullptr;
}

XSPBaseLayer XActObj::GetspLayerMutable( xSpr::xtLayer typeLayer, ID idLayer ) const
{
	for( auto& spLayer : m_listLayer ) {
		if( spLayer->GetidLayer() == idLayer )
			if( XASSERT( spLayer->IsSameType( typeLayer) ) )
				return spLayer;
	}
	return nullptr;
}


XBaseKey* XActObj::AddKeySprAtLast( ID idLayer, XSprite *pSprite )
{
	auto spLayer = GetspLayerByTypeMutable<XLayerImage>( idLayer );
	if( XASSERT(spLayer) ) {
		const float fFrame = spLayer->GetfLastFrame();			// nLayer에 마지막으로 추가되었던 프레임이 몇번이었는지 검사해서 거기에 + 1를 한 번호로 키를 추가한다.
		XBaseKey* pKey = m_spAction->AddKeyImage( fFrame, spLayer, pSprite );
		spLayer->SetfLastFrame( fFrame + 1.0f );
		return pKey;
	}
	return nullptr;
}

int XActObj::AddKeySprWithFrame( ID idLayer, float fFrame, XSprite *pSprite )
{
	auto spLayer = GetspLayerByTypeMutable<XLayerImage>( idLayer );
	if( XASSERT(spLayer) ) {
		m_spAction->AddKeyImage( fFrame, spLayer, pSprite );
		spLayer->SetfLastFrame( fFrame + 1.0f );
	}
	return 1;
}
int XActObj::AddKeyPosWithFrame( ID idLayer, float fFrame, float x, float y )
{
	auto spLayer = GetspLayerByTypeMutable<XLayerMove>( idLayer );
	if( XASSERT( spLayer ) ) {
		m_spAction->AddKeyPos( fFrame, spLayer, x, y );
		//	spLayer->SetfLastFrame( fFrame + 1.0f );
	}
	return 1;
}

XBaseKey_Itor XActObj::AddKey( XBaseKey* pCopyKey, float fFrame, BOOL bNewGenerateID ) {
	return m_spAction->AddKey( pCopyKey );
}


// 액션을 클릭해서 선택하면 각 레이어에 진입/나옴 이벤트를 보낸다
void XActObj::SendLayerEvent_OnSelected( int nEvent )
{
	for( auto& spLayer : m_listLayer ) {
		spLayer->OnSelected( nEvent );
	}
}

XSPBaseLayer XActObj::GetLastLayer()
{
	if( m_listLayer.size() == 0 )
		return nullptr;
	auto itor = m_listLayer.end();
	return ( *--itor );
}

XSPBaseLayer XActObj::GetFirstLayer()
{
	if( m_listLayer.size() == 0 )
		return nullptr;
	auto itor = m_listLayer.begin();
	return ( *itor );
}

/**
 @brief typeLayer타입의 레이어가 몇개나 있는지 리턴
*/
int XActObj::GetNumLayerByType( xSpr::xtLayer typeLayer ) const 
{
	int num = 0;
	for( auto& spLayer : m_listLayer ) {
		if( spLayer->GetbitType() == typeLayer )
			++num;
	}
	return num;
}

// void XActObj::SetpLastKey( ID idLayer, XBaseKey* pKey )
// {
// 	auto spLayer = GetspLayerMutable( idLayer );
// 	if( XASSERT(spLayer) ) {
// 		spLayer->SetpLastKey( pKey );
// 	}
// }
// 
// void XActObj::SetpSprObjCurr( ID idLayer, XSprObj* pSprObj )
// {
// 	auto spLayer = GetspLayerByTypeMutable<XLayerObject>( idLayer );
// 	if( XASSERT(spLayer) ) {
// 		spLayer->SetpSprObjCurr( pSprObj );
// 	}
// }
// 
// void XActObj::SetPosToLayer( XSPLayerMove _spLayer, const XE::VEC2& vPos )
// {
// 	auto spLayer = GetspLayerByTypeMutable<XLayerMove>( _spLayer->GetidLayer() );
// 	if( XASSERT( spLayer ) ) {
// 		spLayer->SetPos( vPos );
// 	}
// }
// 
// void XActObj::SetRotToLayer( XSPLayerMove _spLayer, const float dAng )
// {
// 	auto spLayer = GetspLayerByTypeMutable<XLayerMove>( _spLayer->GetidLayer() );
// 	if( XASSERT( spLayer ) ) {
// 		spLayer->SetAngleZ( dAng );
// 	}
// }
// 
// void XActObj::SetScaleToLayer( XSPLayerMove _spLayer, const XE::VEC2& vScale )
// {
// 	auto spLayer = GetspLayerByTypeMutable<XLayerMove>( _spLayer->GetidLayer() );
// 	if( XASSERT( spLayer ) ) {
// 		spLayer->SetScale( vScale );
// 	}
// }
// 
// void XActObj::SetcnPosToLayer( XSPLayerMove _spLayer, const CHANNEL_POS& cnPos )
// {
// 	auto spLayer = GetspLayerByTypeMutable<XLayerMove>( _spLayer->GetidLayer() );
// 	if( XASSERT( spLayer ) ) {
// 		spLayer->SetcnPos( cnPos );
// 	}
// }
// void XActObj::SetcnRotToLayer( XSPLayerMove _spLayer, const CHANNEL_ROT& cnRot )
// {
// 	auto spLayer = GetspLayerByTypeMutable<XLayerMove>( _spLayer->GetidLayer() );
// 	if( XASSERT( spLayer ) ) {
// 		spLayer->SetcnRot( cnRot );
// 	}
// }
// void XActObj::SetcnScaleToLayer( XSPLayerMove _spLayer, const CHANNEL_SCALE& cnScale )
// {
// 	auto spLayer = GetspLayerByTypeMutable<XLayerMove>( _spLayer->GetidLayer() );
// 	if( XASSERT( spLayer ) ) {
// 		spLayer->SetcnScale( cnScale );
// 	}
// }

XSprite* XActObj::AddAniFrame( const XE::VEC2& sizeSurface
															, const XE::VEC2& vAdj 
															, const XE::VEC2& sizeMem 
															, XSprite* pSprite)
{
// 	auto spLayer = GetspLayerMutable( xSpr::xLT_IMAGE, 0 );
// 	auto spLayer = std::static_pointer_cast<XLayerImage>( spBaseLayer );
	auto spLayer = GetspLayerByIndex<XLayerImage>( 0 );
	if( XASSERT(spLayer) )
		AddKeySprAtLast( spLayer->GetidLayer(), pSprite );
	return pSprite;
}

// XBaseKey *XActObj::FindPrevKey( xSpr::xtKey type, xSpr::xtKeySub subType, float fFrame, XSPBaseLayerConst spLayer )
// {
// 	XBaseKey *pFindKey = nullptr;
// 	for( auto pKey : m_listKey ) {
// 		if( pKey->GetfFrame() > fFrame )
// 			break;
// 
// 		// 같은레이어에 조건에 맞는 키가 있는가
// // 		if( pKey->GetType() == type 
// // 			&& pKey->GetSubType() == subType 
// // 			&& pKey->GetLayerType() == spLayer->GetType() 
// // 			&& pKey->GetnLayer() == spLayer->GetnLayer() )	{
// // 		if( pKey->IsSameLayer( spLayer ) && pKey->GetType() == type  && pKey->GetSubType() == subType ) {
// 		// 같은 채널의 키를 찾는다.
// 		if( pKey->IsSameLayer( spLayer ) && pKey->GetSubType() == subType ) {
// 			XASSERT( pKey->GetType() == type );
// 			if( pFindKey ) {
// 				if( pFindKey->GetfFrame() < pKey->GetfFrame() )
// 					pFindKey = pKey;
// 			} else
// 				pFindKey = pKey;
// 		}
// 	}
// 	return pFindKey;
// }

void XActObj::AddLayer(XSPBaseLayer spNewLayer, BOOL bCalcPriority/* = TRUE */)
{
	if( XBREAK( spNewLayer == nullptr ) )
		return;
	// 이미 있는건지 검사
	auto spLayerExist = GetspLayerByidLayer( spNewLayer->GetidLayer() );
	XBREAK( spLayerExist != nullptr );
	//
	if (m_listLayer.size()) {
		// 		XBaseLayer_Itor itorEnd = m_listLayer.end();
		// 		SPBaseLayer spLast = ( *--itorEnd );
		auto sppLast = m_listLayer.GetLast();
		if (XASSERT(sppLast)) {
			if (bCalcPriority)		// 프라이오리티를 건드리지 않는 옵션
				spNewLayer->SetfPriority((*sppLast)->GetfPriority() + 1.0f);
		}
	} else {
		if (bCalcPriority)
			spNewLayer->SetfPriority(1.0f);
	}
	m_listLayer.push_back(spNewLayer);
}

/**
@brief spNewLayer를 리스트에 추가한다.
@param idLayer 지정한 id로 세팅한다. 0이면 자동으로 생성한다.
*/
void XActObj::AddLayer( ID idLayer, XSPBaseLayer spNewLayer )
{
	XASSERT( spNewLayer->GetidLayer() == 0 );		// spNewLayer는 새로 생성되었을테니 0이어야 한다.
	if( idLayer == 0 )
		idLayer = m_spAction->GenerateIDLayer();
	int nLayer = m_spAction->GenerateMaxLayer( spNewLayer->GetbitType() );
	spNewLayer->SetidLayer( idLayer );
	spNewLayer->SetnLayer( nLayer );
	AddLayer( spNewLayer, TRUE );
}

void XActObj::SortLayer(const XE::VEC2& sizeKey )
{
	m_listLayer.sort( CompLayer );
	UpdateLayersPos( sizeKey );
}

/**
@brief 레이어를 생성한다.
@param nLayer -1이면 레이어번호를 자동생성한다
*/
XSPBaseLayer XActObj::CreateAddLayer(xSpr::xtLayer type,
																		int nLayer,
																		const XE::VEC2& vAdjAxis)
{
	XBREAK(!type);
	XBREAK(nLayer < -1);
	if( nLayer == -1 ) {
		nLayer = m_spAction->GenerateMaxLayer( type );
	}
	ID idLayer = m_spAction->GenerateIDLayer();

	XSPBaseLayer spBaseLayer;
	// 이거 나중에 XBaseLayer의스태틱 함수로 집어넣자. 안그러면 레이어종류가 추가될때마다 일일히 이런곳 찾아서 수정해줘야 한다
	switch (type) {
	case xSpr::xLT_IMAGE: {
		auto spLayer = xSPLayerImage(new XLayerImage);
		spLayer->SetAdjustAxis(vAdjAxis);
		spBaseLayer = spLayer;
	} break;
	case xSpr::xLT_OBJECT: {
		auto spLayer = XSPLayerObject(new XLayerObject);
		spLayer->SetAdjustAxis(vAdjAxis);
		spBaseLayer = spLayer;
	} break;
	case xSpr::xLT_SOUND: {
		auto spLayer = XSPLayerSound(new XLayerSound);
		spBaseLayer = spLayer;
	} break;
	case xSpr::xLT_EVENT: {
		auto spLayer = XSPLayerEvent(new XLayerEvent);
		spBaseLayer = spLayer;
	} break;
	case xSpr::xLT_DUMMY: {
		auto spLayer = XSPLayerDummy(new XLayerDummy);
		spLayer->SetAdjustAxis(vAdjAxis);
		spBaseLayer = spLayer;
	} break;
	default:
		spBaseLayer = nullptr;
		XBREAK(1);
		XALERT("XAniAction::CreateLayer 생성타입이 이상함. %d", (int)type);
		break;
	}
	//
	//
// 	if (nLayer == -1)
// 		spBaseLayer->SetnLayer(m_spAction->GenerateMaxLayer(type));		// 레이어번호 자동생성
// 	else
// 		spBaseLayer->SetnLayer(nLayer);	 // 지정한 레이어번호로 생성
	if( XASSERT(spBaseLayer) ) {
		XBREAK( nLayer < 0 );
		spBaseLayer->SetidLayer( idLayer );
		spBaseLayer->SetnLayer( nLayer );
		AddLayer( spBaseLayer );
//		ID idLayer = m_spAction->GenerateIDLayer();
	}
	return spBaseLayer;
}

/**
@brief spLayerSrc를 카피해서 새로운 레이어를 생성한다.
*/
// XSPBaseLayer XActObj::CreateLayerFromOtherLayer(XSPBaseLayer spLayerSrc)
// {
// 	auto spLayerCopy = spLayerSrc->CopyDeep();
// 	spLayerCopy->SetidLayer( m_spAction->GenerateIDLayer());
// 	return spLayerCopy;
// }

/**
@brief 인자로 주어진 레이어를 제외한 나머지 레이어를 감춘다.
*/
void XActObj::SetShowLayerExcept( XSPBaseLayer spExceptLayer )
{
	for( auto& spLayer : m_listLayer ) {
		if( spLayer->IsSameLayer( spExceptLayer ) )
			spLayer->SetbShow( true );
		else
			spLayer->SetbShow( false );
	}
}
/**
@brief 모든 레이어를 보이게 한다.
*/
void XActObj::SetShowLayerAll()
{
	for( auto& spLayer : m_listLayer ) {
		spLayer->SetbShow( TRUE );
	}
}

/**
@brief 레이어의 오픈여부에 따라 키두개가 서로 같은 레이어에 있는지 검사한다.
*/
bool XActObj::IsSameLayerByKeyWithOpenCheck( XBaseKey* pKey1
																					 , XBaseKey* pKey2 )
{
	// 일단 다른레이어에 있으면 무조건 false
	if( !pKey1->IsSameLayerOnly( pKey2 ) ) {
		return false;
	}
	// 
	auto spLayer = GetspLayerByidLayer( pKey1->GetidLayer() );
	if( XASSERT( spLayer ) ) {
		if( spLayer->GetbAbleOpen() ) {
			// 열수있는 레이어이며 열려있을땐 서브타입까지 검사.
			if( spLayer->GetbOpen() )
				if( pKey1->GetSubType() == pKey2->GetSubType() )
					return true;
		} else {
			// 못여는 레이어이면 서브타입만 검사.
			if( pKey1->GetSubType() == pKey2->GetSubType() )
				return true;
		}
	}
	return false;
}

float XActObj::GetSpeed() const {
	if( m_spAction )
		return m_spAction->GetSpeed();
	return 0.f;
}

/**
@brief 현재 레이어 정보를 액션에 동기화 한다.
*/
// void XActObj::SyncLayerInfoToAction()
// {
// 	XList4<xLayerInfo> listLayerInfo;
// 	MakeLayerInfo( &listLayerInfo );
// 	m_spAction->SetLayerInfo( listLayerInfo );
// }
/**
@brief 각 레이어타입들의 최대 인덱스값을 얻어서 테이블에 넣는다.
*/
void XActObj::SetMaxIndexByLayerType()
{
//	SyncLayerInfoToAction();
	std::vector<xLayerInfo> aryLayerInfo;
	MakeLayerInfo( &aryLayerInfo );
	m_spAction->SetMaxIndexByLayerType( aryLayerInfo );
}
/**
@brief 각 레이어를 돌며 가장큰 idLayer값을 얻어 그것의 +1로 seed값을 쓴다.
*/
void XActObj::SetMaxidLayer()
{
// 	SyncLayerInfoToAction();
	std::vector<xLayerInfo> aryLayerInfo;
	MakeLayerInfo( &aryLayerInfo );
	m_spAction->SetMaxidLayer( aryLayerInfo );
}

void XActObj::SetidAct( ID idAct )
{
	m_spAction->SetidAct( idAct );
}

void XActObj::SetszActName( LPCTSTR szName ) 
{
	m_spAction->SetszActName( szName );
}

/**
 @brief idLayer내에서 고유한 지역 키 아이디를 생성한다.
 @param idLocalInLayer 0이면 새로운 값을 생성하고 값이 있으면 id최대값만 조정한다.
*/
ID XActObj::GenerateidKeyLocal( ID idLayer, ID idLocalInLayer )
{
	auto spLayer = GetspLayerByidLayer( idLayer );
	if( spLayer ) {
		if( idLocalInLayer ) {
			const auto idGlobalLocalKey = std::max( idLocalInLayer, spLayer->GetidGlobalLocalKey() );
			spLayer->SetidGlobalLocalKey( idGlobalLocalKey );
			return idLocalInLayer;
		} else {
			return spLayer->GenerateidKeyLocal();
		}
	}
// 	auto pLayerInfo = FindpLayerInfoMutable( idLayer );
// 	if( pLayerInfo ) {
// 		if( idLocalInLayer ) {
// 			pLayerInfo->m_idLocalByNewKey = std::max( idLocalInLayer, pLayerInfo->m_idLocalByNewKey );
// 			return idLocalInLayer;				
// 		} else {
// 			return pLayerInfo->GenerateidKeyLocal();
// 		}
// 	}
	return 0;
}

/**
 @brief 같은 레이어내에서 키 카피
 @param idLocalInLayer 카피할 소스키의 레이어내 지역아이디
 @param idLayer 카피될 레이어
*/
XBaseKey* XActObj::CreateNewInstanceKeyBySameLayer( ID idLocalinLayerSrc, ID idLayerSrc )
{
	XBREAK( idLayerSrc == 0 );
	XBREAK( idLocalinLayerSrc == 0 );
	// 소스키를 찾음.
	auto pKeySrc = m_spAction->GetpKeyByidLocal( idLocalinLayerSrc, idLayerSrc );
	if( XASSERT(pKeySrc) )
		return CreateNewInstanceKeyBySameLayer( pKeySrc );
	return nullptr;
}

XBaseKey* XActObj::CreateNewInstanceKeyBySameLayer( XBaseKey* pKeySrc )
{
	XBREAK( pKeySrc == nullptr );
	const ID idLayer = pKeySrc->GetidLayer();
	if( XASSERT(idLayer) ) {
		ID idLocalNew = GenerateidKeyLocal( idLayer, 0 );
		if( XASSERT(idLocalNew) ) {
// 			const auto pNewKey = pKeySrc->CreateCopy( GetThis(), idLayer, idLocalNew );
			const auto pNewKey = pKeySrc->CreateCopy();
			pNewKey->SetNewInstance( m_spAction, idLayer, idLocalNew );
			AddKey( pNewKey, pKeySrc->GetfFrame() );
			return pNewKey;
		}
	}
	return nullptr;
}

/**
 @brief 타입과 레이어번호로 idLayer를 얻는다.(구버전 코드의 잔재)
*/
ID XActObj::GetidLayer( xSpr::xtLayer typeLayer, int nLayer ) const
{
	for( auto& spLayer : m_listLayer ) {
		if( spLayer->GetbitType() == typeLayer 
			&& spLayer->GetnLayer() == nLayer )
			return spLayer->GetidLayer();
	}
	return 0;
}

/**
 @brief idKey를 기준으로 vDir방향으로 키 커서를 이동시킨다.
*/
XBaseKey* XActObj::GetKeyByKeyWithDir( XBaseKey* pKeyBase, const XE::POINT& vDir )
{
	XBREAK( vDir.x == 0 );
	if( vDir.x < 0 ) {
		auto pKey = m_spAction->FindPrevKey( pKeyBase );
		return pKey;
	} else
	if( vDir.x > 0 ) {
		auto pKey = m_spAction->FindNextKey( pKeyBase );
		return pKey;
	}
	return nullptr;
}

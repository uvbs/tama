#include "stdafx.h"
#include "XOrder.h"
#include "XPropOrder.h"
#include "XPropHelp.h"
#include "XSeq.h"
#include "XPropSeq.h"
#include "XGameWnd.h"
#include "XGame.h"
#include "XSceneWorld.h"
#include "XSceneBattle.h"
#include "XPropWorld.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( xHelp )

/**
 @brief 명령 인스턴스를 생성한다.
*/
OrderPtr sCreateOrder( XSeq *pSeq, int idxOrder )
{
	OrderPtr spOrder;
	switch( pSeq->GetspProp()->m_aryOrder[idxOrder]->m_Type )
	{
	case xHO_DIALOG: spOrder = OrderPtr( new XOrderDialog( pSeq, idxOrder ) ); break;
	case xHO_INDICATE:	spOrder = OrderPtr( new XOrderIndicate( pSeq, idxOrder ) );	break;
	case xHO_CAMERA:	spOrder = OrderPtr( new XOrderCamera( pSeq, idxOrder ) );	break;
	case xHO_TOUCH:		spOrder = OrderPtr( new XOrderTouchLock( pSeq, idxOrder ) );	break;
//	case xHO_READY_TOUCH:	spOrder = OrderPtr( new XOrderReadyTouch( pSeq, idxOrder ) );	break;
	case xHO_DELAY:		spOrder = OrderPtr( new XOrderDelay( pSeq, idxOrder ) );	break;
	case xHO_MODE_CHANGE:	spOrder = OrderPtr( new XOrderMode( pSeq, idxOrder ) );	break;
	default:
		XBREAK(1);
		break;
	}
	return spOrder;
}
////////////////////////////////////////////////////////////////
XOrder::XOrder( XSeq *pSeq, int idxOrder )
{
	Init();
	m_idxOrder = idxOrder;
	m_pSeq = pSeq;
	m_bSceneActive = GAME->GetpScene()->GetbActive() != FALSE;
	CONSOLE( "XOrder(%d):기존active=%d", GetspProp()->m_Type, 
												( m_bSceneActive ) ? 1 : 0 );
}

PropOrderPtr XOrder::GetspProp()
{
	return m_pSeq->GetspProp()->m_aryOrder[ m_idxOrder ];
}

void XOrder::OnClickWnd( const std::string& idsWnd )
{
	// 소멸조건이 특정버튼 눌림이면 종료를 통보한다.
// 	if( GetspProp()->m_typeEnd == xFIN_PUSH_BUTT ){
// 		if( GetspProp()->m_idsEnd == idsWnd ) {
// 			m_pSeq->OnEndOrder( GetThis() );
// 		}
// 	}
}

void XOrder::DispatchEvent( xHelp::xtEvent event )
{
	if( event == xHE_DRAGGED_SLOT ) {
		// 드래그 완료
		if( GetspProp()->m_typeEnd == xFIN_ORDER_COMPLETE )
			if( GetspProp()->m_idsEnd == "draged_slot" )
				SetbEnd( true );
	}
}

bool XOrder::IsEndTypeTouch() 
{
	return GetspProp()->IsEndTypeTouch();
}

////////////////////////////////////////////////////////////////
XE::VEC2 XOrderDialog::s_vSavePos;
XOrderDialog::XOrderDialog( XSeq *pSeq, int idxOrder )
	: XOrder( pSeq, idxOrder )
{
	Init();
	m_spProp = std::static_pointer_cast<XPropDialog>( pSeq->GetspProp()->m_aryOrder[ idxOrder ] );
}
void XOrderDialog::Destroy()
{
	// 강제종료등에 사용함.
	if( !m_idsWnd.empty() )
		GAME->DestroyWndByIdentifier( m_idsWnd );
}

/**
 @brief 
 OnDestroy를 쓴 이유는 shared_ptr때문. 그래서 SetbDestroy()를 하면 반드시 호출되는 핸들러가 필요했음.
*/
void XOrderDialog::OnDestroy()
{
	GAME->DestroyWndByIdentifier( m_idsWnd );
	m_idsWnd.clear();
	// 좌표보존옵션이면 이 대화창의 좌표를 보관한다.
	if( m_spProp->m_bSavePos ) {
		s_vSavePos = m_spProp->m_vPos;	
	}
}
/**
 @brief 외부에서 강제종료됨.
*/
void XOrderDialog::SetForceClose()
{
	XOrder::SetForceClose();
	// 다른 오더의 종료를 기다려야했던것이라면 그 오더도 종료시킴.
	if( m_spProp->m_typeEnd == xFIN_END_ORDER ) {
		auto spEndOrder = GetpSeq()->FindOrderObj( m_spProp->m_idsEnd );
		if( spEndOrder )
			spEndOrder->SetbEnd( true );
	}

}

void XOrderDialog::OnCreate()
{
	// 대화창 UI를 생성한다.
	// 좌표보존 옵션이면 이 좌표를 백업받는다.
	XE::VEC2 vPos( 0, 43 );
		vPos.x = m_spProp->m_vPos.x;
		vPos.y = m_spProp->m_vPos.y;
	if( m_spProp->m_bSavePos ) {
		// 좌표보존옵션일때 좌표를 백업받는다.
		s_vSavePos = vPos;
	} else {
		// 보존된 좌표가 있으면 그것을 쓴다.
		if( !s_vSavePos.IsMinus() )
			vPos = s_vSavePos;
	}
	// 대화창 생성
	{
		if( vPos.y == -1.f )
			vPos.y = 43;
		auto pPropHero = PROP_HERO->GetpProp( m_spProp->m_strTalker );
		XHero *pHero = nullptr;
		if( XASSERT( pPropHero ) ) {
			// 영웅얼굴을 그리기위해 현재씬에 영웅객체를 요구한다.
			pHero = GAME->GetpScene()->GetpHero( pPropHero->idProp );
// 			_tstring szRes = XE::MakePath( DIR_IMG, pPropHero->strFace );
// 			pWnd->SetstrFaceRes( szRes );
			// 대화창이 시작될때 씬에 이벤트 날림.
		}
		// dialog.으로 시작하는 윈도우 모두 삭제,.
		for( int i = 0; i < 10; ++i ) {
			auto pWndPrev = GAME->FindWithPrefix( "__dialog." );		// 이 접두사로 시작하는 윈도우를 찾음.
			if( pWndPrev ) {
				pWndPrev->SetbDestroy( true );
				// 더이상 안나올때까지 반복.
			} else {
				break;
			}
		}
		auto pWnd = new XWndOrderDialog( GetThis(), vPos, pPropHero->idProp, pHero );
		pWnd->SetstrIdentifierf( "__dialog.%s.%d", GetpSeq()->GetspProp()->m_strIdentifier.c_str(), GetidxOrder() );
		m_idsWnd = pWnd->GetstrIdentifier();
		auto strText = m_spProp->GetstrText();
		XGAME::sReplaceToken( strText, _T( "#nick#" ), ACCOUNT->GetstrName() );
		TCHAR szBuff[ 1024 ];
		XE::ConvertJosaStr( szBuff, strText );
		pWnd->SetstrText( szBuff );
		if( m_spProp->m_typeEnd == xFIN_TOUCH ) {
			pWnd->SetbTouch( true );
		}
		GAME->GetpRootSeq()->Add( pWnd );
		// x좌표가 지정되지 않았을때 자동정렬한다.
		if( vPos.x == -1.f )
			pWnd->AutoLayoutHCenter();
		GAME->GetpScene()->OnCreateOrderDialog( pPropHero->idProp );
	}
}

void XOrderDialog::OnClickWnd( const std::string& idsClickedWnd )
{
	XOrder::OnClickWnd( idsClickedWnd );
	// 소멸조건이 특정버튼 눌림이면 종료를 통보한다.
	if( idsClickedWnd == m_idsWnd ) {
		CONSOLE("XOrderDialog::clicked dialog window");
		if( GetspProp()->m_typeEnd == xFIN_TOUCH ){
			SetbEnd( true );
		} else
		if( m_spProp->m_typeEnd == xFIN_PUSH_TARGET ) {
			GAME->SetActive( GetbSceneActive() );	// 예전상태로 되돌림.
			GAME->GetlistAllowWnd().clear();
			SetbEnd( true );
		}
	}
}

////////////////////////////////////////////////////////////////
XOrderIndicate::XOrderIndicate( XSeq *pSeq, int idxOrder )
	: XOrder( pSeq, idxOrder )
{
	Init();
	m_spPropSeq = pSeq->GetspProp();
	m_spProp = std::static_pointer_cast<XPropIndicate>( m_spPropSeq->m_aryOrder[ idxOrder ] );
}

void XOrderIndicate::OnDestroy() 
{
	if( GetspProp()->m_typeEnd == xFIN_PUSH_TARGET ){
		GAME->SetActive( GetbSceneActive() );	// 예전상태로 되돌림.
		GAME->GetlistAllowWnd().clear();
	}
	// 화살표 윈도우 삭제.
	for( auto& idsWnd : m_aryIdsWnd ) {
		if( !idsWnd.empty() )
			GAME->DestroyWndByIdentifier( idsWnd );
	}
}

void XOrderIndicate::OnCreate()
{
	// 터치를 허용하는 윈도우들의 목록처리
	if( m_spProp->m_aryIdsAllow.size() > 0 ) {
		for( auto& idsAllow : m_spProp->m_aryIdsAllow ) {
			if( !idsAllow.empty() ) {
				GAME->SetActive( true );
				GAME->AddidsAllowWnd( idsAllow );
				CreateIndicator( idsAllow );
			}
		}
	}
	if( m_spProp->m_aryIdsTarget.size() ) {
		// 타겟리스트가 있을때
		bool bFound = false;
		for( auto& idsTarget : m_spProp->m_aryIdsTarget ) {
			if( !idsTarget.empty() ) {
				// 각타겟에 인디케이터를 생성한다.
				bFound = CreateIndicator();
			} else {
				CONSOLE( "%s:ids_target(%s)찾을수 없음.", __TFUNC__, C2SZ(idsTarget) );
			}
		}
		if( !bFound )
			m_timerSec.Set( 1.f );	// 1초에 한번씩 재검색하기 위해
		// 아직 창이 안떳다든가 하는이유로 타겟팅에 실패하면 process에서 다시 찾는다.
		if( m_spProp->m_typeEnd == xFIN_PUSH_TARGET ) {
			GAME->SetActive( true );	// 입력을 모두 풀고
			GAME->AddidsAllowWnd( m_spProp->m_aryIdsTarget );
		}
	} else {
		// end조건이 order_complete이면 좌표가 반드시 있어야 한다.(뭔가 이상하군)
		XBREAK( m_spProp->m_typeEnd != xFIN_ORDER_COMPLETE && m_spProp->m_vPos.IsZero() );
		if( m_spProp->m_secLife > 0 ) {
			m_timerFinish.Set( m_spProp->m_secLife );
		} else {
// 			if( m_spProp->m_aryIdsTarget.size() == 0 )
// 				m_timerFinish.Set( 10.f );	// 타겟이 없는 화살표는 10초후에 자동으로 종료된다.
		}
		if( m_spProp->m_idsRoot.empty() )
			CreateSpr( m_spProp->m_vPos, GAME->GetpScene(), XE::VEC2(0) );
		else {
			XWnd *pRoot = GAME->Find( m_spProp->m_idsRoot );
			if( XASSERT( pRoot ) ) {
				CreateSpr( pRoot, m_spProp->m_idsRoot );
			}
// 				CreateSpr( m_spProp->m_vPos, pRoot, std::string() );
		}
	}
	if( m_spProp->m_typeEnd == xFIN_ORDER_COMPLETE ) {
		GAME->SetActive( true );
	}
}

/**
 @brief ids를 추적하는 버전
*/
bool XOrderIndicate::CreateSpr( XWnd *pRoot, const std::string& ids )
{
	XBREAK( ids.empty() );
	_tstring strSpr = _T( "ui_arrow.spr" );
	ID idAct = 1;
	if( !m_spProp->m_strSpr.empty() ) {
		strSpr = m_spProp->m_strSpr;
		idAct = m_spProp->m_idAct;
		if( idAct == 0 )
			idAct = 1;
	}
	if( m_spProp->m_typeEnd == xFIN_END_ORDER ) {
		// 단지 가리키기만 하는 화살표
		idAct = 3;
	}
	if( pRoot == nullptr )
		return false;
	XWnd *pExist = pRoot->Findf( "%s.indicate.%d.%s",
							GetpSeq()->GetspProp()->m_strIdentifier.c_str(),
							GetidxOrder(), ids.c_str() );
	if( pExist == nullptr ) {
		XWndSprTrace* pWndSpr = nullptr;
		float dAng = m_spProp->m_dAng;
		if( dAng >= 0 ) {
			pWndSpr = new XWndSprTrace( strSpr, idAct, ids, dAng + 180.f );
		} else {
			// center나 원점.
			pWndSpr = new XWndSprTrace( strSpr, idAct, ids, dAng );
		}
//		int dAng = (int)m_spProp->m_dAng;
// 		if( dAng > 90 && dAng < 270 && idAct == 1 )
// 			idAct = 2;
// 		auto pWndSpr = new XWndSprObj( strSpr, idAct, vPos );
// 		pWndSpr->SetstrIdentifierf( "%s.indicate.%d.%s",
// 								GetpSeq()->GetspProp()->m_strIdentifier.c_str(),
// 								GetidxOrder(), ids.c_str() );
		pWndSpr->SetstrIdentifierf( "%s.indicate.%d.%s",
								GetpSeq()->GetspProp()->m_strIdentifier.c_str(),
								GetidxOrder(), ids.c_str() );
		pWndSpr->SetbActive( false );
		m_aryIdsWnd.push_back( pWndSpr->GetstrIdentifier() );
		if( m_spProp->m_dAng >= 0 )
			pWndSpr->SetRotateZ( m_spProp->m_dAng );
		pRoot->Add( pWndSpr );
		return true;
	}
	return false;
}

/**
 @brief 대상이 객체가 아닌 그냥 좌표일 경우.
*/
bool XOrderIndicate::CreateSpr( const XE::VEC2& vPos, XWnd *pRoot, const XE::VEC2& sizeTarget )
{
	_tstring strSpr = _T( "ui_arrow.spr" );
	ID idAct = 1;
	if( !m_spProp->m_strSpr.empty() ) {
		strSpr = m_spProp->m_strSpr;
		idAct = m_spProp->m_idAct;
		if( idAct == 0 )
			idAct = 1;
	}
	if( m_spProp->m_typeEnd == xFIN_END_ORDER ) {
		// 단지 가리키기만 하는 화살표
		idAct = 3;
	}
	if( pRoot == nullptr )
		return false;
	XWnd *pExist = pRoot->Findf( "%s.indicate.%d",
							GetpSeq()->GetspProp()->m_strIdentifier.c_str(),
							GetidxOrder() );
	if( pExist == nullptr ) {
		float dAngPlace = m_spProp->m_dAng;
		if( dAngPlace >= 0 )
			dAngPlace += 180.f;
		auto pWndSpr = new XWndSprTrace( strSpr, idAct, vPos, dAngPlace, sizeTarget ); 
		pWndSpr->SetstrIdentifierf( "%s.indicate.%d",
								GetpSeq()->GetspProp()->m_strIdentifier.c_str(),
								GetidxOrder() );
		pWndSpr->SetbActive( false );
		m_aryIdsWnd.push_back( pWndSpr->GetstrIdentifier() );
		if( m_spProp->m_dAng >= 0 )
		pWndSpr->SetRotateZ( m_spProp->m_dAng );
		pRoot->Add( pWndSpr );
		return true;
	}
	return false;
}
bool XOrderIndicate::CreateIndicator()
{
	if( m_bTargeted )
		return true;
	int numFounds = 0;
	bool bOk = false;
	XBREAK( m_spProp->m_aryIdsTarget.size() == 0 );
	for( auto& idsTarget : m_spProp->m_aryIdsTarget ) {
		bool bOk = CreateIndicator( idsTarget );
		if( bOk ) {
			++numFounds;
		}
	}
	// 리스트에있는 모든 윈도우를 다 찾았을경우면 true한다. 누락되면 계속 찾기를 시도한다.
	if( m_spProp->m_aryIdsTarget.size() == numFounds ) {
		m_bTargeted = true;
	}
	return bOk;
}

bool XOrderIndicate::CreateIndicator( const std::string& idsTarget )
{
	bool bOk = false;
	if( idsTarget.empty() )
		return false;
	auto pWndTarget = GAME->Find( idsTarget );
	if( pWndTarget && pWndTarget->GetbEnable() ) {
// 		XE::VEC2 vPos = pWndTarget->GetPosScreen();
// 		if( vPos.x < 0 )
// 			vPos.x = 0;
// 		if( vPos.y < 0 )
// 			vPos.y = 0;
// 		XE::VEC2 vSize = pWndTarget->GetSizeFinal();
// 		if( m_spProp->m_dAng == 0 ) {	// right
// 			vPos.y += vSize.h / 2.f;
// 		} else
// 		if( m_spProp->m_dAng == 90.f ) {	// down
// 			vPos.x += vSize.w / 2.f;
// 		} else
// 		if( m_spProp->m_dAng == 180.f ) {// left
// 			vPos.x += vSize.w;
// 			vPos.y += vSize.h / 2.f;
// 		} else
// 		if( m_spProp->m_dAng == 270.f ) {	// up
// 			vPos.x += vSize.w / 2.f;
// 			vPos.y += vSize.h;
// 		}
		XWnd *pRoot = nullptr;
		if( m_spProp->m_idsRoot.empty() )
			pRoot = GAME->GetpScene();
		else
			pRoot = GAME->Find( m_spProp->m_idsRoot );
		if( XASSERT(pRoot) ) {
			bOk = CreateSpr( pRoot, idsTarget );
// 			bOk = CreateSpr( vPos, pRoot, idsTarget );
		}
	} else {
		_tstring str = C2SZ(idsTarget.c_str());
		CONSOLE("ids_target(%s)찾을수 없음.", str.c_str() );
	}
	return bOk;
}

void XOrderIndicate::Process( float dt )
{
	if( m_timerSec.IsOver() ) {
		if( CreateIndicator() == false ) {
			if( ++m_numFailed >= 3 )
				SetbEnd( true );	// 타겟을 3초간 못찾으면 그냥 끝냄.
		}
		m_timerSec.Reset();
	}
	// 시간되면 자동 종료
	if( m_timerFinish.IsOver() ) {
		// 이 인디케이터가 다른 order의 end가 종료시점일때
		if( GetspProp()->m_typeEnd == xFIN_END_ORDER ) {
			auto spOrderProp = m_spPropSeq->GetspOrderProp( GetspProp()->m_idsEnd );
			if( XASSERT(spOrderProp) ) {
				// 그 다른 order가 touch방식이 아닐때만 자동 사라짐.
				if( spOrderProp->m_typeEnd != xFIN_TOUCH ) {
					CONSOLE("XOrderIndicate::Process:시간초과로 자동 종료됨.");
					SetbEnd( true );
					m_timerFinish.Off();
				} else {
					m_timerFinish.Off();		// 더이상 검사하지 않도록.
				}
			}
		} else
		if( GetspProp()->m_typeEnd == xFIN_DELAY ) {
			m_timerFinish.Off();
			SetbEnd( true );
		}
	}
}

void XOrderIndicate::OnClickWnd( const std::string& idsClickedWnd )
{
	XOrder::OnClickWnd( idsClickedWnd );
	// 소멸조건이 특정버튼 눌림이면 종료를 통보한다.
	if( GetspProp()->m_typeEnd == xFIN_PUSH_TARGET ){
		for( auto& idsTarget : GetspProp()->m_aryIdsTarget ) {
			if( idsClickedWnd == idsTarget ) {
				// PUSH_TARGET속성일때만 이전상태로 돌린다.
// 				GAME->SetActive( GetbSceneActive() );	// 예전상태로 되돌림.
// 				GAME->GetlistAllowWnd().clear();
				SetbEnd( true );
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////////////
std::stack<XE::VEC2> XOrderCamera::s_stackPos;		// push_pos명령으로 저장된 좌표스택
XOrderCamera::XOrderCamera( XSeq *pSeq, int idxOrder )
	: XOrder( pSeq, idxOrder )
{
	Init();
	m_spProp = std::static_pointer_cast<XPropCamera>( pSeq->GetspProp()->m_aryOrder[ idxOrder ] );
	if( SCENE_WORLD ) {
		if( m_spProp->m_bPopPos ) {
			// 좌표 pop mode
			if( XASSERT(!s_stackPos.empty()) ) {
				// 좌표 복구.
				const XE::VEC2 vwCamera = s_stackPos.top();
				s_stackPos.pop();
				SCENE_WORLD->DoMovePosInWorld( vwCamera );
			}
		} else {
			if( m_spProp->m_bPushPos ) {
				// 좌표 백업
				s_stackPos.push( SCENE_WORLD->GetvwCamera() );
			}
			// 지정된 좌표로 이동
			if( m_spProp->m_aryIdsTarget.size() ) {
				// 카메라에서는 idsTarget을 하나만 쓴다.
 				auto idsTarget = m_spProp->m_aryIdsTarget[0];
				auto pWnd = GAME->Find( idsTarget );
				if( pWnd ) {
					auto vPos = pWnd->GetPosLocal();
					SCENE_WORLD->DoMovePosInWorld( vPos, 0.5f );
				} else {
					XBREAKF( 1, "%s not found", C2SZ(idsTarget) );
				}
			} else {
				SCENE_WORLD->DoMovePosInWorld( m_spProp->m_vwDst, 0.5f );
			}
		}
	} else
	if( SCENE_BATTLE ) {
		if( m_spProp->m_bPopPos ) {
			if( XASSERT( !s_stackPos.empty() ) ) {
				SCENE_BATTLE->DoMoveCamera( s_stackPos.top() );
				s_stackPos.pop();
			}
		} else {
			if( m_spProp->m_bPushPos ) {
				s_stackPos.push( SCENE_BATTLE->GetvwCamera() );
			}
			// 지정된 좌표로 이동
			XBREAK( m_spProp->m_vwDst.IsZero() );
			SCENE_BATTLE->DoMoveCamera( m_spProp->m_vwDst );
		}
	}
}

/**
 @brief 카메라 패닝이 끝남
*/
void XOrderCamera::OnEndPanning()
{
	SetbEnd( true );
}

void XOrderCamera::DispatchEvent( xHelp::xtEvent event )
{
	if( event == xHE_END_CAMERA_MOVING )
		SetbEnd( true );
}

////////////////////////////////////////////////////////////////
XOrderTouchLock::XOrderTouchLock( XSeq *pSeq, int idxOrder )
	: XOrder( pSeq, idxOrder )
{
	Init();
	m_spProp = std::static_pointer_cast<XPropTouch>( pSeq->GetspProp()->m_aryOrder[ idxOrder ] );
}
void XOrderTouchLock::OnCreate()
{
	bool bComplete = false;
	if( m_spProp->m_aryIdsTarget.size() == 0 ) {
		GAME->ClearidsAllow();
		GAME->SetActive( m_spProp->m_bFlag );
		bComplete = true;
	} else {
		bComplete = DoControlWnd();
		if( !bComplete )
			m_timerSec.Set( 1.f );
	}
	if( bComplete )
		// 실제 락이 성공했을때만 종료를 통보한다.
		SetbEnd( true );
}
bool XOrderTouchLock::DoControlWnd() 
{
	XBREAK( m_spProp->m_aryIdsTarget.size() == 0 );
	// 특정윈도우를 입력잠금하거나 푼다.
	int numFounds = 0;
	for( auto& idsTarget : m_spProp->m_aryIdsTarget ) {
		auto pWnd = GAME->Find( idsTarget );
		if( pWnd ) {
			pWnd->SetbActive( m_spProp->m_bFlag );
			++numFounds;
			if( m_spProp->m_bFlag == false )
				CONSOLE( "XOrderTouch::locked=%s", C2SZ(idsTarget) );
			else
				CONSOLE( "XOrderTouch::unlocked=%s", C2SZ(idsTarget) );
		}
	}
	if( numFounds == m_spProp->m_aryIdsTarget.size() ) {
		m_bControled = true;
	}
	return m_bControled;
}
void XOrderTouchLock::Process( float dt )
{
	if( m_bControled == false ) {
		// 여기서 실행한다는것은 OnCreate에서 실패하고 넘어간것이므로 
		// 1초마다 검사하면 그 사이에 잠시 눌릴수 있는 시간이 생겨서 즉시 검사하도록 바꿈.
		if( DoControlWnd() )	
			// 실제 수행이 성공했을때만 종료를 통보한다.
			SetbEnd( true );
		m_timerSec.Reset();
	}
}


////////////////////////////////////////////////////////////////
XOrderDelay::XOrderDelay( XSeq *pSeq, int idxOrder )
	: XOrder( pSeq, idxOrder )
{
	Init();
	m_spProp = std::static_pointer_cast<XPropDelay>( pSeq->GetspProp()->m_aryOrder[ idxOrder ] );
	m_Timer.Set( m_spProp->m_secDelay );
	if( !GAME->GetpRootSeq()->Find("spr.seq.gear") ) {
		auto psoGear = new XWndSprObj( _T( "ui_loading.spr" ), 1, XE::GetGameSize() / 2.f );
		psoGear->SetstrIdentifier( "spr.seq.gear" );
		GAME->GetpRootSeq()->Add( psoGear );
	}
}

void XOrderDelay::Destroy()
{
	GAME->GetpRootSeq()->DestroyWndByIdentifier( "spr.seq.gear" );
}

void XOrderDelay::Process( float dt )
{
	if( m_Timer.IsOver() )
		SetbEnd( true );
}
////////////////////////////////////////////////////////////////
XOrderMode::XOrderMode( XSeq *pSeq, int idxOrder )
	: XOrder( pSeq, idxOrder )
{
	Init();
	m_spProp = std::static_pointer_cast<XPropMode>( pSeq->GetspProp()->m_aryOrder[ idxOrder ] );
	if( m_spProp->m_strMode == "cut_scene" ) {
		auto pCurtain = static_cast<XWndDialogMode*>( GAME->Find("wnd.curtain") );
		if( pCurtain == nullptr ) {
			// 컷씬용 위아래 띠 모드
			pCurtain = new XWndDialogMode();
			pCurtain->SetstrIdentifier( "wnd.curtain" );
			GAME->Add( pCurtain );
		} 
		if( m_spProp->m_bFlag ) {
			pCurtain->DoCovering();
			GAME->ClearidsAllow();
			GAME->SetActive( false );
		} else {
			GAME->ClearidsAllow();
			GAME->SetActive( true );
			pCurtain->DoDecovering();
		}
	}
}
void XOrderMode::OnCreate()
{
	SetbEnd( true );
}

XE_NAMESPACE_END;




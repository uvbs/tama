#include "stdafx.h"
#include "XSeq.h"
#include "XPropOrder.h"
#include "XOrder.h"
#include "XPropSeq.h"
#include "XGame.h"
#include "xscenebase.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


XE_NAMESPACE_START( xHelp )
////////////////////////////////////////////////////////////////
XSeq::XSeq( PropSeqPtr& spProp )
{
	Init();
	m_spProp = spProp;
	///< 
	m_idxOrder = -1;
	if( GAME->GetpScene() ) {
		m_bSceneActive = GAME->GetpScene()->GetbActive() != FALSE;
		// 이제 명시적으로 touch on을 하지 않는이상, 디폴트로 블로킹되게 함.
		GAME->SetActive( false );	
	}
	XOrderDialog::s_vSavePos.Set( -1 );
	CreateOrder();
}

void XSeq::Destroy()
{
//	GAME->SetidsAllow( std::string() );
	GAME->ClearidsAllow();
	GAME->DestroyTutorialStopButton();
	if( GAME->GetpScene() ) {
#ifdef _DEBUG
		// 이런상황 자주 생기므로 삭제
// 		XBREAK( m_bSceneActive == false );
#endif // _DEBUG
		GAME->SetActive( m_bSceneActive );
	}
	for( auto spOrder : m_listOrder ) {
		spOrder->OnDestroy();
	}
}
/**
 @brief 외부에서 강제로 시퀀스를 삭제하고 모든 오더들이 생성한 윈도우들도 삭제시킨다.
*/
void XSeq::SetDestroyForce()
{
	for( auto& spOrder : m_listOrder ) {
		spOrder->OnDestroy();
	}
	SetbDestroy( TRUE );
}
int XSeq::Process( float dt )
{
	for( auto itor = m_listOrder.begin(); itor != m_listOrder.end(); ) {
		auto spOrder = ( *itor );
		if( spOrder->IsDestroy() ) {
			spOrder->OnDestroy();
			m_listOrder.erase( itor++ );
		} else {
			if( !spOrder->GetbEnd() )
				spOrder->Process( dt );
			else
				OnEndOrder( spOrder );
			// destroy
			if( spOrder->IsDestroy() ) {
				spOrder->OnDestroy();
				m_listOrder.erase( itor++ );
			} else
				++itor;
		}
	}
	if( m_bCreateOrder ) {
		m_bCreateOrder = false;
		CreateOrder();
	}
	if( m_bDestroySeq ) {
		if( m_listOrder.size() == 0 ) {
			GAME->OnEndSeq( this );
			SetbDestroy( TRUE );
			m_bDestroySeq = false;
		}
	}
	return 1;
}
OrderPtr XSeq::FindOrderObj( int idxOrder )
{
	for( auto spOrder : m_listOrder ) {
		if( !spOrder->IsDestroy() ) {
			if( spOrder->GetidxOrder() == idxOrder ) {
				return spOrder;
			}
		}
	}
	return OrderPtr();
}
OrderPtr XSeq::FindOrderObj( const std::string& idsOrder )
{
	for( auto spOrder : m_listOrder ) {
		if( !spOrder->IsDestroy() ) {
			if( spOrder->GetspProp()->m_strIdentifer == idsOrder ) {
				return spOrder;
			}
		}
	}
	return OrderPtr();
}

/**
 @brief spOrder의 실행이 종료됨.
 화살표생성등의 명령은 화살표객체만 생성시키면 곧바로 end가 호출되어 실행이 지나간다.
*/
void XSeq::OnEndOrder( OrderPtr& spOrder )
{
	for( auto spElem : m_listOrder ) {
		// 현재 실행중인 order객체가 소멸조건이 end_order일경우
		// 끝난 오더(spOrder)의 식별자와 같으면 삭제시킨다.
		if( !spOrder->IsDestroy() ) {
			if( spElem->GetspProp()->m_typeEnd == xFIN_END_ORDER ) {
				if( spElem->GetspProp()->m_idsEnd == spOrder->GetspProp()->m_strIdentifer ) {
					spElem->SetbDestroy( true );
				}
			}
		}
	}
	spOrder->SetbDestroy( true );
	m_bCreateOrder = true;
// 	CreateOrder();
}

void XSeq::CreateOrder()
{
	while( m_idxOrder < (int)m_spProp->m_aryOrder.size() ) {
		++m_idxOrder;
		if( m_idxOrder >= (int)m_spProp->m_aryOrder.size() ) {
			break;
		}
		auto spOrder = sCreateOrder( this, m_idxOrder );
		if( spOrder ) {
			CONSOLE("created sequence obj:%d", m_idxOrder);
			m_listOrder.Add( spOrder );
			spOrder->OnCreate();
			// NONE속성은 바로 다음 오더를 생성시킨다.
			auto typeEnd = spOrder->GetspProp()->m_typeEnd;
			bool bBreak = false;
			// none과 end_order는 비동기로 작동한다.
			if( !( typeEnd == xFIN_NONE || typeEnd == xFIN_END_ORDER) )
				bBreak = true;
			if( spOrder->GetspProp()->m_Type == xHO_READY_TOUCH )
				bBreak = true;
			if( bBreak )
				break;
		}
	}
	if( m_idxOrder >= (int)m_spProp->m_aryOrder.size() ) {
		m_bDestroySeq = true;
// 		if( GetNumActiveOrder() == 0 ) {
// 			GAME->OnEndSeq( this );
// 			SetbDestroy( TRUE );
// 		}
	}
}

/**
 @brief 활성화상태의 명령객체수를 얻는다.,
*/
int XSeq::GetNumActiveOrder()
{
	int num = 0;
	for( auto spOrder : m_listOrder ) {
		if( !spOrder->IsDestroy() )
			++num;
	}
	return num;
}

/**
 @brief 어떤 버튼이든지 눌리면 호출된다.
*/
void XSeq::OnClickWnd( const std::string& idsButt )
{
	// 현재 실행중인 명령객체중 소멸조건이 버튼눌림이면 삭제시킨다.
	for( auto spOrder : m_listOrder ) {
		if( !spOrder->IsDestroy() )
			spOrder->OnClickWnd( idsButt );
	}
}

std::string& XSeq::GetidsSeq() 
{
	return m_spProp->m_strIdentifier;
}

void XSeq::DispatchEvent( xHelp::xtEvent event )
{
	for( auto spOrder : m_listOrder ) {
		spOrder->DispatchEvent( event );
	}
}

XE_NAMESPACE_END;

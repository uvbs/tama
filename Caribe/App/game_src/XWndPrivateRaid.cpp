#include "stdafx.h"
#include "XLegion.h"
#include "XSquadron.h"
#include "XWndPrivateRaid.h"
#include "XSpotPrivateRaid.h"
#include "_Wnd2/XWndList.h"
#include "XWndStorageItemElem.h"
#include "XWndTemplate.h"
#include "XHero.h"
#include "XAccount.h"
#include "XFramework/client/XLayout.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XWndPrivateRaid::XWndPrivateRaid(XSpotPrivateRaid* pSpot)
	: XWndPopup( _T( "raid_private.xml" ), "popup" )
	, m_pSpot( pSpot )
{
	Init();
}

BOOL XWndPrivateRaid::OnCreate()
{
	return XWndPopup::OnCreate();
}

/**
 @brief 하단 보유 영웅중 하나를 클릭함.
 새 영웅추가: 하단리스트 클릭 => 출전리스트의 맨 뒤로 감
 출전영웅해제: 상단리스트 클릭(선택) => 같은영웅 한번더 클릭, 리스트에서 삭제
 영웅자리바꿈: 상단리스트에서 선택 => 바꿀 영웅을 클릭(교체)
 출전영웅교체: 상단영웅 선택 => 하단영웅 선택
*/
int XWndPrivateRaid::OnSelectHeroAtHave( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("%s", __TFUNC__);
	//
	auto pWndHero = SafeCast<XWndStoragyItemElem*>( pWnd->Find( p2 ) );
	if(	XASSERT(pWndHero) ) {
		XHero* pHero = ACCOUNT->GetpHeroBySN( pWndHero->GetsnHero() );
		if(	XASSERT(pHero) ) {
			m_pSpot->AddEnterHero( pHero, 0 );
			xSET_SHOW( pWndHero, "img.cover", true );
			SetbUpdate( true );
		}
	}
	return 1;
}

/**
 @brief 좌측의 출전 리스트영웅을 클릭
*/
int XWndPrivateRaid::OnClickedEnterHeroLeft( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("%s", __TFUNC__);
	//
	auto pWndList = SafeCast<XWndList*>( pWnd );
	const int idxSide = (int)p1;
	auto pCtrlHero = SafeCast<XWndStoragyItemElem*>( pWndList->Find( p2 ) );
	if( XASSERT( pCtrlHero ) ) {
		XHero* pHero = ACCOUNT->GetpHeroBySN( pCtrlHero->GetsnHero() );
		if( XASSERT( pHero ) ) {
			const std::string strKey = XE::Format( "list.%d", idxSide );
			if( m_pSpot->IsSelectedHero( pHero, idxSide ) ) {
				// 이미 선택이 되어있던것을 다시 클릭하면 삭제
				m_pSpot->DelEnterHero( pHero, idxSide );
				m_pSpot->SetSelectEnterHero( nullptr, idxSide );
			} else {
				// 아직 선택안되어 있던것이면 
				// 기존에 다른게 선택되어있었다.
				auto pHeroSelected = m_pSpot->GetSelectEnterHero( idxSide );
				if( pHeroSelected ) {
					XBREAK( pHeroSelected->GetsnHero() == pHero->GetsnHero() );
					// 이것은 선택되지 않은상태지만 다른게 이미 선택되어있었으면 스왑
					m_pSpot->ChangeEnterHero( pHero, pHeroSelected, idxSide );
					m_pSpot->SetSelectEnterHero( nullptr, idxSide );
				} else {
					m_pSpot->SetSelectEnterHero( pHero, idxSide );
				}
			}
			SetbUpdate( true );
		}
	}
	return 1;
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief 슬롯을 모두 빈슬롯으로 만든다.
*/
void XWndPrivateRaid::ClearEnterHeroes( const std::string& strKey )
{
	auto pWndList = xGET_LIST_CTRL( this, strKey );
	if( XASSERT(pWndList) ) {
		for( auto pElem : pWndList->GetlistElem() ) {
			auto pCtrlHero = SafeCast<XWndStoragyItemElem*>( pElem );
			if( XASSERT(pCtrlHero) ) {
				pCtrlHero->ClearHero();
				xSET_SHOW( pCtrlHero, "ctrl.unit", false );
			}
		}
	}

}

// void XWndPrivateRaid::DelWndList( const std::string& strKey, XHero* pHero )
// {
// 	auto pWnd = Find( strKey );
// 	if( pWnd ) {
// 		// heroCtrl에서 pHero를 모두 지운다.
// 		// 다시 갱신한다.
// 		pWnd->DestroyWndByIdentifierf( "elem.%08x", )
// 	}
// }


/**
 @brief 출전 영웅들을 UI에 배치
*/
void XWndPrivateRaid::UpdateEnterHeroes( XWndList* pWndList, int idxSide )
{
	// 전체 출전리스트를 모두 표시한다.
	XList4<XHero*> listEnterHero = m_pSpot->GetlistEnter( idxSide );
// 	// 군단이 있으면 군단영웅먼저 리스트에 넣는다.
// 	if( idxSide == 0 ) {
// 		for( auto& sq : m_pSpot->GetlegionDatPlayer().m_listSquad ) {
// 			auto pHero = ACCOUNT->GetpHeroBySN( sq.m_snHero );
// 			if( XASSERT(pHero) )
// 				listEnterHero.push_back( pHero );
// 		}
// 	} else {
// 		for( XSquadron* pSq : m_pSpot->GetspLegion()->GetlistSquadrons() ) {
// 			listEnterHero.push_back( pSq->GetpHero() );
// 		}
// 	}
	// 추가리스트에 있는것을 더한다.
// 	for( auto pHero : m_pSpot->GetlistEnter( idxSide ) ) {
// 		listEnterHero.push_back( pHero );
// 	}
// 	if( idxSide == 0 ) {
// 		for( auto snHero : m_pSpot->GetlistEnterPlayer() ) {
// 			auto pHero = ACCOUNT->GetpHeroBySN( snHero );
// 			listEnterHero.push_back( pHero );
// 		}
// 	} else {
// 		listEnterHero = m_pSpot->GetlistEnterEnemy();
// 	}
	XBREAK( listEnterHero.size() > 30 );
 	int i = 0;
	for( auto pHero : listEnterHero ) {
 		if( i > 30 )
 			break;
		auto pCtrlHero 
			= SafeCast<XWndStoragyItemElem*>( pWndList->Findf("elem.%d", i + 1 ) );
		if( XASSERT( pCtrlHero ) ) {
			if( XASSERT( pHero ) ) {
				pCtrlHero->SetHero( pHero );
				pCtrlHero->SetbSelected( m_pSpot->IsSelectedHero( pHero, idxSide ) );
				auto pWndUnit = ::xGetCtrlUnit2( pCtrlHero, "ctrl.unit" );
				if( XASSERT( pWndUnit ) ) {
					pWndUnit->SetUnit( pHero->GetUnit(), pHero->GetlevelSquad() );
					pWndUnit->SetbShow( true );
				}
			}
		}
		++i;
	}
}

/** ////////////////////////////////////////////////////////////////////////////////////
 @brief snHero로 elemd을 찾는다.
*/
XWndStoragyItemElem* 
XWndPrivateRaid::FindpHeroCtrl( XWndList* pWndList, ID snHero )
{
	for( auto pElem : pWndList->GetlistItems() ) {
		auto pCtrl = SafeCast<XWndStoragyItemElem*>( pElem );
		if( XASSERT(pCtrl) ) {
			if( pCtrl->GetsnHero() == snHero )
				return pCtrl;
		}
	}
	return nullptr;
}


void XWndPrivateRaid::Update()
{
	// 좌우 진영의 츨전 영웅들 리스트
	for( int i = 0; i < 2; ++i ) {
		const std::string strKey = XE::Format( "list.%d", i );
		ClearEnterHeroes( strKey );
		auto pWndList = xGET_LIST_CTRL( this, strKey );
		if( pWndList ) {
			pWndList->SetEvent( XWM_SELECT_ELEM, this,
													&XWndPrivateRaid::OnClickedEnterHeroLeft, i );
			UpdateEnterHeroes( pWndList, i );
		}
	}
// 	{
// 		auto pWndList = xGET_LIST_CTRL( this, "list.my" );
// 		if( pWndList ) {
// 			pWndList->SetEvent( XWM_SELECT_ELEM, this, &XWndPrivateRaid::OnClickedEnterHeroLeft, 0 );
// 			UpdateEnterHeroes( pWndList, m_pSpot->GetlistEnter() );
// 		}
// 	}
	// 보유한 영웅리스트를 하단에 표시한다.
	const auto spAcc = XAccount::sGetPlayerConst();
	if( spAcc ) {
//		int idx = 0;
		auto pWndList = xGET_LIST_CTRL( this, "list.have" );
		if( pWndList ) {
			pWndList->SetEvent( XWM_SELECT_ELEM, this, &XWndPrivateRaid::OnSelectHeroAtHave );
			// 보유한 영웅리스트를 계정에서 가져와 루프를 돈다
			for( const auto pHero : spAcc->GetlistHeroByInvenConst() ) {
				if( XASSERT( pHero ) ) {
//					++idx;
					// 영웅컨트롤을 꺼낸다.
					const std::string strKey = XE::Format( "elem.%08x", pHero->GetsnHero() );
					auto pCtrlHero = dynamic_cast<XWndStoragyItemElem*>( pWndList->Find( strKey ));
					if( pCtrlHero == nullptr ) {
						XWnd* pWnd = GetpLayout()->CreateWnd( "elem", pWndList );
						pCtrlHero = dynamic_cast<XWndStoragyItemElem*>( pWnd );
						if( XASSERT( pCtrlHero ) ) {
							pCtrlHero->SetstrIdentifier( strKey );
							pWndList->AddItem( pCtrlHero );
						}
					}
					// 영웅을 지정
					pCtrlHero->SetHero( pHero );
					pCtrlHero->SetbSelected( m_pSpot->IsSelectedHero( pHero, 0 ) );
					// 유닛을 지정
					auto pCtrlUnit = ::xGetCtrlUnit2( pCtrlHero, "ctrl.unit" );
					if( XASSERT(pCtrlUnit) ) {
						pCtrlUnit->SetUnit( pHero->GetUnit(), pHero->GetlevelSquad() );
						xSET_SHOW( pCtrlHero, "ctrl.unit", true );
					}
					// 출전중인 영웅은 표시를 한다. (아군)
					const bool bEnter = m_pSpot->IsExistEnterHero( pHero, 0 );
					xSET_SHOW( pCtrlHero, "img.cover", bEnter );
				}
			}
		}
	}
	XWndPopup::Update();
}


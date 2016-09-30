#include "stdafx.h"
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
	{
		auto pWndList = xGET_LIST_CTRL( this, "list.my" );
		if( pWndList ) {
			XList4<XHero*> listEnterHeroes;
			SetEnterHeroes( pWndList, listEnterHeroes );
		}
	}
	const auto spAcc = XAccount::sGetPlayerConst();
	if( spAcc ) {
		int idx = 0;
		auto pWndList = xGET_LIST_CTRL( this, "list.have" );
		for( const auto pHero : spAcc->GetlistHeroByInvenConst() ) {
			if( XASSERT(pHero) ) {
				++idx;
				XWnd* pWnd = GetpLayout()->CreateWnd( "elem", pWndList );
				auto pCtrlHero = dynamic_cast<XWndStoragyItemElem*>( pWnd );
				if( XASSERT(pCtrlHero) ) {
					pCtrlHero->SetHero( pHero );
					const std::string strKey = XE::Format("%s.%s%d", 
																								 pWnd->GetstrIdentifier().c_str(), 
																								 pCtrlHero->GetstrIdentifier().c_str(), 
																								 idx );
					pCtrlHero->SetstrIdentifier( strKey );
					pWndList->AddItem( pCtrlHero );
				}
			}
		}
	}

	return XWndPopup::OnCreate();
}

/**
 @brief 출전 영웅들을 UI에 배치
*/
void XWndPrivateRaid::SetEnterHeroes( XWndList* pList, XList4<XHero*> listHero )
{
	XBREAK( listHero.size() > 30 );
	const std::string& strKeyHead = pList->GetstrIdentifier(); 
	int i = 0;
	for( auto pHero : listHero ) {
		if( i > 30 )
			break;
		auto pWnd = pList->Findf("%s.elem.%d", strKeyHead.c_str(), ++i);
		if( XASSERT(pWnd) ) {
			auto pWndHero = SafeCast2<XWndStoragyItemElem*>( pWnd );
			if( XASSERT(pWndHero) ) {
				if( XASSERT( pHero ) ) {
					pWndHero->SetHero( pHero );
					auto pWndUnit = SafeCast2<XWndCircleUnit*>( pWndHero->Find( "ctrl.unit" ) );
					if( XASSERT(pWndUnit) ) {
						pWndUnit->SetUnit( pHero->GetUnit() );
					}
				}
			}
		}
	}
}


void XWndPrivateRaid::Update()
{
	XWndPopup::Update();
}


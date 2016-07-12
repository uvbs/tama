#include "StdAfx.h"
#include "XSceneStorage.h"
#include "XGameEtc.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"

//#include "XGame.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XSceneArmory.h"
#include "XHero.h"
#include "XWndTemplate.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneStorage *SCENE_STORAGE = NULL;

void XSceneStorage::Destroy()
{
	XBREAK( SCENE_STORAGE == NULL );
	XBREAK( SCENE_STORAGE != this );
	SCENE_STORAGE = NULL;
}

XSceneStorage::XSceneStorage( XGame *pGame )
	: XSceneBase( pGame, XGAME::xSC_STORAGE )
	, m_Layout( _T( "layout_storage.xml" ) )
	, m_LayoutTooltip( _T( "layout_item.xml" ) )
{
	XBREAK( SCENE_STORAGE != NULL );
	SCENE_STORAGE = this;
	Init();

	// ui 로드
	m_Layout.CreateLayout( "common_bg", this );
	//	m_Layout.CreateLayout("common_bg_goldcash", this);
	m_Layout.CreateLayout( "storage", this );
	m_Layout.CreateLayout( "select_item_info", this );

	// 텍스트
	// 	xSET_TEXT(this, "text.common.gold", XE::NumberToMoneyString(ACCOUNT->GetGold()));
	// 	xSET_TEXT(this, "text.common.cash", XE::NumberToMoneyString(ACCOUNT->GetCashtem()));

	// 버튼
	xSET_BUTT_HANDLER( this, "butt.back", &XSceneStorage::OnBack );

	// 전체, 장비, 전리품, 기타 버튼
	auto pButt = xSET_BUTT_HANDLER( this, "butt.tab.all", &XSceneStorage::OnClickTabAll );
	pButt->SetPush( TRUE );
	xSET_BUTT_HANDLER( this, "butt.tab.equipment", &XSceneStorage::OnClickTabEquipment );
	xSET_BUTT_HANDLER( this, "butt.tab.trophy", &XSceneStorage::OnClickTabTrophy );
	xSET_BUTT_HANDLER( this, "butt.tab.etc", &XSceneStorage::OnClickTabEtc );

	SetbUpdate( true );		// Update()가 호출 됩니다.
	UpdateAll();
}

void XSceneStorage::Create( void )
{
	XEBaseScene::Create();
}

void XSceneStorage::Update()
{
	XGAME::CreateUpdateTopResource( this );
	XEBaseScene::Update();
}

int XSceneStorage::Process( float dt )
{
	return XEBaseScene::Process( dt );
}

void XSceneStorage::Draw( void )
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneStorage::OnLButtonDown( float lx, float ly )
{
	XEBaseScene::OnLButtonDown( lx, ly );
}

void XSceneStorage::OnLButtonUp( float lx, float ly )
{
	XEBaseScene::OnLButtonUp( lx, ly );
}

void XSceneStorage::OnMouseMove( float lx, float ly )
{
	XEBaseScene::OnMouseMove( lx, ly );
}

//////////////////////////////////////////////////////////////////////////

int XSceneStorage::OnBack( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	DoExit( XGAME::xSC_WORLD );

	return 1;
}

int XSceneStorage::OnClickTabAll( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	UpdateAll();

	return 1;
}

int XSceneStorage::OnClickTabEquipment( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	UpdateEquipment();

	return 1;
}

int XSceneStorage::OnClickTabTrophy( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	UpdateTrophy();

	return 1;
}

int XSceneStorage::OnClickTabEtc( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	UpdateEtc();

	return 1;
}

int XSceneStorage::OnClickStorageListSlot( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	// 클릭한 아이템
	ID idItem = p2;
	auto pElem = dynamic_cast<XWndStoragyItemElem*>( pWnd->Find( idItem ) );
	if( pElem ) {
		// 선택한 아이템이 아니면서 아이템 정보가 있을 때 선택 가능
		if( pElem != m_pSelectItem && pElem->GetpItem() != NULL ) {
			// 선택 표시 업데이트
			if( m_pSelectItem )
				m_pSelectItem->SetbSelected( false );		// 원래 선택된 아이템 선택 표시 꺼주기
			m_pSelectItem = pElem;
			m_pSelectItem->SetbSelected( true );		// 선택 표시 켜주기
			//			m_pSelectItem->SetbGlowAnimation( true );
			UpdateSelItemInfo();		// 오른쪽 아이템 정보 업데이트
			if( m_pSelectItem->GetpItem()->GetType() == XGAME::xIT_SOUL )
				xSET_SHOW( this, "img.soulstone", true );
			else
				xSET_SHOW( this, "img.soulstone", false );
			pElem->SetbUpdate( true );
		}
	}
	return 1;
}

int XSceneStorage::OnClickEquipmentSell( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	// 팝업
	m_Layout.CreateLayout( "commonitemtextpopup", this );

	XWnd *pPopup = Find( "img.common.popupitemtext" );
	if( pPopup ) {
		pPopup->SetbModal( TRUE );
		xSET_TEXT( this, "text.title", XTEXT( 80015 ) );			// "판매"
		xSET_TEXT( this, "text.common.ask", XTEXT( 2003 ) );		// "정말로 판매하시겠습니까?"
		xSET_BUTT_TEXT( this, "butt.ok", XTEXT( 80015 ) );			// "판매"

		//xSET_IMG(this, "img.ask.itemimg", m_pSelectItem->GetpItem()->GetResPathIcon());
		XWndImage *pImg = SafeCast<XWndImage*, XWnd*>( Find( "img.ask.itemimg" ) );
		if( pImg ) {
			pImg->SetSurface( m_pSelectItem->GetpItem()->GetResPathIcon() );
			if( m_pSelectItem->GetpItem()->GetType() == XGAME::xIT_SOUL ) {
				pImg->SetScaleLocal( 0.76f );
				XWndImage *pSoulStone = new XWndImage( TRUE, XE::MakePath( DIR_UI, _T( "gem_small.png" ) ), 241, 129 );
				pPopup->Add( pSoulStone );
			} else
				pImg->SetScaleLocal( 1.f );
		}
		xSET_BUTT_HANDLER( this, "butt.ok", &XSceneStorage::OnClickSell );
		xSET_BUTT_HANDLER( this, "butt.cancel", &XSceneStorage::OnClickPopupCancel );
	}

	return 1;
}

int XSceneStorage::OnClickSell( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	if( m_pSelectItem == nullptr )
		return 1;
	auto pHero = ACCOUNT->GetHeroByEquip( m_pSelectItem->GetsnItem() );
	if( pHero ) {
		_tstring strSrc = XE::Format( XTEXT( 2087 ), pHero->GetstrName().c_str() );
		TCHAR szBuff[1024];
		XE::ConvertJosaStr( szBuff, strSrc.c_str() );
		XWND_ALERT( "%s", szBuff );
		return 1;
	}
	GAMESVR_SOCKET->SendItemSell( pWnd, m_pSelectItem->GetsnItem() );

	OnClickPopupCancel( pWnd, 0, 0 );

	return 1;
}

int XSceneStorage::OnClickEtcUse( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	// 팝업
	XWnd *pPopup = new XWndPopup( m_Layout.GetpLayout(), "usepopup", nullptr );
	Add( pPopup );

	if( pPopup ) {
		pPopup->SetbModal( TRUE );

		xSET_TEXT( this, "text.storage.ask", XTEXT( 2004 ) );		// "정말로 사용하시겠습니까?"

		xSET_IMG( this, "img.ask.itemimg", m_pSelectItem->GetpItem()->GetResPathIcon() );

		xSET_BUTT_HANDLER( this, "butt.ok", &XSceneStorage::OnClickUse );
		xSET_BUTT_HANDLER( this, "butt.cancel", &XSceneStorage::OnClickPopupCancel );
	}

	return 1;
}

int XSceneStorage::OnClickUse( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	if( GAMESVR_SOCKET->SendItemSpent( pWnd, m_pSelectItem->GetsnItem() ) )
		pWnd->SetbEnable( FALSE );

	OnClickPopupCancel( pWnd, 0, 0 );

	return 1;
}

int XSceneStorage::OnClickLock( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	int keyItemID = XGC->m_storageLockItem;		// 키 아이템
	unsigned int gemNum = XGC->m_storageLockGem;	// 소모되는 젬량

	bool bKey = false;

	if( ACCOUNT ) {
		// 창고 한계 검사
		if( XGAME::ITEM_MAX_COUNT <= ACCOUNT->GetmaxItems() ) {
			// 팝업
			m_Layout.CreateLayout( "commontextpopup", this );

			XWnd *pPopup = Find( "img.common.popuptext" );
			if( pPopup ) {
				pPopup->SetbModal( TRUE );

				xSET_TEXT( this, "text.title", XTEXT( 80019 ) );			// "확장"
				xSET_TEXT( this, "text.common.ask", XTEXT( 80166 ) );		// "창고 확장 한계에 도달했습니다"
				//xSET_BUTT_TEXT(this, "butt.ok1", XTEXT(2));			// "확인"

				xSET_SHOW( this, "butt.ok1", TRUE );
				xSET_SHOW( this, "butt.ok", FALSE );
				xSET_SHOW( this, "butt.cancel", FALSE );

				xSET_BUTT_HANDLER( this, "butt.ok1", &XSceneStorage::OnClickPopupCancel );

				//xSET_BUTT_HANDLER(this, "butt.ok", &XSceneStorage::OnClickPopupCancel);
				//xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneStorage::OnClickPopupCancel);
			}
		} else {
			XList4<XBaseItem*> itemList;
			ACCOUNT->GetInvenItem( itemList );		// 인벤토리 얻어오기

			int i = 0;
			for( auto pItem : itemList ) {
				if( pItem->GetidProp() == keyItemID ) {
					bKey = true;
					break;
				}
				++i;
			}

			if( bKey ) {
				// 팝업
				m_Layout.CreateLayout( "commonitemtextpopup", this );

				XWnd *pPopup = Find( "img.common.popupitemtext" );
				if( pPopup ) {
					pPopup->SetbModal( TRUE );

					xSET_TEXT( this, "text.title", XTEXT( 80019 ) );			// "확장"
					xSET_TEXT( this, "text.common.ask", XTEXT( 80059 ) );		// "열쇠를 소비하여 창고 공간을 확장합니다. 확장하시겠습니까?"
					xSET_BUTT_TEXT( this, "butt.ok", XTEXT( 80019 ) );			// "확장"

					xSET_IMG( this, "img.ask.itemimg", XE::MakePath( DIR_IMG, PROP_ITEM->GetpProp( keyItemID )->strIcon.c_str() ) );

					xSET_BUTT_HANDLER_PARAM( this, "butt.ok", this, &XSceneStorage::OnClickLockOK, XGAME::xSC_SPENT_ITEM );
					xSET_BUTT_HANDLER( this, "butt.cancel", &XSceneStorage::OnClickPopupCancel );
				}
			} else {
				if( ACCOUNT->GetCashtem() >= 5 ) {
					m_Layout.CreateLayout( "commonitemtextpopup", this );

					XWnd *pPopup = Find( "img.common.popupitemtext" );
					if( pPopup ) {
						pPopup->SetbModal( TRUE );

						xSET_TEXT( this, "text.title", XTEXT( 80019 ) );			// "확장"
						xSET_TEXT( this, "text.common.ask", XE::Format( XTEXT( 80060 ), gemNum ) );		// "열쇠가 부족합니다. 캐쉬 %d개를 소모하여 공간을 확장하시겠습니까?"
						xSET_BUTT_TEXT( this, "butt.ok", XTEXT( 80019 ) );			// "확장"

						xSET_IMG( this, "img.ask.itemimg", XE::MakePath( DIR_IMG, PROP_ITEM->GetpProp( keyItemID )->strIcon.c_str() ) );

						xSET_BUTT_HANDLER_PARAM( this, "butt.ok", this, &XSceneStorage::OnClickLockOK, XGAME::xSC_SPENT_GEM );
						xSET_BUTT_HANDLER( this, "butt.cancel", &XSceneStorage::OnClickPopupCancel );
					}
				} else {
					// 키 아이템도 없고 캐쉬도 부족

					// 팝업
					m_Layout.CreateLayout( "commontextpopup", this );

					XWnd *pPopup = Find( "img.common.popuptext" );
					if( pPopup ) {
						pPopup->SetbModal( TRUE );

						xSET_TEXT( this, "text.title", XTEXT( 80019 ) );			// "확장"
						xSET_TEXT( this, "text.common.ask", XTEXT( 80150 ) );		// "아이템과 캐쉬가 부족합니다. 구매해주세요"
						//xSET_BUTT_TEXT(this, "butt.ok1", XTEXT(2));			// "확인"

						xSET_SHOW( this, "butt.ok1", TRUE );
						xSET_SHOW( this, "butt.ok", FALSE );
						xSET_SHOW( this, "butt.cancel", FALSE );

						xSET_BUTT_HANDLER( this, "butt.ok1", &XSceneStorage::OnClickPopupCancel );

						//xSET_BUTT_HANDLER(this, "butt.ok", &XSceneStorage::OnClickPopupCancel);
						//xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneStorage::OnClickPopupCancel);
					}
				}
			}
		}
	}
	return 1;
}

int XSceneStorage::OnClickLockOK( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	if( GAMESVR_SOCKET ) {
		if( ( XGAME::xtSpentCall )p1 == XGAME::xSC_SPENT_ITEM ) {
			int numItem = ACCOUNT->GetNumItems( XGC->m_storageLockItem );
			if( numItem <= 0 ) {
				XWND_ALERT( "%s", XTEXT( 2002 ) );
				return 1;
			}
			GAMESVR_SOCKET->SendInventoryExpand( this, ( XGAME::xtSpentCall )p1 );
		} else if( ( XGAME::xtSpentCall )p1 == XGAME::xSC_SPENT_GEM ) {
			if( !ACCOUNT->IsEnoughCash( XGC->m_storageLockGem ) ) {
				XWND_ALERT( "%s", XTEXT( 80140 ) );
				return 1;
			}
			GAMESVR_SOCKET->SendInventoryExpand( this, ( XGAME::xtSpentCall )p1 );
		}
	}

	OnClickPopupCancel( pWnd, 0, 0 );

	return 1;
}

int XSceneStorage::OnClickPopupCancel( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	if( pWnd->GetpParent() )
		pWnd->GetpParent()->SetbDestroy( TRUE );

	return 1;
}

void XSceneStorage::UpdateAll()
{
	m_tab = ALL;

	XWnd *pAll = Find( "wnd.storage.select_item_info" );
	if( pAll )
		pAll->SetbDestroy( TRUE );

	m_Layout.CreateLayout( "select_item_info", this );
	XWnd *pRoot = Find( "wnd.item.tooltip" );
	if( pRoot ) {
		// 아이템 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "item_tooltip", pRoot );
	}

	int num = 0;
	// List
	XWndList *pWndList = SafeCast<XWndList*, XWnd*>( Find( "list.storage" ) );
	if( pWndList ) {
		pWndList->DestroyAllItem();

		bool first = true;
		XList4<XBaseItem*> listItem;
		ACCOUNT->GetInvenItem( listItem );
		for( auto pItem : listItem ) {
			auto pElem = new XWndStoragyItemElem( pItem );
			if( pItem->GetNum() == 1 )
				pElem->SetNum( 0 );
			else
				pElem->SetNum( pItem->GetNum() );
			pWndList->AddItem( pItem->GetsnItem(), pElem );
			if( first ) {
				m_pSelectItem = pElem;
				m_pSelectItem->SetbSelected( true );
				//				m_pSelectItem->SetbGlowAnimation( true );
				m_pSelectItem->SetbUpdate( true );
				UpdateSelItemInfo();
				first = false;
			}
			if( ACCOUNT->GetmaxItems() <= num )
				pElem->SetSlotLock();

			num++;
		}

		// 표시 해야될 슬롯 갯수. 현재 5의 공배수
		int allSlotNum = 0;
		if( num > 0 ) {
			allSlotNum = num / 5 * 5;
			if( num % 5 > 0 )
				allSlotNum += 5;
		} else
			allSlotNum = 0;

		if( ACCOUNT->GetmaxItems() > allSlotNum )
			allSlotNum = ACCOUNT->GetmaxItems();
		// 빈슬롯
		for( int i = num; i < allSlotNum; i++ ) {
			int temp = ACCOUNT->GetmaxItems();
			auto pElem = new XWndStoragyItemElem( (XBaseItem *)NULL );
			pElem->SetNum( 0 );
			pWndList->AddItem( 0, pElem );

			if( ACCOUNT->GetmaxItems() <= i )
				pElem->SetSlotLock();
		}

		// 확장 한계 검사
		if( XGAME::ITEM_MAX_COUNT > ACCOUNT->GetmaxItems() ) {
			auto pElem = new XWndStoragyItemElem( (XBaseItem *)NULL );
			if( pElem ) {
				pElem->SetLockButt();
				pElem->SetNum( 0 );
				XWndButton *pButt = pElem->GetLockButt();
				if( pButt )
					pButt->SetEvent( XWM_CLICKED, this, &XSceneStorage::OnClickLock );
				pElem->SetstrIdentifier( "lock" );
				pWndList->AddItem( 0, pElem );
				pElem->SetSizeLocal( pButt->GetWidthLocal(), pButt->GetHeightLocal() );
			}
		}
	}

	pWndList->SetEvent( XWM_SELECT_ELEM, this, &XSceneStorage::OnClickStorageListSlot );
	pWndList->SetScrollDir( XE::xVERT );

	xSET_SHOW( this, "text.storage.title", num > 0 );
	if( m_pSelectItem )
		xSET_TEXT( this, "text.storage.title", XTEXT( 80031 ) );

	XWndButton *pButt = dynamic_cast<XWndButton*>( Find( "butt.storage.sell" ) );
	if( pButt )
		pButt->SetEvent( XWM_CLICKED, this, &XSceneStorage::OnClickEquipmentSell );

	pButt = dynamic_cast<XWndButton*>( Find( "butt.storage.use" ) );
	if( pButt )
		pButt->SetEvent( XWM_CLICKED, this, &XSceneStorage::OnClickEtcUse );
}

void XSceneStorage::UpdateEmptySlot( XWndList *pWndList )
{

}

void XSceneStorage::UpdateEquipment()
{
	m_tab = EQUIP;

	XWnd *pAll = Find( "wnd.storage.select_item_info" );
	if( pAll )
		pAll->SetbDestroy( TRUE );

	m_Layout.CreateLayout( "select_item_info", this );
	XWnd *pRoot = Find( "wnd.item.tooltip" );
	if( pRoot ) {
		// 아이템 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "item_tooltip", pRoot );
	}

	// List
	int num = FilterList( EQUIP );
	XWndList *pWndList = SafeCast<XWndList*, XWnd*>( Find( "list.storage" ) );
	if( pWndList ) {
		pWndList->SetEvent( XWM_SELECT_ELEM, this, &XSceneStorage::OnClickStorageListSlot );
		pWndList->SetScrollDir( XE::xVERT );
	}

	XWndButton *pButt = dynamic_cast<XWndButton*>( Find( "butt.storage.sell" ) );
	if( pButt )
		pButt->SetEvent( XWM_CLICKED, this, &XSceneStorage::OnClickEquipmentSell );

	xSET_SHOW( this, "text.storage.title", num > 0 );
	if( m_pSelectItem )
		xSET_TEXT( this, "text.storage.title", XTEXT( 80002 ) );

	// 	xSET_SHOW(this, "storage.parchment1", FALSE);
	// 	xSET_SHOW(this, "storage.parchment2", TRUE);
}

void XSceneStorage::UpdateTrophy()
{
	m_tab = TROPHY;

	XWnd *pAll = Find( "wnd.storage.select_item_info" );
	if( pAll )
		pAll->SetbDestroy( TRUE );

	m_Layout.CreateLayout( "select_item_info", this );
	XWnd *pRoot = Find( "wnd.item.tooltip" );
	if( pRoot ) {
		// 아이템 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "item_tooltip", pRoot );
	}

	// List
	int num = FilterList( TROPHY );
	XWndList *pWndList = SafeCast<XWndList*, XWnd*>( Find( "list.storage" ) );
	if( pWndList ) {
		pWndList->SetEvent( XWM_SELECT_ELEM, this, &XSceneStorage::OnClickStorageListSlot );
		pWndList->SetScrollDir( XE::xVERT );
	}
	xSET_SHOW( this, "text.storage.title", num > 0 );
	if( m_pSelectItem )
		xSET_TEXT( this, "text.storage.title", XTEXT( 80013 ) );

	// 	xSET_SHOW(this, "storage.parchment1", TRUE);
	// 	xSET_SHOW(this, "storage.parchment2", FALSE);
}

void XSceneStorage::UpdateEtc()
{
	m_tab = ETC;

	XWnd *pAll = Find( "wnd.storage.select_item_info" );
	if( pAll )
		pAll->SetbDestroy( TRUE );

	m_Layout.CreateLayout( "select_item_info", this );
	XWnd *pRoot = Find( "wnd.item.tooltip" );
	if( pRoot ) {
		// 아이템 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "item_tooltip", pRoot );
	}

	// List
	int num = FilterList( ETC );
	XWndList *pWndList = SafeCast<XWndList*, XWnd*>( Find( "list.storage" ) );
	if( pWndList ) {
		pWndList->SetEvent( XWM_SELECT_ELEM, this, &XSceneStorage::OnClickStorageListSlot );
		pWndList->SetScrollDir( XE::xVERT );
	}

	XWndButton *pButt = dynamic_cast<XWndButton*>( Find( "butt.storage.use" ) );
	if( pButt )
		pButt->SetEvent( XWM_CLICKED, this, &XSceneStorage::OnClickEtcUse );

	xSET_SHOW( this, "text.storage.title", num > 0 );
	if( m_pSelectItem )
		xSET_TEXT( this, "text.storage.title", XTEXT( 80014 ) );

	// 	xSET_SHOW(this, "storage.parchment1", TRUE);
	// 	xSET_SHOW(this, "storage.parchment2", FALSE);
}

void XSceneStorage::UpdateSelItemInfo()
{
	XWnd *pAll = Find( "wnd.storage.select_item_info" );
	if( pAll )
		pAll->SetbDestroy( TRUE );
	if( m_pSelectItem == nullptr )
		return;
	if( m_pSelectItem->GetpItem() == nullptr )
		return;
	auto pProp = m_pSelectItem->getpProp();
	if( pProp == nullptr )
		return;
	XBREAK( XGC->m_rateShopSell == 0 || XGC->m_rateShopSell > 1.f );
	//	DWORD cost = pProp->GetSellCost( ACCOUNT->GetLevel() );
	//	int costSell = (int)( pProp->GetBuyCost( ACCOUNT->GetLevel() ) * XGC->m_rateShopSell );
	int costSell = (int)( pProp->GetSellCost( ACCOUNT->GetLevel() ) );
	//
	m_Layout.CreateLayout( "select_item_info", this );
	if( pProp->IsSoul() ) {
		xSET_SHOW( this, "img.bg.hero.tooltip", true );
		xSET_SHOW( this, "storage.parchment", false );
		XWnd *pRoot = Find( "wnd.hero.tooltip" );
		if( pRoot ) {
			// 툴팁 레이아웃모듈을 읽는다.
			m_LayoutTooltip.CreateLayout( "hero_tooltip", pRoot );
			pRoot->SetbShow( true );
			XGAME::UpdateHeroTooltip( pProp->strIdHero, pRoot, costSell );
		}
	} else {
		xSET_SHOW( this, "img.bg.hero.tooltip", false );
		xSET_SHOW( this, "storage.parchment", true );
		XWnd *pRoot = Find( "wnd.item.tooltip" );
		if( pRoot ) {
			// 툴팁 레이아웃모듈을 읽는다.
			m_LayoutTooltip.CreateLayout( "item_tooltip", pRoot );
			pRoot->SetbShow( true );
			XGAME::UpdateItemTooltip( pProp, pRoot, costSell );
		}
	}
	xSET_SHOW( this, "text.storage.title", m_pSelectItem != nullptr );
	if( m_tab == ALL )
		xSET_TEXT( this, "text.storage.title", XTEXT( 80031 ) );		// "전체"
	else if( m_tab == EQUIP )
		xSET_TEXT( this, "text.storage.title", XTEXT( 80002 ) );		// "장비"
	else if( m_tab == TROPHY )
		xSET_TEXT( this, "text.storage.title", XTEXT( 80013 ) );		// "전리품"
	else if( m_tab == ETC )
		xSET_TEXT( this, "text.storage.title", XTEXT( 80014 ) );		// "기타"
	// 판매 버튼 나타나게 함.
	XArrayLinearN<XWnd*, 256> ary;
	auto type = m_pSelectItem->GetpItem()->GetType();
	// 아래종류템이 아니면 다 팔수 있다.
	if( type != XGAME::xIT_QUEST && type != XGAME::xIT_EVENT
		&& type != XGAME::xIT_CASH ) {
		auto pButt = dynamic_cast<XWndButton*>( Find( "butt.storage.sell" ) );
		if( pButt )
			pButt->SetEvent( XWM_CLICKED, this, &XSceneStorage::OnClickEquipmentSell );
		xSET_SHOW( this, "butt.storage.sell", TRUE );
		ary.Add( pButt );
		// 모든 판매가격은 1/10
	} else {
		xSET_SHOW( this, "butt.storage.sell", FALSE );
		costSell = 0;
	}
	// 사용 버튼 나타나게 함.
	// 		if( m_pSelectItem->GetpItem()->GetpProp()->useType == XGAME::xIU_USE )
	if( pProp->useType == XGAME::xIU_USE ) {
		auto pUseButt = xSET_SHOW( this, "butt.storage.use", TRUE );
		if( pUseButt ) {
			pUseButt->SetEvent( XWM_CLICKED, this, &XSceneStorage::OnClickEtcUse );
			ary.Add( pUseButt );
		}
	} else
		xSET_SHOW( this, "butt.storage.use", FALSE );

	// 	XGAME::UpdateItemTooltip( pProp, pRoot, costSell );
	// 버튼들 정렬
	auto pRightPanel = Find( "wnd.storage.select_item_info" );
	if( pRightPanel && ary.size() > 0 )
		pRightPanel->AutoLayoutHCenterWithAry( ary, 20.f );
}

void XSceneStorage::RecvUpdate()
{
	if( m_tab == ALL )
		UpdateAll();
	else if( m_tab == EQUIP )
		UpdateEquipment();
	else if( m_tab == TROPHY )
		UpdateTrophy();
	else if( m_tab == ETC )
		UpdateEtc();

	// 코드 중복되지 않도록 할것.
	// 	xSET_TEXT(this, "text.common.gold", XE::NumberToMoneyString(ACCOUNT->GetGold()));
	// 	xSET_TEXT(this, "text.common.cash", XE::NumberToMoneyString(ACCOUNT->GetCashtem()));
}

int XSceneStorage::FilterList( eTab tabType )
{
	int numFiltered = 0;
	XWndList *pWndList = SafeCast<XWndList*, XWnd*>( Find( "list.storage" ) );
	if( pWndList ) {
		pWndList->DestroyAllItem();
		bool first = true;
		XList4<XBaseItem*> listItem;
		ACCOUNT->GetInvenItem( listItem );
		int num = 0;
		for( auto pItem : listItem ) {
			num++;		// 중간에 continue 땜시 맨 앞으로
			bool bInclude = false;
			if( pItem->GetType() == XGAME::xtItem::xIT_MEDAL
				|| pItem->GetType() == XGAME::xtItem::xIT_SCALP
				|| pItem->GetType() == XGAME::xtItem::xIT_SCROLL ) {
				if( tabType == TROPHY )
					bInclude = true;

			} else
				if( pItem->GetType() == XGAME::xtItem::xIT_EQUIP ) {
					if( tabType == EQUIP )
						bInclude = true;
				} else {
					if( tabType == ETC )
						bInclude = true;	// 나머지 모든종류
				}

			if( bInclude ) {
				++numFiltered;
				auto pElem = new XWndStoragyItemElem( pItem );
				pWndList->AddItem( pItem->GetsnItem(), pElem );
				// 첫번째 아이템이면
				if( first ) {
					m_pSelectItem = pElem;
					m_pSelectItem->SetbSelected( true );
					//					m_pSelectItem->SetbGlowAnimation( true );
					m_pSelectItem->SetbUpdate( true );
					UpdateSelItemInfo();
					if( m_pSelectItem->GetpItem()->GetType() == XGAME::xIT_SOUL )
						xSET_SHOW( this, "img.soulstone", true );
					else
						xSET_SHOW( this, "img.soulstone", false );
					first = false;
				}
				if( ACCOUNT->GetmaxItems() < num )
					pElem->SetSlotLock();
				// 새로운 아이템이면
				if( pItem->GetbNew() ) {
				}
			}
		}
		int allSlotNum = 0;
		if( num > 0 ) {
			allSlotNum = num / 5 * 5;
			if( num % 5 > 0 )
				allSlotNum += 5;
		} else
			allSlotNum = 0;
		if( ACCOUNT->GetmaxItems() > allSlotNum )
			allSlotNum = ACCOUNT->GetmaxItems();
		for( int i = num; i < allSlotNum; i++ ) {
			int temp = ACCOUNT->GetmaxItems();
			auto pElem = new XWndStoragyItemElem( (XBaseItem *)NULL );
			pElem->SetNum( 0 );
			pWndList->AddItem( 0, pElem );
			if( ACCOUNT->GetmaxItems() <= i )
				pElem->SetSlotLock();
		}
	} // pWndList
	return numFiltered;
}


#include "StdAfx.h"
#include "XSceneArmory.h"
//#include "XGame.h"
#include "XGameWnd.h"
#include "XSceneStorage.h"
#include "XSceneGuildShop.h"
#include "XSockGameSvr.h"
#include "XGameEtc.h"
#include "XWndTemplate.h"
#include "XSoundMng.h"
#include "_Wnd2/XWndButton.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define COST_SOUL	XGC->m_costMedalForArmoryHero
#define IDS_MEDAL	_T("medal_tanker01")

XSceneArmory *SCENE_ARMORY = NULL;

void XSceneArmory::Destroy()
{	
//	SAFE_DELETE(m_pLayout);
	XBREAK(SCENE_ARMORY == NULL);
	XBREAK(SCENE_ARMORY != this);
	SCENE_ARMORY = NULL;
	SOUNDMNG->CloseSound(15);
}

XSceneArmory::XSceneArmory(XGame *pGame)
	: XSceneBase( pGame, XGAME::xSC_ARMORY )
	, m_Layout(_T("layout_armory.xml"))
	, m_LayoutTooltip(_T("layout_item.xml"))
{ 
	XBREAK(SCENE_ARMORY != NULL);
	SCENE_ARMORY = this;
	Init(); 
	
	m_Layout.CreateLayout("common_bg", this);
//	m_Layout.CreateLayout("common_bg_goldcash", this);
	m_Layout.CreateLayout("armory", this);
	m_Layout.CreateLayout("itemtab", this);

	// 최대값 테스트
	//xSET_TEXT(this, "text.common.gold", XE::Format(_T("%d"), 1000000000));
	//xSET_TEXT(this, "text.common.cash", XE::Format(_T("%d"), 1000000000));

	xSET_BUTT_HANDLER(this, "butt.back", &XSceneArmory::OnBack);

// 	// 상점 목록이 있는지 체크
// 	std::vector<ID> listItem;
// 	ACCOUNT->GetListShopSell(listItem);

// 	NewListRecvUpdate(true);
	xSET_BUTT_HANDLER(this, "butt.armory.new", &XSceneArmory::OnClickNewItemChange);
	// 아이템 설명부 레이아웃 생성
// 	m_Layout.CreateLayout( "itemtab", this );
	// 아이템 툴팁모듈이 들어갈 루트 윈도우
	XWnd *pRoot = Find( "wnd.item.tooltip" );
	if( pRoot ) {
		// 아이템 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "item_tooltip", pRoot );
	}
	pRoot = Find( "wnd.hero.tooltip" );
	if( pRoot ) {
		// 영웅 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "hero_tooltip", pRoot );
		pRoot->SetbShow( false );
	}
	SetbUpdate( true );		// Update()가 호출 됩니다.
// 	SOUNDMNG->OpenPlaySound(15, true);
}

void XSceneArmory::Create(void)
{
	XEBaseScene::Create();
}

int XSceneArmory::OnEnterScene( XWnd* pWnd, DWORD dw1, DWORD dw2 )
{
	SOUNDMNG->OpenPlaySound( 15, true );		// 무기점 배경음.
	return XSceneBase::OnEnterScene( pWnd, dw1, dw2 );
}

void XSceneArmory::Update()
{
	XGAME::CreateUpdateTopResource( this );
	if( m_idSelected == 0 )
		m_pSelectItem = nullptr;
	// 텍스트
// 	xSET_TEXT( this, "text.common.gold", XE::NumberToMoneyString(ACCOUNT->GetGold()) );
// 	xSET_TEXT( this, "text.common.cash", XE::NumberToMoneyString(ACCOUNT->GetCashtem()) );
	// 상점 목록이 있는지 체크
	std::vector<ID> listItem;
	ACCOUNT->GetListShopSell( listItem );
	XWnd *pWnd = Find( "wnd.armory.itemslotbg" );
	if( pWnd ) {
		pWnd->DestroyAllWnd();
		if (listItem.size() != 0) {
			int x = 210, y = 86;
			for( unsigned int i = 0; i < listItem.size(); i++ ) {
				ID idItem = listItem[i];
				auto pElem = new XWndStoragyItemElem( idItem );
				pElem->SetPosLocal(x, y);
				pElem->SetScaleLocal( 0.8f );
				pElem->SetEvent(XWM_CLICKED, this, &XSceneArmory::OnClickSlot, idItem );
				pWnd->Add( idItem, pElem);
//				pElem->SetbNum( FALSE );
//				pElem->SetNum(0);
 				if( m_idSelected == 0 )
					m_idSelected = idItem;
				if( idItem == m_idSelected ) {
					m_pSelectItem = pElem;
					m_pSelectItem->SetbSelected( true );
				}
				x += 47;
				if ((i + 1) % 4 == 0) {
					y += 53;
					x = 210;
				}
			}
			xSET_TEXT(this, "text.armory.char", XTEXT(80063));
			xSET_SHOW(this, "butt.armory.buy", TRUE);
		} else {
			xSET_TEXT(this, "text.armory.char", XTEXT(80152));
			xSET_SHOW(this, "butt.armory.buy", FALSE);
		}
		if( m_pSelectItem ) {
//			UpdateTooltipBySelected();
			UpdateBuyItemInfo();
		}
	}

	XEBaseScene::Update();
}

int XSceneArmory::Process(float dt)
{
	return XEBaseScene::Process( dt );
}

//
void XSceneArmory::Draw(void)
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneArmory::OnLButtonDown(float lx, float ly)
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneArmory::OnLButtonUp(float lx, float ly) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneArmory::OnMouseMove(float lx, float ly) {
	XEBaseScene::OnMouseMove( lx, ly );
}

int XSceneArmory::OnBack(XWnd *pWnd, DWORD p1, DWORD p2)
{
	DoExit(XGAME::xSC_WORLD);
	return 1;
}

int XSceneArmory::OnClickSlot(XWnd *_pWnd, DWORD p1, DWORD p2)
{
	auto pWnd = Find(p1);
	if( pWnd )
		m_idSelected = p1;
	SetbUpdate( true );
	return 1;
}

int XSceneArmory::OnClickNewItemChange(XWnd *pWnd, DWORD p1, DWORD p2)
{
	const int ticketItemID = XGC->m_armoryRecallItem;
	const DWORD gemNum = XGC->m_armoryRecallGem;
	XList4<XBaseItem*> itemList;
	ACCOUNT->GetInvenItem(itemList);		// 인벤토리 얻어오기
	bool bTicket = false;		// 티켓 아이템 유무
	int i = 0;
	for( auto pItem : itemList ) {
		if (pItem->GetidProp() == ticketItemID) {
			bTicket = true;
			break;
		}
		++i;
	}
	if (bTicket) {
		// 티켓이 있으면
		// 아이템을 사용해서 리셋을 하시겠습니까?
		auto pAlert = XGAME::DoAlertWithItem( ticketItemID, XTEXT(80066), XWnd::xYESNO );
		if( pAlert ) {
			pAlert->SetEvent( XWM_YES, this, 
												&XSceneArmory::OnClickNewItemChangeOK, XGAME::xSC_SPENT_ITEM );
		}
		// 팝업
// 		m_Layout.CreateLayout("commonitemtextpopup", this);
// 		XWnd *pPopup = Find("img.common.popupitemtext");
// 		if (pPopup) {
// 			pPopup->SetbModal(TRUE);
// 			xSET_TEXT(this, "text.title", XTEXT(80065));			// "새로배치"
// 			xSET_TEXT(this, "text.common.ask", XTEXT(80066));		// "아이템을 사용하여 새로배치하시겠습니까?"
// 			xSET_BUTT_TEXT(this, "butt.ok", XTEXT(80065));			// "새로배치"
// 			xSET_IMG(this, "img.ask.itemimg", XE::MakePath(DIR_IMG, PROP_ITEM->GetpProp(ticketItemID)->strIcon.c_str()));
// 			xSET_BUTT_HANDLER_PARAM(this, "butt.ok", this, &XSceneArmory::OnClickNewItemChangeOK, XGAME::xSC_SPENT_ITEM);
// 			xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneArmory::OnClickPopupCancel);
// 		}
	} else {
		// 티켓이 없으면 캐시로 구매여부를 물어본다.
		auto pPopup = new XWndPaymentByCash( XTEXT( 80068 ), XTEXT( 80065 ) );
		pPopup->SetItem( (ID)ticketItemID, gemNum );
		Add( pPopup );
		pPopup->SetEvent( XWM_OK,
											this,
											&XSceneArmory::OnClickNewItemChangeOK, 
											XGAME::xSC_SPENT_GEM );
		// 			if (ACCOUNT->GetCashtem() >= gemNum) {		// 캐쉬 검사
// 				m_Layout.CreateLayout("commonitemtextpopup", this);
// 				XWnd *pPopup = Find("img.common.popupitemtext");
// 				if (pPopup) {
// 					pPopup->SetbModal(TRUE);
// 					xSET_TEXT(this, "text.title", XTEXT(80065));			// "새로배치"
// 					xSET_TEXT(this, "text.common.ask", XE::Format(XTEXT(80068), gemNum));		// "아이템이 부족합니다. 캐쉬 %d개를 사용하여 새로배치하시겠습니까?"
// 					xSET_BUTT_TEXT(this, "butt.ok", XTEXT(80065));			// "새로배치"
// 					xSET_IMG(this, "img.ask.itemimg", XE::MakePath(DIR_IMG, PROP_ITEM->GetpProp(ticketItemID)->strIcon.c_str()));
// 					xSET_BUTT_HANDLER_PARAM(this, "butt.ok", this, &XSceneArmory::OnClickNewItemChangeOK, XGAME::xSC_SPENT_GEM);
// 					xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneArmory::OnClickPopupCancel);
// 				}
		}
// 	else {
// 				// 티켓 아이템도 없고 캐쉬도 부족
// 				XWND_ALERT_T( _T("%s"), XTEXT(80150) );
			// 팝업
// 				m_Layout.CreateLayout("commontextpopup", this);
// 
// 				XWnd *pPopup = Find("img.common.popuptext");
// 				if (pPopup)
// 				{
// 					pPopup->SetbModal(TRUE);
// 
// 					xSET_TEXT(this, "text.title", XTEXT(80130));			// "즉시호출"
// 					xSET_TEXT(this, "text.common.ask", XTEXT(80150));		// "아이템과 캐쉬가 부족합니다. 구매해주세요"
				//xSET_BUTT_TEXT(this, "butt.ok", XTEXT(2));

// 					xSET_SHOW(this, "butt.ok1", TRUE);
// 					xSET_SHOW(this, "butt.ok", FALSE);
//					xSET_SHOW(this, "butt.cancel", FALSE);

//					xSET_BUTT_HANDLER(this, "butt.ok1", &XSceneStorage::OnClickPopupCancel);

				//xSET_BUTT_HANDLER(this, "butt.ok", &XSceneArmory::OnClickPopupCancel);
//					xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneArmory::OnClickPopupCancel);
//				}
//			}
//		}
	return 1;
}

int XSceneArmory::OnClickNewItemChangeOK(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (GAMESVR_SOCKET)
	{
		m_idSelected = 0;
		if ((XGAME::xtSpentCall)p1 == XGAME::xSC_SPENT_ITEM)
			GAMESVR_SOCKET->SendArmoryListCashChange(this, (XGAME::xtSpentCall)p1);
		else if ((XGAME::xtSpentCall)p1 == XGAME::xSC_SPENT_GEM)
			GAMESVR_SOCKET->SendArmoryListCashChange(this, (XGAME::xtSpentCall)p1);
	}
	return 1;
}

int XSceneArmory::OnClickBuy(XWnd *pWnd, DWORD p1, DWORD p2)
{
	auto pPropItem = m_pSelectItem->getpProp();
	XBREAK( pPropItem == nullptr );
	const auto price = pPropItem->GetBuyCost( ACCOUNT->GetLevel() );
	if( pPropItem->IsSoul() ) {
		int numMedal = ACCOUNT->GetNumItems( IDS_MEDAL );
		if( numMedal >= COST_SOUL ) {
			auto pAlert = XGAME::DoAlertWithItem( IDS_MEDAL, XTEXT( 80071 ), XWnd::xYESNO );
			if( pAlert ) {
				pAlert->SetEvent( XWM_YES, this,
													&XSceneArmory::OnClickBuyOK, XGAME::xCOIN_MEDAL );
			}
			// 			// 팝업
// 			m_Layout.CreateLayout( "commontextpopup", this );
// 			XWnd *pPopup = Find( "img.common.popuptext" );
// 			if( pPopup ) {
// 				pPopup->SetbModal( TRUE );
// 				xSET_TEXT( this, "text.title", XTEXT( 80043 ) );				// "구입"
// 				xSET_TEXT( this, "text.common.ask", XTEXT( 80071 ) );			// "정말로 구매하시겠습니까?"
// 				xSET_BUTT_TEXT( this, "butt.ok", XTEXT( 80043 ) );				// "구입"
// 				xSET_BUTT_HANDLER_PARAM( this, "butt.ok", this, &XSceneArmory::OnClickBuyOK, XGAME::xCOIN_MEDAL );
// 				xSET_BUTT_HANDLER( this, "butt.cancel", &XSceneArmory::OnClickPopupCancel );
// 			}
		} else {
			// 캐시로 구입하시겠습니까?
			XWND_ALERT_T( _T("%s"), XTEXT(2262) );
		}
	} else {
		if (ACCOUNT->GetGold() >= price) {
			auto pAlert = XWND_ALERT_YESNO_T( "alert.confirm", XTEXT( 80071 ) );
			if( pAlert ) {
				pAlert->SetEvent( XWM_YES, this,
													&XSceneArmory::OnClickBuyOK, XGAME::xCOIN_GOLD );
			}
			// 			// 아이템 가격을 지불가능함.
// 			m_Layout.CreateLayout("commontextpopup", this);
// 			XWnd *pPopup = Find("img.common.popuptext");
// 			if (pPopup) {
// 				pPopup->SetbModal(TRUE);
// 				xSET_TEXT(this, "text.title", XTEXT(80043));				// "구입"
// 				xSET_TEXT(this, "text.common.ask", XTEXT(80071));			// "정말로 구매하시겠습니까?"
// 				xSET_BUTT_TEXT(this, "butt.ok", XTEXT(80043));				// "구입"
// 				xSET_BUTT_HANDLER_PARAM(this, "butt.ok", this, &XSceneArmory::OnClickBuyOK, XGAME::xCOIN_GOLD );
// 				xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneArmory::OnClickPopupCancel);
// 			}
		} else {
		// 금화가 없으면 캐시로 구입안내
//		if (ACCOUNT->GetCashtem() >= m_pSelectItem->getpProp()->cashCost) {
			// 팝업
			int goldLack = price - ACCOUNT->GetGold();
			auto pPopup = new XWndPaymentByCash( XTEXT( 80068 ), XTEXT( 80065 ) );
			pPopup->SetGold( goldLack );
			Add( pPopup );
			pPopup->SetEvent( XWM_OK,
												this,
												&XSceneArmory::OnClickNewItemChangeOK, 
												XGAME::xSC_SPENT_GEM );
		}
	}
// 			m_Layout.CreateLayout("commonitemtextpopup", this);
// 			XWnd *pPopup = Find("img.common.popupitemtext");
// 			if (pPopup) {
// 				pPopup->SetbModal(TRUE);
// //				auto pProp = m_pSelectItem->getpProp();
// 				xSET_TEXT(this, "text.title", XTEXT(80169));				// "금화 부족"
// 				_tstring strText = XE::Format( XTEXT( 80069 ),pPropItem->cashCost, price );			// "금화가 부족합니다. 캐쉬 50개를 사용하여 1000금화 아이템을 구매하시겟습니까?"
// 				if( XE::LANG.GetstrKey() == _T("english") ) {
// 					strText = XE::Format( XTEXT( 80069 ), price, pPropItem->cashCost );
// 				}
// 				xSET_TEXT( this, "text.common.ask", strText );
// 				xSET_BUTT_TEXT(this, "butt.ok", XTEXT(80070));				// "구매"
// 			
// 				xSET_IMG(this, "img.ask.itemimg", XE::MakePath(DIR_UI, _T("hero_gold.png")));
// 				xSET_BUTT_HANDLER_PARAM(this, "butt.ok", this, &XSceneArmory::OnClickBuyOK, XGAME::xCOIN_CASH );
// 				xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneArmory::OnClickPopupCancel);
// 			}
// 		} else {
// 			// 티켓 아이템도 없고 캐쉬도 부족
// 			XWND_ALERT_T( _T( "%s" ), XTEXT( 80150 ) );
// 			// 팝업
// // 			m_Layout.CreateLayout("commontextpopup", this);
// // 			XWnd *pPopup = Find("img.common.popuptext");
// // 			if (pPopup) {
// // 				pPopup->SetbModal(TRUE);
// // 				xSET_TEXT(this, "text.title", XTEXT(80070));			// "구매"
// // 				xSET_TEXT(this, "text.common.ask", XTEXT(80150));		// "아이템과 캐쉬가 부족합니다. 구매해주세요"
// // 				//xSET_BUTT_TEXT(this, "butt.ok", XTEXT(2));
// // //				xSET_SHOW(this, "butt.ok1", TRUE);
// // //				xSET_SHOW(this, "butt.ok", FALSE);
// // //				xSET_SHOW(this, "butt.cancel", FALSE);
// // //				xSET_BUTT_HANDLER(this, "butt.ok1", &XSceneStorage::OnClickPopupCancel);
// // 				//xSET_BUTT_HANDLER(this, "butt.ok", &XSceneArmory::OnClickPopupCancel);
// // 				xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneArmory::OnClickPopupCancel);
// // 			}
// 		}
//	}
	SetbUpdate( true );
	return 1;
}

int XSceneArmory::OnClickBuyOK(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (GAMESVR_SOCKET) {
		auto typeCoin = (XGAME::xtCoin)p1;
		GAMESVR_SOCKET->SendItemBuy(this, m_pSelectItem->getpProp()->idProp, XGAME::xSHOP_ARMORY, typeCoin );
		ACCOUNT->RemoveListShopSell(m_pSelectItem->getpProp()->idProp);		// 리스트에서 바로 빼주자
		m_idSelected = 0;
//		UpdateBuyItemInfo();
		SetbUpdate( true );
	}
//	OnClickPopupCancel(pWnd, 0, 0);
	SOUNDMNG->OpenPlaySound(13);


	return 1;
}

// int XSceneArmory::OnClickPopupCancel(XWnd *pWnd, DWORD p1, DWORD p2)
// {
// 	if (pWnd->GetpParent())
// 		pWnd->GetpParent()->SetbDestroy(TRUE);
// 
// 	return 1;
// }

// void XSceneArmory::RecvUpdate()
// {
// 	xSET_TEXT(this, "text.common.gold", XE::Format(_T("%d"), ACCOUNT->GetGold()));
// 	xSET_TEXT(this, "text.common.cash", XE::Format(_T("%d"), ACCOUNT->GetCashtem()));
// 
// 	NewListRecvUpdate(true);
// }

void XSceneArmory::NewListRecvUpdate(bool bSuccess)
{
	std::vector<ID> listItem;
	ACCOUNT->GetListShopSell(listItem);
	XWnd *pWnd = Find("wnd.armory.itemslotbg");
	if (!pWnd)
		return;
	pWnd->DestroyAllWnd();
	if (listItem.size() != 0)		{
		bool first = true;
		int x = 200, y = 74;
		for (unsigned int i = 0; i < listItem.size() ; i++)		{
			XWndStoragyItemElem *pElem = new XWndStoragyItemElem(listItem[i]);
			pElem->SetPosLocal(x, y);
			pElem->SetEvent(XWM_CLICKED, this, &XSceneArmory::OnClickSlot);
//				pElem->SetbNum(FALSE);
//				pElem->SetNum(0);
			pWnd->Add(pElem);

			if (first)	{
				m_pSelectItem = pElem;
				m_pSelectItem->SetbSelected( true );
				UpdateBuyItemInfo();
				first = false;
			}
			x += 70;
			if ((i + 1) % 3 == 0)		{
				y += 55;
				x = 200;
			}
		}
		xSET_TEXT(this, "text.armory.char", XTEXT(80063));
		xSET_SHOW(this, "butt.armory.buy", TRUE);
	}	else {
		xSET_TEXT(this, "text.armory.char", XTEXT(80152));
		xSET_SHOW(this, "butt.armory.buy", FALSE);
	}
}

// void XSceneArmory::UpdateBuyItemInfo()
// {
// 	return;
// 	XWnd *pAll = Find("wnd.armory.itemteb");
// 	if (pAll)
// 		pAll->SetbDestroy(TRUE);
// 
// 	m_Layout.CreateLayout("itemtab", this);
// 
// 	if (m_pSelectItem)
// 	{
// 		// 아이템 이미지
// // 		XWndImage* img = dynamic_cast<XWndImage*>(Find("img.armory.itemimg"));
// // 		if( img )
// // 			img->SetSurface(m_pSelectItem->GetItemImg());
// 		xSET_IMG( this, "img.armory.itemimg", m_pSelectItem->getpProp()->strIcon);
// 		
// 		// 아이템 별
// 		if (m_pSelectItem->GetnumStars() == XGAME::xGD_LEGENDARY)
// 		{
// 			xSET_SHOW(this, "img.armory.star4", TRUE);
// 			xSET_SHOW(this, "img.armory.star3", TRUE);
// 			xSET_SHOW(this, "img.armory.star2", TRUE);
// 			xSET_SHOW(this, "img.armory.star1", TRUE);
// 		}
// 		else if (m_pSelectItem->GetnumStars() == XGAME::xGD_EPIC)
// 		{
// 			xSET_SHOW(this, "img.armory.star4", FALSE);
// 			xSET_SHOW(this, "img.armory.star3", TRUE);
// 			xSET_SHOW(this, "img.armory.star2", TRUE);
// 			xSET_SHOW(this, "img.armory.star1", TRUE);
// 		}
// 		else if(m_pSelectItem->GetnumStars() == XGAME::xGD_RARE)
// 		{
// 			xSET_SHOW(this, "img.armory.star4", FALSE);
// 			xSET_SHOW(this, "img.armory.star3", FALSE);
// 			xSET_SHOW(this, "img.armory.star2", TRUE);
// 			xSET_SHOW(this, "img.armory.star1", TRUE);
// 		}
// 		else if(m_pSelectItem->GetnumStars() == XGAME::xGD_VETERAN)
// 		{
// 			xSET_SHOW(this, "img.armory.star4", FALSE);
// 			xSET_SHOW(this, "img.armory.star3", FALSE);
// 			xSET_SHOW(this, "img.armory.star2", FALSE);
// 			xSET_SHOW(this, "img.armory.star1", TRUE);
// 		}
// 		else if(m_pSelectItem->GetnumStars() == XGAME::xGD_COMMON)
// 		{
// 			xSET_SHOW(this, "img.armory.star4", FALSE);
// 			xSET_SHOW(this, "img.armory.star3", FALSE);
// 			xSET_SHOW(this, "img.armory.star2", FALSE);
// 			xSET_SHOW(this, "img.armory.star1", FALSE);
// 		}
// 
// 		// 아이템 이름
// 		xSET_SHOW(this, "text.armory.armoryname", TRUE);
// 		xSET_TEXT(this, "text.armory.armoryname", XTEXT(m_pSelectItem->getpProp()->idName));
// 
// 		// 아이템 능력치
// 		XSceneStorage::sUpdateItemOption( m_pSelectItem->getpProp(), this, "armory", 16.f );
// 
// 		xSET_SHOW(this, "text.armory.itemdesc", TRUE);
// 		xSET_TEXT(this, "text.armory.itemdesc", XTEXT(m_pSelectItem->getpProp()->idDesc));
// 
// 		xSET_SHOW(this, "img.armory.gold", TRUE);
// 		xSET_SHOW(this, "text.armory.cost", TRUE);
// 		xSET_TEXT(this, "text.armory.cost", XE::Format(_T("%d"), m_pSelectItem->getpProp()->GetCost(ACCOUNT->GetLevel())));
// 
// 		xSET_BUTT_HANDLER(this, "butt.armory.new", &XSceneArmory::OnClickNewItemChange);
// 		xSET_BUTT_HANDLER(this, "butt.armory.buy", &XSceneArmory::OnClickBuy);
// 		
// 		xSET_SHOW(this, "butt.armory.buy", TRUE);
// 	}
// }
/**
 @brief 선택한 아이템의 정보를 업데이트 한다.
*/
void XSceneArmory::UpdateBuyItemInfo()
{
	// 루트가 될 윈도우가 없으면 리턴
	XWnd *pRoot = Find("wnd.item.tooltip");
	if( pRoot == nullptr )
		return;
	xSET_SHOW( this, "img.bg.item.tooltip", false );
	xSET_SHOW( this, "img.bg.hero.tooltip", false );
	auto pProp = m_pSelectItem->getpProp();
	auto cost = pProp->GetBuyCost(ACCOUNT->GetLevel());
	if( pProp ) {
		if( pProp->IsSoul() ) {
			xSET_SHOW( this, "img.bg.hero.tooltip", true );
			UpdateToolTipBySoul( pProp );
		} else {
			xSET_SHOW( this, "img.bg.item.tooltip", true );
			xSET_SHOW( this, "wnd.item.tooltip", true );
			xSET_SHOW( this, "wnd.hero.tooltip", false );
			XGAME::UpdateItemTooltip( pProp, pRoot, cost );
		}
	}
	xSET_BUTT_HANDLER(this, "butt.armory.new", &XSceneArmory::OnClickNewItemChange);
	xSET_BUTT_HANDLER(this, "butt.armory.buy", &XSceneArmory::OnClickBuy);
}

void XSceneArmory::UpdateToolTipBySoul( XPropItem::xPROP *pProp )
{
	XASSERT( pProp->IsSoul() );
	XWnd *pRoot = Find( "wnd.hero.tooltip" );
	if( pRoot == nullptr )
		return;
	const int costMedal = COST_SOUL;
	if( costMedal > 0 ) {
		xSET_SHOW( this, "wnd.hero.tooltip", true );
		xSET_SHOW( this, "wnd.item.tooltip", false );
		auto pPropHero = PROP_HERO->GetpProp( pProp->strIdHero );
		if( pPropHero ) {
			XGAME::UpdateHeroTooltip( pPropHero->idProp, pRoot, costMedal );
		}
		xSET_SHOW( pRoot, "wnd.sell", true );
		auto pText = xSET_TEXT( pRoot, "text.cost", XE::NumberToMoneyString( costMedal ) );
		auto pImg = xSET_IMG( pRoot, "img.gold", PATH_UI( "medal_coin_small.png" ) );
		int numMedal = ACCOUNT->GetNumItems( _T( "medal_tanker01" ) );
		if( numMedal < costMedal )
			pText->SetColorText( XCOLOR_RED );
	}
	else
		xSET_SHOW( pRoot, "wnd.sell", FALSE );
}

/**
 @brief pRoot아래에 있는 아이템 툴팁모듈을 업데이트한다.
 @param cost 아이템 가격을 표시해야할때 가격을 넘겨준다.
*/

static void sUpdateEquipItemTooltip( XPropItem::xPROP *pProp, XWnd *pRoot, int cost )
{
	// 아이템 이미지
	if( pProp == nullptr )
		return;

	// 아이템 이름
	auto pText =
		xSET_TEXT( pRoot, "text.name", XTEXT( pProp->idName ) );
	if( pText )
		pText->SetColorText( XGAME::GetGradeColor( pProp->grade ) );
	// 아이템 이미지
	xSET_IMG( pRoot, "img.item", XE::MakePath( DIR_IMG, pProp->strIcon ) );
	// 별
	int numStar = pProp->grade;
	xCTRL_LOOP( pRoot, "img.star", i, 5, XWndImage*, pImg )
	{
		if( i < numStar )
			pImg->SetbShow( TRUE );
		else
			pImg->SetbShow( FALSE );
	} END_LOOP;
	// 장비 종류와 등급
	pText = xSET_TEXT( pRoot, "text.grade", XFORMAT("%s %s", XGAME::GetStrGrade(pProp->grade), XTEXT(80002)));
	if( pText )
		pText->SetColorText( XGAME::GetGradeColor( pProp->grade ) );
	// 스탯
	xCTRL_LOOP( pRoot, "wnd.stat", i, 4, XWnd*, pWndStat )
	{
		// 아이콘+텍스트의 묶음 윈도우 루프
		auto adj = pProp->aryAdjParam[i];
		if( XGAME::IsValidAdjParam( adj.adjParam )  )
		{
			// 옵션이 있을경우에만 한다.
			auto resIcon = XGAME::GetIconAdjParam( adj.adjParam );
			if( XE::IsHave(resIcon) )
				// 아이콘 윈도우
				xSET_IMG( pWndStat, "img.stat", resIcon );
			auto szText = XFORMAT("%s %+.0f%%", XGAME::GetStrAdjParam( adj.adjParam), adj.param);
			xSET_TEXT( pWndStat, "text.stat", szText );
			pWndStat->SetbShow( TRUE );
		} else
			pWndStat->SetbShow( FALSE );
	} END_LOOP;
	// 아이템 장착효과
	// 아직 구현안됨
	auto pTextInvoke = pRoot->Find("text.invoke");

	// 아이템 설명
	pText = xGET_TEXT_CTRL( pRoot, "text.desc" );
	if( pText )
	{
		if( pProp->idDesc )
		{
			pText->SetbShow( TRUE );
			xSET_TEXT( pRoot, "text.desc", XTEXT(pProp->idDesc) );
			if( !pTextInvoke->GetbShow() && pTextInvoke )
				pText->SetPosLocal( pTextInvoke->GetPosLocal() );
		} else
			pText->SetbShow( FALSE );
	}
	// 판매가격
	if( cost > 0 )
	{
		xSET_SHOW( pRoot, "wnd.sell", TRUE );
		pText = xSET_TEXT( pRoot, "text.cost", XE::NumberToMoneyString( cost ) );
		if( ACCOUNT->GetGold() < (DWORD)cost )
			pText->SetColorText( XCOLOR_RED );
		else
			pText->SetColorText( XCOLOR_WHITE );
	} else
		xSET_SHOW( pRoot, "wnd.sell", FALSE );
}

static void sUpdateEtcItemTooltip( XPropItem::xPROP *pProp, XWnd *pRoot, int cost )
{
	// 아이템 이미지
	if( pProp == nullptr )
		return;

	// 아이템 이름
	auto pText =
		xSET_TEXT( pRoot, "text.name", XTEXT( pProp->idName ) );
	if( pText ) {
		pText->SetColorText( XGAME::GetGradeColor( pProp->grade ) );
		// 영혼석의 경우 이름을 프로그램으로 조합한다.
		if( pProp->IsSoul() ) {
			auto pPropHero = PROP_HERO->GetpProp( pProp->strIdHero );
			if( pPropHero ) {
				// %s의 영혼석.
				pText->SetText( 
					XE::Format( XTEXT( 2158 ), pPropHero->GetstrName().c_str() ) );
			}
			auto pImgFrame = xGET_IMAGE_CTRL( pRoot, "img.frame" );
			if( pImgFrame ) {
				// 영혼석의경우 프레임을 바꿔준다.
				pImgFrame->SetSurfaceRes( PATH_UI("frame_soul.png") );
				float scale = 1.3f;
				pImgFrame->SetScaleLocal( scale );
//				pImgFrame->SetScaleLocal( 1.0714f );
			}
		}
	}
	// 아이템 이미지
	//xSET_IMG( pRoot, "img.item", XE::MakePath( DIR_IMG, pProp->strIcon ) );
	auto pImg = SafeCast<XWndImage*>(pRoot->Find("img.item"));
	if (pImg) {
		pImg->SetSurface(XE::MakePath(DIR_IMG, pProp->strIcon));
		if (pProp->type == XGAME::xIT_SOUL)
			pImg->SetScaleLocal(0.76f);
		else
			pImg->SetScaleLocal(1.f);
	}
	// 별
	int numStar = pProp->grade;
	xCTRL_LOOP( pRoot, "img.star", i, 5, XWndImage*, pImg ) {
		if( i < numStar )
			pImg->SetbShow( TRUE );
		else
			pImg->SetbShow( FALSE );
	} END_LOOP;
	// 장비 종류와 등급
	pText = xSET_TEXT( pRoot, "text.grade", XFORMAT("%s %s", XGAME::GetStrGrade(pProp->grade), XTEXT(2297)));	// xx아이템
	if( pText )
		pText->SetColorText( XGAME::GetGradeColor( pProp->grade ) );
	// 아이템 장착효과
	// 아직 구현안됨
	auto pTextInvoke = pRoot->Find("text.invoke");
	// 아이템 설명
	pText = xGET_TEXT_CTRL( pRoot, "text.desc" );
	if( pText && pProp->idDesc )
	{
		pText->SetbShow( TRUE );
		xSET_TEXT( pRoot, "text.desc", XTEXT(pProp->idDesc) );
		// 영혼석의 경우 이름을 프로그램으로 조합한다.
		if( pProp->IsSoul() ) {
			auto pPropHero = PROP_HERO->GetpProp( pProp->strIdHero );
			if( pPropHero ) {
				// %s의 영혼석.
				pText->SetText(
					XE::Format( XTEXT( 2159 ), pPropHero->GetstrName().c_str() ) );
			}
		}
		if( !pTextInvoke->GetbShow() && pTextInvoke )
			pText->SetPosLocal( pTextInvoke->GetPosLocal() );
	}
	else
		pText->SetbShow( FALSE );
	// 판매가격
	if( cost > 0 ) {
		xSET_SHOW( pRoot, "wnd.sell", TRUE );
		pText = xSET_TEXT( pRoot, "text.cost", XE::NumberToMoneyString( cost ) );
		if( ACCOUNT->GetGold() < (DWORD)cost )
			pText->SetColorText( XCOLOR_RED );
	} else
		xSET_SHOW( pRoot, "wnd.sell", FALSE );
}

void XGAME::UpdateItemTooltip( XPropItem::xPROP *pProp, XWnd *pRoot, int cost )
{
	if( pProp->type == XGAME::xIT_EQUIP )
		sUpdateEquipItemTooltip( pProp, pRoot, cost );
	else
		sUpdateEtcItemTooltip( pProp, pRoot, cost );
}


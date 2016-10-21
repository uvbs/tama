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
#include "XWndStorageItemElem.h"
#ifdef _VER_ANDROID
#include "XFramework/android/JniHelper.h"
#endif // _VER_ANDROID

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
#ifdef _VER_ANDROID
	JniHelper::ShowAdmob( false, 0, 0 );
#endif // _VER_ANDROID
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
	m_Layout.CreateLayout("armory", this);
	m_Layout.CreateLayout("itemtab", this);
	xSET_BUTT_HANDLER(this, "butt.back", &XSceneArmory::OnBack);
	xSET_BUTT_HANDLER(this, "butt.armory.new", &XSceneArmory::OnClickNewItemChange);
	// 아이템 설명부 레이아웃 생성
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
#ifdef _VER_ANDROID
	JniHelper::ShowAdmob( true, 480, 51 );
#endif // _VER_ANDROID
	SetbUpdate( true );		// Update()가 호출 됩니다.
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
	} else {
		// 티켓이 없으면 캐시로 구매여부를 물어본다.
		auto pPopup = new XWndPaymentByCash( XTEXT( 80068 ), XTEXT( 80065 ) );
		pPopup->SetItem( (ID)ticketItemID, gemNum );
		Add( pPopup );
		pPopup->SetEvent( XWM_OK,
											this,
											&XSceneArmory::OnClickNewItemChangeOK, 
											XGAME::xSC_SPENT_GEM );
		}
	return 1;
}

int XSceneArmory::OnClickNewItemChangeOK(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (GAMESVR_SOCKET)	{
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
		} else {
			// 캐시로 구입하시겠습니까?
			XWND_ALERT_T( _T("%s"), XTEXT(2262) );
		}
	} else {
//		if (ACCOUNT->GetGold() >= price) {
		int numLack = 0;		// 부족한 개수
		if( ACCOUNT->IsPayable( pPropItem, 1, &numLack ) ) {
			// 아이템 가격을 지불가능함.
			auto pAlert = XWND_ALERT_YESNO_T( "alert.confirm", XTEXT( 80071 ) );
			if( pAlert ) {
				pAlert->SetEvent( XWM_YES, this,
													&XSceneArmory::OnClickBuyOK, XGAME::xCOIN_PROP );
			}
		} else {
			// 지불 안됨.
			// 팝업
//			int goldLack = price - ACCOUNT->GetGold();
			auto pPopup = new XWndPaymentByCash( XTEXT( 80068 ), XTEXT( 80065 ) );
			//pPopup->SetGold( goldLack );
			auto pPropPaytem = PROP_ITEM->GetpProp( pPropItem->m_strPayItem );
			if( pPropPaytem ) {
				const int numGem = pPropPaytem->cashCost * pPropItem->m_numCost;		// 모자르는 만큼을 젬으로 사야할때 필요한 젬개수.
				pPopup->SetItem( pPropItem->m_strPayItem, numGem );
				Add( pPopup );
				pPopup->SetEvent( XWM_OK,
													this,
													&XSceneArmory::OnClickBuyOK,
													XGAME::xCOIN_CASH );
			}
		}
	}
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
			XGAME::UpdateItemTooltip( pProp, pRoot, pProp->m_strPayItem, cost );
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
 @param idsPay 지불수단. 금화대신에 지불해야하는 아이템(징표같은..)의 ids. 금화와 코인도 "gold", "guild_coin"이름으로 아이템화 되어있다. null이면 표시하지 않는다.
*/
static void sUpdateEquipItemTooltip( const XPropItem::xPROP *pProp, 
																		 XWnd *pRoot, 
																		 const _tstring& idsPay, 
																		 int numPaytem )
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
	if( pText )	{
		if( pProp->idDesc )		{
			pText->SetbShow( TRUE );
			xSET_TEXT( pRoot, "text.desc", XTEXT(pProp->idDesc) );
			if( !pTextInvoke->GetbShow() && pTextInvoke )
				pText->SetPosLocal( pTextInvoke->GetPosLocal() );
		} else
			pText->SetbShow( FALSE );
	}
	// 판매가격
	if( numPaytem > 0 ) {
		auto pPropPay = PROP_ITEM->GetpProp( idsPay );
		xSET_SHOW( pRoot, "img.gold", !idsPay.empty() );
		if( !idsPay.empty() ) {
			const _tstring strRes = XE::MakePath( DIR_IMG, pPropPay->strIcon );
			xSET_IMG( pRoot, "img.gold", strRes );
		}
		xSET_SHOW( pRoot, "wnd.sell", TRUE );
		pText = xSET_TEXT( pRoot, "text.cost", XE::NumberToMoneyString( numPaytem ) );
//		if( ACCOUNT->GetGold() < (DWORD)numPaytem )
		if( ACCOUNT->IsPayable( pProp, 1, nullptr ) ) {
			pText->SetColorText( XCOLOR_WHITE );
		}	else {
			pText->SetColorText( XCOLOR_RED );
		}
	} else
		xSET_SHOW( pRoot, "wnd.sell", FALSE );
}

static void sUpdateEtcItemTooltip( const XPropItem::xPROP *pProp, XWnd *pRoot, const _tstring& idsPay, int numPaytem )
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
	if( numPaytem > 0 ) {
		auto pPropPay = PROP_ITEM->GetpProp( idsPay );
		xSET_SHOW( pRoot, "img.gold", !idsPay.empty() );
		if( !idsPay.empty() ) {
			const _tstring strRes = XE::MakePath( DIR_IMG, pPropPay->strIcon );
			xSET_IMG( pRoot, "img.gold", strRes );
		}
		xSET_SHOW( pRoot, "wnd.sell", TRUE );
		pText = xSET_TEXT( pRoot, "text.cost", XE::NumberToMoneyString( numPaytem ) );
//		if( ACCOUNT->GetGold() < (DWORD)numPaytem )
		if( ACCOUNT->IsPayable( pProp, 1, nullptr ) ) {
			pText->SetColorText( XCOLOR_WHITE );
		} else {
			pText->SetColorText( XCOLOR_RED );
		}
	} else
		xSET_SHOW( pRoot, "wnd.sell", FALSE );
}

void XGAME::UpdateItemTooltip( const XPropItem::xPROP *pProp, 
															 XWnd *pRoot, 
															 const _tstring& idsPay, 
															 int cost )
{
	if( pProp->type == XGAME::xIT_EQUIP )
		sUpdateEquipItemTooltip( pProp, pRoot, idsPay, cost );
	else
		sUpdateEtcItemTooltip( pProp, pRoot, idsPay, cost );
}


#include "StdAfx.h"
#include "XSceneTrader.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XWndTemplate.h"
#include "XSoundMng.h"
#include "_Wnd2/XWndButton.h"
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

XSceneTrader *SCENE_TRADER = NULL;

void XSceneTrader::Destroy()
{
#ifdef _VER_ANDROID
	JniHelper::ShowAdmob( false, 0, 0 );
#endif // _VER_ANDROID
	XBREAK(SCENE_TRADER == NULL);
	XBREAK(SCENE_TRADER != this);
	SCENE_TRADER = NULL;
	SOUNDMNG->CloseSound(17);
}

XSceneTrader::XSceneTrader(XGame *pGame)
	: XSceneBase(pGame, XGAME::xSC_TRADER)
	, m_Layout(_T("layout_trader.xml"))
{
	XBREAK(SCENE_TRADER != NULL);
	SCENE_TRADER = this;
	Init();

	// 초기값

	for (int i = 0; i < XGAME::xRES_MAX; i++)
		m_resourceGold[i] = 0;

	m_totalGold = 0;

	UILoad();				// 초기 UI 로드
	UpdateCharacter();		// 현재 무역상이 있는지 확인
#ifdef _VER_ANDROID
	JniHelper::ShowAdmob( true, 480, 51 );
#endif // _VER_ANDROID
}

void XSceneTrader::Create(void)
{
	XAccount::s_bTraderArrived = false;		// 무역상 알림 확인함(끔)
	XSceneBase::Create();
}

void XSceneTrader::Update()
{
	XGAME::CreateUpdateTopResource( this );
	UpdateCharacter();
	XSceneBase::Update();
}

int XSceneTrader::OnEnterScene( XWnd* pWnd, DWORD dw1, DWORD dw2 )
{
	SOUNDMNG->OpenPlaySound( 17, true );		// 배경음.
	return XSceneBase::OnEnterScene( pWnd, dw1, dw2 );
}

int XSceneTrader::Process(float dt)
{
	UpdateRecallTimeCheck();
	return XSceneBase::Process( dt );
}

void XSceneTrader::Draw(void)
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneTrader::OnLButtonDown(float lx, float ly)
{
	XSceneBase::OnLButtonDown( lx, ly );
}

void XSceneTrader::OnLButtonUp(float lx, float ly)
{
	XSceneBase::OnLButtonUp( lx, ly );
}

void XSceneTrader::OnMouseMove(float lx, float ly)
{
	XSceneBase::OnMouseMove( lx, ly );
}

//////////////////////////////////////////////////////////////////////////

int XSceneTrader::OnBack(XWnd *pWnd, DWORD p1, DWORD p2)
{
	DoExit(XGAME::xSC_WORLD);
	return 1;
}

int XSceneTrader::OnClickTrade(XWnd *pWnd, DWORD p1, DWORD p2)
{
	// 거래할 총 금액 체크
	if (m_totalGold <= 0)
		return 1;
	// 팝업
	auto pAlert = XWND_ALERT_OKCANCEL_T("wnd.trade.confirm", _T("%s"), XTEXT(80135) );
	if( pAlert ) {
		pAlert->SetEvent( XWM_OK, this, &XSceneTrader::OnClickTradeOK );
		pAlert->SetEvent( XWM_CANCEL, this, &XSceneTrader::OnClickTradeCancel );
	}
// 	m_Layout.CreateLayout("commontextpopup", this);
// 	XWnd *pPopup = Find("img.common.popuptext");
// 	if (pPopup) {
// 		pPopup->SetbModal(TRUE);
// 		xSET_TEXT(this, "text.title", XTEXT(80151));			// "자원거래"
// 		xSET_TEXT(this, "text.common.ask", XTEXT(80135));		// "자원을 거래하시겠습니까?"
// 		xSET_BUTT_TEXT(this, "butt.confirm", XTEXT(80028));			// "거래"
// 		xSET_BUTT_HANDLER(this, "butt.confirm", &XSceneTrader::OnClickTradeOK);
// 		xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneTrader::OnClickPopupCancel);
// 	}
	return 1;
}

int XSceneTrader::OnClickTradeCancel(XWnd *pWnd, DWORD p1, DWORD p2)
{
	// 슬라이드 텍스트 0으로 초기화
	for( int i = 0; i < 5; ++i ) {
		const int num = ACCOUNT->GetResource( (XGAME::xtResource)i );
		_tstring strText = XFORMAT("%d / %s", 0, XE::NtS(num));
		xSET_TEXTF( this
							, strText.c_str()
							, "text.trader.resource%d", i );
	}
	// 슬라이드 초기화
	for (int i = 0; i < XGAME::xRES_MAX; i++) {
		m_resourceGold[i] = 0;
		m_slider[i]->SetCurr(0.f);
	}
	UpdateTotalGold();		// 총 금액 업데이트
	return 1;
}

int XSceneTrader::OnClickTradeOK(XWnd *pWnd, DWORD p1, DWORD p2)
{
	// 교환할 자원량 계산
	int wood = int(ACCOUNT->GetResource(XGAME::xtResource::xRES_WOOD) * m_slider[XGAME::xtResource::xRES_WOOD]->GetCurr());
	int iron = int(ACCOUNT->GetResource(XGAME::xtResource::xRES_IRON) * m_slider[XGAME::xtResource::xRES_IRON]->GetCurr());
	int jewel = int(ACCOUNT->GetResource(XGAME::xtResource::xRES_JEWEL) * m_slider[XGAME::xtResource::xRES_JEWEL]->GetCurr());
	int sulfur = int(ACCOUNT->GetResource(XGAME::xtResource::xRES_SULFUR) * m_slider[XGAME::xtResource::xRES_SULFUR]->GetCurr());
	int mandrake = int(ACCOUNT->GetResource(XGAME::xtResource::xRES_MANDRAKE) * m_slider[XGAME::xtResource::xRES_MANDRAKE]->GetCurr());

	GAMESVR_SOCKET->SendReqTrade(this, wood, iron, jewel, sulfur, mandrake);
// 	OnClickPopupCancel(pWnd, 0, 0);

	return 1;
}

int XSceneTrader::OnClickResource(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (  m_resourceGold[p1] <= 0) {
		// 최대량으로
		m_resourceGold[p1] = ACCOUNT->GetResource(XGAME::xtResource(p1));
		m_slider[p1]->SetCurr(1.0f);
	}	else {
		// 0으로 초기화
		m_resourceGold[p1] = 0;
		m_slider[p1]->SetCurr(0.0f);
	}

	// 슬라이드 업데이트
	switch (p1)
	{
	case XGAME::xRES_WOOD:
		UpdateSlidingResource("text.trader.resource0", XGAME::xRES_WOOD);
		break;
	case XGAME::xRES_IRON:
		UpdateSlidingResource("text.trader.resource1", XGAME::xRES_IRON);
		break;
	case XGAME::xRES_JEWEL:
		UpdateSlidingResource("text.trader.resource2", XGAME::xRES_JEWEL);
		break;
	case XGAME::xRES_SULFUR:
		UpdateSlidingResource("text.trader.resource3", XGAME::xRES_SULFUR);
		break;
	case XGAME::xRES_MANDRAKE:
		UpdateSlidingResource("text.trader.resource4", XGAME::xRES_MANDRAKE);
		break;
	}

	return 1;
}

int XSceneTrader::OnClickRecall(XWnd *pWnd, DWORD p1, DWORD p2)
{
	int idWhistle = XGC->m_traderRecallItem;		// 휘슬 아이템 ID
//	unsigned int gemNum = XGC->m_traderRecallGem;		// 소모되는 젬량
	int gemNum = ACCOUNT->GetCashForTraderRecall();

	XList4<XBaseItem*> itemList;
	ACCOUNT->GetInvenItem(itemList);		// 인벤토리 얻어오기

	bool bWhistle = false;		// 휘슬 아이템 유무

	// 휘슬 아이템 있는지 체크
	int i = 0;
	for( auto pItem : itemList ) {
		if( pItem->GetidProp() == idWhistle ) {
			bWhistle = true;
			break;
		}
		++i;
	}
	if (bWhistle) {
		// 휘슬 아이템이 있으면
		// 팝업
		auto pPopup = new XWndPopup( _T("trade_confirm.xml"), "popup", this );
		if( pPopup ) {
			pPopup->SetEvent( XWM_OK, 
												this,
												&XSceneTrader::OnClickRecallOK,
												XGAME::xSC_SPENT_ITEM );
			xSET_TEXT( pPopup, "text.title", XTEXT( 80130 ) );
			xSET_TEXT( pPopup, "text.msg", XTEXT( 80148 ) );
			xSET_TEXT( pPopup, "text.label", XTEXT( 80132 ) );
			auto pProp = PROP_ITEM->GetpProp( idWhistle );
			if( pProp )
				xSET_IMG( pPopup, "img.ask.itemimg", XE::MakePath( DIR_IMG, pProp->strIcon ) );
		}
		Add( pPopup );
// 		m_Layout.CreateLayout("commonitemtextpopup", this);
// 
// 		XWnd *pPopup = Find("img.common.popupitemtext");
// 		if (pPopup)
// 		{
// 			pPopup->SetbModal(TRUE);
// 
// 			xSET_TEXT(this, "text.title", XTEXT(80130));			// "즉시호출"
// 			xSET_TEXT(this, "text.common.ask", XTEXT(80148));		// "아이템을 사용하여 무역상을 즉시 호출하시겠습니까?"
// 			xSET_BUTT_TEXT(this, "butt.confirm", XTEXT(80132));			// "호출"
// 
// 			xSET_IMG(this, "img.ask.itemimg", XE::MakePath(DIR_IMG, PROP_ITEM->GetpProp(whistleItemID)->strIcon.c_str()));
// 
// 			xSET_BUTT_HANDLER_PARAM(this, "butt.confirm", this, &XSceneTrader::OnClickRecallOK, XGAME::xSC_SPENT_ITEM);
// 			xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneTrader::OnClickPopupCancel);
// 		}
	}	else	{
	// 휘슬 아이템이 없으면
	if( ACCOUNT->IsEnoughCash( gemNum ) ) {
		// 팝업
		auto pPopup = new XWndPaymentByCash( XTEXT(80149), XTEXT(80130) );
		pPopup->SetItem( (ID)XGC->m_traderRecallItem, gemNum );
		Add( pPopup );
		pPopup->SetEvent( XWM_OK, 
											this, 
											&XSceneTrader::OnClickRecallOK, 
											XGAME::xSC_SPENT_GEM );
		// 		auto pPopup = new XWndPopup( _T( "trade_confirm.xml" ), "popup", this );
// 		if( pPopup ) {
// 			xSET_BUTT_HANDLER_PARAM( pPopup, "butt.ok", 
// 															 this, 
// 															 &XSceneTrader::OnClickRecallOK, 
// 															 XGAME::xSC_SPENT_ITEM );
// 			xSET_TEXT( pPopup, "text.title", XTEXT( 80130 ) );
// 			xSET_TEXT( pPopup, "text.msg", XTEXT( 80149 ), gemNum );
// 			xSET_TEXT( pPopup, "text.label", XTEXT( 80132 ) );
// 			auto pProp = PROP_ITEM->GetpProp( whistleItemID );
// 			if( pProp )
// 				xSET_IMG( pPopup, "img.ask.itemimg", XE::MakePath( DIR_IMG, pProp->strIcon ) );
// 		}
// 		Add( pPopup );
		// 			m_Layout.CreateLayout("commonitemtextpopup", this);
// 			XWnd *pPopup = Find("img.common.popupitemtext");
// 			if (pPopup)			{
// 				pPopup->SetbModal(TRUE);
// 
// 				xSET_TEXT(this, "text.title", XTEXT(80130));			// "즉시호출"
// 				xSET_TEXT(this, "text.common.ask", XE::Format(XTEXT(80149), gemNum));		// "아이템이 부족합니다. 캐쉬 %d개를 이용하여 즉시 호출하시겠습니까?"
// 				xSET_BUTT_TEXT(this, "butt.confirm", XTEXT(80132));			// "호출"
// 
// 				xSET_IMG(this, "img.ask.itemimg", XE::MakePath(DIR_IMG, PROP_ITEM->GetpProp(whistleItemID)->strIcon.c_str()));
// 
// 				xSET_BUTT_HANDLER_PARAM(this, "butt.confirm", this, &XSceneTrader::OnClickRecallOK, XGAME::xSC_SPENT_GEM);
// 				xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneTrader::OnClickPopupCancel);
// 			}
	} else {
		// 휘슬 아이템도 업고 캐쉬도 부족
		// 팝업
		XWND_ALERT_T( _T("%s"), XTEXT(80150) );
// 			auto pPopup = new XWndPopup( _T( "trade_confirm.xml" ), "popup" );
// 			if( pPopup ) {
// 				xSET_BUTT_HANDLER_PARAM( pPopup, "butt.confirm", this, &XSceneTrader::OnClickRecallOK, XGAME::xSC_SPENT_GEM );
// 			}
// 			Add( pPopup );
// 			m_Layout.CreateLayout( "commontextpopup", this );
// 
// 			XWnd *pPopup = Find( "img.common.popuptext" );
// 			if( pPopup ) {
// 				pPopup->SetbModal( TRUE );
// 
// 				xSET_TEXT( this, "text.title", XTEXT( 80130 ) );			// "즉시호출"
// 				xSET_TEXT( this, "text.common.ask", XTEXT( 80150 ) );		// "아이템과 캐쉬가 부족합니다. 구매해주세요"
// 				xSET_BUTT_TEXT( this, "butt.confirm", XTEXT( 2 ) );				// "확인"
// 
// 				xSET_BUTT_HANDLER( this, "butt.confirm", &XSceneTrader::OnClickPopupCancel );
// 				xSET_BUTT_HANDLER( this, "butt.cancel", &XSceneTrader::OnClickPopupCancel );
// 			}
	}
}
	return 1;
}

int XSceneTrader::OnClickRecallOK(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (GAMESVR_SOCKET)	{
#pragma message("이거 에러처리 클라에서 하지 말고 서버에서 하도록 해서 클라가 패치받지 않아도 수정되게 할것.")
		// 소비되는 종류에 따라 호출
		if ((XGAME::xtSpentCall)p1 == XGAME::xSC_SPENT_ITEM) {
			const int numItem = ACCOUNT->GetNumItems( XGC->m_traderRecallItem );
			if( numItem > 0 )
				GAMESVR_SOCKET->SendCashTradeCall(this, (XGAME::xtSpentCall)p1);
		} else if ((XGAME::xtSpentCall)p1 == XGAME::xSC_SPENT_GEM) {
			if( ACCOUNT->IsCallableTraderByGem() )
				GAMESVR_SOCKET->SendCashTradeCall(this, (XGAME::xtSpentCall)p1);
		}
	}

//	OnClickPopupCancel(pWnd, 0, 0);

	return 1;
}

// int XSceneTrader::OnClickPopupCancel(XWnd *pWnd, DWORD p1, DWORD p2)
// {
// 	if (pWnd->GetpParent())
// 		pWnd->GetpParent()->SetbDestroy(TRUE);
// 
// 	return 1;
// }

int XSceneTrader::OnSlidingResource0(XWnd *pWnd, DWORD p1, DWORD p2)
{
	UpdateSlidingResource("text.trader.resource0", XGAME::xRES_WOOD);

	return 1;
}

int XSceneTrader::OnSlidingResource1(XWnd *pWnd, DWORD p1, DWORD p2)
{
	UpdateSlidingResource("text.trader.resource1", XGAME::xRES_IRON);

	return 1;
}

int XSceneTrader::OnSlidingResource2(XWnd *pWnd, DWORD p1, DWORD p2)
{
	UpdateSlidingResource("text.trader.resource2", XGAME::xRES_JEWEL);

	return 1;
}

int XSceneTrader::OnSlidingResource3(XWnd *pWnd, DWORD p1, DWORD p2)
{
	UpdateSlidingResource("text.trader.resource3", XGAME::xRES_SULFUR);

	return 1;
}

int XSceneTrader::OnSlidingResource4(XWnd *pWnd, DWORD p1, DWORD p2)
{
	UpdateSlidingResource("text.trader.resource4", XGAME::xRES_MANDRAKE);

	return 1;
}

void XSceneTrader::UILoad()
{
	// UI 로드
	m_Layout.CreateLayout("common_bg", this);
//	m_Layout.CreateLayout("common_bg_goldcash", this);

	m_Layout.CreateLayout("trader_bg", this);
	m_Layout.CreateLayout("slidertab", this);
	m_Layout.CreateLayout("resourcetab", this);
	xSET_BUTT_HANDLER(this, "butt.back", &XSceneTrader::OnBack);		// 백버튼

	xSET_BUTT_HANDLER(this, "butt.trader.trade", &XSceneTrader::OnClickTrade);		// 거래 버튼
	xSET_BUTT_HANDLER(this, "butt.trader.cancel", &XSceneTrader::OnClickTradeCancel);	// 취소 버튼

	// 자원 아이콘 Min, Max 버튼
	xSET_BUTT_HANDLER_PARAM(this, "img.trader.wood", this, &XSceneTrader::OnClickResource, XGAME::xRES_WOOD);
	xSET_BUTT_HANDLER_PARAM(this, "img.trader.iron", this, &XSceneTrader::OnClickResource, XGAME::xRES_IRON);
	xSET_BUTT_HANDLER_PARAM(this, "img.trader.jewel", this, &XSceneTrader::OnClickResource, XGAME::xRES_JEWEL);
	xSET_BUTT_HANDLER_PARAM(this, "img.trader.sulfur", this, &XSceneTrader::OnClickResource, XGAME::xRES_SULFUR);
	xSET_BUTT_HANDLER_PARAM(this, "img.trader.mandrake", this, &XSceneTrader::OnClickResource, XGAME::xRES_MANDRAKE);

	// 슬라이더
	XWnd *pWnd;
	pWnd = new XWndSliderOption(270.f, 106.f, 115.f, 15.f, 0, 1.0f, 0);
	pWnd->SetEvent(XWM_SLIDING, this, &XSceneTrader::OnSlidingResource0);
	m_slider[0] = static_cast<XWndSliderOption*>(pWnd);
	Add(pWnd);

	pWnd = new XWndSliderOption(270.f, 160.f, 115.f, 15.f, 0, 1.0f, 0);
	pWnd->SetEvent(XWM_SLIDING, this, &XSceneTrader::OnSlidingResource1);
	m_slider[1] = static_cast<XWndSliderOption*>(pWnd);
	Add(pWnd);

	pWnd = new XWndSliderOption(270.f, 214.f, 115.f, 15.f, 0, 1.0f, 0);
	pWnd->SetEvent(XWM_SLIDING, this, &XSceneTrader::OnSlidingResource2);
	m_slider[2] = static_cast<XWndSliderOption*>(pWnd);
	Add(pWnd);

	pWnd = new XWndSliderOption(270.f, 268.f, 115.f, 15.f, 0, 1.0f, 0);
	pWnd->SetEvent(XWM_SLIDING, this, &XSceneTrader::OnSlidingResource3);
	m_slider[3] = static_cast<XWndSliderOption*>(pWnd);
	Add(pWnd);

	pWnd = new XWndSliderOption(270.f, 322.f, 115.f, 15.f, 0, 1.0f, 0);
	pWnd->SetEvent(XWM_SLIDING, this, &XSceneTrader::OnSlidingResource4);
	m_slider[4] = static_cast<XWndSliderOption*>(pWnd);
	Add(pWnd);

	// 리소스 텍스트
	xSET_TEXT(this, "text.trader.resource0", XE::Format(_T("%d / %d "), 0, ACCOUNT->GetResource(XGAME::xRES_WOOD)));
	xSET_TEXT(this, "text.trader.resource1", XE::Format(_T("%d / %d"), 0, ACCOUNT->GetResource(XGAME::xRES_IRON)));
	xSET_TEXT(this, "text.trader.resource2", XE::Format(_T("%d / %d"), 0, ACCOUNT->GetResource(XGAME::xRES_JEWEL)));
	xSET_TEXT(this, "text.trader.resource3", XE::Format(_T("%d / %d"), 0, ACCOUNT->GetResource(XGAME::xRES_SULFUR)));
	xSET_TEXT(this, "text.trader.resource4", XE::Format(_T("%d / %d"), 0, ACCOUNT->GetResource(XGAME::xRES_MANDRAKE)));

	// 자원 시세 텍스트
	xSET_TEXT(this, "text.trader.rate0", XE::Format(XTEXT(80058), XGC->m_resourceRate[XGAME::xRES_WOOD]));
	xSET_TEXT(this, "text.trader.rate1", XE::Format(XTEXT(80058), XGC->m_resourceRate[XGAME::xRES_IRON]));
	xSET_TEXT(this, "text.trader.rate2", XE::Format(XTEXT(80058), XGC->m_resourceRate[XGAME::xRES_JEWEL]));
	xSET_TEXT(this, "text.trader.rate3", XE::Format(XTEXT(80058), XGC->m_resourceRate[XGAME::xRES_SULFUR]));
	xSET_TEXT(this, "text.trader.rate4", XE::Format(XTEXT(80058), XGC->m_resourceRate[XGAME::xRES_MANDRAKE]));

	// 자원 시세 임시
	/*
	xSET_TEXT(this, "text.trader.rate0", XE::Format(XTEXT(80058), 1));
	xSET_TEXT(this, "text.trader.rate1", XE::Format(XTEXT(80058), 1));
	xSET_TEXT(this, "text.trader.rate2", XE::Format(XTEXT(80058), 1));
	xSET_TEXT(this, "text.trader.rate3", XE::Format(XTEXT(80058), 1));
	xSET_TEXT(this, "text.trader.rate4", XE::Format(XTEXT(80058), 1));
	*/

	// 전체 교환 금액 텍스트
	xSET_TEXT(this, "text.trader.totalgold", XE::Format(_T("%d"), 0));
}

void XSceneTrader::UpdateSlidingResource(const char *key, XGAME::xtResource resource)
{
	if (m_slider[resource])	{
		const int maxRes = ACCOUNT->GetResource(resource);
		const int numCurr = int(maxRes * m_slider[resource]->GetCurr());
		const _tstring str1 = XE::NtS(numCurr);
		const _tstring str2 = XE::NtS(maxRes);
		xSET_TEXT(this, key, XFORMAT("%s / %s", str1.c_str(), str2.c_str()));
		m_resourceGold[resource] = int(numCurr * XGC->m_resourceRate[resource]);
		UpdateTotalGold();
	}
}

void XSceneTrader::UpdateTotalGold()
{
	m_totalGold = 0;
	for (int i = 0; i < XGAME::xRES_MAX; i++)
		m_totalGold += m_resourceGold[i];

//	xSET_TEXT(this, "text.trader.totalgold", XE::Format(_T("%d"), m_totalGold));
	xSET_TEXT(this, "text.trader.totalgold", XE::NumberToMoneyString( m_totalGold ) );
}

void XSceneTrader::UpdateCharacter()
{
	// 리콜시간 텍스트 갱신
	UpdateRecallTimeCheck();

//	bool bChar = (ACCOUNT->GetTradeTimer()->IsOn() == 1);
//	const bool bTrader = (ACCOUNT->GettimerTrader().IsOn() == FALSE);
	const bool bTrader = ACCOUNT->IsReadyTrader();

	xSET_SHOW( this, "wnd.trader.char", bTrader );
	xSET_SHOW( this, "butt.trader.trade", bTrader );
	xSET_SHOW( this, "wnd.trader.recall", !bTrader );
// 	xSET_SHOW( this, "text.trader.recalltime", !bTrader );
	xSET_SHOW( this, "text.trader.whistle", !bTrader );
	xSET_SHOW( this, "text.trader.cash", !bTrader );
	if( !bTrader ) {
		// 무역상이 없으면
// 		xSET_TEXT( this, "text.trader.recalltime"
// 			, XE::Format( _T( "%s\n%d : %d : %d" ), XTEXT( 80131 ), 48, 12, 03 ) );
		xSET_TEXT( this, "text.trader.whistle", _T( "x1" ) );
		const int cashCost = ACCOUNT->GetCashForTraderRecall();
		xSET_TEXT( this, "text.trader.cash", XFORMAT( "x%d", cashCost ) );
		xSET_BUTT_HANDLER( this, "butt.trader.recall", &XSceneTrader::OnClickRecall );
	}
	OnClickTradeCancel(NULL, 0, 0);
}

/**
 @brief 리콜 남은 시간 텍스트 갱신
*/
void XSceneTrader::UpdateRecallTimeCheck()
{
	int secRemain = ACCOUNT->GetsecRemainByTraderRecall();
	xSET_SHOW( this, "text.trader.recalltime", secRemain > 0 );
	if( secRemain > 0 ) {
		const auto strRemain = XGAME::GetstrResearchTime( secRemain );
		xSET_TEXT( this, "text.trader.recalltime", strRemain.c_str() );
	}
}

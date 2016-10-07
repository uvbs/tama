#include "StdAfx.h"
#include "XSceneShop.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XQuestMng.h"
#include "XWndTemplate.h"
#ifdef _VER_ANDROID
#include "XFramework/android/JniHelper.h"
#endif
#include "XSoundMng.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneShop *SCENE_SHOP = NULL;

void XSceneShop::Destroy()
{	
//	SAFE_DELETE(m_pLayout);
	XBREAK(SCENE_SHOP == NULL);
	XBREAK(SCENE_SHOP != this);
	SCENE_SHOP = NULL;
	SOUNDMNG->CloseSound(16);
}

XSceneShop::XSceneShop(XGame *pGame)
	: XSceneBase( pGame, XGAME::xSC_SHOP )
	, m_Layout(_T("layout_shop.xml"))
{ 
	XBREAK(SCENE_SHOP != NULL);
	SCENE_SHOP = this;
	Init(); 

	m_Layout.CreateLayout("common_bg", this);
//	m_Layout.CreateLayout("common_bg_goldcash", this);
	m_Layout.CreateLayout("shop", this);

	// 텍스트
// 	xSET_TEXT(this, "text.common.gold", XE::Format(_T("%d"), ACCOUNT->GetGold()));
// 	xSET_TEXT(this, "text.common.cash", XE::Format(_T("%d"), ACCOUNT->GetCashtem()));
	
	// 버튼
	xSET_BUTT_HANDLER(this, "butt.back", &XSceneShop::OnBack);

	xSET_BUTT_HANDLER(this, "butt.shop.cash", &XSceneShop::OnClickTabCash);		// 캐쉬 탭
//	xSET_BUTT_HANDLER(this, "butt.shop.etc", &XSceneShop::OnClickTabEtc);		// 기타 탭
	xSET_SHOW( this, "butt.shop.etc", FALSE );	// 필요없어서 삭제

	// 리스트
	m_pList = SafeCast<XWndList*, XWnd*>(Find("list.shop"));
//	m_pList->SetScrollDir(XE::xHORIZ);
	m_pList->SetScrollHorizOnly();

	m_shopType = SHOPTYPE_CASH;
	UpdateList();
}

void XSceneShop::Create(void)
{
	XSceneBase::Create();
}

void XSceneShop::Update()
{
	XGAME::CreateUpdateTopResource( this );
	XSceneBase::Update();
}

int XSceneShop::Process(float dt)
{ 
	return XSceneBase::Process( dt );
}

//
void XSceneShop::Draw(void)
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneShop::OnLButtonDown(float lx, float ly)
{
	XSceneBase::OnLButtonDown( lx, ly );
}
void XSceneShop::OnLButtonUp(float lx, float ly) {
	XSceneBase::OnLButtonUp( lx, ly );
}
void XSceneShop::OnMouseMove(float lx, float ly) {
	XSceneBase::OnMouseMove( lx, ly );
}

int XSceneShop::OnBack(XWnd *pWnd, DWORD p1, DWORD p2)
{
	DoExit(XGAME::xSC_WORLD);
	return 1;
}

void XSceneShop::UpdateList()
{
	if (m_pList)	{
		m_pList->DestroyAllItem();		// 우선 리스트 초기화
		m_pList->SetvLT(XE::VEC2(0, 0));

		if (m_shopType == SHOPTYPE_CASH)	{
			// 캐쉬 탭
			const auto& aryCashitems = XGC->GetAryCashItems();	
			int i = 0;
			for( const auto& infoGoods : aryCashitems ) {
				// 리스트 아이템
				XWndShopElem *pElem = new XWndShopElem( infoGoods.m_productID );
				pElem->SetPosLocal(XE::VEC2(i * (pElem->GetpSurface()->GetWidth() + 6), 0));
				m_pList->Add(i + 1, pElem);

				// 캐쉬 이미지
				XWndImage *pImg = new XWndImage(XE::MakePath(DIR_UI, infoGoods.m_img.c_str())
																			, XE::xPF_ARGB8888, infoGoods.m_pos.x, infoGoods.m_pos.y);
				if (pImg)
					pElem->Add(pImg);

				// 아이템 이름
				XWndTextString *pText = new XWndTextString(XE::VEC2(0, 160), XE::Format(_T("%s (%d)"),  XTEXT(infoGoods.m_text), infoGoods.m_gem) );
				if (pText)
				{
					pText->SetStyle(xFONT::xSTYLE_STROKE);
					pText->SetLineLength(123.f);
					pText->SetAlign(XE::xALIGN_HCENTER);
					pElem->Add(pText);
				}

				// 아이템 가격
				_tstring strCost;
				if( infoGoods.m_contry == _T("USD") ) {
					strCost = XFORMAT("$ %.2f", infoGoods.m_price );
				} else
				if( infoGoods.m_contry == _T("softnyx") ) {
					strCost = XFORMAT( "cash %.0f", infoGoods.m_price );
				} else {
					strCost = XFORMAT( "%.2f", infoGoods.m_price );
				}
				pText = new XWndTextString(XE::VEC2(0, 180), strCost, FONT_RESNUM );
				if (pText) {
					pText->SetStyle(xFONT::xSTYLE_STROKE);
					pText->SetLineLength(123.f);
					pText->SetAlign(XE::xALIGN_HCENTER);
					pElem->Add(pText);
				}
				i++;
			}
			m_pList->SetEvent(XWM_SELECT_ELEM, this, &XSceneShop::OnClickList);
		}
		else if (m_shopType == SHOPTYPE_ETC)
		{
			// 기타 탭

			auto etcItems = XGC->GetArrEtcItems();
			int i = 0;
			for (auto it = etcItems.begin(); it != etcItems.end(); it++)
			{
				auto prop = PROP_ITEM->GetpProp(*it);

				XWndShopElem *pElem = new XWndShopElem( prop );
				//pElem->SetSizeLocal(pElem->GetpSurface()->GetWidth(), pElem->GetpSurface()->GetHeight());
				pElem->SetPosLocal(XE::VEC2(i * (pElem->GetpSurface()->GetWidth() + 6), 0));
// 				pElem->m_pEtcItemProp = prop;		// 아이템 정보
				m_pList->Add(i + 1, pElem);

				// 캐쉬 이미지
				XWndImage *pImg = new XWndImage(TRUE, XE::MakePath(DIR_IMG, prop->strIcon), 37, 37);
				if (pImg)
					pElem->Add(pImg);

				// 아이템 이름
				XWndTextString *pText = new XWndTextString(XE::VEC2(15, 160), XTEXT(prop->idName), FONT_NANUM, 20.f);
				if (pText)
				{
					pText->SetStyle(xFONT::xSTYLE_STROKE);
					pText->SetLineLength(100.f);
					pText->SetAlign(XE::xALIGN_CENTER);
					pElem->Add(pText);
				}

				// 아이템 가격
				pText = new XWndTextString(XE::VEC2(38, 180), XE::Format(_T("gem %d"), prop->cashCost), FONT_NANUM, 20.f);
				if (pText)
				{
					pText->SetStyle(xFONT::xSTYLE_STROKE);
					pText->SetLineLength(50.f);
					pText->SetAlign(XE::xALIGN_CENTER);
					pElem->Add(pText);
				}
				i++;
			}
// 			XWndShopElem *pElem = new XWndShopElem();
// 			if (m_pList->GetViewSize().x < XE::VEC2(etcItems.size() * (pElem->GetpSurface()->GetWidth() + 6) - 6).x)
// 				m_pList->SetViewSize(XE::VEC2(etcItems.size() * (pElem->GetpSurface()->GetWidth() + 6) - 6, 216));
// 			SAFE_DELETE(pElem);

			m_pList->SetEvent(XWM_SELECT_ELEM, this, &XSceneShop::OnClickList);
		}
	}
}

int XSceneShop::OnClickTabCash(XWnd *pWnd, DWORD p1, DWORD p2)
{
	m_shopType = SHOPTYPE_CASH;

	UpdateList();
	return 1;
}

int XSceneShop::OnClickTabEtc(XWnd *pWnd, DWORD p1, DWORD p2)
{
	m_shopType = SHOPTYPE_ETC;

	UpdateList();
	return 1;
}

int XSceneShop::OnClickList(XWnd *pWnd, DWORD p1, DWORD p2)
{
	ID idItem = p2;
	XWndShopElem *pElem = dynamic_cast<XWndShopElem*>(pWnd->Find(idItem));

	if (pElem)
	{
		if (m_shopType == SHOPTYPE_CASH)
		{
			// 캐쉬

			// 팝업
			m_Layout.CreateLayout("commontextpopup", this);

			XWnd *pPopup = Find("img.common.popup");
			if (pPopup)
			{
				pPopup->SetbModal(TRUE);

				xSET_TEXT(this, "text.title", XTEXT(80070));			// "구매"
				xSET_TEXT(this, "text.common.ask", XTEXT(80071));		// "정말로 구매하시겠습니까?"
				xSET_BUTT_TEXT(this, "butt.ok", XTEXT(80070));			// "구매"

// 				xSET_BUTT_HANDLER_PARAM(this, "butt.ok", this, &XSceneShop::OnClickBuyCash, (DWORD)pElem->m_pCashItem);
				xSET_BUTT_HANDLER_PARAM( this, "butt.ok", this, &XSceneShop::OnClickBuyCash, (DWORD)pElem->getid() );
				xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneShop::OnClickPopupCancel);
			}
		}
		else if (m_shopType == SHOPTYPE_ETC)
		{
			// 기타

			if (pElem->GetpEtcItemProp()->cashCost <= ACCOUNT->GetCashtem())
			{
				// 팝업
				m_Layout.CreateLayout("commontextpopup", this);

				XWnd *pPopup = Find("img.common.popup");
				if (pPopup)
				{
					pPopup->SetbModal(TRUE);
					
					xSET_TEXT(this, "text.title", XTEXT(80070));			// "구매"
					xSET_TEXT(this, "text.common.ask", XTEXT(80071));		// "정말로 구매하시겠습니까?"
					xSET_BUTT_TEXT(this, "butt.ok", XTEXT(80070));			// "구매"

					xSET_BUTT_HANDLER_PARAM(this, "butt.ok", this, &XSceneShop::OnClickBuyEtc, (DWORD)pElem->GetpEtcItemProp());
					xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneShop::OnClickPopupCancel);
				}
			}
			else
			{
				// 캐쉬 부족

				// 팝업
				m_Layout.CreateLayout("commontextpopup", this);

				XWnd *pPopup = Find("img.common.popup");
				if (pPopup)
				{
					pPopup->SetbModal(TRUE);
					
					xSET_TEXT(this, "text.title", XTEXT(80141));			// "캐쉬 부족"
					xSET_TEXT(this, "text.common.ask", XTEXT(80140));		// "캐쉬가 부족합니다."
					xSET_BUTT_TEXT(this, "butt.ok", XTEXT(2));				// "확인"

					xSET_BUTT_HANDLER(this, "butt.ok", &XSceneShop::OnClickPopupCancel);
					xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneShop::OnClickPopupCancel);
				}
			}
		}
	}

	return 1;
}






int XSceneShop::OnClickBuyCash(XWnd *pWnd, DWORD p1, DWORD p2)
{
//	XGlobalConst::cashItem* pCashItem = (XGlobalConst::cashItem*)p1;
	ID idWndGoods = p1;

	auto pWndElem = SafeCast<XWndShopElem*>( Find( idWndGoods ) );
	if( XBREAK(pWndElem == nullptr) )
		return 1;
	auto idsProduct = pWndElem->GetidsProduct();
	// 서버에 페이로드 요청
	GAMESVR_SOCKET->SendReqCreatePayload( GAME, idsProduct );
	// 일단 구 방식으로 결제 테스트
#if defined(_VER_ANDROID)
// 	const std::string base64EncodedPublicKey = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAm5PqbcvMo7gjcKafqQ24mfI67rL/kF8vQFoVXqZnv/7WaUkFUqD9MdUWsDPhg1rb+3P4UHjnpE7a+JnUefnlJurI1edgifrkgtQtcmT9acrIctqTXbSHglEosceVF+R/1W3lD2aDp7TczJyhKUNH38ivRKC460479sUvqy4I9yNB8eN7t6KcBPvQ6t0eF5e65yd9pXq08srrNYSaAjNWFJdYMDG3X8P/MBFo+R0vUXtfz34uceYGDyMi/J2J4grng9KF1fcunOsgPnQiYdFLqebCEgC/wNxKkVViIAEYd0MjFeTkqkG/6H01WVSIz7tbQsumSjxBP7hcX4DiFlrnvQIDAQAB";
// 	JniHelper::InitGoogleIAPAsync( base64EncodedPublicKey );
//	GAME->DoAsyncBuyIAP( idsProduct.c_str() );
#endif 
// #ifdef _VER_ANDROID
// 	//if (GAMESVR_SOCKET)
// 		//GAMESVR_SOCKET->SendCashItemBuyAndroid(this, pCashItem);
// 
// 	CppToJava("buy", pCashItem->m_productID.c_str());		// 인앱 결제 요청
// 
// #elif defined(_VER_IOS)
// 	if (GAMESVR_SOCKET)
// 		GAMESVR_SOCKET->SendCashItemBuyIOS(this, pCashItem);
// #else
// 	if (GAMESVR_SOCKET)
// 		GAMESVR_SOCKET->SendCashItemBuyAndroid(this, pCashItem);
// #endif 

	OnClickPopupCancel(pWnd, 0, 0);
	
	return 1;
}

int XSceneShop::OnClickBuyEtc(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XPropItem::xPROP *prop = (XPropItem::xPROP *)p1;

	if (GAMESVR_SOCKET)
		GAMESVR_SOCKET->SendItemBuy(this, prop->idProp, XGAME::xSHOP_CASHSHOP, XGAME::xCOIN_CASH );

	OnClickPopupCancel(pWnd, 0, 0);

	return 1;
}

int XSceneShop::OnClickPopupCancel(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(TRUE);

	return 1;
}

void XSceneShop::RevUpdate(bool bBuy)
{
	if (bBuy)
		SOUNDMNG->OpenPlaySound(13);		// 구입소리
// 	xSET_TEXT(this, "text.common.gold", XE::Format(_T("%d"), ACCOUNT->GetGold()));
// 	xSET_TEXT(this, "text.common.cash", XE::Format(_T("%d"), ACCOUNT->GetCashtem()));
}

int XSceneShop::OnEnterScene( XWnd*, DWORD, DWORD )
{
	SOUNDMNG->OpenPlaySound( 16, true );		// 배경음.
	const auto idScene = GetidScene();
	bool bHave = ACCOUNT->GetpQuestMng()->IsHaveQuestTypeWithParam( XGAME::xQC_EVENT_UI_ACTION
																																, XGAME::xUA_ENTER_SCENE
																																, idScene );
	if( bHave )
		GAMESVR_SOCKET->SendUIAction( XGAME::xUA_ENTER_SCENE, idScene );
	return XSceneBase::OnEnterScene( nullptr, 0, 0 );
}

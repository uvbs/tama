#include "StdAfx.h"
#include "XSceneGuildShop.h"
//#include "XGame.h"
#include "XGameWnd.h"
#include "XSceneStorage.h"
#include "XSockGameSvr.h"
#include "XPropItem.h"
#include "XSceneArmory.h"
#include "XSkillMng.h"
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



XSceneGuildShop *SCENE_GUILD_SHOP = NULL;

void XSceneGuildShop::Destroy()
{	
//	SAFE_DELETE(m_pLayout);
	XBREAK(SCENE_GUILD_SHOP == NULL);
	XBREAK(SCENE_GUILD_SHOP != this);
	SCENE_GUILD_SHOP = NULL;
	SOUNDMNG->CloseSound(15);
}

XSceneGuildShop::XSceneGuildShop(XGame *pGame)
	: XSceneBase( pGame, XGAME::xSC_GUILD_SHOP )
	, m_Layout(_T("guild_shop.xml"))
	, m_LayoutTooltip(_T("layout_item.xml"))
{ 
	XBREAK(SCENE_GUILD_SHOP != NULL);
	SCENE_GUILD_SHOP = this;
	Init(); 

	m_Layout.CreateLayout("shop", this);
	xSET_BUTT_HANDLER(this, "butt.back", &XSceneGuildShop::OnBack);
	xSET_BUTT_HANDLER( this, "butt.buy", &XSceneGuildShop::OnClickBuy );
	// 아이템 툴팁모듈이 들어갈 루트 윈도우
	XWnd *pRoot = Find( "wnd.item.tooltip" );
	if( pRoot ) {
		// 아이템 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "item_tooltip", pRoot );
		pRoot->SetbShow( false );
	}
	pRoot = Find( "wnd.hero.tooltip" );
	if( pRoot ) {
		// 영웅 툴팁 레이아웃모듈을 읽는다.
		m_LayoutTooltip.CreateLayout( "hero_tooltip", pRoot );
		pRoot->SetbShow( false );
	}
	SetbUpdate( true );		// Update()가 호출 됩니다.
	// 디폴트로 첫번째 아이템이 선택되어 있다.
// 	auto& aryItem = XGC->m_aryGuildShop;
// 	if( XASSERT(aryItem.size() > 0) ) {
// 		auto pProp = PROP_ITEM->GetpProp( aryItem[0].idsItem );
// 		if( XASSERT(pProp) )
// 			m_idSelected = pProp->idProp;
// 	}
	auto 
	pWnd = Find("wnd.face");
	if( pWnd )
		pWnd->SetEvent( XWM_CLICKED, this, &XSceneGuildShop::OnClickGirl, 0 );
	pWnd = Find( "wnd.breast" );
	if( pWnd )
		pWnd->SetEvent( XWM_CLICKED, this, &XSceneGuildShop::OnClickGirl, 1 );
}

void XSceneGuildShop::Create(void)
{
	XEBaseScene::Create();
}

int XSceneGuildShop::OnEnterScene( XWnd* pWnd, DWORD dw1, DWORD dw2 )
{
	SOUNDMNG->OpenPlaySound( 15, true );		// 무기점 배경음.
	return XSceneBase::OnEnterScene( pWnd, dw1, dw2 );
}

void XSceneGuildShop::Update()
{
	XGAME::CreateUpdateTopGuildCoin( this );
// 	if( m_idSelected == 0 )
// 		m_pSelectItem = nullptr;
	// 텍스트
// 	xSET_TEXT( this, "text.common.gold", XE::NumberToMoneyString(ACCOUNT->GetptGuild()) );
// 	xSET_TEXT( this, "text.common.cash", XE::NumberToMoneyString(ACCOUNT->GetCashtem()) );
	// 상점 목록이 있는지 체크
	XWnd *pRootRack = Find("img.rack");
	if( XASSERT(pRootRack) ) {
		auto& aryItem = XGC->m_aryGuildShop;
		const XE::VEC2 vStart( 14, 16 );
		const XE::VEC2 vDist( 47, 52 );	// 간격
		XE::VEC2 v = vStart;
		int idx = 0;
		int idxSlot = 0;
		for( auto& slot : aryItem ) {
			std::string cidsItem = SZ2C( slot.idsItem );
			auto pProp = PROP_ITEM->GetpProp( slot.idsItem );
			if( XASSERT(pProp) ) {
				ID idItem = pProp->idProp;
				XBREAK( idItem == 0 );
				XWnd *pWnd = Findf( "elem.%s", cidsItem.c_str() );
				if( pWnd == nullptr ) {
					auto pElem = new XWndStoragyItemElem( v, slot.idsItem );
					pWnd = pElem;
					pElem->SetScaleLocal( 0.8f );
					pElem->SetstrIdentifierf( "elem.%s", cidsItem.c_str() );
					pElem->SetEvent( XWM_CLICKED, this, &XSceneGuildShop::OnClickSlot, idxSlot );
					// 선택되어있는 아이템은 외곽선을 표시한다.
					pRootRack->Add( pElem );
					auto vSizeElem = pElem->GetSizeLocal();
// 					auto pCostLabel = new XWndImage( PATH_UI("cost_bg.png"), XE::VEC2(0) );
// 					pRootRack->Add( pCostLabel );
// 					XE::VEC2 vLocal(0,29);
// 					vLocal.x = ( vSizeElem.w * 0.5f ) - ( pCostLabel->GetSizeLocal().w * 0.5f );
// 					pCostLabel->SetPosLocal( v + vLocal );
					auto pText = new XWndTextString( v+XE::VEC2(0,37), XE::VEC2(vSizeElem.w,9), XE::NumberToMoneyString(slot.cost), FONT_RESNUM, 17.f );
					pText->SetStyleStroke();
					pText->SetAlignHCenter();
					pRootRack->Add( pText );
					v.x += vDist.w;
					if( ++idx >= 4 ) {
						v.x = vStart.x;
						v.y += vDist.h;
						idx = 0;
					}
				}
				auto pElem = SafeCast<XWndStoragyItemElem*>( pWnd );
				if( m_idxSlot == idxSlot ) {
					pElem->SetbSelected( true );
				} else
					pElem->SetbSelected( false );
				++idxSlot;
			}
		}
	}
	// 선택한 아이템의 툴팁을 업데이트 한다.
	UpdateToolTip();
	XSceneBase::Update();
}

void XSceneGuildShop::UpdateToolTip()
{
	if( m_idxSlot < 0 )
		return;
	auto& slot = XGC->m_aryGuildShop[ m_idxSlot ];
	auto& idsItem = slot.idsItem;
	auto pProp = PROP_ITEM->GetpProp( idsItem );
	if( XASSERT(pProp) ) {
		if( pProp->IsSoul() ) {
			XWnd *pRoot = Find( "wnd.hero.tooltip" );
			if( pRoot == nullptr )
				return;
			ShowRootHeroTooltip( true );
			auto pPropHero = PROP_HERO->GetpProp( pProp->strIdHero );
			if( pPropHero ) {
				XGAME::UpdateHeroTooltip( pPropHero->idProp, pRoot, slot.cost );
			}
		} else {
			XWnd *pRoot = Find( "wnd.item.tooltip" );
			if( pRoot == nullptr )
				return;
			ShowRootItemTooltip( true );
			XGAME::UpdateItemTooltip( pProp, pRoot, 0 );
			if( slot.cost > 0 ) {
				xSET_SHOW( pRoot, "wnd.sell", true );
				auto pText = xSET_TEXT( pRoot, "text.cost", XE::NumberToMoneyString( slot.cost ) );
				auto pImg = xSET_IMG( pRoot, "img.gold", PATH_UI("guild_coin_small.png"));
				if( ACCOUNT->GetptGuild() < slot.cost )
					pText->SetColorText( XCOLOR_RED );
			}
			else
				xSET_SHOW( pRoot, "wnd.sell", FALSE );

		}
	}
}

int XSceneGuildShop::Process(float dt)
{
	return XEBaseScene::Process( dt );
}

//
void XSceneGuildShop::Draw(void)
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneGuildShop::OnLButtonDown(float lx, float ly)
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneGuildShop::OnLButtonUp(float lx, float ly) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneGuildShop::OnMouseMove(float lx, float ly) {
	XEBaseScene::OnMouseMove( lx, ly );
}

int XSceneGuildShop::OnBack(XWnd *pWnd, DWORD p1, DWORD p2)
{
	DoExit(XGAME::xSC_GUILD);
	return 1;
}

void XSceneGuildShop::ShowRootItemTooltip( bool bFlag )
{
	xSET_SHOW( this, "wnd.item.tooltip", bFlag );
	xSET_SHOW( this, "wnd.hero.tooltip", !bFlag );
}
void XSceneGuildShop::ShowRootHeroTooltip( bool bFlag )
{
	xSET_SHOW( this, "wnd.hero.tooltip", bFlag );
	xSET_SHOW( this, "wnd.item.tooltip", !bFlag );
}

int XSceneGuildShop::OnClickSlot(XWnd *_pWnd, DWORD p1, DWORD p2)
{
//	ID idItem = p1;
//	m_idSelected = idItem;
	int idxSlot = (int)p1;
	m_idxSlot = idxSlot;
	SetbUpdate( true );
	return 1;
}

int XSceneGuildShop::OnClickBuy(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if( ACCOUNT->GetptGuild() > 0 ) {
		GAMESVR_SOCKET->SendReqBuyGuildItem( GAME, m_idxSlot );
	} else {
		auto pWnd = new XWndPopupCashPay();
		pWnd->GetButtOk()->SetEvent( XWM_CLICKED, this, &XSceneGuildShop::OnClickCashPayForGuildPoint );
		pWnd->SetTitle( XTEXT(2210) );
		int needPoint = 30;
		TCHAR szBuff[ 1024 ];
		_tstring strSrc = XE::Format(XTEXT(2209), needPoint );
		XE::ConvertJosaStr( szBuff, strSrc);
		pWnd->SetNeedText( szBuff );
		pWnd->SetCost( 1000 );

		Add( pWnd );
	}
	return 1;

}
/**
 @brief pRoot아래에 있는 아이템 툴팁모듈을 업데이트한다.
 @param cost 아이템 가격을 표시해야할때 가격을 넘겨준다.
*/
// static void sUpdateHeroTooltip( XPropHero::xPROP *pProp, XWnd *pRoot, int cost )
// {
// 	// 영웅 이미지
// 	if( pProp == nullptr )
// 		return;
// 	// 영웅 이름
// 	auto pText =
// 	xSET_TEXT( pRoot, "text.name", pProp->strName );
// // 	if( pText )
// // 		pText->SetColorText( XGAME::GetGradeColor( pProp->GetGrade() ) );
// 	// 영웅 이미지
// 	xSET_IMG( pRoot, "img.hero", XE::MakePath( DIR_IMG, pProp->strFace ) );
// 	// 별
// // 	int numStar = pProp->GetGrade();
// // 	for( int i = 0; i < 5; ++i ) {
// // 		bool bFlag = false;
// // 		if( i < numStar )
// // 			bFlag = true;
// // 		xSET_SHOWF( pRoot, bFlag, "img.star.%d", i+1 );
// // 	}
// 	// 영웅 등급 텍스트
// //	pText = xSET_TEXT( pRoot, "text.grade", XFORMAT("%s 장비", XGAME::GetStrGrade(pProp->grade)));
// // 	if( pText )
// // 		pText->SetColorText( XGAME::GetGradeColor( pProp->grade ) );
// 	// 스탯
// 	XWnd *pRootStat = pRoot->Find("wnd.stat.area");
// 	if( pRootStat ) {
// 		for( int i = 0; i < 6; ++i ) {
// 			auto pText = xGET_TEXT_CTRLF( pRoot, "text.stat.%d", i+1 );
// 			if( pText ) {
// 				float stat = pProp->GetStat( i );
// 				pText->SetText( XFORMAT("%.2f", stat) );
// 			}
// 		}
// 	}
// 	// 지휘가능 유닛
// 	std::vector<XGAME::xtUnit> ary;
// 	if( pProp->typeAtk == XGAME::xAT_TANKER ) {
// 		ary.push_back( XGAME::xUNIT_SPEARMAN );
// 		ary.push_back( XGAME::xUNIT_MINOTAUR );
// 		ary.push_back( XGAME::xUNIT_GOLEM );
// 	} else
// 	if( pProp->typeAtk == XGAME::xAT_RANGE ) {
// 		ary.push_back( XGAME::xUNIT_ARCHER );
// 		ary.push_back( XGAME::xUNIT_CYCLOPS );
// 		ary.push_back( XGAME::xUNIT_TREANT );
// 	} else
// 	if( pProp->typeAtk == XGAME::xAT_SPEED ) {
// 		ary.push_back( XGAME::xUNIT_PALADIN );
// 		ary.push_back( XGAME::xUNIT_LYCAN );
// 		ary.push_back( XGAME::xUNIT_FALLEN_ANGEL );
// 	}
// 	int idx = 0;
// 	for( auto unit : ary ) {
// 		auto pPropUnit = PROP_UNIT->GetpProp( unit );
// 		if( XASSERT(pPropUnit) ) {
// 			xSET_IMGF( pRoot, XGAME::GetResUnitSmall( unit ).c_str(), "img.unit.%d", idx+1 );
// 			++idx;
// 		}
// 	}
// 	{
// 		XWnd *pWnd = pRoot->Find( "key.skill.passive" );
// 		if( pWnd ) {
// 			auto pPropSkill = SKILL_MNG->FindByIdentifier( pProp->strPassive );
// 			if( XASSERT( pPropSkill ) ) {
// 				xSET_TEXT( pWnd, "text.name", pPropSkill->GetSkillName() );
// 				auto pImg = xSET_IMG( pWnd, "img.icon", pPropSkill->GetResIcon() );
// 				if( pImg )
// 					pImg->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickSkillTooltip, pPropSkill->GetidSkill() );
// 			}
// 		}
// 	}
// 	{
// 		XWnd *pWnd = pRoot->Find( "key.skill.active" );
// 		if( pWnd ) {
// 			auto pPropSkill = SKILL_MNG->FindByIdentifier( pProp->strActive );
// 			if( XASSERT( pPropSkill ) ) {
// 				xSET_TEXT( pWnd, "text.name", pPropSkill->GetSkillName() );
// 				auto pImg = xSET_IMG( pWnd, "img.icon", pPropSkill->GetResIcon() );
// 				if( pImg )
// 					pImg->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickSkillTooltip, pPropSkill->GetidSkill() );
// 			}
// 		}
// 	}
// 	// 판매가격
// 	if( cost > 0 ) {
// 		xSET_SHOW( pRoot, "wnd.sell", true );
// 		pText = xSET_TEXT( pRoot, "text.cost", XE::NumberToMoneyString( cost ) );
// 		if( ACCOUNT->GetptGuild() < cost )
// 			pText->SetColorText( XCOLOR_RED );
// 	}
// 	else
// 		xSET_SHOW( pRoot, "wnd.sell", FALSE );
// }

/****************************************************************
* @brief 
*****************************************************************/
int XSceneGuildShop::OnClickGirl( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickGirl");
	//
	if( p1 == 0 ) {
		xSET_TEXT( this, "text.char.say", XTEXT(2207));
	} else
	if( p2 == 0 ) {
		xSET_TEXT( this, "text.char.say", XTEXT( 2208 ) );
	}
	return 1;
}

/****************************************************************
* @brief 캐쉬지불 OK
*****************************************************************/
int XSceneGuildShop::OnClickCashPayForGuildPoint( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickCashPayForGuildPoint");
	if( pWnd && pWnd->GetpParent() )
		pWnd->GetpParent()->SetbDestroy( true );

	//
//	GAMESVR_SOCKET->SendReqCashPay( GAME, 포인트타입, 부족한포인트 );
	return 1;
}



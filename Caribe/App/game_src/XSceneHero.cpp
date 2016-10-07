#include "StdAfx.h"
#include "XSceneHero.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XHero.h"
#include "XWndTemplate.h"
#include "XSoundMng.h"
#include "_Wnd2/XWndButton.h"
#include "XWndStorageItemElem.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneHero *SCENE_HERO = NULL;

void XSceneHero::Destroy() 
{	
	XBREAK( SCENE_HERO == NULL );
	XBREAK( SCENE_HERO != this );
	SCENE_HERO = NULL;
}

XSceneHero::XSceneHero( XGame *pGame ) 
	: XSceneBase( pGame, XGAME::xSC_HERO )
	, m_Layout(_T("layout_hero.xml"))
{ 
	XBREAK( SCENE_HERO != NULL );
	SCENE_HERO = this;
	Init(); 
	// 레이아웃 로드
	m_Layout.CreateLayout("common_bg", this);
//	m_Layout.CreateLayout("common_bg_goldcash", this);
	m_Layout.CreateLayout("hero", this);
	// 백버튼
	xSET_BUTT_HANDLER(this, "butt.back", &XSceneHero::OnBack);
	// 텍스트
// 	xSET_TEXT(this, "text.common.gold", XE::NtS(ACCOUNT->GetGold()));
// 	xSET_TEXT(this, "text.common.cash", XE::NtS(ACCOUNT->GetCashtem()));
	// 버튼 핸들러.
	xSET_BUTT_HANDLER( this, "butt.empoly.normal", &XSceneHero::OnSummonLowTemp );
//	xSET_BUTT_HANDLER_PARAM(this, "butt.empoly.high.gold", this, &XSceneHero::OnSummonHighTemp, (DWORD)XGAME::xGA_HIGH_GOLD);
// 	xSET_BUTT_HANDLER_PARAM(this, "butt.empoly.high.cashtem", this, &XSceneHero::OnSummonHighTemp, (DWORD)XGAME::xGA_HIGH_CASH);
	xSET_BUTT_HANDLER( this, "butt.empoly.high.cashtem", &XSceneHero::OnSummonHighTemp );

	// 버튼에 가격 넣기
// 	xSET_TEXT( this, "text.cost.normal", XE::NtS( ACCOUNT->GetGoldNormalSummon() ) );
// 	auto pButt = dynamic_cast<XWndButtonString*>(Find("butt.empoly.high.gold"));
// 	if (pButt)
// 		pButt->SetText(XE::Format(_T("%d"), COST_HIGH_GOLD_GATHA ));
	auto pButt = dynamic_cast<XWndButtonString*>(Find("butt.empoly.high.cashtem"));
	if (pButt)
		pButt->SetText( XE::NtS(XGC->GetcashHighSummon()) );

	// 텍스트
// 	xSET_TEXT(this, "text.desc.normal", XE::Format(XTEXT(80034), COST_LOW_GATHA));
// 	xSET_TEXT(this, "text.desc.high", XE::Format(XTEXT(80035), COST_HIGH_GOLD_GATHA, COST_HIGH_CASH_GATHA));
	///< 
}

void XSceneHero::Create( void )
{
	XEBaseScene::Create();
}

void XSceneHero::Update()
{
	XGAME::CreateUpdateTopResource( this );
// 	xSET_TEXT( this, "text.common.gold", XE::NtS( ACCOUNT->GetGold() ) );
// 	xSET_TEXT( this, "text.common.cash", XE::NtS( ACCOUNT->GetCashtem() ) );
// 	auto pButt = xGET_BUTT_CTRL( this, "butt.empoly.normal" );
// 	if( XASSERT(pButt) ) {
	// 일반소환버튼에 가격표시
		int cost = ACCOUNT->GetGoldNormalSummon();
// 		int cost = COST_LOW_GATHA;
		// 최초 소환은 무료
		if( ACCOUNT->GetFlagTutorial().bSummonHero == 0 )
			cost = 0;
//		if( cost > 0 ) {
			xSET_TEXT( this, "text.cost.normal", XE::NtS(cost) );
// 		} else {
// 			xSET_TEXT( this, "text.cost.normal", XTEXT(2121) );		// 무료
// 		}
// 	}
	//
	XSceneBase::Update();
}

int XSceneHero::Process( float dt ) 
{ 
	return XEBaseScene::Process( dt );
}

void XSceneHero::Draw( void ) 
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneHero::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneHero::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneHero::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}

/****************************************************************
* @brief
*****************************************************************/
int XSceneHero::OnBack(XWnd* pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnBack");
	DoExit(XGAME::xSC_WORLD);

	return 1;
}

/****************************************************************
* @brief 일반소환버튼 누름.
*****************************************************************/
int XSceneHero::OnSummonLowTemp( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnSummonLowTemp");
	// 영웅 소환
	int gold = ACCOUNT->GetGoldNormalSummon();
	if( ACCOUNT->GetFlagTutorial().bSummonHero ) {
		// 튜토리얼 소환이 아닐때만 검사함.
		if( ACCOUNT->IsNotEnoughGold( gold ) ) {
			XWND_ALERT_T( XTEXT( 80056 ), XE::NtS(gold) );	// 금화가 부족함.
			return 1;
		}
	} else {
		gold = 0;		// 최초소환은 무료
	}
	// 팝업
	m_Layout.CreateLayout("askpopup", this);
	XWnd *pPopup = Find("img.ask");
	pPopup->SetbModal(TRUE);
	xSET_BUTT_HANDLER_PARAM2(this, "butt.ok", this, &XSceneHero::OnSummonPopupOK, (DWORD)XGAME::xGA_LOW_GOLD, 0);
	xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneHero::OnClose);
	xSET_TEXT( this, "text.hero.ask"
		, XE::Format( XTEXT( 80054 ), XTEXT( 80055 ), XE::NtS( gold ) ) );	// 금화xxx로 소환?
	return 1;
}
/**
 @brief 캐시소환버튼 누름.
*/
int XSceneHero::OnSummonHighTemp( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnSummonHighTemp");
	// 영웅 소환
	/*
		.영웅 프로퍼티에서 랜덤으로 소환(법사류 제외)
		.영웅 클래스에따라 1티어 유닛들로 자동 배정
		.유닛수는 5부터
	*/
//	XGAME::xtGatha dwType = (XGAME::xtGatha)p1;
//	XASSERT( dwType == xGA_HIGH_CASH );
	int cash = XGC->GetcashHighSummon();
	if ( ACCOUNT->IsNotEnoughCash(cash) ) {
		XWND_ALERT_T(XTEXT(80057), XE::NtS(cash) );		// 캐시가 부족함.
		return 1;
	}
	// 팝업
	m_Layout.CreateLayout("askpopup", this);
	XWnd *pPopup = Find("img.ask");
	pPopup->SetbModal(TRUE);
	xSET_BUTT_HANDLER_NEW(this, "butt.ok", this, &XSceneHero::OnSummonPopupOK, XGAME::xGA_HIGH_CASH );
	xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneHero::OnClose);
// 	if (dwType == XGAME::xGA_HIGH_GOLD) {
// 		xSET_TEXT(this, "text.hero.ask", XE::Format(XTEXT(80054), XTEXT(80055), COST_HIGH_GOLD_GATHA));
// 	} else {
		xSET_TEXT(this, "text.hero.ask"
			, XE::Format(XTEXT(80054), XTEXT(80012), XE::NtS(cash)) );	// 캐시xxx로 소환?
//	}
// 	if ((dwType == XGAME::xGA_HIGH_GOLD && ACCOUNT->GetGold() < (DWORD)COST_HIGH_GOLD_GATHA) ||
// 		(dwType == XGAME::xGA_HIGH_CASH && ACCOUNT->GetCashtem() < (DWORD)COST_HIGH_CASH_GATHA)) {
// 		XWND_ALERT_T(XTEXT(80057), COST_HIGH_GOLD_GATHA, COST_HIGH_CASH_GATHA);
// 		return 1;
// 	}
// 	// 팝업
// 	m_Layout.CreateLayout("askpopup", this);
// 	XWnd *pPopup = Find("img.ask");
// 	pPopup->SetbModal(TRUE);
// 	xSET_BUTT_HANDLER_PARAM2(this, "butt.ok", this, &XSceneHero::OnSummonPopupOK, (DWORD)dwType, 0);
// 	xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneHero::OnClose);
// 	if (dwType == XGAME::xGA_HIGH_GOLD) {
// 		xSET_TEXT(this, "text.hero.ask", XE::Format(XTEXT(80054), XTEXT(80055), COST_HIGH_GOLD_GATHA));
// 	} else {
// 		xSET_TEXT(this, "text.hero.ask", XE::Format(XTEXT(80054), XTEXT(80012), COST_HIGH_CASH_GATHA));
// 	}

	return 1;
}
/**
 @brief 고용하시겠습니까?의 Ok버튼.
*/
int XSceneHero::OnSummonPopupOK(XWnd* pWnd, DWORD p1, DWORD p2)
{
	OnClose(pWnd, 0, 0);
	xSET_SHOW( this, "butt.empoly.normal", FALSE );
	xSET_SHOW( this, "butt.empoly.high.gold", FALSE );
	xSET_SHOW( this, "butt.empoly.high.cashtem", FALSE );

	auto type = (XGAME::xtGatha) p1;
	XBREAK( type <= XGAME::xGA_NONE || type >= XGAME::xGA_MAX );
	// 영웅 소환
	GAMESVR_SOCKET->SendReqSummonHero( this, type );

	return 1;
}

void XSceneHero::OnRecvSummon( bool bPiece, ID idPropHero, ID snHero, 
								XGAME::xtGatha typeSummon, int numPiece )
{
// 	xSET_TEXT(this, "text.common.gold", XE::Format(_T("%d"), ACCOUNT->GetGold()));
// 	xSET_TEXT(this, "text.common.cash", XE::Format(_T("%d"), ACCOUNT->GetCashtem()));
	XE::VEC2 vPos = {159, 204};
	// 도장찍기 애니메이션 실행
	XWnd *pWnd = nullptr;
	if( typeSummon == XGAME::xGA_LOW_GOLD )
		pWnd = Find( "img.bg.normal" );
	else
	if( typeSummon == XGAME::xGA_HIGH_CASH )
		pWnd = Find( "img.bg.high" );
	if( pWnd ) {
		// 도장찍기애니메이션이 끝나면 콜백이 호출된다.
		auto pSpr = new XWndCallbackSpr( this, 
																		_T( "stamp.spr" ), 1, 
																		pWnd->GetPosFinal() + vPos, 
																		&XSceneHero::OnSummonFinish );
		Add( pSpr );
	}
	_m_bPiece = bPiece;
	_m_idHero = idPropHero;
	_m_snHero = snHero;
	_m_typeGatha = typeSummon;
	_m_numPiece = numPiece;
}
/**
 @brief 도장찍기 애니메이션이 끝나면 호출
 영웅 소환 결과창
*/
int XSceneHero::OnSummonFinish(XWnd* pWnd, DWORD p1, DWORD p2)
{
	pWnd->SetbDestroy( TRUE );
	m_Layout.CreateLayout( "complate", this );
	auto gradeCurr = XGAME::xGD_NONE;
	// 팝업배경
	XWnd *pPopup = Find( "img.complate" );
	XBREAK( pPopup == nullptr );
	pPopup->SetbModal( TRUE );
	//
	//XPropHero::xPROP *pPropHero = nullptr;
	if( _m_bPiece ) {
		auto pPropHero = PROP_HERO->GetpProp( _m_idHero );
		XBREAK( pPropHero == nullptr );
		XWnd *pBg = Find( "img.unit.bg" );
		if( pBg ) {
			ID idItem = ACCOUNT->GetidItemPieceByidHero( _m_idHero );
			auto pWndItem = new XWndStoragyItemElem( idItem );
			XBREAK( _m_numPiece == 0 );
			pWndItem->SetNum( _m_numPiece );
			pBg->Add( pWndItem );
			pWndItem->AutoLayoutCenter();
		}
		// xxx의 영혼석.
		_tstring strName = XE::Format(XTEXT(2158), pPropHero->GetstrName().c_str() );
		xSET_TEXT( this, "text.hero.name", strName );
	} else {
		XSPHero pHero = ACCOUNT->GetHero( _m_snHero );
		XBREAK( pHero == NULL );
		auto pPropHero = pHero->GetpProp();
		gradeCurr = pHero->GetGrade();
		// 별
		for( int j = 1; j < XGAME::xGD_MAX; ++j ) {
			if( j <= gradeCurr ) {
				xSET_SHOWF( this, true, "img.star%d", j );
				xSET_SHOWF( this, false, "img.star.empty%d", j );
			} else {
				xSET_SHOWF( this, false, "img.star%d", j );
				xSET_SHOWF( this, true, "img.star.empty%d", j );
			}
		}
		// 영웅의 공격모션 출력
		auto pSpr = new XWndCallbackSpr(this, 
										pHero->GetpProp()->strSpr.c_str(), 
										ACT_ATTACK1, 
										XE::VEC2(249, 168), 
										&XSceneHero::OnSummonHeroAnimation);
		pPopup->Add(pSpr);
		pSpr->SetPosLocal((pPopup->GetSizeLocal().x - pSpr->GetSizeLocal().x) / 2  + pSpr->GetSizeLocal().x / 2 , pSpr->GetPosLocal().y);
		xSET_TEXT( this, "text.hero.name", pPropHero->GetstrName() );
	}
	if( _m_typeGatha == XGAME::xGA_LOW_GOLD ) {
		if( gradeCurr >= XGAME::xGD_RARE )
			SOUNDMNG->OpenPlaySound( 19 );
		else
			SOUNDMNG->OpenPlaySound( 20 );
	} else {
		if( gradeCurr >= XGAME::xGD_EPIC )
			SOUNDMNG->OpenPlaySound( 19 );
		else
			SOUNDMNG->OpenPlaySound( 20 );
	}
	xSET_BUTT_HANDLER(this, "butt.ok", &XSceneHero::OnClose);
	xSET_SHOW(this, "butt.empoly.normal", TRUE);
	xSET_SHOW(GAME, "butt.empoly.high.gold", TRUE);
	xSET_SHOW(GAME, "butt.empoly.high.cashtem", TRUE);
	return 1;
}

int XSceneHero::OnClose(XWnd* pWnd, DWORD p1, DWORD p2)
{
	if (pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(TRUE);
	return 1;
}

int XSceneHero::OnSummonHeroAnimation(XWnd* pWnd, DWORD p1, DWORD p2)
{
	auto pSpr = SafeCast<XWndCallbackSpr*>(pWnd);
	pSpr->SetAction(ACT_IDLE1);
	return 1;
}


#include "stdafx.h"
#include "XWndTemplate.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndCtrls.h"
#include "XGameWnd.h"
#include "XGame.h"
#include "XWndTech.h"
#include "XWndResCtrl.h"
#include "skill/XSkillDat.h"
#include "XHero.h"
#include "XPropItem.h"
#include "XSoundMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( XGAME )
//
/**
 @brief 이거 쓰지말고 XWndCircleUnit쓸것.
*/
XWndImage* _CreateUnitFace( XWnd *pRoot, const std::string& ids, XGAME::xtUnit unit )
{
	auto pWndImage = XWndImage::sUpdateCtrl( pRoot, ids );
	if( pWndImage ) {
		pWndImage->SetSurfaceRes( PATH_UI( "corps_legionnaire_bg.png" ) );
		auto resImg = XGAME::GetResUnitSmall( unit );
		XWndImage::sUpdateCtrl( pWndImage, XE::VEC2( 0 ), resImg, true, ids + ".face" );
	}
	return pWndImage;
}

/**
 @brief 이거 쓰지말고 XWndCircleUnit쓸것.
*/
XWndImage* _CreateUnitFace( XWnd *pRoot, XGAME::xtUnit unit )
{
	auto pImgBg = new XWndImage();
	if( pImgBg ) {
		pImgBg->SetSurfaceRes( PATH_UI( "corps_legionnaire_bg.png" ) );
		if( unit ) {
			auto resImg = XGAME::GetResUnitSmall( unit );
			auto pImgFace = new XWndImage();
			if( pImgFace ) {
				pImgFace->SetSurfaceRes( resImg, XE::xPF_ARGB1555 );
			}
		}
	}
	return pImgBg;
}

/**
 @brief 원형바탕에 레벨숫자를 표시하는 템플릿
*/
XWndImage* UpdateLevelByHero( XWnd *pRoot, const std::string& ids, int lv )
{
	auto pImgLv = XWndImage::sUpdateCtrl( pRoot
																			, XE::VEC2(0)
																			, PATH_UI("bg_level.png")
																			, XE::xPF_ARGB1555
																			, ids.c_str() );
	if( pImgLv ) {
		auto pText = XWndTextString::sUpdateCtrl( pImgLv, "__text.lv", FONT_RESNUM, 14, true );
		if( pText ) {
			pText->SetStyleStroke();
			pText->SetText( XFORMAT("%d", lv) );
		}
	}
	return pImgLv;
}

// XWndImage* UpdateUnitFace( XWnd *pRoot, const std::string& ids, XGAME::xtUnit unit )
// {
// 	auto pWndImage = XWndImage::sUpdateCtrl( pRoot, vPos, PATH_UI( "corps_legionnaire_bg.png" ), true, ids );
// 	if( pWndImage ) {
// 		auto resImg = XGAME::GetResUnitSmall( unit );
// 		XWndImage::sUpdateCtrl( pWndImage, XE::VEC2( 0 ), resImg, true, ids + ".face" );
// 	}
// 	return pWndImage;
// }

XWnd* GetpRootTopRes( XWnd *pRootFind )
{
	return pRootFind->Find( "bar.top.res" );
}

/**
 @brief 상단 자원바에서 특정 자원 컨트롤의 시작위치를 얻는다.
*/
XWnd* GetpWndTopRes( XGAME::xtResource resType, XWnd *pRootFind )
{
	auto pRootBar = GetpRootTopRes( pRootFind );
	if( XASSERT(pRootBar) ) {
		auto pResCtrl = pRootBar->Findf( "top.res.%s", XGAME::GetIdsRes( resType ) );
		if( XASSERT(pResCtrl) ) {
			return pResCtrl;
		}
	}
	return nullptr;
}

/**
 @brief 상단의 가로로 리소스와 금화를 보여주는 UI
*/
void CreateUpdateTopResource( XWnd *pRoot )
{
	XWnd *pBar = pRoot->Find("bar.top.res");
	if( pBar == nullptr ) {
		pBar = new XWndRect( 0.f, 0.f, XE::GetGameWidth(), 16.f, XCOLOR_RGBA( 0, 0, 0, 192 ) );
		pBar->SetstrIdentifier( "bar.top.res" );
		pRoot->Add( pBar );
	}
	XE::VEC2 v( 35.f, 0.f );
	{
		auto pCtrl = SafeCast2<XWndResourceCtrl*>( pBar->Find( "top.res.cash" ) );
		if( pCtrl == nullptr ) {
			pCtrl = new XWndResourceCtrl( v, true );		// NumberCounter방식으로 생성.
			pCtrl->SetbSymbolNumber( false );
			pCtrl->SetstrIdentifier( "top.res.cash" );
			pCtrl->AddRes( XGAME::xRES_CASH, 0 );
			pBar->Add( pCtrl );
			pCtrl->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickTopGemUI );
			v.x += 70.f;
		}
		pCtrl->EditRes( XGAME::xRES_CASH, (int)ACCOUNT->GetCashtem() );
	}
	//
	{
		auto pCtrl = SafeCast2<XWndResourceCtrl*>( pBar->Find( "top.res.gold" ) );
		if( pCtrl == nullptr ) {
			pCtrl = new XWndResourceCtrl( v, true );
			pCtrl->SetbSymbolNumber( true );		// 큰숫자는 기호로
			pCtrl->SetstrIdentifier( "top.res.gold" );
			pCtrl->AddRes( XGAME::xRES_GOLD, 0 );
			pBar->Add( pCtrl );
			v.x += 87.f;
		}
		const auto gold = (int)ACCOUNT->GetGold();
		pCtrl->EditRes( XGAME::xRES_GOLD, gold );
	}
	//
	for( int i = 0; i < XGAME::xRES_MAX; ++i ) {
		const auto resType = (XGAME::xtResource)i;
		const auto idsRes = XGAME::GetIdsRes(resType);
		auto pCtrl = SafeCast2<XWndResourceCtrl*>( pBar->Findf("top.res.%s", idsRes) );
		if( pCtrl == nullptr ) {
			pCtrl = new XWndResourceCtrl( v, true );
			pCtrl->SetbSymbolNumber( true );		// 큰숫자는 기호로
			pCtrl->SetstrIdentifierf("top.res.%s", idsRes );
			pCtrl->AddRes( resType, 0 );
			pBar->Add( pCtrl );
			v.x += 87.f;
		}
		pCtrl->EditRes( resType, ACCOUNT->GetResource( resType ) );
	}
}
/**
 @brief typeRes만 수동으로 갱신시킨다. TextCounter타입일때 주로 사용한다.
 @param typeRes xRES_NONE 이면
*/
void UpdateTopResourceByManual( XWnd *pRootScene, XGAME::xtResource typeRes )
{
	XWnd *pBar = pRootScene->Find( "bar.top.res" );
	if( pBar == nullptr )
		return;		// 아직 만들어지 않았으면 취소시킴.
	// 미구현
}
/**
 @brief
*/
void CreateUpdateTopGuildCoin( XWnd *pRoot )
{
	XWnd *pBar = pRoot->Find("bar.top.res");
	if( pBar == nullptr ) {
		pBar = new XWndRect( 0.f, 0.f, XE::GetGameWidth(), 16.f, XCOLOR_RGBA( 0, 0, 0, 192 ) );
		pBar->SetstrIdentifier( "bar.top.res" );
		pRoot->Add( pBar );
	}
	XE::VEC2 v( 35.f, 0.f );
	{
		auto pCtrl = SafeCast2<XWndResourceCtrl*>( pBar->Find( "top.res.cash" ) );
		if( pCtrl == nullptr ) {
			pCtrl = new XWndResourceCtrl( v, true );
			pCtrl->SetbSymbolNumber( true );		// 큰숫자는 기호로
			pCtrl->SetstrIdentifier( "top.res.cash" );
			pCtrl->AddRes( XGAME::xRES_CASH, 0 );
			pBar->Add( pCtrl );
			// 		v.x += 70.f;
		}
		pCtrl->EditRes( XGAME::xRES_CASH, (int)ACCOUNT->GetCashtem() );
	}
	{
		auto pCtrl = SafeCast2<XWndResourceCtrl*>( pBar->Find( "top.res.gold" ) );
		if( pCtrl == nullptr ) {
			pCtrl = new XWndResourceCtrl( v, true );
			pCtrl->SetbSymbolNumber( true );		// 큰숫자는 기호로
			pCtrl->SetstrIdentifier( "top.res.gold" );
			pCtrl->AddRes( XGAME::xRES_GOLD, 0 );
			pBar->Add( pCtrl );
			// 		v.x += 87.f;
		}
		pCtrl->EditRes( XGAME::xRES_GOLD, (int)ACCOUNT->GetGold() );
	}
	//
	{
		auto pCtrl = SafeCast2<XWndResourceCtrl*>( pBar->Find( "top.res.guildcoin" ) );
		if( pCtrl == nullptr ) {
			pCtrl = new XWndResourceCtrl( v );
			pCtrl->SetstrIdentifier( "top.res.guildcoin" );
			pCtrl->AddRes( XGAME::xRES_GUILD_COIN, 0 );
			pBar->Add( pCtrl );
			// 		v.x += 87.f;
		}
		pCtrl->EditRes( XGAME::xRES_GUILD_COIN, (int)ACCOUNT->GetGuildCoin() );
	}
	pBar->AutoLayoutHCenterByChilds( 200.f );
}

//
XE_NAMESPACE_END; // XGAME

////////////////////////////////////////////////////////////////
XWndTooltip::XWndTooltip( const _tstring& strXml
												, const std::string& strKey
												, LPCTSTR fileFrame )
{
	Init();
	m_pWndArrow = new XWndArrow4Abil( _T("arrow_tooltip.png") );
	m_pWndArrow->SetbShow( false );		// 뷰 다나타나기전까진 숨겨둠.
	Add( m_pWndArrow );
// 	auto pView = new XWndView( 0, 0, 1, 1, fileFrame );
	XLayoutObj layoutObj( strXml );
	layoutObj.CreateLayout( strKey, this );
	//
	auto pView = SafeCast<XWndView*>( Find("view.bg") );
	if( pView ) {
		pView->SetEvent( XWM_FINISH_APPEAR, this, &XWndTooltip::OnFinishAppear );
	}
	// 크기 계산은 여기서 하지 않고 실제 텍스트가 입력되면 계산한다.
	SetEnableNcEvent( TRUE );	// 화면밖을 찍어도 꺼지게 할것인가.
}

// BOOL XWndTooltip::OnCreate()
// {
// 	return XWnd::OnCreate();
// }

/**
 @brief
*/
int XWndTooltip::OnFinishAppear( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnFinishAppear");
	//
	if( m_pWndArrow )
		m_pWndArrow->SetbShow( true );
	return 1;
}

/**
 @brief 툴팁 윈도우의 텍스트를 교체한다.
 @note 텍스트교체 후 다이나믹 뷰의 크기와 this의 크기도 자동 재계산 된다.
*/
void XWndTooltip::SetText( LPCTSTR szText, const char* cKey )
{
	auto pWndText = xGET_TEXT_CTRL( this, cKey );
	if( pWndText ) {
		if( m_Length > 0 )
			pWndText->SetLineLength( m_Length );
		pWndText->SetText( szText );
	}
}

/**
 @brief 텍스트등의 레이아웃에 맞춰 자동으로 뷰 크기를 조정한다.
*/
void XWndTooltip::SetAutoSize()
{
	auto pWndView = SafeCast<XWndView*>( Find("view.bg") );
	if( pWndView ) {
		// view 레이아웃의 크기
		const auto sizeLayout = pWndView->GetSizeNoTransLayout();
		const auto sizeLayoutAligned = sizeLayout.RoundUpDiv(16);
		{
			// 뷰의 크기가 명시되어 있지않으면 전체 레이아웃 크기로 자동 조절
			const auto sizeView = pWndView->GetSizeLocal();		// layout에 명시된 크기
			if( sizeView.w <= 0 )
				pWndView->SetWidth( sizeLayoutAligned.w );
			if( sizeView.h <= 0 )
				pWndView->SetHeight( sizeLayoutAligned.h );
		}
		// 텍스트레이어는 기본으로 있다고 가정함.
		auto pWndText = xGET_TEXT_CTRL( this, "__text.tooltip" );
		if( pWndText ) {
			// 텍스트의 크기
			const auto sizeText = pWndText->GetSizeNoTransLayout();
			// 뷰 레이아웃크기가 텍스트 레이아웃보다 작으면 텍스트에 맞춰 자동 조절
			auto sizeAligned = sizeText.RoundUpDiv(16);
			if( (sizeLayout.w < sizeAligned.w) || (sizeLayout.h < sizeAligned.h) ) {
				if( sizeLayout.w < sizeAligned.w ) {
					pWndView->SetWidth( sizeAligned.w );
				}
				if( sizeLayout.h < sizeAligned.h ) {
					pWndView->SetHeight( sizeAligned.h );
				}
				pWndText->SetSizeLocal( sizeAligned );
			} else {
				// 이미 뷰 레이아웃이 텍스트 레이아웃보다 크다면 그 크기를 그대로 사용한다.
				pWndView->SetSizeLocal( sizeLayoutAligned );
			}
			// 조정된 뷰의 크기로 this의 크기도 맞춤.
			const auto sizeView = pWndView->GetSizeLocal();
			SetSizeLocal( sizeView );
			pWndView->SetbUpdate( true );
		}
	}
}

/**
 @brief 텍스트 문단폭을 지정한다.
*/
void XWndTooltip::SetLineLength( float w )
{
	m_Length = w;
}

/**
 @brief pWndTrace윈도우를 기준으로 툴팁의 위치가 자동 재 조정된다.
*/
void XWndTooltip::AutoPosByWinSrc( XWnd* pWndTrace )
{
	auto pWndView = SafeCast<XWndView*>( Find( "view.bg" ) );
	if( pWndView ) {
		const XE::VEC2 sizeFrame( 8, 8 );		// 프레임의 두께
		const auto vPosSrc = pWndTrace->GetPosFinal();
		const auto sizePopup = pWndView->GetSizeFinal();
		// 가리켜야할 윈도우의 왼쪽 상단에 위치(오른손 잡이용)
		const auto vLTOrig = vPosSrc + XE::VEC2( -(sizePopup.w + 32.f), -(sizePopup.h + 32.f) );			// 원래 찍혔어야 했을 좌표
// 		vLTOrig.y -= sizePopup.h + 16.f;
		auto vLTAdj = vLTOrig;		// 보정된 좌표
		// 왼쪽이 잘리면
		if( vLTAdj.x < 0 + sizeFrame.w )
			vLTAdj.x = sizeFrame.w;
		// 상단이 잘리면
		if( vLTAdj.y < 0 + sizeFrame.h )
			vLTAdj.y = sizeFrame.h;
		// 왼쪽/위로 보정된 좌표가 오리지날 좌표의 x,y축 모두를 넘어서면 타겟의 아래쪽으로 자리를 바꿈.
		if( vLTAdj.x + sizePopup.w > vPosSrc.x
			&& vLTAdj.y + sizePopup.h > vPosSrc.h )
			//
			vLTAdj.y = vPosSrc.y + 32.f;
		SetPosLocal( vLTAdj );
	}
}

void XWndTooltip::SetDirection( const XE::VEC2& vEnd )
{
	if( m_pWndArrow ) {
		const auto vCenter = GetPosFinal() + GetSizeFinal() * 0.5f;
		const auto vCenterLocal = GetSizeLocalNoTrans() * 0.5f;
		m_pWndArrow->SetPosLocal( vCenterLocal );
		m_pWndArrow->SetDirection( vCenter, vEnd, nullptr );
	}
}

void XWndTooltip::OnNCLButtonDown( float lx, float ly )
{
	m_bTouchNC = true;
}
// void XGameWndAlert::OnNCMouseMove( float lx, float ly )
// {
// 	if( m_bTouchNC )
// 		SetbDestroy( true );
// 	m_bTouchNC = false;
// }
void XWndTooltip::OnNCLButtonUp( float lx, float ly )
{
	if( m_bTouchNC )
		SetbDestroy( TRUE );
	m_bTouchNC = false;
}

////////////////////////////////////////////////////////////////
/**
 @brief 유닛과 유닛의 원형프레임을 그려주는 모듈.
*/
XWndCircleUnit::XWndCircleUnit()
	: XWndImage( PATH_UI("corps_legionnaire_bg.png"), XE::VEC2() ) 
{
	Init();
}
XWndCircleUnit::XWndCircleUnit( XGAME::xtUnit unit, const XE::VEC2& vPos, XHero *pHero )
	: XWndImage( PATH_UI("corps_legionnaire_bg.png"), vPos )
{
	Init();
	m_Unit = unit;
	m_pHero = pHero;
}

void XWndCircleUnit::Update()
{
	const auto blendFunc = GetblendFunc();
	// 유닛 초상화.
	if( m_Unit ) {
		auto pFace = xGET_IMAGE_CTRL( this, "img.unit" );
		if( pFace == nullptr ) {
			auto strResUnit = XGAME::GetResUnitSmall( m_Unit );
			pFace = new XWndImage( strResUnit, 0, 0 );
			pFace->SetstrIdentifier( "img.unit" );
			Add( pFace );
		}
		if( pFace ) {
			auto strResUnit = XGAME::GetResUnitSmall( m_Unit );
			pFace->SetSurfaceRes( strResUnit );
			pFace->SetblendFunc( blendFunc );
		}
	}
	if( m_pHero ) {
		// 유닛부대 레벨
		auto pTextLv = xGET_TEXT_CTRL( this, "text.lv.squad");
		if( pTextLv == nullptr ) {
			pTextLv = new XWndTextString( XE::VEC2(0)
																	, _T("")
																	, FONT_NANUM, 15.f );
			pTextLv->SetLineLength( GetSizeFinal().w );
			pTextLv->SetAlignHCenter();
			pTextLv->SetStyleStroke();
			pTextLv->SetstrIdentifier("text.lv.squad");
			Add( pTextLv );
		}
		if( pTextLv ) {
			pTextLv->SetbShow( m_bShowLevelSquad );
			pTextLv->SetText( XFORMAT("%d",m_pHero->GetlevelSquad()) );
		}
	}
	XWndImage::Update();
}

////////////////////////////////////////////////////////////////
XWndCircleSkill::XWndCircleSkill( XSKILL::XSkillDat *pDat
																, const XE::VEC2& vPos
																, XHero *pHero )
		: XWndImage( PATH_UI("common_bg_skillsimbol.png"), vPos )
{
	Init();
	m_pSkillDat = pDat;
	m_pHero = pHero;
}

void XWndCircleSkill::Update()
{
	auto blendFunc = GetblendFunc();
	// 스킬아이콘
	auto pFace = xGET_IMAGE_CTRL( this, "img.skill" );
	if( pFace == nullptr ) {
		pFace = new XWndImage( XE::VEC2(0) );
		pFace->SetstrIdentifier( "img.skill" );
		Add( pFace );
	}
	if( pFace ) {
		auto strResUnit = XE::MakePath( DIR_IMG, m_pSkillDat->GetstrIcon() );
		pFace->SetSurfaceRes( strResUnit );
		pFace->SetblendFunc( blendFunc );
	}
	XWndImage::Update();
}

/**
 @brief 아이템 아이콘을 함께 보여주는 알림창
*/
XGameWndAlert* XGAME::DoAlertWithItem( const _tstring& strIdsItem, const _tstring& strText )
{
	auto pPropItem = PROP_ITEM->GetpProp( strIdsItem );
	if( XASSERT( pPropItem ) ) {
		TCHAR szBuff[1024];
// 		_tstring str = XE::Format( szFormat, strText );
		XE::ConvertJosaStr( szBuff, strText );	
		auto pAlert = XWND_ALERT( "%s", szBuff );
		if( pAlert ) {
			auto pWndText = pAlert->GetpWndTextDesc();
			if( pWndText ) {
				pWndText->SetY( pWndText->GetPosLocal().y - 36.f );
			}
			auto pWndItem = new XWndStoragyItemElem( XE::VEC2( 0, 50 )
																						, strIdsItem );
			pWndItem->SetEventItemTooltip();
			pAlert->Add( pWndItem );
			pWndItem->AutoLayoutHCenter();
		}
		return pAlert;
	}
	return nullptr;
}

XWndPopup* XGAME::DoPopupBattleResult( XGAME::xBattleResult& result, XWnd* pParent, XSpot* pBaseSpot )
{
#ifndef _XSINGLE
	XBREAK( pBaseSpot == nullptr );
	auto pPopup = new XWndPopup( _T("popup_battle_result.xml"), "popup_result" );
	pPopup->SetEnableNcEvent( FALSE );		// 창밖터치로 꺼지지 못하게.
	pParent->Add( pPopup );
//	auto pBaseSpot = sGetpWorld()->GetSpot( result.idSpot );
	xSET_TEXT( pPopup, "text.use.ap", XFORMAT( "%+d", -pBaseSpot->GetNeedAP( ACCOUNT ) ) );
	xSET_TEXT( pPopup, "text.add.score", XFORMAT( "%+d", result.logForAttacker.addScore ) );
#ifdef _DEBUG
	{
		pPopup->AddButtonCancel( nullptr, _T( "common_butt_x.png" ) );
	}
#endif
	if( result.IsWin() ) {
		SOUNDMNG->OpenPlaySound( 5 );	// 승리소리
// 		int idxLegion = ACCOUNT->GetCurrLegionIdx();
// 		pPopup->SetEvent( XWM_OK, pParent, &XSceneBattle::OnOkBattleResult, idxLegion );
		pPopup->SetOkButton( "butt.ok" );
		xSET_SHOW( pPopup, "img.victory", TRUE );
		int idx = 0;
		{
			XWnd *pWndItems = pPopup->Find( "wnd.loot.item" );
			if( pWndItems ) {
				auto vStart = pWndItems->GetPosLocal();
				auto v = vStart;
				XARRAYLINEARN_LOOP_AUTO( result.aryDrops, itemBox ) {
					auto pProp = std::get<0>( itemBox );
					int numItem = std::get<1>( itemBox );
					// 아이템아이콘 컨트롤을 생성시켜서 슬롯에 넣는다.
					auto pWndItem = new XWndStoragyItemElem( v, pProp->idProp );
					pWndItem->SetNum( numItem );
					pWndItem->SetEventItemTooltip();
					pPopup->Add( pWndItem );
					const auto vSize = pWndItem->GetSizeLocal();
					v.x += vSize.w + 6.f;
					if( ++idx >= 5 ) {
						v.x = vStart.x;
						v.y = vStart.y + vSize.h + 6.f;
						idx = 0;
					}
				}END_LOOP;
				if( result.aryDrops.size() == 0 ) {
					auto pText = xGET_TEXT_CTRL( pPopup, "text.tip" );
					if( pText ) {
						pText->SetText( XTEXT( 2224 ) );	// 획득아이템 없음.
						pText->SetbShow( true );
					}
				}
			}
		}
		// 전투 별점 표시
		for( int i = 0; i < result.numStar; ++i ) {
			auto pStarEmpty = pPopup->Findf( "spr.star.%d", i + 1 );
			if( pStarEmpty ) {
				auto pSpr = SafeCast<XWndSprObj*>( pStarEmpty );
				if( pSpr ) {
					//					pSpr->SetActionDelayed( 2, 0.5f );	// 0.5초후에 액션2로 바꿔라.
					pSpr->SetAction( 2 );
				}
			}
		}
		xSET_SHOW( pPopup, "butt.statistic", false );
		// 정복도 패널티
		xSET_TEXT( pPopup, "text.star.penalty",
			XFORMAT( "%s(%+d%%)", XTEXT( 2237 ), -( 100 - result.m_mulByStar ) ) );
	} else {
		// 패배시
		SOUNDMNG->OpenPlaySound( 4 );
		xSET_SHOW( pPopup, "img.defeat", true );
		xSET_SHOW( pPopup, "butt.statistic", true );
//		xSET_BUTT_HANDLER_PARAM( pPopup, "butt.ok", this, &XSceneBattle::OnOkBattleResult, (DWORD)-1 );
//		xSetButtHander( pPopup, this, "butt.statistic", &XSceneBattle::OnClickStatistic );
		ID idText = xRandom( 55000, 55024 );
		auto pText = xGET_TEXT_CTRL( pPopup, "text.tip" );
		if( pText ) {
			pText->SetText( XTEXT( idText ) );
			pText->SetbShow( true );
		}
	}
	// 획득 자원을 표시해야 하면.
	if( result.logForAttacker.aryLoot.size() > 0 ) {
		XWnd *pRoot = pPopup->Find( "wnd.loot.res" );
		if( pRoot ) {
			auto pWndResources = new XWndResourceCtrl( XE::VEC2( 0 ),
				result.logForAttacker.aryLoot, true, 1.f );
			pRoot->Add( pWndResources );
			pWndResources->AutoLayoutCenter();
		}
	}
	return pPopup;
#else
	return nullptr;
#endif // not _XSINGLE
}


#include "stdafx.h"
#include "XGameWnd.h"
//#include "XSoundMng.h"
#include "client/XAppMain.h"
#include "../XDrawGraph.h"
#include "XGame.h"
#include "XFontMng.h"
#include "XFramework/XConstant.h"
//#include "XWindow.h"
#include "XImageMng.h"
#include "XFramework/client/XLayout.h"
#include "XAccount.h"
#include "XSceneWorld.h"
#include "XUnitHero.h"
#include "XBaseItem.h"
#include "XSockGameSvr.h"
#include "XSceneUnitOrg.h"
#include "XQuestMng.h"
#include "XSceneTech.h"
#include "XLegionObj.h"
#include "XStatistic.h"
#include "XPropUpgrade.h"
#include "XQuestCon.h"
#include "XSkillMng.h"
#include "XCampObjHero.h"
#include "XStageObjHero.h"
#include "XOrder.h"
#include "XSquadron.h"
#include "skill/XSkillDat.h"
#include "XSpots.h"
#include "XSceneBattle.h"
#include "XLegion.h"
#include "XSceneTitle.h"
// #include "XPropBgObj.h"
#include "XWndTemplate.h"
#include "XWorld.h"
//#include "XFramework/XSoundTable.h"
#include "XWndResCtrl.h"
#include "../XDrawGraph.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndProgressBar.h"
#ifdef _xIN_TOOL
#include "CaribeView.h"
#endif // _xIN_TOOL


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xCampaign;
using namespace xHelp;
using namespace XGAME;
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif


////////////////////////////////////////////////////////////////
XWndLayerTest::XWndLayerTest( LPCTSTR szXml, const char* cNode )
	: XWndPopup( szXml, cNode )
{
	Init();
	SetEnableNcEvent( TRUE );
}

void XWndLayerTest::Update()
{
	XWndPopup::Update();
}


//////////////////////////////////////////////////////////////////////////
XGameWndAlert::XGameWndAlert()
	: XWndAlert( nullptr, 
				288.f, 160.f, 
				_T("popup01.png"), 
				BUTT_MID, nullptr, nullptr,
				_T(""), 
				_T(""), 
				XWnd::xCLOSE, 
				XCOLOR_WHITE )
{
	Init();
	SetEnableNcEvent( TRUE );	// 화면밖을 찍어도 꺼지게 할것인가.
}

XGameWndAlert::XGameWndAlert( LPCTSTR szLayout, const char *cKey, bool bDummy )
	: XWndAlert( szLayout, cKey )
{
	Init();
	SetEnableNcEvent( TRUE );	// 화면밖을 찍어도 꺼지게 할것인가.
}
XGameWndAlert::XGameWndAlert( LPCTSTR _szText, LPCTSTR _szTitle, XWnd::xtAlert type, XCOLOR col )
	: XWndAlert( nullptr, 
				288.f, 160.f, 
				_T("popup01.png"), 
				BUTT_MID, nullptr, nullptr,
				_szText, 
				_szTitle, 
				type, 
				col )
//	: XWndAlert( 16.f, 174.f, 288.f, 160.f, _T("popup01.png"), BUTT_MID, _szText, _szTitle, type, col )
{
	Init();
	SetEnableNcEvent( TRUE );	// 화면밖을 찍어도 꺼지게 할것인가.
}

/**
 아무텍스트도 버튼도 없는 버전(다이얼로그 바탕으로 쓸때 사용)
*/ 
XGameWndAlert::XGameWndAlert( float w, float h )
	: XWndAlert( w, h )
// 	: XWndAlert( nullptr,
// 				w, h,
// 				_T("popup01.png"),
// 				BUTT_MID, nullptr, nullptr,
// 				_T(""),
// 				_T(""),
// 				XWnd::xALERT_NONE,
// 				XCOLOR_WHITE )
{
	Init();
	SetstrImgUp( _tstring(BUTT_MID) );
	SetType( XWnd::xALERT_NONE );
	SetstrTitle(_tstring() );
	SetstrText( _tstring() );
	CreateWndAlert();
	SetEnableNcEvent( TRUE );
}

void XGameWndAlert::OnNCLButtonDown( float lx, float ly )
{
	m_bTouchNC = TRUE;
}
void XGameWndAlert::OnNCMouseMove( float lx, float ly )
{
	if( m_bTouchNC )
		SetbDestroy( TRUE );
	m_bTouchNC = FALSE;
}
void XGameWndAlert::OnNCLButtonUp( float lx, float ly )
{
	if( m_bTouchNC )
		SetbDestroy(TRUE);
	m_bTouchNC = FALSE;
}

////////////////////////////////////////////////////////////////
XWndSpotRecon::XWndSpotRecon( XLegion *pLegion, XSpot *pBaseSpot )
	: XWndPopup( _T("layout_recon.xml"), "popup_recon" )
{
	Init();
	//
//	m_pAcc = pAcc;
	LPCTSTR szName = pBaseSpot->GetszName();
	m_pBaseSpot = pBaseSpot;
//	m_pLegion = pLegion;
	SetstrIdentifier("wnd.recon.result");
	if( szName )
		xSET_TEXT( this, "text.name.enemy", szName );
	if( pBaseSpot->IsNoAttack() )
		xSET_ENABLE( this, "butt.attack", FALSE );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		auto pButt = new XWndButtonDebug( 32.f, 32.f, 32.f, 32.f, _T("show") );
		pButt->SetEvent( XWM_CLICKED, this, &XWndSpotRecon::OnClickShow );
		Add( pButt );
	}
#endif // _CHEAT
	SetbUpdate( true );
	SetbModal( TRUE );
	// 팝업뜸 이벤트를 전달
//	GAME->DispatchEvent( xAE_POPUP, 0, GetstrIdentifier() );
}

void XWndSpotRecon::Destroy()
{
}

void XWndSpotRecon::Update()
{
	XE::VEC2 vStart( 279, 53 );
	const XE::VEC2 vSize(49, 49);
	XE::VEC2 v = vStart;
	auto spLegion = m_pBaseSpot->GetspLegion();
	XBREAK( spLegion == nullptr );
	// 우측 군단 슬롯
	for( int i = 0; i < 3; ++i ) {
		for( int k = 0; k < 5; ++k ) {
			v.x = vStart.x + 70.f * i;
			v.y = vStart.y + 52.f * k;
			int idx = ( i * 5 + k );
			ID idWnd = 100 + idx;
			XHero *pHero = nullptr;
			auto pSquad = spLegion->GetSquadron( idx );
			bool bFog = false;
			auto vCenter = v + vSize * 0.5f;
			if( pSquad ) {
				pHero = pSquad->GetpHero();
				XBREAK( pHero == nullptr );
				bFog = spLegion->IsFog( pHero->GetsnHero() );
#ifdef _CHEAT
				if( XAPP->m_bDebugMode && m_bCheatShow ) 
#endif // _CHEAT
				{
					bFog = false;		// 안개사용안함.
				}
				if( bFog )
					pHero = nullptr;
			}
			// 부대슬롯(혹은 빈슬롯) 생성
			auto pWnd = Find( idWnd );
			XWndSquadInLegion *pWndSquad = nullptr;
			if( pWnd )
				pWndSquad = SafeCast<XWndSquadInLegion*>( pWnd );
			if( pWndSquad == nullptr ) {
				pWndSquad
					= new XWndSquadInLegion( pHero, v, spLegion.get(), true, false, bFog );
				if( pHero )
					pWndSquad->SetEvent( XWM_CLICKED, this, &XWndSpotRecon::OnClickSquad, pHero->GetsnHero() );
				Add( idWnd, pWndSquad );
				if( bFog ) {
					auto pWndGold = new XWndResourceCtrl( XE::VEC2(7,36) );
					pWndGold->AddRes( XGAME::xRES_GOLD, ACCOUNT->GetCostOpenFog( spLegion ) );
					pWndGold->SetScaleLocal( 0.8f );
					pWndGold->SetstrIdentifier("ctrl.gold");
					pWndSquad->Add( pWndGold );
					pWndGold->AutoLayoutHCenter();
				}
			} else {
				pWndSquad->SetFace( pHero, bFog );
				auto pWndGold = SafeCast2<XWndResourceCtrl*>( Find("ctgrl.gold") );
				if( pWndGold )
					pWndGold->EditRes( XGAME::xRES_GOLD, ACCOUNT->GetCostOpenFog( spLegion ) );
			}
		}
	}
//	여기도 클릭하면 안개없어지는 코드 넣을것.
	v = XE::VEC2( 56, 202 );
// 	ID idWnd = 1;
// 	for( auto& res : m_listLoot ) {
// 		auto pExist = Find( idWnd );
// 		if( pExist == nullptr ) {
// 			auto path = XE::MakePath( DIR_UI, XGAME::GetResourceIcon( res.type ) );
// 			auto pImg = new XWndImage( path, v );
// 			Add( idWnd, pImg );
// 			auto pText = new XWndTextString( v.x + 23.f, v.y + 3.f, XE::NumberToMoneyString( (int)res.num ), FONT_NANUM_BOLD, 30.f );
// 			Add( pText );
// 		}
// 		++idWnd;
// 		v.y += 20.f;
// 	}
	auto pWndRes = new XWndResourceCtrl( v, m_listLoot, true );
	Add( pWndRes );

	XWndPopup::Update();
}

/**
 @brief 
*/
int XWndSpotRecon::OnClickSquad( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickSquad");
	//
// 	ID snHero = p1;
// 	auto pHero = m_pBaseSpot->GetspLegion()->GetpHeroBySN( snHero );
// 	if( XASSERT(pHero) ) {
// 		auto pPopup = new XWndAbilTreeDebug( /*m_pAcc, */pHero->GetUnit() );
// 		Add( pPopup );
// 	}
	return 1;
}

/**
 @brief 
*/
int XWndSpotRecon::OnClickShow( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickShow");
	//
	m_bCheatShow = true;
	SetbUpdate( true );
	return 1;
}


////////////////////////////////////////////////////////////////
XWndCastleInfo::XWndCastleInfo()
	: XGameWndAlert( 288.f, 178.f )
{
	Init();
	//
	SetText(_T("목재/철 생산량"));
	AddButtonWithHeader( XWnd::xOK, _T("확인"), FONT_NANUM_BOLD, 25.f, _T("butt01"));
}

void XWndCastleInfo::Destroy()
{
}


////////////////////////////////////////////////////////////////
/**
 @brief 
*/
XWndSquadInLegion* XWndSquadInLegion::sUpdateCtrl( XWnd *pRoot, int idxSquad, const XE::VEC2& v, LegionPtr spLegion )
{
	std::string idsCtrl = XWndSquadInLegion::sGetIds( idxSquad );
	auto pWndCtrl = SafeCast2<XWndSquadInLegion*>( pRoot->Find( idsCtrl ) );
	if( pWndCtrl == nullptr ) {
		pWndCtrl = new XWndSquadInLegion( idxSquad, v, spLegion.get() );
		pWndCtrl->SetstrIdentifier( idsCtrl );
		pRoot->Add( pWndCtrl );
	}
	return pWndCtrl;
}
/**
 @brief 
 @param bDrawFrame 영웅 외곽 프레임을 그릴건지 말건지
*/
int XWndSquadInLegion::s_idxSelectedSquad = -1;
XWndSquadInLegion::XWndSquadInLegion( XHero *pHero, const XE::VEC2& vPos, XLegion *pLegion, bool bDrawFrame, bool bDragDrop, bool bQuestion )
	: XWnd( vPos.x, vPos.y )
{
	Init();
//	XE::MakePath(DIR_UI, _T("rect_squad.png"))
	m_bDrawFrame = bDrawFrame;
	m_pLegion = pLegion;
	if( bDrawFrame ) {
		// 사각 프레임
		auto pImg = new XWndImage( PATH_UI("common_bg_item.png"), 0, 0 );
		pImg->SetstrIdentifier( "img.frame" );
		pImg->SetScaleLocal( 0.9f );
		Add( pImg );
	}
	SetFace( pHero, bQuestion );
	SetAutoSize();
	// drop&drop이 가능하도록 설정
	if( bDragDrop ) {
		SetDragWnd( TRUE );
		SetDropWnd( TRUE );
	}
	for (int i = 0; i < 4; ++i)
		m_pStar[i] = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("common_etc_bicstar.png")));
	//
	m_pName = FONTMNG->CreateFontObj(FONT_NANUM, 18.f);
	m_pName->SetAlign(XE::xALIGN_HCENTER);
	m_pName->SetStyle(xFONT::xSTYLE_STROKE);
	m_pName->SetLineLength(64.f);
	m_pCrown = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("corps_crown.png")));
}

XWndSquadInLegion::XWndSquadInLegion( int idxSquad, const XE::VEC2& vPos, XLegion *pLegion, bool bDrawFrame, bool bDragDrop )
	: XWnd( vPos.x, vPos.y )
{
	Init();
	m_bDrawFrame = bDrawFrame;
	m_pLegion = pLegion;
	m_idxSquad = idxSquad;
	if( bDrawFrame ) {
		// 사각 프레임
		auto pImg = new XWndImage( PATH_UI( "common_bg_item.png" ), 0, 0 );
		pImg->SetstrIdentifier( "img.frame" );
		pImg->SetScaleLocal( 0.9f );
		Add( pImg );
	}
	SetAutoSize();
	// drop&drop이 가능하도록 설정
	if( bDragDrop ) {
		SetDragWnd( TRUE );
		SetDropWnd( TRUE );
	}
	for (int i = 0; i < 4; ++i)
		m_pStar[i] = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("common_etc_bicstar.png")));
	//
	m_pName = FONTMNG->CreateFontObj(FONT_NANUM, 18.f);
	m_pName->SetAlign(XE::xALIGN_HCENTER);
	m_pName->SetStyle(xFONT::xSTYLE_STROKE);
	m_pName->SetLineLength(64.f);
	m_pCrown = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("corps_crown.png")));
}
void XWndSquadInLegion::Destroy()
{
//	SAFE_RELEASE2( IMAGE_MNG, m_psfcSuperior );
//	SAFE_RELEASE2( IMAGE_MNG, m_psfcSelected );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcFace );
	for (int i = 0; i < XNUM_ARRAY(m_pStar); ++i)
		SAFE_RELEASE2(IMAGE_MNG, m_pStar[i]);
//	SAFE_RELEASE2(IMAGE_MNG, m_pNameCard);
	SAFE_DELETE(m_pName);
	SAFE_RELEASE2(IMAGE_MNG, m_pCrown);
}

void XWndSquadInLegion::Update()
{
	{
		bool bShowFrame = false;
		auto pImg = SafeCast2<XWndImage*>( Find("img.frame") );
		if( m_bDrawFrame ) {
			bShowFrame = true;
			// 사각 프레임
			if( XASSERT(pImg) ) {
				const _tstring strImg = ( m_pHero ) ? PATH_UI( "common_bg_item.png" )
					: PATH_UI( "common_bg_item2.png" );
				pImg->SetSurfaceRes( strImg );
				auto pText = xGET_TEXT_CTRL( pImg, "text.empty" );
				if( !m_pHero ) {
					if( !pText ) {
						pText = new XWndTextString( XE::VEC2( 0 ), XTEXT( 2030 ), FONT_NANUM, 15.f, XCOLOR_DARKGRAY ); // "empty"
//						pText->SetSizeLocal( pImg->GetSizeLocalNoTrans() );
						pText->SetLineLength( pImg->GetSizeLocalNoTrans().w );
						pText->SetstrIdentifier( "text.empty" );
						pText->SetAlignCenter();
						pText->SetStyleStroke();
						pImg->Add( pText );
//						pText->AutoLayoutCenter();
					}
				}
				if( pText )
					pText->SetbShow( m_pHero == nullptr );
			}
		}
		if( pImg )
			pImg->SetbShow( bShowFrame );
	}
	if( m_pHero ) {
		if( m_bPlayer ) {
// 			XBREAK( m_idxSquad < 0 );
// 			if( s_idxSelectedSquad >= 0 && m_idxSquad == s_idxSelectedSquad ) {
// 				if( m_psfcSelected == nullptr ) {
// 					m_psfcSelected = IMAGE_MNG->Load( PATH_UI("common_bg_item_glow.png") );
// 					XBREAK( m_psfcSelected == nullptr );
// 				}
// 			} else {
// 				SAFE_RELEASE2( IMAGE_MNG, m_psfcSelected );
// 			}
		} else {
			// enemy
			ID idAct = 1;
			switch( m_nSuperior ) {
			case 2:	idAct = 2;	break;
			case 1: idAct = 1;	break;
			case 0: idAct = 5;	break;
			case -1: idAct = 3; break;
			case -2: idAct = 4;	break;
			default:
				XBREAK(1);
				break;
			}
			if( s_idxSelectedSquad >= 0 ) {
				auto pWndSpr = static_cast<XWndSprObj*>( Find( "spr.arrow" ) );
				if( pWndSpr == nullptr ) {
					pWndSpr = new XWndSprObj( _T( "ui_upstat.spr" ), idAct, -6, 10 );
					pWndSpr->SetstrIdentifier( "spr.arrow" );
					Add( pWndSpr );
				}
				pWndSpr->SetAction( idAct );
			}
		}
	} else {
		DestroyWndByIdentifier( "spr.arrow" );
	}
	{
		bool bShowGlow = false;
		auto pImgSel = SafeCast2<XWndImage*>( Find("img.glow.selected") );
		if( m_bPlayer ) {
			XBREAK( m_idxSquad < 0 );
			if( s_idxSelectedSquad >= 0 && m_idxSquad == s_idxSelectedSquad ) {
				bShowGlow = true;
				if( pImgSel == nullptr ) {
					pImgSel = CreateGlowSelected();
// 					pImgSel = new XWndImage( PATH_UI( "common_bg_item_glow.png" ), XE::VEC2( -2.5f ) );
// 					pImgSel->SetstrIdentifier( "img.glow.selected" );
// 					pImgSel->SetblendFunc( XE::xBF_ADD );
// 					pImgSel->SetScaleLocal( 0.9f );
					Insert( "img.slot.unit", pImgSel );
				}
			}
		}
		if( pImgSel )
			pImgSel->SetbShow( bShowGlow );
	}

	XWnd::Update();
}

XWndImage* XWndSquadInLegion::CreateGlowSelected()
{
	auto 
	pImgSel = new XWndImage( PATH_UI( "common_bg_item_glow.png" ), XE::VEC2( -2.5f ) );
	pImgSel->SetstrIdentifier( "img.glow.selected" );
	pImgSel->SetblendFunc( XE::xBF_ADD );
	pImgSel->SetScaleLocal( 0.9f );
	return pImgSel;
}

void XWndSquadInLegion::SetFace( XHero *pHero, bool bQuestion )
{
	if( pHero == nullptr && pHero == m_pHero )
		if( m_bQuestion == bQuestion )
			return;
	if( pHero && m_pHero )
		if( pHero->GetsnHero() == m_pHero->GetsnHero() )
			if( pHero->GetUnit() == m_Unit )
				return;
	m_bQuestion = bQuestion;
	SAFE_RELEASE2( IMAGE_MNG, m_psfcFace );
	DestroyWndByIdentifier("img.hero");
//	DestroyWndByIdentifier("img.slot.hero");
// 	DestroyWndByIdentifier("img.unit");
	DestroyWndByIdentifier("img.slot.unit");
	if( pHero == nullptr ) {
		DestroyWndByIdentifier("img.name.tag");
	}
	m_pHero = pHero;
	if( pHero ) {
		m_Unit = pHero->GetUnit();
		bool bResourceSquad = false;
		auto pSquad = m_pLegion->GetSquadronByHeroSN( pHero->GetsnHero() );
		if( pSquad )
			bResourceSquad = m_pLegion->IsResourceSquad( pHero->GetsnHero() );
		// 영웅 얼굴
		auto pImg = new XWndImage( XE::MakePath( DIR_IMG, pHero->GetpProp()->strFace.c_str() ),
										3, 3 );
		pImg->SetstrIdentifier( "img.hero" );
		pImg->SetScaleLocal(0.67f);
		Add( pImg );
		if( !pHero->GetbLive() ) {
			auto vPos = pImg->GetPosLocal();
			auto pWnd = new XWndRect( vPos, pImg->GetSizeLocal(), XCOLOR_RGBA( 255, 0, 0, 128 ) );
			Add( pWnd );
			auto pText = new XWndTextString( vPos+XE::VEC2(5,5), XTEXT(2127), FONT_NANUM, 20.f, XCOLOR_RGBA(255,128,128,255) );
			Add( pText );
		}
		// 영웅얼굴 서피스 버전(drag를 위함)
		m_psfcFace = IMAGE_MNG->Load( TRUE, 
								XE::MakePath( DIR_IMG, pHero->GetpProp()->strFace.c_str() ));
		if( m_bDrawFrame ) {
			// 알파뚫린 영웅 프레임으로 한번더 덮어준다.
// 			pImg = new XWndImage( XE::MakePath( DIR_UI, _T( "common_unit_bg_s.png" ) ), 0, 0 );
// 			pImg->SetstrIdentifier( "img.slot.hero" );
// 			Add( pImg );
		}
		if( bResourceSquad ) {
			if( Find("img.resource") == nullptr ) {
				pImg = new XWndImage( PATH_UI( "world_top_res_all.png" ), -6, -9 );
				pImg->SetstrIdentifier( "img.resource" );
				Add( pImg );
			}
		}
		if( Find("img.resource") )
			xSET_SHOW( this, "img.resource", bResourceSquad );
		// 이름 판
		XWnd *pExist = nullptr;
		pExist = Find("img.name.tag");
		if( pExist != nullptr ) {
			pExist->SetbDestroy(true);
		}
		pImg = new XWndImage(PATH_UI("bg_name.png"), XE::xPF_ARGB8888, XE::VEC2(-1, 40));
		pImg->SetstrIdentifier("img.name.tag");
//		pImg->SetScaleLocal(1.5f, 1.f);
		Add(pImg);

		{
			bool bShowGlow = false;
			auto pImgSel = SafeCast2<XWndImage*>( Find("img.glow.selected") );
			if( m_bPlayer ) {
				XBREAK( m_idxSquad < 0 );
				if( s_idxSelectedSquad >= 0 && m_idxSquad == s_idxSelectedSquad ) {
					bShowGlow = true;
					if( pImgSel == nullptr ) {
						pImgSel = CreateGlowSelected();
// 						pImgSel = new XWndImage( PATH_UI( "common_bg_item_glow.png" ), XE::VEC2( -2.5f ) );
// 						pImgSel->SetstrIdentifier( "img.glow.selected" );
// 						pImgSel->SetblendFunc( XE::xBF_ADD );
// 						pImgSel->SetScaleLocal( 0.9f );
						Add( pImgSel );
					}
				}
			}
			if( pImgSel )
				pImgSel->SetbShow( bShowGlow );
		}

		auto pPropUnit = PROP_UNIT->GetpProp( pHero->GetUnit() );
		if( pPropUnit ) {
			// 유닛용 원형 프레임
			auto pWndUnit = new XWndCircleUnit( pHero->GetUnit(), XE::VEC2(31,-11), pHero );
			pWndUnit->SetbShowLevelSquad( true );
			pWndUnit->SetstrIdentifier("img.slot.unit");
			Add( pWndUnit );
		}
//		auto pText = new XWndTextStringArea( )
	} else
	{
		m_nGrade = 0;
		if( bQuestion ) {
			// 영웅얼굴을 ?로 표시
			auto pImg = new XWndImage( PATH_IMG( "hero_q.png" ), 4, 4 );
			pImg->SetstrIdentifier( "img.hero" );
			pImg->SetScaleLocal( 0.67f );
			Add( pImg );
		}
// 		XWndImage *pImg = new XWndImage(XE::MakePath(DIR_UI, _T("corps_legionnaire_bg.png")), 46, 19);
// 		pImg->SetstrIdentifier("img.slot.unit");
// 		Add(pImg);
	}
}

void XWndSquadInLegion::Draw( void )
{
	const XE::VEC2 vPos = GetPosFinal();
	XWnd::Draw();
	//
// 	if( m_psfcSelected ) {
// 		m_psfcSelected->SetScale( 0.9f );
// 		m_psfcSelected->SetBlendFunc( XE::xBF_ADD );
// 		m_psfcSelected->Draw( vPos - XE::VEC2(2.5f) );
// 	}
	///< 
	if (m_pHero) {
		for (int i = 0; i < m_nGrade; ++i) {
			if (m_pStar[i] != NULL) {
				m_pStar[i]->SetScale(0.6f);
				m_pStar[i]->Draw(vPos.x - 1 + i * 13, vPos.y - 1);
			}
		}
		if (m_pCrown && m_pLegion && m_pLegion->GetpLeader() == m_pHero) {
			m_pCrown->SetScale( 0.8f );
			m_pCrown->Draw(vPos.x + 40, vPos.y - 12);
		}
		XCOLOR col = XGAME::GetGradeColor( m_pHero->GetGrade() );
		m_pName->SetColor( col );
		m_pName->DrawString(vPos + XE::VEC2(-8, 38), m_pHero->GetpProp()->GetstrName().c_str());
		m_pName->SetColor( XCOLOR_WHITE );
		m_pName->DrawString( vPos, XE::Format(_T("Lv%d"), m_pHero->GetLevel()) );
	}
}

void XWndSquadInLegion::DrawDrag( const XE::VEC2& vMouse )
{
	if( m_psfcFace )
	{
		XE::VEC2 vSize = m_psfcFace->GetSize();
		m_psfcFace->SetfAlpha( 0.5f );
		m_psfcFace->Draw( vMouse - vSize / 2.f );
	}
}

////////////////////////////////////////////////////////////////
/**
 @brief 
*/
XWndVisit::XWndVisit( XSpotVisit *pSpot )
	: XWndView( _T( "common_bg_popup.png" ) )
{
	Init();
	m_pSpot = pSpot;
}

BOOL XWndVisit::OnCreate()
{
	XArrayLinearN<XWnd*, 256> ary;

	// 캠페인 제목
	{
		XWndTextString *pText = new XWndTextString( XE::VEC2( 156, 25 ), 
													m_pSpot->GetName(), 
													FONT_NANUM_BOLD, 
													40.f );
		pText->SetLineLength( 211.f );
		pText->SetAlign( XE::xALIGN_HCENTER );
		pText->SetStyle( xFONT::xSTYLE_STROKE );
		Add( pText );
		// 설명
		pText = new XWndTextString( XE::VEC2(93,107), m_pSpot->GetDialog(), FONT_NANUM, 25.f );
		pText->SetLineLength( 332.f );
		Add( pText );


	}
	///< ok버튼
	XWndButtonString *pButt = new XWndButtonString( 0.f, 286.f,
												_T( "OK" ),
												30.f,
												BUTT_MID );
	pButt->SetEvent( XWM_CLICKED, this, &XWndVisit::OnOk );
	Add( pButt );
	pButt->AutoLayoutHCenter();
	return TRUE;
}

void XWndVisit::Destroy()
{
}

/****************************************************************
* @brief
*****************************************************************/
int XWndVisit::OnOk( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "OnOk" );
	//
	SetbDestroy( TRUE );
	return 1;
}

////////////////////////////////////////////////////////////////
#ifdef _xIN_TOOL
XWndSpotForTool* XWndSpotForTool::s_pDrag = nullptr;
XWndSpotForTool* XWndSpotForTool::s_pSelected = nullptr;
XWndSpotForTool* XWndSpotForTool::s_pMouseOver = nullptr;
XE::VEC2 XWndSpotForTool::s_vUndo;		// 드래그 하기 이전에 있던 스팟의 좌표(언두용)
ID XWndSpotForTool::s_idUndoSpot = 0;
//
XWndSpotForTool::XWndSpotForTool( XPropWorld::xBASESPOT *pBaseProp )
	: XWndSprObj( 0, 0 )
{
	Init();
	SetPosLocal( pBaseProp->vWorld );
	m_pBaseProp = pBaseProp;
	if( !pBaseProp->strSpr.empty() )	// spr이 따로 지정되어있다면.
		SetSprObj( pBaseProp->strSpr.c_str(), 1 );
	else {
		// 각 스팟별 스프라이트 지정
		switch( pBaseProp->type ) {
		case XGAME::xSPOT_CASTLE:
			SetSprObj( SPR_CASTLE );
			break;
		case XGAME::xSPOT_JEWEL:
			SetSprObj( SPR_JEWEL );
			break;
		case XGAME::xSPOT_SULFUR:
			SetSprObj( SPR_SULFUR );
			break;
		case XGAME::xSPOT_MANDRAKE:
			SetSprObj( SPR_MANDRAKE );
			break;
		case XGAME::xSPOT_NPC:
			SetSprObj( SPR_NPC );
			break;
		case XGAME::xSPOT_DAILY:
			SetSprObj( SPR_DAILY );
			break;
		case XGAME::xSPOT_SPECIAL:
			SetSprObj( SPR_SPECIAL );
			break;
		case XGAME::xSPOT_CAMPAIGN:
			SetSprObj( SPR_CAMPAIGN );
			break;
		case XGAME::xSPOT_VISIT:
			SetSprObj( SPR_VISIT );
			break;
		case XGAME::xSPOT_CASH:
			SetSprObj( SPR_CASH );
			break;
		case XGAME::xSPOT_COMMON:
			SetSprObj( SPR_COMMON );
			break;
		default:
			XBREAK( 1 );
			break;
		}
	}
	// 이름영역 공통
// 	m_pTextName = new XWndTextString( XE::VEC2( 0 ), _T( "" ), FONT_NANUM, 20.0f );
// 	if( m_pTextName ) {
// 		m_pTextName->SetstrIdentifier( "text.spot.name" );
// 		m_pTextName->SetStyle( xFONT::xSTYLE_STROKE );
// 		Add( m_pTextName );
// 		m_pTextName->SetbShow( FALSE );	// 이름안나오는 스팟\도 있으니 일단 감춤.
// 	}
	SetbUpdate( TRUE );
}

void XWndSpotForTool::OnLButtonDown( float lx, float ly )
{
//	if( XAPP->IsToolSpotMode() ) {
	if( XBaseTool::s_pCurr->GetToolMode() == xTM_SPOT ) {
		if( s_pDrag == nullptr ) {
			s_pDrag = this;
			m_vLDown.Set( lx, ly );
			m_vPrev.Set( lx, ly );
		}
		if( SCENE_WORLD )
			SCENE_WORLD->GetpScrollView()->SetLock( TRUE );
	}
	__super::OnLButtonDown( lx, ly );
}

void XWndSpotForTool::OnMouseMove( float lx, float ly )
{
	if( XBaseTool::s_pCurr->GetToolMode() == xTM_SPOT ) {
		if( s_pDrag == this ) {
			XE::VEC2 vCurr( lx, ly );
			XE::VEC2 vDist = vCurr - m_vPrev;
			// 살짝 움직인건 인정 안함.
			if( vDist.Lengthsq() > 3.f * 3.f ) {
				XE::VEC2 vPos = GetPosLocal();
				vPos += vDist;
				SetPosLocal( vPos );
			}
	//		m_vPrev = vCurr;
		}
	}
	__super::OnMouseMove( lx, ly );
}

void XWndSpotForTool::OnLButtonUp( float lx, float ly )
{
	if( XBaseTool::s_pCurr->GetToolMode() == xTM_SPOT ) {
		if( s_pDrag == this ) {
			XE::VEC2 vPrevSpot = m_pBaseProp->vWorld;
			XE::VEC2 v = GetPosLocal();
			XE::VEC2 vDist = v - m_pBaseProp->vWorld;
			m_pBaseProp->vWorld = v;
			s_pSelected = this;
			if( vDist.Lengthsq() > 3.f * 3.f ) {
				s_idUndoSpot = m_pBaseProp->idSpot;
				s_vUndo = vPrevSpot;
				if( SCENE_WORLD ) {
					SCENE_WORLD->UpdateCloudSpotList( true );	// 
					SCENE_WORLD->UpdateAutoSave();
				}
			}
		}
		s_pDrag = nullptr;
		if( SCENE_WORLD )
			SCENE_WORLD->GetpScrollView()->SetLock( FALSE );
	}
	__super::OnLButtonUp( lx, ly );
}

void XWndSpotForTool::Update()
{
	if( m_pBaseProp ) {
		SetPosLocal( m_pBaseProp->vWorld );
		// 생산되는 자원을 아이콘으로 찍는다.
		// 목재/철은 찍지 않음
		// 생산되는 자원은 윈도우를 만들고, 생산안되는 자원은 윈도우를 삭제
		// 
		if( m_pBaseProp->type == XGAME::xSPOT_CASTLE ) {
			auto pProp = static_cast<XPropWorld::xCASTLE*>( m_pBaseProp );
			if( XASSERT(pProp) ) {
				XE::VEC2 v(-10, -33 );
				// 목재/철은 안찍음
				for( int i = XGAME::xRES_JEWEL; i < XGAME::xRES_MAX; ++i ) {
					auto pResNum = pProp->GetProduce( (XGAME::xtResource)i );
					if( pResNum ) {
						// 있는건 생성
						auto szRes = XGAME::GetResourceIcon2( pResNum->type );
						XWndImage *pImg = static_cast<XWndImage*>( Findf( "img.res.%d", pResNum->type ) );
						if( pImg == nullptr ) {
							pImg = new XWndImage( szRes, v );
							pImg->SetstrIdentifierf( "img.res.%d", pResNum->type );
							Add( pImg );
						}
						pImg->SetPosLocal( v );
						v.x += pImg->GetSizeLocal().w;
					} else {
						// 없는건 삭제
						DestroyWndByIdentifierf( "img.res.%d", i );
					}
				}
			}
		}
	}
}

void XWndSpotForTool::Draw()
{
	if( s_pMouseOver && s_pMouseOver->getid() == getid() )
	{
		if( XWnd::s_pMouseOver == nullptr ||
			(XWnd::s_pMouseOver && XWnd::s_pMouseOver->getid() != getid()) )
			s_pMouseOver = nullptr;
	}
	if( XWnd::s_pMouseOver && XWnd::s_pMouseOver->getid() == getid() ) {
		
		s_pMouseOver = this;
		XE::VEC2 v = GetPosFinal();
		XE::VEC2 size = GetSizeFinal();
		GRAPHICS->DrawCircle( v.x, v.y, size.Length(), XCOLOR_RED );
		XE::VEC2 vMouse = INPUTMNG->GetMousePos();
		vMouse += XE::VEC2( 30, 30 );
		XE::VEC2 vWorld = m_pBaseProp->vWorld;
		_tstring str = XFORMAT( "pos:%d,%d\n%s\ncode=%d\n", (int)vWorld.x, (int)vWorld.y,
														m_pBaseProp->strIdentifier.c_str(),
														m_pBaseProp->idCode );
		str += XTEXT(m_pBaseProp->idName);
		PUT_STRINGF( vMouse.x, vMouse.y, XCOLOR_WHITE, "%s", str.c_str() );
	}
	if( s_pSelected == this )
	{
		XWndSprObj::SetColor( 1.f, 0.f, 0.f );
	} else
		XWndSprObj::SetColor( 1.f, 1.f, 1.f );
	//
	XWndSprObj::Draw();
}
#endif // _xIN_TOOL


////////////////////////////////////////////////////////////////
// XWndEncounter::XWndEncounter( int numSulfur, ID idEnemy, int level, LPCTSTR szEnemy )
// 	: XWndPopup( _T("layout_encounter.xml"), "popup"	)
XWndEncounter::XWndEncounter( XSpotSulfur *pSpot, const XGAME::xBattleStartInfo& info )
	: XWndPopup( _T("layout_encounter.xml"), "popup"	)
{
	Init();
	///< 
	ID idEncounterEnemy = info.m_idEnemy;
//	ID idEncounterEnemy = pSpot->GetidEncounterUser();
//	xSET_TEXT( this, "text.name.enemy", XFORMAT("Lv%d %s", pSpot->GetLevel(), pSpot->GetstrName().c_str()) );
//	xSET_TEXT( this, "text.num.sulfur", XFORMAT("x%s", XE::NumberToMoneyString(pSpot->GetnumSulfur())) );
	auto pButt = xGET_BUTT_CTRL( this, "butt.retreat" );
	XBREAK( pButt == nullptr );
	int cost = ACCOUNT->GetCostCashSkill( XGAME::xCS_SULFUR_RETREAT );
	xSET_TEXT( this, "text.cost.cash", XFORMAT("%s", XE::NtS(cost)) );
	auto pRoot = Find("wnd.sulfur");
	if( pRoot ) {
		XGAME::xReward reward;
		reward.SetResource( XGAME::xRES_SULFUR, pSpot->GetnumSulfur() );
		auto pWndReward = new XWndStoragyItemElem( XE::VEC2(0), reward );
//		pWndReward->SetNum( reward.num );
		pRoot->Add( pWndReward );
	}
	xSET_TEXT( this, "text.level", XFORMAT("%d", info.m_Level) );
	xSET_TEXT( this, "text.name", info.m_strName );
	xSET_TEXT( this, "text.power", XE::NtS(info.m_Power) );
	xSET_TEXT( this, "text.score", XE::NtS(info.m_Ladder) );
	
}
////////////////////////////////////////////////////////////////
XWndBuffList::XWndBuffList( float x, float y )
{
	Init();
	SetbUpdate( true );
	SetbTouchable( FALSE );
	SetPosLocal( x, y );
}

void XWndBuffList::Update()
{
	// 계정의 버프리스트를 돌면서 버프윈도우를 만든다. 이미 있으면 만들지 않는ㄷㅏ.
	XE::VEC2 v;
	for( auto& buff : ACCOUNT->GetlistBuff() )
	{
		auto pProp = PROP_ITEM->GetpProp( buff.idBuff );
		XBREAK( pProp == nullptr );
		std::string strIdentifier = "wnd.buff.";
		strIdentifier += SZ2C( buff.sid.c_str() );
		XWnd *pWnd = Find(strIdentifier.c_str() );
		if( pWnd == nullptr )
		{
			LPCTSTR szIcon = XE::MakePath( DIR_IMG, pProp->strIcon.c_str() );
			pWnd = new XWndBuffElem( &buff );
			pWnd->SetstrIdentifier( strIdentifier.c_str() );
			Add( pWnd );
		}
		pWnd->SetPosLocal( v );
		v.x += 32.f;
	}
	//
	XWnd::Update();
}


////////////////////////////////////////////////////////////////
XWndBuffElem::XWndBuffElem( XGAME::xBuff *pBuff )
	: XWndImage( XE::MakePath(DIR_IMG, pBuff->strIcon), 0, 0 )
{
	Init();
// 	m_idBuff = idBuff;z
// 	m_resImg = resImg;
	m_pBuff = pBuff;
	SetScaleLocal(0.5f);
	SetAutoUpdate( 0.1f );
	SetEvent( XWM_CLICKED, this, &XWndBuffElem::OnClick );
}

void XWndBuffElem::Destroy()
{
	auto pExist = SCENE_WORLD->Find("wnd.view.buff");
	if( pExist )
		pExist->SetbDestroy( TRUE );
}

void XWndBuffElem::Update()
{
// 	if( !m_resImg.empty() )
// 	{
// 		SetSurfaceRes( m_resImg.c_str() );
// 	}
	auto pBuff = ACCOUNT->GetBuff( m_pBuff->idBuff );
	if( pBuff == nullptr )
	{
		SetbDestroy( TRUE );
		return;
	}
	auto pText = xGET_TEXT_CTRL( this, "text.remain.time");
	if( pText == nullptr )
	{
		pText = new XWndTextString( XE::VEC2(-8,32), _T(""), FONT_NANUM, 15.f );
		pText->SetstrIdentifier("text.remain.time");
		pText->SetStyleStroke();
		Add( pText );
	}
	float sec = pBuff->timer.GetsecRemainTime();
	if( sec < 0 )
		sec = 0;
	_tstring str = XSceneTech::_sGetResearchTime( (int)sec );
	pText->SetText( str.c_str() );

}

/****************************************************************
* @brief 
*****************************************************************/
int XWndBuffElem::OnClick( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClick");
	//
	auto v = GetPosFinal();
	XWnd *pExist = SCENE_WORLD->Find("wnd.view.buff");
	if( pExist )
		pExist->SetbDestroy( TRUE );
	auto pView = new XWndView( v.x, v.y + 32.f, 96.f, 96.f, _T("popup01.png") );
	pView->SetstrIdentifier("wnd.view.buff");
	pView->SetEvent( XWM_CLICKED, this, &XWndBuffElem::OnClickTooltip );
	SCENE_WORLD->Add( pView );
	// 버프 이름.
	auto pText = new XWndTextString( XE::VEC2(0,0), XTEXT(m_pBuff->idName), FONT_NANUM_BOLD, 20.f );
	pText->SetStyleStroke();
	pView->Add( pText );
	pText = new XWndTextString( XE::VEC2( 0, 13 ), XTEXT( m_pBuff->idDesc ), FONT_NANUM_BOLD, 18.f );
	pText->SetLineLength( 96.f );
	pText->SetStyleShadow();
	pView->Add( pText );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndBuffElem::OnClickTooltip( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickTooltip");
	//
	pWnd->SetbDestroy( TRUE );
	return 1;
}

////////////////////////////////////////////////////////////////
/**
 @brief 전투 통계
*/
XWndStatistic::XWndStatistic( XSPLegionObj spLegionObj1, XSPLegionObj spLegionObj2 )
	: XWndPopup(_T("layout_statistic.xml"), "popup_statistic" )
{
	Init();
	m_aryLegionObj.Add( spLegionObj1 );
	m_aryLegionObj.Add( spLegionObj2 );
	///< 
	//
	auto pView = new XWndScrollView( XE::VEC2(27,84), XE::VEC2(469,219) );
//	pView->SetScrollLockVert();
	pView->SetScrollVertOnly();
	Add( pView );
	m_pView = pView;
	xSetButtHander( this, this, "butt.left", &XWndStatistic::OnClickPrev );
	xSetButtHander( this, this, "butt.right", &XWndStatistic::OnClickNext );
	//
	SetbUpdate( true );
}

void XWndStatistic::CreateHerosUI( XWnd *pRoot, XSPLegionObj spLegionObj, float max, xtStatistic type, XGAME::xtSide side )
{
	switch( type )
	{
	case XWndStatistic::xST_DEAL:
		spLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageDeal > pStat2->damageDeal ) ? true : false;
		} );
		break;
	case XWndStatistic::xST_ATTACKED:
		spLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageAttacked > pStat2->damageAttacked ) ? true : false;
		} );
		break;
	case XWndStatistic::xST_DEAL_BY_CRITICAL:
		spLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageDealByCritical > pStat2->damageDealByCritical ) ? true : false;
		} );
		break;
	case XWndStatistic::xST_BY_EVADE:
		spLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageByEvade > pStat2->damageByEvade ) ? true : false;
		} );
		break;
// 	case XWndStatistic::xST_HEAL:
// 		pLegionObj->GetpStatObj()->GetlistSquads().sort(
// 			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
// 			return ( pStat1->heal > pStat2->heal ) ? true : false;
// 		} );
// 		break;
	case XWndStatistic::xST_TREATED:
		spLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->treated > pStat2->treated ) ? true : false;
		} );
		break;
	case XWndStatistic::xST_DEAL_BY_SKILL:
		spLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageDealBySkill > pStat2->damageDealBySkill ) ? true : false;
		} );
		break;
	default:
		XBREAK(1);
		break;
	}
	float y = 0;
	for( auto pSquad : spLegionObj->GetpStatObj()->GetlistSquads() )
	{
		auto pHero = pSquad->pHero;
		auto pWndHero = new XWndInvenHeroElem( pHero, spLegionObj->GetspLegion().get() );
		pWndHero->SetUnitFace();
		if( side == XGAME::xSIDE_PLAYER )
			pWndHero->SetPosLocal( XE::VEC2( 0, y ) );
		else
			pWndHero->SetPosLocal( XE::VEC2( 382, y ) );
		pRoot->Add( pWndHero );
		float lerp = 0.f;
		_tstring strNum;
		switch( type )
		{
		case XWndStatistic::xST_DEAL:
			lerp = pSquad->damageDeal / max;
			strNum = XE::NumberToMoneyString( (int)pSquad->damageDeal );
			break;
		case XWndStatistic::xST_ATTACKED:
			lerp = pSquad->damageAttacked / max;
			strNum = XE::NumberToMoneyString( (int)pSquad->damageAttacked );
			break;
		case XWndStatistic::xST_DEAL_BY_CRITICAL:
			lerp = pSquad->damageDealByCritical / max;
			strNum = XE::NumberToMoneyString( (int)pSquad->damageDealByCritical );
			break;
		case XWndStatistic::xST_BY_EVADE:
			lerp = pSquad->damageByEvade / max;
			strNum = XE::NumberToMoneyString( (int)pSquad->damageByEvade );
			break;
// 		case XWndStatistic::xST_HEAL:
// 			lerp = pSquad->heal / max;
//			strNum = XE::NumberToMoneyString( (int)pSquad->heal );
// 			break;
		case XWndStatistic::xST_TREATED:
			lerp = pSquad->treated / max;
			strNum = XE::NumberToMoneyString( (int)pSquad->treated );
			break;
		case XWndStatistic::xST_DEAL_BY_SKILL:
			lerp = pSquad->damageDealBySkill / max;
			strNum = XE::NumberToMoneyString( (int)pSquad->damageDealBySkill );
			break;
		default:
			XBREAK(1);
			break;
		}
		auto vPosHero = pWndHero->GetPosLocal();
		auto pWndBar = new XWndProgressBar( 0, 0, _T( "statistic_bar.png" ), nullptr );
		XE::VEC2 vBar;
		if( side == XGAME::xSIDE_PLAYER )
		{
			vBar = XE::VEC2(55.f, y+1.f);
		} else
		{
			auto sizeBar = pWndBar->GetSizeLocal();
			vBar = XE::VEC2( vPosHero.x - sizeBar.w, y+1.f );
			pWndBar->SetbReverse( true );
		}
		pWndBar->SetPosLocal( vBar );
		pWndBar->SetLerp( lerp );
		pRoot->Add( pWndBar );
		auto pText = new XWndTextString( 0, 0, strNum.c_str(), FONT_NANUM, 18.f );
		pText->SetStyleStroke();
		auto vText = vBar;
//		vText.y -= 10.f;
//		vText.x += 3.f;
		vText.y += 2.f;
		pText->SetPosLocal( vText );
		if( side == XGAME::xSIDE_PLAYER )
		{
		} else
		{
			pText->SetLineLength( pWndBar->GetSizeLocal().w );
			pText->SetAlign( XE::xALIGN_RIGHT );
		}
		pRoot->Add( pText );
		y += 57.f;
	}
}

void XWndStatistic::Destroy()
{
}

void XWndStatistic::Update()
{
	_tstring strTitle;
	switch( m_Type )
	{
	case XWndStatistic::xST_DEAL:
		strTitle = XTEXT( 2079 );
		break;
	case XWndStatistic::xST_ATTACKED:
		strTitle = XTEXT( 2080 );
		break;
	case XWndStatistic::xST_DEAL_BY_CRITICAL:
		strTitle = XTEXT( 2081 );
		break;
	case XWndStatistic::xST_BY_EVADE:
		strTitle = XTEXT( 2082 );
		break;
// 	case XWndStatistic::xST_HEAL:
// 		strTitle = XTEXT( 2083 );
// 		break;
	case XWndStatistic::xST_TREATED:
		strTitle = XTEXT( 2084 );
		break;
	case XWndStatistic::xST_DEAL_BY_SKILL:
		strTitle = XTEXT( 2085 );
		break;
	default:
		XBREAK(1);
		break;
	}
	xSET_TEXT( this, "text.title", strTitle );
	if( m_pView )
	{
		m_pView->DestroyChildAll();
		float max = 0;
		switch( m_Type )
		{
		case XWndStatistic::xST_DEAL:
			max = std::max( m_aryLegionObj[ 0 ]->GetpStatObj()->GetMaxDamageDeal(),
							m_aryLegionObj[ 1 ]->GetpStatObj()->GetMaxDamageDeal() );
			break;
		case XWndStatistic::xST_ATTACKED:
			max = std::max( m_aryLegionObj[ 0 ]->GetpStatObj()->GetMaxDamageAttacked(),
							m_aryLegionObj[ 1 ]->GetpStatObj()->GetMaxDamageAttacked() );
			break;
		case XWndStatistic::xST_DEAL_BY_CRITICAL:
			max = std::max( m_aryLegionObj[ 0 ]->GetpStatObj()->GetMaxDamageDealByCritical(),
							m_aryLegionObj[ 1 ]->GetpStatObj()->GetMaxDamageDealByCritical() );
			break;
		case XWndStatistic::xST_BY_EVADE:
			max = std::max( m_aryLegionObj[ 0 ]->GetpStatObj()->GetMaxDamageByEvade(),
							m_aryLegionObj[ 1 ]->GetpStatObj()->GetMaxDamageByEvade() );
			break;
// 		case XWndStatistic::xST_HEAL:
// 			max = std::max( m_aryLegionObj[ 0 ]->GetpStatObj()->GetMaxHeal(),
// 							m_aryLegionObj[ 1 ]->GetpStatObj()->GetMaxHeal() );
// 			break;
		case XWndStatistic::xST_TREATED:
			max = std::max( m_aryLegionObj[ 0 ]->GetpStatObj()->GetMaxTreated(),
							m_aryLegionObj[ 1 ]->GetpStatObj()->GetMaxTreated() );
			break;
		case XWndStatistic::xST_DEAL_BY_SKILL:
			max = std::max( m_aryLegionObj[ 0 ]->GetpStatObj()->GetMaxDamageDealBySkill(),
							m_aryLegionObj[ 1 ]->GetpStatObj()->GetMaxDamageDealBySkill() );
			break;
		default:
			XBREAK(1);
			break;
		}
		CreateHerosUI( m_pView, m_aryLegionObj[0], max, m_Type, XGAME::xSIDE_PLAYER );
		CreateHerosUI( m_pView, m_aryLegionObj[1], max, m_Type, XGAME::xSIDE_OTHER );
//		auto vSize = m_pView->GetMaxSizeByChild();
		int maxSquad = std::max( m_aryLegionObj[0]->GetlistSquad().size(),
								 m_aryLegionObj[1]->GetlistSquad().size() );
		auto vSize = XE::VEC2( m_pView->GetSizeLocal().w, maxSquad * 57.f );
		if( vSize.h < m_pView->GetSizeLocal().h )
			vSize.h = m_pView->GetSizeLocal().h;
		m_pView->SetViewSize( vSize );

	}
	XWndPopup::Update();
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndStatistic::OnClickPrev( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickPrev");
	//
	int type = m_Type;
	if( --type <= xST_NONE )
		type = xST_MAX - 1;
	m_Type = (xtStatistic)type;
	SetbUpdate( true );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndStatistic::OnClickNext( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickNext");
	//
	int type = m_Type;
	if( ++type >= xST_MAX )
		type = xST_NONE + 1;
	m_Type = (xtStatistic)type;
	SetbUpdate( true );
	return 1;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
XWndSlotByTrain::XWndSlotByTrain( XHero *pHero, XGAME::xtTrain type, ID snSlot, XLayout *pLayout, XWndTrainingCenter *pWndPopup )
{
	Init();
	m_Type = type;
	m_pHero = pHero;
	m_snSlot = snSlot;
	m_pLayout = pLayout;
	m_pPopup = pWndPopup;

// 	if( Find( "img.bg" ) == nullptr ) {
		// 슬롯 배경이미지.
		auto pImgSlot = new XWndImage( PATH_UI( "legion_herolist_bg.png" ), 0, 0 );
		pImgSlot->SetstrIdentifier( "img.bg" );
		pImgSlot->SetScaleLocal( 0.8785f );
		Add( pImgSlot );
		SetSizeLocal( pImgSlot->GetSizeFinal() );
// 	}
// 	XWnd* pWndRoot = Find( "wnd.root.layout" );
// 	if( pWndRoot == nullptr ) {
		auto pWndRoot = new XWnd();
		pWndRoot->SetstrIdentifier( "wnd.root.layout" );
		pWndRoot->SetSizeLocal( GetSizeLocal() );
		Add( pWndRoot );
// 	}
		switch( m_Type )
		{
		case XGAME::xTR_LEVEL_UP:
			m_pLayout->CreateLayout( "slot_base", pWndRoot );
			m_pLayout->CreateLayout( "slot_lvup", pWndRoot );
			m_pPopup->SetButtHander( pWndRoot, "butt.complete", &XWndTrainingCenter::OnClickComplete, m_snSlot );
			break;
		case XGAME::xTR_SQUAD_UP:
			m_pLayout->CreateLayout( "slot_base", pWndRoot );
			m_pLayout->CreateLayout( "slot_squadup", pWndRoot );
			m_pPopup->SetButtHander( pWndRoot, "butt.complete", &XWndTrainingCenter::OnClickComplete, m_snSlot );
			break;
		case XGAME::xTR_SKILL_ACTIVE_UP:
		case XGAME::xTR_SKILL_PASSIVE_UP:
			m_pLayout->CreateLayout( "slot_base", pWndRoot );
			m_pLayout->CreateLayout( "slot_skillup", pWndRoot );
			m_pPopup->SetButtHander( pWndRoot, "butt.complete", &XWndTrainingCenter::OnClickComplete, m_snSlot );
			break;
		case XGAME::xTR_EMPTY:
			m_pLayout->CreateLayout( "slot_empty", pWndRoot );
			break;
		case XGAME::xTR_LOCK: 	// lock상태 슬롯
			m_pLayout->CreateLayout( "slot_lock", pWndRoot );
			m_pPopup->SetButtHander( pWndRoot, "butt.unlock", &XWndTrainingCenter::OnClickUnlockSlot );
			break;
		default:
			XBREAK( 1 );
			break;
		}
		SetAutoUpdate( 0.1f );
}

void XWndSlotByTrain::Update()
{
	XWnd* pWndRoot = Find( "wnd.root.layout" );
	UpdateSlotBase( pWndRoot );
	switch( m_Type )
	{
	case XGAME::xTR_LEVEL_UP:
		UpdateSlotLvUp( pWndRoot );
		break;
	case XGAME::xTR_SQUAD_UP:
		UpdateSlotSquadUp( pWndRoot );
		break;
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP:
		UpdateSlotSkillUp( pWndRoot );
		break;
	case XGAME::xTR_EMPTY:
		UpdateSlotEmpty( pWndRoot );
		break;
	case XGAME::xTR_LOCK: 	// lock상태 슬롯
		UpdateSlotLock( pWndRoot );
		break;
	default:
		XBREAK(1);
		break;
	}

	XWnd::Update();
}

void XWndSlotByTrain::OnAutoUpdate()
{
	// snSlot이 있는 모든 슬롯 공통
	if( m_snSlot ) {
		auto pSlot = ACCOUNT->GetpTrainingSlot( m_snSlot );
		if( pSlot ) {
			// 즉시완료 비용 갱신
			int gold = ACCOUNT->GetGoldResearch( pSlot->GetsecRemain() );
			xSET_TEXT( this, "text.cost.complete", XE::NtS( gold ) );
			xSET_TEXT( this, "text.remain.time", pSlot->GetstrSecRemain() );
		}
	}
	switch( m_Type )
	{
	case XGAME::xTR_LEVEL_UP: {
	} break;
	case XGAME::xTR_SQUAD_UP:
		break;
	case XGAME::xTR_SKILL_ACTIVE_UP:
		break;
	case XGAME::xTR_SKILL_PASSIVE_UP:
		break;
	case XGAME::xTR_MAX:
		break;
	case XGAME::xTR_EMPTY:
		break;
	case XGAME::xTR_LOCK:
		break;
	default:
		XBREAK(1);
		break;
	}
}

void XWndSlotByTrain::UpdateSlotBase( XWnd* pWndRoot )
{
	XWnd *pWndFace = pWndRoot->Find( "wnd.root.hero" );
	if( pWndFace && pWndFace->GetNumChild() == 0 ) {
		auto pWnd = new XWndStoragyItemElem( XE::VEC2( 0 ), m_pHero );
		pWndFace->Add( pWnd );
		pWndFace->AutoLayoutHCenter();
	}
// 	auto pSlot = ACCOUNT->GetpTrainingSlot( m_snSlot );
// 	if( pSlot ) {
// 		// 즉시완료 비용 갱신
// 		int gold = ACCOUNT->GetGoldResearch( pSlot->GetsecRemain() );
// 		xSET_TEXT( this, "text.cost.complete", XE::NtS( gold ) );
// 	}
}
/**
 @brief 잠긴 슬롯 업데이트
*/
void XWndSlotByTrain::UpdateSlotLock( XWnd* pWndRoot )
{
	xSET_TEXT( pWndRoot, "text.cost.unlock", XE::NtS(300) );	// 슬롯 언락 가격
}
/**
 @brief 비어있음 슬롯 업데이트
*/
void XWndSlotByTrain::UpdateSlotEmpty( XWnd* pWndRoot )
{
}
/**
 @brief 영웅 렙업 슬롯
*/
void XWndSlotByTrain::UpdateSlotLvUp( XWnd* pWndRoot )
{
	auto pRootLeft = pWndRoot->Findf("wnd.stat.%d", 1);
	auto pRootRight = pWndRoot->Findf("wnd.stat.%d", 2);
	if( pRootLeft && pRootRight ) {
		XHero *pHero = m_pHero;
		//
		const auto type = m_Type;
		auto pSlot = ACCOUNT->GetpTrainingSlot( m_snSlot );
		if( pSlot ) {
			int lvWill = ACCOUNT->GetLvHeroAfterAddExp( pHero, type, pSlot->GetAddExp(), false, nullptr );
			std::vector<float> aryOrig;
			const bool bShow = true;
			aryOrig.push_back( pHero->GetAttackMeleeRatio( bShow )  );
			aryOrig.push_back( pHero->GetAttackRangeRatio( bShow )  );
			aryOrig.push_back( pHero->GetDefenseRatio( bShow )  );
			aryOrig.push_back( pHero->GetHpMaxRatio( bShow )  );
			aryOrig.push_back( pHero->GetAttackSpeed( bShow )  );
			aryOrig.push_back( pHero->GetMoveSpeed( bShow )  );
			std::vector<float> aryStat;
			aryStat.push_back( pHero->GetAttackMeleeRatio( lvWill, bShow )  );
			aryStat.push_back( pHero->GetAttackRangeRatio( lvWill, bShow )  );
			aryStat.push_back( pHero->GetDefenseRatio( lvWill, bShow )  );
			aryStat.push_back( pHero->GetHpMaxRatio( lvWill, bShow )  );
			aryStat.push_back( pHero->GetAttackSpeed( lvWill, bShow )  );
			aryStat.push_back( pHero->GetMoveSpeed( lvWill, bShow )  );
			//
			xSET_TEXT( pRootLeft, "text.level", pHero->GetLevel() );
			xSET_TEXT( pRootRight, "text.level", lvWill );
			for( int i = 0; i < 6; ++i ) {
				const auto statOrig = aryOrig[i];
				const auto statUp = aryStat[i];
				xSET_TEXTF( pRootLeft, XFORMAT( "%.0f%%", statOrig ), "text.stat.%d", i + 1 );
				auto pText =
				xSET_TEXTF( pRootRight, XFORMAT( "%.0f%%", statUp ), "text.stat.%d", i + 1 );
				if( statUp > statOrig )
					pText->SetColorText( XCOLOR_GREEN );
				else
					pText->SetColorText( XCOLOR_WHITE );
			}
		}
	}
}
/**
 @brief 부대 렙업 슬롯
*/
void XWndSlotByTrain::UpdateSlotSquadUp( XWnd* pWndRoot )
{
	auto pRootLeft = pWndRoot->Findf( "wnd.stat.%d", 1 );
	auto pRootRight = pWndRoot->Findf( "wnd.stat.%d", 2 );
	if( !pRootLeft || !pRootRight )
		return;
	XHero *pHero = m_pHero;
	auto pPropUnit = PROP_UNIT->GetpProp( pHero->GetUnit() );
	if( XASSERT( pPropUnit ) ) {
		const auto type = m_Type;
		auto resFace = XGAME::GetResUnitSmall( (XGAME::xtUnit)pPropUnit->idProp );
		auto pSlot = ACCOUNT->GetpTrainingSlot( m_snSlot );
		if( !pSlot ) 
			return;
		int lvWill = ACCOUNT->GetLvHeroAfterAddExp( pHero, type, pSlot->GetAddExp(), false, nullptr );
		xSET_IMG( pRootLeft, "img.unit", resFace );
		xSET_IMG( pRootRight, "img.unit", resFace );
		xSET_TEXT( pRootLeft, "text.level", pHero->GetLevel( type ) );
		auto pText =
		xSET_TEXT( pRootRight, "text.level", lvWill );
		if( lvWill > pHero->GetLevel(type) )
			pText->SetColorText( XCOLOR_GREEN );
		else
			pText->SetColorText( XCOLOR_WHITE );
		xSquadStat statOrig;
		xSquadStat statUp;
		pHero->GetSquadStatWithTech( pHero->GetlevelSquad(), &statOrig );
		pHero->GetSquadStatWithTech( lvWill, &statUp );
		for( int i = 0; i < 6; ++i ) {
			const auto valOrig = statOrig.GetStat(i);
			const auto valUp = statUp.GetStat(i);
			xSET_TEXTF( pRootLeft, XFORMAT( "%.0f", valOrig ), "text.stat.%d", i + 1 );
			pText =
			xSET_TEXTF( pRootRight, XFORMAT( "%.0f", valUp ), "text.stat.%d", i + 1 );
			if( valUp > valOrig )
				pText->SetColorText( XCOLOR_GREEN );
			else
				pText->SetColorText( XCOLOR_WHITE );
		}

	}
}
/**
 @brief 스킬 렙업 슬롯
*/
void XWndSlotByTrain::UpdateSlotSkillUp( XWnd* pWndRoot )
{
	auto pRootLeft = pWndRoot->Findf( "wnd.stat.%d", 1 );
	auto pRootRight = pWndRoot->Findf( "wnd.stat.%d", 2 );
	if( !pRootLeft || !pRootRight )
		return;
	const auto type = m_Type;
	XHero *pHero = m_pHero;
	auto pSkillDat = m_pHero->GetSkillDat( type );
	if( XASSERT(pSkillDat) ) {
		const _tstring resIcon = XE::MakePath( DIR_IMG, pSkillDat->GetstrIcon() );
		xSET_IMG( pRootLeft, "img.skill", resIcon );
		xSET_IMG( pRootRight, "img.skill", resIcon );
		auto pSlot = ACCOUNT->GetpTrainingSlot( m_snSlot );
		if( !pSlot )
			return;
		int lvWill = ACCOUNT->GetLvHeroAfterAddExp( pHero, type, pSlot->GetAddExp(), false, nullptr );
		xSET_TEXT( pRootLeft, "text.level", pHero->GetLevel( type ) );
		auto pText =
		xSET_TEXT( pRootRight, "text.level", lvWill );
		if( lvWill > pHero->GetLevel( type ) )
			pText->SetColorText( XCOLOR_GREEN );
		else
			pText->SetColorText( XCOLOR_WHITE );
		//
		_tstring strDesc;
		pSkillDat->GetSkillDesc( &strDesc, lvWill );
		xSET_TEXT( pWndRoot, "text.name", pSkillDat->GetstrName() );
		xSET_TEXT( pWndRoot, "text.desc", strDesc );
	}
}


//////////////////////////////////////////////////////////////////////////
XWndTrainingCenter::XWndTrainingCenter()
	: XWndPopup( _T( "layout_barrack.xml" ), "popup_barrack" )
{
	Init();
	SetstrIdentifier( "popup.train.center" );
	CreateSlots();
	SetbModal( TRUE );
}

void XWndTrainingCenter::Destroy()
{
}

BOOL XWndTrainingCenter::OnCreate()
{
	CONSOLE("0x%08x", (DWORD)this );
	// 다수영웅의 훈련이 완료될수 있으므로 이벤트 하나로는 안될듯.
	// 훈련중인 영웅 수만큼 콜백을 등록하고 키가 같으면 호출하고 지워야 할듯하다.
	XDelegator::sGet().DoRequest2( "train.complete", this
		, &XWndTrainingCenter::DelegateTrainComplete );
	XWndPopup::OnCreate();

	return TRUE;
}

void XWndTrainingCenter::CreateSlots()
{
	auto pList = xGET_LIST_CTRL( this, "list.slot" );
	if( XBREAK(pList && pList->GetNumItem() > 0) )
		return;

	// 훈련중인 슬롯
	const auto& listSlot = ACCOUNT->GetlistTrainSlot();
	for( auto& slotTrain : listSlot ) {
		auto pHero = ACCOUNT->GetHero( slotTrain.snHero );
		if( XASSERT( pHero ) ) {
			UpdateSlot( pHero, slotTrain, slotTrain.snSlot );
		}
	}
	int idEtc = 1;
	// 훈련중이 아닌 비어있는 슬롯.
	int numFreeSlot = ACCOUNT->GetNumRemainFreeSlot();
	if( XBREAK( numFreeSlot < 0 ) )
		numFreeSlot = 0;
	for( int i = 0; i < numFreeSlot; ++i ) {
		XAccount::xTrainSlot slot;
		slot.type = XGAME::xTR_EMPTY;
		slot.snSlot = 0;
		UpdateSlot( nullptr, slot, idEtc++ );
	}
	// lock slot
	{
		XAccount::xTrainSlot slot;
		slot.type = XGAME::xTR_LOCK;
		slot.snSlot = 0;
		UpdateSlot( nullptr, slot, idEtc++ );
	}
}

/**
 @brief 
 훈련중인 슬롯리스트를 먼저 나열하고 비어있는 슬롯 마지막에 잠김 슬롯 하나를 표시한다.
*/
void XWndTrainingCenter::Update()
{
	XWndPopup::Update();
}

void XWndTrainingCenter::UpdateSlot( XHero *pHero, const XAccount::xTrainSlot& slot, ID idWnd )
{
	auto pList = xGET_LIST_CTRL( this, "list.slot" );
	XWnd *pWndSlot = pList->Findf( "wnd.slot.0x%x", idWnd );
	// 이미 생성한 슬롯은 다시 생성하지 않도록 한다.
	if( pWndSlot == nullptr ) {
		pWndSlot = new XWndSlotByTrain( pHero, slot.type, slot.snSlot, GetpLayout(), this );
		pWndSlot->SetstrIdentifierf( "wnd.slot.0x%x", idWnd );
		pList->AddItem( idWnd, pWndSlot );
	}
}

void XWndTrainingCenter::DelegateUnlockTrainSlot( const std::string& idsEvent )
{
	// 리스트 클리어하고 다시 생성
	CONSOLE( "0x%08x", ( DWORD )this );
	auto pWndList = xGET_LIST_CTRL( this, "list.slot" );
	if( pWndList ) {
		pWndList->DestroyAllItem();
		CreateSlots();
	}
	SetbUpdate( true );
}


void XWndTrainingCenter::DelegateTrainComplete( const std::string& idsEvent
																							, XGAME::xtTrain type
																							, XHero* pHero )
{
	// 리스트 클리어하고 다시 생성
	CONSOLE( "0x%08x", ( DWORD )this );
	auto pWndList = xGET_LIST_CTRL( this, "list.slot" );
	if( pWndList ) {
		pWndList->DestroyAllItem();
		CreateSlots();
	}
//	SetbUpdate( true );
}

int XWndTrainingCenter::OnClickUnlockSlot( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickLockfree" );
	//
	const int cashCost = ACCOUNT->GetCashUnlockTrainingSlot();
	const int numFreeSlot = ACCOUNT->GetnumFreeSlot();
	if( ACCOUNT->IsNotEnoughCash(cashCost) ) {
		XWND_ALERT( "%s", XTEXT(80140) );		// 캐시부족
		return 1;
	}
	if( numFreeSlot >= ACCOUNT->GetMaxTrainSlot() ) {
		XWND_ALERT( "%s", XTEXT(2283) );		// 더이상 확장못함.
		return 1;
	}
	// callback
	XDelegator::sGet().DoRequest( "unlock.train.slot", this
															, &XWndTrainingCenter::DelegateUnlockTrainSlot );
	GAMESVR_SOCKET->SendReqUnlockTrainingSlot( GAME );
	return 1;
}

/**
 @brief 
*/
int XWndTrainingCenter::OnClickComplete( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickComplete: snSlot=%d", p1);
	//
	const ID snSlot = p1;
	auto pSlot = ACCOUNT->GetpTrainingSlot( snSlot );
	if( pSlot ) {
		const bool bCashPay = false;		// 금화로 지불
		GAMESVR_SOCKET->SendReqTrainCompleteQuick( GAME, pSlot->snSlot, pSlot->type, pSlot->snHero, bCashPay );
	}

	return 1;
}

////////////////////////////////////////////////////////////////
XWndTrainCompleInWorld::XWndTrainCompleInWorld( XHero *pHero )
	: XWndImage( nullptr, 0, 0 )
{
	Init();
	m_snHero = pHero->GetsnHero();
	SetSurfaceRes( XE::MakePath(DIR_IMG, pHero->GetpProp()->strFace ) );
	XE::VEC2 v = GetSizeLocal() * 0.5f;
	v.y = GetSizeLocal().h * 0.75f;
	SetScaleLocal( 0.6f );
	auto pWndSpr = new XWndSprObj( _T("ui_levelup.spr"), 2, v );
	pWndSpr->SetScaleLocal( 0.8f );
	Add( pWndSpr );
}

void XWndTrainCompleInWorld::Destroy()
{
}

void XWndTrainCompleInWorld::Update()
{

	XWndImage::Update();
}


////////////////////////////////////////////////////////////////
// XWndTrainCompleteInWorldMng::XWndTrainCompleteInWorldMng( const XE::VEC2& vPos )
// 	: XWnd( vPos )
// {
// 	Init();
// }
// 
// void XWndTrainCompleteInWorldMng::Destroy()
// {
// }
// 
// void XWndTrainCompleteInWorldMng::Update()
// {
// 	XArrayLinearN<XHero*, 256> aryHeroes;
// 	// 현재 렙업확인을 대기중인 영웅 리스트
// 	ACCOUNT->GetLevelupReadyHeroes( &aryHeroes );
// 	// 기존 리스트와 비교해서 없어진건 빼준다.
// 	XARRAYLINEARN_LOOP_AUTO( m_aryHeroesPrev, snHero )
// 	{
// 		bool bDel = false;
// 		XHero *pHero = ACCOUNT->GetHero( snHero );
// 		if( pHero && pHero->IsAnyLevelupReady() )
// 		{
// 		} else
// 			bDel = true;
// 		// 이전 update때는 있었으나 지금은 없어진 훈련완료영웅
// 		if( bDel )
// 		{
// 			DestroyWndByIdentifierf("wnd.complete.hero.%d", snHero);
// 		}
// 	} END_LOOP;
// 	m_aryHeroesPrev.Clear();
// 	// 렙업 대기중인 영웅 윈도우를 생성.
// 	XE::VEC2 v(0);
// 	XARRAYLINEARN_LOOP_AUTO( aryHeroes, pHero )
// 	{
// 		auto pWnd = Findf("wnd.complete.hero.%d", pHero->GetsnHero() );
// 		if( pWnd == nullptr )
// 		{
// 			auto pWndElem = new XWndTrainCompleInWorld( pHero );
// 			pWndElem->SetEvent( XWM_CLICKED, this, &XWndTrainCompleteInWorldMng::OnClickHero, pHero->GetsnHero() );
// 			pWndElem->SetstrIdentifierf("wnd.complete.hero.%d", pHero->GetsnHero() );
// 			pWndElem->SetPosLocal( v );
// 			Add( pWndElem );
// 			pWnd = pWndElem;
// 		}
// 		auto vSize = pWnd->GetSizeFinal();
// 		XBREAK( vSize.w <= 0 );
// 		v.x += vSize.w * 1.2f;
// 		m_aryHeroesPrev.Add( pHero->GetsnHero() );
// 	} END_LOOP;
// 
// 	XWnd::Update();
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndTrainCompleteInWorldMng::OnClickHero( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	ID snHero = p1;
// 	CONSOLE("OnClickHero:0x%08x", snHero );
// 	//
// 	XHero *pHero = ACCOUNT->GetHero( snHero );
// 	if( XBREAK(pHero == nullptr) )
// 		return 1;
// 	XGAME::xtTrain type;
// 	for( int i = 1; i < XGAME::xTR_MAX; ++i )
// 	{
// 		type = (XGAME::xtTrain)i;
// 		if( pHero->IsLevelupReady( type ) )
// 		{
// 			GAMESVR_SOCKET->SendReqLevelupConfirm( GAME, type, pHero );
// 			break;
// 		}
// 	}
// 	return 1;
// }

////////////////////////////////////////////////////////////////
// XWndTrainComplete::XWndTrainComplete( XHero *pHero, XGAME::xtTrain type )
// 	: XGameWndAlert( nullptr, nullptr, XWnd::xOK )
// {
// 	Init();
// 	_tstring str;
// 	switch( type )
// 	{
// 	case XGAME::xTR_LEVEL_UP:
// 		str = XE::Format( XTEXT( 2092 ), pHero->GetstrName().c_str(), pHero->GetLevel() );	// 영웅 xxx가 레벨x가 되었습니다.
// 		break;
// 	case XGAME::xTR_SQUAD_UP:
// 		str = XE::Format( XTEXT( 2094 ), pHero->GetstrName().c_str(), pHero->GetlevelSquad() );
// 		break;
// 	case XGAME::xTR_SKILL_ACTIVE_UP: {
// 		_tstring strSkill = XTEXT( pHero->GetSkillDatActive()->GetidName() );
// 		str = XE::Format( XTEXT( 2095 ), pHero->GetstrName().c_str(), 
// 										strSkill.c_str(),
// 										pHero->GetlvActive() );
// 	} break;
// 	case XGAME::xTR_SKILL_PASSIVE_UP: {
// 		_tstring strSkill = XTEXT( pHero->GetSkillDatPassive()->GetidName() );
// 		str = XE::Format( XTEXT( 2095 ), pHero->GetstrName().c_str(),
// 			strSkill.c_str(),
// 			pHero->GetlvPassive() );
// 	} break;
// 	default:
// 		break;
// 	}
// 	TCHAR szBuff[ 1024 ];
// 	XE::ConvertJosaStr( szBuff, str.c_str() );
// 	SetText( szBuff );
// 	auto vlCenter = GetSizeFinal() / 2.f;
// 	vlCenter.y -= 64.f;
// 	auto pWndSpr = new XWndSprObj( _T( "ui_firework.spr" ), 1, vlCenter, xRPT_1PLAY );
// 	Add( pWndSpr );
// 	SOUNDMNG->OpenPlaySound(27);
// }
// 
// void XWndTrainComplete::Destroy()
// {
// }
// 
// void XWndTrainComplete::Update()
// {
// 	XGameWndAlert::Update();
// }

////////////////////////////////////////////////////////////////
/**
 @brief 계정레벨이 올랐을때 뜨는 팝업.
*/
XWndLevelup::XWndLevelup()
	: XWndPopup(_T("layout_levelup.xml"), "popup_levelup")
{
	Init();
	//
	int level = ACCOUNT->GetLevel();
	auto pText = xGET_TEXT_CTRL( this, "text.level" );
	if( pText )
	{
		_tstring str = XE::Format( XTEXT( 2098 ), level );
		TCHAR szBuff[ 1024 ];
		XE::ConvertJosaStr( szBuff, str.c_str() );
		pText->SetText( szBuff );
	}
	int idx = 1;
	for( int i = 0; i < 4; ++i ) {
		auto pText = xGET_TEXT_CTRLF( this, "text.info.%d", idx );
		if( pText ) {
			_tstring str;
			if( i == 0 ) {
				_tstring str1 = XE::NumberToMoneyString( ACCOUNT->GetmaxAP( level - 1 ) );
				_tstring str2 = XE::NumberToMoneyString(ACCOUNT->GetmaxAP());
				str = XFORMAT("%s: %s => %s", XTEXT(2099), str1.c_str(), str2.c_str() );
				++idx;
			} else
			if( i == 1 ) {
// 				str = XTEXT(2100);
// 				++idx;
			} else
			if( i == 2 ) {
				int numCurr = XAccount::sGetMaxSquadByLevelForPlayer( level );
				int numPrev = XAccount::sGetMaxSquadByLevelForPlayer( level - 1 );
				if( numCurr != numPrev ) {
					str = XFORMAT( "%s: %d => %d", XTEXT( 2101 ), numPrev, numCurr );
					++idx;
				}
			} else
			if( i == 3 ) {
				if( ACCOUNT->GetbUnlockTicketForPaladin() ) {
					str = XE::Format( XTEXT(2141),		// xx유닛을 사용할수 있게 되었슴다.
									XGAME::GetStrUnit( XGAME::xUNIT_PALADIN) );
				} else
				// 중/대형유닛을 사용할수 있게 됨.
				if( ACCOUNT->GetnumUnlockTicketForMiddleOrBig() ) {
					str = XTEXT(2144);	// 새로운 유닛을 사용할수 있게 되었슴다.
				}
// 				// 렙업에 의해 새로 언락시킬수 있게 된 유닛이 있는지.
// 				if( ACCOUNT->IsUnlockableUnitByLevel() )
// 				{
// 					str = XFORMAT( "새로운 종류의 병사를 사용할 수 있게 되었습니다.", XTEXT( 30003 ), XTEXT( 30004 ), XTEXT( 30005 ) );
// 				}
			}
			pText->SetText( str );
			
		}
	}
}

////////////////////////////////////////////////////////////////
/**
 @brief 영웅의 능력이 반영된 부대능력치를 보여주는 버전
*/
XWndUnitinfo::XWndUnitinfo( XHero *pHero, LPCTSTR szTitle )
	: XWndPopup(_T("layout_unit_info.xml"), "popup_unit")
{
	Init();
	m_pHero = pHero;
	if( XASSERT(pHero) )
		m_Unit = pHero->GetUnit();
	SetbModal( TRUE );
	xSET_TEXT( this, "text.popup.title", szTitle );
	SetbUpdate( true );
}

/**
 @brief 유닛의 기본 스탯정보만 보여주는 버전
*/
XWndUnitinfo::XWndUnitinfo( XGAME::xtUnit unit, LPCTSTR szTitle )
	: XWndPopup(_T("layout_unit_info.xml"), "popup_unit")
{
	Init();
	SetstrIdentifier("popup.unit.unlock");
	m_Unit = unit;
	SetbModal( TRUE );
	xSET_TEXT( this, "text.popup.title", szTitle );
	SetbUpdate( true );
}

void XWndUnitinfo::Update()
{
	XBREAK( m_Unit == XGAME::xUNIT_NONE );
	auto pPropUnit = PROP_UNIT->GetpProp( m_Unit );
	if( XBREAK( pPropUnit == nullptr ) )
		return;
	xSquadStat statCurr;
	if( m_pHero ) {
// 		const auto& research = ACCOUNT->GetResearching();
// 		auto pHero = ACCOUNT->GetHero( research.GetsnHero() );	// 왜 이렇게 했지???
// 		if( pHero ) 
//			pHero->GetSquadStatWithTech( m_pHero->GetlevelSquad(), &statCurr );
		m_pHero->GetSquadStatWithTech( m_pHero->GetlevelSquad(), &statCurr );
	} else {
		statCurr.meleePower = pPropUnit->atkMelee;
		statCurr.rangePower = pPropUnit->atkRange;
		statCurr.def = pPropUnit->def;
		statCurr.hp = (int)pPropUnit->hpMax;
		statCurr.speedAtk = pPropUnit->atkSpeed;
		statCurr.speedMoveForMeter = xPIXEL_TO_METER( pPropUnit->movSpeedPerSec );
	}
	xSET_TEXT( this, "text.levelup.melee", XFORMAT( "%d", (int)statCurr.meleePower ) );
	if( m_Unit == XGAME::xUNIT_CYCLOPS )
		xSET_TEXT( this, "text.levelup.range", XFORMAT( "%d(광역)", (int)statCurr.rangePower ) );
	else
		xSET_TEXT( this, "text.levelup.range", XFORMAT( "%d", (int)statCurr.rangePower ) );
	xSET_TEXT( this, "text.levelup.def", XFORMAT( "%d", (int)statCurr.def ) );
	xSET_TEXT( this, "text.levelup.hp", XFORMAT( "%d", statCurr.hp ) );
	xSET_TEXT( this, "text.levelup.atk.speed", XFORMAT( "%d", (int)( statCurr.speedAtk * 100.f ) ) );
	xSET_TEXT( this, "text.levelup.move.speed", XFORMAT( "%d", (int)statCurr.speedMoveForMeter ) );
	xSET_TEXT( this, "text.desc", XTEXT( pPropUnit->idDesc ) );
	xSET_IMG( this, "img.icon.complate.legion", XE::MakePath( DIR_IMG, pPropUnit->strFace ) );
	if( m_pHero )
		xSET_TEXT( this, "text.legion.name", XFORMAT( "Lv%d %s", m_pHero->GetlevelSquad(), pPropUnit->strName.c_str() ) );
	else
		xSET_TEXT( this, "text.legion.name", pPropUnit->strName );
	XWndPopup::Update();
}

////////////////////////////////////////////////////////////////
/**
 @brief 각종 모자라는 자원을 캐쉬로 대신 지불할때 쓰는 공통 팝업.
 @param szMsg 본문메시지
 @param szTitle 타이틀 메시지
*/
XWndPaymentByCash::XWndPaymentByCash( LPCTSTR szMsg, LPCTSTR szTitle )
	: XWndPopup( _T( "buy_by_cash.xml" ), "popup" )
{
	Init();
	if( XE::IsHave( szTitle ) ) {
		m_strTitle = szTitle;
	}
	if( XE::IsHave( szMsg ) ) {
		m_strMsg = szMsg;
	}
	xSET_SHOW( this, "text.tip", (ACCOUNT->GetLevel() < 10) );
}

void XWndPaymentByCash::Update()
{
	// 캐쉬가 부족하면 빨간색.
	auto pTextCash = xSET_TEXT( this, "text.num.cash", XE::NtS(m_needCash) );
// 	auto pTextCash = xGET_TEXT_CTRL( this, "text.num.cash" );
	if( pTextCash && ACCOUNT->IsNotEnoughCash( m_needCash ) )
		pTextCash->SetColorText( XCOLOR_RED );
	if( m_strTitle.empty() ) {
		switch( m_typePayment ) {
		case xPR_GOLD:		m_strTitle = XTEXT(80169); break;
		case xPR_RES:			m_strTitle = XTEXT(2339); break;
		case xPR_AP:		m_strTitle = XTEXT(2340); break;
		case xPR_TIME:		break;
		case xPR_TRY_DAILY:		m_strTitle = XTEXT(2227); break;		// 도전횟수 부족
		default:
			XBREAK(1);
			break;
		}
	}
	xSET_TEXT( this, "text.title", m_strTitle );
	XWndPopup::Update();
}

int XWndPaymentByCash::OnOk( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	if( ACCOUNT->IsNotEnoughCash( m_needCash ) ) {
		XWND_ALERT_YESNO( "alert.goto.shop", "%s", _T("goto gem shop?") );
		return 1;
	}
	return XWndPopup::OnOk( pWnd, p1, p2 );
}

void XWndPaymentByCash::SetResource( const std::vector<XGAME::xRES_NUM>& aryLack )
{
	auto pWndRoot = Find( "wnd.half.top" );
	if( XASSERT( pWndRoot ) ) {
		m_typePayment = xPR_RES;
		auto pWnd = new XWndResourceCtrl( XE::VEC2( 0, 0 ), aryLack, true );
		Add( pWnd );
		pWnd->AutoLayoutCenter();
		m_needCash = 0;
		for( auto& res : aryLack ) {
			m_needCash += ACCOUNT->GetCashFromRes( res.type, (int)res.num );
		}
		SetbUpdate( true );
	}
}

void XWndPaymentByCash::SetResource( xtResource resType, int numLack )
{
	auto pWndRoot = Find("wnd.half.top");
	if( XASSERT(pWndRoot) ) {
		m_typePayment = xPR_RES;
		auto pWnd = new XWndResourceCtrl( XE::VEC2( 0 ), resType, numLack );
		pWndRoot->Add( pWnd );
		pWnd->AutoLayoutCenter();
		m_needCash = ACCOUNT->GetCashFromRes( resType, numLack );
		SetbUpdate( true );
	}
}

void XWndPaymentByCash::SetGold( int goldLack )
{
	auto pWndRoot = Find( "wnd.half.top" );
	if( XASSERT( pWndRoot ) ) {
		m_typePayment = xPR_GOLD;
		auto pWnd = new XWndResourceCtrl( XE::VEC2( 0, 0 ), xRES_GOLD, goldLack );
		pWndRoot->Add( pWnd );
		pWnd->AutoLayoutCenter();
		SetbUpdate( true );
	}
	m_needCash = ACCOUNT->GetCashFromGold( goldLack );
}

void XWndPaymentByCash::SetAP( int apLack )
{
	m_typePayment = xPR_AP;
	auto pWndRoot = Find( "wnd.half.top" );
	if( XASSERT( pWndRoot ) ) {
		auto pImg = XWndImage::sUpdateCtrl( pWndRoot, XE::VEC2(0,0), PATH_UI("icon_ap.png"), "__img.ap" );
		if( pImg ) {
			auto pText = XWndTextString::sUpdateCtrl( pImg, "__text.ap", XE::VEC2( 25,0), FONT_RESNUM, 18, true );
			if( pText ) {
				pText->SetText( XE::NtS(apLack) );
				pText->SetStyleStroke();
				pText->AutoLayoutVCenter();
			}
			pImg->AutoLayoutCenter();
		}
		SetbUpdate( true );
	}
	m_needCash = ACCOUNT->GetCashFromAP( apLack );
}

void XWndPaymentByCash::SetTime( xSec secLack )
{
	m_typePayment = xPR_TIME;
	auto pWndRoot = Find( "wnd.half.top" );
	if( XASSERT( pWndRoot ) ) {
		auto pImg = XWndImage::sUpdateCtrl( pWndRoot, XE::VEC2( 0, 0 ), PATH_UI( "icon_clock.png" ), "__img.clock" );
		if( pImg ) {
			auto pText = XWndTextString::sUpdateCtrl( pImg, "__text.sec", XE::VEC2( 25, 0 ), FONT_NANUM, 18, true );
			if( pText ) {
				pText->SetText( XGAME::GetstrResearchTime( secLack ) );
				pText->SetStyleStroke();
				pText->AutoLayoutVCenter();
			}
			pImg->AutoLayoutCenter();
		}
		SetbUpdate( true );
	}
	m_needCash = ACCOUNT->GetCashResearch( secLack );
}

/**
 @brief // 요일스팟 도전횟수 리필
*/
void XWndPaymentByCash::SetFillTryByDailySpot()		
{
	m_typePayment = xPR_TRY_DAILY;
 	auto pWndRoot = Find( "wnd.half.top" );
 	if( XASSERT( pWndRoot ) ) {
		auto pWnd = pWndRoot->Find( "wnd.challs" );
		if( pWnd == nullptr ) {
			pWnd = new XWnd();
			pWnd->SetstrIdentifier( "wnd.challs" );
			pWndRoot->Add( pWnd );
			XE::VEC2 vPos;
			for( int i = 0; i < XGlobalConst::sGet()->m_numEnterDaily; ++i ) {
				auto pImg = new XWndImage();
				pImg->SetPosLocal( vPos );
				pImg->SetSurfaceRes( PATH_UI("chall_mark_on.png") );
				vPos.x += pImg->GetSizeLocal().w + 1.f;
				pWnd->Add( pImg );
			}
			pWnd->AutoLayoutCenter();
		}
 	}
	m_needCash = XGlobalConst::sGet()->m_gemFillDailyTry;
}

////////////////////////////////////////////////////////////////
XWndOrderDialog::XWndOrderDialog( OrderPtr spOrder, float x, float y, ID idHero, XHero *pHero )
	: XWndView( x, y, _T("bg_dialog.png"))
{
	Init();
	m_spOrder = spOrder;
	m_idHero = idHero;
	m_pHero = pHero;
	//
// 	auto pImg = new XWndImage( m_strFaceRes, 8, 8 );
// 	pImg->SetstrIdentifier( "img.face" );
// 	Add( pImg );
	auto pText = new XWndTextString( XE::VEC2( 75,0 ), XE::VEC2( 203, 78 ), _T(""), FONT_NANUM, 22.f );
	pText->SetstrIdentifier( "text.dialog" );
	pText->SetAlign( XE::xALIGN_CENTER );
	pText->SetStyleShadow();
	Add( pText );
	const auto vSize = GetSizeLocal();
	XE::VEC2 vPos = GetPosLocal();
	if( vPos.x + vSize.w >= XE::GetGameWidth() ) {
		vPos.x = (XE::GetGameWidth() - vSize.w);
	} else
	if( vPos.x < 0 ) {
		vPos.x = 0;
	}
	if( vPos.y + vSize.w >= XE::GetGameHeight() ) {
		vPos.y = (XE::GetGameHeight() - vSize.h);
	} else
	if( vPos.y < 0 ) {
		vPos.y = 0;
	}
	SetPosLocal( vPos );
	auto pWndSpr = new XWndSprObj( _T( "ui_touch.spr" ), 1, 276.f, 69.f );
	pWndSpr->SetstrIdentifier( "spr.touch" );
	Add( pWndSpr );
	pWndSpr->SetbShow( FALSE );
	SetEnableNcEvent( TRUE );
	//
	SetbUpdate( true );
}
void XWndOrderDialog::Update()
{
	auto vSize = GetSizeLocal();
	if( m_idHero ) {
		auto pWndPortrait = SafeCast2<XWndStoragyItemElem*>( Find("wnd.portrait") );
		if( pWndPortrait == nullptr ) {
			if( m_pHero ) {
				pWndPortrait = new XWndStoragyItemElem( XE::VEC2( 8 ), m_pHero );
			} else {
				XGAME::xReward reward;
				reward.SetHero( m_idHero, 0 );
				pWndPortrait = new XWndStoragyItemElem( XE::VEC2( 8 ), reward );
			}
			pWndPortrait->SetstrIdentifier( "wnd.portrait" );
			Add( pWndPortrait );
			pWndPortrait->AutoLayoutVCenter();
		}
	}
// 	xSET_IMG( this, "img.face", m_strFaceRes );
	xSET_TEXT( this, "text.dialog", m_strText );
	if( m_bTouch )
		xSET_SHOW( this, "spr.touch", true );
	else
		xSET_SHOW( this, "spr.touch", false );
	XWndView::Update();
}
int XWndOrderDialog::OnClose( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClose");
	//
	m_spOrder->SetForceClose();	// 강제종료
	SetbDestroy( TRUE );
	return 1;
}
/**
 @brief 창의 외부영역을 터치
 XWndOrderDialog가 닫히는 메커니즘.
 어떤 컨트롤을 클릭하면 XGame::OnClickWnd가 호출되어 "어떤것을 눌림" 이벤트가 날아오고 
 다시 각 Order들에게 OnClickWnd메시지를 보낸다.
 XOrderDialog는 자신이 종료하는 타입에 맞춰서 종료되며 Order측에서 창을 정리한다.
*/
void XWndOrderDialog::OnNCLButtonUp( float lx, float ly )
{
	// this창 영역을 클릭한것처럼 시뮬
	m_spOrder->OnClickWnd( GetstrIdentifier() );
// 	XWndView::OnNCLButtonUp( lx, ly );
}


////////////////////////////////////////////////////////////////
XWndDialogMode::XWndDialogMode()
{
	Init();
	DoCovering();
}

int XWndDialogMode::Process( float dt )
{
	// 가려지고 있음.
	if( m_Mode == 1 ) {
		float lerp = m_Timer.GetSlerp();
		if( lerp > 1.f )
			lerp = 1.f;
		if( lerp == 1.f )
			m_Mode = 2;

		m_HeightCurr = m_Height * lerp;
	} else
	if( m_Mode == 3 ) {
		float lerp = m_Timer.GetSlerp();
		if( lerp >= 1.f ) {
			lerp = 1.f;
			m_Mode = 0;
			SetbDestroy( TRUE );
		}

		m_HeightCurr = m_Height * (1.f - lerp);
	}
	return 1;
}

void XWndDialogMode::Draw()
{
	XE::VEC2 vSize;
	vSize.w = XE::GetGameWidth();
	vSize.h = m_HeightCurr;
	XE::VEC2 vPos(0);
	GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_BLACK );
	vPos.y = XE::GetGameHeight() - m_HeightCurr;
	GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_BLACK );
}

////////////////////////////////////////////////////////////////
XWndPopupCashPay::XWndPopupCashPay()
	: XWndPopup( _T( "layout_need_ap.xml" ), "popup_cash_pay" )
{
	Init();
}

void XWndPopupCashPay::SetTitle( LPCTSTR szTitle )
{
	xSET_TEXT( this, "text.title", szTitle );
}
void XWndPopupCashPay::SetNeedText( LPCTSTR szDesc )
{
	xSET_TEXT( this, "text.need", szDesc );
}
void XWndPopupCashPay::SetCost( int cash )
{
	xSET_TEXT( this, "text.cost", XE::NumberToMoneyString(cash) );
}

XWnd* XWndPopupCashPay::GetButtOk()
{
	return Find("butt.use.cash");
}

////////////////////////////////////////////////////////////////
XWndHeroPortrait::XWndHeroPortrait( const XE::VEC2& vPos, XPropHero::xPROP *pProp )
	: XWnd( vPos )
{
	Init();
	m_pProp = pProp;
	XBREAK( m_pProp == nullptr );
	XLayoutObj::sCreateLayout( _T( "hero_portrait.xml" ), "portrait", this );
	auto sizeLocalThis = GetSizeNoTransLayout();
	SetSizeLocal( sizeLocalThis );
	SetbUpdate( true );
}

void XWndHeroPortrait::Update()
{
	auto vScale = GetScaleLocal();
	// 영웅 이름
	auto pText = xSET_TEXT( this, "text.name", m_pProp->GetstrName() );
// 	if( pText )
// 		pText->SetColorText( XGAME::GetGradeColor( m_pProp->GetGrade() ) );
	// 영웅 얼굴
	xSET_IMG( this, "img.hero", XE::MakePath( DIR_IMG, m_pProp->strFace ), XE::xPF_ARGB8888 );
	XWnd::Update();
}
////////////////////////////////////////////////////////////////
XWndAbilTreeDebug::XWndAbilTreeDebug( XGAME::xtUnit unit )
	: XWndPopup( _T("abil_tree.xml"), "popup_tree" )
{
	Init();
//	m_pAcc = pAcc;
	m_Unit = unit;
}

void XWndAbilTreeDebug::Update()
{
	XArrayLinearN<XPropTech::xNodeAbil*, 1024> aryAbil;
	XPropTech::sGet()->GetNodesToAry( m_Unit, &aryAbil );
	const auto& research = ACCOUNT->GetResearching();
	auto pHero = ACCOUNT->GetHero( research.GetsnHero() );
	if( pHero ) {
		_tstring strMsg;
		XARRAYLINEARN_LOOP_AUTO( aryAbil, pNode ) {
			const auto abil = pHero->GetAbilNode( m_Unit, pNode->idNode );
			if( abil.point > 0 ) {
	// 			auto pSkillDat = SKILL_MNG->FindByIdentifier( pNode->strSkill );
	// 			if( pSkillDat ) {
					strMsg += XFORMAT( "%s:%d\n", XTEXT( pNode->idName ), abil.point );
	// 			}
			}
		} END_LOOP;
		xSET_TEXT( this, "text.abil", strMsg );
	}
	//
	XWndPopup::Update();
}

////////////////////////////////////////////////////////////////
/**
 @brief 영웅을 선택하는 창을 생성한다.
 @param pHeroFrom 바꿔야할 영웅.
*/
XWndSelectHeroesInReady::XWndSelectHeroesInReady( XHero *pHeroFrom )
	: XWndPopup( _T( "layout_ready_battle.xml" ), "popup_select_hero" )
{
	Init();
	m_pHeroFrom = pHeroFrom;
	m_pHeroSelected = pHeroFrom;
	SetPosLocal( 225, 21 );
	auto pWndList = xGET_LIST_CTRL( this, "list.heroes" );
	if( pWndList )
		pWndList->SetEvent( XWM_SELECT_ELEM, this, &XWndSelectHeroesInReady::OnClickHero );
	SetbModal( FALSE );
}

void XWndSelectHeroesInReady::Destroy()
{
	GetpParent()->DestroyWndByIdentifier("wnd.units");
}

BOOL XWndSelectHeroesInReady::OnCreate()
{
	// 유닛선택창 바로 보여줌.
	CreatePopupSelectUnit( m_pHeroSelected );
	return XWndPopup::OnCreate();
}

void XWndSelectHeroesInReady::Update()
{
	const ID snHeroSelected = (m_pHeroSelected)? m_pHeroSelected->GetsnHero() : 0;
	const ID snHeroFrom = (m_pHeroFrom)? m_pHeroFrom->GetsnHero() : 0;
	// 모든 영웅의 리스트를 받아옴
	XList4<XHero*> listHero;
	ACCOUNT->_GetInvenHero( listHero );
	// 
//	listHero.sort( XSceneUnitOrg::CompParty );
	auto pWndList = xGET_LIST_CTRL( this, "list.heroes" );
	if( pWndList ) {
		// 기존 영웅을 먼저 표시
		if( m_pHeroFrom ) {
//			auto pWndElem = XWndInvenHeroElem::sUpdateCtrl( pWndList, m_pHeroFrom );
			auto pWndElem = XWndStoragyItemElem::sUpdateCtrlToList( pWndList, m_pHeroFrom );
			if( pWndElem ) {
				if( m_pHeroFrom && m_pHeroSelected && snHeroFrom == snHeroSelected )
						pWndElem->SetbSelected( true );
					else
						pWndElem->SetbSelected( false );
			}
		}
		auto spLegion = ACCOUNT->GetCurrLegion();
		// 그 외 영웅을 표시
		for( auto pHero : listHero ) {
			ID snHero = pHero->GetsnHero();
			// 출전중인 영웅도 아니고 // 바꿔야 할 이전 영웅도 아니면 생성
			if( !spLegion->GetpHeroBySN( snHero )	&& snHero != snHeroFrom )	{						
//				auto pWndElem = XWndInvenHeroElem::sUpdateCtrl( pWndList, pHero );
				auto pWndElem = XWndStoragyItemElem::sUpdateCtrlToList( pWndList, pHero );
				if( pWndElem ) {
					pWndElem->SetbSelected( pHero->GetsnHero() == snHeroSelected );
				}
			}
		}
	} // pWndList
	//
	XWndPopup::Update();
}

/**
 @brief 
*/
int XWndSelectHeroesInReady::OnClickHero( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickHero");
	//
	ID snHero = p2;
	auto pHero = ACCOUNT->GetpHeroBySN( snHero );
	if( pHero ) {
		m_pHeroSelected = pHero;
		// 일단은 선택하는 즉시 바뀌는걸로 해봄.
		auto spLegion = ACCOUNT->GetCurrLegion();
		if( m_pHeroFrom ) {
			bool bOk = spLegion->ChangeHeroInSquad( m_pHeroFrom, pHero );
			XBREAK( bOk == false );
			m_pHeroFrom = pHero;
			// 유닛 선택창 띄움
			CreatePopupSelectUnit( pHero );
		}
	} else {
		m_pHeroSelected = nullptr;
	}
	//
	SetbUpdate( true );
	return 1;
}
/**
 @brief From영웅을 교체하고 다시 업데이트한다.
*/
void XWndSelectHeroesInReady::SetHeroFrom( XHero* pHero )
{
	m_pHeroFrom = pHero;
	m_pHeroSelected = pHero;
	SetbUpdate( true );
}

void XWndSelectHeroesInReady::CreatePopupSelectUnit( XHero *pHero )
{
	XBREAK( GetpParent() == nullptr );
	GetpParent()->DestroyWndByIdentifier( "wnd.units" );
	auto pWndUnit = new XWndSelectUnitsInReady( pHero );
	pWndUnit->SetPosLocal( GetPosLocal().x + GetSizeLocal().w, GetPosLocal().y );
	pWndUnit->SetstrIdentifier( "wnd.units" );
	GetpParent()->Add( pWndUnit );
	GAME->GetpScene()->SetbUpdate( true );
}

////////////////////////////////////////////////////////////////
XWndSelectUnitsInReady::XWndSelectUnitsInReady( XHero *pHero )
	: XWndPopup( _T( "layout_ready_battle.xml" ), "popup_select_unit" )
{
	Init();
	m_pHero = pHero;
	SetPosLocal( 222, 21 );
	SetbModal( FALSE );
}

void XWndSelectUnitsInReady::Update()
{
	if( m_pHero ) {
		auto typeAtk = m_pHero->GetTypeAtk();
		for( int i = 0; i < 3; ++i ) {
			auto unitIdx = XGAME::GetUnitBySizeAndAtkType( (XGAME::xtSize)(i+1), typeAtk );
			_tstring resUnit = XGAME::GetResUnitSmall( unitIdx );
			auto pImg = xSET_IMGF( this, resUnit.c_str(), "img.unit%d", i+1 );
			if( pImg ) {
				if( ACCOUNT->IsLockUnit( unitIdx ) ) {
					pImg->SetbEnable( false );
					pImg->SetblendFunc( XE::xBF_GRAY );
					xSET_SHOW( pImg, "text.point", false );
				} else {
					pImg->SetbEnable( true );
					pImg->SetblendFunc( XE::xBF_MULTIPLY );
					auto numPoint = m_pHero->GetNumSetAbilPointByUnit( unitIdx );
					auto pText = xSET_TEXT_FORMAT( pImg, "text.point", _T("%d"), numPoint );
					if( pText )
						pText->SetbShow( true );
				}
				pImg->SetEvent( XWM_CLICKED, this, &XWndSelectUnitsInReady::OnClickUnit, unitIdx );
				bool bShow = (m_pHero->GetUnit() == unitIdx);
				xSET_SHOW( pImg, "img.glow", bShow );
			}
		}
	}
	XWndPopup::Update();
}

/**
 @brief 
*/
int XWndSelectUnitsInReady::OnClickUnit( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickUnit");
	//
	auto unit = (XGAME::xtUnit)p1;
	m_pHero->SetUnit( unit );
//	SetbUpdate( true );
	GAME->GetpScene()->SetbUpdate( true );
	return 1;
}

////////////////////////////////////////////////////////////////
/**
 @brief 앱 최초실행시 계정이 있는지 묻는 창.
*/
XWndLoginFirst::XWndLoginFirst( XSceneTitle *pScene )
	: XWndPopup( _T( "layout_title.xml" ), "popup_first" )
{
	Init();
	XBREAK( pScene == nullptr );
	m_pScene = pScene;
	xSetButtHander( this, pScene, "butt.no", &XSceneTitle::OnNoHaveNoAccount );
	xSetButtHander( this, pScene, "butt.yes", &XSceneTitle::OnYesHaveAccount );
	xSetButtHander( this, pScene, "butt.facebook", &XSceneTitle::OnClickLoginFacebook );
}

void XWndLoginFirst::Update()
{
#if defined(_CHEAT)
	if( XAPP->m_bDebugMode ) {
		auto pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.ipchange", XE::VEC2( 170, 0 ), XE::VEC2( 64, 32 ), _T( "ip교체" ), true );
		pButt->SetEvent( XWM_CLICKED, m_pScene, &XSceneTitle::OnClickIpChange );
	}
#endif
	XWndPopup::Update();
}



//////////////////////////////////////////////////////////////////////////
/**
 @brief 
 @param dAngPlace this객체가 idsSrc를 따라다닐때 idsSrc객체를 중심으로 dAngPlace방향으로 떨어져 위치한다. 거리는 idsSrc객체의 크기로 한다.
*/
XWndSprTrace::XWndSprTrace( const _tstring& strSpr
													, ID idAct
													, const std::string& idsSrc
													, float dAngPlace )
	: XWndSprObj( strSpr, idAct, XE::VEC2(0) )
	, m_idsSrc( idsSrc ) 
	, m_dAngPlace( dAngPlace )
{
	Init();
	XBREAK( idsSrc.empty() );		// idsSrc 가 없으면 다른 생성자를 쓸것.
	auto pWndSrc = GAME->Find( m_idsSrc );
	if( pWndSrc ) {
		const auto vSize = pWndSrc->GetSizeFinal() * 0.5f;
		m_radiusSrc = vSize.Length();
	}
}
/**
 @brief idsSrc없이 좌표를 직접 갖고 있는 타입
*/
XWndSprTrace::XWndSprTrace( const _tstring& strSpr
													, ID idAct
													, const XE::VEC2& vPos
													, float dAngPlace
													, const XE::VEC2& sizeTarget )
	: XWndSprObj( strSpr, idAct, vPos )
	, m_vPosOrig( vPos )
	, m_dAngPlace( dAngPlace )
	, m_sizeTarget( sizeTarget )
{
	Init();
	if( !m_sizeTarget.IsZero() ) {
		m_radiusSrc = (m_sizeTarget * 0.5f).Length();
	}
}
int XWndSprTrace::Process( float dt )
{
	XE::VEC2 vPos;
	if( !m_idsSrc.empty() ) {
		// 타겟의 중앙을 중심으로 타겟크기의 외곽에 위치시킨다.
		auto pWndSrc = GetWndTop()->Find( m_idsSrc );
		if( pWndSrc ) {
			const auto vPosScr = pWndSrc->GetPosScreen();
			const auto sizeTarget = pWndSrc->GetSizeFinal();
			const auto vCenter = vPosScr + sizeTarget * 0.5f;		// 타겟의 중심좌표
			m_radiusSrc = (sizeTarget * 0.5f).Length();
			vPos = vPosScr;
			if( m_dAngPlace == -2.f ) {
				// 그냥 객체의 원점.(사각 윈도우의 경우좌상귀, 스팟의 경우 중심)
				vPos = vPosScr;
			} else
			if( m_dAngPlace == -1.f ) {
				vPos = vCenter;
			} else
			if( m_dAngPlace == 0 ) {
				// 타겟의 오른쪽에 위치시킴
				vPos.x += sizeTarget.w;
				vPos.y += sizeTarget.h * 0.5f;
			} else
			if( m_dAngPlace == 90 ) {
				// 타겟의 아래에 위치시킴
				vPos.x += sizeTarget.w * 0.5f;
				vPos.y += sizeTarget.h;
			} else
			if( m_dAngPlace == 180 ) {
				// 타겟의 왼쪽에 위치시킴
				vPos.x += 0;
				vPos.y += sizeTarget.h * 0.5f;
			} else
			if( m_dAngPlace == 270 ) {
				// 타겟의 위에 위치시킴
				vPos.x += sizeTarget.w * 0.5f;
				vPos.y += 0;
			} else {
				// 그 외 각도
				if( m_radiusSrc == 0.f ) {
					m_radiusSrc = (sizeTarget * 0.5f).Length();
				}
				vPos = vCenter + XE::GetAngleVector( m_dAngPlace, m_radiusSrc );
			}
		} else {
			CONSOLE("destroyed XWndSprTrace: idsSrc=%s", C2SZ(m_idsSrc) );
			SetbDestroy( true );
			return 1;
		}
	} else {// if( !m_idsSrc.empty() ) {
		// // m_idsSrc is null
		if( m_dAngPlace == -2.f ) {
			// 원점
			vPos = m_vPosOrig;
		} else
		if( m_dAngPlace == -1.f ) {
			// center
			vPos = m_vPosOrig + m_sizeTarget * 0.5f;
		} else {
			// 지시한 방향과 거리로
			vPos = m_vPosOrig + XE::GetAngleVector( m_dAngPlace, m_radiusSrc );
		}
	}
	XBREAK( vPos.IsZero() );
	if( vPos.x < 0 )
		vPos.x = 0;
	if( vPos.y < 0 )
		vPos.y = 0;
	if( vPos.x > XE::GetGameWidth() )
		vPos.x = XE::GetGameWidth();
	if( vPos.y > XE::GetGameHeight() )
		vPos.y = XE::GetGameHeight();
	SetPosLocal( vPos );
	return XWndSprObj::Process( dt );
}


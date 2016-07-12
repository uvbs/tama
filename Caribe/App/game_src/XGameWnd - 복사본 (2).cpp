#include "stdafx.h"
#include "XGameWnd.h"
#include "XSoundMng.h"
#include "client/XAppMain.h"
#include "XGame.h"
#include "XFontMng.h"
#include "XFramework/XConstant.h"
#include "XWindow.h"
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
#include "XPropBgObj.h"
#include "XWndTemplate.h"
#include "XWorld.h"
#include "XFramework/XSoundTable.h"
#include "XWndResCtrl.h"
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
XWndSkillButton::XWndSkillButton( float x, float y, 
								XSKILL::XSkillDat *pSkillDat, 
								XUnitHero *pUnitHero )
	: XWnd( x, y ), XWndButton( x, y, nullptr, nullptr )
{
	Init();
	XBREAK( pSkillDat == NULL );
	SetpSurface( 0, PATH_UI("common_bg_skillsimbol.png") );
	XWndImage *pImg = new XWndImage( XE::MakePath( DIR_IMG, pSkillDat->GetstrIcon().c_str() ),
									1.f, 1.f );
	Add( pImg );
	if( pUnitHero ) {
		m_timerCool = pUnitHero->GettimerCool();
		m_snHero = pUnitHero->GetHero()->GetsnHero();
	}
// 	if( ( m_timerCool.IsOver() || m_timerCool.IsOff() ) )
// 		SetbActive( TRUE );
// 	else
// 		SetbActive( FALSE );
}

void XWndSkillButton::Destroy()
{
}

/**
 @brief 쿨타이머를 스타트 시킨다.
*/
void XWndSkillButton::SetCoolTimer( float sec )
{
	m_timerCool.Set( sec );
	// 쿨타이머가 시작되면 버튼눌림을 막음
//	SetbActive( FALSE );
}

int XWndSkillButton::Process( float dt )
{
	// 디액티브 상태에서 쿨타이머가 끝났으면 다시 활성화 시킴
// 	if( (m_timerCool.IsOver() || m_timerCool.IsOff()) )
// 	{
// 		SetbActive( TRUE );
// 	} else
// 		SetbActive( FALSE );
	return XWndButton::Process( dt );
}

void XWndSkillButton::Draw( void )
{
	bool bCool = false;
	///< 쿨타이머가 돌고있는동안 타이머 상태를 표시.
	if( m_timerCool.IsOn() && m_timerCool.IsOver() == FALSE ) {
		bCool = true;
// 		SetAlphaLocal( 0.5f );
		SetAlphaLocal( 1.f );
	} else
		SetAlphaLocal( 1.f );
	XWndButton::Draw();
	///< 쿨타이머가 돌고있는동안 타이머 상태를 표시.
	if( bCool ) {
		XE::VEC2 v = GetPosFinal();
		XCOLOR col = XCOLOR_RGBA( 0, 0, 0, 128 );
		float a = (1.f - m_timerCool.GetSlerp()) * -360;
		GRAPHICS->DrawPie( v + 30, 30.f, 0, a, col );
		v += XE::VEC2(26,43);
		PUT_STRINGF( v.x, v.y, XCOLOR_WHITE, "%d", (int)m_timerCool.GetRemainSec() );
	}
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
XWndCampaignBase::XWndCampaignBase( XSpot *pBaseSpot, CampObjPtr& spCampObj )
	: XWndPopup( _T( "campaign.xml" ), "popup" )
{
	Init();
	m_pBaseSpot = pBaseSpot;
//	m_spCampObj = spCampObj;
	m_pProp = spCampObj->GetpProp();
	GetpLayout()->CreateLayout( "campaign_default", this );
	auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
	pWndList->XWndList::SetpDelegate( this );
	SetbModal( TRUE );
	SetbUpdate( true );
}

xCampaign::CampObjPtr XWndCampaignBase::GetspCampObj() 
{
	return m_pBaseSpot->GetspCampObj();
}

void XWndCampaignBase::Update()
{
	_tstring strName;
	strName += XTEXT( m_pProp->idName );
	auto spCampObj = GetspCampObj();
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		auto pText = xSET_TEXT( this, "text.debug.num.star", XFORMAT("star=%d", spCampObj->GetnumStar()) );
		if( pText )
			pText->SetbShow( true );
		strName += XFORMAT( "(%s)", m_pProp->strIdentifier.c_str() );
	}
#endif // cheat
	xSET_TEXT( this, "text.title", strName );
	// 도전횟수 마크
	if( m_pProp->numTry > 0 ) {
		xSET_SHOW( this, "text.rest.challenge", true );
		int numRemain = m_pProp->numTry - spCampObj->GetcntTry();
		for( int i = 0; i < m_pProp->numTry; ++i ) {
			auto pMark = xGET_IMAGE_CTRLF( this, "img.mark.%d", i + 1 );
			if( pMark ) {
				pMark->SetbShow( TRUE );
				if( i < numRemain )
					pMark->SetSurfaceRes( PATH_UI( "chall_mark_on.png" ) );
			}
		}
	} else {
		xSET_SHOW( this, "text.rest.challenge", false );
	}

// 	if( m_pProp->numTry > 0 ) {
// 		int numRemain = m_pProp->numTry - m_spCampObj->GetcntTry();
// 		_tstring str = XE::Format(XTEXT(80192), numRemain, m_pProp->numTry );
// 		auto pText = xSET_TEXT( this, "text.num.try", str );
// 		if( pText ) 
// 			pText->SetbShow( TRUE );
// 	} else
// 		xSET_SHOW( this, "text.num.try", false );
	auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
	//if( 1 || !m_bProgressLoading ) 
	{
//		auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
		pWndList->DestroyAllItem();
		if( XASSERT( pWndList ) ) {
			XBREAK( GetpLayout() == nullptr );
			//
 			for( auto spStageObj : spCampObj->GetaryStages() ) {
				XWnd *pElem = new XWnd( XE::VEC2(0), XE::VEC2(67,134) );
				if( !m_bProgressLoading ) {
					// 단계별 로딩모드에선 지금 생성하지 않는다.
					pElem = UpdateListElem( pElem, spStageObj );
				}
//				auto pElem = UpdateListElem( spStageObj );
				if( pElem ) {
					auto spPropStage = spStageObj->GetspPropStage();
					pWndList->AddItem( spPropStage->idProp, pElem );
				}
			}
		}
		// 리스트 선택 이벤트 핸들러를 등록요청한다.
		SetEventByList( pWndList );   // virtual
	}
	// 리셋주기가 있으면 남은시간을 위해 자동업데이트를 켠다.
	if( m_pProp->secResetCycle ) {
		SetAutoUpdate( 0.1f );
		xSET_SHOW( this, "text.remain.reset", true );
	} else {
		ClearAutoUpdate();
		xSET_SHOW( this, "text.remain.reset", false );
	}
	//
	XWndPopup::Update();
}

void XWndCampaignBase::OnAutoUpdate( )
{
	if( m_pProp->secResetCycle ) {
		auto spCampObj = GetspCampObj();
		auto secRemain = spCampObj->GetsecRemainReset();
		if( secRemain > 0 ) {
			_tstring strRemain = XFORMAT("%s:", XTEXT(80131) );	// 남은 시간
			strRemain += XSceneTech::sGetResearchTime( secRemain );
			xSET_TEXT( this, "text.remain.reset", strRemain );
		}
	}
}

void XWndCampaignBase::DelegateStopScroll( XWndList* pList
																				, const XE::VEC2& vCurrLT
																				, const XE::VEC2& sizeScrollWindow )
{
	if( m_bProgressLoading ) {
		auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
		if( pWndList == nullptr )
			return;
		for( auto spStageObj : GetspCampObj()->GetaryStages() ) {
			auto pElem =  Find( spStageObj->GetidProp() );
			if( pElem ) {
				auto vLT = pElem->GetPosLocal();
				auto vRB = vLT + pElem->GetSizeLocal();
				if( XE::IsOverlapWithRect( vCurrLT, vCurrLT + sizeScrollWindow
																	, vLT, vRB ) ) {
 					UpdateElemByStopScroll( pElem, spStageObj );
					CONSOLE("UpdateElem:idxStg=%d", spStageObj->GetidxStage() );

				}
			}
		}
	}
}

void XWndCampaignHero::UpdateElemByStopScroll( XWnd *pElem, StageObjPtr spStageObj )
{
	UpdateListElem( pElem, spStageObj );
}

int XWndCampaignBase::Process( float dt )
{
// 	auto spCampObj = GetspCampObj();
// 	if( m_bProgressLoading && spCampObj ) {
// 		if( m_idxLoading < spCampObj->GetNumStages() ) {
// 			auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
// 			if( XASSERT( pWndList ) ) {
// 				auto spStageObj = spCampObj->GetspStage( m_idxLoading );
// 				if( spStageObj ) {
// 					auto spPropStage = spStageObj->GetspPropStage();
// 					pWndList->DelItem( spPropStage->idProp );
// 					auto pElem = UpdateListElem( spStageObj );
// 					CONSOLE("loaded stage layout:%d", m_idxLoading );
// 					if( pElem ) {
// 						pWndList->AddItem( spPropStage->idProp, pElem );
// 					}
// 				}
// 			}
// 			++m_idxLoading;
// 		}
// 	}
	return XWndPopup::Process( dt );
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndCampaignBase::OnOk( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnOk");
	//
	SetbDestroy( TRUE );
	return 1;
}

int XWndCampaignBase::OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickStage:%d", p1);
	int idxStage = (int)p1;
	if( ACCOUNT->GetAP() < ACCOUNT->GetAPPerBattle() ) {
		XWND_ALERT("%s", XTEXT(2240));
		return 1;
	}
	// 어느스팟의 몇번째 스테이지를 선택했는지 서버로 보낸다.
	auto spCampObj = GetspCampObj();
	auto spStageObj = spCampObj->GetspStage( idxStage );
	if( XBREAK( spStageObj == nullptr ) )
		return 1;
	auto spPropStage = spStageObj->GetspPropStage();
	GAMESVR_SOCKET->SendReqClickStageInCampaign( GAME, 
												m_pBaseSpot->GetidSpot(),
												m_pBaseSpot->GetsnSpot(),
												spPropStage->idxStage,
												spPropStage->idProp,
												getid() );
	return 1;
}

void XWndCampaignBase::OnRecvClickStage( XGAME::xtError errCode, int idxStage )
{
	auto spCampObj = GetspCampObj();
	switch( errCode )
	{
	case XGAME::xE_NOT_ENOUGH_LEVEL:
		XWND_ALERT( "%s", XTEXT( 2089 ) );
		break;
	case XGAME::xE_NOT_ENOUGH_AP:
		XWND_ALERT( "%s", _T( "Not enough AP" ) );
		break;
	case XGAME::xE_NOT_ENOUGH_NUM_TRY:
		XWND_ALERT( "%s", XTEXT( 2155 ) );
		break;
	case XGAME::xE_LOCK:
		XWND_ALERT( "%s", XTEXT( 2157 ) );	// 잠겨있습니다.
		break;
	case XGAME::xE_NOT_ENOUGH_NUM_CLEAR:
		XWND_ALERT( "%s", XTEXT( 2156 ) );	// 그외 모든 에러(더이상 도전할수 엄슴)
		break;
	case XGAME::xE_OK: {
		auto spStageObj = spCampObj->GetspStage( idxStage );
		if( XBREAK(spStageObj == nullptr) )
			return;
		// 전투전 팝업을 띄움.
		m_pBaseSpot->SetLevel( spStageObj->GetLevelLegion() );
		auto pPopup = new XWndPopupSpotMenu( m_pBaseSpot, idxStage );
		pPopup->AddMenuAttack();			// 공격버튼
		pPopup->SetPower( spStageObj->GetPower() );
		pPopup->SetLevel( spStageObj->GetLevelLegion() );
		_tstring strName = spCampObj->GetNameCamp();
		if( strName.empty() )
			strName = m_pBaseSpot->GetszName();
		pPopup->SetstrName( strName );
		pPopup->SetItems( spStageObj->GetaryDrops() );
		GAME->GetpScene()->Add( pPopup );
	} break;
	default:
		XWND_ALERT( "%s", XTEXT( 2156 ) );	// 그외 모든 에러(더이상 도전할수 엄슴)
		break;
	}
}

////////////////////////////////////////////////////////////////
XWndPopupCampaign::XWndPopupCampaign( XSpot *pBaseSpot, xCampaign::CampObjPtr spCampObj )
	: XWndCampaignBase( pBaseSpot, spCampObj )
{
	Init();
	SetstrIdentifier( "popup.campaign" );
	auto pProp = spCampObj->GetpProp();
	if( !pProp->m_strcLayout.empty() ) {
		// 커스텀 레이아웃
		if( !pProp->m_strtXml.empty() ) {
			XLayoutObj layoutObj( pProp->m_strtXml.c_str() );
			layoutObj.CreateLayout( pProp->m_strcLayout, this );
		} else {
			GetpLayout()->CreateLayout( pProp->m_strcLayout.c_str(), this );
		}
	}
}

XWndCampaignBase::xtStageState 
XWndPopupCampaign::GetStageState( StageObjPtr spStage )
{
	// 일반 캠페인은 3별클리어 하면 클리어상태가 됨.
	if( spStage->GetnumStar() >= 3 )
		return xSS_CLEARED;
	// 언락한 스테이지 인덱스보다 작으면 진입가능
	int idxStage = spStage->GetidxStage();
	auto spCampObj = GetspCampObj();
	if( idxStage <= spCampObj->GetidxLastUnlock() )
		return xSS_ENTERABLE;
	// 마지막 스테이지는 보스 스테이지
	if( idxStage == spCampObj->GetNumStages() - 1 )
		return xSS_BOSS;
	return xSS_LOCK;
}

XWnd* XWndPopupCampaign::UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj )
{
	auto spCampObj = GetspCampObj();
	auto spPropStage = spStageObj->GetspPropStage();
	XBREAK( GetpLayout() == nullptr );
//	XWnd *pElem = new XWnd;
	// 범용 레이아웃을 사용.
	GetpLayout()->CreateLayout( "elem_campaign", pElem );
// 	auto spStageObj = m_spCampObj->GetspStage( spPropStage->idxStage );
	if( XBREAK( spStageObj == nullptr ) )
		return nullptr;
	int numStage = spCampObj->GetNumStages();
	int i = spPropStage->idxStage;
	XBREAK( i < 0 );
	_tstring strImg;
	bool bLock = false;
	bool bComplete = false;
	bool bBoss = false;
	// 각 스테이지 버튼 그림
	auto pButt = xGET_BUTT_CTRL( pElem, "img.bg.elem" );
	if( pButt == nullptr )
		return nullptr;
	auto state = GetStageState( spStageObj );
	switch( state )
	{
	case XWndCampaignBase::xSS_ENTERABLE:
		strImg = _T( "stage_elem.png" );
		break;
	case XWndCampaignBase::xSS_CLEARED:
		strImg = _T( "stage_clear.png" );
		xSET_SHOW( pElem, "text.complete", true );
		pButt->SetbActive( false );
		break;
	case XWndCampaignBase::xSS_LOCK:
		strImg = _T( "stage_elem_lock.png" );
		bLock = true;
		break;
	case XWndCampaignBase::xSS_BOSS:
		strImg = _T( "stage_elem_boss.png" );
		bBoss = true;
		break;
	default:
		XBREAK(1);
		break;
	}
// 	if( m_spCampObj->IsActiveStage( spStageObj ) )
// 		strImg = _T( "stage_elem.png" );
// 	else if( spStageObj->GetnumStar() >= 3 ) {
// 		strImg = _T( "stage_clear.png" );
// 		xSET_SHOW(pElem, "text.complete", true );
// 	} else if( i == numStage - 1 ) {
// 		strImg = _T( "stage_elem_boss.png" );
// 		bBoss = true;
// 	} else {
// 		strImg = _T( "stage_elem_lock.png" );
// 		bLock = true;
// 	}
	pButt->SetpSurface( 0, XE::MakePath( DIR_UI, strImg ) );
	// 군단 레벨
	{
		int lvLegion = spPropStage->legion.lvLegion;
		if( lvLegion == 0 )
			lvLegion = ACCOUNT->GetLevel() + spPropStage->legion.adjLvLegion;
		auto pText = xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", lvLegion ) );
		if( pText ) {
			XCOLOR col = XCOLOR_WHITE;
			if( spStageObj->GetPower() )
				col = XGAME::GetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
			pText->SetColorText( col );
		}
	}
	// 스테이지당 도전횟수 별표시
// 	if( !(bLock || bBoss) ) {	// 잠겨있는 스테이지는 표시안함.
// 		// 여러번 도전가능한 스테이지의 경우.
// 		int numClear = spStageObj->GetnumClear();
// 		for( int i = 0; i < spPropStage->maxTry; ++i ) {
// 			auto pMark = xGET_IMAGE_CTRLF( pElem, "img.clear.%d", i + 1 );
// 			if( pMark ) {
// 				pMark->SetbShow( TRUE );
// 				if( i < numClear )
// 					pMark->SetSurfaceRes( PATH_UI( "star_on.png" ) );
// 			}
// 		}
// 	}
// 	else
// 		pText->SetbShow( FALSE );
	if( IsShowStar() && spStageObj->GetnumStar() > 0 ) {
		// 별점을 획득한 스테이지만 별표시
		int numStar = spStageObj->GetnumStar();
		XWnd *pParent = nullptr;
		XArrayLinearN<XWnd*,256> ary;
		for( int i = 0; i < 3; ++i ) {
			auto pMark = xGET_IMAGE_CTRLF( pElem, "img.clear.%d", i + 1 );
			if( pMark ) {
				pParent = pMark->GetpParent();
				pMark->SetbShow( true );
				if( i < numStar )
					pMark->SetSurfaceRes( PATH_UI( "star_on.png" ) );
				ary.Add( pMark );
			}
		}
		pParent->AutoLayoutHCenterWithAry( ary, 15.f );
	}
// 	else
// 		pText->SetbShow( FALSE );
	pButt->SetEvent( XWM_CLICKED, this, &XWndPopupCampaign::OnClickStage, i );
	pElem->SetAutoSize();
	pElem->SetstrIdentifierf( "stage.%d", i );
	// 스테이지별 별점보상이 있으면 버튼을 보여준다.
	if( spPropStage->aryReward.size() ) {
		int numNeed = 3 + i * 3;
		if( spStageObj->GetbRecvReward() ) {
			// 이미 보상받았으면 빈칸으로 표시
			// 버튼 표시안함.
			xSET_SHOW( pElem, "butt.reward", false );
		} else {
			// 아직 보상안받았고
			// 별점이 충분하면 버튼 활성화
			xSET_SHOW( pElem, "butt.reward", true );
			if( spCampObj->GetnumStar() >= numNeed ) {
				xSET_ENABLE( pElem, "butt.reward", true );
			} else {
				// 별점 모자르면 비활성화
				xSET_ENABLE( pElem, "butt.reward", false );
			}
		}
		SetButtHander( pElem, "butt.reward", &XWndPopupCampaign::OnClickReward, i );
		xSET_SHOW( pElem, "img.star.num", true );
		xSET_TEXT( pElem, "text.num.star", XFORMAT("%d", 3 + i * 3) );
		XArrayLinearN<XWnd*, 256> aryIcons;
		for( auto& reward : spPropStage->aryReward ) {
			XWnd *pWndReward = nullptr;
			pWndReward = new XWndStoragyItemElem( XE::VEC2( 0, 113 ), reward );
			pWndReward->SetScaleLocal( 0.5f );
			pWndReward->SetbActive( false );	// 버튼이 활성화되어있을땐 
			pElem->Add( pWndReward );
			auto pImgVCheck = xGET_IMAGE_CTRL( pWndReward, "img.vcheck" ) ;
			if( spStageObj->GetbRecvReward() ) {
				if( pImgVCheck == nullptr ) {
					pImgVCheck = new XWndImage(PATH_UI("v_check.png") , XE::VEC2(0,-10) );
					pImgVCheck->SetstrIdentifier("img.vcheck");
					pWndReward->Add( pImgVCheck );
					pImgVCheck->AutoLayoutHCenter();
					pImgVCheck->SetblendFunc( XE::xBF_ADD );
				}
				pImgVCheck->SetbShow( true );
			} else {
				if( pImgVCheck )
					pImgVCheck->SetbShow( false );
			}
			aryIcons.Add( pWndReward );
		}
		pElem->AutoLayoutHCenterWithAry( aryIcons, 10.f );
	} else {
		xSET_SHOW( pElem, "butt.reward", false );
		xSET_SHOW( pElem, "img.star.num", false );
	}
#ifdef _CHEAT
// 	if( XAPP->m_bDebugMode ) {
// 		int lvLegion = spPropStage->legion.lvLegion;
// 		if( lvLegion == 0 )
// 			lvLegion = ACCOUNT->GetLevel() + spPropStage->legion.adjLvLegion;
// 		auto pText = xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", lvLegion ) );
// 		if( pText ) {
// 			pText->SetbShow( TRUE );
// 			XCOLOR col = XCOLOR_WHITE;
// 			if( spStageObj->GetPower() )
// 				col = XGAME::GetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
// 			pText->SetColorText( col );
// 		}
// 	} else
// 		xSET_SHOW( pElem, "text.lv.legion", false );
#endif // _CHEAT
	///< 
	return pElem;
}

void XWndPopupCampaign::Update()
{
	XWndCampaignBase::Update();
}

void XWndPopupCampaign::SetEventByList( XWndList *pList )
{

}
int XWndPopupCampaign::OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickStage:%d", p1);
	int idxStage = (int)p1;
	// 어느스팟의 몇번째 스테이지를 선택했는지 서버로 보낸다.
	auto spCampObj = GetspCampObj();
	auto spStageObj = GetspCampObj()->GetspStage( idxStage );
	if( XBREAK( spStageObj == nullptr ) )
		return 1;
	auto spPropStage = spStageObj->GetspPropStage();
	if( spPropStage->levelLimit != 0 &&
		ACCOUNT->GetLevel() < spPropStage->levelLimit ) {
		XWND_ALERT( "%s", XTEXT( 2089 ) );
		return 1;
	}
	if( ACCOUNT->GetAP() < ACCOUNT->GetAPPerBattle() ) {
		XWND_ALERT( "%s", XTEXT(2240) );
		return 1;
	}
	auto bOk = spCampObj->IsAbleTry( spStageObj );
	if( bOk == XGAME::xE_NOT_ENOUGH_NUM_TRY ) {
		XWND_ALERT( "%s", XTEXT(2155) );
		return 1;
	} else
	if( bOk == XGAME::xE_OK ) {
		// 전투전 팝업을 띄움.
		auto pPopup = new XWndPopupSpotMenu( m_pBaseSpot, idxStage );
		//	pPopup->SetidCloseByAttack( getid() );	// 공격시작으로 전투팝업이 닫힐때 this도 함께 닫힘.
		pPopup->AddMenuAttack();			// 공격버튼
		pPopup->SetPower( spStageObj->GetPower() );
		pPopup->SetLevel( spStageObj->GetLevelLegion() );
		_tstring strName = spCampObj->GetNameCamp();
		if( strName.empty() )
			strName = m_pBaseSpot->GetszName();
		pPopup->SetstrName( strName );
		pPopup->SetItems( spStageObj->GetaryDrops() );
		GAME->GetpScene()->Add( pPopup );
	} else
	if( bOk == XGAME::xE_LOCK ) {
		XWND_ALERT( "%s", XTEXT( 2157 ) );	// 잠겨있습니다.
		return 1;
	} else {
		XWND_ALERT( "%s", XTEXT(2156) );	// 그외 모든 에러(더이상 도전할수 엄슴)
		return 1;
	}
// 	auto idsMedal = XGAME::GetIdsMedal( XGAME::xAT_TANKER, 0 );
// 	pPopup->AddItems( idsMedal,  );
// 	auto pPropItem = PROP_ITEM->GetpProp( idsMedal );
// 	if( XASSERT(pPropItem) ) {
// //		pPopup->SetidItem( pPropItem->idProp );
// 		pPopup->AddItems( )
// 	}
// 	GAMESVR_SOCKET->SendReqClickStageInCampaign( GAME,
// 		m_pBaseSpot->GetidSpot(),
// 		m_pBaseSpot->GetsnSpot(),
// 		idxStage, spPropStage->idProp );
//	SetbDestroy( TRUE );
	return 1;
}

LPCTSTR XWndPopupCampaign::GetidsDropItem( std::shared_ptr<XPropCamp::xStage> spPropStage )
{
	return spPropStage->sidDropItem.c_str();
}

int XWndPopupCampaign::OnClickReward( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickReward");
	//
	int idxStage = p1;
	GAMESVR_SOCKET->SendReqCampaignReward( GAME, m_pBaseSpot->GetidSpot(), GetspCampObj()->GetidProp(), idxStage );
	return 1;
}

////////////////////////////////////////////////////////////////
/**
 @brief 
*/
XWndGuildRaid::XWndGuildRaid( XSpotCommon *pSpot, CampObjPtr& spCampObj )
  : XWndCampaignBase( pSpot, spCampObj )
{
  Init();
  m_pBaseSpot = pSpot;
  m_spCampObj = spCampObj;
  DestroyWndByIdentifier("img.ap");
  GetpLayout()->CreateLayout( "guild_raid", this );
  SetstrIdentifier("wnd.guild.raid");
  auto pWndList = Find("list.stage");
  if( pWndList ) {
	  auto vPos = pWndList->GetPosLocal();
	  vPos.y = 82.f;
	  pWndList->SetPosLocal( vPos );
  }
}
template<typename T>
void XWndGuildRaid::ArrangeDebugButton( const XE::VEC2& vPos, const XE::VEC2& vSize, LPCTSTR szLabel, const char *cIdentifier, T func ) 
{
	auto pButt = Find( cIdentifier );
	if( pButt == nullptr ) {
		pButt = new XWndButtonDebug( vPos, vSize, szLabel );
		pButt->SetstrIdentifier( cIdentifier );
		pButt->SetEvent( XWM_CLICKED, this, func );
		Add( pButt );
	}
}


void XWndGuildRaid::Update() 
{
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		ArrangeDebugButton( XE::VEC2(18,18), XE::VEC2(30,30), _T("clear"), "butt.debug.clear", &XWndGuildRaid::OnClickClear );
#endif // _CHEAT
  xSET_TEXT( this, "text.ap", XFORMAT( "%d", 50 ) );
  bool bAutoUpdate = false;
//  auto spCampObj = GetspCampObj();
	auto spCampObj = m_spCampObj;	// 일단 이렇게 함. 내부에 포인트 갖고 있는방식 갠찮을지는 나중에 다시 검증해야함.
	if( spCampObj ) {
		if( spCampObj->IsTryingUser() )
			bAutoUpdate = true;
		if( spCampObj->IsOpenCampaign() ) {
			if( spCampObj->IsClearCampaign() ) {
				xSET_SHOW( this, "text.remain.reset", true );
				bAutoUpdate = true;
			}
			else {
				xSET_SHOW( this, "text.remain.reset", false );
			}
			xSET_SHOW( this, "butt.start", false );
		}
		else {
			auto pButt = xSET_SHOW( this, "butt.start", true );
			pButt->SetEvent( XWM_CLICKED, this, &XWndGuildRaid::OnClickStart );
		}
	}
  if( bAutoUpdate /*|| 1 */)
	  SetAutoUpdate( 0.1f );
  else
	  ClearAutoUpdate();
  XWndCampaignBase::Update();
  ///< 임시
  SetAutoUpdate( 0.1f );
  xSET_SHOW( this, "text.remain.reset", true );

}

/****************************************************************
* @brief 
*****************************************************************/
int XWndGuildRaid::OnClickClear( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickClear");
	//
	GAMESVR_SOCKET->SendCheat( GAME, 21, m_pBaseSpot->GetidSpot() );
// 	auto spCampObj = GetspCampObj();
	auto spCampObj = m_spCampObj;	// 일단 이렇게 함. 내부에 포인트 갖고 있는방식 갠찮을지는 나중에 다시 검증해야함.
	if( spCampObj )
		spCampObj->_DoClear();
	
	return 1;
}

XWnd* XWndGuildRaid::UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj )
{
	auto spCampObj = m_spCampObj;	// 일단 이렇게 함. 내부에 포인트 갖고 있는방식 갠찮을지는 나중에 다시 검증해야함.
// 	auto spCampObj = GetspCampObj();
	auto spPropStage = spStageObj->GetspPropStage();
	XBREAK( GetpLayout() == nullptr );
//	XWnd *pElem = new XWnd;
	GetpLayout()->CreateLayout( "elem_guild_raid", pElem );
// 	auto spStageObj = m_spCampObj->GetspStage( spPropStage->idxStage );
	if( XBREAK( spStageObj == nullptr ) )
		return nullptr;
	int numStage = spCampObj->GetNumStages();
	int i = spPropStage->idxStage;
	XBREAK( i < 0 );
	_tstring strImg;
	if( spCampObj->IsCloseCampaign() )		// 캠페인이 닫혀있는상태면 모두 잠금이미지로
		strImg = _T( "stage_elem_lock.png" );
	else if( i == spCampObj->GetidxLastUnlock() )
		strImg = _T( "stage_elem.png" );
	else if( i < spCampObj->GetidxLastUnlock() )
		strImg = _T( "stage_clear.png" );
	else if( i == numStage - 1 )
		strImg = _T( "stage_elem_boss.png" );
	else
		strImg = _T( "stage_elem_lock.png" );
	// 각 스테이지 버튼 그림
	auto pButt = xGET_BUTT_CTRL( pElem, "img.bg.elem" );
	if( pButt == nullptr )
		return nullptr;
	pButt->SetpSurface( 0, XE::MakePath( DIR_UI, strImg ) );
	// 군단 레벨
	xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", spPropStage->legion.lvLegion ) );
	// 보상 포인트
	int reward = 0;
	if( i == numStage - 1 )	// 마지막스테이지
		reward = spCampObj->GetRewardPointLastStage( ACCOUNT->GetLevel() );
	else
		reward = (int)spCampObj->GetRewardPerStage( ACCOUNT->GetLevel() );
	_tstring strReward = XFORMAT("+%s", XE::NumberToMoneyString( reward ));
	xSET_TEXT( pElem, "text.reward.point", strReward );
	xSET_TEXT( pElem, "text.power", XE::NumberToMoneyString(spStageObj->GetPower()) );
	bool bAttacking = false;
	if( spCampObj->IsTryingUser() && i == spCampObj->GetidxLastUnlock() )
		bAttacking = true;
	// 도전중인 유저의 이름과 남은시간
	bool bButtActive = true;
	if( bAttacking ) {
		xSET_SHOW( pElem, "text.name.user", true );
		xSET_SHOW( pElem, "text.remain.battle", true );
		bButtActive = false;
	}
	else {
		xSET_SHOW( pElem, "text.name.user", false );
		xSET_SHOW( pElem, "text.remain.battle", false );
	}
	// 도전가능한 스테이지일 경우 이벤트핸들러
	if( !( spCampObj->IsOpenCampaign() && spCampObj->IsAbleTry( spStageObj ) == XGAME::xE_OK ) )
		bButtActive = false;

	if( bButtActive ) {
		pButt->SetbActive( TRUE );
		pButt->SetEvent( XWM_CLICKED, this, &XWndGuildRaid::OnClickStage, i );
	}
	else
		pButt->SetbActive( FALSE );
	pElem->SetAutoSize();
	pElem->SetstrIdentifierf( "stage.%d", i );
	///< 
	return pElem;
}

void XWndGuildRaid::OnAutoUpdate()
{
// 	auto spCampObj = GetspCampObj();
	auto spCampObj = m_spCampObj;	// 일단 이렇게 함. 내부에 포인트 갖고 있는방식 갠찮을지는 나중에 다시 검증해야함.
	if( spCampObj->IsTryingUser() ) {
		int idxLastUnlock = spCampObj->GetidxLastUnlock();
		if( idxLastUnlock >= 0 ) {
			XWnd *pElem = Findf("stage.%d", idxLastUnlock);
			if( pElem ) {
				_tstring strUser = XFORMAT("%s %s", spCampObj->GetstrTryer().c_str(), XTEXT( 2124 ) );
				auto secRemain = spCampObj->GetsecRemainTry();
				auto strRemain = XSceneTech::sGetResearchTime( secRemain );
				xSET_TEXT( pElem, "text.name.user", strUser );
				xSET_SHOW( pElem, "text.name.user", true );
				xSET_SHOW( pElem, "text.remain.battle", true );
				xSET_TEXT( pElem, "text.remain.battle", strRemain );
			}
		}
	} else 
	if( spCampObj->IsClearCampaign() ) {	// <<= 왜 클리어시에만 남은시간 나오게 했지?
		// 닫히기까지 남은시간
		xSET_SHOW( this, "text.remain.reset", true );
		auto secRemain = spCampObj->GetsecRemainClose();
		if( secRemain > 0 ) {
			auto strRemain = XSceneTech::sGetResearchTime( secRemain );
			xSET_TEXT( this, "text.remain.reset", strRemain );
		} else {
			xSET_TEXT( this, "text.remain.reset", XTEXT(2125) );	// 공격을 시작하십시오.
		}
	}
	xSET_SHOW( this, "text.remain.reset", true );
	auto secRemain = spCampObj->GetsecRemainClose();
	auto strRemain = XSceneTech::sGetResearchTime( secRemain );
	xSET_TEXT( this, "text.remain.reset", XFORMAT("%s %s", XTEXT(80131), strRemain.c_str() ) );


}

void XWndGuildRaid::SetEventByList( XWndList *pWndList )
{
//  pWndList->SetEvent( XWM_SELECT_ELEM, this, &XWndGuildRaid::OnClickStage );
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndGuildRaid::OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickStage:%d", p1 );
	//
	int idxStage = (int)p1;
	// 어느스팟의 몇번째 스테이지를 선택했는지 서버로 보낸다.
	auto spCampObj = m_spCampObj;	// 일단 이렇게 함. 내부에 포인트 갖고 있는방식 갠찮을지는 나중에 다시 검증해야함.
// 	auto spCampObj = GetspCampObj();
	auto spStageObj = spCampObj->GetspStage( idxStage );
	if( XBREAK( spStageObj == nullptr ) )
		return 1;
	auto spPropStage = spStageObj->GetspPropStage();
#ifdef _CHEAT
	if( !ACCOUNT->m_bDebugMode ) 
#endif // _CHEAT
	{
		if( spPropStage->levelLimit != 0 &&
			ACCOUNT->GetLevel() < spPropStage->levelLimit )	{
			XWND_ALERT( "%s", XTEXT( 2089 ) );
			return 1;
		}
		if( ACCOUNT->GetAP() < ACCOUNT->GetAPPerBattle() )	{
			XWND_ALERT( "%s", XTEXT(2240) );
			return 1;
		}
		if( spCampObj->FindTryer( ACCOUNT->GetidAccount() ) ) {
			XWND_ALERT( "%s", XTEXT( 2128 ) );
			return 1;
		}
	}
	GAMESVR_SOCKET->SendReqSpotAttack( GAME, m_pBaseSpot->GetidSpot(), idxStage );
// 	GAMESVR_SOCKET->SendReqClickStageInCampaign( GAME,
// 											m_pBaseSpot->GetidSpot(),
// 											m_pBaseSpot->GetsnSpot(),
// 											idxStage, spPropStage->idProp );
	
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndGuildRaid::OnClickStart( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickStart");
	//
	GAMESVR_SOCKET->SendReqSendReqGuildRaidOpen( GAME, m_pBaseSpot->GetidSpot() );	
	SetbDestroy( TRUE );	// 현재 길드정보가 없어서 업데이트가 힘드니 일단 닫고 다시 여는걸로.
	return 1;
}

////////////////////////////////////////////////////////////////
/**
 @brief 
*/
XWndPopupCampaignMedal::XWndPopupCampaignMedal( XSpot *pBaseSpot, xCampaign::CampObjPtr& spCampObj )
	: XWndPopupCampaign( pBaseSpot, spCampObj )
{
	// 이걸 왜 XWndPopuCampaign(퀘용일반캠페인)을 상속받았지? -_-?
	Init();
//	GetpLayout()->CreateLayout( "medal_camp", this );
}

LPCTSTR XWndPopupCampaignMedal::GetidsDropItem( std::shared_ptr<XPropCamp::xStage> spPropStage )
{
	// 등급에 따라서 가라 아이템 ids를 리턴한다.
	// 3종류의 메달을 겹쳐놓은것 같은 비주얼로 한다.
	return _T("");
}

XWndCampaignBase::xtStageState
XWndPopupCampaignMedal::GetStageState( StageObjPtr spStage )
{
	// 언락한 스테이지 인덱스보다 작으면 진입가능
	int idxStage = spStage->GetidxStage();
	auto spCampObj = GetspCampObj();
	if( idxStage <= spCampObj->GetidxLastUnlock() )
		return xSS_ENTERABLE;
	return xSS_LOCK;
}

// XWnd* XWndPopupCampaignMedal::UpdateListElem( std::shared_ptr<XPropCamp::xStage> spPropStage )
XWnd* XWndPopupCampaignMedal::UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj )
{
	auto spCampObj = GetspCampObj();
	auto spPropStage = spStageObj->GetspPropStage();
//	XWnd *pElem = new XWnd;
	GetpLayout()->CreateLayout( "elem_campaign_medal", pElem );
// 	auto spStageObj = m_spCampObj->GetspStage( spPropStage->idxStage );
	if( XBREAK( spStageObj == nullptr ) )
		return nullptr;
	int numStage = spCampObj->GetNumStages();
	int i = spPropStage->idxStage;
	XBREAK( i < 0 );
	_tstring strImg;
	bool bLock = false;
	bool bComplete = false;
	bool bBoss = false;
	// 각 스테이지 버튼 그림
	auto pButt = xGET_BUTT_CTRL( pElem, "img.bg.elem" );
	if( pButt == nullptr )
		return nullptr;
	pButt->SetbActive( true );
	auto state = GetStageState( spStageObj );
	switch( state )
	{
	case XWndCampaignBase::xSS_ENTERABLE:
		strImg = _T( "stage_elem.png" );
		break;
	case XWndCampaignBase::xSS_CLEARED:
		strImg = _T( "stage_clear.png" );
		xSET_SHOW( pElem, "text.complete", true );
		pButt->SetbActive( false );
		break;
	case XWndCampaignBase::xSS_LOCK:
		strImg = _T( "stage_elem_lock.png" );
		bLock = true;
		pButt->SetbActive( false );
		break;
	case XWndCampaignBase::xSS_BOSS:
		strImg = _T( "stage_elem_boss.png" );
		bBoss = true;
		break;
	default:
		XBREAK(1);
		break;
	}
	pButt->SetpSurface( 0, XE::MakePath( DIR_UI, strImg ) );
	// 군단 레벨
	int lvLegion = spPropStage->legion.lvLegion;
	if( lvLegion == 0 )
		lvLegion = ACCOUNT->GetLevel() + spPropStage->legion.adjLvLegion;
	auto pText = xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", lvLegion ) );
	if( pText ) {
		XCOLOR col = XCOLOR_WHITE;
		if( spStageObj->GetPower() )
			col = XGAME::GetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
		pText->SetColorText( col );
	}
	if( IsShowStar() && spStageObj->GetnumStar() > 0 ) {
		// 별점을 획득한 스테이지만 별표시
		int numStar = spStageObj->GetnumStar();
		XWnd *pParent = nullptr;
		XArrayLinearN<XWnd*,256> ary;
		for( int i = 0; i < 3; ++i ) {
			auto pMark = xGET_IMAGE_CTRLF( pElem, "img.clear.%d", i + 1 );
			if( pMark ) {
				pParent = pMark->GetpParent();
				pMark->SetbShow( true );
				if( i < numStar )
					pMark->SetSurfaceRes( PATH_UI( "star_on.png" ) );
				ary.Add( pMark );
			}
		}
		pParent->AutoLayoutHCenterWithAry( ary, 15.f );
	}
	else
		pText->SetbShow( FALSE );
	pButt->SetEvent( XWM_CLICKED, this, &XWndPopupCampaign::OnClickStage, i );
	pElem->SetAutoSize();
	pElem->SetstrIdentifierf( "stage.%d", i );
	///< 보상템 보여줌.
	auto dropItem = spStageObj->GetDrop( 0 );
	if( dropItem.idDropItem ) {
		auto pItem = new XWndStoragyItemElem( dropItem.idDropItem );
		pItem->SetstrIdentifier( "wnd.drop" );
		pItem->SetPosLocal( 0.f, 102.f );
		pItem->SetNum( dropItem.num );
		pItem->SetEventItemTooltip();
		pItem->SetScaleLocal( 0.7f );
		pElem->Add( pItem );
		pItem->AutoLayoutHCenter();
	}
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		int lvLegion = spPropStage->legion.lvLegion;
		if( lvLegion == 0 )
			lvLegion = ACCOUNT->GetLevel() + spPropStage->legion.adjLvLegion;
		auto pText = xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", lvLegion ) );
		if( pText ) {
			pText->SetbShow( TRUE );
			XCOLOR col = XCOLOR_WHITE;
			if( spStageObj->GetPower() )
				col = XGAME::GetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
			pText->SetColorText( col );
		}
	} else
		xSET_SHOW( pElem, "text.lv.legion", false );
#endif // _CHEAT
	///< 
	return pElem;
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
	_tstring str = XSceneTech::sGetResearchTime( (int)sec );
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
XWndSpotMsg::XWndSpotMsg( ID idSpot, LPCTSTR szText, float size, XCOLOR col )
{
	Init();
	m_strText = szText;
	m_Size = size;
	if( SCENE_WORLD ) {
		if( XASSERT(idSpot) ) {
			XWnd* pWnd = SCENE_WORLD->Find( idSpot );
			if( pWnd ) {
				SetPosLocal( pWnd->GetPosLocal() );
				auto pText = new XWndTextString( XE::VEC2( 0 ), szText, FONT_NANUM, size, col );
				pText->SetStyleStroke();
				Add( pText );
			}
		}
	}
	SetAutoSize();
	m_timerLife.Set( 3.f );
	m_State = 0;
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 v = GetPosLocal();
	v -= vSize / 2.f;
	SetPosLocal( v );
}
XWndSpotMsg::XWndSpotMsg( const XE::VEC2& vPos, LPCTSTR szText, float size, XCOLOR col )
{
	Init();
	m_strText = szText;
	m_Size = size;
	if( SCENE_WORLD ) {
// 		if( idSpot ) {
// 			XWnd* pWnd = SCENE_WORLD->Find( idSpot );
// 			if( pWnd ) {
// 				SetPosLocal( pWnd->GetPosLocal() );
		SetPosLocal( vPos );
				auto pText = new XWndTextString( XE::VEC2( 0 ), szText, FONT_NANUM, size, col );
				pText->SetStyleStroke();
				Add( pText );
// 			}
//		}
	}
	SetAutoSize();
	m_timerLife.Set( 3.f );
	m_State = 0;
	XE::VEC2 vSize = GetSizeFinal();
	XE::VEC2 v = GetPosLocal();
	v -= vSize / 2.f;
	SetPosLocal( v );
}

void XWndSpotMsg::Destroy()
{
}

int XWndSpotMsg::Process( float dt )
{
	XE::VEC2 v = GetPosLocal();
	do 
	{
		if( m_State == 0 )
		{
			if( m_timerLife.IsOver() )
			{
				++m_State;
				m_timerLife.Set( 2.f );
				SetAlphaLocal( 1.0f );
				break;
			}
			v.y -= 0.3f;
			SetPosLocal( v );
			float lerp = m_timerLife.GetSlerp();
			SetAlphaLocal( lerp );
		} else
		if( m_State == 1 )
		{
			SetAlphaLocal( 1.0f );
			if( m_timerLife.IsOver() )
			{
				++m_State;
				m_timerLife.Set( 1.f );
				break;
			}
		} else
		if( m_State == 2 )
		{
			if( m_timerLife.IsOver() )
			{
				SetbDestroy( TRUE );
				break;
			}
			float lerp = m_timerLife.GetSlerp();
			SetAlphaLocal( 1.0f - lerp );
		}

	} while (0);
	return XWnd::Process( dt );
}

////////////////////////////////////////////////////////////////
XWndProduceMsg::XWndProduceMsg( ID idSpot, LPCTSTR szText, float size, XCOLOR col )
	: XWndSpotMsg( idSpot, szText, size, col )
{
	Init();
	m_timerLife.Set( 2.f );
	auto v = GetPosLocal();
	v.y -= 25.f;
	SetPosLocal( v );
}

XWndProduceMsg::XWndProduceMsg( const XE::VEC2& vPos, LPCTSTR szText, float size, XCOLOR col )
	: XWndSpotMsg( vPos, szText, size, col )
{
	Init();
	m_timerLife.Set( 2.f );
	auto v = GetPosLocal();
	v.y -= 25.f;
	SetPosLocal( v );
}

int XWndProduceMsg::Process( float dt )
{
	XE::VEC2 v = GetPosLocal();
	do 
	{
		v.y -= 0.3f;
		SetPosLocal( v );
		if( m_State == 0 )
		{
			if( m_timerLife.IsOver() )
			{
				++m_State;
				m_timerLife.Set( 1.f );
				SetAlphaLocal( 1.0f );
				break;
			}
		} else
		if( m_State == 1 )
		{
			if( m_timerLife.IsOver() )
			{
				SetbDestroy( TRUE );
				break;
			}
			float lerp = m_timerLife.GetSlerp();
			SetAlphaLocal( 1.0f - lerp );
		}

	} while (0);	
	return 1;
}
////////////////////////////////////////////////////////////////
/**
 @brief 전투 통계
*/
XWndStatistic::XWndStatistic( XLegionObj *pLegionObj1, XLegionObj *pLegionObj2 )
	: XWndPopup(_T("layout_statistic.xml"), "popup_statistic" )
{
	Init();
	m_aryLegionObj.Add( pLegionObj1 );
	m_aryLegionObj.Add( pLegionObj2 );
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

void XWndStatistic::CreateHerosUI( XWnd *pRoot, XLegionObj *pLegionObj, float max, xtStatistic type, XGAME::xtSide side )
{
	switch( type )
	{
	case XWndStatistic::xST_DEAL:
		pLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageDeal > pStat2->damageDeal ) ? true : false;
		} );
		break;
	case XWndStatistic::xST_ATTACKED:
		pLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageAttacked > pStat2->damageAttacked ) ? true : false;
		} );
		break;
	case XWndStatistic::xST_DEAL_BY_CRITICAL:
		pLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageDealByCritical > pStat2->damageDealByCritical ) ? true : false;
		} );
		break;
	case XWndStatistic::xST_BY_EVADE:
		pLegionObj->GetpStatObj()->GetlistSquads().sort(
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
		pLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->treated > pStat2->treated ) ? true : false;
		} );
		break;
	case XWndStatistic::xST_DEAL_BY_SKILL:
		pLegionObj->GetpStatObj()->GetlistSquads().sort(
			[]( XStatistic::xSquad *pStat1, XStatistic::xSquad *pStat2 )->bool{
			return ( pStat1->damageDealBySkill > pStat2->damageDealBySkill ) ? true : false;
		} );
		break;
	default:
		XBREAK(1);
		break;
	}
	float y = 0;
	for( auto pSquad : pLegionObj->GetpStatObj()->GetlistSquads() )
	{
		auto pHero = pSquad->pHero;
		auto pWndHero = new XWndInvenHeroElem( pHero, pLegionObj->GetspLegion().get() );
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
			const float multiply = XHero::s_fMultiply;
			std::vector<float> aryOrig;
			aryOrig.push_back( pHero->GetAttackMeleeRatio() * multiply );
			aryOrig.push_back( pHero->GetAttackRangeRatio() * multiply );
			aryOrig.push_back( pHero->GetDefenseRatio() * multiply );
			aryOrig.push_back( pHero->GetHpMaxRatio() * multiply );
			aryOrig.push_back( pHero->GetAttackSpeed() * multiply );
			aryOrig.push_back( pHero->GetMoveSpeed() * multiply );
			std::vector<float> aryStat;
			aryStat.push_back( pHero->GetAttackMeleeRatio( lvWill ) * multiply );
			aryStat.push_back( pHero->GetAttackRangeRatio( lvWill ) * multiply );
			aryStat.push_back( pHero->GetDefenseRatio( lvWill ) * multiply );
			aryStat.push_back( pHero->GetHpMaxRatio( lvWill ) * multiply );
			aryStat.push_back( pHero->GetAttackSpeed( lvWill ) * multiply );
			aryStat.push_back( pHero->GetMoveSpeed( lvWill ) * multiply );
			//
			xSET_TEXT( pRootLeft, "text.level", pHero->GetLevel() );
			xSET_TEXT( pRootRight, "text.level", lvWill );
			for( int i = 0; i < 6; ++i ) {
				const auto statOrig = aryOrig[i];
				const auto statUp = aryStat[i];
				xSET_TEXTF( pRootLeft, XFORMAT( "%.0f", statOrig ), "text.stat.%d", i + 1 );
				auto pText =
				xSET_TEXTF( pRootRight, XFORMAT( "%.0f", statUp ), "text.stat.%d", i + 1 );
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
				int numCurr = XAccount::sGetMaxSquadByLevel( level );
				int numPrev = XAccount::sGetMaxSquadByLevel( level - 1 );
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
*/
XWndPaymentByCash::XWndPaymentByCash( XGAME::xtResource resType, int goldLack, int cash, LPCTSTR szMsg )
  : XWndPopup( _T( "buy_by_cash.xml" ), "popup" )
{
  Init();
  auto pWnd = new XWndResourceCtrl( XE::VEC2( 125, 60 ) );
  Add( pWnd );
  pWnd->AddRes( resType, (float)goldLack );
  auto pTextCash 
    = xSET_TEXT( this, "text.num.cash", 
                    XE::NumberToMoneyString( cash ) );
  if( szMsg )
    xSET_TEXT( this, "text.msg", szMsg );
  // 캐쉬가 부족하면 빨간색.
  if( pTextCash && ACCOUNT->IsNotEnoughCash( cash ) )
    pTextCash->SetColorText( XCOLOR_RED );

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
/**
 @brief 영웅의 전장
*/
XWndCampaignHero::XWndCampaignHero( XSpotCommon *pSpot, xCampaign::CampObjPtr& spCampObj )
	: XWndCampaignBase( pSpot, spCampObj )
{
	Init();
//	GetpLayout()->CreateLayout( "hero_camp", this );
	m_pSpot = pSpot;
}

XWndCampaignBase::xtStageState
XWndCampaignHero::GetStageState( xCampaign::StageObjPtr spStage ) 
{
	int idxStage = spStage->GetidxStage();
	auto spCampObj = GetspCampObj();
	auto idxLastUnlock = spCampObj->GetidxLastUnlock();
	if( idxStage < idxLastUnlock )
		return XWndCampaignBase::xSS_RETRY;
	else
	if( idxStage == idxLastUnlock )
		return XWndCampaignBase::xSS_NEW_OPEN;
	else
		return XWndCampaignBase::xSS_LOCK;
}

// XWnd* XWndCampaignHero::UpdateListElem( std::shared_ptr<XPropCamp::xStage> spPropStage )
XWnd* XWndCampaignHero::UpdateListElem( XWnd *pElem, xCampaign::StageObjPtr spStageObj )
{
	if( pElem->GetNumChild() > 0 )
		return nullptr;
	// UpdateListElem에서 공통되는 코드들 다 위로 올리자
	auto spCampObj = GetspCampObj();
	auto spPropStage = spStageObj->GetspPropStage();
// 	XWnd *pElem = new XWnd;
	GetpLayout()->CreateLayout( "elem_campaign_hero", pElem );
// 	auto spStageObj = m_spCampObj->GetspStage( spPropStage->idxStage );
 	auto spStageObjHero
		= std::static_pointer_cast<XStageObjHero>( spStageObj );
 	if( XBREAK( spStageObj == nullptr ) )
 		return nullptr;
	int numStage = spCampObj->GetNumStages();
	const int idxStage = spPropStage->idxStage;
	XBREAK( idxStage < 0 );
	auto spLegion = spStageObj->GetspLegion();
	_tstring strImg;
	bool bLock = false;
	bool bComplete = false;
	bool bBoss = false;
	// 각스테이지위에 올려놓은 가상버튼
	auto pButt = xGET_BUTT_CTRL( pElem, "butt.stage" );
	if( pButt == nullptr )
		return nullptr;
	pButt->SetbActive( true );
	xSET_SHOW( pElem, "wnd.disable", false );	// lock상태일때 위에 덮을 검은장막과 자물쇠
	xSET_SHOW( pElem, "img.vcheck", false );
	{
		auto pText 
			= xSET_TEXT( pElem, "text.stage.num", XFORMAT( "%d", idxStage + 1 ) );
		if( pText ) {
 			auto col = XGAME::GetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
 			pText->SetColorText( col );
		}
	}
	SetHeroFace( pElem, spStageObjHero );
	xSET_SHOW( pElem, "img.vcheck", false );
	xSET_ENABLE( pElem, "butt.stage", false );
	// 현재 스테이지 상태
	auto state = GetStageState( spStageObj );
	switch( state )
	{
	case XWndCampaignBase::xSS_RETRY: {
		// 재도전 가능한곳.
		ShowStar( pElem, true, spStageObj->GetnumStar() );
		int numRemain = spPropStage->maxWin - spStageObj->GetnumClear();
		if( numRemain < 0 )
			numRemain = 0;
		if( numRemain == 0 ) {
			ShowRetryMark( pElem, false, numRemain );
			xSET_SHOW( pElem, "img.vcheck", true );	// 횟수를 다 썼으면 V표시
		} else {
			ShowRetryMark( pElem, true, numRemain );
			xSET_ENABLE( pElem, "butt.stage", true );	// 도전횟수가 남았으면 버튼 활성화.
		}
	} break;
	case XWndCampaignBase::xSS_NEW_OPEN: {
		// 새로오픈된 스테이지
		ShowStar( pElem, true, 0 );
		ShowRetryMark( pElem, false, 0 );
		xSET_ENABLE( pElem, "butt.stage", true );
	} break;
	case XWndCampaignBase::xSS_LOCK:
		ShowStar( pElem, false, 0 );
		ShowRetryMark( pElem, false, 0 );
		xSET_SHOW( pElem, "wnd.disable", true );	// lock상태일때 위에 덮을 검은장막과 자물쇠
		pButt->SetbActive( false );
		break;
	default:
		XBREAK(1);
		break;
	}
	pButt->SetEvent( XWM_CLICKED, this, &XWndCampaignBase::OnClickStage, idxStage );
	pElem->SetstrIdentifierf( "stage.%d", idxStage );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		auto pText = xGET_TEXT_CTRL( pElem, "text.debug.level" );
		if( pText == nullptr ) {
			pText = new XWndTextString( XE::VEC2(2,17) );
			pText->SetstrIdentifier( "text.debug.level" );
			pElem->Add( pText );
		}
		pText->SetText( XFORMAT("%d(%d)", spPropStage->legion.lvLegion, spPropStage->levelLimit) );
	}
#endif // CHEAT
	return pElem;
}

/**
 @brief 별UI를 보이거나 감춤.
*/
void XWndCampaignHero::ShowStar( XWnd *pRoot, bool bShow, int numStar )
{
	if( !bShow )
		numStar = 0;
	for( int i = 1; i <= 3; ++i ) {
		bool bOn = false;
		if( i <= numStar )
			bOn = true;
		xSET_SHOWF( pRoot, bOn, "img.star.on%d", i );
		if( !bShow )	// 별UI자체를 감추는것이라면 별슬롯도 가림
			bOn = true;
		xSET_SHOWF( pRoot, !bOn, "img.star.off%d", i );
	}
}
/**
 @brief 재도전 UI를 보이거나 감춤.
*/
void XWndCampaignHero::ShowRetryMark( XWnd *pRoot, bool bShow, int num )
{
	if( !bShow )
		num = 0;
	for( int i = 1; i <= 2; ++i ) {
		bool bOn = false;
		if( i <= num )
			bOn = true;
		xSET_SHOWF( pRoot, bOn, "img.mark.on%d", i );
		if( !bShow )	// UI자체를 감추는것이라면 슬롯도 가림
			bOn = true;
		xSET_SHOWF( pRoot, !bOn, "img.mark.off%d", i );
	}
}
/**
 @brief 영웅의 얼굴을 세팅
*/
void XWndCampaignHero::SetHeroFace( XWnd *pElem, 
								xCampaign::StageObjHeroPtr spStageObj )
{
	XWnd *pRootFace = pElem->Find("wnd.hero.face");
	if( XASSERT(pRootFace) ) {
		XGAME::xReward reward;
//		ID idPropHero = m_pSpot->GetidHeroByStage( idxStage );
		ID idPropHero = spStageObj->GetidHero();
// 		ID idPropHero = PROP_HERO->GetidPropByIds( _T("zanta") );
		XBREAK( idPropHero == 0 );
		reward.SetHero( idPropHero );
		auto pWndFace = new XWndStoragyItemElem( XE::VEC2(0), reward );
		pWndFace->SetvScaleItemImg( 0.75f );
		pWndFace->SetNum( 0 );
		auto pPropHero = PROP_HERO->GetpProp( idPropHero );
		XBREAK( pPropHero == nullptr );
		auto num = ACCOUNT->GetNumSoulStone( pPropHero->strIdentifier );
		auto pHero = ACCOUNT->GetHeroByidProp( idPropHero );
		if( pHero ) {
			// 보유한 영웅
			_tstring str = XFORMAT("보유영웅(%d)", num );
			auto pText = new XWndTextString( XE::VEC2(2,2), str, FONT_NANUM, 15.f );
			pText->SetStyleStroke();
			pWndFace->Add( pText );
		} else {
			if( num > 0 ) {
				_tstring str = XFORMAT( "(%d)", num );
				auto pText = new XWndTextString( XE::VEC2( 2, 2 ), str, FONT_NANUM, 20.f );
				pWndFace->Add( pText );
			}
		}
		pRootFace->Add( pWndFace );
	}
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


////////////////////////////////////////////////////////////////
/**
 @brief 월드씬에서 좌측 프로필 사진을 누르면 플레이어의 자세한 정보창이 뜬다.
*/
XWndPlayerInfo::XWndPlayerInfo()
	: XWndPopup( _T("player_info.xml"), "popup_player" )
{
	Init();
	SetbModal( TRUE );
	auto pWndEdit = SafeCast<XWndEdit*>( Find("edit.hello.msg") );
	if( pWndEdit ) {
		pWndEdit->SetszString( ACCOUNT->GetstrHello() );
		pWndEdit->SetpDelegate( GAME );
	}
}

void XWndPlayerInfo::Update()
{
	// 플레이어 프로필사진 업데이트
	XGAME::UpdateProfileImage( this, "wnd.profile", true, GAME->GetpsfcProfile() );
	// 플레이시간 업데이트
	int h, m, s;
	XSYSTEM::GetHourMinSec( ACCOUNT->GetsecPlay(), &h, &m, &s );
	xSET_TEXT( this, "text.play.time", XFORMAT( "%s: %0d:%02d:%02d", XTEXT(2271), h, m, s ) );
	xSET_TEXT( this, "text.name", XFORMAT( "%s", ACCOUNT->GetstrName() ) );
	xSET_TEXT( this, "text.level", XFORMAT( "%d", ACCOUNT->GetLevel() ) );
	xSET_TEXT( this, "text.trophy", XFORMAT( "%d", ACCOUNT->GetLadder() ) ); 
	int powerTotal = ACCOUNT->GetPowerExcludeEmpty();
	xSET_TEXT( this, "text.power", XFORMAT( "%s", XE::NumberToMoneyString( powerTotal ) ) ); 
	// 경험치 바
	auto pBar = XLayout::sGetCtrl<XWndProgressBar*>( this, "pbar.exp" );
// 	if( pBar == nullptr ) {
// 		pBar = new XWndProgressBar( 80.f, 14.f, 64.f, 10.f );
// 		pBar->SetstrIdentifier( "pbar.exp" );
// 		Add( pBar );
// 	}
	pBar->SetLerp( (float)ACCOUNT->GetExp() / ACCOUNT->GetMaxExpCurrLevel() );
	XWndPopup::Update();
}

////////////////////////////////////////////////////////////////

XWndBgObj::XWndBgObj( const xnBgObj::xProp& prop )
	: XWndSprObj( prop.m_strSpr, prop.m_idAct, prop.m_vwPos ), m_Prop(prop)
{
	Init();
}

void XWndBgObj::Destroy()
{
	const std::string strFile = SZ2C( m_Prop.m_strSnd );
	const auto idSound = SOUND_TBL->Find( strFile );
	SOUNDMNG->CloseSound( idSound );
}
void XWndBgObj::ProcessMsg( const std::string& strMsg )
{
	if( strMsg == "update_sound" ) {
		if( m_idStream ) {
			const auto vwFocus = SCENE_WORLD->GetvwCamera();
			const float distsq = (m_Prop.m_vwPos - vwFocus).Lengthsq();
			float vol = distsq / (320.f * 320.f);
			if( vol < 0 )
				vol = 0.f;
			if( vol > 1.f )
				vol = 1.f;
			vol = 1.f - vol;
			SOUNDMNG->SetSoundVolume( m_idStream, vol );
//			CONSOLE("vol:%.1f", vol);
		}
	}
}

void XWndBgObj::Update()
{
	if( !m_Prop.m_strSnd.empty() && m_idStream == 0 ) {
		const std::string strFile = SZ2C( m_Prop.m_strSnd );
		const auto idSound = SOUND_TBL->Find( strFile );
		m_idStream = SOUNDMNG->OpenPlaySound( idSound, TRUE );
		if( m_idStream == xINVALID_STREAM ) {
			auto pReplay = SOUNDMNG->GetpLastReplay();
			if( pReplay ) {
				SOUNDMNG->SetCallback( pReplay, [this]( ID idStream ){
					m_idStream = idStream;
				});
			}
		}
		CONSOLE( "%s(id=%d):idStream=%d", __TFUNC__, m_Prop.m_idObj, m_idStream );
	}
	XWndSprObj::Update();
}

int XWndBgObj::Process( float dt )
{
// 	if( !m_Prop.m_strSnd.empty() && m_idStream == 0 ) {
// 		// 사운드가 지정되어있는데 스트림이 없다면 다시 플레이 시킴
// 		std::string strFile = SZ2C( m_Prop.m_strSnd );
// 		const std::string strcExt = XE::GetFileExt( strFile );
// 		if( strcExt.empty() )
// #ifdef WIN32
// 			strFile += ".wav";
// #else
// 			strFile += ".ogg";
// #endif // WIN32
// 		const auto idSound = SOUND_TBL->Find( strFile );
// 		m_idStream = SOUNDMNG->OpenPlaySound( idSound, TRUE );
// 		CONSOLE( "%s(id=%d):idStream=%d", __TFUNC__, m_Prop.m_idObj, m_idStream );
// 	}
	return XWndSprObj::Process( dt );
}

//////////////////////////////////////////////////////////////////////////
/**
 @brief 월드 배경에 배치되는 오브젝트들의 루트레이어 객체
*/
XWndBgObjLayer::XWndBgObjLayer()
{
	Init();
}

/**
 @brief 월드맵 스크롤에 따라 배경 객체를 생성.
 .화면과 화면가장자리 일정범위에 들어온 객체만 생성한다.
 .구름에 가려진 객체는 생성하지 않는다.
 .스크롤할때 드래그가 멈추면 생성한다.
 .화면을 벗어난지 일정시간이 지나면 삭제시킨다.
 .사운드객체는 화면의 중심과의 거리를 비교해서 사운드 볼륨을 조절한다.

*/
void XWndBgObjLayer::Update()
{
// 	const auto vwCenter = SCENE_WORLD->GetvwCamera();
// 	const auto sizeView = SCENE_WORLD->GetvwSizeView();
// 	UpdateCurrFocus( vwCenter, sizeView );
// 	auto& listObjs = XPropBgObj::sGet()->GetlistBgObjs();
// 	for( auto pProp : listObjs ) {
// 		if( pProp->m_Type != XGAME::xBOT_SOUND ) {
// 			auto pWndBgObj = SafeCast2<XWndBgObj*>( Findf( "__bg.%d", pProp->m_idObj ) );
// 			if( pWndBgObj == nullptr ) {
// 				pWndBgObj = new XWndBgObj( pProp->m_strSpr
// 																	, pProp->m_idAct
// 																	, pProp->m_vwPos );
// 				pWndBgObj->GoRandomFrame();
// 				pWndBgObj->SetstrIdentifierf("__bg.%d", pProp->m_idObj );
// 	#ifdef _xIN_TOOL
// 				pWndBgObj->SetEvent( XWM_CLICKED, this, &XWndBgObjLayer::OnClickBgObj );
// 	#endif // _xIN_TOOL
// 				Add( pWndBgObj );
// 			} else {
// 				pWndBgObj->SetSprObj( pProp->m_strSpr, pProp->m_idAct );
// 			}
// 		} else {
// 			// sound obj
// 
// 		}
// 	}
	XWnd::Update();
}

/**
 @brief 월드맵좌표 vFocus위치를 중심으로 한 bgobj들을 갱신한다.
*/
void XWndBgObjLayer::UpdateCurrFocus( const XE::VEC2& vFocus, const XE::VEC2& sizeView )
{
	int cntCreated = 0;
	auto& listObjs = XPropBgObj::sGet()->GetlistBgObjs();
	for( auto pProp : listObjs ) {
		const auto vwLT = vFocus - (sizeView * 0.5f);
		auto bIsIn = pProp->IsInArea( vwLT, sizeView );
#ifdef _xIN_TOOL
		if( XBaseTool::sIsToolBgObjMode() )		// 툴모드에선 무조건 in상태
			bIsIn = true;
#endif // _xIN_TOOL
// 		if( pProp->m_Type != XGAME::xBOT_SOUND ) {
			if( UpdateSprObj( pProp, bIsIn ) )
				++cntCreated;
// 		} else {
// 			// sound obj
// 			UpdateSoundObj( pProp, bIsIn );
// 		}
	}
#ifdef _DEBUG
	CONSOLE("created bgObj: %d", cntCreated );
#endif // _DEBUG
}

bool XWndBgObjLayer::UpdateSprObj( const xnBgObj::xProp* pProp, bool bIsIn )
{
	bool bCreated = false;
	auto pWndBgObj = SafeCast2<XWndBgObj*>( Findf( "__bg.%d", pProp->m_idObj ) );
	if( bIsIn ) {
		if( pWndBgObj == nullptr ) {
			pWndBgObj = new XWndBgObj( *pProp );
			pWndBgObj->GoRandomFrame();
			pWndBgObj->SetstrIdentifierf( "__bg.%d", pProp->m_idObj );
			Add( pWndBgObj );
			bCreated = true;
		} else {
			pWndBgObj->SetSprObj( pProp->m_strSpr, pProp->m_idAct );
		}
#ifdef _xIN_TOOL
		if( XBaseTool::sIsToolBgObjMode() )
			pWndBgObj->SetEvent( XWM_CLICKED, this, &XWndBgObjLayer::OnClickBgObj );
#endif // _xIN_TOOL
		pWndBgObj->OffTimer();		// 화면내에 잇는것은 타이머를 작동시키지 않는다.
	} else {
		// bgObj is out area
		if( pWndBgObj ) {
			// 10초동안 화면을 벗어나있으면 삭제됨
			pWndBgObj->SetTimer( 10.f );	// 타이머 작동시작. 이미 켜져있으면 작동시키지 않음.
			if( pWndBgObj->IsOverTime() ) {
// 				CONSOLE("destroy bgObj:id=%d", pProp->m_idObj );
				pWndBgObj->SetbDestroy( true );
			}
		}
	}
	return bCreated;
}

bool XWndBgObjLayer::UpdateSoundObj( const xnBgObj::xProp* pProp, bool bIsIn )
{
	return false;
}

/*
기본적으론 한번 생성되면 자동으로 삭제되지 않는다.
stop scroll이벤트가 일어날때마다 한번씩 업데이트
업데이트때마다
화면밖을 벗어나있는 오브젝트는 "벗어남" 타이머가 작동을 시작하고 이중 일정시간이 지난객체는 삭제
*/


#ifdef _xIN_TOOL
XWndBgObj* XWndBgObjLayer::CreateWndBgObj( xnBgObj::xProp *pProp )
{
	if( XBREAK(pProp == nullptr) )
		return nullptr;
	auto pWndExist = Findf( "__bg.%d", pProp->m_idObj );
	if( XBREAK( pWndExist != nullptr) )
		return nullptr;
	XBREAK( pProp->m_strSpr.empty() );
	auto pWndBgObj = new XWndBgObj( *pProp );
// 	auto pWndBgObj = new XWndSprObj( pProp->m_strSpr
// 																, pProp->m_idAct
// 																, pProp->m_vwPos );
	pWndBgObj->SetstrIdentifierf("__bg.%d", pProp->m_idObj );
#ifdef _xIN_TOOL
	pWndBgObj->SetEvent( XWM_CLICKED, this, &XWndBgObjLayer::OnClickBgObj );
#endif // _xIN_TOOL
	Add( pWndBgObj );
	return pWndBgObj;
}

void XWndBgObjLayer::Draw()
{
	XWnd::Draw();
	//
	if( XBaseTool::sIsToolBgObjMode() ) {
		if( XAPP->m_bViewBgObjBoundBox ) {
			//
			for( auto pWnd : m_listItems ) {
				auto bb = pWnd->GetBoundBoxByVisibleFinal();
				GRAPHICS->DrawRect( bb.vLT, bb.vRB, XCOLOR_RGBA( 0, 0, 0, 128 ) );
				auto pWndBgObj = SafeCast<XWndBgObj*>( pWnd );
				if( pWndBgObj && !pWndBgObj->GetProp().m_strSnd.empty() ) {
					PUT_STRING_SHADOW( bb.vLT.x, bb.vLT.y, XCOLOR_YELLOW, _T( "S" ) );
				}
			}
		}
		if( m_pSelected ) {
			auto bb = m_pSelected->GetBoundBoxByVisibleFinal();
			GRAPHICS->DrawRect( bb.vLT, bb.vRB, XCOLOR_RGBA( 255, 0, 0, 200 ) );
		}
	}
}

/**
 @brief 
*/
int XWndBgObjLayer::OnClickBgObj( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	//
	if( XBaseTool::sIsToolBgObjMode() ) {
		m_pSelected = SafeCast<XWndBgObj*>( pWnd );
		if( m_pSelected ) {
			const std::string str = pWnd->GetstrIdentifier();
			const std::string strNum = str.substr( 5 );
			ID idObj = (ID)atoi( strNum.c_str() );
			if( idObj ) {
				m_pPropSelected = XPropBgObj::sGet()->GetpProp( idObj );
#ifdef _xIN_TOOL
				CONSOLE( "OnClickBgObj:id=%d, snd=%s", m_pPropSelected->m_idObj, m_pPropSelected->m_strSnd.c_str() );
#endif // _xIN_TOOL
				XBaseTool::s_pCurr->ResetAutoSave();		//터치하고 그러면 자동세이브가 미뤄지도록.
			}
		}
	}
	return 1;
}

void XWndBgObjLayer::OnRButtonDown( float lx, float ly )
{
	CONSOLE("XWndBgObjLayer::OnRButtonDown");
	m_bRButtDown = true;
	XAPP->SetbDebugDrawArea( FALSE );		// 사각형 못그리게 함.
	m_bMoved = false;
	m_bDragging = false;
	m_vTouchPrev = SCENE_WORLD->GetvMouseWorld();
	if( m_pPropSelected )
		m_PropPrev = *m_pPropSelected;
}
void XWndBgObjLayer::OnRButtonUp( float lx, float ly )
{
	CONSOLE( "XWndBgObjLayer::OnRButtonUp" );
	if( m_bRButtDown && m_bMoved && m_pPropSelected ) {
		if( (int)m_pPropSelected->m_vwPos.x != (int)m_PropPrev.m_vwPos.x 
			&& (int)m_pPropSelected->m_vwPos.y != (int)m_PropPrev.m_vwPos.y )
		XBaseTool::s_pCurr->UpdateAutoSave();
	}
	m_bRButtDown = false;
	XAPP->SetbDebugDrawArea( TRUE );		// 복구
	m_bCopyed = false;
	m_bDragging = false;
	m_PropPrev = xnBgObj::xProp();
}

void XWndBgObjLayer::OnMouseMove( float lx, float ly )
{
	if( XBaseTool::sIsToolBgObjMode() ) {
		if( m_bRButtDown && m_pSelected && m_pPropSelected ) {
			const auto vwCurr = SCENE_WORLD->GetvMouseWorld();
			const auto vDist = vwCurr - m_vTouchPrev;
			if( XE::GetMain()->m_bCtrl && m_bCopyed == false ) {
				if( vDist.Lengthsq() > 5.f * 5.f ) {
					// 선택된 객체의 카피본을 만든다.
					auto pNewProp = XPropBgObj::sGet()->CreateNewProp();
					*pNewProp = *m_pPropSelected;		// 복사
					pNewProp->m_idObj = 0;		// 아이디를 새로 부여받기위해 클리어.
					XPropBgObj::sGet()->AddBgObj( pNewProp );
					m_pPropSelected = pNewProp;
					m_pSelected = CreateWndBgObj( pNewProp );
					m_bCopyed = true;
				}
			}
			m_vTouchPrev = vwCurr;
			m_pPropSelected->m_vwPos += vDist;
			m_pSelected->SetPosLocal( m_pPropSelected->m_vwPos );
			m_bMoved = true;
			m_bDragging = true;
			XBaseTool::s_pCurr->ResetAutoSave();		// 드래그중엔 자동세이브 안되게
		}
	}
}

BOOL  XWndBgObjLayer::OnKeyDown( int keyCode )
{
	if( XBaseTool::sIsToolBgObjMode() ) {
		if( keyCode == XE::KEY_DEL ) {
			if( m_pPropSelected ) {
				if( m_pSelected )
					m_pSelected->SetbDestroy( true );
				XPropBgObj::sGet()->DestroyBgObj( m_pPropSelected->m_idObj );
				m_pPropSelected = nullptr;
				m_pSelected = nullptr;
				XBaseTool::s_pCurr->ResetAutoSave();		//자동세이브가 미뤄지도록.
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif // _xIN_TOOL


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


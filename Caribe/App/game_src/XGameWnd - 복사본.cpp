#include "stdafx.h"
#include "XGameWnd.h"
#include "XSoundMng.h"
#include "client/XAppMain.h"
#include "XGame.h"
#include "XFontMng.h"
#include "XConstant.h"
#include "XWindow.h"
#include "XImageMng.h"
#include "client/XLayout.h"
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
XWndSpotRecon::XWndSpotRecon( /*XAccount *pAcc, */XLegion *pLegion, XSpot *pBaseSpot )
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
	ID idWnd = 1;
	for( auto& res : m_listLoot ) {
		auto pExist = Find( idWnd );
		if( pExist == nullptr ) {
			auto path = XE::MakePath( DIR_UI, XGAME::GetResourceIcon( res.type ) );
			auto pImg = new XWndImage( path, v );
			Add( idWnd, pImg );
			auto pText = new XWndTextString( v.x + 23.f, v.y + 3.f, XE::NumberToMoneyString( (int)res.num ), FONT_NANUM_BOLD, 30.f );
			Add( pText );
		}
		++idWnd;
		v.y += 20.f;
	}
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


/*
////////////////////////////////////////////////////////////////
ID XWndSpot::s_idLastTouch = 0;
XWndSpot::XWndSpot( XSpot *pBaseSpot, LPCTSTR szSpr, ID idAct, const XE::VEC2& vPos )
	: XWndSprObj( szSpr, idAct, vPos ) 
{
	Init();
	m_pBaseSpot = pBaseSpot;
	m_idSpot = pBaseSpot->GetidSpot();
	auto pProp = pBaseSpot->GetpBaseProp();
	if( !pProp->strSpr.empty() ) {	// spr이 따로 지정되어있다면.
		XBREAK( pProp->idAct == 0 );
		SetSprObj( pProp->strSpr.c_str(), pProp->idAct );
	}
	// 이름영역 공통
	m_pTextName = new XWndTextString( XE::VEC2(0), _T(""), FONT_NANUM, 18.0f );
	if( m_pTextName ) {
		m_pTextName->SetstrIdentifier( "text.spot.name" );
		m_pTextName->SetStyle( xFONT::xSTYLE_STROKE );
		Add( m_pTextName );
		m_pTextName->SetbShow( FALSE );	// 이름안나오는 스팟\도 있으니 일단 감춤.
	}
	// 레벨바탕 심볼 이미지
	m_pImg = new XWndImage( PATH_UI("level_bg.png"), 0, 0 );	// 이미지의 사이즈를 정하기 위해 46x46짜리 디폴트 이미지를 지정함.
	if( m_pImg ) {
		m_pImg->SetbShow( FALSE );
		Add( m_pImg );
		// 레벨 텍스트
		m_pLevelText = new XWndTextString( nullptr, FONT_RESNUM, 25.0f );
		if( m_pLevelText ) {
			m_pLevelText->SetbShow( FALSE );
			m_pLevelText->SetStyle( xFONT::xSTYLE_STROKE );
			m_pLevelText->SetColorText( XCOLOR_RGBA(255,204,0,255) );
			m_pLevelText->SetAlign( XE::xALIGN_CENTER );
			m_pImg->Add( m_pLevelText );
			m_pLevelText->SetbActive( FALSE );
		}
	}
	m_psoEff = new XSprObj(_T("spot_eff.spr"));
	m_psoEff->SetAction( 1 );
	m_psoIndicate = new XSprObj(_T("spot_indicate.spr"));
	if( m_pBaseSpot->IsEventSpot() )
		m_psoIndicate->SetAction( 1 );	// 이벤트 스팟일때만 움직이게
	else
		m_psoIndicate->SetAction( 2 );
#ifdef _CHEAT
	// 디버그용 텍스트
	m_pTextDebug = new XWndTextString( -82, -43, _T( "" ) );
	if( m_pTextDebug ) {
		m_pTextDebug->SetStyle( xFONT::xSTYLE_STROKE );
		Add( m_pTextDebug );
	}
	//m_TimerUpdate.Set( 1.f );
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 0.2f );
#endif
	SetbUpdate( true );
}
void XWndSpot::Destroy()
{
	SAFE_DELETE( m_psoIndicate );
	SAFE_DELETE( m_psoEff );
}

void XWndSpot::Update()
{
	XBREAK( GetDestroy() );
	if( m_pBaseSpot->GetbDestroy() ) {
		SetbDestroy( true );
		return;
	}
	// 이름 업데이트
	if( IsEnemySpot() )
		m_psoEff->SetAction( 2 );
	else
		m_psoEff->SetAction( 1 );
	bool bShowName = true;
#pragma message("-----------------IsActive()로 판단하지 말고 보다 구체적인 스팟 상태가 필요함. 리젠대기중/전투가능/터치가능 등등")
#pragma message("----------------또한 아래 각 요소들을 update시 요소개별적으로 하위클래스에게 업데이트 형식을 맞겨야 좀더 관리하기가 좋아짐")
	if( m_pBaseSpot->IsActive() ) {
		if( GetstrName().empty() ) {
			// 이름이 없으면 ?로 표시
			auto pText = xSET_TEXT( this, "text.spot.name", _T("?") );
			if( pText ) {
				if( m_pBaseSpot->GetpBaseProp()->strIdentifier == _T("spot.home") )
					pText->SetText( ACCOUNT->GetstrName() );
				pText->SetColorText( XCOLOR_WHITE );
				auto vSizeText = pText->GetLayoutSize();
				pText->SetPosLocal( XE::VEC2( ( -vSizeText.x ) / 4 - 5.f, 
										(GetSizeLocal().h / 2) - 5.f ) );
			}
		} else {
			auto pText = xSET_TEXT( this, "text.spot.name", GetstrName().c_str() );
			if( pText ) {
				XCOLOR col = XCOLOR_WHITE;
				if( IsEnemySpot() ) {
					// 적이면 전투력 차이에 따라 색을 달리한다.
					if( ACCOUNT->GetPowerExcludeEmpty() == 0 )
						ACCOUNT->UpdatePower();
					if( m_pBaseSpot->GetPower() ) { 
						col = XGAME::GetColorPower( m_pBaseSpot->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
						// 스팟 난이도 아이콘
						int lvHard = XGAME::GetHardLevel( m_pBaseSpot->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
//						int lvHard = ACCOUNT->GetGradeLevel( m_pBaseSpot->GetPower() );
						auto pImg = static_cast<XWndImage*>( Find("img.hard.lv") );
						if( pImg == nullptr ) {
							auto bb = GetBoundBoxLocal();
							pImg = new XWndImage( PATH_UI("world_hard0.png"), XE::VEC2( 0, -bb.GetHeight() ) );
							pImg->SetstrIdentifier( "img.hard.lv" );
							Add( pImg );
							auto vSize = pImg->GetSizeLocal(); 
							pImg->SetX( -(vSize.w * 0.5f) );
							pImg->SetY( bb.vLT.y - vSize.h );
						} else
							pImg->SetbShow( true );
						_tstring strImg = XE::Format( _T( "world_hard%d.png" ), lvHard + 2 );
						pImg->SetSurfaceRes( XE::MakePath( DIR_UI, strImg ) );
					} else
						col = XCOLOR_WHITE;	// 정찰이 안된상태에선 흰색으로 표시
				}
				auto sizeText = pText->GetLayoutSize();
				pText->SetPosLocal( XE::VEC2( ( -sizeText.x ) / 4 - 5.f,
											(GetSizeLocal().y / 2 ) - 5.f ) );
				pText->SetColorText( col );
			} // pText
		}
		if( m_pTextName ) {
			auto pText = m_pTextName;
			// 레벨 배경이미지
			if( m_pImg ) {
				m_pImg->SetbShow( true );
				auto vText = m_pTextName->GetPosLocal();
				if( m_pBaseSpot->IsNpc() ) {
					// npc의 경우는 일반 심볼
					// 현재는 심볼 없이 텍스트로만 표현.
					if( m_pBaseSpot->IsElite() )
						m_pImg->SetSurfaceRes( PATH_UI( "level_bg_elite.png" ) );
					else {
						m_pImg->SetSurfaceRes( PATH_UI( "level_bg.png" ) );
						m_pImg->SetbShow( FALSE );
					}
				} else
				// pc의 경우는 사람얼굴 있는 심볼
				if( IsEnemySpot() ) {
					m_pImg->SetSurfaceRes( PATH_UI( "world_middle_enemy.png" ) );
				} else {
					m_pImg->SetSurfaceRes( PATH_UI( "level_bg.png" ) );
// 					m_pImg->SetSurfaceRes( PATH_UI( "world_middle_ally.png" ) );
				}
				// 일반NPC스팟은 이름을 감춤
				if( m_pBaseSpot->GettypeSpot() == XGAME::xSPOT_NPC && !m_pBaseSpot->IsEventSpot() )
					bShowName = false;
				auto vImg = m_pImg->GetPosLocal();
				auto sizeImg = m_pImg->GetSizeLocal();
				// 이름앞에 레벨심볼 위치 조정
				XE::VEC2 v;
				v.x = vText.x - sizeImg.x - 3;
				v.y = vText.y + (pText->GetLayoutSize().h / 2.f) - (sizeImg.y / 2.f);
				m_pImg->SetPosLocal( v );
				// 레벨 텍스트
				if( m_pLevelText ) {
					m_pLevelText->SetbShow( TRUE );
					int level = 0;
					if( IsEnemySpot() ) {
						level = m_pBaseSpot->GetLevel();
						// 레벨이 지정되어있더라도 파워를 모르면 ?로 표시
						if( m_pBaseSpot->GetPower() == 0 )
							level = 0;
					} else
						level = ACCOUNT->GetLevel();
					if( level )
						m_pLevelText->SetText( XFORMAT( "%d", level ) );
					else {
						if( m_pBaseSpot->GetpBaseProp()->strIdentifier == _T( "spot.home" ) )
							m_pLevelText->SetText( XFORMAT("%d", ACCOUNT->GetLevel()));
						else
							m_pLevelText->SetText( _T( "?" ) );
					}
	//					auto vSizeLevel = m_pLevelText->GetLayoutSize();
	//					m_pLevelText->SetPosLocal( vImg + ( ( sizeImg - XE::VEC2( vSizeLevel.x / 2, vSizeLevel.y ) ) / 2 ) );
				}
			}
			pText->SetbShow( bShowName );
		}
		if( m_pBaseSpot->IsEventSpot() && m_pBaseSpot->IsActive() ) {
			auto pWnd = Find("spr.exclam");
			if( pWnd == nullptr ) {
				auto pExclam = new XWndSprObj( _T( "ui_exclam.spr" ), 1, XE::VEC2(0,0) );
				pExclam->SetstrIdentifier( "spr.exclam" );
				Add( pExclam );
				auto bb = GetBoundBoxLocal();
				auto vSize = pExclam->GetSizeLocal();
				pExclam->SetY( 5.f );
// 				pExclam->SetY( bb.vLT.y - 15.f );
			}
		}
		// 연패했을때 느낌표.
		if( m_pBaseSpot->IsSuccessiveDefeat() &&
			m_pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE ) {
			if( Find( "icon.alert" ) == nullptr )
			{
				auto pMark = new XWndSprObj( _T( "ui_alert.spr" ), 1, 8, 8 );
				pMark->SetstrIdentifier( "icon.alert" );
				Add( pMark );
			}
		}
		else {
			DestroyWndByIdentifier( "icon.alert" );
		}

	} // IsActive
	else {
		if( m_pTextName )
			m_pTextName->SetbShow( FALSE );	// 이름안나오는 스팟\도 있으니 일단 감춤.
		if( m_pImg )
			m_pImg->SetbShow( FALSE );
		if( m_pLevelText )
			m_pLevelText->SetbShow( FALSE );
		auto pImg = Find( "img.hard.lv" );
		if( pImg )
			pImg->SetbShow( false );
		
	}
	// 스팟 별점
	if( ACCOUNT->GetpQuestMng()->IsHaveGetStarQuest() ) {
		if( !m_pBaseSpot->IsEventSpot() ) {
			bool bShowStar = false;
			int numStar = ACCOUNT->GetNumSpotStar( m_pBaseSpot->GetidSpot() );
			if( m_pBaseSpot->GettypeSpot() == xSPOT_CASTLE
				|| m_pBaseSpot->GettypeSpot() == xSPOT_NPC ) {
				if( !m_pBaseSpot->IsEventSpot() ) {
					if( numStar < 3 && numStar >= 0 )
						bShowStar = true;
				}
			}
			if( bShowStar ) {
				const float wStar = 13.f;
				XE::VEC2 v( -((wStar * 3.f) * 0.5f), -53.f );
				for( int i = 0; i < 3; ++i ) {
					auto pImg = xGET_IMAGE_CTRLF( this, "img.star.%d", i + 1 );
					if( numStar >= i+1 ) {
						// on
						if( pImg == nullptr ) {
							pImg = new XWndImage( PATH_UI( "common_etc_smallstar.png" ), v );
							pImg->SetstrIdentifierf( "img.star.%d", i + 1 );
							Add( pImg );
						} else {
							pImg->SetSurfaceRes( PATH_UI( "common_etc_smallstar.png" ) );
						}
					} else {
						// off
						if( pImg == nullptr ) {
							pImg = new XWndImage( PATH_UI( "common_etc_smallstar_empty.png" ), v );
							pImg->SetstrIdentifierf( "img.star.%d", i + 1 );
							Add( pImg );
						} else {
							pImg->SetSurfaceRes( PATH_UI( "common_etc_smallstar_empty.png" ) );
						}
					}
					v.x += wStar;
				}
			} else {
				// 별표시 할필요 없으면 걍 윈도 삭제하면 됨.
				for( int i = 0; i < 3; ++i )
					DestroyWndByIdentifierf("img.star.%d", i+1 );
			}
		}
	}

#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( m_pTextDebug ) {
		if( XAPP->GetbDebugMode() ) {
			_tstring strText = GetstrDebugText();
			m_pTextDebug->SetbShow( TRUE );
			m_pTextDebug->SetText( strText );
			auto vSize = m_pTextDebug->GetLayoutSize();
			auto vPos = XE::VEC2( -vSize.x / 4, GetSizeLocal().y / 2 );
			vPos.y += 15.f;
			m_pTextDebug->SetPosLocal( vPos );
		} else {
			m_pTextDebug->SetbShow( FALSE );
		}
	}
#endif
	XWndSprObj::Update();
}

void XWndSpot::OnAutoUpdate()
{
	if( m_pTextDebug ) {
		if( XAPP->GetbDebugMode() ) {
			_tstring strText = GetstrDebugText();
			m_pTextDebug->SetText( strText );
		}
	}
}

void XWndSpot::OnLButtonDown( float lx, float ly )
{
	XWndSprObj::OnLButtonDown( lx, ly );
}

void XWndSpot::OnLButtonUp( float lx, float ly )
{
	s_idLastTouch = getid();
	XWndSprObj::OnLButtonUp( lx, ly );
}

int XWndSpot::Process( float dt )
{
	m_psoEff->FrameMove( dt );
	m_psoIndicate->FrameMove( dt );
	return XWndSprObj::Process( dt );
}

/ **
 @brief 
* /
void XWndSpot::Draw()
{
#ifdef WIN32
	if( XWnd::s_pMouseOver && XWnd::s_pMouseOver->getid() == getid() ) {
		XE::VEC2 v = GetPosFinal();
		XE::VEC2 size = GetSizeFinal();
		GRAPHICS->DrawCircle( v.x, v.y, size.Length(), XCOLOR_RED );
		XE::VEC2 vMouse = INPUTMNG->GetMousePos();
		vMouse += XE::VEC2( 30, 30 );
		XE::VEC2 vWorld = m_pBaseSpot->GetpBaseProp()->vWorld;
		auto pProp = m_pBaseSpot->GetpBaseProp();
		_tstring str = XFORMAT( "pos:%d,%d\n%s(%d)\ncode=%d\narea:%d",
								(int)vWorld.x, (int)vWorld.y,
								pProp->strIdentifier.c_str(), pProp->idSpot,
								pProp->idCode,
								pProp->idArea );
		PUT_STRINGF( vMouse.x, vMouse.y, XCOLOR_WHITE, "%s", str.c_str() );
	}
#endif
	if( m_timerBlink.IsOn() ) {
		if( m_timerBlink.IsOver() ) {
			m_timerBlink.Off();
			XWndSprObj::SetColor( XCOLOR_WHITE );
		} else {
//			float wave = abs( cosf( m_timerBlink.GetPassSec() * 3.f ) );
			auto lerpTime = m_timerBlink.GetSlerp();
			float wave = XE::xiCos( lerpTime * 8.f, 0, 1.f, 0 );
//			float wave = ::cosf( m_timerBlink.GetPassSec() * 3.f );
			XCOLOR col = XCOLOR_RGBA_FLOAT( 1.f, wave, wave, 1.f );
			XWndSprObj::SetColor( col );
		}
	}
	//
	m_psoEff->Draw( GetPosFinal() );
	if( getid() == s_idLastTouch ) {
		if( m_pBaseSpot->IsActive() )
			m_psoIndicate->Draw( GetPosFinal() );
	}
	XWndSprObj::Draw();
#ifdef _CHEAT
	if( XAPP->m_bDebugMode && XAPP->m_bDebugViewBoundBoxSpot ) {
		XE::xRECT rect = GetBoundBox();
		GRAPHICS->DrawRect( rect.vLT, rect.vRB, XCOLOR_BLUE );
		const auto vPos = GetPosFinal();
		const float sizeCross = 20.f;
		GRAPHICS->DrawHLine( vPos.x - (sizeCross * 0.5f), vPos.y, sizeCross, XCOLOR_BLUE );
		GRAPHICS->DrawVLine( vPos.x, vPos.y - (sizeCross * 0.5f), sizeCross, XCOLOR_BLUE );
	}
#endif // _CHEAT
}

_tstring XWndSpot::GetstrName()
{
	return m_pBaseSpot->GetstrName();
}

////////////////////////////////////////////////////////////////
XWndCastleSpot::XWndCastleSpot( XSpotCastle* pSpot )
	: XWndSpot( pSpot, SPR_CASTLE, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
#ifdef _CHEAT
	SetAutoUpdate( 0.1f );
#endif
//	SetbUpdate( true );

}

void XWndCastleSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->GetidOwner() == 0 ) {
		strOut += XTEXT(2035);	// 정찰하지 않음.
	} else
	if( m_pSpot->GetidOwner() == ACCOUNT->GetidAccount() ) {
		// xxx/시간당
		m_pSpot->GetStrProduce( strOut );
//		strOut += XE::Format( XTEXT( 80201 ), XE::NumberToMoneyString( int( m_pSpot->GetProducePerMin() * 60.f ) ) );
	} else {
		for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
			auto type = (XGAME::xtResource)i;
			int amount = m_pSpot->GetLootAmount( type );
			if( amount ) {
				_tstring str = XE::NumberToMoneyString( amount );
				strOut += XFORMAT( "%s:%s\n", XGAME::GetStrResource( type ), str.c_str() );
			}
		}
	}
}

_tstring XWndCastleSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
//		strText += XE::Format(_T(":(%d)\n"), (int)m_pSpot->GetTimerCalc().GetsecPassTime());
		strText += XE::Format( _T( "(Lv%d)(sc:%d)" ), m_pSpot->GetLevel(), m_pSpot->GetScore() );
		strText += XE::Format( _T( ":(%d sec)\n" ), (int)m_pSpot->GetTimerCalc().GetsecRemainTime() );
		strText += XE::Format( _T( "%s" ), m_pSpot->GetStrLocalStorage().c_str() );
	}
#endif
	return strText;
}

_tstring XWndCastleSpot::GetstrName()
{
	_tstring strText;
	auto pSpot = SafeCast<XSpotCastle*>( GetpBaseSpot() );
	if( pSpot->GetidOwner() == ACCOUNT->GetidAccount() )
		strText = ACCOUNT->GetstrName();
	else
		strText = GetpBaseSpot()->GetstrName();
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		if( pSpot->GetidOwner() != ACCOUNT->GetidAccount() )
			strText += XFORMAT("(%d)", pSpot->GetidOwner() );
	}
#endif 
	return strText;
}

void XWndCastleSpot::Update( void )
{
	if( m_pSpot->IsDestroy() ) {
		SetbDestroy( TRUE );
		return;
	}
	if( ACCOUNT->GetidAccount() == m_pSpot->GetidOwner() )
		XWndSprObj::SetAction( 1 );
	else 
		XWndSprObj::SetAction( 2 );
  XWndSpot::Update();
}

bool XWndCastleSpot::IsEnemySpot() 
{
	return ( ACCOUNT->GetidAccount() != m_pSpot->GetidOwner() );
}

////////////////////////////////////////////////////////////////
XWndJewelSpot::XWndJewelSpot( XSpotJewel* pSpot ) 
	: XWndSpot( pSpot, SPR_JEWEL, 
				pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	// 방어도
	auto pText = new XWndTextString( XE::VEC2( 0 ), _T( "" ), FONT_NANUM, 20.0f );
	if( pText ) {
		pText->SetstrIdentifier( "text.def.mine" );
		pText->SetStyle( xFONT::xSTYLE_STROKE );
		Add( pText );
		// 광산레벨바탕 심볼 이미지
		auto pImg = new XWndImage( PATH_UI( "bg_level.png" ), 0, 0 );
		if( pImg ) {
			pImg->SetstrIdentifier("img.bg.level.mine");
			Add( pImg );
			// 레벨 텍스트
			pText = new XWndTextString( nullptr, FONT_NANUM, 20.0f );
			pText->SetstrIdentifier("text.level.mine");
			pText->SetStyle( xFONT::xSTYLE_STROKE );
			pText->SetAlignCenter();
			pImg->Add( pText );
		}
	}

}

void XWndJewelSpot::Destroy()
{
}

bool XWndJewelSpot::IsEnemySpot() 
{
	return ( ACCOUNT->GetidAccount() != m_pSpot->GetidOwner() );
}

void XWndJewelSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->GetidOwner() == 0 ) {
		strOut += XTEXT(2035);	// 정찰하지 않음.
	} else
	if( m_pSpot->GetidOwner() == ACCOUNT->GetidAccount() )
	{
		// 시간당 생산량으로 환산해서 보여줌.
		// 생산량:%d/시간
		strOut += XE::Format( XTEXT( 2036 ), (int)( m_pSpot->GetpProp()->GetProduce() * 60.f ) );
		strOut += _T("\n");
		// 현재보유량
		strOut += XE::Format( XTEXT( 2037 ), m_pSpot->GetLocalStorageAmount() );
	} else
	{
		//strOut += _T( "획득 가능 자원:\n" );
		strOut += XFORMAT( "%s:%d\n", XGAME::GetStrResource( XGAME::xRES_JEWEL ), m_pSpot->GetlootJewel() );
	}
  strOut += XFORMAT( "lvDef:%d\n", m_pSpot->GetlevelMine() );
  strOut += XFORMAT( "Def:%d\n", m_pSpot->GetDefense() );
}


void XWndJewelSpot::Update( void )
{
	if (m_pSpot) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
		// 광산 방어도 및 광산레벨표시
		auto pTextDef = xGET_TEXT_CTRL( this, "text.def.mine" );
		if( pTextDef ) {
			pTextDef->SetText( XFORMAT( "%s:%d", XTEXT( 2153 ), m_pSpot->GetDefense() ) );
			auto sizeDef = pTextDef->GetLayoutSize();
			pTextDef->SetPosLocal( XE::VEC2( ( -sizeDef.x ) / 4, 
												-(GetSizeLocal().h / 2) ) );
			auto vTextDef = pTextDef->GetPosLocal();
			XWnd *pImg = Find("img.bg.level.mine");
			if( pImg ) {
				auto pTextLv = xGET_TEXT_CTRL( this, "text.level.mine" );
				if( pTextLv ) {
					pTextLv->SetText( XFORMAT( "%d", m_pSpot->GetlevelMine() ) );
				}
				auto sizeImg = pImg->GetSizeFinal();
				// 이름앞에 레벨심볼 위치 조정
				pImg->SetPosLocal( vTextDef.x - sizeImg.x - 3,
					vTextDef.y + ( sizeDef.y - sizeImg.y ) );
			}
		}
	}
	XWndSpot::Update();
}

_tstring XWndJewelSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		if( m_pSpot->GetidOwner() != 0 )
			strText = XE::Format( _T( "%s(%d)<->적:%d\n보석보유량:%d/%d\nlv:%d:광산레벨:%d\ndef:%d\nidx army:%d\nloot:%d" ),
									m_pSpot->GetszName(),
									m_pSpot->GetidOwner(),
									m_pSpot->GetidMatchEnemy(),
									(int)m_pSpot->GetlsLocal().numCurr,
									(int)m_pSpot->GetlsLocal().maxSize,
									m_pSpot->GetLevel(),
									m_pSpot->GetlevelMine(),
									m_pSpot->GetDefense(),
									m_pSpot->GetidxLegion(),
									m_pSpot->GetlootJewel() );
		else
			strText = _T( "?" );
	}
#endif
	return strText;
}

///////////////////////////////////////////////////////////////
XWndSulfurSpot::XWndSulfurSpot( XSpotSulfur* pSpot )
	: XWndSpot( pSpot, SPR_SULFUR, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;

	SetSizeLocal( 45, 45 );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 1.f );
#endif
}

void XWndSulfurSpot::Destroy()
{
}

void XWndSulfurSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->IsActive() ) {
		strOut += XE::Format(_T("%s\n"), XE::NumberToMoneyString(m_pSpot->GetnumSulfur()));
	} else
		strOut += XTEXT(2030);	// 비어있음.
}


void XWndSulfurSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
		if( m_pSpot->IsActive() )
			SetAction( 2 );
		else
			SetAction( 1 );
	}
	XWndSpot::Update();
}

_tstring XWndSulfurSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "%s(%d):%d(%d)" ), m_pSpot->GetszName(),
								m_pSpot->GetLevel(),
								(int)m_pSpot->GetnumSulfur(),
								(int)( m_pSpot->GettimerSpawn().GetsecRemainTime() ) );
	}
#endif
	return strText;
}

#ifdef WIN32
void XWndSulfurSpot::Draw( void )
{
	XWndSpot::Draw();
}
#endif // WIN32

////////////////////////////////////////////////////////////////
XWndMandrakeSpot::XWndMandrakeSpot( XSpotMandrake* pSpot )
	: XWndSpot( pSpot, SPR_MANDRAKE, 
				pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;

	// 연승 텍스트
	auto pText = new XWndTextString( 0, 0, _T( "" ) );
	if( pText ) {
		m_pTextWin = pText;
		pText->SetStyleStroke();
		pText->SetColorText( XCOLOR_RGBA(255,192,0,255) );
		Add( pText );
		pText->SetbShow( FALSE );
	}

;
}

void XWndMandrakeSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->GetidOwner() == 0 ) {
		//strOut += _T("?");
		strOut += XTEXT(2035);	// 정찰하지 않음.
	} else
	if( m_pSpot->GetidOwner() == ACCOUNT->GetidAccount() ) {
		// 시간당 생산량으로 환산해서 보여줌.
		strOut += XE::Format( XTEXT(2036), (int)(m_pSpot->GetpProp()->GetProduce() * 60.f) );
		strOut += _T("\n");
		// 연승횟수: n연승
		strOut += XFORMAT("%s:%d%s\n", XTEXT(2046), m_pSpot->GetWin(), XTEXT(2047) );
		// 누적보상:만드레이크 0000
		strOut += XFORMAT("%s:\n%s x%d\n", XTEXT(2048), STR_MANDRAKE, m_pSpot->GetReward() );
	} else {
		strOut += XFORMAT("%s\n", XTEXT(2045));	// 승리보상
		strOut += XFORMAT("     %s", XE::NumberToMoneyString(m_pSpot->Getloot()));
	}

}


void XWndMandrakeSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
		if( m_pTextWin ) {
			if( m_pSpot->GetWin() > 1 ) {
				m_pTextWin->SetText( XE::Format( XTEXT( 2049 ), m_pSpot->GetWin() ) );
				XE::VEC2 sizeText = m_pTextWin->GetLayoutSize();
				m_pTextWin->SetPosLocal( XE::VEC2( -sizeText.w / 2, -37 ) );
				m_pTextWin->SetbShow( TRUE );
			} else
				m_pTextWin->SetbShow( FALSE );
		} 
	}
	XWndSpot::Update();
}

bool XWndMandrakeSpot::IsEnemySpot()
{
	return ( ACCOUNT->GetidAccount() != m_pSpot->GetidOwner() );
}

_tstring XWndMandrakeSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
// 		_tstring strName = ( m_pSpot->GetidOwner() == 0 ) ? _T( "?" ) : m_pSpot->GetszName();
		strText = XE::Format( _T( "idOwner=%d<->%d\nMandrake:%d/%d\nlv:%d\nwin:%d\nreward:%d" ),
								m_pSpot->GetidOwner(),
								m_pSpot->GetidEnemy(),
								(int)m_pSpot->GetlsLocal().numCurr,
								(int)m_pSpot->GetlsLocal().maxSize,
								m_pSpot->GetLevel(),
								m_pSpot->GetWin(),
								m_pSpot->GetReward() );
	}
#endif
	return strText;
}

///////////////////////////////////////////////////////////////
XWndNpcSpot::XWndNpcSpot( XSpotNpc* pSpot )
	: XWndSpot( pSpot, SPR_NPC, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	SetbHideDeactive( true );		// 스팟이 비활성화상태일때 이름을 표시하지 않는다.
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 1.f );
#endif
}
void XWndNpcSpot::SetSprSpot()
{
	if( m_pSpot->IsActive() ) {
// 		auto clan = m_pSpot->GetpProp()->clan;
// 		switch( clan )
// 		{
// 		case XGAME::xCL_CROW:		SetSprObj( SPR_NPC_CROW );	break;
// 		case XGAME::xCL_IRONLORD:	SetSprObj( SPR_NPC_IRONLORD );	break;
// 		case XGAME::xCL_FREEDOM:	SetSprObj( SPR_NPC_FREEDOM );	break;
// 		case XGAME::xCL_ANCIENT:	SetSprObj( SPR_NPC_ANCIENT );	break;
// 		case XGAME::xCL_FLAME:		SetSprObj( SPR_NPC_FLAME );	break;
// 		default:
			if( GetpBaseSpot()->GetpBaseProp()->strSpr.empty() ) {
				// 1레벨 지역이고 계정렙이 5이하이면 야만족만 나오게 한다.
				if( m_pSpot->GetpAreaProp()->lvArea == 1 && ACCOUNT->GetLevel() <= 5 )
					SetSprObj( SPR_NPC3 );	// 야만족
				else
					SetSprObj( SPR_NPC );	// 제국 부대
			} else
				SetSprObj( GetpBaseSpot()->GetpBaseProp()->strSpr, 1 );
// 			break;
// 		}
	} else {
		SetSprObj( _T("spot_npc_empty.spr"), 2 );
	}
}
void XWndNpcSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->IsActive() )
	{
		if( m_pSpot->IsReconed() )
			strOut += XFORMAT("%s", XE::NumberToMoneyString(m_pSpot->GetnumResource()));
		else
			strOut += _T("?");
	} else
		strOut += XTEXT(2030);	// 비어있음.
}


void XWndNpcSpot::Update( void )
{
#ifdef _xIN_TOOL
	{
		auto pProp = PROP_WORLD->GetpProp( m_idSpot );
		if( pProp == nullptr ) {
			SetbDestroy( TRUE );
			return;
		}
	}
#endif
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() )
		{
			SetbDestroy( TRUE );
			return;
		}
		SetSprSpot();
		if( m_pSpot->IsActive() )
			SetAction( 1 );		// 부대있는 모습
		else
			SetAction( 2 );		// 없는 모습
 	}
	XWndSpot::Update();
}

_tstring XWndNpcSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "lv%d:id:%d(%d)" ), m_pSpot->GetLevel(),
							getid(),
							(int)m_pSpot->GettimerSpawn().GetsecRemainTime() );
	}
#endif
	return strText;
}

_tstring XWndNpcSpot::GetstrName()
{
	_tstring strName = GetpBaseSpot()->GetstrName();
	return strName;
}

///////////////////////////////////////////////////////////////
XWndDailySpot::XWndDailySpot( XSpotDaily* pSpot )
	: XWndSpot( pSpot, SPR_DAILY, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	// 
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 0.5f );
#endif
}

void XWndDailySpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
}

void XWndDailySpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot ) {		
// 		strOut += XFORMAT( "Lv%d %s\n", m_pSpot->GetLevel(), m_pSpot->GetszName() );
// 		if( m_pSpot->GettimerEnter().IsOn() ) {
// 			int secRemain = (int)m_pSpot->GettimerEnter().GetsecRemainTime();	// 남은시간(초)
// 			int hour, min, sec;
// 			XTimer2::sGetHourMinSec( secRemain, &hour, &min, &sec );
// 			strOut += XE::Format( _T( "남은시간:\n<%0d:%02d:%02d>\n" ),
// 									hour, min, sec );
// 		} else {
// 			strOut += _T( "미입장\n" );
// 		}
		strOut += XFORMAT( "Lv%d %s\n", m_pSpot->GetLevel(), m_pSpot->GetszName() );
		strOut += XE::Format( _T( "입장횟수:%d/%d" ), m_pSpot->GetnumEnter(), 
													XSpotDaily::xNUM_ENTER );
	}
}

_tstring XWndDailySpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
	}
#endif
	return strText;
}

//////////////////////////////////////////////////////////////
XWndSpecialSpot::XWndSpecialSpot( XSpotSpecial* pSpot )
	: XWndSpot( pSpot, SPR_SPECIAL, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	// 
}

void XWndSpecialSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
}

void XWndSpecialSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot )
	{
		strOut += XFORMAT("Lv%d %s\n", m_pSpot->GetLevel() + m_pSpot->GetidxRound(), 
										m_pSpot->GetszName() );
		int secRemain = (int)m_pSpot->GettimerRecharge().GetRemainSec();	// 남은시간(초)
		int hour, min, sec;
		XTimer2::sGetHourMinSec( secRemain, &hour, &min, &sec );
		strOut += XE::Format( _T( "충전시간: <%d:%d>\n라운드:%d\n총 입장횟수:%d\n남은 입장횟수:%d" ),
								min, sec,
								m_pSpot->GetidxRound() + 1,
								m_pSpot->GetnumEnter(), 
								m_pSpot->GetnumEnterTicket() );
	}
}

_tstring XWndSpecialSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
	}
#endif
	return strText;
}


//////////////////////////////////////////////////////////////////////////
XWndCampaignSpot::XWndCampaignSpot( XSpotCampaign* pSpot )
	: XWndSpot( pSpot, SPR_CAMPAIGN, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	// Campaign이름
// 	XWndTextString *pText = new XWndTextString( 11, -30, pSpot->GetszName(), GAME->GetpfdSystem(), XCOLOR_RGBA(185,0,225,255) );
// 	pText->SetStyle( xFONT::xSTYLE_STROKE );
// 	Add( pText );
// 	m_pText = pText;
// 	SetbUpdate( TRUE );
}

void XWndCampaignSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
// #ifdef _CHEAT
// 		_tstring str;
// 		if( XAPP->m_bDebugMode )
// 			str = XE::Format( _T( "Lv%d %s:id:%d" ), m_pSpot->GetLevel(), 
// 													m_pSpot->GetszName(),
// 													getid() );
// 		else
// 			str = XFORMAT( "Lv%d %s", m_pSpot->GetLevel(), m_pSpot->GetszName() );
// #else
// 		_tstring str = XE::Format( _T( "%s(%d)" ), m_pSpot->GetszName(),
// 													m_pSpot->GetLevel() );
// #endif
// 		m_pText->SetText( str.c_str() );
// 		XE::VEC2 vSizeLabel = m_pText->GetLayoutSize();
// 		XE::VEC2 vPosText = m_pText->GetPosLocal();
// 		XE::VEC2 vPosSpot = GetPosFinal();
// 		XE::VEC2 vSizeSpot = GetSizeFinal();
// //		XE::VEC2 vLT = vPosSpot + vSizeSpot / 2.f;	// 스팟의 중심좌표
// //		vLT -= vSizeLabel / 2.f;	// 스팟을 중심으로 텍스트레이블을 중앙정렬했을때 텍스트 좌상귀
// 		vPosText.x = -(vSizeLabel.w / 2.f);
// 		m_pText->SetPosLocal( vPosText );
	}
	XWndSpot::Update();
}

_tstring XWndCampaignSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "Lv%d %s:id:%d" ), m_pSpot->GetLevel(),
			m_pSpot->GetszName(),
			getid() );
	}
#endif
	return strText;
}

//////////////////////////////////////////////////////////////////////////
XWndVisitSpot::XWndVisitSpot( XSpotVisit* pSpot )
	: XWndSpot( pSpot, _T( "spot_visit.spr" ), xRandom(3)+1, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	// Visit이름
// 	m_pText = new XWndTextString( XE::VEC2( 0, 0 ), _T("") );
// 	if( m_pText )
// 	{
// 		m_pText->SetStyle( xFONT::xSTYLE_STROKE );
// 		if( pSpot->IsEventSpot() )
// 			m_pText->SetColorText( XCOLOR_YELLOW );
// 		Add( m_pText );
// 	}
// 	SetbUpdate( TRUE );
}

void XWndVisitSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
	GetpImg()->SetbShow( FALSE );
}

_tstring XWndVisitSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "%s:id:%d" ), m_pSpot->GetszName(), getid() );
	}
#endif
	return strText;
}


//////////////////////////////////////////////////////////////////////////
XWndCashSpot::XWndCashSpot( XSpotCash* pSpot )
	: XWndSpot( pSpot, SPR_CASH, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
}

void XWndCashSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
		if( m_pSpot->IsActive() )
			SetbShow( TRUE );
		else
			SetbShow( FALSE );
	}
}

_tstring XWndCashSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XFORMAT( "id:%d", getid() );
	}
#endif
	return strText;
}

//////////////////////////////////////////////////////////////////////////
XWndCommonSpot::XWndCommonSpot( XSpotCommon* pSpot )
  : XWndSpot( pSpot, SPR_COMMON, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	auto pProp = pSpot->GetpProp();
}

void XWndCommonSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
}

bool XWndCommonSpot::IsEnemySpot()
{
	auto& idsSpot = GetpBaseSpot()->GetpBaseProp()->strIdentifier;
	if( idsSpot == _T("spot.home") )
		return false;
	if( !m_pSpot->IsCampaignType() )
		return false;
	return true;
}

_tstring XWndCommonSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "id:%d" ), getid() );
	}
#endif
	return strText;
}

*/

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
		pImg = new XWndImage(PATH_UI("bg_name.png"), 2, 40);
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
		m_pName->DrawString(vPos + XE::VEC2(-8, 38), m_pHero->GetpProp()->strName.c_str());
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

/****************************************************************
* @brief 
*****************************************************************/
// int XWndSquadInLegion::OnDrop( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	ID idDrag = p2;
// 	CONSOLE("OnDrop:drag=%d, drop=%d", idDrag, GetID() );
// 	return 1;
// }

// int XWndSquadInLegion::s_idxSelectedSquad = -1;
// XWndSquadInLegion::XWndSquadInLegion( XHero *pHero, const XE::VEC2& vPos, XLegion *pLegion, bool bDrawFrame, bool bDragDrop, bool bQuestion )
// 	: XWnd( vPos.x, vPos.y )
// {
// 	Init();
// //	XE::MakePath(DIR_UI, _T("rect_squad.png"))
// 	m_bDrawFrame = bDrawFrame;
// 	m_pLegion = pLegion;
// 	if( bDrawFrame ) {
// 		// 사각 프레임
//  		auto pImg = new XWndImage( XE::MakePath( DIR_UI, _T( "corps_unit_bg_m.png" ) ), 0, 0 );
//  		Add( pImg );
// 	}
// 	SetFace( pHero, bQuestion );
// 	SetAutoSize();
// 	// drop&drop이 가능하도록 설정
// 	if( bDragDrop ) {
// 		SetDragWnd( TRUE );
// 		SetDropWnd( TRUE );
// 	}
// 	for (int i = 0; i < 4; ++i)
// 		m_pStar[i] = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("common_etc_bicstar.png")));
// //	m_pNameCard = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("common_etc_namecard_s.png")));
// 	//
// 	m_pName = FONTMNG->CreateFontObj(FONT_NANUM, 18.f);
// 	m_pName->SetAlign(XE::xALIGN_HCENTER);
// 	m_pName->SetStyle(xFONT::xSTYLE_STROKE);
// 	m_pName->SetLineLength(60.f);
// 	m_pCrown = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("corps_crown.png")));
// }
// 
// XWndSquadInLegion::XWndSquadInLegion( int idxSquad, const XE::VEC2& vPos, XLegion *pLegion, bool bDrawFrame, bool bDragDrop )
// 	: XWnd( vPos.x, vPos.y )
// {
// 	Init();
// 	m_bDrawFrame = bDrawFrame;
// 	m_pLegion = pLegion;
// 	m_idxSquad = idxSquad;
// 	if( bDrawFrame ) {
// 		// 사각 프레임
//  		auto pImg = new XWndImage( XE::MakePath( DIR_UI, _T( "corps_unit_bg_m.png" ) ), 0, 0 );
//  		Add( pImg );
// 	}
// 	SetAutoSize();
// 	// drop&drop이 가능하도록 설정
// 	if( bDragDrop ) {
// 		SetDragWnd( TRUE );
// 		SetDropWnd( TRUE );
// 	}
// 	for (int i = 0; i < 4; ++i)
// 		m_pStar[i] = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("common_etc_bicstar.png")));
// //	m_pNameCard = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("common_etc_namecard_s.png")));
// 	//
// 	m_pName = FONTMNG->CreateFontObj(FONT_NANUM, 18.f);
// 	m_pName->SetAlign(XE::xALIGN_HCENTER);
// 	m_pName->SetStyle(xFONT::xSTYLE_STROKE);
// 	m_pName->SetLineLength(60.f);
// 	m_pCrown = IMAGE_MNG->Load(TRUE, XE::MakePath(DIR_UI, _T("corps_crown.png")));
// }
// void XWndSquadInLegion::Destroy()
// {
// //	SAFE_RELEASE2( IMAGE_MNG, m_psfcSuperior );
// 	SAFE_RELEASE2( IMAGE_MNG, m_psfcSelected );
// 	SAFE_RELEASE2( IMAGE_MNG, m_psfcFace );
// 	for (int i = 0; i < XNUM_ARRAY(m_pStar); ++i)
// 		SAFE_RELEASE2(IMAGE_MNG, m_pStar[i]);
// //	SAFE_RELEASE2(IMAGE_MNG, m_pNameCard);
// 	SAFE_DELETE(m_pName);
// 	SAFE_RELEASE2(IMAGE_MNG, m_pCrown);
// }
// 
// void XWndSquadInLegion::Update()
// {
// 	if( m_pHero ) {
// 		if( m_bPlayer ) {
// //			int idxSquad = m_pLegion->GetSquadronIdxByHeroSN( m_pHero->GetsnHero() );
// 			XBREAK( m_idxSquad < 0 );
// 			if( s_idxSelectedSquad >= 0 && m_idxSquad == s_idxSelectedSquad ) {
// 				if( m_psfcSelected == nullptr ) {
// 					m_psfcSelected = IMAGE_MNG->Load( PATH_UI("common_unit_bg_s_select.png") );
// 					XBREAK( m_psfcSelected == nullptr );
// 				}
// 			} else {
// 				SAFE_RELEASE2( IMAGE_MNG, m_psfcSelected );
// 			}
// 		} else {
// 			// enemy
// 			ID idAct = 1;
// 			switch( m_nSuperior ) {
// 			case 2:	idAct = 2;	break;
// 			case 1: idAct = 1;	break;
// 			case 0: idAct = 5;	break;
// 			case -1: idAct = 3; break;
// 			case -2: idAct = 4;	break;
// 			default:
// 				XBREAK(1);
// 				break;
// 			}
// 			if( s_idxSelectedSquad >= 0 ) {
// 				auto pWndSpr = static_cast<XWndSprObj*>( Find( "spr.arrow" ) );
// 				if( pWndSpr == nullptr ) {
// 					pWndSpr = new XWndSprObj( _T( "ui_upstat.spr" ), idAct, -6, 10 );
// 					pWndSpr->SetstrIdentifier( "spr.arrow" );
// 					Add( pWndSpr );
// 				}
// 				pWndSpr->SetAction( idAct );
// 			}
// //			SAFE_RELEASE2( IMAGE_MNG, m_psfcSuperior );
// // 			if( s_idxSelectedSquad >= 0 ) {
// // 				switch( m_nSuperior ) {
// // 				case 2:
// // 					m_psfcSuperior = IMAGE_MNG->Load( PATH_UI("legion_stat_up2.png") );
// // 					break;
// // 				case 1:
// // 					m_psfcSuperior = IMAGE_MNG->Load( PATH_UI( "legion_stat_up.png" ) );
// // 					break;
// // 				case 0:
// // 					m_psfcSuperior = IMAGE_MNG->Load( PATH_UI( "legion_stat_same.png" ) );
// // 					break;
// // 				case -1:
// // 					m_psfcSuperior = IMAGE_MNG->Load( PATH_UI( "legion_stat_down.png" ) );
// // 					break;
// // 				case -2:
// // 					m_psfcSuperior = IMAGE_MNG->Load( PATH_UI( "legion_stat_down2.png" ) );
// // 					break;
// // 				default:
// // 					XBREAK(1);
// // 					break;
// // 				} 
// // 			}
// 		}
// 	} else {
// //		SAFE_RELEASE2( IMAGE_MNG, m_psfcSuperior );
// 		DestroyWndByIdentifier( "spr.arrow" );
// 	}
// 	XWnd::Update();
// }
// 
// void XWndSquadInLegion::SetFace( XHero *pHero, bool bQuestion )
// {
// 	if( pHero == nullptr && pHero == m_pHero )
// 		if( m_bQuestion == bQuestion )
// 			return;
// 	if( pHero && m_pHero )
// 		if( pHero->GetsnHero() == m_pHero->GetsnHero() )
// 			if( pHero->GetUnit() == m_Unit )
// 				return;
// 	m_bQuestion = bQuestion;
// 	SAFE_RELEASE2( IMAGE_MNG, m_psfcFace );
// 	DestroyWndByIdentifier("img.hero");
// 	DestroyWndByIdentifier("img.slot.hero");
// // 	DestroyWndByIdentifier("img.unit");
// 	DestroyWndByIdentifier("img.slot.unit");
// 	if( pHero == nullptr ) {
// 		DestroyWndByIdentifier("img.name.tag");
// 	}
// 	m_pHero = pHero;
// 	if( pHero ) {
// 		m_Unit = pHero->GetUnit();
// 		bool bResourceSquad = false;
// 		auto pSquad = m_pLegion->GetSquadronByHeroSN( pHero->GetsnHero() );
// 		if( pSquad )
// 			bResourceSquad = m_pLegion->IsResourceSquad( pHero->GetsnHero() );
// 		// 영웅 얼굴
// 		auto pImg = new XWndImage( XE::MakePath( DIR_IMG, pHero->GetpProp()->strFace.c_str() ),
// 										4, 4 );
// 		pImg->SetstrIdentifier( "img.hero" );
// 		pImg->SetScaleLocal(0.67f);
// 		Add( pImg );
// 		if( !pHero->GetbLive() ) {
// 			auto vPos = pImg->GetPosLocal();
// 			auto pWnd = new XWndRect( vPos, pImg->GetSizeLocal(), XCOLOR_RGBA( 255, 0, 0, 128 ) );
// 			Add( pWnd );
// 			auto pText = new XWndTextString( vPos+XE::VEC2(5,5), XTEXT(2127), FONT_NANUM, 20.f, XCOLOR_RGBA(255,128,128,255) );
// 			Add( pText );
// 		}
// 		// 영웅얼굴 서피스 버전(drag를 위함)
// 		m_psfcFace = IMAGE_MNG->Load( TRUE, 
// 								XE::MakePath( DIR_IMG, pHero->GetpProp()->strFace.c_str() ));
// 		if( m_bDrawFrame ) {
// 			// 알파뚫린 영웅 프레임으로 한번더 덮어준다.
// 			pImg = new XWndImage( XE::MakePath( DIR_UI, _T( "common_unit_bg_s.png" ) ), 0, 0 );
// 			pImg->SetstrIdentifier( "img.slot.hero" );
// 			Add( pImg );
// 		}
// 		if( bResourceSquad ) {
// 			if( Find("img.resource") == nullptr ) {
// 				pImg = new XWndImage( PATH_UI( "world_top_res_all.png" ), -6, -9 );
// 				pImg->SetstrIdentifier( "img.resource" );
// 				Add( pImg );
// 			}
// 		}
// 		xSET_SHOW( this, "img.resource", bResourceSquad );
// 		// 이름 판
// 		XWnd *pExist = nullptr;
// 		pExist = Find("img.name.tag");
// 		if( pExist != nullptr ) {
// 			pExist->SetbDestroy(true);
// 		}
// 		pImg = new XWndImage(PATH_UI("common_etc_namecard_s.png"), -6, 36);
// 		pImg->SetstrIdentifier("img.name.tag");
// 		pImg->SetScaleLocal(1.5f, 1.f);
// 		Add(pImg);
// 
// 		XPropUnit::xPROP *pPropUnit = PROP_UNIT->GetpProp( pHero->GetUnit() );
// 		if( pPropUnit ) {
// 			// 유닛용 원형 프레임
// 			auto pWndUnit = new XWndCircleUnit( pHero->GetUnit(), XE::VEC2(36,19), pHero );
// 			pWndUnit->SetbShowLevelSquad( true );
// 			pWndUnit->SetstrIdentifier("img.slot.unit");
// 			Add( pWndUnit );
// // 			pImg = new XWndImage(PATH_UI("corps_legionnaire_bg.png"), 36, 19);
// // 			pImg->SetstrIdentifier("img.slot.unit");
// // 			Add(pImg);
// // 			// 원형유닛 초상화
// // 			auto strResUnit = XGAME::GetResUnitSmall( pHero->GetUnit() );
// // 			pImg = new XWndImage(strResUnit, 36.f, 19.f);
// // 			pImg->SetstrIdentifier("img.unit");
// // 			Add(pImg);
// // 			if( pHero ) {
// // 				auto pText = new XWndTextString( 0, 0,
// // 					XFORMAT( "%d", pHero->GetlevelSquad() ), FONT_NANUM, 15.f );
// // 				pText->SetLineLength( pImg->GetSizeFinal().w );
// // 				pText->SetAlignHCenter();
// // 				pText->SetStyleStroke();
// // 				pImg->Add( pText );
// // 			}
// 		}
// //		auto pText = new XWndTextStringArea( )
// 	} else
// 	{
// 		m_nGrade = 0;
// 		if( bQuestion ) {
// 			// 영웅얼굴을 ?로 표시
// 			auto pImg = new XWndImage( PATH_IMG( "hero_q.png" ), 4, 4 );
// 			pImg->SetstrIdentifier( "img.hero" );
// 			pImg->SetScaleLocal( 0.67f );
// 			Add( pImg );
// 		}
// // 		XWndImage *pImg = new XWndImage(XE::MakePath(DIR_UI, _T("corps_legionnaire_bg.png")), 46, 19);
// // 		pImg->SetstrIdentifier("img.slot.unit");
// // 		Add(pImg);
// 	}
// }
// 
// void XWndSquadInLegion::Draw( void )
// {
// 	const XE::VEC2 vPos = GetPosFinal();
// 	XWnd::Draw();
// 	//
// 	if( m_psfcSelected ) {
// 		m_psfcSelected->Draw( vPos );
// 	}
// 	///< 
// 	if (m_pHero) {
// 		for (int i = 0; i < m_nGrade; ++i) {
// 			if (m_pStar[i] != NULL) {
// 				m_pStar[i]->SetScale(0.6f);
// 				m_pStar[i]->Draw(vPos.x - 1 + i * 13, vPos.y - 1);
// 			}
// 		}
// // 		if (m_pNameCard)
// // 		{
// // 			m_pNameCard->SetScale(1.5f, 1.f);
// // 			m_pNameCard->Draw(vPos.x - 6.f, vPos.y + 36.f);
// // 		}
// 		if (m_pCrown && m_pLegion && m_pLegion->GetpLeader() == m_pHero) {
// 			m_pCrown->Draw(vPos.x + 40, vPos.y);
// 		}
// 		XCOLOR col = XGAME::GetGradeColor( m_pHero->GetGrade() );
// 		m_pName->SetColor( col );
// 		m_pName->DrawString(vPos + XE::VEC2(-8, 40), m_pHero->GetpProp()->strName.c_str());
// 		m_pName->SetColor( XCOLOR_WHITE );
// 		m_pName->DrawString( vPos, XE::Format(_T("Lv%d"), m_pHero->GetLevel()) );
// // 		if( m_psfcSuperior )
// // 			m_psfcSuperior->Draw( vPos + XE::VEC2(-10,0) );
// 	}
// }
// 
// void XWndSquadInLegion::DrawDrag( const XE::VEC2& vMouse )
// {
// 	if( m_psfcFace )
// 	{
// 		XE::VEC2 vSize = m_psfcFace->GetSize();
// 		m_psfcFace->SetfAlpha( 0.5f );
// 		m_psfcFace->Draw( vMouse - vSize / 2.f );
// 	}
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// // int XWndSquadInLegion::OnDrop( XWnd* pWnd, DWORD p1, DWORD p2 )
// // {
// // 	ID idDrag = p2;
// // 	CONSOLE("OnDrop:drag=%d, drop=%d", idDrag, GetID() );
// // 	return 1;
// // }

////////////////////////////////////////////////////////////////
XWndArrow4Abil::XWndArrow4Abil( const XE::VEC2& vStart, const XE::VEC2& vEnd )
	: XWnd( vStart.x, vStart.y )
{
	Init();
	m_psfcArrow = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "skill_arrow.png" ) ) );
	m_vStart = vStart;
	m_vEnd = vEnd;
	SetbTouchable( FALSE );		// 터치안되는 컨트롤
}

void XWndArrow4Abil::Destroy()
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcArrow );
}

void XWndArrow4Abil::Draw()
{
	XE::VEC2 vPos = GetPosFinal();
	XE::VEC2 vDir = m_vEnd - m_vStart;		// 시작점과 끝점의 벡터를 얻음.
	XE::VEC2 vStart = vPos;
	XE::VEC2 vEnd = vPos + vDir;
	vEnd.x -= XPropTech::ADJ_X;
	m_psfcArrow->DrawDirection( vStart, vEnd );
}

////////////////////////////////////////////////////////////////
XWndAbilButton::XWndAbilButton( XHero *pHero
															, XGAME::xtUnit unit
															, XPropTech::xNodeAbil *pNodeAbil
															, const XE::VEC2& vPos )
	: XWnd(vPos.x, vPos.y)
	, XWndButton( 0, 0, _T("common_bg_abil.png"), nullptr )
{
	Init();
	m_Unit = unit;
	m_idNode = pNodeAbil->idNode;
	m_pProp = pNodeAbil;
	XBREAK( pHero == nullptr );
	m_snHero = pHero->GetsnHero();
//	XTRACE( "wndAbilButton contructer" );
	XBREAK( pNodeAbil->strIcon.empty() );
	_tstring strRes = XE::MakePath( DIR_IMG, pNodeAbil->strIcon.c_str() );
//	SetpSurface( 0, strRes.c_str() );
//	XTRACE( "set wndabilbutt img finished" );
// 	XWndImage *pBg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("common_bg_itemblank.png")), -4, -4);
// 	Add(pBg);
	auto vScale = GetScaleLocal();
	XE::VEC2 vOfs(3);
	auto pIcon = new XWndImage( strRes, vOfs * vScale );
	pIcon->SetScaleLocal( 0.724f );
	pIcon->SetstrIdentifier("img.abil");
	Add( pIcon );
//	auto pCircle = new XWndImage(PATH_UI("skill_unit_bg_level.png"), 35.f, 32.f );
	auto pCircle = new XWndImage( PATH_UI( "skill_unit_bg_level.png" ), 23.f, 23.f );
	pCircle->SetstrIdentifier("img.circle");
	Add( pCircle );
	// 아이콘 위에 현재 연구남은시간 표시
	auto pText = new XWndTextString( 0, 0, _T(""), FONT_NANUM, 15.f );
	pText->SetstrIdentifier("text.research");
//	pText->SetLineLength( 54.f );
	pText->SetLineLength( 38.f );
	pText->SetAlign( XE::xALIGN_HCENTER );
	pText->SetStyleStroke();
	Add( pText );
	pText->SetbShow( FALSE );
//	XTRACE( "abil level finished" );
	SetbUpdate( TRUE );
}

void XWndAbilButton::Destroy()
{
}
void XWndAbilButton::Update()
{
	auto& research = ACCOUNT->GetResearching();
	if( m_pProp == nullptr )
		return;
// 	SetpSurface( 0, strRes.c_str() );
	auto pImg = xGET_IMAGE_CTRL( this, "img.abil" );
	auto pHero = ACCOUNT->GetHero( m_snHero );
	if( XASSERT(pHero) && pImg ) {
		_tstring strRes = XE::MakePath( DIR_IMG, m_pProp->strIcon.c_str() );
		pImg->SetSurfaceRes( strRes );
		bool bEnable = false;
		// 활성화가 되려면 우선 필요레벨이 되어야 한다.
		bEnable = ACCOUNT->IsEnableAbil( pHero, SCENE_TECH->GetunitSelected(), m_pProp );
		if( bEnable || m_bInRightPanel )
			pImg->SetblendFunc( XE::xBF_MULTIPLY );
		else
			// 오픈가능한 레벨이 아직 안됐으면 그레이스케일로 만듬.
			pImg->SetblendFunc( XE::xBF_GRAY );
	// 	if( bEnable || m_bInRightPanel )
	// 		SetAlphaLocal( 1.f );
	// 	else
	// 		// 오픈가능한 레벨이 아직 안됐으면 그레이스케일로 만듬.
	// 		SetAlphaLocal( 0.5f );		// 나중에 그레이스케일 셰이더로 바꿀것.
		// 락 상태는 자물쇠 그림 그림.
		XWnd *pCircle = Find( "img.circle" );
		if( pCircle ) {
			XWnd *pLock = Find( "img.lock" );		// 자물쇠 그림
			const auto abil = pHero->GetAbilNode( m_Unit, m_idNode );
//			XBREAK( pAbil == nullptr );
			if( abil.IsLock() ) {
				if( pLock == nullptr ) {
					pLock = new XWndImage( PATH_UI( "lock_small.png" ), 4, 2 );
					pLock->SetstrIdentifier( "img.lock" );
					pCircle->Add( pLock );
				}
				if( m_pText ) {
					this->DestroyWnd( m_pText );
					m_pText = nullptr;
				}
			} else {
				// 락이 풀렸는데 자물쇠그림이 있으면 없앰.
				if( pLock )
					pCircle->DestroyWnd( pLock );
				if( m_pText == nullptr ) {
					m_pText = new XWndTextString( XE::VEC2( 0 ),
													pCircle->GetSizeLocal(),
													_T( "" ),
													FONT_RESNUM, 18.f );
					m_pText->SetAlign( XE::xALIGN_CENTER );
					pCircle->Add( m_pText );
				}
				if( m_pText ) {
					m_pText->SetText( XE::Format( _T( "%d" ), abil.point ) );
					if( abil.point > 0 )
						m_pText->SetColorText( XCOLOR_RGBA( 255, 156, 0, 255 ) );
					else
						m_pText->SetColorText( XCOLOR_GRAY );
				}
			} // !IsLock
		} // pCircle
	} // pHero
	XWndButton::Update();
}

int XWndAbilButton::Process( float dt )
{
	// 이특성이 현재 연구중이면.
	if( ACCOUNT->GetResearching().GetidAbil() == m_pProp->idNode ) {
		auto pHero = ACCOUNT->GetHero( ACCOUNT->GetResearching().GetsnHero() );
		if( pHero ) {
			xSET_SHOW( this, "text.research", !m_bInRightPanel );
	//		XAccount::xAbil *pAbil = ACCOUNT->GetAbilNode( m_Unit, m_idNode );
			_tstring strRemain;
			auto& costAbil = pHero->GetCostAbilCurr( /*m_Unit*/ );
			SCENE_TECH->GetRemainResearchTime( &strRemain/*, m_pProp*/, costAbil.sec );
	// 		SCENE_TECH->GetRemainResearchTime( &strRemain, m_pProp, pAbil );
			xSET_TEXT( this, "text.research", strRemain.c_str() );
		}
	}
	return XWndButton::Process( dt );
}

void XWndAbilButton::Draw()
{
	XWndButton::Draw();
}


void XWndAbilButton::SetNode( XGAME::xtUnit unit, XPropTech::xNodeAbil *pProp )
{
	XBREAK( pProp == nullptr );
	m_pProp = pProp;
	m_Unit = unit;
	m_idNode = pProp->idNode;

}

////////////////////////////////////////////////////////////////
// XWndCampaign::XWndCampaign( XSpotCampaign *pSpot )
// 	: XWndView(_T("common_bg_popup.png") )
// {
// 	Init();
// 	m_pSpot = pSpot;
// }
// 
// BOOL XWndCampaign::OnCreate()
// {
// 	XArrayLinearN<XWnd*, 256> ary;
// 
// 	// 캠페인 제목
// 	{
// 		auto pText = new XWndTextString( XE::VEC2(156, 25), m_pSpot->GetspCampObj()->GetNameCamp(), FONT_NANUM_BOLD, 40.f );
// 		pText->SetLineLength( 211.f );
// 		pText->SetAlign( XE::xALIGN_HCENTER );
// 		pText->SetStyle( xFONT::xSTYLE_STROKE );
// 		Add( pText );
// 		// 용맹아이콘
// 		auto pImg = new XWndImage( PATH_UI( "icon_brave.png" ), 413.f, 74.f );
// 		Add( pImg );
// 		_tstring str = XE::Format(_T("%d"), ACCOUNT->GetptBrave() );
// 		pText = new XWndTextString( XE::VEC2(444, 84), str.c_str(), FONT_NANUM_BOLD, 30.f );
// 		Add( pText );
// 	}
// #ifdef _CHEAT
// 	if( XAPP->m_bDebugMode )
// 	{
// 		auto pProp = m_pSpot->GetpProp();
// 		_tstring str = XFORMAT("%s(%s)", pProp->strCamp.c_str(), pProp->strIdentifier.c_str() );
// 		auto pText = new XWndTextString( XE::VEC2( 20, 59 ), str, FONT_NANUM_BOLD, 18.f );
// 		Add( pText );
// 	}
// #endif // cheat
// 	int currStage = m_pSpot->GetspCampObj()->GetidxLastUnlock();	// 현재 깨야할 스테이지
// 	int numStage = m_pSpot->GetspCampObj()->GetNumStages();
// 	XE::VEC2 v(35,114);
// 	for( int i = 0; i < numStage; ++i )
// 	{
// 		auto spStageObj = m_pSpot->GetspCampObj()->GetspStage( i );
//     XBREAK( spStageObj == nullptr );
//     auto spPropStage = spStageObj->GetspPropStage();
// 		_tstring strPng;
// 		if( i == currStage )
// 			strPng = _T("stage_elem.png");
// 		else if( i < currStage )
// 			strPng = _T("stage_clear.png");
// 		else if( i == numStage-1 )
// 			strPng = _T("stage_elem_boss.png");
// 		else
// 			strPng = _T("stage_elem_lock.png");
// //		XWndImage *pImg = new XWndImage( resPng, 0.f, 171.f );
// 		// 스테이지 큰상자(버튼)
// 		auto pButt = new XWndButton( v.x, v.y, strPng.c_str(), nullptr );
// // 		if( i == currStage )
// 		if( spStageObj->IsAbleTry(currStage) )
// 			pButt->SetEvent( XWM_CLICKED, this, &XWndCampaign::OnClickStage, (DWORD)i );
// 		else
// 			pButt->SetbActive( FALSE );
// 		Add( pButt );
// 		// 아래 작은상자
// 		auto pImg = new XWndImage( PATH_UI("stage_under.png"), v.x, v.y + 86.f );
// 		Add( pImg );
// //		if( i >= currStage )
// 		// 도전가능한 스테이지면 정보를 표시
// 		if( spStageObj->IsAbleTry(currStage) )
// 		{
// 			// 아직 안깬스테이지 만
// 			int levelLimit = spPropStage->levelLimit;
// 			int point = spPropStage->point;
// 			XE::VEC2 v(53,4);
// 			if( levelLimit > 0 )
// 			{
// 				_tstring str = XE::Format( _T( "Lv:%d" ), levelLimit );
// 				auto pText = new XWndTextString( v, str, FONT_NANUM, 15.f );
// 				pButt->Add( pText );
// 				if( ACCOUNT->GetLevel() < spPropStage->levelLimit )
// 					pText->SetColorText( XCOLOR_RED );
// 				else
// 					pText->SetColorText( XCOLOR_WHITE );
// 				v.y += 8.f;
// 			}
// 			if( point > 0 )
// 			{
// 				auto pImgBrave = new XWndImage( PATH_UI( "icon_brave.png" ), 53, 12 );
// 				pImgBrave->SetScaleLocal( 0.4f );
// 				pButt->Add( pImgBrave );
// 				_tstring str = XE::Format( _T( "   :-%d" ), point );
// 				auto pText = new XWndTextString( v, str, FONT_NANUM, 15.f );
// 				pButt->Add( pText );
// 				if( ACCOUNT->GetptBrave() < point )
// 					pText->SetColorText( XCOLOR_RED );
// 				else
// 					pText->SetColorText( XCOLOR_WHITE );
// 				v.y += 8.f;
// 			}
// 				
// 			if( !spPropStage->sidDropItem.empty() )
// 			{
// 				auto pPropItem = PROP_ITEM->GetpProp( spPropStage->sidDropItem );
// 				if( XASSERT( pPropItem ) )
// 				{
// 					auto pWndItem = new XWndStoragyItemElem( 0, 4, pPropItem->idProp );
// 					pWndItem->SetScaleLocal( 0.65f );
// 					pWndItem->SetEventItemTooltip();
// 					pImg->Add( pWndItem );
// 					pWndItem->AutoLayoutHCenter();
// 				}
// 			}
// 		}
// 		if( spPropStage->maxTry > 0 )
// 		{
// 			_tstring str = XFORMAT( "clear:%d/%d", spStageObj->GetnumClear(), spPropStage->maxTry );
// 			auto pText = new XWndTextString( XE::VEC2( 0, 46 ), str, FONT_NANUM, 15.f );
// 			if( spStageObj->GetnumClear() >= spPropStage->maxTry )
// 				pText->SetColorText( XCOLOR_RED );
// 			else
// 				pText->SetColorText( XCOLOR_WHITE );
// 			pImg->Add( pText );
// 		}
// 		// 스테이지 번호
// 		{
// // 			_tstring str = XE::Format( _T( "%d" ), i + 1 );
// // 			XWndTextString *pText = new XWndTextString( 0, -12, str );
// // 			pButt->Add( pText );
// 		}
// 		ary.Add( pButt );
// 		v.x += 90.f;
// 	}
// 	AutoLayoutHCenterWithAry( ary, 35.f );
// 	///< 
// 	XWndButtonString *pButt = new XWndButtonString( 0.f, 286.f, 
// 													XTEXT(2), 
// 													XCOLOR_WHITE, 
// 													FONT_NANUM_BOLD, 
// 													30.f, 
// 													_T("common_butt_mid.png") );
// 	pButt->SetEvent( XWM_CLICKED, this, &XWndCampaign::OnOk );
// 	Add( pButt );
// 	pButt->AutoLayoutHCenter();
// 	return TRUE;
// }
// 
// void XWndCampaign::Destroy()
// {
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndCampaign::OnOk( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnOk");
// 	//
// 	SetbDestroy( TRUE );
// 	return 1;
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndCampaign::OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickStage");
// 	int idxStage = (int)p1;
// 	// 어느스팟의 몇번째 스테이지를 선택했는지 서버로 보낸다.
// 	auto spStageObj = m_pSpot->GetspCampObj()->GetspStage( idxStage );
//   if( XBREAK( spStageObj == nullptr ) )
//     return 1;
//   auto spPropStage = spStageObj->GetspPropStage();
// 	if( spPropStage->levelLimit != 0 &&
// 		  ACCOUNT->GetLevel() < spPropStage->levelLimit )
// 	{
// 		XWND_ALERT("%s", XTEXT(2089) );
// 		return 1;
// 	}
// 	if( ACCOUNT->GetAP() < spPropStage->point )
// 	{
// 		XWND_ALERT("%s", _T("Not enough AP"));
// 		return 1;
// 	}
// 	GAMESVR_SOCKET->SendReqSpotAttack( GAME, m_pSpot->GetidSpot(), idxStage );
// // 	GAMESVR_SOCKET->SendReqClickStageInCampaign( GAME, 
// // 												m_pSpot->GetidSpot(), 
// // 												m_pSpot->GetsnSpot(), 
// // 												idxStage, spPropStage->idProp );
// 		
// 	return 1;
// }
// 
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
  if( bAutoUpdate || 1 )
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
/**
 @brief 정식 퀘스트 목록 UI
*/
XWndQuestList2::XWndQuestList2( XLayout *pLayout, float x, float y )
	: XWndView( pLayout, "quest_list" )
{
	Init();
	//
	m_pLayout = pLayout;
	SetbUpdate( TRUE );
	SetbAnimationEnterLeave( FALSE );
	// 반복퀘가 없어서 잠시 막음.
//	xSetButtHander( this, this, "butt.quest.change", &XWndQuestList2::OnClickChange );
}

void XWndQuestList2::Destroy()
{
}

/**
 @brief 해당탭에 업데이트(느낌표)할 퀘스트가 현재 있는가.
*/
bool XWndQuestList2::IsUpdate( bool bRepeatTab )
{
	// 현재 퀘스트 리스트를 받아온다.
	XArrayLinearN<XQuestObj*, 256> aryQuests;
	if( bRepeatTab )
		ACCOUNT->GetpQuestMng()->GetQuestsToAryWithRepeat( &aryQuests );
	else
		ACCOUNT->GetpQuestMng()->GetQuestsToAryWithNormal( &aryQuests );
	for( int i = 0; i < aryQuests.size(); ++i )
	{
		auto pObj = aryQuests[ i ];
		if( pObj->IsUpdateAlert() || pObj->IsAllComplete() )
			return true;
	}
	return false;
}

void XWndQuestList2::Update()
{
	auto pList = GetCtrl<XWndList*>( "list.quest" );
	if( XASSERT(pList) ) {
		// ui_alert땜에 자동으로 계산하면 크기가 커져버린다. 그래서 32로 고정시켜둠.
		pList->SetsizeFixed( XE::VEC2(32,32) );
		// 현재 퀘스트 리스트를 받아온다.
		XArrayLinearN<XQuestObj*, 256> aryQuests;
		if( m_bRepeat )
			ACCOUNT->GetpQuestMng()->GetQuestsToAryWithRepeat( &aryQuests );
		else
			ACCOUNT->GetpQuestMng()->GetQuestsToAryWithNormal( &aryQuests );
		int size = aryQuests.size();
		// 이제 aryQuest는 항상 소트된채로 온다.
		XArrayLinearN<XWnd*, 256> aryChilds;
		pList->GetListWnds( &aryChilds );
		// 없어진 퀘스트는 지움
		XARRAYLINEARN_LOOP_AUTO( aryChilds, pWnd ) {
			auto pQuestObj = ACCOUNT->GetpQuestMng()->GetQuestObj( pWnd->getid() );
			if( pQuestObj == nullptr || pQuestObj->GetbDestroy() ) {
				pList->DelItem( pWnd->getid() );
			}
			if( pQuestObj ) {
				auto pWndElem = SafeCast<XWndQuestElem*>( pWnd );
				if( pWndElem && pWndElem->IsValid() == false )
					pList->DelItem( pWnd->getid() );
					
			}
		} END_LOOP;
		for( int i = 0; i < aryQuests.size(); ++i ) {
			auto pObj = aryQuests[ i ];
			XWnd *pElem = pList->Find( pObj->GetidProp() );
			if( pElem == nullptr ) {
				pElem = new XWndQuestElem( pObj );
				pElem->SetstrIdentifier( SZ2C(pObj->GetstrIdentifer().c_str()) );
				if( pElem ) {
					pList->AddItem( pObj->GetidProp(), pElem );
				}
			}
			if( pElem ) {
				// 소트를 위해서 항상 퀘의 업뎃시간으로 갖고 있는다.
				
				pElem->SetPriority( pObj->GetsecUpdate() );
				if( pObj->IsAllComplete() ) {
					auto pQuestion = xGET_SPROBJ_CTRL( pElem, "icon.question");
					if( pQuestion == nullptr ) {
						pQuestion = new XWndSprObj( _T( "ui_question.spr" ), 1, XE::VEC2( 16, 19 ) );
						pQuestion->SetstrIdentifier( "icon.question" );
						pElem->Add( pQuestion );
					}
				} else {
					pElem->DestroyWndByIdentifier("icon.question");
					// 퀘완료 ?뜰때는 느낌표 안나오게 하려고 여기다 둠.
					if( pObj->GetstateAlert() == XQuestObj::xMS_MUST_HAVE ) {
						pElem->DestroyWndByIdentifier( "icon.alert" );	// 퀘를 받아서 !떠잇는 상태에서 바로 퀘를 완료하면 이렇게 들어올수 있다.
						auto pMark = new XWndSprObj( _T( "ui_alert.spr" ), 1, 26, 5 );
						pMark->SetstrIdentifier( "icon.alert" );
						pElem->Add( pMark );
						pObj->SetstateAlert( XQuestObj::xMS_HAVE );
						//SOUNDMNG->OpenPlaySoundOneSec(26);
					}
				}
				if( pObj->GetstateAlert() == XQuestObj::xMS_MUST_DEL) {
					pElem->DestroyWndByIdentifier("icon.alert");
					pObj->SetstateAlert( XQuestObj::xMS_NONE );
				}
			}
		}
		pList->Sort( []( XWnd* pWnd1, XWnd* pWnd2 )->
			bool {
			if( pWnd1->GetPriority() > pWnd2->GetPriority() )
				return true;
			return false;
			}
		);
		pList->SetbUpdate( TRUE );
//		pList->SortPriority();
		pList->SetEvent( XWM_SELECT_ELEM, this, &XWndQuestList2::OnSelectQuest );
//		pList->SetAutoSize();
	} // if( XASSERT(pList) ) {
	auto pButt = xGET_BUTT_CTRL(this, "butt.quest.change");
	if( pButt )	{
		if( m_bRepeat )
			pButt->SetpSurface( 0, PATH_UI( "butt_quest2.png" ) );
		else
			pButt->SetpSurface( 0, PATH_UI( "butt_quest1.png" ) );
		
		// 반대편 탭의 업데이트가 있는가.
		auto pExist = pButt->Find( "icon.alert" );	// 버튼의 느낌표
		if( IsUpdate( !m_bRepeat ) ) {
			if( pExist == nullptr ) {
				// 없으면 새로 만듬.
				auto pMark = new XWndSprObj( _T( "ui_alert.spr" ), 1, 30, 10 );
//				pMark->SetScaleLocal( 0.5f );
				pMark->SetstrIdentifier( "icon.alert" );
				pButt->Add( pMark );
				pExist = pMark;
				SOUNDMNG->OpenPlaySoundOneSec(26);
			}
			pExist->SetbShow( TRUE );
		} else {
			if( pExist )
				pExist->SetbShow( FALSE );	// 업데이트가 없으면 느낌표 감춤.
		}
	}
	XWndView::Update();
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndQuestList2::OnSelectQuest( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	//
	ID idProp = p2;
//	auto pPropQuest = XQuestProp::sGet()->GetpProp( idProp );
	auto pQuestObj = ACCOUNT->GetpQuestMng()->GetQuestObj( idProp );
	if( pQuestObj ) {
		CONSOLE( "OnSelectQuest:%s", pQuestObj->GetstrIdentifer().c_str() );
		pQuestObj->UpdatesecClicked( false );
	}
	///< 
	auto pWndInfo = new XWndQuestInfo( pQuestObj );
	SCENE_WORLD->Add( pWndInfo );
	pWndInfo->SetbModal( TRUE );
	pQuestObj->SetstateAlert( XQuestObj::xMS_MUST_DEL );	// 확인했으므로 느낌표 지움.

	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndQuestList2::OnClickChange( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickChange");
	//
	m_bRepeat = !m_bRepeat;
	auto pList = GetCtrl<XWndList*>( "list.quest" );
	if( pList )
	{
		pList->DestroyAllItem();
	}
	SetbUpdate( true );
	return 1;
}


////////////////////////////////////////////////////////////////
XWndQuestElem::XWndQuestElem( XQuestObj *pQuestObj )
	: XWndImage( NULL, 0, 0 )
{
	Init();
	m_pQuestObj = pQuestObj;
	float scale = 1.f;
	_tstring strIcon = pQuestObj->GetpProp()->strIcon;
	_tstring strSubIcon;
	if( strIcon.empty() ) {
		switch( pQuestObj->GetQuestType() )
		{
		case XGAME::xQC_EVENT_LEVEL:			strIcon = _T( "q_level.png" );			break;
		case XGAME::xQC_EVENT_OPEN_AREA:			strIcon = _T( "q_area.png" );			break;
		case XGAME::xQC_EVENT_CLEAR_SPOT: {
			strIcon = _T( "q_battle.png" );	// 디폴트
			ID idSpot = pQuestObj->GetidSpotOfCond();
			auto pBaseSpot = sGetpWorld()->GetSpot( idSpot );
			if( XASSERT(pBaseSpot) ) {
				if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE )
					strIcon = _T( "q_user.png" );
				else 
				if( pBaseSpot->GettypeSpot() == XGAME::xSPOT_CAMPAIGN )
					strIcon = _T( "q_camp.png" );
			}
		} break;
		case XGAME::xQC_EVENT_CLEAR_SPOT_TYPE: {
			strIcon = _T( "q_battle.png" );	// 디폴트
			auto typeSpot = pQuestObj->GettypeSpotByCond();
			if( typeSpot == XGAME::xSPOT_CASTLE )
				strIcon = _T( "q_user.png" );
		} break;
		case XGAME::xQC_EVENT_CLEAR_SPOT_CODE:
		case XGAME::xQC_EVENT_CLEAR_STAGE:			
			strIcon = _T( "q_battle.png" );		break;
		case XGAME::xQC_EVENT_RECON_SPOT:
		case XGAME::xQC_EVENT_RECON_SPOT_TYPE:		
			strIcon = _T( "q_search.png" );		break;
		case XGAME::xQC_EVENT_VISIT_SPOT:
		case XGAME::xQC_EVENT_VISIT_SPOT_TYPE:		
			strIcon = _T( "q_visit.png" );		break;
		case XGAME::xQC_EVENT_GET_HERO:			
			strIcon = _T( "q_hero.png" );			break;
		case XGAME::xQC_EVENT_KILL_HERO:
		case XGAME::xQC_EVENT_CLEAR_QUEST:		
			strIcon = _T( "q_battle.png" );		
			break;
		case XGAME::xQC_EVENT_DEFENSE:			strIcon = _T( "q_castle.png" );			break;
		case XGAME::xQC_EVENT_GET_RESOURCE:
		case XGAME::xQC_EVENT_HAVE_RESOURCE: {
			strIcon = _T("q_empty.png");
// 			SetSurface( XE::MakePath( DIR_ICON, _T( "q_empty.png" ) ) );
			ID idItem = pQuestObj->GetCondItem();
			strSubIcon = XGAME::GetResourceIconBig( (XGAME::xtResource)idItem );
			strSubIcon = XE::MakePath( DIR_ICON, strSubIcon );
			scale = 0.57f;
		} break;
		case XGAME::xQC_EVENT_GET_ITEM:
		case XGAME::xQC_EVENT_HAVE_ITEM: {
			strIcon = _T( "q_empty.png" );
// 			SetSurface( XE::MakePath(DIR_ICON,_T("q_empty.png")) );
			ID idItem = pQuestObj->GetCondItem();
			auto pProp = PROP_ITEM->GetpProp( idItem );
			if( XASSERT( pProp ) )
				strSubIcon = XE::MakePath( DIR_IMG, pProp->strIcon );
			scale = 0.57f;
		} break;
		case XGAME::xQC_EVENT_GET_STAR: 			strIcon = _T( "q_star.png" );			break;
		case XGAME::xQC_EVENT_HERO_LEVEL:			strIcon = _T( "q_hero_lv.png" );			break;
		case XGAME::xQC_EVENT_HERO_LEVEL_SQUAD:			strIcon = _T( "q_squad_lv.png" );			break;
		case XGAME::xQC_EVENT_HERO_LEVEL_SKILL:			strIcon = _T( "q_skill_lv.png" );			break;	
		case XGAME::xQC_EVENT_UI_ACTION:
		case XGAME::xQC_EVENT_TRAIN_QUICK_COMPLETE:		
			strIcon = _T( "q_action.png" );	break;
		case XGAME::xQC_EVENT_RESEARCH_ABIL:	strIcon = _T("q_research.png");		break;
		case XGAME::xQC_EVENT_UNLOCK_UNIT:	strIcon = _T( "q_unlock_unit.png" );		break;
		case XGAME::xQC_EVENT_HIRE_HERO:	strIcon = _T( "q_hero.png" );		break;
		case XGAME::xQC_EVENT_HIRE_HERO_NORMAL:	strIcon = _T( "q_hero.png" );		break;
		case XGAME::xQC_EVENT_HIRE_HERO_PREMIUM:	strIcon = _T( "q_hire_high.png" );		break;
		case XGAME::xQC_EVENT_SUMMON_HERO:	strIcon = _T( "q_summon.png" );		break;
		case XGAME::xQC_EVENT_PROMOTION_HERO:	strIcon = _T( "q_promotion.png" );		break;
		case XGAME::xQC_EVENT_BUY_CASH:	strIcon = _T( "q_buy_cash.png" );		break;
		case XGAME::xQC_EVENT_BUY_ITEM:	strIcon = _T( "q_buy_item.png" );		break;
		case XGAME::xQC_EVENT_ATTACK_SPOT:	strIcon = _T( "q_attack.png" );		break;
		case XGAME::xQC_EVENT_CLEAR_GUILD_RAID:	strIcon = _T( "q_win_raid.png" );		break;
		default:
			strIcon = _T( "quest_dummy.png" );
//			XBREAK(1);
			break;
		}
	}
	if( !strIcon.empty() ) {
		strIcon = XE::MakePath( DIR_ICON, strIcon.c_str() );
		SetSurface( strIcon );
	}
	if( !strSubIcon.empty() ) {
		auto pImg = new XWndImage( strSubIcon, 2, 2 );
		pImg->SetScaleLocal( scale );
		Add( pImg );
	}
// 	if( !strSubIcon.empty() ) {
// 		switch( pQuestObj->GetQuestType() )
// 		{
//  		case XGAME::xQC_EVENT_GET_RESOURCE:
//  		case XGAME::xQC_EVENT_HAVE_RESOURCE:
// 		case XGAME::xQC_EVENT_GET_ITEM:
// 		case XGAME::xQC_EVENT_HAVE_ITEM: {
// 			auto pImg = new XWndImage( strSubIcon, 2, 2 );
// 			pImg->SetScaleLocal( scale );
// 			Add( pImg );
// 		} break;
// 		default:
// 			strIcon = XE::MakePath( DIR_ICON, strIcon.c_str() );
// 			SetSurface( strIcon );
// 			break;
// 		}
// 	} else {
// 	}
	auto pText = new XWndTextString( XE::VEC2(0,25), _T(""), FONT_RESNUM, 13.f );
	pText->SetLineLength(32.f);
	pText->SetAlign( XE::xALIGN_HCENTER );
	pText->SetStyleStroke();
	Add( pText );
	m_pText = pText;
	if( m_pQuestObj->IsRepeat() )
	{
		auto pImg = new XWndImage( PATH_UI("icon_loop.png"), 0, 0 );
		pImg->SetScaleLocal( 0.3f );
		Add( pImg );
	}
	SetbUpdate( TRUE );
}

void XWndQuestElem::Destroy()
{
}

void XWndQuestElem::Update()
{
	if( m_pQuestObj && m_pQuestObj->GetbDestroy() )
		m_pQuestObj = nullptr;
	_tstring str;
	if( m_pQuestObj ) {
		m_pQuestObj->GetQuestObjectiveNum( &str );
		if( !str.empty() && m_pText )
			m_pText->SetText( str );
		if( m_pQuestObj->GetbUpdate() ) {
			m_pQuestObj->SetbUpdate( false );
			m_timerBlink.Set( 5.f );
			m_pQuestObj->UpdatesecClicked( false );	// 퀘가 업데이트 되면 이것도 함께 업데이트 된다.
		}
	}
}

int XWndQuestElem::Process( float dt )
{
	/*
	글로우가 나와야 하는 상황
	클라에서만 처리하는 방법
	퀘정보를 서버로부터 받으면 타이머를 작동시킨다.
	일률적으로 보이지 않기 위해 타이머를 x분/2 시간만큼 범위에서 랜덤으로 더 뺀다.
	타이머가 x분이상 지난 타이머는 초기화를 시켜주고 글로우를 작동시킨다.
	퀘가 중간에 없데이트 되면 함께 다시 업데이트 시킨다.
	반복.
	*/
	if( m_pQuestObj->IsOverClickedTime() ) {
		// 글로우 작동
		CONSOLE("퀘스트[%s], Glow작동", m_pQuestObj->GetstrIdentifer().c_str() );
		if( Find("spr.glow") == nullptr ) {
			auto pWndSpr = new XWndSprObj( _T( "ui_glow_quest.spr" ), 1, 0, 0, xRPT_1PLAY );
			pWndSpr->SetstrIdentifier( "spr.glow" );
			Add( pWndSpr );
		}
		// 타이머 리셋
		m_pQuestObj->UpdatesecClicked( false );
	}
	return XWndImage::Process(dt);
}

float xiSin2( float timeLerp, float min, float max, float slope, float speed )
{
	float degree = 180.f * timeLerp * 8.f;	// 0~18까지 변화
	float s = sinf( D2R( degree ) );		// sin0(0)~sin90(1)~sin180(0)까지 변화
	s = fabs(s);
	float size = max - min;				// 상하 폭 계산
	s *= size;								// 폭만큼 비율조절
	s += min;
	s += slope * timeLerp;				// 시간변화에 따른 기울기값을 더해줌
	return s;
}

void XWndQuestElem::Draw()
{
	if( m_pText && m_timerBlink.IsOn() )
	{
		float lerp = m_timerBlink.GetSlerp();
		float ip =  xiSin2( lerp, 0, 1.f, 0, 2.f );
// 		ip *= 4;
// 		ip = fmodf( ip, 1.f );
		m_pText->SetAlphaLocal( ip );
		if( m_timerBlink.IsOver() )
		{
			m_pText->SetAlphaLocal( 1.f );
			m_timerBlink.Off();
		}
	}
	XWndImage::Draw();
}

bool XWndQuestElem::IsValid() 
{
	return m_pQuestObj && m_pQuestObj->GetbDestroy() == false;
}

////////////////////////////////////////////////////////////////
XWndQuestInfo::XWndQuestInfo( XQuestObj *pQuestObj )
	: XWndPopup( _T("quest_info.xml"), "quest_info" )
{
	const float hText = 4.f;		// 한 라인의 평균 높이
	Init();
	m_pQuestObj = pQuestObj;
	SetstrIdentifier("popup.quest.info");
	// 임무완료
	SCENE_WORLD->SetButtHander( this, "butt.reward", &XSceneWorld::OnCompleteQuest, m_pQuestObj->GetidProp() );
	// 퀘스트장소
	SetButtHander( this, "butt.goto", &XWndQuestInfo::OnClickGoto );
	bool bAllComplete = pQuestObj->IsAllComplete();
	xSET_SHOW( this, "butt.reward", bAllComplete );
	xSET_SHOW( this, "butt.goto", !bAllComplete );
	_tstring strName = XTEXT( pQuestObj->GetpProp()->idName );
	auto pButt = xGET_BUTT_CTRL( this, "butt.reward" );
	if( pButt )
		pButt->SetGlow();
	if( bAllComplete ) {
		auto pWndCong = new XWndCongratulation();
		GAME->Add( pWndCong );
	}
	// 퀘 제목
	XE::VEC2 v(0,0); {
		auto pText = xSET_TEXT( this, "quest.info.name", strName );
		if( pText ) {
			v = pText->GetPosLocal();
			auto size = pText->GetLayoutSize();
			v.y += size.h;
			v.y += hText;
		}
	}
	// 퀘 설명
	{
		_tstring strDesc;
		pQuestObj->GetQuestDescReplacedToken( &strDesc );
// 		_tstring strSrc = XTEXT( pQuestObj->GetpProp()->idDesc );
		// 추가 토큰변환.
		XGAME::sReplaceToken( strDesc, _T( "#nick#" ), ACCOUNT->GetstrName() );
		auto pText = xSET_TEXT( this, "quest.info.desc", strDesc );
		if( pText ) {
			auto size = pText->GetLayoutSize();
			pText->SetY( v.y );
			v.y += size.h;
			v.y += hText;
		}
	} {
		auto pImg = Find("img.line1");
		if( pImg ) {
			pImg->SetbShow( !strName.empty() );
			pImg->SetY( v.y );
			v.y += hText;
		}
	} {
	// "목표"
		auto pText = xGET_TEXT_CTRL( this, "text.objective" );
		if( pText ) {
			auto size = pText->GetLayoutSize();
			pText->SetY( v.y );
			v.y += size.h;
			v.y += hText;
		}
	}
	// 퀘 목표리스트.
	{
		_tstring strObjective;
		pQuestObj->GetQuestObjective( &strObjective, false );
		auto pText = xSET_TEXT( this, "quest.info.objective", strObjective );
		auto size = pText->GetLayoutSize();
		pText->SetY( v.y );
		v.y += size.h;
		v.y += hText;
// 		int numCR = XE::GetNumCR( strObjective.c_str() );
// 		v.y += hText * ( numCR - 0 );
// 		v.y += hText * 2.f;		// 2줄 밑으로.
		auto pImg = Find( "img.line2" );
		if( pImg ) {
			pImg->SetbShow( !strObjective.empty() );
			pImg->SetY( v.y );
			v.y += hText;
		}
	}
	// "보상
	{
		auto pText = xGET_TEXT_CTRL( this, "text.reward" );
		if( pText ) {
			auto size = pText->GetLayoutSize();
			pText->SetY( v.y );
			v.y += size.h;
			v.y += hText;
		}
	}
	auto pView = xGET_SCROLLVIEW_CTRL( this, "scrl.view.quest" );
	if( XASSERT(pView) ) {
		XArrayLinearN<XGAME::xReward*, 128> ary;
		m_pQuestObj->GetRewardToAry( &ary );
		XE::VEC2 vSize;
		XARRAYLINEARN_LOOP_AUTO( ary, pReward ) {
			switch( pReward->rewardType ) {
			//////////////////////////////////////////////////////////////////////////
			case XGAME::xtReward::xRW_ITEM: {
				auto pWndIitem = new XWndStoragyItemElem( v, *pReward );
				pView->Add( pWndIitem );
				vSize = pWndIitem->GetSizeFinal();
				pWndIitem->SetPosLocal( v );
				pWndIitem->SetEventItemTooltip();
			} break;
			//////////////////////////////////////////////////////////////////////////
			case XGAME::xtReward::xRW_RESOURCE:
			case XGAME::xtReward::xRW_CASH: {
				auto pWndReward = new XWndStoragyItemElem( v, *pReward );
				pWndReward->SetNum( pReward->num );
				pView->Add( pWndReward );
				vSize = pWndReward->GetSizeFinal();
			} break;
			//////////////////////////////////////////////////////////////////////////
			case XGAME::xtReward::xRW_HERO: {
				auto pPropHero = PROP_HERO->GetpProp( pReward->idReward );
				if( XASSERT( pPropHero ) ) {
					auto pWndHero = new XWndHeroPortrait( v, pPropHero );
					pWndHero->SetScaleLocal( 0.76f );
					pWndHero->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickHeroTooltip, pReward->idReward );
					pView->Add( pWndHero );
				}
			} break;
			default:
				XBREAK(1);
			}
			v.x += vSize.w + 2.f;
		} END_LOOP;
		v.y += vSize.y;
		auto pText = xSET_TEXT( this, "quest.info.reward.exp", 
					XE::Format(XTEXT(2016), m_pQuestObj->GetpProp()->GetExpReward(ACCOUNT->GetLevel())) );
		if( pText ) {
			pText->SetY( v.y );
			auto size = pText->GetLayoutSize();
			v.y += size.h;
			v.y += hText;
		}
		auto pImg = Find( "img.line3" );
		if( pImg ) {
			pImg->SetbShow( true );
			pImg->SetY( v.y );
		}
		pView->SetScrollViewAutoSize();
	}
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		strName += XFORMAT("(%d)", m_pQuestObj->GetpProp()->idProp );
		XWnd *pButt = new XWndButtonDebug( 310.f, 17.f, 30.f, 30.f, _T("clear") );
		if( pButt ) {
			pButt->SetEvent( XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnCheatClearQuest, m_pQuestObj->GetidProp() );
			Add( pButt );
		}
		pButt = new XWndButtonDebug( 0, 17.f, 30.f, 30.f, _T( "del" ) );
		if( pButt ) {
			pButt->SetEvent( XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnCheatDelQuest, m_pQuestObj->GetidProp() );
			Add( pButt );
		}
		auto pText = new XWndTextString( XE::VEC2(0), _T("") );
		if( pText ) {
			_tstring str = XFORMAT( "(%s)", m_pQuestObj->GetstrIdentifer().c_str() );
			pText->SetText( str );
		}
		Add( pText );
	}
#endif
}

int XWndQuestInfo::OnOk( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	m_pQuestObj->SetstateAlert( XQuestObj::xMS_MUST_DEL );
	if( SCENE_WORLD )
		SCENE_WORLD->SetbUpdate( TRUE );
	std::string idsQuest = SZ2C( m_pQuestObj->GetstrIdentifer() );
	GAME->DispatchEvent( xAE_CONFIRM_QUEST, idsQuest );
	return XWndPopup::OnOk( pWnd, p1, p2 );
}

void XWndQuestInfo::OnFinishAppear()
{
	if( m_pQuestObj->IsAllComplete() )
		GAME->DispatchEvent( xAE_POPUP, 0, GetstrIdentifier(), 1 );
	else
		GAME->DispatchEvent( xAE_POPUP, 0, GetstrIdentifier(), 0 );
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndQuestInfo::OnClickGoto( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickGoto");
	//
	std::string idsQuest = SZ2C( m_pQuestObj->GetstrIdentifer() );
	GAME->DispatchEvent( xAE_CONFIRM_QUEST, idsQuest );
	SetbDestroy( TRUE );
	auto& ary = m_pQuestObj->GetaryConds();
	if( ary.size() > 0 ) {
		auto pCondObj = ary[0];
		// cond/what타입으로 위치를 가리킨다.
		auto pPropCond = pCondObj->GetpProp();
		if( DoIndicate( pPropCond->idWhat, pPropCond->paramWhat.dwParam, pPropCond->paramWhere.dwParam ) ) {
			// where조건이 있을수 있으니 한번더 돌린다.
			if( pPropCond->idWhere )
				DoIndicate( pPropCond->idWhere, pPropCond->paramWhere.dwParam );
		} else {
			// 실패하면 drop item의 where타입으로 위치를 가리킨다.
			if( m_pQuestObj->GetpProp()->aryDrops.size() > 0 ) {
				auto& drop = m_pQuestObj->GetpProp()->aryDrops[ 0 ];	// 편의상 첫번째 항목을 기준으로만 가리킨다.
				XARRAYLINEAR_LOOP_AUTO( drop.aryWhere, &where ) {
					ID idArea = 0;
					ID idWhere = 0;
					DWORD dwParam = 0;
					for( auto& attr : where.aryAttr ) {
						if( attr.idWhere == XGAME::xQC_WHERE_AREA ) {
							idArea = attr.param.dwParam;
						}
						else {
							idWhere = attr.idWhere;
							dwParam = attr.param.dwParam;
						}
					}
					DoIndicate( idWhere, dwParam, idArea );
				} END_LOOP;
			}
		}
	}
	return 1;
}

bool XWndQuestInfo::DoIndicate( ID idType, DWORD dwParam, ID idArea )
{
	float secBlink = 10.f;
	auto typeCond = (XGAME::xtQuestCond)idType;
	switch( typeCond ) {
// 	case XGAME::xQC_EVENT_LEVEL: {
// 		std::string idsQuest = SZ2C( m_pQuestObj->GetstrIdentifer() );
// 		GAME->DispatchEvent( xAE_WHERE_QUEST, (DWORD)typeCond, idsQuest );
// 	} break;
	case XGAME::xQC_WHERE_AREA:
	case XGAME::xQC_EVENT_OPEN_AREA:
		SCENE_WORLD->DoMoveToArea( dwParam );
		return true;
	case XGAME::xQC_WHERE_SPOT:
	case XGAME::xQC_EVENT_VISIT_SPOT:
	case XGAME::xQC_EVENT_RECON_SPOT:
	case XGAME::xQC_EVENT_CLEAR_SPOT: {
		ID idSpot = dwParam;
		SCENE_WORLD->DoMoveToSpot( idSpot );
		SCENE_WORLD->DoBlinkSpot( idSpot );
	} return true;
	case XGAME::xQC_WHERE_SPOT_CODE:
	case XGAME::xQC_EVENT_CLEAR_SPOT_CODE: {
		auto code = dwParam;
		SCENE_WORLD->DoMoveToCodeSpots( code );
		SCENE_WORLD->DoBlinkSpotCode( code );
	} return true;
	case XGAME::xQC_WHERE_SPOT_TYPE:
	case XGAME::xQC_EVENT_CLEAR_SPOT_TYPE:
	case XGAME::xQC_EVENT_RECON_SPOT_TYPE:
	case XGAME::xQC_EVENT_VISIT_SPOT_TYPE: {
		auto type = (XGAME::xtSpot)dwParam;
		if( idArea )
			SCENE_WORLD->DoMoveToArea( idArea );
		SCENE_WORLD->DoBlinkSpotType( type, secBlink, idArea );
	} return true;
// 	case XGAME::xQC_EVENT_GET_HERO:
// 	case XGAME::xQC_EVENT_KILL_HERO:
// 	case XGAME::xQC_EVENT_CLEAR_QUEST:
// 	case XGAME::xQC_EVENT_DEFENSE:
// 	case XGAME::xQC_EVENT_HAVE_ITEM:
// 	case XGAME::xQC_EVENT_GET_ITEM: 
// 	case XGAME::xQC_EVENT_CLEAR_STAGE:
// 	case XGAME::xQC_EVENT_HERO_LEVEL:
// 	case XGAME::xQC_EVENT_HERO_LEVEL_SQUAD:
// 	case XGAME::xQC_EVENT_HERO_LEVEL_SKILL:
// 	case XGAME::xQC_EVENT_BUY_CASH:
// 	case XGAME::xQC_EVENT_HIRE_HERO_PREMIUM:
// 		break;
	case XGAME::xQC_EVENT_LEVEL:
		GAME->DispatchEvent( xAE_WHERE_QUEST, (DWORD)typeCond );
		break;
	case XGAME::xQC_EVENT_GET_RESOURCE:
	case XGAME::xQC_EVENT_HAVE_RESOURCE: {
		std::string idsQuest = SZ2C( m_pQuestObj->GetstrIdentifer() );
		auto pCondObj = m_pQuestObj->GetCondObjByWhat( typeCond );
		if( pCondObj ) {
			GAME->DispatchEvent( xAE_WHERE_QUEST, (DWORD)typeCond, pCondObj->GetpProp()->paramWhat.dwParam );;
		}
	} break;
	case XGAME::xQC_EVENT_UI_ACTION: {
		// 액션을 시키는 퀘스트는 컷씬시스템으로 처리한다.
		std::string idsQuest = SZ2C( m_pQuestObj->GetstrIdentifer() );
		GAME->DispatchEvent( xAE_WHERE_QUEST, (DWORD)typeCond, idsQuest );
	} break;
	default:
//		XBREAK(1);
		break;
	}
	return false;
}

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
// 	XDelegator::sGet()->DoRequest( "train.complete", GetstrIdentifier() );
// 	XDelegator::sGet().DoRequest2( "train.complete", GetstrIdentifier(), &XWndTrainingCenter::DelegateTrainComplete );
// 	XDelegator::sGet().DoRequest( "unlock.train.slot", GetstrIdentifier(), &XWndTrainingCenter::DelegateUnlockTrainSlot );
// 	XDelegator::sGet().DoRequest2( "train.complete", GetstrIdentifier()
// 		, static_cast<void(XWnd::*)( const std::string&, XGAME::xtTrain, XHero* )>( &XWndTrainingCenter::DelegateTrainComplete ) );
// 	XDelegator::sGet().DoRequest( "unlock.train.slot", GetstrIdentifier()
// 		,static_cast<void(XWnd::*)( const std::string& )>( &XWndTrainingCenter::DelegateUnlockTrainSlot ) );
	XDelegator::sGet().DoRequest2( "train.complete", this
		, &XWndTrainingCenter::DelegateTrainComplete );
	XDelegator::sGet().DoRequest( "unlock.train.slot", this
		, &XWndTrainingCenter::DelegateUnlockTrainSlot );
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
//	SetbUpdate( true );
}


void XWndTrainingCenter::DelegateTrainComplete( const std::string& idsEvent, XGAME::xtTrain type, XHero* pHero )
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
 @brief 부대 메달제공 팝업
*/
// XWndSquadLevelup::XWndSquadLevelup( XHero *pHero )
// 	: XWndPopup(_T("squad_levelup.xml"), "popup_squad" )
// {
// 	Init();
// 	m_pHero = pHero;
// 	SetstrIdentifier("popup.squad.levelup");
// 	auto pImg = xGET_IMAGE_CTRL( this, "img.namecard" );
// 	if( pImg )
// 		pImg->SetScaleLocal( 2.3f, 1.3f );
// 	// 메달 제공 버튼
// 	SetButtHander( this, "butt.provide", &XWndSquadLevelup::OnClickProvideMedal );
// 	SetButtHander( this, "butt.train", &XWndSquadLevelup::OnClickTrain );
// 	ID idNeed = 0;
// 	int nNeedNum = 0;
// 	bool bAbleLevelup = ACCOUNT->IsAbleLevelUpSquad( pHero, &idNeed, &nNeedNum );
// 	XBREAK( idNeed == 0 || nNeedNum == 0 );
// 
// 	xSquadStat statNext;
// 	ACCOUNT->GetSquadStatWithTech( pHero, pHero->GetlevelSquad() + 1, &statNext );
// 	xSET_TEXT(this, "text.levelup.melee", XE::Format(_T("%d"), (int)statNext.meleePower ) );
// 	xSET_TEXT(this, "text.levelup.range", XE::Format(_T("%d"), (int)statNext.rangePower ) );
// 	xSET_TEXT(this, "text.levelup.def", XE::Format(_T("%d"), (int)statNext.def ));
// 	xSET_TEXT(this, "text.levelup.hp", XE::Format(_T("%d"), (int)statNext.hp ));
// 	xSET_TEXT(this, "text.levelup.atk.speed", XE::Format(_T("%d"), (int)(statNext.speedAtk * 100.f) ) );
// 	xSET_TEXT(this, "text.levelup.move.speed", XE::Format(_T("%d"), (int)(statNext.speedMoveForMeter * 100.f) ));
// 
// 	xSquadStat statCurr;
// 	ACCOUNT->GetSquadStatWithTech( pHero, pHero->GetlevelSquad(), &statCurr );
// 	auto pArrow = static_cast<XWndStatArrow*>(Find("img.arrow1"));
// 	if (pArrow)
// 		pArrow->SetDirection( statNext.meleePower, statCurr.meleePower );
// 	pArrow = static_cast<XWndStatArrow*>(Find("img.arrow2"));
// 	if (pArrow)
// 		pArrow->SetDirection( statNext.rangePower, statCurr.rangePower );
// 	pArrow = static_cast<XWndStatArrow*>(Find("img.arrow3"));
// 	if (pArrow)
// 		pArrow->SetDirection( statNext.def, statCurr.def );
// 	pArrow = static_cast<XWndStatArrow*>(Find("img.arrow4"));
// 	if (pArrow)
// 		pArrow->SetDirection( (float)statNext.hp, (float)statCurr.hp );
// 	pArrow = static_cast<XWndStatArrow*>(Find("img.arrow5"));
// 	if (pArrow)
// 		pArrow->SetDirection(0.f,0.f);
// 	pArrow = static_cast<XWndStatArrow*>(Find("img.arrow6"));
// 	if (pArrow)
// 		pArrow->SetDirection(0.f,0.f);
// 
// 	auto pPropSquadNext = pHero->GetpPropSquadupNext();
// 	XBREAK( pPropSquadNext == nullptr );
// 	auto pPropItem = PROP_ITEM->GetpProp(idNeed);
// 	int nNum = ACCOUNT->GetNumItems(idNeed);
// 	if (pPropItem)
// 	{
// 		int num = ACCOUNT->GetNumItems(idNeed);
// 		auto pPropUnit = PROP_UNIT->GetpProp(pHero->GetUnit());
// 		if (XASSERT(pPropUnit))
// 		{
// 			xSET_IMG(this, "img.icon.complate.legion", XE::MakePath(DIR_IMG, pPropUnit->strFace));
// 		}
// 		xSET_TEXT(this, "text.legion.name", XE::Format(XTEXT(80047), pHero->GetlevelSquad() + 1, pPropUnit->strName.c_str()));
// // 		auto pText = xSET_TEXT(this, "text.curr", XFORMAT("%d/%d", pHero->GetNumProvidedMedal(), pPropSquadNext->numItem));
// //		pText->SetColorText( XCOLOR_WHITE );
// 		// 메달 이미지를 버튼에 박음.
// 		xSET_IMG( this, "img.item", xPathIMG(pPropItem->strIcon) );
// // 		xSET_IMG(this, "img.icon.book", XE::MakePath(DIR_IMG, pProp->strIcon));
// 		XWnd* pWndIcon = Find( "wnd.icon.pos" );
// 		if( pWndIcon )
// 		{
// 			// 템이 찍힐 위치에 템이미지를 박아준다.
// 			if( pWndIcon->Find( "wnd.icon.medal" ) == nullptr )
// 			{
// 				auto pWndItem = new XWndStoragyItemElem( pPropItem->idProp );
// 				pWndItem->SetstrIdentifier( "wnd.icon.medal" );
// 				pWndItem->SetEventItemTooltip();
// 				pWndIcon->Add( pWndItem );
// 			}
// 		}
// 	}
// 	SetbUpdate( true );
// }
// 
// void XWndSquadLevelup::Update()
// {
// 	if( m_pHero ) {
// 		auto pHero = m_pHero;
// 		auto pPropSquadNext = m_pHero->GetpPropSquadupNext();
// 		// 렙업하면 자동적으로 나옴.
// 		if( m_pHero->GetbLevelupReady(m_Type) ) {
// 			GAME->SetbUpdate( true );
// 			SetbDestroy( TRUE );
// 			return;
// 		}
// 		// 훈련중인가
// 		auto pSlot = ACCOUNT->GetTrainingSquadupHero( pHero->GetsnHero() );
// 		if( pSlot ) {
// 			// 훈련중
// 			xSET_SHOW( this, "img.need.res1", false );
// 			xSET_SHOW( this, "butt.train", false );
// 			xSET_SHOW( this, "text.time.remain", true );
// 			auto pButt = xSET_SHOW( this, "butt.complete", true );
// 			if( pButt )
// 			{
// 				pButt->SetEvent( XWM_CLICKED, this, &XWndSquadLevelup::OnClickComplete );
// 				pButt->SetbEnable( TRUE );
// 			}
// 			xSET_TEXT( this, "text.get.medal",
// 							XFORMAT( "메달:+%d", pSlot->GetnumTrainingItem() ) );
// 			if( !IsAutoUpdate() )
// 				SetAutoUpdate( 0.1f );
// 		} else {
// 			// 훈련중이 아님
// 			bool bShow = true;
// 			if( ACCOUNT->IsLockTraingCenter() )
// 				bShow = false;
// 			if( bShow ) {
// 				// 현재 내가 소지한 유황과 만드레이크로 할수 있는 훈련양
// 				xTrainInfo infoTrain;
// 				ACCOUNT->GetTrainInfoBySquad( pHero, m_Type, pPropSquadNext, &infoTrain );
// 				int useRes1 = infoTrain.useRes1;
// 				int useRes2 = infoTrain.useRes2;
// // 				if( useRes1 == 0 )
// // 					useRes1 = pPropSquadNext->GetNeedRes(0);
// // 				if( useRes2 == 0 )
// // 					useRes2 = pPropSquadNext->GetNeedRes( 1 );
// 				XCOLOR col = XCOLOR_WHITE;
// // 				if( infoTrain.numItemWill == 0 )
// 				if( infoTrain.useRes1 == 0 ) {
// 					useRes1 = infoTrain.needRes1;
// 					if( ACCOUNT->GetSulphur() < infoTrain.needRes1 )
// 						col = XCOLOR_RED;
// 				}
// 				auto
// 				pText = xSET_TEXT( this, "text.need.res1", 
// 										XE::NumberToMoneyString( useRes1 ) );
// 				pText->SetColorText( col );
// 				///< 
// 				col = XCOLOR_WHITE;
// 				if( infoTrain.useRes2 == 0 ) {
// 					useRes2 = infoTrain.needRes2;
// 					if( ACCOUNT->GetMandrake() < infoTrain.needRes2 )
// 						col = XCOLOR_RED;
// 				}
// 				pText = xSET_TEXT( this, "text.need.res2",
// 										XE::NumberToMoneyString( useRes2 ) );
// 				pText->SetColorText( col );
// 				if( infoTrain.numItemWill == 0 ) {
// //					pText->SetColorText( XCOLOR_RED );
// 					xSET_SHOW( this, "img.clock", false );
// 					pText = xSET_TEXT( this, "text.time", _T( "자원 부족" ) );
// 					pText->SetColorText( XCOLOR_RED );
// 					xSET_SHOW( this, "text.get.item", false );
// 					m_bNotEnoughRes = true;
// 				} else {
// //					pText->SetColorText( XCOLOR_WHITE );
// 					_tstring str;
// 					str += XSceneTech::sGetResearchTime( infoTrain.secTotal * infoTrain.numItemWill );
// 					xSET_TEXT( this, "text.time", str );
// 					xSET_TEXT( this, "text.get.item",
// 						XFORMAT( "메달:+%d", infoTrain.numItemWill ) );
// 				}
// 			}
// 			xSET_TEXT( this, "text.curr", XFORMAT( "%d / %d", m_pHero->GetNumProvided(m_Type), pPropSquadNext->numItem ) );
// 			// 훈련중이 아님
// 			xSET_SHOW( this, "img.need.res1", bShow );
// 			xSET_SHOW( this, "img.need.res2", bShow );
// 			xSET_SHOW( this, "butt.train", bShow );
// 			xSET_ENABLE( this, "butt.train", !m_bNotEnoughRes );;
// 			xSET_SHOW( this, "text.time.remain", false );
// 			auto pWnd = xSET_SHOW( this, "butt.complete", false );
// 			if( pWnd )
// 				pWnd->SetbEnable( FALSE );
// 			ClearAutoUpdate();
// 		}
// 		ID idNeed;
// 		int numNeed;
// 		m_pHero->GetItemLevelUpSquad( &idNeed, &numNeed );
// 		XBREAK( idNeed == 0 || numNeed == 0 );
// 		int numHave = ACCOUNT->GetNumItems( idNeed );
// 		// 메달하나라도 없으면 비활성화
// 		// 현재 인벤에 갖고 있는 메달수
// 		auto pText = xSET_TEXT( this, "text.num.inven", XFORMAT( "x%d", numHave ) );
// 		if( numHave > 0 ) {
// 			xSET_ENABLE( this, "butt.provide", TRUE );
// 			pText->SetColorText( XCOLOR_WHITE );
// 		} else {
// 			xSET_ENABLE( this, "butt.provide", FALSE );
// 			pText->SetColorText( XCOLOR_RED );
// 		}
// 	}
// 
// 	XWndPopup::Update();
// }
// 
// /****************************************************************
// * @brief
// *****************************************************************/
// void XWndSquadLevelup::OnAutoUpdate()
// {
// 	auto pText = xGET_TEXT_CTRL( this, "text.time.remain" );
// 	auto pHero = m_pHero;
// 	if( pHero ) {
// 		auto pSlot = ACCOUNT->GetTrainingSquadupHero( pHero->GetsnHero() );
// 		if( pSlot ) {
// 			if( pText ) {
// 				pText->SetbShow( TRUE );
// 				_tstring strRemain;
// 				XSceneTech::sGetRemainResearchTime( &strRemain,
// 													pSlot->timerStart.GetsecStart(),
// 													pSlot->secTotal );
// 				strRemain += XFORMAT("(%d)", pSlot->GetsecPass());
// 				pText->SetText( strRemain );
// 				int cash = ACCOUNT->GetCostRemainTrain( pSlot );
// 				xSET_TEXT( this, "text.cost.complete", XFORMAT( "%s", XE::NumberToMoneyString( cash ) ) );
// 			}
// 			// 이미 훈련으로 얻은 개수
// 			auto pPropSquadNext = pHero->GetpPropSquadupNext();
// 			auto numAlreadyGet = pSlot->GetsecPass() / pPropSquadNext->needTrain.secTrainPerItem;
// 			// 현재까지 먹인 수
// 			xSET_TEXT( this, "text.curr", 
// 				XFORMAT( "%d(%d)/%d", m_pHero->GetNumProvided(m_Type), 
// 								pSlot->GetnumTrainingItem(),
// 								pPropSquadNext->numItem ) );
// 		}
// 
// 	}
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndSquadLevelup::OnClickComplete( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickComplete");
// 	//
// 	pWnd->SetbEnable( FALSE );
// 	if( XASSERT(m_pHero) )
// 	{
// 		auto pSlot = ACCOUNT->GetTrainingHero( m_pHero->GetsnHero(), m_Type );
// 		if( XASSERT(pSlot) )
// 		{
// 			GAMESVR_SOCKET->SendReqTrainCompleteQuick( GAME, pSlot->snSlot, m_Type, m_pHero->GetsnHero() );
// 		}
// 	}
// 	
// 	return 1;
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndSquadLevelup::OnClickProvideMedal( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickProvideMedal");
// 	//
// 	if( XASSERT(m_pHero) )
// 	{
// 		if( ACCOUNT->IsAbleProvideSquad( m_pHero ) )
// 			GAMESVR_SOCKET->SendReqProvideBooty( GAME, 
// 												m_pHero->GetsnHero(), 
// 												m_Type, 
// 												1 );
// 	}
// 	
// 	return 1;
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndSquadLevelup::OnClickTrain( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickTrain");
// 	//
// 	// 훈련소 빈 슬롯이 없으면 에러
// 	if( ACCOUNT->GetNumRemainFreeSlot() <= 0 ) {
// 		XWND_ALERT( "%s", XTEXT( 2093 ) );
// 		return 1;
// 	}
// 	if( m_bNotEnoughRes ) {
// 		XWND_ALERT( "%s", _T( "자원이 부족합니다." ) );
// 		return 1;
// 	}
// 	pWnd->SetbEnable( FALSE );
// 	//	SetbDestroy( TRUE );
// 	// 자원을 소모시키고 훈련을 시작한다.
// 	GAMESVR_SOCKET->SendReqTrainHero( GAME, m_pHero, m_Type );
// 	SCENE_UNITORG->SetpKeepHero( m_pHero );
// 	
// 	return 1;
// }
// 
// ////////////////////////////////////////////////////////////////
// /**
//  @brief 스킬 보옥제공 팝업
// */
// XWndSkillLevelup::XWndSkillLevelup( XHero *pHero, XGAME::xtTrain type )
// 	: XWndPopup(_T("skill_levelup.xml"), "popup_skill" )
// {
// 	Init();
// 	m_pHero = pHero;
// 	m_Type = type;
// 	SetstrIdentifier("popup.skill.levelup");
// 	auto pImg = xGET_IMAGE_CTRL( this, "img.namecard" );
// 	if( pImg )
// 		pImg->SetScaleLocal( 2.3f, 1.3f );
// 	// 보옥 제공 버튼
// 	SetButtHander( this, "butt.provide", &XWndSkillLevelup::OnClickProvideScroll );
// 	SetButtHander( this, "butt.train", &XWndSkillLevelup::OnClickTrain );
// 	ID idNeed = 0;
// 	int numNeed = 0;
// 	// pHero가 렙업에 필요한 아이템을 얻는다.
// 	pHero->GetItemLevelUpSkill( type, &idNeed, &numNeed );
// 	XBREAK( idNeed == 0 || numNeed == 0 );
// 
// 	auto pPropSkillNext = pHero->GetpPropSkillupNext( type );
// 	XBREAK( pPropSkillNext == nullptr );
// 	auto pPropItem = PROP_ITEM->GetpProp(idNeed);
// 	if( XBREAK(pPropItem == nullptr) )
// 		return;
// 	int numHave = ACCOUNT->GetNumItems( idNeed );
// 
// 	XSKILL::XSkillDat* pSkill = pHero->GetSkillDat( type );
// 	int lvSkill = pHero->GetLevelSkill( type );
// //	int nLevel = pHero->GetLevelSkill( type );
// 	_tstring strDesc;
// 	pSkill->GetSkillDesc(&strDesc, lvSkill+1);
// 	xSET_TEXT(this, "text.skill.desc", strDesc.c_str());
// 	xSET_TEXT(this, "text.skill.name", XFORMAT("%s", pSkill->GetSkillName()));
// 	xSET_IMG(this, "img.skill.icon", XE::MakePath(DIR_IMG, pSkill->GetstrIcon()));
// 	// 아이템 이미지를 버튼에 박음.
// 	xSET_IMG( this, "img.item", xPathIMG( pPropItem->strIcon ) );
// 	XWnd* pWndIcon = Find("wnd.icon.pos");
// 	if( pWndIcon ) {
// 		// 템이 찍힐 위치에 템이미지를 박아준다.
// 		if( pWndIcon->Find("wnd.icon.sphere") == nullptr ) {
// 			auto pWndItem = new XWndStoragyItemElem( pPropItem->idProp );
// 			pWndItem->SetstrIdentifier( "wnd.icon.sphere" );
// 			pWndItem->SetEventItemTooltip();
// 			pWndIcon->Add( pWndItem );
// 		}
// 	}
// 	SetbUpdate( true );
// 
// }
// 
// void XWndSkillLevelup::Update()
// {
// 	if( m_pHero ) {
// 		auto pHero = m_pHero;
// 		auto pPropSkillNext = m_pHero->GetpPropSkillupNext( m_Type );
// 		// 렙업하면 자동적으로 나옴.
// 		if( m_pHero->GetbLevelupReady(m_Type) ) {
// 			GAME->SetbUpdate( true );
// 			SetbDestroy( TRUE );
// 			return;
// 		}
// 		// 훈련중인가
// 		auto pSlot = ACCOUNT->GetTrainingSkillupHero( pHero->GetsnHero(), m_Type );
// 		if( pSlot ) {
// 			// 훈련중
// 			xSET_SHOW( this, "img.need.res1", false );
// 			xSET_SHOW( this, "butt.train", false );
// 			xSET_SHOW( this, "text.time.remain", true );
// 			auto pButt = xSET_SHOW( this, "butt.complete", true );
// 			if( pButt ) {
// 				pButt->SetEvent( XWM_CLICKED, this, &XWndSkillLevelup::OnClickComplete );
// 				pButt->SetbEnable( TRUE );
// 			}
// 			xSET_TEXT( this, "text.get.item",
// 							XFORMAT( "보옥:+%d", pSlot->GetnumTrainingItem() ) );
// 			if( !IsAutoUpdate() )
// 				SetAutoUpdate( 0.1f );
// 		} else {
// 			// 훈련중이 아님
// 			xSET_TEXT( this, "text.curr", XFORMAT( "%d/%d", m_pHero->GetNumProvided(m_Type), pPropSkillNext->numItem ) );
// 			// 현재 내 자원으로 할수 있는 훈련양
// 			if( ACCOUNT->IsLockTraingCenter() ) {
// 				xSET_SHOW( this, "text.need.res1", false );
// 				xSET_SHOW( this, "text.time", false );
// 				xSET_SHOW( this, "butt.train", false );
// 				xSET_SHOW( this, "img.need.res1", false );
// 				xSET_SHOW( this, "img.need.res2", false );
// 			} else {
// 				XGAME::xTrainInfo infoTrain;
// 				auto pPropSkillNext = pHero->GetpPropSkillupNext( m_Type );
// 				ACCOUNT->GetTrainInfoBySkill( pHero, m_Type, pPropSkillNext, &infoTrain );
// 				int useRes1 = infoTrain.useRes1;
// 				if( useRes1 == 0 )
// 					useRes1 = infoTrain.needRes1;
// 				auto
// 				pText = xSET_TEXT( this, "text.need.res1", XE::NumberToMoneyString( useRes1 ) );
// 				if( infoTrain.numItemWill == 0 ) {
// 					pText->SetColorText( XCOLOR_RED );
// 					xSET_SHOW( this, "img.clock", false );
// 					pText = xSET_TEXT( this, "text.time", _T( "자원 부족" ) );
// 					pText->SetColorText( XCOLOR_RED );
// 					xSET_SHOW( this, "text.get.item", false );
// 					m_bNotEnoughRes = true;
// 				} else {
// 					_tstring str;
// 					str += XSceneTech::sGetResearchTime( infoTrain.secTotal );
// 					xSET_TEXT( this, "text.time", str );
// 					xSET_TEXT( this, "text.get.item",
// 						XFORMAT( "보옥:+%d", infoTrain.numItemWill ) );
// 				}
// // 				auto
// // 				pText = xSET_TEXT( this, "text.need.res1", 
// // 											XE::NumberToMoneyString( -infoTrain.useRes1 ) );
// // 				if( infoTrain.numItemWill == 0 ) {
// // 					pText->SetColorText( XCOLOR_RED );
// // 					xSET_SHOW( this, "img.clock", false );
// // 					pText = xSET_TEXT( this, "text.time", _T("자원 부족") );
// // 					pText->SetColorText( XCOLOR_RED );
// // 					xSET_SHOW( this, "text.get.scroll", false );
// // 					m_bNotEnoughRes = true;
// // 				} else {
// // 					_tstring str;
// // 					str += XSceneTech::sGetResearchTime( infoTrain.secTotal * infoTrain.numItemWill );
// // 					xSET_TEXT( this, "text.time", str );
// // 					xSET_TEXT( this, "text.get.scroll",
// // 											XFORMAT( "보옥:+%d", infoTrain.numItemWill ) );
// // 				}
// 				xSET_SHOW( this, "img.need.res1", true );
// 				xSET_SHOW( this, "butt.train", true );
// 				xSET_ENABLE( this, "butt.train", true );;
// 				xSET_SHOW( this, "text.time.remain", false );
// 				auto pWnd = xSET_SHOW( this, "butt.complete", false );
// 				if( pWnd )
// 					pWnd->SetbEnable( FALSE );
// 				ClearAutoUpdate();
// 			}
// 		}
// 		ID idNeed;
// 		int numNeed;
// 		m_pHero->GetItemLevelUpSkill( m_Type, &idNeed, &numNeed );
// 		XBREAK( idNeed == 0 || numNeed == 0 );
// 		int numHave = ACCOUNT->GetNumItems( idNeed );
// 		// 보옥하나라도 없으면 비활성화
// 		// 현재 인벤에 갖고 있는 보옥수
// 		auto pText = xSET_TEXT( this, "text.num.inven", XFORMAT( "x%d", numHave ) );
// 		if( numHave > 0 )
// 		{
// 			xSET_ENABLE( this, "butt.provide", TRUE );
// 			pText->SetColorText( XCOLOR_WHITE );
// 		}
// 		else
// 		{
// 			xSET_ENABLE( this, "butt.provide", FALSE );
// 			pText->SetColorText( XCOLOR_RED );
// 		}
// 	}
// 
// 	XWndPopup::Update();
// }
// 
// /****************************************************************
// * @brief
// *****************************************************************/
// void XWndSkillLevelup::OnAutoUpdate()
// {
// 	auto pText = xGET_TEXT_CTRL( this, "text.time.remain" );
// 	auto pHero = m_pHero;
// 	if( pHero )
// 	{
// 		auto pSlot = ACCOUNT->GetTrainingSkillupHero( pHero->GetsnHero(), m_Type );
// 		if( pSlot )
// 		{
// 			if( pText )
// 			{
// 				pText->SetbShow( TRUE );
// 				_tstring strRemain;
// 				XSceneTech::sGetRemainResearchTime( &strRemain,
// 											pSlot->timerStart.GetsecStart(),
// 											pSlot->secTotal );
// 				strRemain += XFORMAT("(%d)", pSlot->GetsecPass());
// 				pText->SetText( strRemain );
// 				int cash = ACCOUNT->GetCostRemainTrain( pSlot );
// 				xSET_TEXT( this, "text.cost.complete", XFORMAT( "%s", XE::NumberToMoneyString( cash ) ) );
// 			}
// 			// 이미 훈련으로 얻은 개수
// 			auto pPropSkillNext = pHero->GetpPropSkillupNext( m_Type );
// 			auto numAlreadyGet = pSlot->GetsecPass() / pPropSkillNext->needTrain.secTrainPerItem;
// 			// 현재까지 먹인 수
// 			xSET_TEXT( this, "text.curr", 
// 				XFORMAT( "%d(%d)/%d", m_pHero->GetNumProvided(m_Type), 
// 								pSlot->GetnumTrainingItem(),
// 								pPropSkillNext->numItem ) );
// 		}
// 
// 	}
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndSkillLevelup::OnClickComplete( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickComplete");
// 	//
// 	pWnd->SetbEnable( FALSE );
// 	if( XASSERT(m_pHero) )
// 	{
// 		auto pSlot = ACCOUNT->GetTrainingHero( m_pHero->GetsnHero(), m_Type );
// 		if( XASSERT(pSlot) )
// 		{
// 			GAMESVR_SOCKET->SendReqTrainCompleteQuick( GAME, pSlot->snSlot, m_Type, m_pHero->GetsnHero() );
// 		}
// 	}
// 	
// 	return 1;
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndSkillLevelup::OnClickProvideScroll( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickProvideScroll");
// 	//
// 	if( XASSERT(m_pHero) )
// 	{
// 		if( ACCOUNT->IsAbleProvideSkill( m_pHero, m_Type ) )
// 			GAMESVR_SOCKET->SendReqProvideBooty( GAME, 
// 												m_pHero->GetsnHero(), 
// 												m_Type, 
// 												1 );
// 	}
// 	
// 	return 1;
// }
// 
// /****************************************************************
// * @brief 
// *****************************************************************/
// int XWndSkillLevelup::OnClickTrain( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickTrain");
// 	//
// 	// 훈련소 빈 슬롯이 없으면 에러
// 	if( ACCOUNT->GetNumRemainFreeSlot() <= 0 ) {
// 		XWND_ALERT( "%s", XTEXT( 2093 ) );
// 		return 1;
// 	}
// 	if( m_bNotEnoughRes ) {
// 		XWND_ALERT( "%s", _T( "자원이 부족합니다." ) );
// 		return 1;
// 	}
// 	pWnd->SetbEnable( FALSE );
// 	//	SetbDestroy( TRUE );
// 	// 자원을 소모시키고 훈련을 시작한다.
// 	GAMESVR_SOCKET->SendReqTrainHero( GAME, m_pHero, m_Type );
// 	
// 	return 1;
// }
// 

////////////////////////////////////////////////////////////////
XWndResourceCtrl::XWndResourceCtrl( const XE::VEC2& vPos, XGAME::xtResource type, int num, XCOLOR col )
	: XWnd( vPos )
	, m_aryNumResPrev( XGAME::xRES_ALL )
{
	Init();
	AddRes( type, num, col );
	SetbUpdate( true );
}

void XWndResourceCtrl::Destroy()
{
}

// void XWndResourceCtrl::SetScaleLocal( float sx, float sy )
// {
// 	// this자체를 scale해버리면 차일드들의 pos까지 스케일이 되어 제대로 안나온다.
// 	// 그래서 차일드들만 스케일하는걸로 바꿈.
// 	for( auto pWnd : m_listItems ) {
// 		pWnd->SetScaleLocal( sx, sy );
// 	}
// //	이방식으로 바꾸니 아이콘은 확대되는데 숫자가 ㅎ확대 안된다.
// }

void XWndResourceCtrl::AddRes( XGAME::xtResource type, int num, XCOLOR col ) 
{
	xEach res;
	if( num < 0 )
		res.type = XGAME::xRES_NONE;
	else
		res.type = type;
	res.num = num;
	res.col = col;
	m_listResource.Add( res );
	m_aryNumResPrev[ type ] = num;		// 최초값
	UpdateWnd();  // 즉시 윈도우를 만들어 child wnd와 크기를 가질수 있게 한다.
}

void XWndResourceCtrl::UpdateWnd()
{
	auto vScale = GetScaleLocal();
//	XE::VEC2 vIcon( 0, -4 );		// world_top_xxx.png의 그림에 여백이 있어서 세로위치를 약간 보정해줘야 한다.
	XE::VEC2 vIcon;
	vIcon *= vScale;
	for( auto& res : m_listResource ) {
//		auto resIcon = XGAME::GetResourceIcon2( res.type );
		LPCTSTR file = XGAME::GetResourceResFile( res.type );	// png/spr
		const _tstring ext = XE::GetFileExt( file );
		bool bSpr = (ext == "spr");
		XWndTextString *pWndText = nullptr;
		auto pWndIcon = Findf( "img.%d", res.type );
		//
		if( pWndIcon == nullptr ) {
			// 자원아이콘
			if( bSpr ) {
				pWndIcon = new XWndSprObj( file, 1, XE::VEC2(0) );
				pWndIcon->SetstrIdentifierf( "img.%d", res.type );
				Add( pWndIcon );
			} else {
				pWndIcon = new XWndImage( resIcon, XE::VEC2( 0 ) );
				pWndIcon->SetstrIdentifierf( "img.%d", res.type );
				Add( pWndIcon );
			}
			const XE::VEC2 vText( 22, 0 );
			// 자원양 숫자.
//			float sizeFont = (float)((int)(20.f * vScale.x));
			const float sizeFont = 20.f;
			XWndTextString *pText = nullptr;
			if( m_bCounter ) {
				auto pTextCnt = new XWndTextNumberCounter( vText * vScale,
																					res.num,
																					FONT_RESNUM, sizeFont, res.col );
				pTextCnt->SetbComma( TRUE );
				pTextCnt->SetbSymbol( true );
				pText = pTextCnt;
			} else {
				const _tstring strNum = (res.num < 0)? 
																		_T("???") 
																		: ((m_bSymbolNumber)? 
																			XE::NtSK( res.num ) 
																			: XE::NtS(res.num));
// 				if( res.num < 0 )
// 					strNum = _T( "???" );
// 				else
// 					strNum = XE::NumberToMoneyString( res.num );
// 			auto pText = new XWndTextString( vText,
				pText = new XWndTextString( vText * vScale,
																		strNum,
																		FONT_RESNUM, sizeFont, res.col );
			}
			pText->SetStyleStroke();
			pText->SetstrIdentifier("text.num");
			// 중앙정렬등에 사용하려고 크기를 넣음.
			const auto sizeCtrl = pText->GetSizeLocal();
			const auto sizeText = pText->GetLayoutSize();
			pText->SetSizeLocal( sizeText.w, pWndIcon->GetSizeLocal().h );
			pText->SetAlignVCenter();
			pWndIcon->Add( pText );
			pWndText = pText;
		} else {
			auto pText = xGET_TEXT_CTRL( pWndIcon, "text.num" );
			if( pText ) {
				pWndText = pText;
				if( m_bCounter ) {
					UpdateTextCounter( SafeCast<XWndTextNumberCounter*>( pText ), res.type, res.num );
				} else {
					_tstring strNum;
					if( res.num < 0 ) {
						strNum = _T( "???" );
					} else {
						if( m_bSymbolNumber )
							strNum = XE::NumberToMoneyStringK( res.num );
						else
							strNum = XE::NumberToMoneyString( res.num );
					}
					pText->SetText( strNum );
				}
				pText->SetColorText( res.col );
			}
		} // pWnd == nullptr
		// 자원아이콘 위치시킴
		XE::VEC2 vOfs;
		if( res.type == xRES_CASH )
			vOfs.Set( 0, 0 );
		else
			vOfs.Set( 0, -4 );
		pWndIcon->SetPosLocal( vIcon );
//		XE::VEC2 vSize = pWnd->GetSizeLocal();
		if( m_bVert ) {
			XE::VEC2 vSize( 15, 14 );
			vSize.h += 2;
			vSize.w = 0;
			vIcon += vSize * vScale;
		} else {
			if( pWndText ) {
				const auto sizeLayout = pWndIcon->GetSizeLocalLayout();
				vIcon.x += (sizeLayout.w + 10.f) * vScale.x;
			}
		}
	} // for
	auto vSizeLocal = GetSizeLocalLayout();
	SetSizeLocal( vSizeLocal );
}
/**
 @brief typeRes리소스의 값을 카운팅 애니메이션이 되도록 업데이트한다. 값은 미리 입력
*/
void XWndResourceCtrl::UpdateTextCounter( XGAME::xtResource typeRes )
{
	if( !m_bCounter )
		return;
	for( auto res : m_listResource ) {
		if( res.type == typeRes || typeRes == xRES_NONE ) {
			const auto numPrev = m_aryNumResPrev[ res.type ];
			if( res.num != numPrev ) {		// 최적화를 위해서 먼저 비교하고 들어가도록 함.
				auto pWndIcon = Findf( "img.%d", res.type );
				if( XASSERT(pWndIcon) ) {
					// 업데이트할때 이전값과 달라졌으면 카운터텍스트 갱신
					auto pTextCnt = SafeCast<XWndTextNumberCounter*>( pWndIcon->Find("text.num") );
					if( XASSERT( pTextCnt ) ) {
						UpdateTextCounter( pTextCnt, res.type, res.num );
					}
				}
			}
		}
	}
}

void XWndResourceCtrl::UpdateTextCounter( XWndTextNumberCounter* pTextCnt
																				, XGAME::xtResource typeRes
																				, int numCurr )
{
	if( XBREAK( !m_bCounter ) )
		return;
	if( XBREAK( pTextCnt == nullptr ) )
		return;
	const auto numPrev = m_aryNumResPrev[ typeRes ];
	if( numCurr != numPrev ) {
		// 이전과 값이 달라졌으면 카운팅 애니메이션을 한다.
		pTextCnt->SetnumMax( numCurr );		// 최대로 올라가야할 숫자
		pTextCnt->SetOn( numPrev );		// 애니메이션을 시작시키고 카운팅 초기값을 입력.
		m_aryNumResPrev[ typeRes ] = numCurr;		// 최신값으로 갱신.
	}
}
void XWndResourceCtrl::Update()
{
  UpdateWnd();
	XWnd::Update();
}

/**
 @brief 기 등록된 리소스의 개수를 바꿈.
*/
void XWndResourceCtrl::EditRes( XGAME::xtResource type, int num, XCOLOR col )
{
	for( auto& res : m_listResource ) {
		if( res.type == type ) {
			m_aryNumResPrev[ res.type ] = res.num;		// 이전값을 받아둠.
			res.num = num;
			if( col )
				res.col = col;
			break;
		}
	}
	SetbUpdate( true );
}

void XWndResourceCtrl::Draw()
{
	XWnd::Draw();
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
// XWndPopupDailyCamp::XWndPopupDailyCamp( XSpotDaily *pSpot )
// 	: XWndPopup(_T("popup_daily.xml"), "popup_daily_camp")
// {
// 	Init();
// 	XBREAK(pSpot == nullptr);
// 	m_pSpot = pSpot;
// 	SetbUpdate( true );
// //	SetAutoUpdate( 0.1f );
// 
// 	xSET_TEXT(this, "text.power", XE::Format(XTEXT(80191), m_pSpot->GetPower()));
// 	xSET_TEXT(this, "text.rest.challenge", XE::Format(XTEXT(80192), XSpotDaily::xNUM_ENTER - m_pSpot->GetnumEnter(), XSpotDaily::xNUM_ENTER));
// 
// 	XWndButton *pButt = dynamic_cast<XWndButton*>(Find("butt.battle"));
// // 	if (ACCOUNT->IsInvenOver())
// // 		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnClickAttackFail, m_pSpot->GetidSpot());
// // 	else
// 		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnAttackSpot, m_pSpot->GetidSpot());
// 
// // 	SetButtHander( this, "img.0", &XWndPopupDailyCamp::OnClickDow, 0 );
// // 	SetButtHander( this, "img.1", &XWndPopupDailyCamp::OnClickDow, 1 );
// // 	SetButtHander( this, "img.2", &XWndPopupDailyCamp::OnClickDow, 2 );
// // 	SetButtHander( this, "img.3", &XWndPopupDailyCamp::OnClickDow, 3 );
// // 	SetButtHander( this, "img.4", &XWndPopupDailyCamp::OnClickDow, 4 );
// // 	SetButtHander( this, "img.5", &XWndPopupDailyCamp::OnClickDow, 5 );
// // 	SetButtHander( this, "img.6", &XWndPopupDailyCamp::OnClickDow, 6 );
// // 	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
// // 	m_dowSelected = dowToday;
// }
// 
// void XWndPopupDailyCamp::Update()
// {
// 	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
// 	for( int i = 0; i < 7; ++i ) {
// 		auto pImg = xSET_IMGF( this, XE::MakePath( DIR_UI, _T( "daily_elem_off.png" ) ),"img.%d", i );
// 		if( pImg ) {
// 			XWnd *pGlow = pImg->Find( "img.glow" );
// 			if( pGlow )
// 				pGlow->SetbShow( FALSE );
// 		}
// 	}
// 	{
// 		// 오늘요일의 버튼이미지를 찾아서 글로우를 켬
// 		XWnd *pImg = Findf("img.%d", dowToday);
// 		if( pImg ) {
// 			XWnd *pGlow = pImg->Find( "img.glow" );
// 			if( pGlow ) {
// 				pGlow->SetbShow( TRUE );
// 			}
// 		}
// 		// 선택된 요일의 버튼이미지를 바꿔줌.
// 		xSET_IMGF( this, XE::MakePath( DIR_UI, _T( "daily_elem.png" ) ), "img.%d", (int)m_dowSelected );
// 	}
// 	// 선택한 요일의 드랍아이템 아이디를 가져옴.
// // 	ID idItem = XSpot::sGetIdDailyDropItem( (XE::xtDOW)m_dowSelected );
// // 	auto pProp = PROP_ITEM->GetpProp(idItem);
// // 	if (XBREAK(pProp == nullptr))
// // 		return;
// 	/*xSET_IMG(this, "img.reward.iocn", XE::MakePath(DIR_IMG, pProp->strIcon));
// 	xSET_TEXT(this, "text.reward", XTEXT(pProp->idName));*/
// 	if (Find("img.reward"))
// 		Find("img.reward")->SetbDestroy(TRUE);
// 
// // 	auto pItem = new XWndStoragyItemElem((ID)pProp->idProp);
// // 	pItem->SetstrIdentifier("img.reward");
// // 	pItem->SetPosLocal(167.f, 125.f);
// // 	pItem->SetEventItemTooltip();
// // 	Add(pItem);
// // 	auto pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("common_etc_namecard_m.png")), 0, 50);
// // 	pItem->Add(pImg);
// // 	auto pText = new XWndTextString(XE::VEC2(0, 7), _T(""), FONT_NANUM_BOLD, 15.f);
// // 	pText->SetStyle(xFONT::xSTYLE_STROKE);
// // 	pText->SetText(XTEXT(pProp->idName));
// // 	pText->SetAlign(XE::xALIGN_HCENTER);
// // 	pText->SetLineLength(56.f);
// // 	pImg->Add(pText);
// 
// 	XWndPopup::Update();
// }
// 
// void XWndPopupDailyCamp::OnAutoUpdate()
// {
// // 	int secRemain = (int)m_pSpot->GettimerEnter().GetsecRemainTime();	// 남은시간(초)
// // 	int hour, min, sec;
// // 	XTimer2::sGetHourMinSec(secRemain, &hour, &min, &sec);
// // 
// // 	xSET_TEXT(this, "text.rest.time", XE::Format(XTEXT(80193), hour, min, sec));
// }

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
	auto sizeLocalThis = GetSizeLocalLayout();
	SetSizeLocal( sizeLocalThis );
	SetbUpdate( true );
}

void XWndHeroPortrait::Update()
{
	auto vScale = GetScaleLocal();
	// 영웅 이름
	auto pText = xSET_TEXT( this, "text.name", m_pProp->strName );
// 	if( pText )
// 		pText->SetColorText( XGAME::GetGradeColor( m_pProp->GetGrade() ) );
	// 영웅 얼굴
	xSET_IMG( this, "img.hero", XE::MakePath( DIR_IMG, m_pProp->strFace ) );
	// 별
// 	int numStar = m_pProp->GetGrade();
// 	for( int i = 0; i < 5; ++i ) {
// 		bool bFlag = false;
// 		if( i < numStar )
// 			bFlag = true;
// 		auto pWnd = xSET_SHOWF( this, bFlag, "img.star.%d", i + 1 );
// 		if( pWnd ) {
// 			auto v = pWnd->GetPosLocal() * vScale;
// 			pWnd->SetPosLocal( v );
// 		}
// 
// 	}
	XWnd::Update();
}

XWndLevelupHero::XWndLevelupHero( XHero *pHero, XGAME::xtTrain typeTrain )
	: XWndPopup( _T("layout_book_hero.xml"), "popup_training_base" )
{
	Init();
	m_pHero = pHero;
	m_typeTrain = typeTrain;
	XBREAK( GetpLayout() == nullptr );
	auto pRoot = Find( "wnd.root.trainer" );
	XBREAK( pRoot == nullptr );
	const int level = pHero->GetLevel( m_typeTrain );
	// 영웅 초상화
	auto pWnd = new XWndStoragyItemElem( XE::VEC2( 0 ), pHero );
	pWnd->SetbShowName( false );
	pRoot->Add( pWnd );
	switch( typeTrain )
	{
	case XGAME::xTR_LEVEL_UP: {
		GetpLayout()->CreateLayout( "sub_levelup", this );
		xSET_TEXT( this, "text.title", XTEXT(2249) );
		auto pTextName = xSET_TEXT_FORMAT( this, "text.name.hero"
			, _T("Lv.%d %s"), level, pHero->GetstrName().c_str() );
		if( pTextName )
			pTextName->SetColorText( XGAME::GetGradeColor(pHero->GetGrade()) );
	} break;
	case XGAME::xTR_SQUAD_UP: {
		GetpLayout()->CreateLayout( "sub_levelup", this );
		xSET_TEXT( this, "text.title", XTEXT( 2250 ) );
		auto pWndUnit = new XWndCircleUnit( pHero->GetUnit(), XE::VEC2(105,20), pHero );
		pWndUnit->SetbShowLevelSquad( false );
		Add( pWndUnit );
		auto pText = xGET_TEXT_CTRL( this, "text.name.hero");
		if( pText ) {
			auto v = pText->GetPosLocal();
			v.x += 30.f;
			pText->SetPosLocal( v );
			pText->SetText( XFORMAT("%s %d", XTEXT(2257), pHero->GetlevelSquad()) );
		}
	} break;
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP: {
		GetpLayout()->CreateLayout( "sub_skillup", this );
		auto pDat = pHero->GetSkillDat( m_typeTrain );
		xSET_TEXT( this, "text.title", XTEXT( 2251 ) );
		auto pWndSkill = new XWndCircleSkill( pDat, XE::VEC2(105,20), nullptr );
		pWndSkill->SetScaleLocal( 0.6f );
		Add( pWndSkill );
		auto pText = xGET_TEXT_CTRL( this, "text.name.hero");
		if( pText ) {
			auto v = pText->GetPosLocal();
			v.x += 40.f;
			pText->SetPosLocal( v );
			pText->SetText( 
				XFORMAT("%s %d", XTEXT(2258), level) );
		}
		auto pWndIconRoot = Find("wnd.root.skill.icon");
		if( pWndIconRoot ) {
			auto pWndIcon = new XWndCircleSkill( pDat, XE::VEC2(0), pHero );
			pWndIconRoot->Add( pWndIcon );
		}
	} break;
	default:
		XBREAK(1);
		break;
	}
	SetstrIdentifier( "popup.levelup.hero");
	SetButtHander( this, "butt.minus", &XWndLevelupHero::OnClickMinus );
	SetButtHander( this, "butt.plus", &XWndLevelupHero::OnClickPlus );
	SetButtHander( this, "butt.max", &XWndLevelupHero::OnClickMax, 0 );
	SetButtHander( this, "butt.min", &XWndLevelupHero::OnClickMax, 1 );
	SetButtHander( this, "butt.train", &XWndLevelupHero::OnClickTrain );
	SetButtHander( this, "butt.complete.gold", &XWndLevelupHero::OnClickCompleteNow, 0 );
	SetButtHander( this, "butt.complete.cash", &XWndLevelupHero::OnClickCompleteNow, 1 );
	//
	auto pBar = SafeCast2<XWndProgressBar2*>( Find( "pbar.exp" ) );
	if( pBar ) {
		pBar->SetpDelegate( this );
		pBar->SetLerp( 2, pHero->GetExp( m_typeTrain ), pHero->GetMaxExpCurrLevel( m_typeTrain ) );
		xSET_TEXT_FORMAT( pBar, "text.level.will", _T( "%d" ), level );
		auto pSlot = ACCOUNT->GetTrainingHero( pHero->GetsnHero(), m_typeTrain );
		if( pSlot ) {
			XFLevel lvObj = m_pHero->GetLevelObj( m_typeTrain );
			lvObj.AddExp( pSlot->GetAddExp() );
			const int lvLimit = m_pHero->GetLvLimitByAccLv( ACCOUNT->GetLevel(), m_typeTrain );
			if( lvObj.GetLevel() > lvLimit ) {
				lvObj.SetLevel( lvLimit );
				lvObj.SetExp( lvObj.GetMaxExpCurrLevel() );
			}
			m_Level = lvObj.GetLevel();
			m_ExpRest = lvObj.GetExp();
			auto expMax = lvObj.GetMaxExpCurrLevel();
			pBar->SetLerp( 1, (float)m_ExpRest / expMax );
			bool bOver = ( m_Level > level );
			pBar->SetShowLayer( 2, !bOver );
			pBar->SetShowLayer( 5, bOver );
			pBar->SetLerp( 1, level, m_Level, (float)m_ExpRest / expMax );
			UpdateLevel( pBar, m_Level );
		}
	}
}

void XWndLevelupHero::Destroy()
{
	GAME->GetpScene()->SetbUpdate( true );
}

void XWndLevelupHero::Update()
{
	auto pHero = m_pHero;
 	auto pBar = SafeCast2<XWndProgressBar2*>( Find( "pbar.exp" ) );
 	XBREAK( pBar == nullptr );
	//
	const int lvTrain = pHero->GetLevel( m_typeTrain );
	const bool bTraining = ACCOUNT->IsTrainingHero( m_pHero->GetsnHero(), m_typeTrain );
	if( bTraining ) {
		// 훈련중
		xSET_SHOW( this, "wnd.training", true );
		xSET_SHOW( this, "wnd.train.before", false );
// 		auto pSlot = ACCOUNT->GetTrainingHero( m_pHero->GetsnHero(), m_typeTrain );
// 		if( XASSERT(pSlot) ) {
			SetAutoUpdate( 0.1f );
// 		} 
	} else {
		// 현재 exp
 		pBar->SetLerp( 2, pHero->GetExp( m_typeTrain ), pHero->GetMaxExpCurrLevel( m_typeTrain ) );
//		const int lvHero = m_pHero->GetLevel();
		ClearAutoUpdate();
		bool bGoldUse = (m_GoldUse > 0);
		xSET_SHOW( this, "wnd.training", false );
		xSET_SHOW( this, "wnd.train.before", true );
		xSET_ENABLE( this, "butt.minus", bGoldUse );
		// 훈련 최소 금화
		xSET_TEXT( this, "text.min.gold", XE::NtS(GetGoldMinForTrain( m_pHero->GetLevel() )) );
		// 훈련시간 표시
		xSET_TEXT( this, "text.time", XGAME::GetstrResearchTime( m_secTrainByGold ) );
		if( m_GoldUse == 0 ) {
			int minGold = GetGoldMinForTrain( pHero->GetLevel() );
			xSET_TEXT( this, "text.cost", XE::NumberToMoneyString( minGold ) );
		} else {
			xSET_TEXT( this, "text.cost", XE::NumberToMoneyString( m_GoldUse ) );
		}
		xSET_ENABLE( this, "butt.train", bGoldUse );
#ifdef _CHEAT
		if( XAPP->m_bDebugMode ) {
			if( m_ExpRest == 0 )
				m_ExpRest = m_pHero->GetExp( m_typeTrain );
			if( m_Level == 0 )
				m_Level = lvTrain;
			_tstring strExpTrain = XE::NumberToMoneyString( m_ExpByGold );
			_tstring strExpCurr = XE::NumberToMoneyString( m_ExpRest );
			auto expMaxAfter = m_pHero->GetMaxExpWithLevel( m_typeTrain, m_Level );
			_tstring strExpMax = XE::NumberToMoneyString( expMaxAfter );
			xSET_TEXT( this, "text.graph",
				XFORMAT( "Lv%d %s(%s)/%s", m_Level
																, strExpCurr.c_str()
																, strExpTrain.c_str()
																, strExpMax.c_str() ) );
		}
#endif // _CHEAT
	}
	int lvCurrByAni = pBar->GetlvCurrByAni();
	if( lvCurrByAni == 0 )
		lvCurrByAni = lvTrain;
	int lvMax = pHero->GetMaxLevel( m_typeTrain );
//	xSET_SHOW( this, "text.next", lvCurrByAni <= lvMax );
	if( lvCurrByAni >= lvMax )
		xSET_TEXT( this, "text.next", XTEXT(2111) );		// "다음레벨"을 "최대레벨"로 바꾼다.
	switch( m_typeTrain )
	{
	case XGAME::xTR_LEVEL_UP: {
// 		int lv = lvCurrByAni + 1;
// 		if( lv > lvMax )
// 			lv = lvCurrByAni;
		int lv = lvCurrByAni;
		const float multiply = XHero::s_fMultiply;
		std::vector<float> aryOrig;
		aryOrig.push_back( pHero->GetAttackMeleeRatio() * multiply );
		aryOrig.push_back( pHero->GetAttackRangeRatio() * multiply );
		aryOrig.push_back( pHero->GetDefenseRatio() * multiply );
		aryOrig.push_back( pHero->GetHpMaxRatio() * multiply );
		aryOrig.push_back( pHero->GetAttackSpeed() * multiply );
		aryOrig.push_back( pHero->GetMoveSpeed() * multiply );
//		const int lvCurrByAni = pBar->GetlvCurrByAni();
		std::vector<float> aryStat;
		aryStat.push_back( pHero->GetAttackMeleeRatio( lv ) * multiply );
		aryStat.push_back( pHero->GetAttackRangeRatio( lv ) * multiply );
		aryStat.push_back( pHero->GetDefenseRatio( lv ) * multiply );
		aryStat.push_back( pHero->GetHpMaxRatio( lv ) * multiply );
		aryStat.push_back( pHero->GetAttackSpeed( lv ) * multiply );
		aryStat.push_back( pHero->GetMoveSpeed( lv ) * multiply );
		for( int i = 0; i < 6; ++i ) {
			auto statOrig = aryOrig[i];
			auto statUp = aryStat[i];
			xSET_TEXTF( this, XFORMAT("%.0f", statOrig), "text.stat.%d", i+1 );
			// 변동치가 있을때만 표시
			int up = int(statUp) - int(statOrig);
			auto pText = xGET_TEXT_CTRLF( this, "text.up.%d", i+1 );
			if( pText ) {
				pText->SetbShow( up > 0 );
				pText->SetText( XFORMAT( "+%d", up ) );
			}
			auto pWndSpr = xGET_SPROBJ_CTRLF( this, "spr.up.%d", i+1 );
			if( pWndSpr ) {
				pWndSpr->SetbShow( up > 0 );
			}
		}
		// 영웅 대기모습
		auto pWndSpr = xGET_SPROBJ_CTRL( this, "spr.train.before" );
		if( pWndSpr ) {
			pWndSpr->SetpDelegate( this );
			auto& strSpr = m_pHero->GetpProp()->strSpr;
			pWndSpr->SetbShow( !bTraining );
			pWndSpr->SetScaleLocal( 0.5f );
			pWndSpr->SetSprObj( strSpr, ACT_IDLE1, xRPT_LOOP );
		}
		pWndSpr = xGET_SPROBJ_CTRL( this, "spr.training" );
		if( pWndSpr ) {
			pWndSpr->SetpDelegate( this );
			auto& strSpr = m_pHero->GetpProp()->strSpr;
			pWndSpr->SetbShow( bTraining );
			pWndSpr->SetScaleLocal( 0.5f );
			pWndSpr->SetSprObj( strSpr, ACT_ATTACK1, xRPT_1PLAY );
		}
	} break;
	case XGAME::xTR_SQUAD_UP: {
		xSquadStat statOrig;
		xSquadStat statUp;
		int lv = lvCurrByAni;
		pHero->GetSquadStatWithTech( pHero->GetlevelSquad(), &statOrig );
		pHero->GetSquadStatWithTech( lv, &statUp );
		for( int i = 0; i < 6; ++i ) {
			xSET_TEXTF( this, XFORMAT( "%.0f", statOrig.GetStat(i) ), "text.stat.%d", i + 1 );
			// 변동치가 있을때만 표시
			int up = int( statUp.GetStat(i) ) - int( statOrig.GetStat(i) );
			auto pText = xGET_TEXT_CTRLF( this, "text.up.%d", i + 1 );
			if( pText ) {
				pText->SetbShow( up > 0 );
				pText->SetText( XFORMAT( "+%d", up ) );
			}
			auto pWndSpr = xGET_SPROBJ_CTRLF( this, "spr.up.%d", i + 1 );
			if( pWndSpr ) {
				pWndSpr->SetbShow( up > 0 );
			}
		}
		// 유닛 대기모습.
		auto pPropUnit = PROP_UNIT->GetpProp( m_pHero->GetUnit() );
		if( pPropUnit ) {
			auto& strSpr = pPropUnit->strSpr;
			auto pWndSpr = xGET_SPROBJ_CTRL( this, "spr.train.before" );
			if( pWndSpr ) {
				pWndSpr->SetpDelegate( this );
				pWndSpr->SetbShow( !bTraining );
				pWndSpr->SetSprObj( strSpr, ACT_IDLE1, xRPT_LOOP );
			}
			pWndSpr = xGET_SPROBJ_CTRL( this, "spr.training" );
			if( pWndSpr ) {
				pWndSpr->SetpDelegate( this );
				pWndSpr->SetbShow( bTraining );
				pWndSpr->SetSprObj( strSpr, ACT_ATTACK1, xRPT_1PLAY );
			}
		}
	} break;
	case XGAME::xTR_SKILL_ACTIVE_UP:
	case XGAME::xTR_SKILL_PASSIVE_UP: {
		auto pDat = pHero->GetSkillDat( m_typeTrain );
		XBREAK( pDat == nullptr );
		int lv = lvCurrByAni;
// 		int lv = lvCurrByAni + 1;	// 디폴트로 다음레벨을 보여준다.
		if( lv < lvMax ) {
			xSET_TEXT_FORMAT( this
				, "text.name.skill", _T( "%s Lv.%d" ), pDat->GetstrName(), lv );
		} else {
			lv = lvMax;
			xSET_TEXT_FORMAT( this
				, "text.name.skill", _T( "%s Lv.%s" ), pDat->GetstrName(), XTEXT(2229));	// Lv최대
		}
		_tstring strDesc;
		pDat->GetSkillDesc( &strDesc, lv );
		xSET_TEXT( this, "text.desc.skill", strDesc );
	} break;
	default:
		XBREAK(1);
		break;
	}
	XWndPopup::Update();
}

void XWndLevelupHero::OnAutoUpdate()
{
	if( ACCOUNT->IsTrainingHero( m_pHero->GetsnHero(), m_typeTrain ) ) {
		auto pSlot = ACCOUNT->GetTrainingHero( m_pHero->GetsnHero(), m_typeTrain );
		if( XASSERT(pSlot) ) {
			// 훈련 남은 시간 표시
			xSET_TEXT( this, "text.remain.time", pSlot->GetstrSecRemain() );
			//
			{
				int cash = ACCOUNT->GetCashResearch( pSlot->GetsecRemain() );
				xSET_TEXT( this, "text.cost.cash", XE::NumberToMoneyString( cash ) );
				xSET_ENABLE( this, "butt.complete.cash", ACCOUNT->IsEnoughCash( cash ) );
			}
			{
				int gold = ACCOUNT->GetGoldResearch( pSlot->GetsecRemain() );
				xSET_TEXT( this, "text.cost.gold", XE::NumberToMoneyString( gold ) );
				xSET_ENABLE( this, "butt.complete.gold", ACCOUNT->IsEnoughGold( gold ) );
			}
		}
	}
}

void XWndLevelupHero::OnDelegateChangeLevel( XWndProgressBar2 *pBar, int lvCurrByAni )
{
	// 훈련후 도달할 레벨 업데이트
	UpdateLevel( pBar, lvCurrByAni );
	SetbUpdate( true );
}

bool XWndLevelupHero::DelegateSprObjFinishAni( XWndSprObj *pWndSpr )
{
	if( m_typeTrain == XGAME::xTR_LEVEL_UP || m_typeTrain == XGAME::xTR_SQUAD_UP ) {
		bool bTraining = ACCOUNT->IsTrainingHero( m_pHero->GetsnHero(), m_typeTrain );
		if( bTraining ) {
			if( xRandom(1) == 0 ) {
				pWndSpr->SetAction( ACT_ATTACK1, xRPT_1PLAY );
			} else {
				if( m_pHero->IsRange() )
					pWndSpr->SetAction( ACT_ATTACK1, xRPT_1PLAY );
				else
					pWndSpr->SetAction( ACT_ATTACK2, xRPT_1PLAY );
			}
		}
	}
	return false;
}

void XWndLevelupHero::UpdateLevel( XWndProgressBar2 *pBar, int lvCurrByAni )
{
	XWnd *pRoot = pBar;
	if( pRoot == nullptr )
		pRoot = this;
	auto pTextAfter 
		= xSET_TEXT_FORMAT( pRoot, "text.level.will", _T("%d"), lvCurrByAni );
	if( pTextAfter ) {
		int lvStart = m_pHero->GetLevel( m_typeTrain );
		if( lvCurrByAni > lvStart )
			pTextAfter->SetColorText( XCOLOR_GREEN );
		else
			pTextAfter->SetColorText( XCOLOR_WHITE );
	}
}
/**
 @brief 
*/
int XWndLevelupHero::OnClickMinus( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickMinus");
	//
	auto pBar = SafeCast2<XWndProgressBar2*>( Find( "pbar.exp" ) );
	const int lvTrain = m_pHero->GetLevel( m_typeTrain );
	int lvCurr = m_Level;
	int expCurr = m_ExpRest;
	int expMaxCurr = m_pHero->GetMaxExpWithLevel( m_typeTrain, lvCurr );
	if( lvCurr == 0 )
		lvCurr = lvTrain;
	if( expCurr == 0 )
		expCurr = m_pHero->GetExp( m_typeTrain );
	if( expMaxCurr == 0 ) {
		expMaxCurr = m_pHero->GetMaxExpWithLevel( m_typeTrain, lvCurr );
		XBREAK( expMaxCurr == 0 );
	}
	float lerpCurr = (float)expCurr / expMaxCurr;
	// 애니메이션중이 다시 눌릴수도 있으므로 항상 애니메이션시작 레벨로 업데이트한다.
	UpdateLevel( pBar, lvCurr );
	//
	m_GoldUse -= GetGoldMinForTrain( m_pHero->GetLevel() );
	UpdateGold();
	if( pBar ) {
		int lvDst = m_Level;
		if( lvDst == 0 )
			lvDst = lvTrain;
		auto expMaxAfter = m_pHero->GetMaxExpWithLevel( m_typeTrain, lvDst );
		int expDst = m_ExpRest;
		if( expDst == 0 )
			expDst = m_pHero->GetExp( m_typeTrain );
		float lerpDst = (float)expDst / expMaxAfter;
		int lvOrig = lvTrain;
		pBar->DoLerpAni( 1, lvOrig, lvCurr, lerpCurr, lvDst, lerpDst, 0.25f );
	}
	SetbUpdate( true );
	return 1;
}
/**
 @brief 
*/
int XWndLevelupHero::OnClickPlus( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickPlus");
	//
	//
	auto goldMinForTrain = GetGoldMinForTrain( m_pHero->GetLevel() );
	if( m_GoldUse + goldMinForTrain > (int)ACCOUNT->GetGold() ) {
		XWND_ALERT_T(_T("%s"), XTEXT(2143) );		// 금화 부족함.
	} else {
		auto pBar = SafeCast2<XWndProgressBar2*>( Find( "pbar.exp" ) );
		const int lvTrain = m_pHero->GetLevel( m_typeTrain );
		int lvCurr = m_Level;
		int expCurr = m_ExpRest;
		int expMaxCurr = m_pHero->GetMaxExpWithLevel( m_typeTrain, lvCurr );
		if( lvCurr == 0 )
			lvCurr = lvTrain;
		if( expCurr == 0 )
			expCurr = m_pHero->GetExp( m_typeTrain );
		if( expMaxCurr == 0 ) {
			expMaxCurr = m_pHero->GetMaxExpWithLevel( m_typeTrain, lvCurr );
			XBREAK( expMaxCurr == 0 );
		}
		float lerpCurr = (float)expCurr / expMaxCurr;
		// 애니메이션중이 다시 눌릴수도 있으므로 항상 애니메이션시작 레벨로 업데이트한다.
		UpdateLevel( pBar, lvCurr );
		//
		m_GoldUse += goldMinForTrain;
		UpdateGold();
		if( pBar ) {
			int lvDst = m_Level;
			auto expMaxAfter = m_pHero->GetMaxExpWithLevel( m_typeTrain, m_Level );
			float lerpDst = (float)m_ExpRest / expMaxAfter;
			int lvOrig = lvTrain;
			pBar->DoLerpAni( 1, lvOrig, lvCurr, lerpCurr, lvDst, lerpDst, 0.25f );
		}
	}
	// 
	SetbUpdate( true );
	return 1;
}

void XWndLevelupHero::UpdateGold()
{
	// 현재 가진 금화를 1000단위 절삭.
	int goldMax = ((int)ACCOUNT->GetGold() / 1000) * 1000;
	if( m_GoldUse > goldMax )
		m_GoldUse = goldMax;
	if( m_GoldUse < 0 )
		m_GoldUse = 0;
	const int lvHero = m_pHero->GetLevel();
	// 렙업됨에 따라 비용이 점차 증가하는것을 고려한 exp와 훈련시간을 얻음.
	int expByGold = 0;
	int secByGold = 0;
	int goldRemain = 0;
	if( m_GoldUse > 0 ) {
		ACCOUNT->GetTrainExpByGoldCurrLv( m_pHero, m_GoldUse, m_typeTrain, &expByGold, &secByGold, &goldRemain );
		// 현재 레벨 객체를 복사받음.
		XFLevel lvObj = m_pHero->GetLevelObj( m_typeTrain );
		lvObj.AddExp( expByGold );
		const int lvLimit = m_pHero->GetLvLimitByAccLv( ACCOUNT->GetLevel(), m_typeTrain );
		if( lvObj.GetLevel() > lvLimit ) {
			lvObj.SetLevel( lvLimit );
			lvObj.SetExp( lvObj.GetMaxExpCurrLevel() );
		}
		m_ExpByGold = expByGold;	// 총 더한양
		m_Level = lvObj.GetLevel();
		m_ExpRest = lvObj.GetExp();
		m_secTrainByGold = secByGold;
		if( goldRemain > 0 )
			m_GoldUse -= goldRemain;
	} else {
		m_Level = 0;
		m_ExpByGold = 0;
		m_ExpRest = 0;
		m_secTrainByGold = 0;
	}
//	int expRemain = (int)m_pHero->GetExpRemain( m_typeTrain );
// 	if( expByGold > expRemain ) {
// 		expByGold = expRemain;
// 		// exp를 골드로 환산
// 		m_Gold = ACCOUNT->GetGoldByExp( lvHero, expByGold, m_typeTrain );
// 		if( m_Gold > 10000 )
// 			m_Gold = ((int)m_Gold / 10000) * 10000;	// 다시 만단위로 절삭
// 	}
}

/**
 @brief 
*/
int XWndLevelupHero::OnClickMax( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickMax");
	int type = (int)p1;	// 0:max 1:min
	//
	auto pBar = SafeCast2<XWndProgressBar2*>( Find( "pbar.exp" ) );
	const int lvTrain = m_pHero->GetLevel( m_typeTrain );
	int lvCurr = m_Level;
	int expCurr = m_ExpRest;
	int expMaxCurr = m_pHero->GetMaxExpWithLevel( m_typeTrain, lvCurr );
	if( lvCurr == 0 )
		lvCurr = lvTrain;
	if( expCurr == 0 )
		expCurr = m_pHero->GetExp( m_typeTrain );
	if( expMaxCurr == 0 ) {
		expMaxCurr = m_pHero->GetMaxExpWithLevel( m_typeTrain, lvCurr );
		XBREAK( expMaxCurr == 0 );
	}
	float lerpCurr = (float)expCurr / expMaxCurr;
	// 애니메이션중이 다시 눌릴수도 있으므로 항상 애니메이션시작 레벨로 업데이트한다.
	UpdateLevel( pBar, lvCurr );
	//
	if( type == 0 )
		m_GoldUse = (int)ACCOUNT->GetGold();
	else
		m_GoldUse = GetGoldMinForTrain( m_pHero->GetLevel() );
	UpdateGold();
	//
	if( pBar ) {
		int lvDst = m_Level;
		auto expMaxAfter = m_pHero->GetMaxExpWithLevel( m_typeTrain, m_Level );
		float lerpDst = (float)m_ExpRest / expMaxAfter;
		int lvOrig = lvTrain;
		pBar->DoLerpAni( 1, lvOrig, lvCurr, lerpCurr, lvDst, lerpDst, 0.25f );
	}
	SetbUpdate( true );
	return 1;
}

/**
 @brief 
*/
int XWndLevelupHero::OnClickTrain( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickTrain");
	//
	if( m_GoldUse > 0 ) {
		if( ACCOUNT->GetNumRemainFreeSlot() > 0 ) {
			GAMESVR_SOCKET->SendReqTrainHeroByGold( GAME, m_pHero, m_GoldUse, m_typeTrain );
		} else {
			XWND_ALERT("%s", XTEXT(2093));	// 비어있는 훈련소가 없음.
		}
	}
	return 1;
}

/**
 @brief 
*/
int XWndLevelupHero::OnClickCompleteNow( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickCompleteNow");
	bool bCash = (p1 != 0);
	auto pSlot = ACCOUNT->GetTrainingHero( m_pHero->GetsnHero(), m_typeTrain );
	if( bCash ) {
		GAMESVR_SOCKET->SendReqTrainCompleteQuick( GAME, pSlot->snSlot, m_typeTrain, m_pHero->GetsnHero(), bCash );
	} else {
		GAMESVR_SOCKET->SendReqTrainCompleteQuick( GAME, pSlot->snSlot, m_typeTrain, m_pHero->GetsnHero(), bCash );
	}
	return 1;
}

void XWndLevelupHero::UpdateTrain( ID snHero, XGAME::xtTrain typeTrain, ID snSlot )
{
	SetbUpdate( true );
}


/*
 사양
 영웅 이름 출력
 현재경험치(+추가경험치)/최대경험치 표시
 +-버튼 10,000씩 증감
 최대버튼 할수있는 최대값으로 훈련
 얻을수 있는 최대경험치
   .현재 레벨의 남은 경험치양(A)
   .현재 돈으로 얻을수 있는 경험치양
     .이경험치는 A를 넘을 수 없다.
얻는 경험치에 따른 훈련시간 표시
훈련시작버튼 누르면 서버로 전송(투자하는 골드(gold), 영웅아이디)
  .가능한 경험치양( gold )
*/

////////////////////////////////////////////////////////////////
/**
 @brief 영웅의 전장
*/
XWndCampaignHero::XWndCampaignHero( XSpotCommon *pSpot, xCampaign::CampObjPtr& spCampObj )
	: XWndCampaignBase( pSpot, spCampObj )
{
	Init();
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
XWndAbilTreeDebug::XWndAbilTreeDebug( /*XAccount *pAcc, */XGAME::xtUnit unit )
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
			auto pWndElem = XWndInvenHeroElem::sUpdateCtrl( pWndList, m_pHeroFrom );
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
				auto pWndElem = XWndInvenHeroElem::sUpdateCtrl( pWndList, pHero );
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

/**
 @brief pWndList에 pHero의 XWndInvenHeroElem객체를 생성하거나 업데이트하는 표준함수.
 @param pHero 코딩의 편의상 null이 될수도 있어야 함.
*/
XWndInvenHeroElem* XWndInvenHeroElem::sUpdateCtrl( XWndList *pWndList, XHero *pHero )
{
	if( pHero == nullptr )
		return nullptr;
	std::string idsElem = XE::Format( "elem.hero.%s", SZ2C( pHero->GetstrIdentifer() ) );
	auto pWndElem
		= SafeCast2<XWndInvenHeroElem*>( pWndList->Find( idsElem ) );
	if( pWndElem == nullptr ) {
		pWndElem = new XWndInvenHeroElem( pHero );
		pWndElem->SetstrIdentifier( idsElem );
		pWndList->AddItem( pHero->GetsnHero(), pWndElem );
	}
	return pWndElem;
}
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
 @brief 전투후 영웅들 경험치 상황 팝업.
*/
XWndBattleAfterHeroExp::XWndBattleAfterHeroExp( int idxLegion )
	: XWndPopup( _T( "popup_battle_exp.xml" ), "popup_exp" )
{
	Init();
	m_idxLegion = idxLegion;
	xSetButtHander( this, SCENE_BATTLE, "butt.ok", &XSceneBattle::OnOkHerosExp );
	xSetButtHander( this, SCENE_BATTLE, "butt.statistic", &XSceneBattle::OnClickStatistic );
}

void XWndBattleAfterHeroExp::Update()
{
	XLegion *pLegion = ACCOUNT->GetLegionByIdx(m_idxLegion).get();
	if( XASSERT(pLegion) ) {
		XArrayLinearN<XHero*, XGAME::MAX_SQUAD> ary;
		pLegion->GetHerosToAry(ary);
		int nAdjustY = 0, size = ary.size();
		nAdjustY = ary.size() / 5;
		if (ary.size() % 5 == 0)
			nAdjustY--;
		nAdjustY *= 42;
		if (ary.size() > 5)
			size = 5;
		auto sizePopup = GetSizeLocal();
 		XE::VEC2 vStart((316 - 70) / 2 + 114 - 43 * (size - 1), (250 - 81) / 2 + 45 - nAdjustY);
		XARRAYLINEARN_LOOP(ary, XHero*, pHero) {
			if (_i != 0 && _i % 5 == 0) {
				if (_i == ary.size() - (ary.size() % 5))
					size = ary.size() % 5;
				vStart.x = (float)(316 - 70) / 2 + 114 - 43 * (size - 1);
				vStart.y += 84;
			}
			char key[256];
			sprintf_s(key, "wnd.slot%d", _i + 1);
			XWnd *pSlot = Find(key);
			if (pSlot) {
				pSlot->SetPosLocal(vStart);
				pSlot->SetbShow(TRUE);
			}
			sprintf_s(key, "img.hero.slot%d", _i+1);
			_tstring resImg = XE::MakePath(DIR_IMG, pHero->GetpProp()->strFace.c_str());
			xSET_IMG_RET(pImg, this, key, resImg.c_str());
			pSlot = pImg->GetpParent();
			sprintf_s(key, "img.hero.bg%d", _i + 1);
			xSET_IMG(this, key, XE::MakePath(DIR_UI, _T("common_unit_bg_m.png")));
			sprintf_s(key, "text.hero.slot%d", _i+1);
			xSET_TEXT(this, key, pHero->GetstrName().c_str());

			sprintf_s(key, "pbar.hero.slot%d", _i+1);
			auto pPBar = SafeCast<XWndProgressBar*>(Find(key));
			if (pPBar) {
				pPBar->SetbShow(TRUE);
	//				pPBar->SetLerp((float)pHero->GetExp() / pHero->GetXFLevelObj().GetMaxExpCurrLevel());
				pPBar->SetLerp((float)pHero->GetExp() / pHero->GetMaxExpCurrLevel());
			}
// 			if (pHero->GetbLevelUpAndClear( XGAME::xTR_LEVEL_UP )) {
// 				pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("ingame_levelup.png")), 2.f, 2.f);
// 				pSlot->Add(pImg);
// 			}
			for (int i = 0; i < pHero->GetGrade(); ++i) {
				pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("common_etc_smallstar.png")), 10.f + i * 13, 6.f);
				pSlot->Add(pImg);
			}
			XWndTextString* pText = new XWndTextString(XE::VEC2(0,7),
											XE::Format(_T("Lv%d"), pHero->GetLevel()),
											FONT_NANUM, FONT_SIZE_DEFAULT);
			pText->SetStyle(xFONT::xSTYLE_STROKE);
			pText->SetLineLength(72.f);
			pText->SetAlign(XE::xALIGN_CENTER);
			pSlot->Add(pText);

			vStart.x += 86;
		}END_LOOP;	
	}
	XWndPopup::Update();
}

////////////////////////////////////////////////////////////////
/**
 @brief 유닛과 유닛의 원형프레임을 그려주는 모듈.
*/
XWndCircleUnit::XWndCircleUnit( XGAME::xtUnit unit, const XE::VEC2& vPos, XHero *pHero )
	: XWndImage( PATH_UI("corps_legionnaire_bg.png"), vPos )
{
	Init();
	m_Unit = unit;
	m_pHero = pHero;
}

void XWndCircleUnit::Update()
{
	auto blendFunc = GetblendFunc();
	// 유닛 초상화.
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

////////////////////////////////////////////////////////////////
XWndCongratulation::XWndCongratulation()
	: XWndSprObj(_T("ui_firework.spr"), 2, XE::GetGameSize() * .5f, xRPT_1PLAY )
{
	Init();
//	SetAction( 2, xRPT_1PLAY );
// 	auto pWndDark = new XWndImage( PATH_UI("bg_popup_dark.png"), XE::VEC2(0) );
// 	pWndDark->SetScaleLocal(1.f, 0.5f);
// 	Add( pWndDark );
// 	auto sizeDark = pWndDark->GetSizeLocal();
// 	SetSizeLocal( sizeDark );
// 	SetPosLocal( 0, )
// 	auto vCenter = GetPosLocal() + sizeDark
// 	auto pWndSpr = new XWndSprObj( _T( "ui_firework.spr" ), 1
// 		, XE::VEC2( vSizeParent.w*0.5f, 155 ), xRPT_1PLAY );
// 	Add( pWndSpr );
	auto vPos = GetPosLocal();
	auto vSize = GetSizeLocal();
	auto rect = GetBoundBoxLocal();
	auto vLT = vPos + rect.vLT;
	auto vCenter = vLT + vSize * 0.5f;
//	auto vTextLT = vCenter - pText->GetSizeLocal() * 0.5f;
//	auto v = XE::VEC2(0, vLT.y);
	auto v = vLT - vPos;
	v.y += 20.f;
	auto pText = new XWndTextString( v, XTEXT( 2260 )
																, FONT_NANUM_BOLD, 50.f );
	pText->SetStyleStroke();
	pText->SetAlignHCenter();
	pText->SetLineLength( vSize.w );
	pText->SetstrIdentifier("text.1");
	Add( pText );
	v.y += 120.f;
	pText = new XWndTextString( v, XTEXT( 2261 )
																, FONT_NANUM_BOLD, 40.f );
	pText->SetStyleStroke();
	pText->SetAlignHCenter();
	pText->SetLineLength( vSize.w );
	pText->SetstrIdentifier( "text.2" );
	Add( pText );
	SetpDelegateBySprObj( this );
//	XParticleMng::sGet()->CreateSfx( "fire_cracker", vPos );
	// 디폴트 프로젝션은 델리게이트의 디폴트를 사용한다.
	auto pLayer = new XWndParticleLayer( "fire_cracker", vPos, 3.f, this );
	Add( pLayer );
	m_timerLife.Set( 1.0f );
}

void XWndCongratulation::OnFinishAni() 
{
	SetbDestroy( true );
}
void XWndCongratulation::OnProcessDummy( ID id, const XE::VEC2& vLocalT
																			, float ang
																			, const XE::VEC2& vScale
																			, const CHANNEL_EFFECT& cnEffect
																			, float dt, float fFrmCurr ) 
{
	auto 
	pText = xGET_TEXT_CTRL( this, "text.1");
	if( pText ) {
		pText->SetAlphaLocal( cnEffect.fAlpha );
	}
	pText = xGET_TEXT_CTRL( this, "text.2" );
	if( pText ) {
		pText->SetAlphaLocal( cnEffect.fAlpha );
	}
}

void XWndCongratulation::OnLButtonDown( float lx, float ly )
{
	m_bClicked = true;
	XWndSprObj::OnLButtonDown( lx, ly );
}
void XWndCongratulation::OnLButtonUp( float lx, float ly )
{
	if( m_bClicked && m_timerLife.IsOver() )
		SetbDestroy( true );
	XWndSprObj::OnLButtonUp( lx, ly );
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

//////////////////////////////////////////////////////////////////////////
/**
 @brief 월드 배경에 배치되는 오브젝트들의 루트레이어 객체
*/
XWndBgObjLayer::XWndBgObjLayer()
{
	Init();
}

void XWndBgObjLayer::Update()
{
	for( auto pProp : XPropBgObj::sGet()->GetlistBgObjs() ) {
		auto pWndBgObj = SafeCast2<XWndSprObj*>( Findf( "__bg.%d", pProp->m_idObj ) );
		if( pWndBgObj == nullptr ) {
			pWndBgObj = new XWndSprObj( pProp->m_strSpr
																, pProp->m_idAct
																, pProp->m_vwPos );
			pWndBgObj->GoRandomFrame();
			pWndBgObj->SetstrIdentifierf("__bg.%d", pProp->m_idObj );
#ifdef _xIN_TOOL
			pWndBgObj->SetEvent( XWM_CLICKED, this, &XWndBgObjLayer::OnClickBgObj );
#endif // _xIN_TOOL
			Add( pWndBgObj );
		} else {
			pWndBgObj->SetSprObj( pProp->m_strSpr, pProp->m_idAct );
		}

	}
}

#ifdef _xIN_TOOL
XWndSprObj* XWndBgObjLayer::CreateWndBgObj( xnBgObj::xProp *pProp )
{
	if( XBREAK(pProp == nullptr) )
		return nullptr;
	auto pWndExist = Findf( "__bg.%d", pProp->m_idObj );
	if( XBREAK( pWndExist != nullptr) )
		return nullptr;
	auto pWndBgObj = new XWndSprObj( pProp->m_strSpr
																, pProp->m_idAct
																, pProp->m_vwPos );
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
			for( auto pWnd : m_listItems ) {
				auto bb = pWnd->GetBoundBoxFinal();
				GRAPHICS->DrawRect( bb.vLT, bb.vRB, XCOLOR_RGBA( 0, 0, 0, 128 ) );
			}
		}
		if( m_pSelected ) {
			auto bb = m_pSelected->GetBoundBoxFinal();
			GRAPHICS->DrawRect( bb.vLT, bb.vRB, XCOLOR_RGBA( 255, 0, 0, 200 ) );
		}
	}
}

/**
 @brief 
*/
int XWndBgObjLayer::OnClickBgObj( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickBgObj");
	//
	if( XBaseTool::sIsToolBgObjMode() ) {
		m_pSelected = pWnd;
		if( pWnd ) {
			const std::string str = pWnd->GetstrIdentifier();
			const std::string strNum = str.substr( 5 );
			ID idObj = (ID)atoi( strNum.c_str() );
			if( idObj ) {
				m_pPropSelected = XPropBgObj::sGet()->GetpProp( idObj );
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
}
void XWndBgObjLayer::OnRButtonUp( float lx, float ly )
{
	CONSOLE( "XWndBgObjLayer::OnRButtonUp" );
	if( m_bRButtDown && m_bMoved && m_pPropSelected )
		XBaseTool::s_pCurr->UpdateAutoSave();
	m_bRButtDown = false;
	XAPP->SetbDebugDrawArea( TRUE );		// 복구
	m_bCopyed = false;
	m_bDragging = false;
}

void XWndBgObjLayer::OnMouseMove( float lx, float ly )
{
	if( XBaseTool::sIsToolBgObjMode() ) {
		if( m_bRButtDown && m_pSelected && m_pPropSelected ) {
			if( GetView()->m_bCtrl && m_bCopyed == false ) {
				// 선택된 객체의 카피본을 만든다.
				auto pNewProp = XPropBgObj::sGet()->CreateNewProp();
				*pNewProp = *m_pPropSelected;		// 복사
				pNewProp->m_idObj = 0;		// 아이디를 새로 부여받기위해 클리어.
				XPropBgObj::sGet()->AddBgObj( pNewProp );
				m_pPropSelected = pNewProp;
				m_pSelected = CreateWndBgObj( pNewProp );
				m_bCopyed = true;
			}
			auto vwCurr = SCENE_WORLD->GetvMouseWorld();
//			m_pPropSelected->m_vwPos = vwMouse;
			auto vDist = vwCurr - m_vTouchPrev;
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

////////////////////////////////////////////////////////////////
/**
 @brief 치트모드용 퀘스트 목록
*/
XWndCheatQuestList::XWndCheatQuestList( float x, float y )
	: XWnd( x, y )
{
	Init();
	SetSizeLocal(XE::VEC2(128,150));
}

BOOL XWndCheatQuestList::OnCreate()
{
	XWndTextString *pText = nullptr;
	// 5가지 징표퀘 정보
	XE::VEC2 v(5,10);
	pText = new XWndTextString( v.x, v.y, _T( "" ), BASE_FONT );
	pText->SetstrIdentifier("text.quest");
	Add( pText );
	SetbUpdate( TRUE );
	return TRUE;
}

void XWndCheatQuestList::Destroy()
{
}

void XWndCheatQuestList::Update()
{
	int numItem = 0;
	int maxItem = 10;
	if( ACCOUNT == nullptr )
		return;
	//
	_tstring strObjective;

	// 현재 퀘스트 리스트를 받아온다.
	m_aryQuest.Clear();
//	XArrayLinearN<XQuestObj*, 256> ary;
	ACCOUNT->GetpQuestMng()->GetQuestsToAry( &m_aryQuest );
}

void XWndCheatQuestList::Draw( void )
{
	XE::VEC2 vPos = GetPosFinal();
	XE::VEC2 vSize = GetSizeFinal();
	GRAPHICS->FillRectSize( vPos, vSize, XCOLOR_RGBA(0,0,0,100) );
	XE::VEC2 v = vPos;
	v.y += 4.f;
	XARRAYLINEARN_LOOP_AUTO( m_aryQuest, pObj )
	{
		_tstring strObjective;
		pObj->GetQuestObjective( &strObjective, true );
		int numCR = XE::GetNumCR( strObjective.c_str() );
		PUT_STRING( v.x, v.y, XCOLOR_WHITE, strObjective.c_str() );
		v.y += 11.f * (numCR+0);
	} END_LOOP;
	XWnd::Draw();
}
#include "stdafx.h"
#include "XWndPopupDaily.h"
#include "XSystem.h"
#include "client/XAppMain.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndImage.h"
#include "XWndResCtrl.h"
#include "JWWnd.h"
#include "XSockGameSvr.h"
#include "XSceneWorld.h"
#include "XAccount.h"
#include "XGame.h"
#include "XSpotDaily.h"
#include "XStruct.h"
#include "XWndTemplate.h"
#if _DEV_LEVEL <= DLV_DEV_PERSONAL
#include "XCampObj.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;


bool XWndPopupDaily::s_bTestMode = false;
////////////////////////////////////////////////////////////////
XWndPopupDaily::XWndPopupDaily( XSpotDaily *pSpot )
	: XWndPopup( _T( "popup_daily.xml" ), "popup_daily" )
{
	Init();
	XBREAK( pSpot == nullptr );
	m_pSpot = pSpot;
	SetbUpdate( true );
//	SetButtHander( this, "butt.battle", &XWndPopupDaily::OnClickEnter );
	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
	m_dowToday = dowToday;
	m_timerAlpha.Set( 1.f );
	SetstrIdentifier( "popup.daily" );
}

/**
 @brief 
*/
void XWndPopupDaily::Update()
{
#ifdef _CHEAT
	const auto bShow = (XAPP->m_bDebugMode != FALSE);
	if( XAPP->m_bDebugMode ) {
		XE::VEC2 vPos( 0, 0 );
		const XE::VEC2 vSize( 30, 30 );
		if( Find( "butt.reset" ) == nullptr ) {
			auto 
			pButt = new XWndButtonDebug( vPos, vSize, _T( "리셋" ) );
			pButt->SetstrIdentifier( "butt.reset" );
			pButt->SetEvent( XWM_CLICKED, this, &XWndPopupDaily::OnClickCheat, 0 );
			Add( pButt );
			vPos.x += vSize.w + 2.f;
			pButt = new XWndButtonDebug( vPos, vSize, _T( "단계+" ) );
			pButt->SetstrIdentifier( "butt.lvup" );
			pButt->SetEvent( XWM_CLICKED, this, &XWndPopupDaily::OnClickCheat, 1 );
			Add( pButt );
			vPos.x += vSize.w + 2.f;
			pButt = new XWndButtonDebug( vPos, vSize, _T( "단계-" ) );
			pButt->SetstrIdentifier( "butt.lvdown" );
			pButt->SetEvent( XWM_CLICKED, this, &XWndPopupDaily::OnClickCheat, 2 );
			Add( pButt );
			vPos.x += vSize.w + 2.f;
			pButt = new XWndButtonDebug( vPos, vSize, _T( "clear" ) );
			pButt->SetstrIdentifier( "butt.clear" );
			pButt->SetEvent( XWM_CLICKED, this, &XWndPopupDaily::OnClickCheat, 3 );
			Add( pButt );
			vPos.x += vSize.w + 2.f;
		}
//		pButt = new XWndButtonDebug( vPos, vSize, _T( "test mode" ) );
		auto 
		pButt = XWndButtonDebug::sUpdateCtrl( this, "butt.test.mode", vPos, vSize, _T("test mode"), bShow );
		if( pButt ) {
			pButt->SetbCheck( s_bTestMode );
			pButt->SetEvent( XWM_CLICKED, this, &XWndPopupDaily::OnClickCheat, 4 );
		}
// 		Add( pButt );
		vPos.x += vSize.w + 2.f;
	}
#endif // _CHEAT
	xSET_IMG( this, "img.elem.6", PATH_UI( "daily_elem_st.png" ) );
	xSET_IMG( this, "img.elem.7", PATH_UI( "daily_elem_su.png" ) );
 	const XE::xtDOW aryDow[7] = {XE::xDOW_MONDAY, XE::xDOW_TUESDAY, XE::xDOW_WEDNESDAY, XE::xDOW_THURSDAY, XE::xDOW_FRIDAY, XE::xDOW_SATURDAY, XE::xDOW_SUNDAY};
	//	float scale = 4.f;	// 테스트용
	// 각 요일 업데이트
	for( int i = 0; i < 7; ++i ) {
		const auto dow = aryDow[i];
		auto pWnd = Findf( "img.elem.%d", i + 1 );
		if( pWnd ) {
			UpdateElem( pWnd, dow, i );
		}
	}
	// 단계 표시
	xSET_TEXT_FORMAT( this, "text.floor", _T("%s %d"), XTEXT(2345), m_pSpot->GetidxFloor() + 1 );
	// 도전횟수 마크
	int numRemain = _XGC->m_numEnterDaily - m_pSpot->GetnumEnter();
	for( int i = 0; i < numRemain; ++i ) {
		auto pMark = xGET_IMAGE_CTRLF( this, "img.mark.%d", i + 1 );
		if( pMark ) {
			pMark->SetSurfaceRes( PATH_UI( "chall_mark_on.png" ) );
		}
	}
	// 업데이트 알림
	if( m_pSpot->GetbitFlag().bUpdated ) {
		GAME->AddAlert( this, 1, XE::VEC2(272,67) );
	} else {
		GAME->DelAlert( this, 1 );
	}
	XWndPopup::Update();
}


/**
 @brief 각 요일을 업데이트 한다.
*/
void XWndPopupDaily::UpdateElem( XWnd* pWndElem, XE::xtDOW dow, int idx )
{
//	const XE::xtDOW aryDow[7] = {XE::xDOW_MONDAY, XE::xDOW_TUESDAY, XE::xDOW_WEDNESDAY, XE::xDOW_THURSDAY, XE::xDOW_FRIDAY, XE::xDOW_SATURDAY, XE::xDOW_SUNDAY};
//	for( int i = 0; i < 7; ++i ) {
//		const auto dow = aryDow[i];
// 		auto pWndElem = Findf( "img.elem.%d", i + 1 );
// 		if( pWndElem ) {
			const auto dowToday = XSYSTEM::GetDayOfWeek();
#ifdef _CHEAT
			if( (XAPP->m_bDebugMode && s_bTestMode) || dow == dowToday )
#else
			if( dow == dowToday )
#endif // _CHEAT
			{
				pWndElem->SetEvent( XWM_CLICKED, this, &XWndPopupDaily::OnClickEnter, (DWORD)dow );
			}
			auto pTextDow = xSET_TEXT( pWndElem, "text.dow", XTEXT( 80194 + idx ) );
			if( pTextDow && dow == m_dowToday ) {
				auto col = XGAME::xGetColorPower( m_pSpot->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
				pTextDow->SetColorText( col );
			}
			xSET_SHOW( pWndElem, "spr.glow", dow == m_dowToday );
			// 등장 유닛 표시
			auto pCtrlUnit = SafeCast<XWndCircleUnit*>( pWndElem->Find( "ctrl.unit" ) );
			if( pCtrlUnit ) {
				const auto unit = m_pSpot->GetUnitByDow( dow );
				pCtrlUnit->SetUnit( unit );
			}
			// 드랍아이템
			CreateRewardCtrl( pWndElem, dow );
			const bool bClear = m_pSpot->IsClearDay( dow );
			xSET_SHOW( pWndElem, "wnd.stars", !bClear );
			xSET_SHOW( pWndElem, "img.vcheck", bClear );
			// 별 업데이트
			if( !bClear ) {
				const int numStar = m_pSpot->GetnumStar( dow );
				for( int i = 0; i < 3; ++i ) {
					auto pImgStar = SafeCast<XWndImage*>( pWndElem->Findf( "img.star.%d", i + 1 ) );
					if( pImgStar ) {
						if( i < numStar ) {
							pImgStar->SetSurfaceRes( PATH_UI( "star_on.png" ) );
						} else {
							pImgStar->SetSurfaceRes( PATH_UI( "star_off.png" ) );
						}
					}
				}
			}
//		}
//	} // for
}

/**
 @brief 보상 컨트롤 생성
*/
void XWndPopupDaily::CreateRewardCtrl( XWnd* pWndElem, const XE::xtDOW dow )
{
	// 드랍아이템
	XVector<XGAME::xReward> aryReward;
	const int lvFloor = m_pSpot->GetlvLegionCurrFloor();
	XSpotDaily::sGetRewardDaily( m_pSpot->GetpProp(), dow, lvFloor, &aryReward );
// 	XSpotDaily::sGetRewardDaily( m_pSpot->GetpProp(), dow, ACCOUNT->GetLevel(), &aryReward );
	if( aryReward.size() ) {
		if( aryReward[0].IsTypeResource() ) {
			auto pWndRes = SafeCast<XWndResourceCtrl*>( pWndElem->Find("ctrl.res") );
			if( pWndRes == nullptr ) {
				pWndRes = new XWndResourceCtrl( XE::VEC2( 6, 31 ) );
				pWndRes->SetstrIdentifier( "ctrl.res" );
				pWndRes->SetScaleLocal( 0.8f );
				pWndRes->SetbTouchable( false );
				pWndElem->Add( pWndRes );
			}
			pWndRes->ClearRes();
			pWndRes->AddResWithAry( aryReward );
		} else {
			CreateRewardCtrlByItem( pWndElem, aryReward, dow );
		} // not resource reward
	} // if( aryReward.size() )
}

/**
 @brief 아이템 형태의 보상일경우 그에 맞는 컨트롤 생성
*/
void XWndPopupDaily::CreateRewardCtrlByItem( XWnd* pWndElem
																					, const XVector<xReward>& aryReward
																					, const XE::xtDOW dow )
{
	for( auto& reward : aryReward ) {
		if( reward.rewardType == XGAME::xtReward::xRW_ITEM ) {
			auto pProp = PROP_ITEM->GetpProp( reward.GetidItem() );
			if( XASSERT( pProp ) ) {
				if( pWndElem->Find( "img.reward" ) )
					pWndElem->Find( "img.reward" )->SetbDestroy( TRUE );
				auto pItem = new XWndStoragyItemElem( (ID)pProp->idProp );
				pItem->SetstrIdentifier( "img.reward" );
				pItem->SetPosLocal( 4.f, 35.f );
				pItem->SetNum( reward.num );
				pItem->SetEventItemTooltip();
				pWndElem->Add( pItem );
				if( dow == m_dowToday ) {
					auto pWndGlow = xGET_IMAGE_CTRL( pWndElem, "img.glow" );
					if( pWndGlow ) {
						XE::VEC2 v( -7 );
						pWndGlow->SetPosLocal( v );
						pWndGlow->SetblendFunc( XE::xBF_ADD );
						pWndGlow->SetbShow( TRUE );
					}
					auto pButt = xGET_BUTT_CTRL( this, "butt.battle" );
					if( pButt ) {
						XE::VEC2 v( 0, 95 );
						auto vElem = pWndElem->GetPosLocal();
						pButt->SetPosLocal( vElem + v );
						if( m_pSpot->IsAttackable( ACCOUNT ) )
							pButt->SetbEnable( true );
						else
							pButt->SetbEnable( false );
					}
				}
			}
		} // if( reward.rewardType == XGAME::xtReward::xRW_ITEM ) {
	} // for( auto& reward : aryReward ) {
}


int XWndPopupDaily::Process( float dt )
{
	int aryIdxDow[7] = {7, 1, 2, 3, 4, 5, 6};
	auto pElem = Findf( "img.elem.%d", aryIdxDow[m_dowToday] );
	if( pElem ) {
		auto pWndGlow = xGET_IMAGE_CTRL( pElem, "img.glow" );
		if( pWndGlow ) {
			pWndGlow->SetbShow( true );
			auto lerpTime = m_timerAlpha.GetSlerp();
			float a = XE::xiCos( lerpTime, 0.3f, 1.f, 0 );
			pWndGlow->SetAlphaLocal( a );
		}
	}
	if( m_timerAlpha.IsOver() )
		m_timerAlpha.Reset();
	return XWndPopup::Process( dt );
}

void XWndPopupDaily::OnAutoUpdate()
{

}

/****************************************************************
* @brief
*****************************************************************/
int XWndPopupDaily::OnClickCheat( XWnd* pWnd, DWORD p1, DWORD p2 )
{
#ifdef _CHEAT
	CONSOLE( "%s:%d", __TFUNC__, p1 );
	//
	if( p1 != 4 ) {
		GAMESVR_SOCKET->SendCheat( GAME, 22, m_pSpot->GetidSpot(), p1 );
	} else {
		s_bTestMode = !s_bTestMode;
	}
// 	if( p1 == 0 ) {
// 		m_pSpot->SetnumEnter( 0 );
// 		GAMESVR_SOCKET->SendCheat( GAME, 22, m_pSpot->GetidSpot(), p1 );
// 	} else
// 	if( p1 == 1 ) {
// 		m_pSpot->IncFloor();
// 		GAMESVR_SOCKET->SendCheat( GAME, 22, m_pSpot->GetidSpot(), p1 );
// 	} else
// 	if( p1 == 2 ) {
// 		m_pSpot->DecFloor();
// 		GAMESVR_SOCKET->SendCheat( GAME, 22, m_pSpot->GetidSpot(), p1 );
// 	} else
// 	if( p1 == 3 ) {
// 		m_pSpot->DecFloor();
// 		GAMESVR_SOCKET->SendCheat( GAME, 22, m_pSpot->GetidSpot(), p1 );
// 	}
	SetbUpdate( true );
#endif // _CHEAT
	return 1;
}

int XWndPopupDaily::OnClickDow( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "OnClickDow:%d", p1 );
	//
	//	m_dowSelected = (int)p1;
	//	SetbUpdate( true );
	// 	auto pPopup = new XWndPopupDailyCamp( m_pSpot );
	// 	GAME->Add( pPopup );
	// 	pPopup->SetbModal( TRUE );
	return 1;
}

int XWndPopupDaily::OnClickEnter( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE( "%s:%s", __TFUNC__, C2SZ(pWnd->GetstrIdentifier()) );
	//
#if _DEV_LEVEL <= DLV_DEV_PERSONAL
	auto dow = ( XE::xtDOW )p1;
	xCampaign::XCampObj::s_idxStage = (int)dow;		// 개발자용버전에서는 어느 요일을 클릭했는지까지 보낸다.
#endif
	if( SCENE_WORLD )
		SCENE_WORLD->OnAttackSpot( nullptr, m_pSpot->GetidSpot(), 0 );
	return 1;
}

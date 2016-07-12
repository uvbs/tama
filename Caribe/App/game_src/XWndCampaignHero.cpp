#include "stdafx.h"
#include "XWndCampaignHero.h"
#include "XFramework/client/XLayout.h"
//#include "XFramework/client/XLayoutObj.h"
// #include "XSceneTech.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndButtonRadio.h"
#include "_Wnd2/XWndList.h"
#include "XStageObj.h"
#include "XCampObj.h"
#include "XCampObjHero2.h"
#include "XStageObjHero2.h"
//#include "XGame.h"
#include "XAccount.h"
#include "JWWnd.h"
//#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XFramework/client/XEContent.h"
#ifdef _CHEAT
#include "client/XAppMain.h"
#endif // _CHEAT

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xCampaign;
using namespace XGAME;
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif


int XWndCampaignHero2::s_idxFloorSelected = -1;
XE::VEC2 XWndCampaignHero2::s_vPosScroll;
////////////////////////////////////////////////////////////////
/**
 @brief 영웅의 전장
*/
XWndCampaignHero2::XWndCampaignHero2( XSpotCommon *pSpot, XSPCampObj spCampObj )
	: XWndPopup( _T( "camp_hero.xml" ), "popup" )
{
	Init();
	m_pSpot = pSpot;
	m_pProp = SafeCast<XPropCamp::xPropHero*>( spCampObj->GetpProp() );
	auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
	if( pWndList ) {
		if( !s_vPosScroll.IsZero() ) {
			pWndList->SetFocusView( s_vPosScroll );
		}
		pWndList->SetEvent( XWM_SELECT_ELEM, this, &XWndCampaignHero2::OnClickStage );
	}
	m_bProgressLoading = true;
	if( s_idxFloorSelected < 0 ) {
		auto spCampObjHero = GetspCampObj();
		if( spCampObjHero ) {
			// 층이 선택이 안되어있으면 가장 상위층을 처음에 선택함.
			s_idxFloorSelected = spCampObjHero->GetidxFloorByOpen();
		}
	}
	// 필터 버튼
	for( int i = 0; i < 4; ++i ) {
		auto pButt = xGET_BUTT_CTRLF( this, "butt.filter.%d", i );
		if( pButt ) {
			pButt->SetEvent2( XWM_CLICKED, [this, i]( XWnd* pWnd ) {
				if( i != m_idFilter ) {
					m_idFilter = i;
					m_aryFiltered.clear();
					auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
					if( XASSERT( pWndList ) ) {
						pWndList->DestroyAllItem();
					}
					SetbUpdate( true );
				}
			});
			pButt->SetPush( i == (int)m_idFilter );
		}
	}

}

BOOL XWndCampaignHero2::OnCreate()
{
	XWndPopup::OnCreate();
	UpdateFloorButts( GetspCampObj() );
	return TRUE;
}


void XWndCampaignHero2::OnDestroy()
{
	auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
	if( XASSERT( pWndList ) ) {
		s_vPosScroll = pWndList->GetFocusView();
	}
	XWndPopup::OnDestroy();
}

XSPCampObjHero2 XWndCampaignHero2::GetspCampObj()
{
	return std::static_pointer_cast<XCampObjHero2>( m_pSpot->GetspCampObj() );
}

xCampaign::xtStageState
XWndCampaignHero2::GetStageState( XSPStageObj spStage, int idxFloor ) 
{
	auto spCampObj = GetspCampObj();
	if( idxFloor < spCampObj->GetidxFloorByOpen() )
		return xSS_RETRY;
	int idxStage = spStage->GetidxStage();
	auto idxLastUnlock = spCampObj->GetidxLastUnlock();
	if( idxStage < idxLastUnlock )
		return xSS_RETRY;
	else
	if( idxStage == idxLastUnlock )
		return xSS_NEW_OPEN;
	else
		return xSS_LOCK;
}

void XWndCampaignHero2::UpdateFiltered()
{
	m_aryFiltered.clear();
	auto spCampObj = GetspCampObj();
	// 필터링
	int idxUnlock = spCampObj->GetidxLastUnlock();
	if( s_idxFloorSelected < spCampObj->GetidxFloorByOpen() )
		idxUnlock = -1;
	for( auto spStageObj : spCampObj->GetaryStages( s_idxFloorSelected ) ) {
		const int idxStage = spStageObj->GetidxStage();
		auto spStageObjHero = std::static_pointer_cast<XStageObjHero2>( spStageObj );
		if( XASSERT(spStageObjHero) ) {
			auto pPropHero = PROP_HERO->GetpProp( spStageObjHero->GetidHero() );
			if( XASSERT(pPropHero) ) {
				XSPStageObjHero2 spStageObjFiltered;
				if( m_idFilter == 0 ) {
					spStageObjFiltered = spStageObjHero;
				} else
				if( m_idFilter == 1 ) {
					if( pPropHero->typeAtk == xAT_TANKER ) {
						spStageObjFiltered = spStageObjHero;
					}
				} else
				if( m_idFilter == 2 ) {
					if( pPropHero->typeAtk == xAT_RANGE ) {
						spStageObjFiltered = spStageObjHero;
					}
				} else
				if( m_idFilter == 3 ) {
					if( pPropHero->typeAtk == xAT_SPEED ) {
						spStageObjFiltered = spStageObjHero;
					}
				}
				if( spStageObjFiltered ) {
					// 필터링 된건 바로 넣음.
					m_aryFiltered.Add( spStageObjFiltered );
// 					// idxUnlock스테이지는 뒤에 다시 추가하지 않기 위해.
// 					if( idxStage == idxUnlock )
// 						idxUnlock = -1;
				} else {
					// 필터링 걸러진것중에서 idxUnlock은 무조건 넣음.
// 					if( idxStage == idxUnlock ) {
// 						m_aryFiltered.Add( spStageObjHero );
// 					}
				}
			} // pPropHero
		}
	}
// 	if( idxUnlock >= 0 ) {
// 		// 언락된 마지막스테이지는 항상 표시됨.
// 		m_aryFiltered.Add( spCampObj->GetspStageObjHero( idxUnlock, s_idxFloorSelected ) );
// 	}
}

void XWndCampaignHero2::Update()
{
	if( m_aryFiltered.Size() == 0 ) {
		UpdateFiltered();
	}
	_tstring strName = XTEXT( m_pProp->idName );
	auto spCampObj = GetspCampObj();
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		strName += XFORMAT( "(%s)", m_pProp->strIdentifier.c_str() );
	}
#endif // cheat
	xSET_TEXT( this, "text.title", strName );
	//
	auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
	if( XASSERT( pWndList ) ) {
// 		for( auto spStageObj : spCampObj->GetaryStages( s_idxFloorSelected ) ) {
		for( auto spStageObj : m_aryFiltered ) {
			auto spPropStage = spStageObj->GetspPropStage();
			XWnd *pElem = pWndList->FindByID( spPropStage->idProp );
			if( pElem == nullptr ) {
				pElem = new XWnd( XE::VEC2( 0 ), XE::VEC2( 67, 134 ) );
				pWndList->AddItem( spPropStage->idProp, pElem );
			}
			if( !m_bProgressLoading ) {
				// 단계별 로딩모드에선 지금 생성하지 않는다.
				pElem = UpdateListElem( pElem, spStageObj );
			}
		}
		// 리스트 선택 이벤트 핸들러를 등록요청한다.
//		SetEventByList( pWndList );   // virtual
	}
	m_idxProgressLoading = 0;
	// 리셋주기가 있으면 남은시간을 위해 자동업데이트를 켠다.
	if( XASSERT(m_pProp->secResetCycle) ) {
		SetAutoUpdate( 0.1f );
		xSET_SHOW( this, "text.remain.reset", true );
	} else {
		ClearAutoUpdate();
		xSET_SHOW( this, "text.remain.reset", false );
	}
	const int apNeed = ACCOUNT->GetAPPerBattle();
	xSET_TEXT_FORMAT( this, "text.ap", _T("%d"), apNeed );
	//
	XWndPopup::Update();
}

void XWndCampaignHero2::UpdateFloorButts( XSPCampObjHero2 spCampObj )
{
	// 단계 버튼들
	const int idxFloorByOpen = spCampObj->GetidxFloorByOpen();
	XVector<XWnd*> aryButt;
	for( int i = 0; i < 10; ++i ) {
		auto pWndButt = xGET_BUTT_CTRLF( this, "butt.level%d", i + 1 );
		if( pWndButt ) {
			pWndButt->SetX(0);
			if( s_idxFloorSelected == i )
				pWndButt->SetPush( TRUE );
			else
				pWndButt->SetPush( FALSE );
			// 잠긴버튼 하나만 보이게 하고 나머지는 다 감춤.
			const bool bShow = ( i - 1 <= idxFloorByOpen );
			const bool bOpen = ( i <= idxFloorByOpen );
			pWndButt->SetbShow( bShow );
			xSET_SHOW( pWndButt, "img.key", !bOpen );		// 자물쇠 on/off
			if( bShow ) {
				xSET_TEXT_FORMAT( pWndButt, "text.num", _T( "%d" ), i + 1 );
				aryButt.Add( pWndButt );
			}
			if( !bOpen ) {
				pWndButt->SetblendFunc( XE::xBF_GRAY );
			}
			pWndButt->SetEvent( XWM_CLICKED, this, &XWndCampaignHero2::OnClickFloor, i );
		}
	}
	// 중앙을 기준으로 버튼사이에 틈이 없도록 배치
	AutoLayoutHCenterWithAry( aryButt, 0 );
}

void XWndCampaignHero2::OnAutoUpdate()
{
	if( m_pProp->secResetCycle ) {
		// 남은시간 업데이트
		auto spCampObj = GetspCampObj();
		if( XASSERT(spCampObj) ) {
			const auto secRemain = spCampObj->GetsecRemainReset();
			if( secRemain > 0 ) {
				const _tstring strRemain = XFORMAT( "%s:%s", XTEXT( 80131 )		// 남은 시간
																										, XGAME::GetstrResearchTime( secRemain ).c_str() );	
				xSET_TEXT( this, "text.remain.reset", strRemain );
			}
		}
	}
}

/**
 @brief 
*/
int XWndCampaignHero2::OnClickFloor( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("%s:%d", __TFUNC__, p1);
	//
	int idxFloor = (int)p1;
	auto spCampObj = GetspCampObj();
	if( idxFloor <= spCampObj->GetidxFloorByOpen() ) {
		if( s_idxFloorSelected != idxFloor ) {
			s_idxFloorSelected = idxFloor;
			auto pWndButt = dynamic_cast<XWndButtonRadio*>( Findf( "butt.level%d", idxFloor+1 ) );
			if( pWndButt ) {
				auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
				if( XASSERT( pWndList ) ) {
					pWndList->DestroyAllItem();
				}
				m_aryFiltered.clear();
				UpdateFloorButts( spCampObj );
				SetbUpdate( true );
			}
		}
	} else {
		XWND_ALERT( "%s", XTEXT(2342) );	// 이전난이도를 모두 클리어 해야함.
		auto pButt = xGET_BUTT_CTRLF( this, "butt.level%d", idxFloor + 1 );
		if( pButt )
			pButt->SetPush( FALSE );
		pButt = xGET_BUTT_CTRLF( this, "butt.level%d", s_idxFloorSelected + 1 );
		if( pButt )
			pButt->SetPush( TRUE );
	}
	return 1;
}


XWnd* XWndCampaignHero2::UpdateListElem( XWnd *pElem, XSPStageObj spStageObj )
{
	// UpdateListElem에서 공통되는 코드들 다 위로 올리자
	auto spCampObj = GetspCampObj();
	auto spPropStage = spStageObj->GetspPropStage();
	auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
	if( !pWndList )
		return nullptr;
	const int idxStage = spPropStage->idxStage;
	if( pElem->GetNumChild() == 0 ) {
		const auto strcNode = pWndList->GetstrItemLayoutForXML();
		XBREAK( strcNode.empty() );
		GetpLayout()->CreateLayout( strcNode, pElem );
		pElem->SetstrIdentifierf( "stage.%d", idxStage );
	}
	auto spStageObjHero
		= std::static_pointer_cast<XStageObjHero2>( spStageObj );
	if( XBREAK( spStageObj == nullptr ) )
		return nullptr;
	const int numStage = spCampObj->GetNumStages( s_idxFloorSelected );
	XBREAK( idxStage < 0 );
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
			auto col = xGetColorPower( spStageObj->GetPower(), XAccount::sGetPlayer()->GetPowerExcludeEmpty() );
			pText->SetColorText( col );
		}
	}


	SetHeroFace( pElem, spStageObjHero );
	// 일단 모두 끔
	xSET_SHOW( pElem, "img.vcheck", false );
	xSET_ENABLE( pElem, "butt.stage", false );
	// 현재 스테이지 상태
	auto state = GetStageState( spStageObj, s_idxFloorSelected );
	xSET_SHOW( pElem, "img.elem.retry", state == xSS_RETRY);
	xSET_SHOW( pElem, "img.elem.new", state == xSS_NEW_OPEN );
	xSET_SHOW( pElem, "img.elem.lock", state == xSS_LOCK );
	xSET_SHOW( pElem, "wnd.disable", state == xSS_LOCK );	// lock상태일때 위에 덮을 검은장막과 자물쇠
	switch( state )
	{
	case xSS_RETRY: {
		// 재도전 가능한곳.
//		ShowStar( pElem, true, spStageObj->GetnumStar() );
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
	case xSS_NEW_OPEN: {
		// 새로오픈된 스테이지
//		ShowStar( pElem, true, 0 );
		ShowRetryMark( pElem, false, 0 );
		xSET_ENABLE( pElem, "butt.stage", true );
	} break;
	case xSS_LOCK:
//		ShowStar( pElem, false, 0 );
		ShowRetryMark( pElem, false, 0 );
//		xSET_SHOW( pElem, "wnd.disable", true );	// lock상태일때 위에 덮을 검은장막과 자물쇠
		pButt->SetbActive( false );
		break;
	default:
		XBREAK(1);
		break;
	}
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
#ifdef _DEBUG
		auto pText = XWndTextString::sUpdateCtrl( pElem, "text.debug.level", XE::VEC2(2,20), FONT_NANUM, 12, true );
		pText->SetText( XFORMAT( "lvLegion:%d", spStageObj->GetLevelLegion() ) );
// 		pText->SetText( XFORMAT("lvLegion:%d(lvLimit:%d)", spPropStage->legion.lvLegion, spPropStage->levelLimit) );
#endif // _DEBUG
	}
#endif // CHEAT
	return pElem;
}

int XWndCampaignHero2::Process( float dt )
{
	if( !XEContent::sGet()->IsbDraging() )
		ProgressLoading( );
	return XWndPopup::Process( dt );
}

void XWndCampaignHero2::ProgressLoading()
{
	auto spCampObj = GetspCampObj();
	if( m_aryFiltered.Size() == 0 ) {
		UpdateFiltered();
	}
	const int numStages = m_aryFiltered.Size();
// 	if( m_idxProgressLoading < spCampObj->GetNumStages( s_idxFloorSelected ) ) {
	if( m_idxProgressLoading < numStages ) {
		auto pWndList = xGET_LIST_CTRL( this, "list.stage" );
		if( pWndList == nullptr )
			return;
// 		auto spStageObj = GetspCampObj()->GetspStage( m_idxProgressLoading++, s_idxFloorSelected );
		auto spStageObj = m_aryFiltered[ m_idxProgressLoading++ ];
		if( spStageObj ) {
			auto pElem = pWndList->Find( spStageObj->GetidProp() );
			UpdateListElem( pElem, spStageObj );
		}
		if( m_idxProgressLoading >= numStages ) {
//			m_bProgressLoading = false;
			CONSOLE("finished load camp hero");
		}
	}
}

/**
 @brief 재도전 UI를 보이거나 감춤.
*/
void XWndCampaignHero2::ShowRetryMark( XWnd *pRoot, bool bShow, int num )
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
void XWndCampaignHero2::SetHeroFace( XWnd *pElem
																	, XSPStageObjHero2 spStageObj )
{
	XBREAK( spStageObj == nullptr );
	auto pWndFace = SafeCast<XWndStoragyItemElem*>( pElem->Find( "wnd.hero.face" ) );
	if( pWndFace ) {
		const ID idPropHero = spStageObj->GetidHero();
		if( XASSERT(idPropHero) )
			pWndFace->SetHero( idPropHero );
		pWndFace->SetNum( 0 );
		auto num = ACCOUNT->GetNumSoulStoneWithidPropHero( idPropHero );
		auto pHero = ACCOUNT->GetHeroByidProp( idPropHero );
		auto pText = XWndTextString::sUpdateCtrl( pWndFace, "text.have.hero", XE::VEC2( 2, 2 ), FONT_NANUM, 15, true );
		_tstring str;
		if( pHero ) {
			// 보유한 영웅
			str = XFORMAT( "%s(%d)", XTEXT(2341), num );		// 보유영웅(0)
		} else {
			if( num > 0 ) {
				str = XFORMAT( "(%d)", num );
			}
		}
		if( pText ) {
			pText->SetStyleStroke();
			pText->SetText( str );
		}
	}
}

int XWndCampaignHero2::OnClickStage( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	ID idPropStageObj = p2;
	CONSOLE( "OnClickStage:%d", idPropStageObj );

//	int idxStage = (int)p1;
	if( ACCOUNT->GetAP() < ACCOUNT->GetAPPerBattle() ) {
		XWND_ALERT( "%s", XTEXT( 2240 ) );
		return 1;
	}
	// 어느스팟의 몇번째 스테이지를 선택했는지 서버로 보낸다.
	auto spCampObj = GetspCampObj();
// 	auto spStageObj = spCampObj->GetspStage( idxStage );
	auto spStageObj = spCampObj->GetspStageObjWithidProp( idPropStageObj, s_idxFloorSelected );
	if( XBREAK( spStageObj == nullptr ) )
		return 1;
	const auto spPropStage = spStageObj->GetspPropStage();
	const int idxStage = spPropStage->idxStage;
	const auto lvLimit = spCampObj->GetlvLimit( idxStage );
	if( lvLimit && ACCOUNT->GetLevel() < lvLimit ) {
		XWND_ALERT( "%s", XTEXT( 2089 ) );
		return 1;
	}
	const auto errCode = spCampObj->IsAbleTry( spStageObj, s_idxFloorSelected );
	switch( errCode ) {
	case xE_LOCK:
		return 1;
	case xE_NOT_ENOUGH_NUM_TRY:
		return 1;
	case xE_NOT_ENOUGH_NUM_CLEAR:
		return 1;
	case xE_OK:
		break;
	default:
		XBREAK(1);
		break;
	}
	GAMESVR_SOCKET->SendReqClickStageInCampaign( GAME,
																								m_pSpot->GetidSpot(),
																								m_pSpot->GetsnSpot(),
																								spPropStage->idxStage,
																								s_idxFloorSelected,
																								spPropStage->idProp,
																								/*getid()*/0 );
	return 1;
}


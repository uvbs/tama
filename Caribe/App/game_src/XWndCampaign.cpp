#include "stdafx.h"
#include "XWndCampaign.h"
#include "XFramework/client/XLayout.h"
#include "XFramework/client/XLayoutObj.h"
#include "XSceneTech.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndList.h"
#include "XStageObj.h"
#include "XCampObj.h"
#include "XCampObjHero.h"
#include "XStageObjHero.h"
//#include "XGame.h"
#include "XAccount.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XPropLegionH.h"
#include "XWndStorageItemElem.h"
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

/**
 @brief 캠페인관련 에러창을 띄운다.
 @return 에러 팝업을 띄웠으면 true;
*/
bool xCampaign::sDoAlertCampaignByError( xtError errCode )
{
	switch( errCode ) {
	case xE_NOT_ENOUGH_LEVEL:
		XWND_ALERT( "%s", XTEXT( 2089 ) );
		break;
	case xE_NOT_ENOUGH_AP:
		XWND_ALERT( "%s", _T( "Not enough AP" ) );
		break;
	case xE_NOT_ENOUGH_NUM_TRY:
		XWND_ALERT( "%s", XTEXT( 2155 ) );
		break;
	case xE_LOCK:
		XWND_ALERT( "%s", XTEXT( 2157 ) );	// 잠겨있습니다.
		break;
	case xE_NOT_ENOUGH_NUM_CLEAR:
		XWND_ALERT( "%s", XTEXT( 2156 ) );	// 그외 모든 에러(더이상 도전할수 엄슴)
		break;
	case xE_OK: {
		return false;
	} break;
	default:
		XWND_ALERT( "%s", XTEXT( 2156 ) );	// 그외 모든 에러(더이상 도전할수 엄슴)
	}
	return true;
}

/**
 @brief 전투전 팝업을 띄운다. 캠페인 전용이다.
*/
void xCampaign::sDoPopupSpotWithStageObj( XSpot* pBaseSpot, XSPCampObj spCampObj, XSPStageObj spStageObj, int idxFloor )
{
	const int idxStage = spStageObj->GetidxStage();
	auto pSpot = pBaseSpot;
	// 전투전 팝업을 띄움.
	pSpot->SetLevel( spStageObj->GetLevelLegion() );
	XCampObj::s_idxStage = idxStage;
	XCampObj::s_idxFloor = idxFloor;

	auto pPopup = new XWndPopupSpotMenu( pSpot, idxStage );
	pPopup->AddMenuAttack();			// 공격버튼
	pPopup->SetPower( spStageObj->GetPower() );
	pPopup->SetLevel( spStageObj->GetLevelLegion() );
	_tstring strName = spCampObj->GetNameCamp();
	if( strName.empty() )
		strName = pSpot->GetszName();
	pPopup->SetstrName( strName );
	pPopup->SetItems( spStageObj->GetaryDrops() );
	XGAME::xGetpScene()->Add( pPopup );
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
			strRemain += XSceneTech::_sGetResearchTime( secRemain );
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
												0,
												spPropStage->idProp,
												getid() );
	return 1;
}

// void XWndCampaignBase::OnRecvClickStage( XGAME::xtError errCode, int idxStage )
// {
// 	auto spCampObj = GetspCampObj();
// 	switch( errCode )
// 	{
// 	case XGAME::xE_NOT_ENOUGH_LEVEL:
// 		XWND_ALERT( "%s", XTEXT( 2089 ) );
// 		break;
// 	case XGAME::xE_NOT_ENOUGH_AP:
// 		XWND_ALERT( "%s", _T( "Not enough AP" ) );
// 		break;
// 	case XGAME::xE_NOT_ENOUGH_NUM_TRY:
// 		XWND_ALERT( "%s", XTEXT( 2155 ) );
// 		break;
// 	case XGAME::xE_LOCK:
// 		XWND_ALERT( "%s", XTEXT( 2157 ) );	// 잠겨있습니다.
// 		break;
// 	case XGAME::xE_NOT_ENOUGH_NUM_CLEAR:
// 		XWND_ALERT( "%s", XTEXT( 2156 ) );	// 그외 모든 에러(더이상 도전할수 엄슴)
// 		break;
// 	case XGAME::xE_OK: {
// 		auto spStageObj = spCampObj->GetspStage( idxStage );
// 		if( XBREAK(spStageObj == nullptr) )
// 			return;
// 		// 전투전 팝업을 띄움.
// 		m_pBaseSpot->SetLevel( spStageObj->GetLevelLegion() );
// 		auto pPopup = new XWndPopupSpotMenu( m_pBaseSpot, idxStage );
// 		pPopup->AddMenuAttack();			// 공격버튼
// 		pPopup->SetPower( spStageObj->GetPower() );
// 		pPopup->SetLevel( spStageObj->GetLevelLegion() );
// 		_tstring strName = spCampObj->GetNameCamp();
// 		if( strName.empty() )
// 			strName = m_pBaseSpot->GetszName();
// 		pPopup->SetstrName( strName );
// 		pPopup->SetItems( spStageObj->GetaryDrops() );
// 		GAME->GetpScene()->Add( pPopup );
// 	} break;
// 	default:
// 		XWND_ALERT( "%s", XTEXT( 2156 ) );	// 그외 모든 에러(더이상 도전할수 엄슴)
// 		break;
// 	}
// }

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
		int lvLegion = spPropStage->m_spxLegion->lvLegion;
		if( lvLegion == 0 )
			lvLegion = ACCOUNT->GetLevel() + spPropStage->m_spxLegion->adjLvLegion;
		auto pText = xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", lvLegion ) );
		if( pText ) {
			XCOLOR col = XCOLOR_WHITE;
			if( spStageObj->GetPower() )
				col = XGAME::xGetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
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
	const int lvLimit = spCampObj->GetlvLimit( idxStage );
	if( lvLimit != 0 && ACCOUNT->GetLevel() < lvLimit ) {
		XWND_ALERT( "%s", XTEXT( 2089 ) );
		return 1;
	}
// 	if( spPropStage->levelLimit != 0 &&
// 		ACCOUNT->GetLevel() < spPropStage->levelLimit ) {
// 		XWND_ALERT( "%s", XTEXT( 2089 ) );
// 		return 1;
// 	}
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
		xCampaign::sDoPopupSpotWithStageObj( m_pBaseSpot, spCampObj, spStageObj, 0 );
// 		auto pPopup = new XWndPopupSpotMenu( m_pBaseSpot, idxStage );
// 		//	pPopup->SetidCloseByAttack( getid() );	// 공격시작으로 전투팝업이 닫힐때 this도 함께 닫힘.
// 		pPopup->AddMenuAttack();			// 공격버튼
// 		pPopup->SetPower( spStageObj->GetPower() );
// 		pPopup->SetLevel( spStageObj->GetLevelLegion() );
// 		_tstring strName = spCampObj->GetNameCamp();
// 		if( strName.empty() )
// 			strName = m_pBaseSpot->GetszName();
// 		pPopup->SetstrName( strName );
// 		pPopup->SetItems( spStageObj->GetaryDrops() );
// 		GAME->GetpScene()->Add( pPopup );
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
	xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", spPropStage->m_spxLegion->lvLegion ) );
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
				auto strRemain = XSceneTech::_sGetResearchTime( secRemain );
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
			auto strRemain = XSceneTech::_sGetResearchTime( secRemain );
			xSET_TEXT( this, "text.remain.reset", strRemain );
		} else {
			xSET_TEXT( this, "text.remain.reset", XTEXT(2125) );	// 공격을 시작하십시오.
		}
	}
	xSET_SHOW( this, "text.remain.reset", true );
	auto secRemain = spCampObj->GetsecRemainClose();
	auto strRemain = XSceneTech::_sGetResearchTime( secRemain );
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
		const int lvLimit = spCampObj->GetlvLimit( idxStage );
		if( lvLimit != 0 && ACCOUNT->GetLevel() < lvLimit ) {
			XWND_ALERT( "%s", XTEXT( 2089 ) );
			return 1;
		}
// 		if( spPropStage->levelLimit != 0 &&
// 			ACCOUNT->GetLevel() < spPropStage->levelLimit )	{
// 			XWND_ALERT( "%s", XTEXT( 2089 ) );
// 			return 1;
// 		}
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
	int lvLegion = spPropStage->m_spxLegion->lvLegion;
	if( lvLegion == 0 )
		lvLegion = ACCOUNT->GetLevel() + spPropStage->m_spxLegion->adjLvLegion;
	auto pText = xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", lvLegion ) );
	if( pText ) {
		XCOLOR col = XCOLOR_WHITE;
		if( spStageObj->GetPower() )
			col = XGAME::xGetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
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
		int lvLegion = spPropStage->m_spxLegion->lvLegion;
		if( lvLegion == 0 )
			lvLegion = ACCOUNT->GetLevel() + spPropStage->m_spxLegion->adjLvLegion;
		auto pText = xSET_TEXT( pElem, "text.lv.legion", XFORMAT( "%d", lvLegion ) );
		if( pText ) {
			pText->SetbShow( TRUE );
			XCOLOR col = XCOLOR_WHITE;
			if( spStageObj->GetPower() )
				col = XGAME::xGetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
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
 @brief 영웅의 전장
*/
XWndCampaignHero::XWndCampaignHero( XSpotCommon *pSpot, xCampaign::CampObjPtr& spCampObj )
	: XWndCampaignBase( pSpot, spCampObj )
{
	Init();
//	GetpLayout()->CreateLayout( "hero_camp", this );
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
			auto col = XGAME::xGetColorPower( spStageObj->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
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
		pText->SetText( XFORMAT( "%d(%d)", spPropStage->m_spxLegion->lvLegion
																		, spCampObj->GetlvLimit( idxStage ) ) );
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


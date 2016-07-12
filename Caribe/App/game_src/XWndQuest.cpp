#include "stdafx.h"
#include "XWndQuest.h"
#include "_Wnd2/XWndPopup.h"
#include "_Wnd2/XWndCtrls.h"
#include "XQuest.h"
#include "XQuestCon.h"
#include "XQuestMng.h"
#include "XQuestProp.h"
#include "XAccount.h"
#include "XGame.h"
#include "XSceneWorld.h"
#include "XSoundMng.h"
#include "XWorld.h"
#include "JBWnd.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"
#include "Sprite/Layer.h"
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

using namespace XGAME;

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
				SOUNDMNG->OpenPlaySoundBySec(26, 1.f);
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
			if( pBaseSpot ) {		// 퀘스팟의 경우 사라지기때문에 null일수 있음.
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
// 	if( m_pQuestObj->IsOverClickedTime() ) {
// 		// 글로우 작동
// 		CONSOLE("퀘스트[%s], Glow작동", m_pQuestObj->GetstrIdentifer().c_str() );
// 		if( Find("spr.glow") == nullptr ) {
// 			auto pWndSpr = new XWndSprObj( _T( "ui_glow_quest.spr" ), 1, 0, 0, xRPT_1PLAY );
// 			pWndSpr->SetstrIdentifier( "spr.glow" );
// 			Add( pWndSpr );
// 		}
// 		// 타이머 리셋
// 		m_pQuestObj->UpdatesecClicked( false );
// 	}
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
void XWndQuestInfo::sDestroyWnd() {
	XE::GetGame()->DestroyWndByIdentifier( "popup.quest.info" );
}
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
		pButt->SetGlow( _T("_glow"), XE::VEC2(-5,-6) );
	if( bAllComplete ) {
		auto pWndCong = new XWndCongratulation();
		GAME->Add( pWndCong );
	}
	// 퀘 제목
	XE::VEC2 v(0,0); {
		auto pText = xSET_TEXT( this, "quest.info.name", strName );
		if( pText ) {
			v = pText->GetPosLocal();
			auto size = pText->GetSizeNoTransLayout();
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
			auto size = pText->GetSizeNoTransLayout();
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
			auto size = pText->GetSizeNoTransLayout();
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
		auto size = pText->GetSizeNoTransLayout();
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
			auto size = pText->GetSizeNoTransLayout();
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
			auto size = pText->GetSizeNoTransLayout();
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
//				XARRAYLINEAR_LOOP_AUTO( drop.aryWhere, &where ) {
				for( auto& where : drop.aryWhere ) {
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
				}
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
XWndCongratulation::XWndCongratulation()
	: XWndSprObj(_T("ui_firework.spr"), 2, XE::GetGameSize() * .5f, xRPT_1PLAY )
{
	Init();
	auto vPos = GetPosLocal();
	auto vSize = GetSizeLocal();
	auto rect = GetBoundBoxByVisibleLocal();
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


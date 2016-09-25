#include "stdafx.h"
#include "XWndUnitOrg.h"
#include "XHero.h"
#include "XFramework/client/XLayout.h"
#include "JWWnd.h"
#include "XWndTemplate.h"
#include "XAccount.h"
#include "XGame.h"
#include "XSockGameSvr.h"
#include "XExpTableHero.h"
#include "skill/XSkillDat.h"
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

/**
 @brief 
*/
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
		auto pWndUnit = new XWndCircleUnit( pHero->GetUnit(), XE::VEC2(55,-10), pHero );
		pWndUnit->SetbShowLevelSquad( false );
		pRoot->Add( pWndUnit );
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
		auto pWndSkill = new XWndCircleSkill( pDat, XE::VEC2(55,-10), nullptr );
		pWndSkill->SetScaleLocal( 0.6f );
		pRoot->Add( pWndSkill );
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
			m_lvBar = m_Level;
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
	if( m_Level == 0 ) {
		m_Level = pHero->GetLevel( typeTrain );
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
		const int lvBar = (m_lvBar)? m_lvBar : m_pHero->GetLevel( m_typeTrain );
		xSET_TEXT( this, "text.min.gold", XE::NtS( GetGoldMinForTrain( m_typeTrain, lvBar ) ) );
// 		xSET_TEXT( this, "text.min.gold", XE::NtS(GetGoldMinForTrain( m_pHero->GetLevel() )) );
		// 훈련시간 표시
		xSET_TEXT( this, "text.time", XGAME::GetstrResearchTime( m_secTrainByGold ) );
		if( m_GoldUse == 0 ) {
// 			int minGold = GetGoldMinForTrain( pHero->GetLevel() );
			const int minGold = GetGoldMinForTrain( m_typeTrain, lvBar );
			xSET_TEXT( this, "text.cost", XE::NumberToMoneyString( minGold ) );
		} else {
			xSET_TEXT( this, "text.cost", XE::NumberToMoneyString( m_GoldUse ) );
		}
		xSET_ENABLE( this, "butt.train", bGoldUse );
#ifdef _CHEAT
		if( XAPP->m_bDebugMode ) {
			const int expRest = m_ExpRest;
			const int level = m_Level;
// 			if( m_ExpRest == 0 )
// 				m_ExpRest = m_pHero->GetExp( m_typeTrain );
// 			if( m_Level == 0 ) {
// 				m_Level = lvTrain;
// 			}
			_tstring strExpTrain = XE::NumberToMoneyString( m_ExpByGold );
			_tstring strExpCurr = XE::NumberToMoneyString( expRest );
			auto expMaxAfter = m_pHero->GetMaxExpWithLevel( m_typeTrain, level );
			_tstring strExpMax = XE::NumberToMoneyString( expMaxAfter );
			xSET_TEXT( this, "text.graph",
				XFORMAT( "Lv%d %s(%s)/%s", level
																, strExpCurr.c_str()
																, strExpTrain.c_str()
																, strExpMax.c_str() ) );
		}
#endif // _CHEAT
	}
	int lvCurrByAni = pBar->GetlvCurrByAni();
	if( lvCurrByAni == 0 )
		lvCurrByAni = lvTrain;
	m_lvBar = lvCurrByAni;
	int lvMax = pHero->GetMaxLevel( m_typeTrain );
//	xSET_SHOW( this, "text.next", lvCurrByAni <= lvMax );
	if( lvCurrByAni >= lvMax )
		xSET_TEXT( this, "text.next", XTEXT(2111) );		// "다음레벨"을 "최대레벨"로 바꾼다.
	switch( m_typeTrain )
	{
	case XGAME::xTR_LEVEL_UP: {
		int lv = lvCurrByAni;
		std::vector<float> aryOrig;
		const bool bShow = true;
		aryOrig.push_back( pHero->GetAttackMeleeRatio( bShow )  );
		aryOrig.push_back( pHero->GetAttackRangeRatio( bShow )  );
		aryOrig.push_back( pHero->GetDefenseRatio( bShow )  );
		aryOrig.push_back( pHero->GetHpMaxRatio( bShow )  );
		aryOrig.push_back( pHero->GetAttackSpeed( bShow )  );
		aryOrig.push_back( pHero->GetMoveSpeed( bShow )  );
//		const int lvCurrByAni = pBar->GetlvCurrByAni();
		std::vector<float> aryStat;
		aryStat.push_back( pHero->GetAttackMeleeRatio( lv, bShow )  );
		aryStat.push_back( pHero->GetAttackRangeRatio( lv, bShow )  );
		aryStat.push_back( pHero->GetDefenseRatio( lv, bShow )  );
		aryStat.push_back( pHero->GetHpMaxRatio( lv, bShow )  );
		aryStat.push_back( pHero->GetAttackSpeed( lv, bShow )  );
		aryStat.push_back( pHero->GetMoveSpeed( lv, bShow )  );
		for( int i = 0; i < 6; ++i ) {
			auto statOrig = aryOrig[i];
			auto statUp = aryStat[i];
			xSET_TEXTF( this, XFORMAT("%.0f%%", statOrig), "text.stat.%d", i+1 );
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
	m_lvBar = lvCurrByAni;
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
	const auto goldMinForTrain = GetGoldMinForTrain( m_typeTrain, m_lvBar );
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
	m_GoldUse -= goldMinForTrain;
// 	m_GoldUse -= GetGoldMinForTrain( m_pHero->GetLevel() );
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
// 	auto goldMinForTrain = GetGoldMinForTrain( m_pHero->GetLevel() );
//	const int lvBar = (m_lvBar)? m_lvBar : m_pHero->GetLevel();
	const auto goldMinForTrain = GetGoldMinForTrain( m_typeTrain, m_lvBar );
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
	else {
		m_GoldUse = GetGoldMinForTrain( m_typeTrain, m_lvBar );
	}
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

int XWndLevelupHero::GetGoldMinForTrain( XGAME::xtTrain typeTrain, int lvHero ) const
{
	int goldPerLv = XAccount::sGetGoldByMaxExp( lvHero, typeTrain );
// 	const int goldPerLv = EXP_TABLE_HERO->GetGoldByLvUp( lvHero );
	int goldMin = (int)(goldPerLv * 0.1f);
	if( goldMin < 500 )
		goldMin = 500;
	return goldMin;
//	return (int)( ::powf( (float)lvHero, 2.f ) * 500.f );
}

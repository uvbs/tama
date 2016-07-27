#include "stdafx.h"
#include "XGameEtc.h"
#include "XFramework/client/XLayout.h"
#include "XPropHero.h"
#include "XPropUnit.h"
#include "skill/xSkill.h"
#include "XSkillMng.h"
#include "XGame.h"
#include "XAccount.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndImage.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

//////////////////////////////////////////////////////////////////////////
void XGAME::UpdateHeroTooltip( ID idHero, XWnd *pRoot, int cost )
{
	auto pProp = PROP_HERO->GetpProp( idHero );
	if( XASSERT( pProp ) )
		XGAME::UpdateHeroTooltip( pProp, pRoot, cost );
}

void XGAME::UpdateHeroTooltip( LPCTSTR idsHero, XWnd *pRoot, int cost )
{
	auto pProp = PROP_HERO->GetpProp( idsHero );
	if( XASSERT(pProp) )
		UpdateHeroTooltip( pProp, pRoot, cost );
}

/**
 @brief pRoot아래에 있는 아이템 툴팁모듈을 업데이트한다.
 @param cost 아이템 가격을 표시해야할때 가격을 넘겨준다.
*/
void XGAME::UpdateHeroTooltip( const XPropHero::xPROP *pProp, XWnd *pRoot, int cost )
{
	// 영웅 이미지
	if( pProp == nullptr )
		return;
	// 영웅 이름
	auto pText =
	xSET_TEXT( pRoot, "text.name", pProp->GetstrName() );
// 	if( pText )
// 		pText->SetColorText( XGAME::GetGradeColor( pProp->GetGrade() ) );
	// 영웅 이미지
	xSET_IMG( pRoot, "img.hero", XE::MakePath( DIR_IMG, pProp->strFace ), XE::xPF_ARGB8888 );
	// 별
// 	int numStar = pProp->GetGrade();
// 	for( int i = 0; i < 5; ++i ) {
// 		bool bFlag = false;
// 		if( i < numStar )
// 			bFlag = true;
// 		xSET_SHOWF( pRoot, bFlag, "img.star.%d", i+1 );
// 	}
	// 영웅 등급 텍스트
//	pText = xSET_TEXT( pRoot, "text.grade", XFORMAT("%s 장비", XGAME::GetStrGrade(pProp->grade)));
// 	if( pText )
// 		pText->SetColorText( XGAME::GetGradeColor( pProp->grade ) );
	// 스탯
	XWnd *pRootStat = pRoot->Find("wnd.stat.area");
	if( pRootStat ) {
		for( int i = 0; i < 6; ++i ) {
			auto pText = xGET_TEXT_CTRLF( pRoot, "text.stat.%d", i+1 );
			if( pText ) {
				float stat = pProp->GetStat2( (XGAME::xtStat)(i + 1), xGD_COMMON, 1, true );
				pText->SetText( XFORMAT("%.0f%%", stat) );
			}
		}
	}
	// 지휘가능 유닛
	std::vector<XGAME::xtUnit> ary;
	if( pProp->typeAtk == XGAME::xAT_TANKER ) {
		ary.push_back( XGAME::xUNIT_SPEARMAN );
		ary.push_back( XGAME::xUNIT_MINOTAUR );
		ary.push_back( XGAME::xUNIT_GOLEM );
	} else
	if( pProp->typeAtk == XGAME::xAT_RANGE ) {
		ary.push_back( XGAME::xUNIT_ARCHER );
		ary.push_back( XGAME::xUNIT_CYCLOPS );
		ary.push_back( XGAME::xUNIT_TREANT );
	} else
	if( pProp->typeAtk == XGAME::xAT_SPEED ) {
		ary.push_back( XGAME::xUNIT_PALADIN );
		ary.push_back( XGAME::xUNIT_LYCAN );
		ary.push_back( XGAME::xUNIT_FALLEN_ANGEL );
	}
	int idx = 0;
	for( auto unit : ary ) {
		auto pPropUnit = PROP_UNIT->GetpProp( unit );
		if( XASSERT(pPropUnit) ) {
			xSET_IMGF( pRoot, XGAME::GetResUnitSmall( unit ).c_str(), "img.unit.%d", idx+1 );
			++idx;
		}
	}
	{
		XWnd *pWnd = pRoot->Find( "key.skill.passive" );
		if( pWnd ) {
			auto pPropSkill = SKILL_MNG->FindByIdentifier( pProp->strPassive );
			if( XASSERT( pPropSkill ) ) {
				xSET_TEXT( pWnd, "text.name", pPropSkill->GetSkillName() );
				auto pImg = xSET_IMG( pWnd, "img.icon", pPropSkill->GetResIcon() );
				if( pImg )
					pImg->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickSkillTooltip, pPropSkill->GetidSkill() );
			}
		}
	}
	{
		XWnd *pWnd = pRoot->Find( "key.skill.active" );
		if( pWnd ) {
			auto pPropSkill = SKILL_MNG->FindByIdentifier( pProp->strActive );
			if( XASSERT( pPropSkill ) ) {
				xSET_TEXT( pWnd, "text.name", pPropSkill->GetSkillName() );
				auto pImg = xSET_IMG( pWnd, "img.icon", pPropSkill->GetResIcon() );
				if( pImg )
					pImg->SetEvent( XWM_CLICKED, GAME, &XGame::OnClickSkillTooltip, pPropSkill->GetidSkill() );
			}
		}
	}
	// 판매가격
	if( cost > 0 ) {
		xSET_SHOW( pRoot, "wnd.sell", true );
		pText = xSET_TEXT( pRoot, "text.cost", XE::NumberToMoneyString( cost ) );
		if( ACCOUNT->GetptGuild() < cost )
			pText->SetColorText( XCOLOR_RED );
	}
	else
		xSET_SHOW( pRoot, "wnd.sell", FALSE );
}
#include "StdAfx.h"
#include "XSceneUnitOrg.h"
#include "XWndUnitOrg.h"
#ifdef WIN32
#include "CaribeView.h"
#endif // WIN32
#include "XGame.h"
#include "XGameWnd.h"
#include "XPropUnit.h"
#include "XSockGameSvr.h"
#include "skill/xSkill.h"
#include "XSkillMng.h"
#include "XBaseItem.h"
#include "client/XAppMain.h"
#include "XPropSquad.h"
#include "XSceneTech.h"
#include "XPropUpgrade.h"
#include "XQuestMng.h"
#include "XLegion.h"
#include "XWndTemplate.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"
#include "_Wnd2/XWndProgressBar.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

XSceneUnitOrg *SCENE_UNITORG = nullptr;

XSceneEquip::xSPM_EQUIP XSceneUnitOrg::s_EquipParam;
XSceneUnitOrg::xTab XSceneUnitOrg::s_tabCurr = xTAB_NONE;		// 현재 선택된 탭

void XSceneUnitOrg::Destroy() 
{	
	SAFE_DELETE(m_pKeepHero);
//	SAFE_DELETE(m_pLayout);
	XBREAK( SCENE_UNITORG == nullptr );
	XBREAK( SCENE_UNITORG != this );
	SCENE_UNITORG = nullptr;
}

XSceneUnitOrg::XSceneUnitOrg(XGame *pGame, SceneParamPtr& spBaseParam)
	: XSceneBase( pGame, XGAME::xSC_UNIT_ORG )
//	: XSceneBase( pGame, XGAME::xSC_UNIT_ORG )
	, m_Layout(_T("layout_unitorg.xml"))
{ 
	XBREAK( SCENE_UNITORG != nullptr );
	SCENE_UNITORG = this;
	Init(); 
	std::shared_ptr<XSceneEquip::xSPM_EQUIP> spParam;
	if (spBaseParam != nullptr)
	{
		spParam = std::static_pointer_cast<XSceneEquip::xSPM_EQUIP>( spBaseParam );	
		s_EquipParam = (*spParam);
		m_bEquip = TRUE;
	} else
		s_EquipParam.Init();

	XLegion *ple = ACCOUNT->GetCurrLegion().get();

	if( s_tabCurr == xTAB_NONE )
		s_tabCurr = xTAB_SKILL;
	m_Layout.CreateLayout( "unitorg", this );
	XWndImage *pImg = SafeCast<XWndImage*, XWnd*>(Find("img.stat.icon"));
	if (pImg)
		pImg->SetScaleLocal(0.655f, 0.69f);
	m_Layout.CreateLayout( "unitorg_top_radio", this );
	SetbUpdate( TRUE );

#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		XE::VEC2 v(600, 67);
		XE::VEC2 vSize(32);
		auto pButt = new XWndButtonDebug( v, vSize, _T( "Hero LvUp" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneUnitOrg::OnClickCheatHeroLvUp, XGAME::xTR_LEVEL_UP );
		Add( pButt );
		v.y += 32.f;
		pButt = new XWndButtonDebug( v, vSize, _T( "squad up" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneUnitOrg::OnClickCheatHeroLvUp, XGAME::xTR_SQUAD_UP );
		Add( pButt );
		v.y += 32.f;
		pButt = new XWndButtonDebug( v, vSize, _T( "active up" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneUnitOrg::OnClickCheatHeroLvUp, XGAME::xTR_SKILL_ACTIVE_UP );
		Add( pButt );
		v.y += 32.f;
		pButt = new XWndButtonDebug( v, vSize, _T( "passive up" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneUnitOrg::OnClickCheatHeroLvUp, XGAME::xTR_SKILL_PASSIVE_UP );
		Add( pButt );
		v.y += 32.f;
	}
#endif

	SetbUpdate( TRUE );
}

void XSceneUnitOrg::Create( void )
{
	XEBaseScene::Create();
}

int XSceneUnitOrg::Process( float dt ) 
{ 
	return XEBaseScene::Process( dt );
}

//
void XSceneUnitOrg::Draw( void ) 
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
#if defined(_XUZHU) && defined(_DEBUG)
// 	if( m_psfcBrilliant )
// 	{
// 		m_psfcBrilliant->SetAdjustAxisCenter();
// 		static float s_dRot = 0.f;
// 		m_psfcBrilliant->SetRotateZ( s_dRot );
// 		m_psfcBrilliant->Draw( XEBaseScene::GetvMouse() );
// 		m_psfcBrilliant->SetAdjustAxisCenter();
// 		m_psfcBrilliant->SetRotateZ( 360.f - s_dRot );
// 		m_psfcBrilliant->Draw( XEBaseScene::GetvMouse() );
// 		s_dRot += 1.f;
// 	}
#endif // _XUZHU
}

void XSceneUnitOrg::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneUnitOrg::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneUnitOrg::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );

}

int XSceneUnitOrg::OnBack(XWnd *pWnd, DWORD p1, DWORD p2)
{
	DoExit(XGAME::xSC_WORLD);
	return 1;
}

int XSceneUnitOrg::OnClickTabSkill(XWnd *pWnd, DWORD p1, DWORD p2)
{
	s_tabCurr = xTAB_SKILL;
	XWnd *pOld = Find("wnd.equipment");
	if(pOld)
	{
		pOld->SetbDestroy(TRUE);
		XWnd *pWndTop = Find("img.unit.bg.top");
		m_Layout.CreateLayout("skill", pWndTop);
		SetbUpdate( true );
//		UpdateTabSkill();
		return 1;
	}
	return 0;
}
int XSceneUnitOrg::OnClickTabEquipment(XWnd *pWnd, DWORD p1, DWORD p2)
{
	s_tabCurr = xTAB_EQUIP;
	XWnd *pOld = Find("wnd.skill");
	if (pOld)
	{
		pOld->SetbDestroy(TRUE);
		XWnd *pWndTop = Find("img.unit.bg.top");
		m_Layout.CreateLayout("equipment", pWndTop);
//		UpdateTabEquipment();
		SetbUpdate( true );
		return 1;
	}
	return 0;
}

void XSceneUnitOrg::Update(void)
{
	XGAME::CreateUpdateTopResource( this );
	// 좌측 영웅 인벤
	UpdateHeroInven();
	//
	XBREAK( m_pSelHero == nullptr );
	XHero *pHero = m_pSelHero->GetpHero();

	{
		if( s_tabCurr == xTAB_SKILL ) {
			auto pWndButt = xGET_BUTT_CTRL( this, "butt.tab.skill" );
			if( pWndButt ) {
					pWndButt->SetPush( TRUE );
			}
		} else
		if( s_tabCurr == xTAB_EQUIP ) {
			auto pWndButt = xGET_BUTT_CTRL( this, "butt.tab.equipment" );
			if( pWndButt ) {
					pWndButt->SetPush( TRUE );
			}
		}
	}

	DestroyWndByIdentifier( "wnd.unit.bg.top" );
	auto pRoot = Find("img.unit.bg.top");
	if (pRoot) {
		m_Layout.CreateLayout("unitorg_top", pRoot);
		if( s_tabCurr == xTAB_SKILL ) {
			DestroyWndByIdentifier("wnd.skill");
			m_Layout.CreateLayout( "skill", pRoot );
		} else if( s_tabCurr == xTAB_EQUIP ) {
			DestroyWndByIdentifier( "wnd.equipment" );
			m_Layout.CreateLayout( "equipment", pRoot );
		} else
			XBREAK(1);
		xSET_BUTT_HANDLER(this, "butt.tab.skill", &XSceneUnitOrg::OnClickTabSkill);
		xSET_BUTT_HANDLER(this, "butt.tab.equipment", &XSceneUnitOrg::OnClickTabEquipment);
		xSET_BUTT_HANDLER(this, "butt.back", &XSceneUnitOrg::OnBack);
		xSET_BUTT_HANDLER(this, "butt.troop", &XSceneUnitOrg::OnDoExitLegion);
		xSetButtHander(this, this, "butt.train_center", &XSceneUnitOrg::OnClickTrainingCenter);
		if (pHero) {
			xSET_ENABLE(this, "butt.upgrade.hero", true);
			xSET_ENABLE(this, "butt.upgrade.squad", true);
			xSET_ENABLE(this, "butt.unit.info", true);
			xSET_ENABLE(this, "butt.info.squad", true);
			xSET_BUTT_HANDLER(this, "butt.upgrade.hero", &XSceneUnitOrg::OnClickLevelupHero);
			xSET_BUTT_HANDLER(this, "butt.upgrade.squad", &XSceneUnitOrg::OnClickSquadUpgrade);
			xSET_BUTT_HANDLER(this, "butt.unit.info", &XSceneUnitOrg::OnClickUnitInfo);
			xSetButtHander(this, this, "butt.info.squad", &XSceneUnitOrg::OnClickInfoSquad);
		} else {
			xSET_ENABLE(this, "butt.upgrade.hero", false);
			xSET_ENABLE(this, "butt.upgrade.squad", false);
			xSET_ENABLE(this, "butt.unit.info", false);
			xSET_ENABLE(this, "butt.info.squad", false);
		}
	}
	// 훈련소 버튼 업데이트
	UpdateTrainingCenterButton(pHero);
	if (pHero) {
		// 탭에 녹색점 표시
		if (s_tabCurr == xTAB_SKILL) {
			auto pWnd = Find("butt.tab.equipment");
			// 스킬탭 상태일때 더 좋은아이템을 끼울수 있다면 장비버튼에 녹색점
			if (ACCOUNT->IsHaveBetterThanParts(pHero)) {
				if (pWnd)
					GAME->SetGreenAlert(pWnd, true, XE::VEC2(79, 5));
				else
					GAME->SetGreenAlert(pWnd, false);
			} else
				GAME->SetGreenAlert(pWnd, false);
		} else
		if (s_tabCurr == xTAB_EQUIP) {
// 			auto pWnd = Find("butt.tab.skill");
// 			if (ACCOUNT->IsAbleLevelupSkill(pHero, XGAME::xTR_SKILL_ACTIVE_UP)
// 				|| ACCOUNT->IsAbleLevelupSkill(pHero, XGAME::xTR_SKILL_PASSIVE_UP)
// 				|| ACCOUNT->IsAbleProvideSkill(pHero, XGAME::xTR_SKILL_ACTIVE_UP)
// 				|| ACCOUNT->IsAbleProvideSkill(pHero, XGAME::xTR_SKILL_PASSIVE_UP)) {
// 				if (pWnd)
// 					GAME->SetGreenAlert(pWnd, true, XE::VEC2(79, 2));
// 				else
// 					GAME->SetGreenAlert(pWnd, false);
// 			} else
// 				GAME->SetGreenAlert(pWnd, false);
		}
	}
	// 영웅정보
	UpdateHeroInfo();
	// 군단 전투력
	ACCOUNT->UpdatePower();
	xSET_TEXT( this, "text.legion.power", XFORMAT( "%s:%s", XTEXT( 2115 ), XE::NumberToMoneyString( ACCOUNT->GetPowerExcludeEmpty() ) ) );
	int powerMax = XGC->m_aryMaxPowerPerLevel[ ACCOUNT->GetLevel() ];
#ifdef _CHEAT
	xSET_TEXT( this, "text.avg.power", XFORMAT( "max:%s", XE::NumberToMoneyString( powerMax ) ) );
	if( XAPP->m_bDebugMode )
		xSET_SHOW( this, "text.avg.power", true );
	else
		xSET_SHOW( this, "text.avg.power", false );
#endif // _CHEAT

	XEBaseScene::Update();
} // Update()

// 영웅 인벤
void XSceneUnitOrg::UpdateHeroInven()
{
	auto pWndList = SafeCast<XWndList*>( Find( "list.inven.hero" ) );
	if( pWndList == nullptr )
		return;
	ID snSelected = (m_pSelHero )? m_pSelHero->GetsnHero() : 0;
	m_pSelHero = nullptr;
	pWndList->DestroyAllItem();
	XList4<XHero*> listHero;
	ACCOUNT->_GetInvenHero(listHero);
	listHero.sort(XSceneUnitOrg::CompParty);
	std::map<ID,int> mapIdxHero;
	int i = 0;
	for( auto pHero : listHero ) {
		auto pElem = new XWndInvenHeroElem( pHero );
		// 영웅들이 중복되어있을경우 각자 인덱스를 붙인다.
		int idx = 0;
		auto itor = mapIdxHero.find( pHero->GetidProp() );
		if( itor == mapIdxHero.end() ) {
			mapIdxHero[ pHero->GetidProp() ] = 0;
		} else {
			auto& elem = itor->second;
			idx = ++elem;
		}
		pElem->SetstrIdentifierf( "elem.hero.%s.%d", SZ2C( pHero->GetstrIdentifer().c_str() ), idx );
		pWndList->AddItem( i+1, pElem );
		if( m_pSelHero == nullptr ) {
			if ( m_strIdSoulStone.empty() ) {
				if (snSelected)	{
					if (pHero->GetsnHero() == snSelected)
						m_pSelHero = pElem;
				} else {
					if ((s_EquipParam.IsEmpty() && i == 0)
						|| (!s_EquipParam.IsEmpty() && pHero->GetsnHero() == s_EquipParam.snHero))
						m_pSelHero = pElem;
				}
			}
		}
		if( m_pSelHero && m_pSelHero->GetsnHero() == pHero->GetsnHero() ) {
			m_pSelHero->SetbSelected( true );
		}
		pElem->SetpDelegate( GAME );
		bool bEnter = (ACCOUNT->GetCurrLegion()->GetpHeroBySN(pHero->GetsnHero()) != nullptr);
		bool bGreenAlert = false;
		// 현재군단에 참전중인 영웅만 알림을 켬.
		if( bEnter && ACCOUNT->IsHaveBetterThanParts( pHero ) )
			bGreenAlert = true;
		if( ACCOUNT->IsNoCheckUnlockUnitWithHero(pHero) )
			bGreenAlert = true;
// 		if(  ACCOUNT->IsHaveBetterThanParts( pHero )
// 			|| ACCOUNT->IsNoCheckUnlockUnitWithHero(pHero) )
		if( bGreenAlert )
			GAME->SetGreenAlert( pElem, true, XE::VEC2(48,45) );
		else
			GAME->SetGreenAlert( pElem, false );

		++i;
	} // hero loop
	// 아직 영웅을 보유하지못했으나 영혼석은 보유한 영웅을 리스트에 포함시킨다.
	XList4<XBaseItem*> listSoulStone;
	ACCOUNT->GetListSoulStoneExcludeHaveHero( &listSoulStone );
	// 소환 가능한 상태가 된 영웅을 먼저 표시	
	for( auto itor = listSoulStone.begin(); itor != listSoulStone.end(); ) {
		auto pItem = (*itor);
		if( pItem ) {
			auto pPropItem = pItem->GetpProp();
			auto pPropHero = PROP_HERO->GetpProp( pPropItem->strIdHero );
			if( XASSERT( pPropHero ) ) {
				auto bSummonable = ACCOUNT->IsAbleSummonHeroBySoulStone( pPropHero->strIdentifier );
				XWndInvenHeroElem* pElem = nullptr;
				if( bSummonable == xE_OK) {
					pElem = new XWndInvenHeroElem( pPropHero );
					pElem->SetstrIdentifierf( "elem.hero.%s", SZ2C( pPropHero->strIdentifier.c_str() ) );
					pWndList->AddItem( i + 1, pElem );
					// 현재 선택된 영혼석이라면 선택된 elem으로 설정한다.
					if( m_strIdSoulStone == pPropItem->strIdHero ) {
						m_pSelHero = pElem;
						pElem->SetbSelected( true );
					}
					GAME->SetGreenAlert( pElem, true, XE::VEC2( 48, 45 ) );
					listSoulStone.erase( itor++ );		// 출력하고 리스트에서 뺌.
					i++;
				} else {
					GAME->SetGreenAlert( pElem, false );
					++itor;
				}
			}
		} else {
			++itor;
		}
	}
	// 보유한 개수로 소트
	listSoulStone.sort( [](XBaseItem* pNode1, XBaseItem* pNode2)->bool {
		if( pNode1 && pNode2 ) {
			return pNode1->GetNum() > pNode2->GetNum();
		}
		return false;
	});
	// 영혼석 영웅들을 리스트컨트롤에 추가시킨다.
	for (auto pItem : listSoulStone) {
		auto pPropItem = pItem->GetpProp();
		auto pPropHero = PROP_HERO->GetpProp( pPropItem->strIdHero );
		if ( XASSERT(pPropHero) ) {
			auto pElem = new XWndInvenHeroElem( pPropHero );
			pElem->SetstrIdentifierf("elem.hero.%s", SZ2C(pPropHero->strIdentifier.c_str()));
			pWndList->AddItem(i + 1, pElem);
			// 현재 선택된 영혼석이라면 선택된 elem으로 설정한다.
			if ( m_strIdSoulStone == pPropItem->strIdHero ) {
				m_pSelHero = pElem;
				pElem->SetbSelected( true );
			}
			auto bOk = ACCOUNT->IsAbleSummonHeroBySoulStone( pPropHero->strIdentifier );
			if( bOk == XGAME::xE_OK )
				GAME->SetGreenAlert( pElem, true, XE::VEC2( 48, 45 ) );
			else
				GAME->SetGreenAlert( pElem, false );
			i++;
		}
	}
// 	XList4<ID> listSoulStone;
// 	ACCOUNT->GetsnlistSoulStone( listSoulStone );

	//리스트에 아이템을 전부 추가했는데 아직도 선택된 아이템이 없다면 1번을 강제선택
	if (m_pSelHero == nullptr && pWndList->GetNumItem() > 0)
	{
// 		m_pSelHero = SafeCast<XWndInvenHeroElem*>(pWndList->GetlistItem().GetFirst());
		m_pSelHero = SafeCast<XWndInvenHeroElem*>( pWndList->GetpFirst() );
		m_pSelHero->SetbSelected( true );
	}

	pWndList->SetEvent( XWM_SELECT_ELEM, this, &XSceneUnitOrg::OnClickInventory );
	pWndList->SetScrollDir( XE::xVERT );
}

void XSceneUnitOrg::UpdateHeroInfo(void)
{
	if( m_pSelHero == nullptr )
		return;
	XHero *pHero = m_pSelHero->GetpHero();
	/*if( XBREAK( pHero == nullptr ) )
		return;*/
	if (pHero == nullptr && m_pSelHero->IsSoulStone() == false)
	{
		XBREAK(1);
		return;
	}
	// 영웅 기본정보 업데이트
	UpdateHeroBasic();
	// 부대정보 업데이트
	UpdateSquadInfo();
	//스킬
	if (Find("wnd.skill"))
		UpdateSkillInfo();
	// 장비
	else if (Find("wnd.equipment"))
		UpdateTabEquipment();
	// 영웅 전투력
	if (pHero) {
		int power = XLegion::sGetMilitaryPower(pHero);
		xSET_TEXT(this, "text.hero.power", XFORMAT("%s:%s", XTEXT(2303), XE::NtS(power)));
		auto pWnd = Find("butt.troop");
		if (pWnd) {
			pWnd->SetpDelegate(GAME);
			if( ACCOUNT->IsRemainSquad() )
				GAME->AddAlert( pWnd, 1 );
			else
				GAME->DelAlert( pWnd, 1 );
// 			if (ACCOUNT->IsRemainSquad())
// 				GAME->AddBrilliant(pWnd->getid(), 2.f);
// 			else
// 				GAME->DelBrilliant(pWnd->getid());
		}
	} else {
		xSET_TEXT_FORMAT(this, "text.hero.power", _T("%s:?"), XTEXT(2303) );
	}
}

/**
 @brief 영웅 기본정보 업데이트.
*/
void XSceneUnitOrg::UpdateHeroBasic()
{
	if( m_pSelHero == nullptr )
		return;
	XHero *pHero = m_pSelHero->GetpHero();
	/*if (XBREAK(pHero == nullptr))
		return;*/
	// 별
	XGAME::xtGrade heroGrade = XGAME::xGD_NONE;
	const XGAME::xtGrade heroGradeMax = XGAME::xGD_LEGENDARY;
	if (pHero) {
		heroGrade = pHero->GetGrade();
	} else {
//		heroGrade = m_pSelHero->GetpProp()->GetGradeMax();
		heroGrade = XGAME::xGD_COMMON;	// 1성부터 시작하므로
	}
	for (int j = 1; j <= heroGradeMax; ++j) {
		if( j <= heroGrade ) {
			// 슬롯감추고 별 나타냄
			xSET_SHOWF( this, false, "img.empty.star%d", j );	
			xSET_SHOWF(this, true, "img.star%d", j);
		} else {
			xSET_SHOWF( this, true, "img.empty.star%d", j );
			xSET_SHOWF( this, false, "img.star%d", j );
		}
	}
	// 영웅 얼굴
	xSET_IMG( this, "img.icon.hero", XE::MakePath( DIR_IMG, m_pSelHero->GetpProp()->strFace ) );
	// 경험치 바
	XWnd *pWndBgTop = Find( "wnd.unit.bg.top" );
	if( pWndBgTop )	{
		auto pPBar = SafeCast<XWndProgressBar*>( pWndBgTop->Find( "pbar.exp" ) );
		if( pPBar && pHero )
			pPBar->SetLerp( (float)pHero->GetExp() / pHero->GetMaxExpCurrLevel() );
	}
//	int numMax = 0;
	xSET_SHOW( this, "text.soulstone", true );
//	xSET_SHOW( this, "text.need.maxlevel", false );	// "최대레벨 필요"
	// 현재 등급에서 승급하기 위해 필요한 총 개수를 얻는다.
	const int numMax = XGAME::GetNeedSoulPromotion( m_pSelHero->GetGrade() );
	if (pHero) {
// 		numMax = XGAME::GetNeedSoulPromotion(m_pSelHero->GetGradeMax());
		// 영웅 레벨
		xSET_TEXT(this, "text.hero.level", XE::Format(_T("Lv%d %s"), pHero->GetLevel(), pHero->GetpProp()->GetstrName().c_str()));
		// 클랜
// 		auto tClan = pHero->GetClan();
// 		xSET_TEXT(this, "text.hero.clan", XGAME::GetstrClan(tClan));
		// 영웅 스탯
		const bool bShow = true;
		xSET_TEXT( this, "text.hero.atk", XFORMAT( "%.0f%%", pHero->GetAttackMeleeRatio( bShow ) ) );
		xSET_TEXT( this, "text.hero.range", XFORMAT( "%.0f%%", pHero->GetAttackRangeRatio( bShow )  ) );
		xSET_TEXT( this, "text.hero.def", XFORMAT( "%.0f%%", pHero->GetDefenseRatio( bShow )  ) );
		xSET_TEXT( this, "text.hero.hp", XFORMAT( "%.0f%%", pHero->GetHpMaxRatio( bShow )  ) );
		xSET_TEXT( this, "text.hero.speed.atk", XFORMAT( "%.0f%%", pHero->GetAttackSpeed( bShow )  ) );
		xSET_TEXT( this, "text.hero.speed.move", XFORMAT( "%.0f%%", pHero->GetMoveSpeed( bShow )  ) );
// 		xSET_TEXT(this, "text.hero.atk", XFORMAT("%.0f", pHero->GetAttackMeleeRatio()* multiply));
// 		xSET_TEXT(this, "text.hero.range", XFORMAT("%.0f", pHero->GetAttackRangeRatio() * multiply));
// 		xSET_TEXT(this, "text.hero.def", XFORMAT("%.0f", pHero->GetDefenseRatio() * multiply));
// 		xSET_TEXT(this, "text.hero.hp", XFORMAT("%.0f", pHero->GetHpMaxRatio() * multiply));
// 		xSET_TEXT(this, "text.hero.speed.atk", XFORMAT("%.0f", pHero->GetAttackSpeed() * multiply));
// 		xSET_TEXT(this, "text.hero.speed.move", XFORMAT("%.0f", pHero->GetMoveSpeed() * multiply));
		// 해고버튼
		xSET_BUTT_HANDLER(this, "butt.hero.fire", &XSceneUnitOrg::OnClickReleaseHero);
		UpdateHeroLevelUpgrade(pHero);
		// 승급버튼
		auto pButt = xGET_BUTT_CTRL( this, "butt.promotion" );
		if( pButt ) {
			pButt->SetText( XTEXT( 80206 ) );	// 승급
			pButt->SetEvent( XWM_CLICKED, this, &XSceneUnitOrg::OnClickPromotion, 0 );
			auto bOk = ACCOUNT->IsPromotionHero( pHero );
			if( bOk == XGAME::xE_OK ) {
				pButt->SetbEnable( TRUE );
				GAME->SetGreenAlert( pButt, true, XE::VEC2( 78, 2 ) );
			} else
			if( bOk == XGAME::xE_NO_MORE || bOk == XGAME::xE_CAN_NOT ) {
				pButt->SetbShow( FALSE );	// 더이상 승급할수 없으면 아예 없앰.
				xSET_SHOW( this, "text.soulstone", false );
			} else {
// 				if( bOk == XGAME::xE_NOT_ENOUGH_LEVEL ) {
// 					auto pText = xGET_TEXT_CTRL( this, "text.need.maxlevel" );
// 					if( XASSERT(pText) ) {
// 						pText->SetbShow( TRUE );
// 						// 최대레벨필요
// 						pText->SetText( XE::Format(XTEXT(2160), pHero->GetMaxLevel()) );
// 					}
// 				}
				pButt->SetbEnable( FALSE );
				GAME->SetGreenAlert( pButt, false );
			}
		}
// 		XWndButtonString *pButt = dynamic_cast<XWndButtonString*>(Find("butt.promotion"));
// 		if (pButt)
// 			pButt->SetText(XTEXT(80206));
	} else {
//		numMax = XGAME::GetNeedSoulSummon(m_pSelHero->GetGrade());
		auto pPropHero = m_pSelHero->GetpProp();

		xSET_TEXT(this, "text.hero.level", XE::Format(_T("Lv1 %s"), pPropHero->GetstrName().c_str()));
		xSET_TEXT(this, "text.hero.atk", XFORMAT("%.0f%%", pPropHero->GetStat2( xSTAT_ATK_MELEE, xGD_COMMON, 1, true)));
		xSET_TEXT(this, "text.hero.range", XFORMAT("%.0f%%", pPropHero->GetStat2( xSTAT_ATK_RANGE, xGD_COMMON, 1, true)));
		xSET_TEXT(this, "text.hero.def", XFORMAT("%.0f%%", pPropHero->GetStat2( xSTAT_DEF, xGD_COMMON, 1, true)));
		xSET_TEXT(this, "text.hero.hp", XFORMAT("%.0f%%", pPropHero->GetStat2( xSTAT_HP, xGD_COMMON, 1, true)));
		xSET_TEXT(this, "text.hero.speed.atk", XFORMAT("%.0f%%", pPropHero->GetStat2( xSTAT_SPEED_ATK, xGD_COMMON, 1, true)));
		xSET_TEXT(this, "text.hero.speed.move", XFORMAT("%.0f%%", pPropHero->GetStat2( xSTAT_SPEED_MOV, xGD_COMMON, 1, true)));
		xSET_SHOW( this, "butt.hero.fire", false );
//		xSET_ENABLE(this, "butt.hero.fire", false);
		// 소환버튼
		auto pButt = xGET_BUTT_CTRL( this, "butt.promotion" );
		if( pButt ) {
			pButt->SetText( XTEXT( 80205 ) );	// 소환
			pButt->SetEvent( XWM_CLICKED, this, &XSceneUnitOrg::OnClickPromotion, 1 );
			auto bOk = ACCOUNT->IsAbleSummonHeroBySoulStone( m_strIdSoulStone );
			if( bOk == XGAME::xE_OK ) {
				pButt->SetbEnable( TRUE );
				GAME->SetGreenAlert( pButt, true, XE::VEC2( 78, 2 ) );
			} else {
				pButt->SetbEnable( FALSE );
				GAME->SetGreenAlert( pButt, false );
			}
		}
// 		XWndButtonString *pButt = dynamic_cast<XWndButtonString*>(Find("butt.promotion"));
// 		if (pButt)
// 			pButt->SetText(XTEXT(80205));
	}
	// 현재 모은 조각수
	{
		XWnd *pText = Find( "text.soulstone" );
		if( pText && pText->GetbShow() ) {
			int numCurrPiece 
				= ACCOUNT->GetNumSoulStone( m_pSelHero->GetpProp()->strIdentifier );
			xSET_TEXT( this, "text.soulstone", 
				XFORMAT( "%d/%d", numCurrPiece, numMax ) );
		}
	}
// 	// 3성미만은 승급 안됨.
// 	if (m_pSelHero->GetGrade() < XGAME::xGD_RARE) {
// 		xSET_SHOW(this, "butt.promotion", false);
// 		xSET_SHOW(this, "text.soulstone", false);
// 	} else {
// 		xSET_SHOW(this, "butt.promotion", true);
// 		xSET_SHOW(this, "text.soulstone", true);
// 		// 현재 모은 조각수
// 		int numCurrPiece = ACCOUNT->GetNumSoulStone( m_pSelHero->GetpProp()->strIdentifier );
// 		xSET_TEXT( this, "text.soulstone", XFORMAT( "%d/%d", numCurrPiece, numMax ) );
// 	}
	///< 영웅 렙업버튼(+) 관련 업데이트
}

/**
 @brief 영웅 렙업버튼(+) 관련 업데이트
*/
void XSceneUnitOrg::UpdateHeroLevelUpgrade( XHero *pHero )
{
	if (pHero == nullptr)
		return;
	const XGAME::xtTrain type = XGAME::xTR_LEVEL_UP;
	bool bBrilliantLeveup = false;
	XWnd *pButt = Find( "butt.upgrade.hero" );
	if( XBREAK( pButt == nullptr ) )
		return;
// 	if( ACCOUNT->IsLockAcademy() ) {
// 		pButt->SetbShow( false );
// 		return;
// 	}
	pButt->SetbShow( true );
	pButt->SetbEnable( true );
//	GAME->DelBrilliant( pButt->getid() );
	// 렙업 확인 이펙 삭제
//	DestroyWndByIdentifier( "spr.levelup.level" );
	xSET_SHOW( this, "text.time.train.level", false );	// 렙업훈련 남은시간
	xSET_SHOW( this, "text.limit.level.hero", false );	// 렙제한.
	// 렙업 대기상태인지
// 	if( pHero->GetbLevelupReady( type ) ) {
// 		XBREAK( !pHero->IsFullExp( type ) );
// 		// 레벨업 준비 이펙트를 띄운다.
// 		DoCreateLevelupReady( pHero, type );
// 		// 렙업 확인 spr이펙트
// 		// +버튼 감추기( 렙업 확인전까지는 표시안된다.
// 		pButt->SetbShow( false );
// 	} else 
	{
		pButt->SetpDelegate( GAME );
		// 책제공이 가능한 상태면 찬란효과
// 		if( ACCOUNT->IsAbleProvideLevelup( pHero ) )
// 			GAME->AddBrilliant( pButt->getid(), 2.f );
		// 이 영웅이 렙업훈련중인지 확인
		if( ACCOUNT->IsTrainingHero( pHero->GetsnHero(), type ) ) {
			auto pText = Find( "text.time.train.level" );
			if( pText ) {
				pText->SetbShow( true );
				if( IsAutoUpdate() == false )
					SetAutoUpdate( 0.1f );
				// 남은 연구시간 갱신은 OnAutoUpdate()에서 담당
			}
		} else {
			// 영웅 렙제한 표시
			if( pHero->IsMaxLevel(type) ) {
				pButt->SetbEnable( false );
				xSET_SHOW( this, "text.limit.level.hero", true );
				xSET_TEXT( this, "text.limit.level.hero", XTEXT(2111) );	// 최대레벨입니다.
			} else
			// 영웅레벨이 군주레벨보다 더 크면 버튼 비활성화 시킨다.
			if( pHero->GetLevel() >= ACCOUNT->GetLevel() ) {
				pButt->SetbEnable( false );
				xSET_SHOW( this, "text.limit.level.hero", true );
				xSET_TEXT( this, "text.limit.level.hero", XE::Format( XTEXT( 2110 ), pHero->GetLevel() + 1) );	// 군주레벨 %필요
			}
		}
	}
}
/**
 @brief 부대정보 업데이트
*/
void XSceneUnitOrg::UpdateSquadInfo()
{
	if( m_pSelHero == nullptr )
		return;
	XHero *pHero = m_pSelHero->GetpHero();
	auto pPropHero = m_pSelHero->GetpProp();
	const auto type = XGAME::xTR_SQUAD_UP;
	/*if( XBREAK( pHero == nullptr ) )
		return;*/
	// 부대 모습
	if (pHero) {
		XWnd *pWndBgTop = Find("wnd.unit.bg.top");
		if (pWndBgTop)	{
			SetSprSquad(pWndBgTop, pHero);
			xSET_TEXT(pWndBgTop, "text.squad.level", XFORMAT("Lv%d", pHero->GetlevelSquad()));
		}
		auto pPropUnit = PROP_UNIT->GetpProp(pHero->GetUnit());
		if (XBREAK(pPropUnit == nullptr))
			return;
		// 유닛얼굴 표시
		{
			auto pImg = xSET_IMG( this, "img.icon.legion", XE::MakePath( DIR_IMG, pPropUnit->strFace ) );
			if( pImg ) {
				if( ACCOUNT->IsNoCheckUnlockUnitWithHero(pHero) )
					GAME->SetGreenAlert( pImg, true, XE::VEC2( 45, 1 ), 2.f );
				else
					GAME->SetGreenAlert( pImg, false );
			}
		}
		// 유닛얼굴부분 클릭 이벤트 핸들러 
		{
//			auto pImg = GetImageCtrl( "img.icon.legion.bg" );
			auto pImg = xGET_IMAGE_CTRL( this, "img.icon.legion.bg" );
			if( pImg )
				pImg->SetEvent( XWM_CLICKED, this, &XSceneUnitOrg::OnClickLegionSelect );
		}
		// 현재까지 먹은 메달 표시
		xSET_TEXT(this, "text.medal.curr", 
			XFORMAT("%d/%d", pHero->GetExp( type ), pHero->GetMaxExp( type )) );
		// 부대업글+버튼 업데이트
		UpdateSquadUpgradeButton(pHero);
	} else {
		// 영혼석의 경우 대표 유닛으로 표시
		auto atkType = pPropHero->typeAtk;
		XGAME::xtUnit unit = XGAME::xUNIT_NONE;
		switch( atkType )
		{
		case XGAME::xAT_TANKER:	unit = XGAME::xUNIT_SPEARMAN;	break;
		case XGAME::xAT_RANGE:	unit = XGAME::xUNIT_ARCHER;	break;
		case XGAME::xAT_SPEED:	unit = XGAME::xUNIT_PALADIN;	break;
		default:
			XBREAK(1);
			break;
		}
		if( unit ) {
			auto pPropUnit = PROP_UNIT->GetpProp( unit );
			if( XASSERT(pPropUnit) ) {
				// 유닛얼굴 표시
				_tstring resFace = XE::MakePath( DIR_IMG, pPropUnit->strFace );
				xSET_IMG( this, "img.icon.legion", resFace );
			}
		}
		xSET_SHOW( this, "butt.unit.info", false );
	}
}

/**
 @brief 부대업버튼(+)관련 업데이트.
*/
void XSceneUnitOrg::UpdateSquadUpgradeButton( XHero *pHero )
{
	const XGAME::xtTrain typeTrain = XGAME::xTR_SQUAD_UP;
	// 부대업글(+)버튼
	auto pButt = Find("butt.upgrade.squad");
	if( pButt == nullptr )
		return;
// 	if( ACCOUNT->IsLockHangout() ) {
// 		pButt->SetbShow( false );
// 		return;
// 	}
	pButt->SetbShow( true );
	pButt->SetbEnable( true );
	xSET_SHOW( this, "text.limit.hero", FALSE );
	DestroyWndByIdentifier( "spr.levelup.squad" );
	GAME->SetGreenAlert( pButt, false );
	// 이 영웅이 훈련을 마치고 렙업대기상태인지 확인.
// 	if( pHero->GetbLevelupReady( typeTrain ) ) {
// 		// 렙업확인전까지는 +버튼 감춤
// 		pButt->SetbShow( false );
// 		// 레벨업 준비 이펙트를 띄운다.
// 		DoCreateLevelupReady( pHero, typeTrain );
// 	} else 
	{
		// 훈련중이 아니면 버튼보이게 함.
		if( pHero->IsMaxLevel(typeTrain) )
			pButt->SetbShow( false );
		else {
// 			if( ACCOUNT->IsAbleProvideSquad( pHero ) )
// 				GAME->SetGreenAlert( pButt, true, XE::VEC2(35,2) );
// 			else 
			{
				auto pPropSquadNext = pHero->GetpPropSquadupNext();
				if( pPropSquadNext && 
					pHero->GetLevel() < pPropSquadNext->levelLimitByHero ) {
					// 영웅레벨 x필요
					pButt->SetbEnable( false );
					auto pText = xSET_SHOW( this, "text.limit.hero", TRUE );
					xSET_TEXT( this, "text.limit.hero", 
								XE::Format( XTEXT( 2088 ), 
									pPropSquadNext->levelLimitByHero ) );
				}
			}
			// 이 영웅이 렙업훈련중인지 확인
			if( ACCOUNT->IsTrainingHero( pHero->GetsnHero(), typeTrain ) ) {
				auto pText = xGET_TEXT_CTRL( this, "text.time.train.squad" );
				if( pText ) {
					pText->SetbShow( true );
					if( IsAutoUpdate() == false )
						SetAutoUpdate( 0.1f );
					// 남은 연구시간 갱신은 OnAutoUpdate()에서 담당
				}
			}
		}
	}
// 	pButt->SetpDelegate( GAME );
// 	// 현재 영웅의 유닛이 업글이 가능한 상태인지 확인한다.
// 	if( ACCOUNT->IsAbleLevelUpSquad( pHero ) )
// 		GAME->AddBrilliant( pButt->getid(), 2.f );
// 	else
// 		GAME->DelBrilliant( pButt->getid() );
}

/**
 @brief 훈련소 버튼 업데이트
*/
void XSceneUnitOrg::UpdateTrainingCenterButton( XHero *pHero )
{
	auto pButt = Find( "butt.train_center" );
	if( XBREAK(pButt == nullptr) )
		return;
// 	if( ACCOUNT->GetbitUnlockMenu().IsNotBit( XGAME::xBM_TRAING_CENTER ) ) {
// 	if( ACCOUNT->IsLockAcademy() ) {
// 		pButt->SetbShow( false );
// 		return;
// 	}
	pButt->SetbShow( true );
	auto pGear = Find( "spr.gear" );		// 톱니 애니
	auto pSleep = Find( "spr.sleep" );		// zZZ애니
	// 비어있는 훈련슬롯이 있으면
	if( ACCOUNT->GetNumRemainFreeSlot() == 0 ) {
		// 톱니를 보이게 한다.
		if( pGear == nullptr ) {
			pGear = new XWndSprObj( _T( "wait.spr" ), 2, 76.f, 16.f );
			pButt->Add( pGear );
		}
		pGear->SetbShow( true );
		if( pSleep )
			pSleep->SetbShow( true );
	} else {
		if( pSleep == nullptr ) {
			pSleep = new XWndSprObj( _T( "ui_sleep.spr" ), 1, 76.f, 33.f );
			pButt->Add( pSleep );
		}
		// 훈련중이지 않으면 톱니 감춤
		if( pGear )
			pGear->SetbShow( FALSE );
	}
}

void XSceneUnitOrg::OnAutoUpdate() 
{
	XHero *pHero = m_pSelHero->GetpHero();
	//
	auto pText = xGET_TEXT_CTRL( this, "text.time.train.level" );
	if( pText && pHero )
	{
		auto pSlot = ACCOUNT->GetTrainingLevelupHero( pHero->GetsnHero() );
		if( pSlot )
		{
			pText->SetbShow( TRUE );
			_tstring strRemain;
//			CONSOLE("%d,%d", pPropSlot->timerStart.GetsecStart(), pPropSlot->secTotal );
			XSceneTech::sGetRemainResearchTime( &strRemain,
												pSlot->timerStart.GetsecStart(),
												pSlot->secTotal );
			pText->SetText( strRemain );
		}
	}
	pText = xGET_TEXT_CTRL( this, "text.time.train.squad" );
	if( pText && pHero )
	{
		auto pSlot = ACCOUNT->GetTrainingSquadupHero( pHero->GetsnHero() );
		if( pSlot )
		{
			pText->SetbShow( TRUE );
			_tstring strRemain;
			XSceneTech::sGetRemainResearchTime( &strRemain,
												pSlot->timerStart.GetsecStart(),
												pSlot->secTotal );
			pText->SetText( strRemain );
		}
	}
	for( int i = 0; i < 2; ++i )
	{
		auto typeSkill = (XGAME::xtIdxSkilltype) i;
		auto type = (typeSkill==XGAME::xPASSIVE)? XGAME::xTR_SKILL_PASSIVE_UP : XGAME::xTR_SKILL_ACTIVE_UP;
		char cSuffix[64];
		if( typeSkill == XGAME::xPASSIVE )
			strcpy_s( cSuffix, "passive" );
		else
			strcpy_s( cSuffix, "active" );
		char cKey[256];
		sprintf_s( cKey, "text.time.train.%s", cSuffix );
		pText = xGET_TEXT_CTRL( this, cKey );
		if( pText && pHero )
		{
			auto pPropSlot = ACCOUNT->GetTrainingSkillupHero( pHero->GetsnHero(), type );
			if( pPropSlot )
			{
				pText->SetbShow( TRUE );
				_tstring strRemain;
				XSceneTech::sGetRemainResearchTime( &strRemain,
													pPropSlot->timerStart.GetsecStart(),
													pPropSlot->secTotal );
				pText->SetText( strRemain );
			}
		}
	}
}


int XSceneUnitOrg::OnClickInventory(XWnd *pWnd, DWORD p1, DWORD p2)
{
	ID idHero = p2;
	XWndInvenHeroElem *pElem = SafeCast<XWndInvenHeroElem*, XWnd*>(pWnd->Find(idHero));
	if (pElem != m_pSelHero)
	{
		if (m_pSelHero)
			m_pSelHero->SetbSelected( false );
		m_pSelHero = pElem;
		if (m_pSelHero->GetpHero() == nullptr)
			m_strIdSoulStone = m_pSelHero->GetpProp()->strIdentifier;
		else
			m_strIdSoulStone = _T("");
		m_pSelHero->SetbSelected( true );
		//UpdateHeroInfo();
		SetbUpdate( TRUE );
		return 1;
	}
	return 0;
}
/**
 @brief 영웅정보->영웅초상화 누름(유닛교체 목적)
*/
int XSceneUnitOrg::OnClickLegionSelect(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XWnd *pPopup = new XWndView( m_Layout.GetpLayout(), "base_popup", nullptr );
	pPopup->SetstrIdentifier("wnd.legion.select");
	Add( pPopup );
	if (pPopup) {
		m_Layout.CreateLayout("legion_list", pPopup);
		pPopup->SetbModal(TRUE);
		xSET_TEXT( pPopup, "text.popup.title", XTEXT( 80007 ) );
		xSET_BUTT_HANDLER( pPopup, "butt.legion.exit", &XSceneUnitOrg::OnSelectExit );
		auto pListLegion = xGET_LIST_CTRL( pPopup, "list.legion");
		pListLegion->SetTypeHoriz();
		XHero *pHero = m_pSelHero->GetpHero();
		auto unitSel = XGAME::xUNIT_NONE;
		int typeSize = 0;
		for (int i = 1; i < XGAME::xUNIT_MAX; ++i) {
			auto unit = (XGAME::xtUnit)i;
			auto pPropUnit = PROP_UNIT->GetpProp( unit );
			if (pHero->GetpProp()->typeAtk == pPropUnit->typeAtk) {
// 				if( XAccount::sIsSelectable( pPropUnit->size, ACCOUNT->GetLevel() ) )
				auto pElem = new XWndInvenLegionElem( pPropUnit );
				if (unit == pHero->GetUnit()) {
					pElem->SetbSelected( true );
					unitSel = unit;
					xSET_BUTT_HANDLER_PARAM(this, "butt.legion.decide", this, &XSceneUnitOrg::OnSelectLegion, pElem->GetpProp()->idProp);
				}
				if( ACCOUNT->IsLockUnit( unit ) ) {
					pElem->SetLock( true );
					pElem->SetbActive( FALSE );
				}
				if( ACCOUNT->IsNoCheckUnlockUnitEach( unit ) ) {
					GAME->SetGreenAlert( pElem, true, XE::VEC2(115,5) );
					ACCOUNT->SetCheckUnlockUnit( unit );
					GAMESVR_SOCKET->SendCheckUnlockUnit( unit );
				} else {
					GAME->SetGreenAlert( pElem, false );
				}
				++typeSize;
				pListLegion->AddItem( pPropUnit->idProp, pElem);
			}
		}
		pListLegion->SetFocusView(0.f, 0.f);
		pListLegion->SetEvent(XWM_SELECT_ELEM, this, &XSceneUnitOrg::OnClickListLegion, (DWORD)unitSel );
		return 1;
	}
	return 0;
}

int XSceneUnitOrg::OnSelectExit(XWnd *pWnd, DWORD p1, DWORD p2)
{
// 	XWnd *pParent = Find("wnd.base.popup");
// 	if (pParent)
// 		pParent->SetbDestroy(TRUE);
	pWnd->GetpParent()->SetbDestroy( TRUE );
//	Update();
	SetbUpdate( TRUE );
//	UpdateHeroInfo();
	return 1;
}

int XSceneUnitOrg::OnClickListLegion(XWnd *pWnd, DWORD p1, DWORD p2)
{
	ID idUnitOld = p1, idUnitNew = p2;
	XWndInvenLegionElem *pElem = SafeCast<XWndInvenLegionElem*, XWnd*>(pWnd->Find(idUnitOld));
	if (pElem)
		pElem->SetbSelected( false );
	pElem = SafeCast<XWndInvenLegionElem*, XWnd*>(pWnd->Find(idUnitNew));
	if (pElem)
	{
		pElem->SetbSelected( true );
		xSET_BUTT_HANDLER_PARAM(this, "butt.legion.decide", this, &XSceneUnitOrg::OnSelectLegion, pElem->GetpProp()->idProp);
	}

	XWndList *pList = SafeCast<XWndList*, XWnd*>(pWnd);
	pWnd->SetEvent(XWM_SELECT_ELEM, this, &XSceneUnitOrg::OnClickListLegion, idUnitNew);

	//
//	pWnd->GetpParent()->SetbDestroy( TRUE );
//// 	if( pWndTop == nullptr )
//// 	{
//		XWnd *pWndTop = new XWndView( m_Layout.GetpLayout(), "base_popup", nullptr );
//		pWndTop->SetstrIdentifier( "wnd.legion.detail" );
//		m_Layout.CreateLayout( "legion_detail", pWndTop );
//		Add( pWndTop );
//// 	}
//
//// 	XWndList *pWndList = SafeCast<XWndList*, XWnd*>(pWndTop->Find("list.legion"));
//// 	if (pWndList)
//	{
////		XWndInvenLegionElem *pElem = SafeCast<XWndInvenLegionElem*, XWnd*>(pWndList->Find(idLegion));
////		if (pElem)
//		XPropUnit::xPROP *pPropUnit = PROP_UNIT->GetpProp( idUnit );
//		if( pPropUnit )
//		{
//			xSET_IMG_RET(pImg, pWndTop, "img.legion.detail", XE::MakePath(DIR_IMG, pPropUnit->strFace.c_str()));
//			if( pImg )
//				pImg->SetScaleLocal(2.9f, 2.8f);
//
//			xSET_TEXT(pWndTop, "text.legion.atk.melee", XE::Format(_T("%.0f"), pPropUnit->atkMelee));
//			xSET_TEXT(pWndTop, "text.legion.atk.range", XE::Format(_T("%.0f"), pPropUnit->atkRange));
//			xSET_TEXT(pWndTop, "text.legion.def", XE::Format(_T("%.0f"), pPropUnit->def));
//			xSET_TEXT(pWndTop, "text.legion.hp", XE::Format(_T("%.0f"), pPropUnit->hpMax));
//			xSET_TEXT(pWndTop, "text.legion.atk.speed", XE::Format(_T("%.2f"), pPropUnit->atkSpeed));
//			xSET_TEXT(pWndTop, "text.legion.move.speed", XE::Format(_T("%.2f"), pPropUnit->movSpeedPerSec));
//			xSET_TEXT(pWndTop, "text.popup.title", pPropUnit->strName);
//
//			xSET_BUTT_HANDLER_PARAM( pWndTop, 
//									"butt.legion.detail.decide", 
//									this, 
//									&XSceneUnitOrg::OnSelectLegion, 
//									pPropUnit->idProp );
//// 			XWndButton* pButt = dynamic_cast<XWndButton*>(pWndTop->Find("butt.legion.detail.decide"));
//// 			if (pButt)
//// 				pButt->SetEvent(XWM_CLICKED, this, &XSceneUnitOrg::OnSelectLegion, pElem->GetpProp()->idProp);
//		}
//	}
//	xSET_BUTT_HANDLER_RET(pButt, this, "butt.legion.detail.exit", 
//							&XSceneUnitOrg::OnSelectDetailExit);

// 	pWndTop = Find("wnd.legion.list");
// 	if (pWndTop)
// 		pWndTop->SetbDestroy(TRUE);

	return 1;
}

int XSceneUnitOrg::OnSelectDetailExit(XWnd *pWnd, DWORD p1, DWORD p2)
{
// 	XWnd *pDetail = Find("wnd.legion.detail");
// 	if (pDetail)
// 		pDetail->SetbDestroy(TRUE);
	if( pWnd )
		pWnd->GetpParent()->SetbDestroy( TRUE );
	return OnClickLegionSelect(pWnd, p1, p2);
}

int XSceneUnitOrg::OnSelectLegion(XWnd *pWnd, DWORD p1, DWORD p2)
{
// 	XPropUnit::xPROP *pUnit = PROP_UNIT->GetpProp( p1 );
// 	if (pUnit)
	{
		XGAME::xtUnit unit = (XGAME::xtUnit) p1;
		if (pWnd->GetpParent())
			pWnd->GetpParent()->SetID(XE::GenerateID());
		{
			GAMESVR_SOCKET->SendReqChangeHeroLegion(GAME,
								m_pSelHero->GetpHero()->GetsnHero(),
								unit, pWnd->GetpParent()->GetID());
		}
		return 1;
	}
	return 0;
}

void XSceneUnitOrg::OnRecvChangeHeroLegion(ID idWnd)
{
	XWnd *pWnd = Find(idWnd);
	if (pWnd)
		pWnd->SetbDestroy(TRUE);
//	OnSelectDetailExit(pDlg, 0, 0);
	SetbUpdate( TRUE );
}

int XSceneUnitOrg::OnClickLevelupHero(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XBREAK( m_pSelHero == nullptr );
	XBREAK( m_pSelHero->GetpHero() == nullptr );
	if( ACCOUNT->IsLockAcademy() ) {
		XWND_ALERT_T(XTEXT(2241), 5, XTEXT(2133));		// 5레벨에 열립니다. 하드코딩
		return 1;
	}
	XHero *pHero = m_pSelHero->GetpHero();
	auto pPopup = new XWndLevelupHero( pHero, XGAME::xTR_LEVEL_UP );
	pPopup->SetbModal( TRUE );
	Add( pPopup );
// 	auto pPopup = new XWndHeroLevelUp( pHero );
// 	pPopup->SetbModal( TRUE );
// 	Add( pPopup );

	return 0;
}

/****************************************************************
* @brief 
*****************************************************************/
// int XSceneUnitOrg::OnClickNewLevelupHero( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickNewLevelupHero");
// 	//
// 	XBREAK( m_pSelHero == nullptr );
// 	XBREAK( m_pSelHero->GetpHero() == nullptr );
// 	XHero *pHero = m_pSelHero->GetpHero();
// 	auto pPopup = new XWndNewHeroLevelup( pHero );
// 	pPopup->SetbModal( TRUE );
// 	Add( pPopup );
// 	
// 	return 1;
// }
// 

/**
 @brief 스킬 레벨업
*/
int XSceneUnitOrg::OnClickProvideSkill(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if( XBREAK( m_pSelHero == nullptr ) )
		return 1;
	if( ACCOUNT->IsLockAcademy() ) {
		XWND_ALERT_T( XTEXT( 2241 ), 5, XTEXT(2133) );		// 5레벨에 열립니다.
		return 1;
	}
	XHero *pHero = m_pSelHero->GetpHero();
	if( XBREAK( pHero == nullptr ) )
		return 1;
	auto type = (XGAME::xtTrain)p1;
	auto result = ACCOUNT->GetAbleLevelupSkill(pHero, type);
	XBREAK( result == XGAME::xES_NONE );
	switch (result)
	{
	case XGAME::xES_NO_HAVE_SKILL: return 1;
	case XGAME::xES_FULL_LEVEL: 
	{
		auto pAlert = XWND_ALERT("%s", XTEXT(2006));
		break;
	}
	case XGAME::xES_NOT_ENOUGH_ITEM:
	case XGAME::xES_OK:
	{
// 		auto pPopup = new XWndSkillLevelup( pHero, type );
		auto pPopup = new XWndLevelupHero( pHero, type );
		Add( pPopup );
		pPopup->SetbModal( TRUE );

	} break;
	default:
		XBREAK(1);
	}

	return 1;
}

void XSceneUnitOrg::OnRecvAddHeroExp( ID idWnd, XHero *pHero, BOOL bLevelup )
{
	if( XBREAK( pHero == nullptr ) )
		return;
	
// 	XWnd *pWnd = Find( idWnd );
// 	if( pWnd )
// 		pWnd->SetbDestroy( TRUE );
	if( 0 )		// 잠시 막아둠.
	{
// 		XWnd *pPopup = new XWndView( m_Layout.GetpLayout(), "base_popup", nullptr );
// 		if (pPopup)
// 			pPopup->SetbModal(TRUE);
// 		else
// 			XBREAK(pPopup == nullptr);
// 		pPopup->SetstrIdentifier( "wnd.base.popup" );
// 		Add( pPopup );
// 		m_Layout.CreateLayout("levelup_complete", pPopup);
// 	// 	pPopup = Find("wnd.base.popup");
// 	// 	if (pPopup)
// 	// 		m_Layout.CreateLayout("levelup_complate", pPopup);
// 		xSET_TEXT(pPopup, "text.popup.title", XTEXT(80011));
// 
// 		{
// 			xSET_IMG( pPopup, "img.icon.levelup.hero", XE::MakePath(DIR_IMG, pHero->GetpProp()->strFace.c_str()) );
// 
// 			XWndImage* pImg = SafeCast<XWndImage*, XWnd*>(Find("img.state.level"));
// 			if (pImg) {
// 				if (bLevelup)			//레벨업 했을때 처리
// 					pImg->SetSurface(XE::MakePath(DIR_UI, _T("legion_stat_up.png")));
// // 				else
// // 					pImg->SetSurface(XE::MakePath(DIR_UI, _T("legion_stat_keep.png")));
// 			}
// 
// 			xSET_TEXT(pPopup, "text.levelup.name", XE::Format(XTEXT(80047), pHero->GetLevel(), pHero->GetpProp()->GetstrName().c_str()));
// 			xSET_TEXT(pPopup, "text.levelup.melee", XE::Format(_T("%.0f"),pHero->GetAttackMeleeRatio() * multiply));
// 			xSET_TEXT(pPopup, "text.levelup.range", XE::Format(_T("%.0f"), pHero->GetAttackRangeRatio() * multiply));
// 			xSET_TEXT(pPopup, "text.levelup.def", XE::Format(_T("%.0f"), pHero->GetDefenseRatio() * multiply));
// 			xSET_TEXT(pPopup, "text.levelup.hp", XE::Format(_T("%.0f"), pHero->GetHpMaxRatio() * multiply));
// 			xSET_TEXT(pPopup, "text.levelup.atk.speed", XE::Format(_T("%.0f"), pHero->GetAttackSpeed() * multiply));
// 			xSET_TEXT(pPopup, "text.levelup.move.speed", XE::Format(_T("%.0f"), pHero->GetMoveSpeed() * multiply));
// 			XWndStatArrow *pArrow = static_cast<XWndStatArrow*>(Find("img.arrow1"));
// 			if (pArrow)
// 				pArrow->SetDirection(m_pSelHero->GetpHero()->GetAttackMeleeRatio(), m_pKeepHero->GetAttackMeleeRatio());
// 			pArrow = static_cast<XWndStatArrow*>(Find("img.arrow2"));
// 			if (pArrow)
// 				pArrow->SetDirection(m_pSelHero->GetpHero()->GetAttackRangeRatio(), m_pKeepHero->GetAttackRangeRatio());
// 			pArrow = static_cast<XWndStatArrow*>(Find("img.arrow3"));
// 			if (pArrow)
// 				pArrow->SetDirection(m_pSelHero->GetpHero()->GetDefenseRatio(), m_pKeepHero->GetDefenseRatio());
// 			pArrow = static_cast<XWndStatArrow*>(Find("img.arrow4"));
// 			if (pArrow)
// 				pArrow->SetDirection((float)m_pSelHero->GetpHero()->GetHpMaxRatio(), (float)m_pKeepHero->GetHpMaxRatio());
// 			pArrow = static_cast<XWndStatArrow*>(Find("img.arrow5"));
// 			if (pArrow)
// 				pArrow->SetDirection(0.f, 0.f);
// 			pArrow = static_cast<XWndStatArrow*>(Find("img.arrow6"));
// 			if (pArrow)
// 				pArrow->SetDirection(0.f, 0.f);
// 
// 			XWndProgressBar* pBar = SafeCast<XWndProgressBar*,XWnd*>(pPopup->Find("pbar.levelup.exp"));
// 			if (pBar)
// 				pBar->SetLerp((float)pHero->GetExp() / pHero->GetXFLevelObj().GetMaxExpCurrLevel());
//		}

		xSET_BUTT_HANDLER(this, "butt.levelup.ok", &XSceneUnitOrg::OnFinishLevelup);
	}

}

int XSceneUnitOrg::OnFinishLevelup(XWnd *pWnd, DWORD p1, DWORD p2)
{
//	OnSelectExit(pWnd, p1, p2);
	pWnd->GetpParent()->SetbDestroy( TRUE );
//	UpdateHeroInfo();
	SetbUpdate( true );
	return 1;
}

// int XSceneUnitOrg::OnSelectLevelupHero(XWnd *pWnd, DWORD p1, DWORD p2)
// {
// 	pWnd->SetbEnable( FALSE );
// 	XBREAK( m_pSelHero == nullptr );
// 	XBREAK( m_pSelHero->GetpHero() == nullptr );
// 	GAMESVR_SOCKET->SendReqAddHeroExp(this, m_pSelHero->GetpHero()->GetsnHero() );
// 
// 	return 1;
// }

// inline XWndTextString* xSET_TEXT( XWnd *pRoot, const _tstring& str, const char *cKeyFormat, ... ) {
// 	return xSET_TEXT( pRoot, str.c_str(), cKeyFormat,__VA_ARGS__ );
// }

void XSceneUnitOrg::UpdateSkillInfo(void)
{
	if( XBREAK( m_pSelHero == nullptr ) )
		return;
	XHero *pHero = m_pSelHero->GetpHero();
	auto pPropHero = m_pSelHero->GetpProp();
	if (pHero) {
		xSET_ENABLE(this, "butt.upgrade.active", true);
		xSET_ENABLE(this, "butt.upgrade.passive", true);
		xSET_BUTT_HANDLER_PARAM(this, "butt.upgrade.active", this, &XSceneUnitOrg::OnClickProvideSkill, XGAME::xTR_SKILL_ACTIVE_UP);
		xSET_BUTT_HANDLER_PARAM(this, "butt.upgrade.passive", this, &XSceneUnitOrg::OnClickProvideSkill, XGAME::xTR_SKILL_PASSIVE_UP);
	} else {
// 		xSET_SHOW(this, "butt.upgrade.active", false);
// 		xSET_SHOW(this, "butt.upgrade.passive", false);
//		return;
	}
	for (int _i = 0; _i < 2; ++_i) {
		auto typeSkill = (XGAME::xtIdxSkilltype)_i;
		auto typeTrain = (typeSkill == XGAME::xACTIVE) ?
												XGAME::xTR_SKILL_ACTIVE_UP
												: XGAME::xTR_SKILL_PASSIVE_UP;
		char cSuffix[64];
		if (typeSkill == XGAME::xPASSIVE)
			strcpy_s(cSuffix, "passive");
		else
			strcpy_s(cSuffix, "active");
		auto pImgIcon = SafeCast<XWndImage*>( Findf( "img.icon.%s", cSuffix ) );
		const auto& sidSkill = pPropHero->GetsidSkill( typeSkill );
		const auto pSkillDat = SKILL_MNG->FindByIdentifier( sidSkill );
		// 스킬을 일단 가지고 있으면 들어감.
		if( pSkillDat ) {
// 			xSET_SHOWF(this, FALSE, "butt.skillup.complte.%s", cSuffix);	 // 즉시완료 버튼 기본적으로 감춤.
			if (pHero)
				xSET_ENABLEF(this, true, "butt.upgrade.%s", cSuffix);
			xSET_SHOWF(this, false, "text.limit.level.%s", cSuffix);
			if (pImgIcon) {
				// 스킬아이콘 지정
				pImgIcon->SetSurface(XE::MakePath(DIR_IMG, pSkillDat->GetstrIcon()));
				xSET_SHOWF(this, TRUE, "img.icon.%s.bg", cSuffix);
				// 스킬설명 업데이트
				_tstring strDesc;
				int lvSkill = 1;
				if( pHero ) {
					lvSkill = pHero->GetLevel( typeTrain );
				}
				pSkillDat->GetSkillDesc( &strDesc, lvSkill );
				xSET_TEXTF(this, strDesc.c_str(), "text.%s.desc", cSuffix);
				// 스킬이름 업데이트
				_tstring str = pSkillDat->GetSkillName();
				str += XFORMAT(" Lv%d", lvSkill );
				xSET_TEXTF(this, str, "text.%s.name", cSuffix);
// 				// 활성화가 안된경우 검은막을 씌움.
// 				const bool bActivation = (pHero==nullptr)? false : pHero->IsActivationSkill( typeTrain );
// 				xSET_SHOWF( this, bActivation, "butt.upgrade.%s", cSuffix );	// +버튼
// 				if( !bActivation ) {
// 					// 비활성이면 반투명막을 나타나게 함.
// 					bool bFlag = (pHero != nullptr);
// 					if( typeTrain == xTR_SKILL_PASSIVE_UP ) {
// 						xSET_SHOWF( this, bFlag, "img.disable.%s", cSuffix );	// 검은막 on
// 						xSET_SHOWF( this, bFlag, "text.disable.%s", cSuffix );	// 3등급에 활성화 됨.
// 					}
// 				} else {
// 					if( typeTrain == xTR_SKILL_PASSIVE_UP ) {
// 						xSET_SHOWF( this, false, "img.disable.%s", cSuffix );	// 검은막 off
// 						xSET_SHOWF( this, false, "text.disable.%s", cSuffix );	// 3등급에 활성화 됨.
// 					}
// 				}
				// +버튼에 관련한 업데이트.
				UpdateSkillUpgradeButton(pHero, typeSkill, typeTrain, cSuffix);
			}
		} else {
			// 스킬이 아예 없는경우.
			if( pImgIcon )
				pImgIcon->SetSurfacePtr( nullptr );
			xSET_SHOWF( this, false, "img.icon.%s.bg", cSuffix );	// 스킬아이콘 뒷판
			xSET_TEXTF( this, _T( "" ), "text.%s.desc", cSuffix );
			xSET_TEXTF( this, _T( "" ), "text.%s.name", cSuffix );
			xSET_SHOWF( this, true, "text.%s.none", cSuffix );		// 스킬없음
			xSET_SHOWF( this, false, "butt.upgrade.%s", cSuffix );	// +버튼
		}
	} // for( int i = 0; i < 2; ++i )
}

/**
 @brief 스킬탭의 스킬보옥 관련 표시
*/
void XSceneUnitOrg::UpdateSkillScroll( XHero *pHero, 
										XGAME::xtIdxSkilltype typeSkill,
										XGAME::xtTrain typeTrain,
										const char *cSuffix )
{
// 		auto szStr = XFORMAT("%d/%d", pHero->GetExp( typeTrain ), pHero->GetMaxExp( typeTrain ));
// 		xSET_TEXTF( this, szStr, "text.scroll.curr.%s", cSuffix ); 
}

/**
 @brief 스킬 업그레이드(+)버튼에 관한 업데이트.
 @param pHero 값이있으면 보유한 영웅, null이면 보유하지 않은 영웅
*/
void XSceneUnitOrg::UpdateSkillUpgradeButton( XHero *pHero, 
										XGAME::xtIdxSkilltype typeSkill,
										XGAME::xtTrain typeTrain,
										const char *cSuffix )
{
	auto pButt = Findf( "butt.upgrade.%s", cSuffix );
	if( XBREAK( pButt == nullptr ) )
		return;
	bool bButtShow = true;
	bool bButtEnable = true;
	bool bShowTextLimit = false;
	bool bShowTextTrainTime = false;
	// 보유영웅이 아니면 버튼 감춤
	if( pHero == nullptr )
		bButtShow = false;

	if( typeTrain == xTR_SKILL_PASSIVE_UP ) {
		// 패시브가 활성화가 안된경우 검은막을 씌움.
		const bool bActivation = ( pHero == nullptr ) ?
																false
																: pHero->IsActivationSkill( typeTrain );
		bButtEnable = bActivation;
//		pButt->SetbEnable( bActivation );	// 버튼은 비활성시킴
		//xSET_ENABLEF( this, bActivation, "butt.upgrade.%s", cSuffix );	// +버튼
		xSET_SHOWF( this, (!bActivation && pHero), "img.disable.%s", cSuffix );	// 검은막 on
	}
//	xSET_SHOWF( this, false, "text.limit.level.%s", cSuffix );
	if( pHero ) {
		// 보유영웅일때만.
		// 최대레벨이면 +버튼 보일필요 없다.
		if( pHero->IsMaxLevel( typeTrain ) ) {
//			pButt->SetbShow( false );
			bButtShow = false;
		} else {
			{
				// 영웅 렙제한 표시
				auto pPropNext = pHero->GetpPropSkillupNext( typeTrain );
				if( pHero->GetLevel() < pPropNext->levelLimitByHero ) {
					bButtEnable = false;
					bShowTextLimit = true;
//					pButt->SetbEnable( false );
//					xSET_SHOWF( this, true, "text.limit.level.%s", cSuffix );
					xSET_TEXTF( this, XE::Format( XTEXT( 2088 ), pPropNext->levelLimitByHero )
						, "text.limit.level.%s", cSuffix );
				}
//				GAME->SetGreenAlert( pButt, false );
			}
			// 이 영웅이 훈련중인지 확인
			if( ACCOUNT->IsTrainingHero( pHero->GetsnHero(), typeTrain ) ) {
				bShowTextTrainTime = true;
// 				auto pText = xGET_TEXT_CTRLF( this, "text.time.train.%s", cSuffix );
// 				if( pText ) {
// 					pText->SetbShow( TRUE );
					if( IsAutoUpdate() == false )
						SetAutoUpdate( 0.1f );
					// 남은 연구시간 갱신은 OnAutoUpdate()에서 담당
// 				}
			}
		}
		// 현재까지 먹은 보옥 표시(+버튼이 없으면 표시할필요 없음)
		UpdateSkillScroll( pHero, typeSkill, typeTrain, cSuffix );
	}
	xSET_SHOWF( this, bShowTextLimit, "text.limit.level.%s", cSuffix );
	xSET_SHOWF( this, bShowTextTrainTime, "text.time.train.%s", cSuffix );
	pButt->SetbShow( bButtShow );
	pButt->SetbEnable( bButtEnable );
}

void XSceneUnitOrg::UpdateTabEquipment(void)
{
	if( m_pSelHero == nullptr )
		return;
	auto pHero = m_pSelHero->GetpHero();
	if( pHero == nullptr )
		return;

	xSET_BUTT_HANDLER_PARAM(this, "butt.equip.helm", this, &XSceneUnitOrg::OnClickEquipSlot, (DWORD)XGAME::xPARTS_HEAD);
	xSET_BUTT_HANDLER_PARAM(this, "butt.equip.chest", this, &XSceneUnitOrg::OnClickEquipSlot, (DWORD)XGAME::xPARTS_CHEST);
	xSET_BUTT_HANDLER_PARAM(this, "butt.equip.hand", this, &XSceneUnitOrg::OnClickEquipSlot, (DWORD)XGAME::xPARTS_HAND);
	xSET_BUTT_HANDLER_PARAM(this, "butt.equip.foot", this, &XSceneUnitOrg::OnClickEquipSlot, (DWORD)XGAME::xPARTS_FOOT);
	xSET_BUTT_HANDLER_PARAM(this, "butt.equip.accessory", this, &XSceneUnitOrg::OnClickEquipSlot, (DWORD)XGAME::xPARTS_ACCESSORY);

	if (m_pSelHero)
	{
		for( int i = 1; i < XGAME::xPARTS_MAX; ++i )
		{
			char cKey[256];
			auto parts = (XGAME::xtParts)i;
			sprintf_s( cKey, "img.equip.%s", XGAME::GetPartsIdentifier( parts ) );
			auto pImgSlot = Find( cKey );
			if( pImgSlot )
			{
				auto pWndItem = pImgSlot->Find( "wnd.item.elem" );
				XBaseItem *pItem = pHero->GetEquipItem( parts );
				if( pItem )
				{
//					pImgSlot->SetbShow( FALSE );
					if( pWndItem == nullptr )
					{
						pWndItem = new XWndStoragyItemElem( pItem );
						pWndItem->SetstrIdentifier( "wnd.item.elem" );
						pImgSlot->Add( pWndItem );
//						pWndItem->SetPosLocal( -4, -4 );
					}
					pWndItem->SetbShow( TRUE );
					// 플레이어가 pItem보다 더좋은 아이템을 가지고 있나.
					if( ACCOUNT->IsHaveBetterEquipItem( pItem ) )
						GAME->SetGreenAlert( pImgSlot, true, XE::VEC2( 43, 2 ) );
					else
						GAME->SetGreenAlert( pImgSlot, false );
				} else
				{
//					pImgSlot->SetbShow( TRUE );
					if( pWndItem )
						pWndItem->SetbShow( FALSE );
					const bool bExcludeEquiped = true;
					if( ACCOUNT->GetItemByEquip( parts, bExcludeEquiped ) )
						GAME->SetGreenAlert( pImgSlot, true, XE::VEC2( 43, 2 ) );
					else
						GAME->SetGreenAlert( pImgSlot, false );
				}
			}
		}

	}
}

int XSceneUnitOrg::OnDoExitLegion(XWnd* pWnd, DWORD p1 ,DWORD p2)
{
	DoExit(XGAME::xSC_LEGION);
	return 1;
}

/****************************************************************
* @brief 부대 업그레이드 버튼
*****************************************************************/
int XSceneUnitOrg::OnClickSquadUpgrade( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickSquadUpgrade");
	//
	if( XBREAK( m_pSelHero == nullptr ) )
		return 1;
	if(	ACCOUNT->IsLockHangout() ) {
		XWND_ALERT_T(XTEXT(2241), 6, XTEXT(2134));		// 6레벨 병사집합소를 열면 열립니다.
		return 1;
	}
	XHero *pHero = m_pSelHero->GetpHero();
	if( XBREAK( pHero == nullptr) )
		return 1;

	if (pHero->GetlevelSquad() == PROP_SQUAD->GetMaxLevel()) {
		auto pAlert = XWND_ALERT( "%s", XTEXT(2006));
		return 1;
	}
	// 부대 메달 제공 팝업
// 	auto pPopup = new XWndSquadLevelup( pHero );
	auto pPopup = new XWndLevelupHero( pHero, XGAME::xTR_SQUAD_UP );
	pPopup->SetbModal(TRUE);
	Add( pPopup );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneUnitOrg::OnClickCheatHeroLvUp( XWnd* pWnd, DWORD p1, DWORD p2 )
{
#ifdef _CHEAT
	CONSOLE("OnClickCheatHeroLvUp");
	//
	if( XBREAK( m_pSelHero == nullptr ) )
		return 1;
	XHero *pHero = m_pSelHero->GetpHero();
	XBREAK( pHero == nullptr );
	auto type = (XGAME::xtTrain)p1;
	int lvCurr = pHero->GetLevel( type );
	int lvMax = pHero->GetMaxLevel( type );
	if( lvCurr >= lvMax )
		return 1;
#ifdef WIN32
	int lvNext = (XE::GetMain()->m_bCtrl)? lvCurr + 10 : lvCurr + 1;
#else
	int lvNext = lvCurr + 1;
#endif // WIN32
	if( lvNext > lvMax )
		lvNext = lvMax;
	GAMESVR_SOCKET->SendCheat( this, 2, pHero->GetsnHero(), lvNext, type );
	SetbUpdate( true );
#endif
	return 1;
}

/****************************************************************
* @brief 영웅 소환 해제
*****************************************************************/
int XSceneUnitOrg::OnClickReleaseHero( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickReleaseHero");
	//
	if( m_pSelHero )	{
		if( ACCOUNT->IsTrainingHeroByAny( m_pSelHero->GetsnHero() ) ) {
			XWND_ALERT("%s", _T("훈련중인 영웅은 해고시킬 수 없습니다.") );
			return 1;
		}
	}
	XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_fire", nullptr);
	pPopup->SetbModal(true);
	Add(pPopup);
	xSET_TEXT( this, "text.fire.hero", XTEXT( 80167 ) );	// 해고하시겠?

	xSET_BUTT_HANDLER(this, "butt.decide.fire", &XSceneUnitOrg::OnClickReleaseHeroOk);
	xSET_BUTT_HANDLER(this, "butt.cancel", &XSceneUnitOrg::OnSelectExit);
	
	return 1;
}

/**
 @brief 영웅 소환해제
*/
void XSceneUnitOrg::OnRecvReleaseHero( ID snHero )
{
	XWnd *pWnd = Find( "list.inven.hero" );
	if( pWnd == nullptr )
		return;
	if( m_pSelHero->GetpHero() && m_pSelHero->GetpHero()->GetsnHero() == snHero )
		m_pSelHero = nullptr;
	XWndList *pWndList = SafeCast<XWndList*, XWnd*>( pWnd );
	if( pWndList )
	{
		pWndList->DelItem( snHero );
		pWnd = pWndList->GetElemByIndex( 0 );
		if (pWnd)
		{
			m_pSelHero = SafeCast<XWndInvenHeroElem*, XWnd*>(pWnd);
			m_pSelHero->SetbSelected( true );
		}
	}
	xSET_ENABLE(this, "common_butt_small.png", TRUE);
	SetbUpdate( TRUE );

}
void XSceneUnitOrg::OnRecvLevelupSquad(ID snHero)
{
	XWnd *pPopup = new XWndView(m_Layout.GetpLayout(), "base_popup", nullptr);
	pPopup->SetbModal(TRUE);
	Add(pPopup);
	m_Layout.CreateLayout("levelup_complete", pPopup);
	xSET_TEXT(this, "text.popup.title", XTEXT(80024));
	xSET_BUTT_HANDLER(this, "butt.levelup.ok", &XSceneUnitOrg::OnSelectExit);

	XWnd *pBar = pPopup->Find("pbar.levelup.exp");
	if (pBar)
		pBar->SetbDestroy(TRUE);

	XHero *pHero = ACCOUNT->GetHero(snHero);
	XPropUnit::xPROP *pProp = PROP_UNIT->GetpProp(pHero->GetUnit());
	if (pHero && pProp)
	{
		auto pImg = xSET_IMG(this, "img.icon.levelup.hero", XE::MakePath(DIR_IMG, pProp->strFace.c_str()), XE::xPF_ARGB8888);

		xSquadStat statCurr;
		pHero->GetSquadStatWithTech( pHero->GetlevelSquad(), &statCurr );
		xSET_TEXT(pPopup, "text.levelup.name", pProp->strName.c_str());
		xSET_TEXT(pPopup, "text.levelup.level", XE::Format(_T("Lv%d"), pHero->GetlevelSquad()));
		xSET_TEXT(pPopup, "text.levelup.melee", XE::Format(_T("%d"), (int)statCurr.meleePower ));
		xSET_TEXT(pPopup, "text.levelup.range", XE::Format(_T("%d"), (int)statCurr.rangePower ));
		xSET_TEXT(pPopup, "text.levelup.def", XE::Format(_T("%d"), (int)statCurr.def ));
		xSET_TEXT(pPopup, "text.levelup.hp", XE::Format(_T("%d"), (int)statCurr.hp ));
		xSET_TEXT(pPopup, "text.levelup.atk.speed", XE::Format(_T("%d"), (int)(statCurr.speedAtk * 100) ));
		xSET_TEXT(pPopup, "text.levelup.move.speed", XE::Format(_T("%d"), (int)statCurr.speedMoveForMeter ));
	}
}

int XSceneUnitOrg::OnClickUnitInfo(XWnd *pWnd, DWORD p1, DWORD p2)
{
	CONSOLE("OnClickUnitInfo");
	//
	if (XBREAK(m_pSelHero == nullptr))
		return 1;
	XHero *pHero = m_pSelHero->GetpHero();
	if (XBREAK(pHero == nullptr))
		return 1;
	auto pPopup = new XWndUnitinfo( pHero, XTEXT(80061) );
	Add( pPopup );
	
	return 1;
}

void XSceneUnitOrg::SetpKeepHero(XHero *pHero)
{
	SAFE_DELETE(m_pKeepHero);
	m_pKeepHero = new XHero(*pHero);
}

/**
 @brief 장비 슬롯 누름
*/
int XSceneUnitOrg::OnClickEquipSlot(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XGAME::xtParts typeParts = (XGAME::xtParts)p1;
	auto spParam = std::shared_ptr<XSceneEquip::xSPM_EQUIP>( new XSceneEquip::xSPM_EQUIP() );
	spParam->snHero = m_pSelHero->GetpHero()->GetsnHero();
	spParam->idxParts = typeParts;
	DoExit(XGAME::xSC_EQUIP, spParam);
	return 1;
}

void XSceneUnitOrg::SetSprSquad(XWnd *pWnd, XHero *pHero)
{
	XPropUnit::xPROP *pPropUnit = PROP_UNIT->GetpProp(pHero->GetUnit());
	XE::VEC2 vDist;
	if (pPropUnit->idProp == XGAME::xUNIT_PALADIN)
		vDist = XE::VEC2(xMETER_TO_PIXEL(4.f), xMETER_TO_PIXEL(2.f));
	else
	{
		switch (pPropUnit->size)
		{
		case XGAME::xSIZE_SMALL:	vDist = XE::VEC2(xMETER_TO_PIXEL(2.6f), xMETER_TO_PIXEL(1.4f));		break;
		case XGAME::xSIZE_MIDDLE:	vDist = XE::VEC2(xMETER_TO_PIXEL(5.4f), xMETER_TO_PIXEL(4.0f));	break;
		case XGAME::xSIZE_BIG:		vDist = XE::VEC2(xMETER_TO_PIXEL(10.0f), xMETER_TO_PIXEL(8.0f));	break;
		}
	}

	int row = 5;
	switch (pPropUnit->size)
	{
	case XGAME::xSIZE_SMALL:	row = 5;		break;
	case XGAME::xSIZE_MIDDLE:	row = 2;	break;
	case XGAME::xSIZE_BIG:		row = 2;	break;
	}

	vDist *= 0.8f;

	int numUnit = pHero->GetnumUnit();
	int col = numUnit / row;	// 5명씩 몇줄로 생기는지
	int mod = numUnit % row;
	XE::VEC2 vStart;
	int idxUnit = 0;
	if (col == 0)
	{
		vStart = XE::VEC2(381 + vDist.w * (col) / 2, 290);
		vStart.y -= (mod - 1) * (vDist.y / 2.f);
	}
	else
	{
		int adjust = 0;
		if (mod != 0)
			++adjust;
		vStart = XE::VEC2(381 + vDist.w * (col + adjust - 1) / 2, 290);
		vStart.y -= (row - 1) * (vDist.y / 2.f);
	}
	for (int i = 0; i < col; ++i)
	{
		for (int j = 0; j < row; ++j)
		{
			XE::VEC2 vCurr;
			vCurr.x = vStart.x - vDist.w * i + j * 2;
			vCurr.y = vStart.y + vDist.h * j;
			auto pSpr = new XWndSprObj(pPropUnit->strSpr.c_str(), ACT_IDLE1, vCurr);
//			pSpr->GetpSprObj()->SetbPause(TRUE);
			pSpr->SetFlipHoriz(TRUE);
			pSpr->SetScaleLocal(0.8f);
			pWnd->Add(pSpr);
			++idxUnit;
		}
	}
	for (int j = 0; j < mod; ++j)
	{
		XE::VEC2 vCurr;
		vCurr.x = vStart.x - vDist.w * col + j * 2;
		vCurr.y = vStart.y + vDist.h * j;
		auto pSpr = new XWndSprObj(pPropUnit->strSpr.c_str(), ACT_IDLE1, vCurr);
//		pSpr->GetpSprObj()->SetbPause(TRUE);
		pSpr->SetFlipHoriz(TRUE);
		pSpr->SetScaleLocal(0.8f);
		pWnd->Add(pSpr);
		++idxUnit;
	}
}

bool XSceneUnitOrg::CompParty(XHero *p1, XHero *p2)
{
	XHero *pHero1 = p1;
	XHero *pHero2 = p2;
	int value1 = 0, value2 = 0;
	if (ACCOUNT->GetCurrLegion()->GetSquadronByHeroSN(pHero1->GetsnHero()))
		value1 = 1;
	if (ACCOUNT->GetCurrLegion()->GetSquadronByHeroSN(pHero2->GetsnHero()))
		value2 = 1;
	return value2 < value1;
}

int XSceneUnitOrg::OnClickReleaseHeroOk(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (m_pSelHero)	{
		XHero *pHero = m_pSelHero->GetpHero();
		if (pHero)		{
			GAMESVR_SOCKET->SendReqReleaseHero(this, pHero->GetsnHero());
			pWnd->SetbEnable(FALSE);
		}
	}
	if (pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(true);
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneUnitOrg::OnClickInfoSquad( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickInfoSquad");
	//
	if( XBREAK(m_pSelHero == nullptr) )
		return 1;
	XHero *pHero = m_pSelHero->GetpHero();
	if( XBREAK( pHero == nullptr ) )
		return 1;
	auto pPopup = new XWndPopup( _T( "squad_info.xml" ), "squad_info" );
	pPopup->SetbModal( TRUE );
//	xSET_TEXT( pPopup, "text.info", strInfo );
	Add( pPopup );
//	XE::VEC2 vStart( 54, 52 );
	XE::VEC2 vStart;
	int lvMax = PROP_SQUAD->GetMaxLevel();
	int maxUnitLast = 0;
	XWnd* pWndRoot = new XWnd();
	for( int i = 1; i < lvMax; ++i ) {
		auto& propSquad = PROP_SQUAD->GetTable( i );
		int maxUnit = propSquad.GetMaxUnit( pHero->GetUnit() );
		if( maxUnitLast != maxUnit ) {
			// 부대레벨: 병사수
			const _tstring strInfo = XFORMAT( "%s:%d %s: %d\n", XTEXT(2257), i, XTEXT(2298), maxUnit );
			auto pText = new XWndTextString( vStart, strInfo );
			if( i <= pHero->GetlevelSquad() )
				pText->SetColorText( XCOLOR_YELLOW );
			else
				pText->SetColorText( XCOLOR_WHITE );
			pWndRoot->Add( pText );
			vStart.y += 14.f;
		}
		maxUnitLast = maxUnit;
	}
	pWndRoot->SetAutoSize();
	pPopup->Add( pWndRoot );
	pWndRoot->AutoLayoutCenter();
// 	_tstring strInfo;
// 	int lvMax = PROP_SQUAD->GetMaxLevel();
// 	int maxUnitLast = 0;
// 	for( int i = 1; i < lvMax; ++i ) {
// 		auto& propSquad = PROP_SQUAD->GetTable( i );
// 		int maxUnit = propSquad.GetMaxUnit( pHero->GetUnit() );
// 		if( maxUnitLast != maxUnit ) {
// 			strInfo += XFORMAT("부대레벨%d 인원수: %d\n", i, maxUnit );
// 		}
// 		maxUnitLast = maxUnit;
// 	}

	return 1;
}

/****************************************************************
* @brief 훈련완료 확인(렙업이 아님)
*****************************************************************/
// int XSceneUnitOrg::OnClickTrainAccept( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickTrainAccept:%d", p1);
// 	//
// 	auto type = (XGAME::xtTrain) p1;
// 	if( m_pSelHero )
// 	{
// 		XHero *pHero = m_pSelHero->GetpHero();
// 		if( pHero )
// 		{
// 			GAMESVR_SOCKET->SendReqTrainCompleteTouch( GAME, type, pHero );
// 		}
// 	}
// 	
// 
// 	return 1;
// }

/****************************************************************
* @brief 영웅3종렙업세트 렙업이 되었으면 확인
*****************************************************************/
// int XSceneUnitOrg::OnClickLevelupConfirm( XWnd* pWnd, DWORD p1, DWORD p2 )
// {
// 	CONSOLE("OnClickLevelupConfirm");
// 	//
// 	auto type = ( XGAME::xtTrain ) p1;
// 	XBREAK( XGAME::IsInvalidTrainType(type) );
// 	if( m_pSelHero )
// 	{
// 		XHero *pHero = m_pSelHero->GetpHero();
// 		if( pHero ) {
// 			GAMESVR_SOCKET->SendReqLevelupConfirm( GAME, type, pHero );
// 		}
// 	}
// 	
// 	return 1;
// }


// void XSceneUnitOrg::OnRecvLevelupConfirm( XHero *pHero, XGAME::xtTrain type )
// {
// 	XWnd *pWndLv = nullptr;
// 	_tstring str;
// 	switch( type )
// 	{
// 	case XGAME::xTR_LEVEL_UP:
// 		pWndLv = Find( "spr.levelup.level" );
// 		break;
// 	case XGAME::xTR_SQUAD_UP:
// 		pWndLv = Find( "spr.levelup.squad" );
// 		break;
// 	case XGAME::xTR_SKILL_ACTIVE_UP: {
// 		pWndLv = Find( "spr.levelup.skill.active" );
// 	} break;
// 	case XGAME::xTR_SKILL_PASSIVE_UP: {
// 		pWndLv = Find( "spr.levelup.skill.passive" );
// 	} break;
// 	default:
// 		break;
// 	}
// 	if( pWndLv )
// 	{
// 		pWndLv->SetbDestroy( TRUE );
// 	}
// }

/****************************************************************
* @brief 훈련소
*****************************************************************/
int XSceneUnitOrg::OnClickTrainingCenter( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickTrainingCenter");
	if( ACCOUNT->IsLockAcademy() ) {
		XWND_ALERT_T( XTEXT( 2241 ), 5, XTEXT(2133) );		// 5레벨에 열립니다.
		return 1;
	}
	//
	auto pPopup = new XWndTrainingCenter();
	Add( pPopup );
	
	return 1;
}

/****************************************************************
* @brief 영웅 렙업훈련 즉시 완료
*****************************************************************/
int XSceneUnitOrg::OnClickLvupComplete( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickLvupComplete");
	//
	return 1;
}

int XSceneUnitOrg::OnClickPromotion(XWnd* pWnd, DWORD p1, DWORD p2)
{
	if (XBREAK(m_pSelHero == nullptr))
		return 0;
	bool bPromotion = (p1 == 0);	//1이면 소환 0이면 승급
	if (bPromotion) {
		// 승급
		auto pHero = m_pSelHero->GetpHero();
		if( XASSERT(pHero) ) {
			GAMESVR_SOCKET->SendReqPromotionHero( GAME, pHero );
// 			auto result = ACCOUNT->DoPromotionHero( m_pSelHero->GetsnHero() );
// 			switch( result )
// 			{
// 			case 0:break;		// 5등급임
// 			case 1:SetbUpdate( true ); break;	//성공
// 			case 2:break;		// 영혼석 부족
// 			}
		}
	} else {
		auto bOk = ACCOUNT->IsAbleSummonHeroBySoulStone(m_pSelHero->GetpProp()->strIdentifier);
		if( bOk == XGAME::xE_OK ) {
			auto pPropHero = PROP_HERO->GetpProp( m_pSelHero->GetpProp()->strIdentifier );
			if( XASSERT(pPropHero) ) {
				GAMESVR_SOCKET->SendReqSummonHeroByPiece( GAME, pPropHero );
			}
// 			GAMESVR_SOCKET->SendCheat(GAME, 8, m_pSelHero->GetpProp()->idProp, 1);
// 			ACCOUNT->DestroyItemBySN(ACCOUNT->GetsnSoulStone(m_pSelHero->GetpProp()->strIdentifier), XGAME::GetNeedSoulSummon(m_pSelHero->GetGrade()));
		} else {
			XBREAK(1);
		}

	}
	return 1;
}

int XSceneUnitOrg::OnEnterScene( XWnd*, DWORD, DWORD )
{
	const auto idScene = GetidScene();
	bool bHave = ACCOUNT->GetpQuestMng()->IsHaveQuestTypeWithParam( XGAME::xQC_EVENT_UI_ACTION
																																, XGAME::xUA_ENTER_SCENE
																																, idScene );
	if( bHave )
		GAMESVR_SOCKET->SendUIAction( XGAME::xUA_ENTER_SCENE, idScene );
	return XSceneBase::OnEnterScene( nullptr, 0, 0 );
}


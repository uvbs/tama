#include "StdAfx.h"
#include "XSceneTech.h"
#include "XGame.h"
#include "client/XAppMain.h"
#include "XPropTech.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XSkillMng.h"
//#include "XHero.h"
#include "XLegion.h"
#include "XWndTemplate.h"
#include "XWndTech.h"
#include "XWndResCtrl.h"
#include "_Wnd2/XWndProgressBar.h"
#include "_Wnd2/XWndList.h"
#include "XSystem.h"
#ifdef WIN32
#include "CaribeView.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define KEY_SCRL_VIEW	"view.scrl"
#define KEY_ROOT_GAMEVIEW	"wnd.root.game"
#define KEY_ROOT_TOOLVIEW	"wnd.root.tool"

XSceneTech *SCENE_TECH = NULL;


//////////////////////////////////////////////////////////////////////////
void XSceneTech::Destroy() 
{	
	SAFE_RELEASE2( IMAGE_MNG, m_psfcArrow );
	XBREAK( SCENE_TECH == NULL );
	XBREAK( SCENE_TECH != this );
	SCENE_TECH = NULL;
}

XSceneTech::XSceneTech( XGame *pGame ) 
	: XSceneBase( pGame, XGAME::xSC_TECH )
	, m_Layout(_T("layout_tech.xml"))
{ 
	XBREAK( SCENE_TECH != NULL );
	SCENE_TECH = this;
	Init(); 

	m_Layout.CreateLayout("common_bg", this);
	m_Layout.CreateLayout("tech_game", this);
	XTRACE("CreateLayout finished");
	XWnd *pRoot = Find(KEY_ROOT_GAMEVIEW);
	XBREAK( pRoot == nullptr );
	xSET_BUTT_HANDLER( this, "butt.back", &XSceneTech::OnBack );
	for( int i = 0; i < XGAME::xUNIT_MAX; ++i ) {
		if( i ) {
			auto unit = (XGAME::xtUnit)i;
			auto pButt = Findf( "butt.%d", i );
			if( XASSERT(pButt) )
				pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnClickUnit, (DWORD)unit );
		}
	}
	xSET_BUTT_HANDLER( pRoot, "butt.plus", &XSceneTech::OnClickPlus );
	xSET_BUTT_HANDLER( pRoot, "butt.lock.free", &XSceneTech::OnClickLockFree );
	SetButtHander( pRoot, "butt.research.complete", &XSceneTech::OnClickResearchComplete );
	SetButtHander( pRoot, "butt.init.abil", &XSceneTech::OnClickInitAbil );
	XTRACE( "butt event finished" );
	UpdateUnitButtons();
	XTRACE( "UpdateTreeNode finished" );

	m_psfcArrow = IMAGE_MNG->Load( XE::MakePath(DIR_UI, _T("skill_arrow.png")) );
	XTRACE( "arrow load finished" );

#ifdef _XSINGLE
	XWndButtonDebug *pButt = new XWndButtonDebug( 600.f, 290.f, 40.f, 40.f, _T("전투") );
	pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnDebug, 1 );
	Add( pButt );
#endif // _XSINGLE
	ID idResearching = ACCOUNT->GetResearching().GetidAbil();
	SetbUpdate( TRUE );
}

void XSceneTech::Create( void )
{
	XEBaseScene::Create();
	///< 
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		XE::VEC2 v(572,87);
		XE::VEC2 size(30,30);
		auto pButt = new XWndButtonDebug( v, size, _T("금화추가") );
		pButt->SetstrIdentifier("butt.debug.brave");
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnCheat, 200 );
		Add( pButt );
	}
#endif // cheat
}

void XSceneTech::Update()
{
	XGAME::CreateUpdateTopResource( this );
	// 선택된 영웅이 없으면 첫번째 영웅으로 한다.
	XHero *pHero = nullptr;
	if( m_snSelectedHero == 0 ) {
		pHero = ACCOUNT->GetpHeroByIndex( 0 );	// 선택된게 없으면 첫번째로 있는 영웅을 얻는다.
		if( pHero )
			m_snSelectedHero = pHero->GetsnHero();
	} else
		pHero = ACCOUNT->GetHero( m_snSelectedHero );
	if( pHero == nullptr )
		return;
	m_pHeroSected = pHero;
	if( m_unitSelected ) {
		// 선택된 유닛이 현재 선택된영웅의 병과와 맞지 않으면.
		if( XGAME::GetAtkType(m_unitSelected) != pHero->GetTypeAtk() ) {
			// 선택된 유닛의 영웅을 먼저 찾아본다.
			auto pHeroByUnit = ACCOUNT->GetpHeroByUnit( m_unitSelected );
			if( pHeroByUnit ) {
				// 있으면 그 영웅을 자동선택해준다.
				pHero = m_pHeroSected = pHeroByUnit;
				m_snSelectedHero = pHero->GetsnHero();
			} else {
				// 없으면 같은 병과의 영웅을 찾는다.
				auto pHeroAtkType = ACCOUNT->GetpHeroByAtkType( XGAME::GetAtkType( m_unitSelected ) );
				if( pHeroAtkType ) {
					pHero = m_pHeroSected = pHeroAtkType;
					m_snSelectedHero = pHero->GetsnHero();
				} else {
					// 같은병과 영웅도 없으면 선택된 영웅의 유닛의 병과를 기준으로 유닛을 재선택한다.
//					m_unitSelected = XGAME::GetUnitBySizeAndAtkType( XGAME::xSIZE_SMALL, pHeroAtkType->GetTypeAtk() );
				}
			}
		}
	} else {
		m_unitSelected = pHero->GetUnit();
//		m_unitSelected = XGAME::GetUnitBySizeAndAtkType( XGAME::xSIZE_SMALL, pHero->GetpProp()->typeAtk );
	}
	auto pProp = XPropTech::sGet()->GetpNode( m_unitSelected, m_idSelectedNode );
	// 유닛이나 노드가 선택이 안된상태라면 null을 받는다.
	//
	auto pRoot = Find( KEY_ROOT_GAMEVIEW );
	if( pRoot ) {
		// 왼쪽 영웅리스트
		UpdateHeroList( pRoot );
		// 상단 유닛 버튼
		UpdateTopUnitButtons( pRoot, pHero );
		// 특성 트리를 업데이트 한다.
		UpdateTree( pRoot, pHero );
		// 선택된 영웅이 있을때만.
		if( pHero ) {
//		UpdateTreeNode( pRoot );
			const auto abil = pHero->GetAbilNode( m_unitSelected, m_idSelectedNode );
			// 우측 패널 갱신
			UpdateRightPanel( pRoot, pHero, pProp, abil );
		}
		if( pHero ) {
		} else {
			// 영웅이 선택안되어 있어도 들어가서 갱신해야함.
			UpdateRightPanel( pRoot, pHero, pProp, XGAME::xAbil() );
		}
	}
	//
	XEBaseScene::Update();
}

/**
 @brief 왼쪽 영웅 리스트
*/
void XSceneTech::UpdateHeroList( XWnd *pRoot )
{
	auto pList = xGET_LIST_CTRL( pRoot, "list.hero" );
	if( pList ) {
		const XGAME::xResearch& research = ACCOUNT->GetResearching();
		pList->SetEvent( XWM_SELECT_ELEM, this, &XSceneTech::OnClickHero );
		auto spLegion = ACCOUNT->GetCurrLegion();
		{
			if( m_listHeroes.size() == 0 )
				ACCOUNT->_GetInvenHero( m_listHeroes );
			if( m_bUpdateSort ) {
				const auto unitSelected = m_unitSelected;
				const auto typeAtk = XGAME::GetAtkType( unitSelected );
				m_listHeroes.Sort( 
					[ typeAtk, unitSelected, spLegion ]( XHero* pHero1, XHero* pHero2 ) {
						int val1 = 0;		// 왼쪽우선
						int val2 = 0;		// 오른쪽우선.
						const auto unit1 = pHero1->GetUnit();
						const auto unit2 = pHero2->GetUnit();
						const auto atk1 = pHero1->GetTypeAtk();
						const auto atk2 = pHero2->GetTypeAtk();
						if( atk1 == typeAtk ) {		// 왼쪽영웅이 일단 병과가 맞으면 1점
							++val1;
							if( unit1 == unitSelected ) {	// 유닛까지 맞으면 2점
								++val1;
								if( spLegion->IsEnteredHero(pHero1->GetsnHero()) )	// 군단소속까지 있으면 3점
									++val1;
							}
						}
						if( atk2 == typeAtk ) {	// 오른쪽영웅이   "     "
							++val2;
							if( unit2 == unitSelected ) {
								++val2;
								if( spLegion->IsEnteredHero( pHero2->GetsnHero() ) )
									++val2;
							}
						}
						return val1 > val2; 
					} );
				m_bUpdateSort = false;
				pList->DoMoveTop();
			}

		}
		// 소트가 되어 위치가 바뀌는것이므로 모두 파괴하고 다시 리스트를 구성함.
		pList->DestroyAllItem();
		for( auto pHero : m_listHeroes ) {
			auto pButtHero = SafeCast2<XWndStoragyItemElem*>( pList->Find( pHero->GetsnHero() ) );
			if( pButtHero == nullptr ) {
				XGAME::xReward reward;
				pButtHero = new XWndStoragyItemElem( XE::VEC2( 0 ), pHero );
				pButtHero->SetScaleLocal( 1.f );
				pButtHero->SetNum( 0 );
				// 장착된 유닛을 그림.
				auto pWndUnit = new XWndCircleUnit( pHero->GetUnit(), XE::VEC2( 40, 20 ), nullptr );
				pWndUnit->SetbShowLevelSquad( true );
				pButtHero->Insert( pButtHero->GetIdsName(), pWndUnit );
				pList->AddItem( pHero->GetsnHero(), pButtHero );
				// 장착된(?)유닛의 특성포인트를 표시
				auto pImg = new XWndImage(PATH_UI("circle_bg_s.png"), XE::VEC2(21,0));
				pWndUnit->Add( pImg );
				auto pText = new XWndTextString( XFORMAT("%d", pHero->GetNumSetAbilPointByUnit(pHero->GetUnit()))
																, FONT_RESNUM, 15.f );
				pText->SetStyleStroke();
				pText->SetAlignCenter();
				pImg->Add( pText );
				if( pHero->GetTypeAtk() != XGAME::GetAtkType( m_unitSelected ) ) {
					pButtHero->SetblendFunc( XE::xBF_GRAY );
					pWndUnit->SetblendFunc( XE::xBF_GRAY );
					pImg->SetblendFunc( XE::xBF_GRAY );
					pText->SetColorText( XCOLOR_GRAY );
				} else {
					pButtHero->SetblendFunc( XE::xBF_MULTIPLY );
					pWndUnit->SetblendFunc( XE::xBF_MULTIPLY );
					pImg->SetblendFunc( XE::xBF_MULTIPLY );
					pText->SetColorText( XCOLOR_GREEN );
				}
			}
			if( pButtHero ) {
				// 현재 선택된 영웅이거나 연구중인 영웅이라면 선택표시 해줌
				bool bResearching = (research.IsResearching() && research.GetsnHero() == pHero->GetsnHero());
				auto bSelected = (m_snSelectedHero == pHero->GetsnHero() || bResearching );
				pButtHero->SetbSelected( bSelected );
				pButtHero->SetbGlowAnimation( bResearching );
				// 				// 장착된 유닛을 그림.
				// 현재 어떤 유닛이라도 연구가능상태면 녹색점 표시
				bool bAble = ACCOUNT->IsAbleResearchUnit( pHero );
				if( spLegion->IsEnteredHero( pHero->GetsnHero()) )	//출전중인 장수만.
					GAME->SetGreenAlert( pButtHero, bAble, XE::VEC2( 44, 2 ) );
			}
		}
	}
}
/**
 @brief 오른쪽 패널의 현재 선택한 영웅의 특성의 정보
*/
void XSceneTech::UpdateRightPanel( XWnd *_pRoot
																, XHero *pHero
																, XPropTech::xNodeAbil* pProp
																, const XGAME::xAbil& abil )
{
	XBREAK( _pRoot == nullptr );
	auto pRoot = _pRoot->Find( "img.right.panel");
	if( XBREAK(pRoot == nullptr) )
		return;
	if( pHero && pProp ) {
		XBREAK( pProp == nullptr );
		xSET_SHOW( this, "text.limit.level", FALSE );
		if( pProp->GetLvOpanable() > pHero->GetLevel() ) {
			// 아직 오픈레벨이 부족하면
			xSET_SHOW( pRoot, "text.limit.level", TRUE );
			xSET_TEXT( pRoot, "text.limit.level", XFORMAT("%s:%d", XTEXT(2019), pProp->GetLvOpanable()));
		} else {
			// 오픈레벨은 충족
			if( abil.IsLock() ) {
				// 잠겨있으면.
				const int numRemainUnlock = pHero->GetnumRemainAbilUnlock();
				if( numRemainUnlock > 0 ) {

				} else {
					// 잠금해제 비용
					int goldUnlock = pHero->GetGoldUnlockAbilCurr( /*m_unitSelected*/ );
					if( ACCOUNT->IsNotEnoughGold( goldUnlock ) ) {
						xSET_SHOW( this, "text.limit.level", TRUE );
						xSET_TEXT( this, "text.limit.level", XFORMAT( "%s:%d", XTEXT( 2026 ), goldUnlock ) );
					}
				}
			}
		}
	}
	// 선택한 특성의 이름이나 설명등의 업데이트
	UpdateSelectedAbil( pRoot, pHero, pProp, abil );
	// 연구중그래프 갱신( 연구중이면 선택한 영웅에 관계없이)
	UpdateAbilResearch( pRoot, pHero, pProp, abil );
	// 특성초기화 및 여분포인트 표시
	UpdateInitAbil( pRoot, pHero );
}
/**
 @brief 선택한 특성의 아이콘/이름/설명 등을 갱신
*/
void XSceneTech::UpdateSelectedAbil( XWnd *pRoot
																, XHero *pHero
																, XPropTech::xNodeAbil* pProp
																, const XGAME::xAbil& abil )
{
	auto pName = SafeCast2<XWndTextString*>( Find( "text.name" ) );
	auto pDesc = SafeCast2<XWndTextString*>( Find( "text.desc" ) );
	auto pPoint = SafeCast2<XWndTextString*>( Find( "text.level.tech" ) );
	auto pIcon = dynamic_cast<XWndAbilButton*>( Find( "butt.desc.icon" ) );
	if( pIcon )
		pIcon->SetbShow( (pProp && pHero) );
	if( pProp && pHero ) {
		// 선택한 특성의 아이콘
		if( pIcon == nullptr ) {
			XWnd *pImg = Find( "img.select" );
			if( XASSERT( pImg ) ) {
				const auto posBgFrame = pImg->GetPosLocal();		// 우측백판에서의 좌표
				auto pButt = new XWndAbilButton( pHero, m_unitSelected, pProp, posBgFrame );
				pButt->SetstrIdentifier( "butt.desc.icon" );
				pButt->SetbActive( FALSE );
				pButt->SetbInRightPanel( true );
				pRoot->Add( pButt );
				pImg->SetbUpdate( TRUE );
//				xSET_SHOW( this, "butt.plus", TRUE );
			}
		} else {
			pIcon->SetNode( m_unitSelected, pProp );
			pIcon->SetbUpdate( TRUE );
		}
		// 특성의 이름
		if( pName && pProp->idName ) {
			pName->SetbShow( true );
#ifdef _CHEAT
			if( XAPP->m_bDebugMode )
				pName->SetText( XE::Format( _T( "%s(%d:%s)" ), XTEXT( pProp->idName ), pProp->idNode, pProp->strSkill.c_str() ) );
			else
				pName->SetText( XE::Format( _T( "%s" ), XTEXT( pProp->idName ) ) );
#else
			pName->SetText( XE::Format( _T( "%s" ), XTEXT( pProp->idName ) ) );
#endif
		}
		// 특성의 설명
		if( pDesc && pProp->idDesc ) {
			pDesc->SetbShow( true );
			auto pSkillDat = SKILL_MNG->FindByIdentifier( pProp->strSkill );
			if( XASSERT( pSkillDat ) ) {
				auto pNode = XPropTech::sGet()->GetpNodeBySkill( m_unitSelected, pProp->strSkill );
				if( XASSERT( pNode ) ) {
					int level = pHero->GetLevelAbil( m_unitSelected, pNode->idNode );
					if( level <= 0 )
						level = 1;
					_tstring strDesc = XTEXT( pProp->idDesc );
					pSkillDat->GetSkillDesc( &strDesc, level );
					pDesc->SetText( strDesc.c_str() );
				}
			} else
				pDesc->SetText( XE::Format( _T( "%s" ), XTEXT( pProp->idDesc ) ) );
		}
		// 선택한 특성의 현재 포인트 ex:)1/5 
		if( pPoint ) {
			pPoint->SetbShow( true );
			if( abil.IsLock() )
				pPoint->SetText( XFORMAT( "%s", XTEXT( 2022 ) ) );
			else
				pPoint->SetText( XE::Format( _T( "%d/%d" ), abil.point, pProp->maxPoint ) );
		}
	} else {
		// 영웅이나 특성등이 선택 안되어 있을때
		if( pDesc )
			pDesc->SetbShow( false );
		if( pName )
			pName->SetbShow( false );
		if( pPoint )
			pPoint->SetbShow( false );
	}
}

void XSceneTech::UpdateNeedRes( const XPropTech::xtResearch& costAbil )
{
	auto pWndRoot = Find("wnd.need.res");
	if( !pWndRoot )
		return;
	DestroyWndByIdentifier( "ctrl.need.res" );
	auto pWndRes = new XWndResourceCtrl();
	pWndRes->SetScaleLocal( 0.75f );
	pWndRes->SetstrIdentifier("ctrl.need.res");
	pWndRoot->Add( pWndRes );
	for( int i = 0; i < 2; ++i ) {
		if( i < costAbil.aryResourceNeed.size() ) {
			const auto resType = costAbil.aryResourceNeed[i];
			const auto num = costAbil.aryNumNeed[i];
			XCOLOR col = XCOLOR_WHITE;
			if( ACCOUNT->GetResource( resType ) < num )
				col = XCOLOR_RED;
			pWndRes->AddRes( resType, num, col );
		}
	}
	pWndRes->AutoLayoutCenter();
}

/**
 @brief 특성초기화 버튼과 여분 포인트를 표시한다.
*/
void XSceneTech::UpdateInitAbil( XWnd *pRoot, XHero *pHero )
{
	bool bShow1 = pHero->GetnumRemainAbilPoint() > 0;
	bool bShow2 = pHero->GetnumRemainAbilUnlock() > 0;
	xSET_SHOW( pRoot, "img.abil.point", bShow1 );
	xSET_SHOW( pRoot, "img.num.unlock", bShow2 );
	xSET_TEXT_FORMAT( pRoot, "text.remain.point", XFORMAT("%d", pHero->GetnumRemainAbilPoint()) );
	xSET_TEXT_FORMAT( pRoot, "text.remain.unlock", XFORMAT("%d", pHero->GetnumRemainAbilUnlock()) );
}

/**
 @brief 잠금해제 버튼
*/
void XSceneTech::UpdateUnlockButton( XWnd *pRoot, XHero *pHero
																	, XPropTech::xNodeAbil* pPropNode )
{
	XBREAK( pRoot == nullptr );
	XBREAK( pHero == nullptr );
	XBREAK( pPropNode == nullptr );
	xSET_SHOW( pRoot, "wnd.root.lock", TRUE );		// 잠금관련 컨트롤의 루트
	xSET_SHOW( pRoot, "wnd.root.plus", FALSE );
	xSET_SHOW( pRoot, "wnd.root.research", FALSE );
	xSET_SHOW( pRoot, "wnd.root.complete", FALSE );
	xSET_TEXT( pRoot, "text.gold.have", XFORMAT( "%s:\n%s", XTEXT( 2025 ), XE::NumberToMoneyString( ACCOUNT->GetGold() )  ) );
	// 선택된 특성을 언락시킬수 있는가
	auto err = ACCOUNT->GetUnlockableAbil( pHero, m_unitSelected, pPropNode );
	bool bAbleUnlock = (err == XGAME::xE_OK || err == XGAME::xE_NOT_ENOUGH_GOLD);
	auto pButt = xSET_ENABLE( this, "butt.lock.free", bAbleUnlock );
// 	if( err == XGAME::xE_OK || err == XGAME::xE_NOT_ENOUGH_GOLD ) {
// 		xSET_ENABLE( this, "butt.lock.free", TRUE );
// 	}	else
// 		xSET_ENABLE( this, "butt.lock.free", FALSE );
	const int numRemainUnlock = pHero->GetnumRemainAbilUnlock();
	if( pButt ) {
		xSET_SHOW( pButt, "spr.butt.gold", (numRemainUnlock == 0) );
		xSET_SHOW( pButt, "img.butt.lock", (numRemainUnlock > 0) );
		if( numRemainUnlock > 0 ) {
			auto pTextCost = xSET_TEXT( pButt, "text.cost.unlock", XFORMAT("%d", 1) );
			pTextCost->SetColorText( XCOLOR_WHITE );
		} else {
			int goldUnlock = pHero->GetGoldUnlockAbilCurr( /*m_unitSelected*/ );
			auto pTextCost = xSET_TEXT( pRoot, "text.cost.unlock", XE::NumberToMoneyString( goldUnlock ) );
			if( pTextCost ) {
				if( err == XGAME::xE_NOT_ENOUGH_GOLD )
					pTextCost->SetColorText( XCOLOR_RED );
				else
					pTextCost->SetColorText( XCOLOR_WHITE );
			}
		}
	}
}
/**
 @brief 우측 하단 연구비용이나 연구그래프 갱신 
*/
void XSceneTech::UpdateAbilResearch( XWnd *pRoot
																	, XHero *pHeroSelected
																	, XPropTech::xNodeAbil* pPropNodeSelected
																	, const XGAME::xAbil& abilSelected
																	)
{
	XBREAK( pRoot == nullptr );
	XBREAK( pHeroSelected == nullptr );
//	XBREAK( pPropNodeSelected == nullptr );
	xSET_SHOW( this, "wnd.root.lock", false );		// 잠금해제버튼과 그 일당들(?)을 일단 사라지게함.
// 			xSET_SHOW( this, "text.gold.have", FALSE );	// 현재 용맹포인트
	xSET_SHOW( this, "wnd.root.plus", false );		// 연구시작 +버튼과 관련
	xSET_SHOW( this, "wnd.root.research", false );	// 연구중 관련
	xSET_SHOW( this, "wnd.root.complete", false );		// 연구완료 관련
	xSET_SHOW( pRoot, "img.flask", false );
	auto& research = ACCOUNT->GetResearching();
	if( research.IsResearching() && m_snSelectedHero == research.GetsnHero() ) {
		// 현재 어떤 특성을 연구중
		// 연구중이면 관련 컨트롤들을 나타나게 함.
		auto pRootResearch = xSET_SHOW( this, "wnd.root.research", true );
		auto pHeroResearching = ACCOUNT->GetHero( research.GetsnHero() );
		if( XBREAK(pHeroResearching == nullptr) )
			return;
		auto unit = research.Getunit();
		auto idAbil = research.GetidAbil();
		auto& abilResearching = pHeroResearching->GetAbilNode( unit, idAbil );
		if( pRootResearch )
			xSET_TEXT( pRootResearch, "text.hero.researching", XFORMAT("%s %s", pHeroResearching->GetstrName().c_str(), XTEXT(2333)) );
	} else {
		if( pPropNodeSelected ) {
			// 연구중이 아님
			if( abilSelected.IsLock() ) {
				// 락상태면 잠금해제 버튼과 그 일당들 나타나게 함.
				xSET_SHOW( this, "wnd.root.lock", true );
				// 잠금해제버튼 갱신
				UpdateUnlockButton( pRoot, pHeroSelected, pPropNodeSelected );
			} else {
				// 락상태가 아님
				if( abilSelected.point < 5) {
					// 아직 5포인트를 다 채우지 않았을때
					xSET_SHOW( this, "wnd.root.plus", true );
					// +버튼 활성시키거나 비활성화 시킴
					auto bAble = ACCOUNT->IsEnableAbil( pHeroSelected, m_unitSelected, pPropNodeSelected );
					xSET_ENABLE( pRoot, "butt.plus", bAble );
					// 남은 특성포인트
					const int numRemainPoint = pHeroSelected->GetnumRemainAbilPoint();
					if( numRemainPoint > 0 ) {
						// 특성포인트가 있으면 연구비용(자원/시간)이 들지 않는다.
						xSET_SHOW( pRoot, "text.need.resource1", false );
						xSET_SHOW( pRoot, "text.need.resource2", false );
						xSET_SHOW( pRoot, "text.time", false );
						auto pImg = xSET_SHOW( pRoot, "img.flask", true );
						xSET_TEXT( pImg, "text.need.point", _T("1") );
					} else {
						// 자원과 시간으로 연구.
						auto& costAbil = pHeroSelected->GetCostAbilCurr( /*m_unitSelected*/ );
						_tstring str = XE::Format(_T("%s"), XTEXT(2018));	// 연구시간
						str += _sGetResearchTime( costAbil.sec );
						auto pWnd = xSET_TEXT( this, "text.time", str.c_str() );
						if( pWnd )
							pWnd->SetbShow( true );

						// 연구에 필요한 자원
						UpdateNeedRes( costAbil );
// 						for( int i = 0; i < 2; ++i ) {
// 							auto pText = xGET_TEXT_CTRLF( this, "text.need.resource%d", i+1 );
// 							if( pText ) {
// 								if( i < costAbil.aryResourceNeed.size() ) {
// 									pText->SetbShow( true );	
// 									auto resType = costAbil.aryResourceNeed[ i ];
// 									_tstring strResType = XGAME::GetStrResource( resType );
// 									int numNeed = costAbil.aryNumNeed[ i ];
// 				//						auto pText = xSET_TEXT( this, "text.need.resource1", XFORMAT( "%s:%s", strResType.c_str(), XE::NumberToMoneyString(numNeed) ) );
// 									pText->SetText( XFORMAT( "%s:%s", strResType.c_str(), XE::NumberToMoneyString(numNeed) ) );
// 									if( pText && ACCOUNT->GetResource( resType ) < numNeed )
// 										pText->SetColorText( XCOLOR_RED );
// 									else
// 										pText->SetColorText( XCOLOR_WHITE );
// 								} else {
// 									// 자원이 한종류만 필요한건 두번째껀 감춘다.
// 									pText->SetbShow( false );
// 								}
// 							}
// 						}
					}
				} else {
					// 5포인트까지 다 찍음.
					xSET_SHOW( this, "wnd.root.complete", true );
					xSET_TEXT( this, "text.complete", XTEXT(2027) );	// 연구완료
				}
			}
		}
	}
}
/**
 @brief 상단 유닛 버튼.
*/
void XSceneTech::UpdateTopUnitButtons( XWnd *pRoot, XHero* pHero )
{
#ifdef _XSINGLE
	xSET_ENABLE( this, "butt.minotaur.4", TRUE );
	xSET_ENABLE( this, "butt.cyclops.5", TRUE );
	xSET_ENABLE( this, "butt.lycan.6", TRUE );
	xSET_ENABLE( this, "butt.golem.7", TRUE );
	xSET_ENABLE( this, "butt.treant.8", TRUE );
	xSET_ENABLE( this, "butt.fallen_angel.9", TRUE );
#else
	const XGAME::xResearch& research = ACCOUNT->GetResearching();
	//
	for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
		if( i == 0 )	continue;
		auto unit = ( XGAME::xtUnit )i;
		// 잠긴상태의 유닛
		auto pButt = Findf( "butt.%d", i );
		if( pButt ) {
			GAME->DelBrilliant( pButt->getid() );	// 일단지우고
			pButt->SetblendFunc( XE::xBF_MULTIPLY );
			auto pImgUnit = xSET_IMG( pButt, "img.unit", XGAME::GetResUnitSmall(unit) );
			pImgUnit->SetblendFunc( XE::xBF_MULTIPLY );
			// 선택한 영웅이 선택할수 있는 병과의 유닛만 표시된다.
			if( pHero->GetpProp()->typeAtk == XGAME::GetAtkType( unit ) ) {
// 				pButt->SetbEnable( true );
				// 선택된 유닛이라면 글로우 나타나게 함.
				auto pImgGlow = xGET_IMAGE_CTRL( pButt, "img.glow" );
				if( pImgGlow ) {
					pImgGlow->SetbShow( unit == m_unitSelected );
					if( research.IsResearching() 
							&& research.Getunit() == unit
							&& research.GetsnHero() == pHero->GetsnHero() ) {
						pImgGlow->AddComponentWave( "alpha", 0, 0.5f, 0.5f, 1.f );
						pImgGlow->GetcompMngByAlpha().SetbActive( true );
					} else {
						pImgGlow->GetcompMngByAlpha().SetbActive( false );
					}
				}
//				xSET_SHOW( pButt, "img.glow", unit == m_unitSelected );
				xSET_SHOW( pButt, "img.bg.level", true );	// 현재포인트표시 원 나타탬.
				//
				if( ACCOUNT->IsLockUnit( unit ) ) {
					pButt->SetbEnable( true );
					// 선택가능하지만 아직 잠겨있는 유닛
// 					int lvUnlock = ACCOUNT->GetLevelUnlockableUnit( unit );
// 					bool bUnlockable = (lvUnlock == 0);
					bool bUnlockable = ACCOUNT->IsUnlockableUnit( unit );
					xSET_SHOW( pButt, "img.bg.level", false );
					xSET_SHOW( pButt, "img.lock", true );
					xSET_SHOW( pButt, "text.point", false );	// 테크포인트숫자를 감춤.
//					pButt->SetbEnable( bUnlockable );
					if( bUnlockable ) {
						// 잠금해제할수 있는 이펙트 표시
						pButt->SetpDelegate( GAME );
						GAME->AddBrilliant( pButt->getid(), 0.75f );
						xSET_SHOW( pButt, "text.level.open", false );	// 풀수 있는 상태면 오픈레벨표시 안함.
					} else {
						// 현재 잠금해제 할수 없음. 이미 잠금해제되어있거나 포인트가 없거나.
						xSET_SHOW( pButt, "text.level.open", true );
						int lvUnlock = ACCOUNT->GetLevelUnlockableUnit( unit );
						xSET_TEXT( pButt, "text.level.open", XFORMAT( "Lv%d", lvUnlock ) );
					}
				} else {
					// 선택가능하고 잠겨있지 않음.
					auto pHero = ACCOUNT->GetHero( m_snSelectedHero );
					if( pHero ) {
						// 자물쇠를 감추고 포인트숫자를 표시함.
						xSET_SHOW( pButt, "text.level.open", false );
						xSET_SHOW( pButt, "img.lock", false );
						// 현재 자원으로 연구할수 있는 특성이 있으면 녹색점을 찍는다.
						if( ACCOUNT->IsEnoughResourceForResearch( pHero/*, unit*/ ) 
							&& pHero->GetUnit() == unit )	// 현재 이끄는 부대에만 녹색점을 찍음.
								GAME->SetGreenAlert( pButt, true, XE::VEC2(13,-3) );
						else
							GAME->SetGreenAlert( pButt, FALSE );
		// 				int point = ACCOUNT->GetTechPoint( unit );
						int point = pHero->GetNumSetAbilPointByUnit( unit );
						auto pText =
							xSET_TEXT( pButt, "text.point", XFORMAT( "%d", point ) );
						if( pText ) {
							pText->SetbShow( true );
							if( point > 0 )
								pText->SetColorText( XCOLOR_GREEN );
							else
								pText->SetColorText( XCOLOR_WHITE );
						}
					}
				}
			} else { // if( pHero->GetpProp()->typeAtk == XGAME::GetAtkType( unit ) )
				// 선택한 영웅이 사용하지 못하는 유닛
//				pButt->SetbEnable( false );
				pButt->SetblendFunc( XE::xBF_GRAY );
				pImgUnit->SetblendFunc( XE::xBF_GRAY );
				GAME->SetGreenAlert( pButt, false );
				xSET_SHOW( pButt, "text.level.open", false );
				xSET_SHOW( pButt, "img.bg.level", false );	// 현재포인트표시 원 사라짐.
				xSET_SHOW( pButt, "img.lock", false );
				if( ACCOUNT->IsLockUnit( unit ) ) {
					// 사용하지 못하는 유닛이지만 잠금해제시에는 활성화 시켜야함.
// 					int lvUnlock = ACCOUNT->GetLevelUnlockableUnit( unit );
// 					bool bUnlockable = lvUnlock == 0;
					bool bUnlockable = ACCOUNT->IsUnlockableUnit( unit );
					xSET_SHOW( pButt, "text.point", false );	// 테크포인트숫자를 감춤.
					xSET_SHOW( pButt, "img.bg.level", false );	// 현재포인트표시 원 사라짐.
					xSET_SHOW( pButt, "img.lock", true );
//					pButt->SetbEnable( bUnlockable );	// 잠금해제할수 있으면 활성화
					if( bUnlockable ) {
						// 잠금해제할수 있는 이펙트 표시
						pButt->SetpDelegate( GAME );
						GAME->AddBrilliant( pButt->getid(), 0.75f );
						xSET_SHOW( pButt, "text.level.open", false );	// 풀수 있는 상태면 오픈레벨표시 안함.
					}	else {
						// 현재 잠금해제 할수 없음. 이미 잠금해제되어있거나 포인트가 없거나.
						xSET_SHOW( pButt, "text.level.open", true );
						int lvUnlock = ACCOUNT->GetLevelUnlockableUnit( unit );
						xSET_TEXT( pButt, "text.level.open", XFORMAT( "Lv%d", lvUnlock ) );
					}
				}
			}
		} // pButt
	} // for
#endif  // not _XSINGLE
}
/**
 @brief 특성 트리 갱신
*/
void XSceneTech::UpdateTree( XWnd *pRoot, XHero *pHero )
{
	// 연구중인 특성이 있으면 그 특성노드에 글로우를 붙인다.
	const XGAME::xResearch& research = ACCOUNT->GetResearching();
	if( research.IsResearching() && pHero ) {
		XWndAbilButton::s_idResearching = research.GetidAbil();
		XWndAbilButton::s_snHeroResearching = research.GetsnHero();
	} else {
		XWndAbilButton::s_idResearching = 0;
	}
	// 일단은 그냥 파괴하고 다시 생성하는걸로.
// 	XTRACE( "set scrollview finished" );
	// 트리영역의 루트
	XWnd *pRootTree = pRoot->Find( "wnd.tree.root" );
	if( pRootTree ) {
//		pRootTree->DestroyChildAll();
		BOOL bLoaded = FALSE;
		XArrayLinearN<XPropTech::xNodeAbil*, 1024> aryAbil;
		XPropTech::sGet()->GetNodesToAry( m_unitSelected, &aryAbil );
		// 화살표를 생성한다.,
		XARRAYLINEARN_LOOP( aryAbil, XPropTech::xNodeAbil*, pNodeAbil )	{
			for( auto pChild : pNodeAbil->listChild ) {
//				XE::VEC2 vStart = pNodeAbil->vPos + XE::VEC2(-4) + XE::VEC2( ICON_SIZE.w / 2.f, 0 );
				XE::VEC2 vStart = pNodeAbil->vPos + XE::VEC2( 0 ) + XE::VEC2( ICON_SIZE.w / 2.f, 0 );
				vStart.x -= XPropTech::ADJ_X;		// 오른쪽으로 너무 밀려있어서 좀 당김.
				vStart.y -= 75.f;
//				XE::VEC2 vEnd = pChild->vPos + XE::VEC2(-4) + XE::VEC2( ICON_SIZE.w / 2.f, ICON_SIZE.h );
				XE::VEC2 vEnd = pChild->vPos + XE::VEC2( 0 ) + XE::VEC2( ICON_SIZE.w / 2.f, ICON_SIZE.h );
				vEnd.y += 5.f;
				vEnd.y -= 75.f;
				const std::string ids = XE::Format("arrow.%d.%d", pNodeAbil->idNode, pChild->idNode );
				if( !Find( ids ) ) {
					auto pArrow = new XWndArrow4Abil( vStart, vEnd );
					pArrow->SetstrIdentifier( ids );
					pArrow->SetvAdjust( XE::VEC2( -XPropTech::ADJ_X, 0 ) );
					pRootTree->Add( pArrow );
				}
			}
		} END_LOOP;
		XTRACE( "create arrow finished" );
		XE::VEC2 vSelectedNode, vResearching;
		// 노드버튼을 생성한다.
		XARRAYLINEARN_LOOP( aryAbil, XPropTech::xNodeAbil*, pNodeAbil )	{
			XWnd *pExist = Find( pNodeAbil->idNode );
			if( pExist == nullptr ) {
				XE::VEC2 v = pNodeAbil->vPos;
				v.x -= XPropTech::ADJ_X;		// 오른쪽으로 너무 밀려있어서 좀 당김.
				v.y -= 75.f;
				const std::string ids = XE::Format( "butt.abil.%s", SZ2C( pNodeAbil->strSkill.c_str() ) );
				if( !Find( ids ) ) {
					auto pButt = new XWndAbilButton( pHero, m_unitSelected, pNodeAbil, v );
					pButt->SetstrIdentifier( ids );
//					pButt->SetstrIdentifierf("butt.abil.%s", SZ2C(pNodeAbil->strSkill.c_str()) );
					pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnClickNode, pNodeAbil->idNode );
	//				pButt->SetScaleLocal( 0.816f );
					pRootTree->Add( pNodeAbil->idNode, pButt );
					pExist = pButt;
				}
			} 
			if( pExist ) {
				pExist->SetbUpdate( TRUE );
				// 이 노드가 선택된 노드면 글로우 위치를 노드로 옮김
				if( pNodeAbil->idNode == m_idSelectedNode ) {
					vSelectedNode = pExist->GetPosLocal();
				}
			}
		} END_LOOP;
		// 현재 선택한 특성의 선택 글로우.
		auto pImgGlow = SafeCast<XWndImage*>( pRootTree->Find( "img.selected.node" ) );
		if( pImgGlow == nullptr ) {
			pImgGlow = new XWndImage( PATH_UI( "common_bg_item_glow.png" ), 0, 0 );
// 			pImgGlow->SetScaleLocal( 0.691f );
			pImgGlow->SetScaleLocal( 0.75f );
			pImgGlow->SetblendFunc( XE::xBF_ADD );
			pImgGlow->SetbShow( false );
			pImgGlow->SetstrIdentifier( "img.selected.node" );
			pRootTree->Add( pImgGlow );
		}
		if( pImgGlow && !vSelectedNode.IsZero() ) {
			pImgGlow->SetbShow( true );
			pImgGlow->SetPosLocal( vSelectedNode - XE::VEC2(3,3) );
		}
// 		{
// 			asdfasdf
// 			// 연구중인 특성은 글로우를 항상 깜빡이도록 한다.
// 			const auto& research = ACCOUNT->GetResearching();
// 			if( research.IsResearching() ) {
// 				XWndImage::sUpdateCtrl( pRootTree, )
// 			}
// 		}
	} // if( pRootTree ) {
}

/**
 @brief 상단 유닛 버튼들.
*/
void XSceneTech::UpdateUnitButtons( void )
{

}
/**
 @brief 
*/
int XSceneTech::OnClickHero( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickHero:snHero=0x%08x", p2 );
	//
	ID snHero = p2;
	auto pHero = ACCOUNT->GetHero( snHero );
	if( pHero ) {
		CONSOLE( "OnClickHero:snHero=%d", pHero->GetidProp() );
		// 기존에 선택되었던건 해제시킴
		auto pExist = SafeCast2<XWndStoragyItemElem*>( Find( m_snSelectedHero ) );
		if( pExist ) {
			pExist->SetbSelected( false );
		}
		// 선택한 영웅의 elem을 찾아 선택시킴.
		auto pWndElemHero = SafeCast2<XWndStoragyItemElem*>( Find( pHero->GetsnHero() ) );
		if( pWndElemHero ) {
			pWndElemHero->SetbSelected( true );
			m_snSelectedHero = snHero;
			m_unitSelected = pHero->GetUnit();	// 현재 장착된 유닛을 자동으로 선택해줌.
			m_idSelectedNode = 0;
			XWnd *pRootTree = Find( "wnd.tree.root" );
			if( pRootTree )
					pRootTree->DestroyChildAll();
			SetbUpdate( true );
		}
	}
	// 이제 영웅클래스에 특성트리 옮기기 작업.	
	return 1;
}
/**
 @brief 
*/
int XSceneTech::Process( float dt ) 
{ 
#ifdef _xIN_TOOL
	if( XBaseTool::sGetCurrMode() == xTM_TECH ) {
		if( m_timerAutoSave.IsOver() ) {
			// text_ko 재저장.
			TEXT_TBL->Save( _T( "text_ko.txt" ) );
			XPropTech::sGet()->Save( _T( "propTech.xml" ) );
			m_timerAutoSave.Off();
		}
	}
#endif
	// 연구중일때 그래프와 시간갱신
	// 연구중인 특성이 있고 현재 선택된 특성이 연구중일때.
	const XGAME::xResearch& research = ACCOUNT->GetResearching();

	if( research.IsResearching() 
			&& m_snSelectedHero == research.GetsnHero() ) {		// 연구중인 영웅을 선택했을때만 그래프가 나옴.
		auto pHero = ACCOUNT->GetHero( research.GetsnHero() );
		auto pWnd = Find("pbar.research");
		if( pWnd ) {
			auto pBar = dynamic_cast<XWndProgressBar*>( pWnd );
			if( pBar ) {
				pBar->SetbShow( true );
				auto& costAbil = pHero->GetCostAbilCurr();
				// 연구시작으로부터 지나간 시간.
				DWORD secPass, secRemain;
				_tstring strRemain;
				GetRemainResearchTime( &strRemain, costAbil.sec, &secPass, &secRemain );
				double lerp = (double)secPass / costAbil.sec;
				if( lerp < 0 )
					lerp = 0;
				if( lerp > 1.0 )
					lerp = 1.0;
				pBar->SetLerp( (float)lerp );
				xSET_TEXT( pBar, "text.remain.time", strRemain.c_str() );
				//
				auto pText = xGET_TEXT_CTRL( this, "text.cost.complete" );
				if( pText ) {
					// 72초당 1개꼴
					xSec secRemain = (xSec)(costAbil.sec - (int)secPass);
					int cost = ACCOUNT->GetCashResearch( secRemain );
					pText->SetText( XE::NumberToMoneyString(cost) );
				}
			}
		}
	}
	// 글로우 효과에 쓰는 공통 타이머
	{
		if( m_timerGlow.IsOff() )
			m_timerGlow.Set( 1.f );
		if( m_timerGlow.IsOver() )
			m_timerGlow.Reset();
		float lerpTime = m_timerGlow.GetSlerp();
//		float alpha = XE::xiCos( lerpTime, 0.5f, 1.f, 0.f );
		float alpha = 1.f;
		//
		if( m_pHeroSected ) {
			for( int i = 1; i < XGAME::xUNIT_MAX; ++i ) {
				auto unit = ( XGAME::xtUnit )i;
				auto pButt = Findf( "butt.%d", i );
				if( pButt ) {
					auto pImg = pButt->Find( "img.glow" );
					if( pImg ) {
						if( m_unitSelected == unit ) {
							pImg->SetbShow( true );
							pImg->SetAlphaLocal( alpha );
						}
						else { // m_unitSelected
							// 선택된 유닛이 아닐땐 끔
							pImg->SetbShow( false );
						}
					} // pImg
				} // pButt
			} // for
		} // if( m_pHeroSected ) {
// 		XWnd *pRootTree = Find( "wnd.tree.root" );
// 		if( pRootTree ) {
// 			// 현재 선택된 특성의 글로우의 알파애니메이션
// 			auto pImgGlow = SafeCast<XWndImage*>(  pRootTree->Find( "img.selected.node" ) );
// 			if( pImgGlow ) 
// 				pImgGlow->SetAlphaLocal( alpha );
// 		}
	}
	return XEBaseScene::Process( dt );
}

/**
 @brief 
 @param secTotal 총 연구시간
*/
void XSceneTech::GetRemainResearchTime( _tstring *pOutStr, 
//																				XPropTech::xNodeAbil *pProp, 
																				int secTotal,
																				DWORD *pOutsecPass, 
																				DWORD *pOutsecRemain )
{
	const auto& research = ACCOUNT->GetResearching();
	XBREAK( research.IsResearching() == false );
	// 연구시작으로부터 지나간 시간.
	xSec secPass = research.GetsecPass();
	if( pOutsecPass )
		*pOutsecPass = secPass;
	int secRemain = secTotal - (int)secPass;
	if( secRemain < 0 )
		secRemain = 0;
	if( pOutsecRemain )
		*pOutsecRemain = (int)secRemain;
	*pOutStr += _sGetResearchTime(secRemain);
}

void XSceneTech::sGetRemainResearchTime( _tstring *pOutStr, 
										xSec secStart, 
										int secTotal,
										DWORD *pOutsecPass, 
										DWORD *pOutsecRemain )
{
	XBREAK( secStart == 0 );
	XBREAK( secTotal == 0 );
	// 연구시작으로부터 지나간 시간.
	int secPass = (int)(XTimer2::sGetTime() - secStart);
	if( pOutsecPass )
		*pOutsecPass = (DWORD)secPass;
	if( secPass < 0 )
		secPass = 0;
	int secRemain = secTotal - secPass;	// 총연구시간에서 흘러간시간을 뺌
	if( secRemain < 0 )
		secRemain = 0;
	if( pOutsecRemain )
		*pOutsecRemain = (DWORD)secRemain;	// 연구 남은 시간
	*pOutStr += _sGetResearchTime( secRemain );
}

//
void XSceneTech::Draw( void ) 
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
// #ifdef _CHEAT
// 	if( XAPP->m_bDebugMode ) {
// 		XE::VEC2 v(2,23);
// 		PUT_STRINGF(v.x, v.y, XCOLOR_WHITE, "%d", ACCOUNT->GetResource( XGAME::xRES_WOOD) );	v.y += 11.f;
// 		PUT_STRINGF(v.x, v.y, XCOLOR_WHITE, "%d", ACCOUNT->GetResource( XGAME::xRES_IRON ) );	v.y += 11.f;
// 		PUT_STRINGF(v.x, v.y, XCOLOR_WHITE, "%d", ACCOUNT->GetResource( XGAME::xRES_JEWEL ) );	v.y += 11.f;
// 		PUT_STRINGF(v.x, v.y, XCOLOR_WHITE, "%d", ACCOUNT->GetResource( XGAME::xRES_SULFUR ) );	v.y += 11.f;
// 		PUT_STRINGF(v.x, v.y, XCOLOR_WHITE, "%d", ACCOUNT->GetResource( XGAME::xRES_MANDRAKE ) );	v.y += 11.f;
// 	}
// #endif // cheat
#ifdef _xIN_TOOL
	if( XBaseTool::sGetCurrMode() == xTM_TECH ) {
		XE::POINT vLT(126,76);
		XE::POINT vSize(252,266);
		GRAPHICS->DrawRectSize( vLT, vSize, XCOLOR_GRAY );
		for( int i = vLT.y; i <= vLT.y+vSize.h; i += SIZE_GRID ) {
			GRAPHICS->DrawHLine( vLT.x, i, vSize.w, XCOLOR_GRAY );
		}
		for( int i = vLT.x; i <= vLT.x+vSize.w; i += SIZE_GRID ) {
			GRAPHICS->DrawVLine( i, vLT.y, vSize.h, XCOLOR_GRAY );
		}
		///< ldown한 노드가 있고 마우스가 drag상태면 화살표를 그린다
		if( m_modeTool == xMODE_LINK && m_pLDownNodeT && m_stateMouseT == 2 )		{
			XE::VEC2 v = m_pLDownNodeT->vPos;
			v.x += ICON_SIZE.w / 2.f;		// 특성노드 위쪽중간이 시작점
			if( m_pMouseOverNodeT &&
				m_pMouseOverNodeT->idNode != m_pLDownNodeT->idNode ) {
				XE::VEC2 v2 = m_pMouseOverNodeT->vPos;
				v2.x += ICON_SIZE.w / 2.f;		// 특성노드 위쪽중간이 시작점
				v2.y += ICON_SIZE.h;			// 특성노드 아래쪽중간이 끝점
				m_psfcArrow->DrawDirection( v, v2 );
			} else
				m_psfcArrow->DrawDirection( v, m_vMouse );
		}
		// 툴모드의 트리를 그린다.
		DrawTreeT();
		// 현재 마우스 커서위치의 의 그리드를 그린다.
		if( m_pLDownNodeT == nullptr ) {
			XE::VEC2 vGrid = GetPosMouseByGrid();
			GRAPHICS->DrawRectSize( vGrid, ICON_SIZE, XCOLOR_YELLOW );
		}
		if( m_pSelNodeT && m_pSelNodeT->psfcIcon ) {
			m_pSelNodeT->psfcIcon->SetScale( 0.724f );
			m_pSelNodeT->psfcIcon->SetColor( XCOLOR_RED );
			m_pSelNodeT->DrawIcon();
		} 
		if( m_pLDownNodeT && m_pLDownNodeT->psfcIcon ) {
			m_pLDownNodeT->psfcIcon->SetScale( 0.724f );
			m_pLDownNodeT->psfcIcon->SetAdjustAxisCenter();
//			m_pLDownNodeT->psfcIcon->SetScale( 1.2f );
			m_pLDownNodeT->psfcIcon->SetColor( XCOLOR_RED );
			m_pLDownNodeT->DrawIcon();
		} else
		if( m_pMouseOverNodeT && m_pMouseOverNodeT->psfcIcon ) {
			m_pMouseOverNodeT->psfcIcon->SetScale( 0.724f );
			m_pMouseOverNodeT->psfcIcon->SetColor( XCOLOR_RGBA(255,128,128,255) );
			m_pMouseOverNodeT->DrawIcon();
		}

	}
#endif // xin_tool
}

void XSceneTech::OnLButtonDown( float lx, float ly ) 
{
	m_bDrag = TRUE;
#ifdef _xIN_TOOL
	m_stateMouseT = 1;
	if( XBaseTool::sGetCurrMode() == xTM_TECH ) {
		m_pLDownNodeT = m_pMouseOverNodeT;
		if( m_pLDownNodeT )		{
			m_vLocalLDownT = m_vMouse - m_pLDownNodeT->vPos;
		}
	}
#endif // _xIN_TOOL

	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneTech::OnLButtonUp( float lx, float ly ) 
{
	m_bDrag = FALSE;
#ifdef _xIN_TOOL
	m_stateMouseT = 0;
	if( XBaseTool::sGetCurrMode() == xTM_TECH ) {
		if( m_pLDownNodeT && m_pMouseOverNodeT ) {
			ID idLDownNode = m_pLDownNodeT->idNode;
			ID idMouseOverNode = m_pMouseOverNodeT->idNode;
			if( m_modeTool == xMODE_MOVE ) {
				if( idLDownNode == idMouseOverNode )
					m_pSelNodeT = m_pLDownNodeT;
			} else
			if( m_modeTool == xMODE_LINK ) {
				if( idLDownNode != idMouseOverNode ) {
					XPropTech::xNodeAbil **pExist 
						= m_pLDownNodeT->listChild.FindpByID( idMouseOverNode );
					if( pExist == nullptr ) {
						m_pLDownNodeT->listChild.Add( m_pMouseOverNodeT );
						m_pMouseOverNodeT->GetListParentsMutable().Add( m_pLDownNodeT );
					} else {
						// 이미 차일드에 그 노드가 있으면 연결을 끊는다.
						m_pLDownNodeT->listChild.DelByID( idMouseOverNode );
						m_pMouseOverNodeT->GetListParentsMutable().DelByID( idLDownNode );
					}
				}
			}
			UpdateAutoSave();
		}
	}
	m_pLDownNodeT = nullptr;
#endif // _xIN_TOOL
	XEBaseScene::OnLButtonUp( lx, ly );
}

#ifdef _xIN_TOOL
void XSceneTech::DelOutLinkBySelectedNode()
{
	if( XBaseTool::sGetCurrMode() == xTM_TECH ) {
		if( m_pSelNodeT ) {
//			XPropTech::sGet()->DelOutLinkByNode( m_unitSelected, m_pSelNodeT->idNode );
		}
	}
}
#endif // _xIN_TOOL

void XSceneTech::OnMouseMove( float lx, float ly ) 
{
	m_vMouse.Set( lx, ly );
#ifdef _xIN_TOOL
	m_stateMouseT = 2;
//	if( XAPP->m_ToolMode == xTM_TECH  ) {
	if( XBaseTool::sGetCurrMode() == xTM_TECH ) {
		// LDown한 노드가 있을때 마우스를 움직이면 노드 이동
		if( m_pLDownNodeT ) {
			if( m_modeTool == xMODE_MOVE )
				m_pLDownNodeT->vPos = GetPosMouseByGrid( -m_vLocalLDownT );;
			UpdateAutoSave();
		}
		// 현재 마우스 위치에 노드가 있는지 검사해서 있으면 draw()시에 외곽선을 그려준다.
		m_pMouseOverNodeT = XPropTech::sGet()->GetNodeByPos( m_unitSelected, m_vMouse );
	}

#endif // _xIN_TOOL
	XEBaseScene::OnMouseMove( lx, ly );
}

int XSceneTech::OnBack( XWnd *pWnd, DWORD p1, DWORD p2 )
{
#ifdef _XSINGLE
	DoExit( XGAME::xSC_INGAME );
#else
	DoExit( XGAME::xSC_WORLD );
#endif // _XSINGLE
	return 1;
}

#ifdef _xIN_TOOL

void XSceneTech::CreateToolCtrl( void )
{
	XWnd *pRoot = Find( KEY_ROOT_TOOLVIEW );
	if( pRoot )
	{
		XBREAK( pRoot == nullptr );
		XE::VEC2 v( 592, 0 );
		XE::VEC2 size( 40, 45 );
		XWndButtonDebug *pButt = nullptr;
		pButt = new XWndButtonDebug( v, size, _T( "창병" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 1 );
		pRoot->Add( pButt );
		v.y += 40.f;
		pButt = new XWndButtonDebug( v, size, _T( "궁수" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 2 );
		pRoot->Add( pButt );
		v.y += 40.f;
		pButt = new XWndButtonDebug( v, size, _T( "팔라딘" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 3 );
		pRoot->Add( pButt );
		v.y += 40.f;
		pButt = new XWndButtonDebug( v, size, _T( "미노타" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 4 );
		pRoot->Add( pButt );
		v.y += 40.f;
		pButt = new XWndButtonDebug( v, size, _T( "사이클롭" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 5 );
		pRoot->Add( pButt );
		v.y += 40.f;
		pButt = new XWndButtonDebug( v, size, _T( "라이칸" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 6 );
		pRoot->Add( pButt );
		v.y += 40.f;
		pButt = new XWndButtonDebug( v, size, _T( "골렘" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 7 );
		pRoot->Add( pButt );
		v.y += 40.f;
		pButt = new XWndButtonDebug( v, size, _T( "앤트" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 8 );
		pRoot->Add( pButt );
		v.y += 40.f;
		pButt = new XWndButtonDebug( v, size, _T( "타천사" ) );
		pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnSelectUnitTool, 9 );
		pRoot->Add( pButt );
		v.y += 40.f;
	}
}
//////////////////////////////////////////////////////////////////////////
//  툴 코드. tool start
/**
 @brief 툴모드와 게임모드간의 전환이 이뤄질때 호출된다.
*/
// void XSceneTech::OnModeTool( xtToolMode toolMode )
// {
// 	// 툴모드로 전환
// 	if( toolMode == xTM_TECH ) {
// 		xSET_SHOW( this, KEY_ROOT_GAMEVIEW, FALSE );
// 		// 툴 UI생성
// 		m_Layout.CreateLayout( "tech_tool", this );
// 		CreateToolCtrl();
// 	} else
// 	if( toolMode == xTM_NONE ) {// 게임모드로 전환
// 		xSET_SHOW( this, KEY_ROOT_GAMEVIEW, TRUE );
// 		// 툴 UI삭제
// 		XWnd *pWnd = Find(KEY_ROOT_TOOLVIEW);
// 		if( pWnd )
// 			pWnd->SetbDestroy( TRUE );
// 	}
// }

#ifdef _xIN_TOOL
void XSceneTech::DelegateChangeToolMode( xtToolMode modeOld, xtToolMode modeCurr )
{
	XBREAK( modeOld == modeCurr );
	switch( modeOld )
	{
	case xTM_NONE: {
	} break;
	case xTM_CLOUD:
		break;
	case xTM_TECH: {
		xSET_SHOW( this, KEY_ROOT_GAMEVIEW, true );
		// 툴 UI삭제
		XWnd *pWnd = Find(KEY_ROOT_TOOLVIEW);
		if( pWnd )
			pWnd->SetbDestroy( true );
	} break;
	case xTM_SPOT: {
	} break;
	case xTM_OBJ: {
	} break;
	default:
		break;
	}
	//
	switch( modeCurr )
	{
	case xTM_NONE: {
	} break;
	case xTM_CLOUD:
		break;
	case xTM_TECH:
		xSET_SHOW( this, KEY_ROOT_GAMEVIEW, false );
		// 툴 UI생성
		m_Layout.CreateLayout( "tech_tool", this );
		CreateToolCtrl();
		break;
	case xTM_SPOT: {
	} break;
	case xTM_OBJ: {
	} break;
	default:
		break;
	}
	SetbUpdate( true );
//	UpdateUIForTool();
}
#endif // _xIN_TOOL
/**
 @brief 특성 트리를 그린다.
*/
void XSceneTech::DrawTreeT()
{
	BOOL bLoaded = FALSE;
	XArrayLinearN<XPropTech::xNodeAbil*, 1024> aryAbil;
	XPropTech::sGet()->GetNodesToAry( m_unitSelected, &aryAbil );
	// 화살표를 그린다.
	XARRAYLINEARN_LOOP( aryAbil, XPropTech::xNodeAbil*, pNodeAbil ) {
		for( auto pChild : pNodeAbil->listChild ) {
			XE::VEC2 vStart = pNodeAbil->vPos + XE::VEC2(ICON_SIZE.w/2.f, 0);
			XE::VEC2 vEnd = pChild->vPos + XE::VEC2(ICON_SIZE.w/2.f, ICON_SIZE.h );
			m_psfcArrow->DrawDirection( vStart, vEnd );
		}
	} END_LOOP;
	// 노드를 그린다.,
	XARRAYLINEARN_LOOP( aryAbil, XPropTech::xNodeAbil*, pNodeAbil ) {
		if( pNodeAbil->psfcIcon == nullptr ) {
			if( bLoaded == FALSE )
				pNodeAbil->psfcIcon = IMAGE_MNG->Load( XE::MakePath(DIR_IMG, pNodeAbil->strIcon.c_str()) );
			bLoaded = TRUE;
		} else {
			const XE::VEC2& vPos = pNodeAbil->vPos;
			pNodeAbil->psfcIcon->SetScale( 0.724f );
			pNodeAbil->psfcIcon->Draw( vPos );
			if( pNodeAbil->idName )
				PUT_STRINGF_STYLE( vPos.x, vPos.y-12.f, XCOLOR_WHITE, xFONT::xSTYLE_SHADOW, "%s", XTEXT(pNodeAbil->idName) );
			PUT_STRINGF_STYLE( vPos.x, 
								vPos.y + ICON_SIZE.h - 22.f, 
								XCOLOR_WHITE, 
								xFONT::xSTYLE_SHADOW, 
								"max:%d tier:%d lvOpen:%d", pNodeAbil->maxPoint, pNodeAbil->tier, pNodeAbil->GetLvOpanable() );
		}
	} END_LOOP;
}

/**
 @brief 선택된 노드를 삭제시킨다.
*/
void XSceneTech::DelNodeSelected()
{
	if( m_pSelNodeT )	{
		SAFE_DELETE( m_pDeletedNodeT );		// 그 이전에 다른 포인터가 있었다면 삭제시킴(언두불가능)
		m_pDeletedNodeT = m_pSelNodeT;		// 언두를 위해 백업.
		XPropTech::sGet()->DelNode( m_pSelNodeT->idNode );
		m_pSelNodeT = nullptr;
	}
}

/**
 @brief 삭제 언두와 링크언두만 간단하게 지원함.
*/
void XSceneTech::Undo( void )
{
	if( m_pDeletedNodeT )
	{
		XPropTech::xNodeAbil *pNode = m_pDeletedNodeT;
		// 부모노드에게 이 노드를 다시 붙임.
		for( auto pParent : pNode->GetListParents() )
		{
			pParent->listChild.Add( pNode );
		}
		// 자식노드들에게 이 노드를 다시 부모로 붙임.
		for( auto pChild : pNode->listChild )
		{
			pChild->GetListParentsMutable().Add( pNode );
		}
		// 다시 복구시킴.
		XPropTech::sGet()->Add( m_unitSelected, pNode );
		m_pDeletedNodeT = nullptr;
		UpdateAutoSave();
	}
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnSelectUnitTool( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnSelectUnitTool");
	//
	XGAME::xtUnit unit = (XGAME::xtUnit) p1;
	///< 
	m_unitSelected = unit;
	SAFE_DELETE( m_pDeletedNodeT );
	m_pSelNodeT = nullptr;
	m_pLDownNodeT = nullptr;
	m_pMouseOverNodeT = nullptr;
	return 1;
}

void XSceneTech::DoCreateNewNode( XPropTech::xNodeAbil *pNewNode )
{
	XPropTech::sGet()->Add( m_unitSelected, pNewNode );
}

BOOL XSceneTech::OnKeyDown( int keyCode )
{
	if( m_pSelNodeT ) {
		auto vPos = m_pSelNodeT->vPos;
		switch( keyCode )	{
		case XE::KEY_LEFT: {
			vPos.x -= (float)SIZE_GRID;
		} break;
		case XE::KEY_RIGHT: {
			vPos.x += (float)SIZE_GRID;
		} break;
		case XE::KEY_UP: {
			vPos.y -= (float)SIZE_GRID;
		} break;
		case XE::KEY_DOWN: {
			vPos.y += (float)SIZE_GRID;
		} break;
		}
		vPos = GetPosByGrid( vPos );
		m_pSelNodeT->vPos = vPos;
	}
	return FALSE;
}

// 툴코드 tool end
//////////////////////////////////////////////////////////////////////////
#endif // _xIN_TOOL

/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnClickNode( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	ID idNode = p1;
	CONSOLE("OnClickNode:id=%d", idNode );
	//
	auto pHero = ACCOUNT->GetHero( m_snSelectedHero );
	if( pHero ) {
		const auto abil = pHero->GetAbilNode( m_unitSelected, idNode );
//		XBREAK( pAbil == nullptr );
		m_idSelectedNode = idNode;
		auto pProp = XPropTech::sGet()->GetpNode( m_unitSelected, idNode );
		XBREAK( pProp == nullptr );
	}
	SetbUpdate( TRUE );
	return 1;
}

// _tstring XSceneTech::sGetResearchTime( ID idAbil )
// {
// 
// }

/**
 @brief 연구시간 문자열을 생성한다.
 @deprecated 이거 사용하지 말고 XGAME::GetstrResearchTime를 사용할것.
*/
_tstring XSceneTech::_sGetResearchTime( int sec )
{
	XBREAK( sec < 0 );
	int d, h, m, s;
	XSYSTEM::GetDayHourMinSec( (xSec)sec, &d, &h, &m, &s );
	_tstring str;// = _T( "연구시간:" );
// 	str += XFORMAT("%d-", sec );
	if( d > 0 )
		str += XFORMAT( "%d%s", d, XTEXT(2230) );
	if( h > 0 )
		str += XFORMAT( " %d%s", h, XTEXT(2231) );
	// '일'이 없을때만 분/초을 표시한다.
	if( d <= 0 )
	{
		if( m > 0 )
			str += XFORMAT( " %d%s", m, XTEXT(2232) );
		// 시간이 없을때만 초를 표시한다.
		if( s >= 0 && h <= 0 )
			str += XFORMAT( " %d%s", s, XTEXT(2233) );
	}
	return str;
}
/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnClickPlus( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickPlus");
	//
	if( m_idSelectedNode == 0 )
		return 1;
	auto pHero = ACCOUNT->GetHero( m_snSelectedHero );
	if( XBREAK(pHero == nullptr) ) 
		return 1;
//	const auto abil = pHero->GetAbilNode( m_unitSelected, m_idSelectedNode );
//	XBREAK( pAbil == nullptr );
// 	auto pProp = XPropTech::sGet()->GetpNode( m_unitSelected, m_idSelectedNode );
// 	XBREAK( pProp == nullptr );
	// 남은 특성포인트
	const int numRemainPoint = pHero->GetnumRemainAbilPoint();
	if( numRemainPoint > 0 ) {
		// 특성포인트가 있으면 연구중이라도 바로 올릴수 있다.(현재 연구중엔 +버튼이 아예사라짐)
		auto pAlert = new XGameWndAlert( XTEXT(2278), nullptr, XWnd::xOKCANCEL );
		if( pAlert ) {
			pAlert->SetEvent( XWM_OK, this, &XSceneTech::OnOkResearch, 1 );
			Add( pAlert );
			pAlert->SetbModal( TRUE );
		}
	} else {
		auto& research = ACCOUNT->GetResearching();
		if( research.GetidAbil() == 0 ) {
			// 현재 연구중이 아닐때.
			const auto& costAbil = pHero->GetCostAbilCurr();
			int point = pHero->GetNumAbilPoint( m_unitSelected, m_idSelectedNode );
			auto pPopup = new XWndResearchConfirm( m_unitSelected, m_idSelectedNode, point + 1, costAbil );
			pPopup->SetEvent( XWM_OK, this, &XSceneTech::OnOkResearch, 0 );
// 			if( !ACCOUNT->IsEnoughResourceForResearch( pHero ) ) {
// 				xSET_ENABLE( pPopup, "butt.ok", false );
// 			}
			Add( pPopup );
		} else {
			auto pAlert = new XGameWndAlert( XTEXT(2028), nullptr, XWnd::xOK );	// 현재 다른특성을 연구중
			if( pAlert ) {
				Add( pAlert );
				pAlert->SetbModal( TRUE );
			}
		}
	}
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnClickLockFree( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickLockFree");
	//
	auto pHero = ACCOUNT->GetHero( m_snSelectedHero );
	if( XBREAK( pHero == nullptr ) )
		return 1;
	if( ACCOUNT->IsLockUnit( m_unitSelected) ) {
		XWND_ALERT("%s", XTEXT(2109) );		// 유닛잠금을 풀어야 함.
		return 1;
	}
	auto err = ACCOUNT->GetUnlockableAbil( pHero, m_unitSelected, m_idSelectedNode );
	if( err == XGAME::xE_NOT_ENOUGH_GOLD ) {
		XWND_ALERT( "%s", XTEXT( 2143 ) );		// 금화부족
		return 1;
	}
//	int goldUnlock = pHero->GetGoldUnlockAbilCurr( /*m_unitSelected*/ );
//	if( ACCOUNT->GetGold() >= gold )
//	if( ACCOUNT->IsEnoughGold( goldUnlock ) )
	GAMESVR_SOCKET->SendReqAbilLockFree( GAME, pHero->GetsnHero(), m_unitSelected, m_idSelectedNode );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnOkResearch( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnOkResearch");
	//
	auto pHero = ACCOUNT->GetHero( m_snSelectedHero );
	if( XBREAK( pHero == nullptr ) )
		return 1;
	const auto abil = pHero->GetAbilNode( m_unitSelected, m_idSelectedNode );
	auto pProp = XPropTech::sGet()->GetpNode( m_unitSelected, m_idSelectedNode );
	XBREAK( pProp == nullptr );
	bool bByRemainPoint = (p1 == 1);
	if( bByRemainPoint ) {
		GAMESVR_SOCKET->SendReqResearch( this, pHero->GetsnHero()
																		, m_unitSelected, pProp->idNode, bByRemainPoint );
	} else {
		if( abil.point < pProp->maxPoint ) {
			GAMESVR_SOCKET->SendReqResearch( this, pHero->GetsnHero()
																			, m_unitSelected, pProp->idNode, bByRemainPoint );
		}
	}
		
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnClickUnit( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	XGAME::xtUnit unit = (XGAME::xtUnit) p1;
	CONSOLE("OnClickUnit:%s", XGAME::GetStrUnit( unit ) );
	//
	if( ACCOUNT->IsLockUnit( unit ) ) {
// 		int lvUnlock = ACCOUNT->GetLevelUnlockableUnit( unit );
// 		bool bUnlockable = ( lvUnlock == 0 );
		bool bUnlockable = ACCOUNT->IsUnlockableUnit( unit );
		if( bUnlockable ) {
			auto pHero = ACCOUNT->GetHero( m_snSelectedHero );
			if( XASSERT(pHero) ) {
			auto pPopup = new XWndUnitinfo( unit, XTEXT( 2108 ) );
//				auto pPopup = new XWndUnitinfo( pHero, XTEXT(2108) );
				xSET_SHOW( pPopup, "butt.close", false );
				xSET_SHOW( pPopup, "butt.cancel", true );
				auto pButt = xSET_SHOW( pPopup, "butt.unlock", true );
				pButt->SetEvent( XWM_CLICKED, this, &XSceneTech::OnClickUnlockUnit, (DWORD)unit );
				Add( pPopup );
			}
		}
	}
	if( m_unitSelected != unit || m_idSelectedNode == 0 ) {
		m_unitSelected = unit;
		XWnd *pWnd = Find(KEY_SCRL_VIEW);
		if( pWnd )
			pWnd->SetbDestroy( TRUE );
		XArrayLinearN <XPropTech::xNodeAbil*,128> ary;
		XPropTech::sGet()->GetNodesByTierToAry( m_unitSelected, 1, &ary );
		if( ary.size() > 0 )
			m_idSelectedNode = ary[0]->idNode;
//		m_idSelectedNode = 0;
//		UpdateTreeNode( Find(KEY_ROOT_GAMEVIEW) );
		m_bUpdateSort = true;		// 왼쪽 영웅리스트를 다시 정렬하라.
		SetbUpdate( TRUE );
	}
	XWnd *pRootTree = Find( "wnd.tree.root" );
	if( pRootTree )
		pRootTree->DestroyChildAll();
	return 1;
}

void XSceneTech::RecvResearchComplete( XPropTech::xNodeAbil *pProp, const XGAME::xAbil& abil )
{
// 	if( ACCOUNT->GetResearching().GetidAbil() == 0 )
// 		m_pResearchAbil = nullptr;
}
#ifdef _CHEAT
/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnDebug( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnDebug");
	//
	switch( p1 )
	{
	case 1:
		DoExit( XGAME::xSC_INGAME );
	default:
		break;
	}

	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnCheat( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnCheat");
	//
	int type = (int)p1;
	if( type == 200 )
	{
		const int add = 50000;
		GAMESVR_SOCKET->SendCheat( this, type, add );
//		ACCOUNT->AddBrave( add );
		ACCOUNT->AddGold( add );
	}
	if( SCENE_TECH )
		SCENE_TECH->SetbUpdate( true );
	return 1;
}

#endif // cheat

/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnClickResearchComplete( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickResearchComplete");
	//
	auto& research = ACCOUNT->GetResearching();
	auto pHero = ACCOUNT->GetHero( research.GetsnHero() );
	if( XASSERT( pHero ) ) {
		GAMESVR_SOCKET->SendReqResearchCompleteNow( GAME, pHero, research.GetidAbil() );
	}
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneTech::OnClickUnlockUnit( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	auto unit = (XGAME::xtUnit)p1;
	CONSOLE("OnClickUnlockUnit:%d", unit);
	pWnd->SetbEnable( FALSE );
	//
	GAMESVR_SOCKET->SendReqUnlockUnit( GAME, unit );
	return 1;
}

/**
 @brief 특성초기화 버튼 누름.
 특성초기화를 하겠습니까? 확인 창
*/
int XSceneTech::OnClickInitAbil( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickInitAbil");

	//
	if( m_snSelectedHero ) {
		const XGAME::xResearch& research = ACCOUNT->GetResearching();
		auto pHero = ACCOUNT->GetpHeroBySN( m_snSelectedHero );
		if( pHero ) {
			bool bResearching = ( research.IsResearching() && research.GetsnHero() == m_snSelectedHero );
			if( bResearching ) {
				// 현재 이 영웅이 연구중이면 리셋 못함.
				XWND_ALERT("%s", XTEXT(2343));		// 연구중엔 안됨.
				return 1;
			}
			if( pHero->GetnumInitAbil() >= 255 ) {
				XWND_ALERT( "%s", XTEXT(2280) );		// 더이상 초기화 못함.
			} else {
				const int numGetPoint = pHero->GetNumSetAbilPoint();	// 돌려받을 포인트
				const int gold = pHero->GetCostInitAbil();		// 초기화에 필요한 비용
				// 특성포인트를 초기화?
				const _tstring strCost = (gold)? XE::NtS(gold) : XTEXT(2121);		// 2121=무료
				_tstring str;
				if( XE::LANG.GetstrKey() == _T("english") ) {
					str = XE::Format( XTEXT( 2279 ), numGetPoint
																				, pHero->GetstrName().c_str()
																				, strCost.c_str() );
				} else {
					str = XE::Format( XTEXT( 2279 ), pHero->GetstrName().c_str()
																				, numGetPoint
																				, strCost.c_str() );
				}
				auto pAlert = new XGameWndAlert( str, nullptr, XWnd::xOKCANCEL );
				if( pAlert ) {
					pAlert->SetEvent( XWM_OK, this, &XSceneTech::OnOkInitAbil );
					auto pButtOk = pAlert->GetButtOk();
					if( pButtOk ) {
						pButtOk->SetbEnable( ACCOUNT->IsEnoughGold( gold ) );
					}
					Add( pAlert );
					pAlert->SetbModal( TRUE );
				}
			}
		}
	}
	return 1;
}

/**
 @brief 특성초기화 확인창에서 Ok누름
*/
int XSceneTech::OnOkInitAbil( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnOkInitAbil");
	//
	if( m_snSelectedHero )
		GAMESVR_SOCKET->SendReqInitAbil( GAME, m_snSelectedHero );
	
	return 1;
}

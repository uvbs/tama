#include "StdAfx.h"
#include "XSceneLegion.h"
#include "XGame.h"
#include "XGameWnd.h"
#include "XSockGameSvr.h"
#include "XSkillMng.h"
#include "XExpTableUser.h"
#include "XPropHelp.h"
#include "XSquadron.h"
#include "XLegion.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndButton.h"
#include "skill/XSkillDat.h"
#include "XHero.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneLegion *SCENE_LEGION = NULL;

void XSceneLegion::Destroy() 
{	
	SAFE_DELETE(m_pLegion);
	XBREAK( SCENE_LEGION == NULL );
	XBREAK( SCENE_LEGION != this );
	SCENE_LEGION = NULL;
}

XSceneLegion::XSceneLegion( XGame *pGame ) 
	: XSceneBase( pGame, XGAME::xSC_LEGION )
	, m_Layout(_T("layout_legion.xml"))
{ 
	XBREAK( SCENE_LEGION != NULL );
	SCENE_LEGION = this;
	Init(); 
//	SetPosLocal( 0.f, 78.f );
//	SetSizeLocal( XE::GetGameWidth(), 
//		GRAPHICS->GetLogicalScreenSize().h - (GetPosLocal().h + 66.f) );
// 	m_Layout.CreateLayout( new XAppLayout(_T("layout_legion.xml")), 
// 											"legion", this );
	m_Layout.CreateLayout( "legion", this );

	//xSET_BUTT_HANDLER(this, "butt.tab.decide", &XSceneLegion::OnClickDecide);
	xSET_BUTT_HANDLER(this, "butt.back", &XSceneLegion::OnBack);
	xSetButtHander( this, this, "butt.info", &XSceneLegion::OnClickLegionInfo );

	// 저장하기 전까지 사용할 임시 군단 객체를 만든다
	//m_pLegion = new XLegion(*ACCOUNT->GetCurrLegion().get());
	CopyLegionObj();

	XWndList *pWndList = SafeCast<XWndList*, XWnd*>(Find("list.inven.hero"));
	if( pWndList )
	{
//		XList<XSPHero> listHero;
//		ACCOUNT->GetInvenHero( listHero );
		// 현재 구단에 배정된 장군을 제외한 장군리스트를 얻는다.
		XArrayLinearN<XSPHero, 1024> ary;
		ACCOUNT->GetHerosListExceptLegion(&ary, m_pLegion);
		XARRAYLINEARN_LOOP_IDX( ary, XSPHero, i, pHero )		{
			auto pElem = new XWndInvenHeroElem(pHero, m_pLegion);
			pElem->SetUnitFace();
			pWndList->AddItem( pHero->GetsnHero(), pElem );
		} END_LOOP;
		pWndList->SetScrollVertOnly();
		pWndList->SetDragWnd( TRUE );	// drag&drop을 위한.
		pWndList->SetDropWnd( TRUE );
		pWndList->SetEvent( XWM_DROP, this, &XSceneLegion::OnDropSquad );
	}
	m_pHeroList = pWndList;
	{
		/*LegionPtr spLegion = LegionPtr(m_pLegion);
		XBREAK( spLegion == nullptr );*/
		XE::VEC2 vStart( 472, 66 );
		XE::VEC2 v = vStart;
		// 우측 군단 슬롯
		for( int i = 0; i < 3; ++i )	{
			for( int k = 0; k < 5; ++k )	{
				v.x = vStart.x + -98.f * i;
				v.y = vStart.y + 57.f * k;
				int idx = ( i * 5 + k );
				auto pSquad = m_pLegion->GetpSquadronByidxPos( idx );
				XSPHero pHero = NULL;
				if( pSquad )
					pHero = pSquad->GetpHero();
				auto pWnd = new XWndSquadInLegion( pHero, v, m_pLegion );
				ID idWnd = 100 + idx;
				pWnd->SetEvent( XWM_CLICKED, this, &XSceneLegion::OnClickSquad, idWnd );
				pWnd->SetEvent( XWM_DROP, this, &XSceneLegion::OnDropSquad );
				Add( idWnd, pWnd );
			}
		}
	}
	SetbUpdate( TRUE );
}

void XSceneLegion::Create( void )
{
	XSceneBase::Create();
}

void XSceneLegion::Update()
{
	if (m_pLegion)
	{
		/*LegionPtr spLegion = LegionPtr(m_pLegion);
		if (XBREAK(m_pLegion == nullptr))
		return;*/
		int num = m_pLegion->GetNumSquadrons();
		int numMax = XAccount::sGetMaxSquadByLevelForPlayer(ACCOUNT->GetLevel());
		auto pText = xSET_TEXT(this, "text.num.squad", XFORMAT("%d / %d", num, numMax));
		if (pText) {
			if (num == numMax) {
				pText->SetColorText(XCOLOR_WHITE);
				pText->GetcompMngByAlpha().SetbActive( false );
			} else {
				pText->SetColorText(XCOLOR_GREEN);
				pText->GetcompMngByAlpha().SetbActive( true );
				if( pText->GetcompMngByAlpha().FindComponentByFunc("alpha") == nullptr )
					pText->GetcompMngByAlpha().AddComponent( new XECompWave("alpha", nullptr, 0.f, 0.5f) );
			}

		}
	}
	XSceneBase::Update();
}

int XSceneLegion::Process( float dt ) 
{ 
	return XSceneBase::Process( dt );
}

//
void XSceneLegion::Draw( void ) 
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneLegion::OnLButtonDown( float lx, float ly ) 
{
	XSceneBase::OnLButtonDown( lx, ly );
}
void XSceneLegion::OnLButtonUp( float lx, float ly ) {
	XSceneBase::OnLButtonUp( lx, ly );
}
void XSceneLegion::OnMouseMove( float lx, float ly ) {
	XSceneBase::OnMouseMove( lx, ly );
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneLegion::OnBack( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnBack");
	//
	GAMESVR_SOCKET->SendReqChangeSquad(this, ACCOUNT->GetCurrLegionIdx(), m_pLegion);
	//DoExit(XGAME::xSC_UNIT_ORG);
	return 1;
}


/****************************************************************
* @brief 
*****************************************************************/
int XSceneLegion::OnClickSquad( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	ID idWnd = p1;
	CONSOLE( "OnClickSquad:%d", idWnd );

	XWndSquadInLegion *pElem = SafeCast<XWndSquadInLegion*, XWnd*>(pWnd);
	if (pElem && pElem->GetpHero())
	{
		XSPHero pHero = pElem->GetpHero();
		XWnd *pPopup = new XWndPopup(m_Layout.GetpLayout(), "popup_hero_detail", nullptr);
		pPopup->SetbModal(TRUE);
		Add(pPopup);

		for (int j = 1; j <= pHero->GetGrade(); ++j)
		{
			char key[256];
			sprintf_s(key, "img.star%d", j);
			xSET_SHOW(this, key, TRUE);
		}

		if (m_pLegion->GetpLeader() == pHero)
		{
			xSET_SHOW(this, "butt.leader", FALSE);
			XWnd *pButt = Find("butt.close");
			if (pButt)
				pButt->SetPosLocal(XE::VEC2(215, pButt->GetPosLocal().y));
		}

		const bool bShow = true;
		xSET_TEXT(this, "text.hero.melee", XE::Format(_T("%.0f%%"), pHero->GetAttackMeleeRatio( bShow ) ));
		xSET_TEXT(this, "text.hero.range", XE::Format(_T("%.0f%%"), pHero->GetAttackRangeRatio( bShow ) ));
		xSET_TEXT(this, "text.hero.def", XE::Format(_T("%.0f%%"), pHero->GetDefenseRatio( bShow ) ));
		xSET_TEXT(this, "text.hero.hp", XE::Format(_T("%.0f%%"), pHero->GetHpMaxRatio( bShow ) ));
		xSET_TEXT(this, "text.hero.atk.speed", XE::Format(_T("%.0f%%"), pHero->GetAttackSpeed( bShow ) ));
		xSET_TEXT(this, "text.hero.move.speed", XE::Format(_T("%.0f%%"), pHero->GetMoveSpeed( bShow ) ));

		xSET_TEXT(this, "text.hero.name", XE::Format(XTEXT(80047), pHero->GetLevel(), pHero->GetpProp()->GetstrName().c_str()));
		xSET_IMG(this, "img.icon.hero", XE::MakePath(DIR_IMG, pHero->GetpProp()->strFace.c_str()));

		XSKILL::XSkillDat* pSkill = SKILL_MNG->FindByIds(pHero->GetpProp()->strActive.c_str());
		XWndImage* pImg = SafeCast<XWndImage*, XWnd*>(Find("img.icon.active"));
		if (pImg)		{
			if (pSkill)			{
				pImg->SetSurface(XE::MakePath(DIR_IMG, pSkill->GetstrIcon()));
				xSET_SHOW(this, "img.icon.active.bg", TRUE);
				_tstring strDesc;
				pSkill->GetSkillDesc(&strDesc, pHero->GetlvActive());
				xSET_TEXT(this, "text.active.desc", strDesc.c_str());
				xSET_TEXT(this, "text.active.name", pSkill->GetSkillName());
				// 		XWndTextString *pText = new XWndTextString( 71, 14, strDesc.c_str() );
				// 		pText->SetStyle( xFONT::xSTYLE_SHADOW );
				// 		Add( pText );
			}
			else
			{
				pImg->SetSurfacePtr(NULL);
				xSET_SHOW(this, "img.icon.active.bg", FALSE);
				xSET_TEXT(this, "text.active.desc", _T(""));
				xSET_TEXT(this, "text.active.name", _T(""));
				xSET_SHOW(this, "text.active.none", TRUE);
				xSET_SHOW(this, "butt.upgrade.skill1", FALSE);
			}
		}

		pSkill = SKILL_MNG->FindByIds(pHero->GetpProp()->strPassive.c_str());
		pImg = SafeCast<XWndImage*, XWnd*>(Find("img.icon.passive"));
		if (pImg)
		{
			if (pSkill)
			{
				pImg->SetSurface(XE::MakePath(DIR_IMG, pSkill->GetstrIcon()));
				xSET_SHOW(this, "img.icon.passive.bg", TRUE);
				_tstring strDesc;
				pSkill->GetSkillDesc(&strDesc, pHero->GetlvPassive());
				xSET_TEXT(this, "text.passive.desc", strDesc.c_str());
				xSET_TEXT(this, "text.passive.name", pSkill->GetSkillName());
			}
			else
			{
				pImg->SetSurfacePtr(NULL);
				xSET_SHOW(this, "img.icon.passive.bg", FALSE);
				xSET_TEXT(this, "text.passive.desc", _T(""));
				xSET_TEXT(this, "text.passive.name", _T(""));
				xSET_SHOW(this, "text.passive.none", TRUE);
				xSET_SHOW(this, "butt.upgrade.skill2", FALSE);
			}
		}

		xSET_BUTT_HANDLER_PARAM(this, "butt.leader", this, &XSceneLegion::OnClickSetLeader, pHero->GetsnHero());
		xSET_BUTT_HANDLER(this, "butt.close", &XSceneLegion::OnClickPopupClose);
	}
	return 1;
}

/****************************************************************
* @brief 부대 슬롯 위에서 드롭했다.
* @param pWnd drop된 슬롯객체
* @param p2 drag했던 객체 아이디
*****************************************************************/
int XSceneLegion::OnDropSquad( XWnd* pDropWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnDropSquad");
	if( XTimeoutMng::sGet()->IsRequesting() )
		return 1;
	/*LegionPtr spLegion = ACCOUNT->GetCurrLegion();
	if( XBREAK(spLegion == nullptr) )
	return 0;*/
	//int idxLegion = ACCOUNT->GetCurrLegionIdx();
	XWnd *pDragWnd = Find(p2);
	if( pDragWnd ) {
		// pDragWnd는 어떤타입일지 알수 없으므로 dynamic_cast로 변환시켜본다.
		auto pHeroElem = dynamic_cast<XWndInvenHeroElem*>( pDragWnd );
		if( pHeroElem ) {
			// 왼쪽 영웅리스트에서 드래그해서 슬롯에 드랍함.
			XSPHero pHero = pHeroElem->GetpHero();
			XBREAK( pHero == NULL );
			auto pSlot = dynamic_cast<XWndSquadInLegion*>( pDropWnd );
			if( pSlot ) {
				if( IsAbleAddHero() ) {
					int idx = pDropWnd->GetID() - 100;
					// 서버로 드래그&드랍 정보 보냄. 결과 받으면 클라도 변환.
					CreateSquadToLegion(pHero, m_pLegion, idx);
					GAME->DispatchEventToSeq( xHelp::xHE_DRAGGED_SLOT );
				} else {
					XWND_ALERT( "%s", XTEXT(2296) );		// 더이상 추가못함.
				}
			}
		} else {
			// 군단슬롯에서 드래그 시작함.
			auto pSlotSrc = dynamic_cast<XWndSquadInLegion*>( pDragWnd );
			XBREAK( pSlotSrc == NULL );	// 이걸로도 변환이 안되면 에러다.
			if( pSlotSrc && pSlotSrc->GetpHero() ) {
				int idxSrc = pSlotSrc->getid() - 100;
				XWndList *pDropWndList = dynamic_cast<XWndList*>( pDropWnd );
				if( pDropWndList ) {
					// 왼쪽 영웅리스트로 드랍함(슬롯빼기)
					MoveSquadInLegion(idxSrc, -1, pSlotSrc->GetpHero()->GetsnHero(), 
										0 );
				} else {
					// 군단슬롯에서 군단슬롯으로 드래그 함.(swap)
					XWndSquadInLegion *pSlotDst = dynamic_cast<XWndSquadInLegion*>( pDropWnd );
					XBREAK( pSlotDst == NULL );	// 이걸로도 변환이 안되면 에러
					if( pSlotDst )
					{
						// 다른 슬롯에 드랍
						if( pSlotSrc->getid() != pSlotDst->getid() )
						{
							int idxDst = pSlotDst->getid() - 100;
							ID snHeroDst = 0;
							if( pSlotDst->GetpHero() )
								snHeroDst = pSlotDst->GetpHero()->GetsnHero();
							MoveSquadInLegion( idxSrc, idxDst, pSlotSrc->GetpHero()->GetsnHero(),
												snHeroDst );
//  							XSPHero pHeroDst = pSlotDst->GetpHero();
// 							pSlotDst->SetFace( pSlotSrc->GetpHero() );
// 							pSlotSrc->SetFace( pHeroDst );
// 							pLegion->SwapSlotSquad( idxSrc, idxDst );
						}
					}
				}
				

			}
		}

	}
	SetbUpdate( TRUE );
	return 1;
}

/**
 @brief 부대슬롯 wnd를 찾는다.,
*/
XWndSquadInLegion* XSceneLegion::GetWndSquadSlot( int idxSlot )
{
	XWnd *pWndSlot = Find( idxSlot + 100 );
	if( XBREAK( pWndSlot == NULL ) )
		return NULL;
	XWndSquadInLegion *pSlot = SafeCast<XWndSquadInLegion*, XWnd*>( pWndSlot );
	return pSlot;
}
/**
 @brief 새 부대를 생성해서 idxLegion군단의 슬롯에 넣는다.
*/
void XSceneLegion::CreateSquadToLegion( XSPHero pHeroNew, XLegion *pLegion, int idxSlot )
{
	auto pSlot = GetWndSquadSlot( idxSlot );
	if( XBREAK(pSlot == NULL) )
		return;
	// 기존 부대 슬롯에 영웅이 있었을경우
	if (!pSlot->IsFree()) {
		// 기존영웅부대를 왼쪽 리스트로 옮긴다.
		auto pElem = new XWndInvenHeroElem(pSlot->GetpHero(), m_pLegion);
		pElem->SetUnitFace();
		m_pHeroList->AddItem(pElem->GetpHero()->GetsnHero(), pElem);
	}
	// 기존슬롯의 영웅을 새 영웅으로 교체한다.
	pSlot->SetFace( pHeroNew );
	// 왼쪽 리스트에서 새 영웅을 뺀다.
	m_pHeroList->DelItem( pHeroNew->GetsnHero() );
	// 부대객체를 생성한다.
	XSquadron *pSq = new XSquadron( pHeroNew );
	pLegion->AddSquadron(idxSlot, pSq, FALSE);
}

void XSceneLegion::MoveSquadInLegion( int idxSrc, int idxDst, ID snHeroSrc, ID snHeroDst )
{
	if (idxDst == -1)
		m_pLegion->DestroySquadBysnHero(snHeroSrc);
	else
		m_pLegion->SwapSlotSquad(idxSrc, idxDst);

	XSPHero pHeroSrc = ACCOUNT->GetHero( snHeroSrc );
	XSPHero pHeroDst = ACCOUNT->GetHero( snHeroDst );
	XWndSquadInLegion *pSlotSrc = GetWndSquadSlot( idxSrc );
	// 소스측 wnd가 없을순 없다.
	if( XBREAK( pSlotSrc == NULL ) )
		return;
	if( idxDst >= 0 )
	{
		// swap slot상황
		XWndSquadInLegion *pSlotDst = GetWndSquadSlot( idxDst );
		XSPHero pHeroDst = pSlotDst->GetpHero();
		pSlotDst->SetFace( pSlotSrc->GetpHero() );
		pSlotSrc->SetFace( pHeroDst );
	} else
	{
		// remove slot상황
		// WndList에 elem을 만들어 넣는다.
		XWndInvenHeroElem *pElem = new XWndInvenHeroElem(pHeroSrc, m_pLegion);
		pElem->SetUnitFace();
		m_pHeroList->AddItem( snHeroSrc, pElem );
		// 부대슬롯을 비운다.
		pSlotSrc->SetFace( nullptr );	// 슬롯에서 뺀다.
	}
//	m_pHeroList->SetAutoSize();
}

/**
 @brief 부대를 더 추가할수 있는지 검사.
*/
BOOL XSceneLegion::IsAbleAddHero( void )
{
	int maxSquad = 5;
	int level = ACCOUNT->GetLevel();
	maxSquad = XAccount::sGetMaxSquadByLevelForPlayer( level );
	//LegionPtr spLegion = ACCOUNT->GetCurrLegion();
	if( m_pLegion )
		if (m_pLegion->GetNumSquadrons() < maxSquad)
			return TRUE;
	return FALSE;

}

int XSceneLegion::OnClickPopupClose(XWnd *pWnd, DWORD p1, DWORD p2)
{
	if (pWnd->GetpParent())
		pWnd->GetpParent()->SetbDestroy(TRUE);
	return 1;
}

int XSceneLegion::OnClickSetLeader(XWnd *pWnd, DWORD p1, DWORD p2)
{
	XSPHero pHero = ACCOUNT->GetHero(p1);
	if (pHero && m_pLegion->GetpLeader() != pHero)
	{
		m_pLegion->SetpLeader(pHero);
	}
	OnClickPopupClose(pWnd, p1, p2);
	return 1;
}

void XSceneLegion::CopyLegionObj()
{
	XLegion *pLegion = ACCOUNT->GetCurrLegion().get();
	m_pLegion = new XLegion;
	m_pLegion->SetgradeLegion( pLegion->GetgradeLegion() );
// 	for( int i = 0; i < pLegion->GetMaxSquadSlot(); i++ ) {
// 		if( pLegion->GetpSquadronByidxPos(i) ) {
	for( auto pSqSrc : pLegion->GetlistSquadrons() ) {
			auto pSq = new XSquadron( *pSqSrc );
			m_pLegion->AddSquadron( pSqSrc->GetidxPos(), pSq, false );
			if( pLegion->GetpLeader() 
					&& pLegion->GetpLeader()->GetsnHero() == pSq->GetpHero()->GetsnHero() )
				m_pLegion->SetpLeader( pSq->GetpHero() );
	}
}

int XSceneLegion::OnClickDecide(XWnd* pWnd, DWORD p1, DWORD p2)
{
	//ACCOUNT->GetCurrLegion().get()->SetSquadAll(ACCOUNT->GetCurrLegionIdx(), m_pLegion);
	return 1;
}

void XSceneLegion::RecvChangeSquad(int success)
{
	XLegion *pLEgi = ACCOUNT->GetCurrLegion().get();
	if (success == 0)
	{
		//팝업 띄워야하나?
	}
	else
		DoExit(XGAME::xSC_UNIT_ORG);
}

/****************************************************************
* @brief 
*****************************************************************/
int XSceneLegion::OnClickLegionInfo( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickLegionInfo");
	//
	int numLast = 0;
	//
	XE::VEC2 vPos( 0, 0 );
	XWnd* pWndRoot = new XWnd();
	auto pPopup = new XWndPopup( _T( "squad_info.xml" ), "squad_info" );
	pPopup->SetbModal( TRUE );
//	xSET_TEXT( pPopup, "text.info", strInfo );
	Add( pPopup );
	for( int i = 1; i < EXP_TABLE_USER->GetMaxLevel(); ++i )	{
		int numSquad = XAccount::sGetMaxSquadByLevelForPlayer( i );
		if( numLast != numSquad )		{
			const _tstring strInfo = XFORMAT("%s:%d : %s%d\n", XTEXT(2299), i, XTEXT(2300), numSquad );
			auto pText = new XWndTextString( vPos, strInfo );
			if( i <= ACCOUNT->GetLevel() )
				pText->SetColorText( XCOLOR_YELLOW );
			else
				pText->SetColorText( XCOLOR_WHITE );
			pWndRoot->Add( pText );
			vPos.y += 14.f;
		}
		numLast = numSquad;
	}
	pWndRoot->SetAutoSize();
	pPopup->Add( pWndRoot );
	pWndRoot->AutoLayoutCenter();

	_tstring strInfo;// = _T("\n\n\n\n\n\n\n");
// 	for( int i = 1; i < EXP_TABLE_USER->GetMaxLevel(); ++i )	{
// 		int numSquad = XAccount::sGetMaxSquadByLevel( i );
// 		if( numLast != numSquad )		{
// 			strInfo += XFORMAT("레벨:%d : %d부대\n", i, numSquad );
// 		}
// 		numLast = numSquad;
// 	}

// 	auto pPopup = new XWndPopup( _T( "squad_info.xml" ), "squad_info" );
// 	pPopup->SetbModal( TRUE );
// 	xSET_TEXT( pPopup, "text.info", strInfo );
// 	Add( pPopup );

	return 1;
}




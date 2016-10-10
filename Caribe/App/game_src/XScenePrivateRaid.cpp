#include "stdafx.h"
#include "XGame.h"
#include "XSquadObj.h"
#include "XLegionObj.h"
#include "XFramework/Game/XEWorld.h"
#include "XFramework/Game/XEWndWorld.h"
#include "XFramework/Game/XEWndWorldImage.h"
#include "XFramework/Game/XEWorldCamera.h"
#include "XSquadron.h"
#include "XBaseUnit.h"
#include "XLegion.h"
#ifdef _XSINGLE
#include "XSceneBattleSingle.h"
#endif // _XSINGLE
#include "XScenePrivateRaid.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

/////////////////////////////////////////////////////////////////////////////////////////
XScenePrivateRaid* XScenePrivateRaid::s_pSingleton = nullptr;
XScenePrivateRaid* XScenePrivateRaid::sGet(){	return s_pSingleton;}
//////////////////////////////////////////////////////////////////////////
void XScenePrivateRaid::Destroy() 
{	
	XBREAK( s_pSingleton == nullptr );	// 이미 사라져있다면 잘못된것임
	XBREAK( s_pSingleton != this );		// 싱글톤이 this가 아니면 잘못된것임.
	s_pSingleton = nullptr;
}

void XScenePrivateRaid::Create()
{
	xSET_ENABLE( this, "butt.edit", false );
}

XScenePrivateRaid::XScenePrivateRaid( XGame *pGame, XSPSceneParam spParam )
	: XSceneBattle( pGame, XGAME::xSC_PRIVATE_RAID, spParam )
{ 
	XBREAK( s_pSingleton != nullptr );	// 이미 생성되어있다면 잘못된것임.
	s_pSingleton = this;
	Init(); 
	//
	// TODO: 이곳에 코딩하시오
	//
	SetbUpdate( true );
}
// 
// void XScenePrivateRaid::Create( void )
// {
// 	XSceneBattle::Create();
// }

/**
 @brief UI의 생성과 업데이트는 이곳에 넣습니다.
*/
void XScenePrivateRaid::Update()
{
	XSceneBattle::Update();
}

// void XScenePrivateRaid::OnCreateFaces( XSPLegionObj spLegionObj,
// 																			 int idxSide,
// 																			 XWnd* pWndLayer )
// {
// 	// 일단 초상화 컨트롤을 모두 생성시켜서 레이어에 넣는다.
// 	CreateFaceWnds( spLegionObj.get(), i, pWndLayer );
// 	// 15행씩 n열로 정렬
// 	XVector<XVector<XWnd*>> aryFaces;
// 	ArrangeAry( &aryFaces, pWndLayer );
// 	// 각 FaceWnd들을 확축하고 y정렬
// 	if( aryFaces.size() ) {
// 		ArrangeFaces( aryFaces, pWndLayer, i );
// 	}
// }
// 
// /**
//  @brief 매 프레임 실행됩니다.
// */
// int XScenePrivateRaid::Process( float dt ) 
// { 
// 	return XSceneBattle::Process( dt );
// }
// 
// //
// void XScenePrivateRaid::Draw() 
// {
// 	XSceneBattle::Draw();
// 	XSceneBattle::DrawTransition();
// }
// 
// void XScenePrivateRaid::OnLButtonDown( float lx, float ly ) 
// {
// 	XSceneBattle::OnLButtonDown( lx, ly );
// }
// void XScenePrivateRaid::OnLButtonUp( float lx, float ly ) {
// 	XSceneBattle::OnLButtonUp( lx, ly );
// }
// void XScenePrivateRaid::OnMouseMove( float lx, float ly ) {
// 	XSceneBattle::OnMouseMove( lx, ly );
// }
/** //////////////////////////////////////////////////////////////////
 @brief 제한시간
*/
int XScenePrivateRaid::GetsecTimeOver()
{
	return 60 * 3;
}

/** //////////////////////////////////////////////////////////////////
@brief 부대가 전멸함.
*/
void XScenePrivateRaid::OnDieSquad( XSPSquad spSquadObj )
{
	XSceneBattle::OnDieSquad( spSquadObj );
	//
	OnDieSquadPrivateRaid( spSquadObj );
}

void XScenePrivateRaid::OnDieSquadPrivateRaid( XSPSquad spSquadObj )
{
	auto spLegionObj = spSquadObj->GetspLegionObj();
	// 새로 만들어야 할 부대가 아군인지 적인지
	int idx = 0;
	if( spSquadObj->GetSide() == xSIDE_OTHER )
		idx = 1;
	const float dir = (idx == 0) ? -1.f : 1.f;
	auto spParam = std::static_pointer_cast<xPrivateRaidParam>(GetspSceneParam());
	// 다음 영웅 꺼냄
	auto& listHero = spParam->m_aryEnter[idx];
	XSPHero pHeroNext = nullptr;
	if( listHero.size() ) {
		pHeroNext = listHero.front();
		listHero.pop_front();
	} else {
		return;
	}
	// 영웅으로 부대객체 만듬.
	auto pSq = new XSquadron( pHeroNext );
	bool bCreateHero = (idx != 0);
	spLegionObj->GetspLegionMutable()->AddSquadron( -1, pSq, bCreateHero );
	//		auto spLegionObj = m_aryCamp[ idx ].m_spLegionObj;
	// 부대객체로 부대오브젝트 만듬.
	XE::VEC3 vwSpawn;
	if( idx == 0 )
		vwSpawn.x = 0;
	else
		vwSpawn.x = GetspWorld()->GetvwSize().w;
	vwSpawn.y = (float)xRandom( 100, (int)GetspWorld()->GetvwSize().h );
	xtLegionOption option = xLO_DEFAULT;
	auto spNewSquadObj = spLegionObj->sCreateSquadObj( spLegionObj,
																										 pSq,
																										 vwSpawn,
																										 option,
																										 GetpWndWorldMutable() );
	spLegionObj->AddSquad( spNewSquadObj );
	spNewSquadObj->SetAI( TRUE );
	// 지원군 등장
	auto spUnit = spNewSquadObj->GetspLeaderUnit();
	const auto sizeUnit = spUnit->GetSize();
	const auto vwPos = spUnit->GetvwPos() + XE::VEC3( 0, 0, -150 );
	auto spObjText = std::make_shared<XObjYellSkill>( XTEXT(2347),		// 지원군!
																										spUnit, 
																										vwPos, 
																										XCOLOR_WHITE );
	spObjText->SetvDelta( XE::VEC3() );
	spObjText->SetsecLife( 5.f );
	spObjText->SetvInDraw( XE::VEC2(46, 607) );		// 좌우 x좌표임
	XSceneBattle::AddObj( spObjText );
	SetbUpdate( true );

// 		비동기매니저->Add( 1초, []() {
// 			// 월드에 스폰
// 		});
}

/** //////////////////////////////////////////////////////////////////
@brief 현재 게임모드에 맞춰서 초상화로 표시해야할 부대목록을 얻는다.
*/
void XScenePrivateRaid::GetSquadObjToAry( int idxSide,
																		 XSPLegionObjConst spLegionObj,
																		 XVector<XSPSquadObjConst>* pOut )
{
	if( GetspSceneParam()->IsPrivateRaid() ) {
		for( auto sp : spLegionObj->GetlistSquad() ) {
			pOut->push_back( sp );
		}
// 		auto spParam = std::static_pointer_cast<xPrivateRaidParam>( m_spSceneParam );
// 		for( auto pHero : spParam->m_aryEnter ) {
// 			spLegionObj->GetSquad
// 		}
	}
}

#include "stdafx.h"
#include "XWndBattle.h"
#include "XSceneBattle.h"
#include "XAccount.h"
#include "XGame.h"
#include "XLegion.h"
//#include "JWWnd.h"
#include "_Wnd2/XWndProgressBar.h"
#include "_Wnd2/XWndSprObj.h"
#include "_Wnd2/XWndImage.h"
#include "XUnitHero.h"
#include "XSquadObj.h"
#include "Sprite/Layer.h"
#include "sprite/SprObj.h"
#include "XHero.h"
#include "skill/XSkillDat.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
/**
 @brief 전투후 영웅들 경험치 상황 팝업.
*/
XWndBattleAfterHeroExp::XWndBattleAfterHeroExp( int idxLegion, XGAME::xBattleResult* pResult )
	: XWndPopup( _T( "popup_battle_exp.xml" ), "popup_exp" )
{
	Init();
	m_idxLegion = idxLegion;
	m_pResult = pResult;
	xSetButtHander( this, SCENE_BATTLE, "butt.ok", &XSceneBattle::OnOkHerosExp );
	xSetButtHander( this, SCENE_BATTLE, "butt.statistic", &XSceneBattle::OnClickStatistic );
	//
}

void XWndBattleAfterHeroExp::Update()
{
	XLegion *pLegion = ACCOUNT->GetLegionByIdx( m_idxLegion ).get();
	if( XASSERT( pLegion ) ) {
// 		XArrayLinearN<XHero*, XGAME::MAX_SQUAD> ary;
		XVector<XHero*> ary;
		pLegion->GetHerosToAry( &ary );
		int nAdjustY = 0, size = ary.size();
		nAdjustY = ary.size() / 5;
		if( ary.size() % 5 == 0 )
			nAdjustY--;
		nAdjustY *= 42;
		if( ary.size() > 5 )
			size = 5;
		auto sizePopup = GetSizeLocal();
		XE::VEC2 vStart( ( 316 - 70 ) / 2 + 114 - 43 * ( size - 1 ), ( 250 - 81 ) / 2 + 45 - nAdjustY );
		int idx = 0;
//		XARRAYLINEARN_LOOP( ary, XHero*, pHero ) {
		for( auto pHero : ary ) {
			if( idx != 0 && idx % 5 == 0 ) {
				if( idx == ary.size() - ( ary.size() % 5 ) )
					size = ary.size() % 5;
				vStart.x = (float)( 316 - 70 ) / 2 + 114 - 43 * ( size - 1 );
				vStart.y += 84;
			}
			const std::string ids = XE::Format("wnd.slot%d", idx );
			auto pWndHero = XWndStoragyItemElem::sUpdateCtrl( this, vStart, pHero, ids );
			if( pWndHero ) {
				const auto sizeHero = pWndHero->GetSizeLocal();
				const auto vPosBar = vStart + XE::VEC2(0, sizeHero.h + 2.f );
				auto pWndBar = XWndProgressBar2::sUpdateCtrl( this
																										, vPosBar
																										, _tstring()
																										, ids + ".bar" );
				pWndBar->AddLayer( 1, _T("bar_result_bg.png"), true, XE::VEC2(-1,-1) );
				pWndBar->AddLayer( 2, _T("bar_result_main.png") );
				pWndBar->AddLayer( 3, _T("bar_result_cover.png"), true );
				pWndBar->SetLerp( 2, pHero->GetExp(), pHero->GetMaxExpCurrLevel() );
				if ( m_pResult->IsLevelupHero( pHero->GetsnHero() ) ) {
					// 렙업한 영웅은 렙업 마크 띄움.
					auto vPos = vStart;
					vPos.x += ( sizeHero.w * 0.5f );
					auto pWndSpr = XWndSprObj::sUpdateCtrl( this, vPos, ids + ".lvup" );
					pWndSpr->SetSprObj( _T("ui_levelup_text.spr"), 1, xRPT_LOOP );
					pWndSpr->SetsecLife( -1.f );
// 					auto pImgLvup = XWndImage::sUpdateCtrl( this
// 																								, vStart + XE::VEC2(0, -10)
// 																								, PATH_UI("ingame_levelup.png")
// 																								, ids + ".lvup" );
//					pImgLvup->AutoLayoutHCenter();
				}
			}
			++idx;
			vStart.x += 86;
		} // for( auto pHero : ary ) {
	} // pLegion
	XWndPopup::Update();
}

////////////////////////////////////////////////////////////////
XWndSkillButton::XWndSkillButton( float x, float y, 
								XSKILL::XSkillDat *pSkillDat, 
								XUnitHero *pUnitHero )
	: XWnd( x, y ), XWndButton( x, y, nullptr, nullptr )
{
	Init();
	XBREAK( pSkillDat == NULL );
	SetpSurface( 0, PATH_UI("common_bg_skillsimbol.png") );
	XWndImage *pImg = new XWndImage( XE::MakePath( DIR_IMG, pSkillDat->GetstrIcon().c_str() ),
									1.f, 1.f );
	Add( pImg );
	if( pUnitHero ) {
		m_timerCool = pUnitHero->GettimerCool();
		m_snHero = pUnitHero->GetpHero()->GetsnHero();
	}
// 	if( ( m_timerCool.IsOver() || m_timerCool.IsOff() ) )
// 		SetbActive( TRUE );
// 	else
// 		SetbActive( FALSE );
}

void XWndSkillButton::Destroy()
{
}

/**
 @brief 쿨타이머를 스타트 시킨다.
*/
void XWndSkillButton::SetCoolTimer( float sec )
{
	m_timerCool.Set( sec );
	// 쿨타이머가 시작되면 버튼눌림을 막음
//	SetbActive( FALSE );
}

int XWndSkillButton::Process( float dt )
{
	// 디액티브 상태에서 쿨타이머가 끝났으면 다시 활성화 시킴
// 	if( (m_timerCool.IsOver() || m_timerCool.IsOff()) )
// 	{
// 		SetbActive( TRUE );
// 	} else
// 		SetbActive( FALSE );
	return XWndButton::Process( dt );
}

void XWndSkillButton::Draw( void )
{
	bool bCool = false;
	///< 쿨타이머가 돌고있는동안 타이머 상태를 표시.
	if( m_timerCool.IsOn() && m_timerCool.IsOver() == FALSE ) {
		bCool = true;
// 		SetAlphaLocal( 0.5f );
		SetAlphaLocal( 1.f );
	} else
		SetAlphaLocal( 1.f );
	XWndButton::Draw();
	///< 쿨타이머가 돌고있는동안 타이머 상태를 표시.
	if( bCool ) {
		XE::VEC2 v = GetPosFinal();
		XCOLOR col = XCOLOR_RGBA( 0, 0, 0, 128 );
		float a = (1.f - m_timerCool.GetSlerp()) * -360;
		GRAPHICS->DrawPie( v + 30, 30.f, 0, a, col );
		v += XE::VEC2(26,43);
		PUT_STRINGF( v.x, v.y, XCOLOR_WHITE, "%d", (int)m_timerCool.GetRemainSec() );
	}
}
//////////////////////////////////////////////////////////////////////////
XWndFaceInBattle::XWndFaceInBattle( XSPSquad spSquadObj, int side )
	: XWndStoragyItemElem( XE::VEC2(0), spSquadObj->GetpHero(), true )
{
	m_spSquadObj = spSquadObj;
	m_Side = side;
}

XWndFaceInBattle::~XWndFaceInBattle()
{
	SAFE_DELETE( m_psoSkill );
}

BOOL XWndFaceInBattle::OnCreate()
{
	auto pRed = new XWndImage( PATH_UI("common_bg_frame_red.png"), 
														 true,
														 XE::xPF_ARGB4444,
														 XE::VEC2( 3, 2 ) );
	pRed->SetPriority( -100 );
	pRed->SetstrIdentifier( "rect.red" );
	pRed->SetbShow( false );
	Add( pRed );
	const int lvHero = m_spSquadObj->GetpHero()->GetLevel();
	SetLevel( lvHero );
	SetbShowName( true );
	SetbShowNum( false );
	XWndStoragyItemElem::OnCreate();
	auto pBar = new XWndProgressBar2();
	pBar->SetstrIdentifier("bar.hp");
	Add( pBar );
	if( m_Side == 1 ) {
		pBar->AddLayer( 1, _T( "bar_battle_face_bg.png" ), true );
		pBar->AddLayer( 2, _T( "bar_battle_face.png" ), false, XE::VEC2( 0.5f ) );
	} else {
		pBar->AddLayer( 1, _T( "bar_battle_face2_bg.png" ), true );
		pBar->AddLayer( 2, _T( "bar_battle_face2.png" ), false, XE::VEC2( 0.5f ) );
	}
	return TRUE;
}

int XWndFaceInBattle::Process( float dt )
{
	auto pBar = SafeCast2<XWndProgressBar2*>( Find("bar.hp") );
	if( pBar && m_spSquadObj ) {
		if( m_spSquadObj->IsLive() ) {
			const float sumHp = m_spSquadObj->GetMaxHpAllMember();
			const float hp = m_spSquadObj->GetSumHpAllMember();
			pBar->SetLerp( hp / sumHp );
		} else {
			xSET_SHOW( this, "rect.red", true );
			pBar->SetLerp( 0 );
		}
	}
	if( m_psoSkill && !m_psoSkill->GetbPause() ) {
		m_psoSkill->FrameMove( dt );
		if( m_psoSkill->IsFinish() ) {
			m_psoSkill->SetbPause( true );		// 끝나면 멈춤
		}
	}
	return XWndStoragyItemElem::Process( dt );
}

void XWndFaceInBattle::OnUseSkill( const _tstring& strText )
{
	m_strText = strText;
	if( !m_psoSkill ) {
		m_psoSkill = new XSprObj(_T("eff_skill_noticebar.spr") );
		m_psoSkill->SetpDelegate( this );
		m_psoSkill->SetAction( 1, xRPT_1PLAY );
	}
	if( m_psoSkill ) {
		m_psoSkill->SetbPause( false );
		m_psoSkill->ResetAction();
	}
}

void XWndFaceInBattle::Update()
{
	const auto vScale = GetScaleLocal();
	const auto sizeThisNoTrans = GetSizeLocalNoTrans();
	const auto sizeThis = sizeThisNoTrans * vScale;
	auto pBar = Find("bar.hp");
	if( pBar && m_spSquadObj ) {
		if( m_spSquadObj->IsLive() ) {
			const auto sizeBar = pBar->GetSizeLocalNoTrans();
			const auto scale = sizeThisNoTrans.w / sizeBar.w;
			pBar->SetScaleLocal( scale );
			pBar->SetY( sizeThisNoTrans.h );
		} else {
			pBar->SetbShow( false );
		}
	}

	XWndStoragyItemElem::Update();
}

void XWndFaceInBattle::Draw()
{
	XWndStoragyItemElem::Draw();
// 	if( !m_spSquadObj->IsLive() ) {
// 		const auto sizeThis = GetSizeFinal();
// 		GRAPHICS->FillRectSize( XE::VEC2( 0 ), sizeThis, XCOLOR_RGBA( 255, 0, 0, 128 ) );
// 	}
	if( m_psoSkill && !m_psoSkill->GetbPause() ) {
		const auto sizeThis = GetSizeFinal();
		auto vPos = GetPosFinal();
		vPos.x += sizeThis.w;
		m_psoSkill->Draw( vPos );
	}
}

/**
 @brief 스킬쓸때 초상화에서 튀어나는거.
*/
BOOL XWndFaceInBattle::OnDelegateDrawImageLayerBefore( XSprObj *pSprObj
																								 , XSprite *pSprSrc
																								 , XLayerImage *pImageLayer
																								 , XEFFECT_PARAM *pEffectParam
																								 , float x, float y
																								 , const MATRIX &mParent )
{
	if( pImageLayer->GetnLayer() != 3 )
		return FALSE;
	if( !m_strText.empty() ) {
		XCOLOR col = XCOLOR_RGBA( 255, 255, 0, (int)(pEffectParam->fAlpha * 255) );
		X3D::VEC3 v3( x, y, 0 );
		Vec4 v4d;
		MatrixVec4Multiply( v4d, v3, mParent );
		PUT_STRING_STROKE( v4d.x, v4d.y, col, m_strText.c_str() );
	}
	return TRUE;
}

#include "stdafx.h"
#include "XWndTech.h"
#include "XHero.h"
#include "_Wnd2/XWndImage.h"
#include "XGame.h"
#include "XStruct.h"
#include "XAccount.h"
#include "XSceneTech.h"
#include "XWndResCtrl.h"
#include "XSkillMng.h"
#include "skill/XSkillDat.h"
#include "skill/XESkillMng.h"
#include "_Wnd2/XWndSprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XWndArrow4Abil::XWndArrow4Abil( const XE::VEC2& vStart, const XE::VEC2& vEnd )
	: XWnd( vStart.x, vStart.y )
{
	Init();
// 	m_psfcArrow = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "skill_arrow.png" ) ) );
// 	m_vStart = vStart;
// 	m_vEnd = vEnd;
	SetDirection( vStart, vEnd, _T( "skill_arrow.png" ) );
	SetbTouchable( FALSE );		// 터치안되는 컨트롤
}

XWndArrow4Abil::XWndArrow4Abil( LPCTSTR szImg )
{
	Init();
	SetDirection( XE::VEC2(0), XE::VEC2(0), szImg );
	SetbTouchable( FALSE );		// 터치안되는 컨트롤
}

void XWndArrow4Abil::Destroy()
{
	SAFE_RELEASE2( IMAGE_MNG, m_psfcArrow );
}

void XWndArrow4Abil::SetDirection( const XE::VEC2& vStart, const XE::VEC2& vEnd, LPCTSTR szImg )
{
	if( XE::IsHave(szImg) ) {
		SAFE_RELEASE2( IMAGE_MNG, m_psfcArrow );
		m_psfcArrow = IMAGE_MNG->Load( XE::MakePath( DIR_UI, szImg ) );
	}
	m_vStart = vStart;
	m_vEnd = vEnd;
}

void XWndArrow4Abil::Draw()
{
	XE::VEC2 vPos = GetPosFinal();
	XE::VEC2 vDir = m_vEnd - m_vStart;		// 시작점과 끝점의 벡터를 얻음.
	XE::VEC2 vStart = vPos;
	XE::VEC2 vEnd = vPos + vDir;
//	vEnd.x -= XPropTech::ADJ_X;	잠시 테스트를 위해 막음.
	vEnd += m_vAdjust;
	m_psfcArrow->DrawDirection( vStart, vEnd );
}

////////////////////////////////////////////////////////////////
ID XWndAbilButton::s_idSelected = 0;
ID XWndAbilButton::s_idResearching = 0;
ID XWndAbilButton::s_snHeroResearching = 0;
XWndAbilButton::XWndAbilButton( XHero *pHero
															, XGAME::xtUnit unit
															, XPropTech::xNodeAbil *pNodeAbil
															, const XE::VEC2& vPos )
	: XWnd(vPos.x, vPos.y)
	, XWndButton( 0, 0, _T("common_bg_abil.png"), nullptr )
{
	Init();
	m_Unit = unit;
	m_idNode = pNodeAbil->idNode;
	m_pProp = pNodeAbil;
	XBREAK( pHero == nullptr );
	m_snHero = pHero->GetsnHero();
//	XTRACE( "wndAbilButton contructer" );
	XBREAK( pNodeAbil->strIcon.empty() );
	_tstring strRes = XE::MakePath( DIR_IMG, pNodeAbil->strIcon.c_str() );
//	SetpSurface( 0, strRes.c_str() );
//	XTRACE( "set wndabilbutt img finished" );
// 	XWndImage *pBg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("common_bg_itemblank.png")), -4, -4);
// 	Add(pBg);
	auto vScale = GetScaleLocal();
	XE::VEC2 vOfs(3);
//	auto pIcon = new XWndImage( strRes, vOfs * vScale );
	auto pIcon = new XWndImage( vOfs * vScale );
	pIcon->SetScaleLocal( 0.724f );
	pIcon->SetstrIdentifier("img.abil");
	Add( pIcon );
//	auto pCircle = new XWndImage(PATH_UI("skill_unit_bg_level.png"), 35.f, 32.f );
	auto pCircle = new XWndImage( PATH_UI( "skill_unit_bg_level.png" ), 23.f, 23.f );
	pCircle->SetstrIdentifier("img.circle");
	Add( pCircle );
	// 아이콘 위에 현재 연구남은시간 표시
	auto pText = new XWndTextString( 0, 0, _T(""), FONT_NANUM, 15.f );
	pText->SetstrIdentifier("text.research");
//	pText->SetLineLength( 54.f );
	pText->SetLineLength( 38.f );
	pText->SetAlign( XE::xALIGN_HCENTER );
	pText->SetStyleStroke();
	Add( pText );
	pText->SetbShow( FALSE );
//	XTRACE( "abil level finished" );
	SetbUpdate( TRUE );
}

void XWndAbilButton::Destroy()
{
}
void XWndAbilButton::Update()
{
	auto& research = ACCOUNT->GetResearching();
	if( m_pProp == nullptr )
		return;
// 	SetpSurface( 0, strRes.c_str() );
	auto pImg = xGET_IMAGE_CTRL( this, "img.abil" );
	auto pHero = ACCOUNT->GetHero( m_snHero );
	if( XASSERT(pHero) && pImg ) {
		auto strIcon = m_pProp->strIcon;
		if( strIcon.empty() ) {
			auto pSkillDat = XSKILL::XESkillMng::sGet()->FindByIds( m_pProp->strSkill );
			if( pSkillDat ) {
				strIcon = pSkillDat->GetstrIcon();
			}
		}
		const _tstring strRes = XE::MakePath( DIR_IMG, strIcon );
		pImg->SetSurfaceRes( strRes );
		bool bEnable = false;
		// 활성화가 되려면 우선 필요레벨이 되어야 한다.
		bEnable = ACCOUNT->IsEnableAbil( pHero, SCENE_TECH->GetunitSelected(), m_pProp );
		if( bEnable || m_bInRightPanel )
			pImg->SetblendFunc( XE::xBF_MULTIPLY );
		else
			// 오픈가능한 레벨이 아직 안됐으면 그레이스케일로 만듬.
			pImg->SetblendFunc( XE::xBF_GRAY );
	// 	if( bEnable || m_bInRightPanel )
	// 		SetAlphaLocal( 1.f );
	// 	else
	// 		// 오픈가능한 레벨이 아직 안됐으면 그레이스케일로 만듬.
	// 		SetAlphaLocal( 0.5f );		// 나중에 그레이스케일 셰이더로 바꿀것.
		// 락 상태는 자물쇠 그림 그림.
		XWnd *pCircle = Find( "img.circle" );
		if( pCircle ) {
			pCircle->SetbShow( !m_bInRightPanel );		// 우측패널이면 일단 감춤.
			XWnd *pLock = Find( "img.lock" );		// 자물쇠 그림
			if( !m_bInRightPanel ) {
				const auto abil = pHero->GetAbilNode( m_Unit, m_idNode );
//			XBREAK( pAbil == nullptr );
				if( abil.IsLock() ) {
					if( pLock == nullptr ) {
						pLock = new XWndImage( PATH_UI( "lock_small.png" ), 4, 2 );
						pLock->SetstrIdentifier( "img.lock" );
						pCircle->Add( pLock );
					}
					if( m_pText ) {
						this->DestroyWnd( m_pText );
						m_pText = nullptr;
					}
				} else {
					// 락이 풀렸는데 자물쇠그림이 있으면 없앰.
					if( pLock )
						pCircle->DestroyWnd( pLock );
					if( m_pText == nullptr ) {
						m_pText = new XWndTextString( XE::VEC2( 0 ),
																					pCircle->GetSizeLocal(),
																					_T( "" ),
																					FONT_RESNUM, 18.f );
						m_pText->SetAlign( XE::xALIGN_CENTER );
						pCircle->Add( m_pText );
					}
					if( m_pText ) {
						m_pText->SetText( XE::Format( _T( "%d" ), abil.point ) );
						if( abil.point > 0 )
							m_pText->SetColorText( XCOLOR_RGBA( 255, 156, 0, 255 ) );
						else
							m_pText->SetColorText( XCOLOR_GRAY );
					}
				} // !IsLock
			} else {
				// 우측 패널이라면 감춤
			}
			if( pLock )
				pLock->SetbShow( !m_bInRightPanel );
		} // pCircle
		if( m_idNode == s_idResearching && m_snHero == s_snHeroResearching ) {
			if( !m_bInRightPanel ) {
				auto pImgGlow = SafeCast2<XWndSprObj*>( Find( "spr.researching.node" ) );
				if( pImgGlow == nullptr ) {
					const auto sizeThis = GetSizeLocalNoTrans();
					auto pGlow = new XWndSprObj( _T( "ui_glow_tech.spr" ), 1, sizeThis * 0.5f );
					pGlow->SetstrIdentifier( "spr.researching.node" );
					Add( pGlow );
				}
			}
		} else {
			// 연구중 노드가 아니면 글로우 삭제
			DestroyWndByIdentifier( "spr.researching.node");
		}
	} // pHero
	XWndButton::Update();
}

int XWndAbilButton::Process( float dt )
{
	// 이특성이 현재 연구중이면.
	const auto& research = ACCOUNT->GetResearching();
	bool bResearching = ( research.GetidAbil() == m_pProp->idNode && research.GetsnHero() == m_snHero );
	xSET_SHOW( this, "text.research", !m_bInRightPanel && bResearching );		// 우측판넬이 아닐때만 표시함.
	if( bResearching ) {
		auto pHero = ACCOUNT->GetHero( research.GetsnHero() );
		if( pHero ) {
			_tstring strRemain;
			auto& costAbil = pHero->GetCostAbilCurr();
			SCENE_TECH->GetRemainResearchTime( &strRemain, costAbil.sec );
			xSET_TEXT( this, "text.research", strRemain.c_str() );
		}
	}
	return XWndButton::Process( dt );
}

void XWndAbilButton::Draw()
{
	XWndButton::Draw();
}


void XWndAbilButton::SetNode( XGAME::xtUnit unit, XPropTech::xNodeAbil *pProp )
{
	XBREAK( pProp == nullptr );
	m_pProp = pProp;
	m_Unit = unit;
	m_idNode = pProp->idNode;

}

////////////////////////////////////////////////////////////////
XWndResearchConfirm::XWndResearchConfirm( XGAME::xtUnit unit
																				, ID idAbil
																				, int point
																				, const XPropTech::xtResearch& costAbil )
	: XWndPopup( _T( "research_info.xml" ), "research_info" )
{
	Init();
	auto pNode = XPropTech::sGet()->GetpNode( unit, idAbil );
	if( XASSERT( pNode ) ) {
		xSET_IMG( this, "img.abil", XE::MakePath( DIR_IMG, pNode->strIcon ) );
		xSET_TEXT( this, "text.abil.name", XTEXT( pNode->idName ) );
		_tstring strDesc = XTEXT( pNode->idDesc );
		auto pSkillDat = SKILL_MNG->FindByIds( pNode->strSkill );
		pSkillDat->GetSkillDesc( &strDesc, point );
		xSET_TEXT( this, "text.abil.desc", strDesc );
		xSET_TEXT_FORMAT( this, "text.point", _T("%d"), point  );
		xSET_TEXT( this, "text.time", XGAME::GetstrResearchTime( costAbil.sec ) );
		//
		auto pRootRes = Find("wnd.res");
		if( pRootRes ) {
			auto pCtrl = new XWndResourceCtrl();
			pCtrl->SetspaceY( 4.f );
			pRootRes->Add( pCtrl );
			const auto aryRes = costAbil.GetaryNeedRes();
			XCOLOR col = XCOLOR_WHITE;
			bool bNotEnough = false;
			for( auto& res : aryRes ) {
				if( !ACCOUNT->IsEnoughResource( res ) ) {
					col = XCOLOR_RED;
					bNotEnough = true;
				} else {
					col = XCOLOR_WHITE;
				}
				pCtrl->AddRes( res, col );
			}
			if( bNotEnough ) {
				xSET_ENABLE( this, "butt.ok", false );
			}
//			pCtrl->AddRes( aryRes );
			pCtrl->AutoLayoutCenter();
		}
	}
}

BOOL XWndResearchConfirm::OnCreate()
{
	return XWndPopup::OnCreate();
}

void XWndResearchConfirm::Update()
{
	XWndPopup::Update();
}


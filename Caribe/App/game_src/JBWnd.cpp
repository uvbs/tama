#include "stdafx.h"
#include "XStruct.h"
#include "XWndStorageItemElem.h"
#include "JBWnd.h"
#include "XSoundMng.h"
#include "client/XAppMain.h"
#include "XGame.h"
#include "XFontMng.h"
#include "XFramework/XConstant.h"
//#include "XWindow.h"
#include "XImageMng.h"
#include "XFramework/client/XLayout.h"
#include "XAccount.h"
#include "XSceneWorld.h"
//#include "XUnitHero.h"
#include "XBaseItem.h"
#include "XSockGameSvr.h"
#include "XSceneUnitOrg.h"
#include "XSceneGuild.h"
#include "XGuild.h"
#include "XSpots.h"
#include "XSpotDaily.h"
#include "XHero.h"
#include "XLegion.h"
#include "XWorld.h"
#include "XWndResCtrl.h"
#include "_Wnd2/XWndProgressBar.h"
#include "_Wnd2/XWndList.h"
#include "_Wnd2/XWndText.h"
#include "_Wnd2/XWndImage.h"
#include "_Wnd2/XWndButton.h"
#include "XSystem.h"
#include "XCampObj.h"
#include "sprite/SprObj.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
//////////////////////////////////////////////////////////////////////////
// 임재범 전용 윈도우UI 구현코드
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
XWndResIcon::XWndResIcon(XGAME::xtSpot typeSpot, float x, float y)
	:XWnd(x, y)
{
	Init();
	SetPosLocal(x, y);
	Create(typeSpot, x, y);
	SetAutoSize();
	//if (SCENE_WORLD)
	//	SetEvent(XWM_CLICKED, SCENE_WORLD, &XWndResIcon::OnClickResIcon);
}

XWndResIcon::XWndResIcon(XGAME::xtSpot typeSpot, const XE::VEC2& vPos) {
	Init();
	SetPosLocal(vPos);
	Create(typeSpot, vPos.x, vPos.y);
	SetAutoSize();
	//if (SCENE_WORLD)
	//	SetEvent(XWM_CLICKED, SCENE_WORLD, &XWndResIcon::OnClickResIcon);
}

void XWndResIcon::Create(XGAME::xtSpot typeSpot, float x, float y)
{
	m_pBubble = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("speech_bubble.png")), 0.f, 0.f);
	Add(m_pBubble);
	switch (typeSpot)
	{
	case XGAME::xSPOT_CASTLE:
	{
		XWndImage* pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("world_top_iron.png")), 0.f, 0.f);
		pImg->SetPosLocal(25.f, 18.5f - pImg->GetHeightLocal() / 2);
		m_pBubble->Add(pImg);

		pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("world_top_wood.png")), 0.f, 0.f);
		pImg->SetPosLocal(25.f - pImg->GetWidthLocal() / 2 * 1.5f, 18.5f - pImg->GetHeightLocal() / 2);
		m_pBubble->Add(pImg);
	}
		break;
	case XGAME::xSPOT_JEWEL:
	{
		XWndImage* pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("world_top_jewel.png")), 0.f, 0.f);
		pImg->SetPosLocal(25.f - pImg->GetWidthLocal() / 2, 18.5f - pImg->GetHeightLocal() / 2);
		m_pBubble->Add(pImg);
	}
		break;
	case XGAME::xSPOT_SULFUR:
	{
		XWndImage* pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("world_top_sulfur.png")), 0.f, 0.f);
		pImg->SetPosLocal(25.f - pImg->GetWidthLocal() / 2, 18.5f - pImg->GetHeightLocal() / 2);
		m_pBubble->Add(pImg);
	}
		break;
	case XGAME::xSPOT_MANDRAKE:
	{
		XWndImage* pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("world_top_mandrake.png")), 0.f, 0.f);
		pImg->SetPosLocal(25.f - pImg->GetWidthLocal() / 2, 18.5f - pImg->GetHeightLocal() / 2);
		m_pBubble->Add(pImg);
	}
		break;
	case XGAME::xSPOT_NPC:
		break;
	}
}

int XWndResIcon::OnClickResIcon(XWnd *pWnd, DWORD p1, DWORD p2)
{
	//SetbDestroy(TRUE);
	return 1;
}

void XWndResIcon::Draw(void)
{
	XWnd::Draw();
	//
// 	XE::VEC2 vPos = GetPosFinal() + XE::VEC2(5, -10);
// 	PUT_STRINGF(vPos.x, vPos.y, XCOLOR_WHITE, "%d", m_numRes);
}
//////////////////////////////////////////////////////////////////////////


XWndResParticle::XWndResParticle(LPCTSTR szFilename,
																const XE::VEC2& vPosSpot,
//																const XE::VEC2& vPos,
																const XE::VEC2& vTarget,
																XWndScrollView *pScrollWorld,
																XGAME::xtResource typeRes, 
																int start, float time)
	: XWndImage(TRUE, XE::MakePath(DIR_UI, szFilename), vPosSpot.x, vPosSpot.y)
{
	Init();
	m_vTarget = vTarget;
//	m_pSpot = pSpot;
	m_pScrollWorld = pScrollWorld;
	m_vStart = pScrollWorld->GetvAdjScroll();
	m_vStart.Abs();
	//m_vSpot = m_pSpot->GetPosWorld() - m_pScrollWorld->GetvAdjust().Abs();
// 	m_vSpot = m_pScrollWorld->GetvAdjust();
// 	m_vSpot.Abs();
//	m_vSpot = m_pSpot->GetPosWorld() - m_vSpot;
	auto vAdj = m_pScrollWorld->GetvAdjScroll();
	vAdj.Abs();
	m_vSpot = vPosSpot - vAdj;
	//m_vStart.Abs();
	//m_vStart = vPos;
	GetpSurface()->SetAdjustAxis(XE::VEC2(GetpSurface()->GetWidth() / 2, GetpSurface()->GetHeight() / 2));
	SetRotateLocal(xRandomF(0.f, 360.f));
	//SetScaleLocal( 0.7f );
	if (time == 0.f)
		m_Timer.Set(xRandomF(0.f, 0.2f));
	else
		m_Timer.Set(time);
	SetbShow(FALSE);

	if (xRandom(0, 1) == 0) {
		m_curveX = xRandomF(-1000.f, -100.f);
		m_dir = -1;
	} else {
		m_dir = 1;
		m_curveX = xRandomF(100.f, 1000.f);
	}
	m_start = start;
	m_typeRes = typeRes;
	//m_curveY = xRandomF(-200.f,-1.f);
}
int XWndResParticle::Process(float dt)
{
	if (m_Timer.IsOver() && m_Timer.IsOn()) {
		SetbShow(TRUE);
		m_Timer.Off();
		m_timerLife.Set( 1.0f );
	}
	if ( GetbShow() )	{
		XE::VEC2 vPos;
		XE::VEC2 vAdjust(m_pScrollWorld->GetvAdjScroll());
		vAdjust.Abs();
		vAdjust = m_vStart - vAdjust;
		float lerpTime = m_timerLife.GetSlerp();
		if( lerpTime > 1.f )
			lerpTime = 1.f;
		float lerp = XE::xiHigherPowerAccel( lerpTime, 1.f, 0 );
		float alpha = XE::xiHigherPowerDeAccel( lerpTime, 1.f, 0 );
		const auto v0 = m_vSpot + vAdjust + XE::VEC2(m_curveX, -200.f);
		const auto v1 = m_vSpot + vAdjust;
		Vec2CatmullRom(vPos, v0, v1, m_vTarget , m_vTarget, lerp );
		SetPosLocal(vPos);
		SetAlphaLocal( alpha );
		if( lerpTime >= 1.f ) {
			lerpTime = 1.f;
			SetbShow( FALSE );
			SetbDestroy( TRUE );
			if( SCENE_WORLD ) {
				auto pLayer = SCENE_WORLD->GetpLayerUIParticle();
				if( pLayer ) {
					auto pWndSpr = new XWndSprObj( _T("brilliant.spr"), 2, vPos + XE::VEC2(7,7), xRPT_1PLAY );
					pLayer->Add( pWndSpr );
				}
				SCENE_WORLD->ArriveResObj( m_typeRes, m_start );
			}
		}
	}

	return XWndImage::Process(dt);
}

//////////////////////////////////////////////////////////////////////////
/**
 @brief pWndList에 pHero의 XWndInvenHeroElem객체를 생성하거나 업데이트하는 표준함수.
 @param pHero 코딩의 편의상 null이 될수도 있어야 함.
*/
XWndInvenHeroElem* XWndInvenHeroElem::sUpdateCtrl( XWndList *pWndList, XSPHero pHero )
{
	if( pHero == nullptr )
		return nullptr;
	std::string idsElem = XE::Format( "elem.hero.%s", SZ2C( pHero->GetstrIdentifer() ) );
	auto pWndElem
		= SafeCast2<XWndInvenHeroElem*>( pWndList->Find( idsElem ) );
	if( pWndElem == nullptr ) {
		pWndElem = new XWndInvenHeroElem( pHero );
		pWndElem->SetstrIdentifier( idsElem );
		pWndList->AddItem( pHero->GetsnHero(), pWndElem );
	}
	return pWndElem;
}
/**
 @brief 
*/
XWndInvenHeroElem::XWndInvenHeroElem( XSPHero pHero, XLegion *pLegion )
	: m_pProp( pHero->GetpProp() )
{
	Init();
	m_pHero = pHero;
//	m_pProp = pHero->GetpProp();
	if( XBREAK( m_pHero == nullptr ) )
		return;
	m_pLegion = pLegion;
	auto numStar = m_pHero->GetGrade();
	m_psfcStar = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_etc_bicstar.png" ) ) );
	m_psfcStarEmpty = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_etc_bigstar_empty.png" ) ) );
	// 	for (int i = 0; i < numStar; ++i) {
	// 		m_pStar[i] = IMAGE_MNG->Load(  XE::MakePath(DIR_UI, _T("common_etc_bicstar.png")));
	// 		m_aryStarEmpty[ i ] = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_etc_bigstar_empty.png" ) ) );
	// 	}
	m_psfcBgGradation = IMAGE_MNG->Load( PATH_UI( "bg_hero.png" ), XE::xPF_ARGB8888 );
	m_pFace = IMAGE_MNG->Load( XE::MakePath( DIR_IMG, m_pHero->GetpProp()->strFace.c_str() ), XE::xPF_ARGB8888 );
// 	m_pBG = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_unit_bg_s.png" ) ), 0, 0 );
	m_pBG = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_unit_bg_s.png" ) ), XE::xPF_ARGB4444 );

	m_pNameCard = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "corps_heroname_bg.png" ) ) );
	SetSizeLocal( m_pBG->GetWidth() + 4, m_pBG->GetHeight() + m_pNameCard->GetHeight() / 2 );
	//m_pNameCard->SetScale(0.7f);
	auto pImg = new XWndImage( PATH_UI( "bg_level.png" ), 0, 43 );
	pImg->SetstrIdentifier( "img.bg.level" );
	Add( pImg );
	//	auto pText = new XWndTextString( XFORMAT("%d",pHero->GetLevel()), FONT_NANUM, 14.f );
	if( m_pHero ) {
		auto pText = new XWndTextString( XFORMAT( "%d", pHero->GetLevel() ), FONT_RESNUM, 14.f );
		pText->SetstrIdentifier( "xuzhu" );
		pText->SetStyle( xFONT::xSTYLE_STROKE );
		pText->SetAlignCenter();
		pImg->Add( pText );
	}
	m_pName = FONTMNG->CreateFontObj( FONT_NANUM, 16.f );
	m_pName->SetAlign( XE::xALIGN_HCENTER );
	m_pName->SetStyle( xFONT::xSTYLE_STROKE );
	m_pName->SetLineLength( 48.f );
	m_pSelect = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_unit_bg_s_select.png" ) ) );
	m_pParty = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "legion_hero_battle.png" ) ) );
	m_pSoulStone = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "gem_small.png" ) ) );
	m_strName = XE::Format( _T( "%s" ), m_pHero->GetpProp()->GetstrName().c_str() );
}

XWndInvenHeroElem::XWndInvenHeroElem( XPropHero::xPROP *pProp )
{
	Init();
	if( XBREAK( pProp == nullptr ) )
		return;
	m_pProp = pProp;
	m_bSoul = true;
	//	auto numStar = pProp->GetGrade();
	auto numStar = XGAME::xGD_COMMON;
	m_psfcStar = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_etc_bicstar.png" ) ) );
	m_psfcStarEmpty = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_etc_bigstar_empty.png" ) ) );
	// 	for (int i = 0; i < numStar; ++i) {
	// 		m_pStar[i] = IMAGE_MNG->Load(  XE::MakePath(DIR_UI, _T("common_etc_bicstar.png")));
	// 		m_aryStarEmpty[ i ] = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_etc_bigstar_empty.png" ) ) );
	// 	}
	m_psfcBgGradation = IMAGE_MNG->Load( PATH_UI( "bg_hero.png" ), XE::xPF_ARGB8888 );
	m_pFace = IMAGE_MNG->Load( XE::MakePath( DIR_IMG, pProp->strFace.c_str() ), XE::xPF_ARGB8888 );
	m_pBG = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_unit_bg_s.png" ) ) );
	m_pNameCard = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "corps_heroname_bg.png" ) ) );
	SetSizeLocal( m_pBG->GetWidth() + 4, m_pBG->GetHeight() + m_pNameCard->GetHeight() / 2 );
	auto pImg = new XWndImage( PATH_UI( "bg_level.png" ), 0, 43 );
	pImg->SetstrIdentifier( "img.bg.level" );
	Add( pImg );
	auto pText = new XWndTextString( XFORMAT( "?" ), FONT_NANUM, 14.f );
	pText->SetstrIdentifier( "xuzhu" );
	pText->SetStyle( xFONT::xSTYLE_STROKE );
	pImg->Add( pText );
	m_pName = FONTMNG->CreateFontObj( FONT_NANUM, 16.f );
	m_pName->SetAlign( XE::xALIGN_HCENTER );
	m_pName->SetStyle( xFONT::xSTYLE_STROKE );
	m_pName->SetLineLength( 48.f );
	m_pSelect = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "common_unit_bg_s_select.png" ) ) );
	m_pParty = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "legion_hero_battle.png" ) ) );
	m_pSoulStone = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "hero_piece.png" ) ) );
	m_strName = XE::Format( _T( "%s" ), pProp->GetstrName().c_str() );
}

void XWndInvenHeroElem::Destory() 
{
	SAFE_RELEASE2( IMAGE_MNG, m_pBG );
	SAFE_RELEASE2( IMAGE_MNG, m_pFace );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcBgGradation );
	SAFE_RELEASE2( IMAGE_MNG, m_pNameCard );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcStar );
	SAFE_RELEASE2( IMAGE_MNG, m_psfcStarEmpty );
	SAFE_RELEASE2( IMAGE_MNG, m_pSelect );
	SAFE_RELEASE2( IMAGE_MNG, m_pUnitFace );
	SAFE_RELEASE2( IMAGE_MNG, m_pUnitBg );
	SAFE_RELEASE2( IMAGE_MNG, m_pParty );
	SAFE_RELEASE2( IMAGE_MNG, m_pSoulStone );
	SAFE_DELETE( m_pName );
	//SAFE_DELETE(m_pSoulCount);
}

void XWndInvenHeroElem::SetUnitFace( void )
{
	if( XBREAK( m_pHero == NULL ) )
		return;
	auto pProp = PROP_UNIT->GetpProp( m_pHero->GetUnit() );
	if( XBREAK( pProp == NULL ) )
		return;
	_tstring strTitle = XE::GetFileTitle( pProp->strFace.c_str() );
	_tstring strFace = XE::Format( _T( "%s02.png" ), strTitle.c_str() );
	LPCTSTR resImg = XE::MakePath( DIR_IMG, strFace.c_str() );
	m_pUnitFace = IMAGE_MNG->Load( resImg, XE::xPF_ARGB8888 );
	m_pUnitBg = IMAGE_MNG->Load( XE::MakePath( DIR_UI, _T( "corps_legionnaire_bg.png" ) ) );
	if( m_pName )
		m_pName->SetLineLength( 50.f );
	XWnd *pLevel = Find( "img.bg.level" );
	if( pLevel )
		pLevel->SetPosLocal( pLevel->GetPosLocal().x, 36.f );
	SetSizeLocal( 87, 50 );
}

const XPropHero::xPROP* XWndInvenHeroElem::GetpProp()
{
	if( m_pProp )
		return m_pProp;
	else if( m_pHero )
		return m_pHero->GetpProp();

	return nullptr;
}
ID XWndInvenHeroElem::GetsnHero( void )
{
	if( m_pHero )
		return 	m_pHero->GetsnHero();
	return 0;
}
int XWndInvenHeroElem::GetnumStars()
{
	XBREAK( m_pProp == nullptr );
	if( m_pHero )
		return m_pHero->GetGrade();
	return 0;	// 별을 그리지 마라는 의미
	//	return m_pProp->GetGrade();
}

XGAME::xtGrade XWndInvenHeroElem::GetGrade()
{
	XBREAK( m_pProp == nullptr );
	if( m_pHero )
		return m_pHero->GetGrade();
	return XGAME::xGD_NONE;		// 별을 그리지 마라는 의미
	//	return m_pProp->GetGradeMax();
}


void XWndInvenHeroElem::Draw()
{
	XE::VEC2 vPos = GetPosFinal();
	vPos.x += 4;
	if( m_pFace ) {
		XBREAK( m_psfcBgGradation == nullptr );
//		m_psfcBgGradation->SetScale( 0.67f );
		m_pFace->SetScale( 0.67f );
		if( m_bSoul )
			m_pFace->SetfAlpha( 0.5f );
		else
			m_pFace->SetfAlpha( 1.f );
		m_psfcBgGradation->Draw( vPos.x + 4, vPos.y + 4 );
		m_pFace->Draw( vPos.x + 4, vPos.y + 4 );
	}
	if( m_pBG )
		m_pBG->Draw( vPos.x, vPos.y );
	if( m_bSelected )
		m_pSelect->Draw( vPos.x, vPos.y + 1 );
	int numStar = GetnumStars();
	if( m_psfcStar && m_psfcStarEmpty ) {
		for( int i = 1; i < XGAME::xGD_MAX; ++i ) {
			XE::VEC2 v( vPos.x - 1 + ( i - 1 ) * 10, vPos.y - 1 );
			if( i <= numStar ) {
				m_psfcStar->SetScale( 0.6f );
				m_psfcStar->Draw( v );
			}
		}
	}
	if( m_pNameCard ) {
		m_pNameCard->SetScale( 1.2f, 1.f );
		if( m_pUnitFace )
			m_pNameCard->Draw( vPos.x - 1.f, vPos.y + 36.f );
		else
			m_pNameCard->Draw( vPos.x - 1.f, vPos.y + 41.f );
	}
	XWnd::Draw();
	if( m_pName ) {
		XE::VEC2 vName = vPos;
		if( m_pUnitFace )
			vName.Set( vName.x - 8, vName.y - 5 );
		m_pName->DrawString( vPos.x + 5.f, vName.y + 45, m_strName.c_str() );
		if( m_bSoul && m_pSoulStone ) {
			m_pSoulStone->SetScale( 0.3f );
			m_pSoulStone->Draw( vPos + XE::VEC2( 4, 10 ) );
			int count = 0, max;
			if( m_pHero )
				max = XGAME::GetNeedSoulPromotion( GetGrade() );	// 승급에 필요한 개수
			else
				max = XGAME::GetNeedSoulSummon();	// 소환에 필요한 개수.
			count = ACCOUNT->GetNumSoulStone( GetpProp()->strIdentifier );
			if( count >= max ) {
				m_pName->SetColor( XCOLOR_GREEN );
			} else {
				m_pName->SetColor( XCOLOR_WHITE );
			}
			m_pName->DrawString( vPos.x + 5.f, vName.y + 35, XE::Format( _T( "%d/%d" ), count, max ) );
		}
	}

	if( m_pUnitFace && m_pUnitBg )
	{
		m_pUnitBg->Draw( vPos + XE::VEC2( 46, 19 ) );
		m_pUnitFace->Draw( vPos + XE::VEC2( 46, 19 ) );
	}
	if( m_pLegion && m_pHero )
	{
		if( m_pLegion->GetSquadronByHeroSN( m_pHero->GetsnHero() ) )
			m_pParty->Draw( vPos.x - 4, vPos.y + 30.f );
	}
	else if( m_pHero )
		if( ACCOUNT->GetCurrLegion()->GetSquadronByHeroSN( m_pHero->GetsnHero() ) )
			m_pParty->Draw( vPos.x - 4, vPos.y + 30.f );
}

void XWndInvenHeroElem::DrawDrag( const XE::VEC2& vMouse )
{
	if( m_pFace )
	{
		XE::VEC2 vSize = m_pFace->GetSize();
		m_pFace->SetfAlpha( 0.5f );
		m_pFace->Draw( vMouse - vSize / 2.f );
	}
}

void XWndInvenLegionElem::Destroy()
{
	SAFE_RELEASE2( IMAGE_MNG, m_pBG );
	SAFE_RELEASE2( IMAGE_MNG, m_pFace );
	SAFE_RELEASE2( IMAGE_MNG, m_pSelect );
}

//////////////////////////////////////////////////////////////////////////
/**
@brief
*/
XWndInvenLegionElem::XWndInvenLegionElem(XPropUnit::xPROP* pPropUnit)
	:XWnd(0, 0)
{
	Init();
	m_pProp = pPropUnit;
	m_pBG = IMAGE_MNG->Load(  XE::MakePath(DIR_UI, _T("common_unit_bg_bb.png")));
	SetSizeLocal(m_pBG->GetWidth(), m_pBG->GetHeight());
	m_pSelect = IMAGE_MNG->Load(  XE::MakePath(DIR_UI, _T("common_unit_bg_bb_sel.png")));
	m_pFace = IMAGE_MNG->Load(XE::MakePath(DIR_IMG, m_pProp->strFace.c_str()), XE::xPF_ARGB8888);
}

void XWndInvenLegionElem::SetLock(bool bFlag)
{
	m_bLock = bFlag;
	auto pImg = Find("img.lock");
	if (bFlag) {
		if (pImg == nullptr) {
			pImg = new XWndImage(PATH_UI("lock_small.png"), 10.f, 10.f);
			pImg->SetstrIdentifier("img.lock");
			pImg->SetScaleLocal(2.f);
			Add(pImg);
		}
		pImg->SetbShow(TRUE);
	} else {
		pImg->SetbShow(FALSE);
	}
}

void XWndInvenLegionElem::Draw()
{
	if (m_pFace) {
		if (!GetbEnable())
			m_pFace->SetfAlpha(0.5f);
		m_pFace->SetScale(1.9f);
		if( m_bLock )
			m_pFace->SetBlendFunc( XE::xBF_GRAY );
		m_pFace->Draw(GetPosFinal() + 4);
	}
	if (m_pBG) {
		m_pBG->Draw(GetPosFinal());
	}
	if (m_bSelected) {
		m_pSelect->Draw(GetPosFinal() + 1);
	}
	XWnd::Draw();
}

//////////////////////////////////////////////////////////////////////////
XWndLevelupElem::XWndLevelupElem(XBaseItem* pItem)
	: XWndImage(TRUE, XE::MakePath(DIR_IMG, pItem->GetpProp()->strIcon.c_str()), 0.f, 0.f)
{
	Init();

	m_pText = new XWndTextString(XE::VEC2(0), XE::Format(_T("1")), FONT_NANUM, 20.f);
	Add(m_pText);
}

int XWndCallbackSpr::Process( float dt )
{
	if( m_pOwner && GetpSprObj()->IsFinish() )
		(m_pOwner->*m_pFunc)(this, m_dwParam1, m_dwParam2);

	return XWndSprObj::Process( dt );
}

//////////////////////////////////////////////////////////////////////////
XWndStatArrow::XWndStatArrow(float fAfter, float fBefore, float x, float y)
	:XWndImage(TRUE, x, y, 48.f, 48.f)
{
	Init();
	if (fAfter > fBefore)
		SetSurface(XE::MakePath(DIR_UI, _T("legion_stat_up")));
	else if (fAfter < fBefore)
		SetSurface(XE::MakePath(DIR_UI, _T("legion_stat_down")));
// 	else
// 		SetSurface(XE::MakePath(DIR_UI, _T("legion_stat_keep")));
}

//XWndGuildElem::XWndGuildElem(LPCTSTR szName, int nNum)
//	: XWndImage(TRUE, XE::MakePath(DIR_UI, _T("guild_search_box.png")), 0.f, 0.f)
//{
//	Init();
//
//	XWndImage *pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("guild_box_m.png")), 248.f, 0.f);
//	Add(pImg);
//
//	XWndTextString *pText = new XWndTextString(XE::VEC2(6, 10), szName, FONT_NANUM_BOLD, 30.f);
//	pText->SetLineLength(237.f);
//	pText->SetAlign(XE::xALIGN_HCENTER);
//	Add(pText);
//
//	pText = new XWndTextString(XE::VEC2(252, 10), XE::Format(XTEXT(80096), nNum), FONT_NANUM_BOLD, 30.f);
//	pText->SetLineLength(65.f);
//	pText->SetAlign(XE::xALIGN_HCENTER);
//	Add(pText);
//}

XWndGuildElem::XWndGuildElem(XGuild* pGuild, BOOL bReqJoin)
	: XWndImage(TRUE, XE::MakePath(DIR_UI, _T("guild_search_box.png")), 0.f, 0.f)
{
	Init();

	m_pGuild = pGuild;
	XBREAK(m_pGuild == nullptr);

	XWndImage *pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("guild_box_m.png")), 248.f, 0.f);
	Add(pImg);

	XWndTextString *pText = new XWndTextString(XE::VEC2(6, 10), m_pGuild->GetstrName(), FONT_NANUM_BOLD, 30.f);
	pText->SetLineLength(237.f);
	pText->SetAlign(XE::xALIGN_HCENTER);
	Add(pText);

	LPCTSTR szState = XE::Format(XTEXT(80096), m_pGuild->GetMemberCount());
	XCOLOR dwColor = XCOLOR_WHITE;

	if (m_pGuild->GetMaxMemberCount() <= m_pGuild->GetnumMembers())
	{
		szState = XTEXT(80097);
		dwColor = XCOLOR_RED;
		m_bJoin = FALSE;
	}

	if (bReqJoin == TRUE)
	{
		szState = XTEXT(80159);
		dwColor = XCOLOR_RED;
		m_bJoin = FALSE;
	}

	pText = new XWndTextString(XE::VEC2(252, 11), szState, FONT_NANUM_BOLD, 25.f);
	pText->SetColorText(dwColor);
	pText->SetLineLength(65.f);
	pText->SetAlign(XE::xALIGN_HCENTER);
	Add(pText);
}

void XWndGuildElem::Destroy()
{
	//SAFE_DELETE(m_pGuild);
}

XWndGuildMember::XWndGuildMember(XGuild::SGuildMember *pMember)
{
	Init();
//	SetSizeLocal( 287, 36 );
	m_pMember = pMember;
	XBREAK(m_pMember == nullptr);
	auto pImg = new XWndImage( TRUE, XE::MakePath( DIR_UI, _T( "guild_member_info.png" ) ), 0.f, 0.f );
	Add( pImg );
	auto pText = new XWndTextString(XE::VEC2(0, 0), m_pMember->m_strName.c_str(), FONT_MNLS, 30.f);
//	pText->SetLineLength(150.f);
	pText->SetAlign(XE::xALIGN_CENTER);
	pImg->Add(pText);
	pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("guild_box_s.png")), 152.f, 0.f);
	Add(pImg);
	pText = new XWndTextString(XE::VEC2(0, 0), XFORMAT("%d", m_pMember->m_Grade), FONT_NANUM_BOLD, 30.f);
//	pText->SetLineLength(57.f);
	pText->SetAlign(XE::xALIGN_CENTER);
	pImg->Add(pText);

	pImg = new XWndImage(TRUE, XE::MakePath(DIR_UI, _T("guild_box_m.png")), 213.f, 0.f);
	Add(pImg);
// 	pText = new XWndTextString(XE::VEC2(0, 0), XTEXT(80183), FONT_NANUM_BOLD, 30.f);
// //	pText->SetLineLength(72.f);
// 	pText->SetAlign(XE::xALIGN_CENTER);
// 	pImg->Add(pText);
	SetAutoSize();
}


XWndGuildJoinReqMember::XWndGuildJoinReqMember(XGuild::SGuildMember *pUser)
	: XWndImage(TRUE, XE::MakePath(DIR_UI, _T("guild_member_info.png")), 0.f, 0.f)
{
	Init();

	m_pUser = pUser;
	XBREAK(m_pUser == nullptr);

	XWndTextString *pText = new XWndTextString(XE::VEC2(0, 8), m_pUser->m_strName.c_str(), FONT_MNLS, 30.f);
	pText->SetLineLength(150.f);
	pText->SetAlign(XE::xALIGN_HCENTER);
	Add(pText);

	// 승인
	XWndButtonString *pButt = new XWndButtonString(152.f, 0.f, XTEXT(80160), XCOLOR_WHITE, FONT_NANUM_BOLD, 20.f, _T("guild_box_s.png"));
	//pButt->SetAlign(XE::xALIGN_HCENTER);
	pButt->SetEvent(XWM_CLICKED, GAME, &XSceneGuild::OnClickAcceptJoin, 0);
	Add(pButt);

	pButt = new XWndButtonString(213.f, 0.f, XTEXT(80161), XCOLOR_WHITE, FONT_NANUM_BOLD, 20.f, _T("guild_box_m.png"));
	//pButt->SetAlign(XE::xALIGN_HCENTER);
	pButt->SetEvent(XWM_CLICKED, GAME, &XSceneGuild::OnClickAcceptJoin, 1);
	Add(pButt);

	SetSizeLocal(287, 36);
}

XWndPopupDuplicateAcc::XWndPopupDuplicateAcc()
	: XWndPopup(0.f, 0.f, _T("guild_popup_bg.png"))
{
	Init();

	XWndImage *pImg = new XWndImage(true, XE::MakePath(DIR_UI, _T("guild_popup_box.png")), 54.f, 78.f);
	Add(pImg);

	XWndTextString *pText = new XWndTextString(3, 36, XTEXT(80170), FONT_NANUM_BOLD, 20.f);
	pText->SetLineLength(303.f);
	pText->SetStyle(xFONT::xSTYLE_STROKE);
	pText->SetAlign(XE::xALIGN_HCENTER);
	pImg->Add(pText);

	XWndButtonString *pButt = new XWndButtonString(160.f, 184.f, XTEXT(80009), XCOLOR_WHITE, FONT_NANUM_BOLD, 35.f, _T("common_butt_mid.png"));
	pButt->SetEvent(XWM_CLICKED, GAME, &XGame::OnExitApp);
	Add(pButt);

	SetbModal(true);
}

XWndPopupTextScroll::XWndPopupTextScroll(LPCTSTR szText)
	: XWndPopup(_T("layout_notice.xml"), "popup_notice")
{
	Init();
	SetbModal(true);
	m_pScroll = new XWndScrollView(30.f, 70.f, 456.f, 202.f);
	m_pScroll->SetScrollDir(XE::xVERT);
	Add(m_pScroll);

	_tstring strText = szText;
	XWndTextString *pText = new XWndTextString(XE::VEC2(0, 0), strText, FONT_NANUM_BOLD, 23.f);
	pText->SetLineLength(456.f);
	pText->SetAutoSize();
	m_pScroll->Add(pText);

	if (pText->GetSizeLocal().y > 202.f)
		m_pScroll->SetViewSize(XE::VEC2(456.f, pText->GetSizeLocal().y));
	xSET_BUTT_HANDLER(this, "butt.notice.ok", &XWndPopupTextScroll::OnClickOk);
}

int XWndPopupTextScroll::OnClickOk(XWnd *pWnd, DWORD p1, DWORD p2)
{
	SetbDestroy(true);
	return 1;
}

void XWndPopupGuildJoin::Destroy()
{
	SAFE_DELETE(m_pGuild);
}

XWndPopupGuildJoin::XWndPopupGuildJoin(XWndGuildElem *pElem, XLayout *pLayout, const char *cKey, const char *cGroup)
	: XWndPopup(pLayout, cKey, cGroup)
{
	Init();

	SetbModal(true);

	if (!XBREAK(pElem == nullptr))
	{
		//이거 멤버 idaccount가지고있다가 클릭할때마다 검색해서 찾는걸로 바꿔야함
		m_pGuild = new XGuild();
		XArchive ar;
		pElem->GetpGuild()->Serialize(ar);
		m_pGuild->DeSerialize(ar);
// 		if (pElem->GetbJoin() == FALSE)
// 		{
// 			xSET_ENABLE(this, "butt.guild.join", FALSE);
// 		}
		xSET_TEXT(this, "text.guild.name", m_pGuild->GetstrName());
		xSET_TEXT(this, "text.guild.master", m_pGuild->GetstrMasterName());
		xSET_TEXT(this, "text.guild.desc", m_pGuild->GetstrGuildContext());
	}
}

////////////////////////////////////////////////////////////////
_XWndPopupDaily::_XWndPopupDaily( XSpotDaily *pSpot )
	: XWndPopup(_T("popup_daily.xml"), "popup_daily_old")
{
	Init();
	XBREAK(pSpot == nullptr);
	m_pSpot = pSpot;
	SetbUpdate( true );
//	SetAutoUpdate( 0.1f );
	SetButtHander( this, "butt.battle", &_XWndPopupDaily::OnClickEnter );
	XE::xtDOW dowToday = XSYSTEM::GetDayOfWeek();
	m_dowToday = dowToday;
	m_timerAlpha.Set(1.f);
}

void _XWndPopupDaily::Update()
{
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		if( Find("butt.reset") == nullptr ) {
			auto pButt = new XWndButtonDebug( 19.f, 45.f, 50.f, 30.f, _T( "리셋" ) );
			pButt->SetstrIdentifier( "butt.reset" );
			pButt->SetEvent( XWM_CLICKED, this, &_XWndPopupDaily::OnClickReset );
			Add( pButt );
		}
	}
#endif // _CHEAT
	xSET_IMG( this, "img.elem.6", PATH_UI( "daily_elem_st.png" ) );
	xSET_IMG( this, "img.elem.7", PATH_UI( "daily_elem_su.png" ) );
	const XE::xtDOW aryDow[7] = { XE::xDOW_MONDAY, XE::xDOW_TUESDAY, XE::xDOW_WEDNESDAY, XE::xDOW_THURSDAY, XE::xDOW_FRIDAY, XE::xDOW_SATURDAY, XE::xDOW_SUNDAY };
//	float scale = 4.f;	// 테스트용
	//
	for( int i = 0; i < 7; ++i ) {
		auto dow = aryDow[i];
		auto pWnd = Findf("img.elem.%d", i+1);
		if( pWnd ) {
			auto pTextDow = xSET_TEXT( pWnd, "text.dow", XTEXT(80194+i) );
			if( pTextDow && dow == m_dowToday ) {
				auto col = XGAME::xGetColorPower( m_pSpot->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
				pTextDow->SetColorText( col );
			}
			xSET_SHOW( pWnd, "spr.glow", dow == m_dowToday );
			// 드랍아이템
// 			auto reward = XSpot::sGetRewardDaily( m_pSpot->GetpProp(), dow, ACCOUNT->GetLevel(), &aryReward );
			XVector<XGAME::xReward> aryReward;
			XSpotDaily::sGetRewardDaily( m_pSpot->GetpProp(), dow, ACCOUNT->GetLevel(), &aryReward );
			if( aryReward.size() ) {
				if( aryReward[0].IsTypeResource() ) {
					auto pWndRes = new XWndResourceCtrl( XE::VEC2( 6, 31 ) );
					pWndRes->SetScaleLocal( 0.8f );
					pWndRes->AddResWithAry( aryReward );
					pWnd->Add( pWndRes );
				} else {
					for( auto& reward : aryReward ) {
						if( reward.rewardType == XGAME::xtReward::xRW_ITEM ) {
			//				ID idItem = XSpot::sGetIdDailyDropItem( dow );
							auto pProp = PROP_ITEM->GetpProp( reward.GetidItem() );
							if( XASSERT(pProp) ) {
								if( pWnd->Find( "img.reward" ) )
									pWnd->Find( "img.reward" )->SetbDestroy( TRUE );
								auto pItem = new XWndStoragyItemElem( (ID)pProp->idProp );
								pItem->SetstrIdentifier( "img.reward" );
								pItem->SetPosLocal( 4.f, 35.f );
								pItem->SetNum( reward.num );
								pItem->SetEventItemTooltip();
								pWnd->Add( pItem );
								if( dow == m_dowToday ) {
									auto pWndGlow = xGET_IMAGE_CTRL( pWnd, "img.glow" );
									if( pWndGlow ) {
										XE::VEC2 v(-7);
										pWndGlow->SetPosLocal( v );
			//							pWndGlow->SetEffect( xDM_SCREEN );
										pWndGlow->SetblendFunc( XE::xBF_ADD );
										pWndGlow->SetbShow( TRUE );
									}
									auto pButt = xGET_BUTT_CTRL( this, "butt.battle" );
									if( pButt ) {
										XE::VEC2 v(0,95);
										auto vElem = pWnd->GetPosLocal();
										pButt->SetPosLocal( vElem + v );
										if( m_pSpot->IsAttackable( ACCOUNT ) )
											pButt->SetbEnable( true );
										else
											pButt->SetbEnable( false );
									}
								}
							}
						} // if( reward.rewardType == XGAME::xtReward::xRW_ITEM ) {
					} // for( auto& reward : aryReward ) {
				} // not resource reward
			} // if( aryReward.size() )
// 			for( auto& reward : aryReward ) {
// 				if( reward.rewardType == XGAME::xtReward::xRW_RESOURCE ) {
// 					auto pWndRes = new XWndResourceCtrl( XE::VEC2(6,31) );
// 	// 				pWndRes->SetScaleLocal( scale );
// 	// 				scale *= 0.5f;
// 					pWnd->Add( pWndRes );
// 					switch( reward.GetResourceType() )
// 					{
// 					case XGAME::xRES_ALL: {
// 						pWndRes->SetScaleLocal( 0.8f );
// 						pWndRes->AddRes( XGAME::xRES_WOOD, reward.num );
// 						pWndRes->AddRes( XGAME::xRES_IRON, reward.num );
// 						pWndRes->AddRes( XGAME::xRES_JEWEL, reward.num );
// 						pWndRes->AddRes( XGAME::xRES_SULFUR, reward.num );
// 						pWndRes->AddRes( XGAME::xRES_MANDRAKE, reward.num );
// 					} break;
// 					case XGAME::xRES_WOOD_IRON: {
// 						pWndRes->AddRes( XGAME::xRES_WOOD, reward.num );
// 						pWndRes->AddRes( XGAME::xRES_IRON, reward.num );
// 					} break;
// 					default:
// 						pWndRes->AddRes( reward.GetResourceType(), reward.num );
// 						break;
// 					}
// 				} else
// 				if( reward.rewardType == XGAME::xtReward::xRW_ITEM ) {
// 	//				ID idItem = XSpot::sGetIdDailyDropItem( dow );
// 					auto pProp = PROP_ITEM->GetpProp( reward.GetidItem() );
// 					if( XASSERT(pProp) ) {
// 						if( pWnd->Find( "img.reward" ) )
// 							pWnd->Find( "img.reward" )->SetbDestroy( TRUE );
// 						auto pItem = new XWndStoragyItemElem( (ID)pProp->idProp );
// 						pItem->SetstrIdentifier( "img.reward" );
// 						pItem->SetPosLocal( 4.f, 35.f );
// 						pItem->SetNum( reward.num );
// 						pItem->SetEventItemTooltip();
// 						pWnd->Add( pItem );
// 						if( dow == m_dowToday ) {
// 							auto pWndGlow = xGET_IMAGE_CTRL( pWnd, "img.glow" );
// 							if( pWndGlow ) {
// 								XE::VEC2 v(-7);
// 								pWndGlow->SetPosLocal( v );
// 	//							pWndGlow->SetEffect( xDM_SCREEN );
// 								pWndGlow->SetblendFunc( XE::xBF_ADD );
// 								pWndGlow->SetbShow( TRUE );
// 							}
// 							auto pButt = xGET_BUTT_CTRL( this, "butt.battle" );
// 							if( pButt ) {
// 								XE::VEC2 v(0,95);
// 								auto vElem = pWnd->GetPosLocal();
// 								pButt->SetPosLocal( vElem + v );
// 								if( m_pSpot->IsAttackable( ACCOUNT ) )
// 									pButt->SetbEnable( true );
// 								else
// 									pButt->SetbEnable( false );
// 							}
// 						}
// 					}
// 				} // if( reward.rewardType == XGAME::xtReward::xRW_ITEM ) {
// 			} // for( auto& reward : aryReward ) {
		}
	}
	// 도전횟수 마크
	int numRemain = _XGC->m_numEnterDaily - m_pSpot->GetnumEnter();
	for( int i = 0; i < numRemain; ++i ) {
		auto pMark = xGET_IMAGE_CTRLF( this, "img.mark.%d", i+1 );
		if( pMark ) {
			pMark->SetSurfaceRes( PATH_UI("chall_mark_on.png") );
		}
	}
	XWndPopup::Update();
}

int _XWndPopupDaily::Process( float dt )
{
	int aryIdxDow[ 7 ] = {7, 1, 2, 3, 4, 5, 6};
	auto pElem = Findf( "img.elem.%d", aryIdxDow[ m_dowToday ] );
	if( pElem ) {
		auto pWndGlow = xGET_IMAGE_CTRL( pElem, "img.glow" );
		if( pWndGlow ) {
			pWndGlow->SetbShow( true );
			auto lerpTime = m_timerAlpha.GetSlerp();
			float a = XE::xiCos( lerpTime, 0.3f, 1.f, 0 );
			pWndGlow->SetAlphaLocal( a );
		}
	}
	if( m_timerAlpha.IsOver() )
		m_timerAlpha.Reset();
	return XWndPopup::Process( dt );
}

void _XWndPopupDaily::OnAutoUpdate()
{
// 	int secRemain = (int)m_pSpot->GettimerEnter().GetsecRemainTime();	// 남은시간(초)
// 	int hour, min, sec;
// 	XTimer2::sGetHourMinSec(secRemain, &hour, &min, &sec);
// 	if( hour > 0 )
// 		xSET_TEXT(this, "text.rest.time", XE::Format(XTEXT(80193), hour, min, sec));
// 	else
// 		xSET_TEXT( this, "text.rest.time", XE::Format( XTEXT( 80208 ), min, sec ) );
}

/****************************************************************
* @brief 
*****************************************************************/
int _XWndPopupDaily::OnClickReset( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickReset");
	//
#ifdef _CHEAT
	m_pSpot->SetnumEnter( 0 );
	GAMESVR_SOCKET->SendCheat( GAME, 22, m_pSpot->GetidSpot() );
#endif // _CHEAT
	return 1;
}

int _XWndPopupDaily::OnClickDow( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickDow:%d", p1);
	//
//	m_dowSelected = (int)p1;
//	SetbUpdate( true );
// 	auto pPopup = new XWndPopupDailyCamp( m_pSpot );
// 	GAME->Add( pPopup );
// 	pPopup->SetbModal( TRUE );
	return 1;
}

int _XWndPopupDaily::OnClickEnter( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickEnter");
	//
	if( SCENE_WORLD )
		SCENE_WORLD->OnAttackSpot( nullptr, m_pSpot->GetidSpot(), 0 );
	// 일단은 캠페인없이 하기로 하고 CPR버전 다 구현되면 캠페인방식으로 바꾼다.
// 	auto pPopup = new XWndPopupDailyCamp( m_pSpot );
// 	pPopup->SetbModal( TRUE );
// 	GAME->Add( pPopup );
	return 1;
}

//////////////////////////////////////////////////////////////////////////
XWndPopupSpotMenu::XWndPopupSpotMenu( XSpot *spBaseSpot, int p1, int p2, int p3, int p4 )
	: XWndPopup(_T("popup_spot.xml"), "popup_spot")
{
	Init();
	if (XBREAK(SCENE_WORLD == nullptr))
		return;
	XBREAK( spBaseSpot == nullptr );
	m_spBaseSpot = spBaseSpot;
	m_idSpot = spBaseSpot->GetidSpot();
	SetstrIdentifier( "menu.circle" );
	SetbModal(TRUE);
	UpdateSpotImg();		// 상단 스팟 이미지
	const auto typeSpot = spBaseSpot->GettypeSpot();
	switch( typeSpot )
	{
	case XGAME::xSPOT_SULFUR:
		m_bViewLog = ( spBaseSpot->IsActive() == FALSE );		// deActive상태면 일단 로그보여주는 모드를 기본으로.
		GetpLayout()->CreateLayout( "sub_reward", this );
		GetpLayout()->CreateLayout( "sub_sulfur", this );
		break;
	case XGAME::xSPOT_MANDRAKE:
		GetpLayout()->CreateLayout( "sub_mandrake", this );
		break;
	default:
		GetpLayout()->CreateLayout( "sub_reward", this );
		GetpLayout()->CreateLayout( "sub_common", this );
		break;
	}
//	xCampaign::XCampObj::s_idxStage = p1;	// 캠페인스팟의경우 스테이지번호로도 쓰임.
	SetbUpdate(true);
}

void XWndPopupSpotMenu::Destroy()
{
	XCLEAR_ARRAY( m_nParam );
	xCampaign::XCampObj::s_idxStage = -1;	// 이 값은 팝업이 떠있을동안만 유효한것임.
	xCampaign::XCampObj::s_idxFloor = 0;
	if( m_idClose ) {
		XWnd *pTop = GetWndTop();
		pTop->DestroyID( m_idClose );
	}
}
/**
 @brief 상단 스팟이미지 업데이트.
*/
void XWndPopupSpotMenu::UpdateSpotImg()
{
	auto spBaseSpot = m_spBaseSpot;
	// 상단에 스팟 이미지 표시
	auto pWndSpr = xGET_SPROBJ_CTRL( this, "spr.spot" );
	if( pWndSpr ) {
		const auto typeSpot = spBaseSpot->GettypeSpot();
		switch( typeSpot )
		{
		case XGAME::xSPOT_CASTLE:
			if( spBaseSpot->IsEnemy() )
				pWndSpr->SetSprObj( SPR_CASTLE );
			else
				pWndSpr->SetSprObj( SPR_MY_CASTLE );
			break;
		case XGAME::xSPOT_JEWEL:	pWndSpr->SetSprObj( SPR_JEWEL );	break;
		case XGAME::xSPOT_SULFUR:	pWndSpr->SetSprObj( SPR_SULFUR );	break;
		case XGAME::xSPOT_MANDRAKE:	pWndSpr->SetSprObj( SPR_MANDRAKE );	break;
		case XGAME::xSPOT_NPC:		pWndSpr->SetSprObj( SPR_NPC );	break;
		case XGAME::xSPOT_DAILY:	pWndSpr->SetSprObj( SPR_DAILY );	break;
		case XGAME::xSPOT_SPECIAL:	pWndSpr->SetSprObj( SPR_SPECIAL );	break;
		case XGAME::xSPOT_CAMPAIGN:	pWndSpr->SetSprObj( SPR_CAMPAIGN );	break;
		case XGAME::xSPOT_VISIT:	pWndSpr->SetSprObj( SPR_VISIT );	break;
		case XGAME::xSPOT_CASH:		
		case XGAME::xSPOT_GUILD_RAID:	
		case XGAME::xSPOT_PRIVATE_RAID:
		case XGAME::xSPOT_COMMON:	
			break;
		default:
			XBREAK(1);
			break;
		}
	}
}
/**
 @brief 
*/
void XWndPopupSpotMenu::Update()
{
	if (XBREAK(SCENE_WORLD == nullptr))
		return;
	auto pBaseSpot = m_spBaseSpot;
	XBREAK( pBaseSpot == nullptr );	// 널인경우도 있음?
	if( pBaseSpot == nullptr )
		return;
	const bool bEmpty = pBaseSpot->IsEmpty();
	// 모든 스팟 공통 업데이트.
	UpdateLevel( true );	// 레벨
	UpdateSpotName( true );	// 스팟 이름
	UpdateButtons();	// 공통 버튼들 업데이트
	const auto typeSpot = pBaseSpot->GettypeSpot();
	switch( typeSpot )
	{
	case XGAME::xSPOT_CASTLE:
		UpdateForCastle();
		break;
	case XGAME::xSPOT_JEWEL:
		UpdateForJewel();
		break;
	case XGAME::xSPOT_SULFUR:
		UpdateForSulfur();
		break;
	case XGAME::xSPOT_MANDRAKE:
		UpdateForMandrake();
		break;
	default:
		UpdateForCommon();
		break;
	}
#ifdef _CHEAT
	UpdateDebugInfo();	// 디버깅 정보.		
#endif // _CHEAT
}

/**
 @brief 자원,아이템 보상 업데이트.
*/
void XWndPopupSpotMenu::UpdateReward( bool bShow )
{
	auto pRoot = Find( "wnd.root.reward" );
	if( !pRoot )
		return;
	pRoot->SetbShow( bShow );
	if( bShow ) {
		auto spBaseSpot = m_spBaseSpot;
		if( XASSERT(spBaseSpot) ) {
			// 루팅가능한 자원목록
			if( !Find("ctrl.res.reward") ) {
				auto pRootRes = pRoot->Find( "wnd.root.reward.res" );
				if( pRootRes ) {
					pRootRes->SetbShow( true );
// 				auto pWndRes = new XWndResourceCtrl( XE::VEC2( 29, 72 ) );
					auto pWndRes = new XWndResourceCtrl( XE::VEC2( 0, 0 ) );
					pWndRes->SetbSymbolNumber( true );		// 큰숫자는 기호로
					pWndRes->SetstrIdentifier( "ctrl.res.reward" );
					pRootRes->Add( pWndRes );
					for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
						auto type = ( XGAME::xtResource )i;
						int loot = spBaseSpot->GetLootAmount( type );
						XBREAK( loot < 0 );
						if( loot > 0 )
							pWndRes->AddRes( type, loot );
					}
					pWndRes->AutoLayoutCenter();
				}
			}
		}
		UpdateDropReward();							// 드랍 보상품 업데이트.
	}
}

/**
 @brief 유황/만드레이크를 제외한 다른 스팟의 레이아웃 업데이트
*/
void XWndPopupSpotMenu::UpdateForCommon()
{
	auto pBaseSpot = m_spBaseSpot;
	const bool bActive = (pBaseSpot->IsActive() != FALSE);
//	const bool bEmpty = pBaseSpot->IsEmpty();
	auto pRootTop = Find( "wnd.top.root" );
	if( pRootTop ) {
		// 스팟이 비어있는 상태면
		pRootTop->SetbShow( bActive );
		if( bActive ) {
			UpdatePower( pRootTop );		// 전투력
			UpdateScore( pRootTop );		// 레더점수
			UpdateAP( pRootTop, true );				// AP
		}
	}
	UpdateNumLose( this, bActive );							// 최근전적
	UpdateReward( bActive );										// 자원/아이템 보상
	UpdateRemainRegenTime( this, !bActive );		// 리젠 남은시간.
	auto pRootCommon = 
	xSET_SHOW( this, "wnd.root.common", true );
	auto pRootActive = xSET_SHOW( pRootCommon, "wnd.root.active", bActive );
	auto pRootDeActive = xSET_SHOW( pRootCommon, "wnd.root.deactive", !bActive );
// 	if( bActive ) {
// 	} else {
// 	}
}

void XWndPopupSpotMenu::UpdateForCastle()
{
	auto pSpot = SafeCast<XSpotCastle*>( m_spBaseSpot );
	if( pSpot == nullptr ) 
		return;
	const auto state = pSpot->GetState( ACCOUNT->GetidAccount() );
	auto pRootTop = Find( "wnd.top.root" );
	if( !pRootTop )
		return;
	// no match상태만 아니면 보여줌.	
	const bool bShow = (state == xSpot::xSC_ENEMY);
	pRootTop->SetbShow( bShow );		// 루트만 on/off함.
	if( bShow ) {
		UpdatePower( pRootTop );		// 전투력
		UpdateScore( pRootTop );		// 레더점수
		UpdateAP( pRootTop, true );				// AP
	}
	UpdateNumLose( this, state == xSpot::xSC_ENEMY );							// 최근전적
	UpdateReward( state == xSpot::xSC_ENEMY );										// 자원/아이템 보상
	UpdateRemainRegenTime( this, state == xSpot::xSC_MY );				// 리젠 남은시간.
	auto pRootSub = 
	xSET_SHOW( this, "wnd.root.castle", true );
	auto pRootActive = xSET_SHOW( pRootSub, "wnd.root.active", state == xSpot::xSC_ENEMY );
	auto pRootDeActive = xSET_SHOW( pRootSub, "wnd.root.deactive", state != xSpot::xSC_ENEMY );
	if( pRootDeActive ) {
		const bool bMyCastle = (state == xSpot::xSC_MY);
		// 성이 내것.
		auto pWndProduce = 
		xSET_SHOW( pRootDeActive, "wnd.product", bMyCastle );
		if( bMyCastle ) {
			auto pProp = pSpot->GetpProp();
			if( pProp ) {
				int idx = 0;
				for( auto& res : pProp->m_aryProduce ) {
					auto productPerH = res.num * 60.f;
					auto pWnd = Findf("module.res%d", idx+1);
					if( pWnd ) {
						pWnd->SetbShow( true );
						const _tstring strFormat = XTEXT(2036);
						xSET_TEXT_FORMAT( pWnd, "text.product.per", _T("%s/h"), XE::NtS( (int)productPerH ) );
						xSET_SPR( pWnd, "spr.res", XGAME::GetResourceSpr(res.type), 1 );
						auto pBar = SafeCast<XWndProgressBar2*>( pWnd->Find("pbar.main") );
						if( pBar ) {
							const auto currStock = pSpot->GetLocalStorageAmount( res.type );
							const auto maxStock = pSpot->GetLocalStorageMax( res.type );
							float lerp = currStock / maxStock;
							if( lerp < 0 )
								lerp = 0;
							if( lerp > 1.f )
								lerp = 1.f;
							pBar->SetLerp( lerp );
						}
					}
					++idx;
				}

// 				auto productPerH = pProp->GetProduce() * 60.f;
// 				xSET_TEXT_FORMAT( pWndProduce, "text.product"
// 					, _T( "%s/%s" ), XE::NtS( (int)productPerH ), XTEXT( 2231 ) );
			}
		}
	}
}

void XWndPopupSpotMenu::UpdateForJewel()
{
	auto pSpot = SafeCast<XSpotJewel*>( m_spBaseSpot );
	const bool bEnemy = !pSpot->IsMySpot( ACCOUNT->GetidAccount() );
//	const bool bEmpty = pBaseSpot->IsEmpty();
	auto pRootTop = Find( "wnd.top.root" );
	if( pRootTop ) {
		pRootTop->SetbShow( bEnemy );
		if( bEnemy ) {
			UpdatePower( pRootTop );		// 전투력
			UpdateScore( pRootTop );		// 레더점수
			UpdateAP( pRootTop, true );				// AP
		}
	}
	UpdateNumLose( this, bEnemy );							// 최근전적
	UpdateReward( bEnemy );										// 자원/아이템 보상
	UpdateRemainRegenTime( this, !bEnemy );		// 리젠 남은시간.
	auto pRootCommon = 
	xSET_SHOW( this, "wnd.root.common", true );
	auto pRootEnemy = xSET_SHOW( pRootCommon, "wnd.root.active", bEnemy );
	auto pRootMy = xSET_SHOW( pRootCommon, "wnd.root.deactive", !bEnemy );
	// 스팟이 적군것일때.
	if( pRootEnemy && bEnemy ) {
		{
			auto pRoot = pRootEnemy->Find( "wnd.product.enemy" );
			if( pRoot == nullptr ) {
				GetpLayout()->CreateLayout( "module_product_enemy", pRootEnemy );
				pRoot = pRootEnemy->Find( "wnd.product.enemy" );
			}
			if( pRoot ) {
				auto pProp = pSpot->GetpProp();
				if( pProp ) {
					auto productPerH = pProp->GetProduce() * 60.f;
					xSET_TEXT_FORMAT( pRoot, "text.product", _T( "%s/%s" ), XE::NtS( (int)productPerH ), XTEXT( 2231 ) );
				}
			}
		}
		{
			// 보석광산의 방어도 보너스컨트롤 갱신
			auto pRootRes = Find( "wnd.root.reward.res" );
		
			UpdateJewelDefenseCtrl( pSpot, pRootRes, !bEnemy );
		}
	}
	// 스팟이 우리것일때
	if( pRootMy && !bEnemy ) {
		xSET_SHOW( pRootMy, "wnd.product", true );
		auto pProp = pSpot->GetpProp();
		if( pProp ) {
			auto productPerH = pProp->GetProduce() * 60.f;
			xSET_TEXT_FORMAT( pRootMy, "text.product", _T( "%s/%s" ), XE::NtS( (int)productPerH ), XTEXT( 2231 ) );
			auto curr = pSpot->GetLocalStorageAmount();
			xSET_TEXT_FORMAT( pRootMy, "text.have", _T( "%s" ), XE::NtS( (int)curr ) );
			
			UpdateJewelDefenseCtrl( pSpot, pRootMy, !bEnemy );
		}
	}
}

/**
 @brief 광산스팟의 방어도 보너스 표기를 업데이트한다.
*/
void XWndPopupSpotMenu::UpdateJewelDefenseCtrl( XSpotJewel* pSpot, XWnd* pRoot, bool bMy )
{
	if( pRoot ) {
		pRoot->SetbShow( true );
		auto pRootDefense = pRoot->Find( "wnd.defense" );
		if( pRootDefense ) {
			pRootDefense->SetbShow( true );
			auto pProp = pSpot->GetpProp();
			if( pProp ) {
				int bonus = (int)( pSpot->GetBonusHPRateByDefense() * 100.f );
				auto pText
					= xSET_TEXT_FORMAT( pRootDefense, "text.bonus", _T( "+%d%%" ), bonus );
				if( pText ) {
					pText->SetColorText( (bMy)? XCOLOR_WHITE : XCOLOR_RED );
				}
			}
		}
	}
}
/**
 @brief 유황스팟용 업데이트.
*/
void XWndPopupSpotMenu::UpdateForSulfur()
{
	auto pBaseSpot = m_spBaseSpot;
	XBREAK( pBaseSpot->GettypeSpot() != XGAME::xSPOT_SULFUR );
	const bool bActive = ( m_spBaseSpot->IsActive() != FALSE );
	auto pRootTop = Find( "wnd.top.root" );
	if( pRootTop ) {
		pRootTop->SetbShow( bActive );
		if( bActive ) {
			UpdatePower( pRootTop );		// 전투력
			UpdateScore( pRootTop );		// 레더점수
			UpdateAP( pRootTop, true );				// AP
		}
	}
	UpdateNumLose( this, bActive );							// 최근전적
	UpdateReward( !m_bViewLog );					// 자원/아이템 보상. 로그모드에선 끔.
	UpdateRemainRegenTime( this, !bActive );		// 리젠 남은시간.
	//
	auto pRootSulfur = Find( "wnd.root.sulfur" );
	if( pRootSulfur ) {
		pRootSulfur->SetbShow( true );
		auto pRoot = 
		xSET_SHOW( this, "wnd.root.log", m_bViewLog );
		if( pRoot ) {
			bool bHaveLog = false;
			auto pSpot = SafeCast<XSpotSulfur*>( pBaseSpot );
			if( pSpot ) {
				if( pSpot->IsHaveLog() )
					bHaveLog = true;
			}
			if( m_bViewLog ) {
				if( bHaveLog ) {
					// 약탈로그 텍스트를 넣음.
					auto pWndScrl = xGET_SCROLLVIEW_CTRL( pRoot, "scrl.log" );
					if( pWndScrl ) {
						pWndScrl->DestroyChildAll();
						auto& aryEnc = pSpot->GetaryEncounter();
						XE::VEC2 v(0,3);
						for( const auto& enc : aryEnc ) {
							auto secPass = XTimer2::sGetTime() - enc.m_secEncount;
							_tstring strTime = XGAME::GetstrResearchTime( secPass );
// 							_tstring strMsg = XFORMAT("%s전:\n▶ %s의 유황을 %s가로챔.", strTime.c_str()
// 																																			, enc.m_strDefender.c_str()
// 																																			, XE::NumberToMoneyString(enc.m_numSulfur) );
							_tstring strMsg = XFORMAT("%s:\n▶ %s%s %s", strTime.c_str()
																												, XTEXT(2334)
																												, enc.m_strDefender.c_str()
																												, XE::NumberToMoneyString(enc.m_numSulfur) );
							auto pText = new XWndTextString( v, strMsg, FONT_MNLS, 18.f );
							pWndScrl->Add( pText );
							auto sizeText = pText->GetSizeNoTransLayout();
							v.y += sizeText.h + 2.f;
						}
						if( !pSpot->GetbCheckEncounterLog() ) {
							GAMESVR_SOCKET->SendCheckEncounterLog( m_spBaseSpot->GetidSpot() );
							pSpot->SetbCheckEncounterLog( true );
						}
					}
				} // haveLog
			} else {
		
			}
			//
			{
				auto pWnd = Find( "butt.circle.log.sulfur" );
				if( pWnd ) {
					bool bShowButt = (bActive && bHaveLog);
					pWnd->SetbShow( bShowButt );	// 
					pWnd->SetbEnable( bHaveLog );
				} else
					SetbUpdate( true );		// 버튼이 나중에 추가되는 구조라 한번더 업데이트 시켜야 버튼이 나옴.
			}
		}
	}
}

void XWndPopupSpotMenu::UpdateForMandrake()
{
	auto pSpot = SafeCast<XSpotMandrake*>( m_spBaseSpot );
	if( XBREAK(pSpot == nullptr) )
		return;
	const auto state = pSpot->GetState( ACCOUNT->GetidAccount() );
	auto pRootTop = Find( "wnd.top.root" );
	if( pRootTop ) {
		bool bShowTop = (state != xSpot::xSM_NO_MATCH);
		pRootTop->SetbShow( bShowTop );
		if( bShowTop ) {
			UpdatePower( pRootTop );		// 전투력
			UpdateScore( pRootTop );		// 레더점수
			UpdateAP( pRootTop, false );				// AP
		}
	}
	UpdateNumLose( this, false );							// 최근전적
	UpdateReward( false );					// 자원/아이템 보상. 로그모드에선 끔.
	UpdateRemainRegenTime( this, false );		// 리젠 남은시간.
	UpdateLevel( state != xSpot::xSM_NO_MATCH );
	auto pRootMandrake = Find( "wnd.root.mandrake" );
	if( XASSERT( pRootMandrake ) ) {
		pRootMandrake->SetbShow( true );
		switch( state )
		{
		case xSpot::xSM_NO_MATCH: {
			ClearAutoUpdate();
			auto pRootSub = 
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.empty", true );
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.enemy", false );
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.my", false );
			const bool bShowOffReward = pSpot->GetReward() > 0;
			auto pRootOff = xSET_SHOW( pRootSub, "wnd.offwin.reward", bShowOffReward );
			if( bShowOffReward ) {
				xSET_TEXT_FORMAT( pRootOff, "text.win", XTEXT(2269), pSpot->GetWin() );	// x연승보상
				xSET_TEXT( pRootOff, "text.reward", XE::NtS( pSpot->GetReward() ) );		// 총 연승보상값.
			}

		} break;
		case xSpot::xSM_CHALLENGE: {
			ClearAutoUpdate();
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.empty", false );
			auto pRootSub =
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.enemy", true );
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.my", false );
			xSET_TEXT( pRootSub, "text.win", XE::Format( XTEXT(2049), pSpot->GetWin() ) ) ;	// xx승
			xSET_TEXT( pRootSub, "text.win.reward", XE::NtS(pSpot->GetReward()) );	// 승리시보상양
		} break;
		case xSpot::xSM_DEFENSE: {
			SetAutoUpdate( 1.f );
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.empty", false );
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.enemy", false );
			auto pRootSub = 
			xSET_SHOW( pRootMandrake, "wnd.root.mandrake.my", true );
			const bool bShowWin = (pSpot->GetWin() > 0);
			xSET_SHOW( pRootSub, "text.win", bShowWin );
			xSET_SHOW( pRootSub, "img.win", bShowWin );
			xSET_TEXT( pRootSub, "text.win", XE::Format( XTEXT( 2049 ), pSpot->GetWin() ) );	// xx승
			xSET_TEXT( pRootSub, "text.reward", XE::NtS( pSpot->GetReward() ) );	// 현재보상
			xSET_TEXT( pRootSub, "text.reward.next", XE::NtS( pSpot->GetReward() * 2 ) );	// 현재보상
// 			const int producePerMin = (int)pSpot->GetpProp()->GetProduce();
// 			const auto max = pSpot->GetlsLocal().maxSize;
// 			const auto curr = pSpot->GetlsLocal().numCurr;
// 			_tstring str = XFORMAT( "%s/%s", XE::NtS(producePerMin), XTEXT(2267) );
// 			if( XAPP->m_bDebugMode ) {
// 				str += XFORMAT("(%d/%d)", (int)curr, (int)max );
// 			}
// 			xSET_TEXT( pRootSub, "text.produce.per.min", str );
// 			auto pWndBar = SafeCast2<XWndProgressBar2*>( pRootSub->Find( "pbar.produce" ) );
// 			if( pWndBar ) {
// 				pWndBar->SetLerp( curr / max );
// 			}
		} break;
		default:
			XBREAK(1);
			break;

		}
	}
}

void XWndPopupSpotMenu::OnAutoUpdate()
{
	auto pBaseSpot = m_spBaseSpot;
	const auto typeSpot = pBaseSpot->GettypeSpot();
	switch( typeSpot ) {
	case XGAME::xSPOT_MANDRAKE: {
		auto pSpot = SafeCast<XSpotMandrake*>( pBaseSpot );
		if( pSpot ) {
			const auto state = pSpot->GetState( ACCOUNT->GetidAccount() );
			//
			if( state == xSpot::xSM_DEFENSE ) {
				const int producePerMin = (int)pSpot->GetpProp()->GetProduce();
				const auto max = pSpot->GetlsLocal().maxSize;
				const auto curr = pSpot->GetlsLocal().numCurr;
				_tstring str = XFORMAT( "%s/%s", XE::NtS(producePerMin), XTEXT(2267) );
#ifdef _CHEAT
				if( XAPP->m_bDebugMode ) {
					str += XFORMAT("(%d/%d)", (int)curr, (int)max );
				}
#endif // _CHEAT
				auto pRootSub = Find("wnd.root.mandrake.my");
				if( pRootSub ) {
					xSET_TEXT( pRootSub, "text.produce.per.min", str );
					auto pWndBar = SafeCast2<XWndProgressBar2*>( pRootSub->Find( "pbar.produce" ) );
					if( pWndBar ) {
						pWndBar->SetLerp( curr / max );
					}
				}
			}
			{
				// 보상획득.
				auto pButt = Find("butt.circle.withdraw");
				if( pButt ) {
					auto pText = xGET_TEXT_CTRL( pButt, "text.mandrake.get.reward" );
					if( pText ) {
						pText->SetText( XE::NtS( pSpot->GetReward()) );
						const auto sizeButt = pButt->GetSizeLocal();
						pText->SetLineLength( sizeButt.w );
						pText->SetAlignHCenter();
						pText->SetY( sizeButt.h - 5.f );
					}
				}
			}
		}
	} break;
	}
}

void XWndPopupSpotMenu::UpdateForNpc()
{
}

/**
 @brief 드랍 보상(아이템)
*/
void XWndPopupSpotMenu::UpdateDropReward()
{
	if( !m_spBaseSpot->IsActive() )
		return;
	auto pRoot = Find("wnd.root.reward");	// 활성화상태 루트UI
	XBREAK( pRoot == nullptr );
	// 드랍가능 아이템목록
	int idx = 0;
	for( auto& item : m_aryItems ) {
		if( Findf("img.item.%d", idx) == nullptr ) {
			ID idItem = item.idDropItem;
			int num = item.num;
			auto pPropItem = PROP_ITEM->GetpProp( idItem );
			if( XASSERT( pPropItem ) ) {
				auto pWndItem = new XWndStoragyItemElem( idItem );
				pWndItem->SetScaleLocal( 0.47f );
				pWndItem->SetstrIdentifierf( "img.item.%d", idx );
				pWndItem->SetNum( num );
				pRoot->Add( pWndItem );
				auto vSize = pWndItem->GetSizeFinal();
				XE::VEC2 vPos = {107 + idx * vSize.w, 97};
				pWndItem->SetPosLocal( vPos );
				pWndItem->SetEventItemTooltip();
#ifdef _CHEAT
				if( XAPP->m_bDebugMode ) {
					_tstring strProb = XFORMAT( "%.2f", item.chance * 100.f );
					auto pText = new XWndTextString( vPos + XE::VEC2( 0, 24 ), strProb, FONT_SYSTEM, 15.f );
					pRoot->Add( pText );
				}
#endif // _CHEAT
			}
		}
		++idx;
	}
}

/**
 @brief 전투력 업데이트.
*/
void XWndPopupSpotMenu::UpdatePower( XWnd *pRoot )
{
	auto pBaseSpot = m_spBaseSpot;
	XBREAK( pRoot == nullptr );
	XASSERT( pRoot->GetbShow() );
	if( ACCOUNT->GetidAccount() == pBaseSpot->GetidOwner() ) {
		m_Power = ACCOUNT->GetPowerIncludeEmpty();
	} else {
		m_Power = pBaseSpot->GetPower();
	}
	auto pText = xGET_TEXT_CTRL( pRoot, "text.spot.power" );	// 전투력
	if( pText ) {
		if( m_Power > 0 ) {
#ifdef _CHEAT
			if( XAPP->m_bDebugMode ) {
				int bp = m_Power;
				float mul = (float)bp / ACCOUNT->GetPowerExcludeEmpty();
				pText->SetText( XFORMAT( "%s(%.2f)", XE::NumberToMoneyString( bp ), mul ) );
			} else
#endif
				pText->SetText( XE::NtS( m_Power ) );
		} else {
			pText->SetText( _T( "?" ) );
		}
// 			pText->SetbShow( true );
	}
}

void XWndPopupSpotMenu::UpdateLevel( bool bShow )
{
	auto pBaseSpot = m_spBaseSpot;
	if( pBaseSpot->IsEnemy() )
		m_Level = pBaseSpot->GetLevel();
	else
		m_Level = ACCOUNT->GetLevel();
	auto pText = xSET_TEXT( this, "text.spot.level"
																	, XE::Format( _T( "%d" ), m_Level ) );
	if( pText )
		pText->SetbShow( m_spBaseSpot->IsActive() );
}

void XWndPopupSpotMenu::UpdateScore( XWnd *pRoot )
{
	auto pBaseSpot = m_spBaseSpot;
	XBREAK( pRoot == nullptr );
	XASSERT( pRoot->GetbShow() );
	int score = 0;
	if( ACCOUNT->GetidAccount() == pBaseSpot->GetidOwner() ) {
		score = ACCOUNT->GetLadder();
	} else {
		if( pBaseSpot->IsReconed() )
			score = pBaseSpot->GetScore();
		else
			score = -1;
	}
	// pc스팟일때만 표시
	xSET_SHOW( pRoot, "img.spot.trophy", !pBaseSpot->IsNpc() );
	if( pBaseSpot->IsNpc() )
		return;
	auto pText = xGET_TEXT_CTRL(pRoot, "text.spot.trophy");
	if (pText) {
		if( score >= 0 )
			pText->SetText(XE::NtS(score));
		else
			pText->SetText(_T("?"));
	}
}

void XWndPopupSpotMenu::UpdateAP( XWnd *pRoot, bool bShow )
{
	auto pBaseSpot = m_spBaseSpot;
	XBREAK( pRoot == nullptr );
	XASSERT( pRoot->GetbShow() );
	auto pImg = pRoot->Find( "img.ap" );
	if( pImg )
		pImg->SetbShow( bShow );
	else
		return;
	if( !bShow )
		return;
	auto pText = xGET_TEXT_CTRL( pRoot, "text.ap.circle" );
	if( pText ) {
		int ap = pBaseSpot->GetNeedAP( ACCOUNT );
		pText->SetText( XE::Format( _T( "%d" ), ap ) );
		if( ACCOUNT->GetAP() >= ap )
			pText->SetColorText( XCOLOR_WHITE );
		else
			pText->SetColorText( XCOLOR_RED );
	}
}
/**
 @brief 최근전적
 @param bShow false면 아예 이 컨트롤은 처리하지 않는다.  true면 일단 들어와서 다른 조건을 비교해서 켤지말지 정한다.
*/
void XWndPopupSpotMenu::UpdateNumLose( XWnd *pRoot, bool bShow )
{
	XBREAK( pRoot == nullptr );
	XASSERT( pRoot->GetbShow() );
	auto pCtrl = xGET_TEXT_CTRL( pRoot, "text.numlose" );
	if( !pCtrl )
		return;
	if( m_spBaseSpot->IsNpc() )
		bShow = false;
	pCtrl->SetbShow( bShow );
	if( bShow ) {
		if( m_spBaseSpot->GetnumLose() > 0 )
			pCtrl->SetText( XFORMAT( "최근전적=%d패", m_spBaseSpot->GetnumLose() ) );
	}
}
/**
 @brief 리젠 남은 시간.
*/
void XWndPopupSpotMenu::UpdateRemainRegenTime( XWnd *pRoot, bool bShow )
{
	XBREAK( pRoot == nullptr );
// 	if( !pRoot->GetbShow() )
// 		return;
	auto pCtrl = xGET_TEXT_CTRL( pRoot, "text.remain.sec" );
	if( !pCtrl )
		return;
	float sec = m_spBaseSpot->GetsecRemainRegen();
	if( sec <= 0 )
		bShow = false;
	pCtrl->SetbShow( bShow );
	if( bShow ) {
		// 스팟이 아직 리젠안된 상태라면 남은시간을 표시
		pCtrl->SetText( XFORMAT( "%s:%s", XTEXT( 2238 ), XGAME::GetstrResearchTime( (int)sec ).c_str() ) );
	}
}

void XWndPopupSpotMenu::UpdateSpotName( bool bShow )
{
	auto pBaseSpot = m_spBaseSpot;
	XCOLOR col = XCOLOR_WHITE;
	if( ACCOUNT->GetidAccount() == pBaseSpot->GetidOwner() ) {
		m_strName = ACCOUNT->GetstrName();		
	} else {
		const auto power = pBaseSpot->GetPower();
		m_strName = pBaseSpot->GetszName();
		if( m_spBaseSpot->IsActive() && power > 0 )
			col = XGAME::xGetColorPower( power, ACCOUNT->GetPowerExcludeEmpty() );
	}
	auto pText = xSET_TEXT( this, "text.spot.name", m_strName );
	if( pText ) {
		pText->SetColorText( col );
// 		const auto power = pBaseSpot->GetPower();
// 		if( m_spBaseSpot->IsActive() && power > 0 ) {
// 			XCOLOR col = XGAME::GetColorPower( power, ACCOUNT->GetPowerExcludeEmpty() );
// 			pText->SetColorText( col );
// 		} else
// 			pText->SetColorText( XCOLOR_WHITE );
	}
}
/**
 @brief 모든스팟 공통 버튼들에 대한 업데이트.
*/
void XWndPopupSpotMenu::UpdateButtons()
{
	// 자원수거
	if (Find("butt.circle.collect")) {
		if (m_spBaseSpot && m_spBaseSpot->GetLocalStoragePercent() > 0.f)
			xSET_ENABLE(this, "butt.circle.collect", TRUE);
		else
			xSET_ENABLE(this, "butt.circle.collect", FALSE);
	}
	// 리매치
	if (Find("butt.circle.change.player")) {
		if (ACCOUNT->IsEnoughCash(XGAME::xCS_CHANGE_PLAYER))
			xSET_ENABLE(this, "butt.circle.change.player", TRUE);
		else
			xSET_ENABLE(this, "butt.circle.change.player", FALSE);
	}
	// 리젠
	if (Find("butt.circle.regen")) {
		if (ACCOUNT->IsEnoughCash(XGAME::xCS_REGEN_SPOT))
			xSET_ENABLE(this, "butt.circle.regen", TRUE);
		else
			xSET_ENABLE(this, "butt.circle.regen", FALSE);
	}
}

#ifdef _CHEAT
void XWndPopupSpotMenu::UpdateDebugInfo()
{
	if( !XAPP->m_bDebugMode )
		return;
	ID idOwner = 0;
	switch (m_spBaseSpot->GettypeSpot()) {
	case XGAME::xSPOT_CASTLE: {
		auto pSpot = SafeCast<XSpotCastle*>(m_spBaseSpot);
		if (pSpot) {
			idOwner = pSpot->GetidOwner();
			auto pText = XWndTextString::sUpdateCtrl( this, "text.debug.lowest", XE::VEC2(5,10) );
			pText->SetText( XFORMAT( "low=%d", pSpot->GetpowerUpperLimit() ) );
		}
	}	break;
	}
	auto pText = XWndTextString::sUpdateCtrl( this, "text.ids.spot", XE::VEC2(0,0) );
	pText->SetText( m_spBaseSpot->GetpBaseProp()->strIdentifier.c_str() );
}
#endif // _CHEAT

/**
@brief 원형메뉴에 버튼메뉴를 추가시킨다.
*/
XWndButton* XWndPopupSpotMenu::AddMenu(xtMenu typeMenu)
{
	_tstring strImg, strImgOff;
	std::string strIdentifer;
	_tstring strLabel;
//	XArrayLinearN<XWnd*, 256> aryButts;
	switch (typeMenu)
	{
	case XWndPopupSpotMenu::xCM_ATTACK:
		strImg = _T("world_butt_attack.png");
		strIdentifer = "butt.circle.attack";
		strLabel = XTEXT(2038);
		break;
	case XWndPopupSpotMenu::xCM_RECON:
		strImg = _T("world_butt_recon.png");
		strIdentifer = "butt.circle.recon";
		strLabel = XTEXT( 2213 );
		break;
	case XWndPopupSpotMenu::xCM_COLLECT:
		strImg = _T("world_butt_collect.png");
		strIdentifer = "butt.circle.collect";
		strLabel = XTEXT( 2214 );
		break;
	case XWndPopupSpotMenu::xCM_LOG_SULFUR:
		strImg = _T( "world_butt_collect.png" );
		strIdentifer = "butt.circle.log.sulfur";
		strLabel = XTEXT( 2246 );
		break;
	case XWndPopupSpotMenu::xCM_CHANGE_PLAYER_CASH:
	case XWndPopupSpotMenu::xCM_CHANGE_PLAYER_GOLD:
		strImg = _T("world_butt_newplayer.png");
		strIdentifer = "butt.circle.change.player";
		strLabel = XTEXT( 2215 );
		break;
	case XWndPopupSpotMenu::xCM_REGEN:
		strImg = _T("world_butt_regen.png");
		strIdentifer = "butt.circle.regen";
		strLabel = XTEXT( 2216 );
		break;
	case XWndPopupSpotMenu::xCM_MANDRAKE_WITHDRAW:
		strImg = _T("mandrake_butt.png");
		strIdentifer = "butt.circle.withdraw";
		strLabel = XTEXT( 2217 );
		break;
//#ifdef _CHEAT
	case XWndPopupSpotMenu::xCM_KILL:
		strImg = _T("butt_kill.png");
		strIdentifer = "butt.circle.kill";
		strLabel = XTEXT( 2218 );
		break;
//#endif
	default:
		break;
	}
	const auto sizePopup = GetSizeLocal();
	XWnd* pRootButt = Find( "wnd.root.butt" );
	XBREAK( pRootButt == nullptr );
	pRootButt->SetSizeLocal( sizePopup.w, pRootButt->GetSizeLocal().h );
	auto pButt = new XWndButton(0, 0, strImg.c_str(), strImg.c_str(), strImgOff.c_str());
	pButt->SetstrIdentifier(strIdentifer.c_str());
	pRootButt->Add(pButt);
	auto vSize = pButt->GetSizeLocal();
	auto pText = new XWndTextString( XE::VEC2(-10,37)
																, XE::VEC2(vSize.w + 20.f,1)
																, strLabel.c_str()
																, FONT_NANUM, 15.f );
	pText->SetAlignHCenter();
	pText->SetStyleStroke();
	pButt->Add( pText );
	m_aryMenus.Add(pButt);
	///< 
	switch (typeMenu)
	{
	case XWndPopupSpotMenu::xCM_ATTACK:
		pButt->SetEvent(XWM_CLICKED, this, &XWndPopupSpotMenu::OnClickAttack );
		break;
	case XWndPopupSpotMenu::xCM_RECON:
		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnReconSpot, m_idSpot);
		break;
	case XWndPopupSpotMenu::xCM_COLLECT:
		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnCollectSpot, m_idSpot);
		break;
	case XWndPopupSpotMenu::xCM_LOG_SULFUR:
		pButt->SetbShow( false );
		pButt->SetEvent( XWM_CLICKED, this, &XWndPopupSpotMenu::OnLogSulfur, m_idSpot );
		break;
	case XWndPopupSpotMenu::xCM_CHANGE_PLAYER_GOLD: {
		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnClickChangePlayerByGold, m_idSpot);
		auto pSub = new XWndImage(PATH_UI("hero_gold.png"), 0, 46);
		pSub->SetScaleLocal(0.3f);
		pButt->Add(pSub);
		auto pSpot = sGetpWorld()->GetSpot(m_idSpot);
		if (pSpot) {
			int gold = XGC->GetGoldChangePlayer( ACCOUNT->GetLevel() );
//			auto pText = new XWndTextString(XE::VEC2(10, 50),
			auto pText = new XWndTextString( XE::VEC2( 0, 50 ),
																	XFORMAT("%s", XE::NumberToMoneyString(gold)),
																	FONT_RESNUM, 15.f);
			pText->SetStyleStroke();
			pText->SetLineLength( vSize.w );
			pText->SetAlignHCenter();
			pButt->Add(pText);
			if (pSpot->IsSuccessiveDefeat()) {
				pText->SetText(XTEXT(2121));    // 무료
				auto pMark = new XWndSprObj(_T("ui_alert.spr"), 1, 20, 8);
				pMark->SetstrIdentifier("icon.alert");
				pButt->Add(pMark);
			}
		}
	} break;
	case XWndPopupSpotMenu::xCM_CHANGE_PLAYER_CASH: {
		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnClickChangePlayerByCash, m_idSpot);
		auto pSub = new XWndImage(PATH_UI("gem_small.png"), 8, 46);
		pButt->Add(pSub);
		auto pText = new XWndTextString(XE::VEC2(19, 47),
			XFORMAT( "%s", XE::NumberToMoneyString( XGC->m_cashChangePlayer ) ),
			FONT_RESNUM, 15.f );
		pText->SetStyleStroke();
		pButt->Add(pText);
	} break;
	case XWndPopupSpotMenu::xCM_REGEN: {
		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnClickRegen, m_idSpot);
		auto pSub = new XWndImage(PATH_UI("gem_small.png"), 8, 46);
		pButt->Add(pSub);
		auto pText = new XWndTextString(XE::VEC2(19, 47),
			XFORMAT("%s", XE::NumberToMoneyString(XGC->m_cashChangePlayer)),
			FONT_RESNUM, 15.f);
		pText->SetStyleStroke();
		pButt->Add(pText);
	} break;
	case XWndPopupSpotMenu::xCM_MANDRAKE_WITHDRAW: {
		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnMandrakeWithdraw, m_idSpot);
		auto pTextReward = new XWndTextString( _T(""), FONT_RESNUM, 18.f, XCOLOR_GREEN );
		pTextReward->SetStyleStroke();
		pTextReward->SetstrIdentifier( "text.mandrake.get.reward" );
		pButt->Add( pTextReward );
		// "보상획득" 텍스트 좀더 아래로.
		pText->SetY( pText->GetPosLocal().y + 10.f );
	} break;
// #ifdef _CHEAT
	case XWndPopupSpotMenu::xCM_KILL:
//		pButt->SetEvent(XWM_CLICKED, SCENE_WORLD, &XSceneWorld::OnClickKill, m_idSpot);
		break;
//#endif // _CHEAT
	}
	// 위치 업데이트

	pRootButt->AutoLayoutHCenterByChilds( 10.f );
	return SafeCast<XWndButton*>( pButt );
}

bool XWndPopupSpotMenu::IsHaveItems( ID idItem )
{
	for( auto& item : m_aryItems ) {
		if( item.idDropItem == idItem )
			return true;
	}
	return false;
}
void XWndPopupSpotMenu::AddItems( ID idItem, int num )
{
	XBREAK( idItem == 0 );
	XBREAK( num == 0 );
	// 이미 있는건 다시 넣지 않는다.
	if( IsHaveItems( idItem ) )
		return;
	xDropItem item( idItem, num );
	m_aryItems.push_back( item );
}
void XWndPopupSpotMenu::AddItems( LPCTSTR szIds, int num )
{
	auto pPropItem = PROP_ITEM->GetpProp( szIds );
	if( XASSERT(pPropItem) )
		AddItems( pPropItem->idProp, num );
}
void XWndPopupSpotMenu::AddItems( XPropItem::xPROP* pProp, int num )
{
	AddItems( pProp->idProp, num );
}

int XWndPopupSpotMenu::OnLogSulfur( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnLogSulfur");
	//
	m_bViewLog = !m_bViewLog;
	SetbUpdate( true );
	return 1;
}

/****************************************************************
* @brief 
*****************************************************************/
int XWndPopupSpotMenu::OnClickAttack( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	CONSOLE("OnClickAttack");
	//
	if( SCENE_WORLD )
		SCENE_WORLD->OnAttackSpot( nullptr, m_idSpot, 0 );
	SetbDestroy( TRUE );
	return 1;
}


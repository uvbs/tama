#include "stdafx.h"
#include "XWndSpots.h"
//#include "XWindow.h"
#include "XSpots.h"
#include "XSpotDaily.h"
#include "XAccount.h"
#include "XLegion.h"
#include "XSquadron.h"
#include "XGame.h"
#include "XQuestMng.h"
#include "XSceneWorld.h"
#include "XWndWorld.h"
#include "_Wnd2/XWndButton.h"
#include "XTemp.h"
#include "XGlobalConst.h"
#include "sprite/SprObj.h"
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
using namespace xSpot;
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

//////////////////////////////////////////////////////////////////////////
/**
 @brief 
 @param secDelay 액자객체가 생성이되면 몇초후부터 나타나게 할지를 지정한다. 0이면 즉시, -1는 디폴트값이다.
*/
XWndProfileForSpot* XWndSpot::sUpdateProfilePicture( XSpot *pSpot
																										, const std::string& strcFbUserId
																										, XSurface* psfcProfile
																										, float secDelay )
{
	const ID idSpot = pSpot->GetidSpot();
	auto pRoot = SCENE_WORLD->GetpLayerByProfilePictures();	// 프로필사진 전용레이어에 설치.
	// 페북계정이 있다면 일단 액자는 생성해야함.
	if( !strcFbUserId.empty() ) {
		if( pRoot ) {
			if( psfcProfile && !psfcProfile->IsEmpty() ) {	// 서피스는 있는데 이미지가 비어있으면 액자는 생성하지 않음.
				bool bNewCreated = false;
				// 액자컨트롤이 떠있는지 검사.
				auto pWndProfile = SafeCast2<XWndProfileForSpot*>( pRoot->Findf( "wnd.profile.%d", idSpot ) );
				if( pWndProfile == nullptr ) {
					bNewCreated = true;
					pWndProfile = new XWndProfileForSpot( pSpot, strcFbUserId );
					pWndProfile->SetstrIdentifierf( "wnd.profile.%d", idSpot );
					pRoot->Add( pWndProfile );
					SCENE_WORLD->SetbUpdate( true );
					if( secDelay >= 0 )
						pWndProfile->SetDelayedAppear( secDelay );
// 					if( m_pSpot->GetbAttacked() ) {
// 						pWndProfile->SetState( 2 );		// 첨부터 바로 나타나게.
// 						m_pSpot->SetbAttacked( false );
// 					}
				}
				// psfcProfile이 있으면 그걸로 교체되고 없으면 더미 이미지로 교체된다.
				pWndProfile->SetProfileImage( psfcProfile );
				if( bNewCreated )
					return pWndProfile;
			}
		}
	}	else {
		// 페북 계정이 없으면 액자 없음.
		auto pExist = pRoot->Findf( "wnd.profile.%d", idSpot );
		if( pExist )
			pExist->SetbDestroy( true );
	}
	return nullptr;
}

/**
 @brief 인삿말 UI를 업데이트 한다.
*/
void XWndSpot::sUpdateHelloMsg( XSpot *pSpot, const _tstring& strHello )
{
	const ID idSpot = pSpot->GetidSpot();
	auto pRoot = SCENE_WORLD->GetpLayerByProfilePictures();	// 프로필사진 전용레이어에 설치.
	// 인삿말이 등록되어 있다면 출력.
	if( !strHello.empty() ) {
		if( pRoot ) {
			auto pWndProfile = SafeCast2<XWndProfileForSpot*>( pRoot->Findf( "wnd.profile.%d", idSpot ) );
			if( pWndProfile && !pWndProfile->IsShowProfile() )
				pWndProfile = nullptr;
			const auto vSpot = pSpot->GetpBaseProp()->vWorld;
			// 프로필액자가 있으면 그위에 띄우고 없으면 스팟 바로위에 띄운다.
			auto pWndHello = SafeCast2<XWndHello*>( pRoot->Findf( "wnd.hello.%d", idSpot ) );
			if( pWndHello == nullptr ) {
				pWndHello = new XWndHello( idSpot, vSpot );
				pWndHello->SetstrIdentifierf( "wnd.hello.%d", idSpot );
				pRoot->Add( pWndHello );
				SCENE_WORLD->SetbUpdate( true );
			}
			pWndHello->SetText( strHello );
			pWndHello->UpdatePos();
		}
	}
	else {
		// 인삿말이 없으면 윈도우 삭제.
		pRoot->DestroyWndByIdentifierf( "wnd.hello.%d", idSpot );
	}
}

////////////////////////////////////////////////////////////////
ID XWndSpot::s_idLastTouch = 0;
XWndSpot::XWndSpot( XSpot *pBaseSpot, LPCTSTR szSpr, ID idAct, const XE::VEC2& vPos )
	: XWndSprObj( vPos )
//	: XWndSprObj( szSpr, idAct, vPos ) 
{
	Init();
	CreateSprObj( szSpr, idAct, true, true, true, xRPT_LOOP );
	m_pBaseSpot = pBaseSpot;
	m_idSpot = pBaseSpot->GetidSpot();
	auto pProp = pBaseSpot->GetpBaseProp();
	if( !pProp->strSpr.empty() ) {	// spr이 따로 지정되어있다면.
		XBREAK( pProp->idAct == 0 );
		SetSprObj( pProp->strSpr.c_str(), pProp->idAct );
	}
	const auto typeSpot = m_pBaseSpot->GettypeSpot();
	// 기능성 건물들을 제외하곤 모두 바닥에 그라뎅 이펙 붙인다.
	if( !m_pBaseSpot->IsOurBuilding() && typeSpot != XGAME::xSPOT_CASH ) {
		m_psoEff = new XSprObj(_T("spot_eff.spr"));
		if( m_pBaseSpot->IsHomeSpot() || typeSpot == XGAME::xSPOT_CASTLE )	// 성스팟류는 살짝 더크게
			m_psoEff->SetScale( 1.2f );
		m_psoEff->SetAction( 1 );
	}
	m_psoIndicate = new XSprObj(_T("spot_indicate.spr"));
	if( m_pBaseSpot->IsEventSpot() )
		m_psoIndicate->SetAction( 1 );	// 이벤트 스팟일때만 움직이게
	else
		m_psoIndicate->SetAction( 2 );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 0.2f );
#endif
	SetbUpdate( true );
}
void XWndSpot::Destroy()
{
	SAFE_DELETE( m_pMsgPusher );
	SAFE_DELETE( m_psoIndicate );
	SAFE_DELETE( m_psoEff );
}

BOOL XWndSpot::OnCreate()
{
	auto pScrollView = GetpParent();
	XBREAK( pScrollView == nullptr );
	m_pMsgPusher = new XMsgPusher( pScrollView, 1.f );
	return TRUE;
}

void XWndSpot::Update()
{
	XBREAK( GetDestroy() );
	if( m_pBaseSpot->GetbDestroy() ) {
		SetbDestroy( true );
		return;
	}
	// 스팟 바닥의 초록/빨강 그라데이션
	if( m_psoEff ) {
		if( IsEnemySpot() )
			m_psoEff->SetAction( 2 );
		else
			m_psoEff->SetAction( 1 );
	}
	// virtuals
	auto pText = 
	UpdateName( "text.spot.name", m_bShowName );
	UpdateLevel( "text.level", m_bShowLevel, pText );
	UpdateHardMark( "img.hard.lv", m_bShowHardMark );
	UpdateExclamByQuest( "spr.exclam", m_bShowExclamByQuest );
	UpdateExclamByDefeat( "icon.alert", m_bShowExclamByDefeat );
	UpdateStarScore();
#ifdef _CHEAT
	if( XAPP->m_bDebugMode && XAPP->m_bViewSpotInfo )
		UpdateDebugInfo();
#endif // _CHEAT
}

/**
 @brief 스팟의 이름을 얻는다(virtual)
 @param pOutbNameColorByPower 스팟상대 전투력과 비교해 색등급으로 표시해야되면 true
*/
_tstring XWndSpot::GetstrName( bool *pOutbNameColorByPower )
{
	_tstring strText;
	if( IsEnemySpot() ) {
		strText = m_pBaseSpot->GetstrName();
		if( strText.empty() )
			strText = _T("?");
		*pOutbNameColorByPower = true;
	} else {
		strText = ACCOUNT->GetstrName();
		*pOutbNameColorByPower = false;
	}
	return strText;
}


/**
 @brief 스팟의 이름부분을 업데이트한다.
 @param cIdentifier 스팟컨트롤의 ids를 지정한다.
 @param bShow 이름을 표시해야하는지
*/
XWndTextString* XWndSpot::UpdateName( const char* cIdentifier, bool bShow )
{
	if( !bShow ) {
		auto pExist = Find( cIdentifier );
		if( pExist )
			pExist->SetbShow( bShow );
		return SafeCast2<XWndTextString*>( pExist );
	}
	auto pText = XWndTextString::sUpdateCtrl( this
																					, cIdentifier
																					, XE::VEC2(0)
																					, FONT_NANUM_BOLD, 18 );
	if( pText ) {
		pText->SetStyleStroke();
		pText->SetbShow( true );
		bool bNameColorByPower = false;
		_tstring strName = GetstrName( &bNameColorByPower );
#ifdef _CHEAT
		if( XAPP->m_bDebugMode ) {
			if( m_pBaseSpot->GettypeSpot() != xSPOT_CASTLE && m_pBaseSpot->GettypeSpot() != xSPOT_SULFUR )
				strName += XFORMAT(",Lv%d", m_pBaseSpot->GetLevel() );
			strName += XFORMAT( ",LvArea%d", m_pBaseSpot->GetAreaLevel() );
		}
#endif // _CHEAT
		pText->SetText( strName );
		XCOLOR colName = GetColorTextName();
		if( bNameColorByPower ) {
			if( m_pBaseSpot->GetPower() )
				colName = XGAME::xGetColorPower( m_pBaseSpot->GetPower()
																			, ACCOUNT->GetPowerExcludeEmpty() );
		}
		pText->SetColorText( colName );	// 만약 또다른 예외가 있다면 GetColorName() virtual을 만들어서 쓸것.
		const auto sizeText = pText->GetSizeNoTransLayout();
		pText->SetPosLocal( XE::VEC2( ( -sizeText.x ) / 4 - 5.f,
																	( GetSizeLocal().y / 2 ) - 5.f ) );
	}
	return pText;
}

/**
 @brief 스팟의 레벨을 업데이트한다.
 레벨바탕의 이미지와 텍스트를 지정해야한다.
*/
XWndTextString* XWndSpot::UpdateLevel( const char* cIdentifier, bool bShow, XWndTextString* pTextName )
{
	if( !bShow ) {
		auto pExist = Find( cIdentifier );
		if( pExist )
			pExist->SetbShow( bShow );
		return SafeCast2<XWndTextString*>( pExist );
	}
	// 디폴트 레벨바탕 이미지
	// PATH_UI("level_bg.png")
	const _tstring resImg = GetresImgByLevel();		// virtual
	auto pImg = XWndImage::sUpdateCtrl( this
																		, XE::VEC2(0)
																		, resImg
																		, true			// bAlpha
																		, "img.level" );
	if( pImg ) {
		auto pText = XWndTextString::sUpdateCtrl( pImg
																						, cIdentifier
																						, XE::VEC2(0)
																						, FONT_RESNUM, 25 );
		if( pText ) {
//			pText->UpdateSize();
			pText->SetStyleStroke();
			pText->SetbShow( true );
			bool bNameColorByPower = false;
			const int lvSpot = m_pBaseSpot->GetLevel();
			if( lvSpot )
				pText->SetText( XFORMAT("%d", lvSpot) );
			else {
//				if( m_pBaseSpot->GetpBaseProp()->strIdentifier == _T("spot.home") )
				if( m_pBaseSpot->IsHomeSpot() )
					pText->SetText( XFORMAT( "%d", ACCOUNT->GetLevel() ) );
				else
					pText->SetText( _T("?") );
			}
			pText->SetColorText( XCOLOR_RGBA(255,204,0,255) );
			pText->SetAlignCenter();
			const auto vText = (pTextName)? pTextName->GetPosLocal() : XE::VEC2(0);
			const auto vImg = pImg->GetPosLocal();
			const auto sizeImg = pImg->GetSizeLocal();
			// 이름앞에 레벨심볼 위치 조정
			XE::VEC2 v;
			v.x = vText.x - sizeImg.x - 3;
			v.y = vText.y + (pText->GetSizeNoTransLayout().h / 2.f) - (sizeImg.y / 2.f);
			pImg->SetPosLocal( v );
		} // pText
		return pText;
	}
	return nullptr;
}
/**
 @brief 스팟의 레벨을 표시할때 그려지게될 배경이미지의 패스명을 얻는다.
*/
_tstring XWndSpot::GetresImgByLevel()
{
	if( m_pBaseSpot->IsNpc() ) {
		// npc의 경우는 일반 심볼
		if( m_pBaseSpot->IsElite() )
			return PATH_UI( "level_bg_elite.png" );
		else
			return PATH_UI( "level_bg.png" );
	} else {
		// pc의 경우는 사람얼굴 있는 심볼
		if( IsEnemySpot() ) {
			return PATH_UI( "world_middle_enemy.png" );
		} else {
			return PATH_UI( "world_middle_ally.png" );
		}
	}
	XBREAK(1);
	return _T("");
}

XWndImage* XWndSpot::UpdateHardMark( const char* cIdentifier, bool bShow )
{
	// 애초에 출력할필요 없는 스팟은 컨트롤을 만들지 않는다.
	if( !bShow ) {
		auto pExist = Find( cIdentifier );
		if( pExist )
			pExist->SetbShow( bShow );
		return SafeCast2<XWndImage*>( pExist );
	}
	// 스팟 난이도 아이콘
	int power = m_pBaseSpot->GetPower();
	if( power == 0 ) {
		m_pBaseSpot->UpdatePower();
		power = m_pBaseSpot->GetPower();
	}
	if( XASSERT(power) ) {
		int lvHard = XGAME::GetHardLevel( power, ACCOUNT->GetPowerExcludeEmpty() );
		auto pImg = XWndImage::sUpdateCtrl( this, XE::VEC2(0), nullptr, true, cIdentifier );
		if( pImg ) {
			pImg->SetbShow( true );
			const auto bb = GetBoundBoxByVisibleLocal();
			const auto vSize = pImg->GetSizeLocal();
			pImg->SetX( -( vSize.w * 0.5f ) );
			pImg->SetY( bb.vLT.y - vSize.h );
			_tstring strImg = XE::Format( _T( "world_hard%d.png" ), lvHard + 2 );
			pImg->SetSurfaceRes( XE::MakePath( DIR_UI, strImg ) );
		}
		return pImg;
	}
	return nullptr;
}

XWndSprObj* XWndSpot::UpdateExclamByQuest( const char* cIdentifier, bool bShow )
{
	if( !bShow ) {
		auto pExist = Find( cIdentifier );
		if( pExist )
			pExist->SetbShow( bShow );
		return SafeCast2<XWndSprObj*>( pExist );
	}
	if( m_pBaseSpot->IsEventSpot() && m_pBaseSpot->IsActive() ) {	
		auto pWndSpr = XWndSprObj::sUpdateCtrl( this, _T("ui_exclam.spr"), 1, "spr.exclam" );
		if( pWndSpr ) {
			auto bb = GetBoundBoxByVisibleLocal();
			pWndSpr->SetY( 5.f );
		}
		return pWndSpr;
	}
	return nullptr;
}

/**
 @brief 연패했을때 느낌표.
*/
XWndSprObj* XWndSpot::UpdateExclamByDefeat( const char* cIdentifier, bool bShow )
{
	// 애초에 출력할필요 없는 스팟은 컨트롤을 만들지 않는다.
	if( !bShow ) {
		DestroyWndByIdentifier( cIdentifier );
		return nullptr;
	}
	// 연패했을때 느낌표.
	if( m_pBaseSpot->IsSuccessiveDefeat() 
		&& m_pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE ) {
		auto pWndSpr = XWndSprObj::sUpdateCtrl( this, _T( "ui_alert.spr" ), 1, XE::VEC2(8), cIdentifier );
		return pWndSpr;
	} else {
		DestroyWndByIdentifier( "icon.alert" );
	}
	return nullptr;
}

/**
 @brief 별점 표시
*/
void XWndSpot::UpdateStarScore()
{
//	if( ACCOUNT->GetpQuestMng()->IsHaveGetStarQuest() ) {
	if( ACCOUNT->IsAbleGetStar() ) {
		if( !m_pBaseSpot->IsEventSpot() ) {
			bool bShowStar = false;
			int numStar = ACCOUNT->GetNumSpotStar( m_pBaseSpot->GetidSpot() );
			const auto typeSpot = m_pBaseSpot->GettypeSpot();
			if( typeSpot == xSPOT_CASTLE || typeSpot == xSPOT_NPC ) {
				if( !m_pBaseSpot->IsEventSpot() ) {
					if( numStar < 3 && numStar >= 0 )
						bShowStar = true;
				}
			}
			const float wStar = 13.f;
			XE::VEC2 v( -((wStar * 3.f) * 0.5f), -43.f );
			for( int i = 0; i < 3; ++i ) {
				std::string strIdentifier = XE::Format( "img.star.%d", i + 1 );
				if( bShowStar ) {
					auto pImg = XWndImage::sUpdateCtrl( this
																						, v
																						, _T("")
																						, true // bAlpha
																						, strIdentifier );
					if( pImg ) {
						if( numStar >= i + 1 ) {
							// 별표시
							pImg->SetSurfaceRes( PATH_UI("common_etc_smallstar.png") );
						} else {
							// 빈 별 표시
							pImg->SetSurfaceRes( PATH_UI("common_etc_smallstar_empty.png") );
						}
					}
					v.x += wStar;
				} else {
					// 별표시 할필요 없으면 걍 윈도 삭제하면 됨.
					DestroyWndByIdentifier( strIdentifier );
				}
			} // for
		} // if( !m_pBaseSpot->IsEventSpot() ) {
	} // if( ACCOUNT->GetpQuestMng()->IsHaveGetStarQuest() ) {
}

#ifdef _CHEAT
void XWndSpot::UpdateDebugInfo()
{
	auto pText = xGET_TEXT_CTRL( this, "text.debug" );
	if( pText )
		pText->SetbShow( XAPP->m_bDebugMode );
	if( !XAPP->m_bDebugMode )
		return;
	_tstring strText = GetstrDebugText();
	pText = XWndTextString::sUpdateCtrl( this, "text.debug", XE::VEC2(0) );
	if( pText ) {
		pText->SetStyleStroke();
		pText->SetText( strText );
		XE::VEC2 vPos;
		auto pTextName = xGET_TEXT_CTRL( this, "text.spot.name" );
		if( pTextName ) {
			vPos = pTextName->GetPosLocal();
			vPos.y += 10.f;
		} else {
			const auto vSize = pText->GetSizeNoTransLayout();
			vPos = XE::VEC2( -vSize.x / 4, GetSizeLocal().y / 2 );
			vPos.y += 10.f;
		}
		pText->SetPosLocal( vPos );
	}
}
#endif // _CHEAT

void XWndSpot::OnAutoUpdate()
{
#ifdef _CHEAT
	if( XAPP->m_bDebugMode ) {
		auto pText = xGET_TEXT_CTRL( this, "text.debug" );
		if( pText ) {
			_tstring strText = GetstrDebugText();
			pText->SetText( strText );
		}
	}
#endif // _CHEAT
}

void XWndSpot::OnLButtonDown( float lx, float ly )
{
	XWndSprObj::OnLButtonDown( lx, ly );
}

void XWndSpot::OnLButtonUp( float lx, float ly )
{
	s_idLastTouch = getid();
	XWndSprObj::OnLButtonUp( lx, ly );
}

int XWndSpot::Process( float dt )
{
	if( m_pMsgPusher )
		m_pMsgPusher->Process( dt );
	if( m_psoEff )
		m_psoEff->FrameMove( dt );
	m_psoIndicate->FrameMove( dt );
	return XWndSprObj::Process( dt );
}

/**
 @brief 
*/
void XWndSpot::Draw()
{
#ifdef WIN32
	if( XWnd::s_pMouseOver && XWnd::s_pMouseOver->getid() == getid() ) {
		XE::VEC2 v = GetPosFinal();
		XE::VEC2 size = GetSizeFinal();
		GRAPHICS->DrawCircle( v.x, v.y, size.Length(), XCOLOR_RED );
		XE::VEC2 vMouse = INPUTMNG->GetMousePos();
		vMouse += XE::VEC2( 30, 30 );
		XE::VEC2 vWorld = m_pBaseSpot->GetpBaseProp()->vWorld;
		auto pProp = m_pBaseSpot->GetpBaseProp();
		_tstring str = XFORMAT( "pos:%d,%d\n%s(%d)\ncode=%d\narea:%d",
								(int)vWorld.x, (int)vWorld.y,
								pProp->strIdentifier.c_str(), pProp->idSpot,
								pProp->idCode,
								pProp->idArea );
		PUT_STRINGF( vMouse.x, vMouse.y, XCOLOR_WHITE, "%s", str.c_str() );
	}
#endif
	if( m_timerBlink.IsOn() ) {
		if( m_timerBlink.IsOver() ) {
			m_timerBlink.Off();
			XWndSprObj::SetColor( XCOLOR_WHITE );
		} else {
//			float wave = abs( cosf( m_timerBlink.GetPassSec() * 3.f ) );
			auto lerpTime = m_timerBlink.GetSlerp();
			float wave = XE::xiCos( lerpTime * 8.f, 0, 1.f, 0 );
//			float wave = ::cosf( m_timerBlink.GetPassSec() * 3.f );
			XCOLOR col = XCOLOR_RGBA_FLOAT( 1.f, wave, wave, 1.f );
			XWndSprObj::SetColor( col );
		}
	}
	//
	if( m_psoEff )
		m_psoEff->Draw( GetPosFinal() );
	if( getid() == s_idLastTouch ) {
		if( m_pBaseSpot->IsActive() )
			m_psoIndicate->Draw( GetPosFinal() );
	}
	XWndSprObj::Draw();
#ifdef _CHEAT
	if( XAPP->m_bDebugMode && XAPP->m_bDebugViewBoundBoxSpot ) {
		XE::xRECT rect = GetBoundBoxByVisibleFinal();
		GRAPHICS->DrawRect( rect.vLT, rect.vRB, XCOLOR_BLUE );
		const auto vPos = GetPosFinal();
		const float sizeCross = 20.f;
		GRAPHICS->DrawHLine( vPos.x - (sizeCross * 0.5f), vPos.y, sizeCross, XCOLOR_BLUE );
		GRAPHICS->DrawVLine( vPos.x, vPos.y - (sizeCross * 0.5f), sizeCross, XCOLOR_BLUE );
	}
#endif // _CHEAT
}

////////////////////////////////////////////////////////////////
XWndCastleSpot::XWndCastleSpot( XSpotCastle* pSpot )
	: XWndSpot( pSpot, SPR_CASTLE, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;

	SetbShowName( true );
	SetbShowLevel( true );
	SetbShowExclamByQuest( true );
	SetbShowExclamByDefeat( true );
	SetbShowStarScore( false );

#ifdef _CHEAT
	SetAutoUpdate( 0.1f );
#endif


}

void XWndCastleSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->GetidOwner() == 0 ) {
		strOut += XTEXT(2035);	// 정찰하지 않음.
	} else
	if( m_pSpot->GetidOwner() == ACCOUNT->GetidAccount() ) {
		// xxx/시간당
		m_pSpot->GetStrProduce( strOut );
//		strOut += XE::Format( XTEXT( 80201 ), XE::NumberToMoneyString( int( m_pSpot->GetProducePerMin() * 60.f ) ) );
	} else {
		for( int i = XGAME::xRES_WOOD; i < XGAME::xRES_MAX; ++i ) {
			auto type = (XGAME::xtResource)i;
			int amount = m_pSpot->GetLootAmount( type );
			if( amount > 0 ) {
				_tstring str = XE::NumberToMoneyString( amount );
				strOut += XFORMAT( "%s:%s\n", XGAME::GetStrResource( type ), str.c_str() );
			}
		}
	}
}

_tstring XWndCastleSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
//		strText += XE::Format(_T(":(%d)\n"), (int)m_pSpot->GetTimerCalc().GetsecPassTime());
		strText += XE::Format( _T( "idWnd:%d\n" ), getid() );
		strText += XE::Format( _T( "id:%d-%s\n" ), m_pSpot->GetidSpot(), m_pSpot->GetpProp()->strIdentifier.c_str() );
		strText += XE::Format( _T( "(Lv%d)(sc:%d)" ), m_pSpot->GetLevel(), m_pSpot->GetScore() );
		strText += XE::Format( _T( ":(%d sec)\n" ), (int)m_pSpot->GetTimerCalc().GetsecRemainTime() );
		strText += XE::Format( _T( "%s" ), m_pSpot->GetStrLocalStorage().c_str() );
	}
#endif
	return strText;
}

_tstring XWndCastleSpot::GetstrName( bool *pOutbNameColorByPower )
{
	auto strText = XWndSpot::GetstrName( pOutbNameColorByPower );
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		if( IsEnemySpot() && m_pSpot->IsPC() )
			// 유저 적스팟이면 상대 계정번호도 표시
			strText += XFORMAT( "(%d)", m_pSpot->GetidOwner() );
	}
#endif 
	return strText;
}

void XWndCastleSpot::Update( void )
{
	if( m_pSpot->IsDestroy() ) {
		SetbDestroy( TRUE );
		return;
	}
	if( ACCOUNT->GetidAccount() == m_pSpot->GetidOwner() )
		XWndSprObj::SetAction( 1 );
	else 
		XWndSprObj::SetAction( 2 );
	const ID idSpot = m_pSpot->GetidSpot();
	if( 1 )
	{
// 		auto pRoot = SCENE_WORLD->GetpLayerByProfilePictures();	// 프로필사진 전용레이어에 설치.
		// 페북계정이 있다면 프로필 사진을 출력한다.
		const auto& strcFbUserId = m_pSpot->GetstrcFbUserId();
		float secDelay = (m_pSpot->GetbAttacked())? secDelay = 0 : -1.f;
		auto pNewCreated = XWndSpot::sUpdateProfilePicture( m_pSpot
																											, strcFbUserId
																											, m_pSpot->GetpsfcProfile()
																											, secDelay );
		if( pNewCreated )
			m_pSpot->SetbAttacked( false );
		// 인삿말이 등록되어 있다면 출력.
		XWndSpot::sUpdateHelloMsg( m_pSpot, m_pSpot->GetstrHello() );
	}
	XWndSpot::Update();
}

bool XWndCastleSpot::IsEnemySpot() 
{
	return ( ACCOUNT->GetidAccount() != m_pSpot->GetidOwner() );
}

////////////////////////////////////////////////////////////////
XWndJewelSpot::XWndJewelSpot( XSpotJewel* pSpot ) 
	: XWndSpot( pSpot, SPR_JEWEL, 
				pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	SetbShowName( true );
	SetbShowLevel( true );
	SetbShowExclamByQuest( true );
	SetbShowExclamByDefeat( true );
	SetbShowStarScore( false );
}

bool XWndJewelSpot::IsEnemySpot() 
{
	return ( ACCOUNT->GetidAccount() != m_pSpot->GetidOwner() );
}

void XWndJewelSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->GetidOwner() == 0 ) {
		strOut += XTEXT(2035);	// 정찰하지 않음.
	} else
	if( m_pSpot->GetidOwner() == ACCOUNT->GetidAccount() )
	{
		// 시간당 생산량으로 환산해서 보여줌.
		// 생산량:%d/시간
		strOut += XE::Format( XTEXT( 2036 ), (int)( m_pSpot->GetpProp()->GetProduce() * 60.f ) );
		strOut += _T("\n");
		// 현재보유량
		strOut += XE::Format( XTEXT( 2037 ), m_pSpot->GetLocalStorageAmount() );
	} else
	{
		//strOut += _T( "획득 가능 자원:\n" );
		strOut += XFORMAT( "%s:%d\n", XGAME::GetStrResource( XGAME::xRES_JEWEL ), m_pSpot->GetlootJewel() );
	}
	strOut += XFORMAT( "lvDef:%d\n", m_pSpot->GetlevelMine() );
	strOut += XFORMAT( "Def:%d\n", m_pSpot->GetDefense() );
}


void XWndJewelSpot::Update( void )
{
	if (m_pSpot) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
}

_tstring XWndJewelSpot::GetstrName( bool *pOutbNameColorByPower )
{
	auto strText = XWndSpot::GetstrName( pOutbNameColorByPower );
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		if( IsEnemySpot() && m_pSpot->IsPC() )
			// 유저 적스팟이면 상대 계정번호도 표시
			strText += XFORMAT( "(%d)", m_pSpot->GetidOwner() );
	}
#endif 
	return strText;
}

_tstring XWndJewelSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		if( m_pSpot->GetidOwner() != 0 )
			strText = XE::Format( _T( "%s(%d)<->적:%d\n보석보유량:%d/%d\nlv:%d:광산레벨:%d\ndef:%d\nidx army:%d\nloot:%d" ),
									m_pSpot->GetszName(),
									m_pSpot->GetidOwner(),
									m_pSpot->GetidMatchEnemy(),
									(int)m_pSpot->GetlsLocal().numCurr,
									(int)m_pSpot->GetlsLocal().maxSize,
									m_pSpot->GetLevel(),
									m_pSpot->GetlevelMine(),
									m_pSpot->GetDefense(),
									m_pSpot->GetidxLegion(),
									m_pSpot->GetlootJewel() );
		else
			strText = _T( "?" );
	}
#endif
	return strText;
}

///////////////////////////////////////////////////////////////
XWndSulfurSpot::XWndSulfurSpot( XSpotSulfur* pSpot )
	: XWndSpot( pSpot, SPR_SULFUR, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;

	SetbShowName( true );
	SetbShowLevel( true );
	SetbShowExclamByQuest( false );
	SetbShowExclamByDefeat( false );
	SetbShowStarScore( true );
	SetSizeLocal( 45, 45 );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 1.f );
#endif
}

void XWndSulfurSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->IsActive() ) {
		strOut += XE::Format(_T("%s\n"), XE::NumberToMoneyString(m_pSpot->GetnumSulfur()));
	} else
		strOut += XTEXT(2030);	// 비어있음.
}


void XWndSulfurSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
		if( m_pSpot->IsActive() )
			SetAction( 2 );
		else
			SetAction( 1 );
	}
	XWndSpot::Update();
}

_tstring XWndSulfurSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "sulfur:%d(sec:%d)" ), 
								(int)m_pSpot->GetnumSulfur(),
								(int)( m_pSpot->GettimerSpawn().GetsecRemainTime() ) );
	}
#endif
	return strText;
}

#ifdef WIN32
void XWndSulfurSpot::Draw( void )
{
	XWndSpot::Draw();
}
#endif // WIN32

////////////////////////////////////////////////////////////////
XWndMandrakeSpot::XWndMandrakeSpot( XSpotMandrake* pSpot )
	: XWndSpot( pSpot, SPR_MANDRAKE, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;

	// 연승 텍스트
	auto pText = new XWndTextString( 0, 0, _T( "" ) );
	if( pText ) {
		m_pTextWin = pText;
		pText->SetStyleStroke();
		pText->SetColorText( XCOLOR_RGBA(255,192,0,255) );
		Add( pText );
		pText->SetbShow( FALSE );
	}
	SetbShowName( true );
	SetbShowLevel( true );
	SetbShowExclamByQuest( false );
	SetbShowExclamByDefeat( false );
	SetbShowStarScore( false );

}

void XWndMandrakeSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->GetidOwner() == 0 ) {
		//strOut += _T("?");
		strOut += XTEXT(2035);	// 정찰하지 않음.
	} else
	if( m_pSpot->GetidOwner() == ACCOUNT->GetidAccount() ) {
		// 시간당 생산량으로 환산해서 보여줌.
		strOut += XE::Format( XTEXT(2036), (int)(m_pSpot->GetpProp()->GetProduce() * 60.f) );
		strOut += _T("\n");
		// 연승횟수: n연승
		strOut += XFORMAT("%s:%d%s\n", XTEXT(2046), m_pSpot->GetWin(), XTEXT(2047) );
		// 누적보상:만드레이크 0000
		strOut += XFORMAT("%s:\n%s x%d\n", XTEXT(2048), STR_MANDRAKE, m_pSpot->GetReward() );
	} else {
		strOut += XFORMAT("%s\n", XTEXT(2045));	// 승리보상
		strOut += XFORMAT("     %s", XE::NumberToMoneyString(m_pSpot->Getloot()));
	}

}


void XWndMandrakeSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}

		if( m_pTextWin ) {
			if( m_pSpot->GetWin() > 1 ) {
				m_pTextWin->SetText( XE::Format( XTEXT( 2049 ), m_pSpot->GetWin() ) );
// 				XE::VEC2 sizeText = m_pTextWin->GetLayoutSize();
				const auto sizeText = m_pTextWin->GetSizeNoTransLayout();
				m_pTextWin->SetPosLocal( XE::VEC2( -sizeText.w / 2, -37 ) );
				m_pTextWin->SetbShow( TRUE );
			} else
				m_pTextWin->SetbShow( FALSE );
		} 
	}
	XWndSpot::Update();
}

/**
 @brief 현재 스팟상태에 따라 스팟에 표시될 이름을 리턴한다
 @param pOutbNameColorByPower 만약 스팟점유자의 전투력과 비교해서 색등급으로 표시되어야 하면 true를 입력한다.
*/
_tstring XWndMandrakeSpot::GetstrName( bool *pOutbNameColorByPower )
{
	const ID idAcc = ACCOUNT->GetidAccount();
	const auto state = m_pSpot->GetState( idAcc );
	_tstring strText;
	switch( state ) {
	case xSM_NO_MATCH: 
		strText = _tstring(_T("?"));
		break;
	case xSM_CHALLENGE: 
		*pOutbNameColorByPower = true;
		strText = m_pSpot->GetstrName();
		break;
	case xSM_DEFENSE: 
		strText = ACCOUNT->GetstrName();
		break;
	default:
		XBREAK(1);
		break;
	}
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		if( IsEnemySpot() && m_pSpot->IsPC() )
			// 유저 적스팟이면 상대 계정번호도 표시
			strText += XFORMAT( "(%d)", m_pSpot->GetidOwner() );
	}
#endif 
	return strText;
}

bool XWndMandrakeSpot::IsEnemySpot()
{
	return ( ACCOUNT->GetidAccount() != m_pSpot->GetidOwner() );
}

_tstring XWndMandrakeSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
// 		_tstring strName = ( m_pSpot->GetidOwner() == 0 ) ? _T( "?" ) : m_pSpot->GetszName();
		strText = XE::Format( _T( "idOwner=%d<->%d\nMandrake:%d/%d\nlv:%d\nwin:%d\nreward:%d" ),
								m_pSpot->GetidOwner(),
								m_pSpot->GetidMatchEnemy(),
								(int)m_pSpot->GetlsLocal().numCurr,
								(int)m_pSpot->GetlsLocal().maxSize,
								m_pSpot->GetLevel(),
								m_pSpot->GetWin(),
								m_pSpot->GetReward() );
	}
#endif
	return strText;
}

///////////////////////////////////////////////////////////////
XWndNpcSpot::XWndNpcSpot( XSpotNpc* pSpot )
	: XWndSpot( pSpot, SPR_NPC, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	SetbHideDeactive( true );		// 스팟이 비활성화상태일때 이름을 표시하지 않는다.
	SetbShowName( false );
	SetbShowLevel( false );
	SetbShowExclamByQuest( false );
	SetbShowExclamByDefeat( false );
	SetbShowStarScore( true );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 1.f );
#endif
}
void XWndNpcSpot::SetSprSpot()
{
	if( m_pSpot->IsActive() ) {
		if( GetpBaseSpot()->GetpBaseProp()->strSpr.empty() ) {
			// 1레벨 지역이고 계정렙이 5이하이면 야만족만 나오게 한다.
			if( m_pSpot->GetpAreaProp()->lvArea == 1 && ACCOUNT->GetLevel() <= 5 )
				SetSprObj( SPR_NPC3 );	// 야만족
			else
				SetSprObj( SPR_NPC );	// 제국 부대
		} else
			SetSprObj( GetpBaseSpot()->GetpBaseProp()->strSpr, 1 );
	} else {
		SetSprObj( _T("spot_npc_empty.spr"), 2 );
	}
}
void XWndNpcSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot->IsActive() )
	{
		if( m_pSpot->IsReconed() )
			strOut += XFORMAT("%s", XE::NumberToMoneyString(m_pSpot->GetnumResource()));
		else
			strOut += _T("?");
	} else
		strOut += XTEXT(2030);	// 비어있음.
}


void XWndNpcSpot::Update( void )
{
#ifdef _xIN_TOOL
	{
		auto pProp = PROP_WORLD->GetpProp( m_idSpot );
		if( pProp == nullptr ) {
			SetbDestroy( TRUE );
			return;
		}
	}
#endif
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
		const bool bShowName = m_pSpot->IsEventSpot() && m_pSpot->IsActive();
		SetbShowName( bShowName );
		SetbShowLevel( bShowName );
		SetbShowHardMark( m_pSpot->IsActive() != FALSE );
		SetbShowExclamByQuest( bShowName );
		SetbShowExclamByDefeat( false );
		SetbShowStarScore( !m_pSpot->IsEventSpot() );
		SetSprSpot();
		if( m_pSpot->IsActive() )
			SetAction( 1 );		// 부대있는 모습
		else
			SetAction( 2 );		// 없는 모습
	}
	XWndSpot::Update();
}

_tstring XWndNpcSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "lv%d-id:%d(%d)" ), m_pSpot->GetLevel(),
							getid(),
							(int)m_pSpot->GettimerSpawn().GetsecRemainTime() );
	}
#endif
	return strText;
}

///////////////////////////////////////////////////////////////
XWndDailySpot::XWndDailySpot( XSpotDaily* pSpot )
	: XWndSpot( pSpot, SPR_DAILY, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	// 
	SetbShowName( true );
	SetbShowLevel( false );
	SetbShowExclamByQuest( false );
	SetbShowExclamByDefeat( false );
	SetbShowStarScore( false );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 0.5f );
#endif
}

void XWndDailySpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
}

void XWndDailySpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot ) {		
		strOut += XFORMAT( "Lv%d %s\n", m_pSpot->GetLevel(), m_pSpot->GetszName() );
		strOut += XE::Format( _T( "입장횟수:%d/%d" ), m_pSpot->GetnumEnter(), 
													_XGC->m_numEnterDaily );
	}
}

_tstring XWndDailySpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
	}
#endif
	return strText;
}

//////////////////////////////////////////////////////////////////////////
XWndCampaignSpot::XWndCampaignSpot( XSpotCampaign* pSpot )
	: XWndSpot( pSpot, SPR_CAMPAIGN, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;

	SetbShowName( true );
	SetbShowLevel( false );
	SetbShowExclamByQuest( false );
	SetbShowExclamByDefeat( false );
	SetbShowStarScore( false );
}

void XWndCampaignSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
}

_tstring XWndCampaignSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "Lv%d %s:id:%d" ), m_pSpot->GetLevel(),
			m_pSpot->GetszName(),
			getid() );
	}
#endif
	return strText;
}

//////////////////////////////////////////////////////////////////////////
XWndVisitSpot::XWndVisitSpot( XSpotVisit* pSpot )
	: XWndSpot( pSpot, _T( "spot_visit.spr" ), xRandom(3)+1, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;

	SetbShowName( true );
	SetbShowLevel( false );
	SetbShowExclamByQuest( false );
	SetbShowExclamByDefeat( false );
	SetbShowStarScore( false );
}

void XWndVisitSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
//	GetpImg()->SetbShow( FALSE );
}

_tstring XWndVisitSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "%s:id:%d" ), m_pSpot->GetszName(), getid() );
	}
#endif
	return strText;
}


//////////////////////////////////////////////////////////////////////////
XWndCashSpot::XWndCashSpot( XSpotCash* pSpot )
	: XWndSpot( pSpot, SPR_CASH, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	SetbShowName( false );
	SetbShowLevel( false );
	SetbShowExclamByQuest( false );
	SetbShowExclamByDefeat( false );
	SetbShowStarScore( false );
}

void XWndCashSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
		if( m_pSpot->IsActive() )
			SetbShow( TRUE );
		else
			SetbShow( FALSE );
	}
}

_tstring XWndCashSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XFORMAT( "id:%d", getid() );
	}
#endif
	return strText;
}

//////////////////////////////////////////////////////////////////////////
XWndCommonSpot::XWndCommonSpot( XSpotCommon* pSpot )
	: XWndSpot( pSpot, SPR_COMMON, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	auto pProp = pSpot->GetpProp();
	SetbShowName( true );
	SetbShowLevel( m_pSpot->IsHomeSpot() );
	SetbShowExclamByQuest( false );
	SetbShowExclamByDefeat( false );
	SetbShowStarScore( false );
}

void XWndCommonSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
		// 홈스팟에 플레이어 인삿말 표시.
		if( GetpBaseSpot()->IsHomeSpot() ) {
			const auto& strHello = ACCOUNT->GetstrHello();
			XWndSpot::sUpdateHelloMsg( m_pSpot, strHello );
		}
	}
	XWndSpot::Update();
}

bool XWndCommonSpot::IsEnemySpot()
{
	auto& idsSpot = GetpBaseSpot()->GetpBaseProp()->strIdentifier;
//	if( idsSpot == _T("spot.home") )
	if( m_pSpot->IsHomeSpot() )
		return false;
	if( !m_pSpot->IsCampaignType() )
		return false;
	return true;
}

_tstring XWndCommonSpot::GetstrName( bool *pOutbNameColorByPower )
{
	_tstring strText;
	if( m_pSpot->IsHomeSpot() ) {
		strText += ACCOUNT->GetstrName();
	} else {
		strText = m_pSpot->GetstrName();
	}
	return strText;
}

XCOLOR XWndCommonSpot::GetColorTextName() 
{ 
	if( m_pSpot->IsHomeSpot() )
		return XCOLOR_WHITE;
	return XCOLOR_YELLOW; 
}

_tstring XWndCommonSpot::GetstrDebugText()
{
	_tstring strText;
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		strText = XE::Format( _T( "id:%d" ), getid() );
	}
#endif
	return strText;
}


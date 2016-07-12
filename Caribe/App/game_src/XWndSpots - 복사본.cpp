#include "stdafx.h"
#include "XWndSpots.h"
#include "XWindow.h"
#include "XSpots.h"
#include "XAccount.h"
#include "XLegion.h"
#include "XSquadron.h"
#include "XGame.h"
#include "XQuestMng.h"

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

////////////////////////////////////////////////////////////////
ID XWndSpot::s_idLastTouch = 0;
XWndSpot::XWndSpot( XSpot *pBaseSpot, LPCTSTR szSpr, ID idAct, const XE::VEC2& vPos )
	: XWndSprObj( szSpr, idAct, vPos ) 
{
	Init();
	m_pBaseSpot = pBaseSpot;
	m_idSpot = pBaseSpot->GetidSpot();
	auto pProp = pBaseSpot->GetpBaseProp();
	if( !pProp->strSpr.empty() ) {	// spr이 따로 지정되어있다면.
		XBREAK( pProp->idAct == 0 );
		SetSprObj( pProp->strSpr.c_str(), pProp->idAct );
	}
	// 이름영역 공통
	m_pTextName = new XWndTextString( XE::VEC2(0), _T(""), FONT_NANUM, 18.0f );
	if( m_pTextName ) {
		m_pTextName->SetstrIdentifier( "text.spot.name" );
		m_pTextName->SetStyle( xFONT::xSTYLE_STROKE );
		Add( m_pTextName );
		m_pTextName->SetbShow( FALSE );	// 이름안나오는 스팟\도 있으니 일단 감춤.
	}
	// 레벨바탕 심볼 이미지
	m_pImg = new XWndImage( PATH_UI("level_bg.png"), 0, 0 );	// 이미지의 사이즈를 정하기 위해 46x46짜리 디폴트 이미지를 지정함.
	if( m_pImg ) {
		m_pImg->SetbShow( FALSE );
		Add( m_pImg );
		// 레벨 텍스트
		m_pLevelText = new XWndTextString( nullptr, FONT_RESNUM, 25.0f );
		if( m_pLevelText ) {
			m_pLevelText->SetbShow( FALSE );
			m_pLevelText->SetStyle( xFONT::xSTYLE_STROKE );
			m_pLevelText->SetColorText( XCOLOR_RGBA(255,204,0,255) );
			m_pLevelText->SetAlign( XE::xALIGN_CENTER );
			m_pImg->Add( m_pLevelText );
			m_pLevelText->SetbActive( FALSE );
		}
	}
	m_psoEff = new XSprObj(_T("spot_eff.spr"));
	m_psoEff->SetAction( 1 );
	m_psoIndicate = new XSprObj(_T("spot_indicate.spr"));
	if( m_pBaseSpot->IsEventSpot() )
		m_psoIndicate->SetAction( 1 );	// 이벤트 스팟일때만 움직이게
	else
		m_psoIndicate->SetAction( 2 );
#ifdef _CHEAT
	// 디버그용 텍스트
	m_pTextDebug = new XWndTextString( -82, -43, _T( "" ) );
	if( m_pTextDebug ) {
		m_pTextDebug->SetStyle( xFONT::xSTYLE_STROKE );
		Add( m_pTextDebug );
	}
	//m_TimerUpdate.Set( 1.f );
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 0.2f );
#endif
	SetbUpdate( true );
}
void XWndSpot::Destroy()
{
	SAFE_DELETE( m_psoIndicate );
	SAFE_DELETE( m_psoEff );
}

void XWndSpot::Update()
{
	XBREAK( GetDestroy() );
	if( m_pBaseSpot->GetbDestroy() ) {
		SetbDestroy( true );
		return;
	}
	if( IsEnemySpot() )
		m_psoEff->SetAction( 2 );
	else
		m_psoEff->SetAction( 1 );
	// virtuals
	UpdateName( "text.spot.name", m_bShowName );
	UpdateLevel( "text.level", m_bShowLevel );
	UpdateHardMark( "img.hard", m_bShowHardMark );
}

void XWndSpot::Update2()
{
	XBREAK( GetDestroy() );
	if( m_pBaseSpot->GetbDestroy() ) {
		SetbDestroy( true );
		return;
	}
	if( IsEnemySpot() )
		m_psoEff->SetAction( 2 );
	else
		m_psoEff->SetAction( 1 );
	// 이름 업데이트
	bool bShowName = true;
#pragma message("-----------------IsActive()로 판단하지 말고 보다 구체적인 스팟 상태가 필요함. 리젠대기중/전투가능/터치가능 등등")
#pragma message("----------------또한 아래 각 요소들을 update시 요소개별적으로 하위클래스에게 업데이트 형식을 맞겨야 좀더 관리하기가 좋아짐")
	if( m_pBaseSpot->IsActive() ) {
		if( GetstrName().empty() ) {
			// 이름이 없으면 ?로 표시
			auto pText = xSET_TEXT( this, "text.spot.name", _T("?") );
			if( pText ) {
				if( m_pBaseSpot->GetpBaseProp()->strIdentifier == _T("spot.home") )
					pText->SetText( ACCOUNT->GetstrName() );
				pText->SetColorText( XCOLOR_WHITE );
				auto vSizeText = pText->GetLayoutSize();
				pText->SetPosLocal( XE::VEC2( ( -vSizeText.x ) / 4 - 5.f, 
										(GetSizeLocal().h / 2) - 5.f ) );
			}
		} else {
			auto pText = xSET_TEXT( this, "text.spot.name", GetstrName().c_str() );
			if( pText ) {
				XCOLOR col = XCOLOR_WHITE;
				if( IsEnemySpot() ) {
					// 적이면 전투력 차이에 따라 색을 달리한다.
					if( ACCOUNT->GetPowerExcludeEmpty() == 0 )
						ACCOUNT->UpdatePower();
					if( m_pBaseSpot->GetPower() ) { 
						col = XGAME::GetColorPower( m_pBaseSpot->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
						// 스팟 난이도 아이콘
						int lvHard = XGAME::GetHardLevel( m_pBaseSpot->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
//						int lvHard = ACCOUNT->GetGradeLevel( m_pBaseSpot->GetPower() );
						auto pImg = static_cast<XWndImage*>( Find("img.hard.lv") );
						if( pImg == nullptr ) {
							auto bb = GetBoundBoxLocal();
							pImg = new XWndImage( PATH_UI("world_hard0.png"), XE::VEC2( 0, -bb.GetHeight() ) );
							pImg->SetstrIdentifier( "img.hard.lv" );
							Add( pImg );
							auto vSize = pImg->GetSizeLocal(); 
							pImg->SetX( -(vSize.w * 0.5f) );
							pImg->SetY( bb.vLT.y - vSize.h );
						} else
							pImg->SetbShow( true );
						_tstring strImg = XE::Format( _T( "world_hard%d.png" ), lvHard + 2 );
						pImg->SetSurfaceRes( XE::MakePath( DIR_UI, strImg ) );
					} else
						col = XCOLOR_WHITE;	// 정찰이 안된상태에선 흰색으로 표시
				}
				auto sizeText = pText->GetLayoutSize();
				pText->SetPosLocal( XE::VEC2( ( -sizeText.x ) / 4 - 5.f,
											(GetSizeLocal().y / 2 ) - 5.f ) );
				pText->SetColorText( col );
			} // pText
		}
		if( m_pTextName ) {
			auto pText = m_pTextName;
			// 레벨 배경이미지
			if( m_pImg ) {
				m_pImg->SetbShow( true );
				auto vText = m_pTextName->GetPosLocal();
				if( m_pBaseSpot->IsNpc() ) {
					// npc의 경우는 일반 심볼
					// 현재는 심볼 없이 텍스트로만 표현.
					if( m_pBaseSpot->IsElite() )
						m_pImg->SetSurfaceRes( PATH_UI( "level_bg_elite.png" ) );
					else {
						m_pImg->SetSurfaceRes( PATH_UI( "level_bg.png" ) );
						m_pImg->SetbShow( FALSE );
					}
				} else
				// pc의 경우는 사람얼굴 있는 심볼
				if( IsEnemySpot() ) {
					m_pImg->SetSurfaceRes( PATH_UI( "world_middle_enemy.png" ) );
				} else {
					m_pImg->SetSurfaceRes( PATH_UI( "level_bg.png" ) );
// 					m_pImg->SetSurfaceRes( PATH_UI( "world_middle_ally.png" ) );
				}
				// 일반NPC스팟은 이름을 감춤
				if( m_pBaseSpot->GettypeSpot() == XGAME::xSPOT_NPC && !m_pBaseSpot->IsEventSpot() )
					bShowName = false;
				auto vImg = m_pImg->GetPosLocal();
				auto sizeImg = m_pImg->GetSizeLocal();
				// 이름앞에 레벨심볼 위치 조정
				XE::VEC2 v;
				v.x = vText.x - sizeImg.x - 3;
				v.y = vText.y + (pText->GetLayoutSize().h / 2.f) - (sizeImg.y / 2.f);
				m_pImg->SetPosLocal( v );
				// 레벨 텍스트
				if( m_pLevelText ) {
					m_pLevelText->SetbShow( TRUE );
					int level = 0;
					if( IsEnemySpot() ) {
						level = m_pBaseSpot->GetLevel();
						// 레벨이 지정되어있더라도 파워를 모르면 ?로 표시
						if( m_pBaseSpot->GetPower() == 0 )
							level = 0;
					} else
						level = ACCOUNT->GetLevel();
					if( level )
						m_pLevelText->SetText( XFORMAT( "%d", level ) );
					else {
						if( m_pBaseSpot->GetpBaseProp()->strIdentifier == _T( "spot.home" ) )
							m_pLevelText->SetText( XFORMAT("%d", ACCOUNT->GetLevel()));
						else
							m_pLevelText->SetText( _T( "?" ) );
					}
	//					auto vSizeLevel = m_pLevelText->GetLayoutSize();
	//					m_pLevelText->SetPosLocal( vImg + ( ( sizeImg - XE::VEC2( vSizeLevel.x / 2, vSizeLevel.y ) ) / 2 ) );
				}
			}
			pText->SetbShow( bShowName );
		}
		if( m_pBaseSpot->IsEventSpot() && m_pBaseSpot->IsActive() ) {
			auto pWnd = Find("spr.exclam");
			if( pWnd == nullptr ) {
				auto pExclam = new XWndSprObj( _T( "ui_exclam.spr" ), 1, XE::VEC2(0,0) );
				pExclam->SetstrIdentifier( "spr.exclam" );
				Add( pExclam );
				auto bb = GetBoundBoxLocal();
				auto vSize = pExclam->GetSizeLocal();
				pExclam->SetY( 5.f );
// 				pExclam->SetY( bb.vLT.y - 15.f );
			}
		}
		// 연패했을때 느낌표.
		if( m_pBaseSpot->IsSuccessiveDefeat() &&
			m_pBaseSpot->GettypeSpot() == XGAME::xSPOT_CASTLE ) {
			if( Find( "icon.alert" ) == nullptr )
			{
				auto pMark = new XWndSprObj( _T( "ui_alert.spr" ), 1, 8, 8 );
				pMark->SetstrIdentifier( "icon.alert" );
				Add( pMark );
			}
		}
		else {
			DestroyWndByIdentifier( "icon.alert" );
		}

	} // IsActive
	else {
		if( m_pTextName )
			m_pTextName->SetbShow( FALSE );	// 이름안나오는 스팟\도 있으니 일단 감춤.
		if( m_pImg )
			m_pImg->SetbShow( FALSE );
		if( m_pLevelText )
			m_pLevelText->SetbShow( FALSE );
		auto pImg = Find( "img.hard.lv" );
		if( pImg )
			pImg->SetbShow( false );
		
	}
	// 스팟 별점
	if( ACCOUNT->GetpQuestMng()->IsHaveGetStarQuest() ) {
		if( !m_pBaseSpot->IsEventSpot() ) {
			bool bShowStar = false;
			int numStar = ACCOUNT->GetNumSpotStar( m_pBaseSpot->GetidSpot() );
			if( m_pBaseSpot->GettypeSpot() == xSPOT_CASTLE
				|| m_pBaseSpot->GettypeSpot() == xSPOT_NPC ) {
				if( !m_pBaseSpot->IsEventSpot() ) {
					if( numStar < 3 && numStar >= 0 )
						bShowStar = true;
				}
			}
			if( bShowStar ) {
				const float wStar = 13.f;
				XE::VEC2 v( -((wStar * 3.f) * 0.5f), -53.f );
				for( int i = 0; i < 3; ++i ) {
					auto pImg = xGET_IMAGE_CTRLF( this, "img.star.%d", i + 1 );
					if( numStar >= i+1 ) {
						// on
						if( pImg == nullptr ) {
							pImg = new XWndImage( PATH_UI( "common_etc_smallstar.png" ), v );
							pImg->SetstrIdentifierf( "img.star.%d", i + 1 );
							Add( pImg );
						} else {
							pImg->SetSurfaceRes( PATH_UI( "common_etc_smallstar.png" ) );
						}
					} else {
						// off
						if( pImg == nullptr ) {
							pImg = new XWndImage( PATH_UI( "common_etc_smallstar_empty.png" ), v );
							pImg->SetstrIdentifierf( "img.star.%d", i + 1 );
							Add( pImg );
						} else {
							pImg->SetSurfaceRes( PATH_UI( "common_etc_smallstar_empty.png" ) );
						}
					}
					v.x += wStar;
				}
			} else {
				// 별표시 할필요 없으면 걍 윈도 삭제하면 됨.
				for( int i = 0; i < 3; ++i )
					DestroyWndByIdentifierf("img.star.%d", i+1 );
			}
		}
	}

#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( m_pTextDebug ) {
		if( XAPP->GetbDebugMode() ) {
			_tstring strText = GetstrDebugText();
			m_pTextDebug->SetbShow( TRUE );
			m_pTextDebug->SetText( strText );
			auto vSize = m_pTextDebug->GetLayoutSize();
			auto vPos = XE::VEC2( -vSize.x / 4, GetSizeLocal().y / 2 );
			vPos.y += 15.f;
			m_pTextDebug->SetPosLocal( vPos );
		} else {
			m_pTextDebug->SetbShow( FALSE );
		}
	}
#endif
	XWndSprObj::Update();
}

/**
 @brief 스팟의 이름부분을 업데이트한다.
 @param cIdentifier 스팟컨트롤의 ids를 지정한다.
 @param bShow 이름을 표시해야하는지
*/
XWndTextString* XWndSpot::UpdateName( const char* cIdentifier, bool bShow )
{
	// 애초에 이름을 출력할필요 없는 스팟은 이름컨트롤을 만들지 않는다.
	if( !bShow )
		return nullptr;
	auto pText = XWndTextString::sUpdateCtrl( this
																					, cIdentifier
																					, XE::VEC2(0)
																					, FONT_NANUM, 18.f );
	if( pText ) {
		pText->SetStyleStroke();
		pText->SetbShow( true );
		bool bNameColorByPower = false;
		pText->SetText( GetstrName( &bNameColorByPower ) );
		XCOLOR colName = XCOLOR_WHITE;
		if( bNameColorByPower ) {
			if( m_pBaseSpot->GetPower() )
				colName = XGAME::GetColorPower( m_pBaseSpot->GetPower()
																			, ACCOUNT->GetPowerExcludeEmpty() );
		}
		pText->SetColorText( colName );	// 만약 또다른 예외가 있다면 GetColorName() virtual을 만들어서 쓸것.
	}
	return pText;
}

/**
 @brief 스팟의 레벨을 업데이트한다.
 레벨바탕의 이미지와 텍스트를 지정해야한다.
*/
XWndTextString* XWndSpot::UpdateLevel( const char* cIdentifier, bool bShow )
{
	// 애초에 출력할필요 없는 스팟은 컨트롤을 만들지 않는다.
	if( !bShow )
		return nullptr;
	// 디폴트 레벨바탕 이미지
	// PATH_UI("level_bg.png")
	const _tstring resImg = GetresImgByLevel();		// virtual
	auto pImg = XWndImage::sUpdateCtrl( this
																		, XE::VEC2(0)
																		, resImg
																		, true			// bAlpha
																		, "img.level" );
	if( pImg ) {
		auto pText = XWndTextString::sUpdateCtrl( this
																						, cIdentifier
																						, XE::VEC2(0)
																						, FONT_RESNUM, 25 );
		if( pText ) {
			pText->SetStyleStroke();
			pText->SetbShow( true );
			bool bNameColorByPower = false;
			pText->SetText( XFORMAT("%d", m_pBaseSpot->GetLevel()) );
			pText->SetColorText( XCOLOR_RGBA(255,204,0,255) );
			pText->SetAlignCenter();
			pImg->Add( pText );
		}
		return pText;
	}
	return nullptr;
}

void XWndSpot::OnAutoUpdate()
{
	if( m_pTextDebug ) {
		if( XAPP->GetbDebugMode() ) {
			_tstring strText = GetstrDebugText();
			m_pTextDebug->SetText( strText );
		}
	}
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
	m_psoEff->Draw( GetPosFinal() );
	if( getid() == s_idLastTouch ) {
		if( m_pBaseSpot->IsActive() )
			m_psoIndicate->Draw( GetPosFinal() );
	}
	XWndSprObj::Draw();
#ifdef _CHEAT
	if( XAPP->m_bDebugMode && XAPP->m_bDebugViewBoundBoxSpot ) {
		XE::xRECT rect = GetBoundBox();
		GRAPHICS->DrawRect( rect.vLT, rect.vRB, XCOLOR_BLUE );
		const auto vPos = GetPosFinal();
		const float sizeCross = 20.f;
		GRAPHICS->DrawHLine( vPos.x - (sizeCross * 0.5f), vPos.y, sizeCross, XCOLOR_BLUE );
		GRAPHICS->DrawVLine( vPos.x, vPos.y - (sizeCross * 0.5f), sizeCross, XCOLOR_BLUE );
	}
#endif // _CHEAT
}

_tstring XWndSpot::GetstrName()
{
	return m_pBaseSpot->GetstrName();
}

////////////////////////////////////////////////////////////////
XWndCastleSpot::XWndCastleSpot( XSpotCastle* pSpot )
	: XWndSpot( pSpot, SPR_CASTLE, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
#ifdef _CHEAT
	SetAutoUpdate( 0.1f );
#endif
//	SetbUpdate( true );

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
			if( amount ) {
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
		strText += XE::Format( _T( "(Lv%d)(sc:%d)" ), m_pSpot->GetLevel(), m_pSpot->GetScore() );
		strText += XE::Format( _T( ":(%d sec)\n" ), (int)m_pSpot->GetTimerCalc().GetsecRemainTime() );
		strText += XE::Format( _T( "%s" ), m_pSpot->GetStrLocalStorage().c_str() );
	}
#endif
	return strText;
}

_tstring XWndCastleSpot::GetstrName()
{
	_tstring strText;
	auto pSpot = SafeCast<XSpotCastle*>( GetpBaseSpot() );
	if( pSpot->GetidOwner() == ACCOUNT->GetidAccount() )
		strText = ACCOUNT->GetstrName();
	else
		strText = GetpBaseSpot()->GetstrName();
#ifdef _CHEAT
	// 디버그용 텍스트 출력
	if( XAPP->GetbDebugMode() ) {
		if( pSpot->GetidOwner() != ACCOUNT->GetidAccount() )
			strText += XFORMAT("(%d)", pSpot->GetidOwner() );
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
	// 방어도
	auto pText = new XWndTextString( XE::VEC2( 0 ), _T( "" ), FONT_NANUM, 20.0f );
	if( pText ) {
		pText->SetstrIdentifier( "text.def.mine" );
		pText->SetStyle( xFONT::xSTYLE_STROKE );
		Add( pText );
		// 광산레벨바탕 심볼 이미지
		auto pImg = new XWndImage( PATH_UI( "bg_level.png" ), 0, 0 );
		if( pImg ) {
			pImg->SetstrIdentifier("img.bg.level.mine");
			Add( pImg );
			// 레벨 텍스트
			pText = new XWndTextString( nullptr, FONT_NANUM, 20.0f );
			pText->SetstrIdentifier("text.level.mine");
			pText->SetStyle( xFONT::xSTYLE_STROKE );
			pText->SetAlignCenter();
			pImg->Add( pText );
		}
	}

}

void XWndJewelSpot::Destroy()
{
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
		// 광산 방어도 및 광산레벨표시
		auto pTextDef = xGET_TEXT_CTRL( this, "text.def.mine" );
		if( pTextDef ) {
			pTextDef->SetText( XFORMAT( "%s:%d", XTEXT( 2153 ), m_pSpot->GetDefense() ) );
			auto sizeDef = pTextDef->GetLayoutSize();
			pTextDef->SetPosLocal( XE::VEC2( ( -sizeDef.x ) / 4, 
												-(GetSizeLocal().h / 2) ) );
			auto vTextDef = pTextDef->GetPosLocal();
			XWnd *pImg = Find("img.bg.level.mine");
			if( pImg ) {
				auto pTextLv = xGET_TEXT_CTRL( this, "text.level.mine" );
				if( pTextLv ) {
					pTextLv->SetText( XFORMAT( "%d", m_pSpot->GetlevelMine() ) );
				}
				auto sizeImg = pImg->GetSizeFinal();
				// 이름앞에 레벨심볼 위치 조정
				pImg->SetPosLocal( vTextDef.x - sizeImg.x - 3,
					vTextDef.y + ( sizeDef.y - sizeImg.y ) );
			}
		}
	}
	XWndSpot::Update();
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

	SetSizeLocal( 45, 45 );
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 1.f );
#endif
}

void XWndSulfurSpot::Destroy()
{
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
		strText = XE::Format( _T( "%s(%d):%d(%d)" ), m_pSpot->GetszName(),
								m_pSpot->GetLevel(),
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
	: XWndSpot( pSpot, SPR_MANDRAKE, 
				pSpot->GetPosWorld() )
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
	SetbNameColorByPower( true );	// 스팟점유자의 전투력을 비교해서 색등급으로 표시함.

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
				XE::VEC2 sizeText = m_pTextWin->GetLayoutSize();
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
	switch( state ) {
	case xSM_NO_MATCH: return _tstring(_T("?"));
	case xSM_CHALLENGE: 
		*pOutbNameColorByPower = true;
		return m_pSpot->GetstrName();
		break;
	case xSM_DEFENSE: 
		return ACCOUNT->GetstrName();
	default:
		XBREAK(1);
		break;
	}
	return _T("");
}

// XCOLOR 
// 
// /**
//  @brief 이름부분 컨트롤의 업데이트를 한다.
// */
// XWndTextString* XWndMandrakeSpot::UpdateName( const char* cIdentifier, bool bShow )
// {
// 	const ID idAcc = ACCOUNT->GetidAccount();
// 	auto pText = XWndSpot::UpdateName( cIdentifier, bShow );
// 	if( pText == nullptr )
// 		return nullptr;
// 	const auto state = m_pSpot->GetState( idAcc );
// 	switch( state ) {
// 	case xSM_NO_MATCH: {
// 		pText->SetText( _T("?") );
// 		pText->SetColorText( XCOLOR_WHITE );
// 	} break;
// 	case xSM_CHALLENGE: {
// 		pText->SetText( m_pSpot->GetstrName() );
// 		auto col = XCOLOR_WHITE;
// 		if( m_pSpot->GetPower() ) {
// 			col = XGAME::GetColorPower( m_pSpot->GetPower(), ACCOUNT->GetPowerExcludeEmpty() );
// 		}
// 		pText->SetColorText( col );
// 	} break;
// 	case xSM_DEFENSE: {
// 		pText->SetText( m_pSpot->GetstrName() );
// 		pText->SetColorText( XCOLOR_WHITE );
// 	} break;
// 	default:
		break;
// 	}
// 	return pText;
// }

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
								m_pSpot->GetidEnemy(),
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
#ifdef _CHEAT
	if( XAPP->m_bDebugMode )
		SetAutoUpdate( 1.f );
#endif
}
void XWndNpcSpot::SetSprSpot()
{
	if( m_pSpot->IsActive() ) {
// 		auto clan = m_pSpot->GetpProp()->clan;
// 		switch( clan )
// 		{
// 		case XGAME::xCL_CROW:		SetSprObj( SPR_NPC_CROW );	break;
// 		case XGAME::xCL_IRONLORD:	SetSprObj( SPR_NPC_IRONLORD );	break;
// 		case XGAME::xCL_FREEDOM:	SetSprObj( SPR_NPC_FREEDOM );	break;
// 		case XGAME::xCL_ANCIENT:	SetSprObj( SPR_NPC_ANCIENT );	break;
// 		case XGAME::xCL_FLAME:		SetSprObj( SPR_NPC_FLAME );	break;
// 		default:
			if( GetpBaseSpot()->GetpBaseProp()->strSpr.empty() ) {
				// 1레벨 지역이고 계정렙이 5이하이면 야만족만 나오게 한다.
				if( m_pSpot->GetpAreaProp()->lvArea == 1 && ACCOUNT->GetLevel() <= 5 )
					SetSprObj( SPR_NPC3 );	// 야만족
				else
					SetSprObj( SPR_NPC );	// 제국 부대
			} else
				SetSprObj( GetpBaseSpot()->GetpBaseProp()->strSpr, 1 );
// 			break;
// 		}
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
		if( m_pSpot->IsDestroy() )
		{
			SetbDestroy( TRUE );
			return;
		}
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
		strText = XE::Format( _T( "lv%d:id:%d(%d)" ), m_pSpot->GetLevel(),
							getid(),
							(int)m_pSpot->GettimerSpawn().GetsecRemainTime() );
	}
#endif
	return strText;
}

_tstring XWndNpcSpot::GetstrName()
{
	_tstring strName = GetpBaseSpot()->GetstrName();
	return strName;
}

///////////////////////////////////////////////////////////////
XWndDailySpot::XWndDailySpot( XSpotDaily* pSpot )
	: XWndSpot( pSpot, SPR_DAILY, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	// 
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
// 		strOut += XFORMAT( "Lv%d %s\n", m_pSpot->GetLevel(), m_pSpot->GetszName() );
// 		if( m_pSpot->GettimerEnter().IsOn() ) {
// 			int secRemain = (int)m_pSpot->GettimerEnter().GetsecRemainTime();	// 남은시간(초)
// 			int hour, min, sec;
// 			XTimer2::sGetHourMinSec( secRemain, &hour, &min, &sec );
// 			strOut += XE::Format( _T( "남은시간:\n<%0d:%02d:%02d>\n" ),
// 									hour, min, sec );
// 		} else {
// 			strOut += _T( "미입장\n" );
// 		}
		strOut += XFORMAT( "Lv%d %s\n", m_pSpot->GetLevel(), m_pSpot->GetszName() );
		strOut += XE::Format( _T( "입장횟수:%d/%d" ), m_pSpot->GetnumEnter(), 
													XSpotDaily::xNUM_ENTER );
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

//////////////////////////////////////////////////////////////
XWndSpecialSpot::XWndSpecialSpot( XSpotSpecial* pSpot )
	: XWndSpot( pSpot, SPR_SPECIAL, pSpot->GetPosWorld() )
{
	Init();
	m_pSpot = pSpot;
	// 
}

void XWndSpecialSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
}

void XWndSpecialSpot::UpdateInfoText( _tstring& strOut )
{
	if( m_pSpot )
	{
		strOut += XFORMAT("Lv%d %s\n", m_pSpot->GetLevel() + m_pSpot->GetidxRound(), 
										m_pSpot->GetszName() );
		int secRemain = (int)m_pSpot->GettimerRecharge().GetRemainSec();	// 남은시간(초)
		int hour, min, sec;
		XTimer2::sGetHourMinSec( secRemain, &hour, &min, &sec );
		strOut += XE::Format( _T( "충전시간: <%d:%d>\n라운드:%d\n총 입장횟수:%d\n남은 입장횟수:%d" ),
								min, sec,
								m_pSpot->GetidxRound() + 1,
								m_pSpot->GetnumEnter(), 
								m_pSpot->GetnumEnterTicket() );
	}
}

_tstring XWndSpecialSpot::GetstrDebugText()
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
	// Campaign이름
// 	XWndTextString *pText = new XWndTextString( 11, -30, pSpot->GetszName(), GAME->GetpfdSystem(), XCOLOR_RGBA(185,0,225,255) );
// 	pText->SetStyle( xFONT::xSTYLE_STROKE );
// 	Add( pText );
// 	m_pText = pText;
// 	SetbUpdate( TRUE );
}

void XWndCampaignSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
// #ifdef _CHEAT
// 		_tstring str;
// 		if( XAPP->m_bDebugMode )
// 			str = XE::Format( _T( "Lv%d %s:id:%d" ), m_pSpot->GetLevel(), 
// 													m_pSpot->GetszName(),
// 													getid() );
// 		else
// 			str = XFORMAT( "Lv%d %s", m_pSpot->GetLevel(), m_pSpot->GetszName() );
// #else
// 		_tstring str = XE::Format( _T( "%s(%d)" ), m_pSpot->GetszName(),
// 													m_pSpot->GetLevel() );
// #endif
// 		m_pText->SetText( str.c_str() );
// 		XE::VEC2 vSizeLabel = m_pText->GetLayoutSize();
// 		XE::VEC2 vPosText = m_pText->GetPosLocal();
// 		XE::VEC2 vPosSpot = GetPosFinal();
// 		XE::VEC2 vSizeSpot = GetSizeFinal();
// //		XE::VEC2 vLT = vPosSpot + vSizeSpot / 2.f;	// 스팟의 중심좌표
// //		vLT -= vSizeLabel / 2.f;	// 스팟을 중심으로 텍스트레이블을 중앙정렬했을때 텍스트 좌상귀
// 		vPosText.x = -(vSizeLabel.w / 2.f);
// 		m_pText->SetPosLocal( vPosText );
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
	// Visit이름
// 	m_pText = new XWndTextString( XE::VEC2( 0, 0 ), _T("") );
// 	if( m_pText )
// 	{
// 		m_pText->SetStyle( xFONT::xSTYLE_STROKE );
// 		if( pSpot->IsEventSpot() )
// 			m_pText->SetColorText( XCOLOR_YELLOW );
// 		Add( m_pText );
// 	}
// 	SetbUpdate( TRUE );
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
	GetpImg()->SetbShow( FALSE );
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
}

void XWndCommonSpot::Update( void )
{
	if( m_pSpot ) {
		if( m_pSpot->IsDestroy() ) {
			SetbDestroy( TRUE );
			return;
		}
	}
	XWndSpot::Update();
}

bool XWndCommonSpot::IsEnemySpot()
{
	auto& idsSpot = GetpBaseSpot()->GetpBaseProp()->strIdentifier;
	if( idsSpot == _T("spot.home") )
		return false;
	if( !m_pSpot->IsCampaignType() )
		return false;
	return true;
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


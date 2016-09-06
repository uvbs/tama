#include "stdafx.h"
#include "XWndCloudLayer.h"
#include "XGame.h"
#include "XSceneWorld.h"
#include "XPropCloud.h"
#include "client/XAppMain.h"
#include "XWorld.h"
#include "XWndTemplate.h"
#include "XFramework/Game/XEComponents.h"
#include "_Wnd2/XWndImage.h"
#include "XAccount.h"
//#include "XGameWnd.h"
#include "XWndResCtrl.h"
#include "Sprite/Sprite.h"
#include "Sprite/SprMng.h"
#include "Sprite/SprDat.h"
#include "_Wnd2/XWndText.h"

#ifdef _xIN_TOOL
#include "XTool.h"
#endif // _xIN_TOOL

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;

////////////////////////////////////////////////////////////////
XWndCloudLayer::XWndCloudLayer( const XE::VEC2& vPos, 
								const XE::VEC2& vSize )
	: XWnd( vPos.x, vPos.y, vSize.w, vSize.h )
{
	Init();
	m_psdCloud = SPRMNG->Load( _T("cloud.spr"), XE::xHSL(), false );
//	SetbTouchable( FALSE );
	m_objMove[0].SetState1();
	m_objMove[1].SetState2();
}

void XWndCloudLayer::Destroy()
{
	SPRMNG->Release( m_psdCloud );
}

void XWndCloudLayer::UpdateClouds( XArrayLinearN<ID, 512>& ary )
{
	m_aryCloud.Clear();
	m_aryCloud = ary;
}

void XWndCloudLayer::UpdateClouds( const std::vector<ID>& ary )
{
	XBREAK( (int)ary.size() > m_aryCloud.GetMax() );
	m_aryCloud.Clear();
	for( auto idCloud : ary )
		m_aryCloud.Add( idCloud );
}

void XWndCloudLayer::DoDisappear( ID idCloudFade )
{
	xtDisappear fade;
	fade.timerFade.Set( 1.f );
	fade.idCloud = idCloudFade;
	AddDisappear( fade );
	XARRAYLINEARN_LOOP( m_aryCloud, ID&, idCloud ) {
		if( idCloud == idCloudFade ) {
			idCloud = 0;
			break;
		}
	} END_LOOP;
}

void XWndCloudLayer::AddDisappear( xtDisappear& fadeCloud )
{
	m_listDisappear.Add( fadeCloud );
}

/**
 @brief 구름어레이에서 idFind노드를 찾아 idNew값으로 바꿔넣는다.
*/
BOOL XWndCloudLayer::ChangeidCloud( ID idFind, ID idNew )
{
	int size = m_aryCloud.size();
	for( int i = 0; i < size; ++i ) {
		ID& idCloud = m_aryCloud[i];
		if( idCloud == idFind ) {
			idCloud = idNew;
			return TRUE;
		}
	}
	return FALSE;
}

int XWndCloudLayer::Process( float dt )
{
#ifdef _CHEAT
	if( XAPP->m_nToolShowCloud == 0 /*|| XAPP->IsToolSpotMode()*/ )
		SetbShow( FALSE );
	else 
		SetbShow( TRUE );
#endif
//	XLIST_LOOP_MANUAL( m_listDisappear, xtDisappear, itor, pFade )
	for( auto itor = m_listDisappear.begin(); itor != m_listDisappear.end(); ) {
		const auto& fadeCloud = (*itor); 
		if( fadeCloud.timerFade.IsOver() ) {
			m_listDisappear.erase( itor++ );
//			ChangeidCloud( pFade->idCloud, 0 );
//			XLIST_DEL( m_listDisappear, itor );
		} else {
			++itor;
		}
	}// END_LOOP;
	m_objMove[0].Process( dt );
	m_objMove[1].Process( dt );
	return XWnd::Process( dt );
}


/**
 @brief  사라지고 있는 구름인가.
*/
BOOL XWndCloudLayer::GetDisappearCloud( ID idCloud, xtDisappear *pOut )
{
	XBREAK( pOut == nullptr );
//	XLIST_LOOP( m_listDisappear, xtDisappear, fade )
	for( const auto& fade : m_listDisappear ) {
		if( fade.idCloud == idCloud ) {
			*pOut = fade;
			return TRUE;
		}
	}// END_LOOP;
	return FALSE;
}

void XCloudMove::Process( float dt )
{
	m_vOffset += XE::VEC2( c_speed ) * dt;
	// 2초간 나타나고
	if( m_State == 0 ) {
		if( m_timer.IsOff() )
			m_timer.Set( 2.f );
		m_alpha = m_timer.GetSlerp();
		if( m_alpha > 1.f )
			m_alpha = 1.f;
		if( m_alpha >= 1.f ) {
			++m_State;
			m_timer.Off();
		}
	} else
	// 3초간 움직이고
	if( m_State == 1 ) {
		m_alpha = 1.f;
		if( m_timer.IsOff() )
			m_timer.Set( 4.f );
		if( m_timer.IsOver() ) {
			m_timer.Off();
			++m_State;
		}
	} else 
	// 2초간 사라지고
	if( m_State == 2 ) {
		if( m_timer.IsOff() )
			m_timer.Set( 2.f );
		float lerpTime = m_timer.GetSlerp();
		if( lerpTime > 1.f )
			lerpTime = 1.f;
		m_alpha = 1.f - lerpTime;
		if( lerpTime >= 1.f ) {
			SetState1();
			m_timer.Off();
// 			m_vOffset.Set(0);
		}
	}
// 	} else
// 	// 3초간 대기하고
// 	if( m_State == 3 ) {
// 		m_alpha = 0.f;
// 		if( m_timer.IsOff() )
// 			m_timer.Set( 3.f );
// 		if( m_timer.IsOver() ) {
// 			m_timer.Off();
// 			SetState1();
// 		}
// 	}
}

/*
헥사구름의 집합인 구름그룹 리스트를 갖는다. 각 그룹은 아이디가 존재한다.
그룹에는 그룹에 속해있는 헥사인덱스 리스트를 갖는다.
그룹리스트 단위로 루프를 돌면서 하위에 속한 헥사구름을 찍는다.
각 헥사구름의 인덱스를 기반으로 x,y로 분해한 후 x,y좌표를 산출해서 찍는다.
*/
void XWndCloudLayer::Draw()
{
#ifdef _xIN_TOOL
	const auto modeCurr = XBaseTool::sGetCurrMode();
	bool bToolMode = ( modeCurr == xTM_CLOUD || modeCurr == xTM_SPOT);
	if( bToolMode ) {
		XArrayLinearN<ID, 512> ary;
		PROP_CLOUD->GetPropToAry( &ary );
		// 툴모드에선 육각형만 그린다.
		DrawCloudLayer( ary, true, XE::VEC2(), 1.f );
	} else 
#endif // _xIN_TOOL
	{
		DrawCloudLayer( m_aryCloud, false, m_objMove[ 0 ].GetvOffset(), m_objMove[ 0 ].Getalpha() );
		DrawCloudLayer( m_aryCloud, false, m_objMove[ 1 ].GetvOffset(), m_objMove[ 1 ].Getalpha() );
	}
#ifdef _xIN_TOOL
	if( bToolMode == false )
#endif // _xIN_TOOL
		XWnd::Draw();
}


BOOL XWndCloudLayer::IsWndAreaIn( float lx, float ly ) 
{
	auto pScrlView = SafeCast<XWndScrollView*, XWnd*>( GetpParent() );
	if( pScrlView ) {
		XE::VEC2 vwMouse( lx, ly );
		int idx = PROP_CLOUD->GetIdxHexaFromWorld( vwMouse.x, vwMouse.y );
		XPropCloud::xCloud *pProp = PROP_CLOUD->GetpPropByidxHexa( idx );
		// 찍은곳에 구름이 없어야만 구름밑에 객체를 클릭할수 있다.
		if( pProp && sGetpWorld()->IsOpenedArea( pProp->idCloud ))
			return FALSE;
		return TRUE;
	}
	return FALSE;
}

void XWndCloudLayer::DrawCloudLayer( XArrayLinearN<ID, 512>& aryCloud, 
									bool bOutlineOnly, 
									const XE::VEC2& vOffset, 
									float alphaBase )
{
	const XE::VEC2 sizeHexa = XGAME::x_sizeHexa;
	XE::VEC2 sizeLayer = GetSizeFinal();
	int nw = (int)( sizeLayer.w / sizeHexa.w );
	int nh = (int)( sizeLayer.h / sizeHexa.h ) + 1;
	const XE::POINT nHexa(nw, nh);
	//
	XARRAYLINEARN_LOOP( aryCloud, ID, idCloud ) {
		auto pProp = PROP_CLOUD->GetpProp( idCloud );
		if( pProp == NULL )
			continue;
		DrawCloudArea( pProp, nHexa, bOutlineOnly, vOffset, alphaBase );
	} END_LOOP; 
	// 사라지고 있는 구름은 별도로 리스트에 보관해서 처리한다.
	for( const auto& fade : m_listDisappear ) {
		auto pProp = PROP_CLOUD->GetpProp( fade.idCloud );
		if( pProp == nullptr )
			continue;
		DrawCloudArea( pProp, nHexa, bOutlineOnly, vOffset, alphaBase );
	}
}

void XWndCloudLayer::DrawCloudArea( XPropCloud::xCloud* pProp
																		, const XE::POINT& nHexa
																		, bool bOutlineOnly
																		, const XE::VEC2& vOffset
																		, float alphaBase )
{
#ifdef _xIN_TOOL
	const auto modeCurr = XBaseTool::sGetCurrMode();
	bool bToolMode = (modeCurr == xTM_CLOUD || modeCurr == xTM_SPOT);
#endif // _xIN_TOOL
	auto sizeWin = GRAPHICS->GetViewportSize();
	const auto vStart = GetPosFinal();
	const auto sizeHexa = XGAME::x_sizeHexa;
	const ID idCloud = pProp->idCloud;
	XE::VEC2 vSum;
	for( const auto& hexa : pProp->idxs ) {
		int idxx = hexa.idx % (nHexa.w + 1);
		int idxy = hexa.idx / (nHexa.w + 1);
		XE::VEC2 v;
		v.x = vStart.x + idxx * sizeHexa.w;
		v.y = vStart.y + idxy * sizeHexa.h;
		if( idxy & 1 )
			v.x -= sizeHexa.w / 2.f;
		static float mm = 2.f;
		XE::VEC2 vt = vOffset + v + (XE::VEC2( -20, -20 ) * mm);
		const float scaleCloud = 1.4f;
		vSum += v;		// 헥사타일들의 좌표를 모두 더한다.
		BOOL bDraw = TRUE;
		XSprite *psfcCloud = m_psdCloud->GetSprite( hexa.idxImg );
		const auto vSize = psfcCloud->GetSize() * scaleCloud;
		if( v.x > sizeWin.w )
			bDraw = FALSE;
		else if( v.x + vSize.w < 0 )
			bDraw = FALSE;
		else if( v.y > sizeWin.h )
			bDraw = FALSE;
		else if( v.y + vSize.h < 0 )
			bDraw = FALSE;
		if( bDraw ) {
			if( !bOutlineOnly ) {
				DrawHexaElem( vt, idCloud, hexa, psfcCloud, alphaBase, scaleCloud );
			} else {
#ifdef _xIN_TOOL
				DrawHexaElemByOutline( pProp, v, idCloud, hexa, psfcCloud, alphaBase, scaleCloud );
#endif // inTool
			}
		}
	} // loop hexa
#ifdef _xIN_TOOL
		// 헥사타일들의 중간좌표
	if( bToolMode ) {
		DrawHexaDebugInfo( pProp, vSum );
	}
#endif // _xIN_TOOL
}
void XWndCloudLayer::DrawHexaElem( const XE::VEC2& vt
																	 , ID idCloud
																	 , const XPropCloud::xHexa& hexa
																	 , XSprite* psfcCloud
																	 , float alphaBase
																	 , float scaleCloud )
{
	float alpha = alphaBase;
	if( hexa.bOpened )	// 오픈된 구름은 알파 절반으로 찍는다.
		alpha *= 0.8f;
	xtDisappear fade;
	BOOL bDisappear = GetDisappearCloud( idCloud, &fade );
	if( bDisappear ) {
		float slerp = 1.0f - fade.timerFade.GetSlerp();
		if( slerp < 0 )
			slerp = 0;
		alpha *= slerp;
	}
	alpha *= 0.875f;//alpha *= 0.75f;		// 이제 항상 구름은 이 값으로 찍는다. 이게 더 보기가 좋더라.
	psfcCloud->SetfAlpha( alpha );
	psfcCloud->SetScale( scaleCloud );
	psfcCloud->Draw( vt );
}

/**
 @brief 사라지는 중의 헥사그리기
*/
// void XWndCloudLayer::DrawHexaByFade( const XE::VEC2& vt
// 																	 , ID idCloud
// 																	 , const XPropCloud::xHexa& hexa
// 																	 , XSprite* psfcCloud
// 																	 , float alphaBase
// 																	 , float scaleCloud )
// {
// 	float alpha = 1.f;
// 	if( hexa.bOpened )	// 오픈된 구름은 알파 절반으로 찍는다.
// 		alpha *= 0.8f;
// 	xtDisappear fade;
// 	BOOL bDisappear = GetDisappearCloud( idCloud, &fade );
// 	if( bDisappear ) {
// 		float slerp = 1.0f - fade.timerFade.GetSlerp();
// 		if( slerp < 0 )
// 			slerp = 0;
// 		alpha *= slerp;
// 	}
// 	alpha *= 0.875f;//alpha *= 0.75f;		// 이제 항상 구름은 이 값으로 찍는다. 이게 더 보기가 좋더라.
// 	psfcCloud->SetfAlpha( alpha * alphaBase );
// 	psfcCloud->SetScale( scaleCloud );
// 	psfcCloud->Draw( vt );
// }

#ifdef _xIN_TOOL
static const XCOLOR cols[] = {
	XCOLOR_BLACK,
	XCOLOR_WHITE,
	XCOLOR_PURPLE,
	XCOLOR_BLUE,
	XCOLOR_SKY,
	XCOLOR_GREEN,
	XCOLOR_YELLOW,
	XCOLOR_ORANGE,
	XCOLOR_DARKGRAY,
	XCOLOR_LIGHTGRAY,
	XCOLOR_DARKGRAY,
	XCOLOR_BROWN,
	XCOLOR_ORANGE,
};
void XWndCloudLayer::DrawHexaElemByOutline( XPropCloud::xCloud* pProp
																					 , const XE::VEC2& v
																					 , ID idCloud
																					 , const XPropCloud::xHexa& hexa
																					 , XSprite* psfcCloud
																					 , float alphaBase
																					 , float scaleCloud )
{
	int maxCols = XNUM_ARRAY( cols );
	XCOLOR col = cols[idCloud % maxCols];
	if( m_idSelected == idCloud )
		col = XCOLOR_RED;
	const auto modeCurr = XBaseTool::sGetCurrMode();
	bool bToolMode = (modeCurr == xTM_CLOUD || modeCurr == xTM_SPOT);
	const XE::VEC2 sizeHexa = XGAME::x_sizeHexa;
	XE::VEC2 vc = v + HEXA_CENTER_OFFSET;
	auto vt = v + XE::VEC2( 30, 10 );
	float r = sizeHexa.w / 2.f + 3.f;
	GRAPHICS->DrawCircle( vc.x, vc.y, r, col, 6, (360.f / 6.f) / 2.f );
	if( bToolMode ) {
		if( !pProp->strIdentifier.empty() ) {
			if( hexa.bOpened )
				GRAPHICS->DrawCircle( vc.x, vc.y, r*0.8f, col, 6, (360.f / 6.f) / 2.f );
			if( XAPP->m_bViewHexaIdx ) {
				PUT_STRINGFV( vt, col, "hexa:%d", hexa.idx );
				vt.y += 10.f;
			}
		}
	}
}

void XWndCloudLayer::DrawHexaDebugInfo( XPropCloud::xCloud* pProp, const XE::VEC2& vSum )
{
	const auto idCloud = pProp->idCloud;
	int maxCols = XNUM_ARRAY( cols );
	XCOLOR col = cols[idCloud % maxCols];
	if( m_idSelected == idCloud )
		col = XCOLOR_RED;
	int numHexa = 1;
	if( pProp->idxs.size() > 0 )
		numHexa = pProp->idxs.size();
	const auto style = xFONT::xSTYLE_SHADOW;
	XE::VEC2 vAvg = vSum / (float)numHexa;
	auto vt = vAvg;
	if( XAPP->m_bViewAreaCost ) {
		TCHAR szCost[256];
		XE::NumberToMoneyString( szCost, pProp->cost );
		PUT_STRINGFV_STYLE( vt, col, style, "%s", szCost );
		vt.y += 10.f;
	}
	PUT_STRINGFV_STYLE( vt, col, style, "%s(%d)", pProp->strIdentifier.c_str(), pProp->idCloud );
	vt.y += 10.f;
	PUT_STRINGFV_STYLE( vt, col, style, "lvArea:%d/lvOpen:%d", pProp->lvArea, pProp->lvOpenable2 );
	vt.y += 10.f;
	if( !pProp->idsPrecedeArea.empty() ) {
		PUT_STRINGFV_STYLE( vt, col, style, "pre:%s", pProp->idsPrecedeArea.c_str() );
		vt.y += 10.f;
	}
}
#endif

////////////////////////////////////////////////////////////////
/**
 @brief 
*/
XWndAreaBanner::XWndAreaBanner( const XE::VEC2& vCenter, ID idArea, LPCTSTR szName )
//	: XWndImage( PATH_UI("area_banner.png"), XE::VEC2( 0 ) )
{
	Init();
	auto pImgBg = new XWndImage( PATH_UI( "area_banner.png" ), XE::VEC2( 0 ) );
	Add( pImgBg );
	m_pImgBg = pImgBg;
	auto vSize = pImgBg->GetSizeLocal();
	SetSizeLocal( vSize );
	XE::VEC2 vLT = vCenter - ( vSize * 0.5f );
	SetPosLocal( vLT );
	m_idArea = idArea;
	auto pProp = PROP_CLOUD->GetpProp( idArea );
	if( pProp ) {
		auto pText = new XWndTextString( _T( "" ), _T("res_num.ttf"), 18.f, XCOLOR_YELLOW );
		pText->SetSizeLocal( vSize );
		pText->SetY( 32.f );
		pText->SetstrIdentifier("text.level");
 		pText->SetAlign( XE::xALIGN_HCENTER );
		pText->SetStyleStroke();
		Add( pText );
		auto pWndRes = new XWndResourceCtrl( XE::VEC2(0) );
		pWndRes->SetstrIdentifier( "wnd.res" );
		pWndRes->AddRes( xRES_GOLD, pProp->cost, XCOLOR_YELLOW );		// 폰트 크기 지정해야함.
		pImgBg->Add( pWndRes );
		pWndRes->AutoLayoutCenter();
#ifdef _CHEAT
		if( XAPP->m_bDebugMode ) {
			auto pText = new XWndTextString( XE::VEC2(0,-10)
																			, XFORMAT( "(%s:%d)", pProp->strIdentifier.c_str(), pProp->idCloud ) );
			pImgBg->Add( pText );
		}
#endif // _CHEAT
	}
}

void XWndAreaBanner::Update()
{
	auto pProp = PROP_CLOUD->GetpProp( m_idArea );
	if( pProp ) {
		auto pWndRes = SafeCast<XWndResourceCtrl*>( m_pImgBg->Find("wnd.res") );
		if( pWndRes ) {
#ifdef _CHEAT
			bool bShowDebug = (XAPP->m_bDebugMode != FALSE);
			auto pText = XWndTextString::sUpdateCtrl( m_pImgBg
																							, "text.banner.debug"
																							, XE::VEC2( 0, -10 )
																							, bShowDebug );
			if( pText )
				pText->SetText( XFORMAT( "(%s:%d)", pProp->strIdentifier.c_str(), pProp->idCloud ) );
#endif // _CHEAT
			if( !ACCOUNT->IsEnoughGold( pProp->cost ) )
				pWndRes->EditResColor( xRES_GOLD, XCOLOR_RED );
		}
		const auto vC = m_pImgBg->GetSizeLocalNoTrans() * 0.5f;
		auto pImgLock = Find( "img.lock" );
		bool bLock = ACCOUNT->IsLockArea( pProp );
		if( bLock ) {
				if( pImgLock == nullptr ) {
					// 잠금이미지
					auto pImg = new XWndImage( PATH_UI( "worldmap_lock.png" ), XE::VEC2(0,10) );
					auto sizeLock = pImg->GetSizeLocal() / 2.f;
 					sizeLock.w = 0;
					pImg->SetPosLocal( -sizeLock + XE::VEC2(0,12) );
					pImg->SetstrIdentifier( "img.lock" );
					m_pImgBg->Insert( "wnd.res", pImg );
// 					Add( pImg );
					pImg->AutoLayoutHCenter();
				} else
					pImgLock->SetbShow( true );
		} else {
			if( pImgLock )
				pImgLock->SetbShow( false );
		}
		// 유닛 얼굴이 필요할때.
		if( pProp->unitUnlock ) {
			auto unit = XGAME::xUNIT_NONE;
			if( pProp->unitUnlock == XGAME::xUNIT_ANY ) {
				XVector<XGAME::xtUnit> aryLock;
				ACCOUNT->GetLockUnitsToAry( &aryLock );
				if( aryLock.size() > 0 ) {
					if( m_idxUnit >= (int)aryLock.size() )
						m_idxUnit = 0;
					unit = aryLock[ m_idxUnit ];
				} else {
					DestroyWndByIdentifier( "ctrl.unit" );
				}
			} else {
				// 유닛 개별
				unit = pProp->unitUnlock;
			}
			if( unit ) {
				auto pFrame = XGAME::_CreateUnitFace( this, "ctrl.unit", unit );
				if( pFrame ) {
					if( pFrame->GetPosLocal().IsZero() ) {
						if( pProp->unitUnlock == XGAME::xUNIT_ANY )
							if( pFrame->GetcompMngByAlpha().FindComponentByFunc("alpha") == nullptr )
								pFrame->GetcompMngByAlpha().AddComponent( new XECompWave( "alpha", this, 1.f, 0.5f) );
// 							if( pFrame->FindComponent("alpha") == nullptr )
// 								pFrame->AddAlphaComponent( new XECompWave( "alpha", this, 1.f, 0.5f) );
						const auto sizeFrame = pFrame->GetSizeLocal();
						const auto sizeThis = GetSizeLocal();
						auto v = vC;
						v.x -= ( sizeFrame.w * 0.5f );
						v.y += ( (sizeThis.h * 0.5f) - 10.f );
						pFrame->SetPosLocal( v );
						pFrame->SetScaleLocal( 0.8f );
						SetAutoSize();
						auto sizeThis2 = GetSizeLocal();
						int a = 0;
					}
				}
			}
		}
		auto pTextLv = xGET_TEXT_CTRL( this, "text.level" );
		if( pTextLv ) {
			const _tstring str = XFORMAT("Lv%d", pProp->lvArea);
			pTextLv->SetText( str );
		}
	} // pProp;
	XWnd::Update();
}

void XWndAreaBanner::DelegateEndState( XECompWave *pComp, int state )
{
	if( state == 3 ) {
		++m_idxUnit;
		SetbUpdate( true );
	}
}

int XWndAreaBanner::Process( float dt )
{
	if( m_State == 0 ) {
		// 가만있는 상태
		if( m_timerLife.IsOn() ) {
			// 타이머가 작동하고 타이머가 다됐으면 사라지는 상태로 전환
			if( m_timerLife.IsOver() ) {
				m_State = 1;
				m_timerLife.Set( 1.f );
			}
		}
	} else
	if( m_State == 1 ) {
		// 사라지고 있는 중.
		float lerpTime = m_timerLife.GetSlerp();
		if( lerpTime > 1.f )
			lerpTime = 1.f;
		float lerp = XE::xiHigherPowerAccel( lerpTime, 1.0f, 0 );
		SetAlphaLocal( 1.f - lerp );
		if( lerpTime >= 1.f ) {
			SetbDestroy( true );
			m_State = 2;
		}
	}

	return XWnd::Process( dt );
}


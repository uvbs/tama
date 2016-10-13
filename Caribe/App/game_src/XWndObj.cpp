#include "stdafx.h"
#include "Sprite/SprObj.h"
#include "_Wnd2/XWndEdit.h"
#include "_Wnd2/XWndProgressBar.h"
#include "_Wnd2/XWndText.h"
#include "XGame.h"
#include "XAccount.h"
#include "XSceneWorld.h"
#include "XSoundMng.h"
#include "XFramework/XSoundTable.h"
#include "XPropBgObj.h"
#include "XSystem.h"
#include "XWndObj.h"
#include "OpenGL2/XBatchRenderer.h"
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

////////////////////////////////////////////////////////////////
XWndObjBird::XWndObjBird( const XE::VEC2& vPos, float dAng, float speed )
	: XWnd( vPos )
{
	Init();
	m_pSprObj = new XSprObj( _T( "obj_bird.spr" ), XE::xHSL(), true, true, true, nullptr );
	m_pSprObj->SetAction( 1 );
//	auto vSize = m_pSprObj->GetSize();
	const XE::VEC2 vSize(2, 2);
	SetSizeLocal( vSize );
	m_vDelta = XE::GetAngleVector( dAng, speed );
	m_pSprObj->SetRotate( dAng );
}

void XWndObjBird::Destroy()
{
	SAFE_DELETE( m_pSprObj );
}
int XWndObjBird::Process( float dt )
{
	if( m_pSprObj ) {
		auto vPos = GetPosLocal();
		vPos += m_vDelta * dt;
		SetPosLocal( vPos );
		m_pSprObj->FrameMove( dt );
		auto vFinal = GetPosFinal();
		if( vFinal.y < -100.f )
			SetbDestroy( TRUE );

	}
	return 1;
}
void XWndObjBird::Draw()
{
	auto v = GetPosFinal();
	m_pSprObj->SetColor( XCOLOR_WHITE );
	m_pSprObj->SetScale( 1.f );
	m_pSprObj->SetfAlpha( 1.f );
	m_pSprObj->Draw( v );
	m_pSprObj->SetColor( XCOLOR_RGBA(0,0,0,128) );
	m_pSprObj->SetScale( 0.5f );
	m_pSprObj->SetfAlpha( 0.5f );
	m_pSprObj->Draw( v + XE::VEC2(0,75) );
}


////////////////////////////////////////////////////////////////
/**
 @brief 월드씬에서 좌측 프로필 사진을 누르면 플레이어의 자세한 정보창이 뜬다.
*/
XWndPlayerInfo::XWndPlayerInfo()
	: XWndPopup( _T("player_info.xml"), "popup_player" )
{
	Init();
	SetbModal( TRUE );
	auto pWndEdit = SafeCast<XWndEdit*>( Find("edit.hello.msg") );
	if( pWndEdit ) {
		pWndEdit->SetszString( ACCOUNT->GetstrHello() );
		pWndEdit->SetpDelegate( GAME );
	}
}

void XWndPlayerInfo::Update()
{
	// 플레이어 프로필사진 업데이트
	XGAME::UpdateProfileImage( this, "wnd.profile", true, GAME->GetpsfcProfile() );
	// 플레이시간 업데이트
	int h, m, s;
	XSYSTEM::GetHourMinSec( ACCOUNT->GetsecPlay(), &h, &m, &s );
	xSET_TEXT( this, "text.play.time", XFORMAT( "%s: %0d:%02d:%02d", XTEXT(2271), h, m, s ) );
	xSET_TEXT( this, "text.name", XFORMAT( "%s", ACCOUNT->GetstrName() ) );
	xSET_TEXT( this, "text.level", XFORMAT( "%d", ACCOUNT->GetLevel() ) );
	xSET_TEXT( this, "text.trophy", XFORMAT( "%d", ACCOUNT->GetLadder() ) ); 
	int powerTotal = ACCOUNT->GetPowerExcludeEmpty();
	xSET_TEXT( this, "text.power", XFORMAT( "%s", XE::NumberToMoneyString( powerTotal ) ) ); 
	// 경험치 바
	auto pBar = XLayout::sGetCtrl<XWndProgressBar*>( this, "pbar.exp" );
// 	if( pBar == nullptr ) {
// 		pBar = new XWndProgressBar( 80.f, 14.f, 64.f, 10.f );
// 		pBar->SetstrIdentifier( "pbar.exp" );
// 		Add( pBar );
// 	}
	pBar->SetLerp( (float)ACCOUNT->GetExp() / ACCOUNT->GetMaxExpCurrLevel() );
	XWndPopup::Update();
}

////////////////////////////////////////////////////////////////

XWndBgObj::XWndBgObj( const xnBgObj::xProp& prop )
	: XWndSprObj( prop.m_vwPos )
// 	: XWndSprObj( prop.m_strSpr, prop.m_idAct, prop.m_vwPos, 
// 								xRPT_LOOP, true, true, true )
	, m_Prop(prop)
{
	Init();
	XWndSprObj::CreateSprObj( prop.m_strSpr.c_str(), 
														prop.m_idAct, 
														true, 
														true, 
														true,
 														xRPT_LOOP);
}

void XWndBgObj::Destroy()
{
	const std::string strFile = SZ2C( m_Prop.m_strSnd );
	const auto idSound = SOUND_TBL->Find( strFile );
	SOUNDMNG->CloseSound( idSound );
}
void XWndBgObj::ProcessMsg( const std::string& strMsg )
{
	if( strMsg == "update_sound" ) {
		if( m_idStream ) {
			const auto vwFocus = SCENE_WORLD->GetvwCamera();
			const float distsq = (m_Prop.m_vwPos - vwFocus).Lengthsq();
			float vol = distsq / (320.f * 320.f);
			if( vol < 0 )
				vol = 0.f;
			if( vol > 1.f )
				vol = 1.f;
			vol = 1.f - vol;
			SOUNDMNG->SetSoundVolume( m_idStream, vol );
//			CONSOLE("vol:%.1f", vol);
		}
	}
}

void XWndBgObj::Update()
{
	if( !m_Prop.m_strSnd.empty() && m_idStream == 0 ) {
		const std::string strFile = SZ2C( m_Prop.m_strSnd );
		const auto idSound = SOUND_TBL->Find( strFile );
		m_idStream = SOUNDMNG->OpenPlaySound( idSound, TRUE );
		if( m_idStream == xINVALID_STREAM ) {
			auto pReplay = SOUNDMNG->GetpLastReplay();
			if( pReplay ) {
				SOUNDMNG->SetCallback( pReplay, [this]( ID idStream ){
					m_idStream = idStream;
				});
			}
		}
		XTRACE( "%s(id=%d):idStream=%d", __TFUNC__, m_Prop.m_idObj, m_idStream );
	}
	XWndSprObj::Update();
}

int XWndBgObj::Process( float dt )
{
// 	if( !m_Prop.m_strSnd.empty() && m_idStream == 0 ) {
// 		// 사운드가 지정되어있는데 스트림이 없다면 다시 플레이 시킴
// 		std::string strFile = SZ2C( m_Prop.m_strSnd );
// 		const std::string strcExt = XE::GetFileExt( strFile );
// 		if( strcExt.empty() )
// #ifdef WIN32
// 			strFile += ".wav";
// #else
// 			strFile += ".ogg";
// #endif // WIN32
// 		const auto idSound = SOUND_TBL->Find( strFile );
// 		m_idStream = SOUNDMNG->OpenPlaySound( idSound, TRUE );
// 		CONSOLE( "%s(id=%d):idStream=%d", __TFUNC__, m_Prop.m_idObj, m_idStream );
// 	}
	return XWndSprObj::Process( dt );
}

//////////////////////////////////////////////////////////////////////////
/**
 @brief 월드 배경에 배치되는 오브젝트들의 루트레이어 객체
*/
XWndBgObjLayer::XWndBgObjLayer( const XE::VEC2& sizeWorld )
	: XWndBatchRender( "bgObj", false, true, false, false, XE::VEC2(0,0), sizeWorld )
{
	Init();
}

/**
 @brief 월드맵 스크롤에 따라 배경 객체를 생성.
 .화면과 화면가장자리 일정범위에 들어온 객체만 생성한다.
 .구름에 가려진 객체는 생성하지 않는다.
 .스크롤할때 드래그가 멈추면 생성한다.
 .화면을 벗어난지 일정시간이 지나면 삭제시킨다.
 .사운드객체는 화면의 중심과의 거리를 비교해서 사운드 볼륨을 조절한다.

*/
void XWndBgObjLayer::Update()
{
	XWnd::Update();
}

/**
 @brief 월드맵좌표 vFocus위치를 중심으로 한 bgobj들을 갱신한다.
*/
void XWndBgObjLayer::UpdateCurrFocus( const XE::VEC2& vFocus, const XE::VEC2& sizeView )
{
	int cntCreated = 0;
	auto& listObjs = XPropBgObj::sGet()->GetlistBgObjs();
	for( auto pProp : listObjs ) {
		const auto vwLT = vFocus - (sizeView * 0.5f);
		auto bIsIn = pProp->IsInArea( vwLT, sizeView );
#ifdef _xIN_TOOL
		if( XBaseTool::sIsToolBgObjMode() )		// 툴모드에선 무조건 in상태
			bIsIn = true;
#endif // _xIN_TOOL
// 		if( pProp->m_Type != XGAME::xBOT_SOUND ) {
			if( UpdateSprObj( pProp, bIsIn ) )
				++cntCreated;
// 		} else {
// 			// sound obj
// 			UpdateSoundObj( pProp, bIsIn );
// 		}
	}
#ifdef _DEBUG
//	CONSOLE("created bgObj: %d", cntCreated );
#endif // _DEBUG
}

bool XWndBgObjLayer::UpdateSprObj( const xnBgObj::xProp* pProp, bool bIsIn )
{
	bool bCreated = false;
	auto pWndBgObj = SafeCast2<XWndBgObj*>( Findf( "__bg.%d", pProp->m_idObj ) );
	if( bIsIn ) {
		if( pWndBgObj == nullptr ) {
			pWndBgObj = new XWndBgObj( *pProp );
			pWndBgObj->GoRandomFrame();
			pWndBgObj->SetstrIdentifierf( "__bg.%d", pProp->m_idObj );
			Add( pWndBgObj );
			bCreated = true;
		} else {
			pWndBgObj->SetSprObj( pProp->m_strSpr.c_str(), 
														pProp->m_idAct, 
														true, 
														true, 
														true, 
														xRPT_LOOP );
		}
#ifdef _xIN_TOOL
		if( XBaseTool::sIsToolBgObjMode() )
			pWndBgObj->SetEvent( XWM_CLICKED, this, &XWndBgObjLayer::OnClickBgObj );
#endif // _xIN_TOOL
		pWndBgObj->OffTimer();		// 화면내에 잇는것은 타이머를 작동시키지 않는다.
	} else {
		// bgObj is out area
		if( pWndBgObj ) {
			// 10초동안 화면을 벗어나있으면 삭제됨
			pWndBgObj->SetTimer( 10.f );	// 타이머 작동시작. 이미 켜져있으면 작동시키지 않음.
			if( pWndBgObj->IsOverTime() ) {
// 				CONSOLE("destroy bgObj:id=%d", pProp->m_idObj );
				pWndBgObj->SetbDestroy( true );
			}
		}
	}
	return bCreated;
}

bool XWndBgObjLayer::UpdateSoundObj( const xnBgObj::xProp* pProp, bool bIsIn )
{
	return false;
}

/*
기본적으론 한번 생성되면 자동으로 삭제되지 않는다.
stop scroll이벤트가 일어날때마다 한번씩 업데이트
업데이트때마다
화면밖을 벗어나있는 오브젝트는 "벗어남" 타이머가 작동을 시작하고 이중 일정시간이 지난객체는 삭제
*/


#ifdef _xIN_TOOL
XWndBgObj* XWndBgObjLayer::CreateWndBgObj( xnBgObj::xProp *pProp )
{
	if( XBREAK(pProp == nullptr) )
		return nullptr;
	auto pWndExist = Findf( "__bg.%d", pProp->m_idObj );
	if( XBREAK( pWndExist != nullptr) )
		return nullptr;
	XBREAK( pProp->m_strSpr.empty() );
	auto pWndBgObj = new XWndBgObj( *pProp );
// 	auto pWndBgObj = new XWndSprObj( pProp->m_strSpr
// 																, pProp->m_idAct
// 																, pProp->m_vwPos );
	pWndBgObj->SetstrIdentifierf("__bg.%d", pProp->m_idObj );
#ifdef _xIN_TOOL
	pWndBgObj->SetEvent( XWM_CLICKED, this, &XWndBgObjLayer::OnClickBgObj );
#endif // _xIN_TOOL
	Add( pWndBgObj );
	return pWndBgObj;
}

void XWndBgObjLayer::Draw()
{
//	XWnd::Draw();
	//
	if( XBaseTool::sIsToolBgObjMode() ) {
#ifdef _CHEAT
		if( XAPP->m_bViewBgObjBoundBox ) {
			//
			for( auto pWnd : m_listItems ) {
				auto bb = pWnd->GetBoundBoxByVisibleFinal();
				GRAPHICS->DrawRect( bb.vLT, bb.vRB, XCOLOR_RGBA( 0, 0, 0, 128 ) );
				auto pWndBgObj = SafeCast<XWndBgObj*>( pWnd );
				if( pWndBgObj && !pWndBgObj->GetProp().m_strSnd.empty() ) {
					PUT_STRING_SHADOW( bb.vLT.x, bb.vLT.y, XCOLOR_YELLOW, _T( "S" ) );
				}
			}
		}
		if( m_pSelected ) {
			auto bb = m_pSelected->GetBoundBoxByVisibleFinal();
			GRAPHICS->DrawRect( bb.vLT, bb.vRB, XCOLOR_RGBA( 255, 0, 0, 200 ) );
		}
#endif // _CHEAT
	}
}

/**
 @brief 
*/
int XWndBgObjLayer::OnClickBgObj( XWnd* pWnd, DWORD p1, DWORD p2 )
{
	//
	if( XBaseTool::sIsToolBgObjMode() ) {
		m_pSelected = SafeCast<XWndBgObj*>( pWnd );
		if( m_pSelected ) {
			const std::string str = pWnd->GetstrIdentifier();
			const std::string strNum = str.substr( 5 );
			ID idObj = (ID)atoi( strNum.c_str() );
			if( idObj ) {
				m_pPropSelected = XPropBgObj::sGet()->GetpProp( idObj );
#ifdef _xIN_TOOL
				CONSOLE( "OnClickBgObj:id=%d, snd=%s", m_pPropSelected->m_idObj, m_pPropSelected->m_strSnd.c_str() );
#endif // _xIN_TOOL
				XBaseTool::s_pCurr->ResetAutoSave();		//터치하고 그러면 자동세이브가 미뤄지도록.
			}
		}
	}
	return 1;
}

void XWndBgObjLayer::OnRButtonDown( float lx, float ly )
{
	CONSOLE("XWndBgObjLayer::OnRButtonDown");
	m_bRButtDown = true;
	XAPP->SetbDebugDrawArea( FALSE );		// 사각형 못그리게 함.
	m_bMoved = false;
	m_bDragging = false;
	m_vTouchPrev = SCENE_WORLD->GetvMouseWorld();
	if( m_pPropSelected )
		m_PropPrev = *m_pPropSelected;
}
void XWndBgObjLayer::OnRButtonUp( float lx, float ly )
{
	CONSOLE( "XWndBgObjLayer::OnRButtonUp" );
	if( m_bRButtDown && m_bMoved && m_pPropSelected ) {
		if( (int)m_pPropSelected->m_vwPos.x != (int)m_PropPrev.m_vwPos.x 
			&& (int)m_pPropSelected->m_vwPos.y != (int)m_PropPrev.m_vwPos.y )
		XBaseTool::s_pCurr->UpdateAutoSave();
	}
	m_bRButtDown = false;
	XAPP->SetbDebugDrawArea( TRUE );		// 복구
	m_bCopyed = false;
	m_bDragging = false;
	m_PropPrev = xnBgObj::xProp();
}

void XWndBgObjLayer::OnMouseMove( float lx, float ly )
{
	if( XBaseTool::sIsToolBgObjMode() ) {
		if( m_bRButtDown && m_pSelected && m_pPropSelected ) {
			const auto vwCurr = SCENE_WORLD->GetvMouseWorld();
			const auto vDist = vwCurr - m_vTouchPrev;
			if( XE::GetMain()->m_bCtrl && m_bCopyed == false ) {
				if( vDist.Lengthsq() > 5.f * 5.f ) {
					// 선택된 객체의 카피본을 만든다.
					auto pNewProp = XPropBgObj::sGet()->CreateNewProp();
					*pNewProp = *m_pPropSelected;		// 복사
					pNewProp->m_idObj = 0;		// 아이디를 새로 부여받기위해 클리어.
					XPropBgObj::sGet()->AddBgObj( pNewProp );
					m_pPropSelected = pNewProp;
					m_pSelected = CreateWndBgObj( pNewProp );
					m_bCopyed = true;
				}
			}
			m_vTouchPrev = vwCurr;
			m_pPropSelected->m_vwPos += vDist;
			m_pSelected->SetPosLocal( m_pPropSelected->m_vwPos );
			m_bMoved = true;
			m_bDragging = true;
			XBaseTool::s_pCurr->ResetAutoSave();		// 드래그중엔 자동세이브 안되게
		}
	}
}

BOOL  XWndBgObjLayer::OnKeyDown( int keyCode )
{
	if( XBaseTool::sIsToolBgObjMode() ) {
		if( keyCode == XE::KEY_DEL ) {
			if( m_pPropSelected ) {
				if( m_pSelected )
					m_pSelected->SetbDestroy( true );
				XPropBgObj::sGet()->DestroyBgObj( m_pPropSelected->m_idObj );
				m_pPropSelected = nullptr;
				m_pSelected = nullptr;
				XBaseTool::s_pCurr->ResetAutoSave();		//자동세이브가 미뤄지도록.
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif // _xIN_TOOL

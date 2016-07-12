#include "stdafx.h"
#include "XSceneBase.h"
#include "XGame.h"
#include "XGameWnd.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace xHelp;

XSceneBase* XGAME::xGetpScene()
{
	return GAME->GetpScene();
}

//////////////////////////////////////////////////////////////////////////
XSceneBase::XSceneBase( XGame *pGame, XGAME::xtScene typeScene )
	: XEBaseScene( pGame, typeScene ) 
{
	Init();
}

/**
 @brief 씬에 진입한 직후
 @param dwParam2 xSC_INGAME에서는 dwParam2는 스팟 아이디가 된다.
*/
int XSceneBase::OnEnterScene( XWnd*, DWORD, DWORD dwParam2 )
{
	/*
	현재 문제점이 각 씬마다 dwParam2의 의미가 다르다는것이다.
	*/
//	DWORD dwParam2 = 0;
	if( ACCOUNT ) {
		if( GetidScene() == XGAME::xSC_LEGION ) {
			// 군단배치씬에 들어왔을때 여분영웅이 있으면 extra_hero파라메터를 넘긴다. 일단 하드코딩으로 함.
			if( ACCOUNT->GetNumExtraHeroes() > 0 ) {
				XBREAK( dwParam2 != 0 );
				dwParam2 = XGAME::xEP_EXTRA_HERO;
			}
		} else
		// 연구소씬에 들어왔을때 기사가 잠금 되어있으면 추가 파라메터를 넘긴다. 일단 하드코딩
		if( GetidScene() == XGAME::xSC_TECH ) {
			// 기사가 잠겨있고 성당이 오픈된 상태면
			if( ACCOUNT->IsLockUnit(XGAME::xUNIT_PALADIN) && ACCOUNT->IsUnlockMenu( XGAME::xBM_CATHEDRAL ) ) {
				XBREAK( dwParam2 != 0 );
				dwParam2 = XGAME::xEP_LOCK_PALADIN;
			}
		}
		GAME->DispatchEvent( XGAME::xAE_ENTER_SCENE, GetidScene(), dwParam2 );
	}
	return 1;
}

XGameWndAlert* XSceneBase::WndAlertOkCancel( const char *csid, LPCTSTR format, ... ) 
{
	XGameWndAlert *pAlert = nullptr;
	if( XE::GetGame()->Find( csid ) == nullptr ) {
		TCHAR szBuff[ 1024 ];
		va_list vl;
		va_start( vl, format );
		_vstprintf_s( szBuff, format, vl );
		pAlert = new XGameWndAlert( szBuff, nullptr, XWnd::xOKCANCEL );
		va_end( vl );
		pAlert->SetstrIdentifier( csid );
		Add( pAlert );
	}
	return pAlert;
}
XGameWndAlert* XSceneBase::WndAlert( LPCTSTR format, ... )
{
	XGameWndAlert *pAlert = nullptr;
	auto _pWnd = Find( "_alert.try.connect" );
	if( _pWnd == nullptr ) {
		TCHAR _szBuff[ 1024 ];
		va_list vl;
		va_start( vl, format );
		_vstprintf_s( _szBuff, format, vl );
		if( !GAME->IsPlayingSeq() ) {
			pAlert = new XGameWndAlert( _szBuff );
			pAlert->SetstrIdentifier( "_alert.try.connect" );
			Add( pAlert );
		}
	}
	return pAlert;
}

// TYPE: XWnd::xOK 등등.
XGameWndAlert* XSceneBase::WndAlertParam( LPCTSTR szTitle, 
											XWnd::xtAlert type, 
											XCOLOR col, LPCTSTR format, ... )
{
	XGameWndAlert *pAlert = nullptr;
	auto _pWnd = Find( "_alert.try.connect" );
	if( _pWnd == nullptr ) {
		TCHAR _szBuff[ 1024 ];
		va_list vl;
		va_start( vl, format );
		_vstprintf_s( _szBuff, format, vl );
		pAlert = new XGameWndAlert( _szBuff, szTitle, type, col );
		pAlert->SetstrIdentifier( "_alert.try.connect" );
		Add( pAlert );
	}
	return pAlert;
}
XGameWndAlert* XSceneBase::WndAlertYesNo( const char *ids, LPCTSTR format, ... )
{
	XGameWndAlert *pAlert = nullptr;
	auto _pWnd = Find( ids );
	if( _pWnd == nullptr ) {
		TCHAR _szBuff[ 1024 ];
		va_list vl;
		va_start( vl, format );
		_vstprintf_s( _szBuff, format, vl );
		pAlert = new XGameWndAlert( _szBuff, nullptr, XWnd::xYESNO );
		pAlert->SetstrIdentifier( ids );
		Add( pAlert );
	}
	return pAlert;
}

#include "StdAfx.h"
#include "XFramework/XEBaseScene.h"
#include "XFramework/client/XEContent.h"
#include "XFramework/XESceneMng.h"
#include "XSoundMng.h"
#include "OpenGL2/XRenderCmd.h"
#include "OpenGL2/XTextureAtlas.h"


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#define SEC_FADEOUT		0.01f

XEBaseScene::XEBaseScene( XEContent *pGame, ID idScene, BOOL bTransition ) 
	: XWnd( 0.f, 0.f, XE::GetGameWidth(), XE::GetGameHeight() )
	, m_pRenderer( new XRenderCmdMng(__FUNCTION__) )
	, m_pAtlas( new XTextureAtlas( __FUNCTION__ ) )
{ 
	Init(); 
	m_idScene = idScene;
	m_pGame = pGame;
	if( bTransition )
		SetpTransition( new XFadeInOut( TRUE, TRUE, SEC_FADEOUT ) );
	m_pLua = pGame->GetpLua();
	SetbTouchable( FALSE );		// 씬자체는 터치이벤트를 발생시키지 않는다.
}

/**
 @brief 앞으로 가급적 이걸 쓸것.
*/
XEBaseScene::XEBaseScene( XEContent *pGame, const std::string& idsScene, bool bTransition )
	: XWnd( 0.f, 0.f, XE::GetGameWidth(), XE::GetGameHeight() )
	, m_pRenderer( new XRenderCmdMng( __FUNCTION__ ) )
	, m_pAtlas( new XTextureAtlas( __FUNCTION__ ) )
{
	Init();
	m_pGame = pGame;
	XBREAK( idsScene.empty() );
	SetstrIdentifier( idsScene );
	if( bTransition )
		SetpTransition( new XFadeInOut( TRUE, TRUE, SEC_FADEOUT ) );
	m_pLua = pGame->GetpLua();
	SetbTouchable( FALSE );		// 씬자체는 터치이벤트를 발생시키지 않는다.
}

void XEBaseScene::Destroy()
{
	SAFE_DELETE( m_pTransition );
	DestroyWnd( m_pSceneChild );		// 여기서 이걸 먼저 해줘야 XT3::Destroy()때 SPRMNG해제할때 남는게 생기지 않는다.
	if( !IsbBridge() )
		SOUNDMNG->StopBGMusic();
}


/**
 @brief 
*/
int XEBaseScene::Process( float dt ) 
{ 
// 	auto pPrev = XTextureAtlas::sSetpCurrMng( m_pAtlas );
	SET_ATLASES( m_pAtlas ) {
		if( m_pTransition && m_pTransition->GetbDestroy() )
			SAFE_DELETE( m_pTransition );
		//
		XWnd::Process( dt );
		// 다음씬으로 넘어갈 준비를 함.
		if( m_pSceneChild ) {
			// 자식 씬이 파괴되었으면 트랜지션 객체를 생성하고 파괴될 준비를 함.
			if( GetDestroy() == 0 && m_pSceneChild->GetDestroy() ) {
				SetpTransition( new XFadeInOut( TRUE, FALSE, SEC_FADEOUT ) );
				SetDestroy( 2 );
			}
		}
		if( m_pTransition ) {
			// 트랜지션 중.
			if( m_pTransition->Process( dt ) == 0 ) {
				// 트랜지션 끝남.
				OnEndTransition( m_pTransition );
				m_pTransition->SetbDestroy( TRUE );
				if( m_pTransition->IsTransIn() == FALSE ) {
					// fadeOut(어두워짐)이 끝남.
					OnEndTransitionOut( m_pTransition );
					OnDestroySceneBefore();
					SetDestroy( 1 );
				} else {
					OnEndTransitionIn( m_pTransition );
				}
			}
			// 브릿지씬만 아니면 페이드아웃될때 음악도 서서히 줄어든다.
			if( !IsbBridge() ) {
				float lerp = m_pTransition->GetfSlerp();
				if( m_pTransition->IsTransIn() )
					SOUNDMNG->SetBGMVolumeLocal( lerp );
				else
					SOUNDMNG->SetBGMVolumeLocal( 1.f - lerp );
			}
			//			SOUNDMNG->FadeOutBGM( 0.1f );	// 시간
		} else {
			if( m_idNextScene )	// 넥스트신이 지정됐는데 트랜지션이 없으면 그냥 나감.
				SetDestroy( 1 );
		}
	} END_ATLASES;
// 	XTextureAtlas::sSetpCurrMng( pPrev );
	return 1;
}


/**
 @brief 
*/
void XEBaseScene::DoExit( ID idSceneNext, SceneParamPtr spParam/*=NULL*/ )
{
	// 이미 그 씬상태이면 아무것도 안함.
	//	if( m_idScene == idSceneNext )		// 유황스팟 유저인카운터 씬때문에 이기능 삭제함.
	//		return;
	m_pSceneMng->SetspParam( spParam );
	// 이미 Exit한상태면 다시 호출하지 않음.
	if( XBREAK( GetNextScene() == idSceneNext ) )
		return;
	CallScript( "OnEventLeaveScene" );
	if( m_pParentScene )	{
		m_pParentScene->DoExit( idSceneNext );
		return;
	}
	XWnd::sSetDragWnd( nullptr );

	// 	// 다음씬의 정보를 얻음.
	// 	const auto& infoNext = GetpSceneMng()->GetSceneInfo( idSceneNext );
	// 	if( infoNext.m_bDeferDestroyPrevScene ) {
	// 		// 다음씬이 로딩씬이 필요하면 일단 파괴 보류.
	// 		if( m_pDelegate ) {
	// 			m_pDelegate->DelegateCreateBridge( infoNext );
	// 		}
	// 	} else {
	// 다음으로 넘어갈씬을 예약
	SetNextScene( idSceneNext );
	// 트랜지션 발생시켜서 씬 종료하게 함. 
	SAFE_DELETE( m_pTransition );
	SetpTransition( new XFadeInOut( TRUE, FALSE, SEC_FADEOUT ) );
	//	}
}

//
void XEBaseScene::Draw( void ) 
{
	SET_RENDERER( m_pRenderer ) {
		XWnd::Draw();
		if( m_pTransition )
			m_pTransition->Draw();
	} END_RENDERER;
}
void XEBaseScene::DrawTransition( void )
{
//	if( m_pTransition )
//		m_pTransition->Draw();
}

// 페이드인 끝나고 호출
int XEBaseScene::OnEnterScene( XWnd*, DWORD, DWORD ) 
{
	if( m_pLua ) {
		if( m_pLua->IsHaveFunc( "OnEventEnterScene" ) ) {
			CONSOLE( "OnEventEnterScene" );
			CallScript( "OnEventEnterScene" );
		}
	}
	return 1;
}

void XEBaseScene::CallScript( const char *cFunc, DWORD p1, DWORD p2 )	
{
	XLua *pLua = GetpLua();
	if( pLua == NULL )		return;
	if( pLua->IsHaveFunc( cFunc ) )
		CallScriptImplement( pLua, cFunc, p1, p2 );
}

void XEBaseScene::CallScriptImplement( XLua *pLua, const char *cFunc, DWORD p1, DWORD p2 )
{
	pLua->Call<void, XEBaseScene*>( cFunc, this, p1, p2 );
}

BOOL XEBaseScene::OnKeyUp( int keyCode )
{
	if( XWnd::OnKeyUp( keyCode ) )
		return TRUE;
	if( keyCode == XE::KEY_BACK )	{
		// 백버튼 누르면 현재 씬에도 돌아가기 버튼이 있는지 보고 있으면 OnBack을 불러준다. 
		XWnd *pButt = Find( "butt.back" );
		if( pButt )
		{
			OnBack( pButt, 0, 0 );
			return TRUE;
		}
	}
	return FALSE;
}
/**
 @brief this가 브릿지속성일경우 씬이 종료되어야 하는 시점이 되면 외부에서 이벤트가 온다.
*/
void XEBaseScene::OnExitBridge() 
{
	// 브릿지씬이 종료된후 불려지게될 씬번호 등록
	XBREAK( m_idBridgeNextScene == 0 );
	SetNextScene( m_idBridgeNextScene );
	// 트랜지션 발생시켜서 씬 종료하게 함. 
	SAFE_DELETE( m_pTransition );
	SetpTransition( new XFadeInOut( TRUE, FALSE, SEC_FADEOUT ) );
}

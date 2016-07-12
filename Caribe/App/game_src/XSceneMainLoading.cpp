#include "stdafx.h"
#if 0
#include "XSceneMainLoading.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////
XSceneMainLoading* XSceneMainLoading::s_pSingleton = nullptr;
XSceneMainLoading* XSceneMainLoading::sGet(){	return s_pSingleton;}
bool XSceneMainLoading::s_bLoaded = false;
//////////////////////////////////////////////////////////////////////////
void XSceneMainLoading::Destroy() 
{	
	XBREAK( s_pSingleton == nullptr );	// 이미 사라져있다면 잘못된것임
	XBREAK( s_pSingleton != this );		// 싱글톤이 this가 아니면 잘못된것임.
	s_pSingleton = nullptr;
}

XSceneMainLoading::XSceneMainLoading( XGame *pGame ) 
	: XSceneBase( pGame, XGAME::xSC_MAIN_LOADING )
	, m_Layout( _T( "scene_main_loading.xml" ) )
{ 
	XBREAK( s_pSingleton != nullptr );	// 이미 생성되어있다면 잘못된것임.
	s_pSingleton = this;
	Init(); 
	m_Layout.CreateLayout("main", this);
	//
	// TODO: 이곳에 코딩하시오
	//
	SetbUpdate( true );
}

void XSceneMainLoading::Create( void )
{
	XSceneBase::Create();
}

/**
 @brief UI의 생성과 업데이트는 이곳에 넣습니다.
*/
void XSceneMainLoading::Update()
{
	XSceneBase::Update();
}

/**
 @brief 매 프레임 실행됩니다.
*/
int XSceneMainLoading::Process( float dt ) 
{
	if( m_cntProcess == 2 ) {
		// 프로퍼티등의 로딩을 함.
		GAME->CreateGameResource();
		s_bLoaded = true;
		DoExit( XGAME::xSC_TITLE );
	}
	++m_cntProcess;
	return XSceneBase::Process( dt );
}

//
void XSceneMainLoading::Draw() 
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneMainLoading::OnLButtonDown( float lx, float ly ) 
{
	XSceneBase::OnLButtonDown( lx, ly );
}
void XSceneMainLoading::OnLButtonUp( float lx, float ly ) {
	XSceneBase::OnLButtonUp( lx, ly );
}
void XSceneMainLoading::OnMouseMove( float lx, float ly ) {
	XSceneBase::OnMouseMove( lx, ly );
}

#endif // 0
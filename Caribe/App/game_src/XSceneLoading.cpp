#include "stdafx.h"
#include "XSceneLoading.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////
XSceneLoading* XSceneLoading::s_pSingleton = nullptr;
XSceneLoading* XSceneLoading::sGet()
{
	return s_pSingleton;
}
//////////////////////////////////////////////////////////////////////////
void XSceneLoading::Destroy() 
{	
	XBREAK( s_pSingleton == nullptr );	// 이미 사라져있다면 잘못된것임
	XBREAK( s_pSingleton != this );		// 싱글톤이 this가 아니면 잘못된것임.
	s_pSingleton = nullptr;
}

XSceneLoading::XSceneLoading( XGame *pGame, XSPSceneParam& spParam ) 
	: XSceneBase( pGame, XGAME::xSC_LOADING )
	, m_Layout(_T("scene_loading.xml"))
{ 
	XBREAK( s_pSingleton != nullptr );	// 이미 생성되어있다면 잘못된것임.
	s_pSingleton = this;
	m_spParamForNext = spParam;
	Init(); 
	m_Layout.CreateLayout("main", this);
	//
	// TODO: 이곳에 코딩하시오
	//
	SetbUpdate( true );
}

void XSceneLoading::Create( void )
{
	XSceneBase::Create();
}

/**
 @brief UI의 생성과 업데이트는 이곳에 넣습니다.
*/
void XSceneLoading::Update()
{
	XSceneBase::Update();
}

/**
 @brief 매 프레임 실행됩니다.
*/
int XSceneLoading::Process( float dt ) 
{ 
	if( m_cntProcess == 2 ) {
		// 다음씬 로딩시작(동기식 로딩)
		XBREAK( GetpSceneMng() == nullptr );
		// 일단 생성해서 보관
		m_pSceneNextPreCreated = GetpSceneMng()->CreatePreNextScene( GetidBridgeNextScene(), m_spParamForNext );
		XBREAK( m_pSceneNextPreCreated == nullptr );
		// 로딩씬 종료시작 이벤트.
		OnExitBridge();
	}
	++m_cntProcess;
	return XSceneBase::Process( dt );
}

//
void XSceneLoading::Draw() 
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneLoading::OnLButtonDown( float lx, float ly ) 
{
	XSceneBase::OnLButtonDown( lx, ly );
}
void XSceneLoading::OnLButtonUp( float lx, float ly ) {
	XSceneBase::OnLButtonUp( lx, ly );
}
void XSceneLoading::OnMouseMove( float lx, float ly ) {
	XSceneBase::OnMouseMove( lx, ly );
}

/**
 @brief 씬이 파괴되기전에 호출됨.
*/
void XSceneLoading::OnDestroySceneBefore()
{
	XBREAK( GetpSceneMng() == nullptr );
	GetpSceneMng()->OnDestroyBridge( m_pSceneNextPreCreated );
}



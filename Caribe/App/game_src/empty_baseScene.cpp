#include "stdafx.h"
#include "XSceneSample.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////
XSceneSample* XSceneSample::s_pSingleton = nullptr;
XSceneSample* XSceneSample::sGet(){	return s_pSingleton;}
//////////////////////////////////////////////////////////////////////////
void XSceneSample::Destroy() 
{	
	XBREAK( s_pSingleton == nullptr );	// 이미 사라져있다면 잘못된것임
	XBREAK( s_pSingleton != this );		// 싱글톤이 this가 아니면 잘못된것임.
	s_pSingleton = nullptr;
}

XSceneSample::XSceneSample( XGame *pGame, SceneParamPtr& spParam ) 
	: XSceneBase( pGame, XGAME::xSC_SAMPLE )
	, m_Layout(_T("scene_Sample.xml"))
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

void XSceneSample::Create( void )
{
	XSceneBase::Create();
}

/**
 @brief UI의 생성과 업데이트는 이곳에 넣습니다.
*/
void XSceneSample::Update()
{
	XSceneBase::Update();
}

/**
 @brief 매 프레임 실행됩니다.
*/
int XSceneSample::Process( float dt ) 
{ 
	return XSceneBase::Process( dt );
}

//
void XSceneSample::Draw() 
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneSample::OnLButtonDown( float lx, float ly ) 
{
	XSceneBase::OnLButtonDown( lx, ly );
}
void XSceneSample::OnLButtonUp( float lx, float ly ) {
	XSceneBase::OnLButtonUp( lx, ly );
}
void XSceneSample::OnMouseMove( float lx, float ly ) {
	XSceneBase::OnMouseMove( lx, ly );
}


#include "stdafx.h"
#include "XSceneOpening.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////
XSceneOpening* XSceneOpening::s_pSingleton = nullptr;
XSceneOpening* XSceneOpening::sGet(){	return s_pSingleton;}
//////////////////////////////////////////////////////////////////////////
void XSceneOpening::Destroy() 
{	
	XBREAK( s_pSingleton == nullptr );	// 이미 사라져있다면 잘못된것임
	XBREAK( s_pSingleton != this );		// 싱글톤이 this가 아니면 잘못된것임.
	s_pSingleton = nullptr;
}

XSceneOpening::XSceneOpening( XGame *pGame, XSPSceneParam& spParam ) 
	: XSceneBase( pGame, XGAME::xSC_OPENNING )
	, m_Layout(_T("scene_opening.xml"))
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

void XSceneOpening::Create( void )
{
	XSceneBase::Create();
}

/**
 @brief UI의 생성과 업데이트는 이곳에 넣습니다.
*/
void XSceneOpening::Update()
{
	XSceneBase::Update();
}

/**
 @brief 매 프레임 실행됩니다.
*/
int XSceneOpening::Process( float dt ) 
{ 
	return XSceneBase::Process( dt );
}

//
void XSceneOpening::Draw() 
{
	XSceneBase::Draw();
	XSceneBase::DrawTransition();
}

void XSceneOpening::OnLButtonDown( float lx, float ly ) 
{
	XSceneBase::OnLButtonDown( lx, ly );
}
void XSceneOpening::OnLButtonUp( float lx, float ly ) {
	XSceneBase::OnLButtonUp( lx, ly );
}
void XSceneOpening::OnMouseMove( float lx, float ly ) {
	XSceneBase::OnMouseMove( lx, ly );
}


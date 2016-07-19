#include "StdAfx.h"
#include "XSceneTitle.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneTitle *SCENE_TITLE = NULL;

void XSceneTitle::Destroy() 
{	
	XBREAK( SCENE_TITLE == NULL );
	XBREAK( SCENE_TITLE != this );
	SCENE_TITLE = NULL;
}

XSceneTitle::XSceneTitle( XGame *pGame ) 
	: XEBaseScene( pGame, XGAME::xSC_/*TITLE*/ )
{ 
	XBREAK( SCENE_TITLE != NULL );
	SCENE_TITLE = this;
	Init(); 
//	SetPosLocal( 0.f, 78.f );
//	SetSizeLocal( XE::GetGameWidth(), 
//		GRAPHICS->GetLogicalScreenSize().h - (GetPosLocal().h + 66.f) );
//	m_Layout.CreateLayout( new XAppLayout(_T("layout_sample.xml")), 
//											"sample", this, "sample_group" );
	/*XWndButtonString *pButt =
		new XWndButtonString(0.f, 0.f, XTEXT(2033), XCOLOR_WHITE, FONT_NANUM_BOLD, 40.f, BUTT_BIG );
	pWnd->Add( pButt );
	pWnd->SetEvent( XWM_CLICKED, this, &XSceneSample::OnClick );*/
}

void XSceneTitle::Create( void )
{
	XEBaseScene::Create();
}

int XSceneTitle::Process( float dt ) 
{ 
	return XEBaseScene::Process( dt );
}

//
void XSceneTitle::Draw( void ) 
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneTitle::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneTitle::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneTitle::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}

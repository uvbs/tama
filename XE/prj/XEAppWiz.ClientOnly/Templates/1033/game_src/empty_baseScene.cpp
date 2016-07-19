#include "StdAfx.h"
#include "XSceneWorld.h"
#include "XGame.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneWorld *SCENE_WORLD = NULL;

void XSceneWorld::Destroy() 
{	
	XBREAK( SCENE_WORLD == NULL );
	XBREAK( SCENE_WORLD != this );
	SCENE_WORLD = NULL;
}

XSceneWorld::XSceneWorld( XGame *pGame ) 
	: XEBaseScene( pGame, XGAME::xSC_WORLD )
{ 
	XBREAK( SCENE_WORLD != NULL );
	SCENE_WORLD = this;
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

void XSceneWorld::Create( void )
{
	XEBaseScene::Create();
}

int XSceneWorld::Process( float dt ) 
{ 
	return XEBaseScene::Process( dt );
}

//
void XSceneWorld::Draw( void ) 
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneWorld::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneWorld::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneWorld::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}

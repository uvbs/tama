#include "StdAfx.h"
#include "XSceneTechSel.h"
#include "XGame.h"
#include "_Wnd2/XWndButton.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneTechSel *SCENE_TECH_SEL = NULL;

void XSceneTechSel::Destroy() 
{	
	XBREAK( SCENE_TECH_SEL == NULL );
	XBREAK( SCENE_TECH_SEL != this );
	SCENE_TECH_SEL = NULL;
}

XSceneTechSel::XSceneTechSel( XGame *pGame ) 
	: XSceneBase( pGame, XGAME::xSC_TECH_SEL )
	, m_Layout(_T("layout_sample.xml"))
{ 
	XBREAK( SCENE_TECH_SEL != NULL );
	SCENE_TECH_SEL = this;
	Init(); 
	m_Layout.CreateLayout("tech", this);
	xSET_BUTT_HANDLER( this, "butt.back", &XSceneTechSel::OnBack );
	/*XWndButtonString *pButt =
		new XWndButtonString(0.f, 0.f, XTEXT(2033), XCOLOR_WHITE, FONT_NANUM_BOLD, 40.f, BUTT_BIG );
	pWnd->Add( pButt );
	pWnd->SetEvent( XWM_CLICKED, this, &XSceneTechSel::OnClick );*/
}

void XSceneTechSel::Create( void )
{
	XEBaseScene::Create();
}

int XSceneTechSel::Process( float dt ) 
{ 
	return XEBaseScene::Process( dt );
}

//
void XSceneTechSel::Draw( void ) 
{
	XEBaseScene::Draw();
	XEBaseScene::DrawTransition();
}

void XSceneTechSel::OnLButtonDown( float lx, float ly ) 
{
	XEBaseScene::OnLButtonDown( lx, ly );
}
void XSceneTechSel::OnLButtonUp( float lx, float ly ) {
	XEBaseScene::OnLButtonUp( lx, ly );
}
void XSceneTechSel::OnMouseMove( float lx, float ly ) {
	XEBaseScene::OnMouseMove( lx, ly );
}

int XSceneTechSel::OnBack( XWnd *pWnd, DWORD p1, DWORD p2 )
{
	DoExit( XGAME::xSC_WORLD );
	return 1;
}


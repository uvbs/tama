#include "StdAfx.h"
#include "XSceneSample.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XSceneSample *SCENE_SAMPLE = NULL;

void XSceneSample::Destroy() 
{	
	XBREAK( SCENE_SAMPLE == NULL );
	XBREAK( SCENE_SAMPLE != this );
	SCENE_SAMPLE = NULL;
}

XSceneSample::XSceneSample( XGame *pGame ) 
	: XESceneSlide( pGame, xSCENE::xSC_NONE )
{ 
	XBREAK( SCENE_SAMPLE != NULL );
	SCENE_SAMPLE = this;
	Init(); 
//	SetPosLocal( 0.f, 78.f );
//	SetSizeLocal( XE::GetGameWidth(), 
//		GRAPHICS->GetLogicalScreenSize().h - (GetPosLocal().h + 66.f) );
}

void XSceneSample::Create( void )
{
	XESceneSlide::Create( 0 );
}

XWnd* XSceneSample::CreateScene( int idx, DWORD p1, DWORD p2 )
{

	XE::VEC2 vSize = GetSizeLocal();
	XWnd *pWnd = new XWnd( 0.f, 0.f, vSize.w, vSize.h );
	if( idx == 0 )
	{

//		m_Layout.CreateLayout( new XAppLayout(_T("layout_sample.xml")), 
//			"sample", pWnd, "sample_group" );
		/*XWndButtonString *pButt =
			new XWndButtonString(0.f, 0.f, XTEXT(2033), XCOLOR_WHITE, FONT_NANUM_BOLD, 40.f, BUTT_BIG );
		pWnd->Add( pButt );
		pWnd->SetEvent( XWM_CLICKED, this, &XSceneSample::OnClick );*/
	}
	return pWnd;
}

int XSceneSample::Process( float dt ) 
{ 
	return XESceneSlide::Process( dt );
}

//
void XSceneSample::Draw( void ) 
{
	XESceneSlide::Draw();
	XESceneSlide::DrawTransition();
}

void XSceneSample::OnLButtonDown( float lx, float ly ) 
{
	XESceneSlide::OnLButtonDown( lx, ly );
}
void XSceneSample::OnLButtonUp( float lx, float ly ) {
	XESceneSlide::OnLButtonUp( lx, ly );
}
void XSceneSample::OnMouseMove( float lx, float ly ) {
	XESceneSlide::OnMouseMove( lx, ly );
}

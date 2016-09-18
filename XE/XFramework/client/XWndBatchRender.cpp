#include "stdafx.h"
#include "XWndBatchRender.h"
#include "OpenGL2/XRenderCmd.h"
#include "XFramework/XEProfile.h"
#include "OpenGL2/XTextureAtlas.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// XAutoCurrAtlas::XAutoCurrAtlas( XWndBatchRender* pWnd ) {
// 	m_pWndRenderer = pWnd;
// 	pWnd->SetCurrAtlas();
// }
// XAutoCurrAtlas::~XAutoCurrAtlas() {
// 	m_pWndRenderer->ClearCurrAtlas();
// }

////////////////////////////////////////////////////////////////
XWndBatchRender::XWndBatchRender( const char* cTag, bool bBatchRender )
	: m_pAtlas( new XTextureAtlas( cTag ) )
	, m_pRenderer( (bBatchRender)? new XRenderCmdMng( cTag ) : nullptr )
{
	Init();
}

BOOL XWndBatchRender::OnCreate()
{
	return true;
}

void XWndBatchRender::Destroy()
{
	SAFE_DELETE( m_pAtlas );
	SAFE_DELETE( m_pRenderer );
}

// void XWndBatchRender::AttatchBatchRenderer()
// {
// 	m_pRenderer = new XRenderCmdMng( )
// }

int XWndBatchRender::Process( float dt )
{
	int ret = 0;
	XBREAK( m_pAtlas == nullptr );
	SET_ATLASES( m_pAtlas )	{
		ret = XWnd::Process( dt );
	} END_ATLASES;
	return ret;
}

void XWndBatchRender::Draw()
{
	// 이 레이어에 속한 모든 UI는 일괄렌더링을 한다.
	if( m_pRenderer ) {
		SET_RENDERER( m_pRenderer ) {
			XWnd::Draw();
		} END_RENDERER;
	} else {
		XWnd::Draw();
	}
}

// void XWndBatchRender::SetCurrAtlas()
// {
// 	XTextureAtlas::sSetpCurrMng( m_pAtlas );
// }
// void XWndBatchRender::ClearCurrAtlas()
// {
// 	XTextureAtlas::sSetpCurrMng( nullptr );
// }

#include "stdafx.h"
#include "XWndBatchRender.h"
#include "OpenGL2/XBatchRenderer.h"
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
XWndBatchRender::XWndBatchRender( const char* cTag, bool bBatchRender, const XE::xRECT& rc )
	: XWnd( rc.vLT, rc.GetSize() )
	, m_pAtlas( new XTextureAtlas( cTag ) )
	, m_pRenderer( (bBatchRender)? new XBatchRenderer( cTag ) : nullptr )
{
	Init();
}

BOOL XWndBatchRender::OnCreate()
{
	// 크기가 안정해졌을경우 부모의 크기로 맞춘다.
	const auto sizeLocal = GetSizeLocal();
	if( sizeLocal.w < 2 || sizeLocal.h < 2 ) {
		XBREAK( GetpParent() == nullptr );
		const auto sizeParent = GetpParent()->GetSizeLocal();
		SetSizeLocal( sizeParent );
	}
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

void XWndBatchRender::DrawBefore()
{
	m_pPrev = XBatchRenderer::_sSetpCurrRenderer( m_pRenderer );
}

void XWndBatchRender::Draw()
{
	XWnd::Draw();
	// 이 레이어에 속한 모든 UI는 일괄렌더링을 한다.
// 	if( m_pRenderer ) {
// 		SET_RENDERER( m_pRenderer ) {
// 			XWnd::Draw();
// 		} END_RENDERER;
// 	} else {
// 		XWnd::Draw();
// 	}
}

void XWndBatchRender::DrawAfter()
{
	if( m_pRenderer ) {
		m_pRenderer->RenderBatch();
		XBatchRenderer::_sSetpCurrRenderer( m_pPrev );
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

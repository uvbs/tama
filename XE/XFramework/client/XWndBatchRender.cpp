#include "stdafx.h"
#include "XWndBatchRender.h"
#include "OpenGL2/XBatchRenderer.h"
#include "XFramework/XEProfile.h"
#include "OpenGL2/XTextureAtlas.h"
#include "XFramework/client/XEContent.h"

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
XWndBatchRender::XWndBatchRender( const char* cTag, 
																	bool bUseAtlas,
																	bool bBatchRender, 
																	bool bZBuff, 
																	bool bAlphaTest, 
																	const XE::xRECT& rc )
	: XWnd( rc.vLT, rc.GetSize() )
//	, m_spAtlas( new XTextureAtlas( cTag ) )
	, m_pRenderer( (bBatchRender)? new XBatchRenderer( cTag, bZBuff ) : nullptr )
{
	Init();
	if( bUseAtlas ) {
		m_spAtlas = XTextureAtlas::sCreateAtlasMng( cTag );
	}
	SetbTouchable( false );
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
// 	SAFE_DELETE( m_pAtlas );
	SAFE_DELETE( m_pRenderer );
}

// void XWndBatchRender::AttatchBatchRenderer()
// {
// 	m_pRenderer = new XRenderCmdMng( )
// }

int XWndBatchRender::Process( float dt )
{
	int ret = 0;
	if( m_spAtlas ) {
		XBREAK( m_spAtlas == nullptr );
		XTextureAtlas::XAutoPushObj _spAuto( m_spAtlas );
	}
	ret = XWnd::Process( dt );
	return ret;
}

void XWndBatchRender::OnDrawBefore()
{
	m_pPrev = XBatchRenderer::_sSetpCurrRenderer( m_pRenderer );
	m_bZBuffPrev = GRAPHICS->SetbEnableZBuff( m_bZBuff );
//	m_bAlphaTestPrev = GRAPHICS->SetbAlphaTest( m_bAlphaTest );
}

void XWndBatchRender::Draw()
{
	XWnd::Draw();
}

void XWndBatchRender::OnDrawAfter()
{
	if( m_pRenderer ) {
		m_pRenderer->RenderBatch();
	}
	XBatchRenderer::_sSetpCurrRenderer( m_pPrev );
	GRAPHICS->SetbEnableZBuff( m_bZBuffPrev );
//	GRAPHICS->SetbAlphaTest( m_bAlphaTestPrev );
}

void XWndBatchRender::DestroyDevice()
{
	XWnd::DestroyDevice();
	if( m_spAtlas )
		m_spAtlas->DestroyDevice();
}

void XWndBatchRender::OnPause()
{
	XWnd::OnPause();
	if( m_spAtlas )
		m_spAtlas->OnPause();
}

void XWndBatchRender::OnUpdateBefore()
{
	if( m_spAtlas )
		m_spAtlas->PushAtlasMng();
}

void XWndBatchRender::OnUpdateAfter()
{
	if( m_spAtlas )
		m_spAtlas->PopAtlasMng();
}

void XWndBatchRender::OnProcessBefore()
{
	if( m_spAtlas )
		m_spAtlas->PushAtlasMng();
}

void XWndBatchRender::OnProcessAfter()
{
	if( m_spAtlas )
		m_spAtlas->PopAtlasMng();
}

XSPAtlasMng XWndBatchRender::GetspAtlas()
{
	return XEContent::sGet()->GetspAtlas();
}


// XViewGraph.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "XViewGraph.h"
#include "MainFrm.h"
#include "XGraphicsD3DTool.h"
#include "Tool.h"
#include "XDlgEditInterpolation.h"
#include "Sprdef.h"
// XViewGraph

IMPLEMENT_DYNCREATE(XViewGraph, CView)

XViewGraph::XViewGraph()
{
	m_pDlg = nullptr;
}

XViewGraph::~XViewGraph()
{
}

BEGIN_MESSAGE_MAP(XViewGraph, CView)
	ON_WM_MOUSEACTIVATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// XViewGraph 그리기입니다.
#define AXIS_LENGTH		260.f
#define START_POINT		XE::VEC2( 20, 280 )		// 원점
#define END_POINT		XE::VEC2( 280, 20 )

void XViewGraph::DrawInterpolationLine()
{
	XE::VEC2 vPos, vPrev;
	vPrev = START_POINT;
	float t = 0;
	for( int i = 0; i < 10; i++ )
	{
		t += 0.1f;
		float slerp = 0;
		// 보간함수로 보간값을 얻음
		switch( (xSpr::xtInterpolation)m_pDlg->m_nSelectFunc )
		{
		case xSpr::xNONE:	return;
		case xSpr::xLINEAR:		slerp = XE::xiLinearLerp( t );	break;
		case xSpr::xACCEL:		slerp = XE::xiHigherPowerAccel( t, 1.0f, 0 );	break;
		case xSpr::xDEACCEL:	slerp = XE::xiHigherPowerDeAccel( t, 1.0f, 0 );	break;
		case xSpr::xSMOOTH:	slerp = XE::xiSmoothStep( t, 1.0f, 0 );		break;
		case xSpr::xSPLINE:		slerp = XE::xiCatmullrom( t, -10.0f, 0, 1, 1 );		break;
		default:
			XBREAKF( 1, "응?" );
			return;
		}
		// 그래프내 좌표를 계산
		vPos.x = START_POINT.x + AXIS_LENGTH * t;		// 시간축 좌표x축
		vPos.y = START_POINT.y - AXIS_LENGTH * slerp;	// 보간축 좌표y축
		// 이전 라인에 연결해서 그림
		XE::DrawLine( vPrev, vPos, XCOLOR_WHITE );
		vPrev = vPos;
	}
}
void XViewGraph::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
	if( GetChain() )
	{
		LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer )
		{
			RECT rect;
			GetClientRect( &rect );
			XE::SetProjection( rect.right-rect.left, rect.bottom-rect.top );
			D3DDEVICE->SetRenderTarget( 0, pBackBuffer );
			D3DDEVICE->Clear( 0, nullptr, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			if(SUCCEEDED(D3DDEVICE->BeginScene()))
			{
				// 
				XE::DrawLine( 20, 20, 20, 280, XCOLOR_WHITE );	// 수직선
				XE::DrawLine( 20, 280, 280, 280, XCOLOR_WHITE );	// 수평선
				SE::g_pFont->DrawString( 6, 283, _T("0") );
				SE::g_pFont->DrawString( 2, 23, _T("1.0") );
				SE::g_pFont->DrawString( 8, 4, _T("진행") );
				SE::g_pFont->DrawString( 270, 285, _T("시간(t)") );
				// 
				DrawInterpolationLine();
				//
				{
					CString str;
					str.Format( _T("%d, %d"), (int)m_vMouse.x, (int)m_vMouse.y );
					SE::g_pFont->DrawString( 100, 0, str );
				}
				D3DDEVICE->EndScene();
			}
			GetChain()->Present( nullptr, nullptr, GetSafeHwnd(), nullptr, 0 );
			pBackBuffer->Release();
		}
	}
}


// XViewGraph 진단입니다.

#ifdef _DEBUG
void XViewGraph::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void XViewGraph::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// XViewGraph 메시지 처리기입니다.

int XViewGraph::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return MA_ACTIVATE;

//	return __super::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void XViewGraph::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//	XD3DView::CreateView( GetSafeHwnd() );
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
}

void XViewGraph::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( D3DDEVICE )
	{
//		XD3DView::CreateView( GetSafeHwnd() );
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}
}

void XViewGraph::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	Invalidate(0);
	__super::OnLButtonDown(nFlags, point);
}

void XViewGraph::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_vMouse = point;

	Invalidate(0);
	__super::OnMouseMove(nFlags, point);
}


void XViewGraph::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	__super::OnLButtonUp(nFlags, point);
}

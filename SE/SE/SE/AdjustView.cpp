// AdjustView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "AdjustView.h"
#include "ThumbView.h"
#include "MainFrm.h"
#include "XGraphicsD3DTool.h"
#include "Tool.h"
//#include "Graphic2D.h"
#include "UndoMng.h"
#include "Sprite.h"
// CAdjustView

static CAdjustView *s_pAdjustView = nullptr;
CAdjustView *GetAdjustView() { return s_pAdjustView; }

IMPLEMENT_DYNCREATE(CAdjustView, CFormView)

CAdjustView::CAdjustView()
	: CFormView(CAdjustView::IDD)
{
	s_pAdjustView = this;
	m_x = m_y = 0;	
	m_DragMove = FALSE;
	m_bViewBaseSpr = TRUE;
}

CAdjustView::~CAdjustView()
{
	
}

void CAdjustView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAdjustView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// CAdjustView 진단입니다.

#ifdef _DEBUG
void CAdjustView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAdjustView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// static
CAdjustView *CAdjustView::CreateOne( CWnd *pParent )
{
	CAdjustView *pView = new CAdjustView;
	//CAdjustView *pFormView = nullptr;
	//CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CAdjustView);
	//pFormView = (CAdjustView *)pRuntimeClass->CreateObject();

//	CDockableFormViewAppDoc *pDoc = CDockableFormViewAppDoc::CreateOne();
//	pFormView->m_pDocument = pDoc;

	CCreateContext *pContext = nullptr;

#if 0
	if( !pView->CreateEx(0, nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0,0,205,157),
		pParent, -1, pContext ) )
#else
	if (!pView->Create(nullptr, nullptr, WS_CHILD | WS_VISIBLE, CRect(0, 0, 500, 500), pParent, 0, pContext))
#endif
	//if( !pFormView->CreateEx( 0, AfxRegisterWndClass(0, 0, 0, 0), nullptr,
	//	WS_CHILD | WS_VISIBLE, CRect( 0, 0, 205, 157), pParent, -1, pContext) )
	{
		AfxMessageBox( _T("Failed in creating CAdjustView") );
	}

	pView->OnInitialUpdate();

	return pView;
}

int CAdjustView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nResult = 0;

	CFrameWnd* pParentFrame = GetParentFrame();

	if( pParentFrame == pDesktopWnd )
	{
		// When this is docked
		nResult = CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}
	else
	{
		// When this is not docked

		BOOL isMiniFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CMiniFrameWnd ) );
		BOOL isPaneFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CPaneFrameWnd ) );
		BOOL isMultiPaneFrameWnd = pDesktopWnd->IsKindOf( RUNTIME_CLASS( CMultiPaneFrameWnd ) );

		// pDesktopWnd is the frame window for CDockablePane

		nResult = CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );

		//nResult = CWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
		//if( nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT )
		//	return nResult;

		//if (pDesktopWnd != nullptr)
		//{
		//	// either re-activate the current view, or set this view to be active
		//	//CView* pView = pDesktopWnd->GetActiveView();
		//	//HWND hWndFocus = ::GetFocus();
		//	//if (pView == this &&
		//	//	m_hWnd != hWndFocus && !::IsChild(m_hWnd, hWndFocus))
		//	//{
		//	//	// re-activate this view
		//	//	OnActivateView(TRUE, this, this);
		//	//}
		//	//else
		//	//{
		//	//	// activate this view
		//	//	pDesktopWnd->SetActiveView(this);
		//	//}
		//}
	}

	return nResult;
//	return __super::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

// CAdjustView 메시지 처리기입니다.
int CAdjustView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

//	DWORD style = WS_CHILD | WS_VISIBLE | CBRS_TOOLTIPS | CBRS_ALIGN_TOP | CBRS_ALIGN_ANY;
//	m_ToolBar.CreateEx( this, TBSTYLE_FLAT, style );

//	m_ToolBar.Create( this );
//	m_ToolBar.LoadToolBar( IDR_MAINFRAME );
//	m_ToolBar.ShowWindow( SW_SHOW );
//	m_ToolBar.SetBarStyle( CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY );
//	RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0 );
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
    RECT rc;
    GetClientRect(&rc);

	return 0;
}

void CAdjustView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
/*	RECT rect;
	int w, minw, h, minh;
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
	pMainFrm->m_wndSplitter.GetRowInfo( 0, h, minh );	
	pMainFrm->m_wndSplitter2.GetColumnInfo( 1, w, minw );		// 분할창의크기를 구함
	MoveWindow( 0, 0, w, h ); */
	{
		CSize size;
		size = GetTotalSize();
		size.cx = 0;
		size.cy = 0;
		SetScrollSizes( MM_TEXT, size );
	}
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
}

void CAdjustView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( GetChain() )
	{
		LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer )
		{
			D3DDEVICE->SetRenderTarget( 0, pBackBuffer );
			RECT rect;
			GetClientRect( &rect );
//			D3DXMATRIXA16 mOrtho;
//			D3DXMatrixOrthoOffCenterLH( &mOrtho, 0, (float)(rect.right-rect.left), (float)(rect.bottom-rect.top), 0, 0, 1.0f );
			D3DVIEWPORT9 vp;
			vp.X = 0;
			vp.Y = 0;
			vp.Width = rect.right;
			vp.Height = rect.bottom;
			vp.MinZ = 0;
			vp.MaxZ = 1.0f;
			D3DDEVICE->SetViewport( &vp );
			XE::SetProjection( rect.right-rect.left, rect.bottom-rect.top );
//			D3DDEVICE->SetTransform( D3DTS_PROJECTION, &mOrtho );
//			g_mViewProj = /* mView생략 */ mOrtho;
			D3DDEVICE->Clear( 0, nullptr, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			if(SUCCEEDED(D3DDEVICE->BeginScene()))
			{
				if( GetFocus() == this )
					GRAPHICS->DrawRect( 0, 0, (float)rect.right-1, (float)rect.bottom-1, XCOLOR_RED );
				// 십자선
				int nCenterX = rect.left+(rect.right-rect.left)/2;
				int nCenterY = rect.top+(rect.bottom-rect.top)/2;
				XE::DrawLine( 0, (float)nCenterY, 
													(float)rect.right, (float)nCenterY, XCOLOR_DARKGRAY );		// 가로선
				XE::DrawLine( (float)nCenterX, 0, 
													(float)nCenterX, (float)rect.bottom, XCOLOR_DARKGRAY );		// 세로선

				if( SPRDAT->GetNumSprite() > 0 )
				{
					// 기준스프라이트가 지정되어있으면 먼저 그림
					if( m_bViewBaseSpr && TOOL->GetidxBaseSpr() >= 0 )
					{
						XSprite *pBaseSpr = SPRDAT->GetSpriteIndex( TOOL->GetidxBaseSpr() );
						if( pBaseSpr )
						{
							pBaseSpr->SetScale( GetvScaleView() );
							pBaseSpr->Draw( (float)nCenterX, (float)nCenterY );
						} else
							TOOL->SetidxBaseSpr( -1 );
					}
					XSprite *pSpr;
					TOOL->GetSelSpr().GetNextClear();
					while( pSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetNext() ) )					// 선택된 스프라이트를 모두 겹쳐 찍는다
					{
						pSpr->SetScale( GetvScaleView() );
//						pSpr->SetScale( 5.0f, 10.0f, 1.0f );
						pSpr->Draw( (float)nCenterX, (float)nCenterY );
					}
					// 선택된 스프라이트들의 첫번째 스프라이트를 얻음
					TOOL->GetSelSpr().GetNextClear();
					pSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetNext() );
					if( pSpr )
					{
						CString str;
						RECT rect = pSpr->GetrectSrc();
						str.Format( _T("highreso: %d\r\n")\
										_T("size:%.2f,%.2f\r\n")\
										_T("adjust:%.2f,%.2f\r\n")\
										_T("src image:%s\r\n")\
										_T("surface format:%s\r\n") \
										_T("src area:%d,%d - %d,%d") 
										, pSpr->GetbHighReso()
										, pSpr->GetWidth()
										, pSpr->GetHeight()
										, pSpr->GetAdjustX()
										, pSpr->GetAdjustY()
										, pSpr->GetszSrcFile()
										, XE::GetstrEnum( pSpr->GetformatSurface() )
										, rect.left
										, rect.top
										, rect.right
										, rect.bottom	);
#ifdef _DEBUG
						CString strPri;
						strPri.Format( _T("Priority:%3.2f\n"), pSpr->GetfPriority() );
						str = strPri + str;
#endif
						SE::g_pFont->DrawString( 10, 10, str );
					}
				}
				{
					CString str;
					// 확축배율 표시
					str.Format( _T("%d%%"), (int)(GetvScaleView().x * 100.f) );
					SE::g_pFont->DrawString( (float)(rect.right-40), 2.f, str );
				}

				D3DDEVICE->EndScene();
			}
			GetChain()->Present( nullptr, nullptr, GetSafeHwnd(), nullptr, 0 );
			pBackBuffer->Release();
		}
	}
}




void CAdjustView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( D3DDEVICE )
	{
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
//		XD3DView::CreateView( GetSafeHwnd() );
	}
}

static CPoint s_prevPoint;
void CAdjustView::OnMouseMove(UINT nFlags, CPoint point)
{
	if( GetFocus() != this )
	{
		SetFocus();
		ALL_VIEW_UPDATE();
	}
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pointMouse = point;
//	RECT rect;
//	GetClientRect( &rect );
//	float fCenterX = (float)rect.left+(rect.right-rect.left)/2.0f;			// 뷰 전체의 중심점
//	float fCenterY = (float)rect.top+(rect.bottom-rect.top)/2.0f;
	if( SPRDAT )
	{
		XE::VEC2 dist;
		CPoint pt = point - s_prevPoint;
		dist.x = (float)pt.x;
		dist.y = (float)pt.y;
		dist /= GetvScaleView();
		if( m_DragMove )
		{
			TOOL->AdjustAxisLock( &dist.x, &dist.y );
			// 선택한 스프라이트의 adj를 모두 변화시킨다
			XSprite *pSpr;
			TOOL->GetSelSpr().GetNextClear();
			BOOL bAppend = FALSE;
			while( pSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetNext() ) )
			{
				if( m_DragMove == 1 )
					UNDOMNG->PushUndo( new XUndoSpriteChange( SPROBJ, pSpr, _T("Spr Adjust값 조정") ), bAppend );
				bAppend = TRUE;
				float adjx = pSpr->GetAdjustX();
				float adjy = pSpr->GetAdjustY();
				pSpr->ChangeAdjust( adjx+dist.x, adjy+dist.y );
			}
			m_DragMove = 2;
		}
		Update();
	}

	s_prevPoint = point;
//	__super::OnMouseMove(nFlags, point);
	CFormView::OnMouseMove(nFlags, point);
}

void CAdjustView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_DragMove = 1;
	__super::OnLButtonDown(nFlags, point);
}

#define SELECT_LOOP( TYPE, SELOBJ, ELEM )			\
	TYPE ELEM;												\
	SELOBJ.GetNextClear();									\
	while( ELEM = (TYPE)SELOBJ.GetNext() )				\
	
void CAdjustView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( m_DragMove )
	{
		SELECT_LOOP( XSprite*, TOOL->GetSelSpr(), pSpr )
		{
			XUndoSpriteChange *pUndo = dynamic_cast<XUndoSpriteChange *>(UNDOMNG->GetLastQ());
			if( pUndo )
				pUndo->SetSpriteAfter( pSpr );
		}
	}
	m_DragMove = 0;
	__super::OnLButtonUp(nFlags, point);
}

void CAdjustView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN )
	{
		XSprite *pSpr;
		TOOL->GetSelSpr().GetNextClear();
		while( pSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetNext() ) )					// 선택된 스프라이트를 모두 겹쳐 찍는다
		{
			float adjx = pSpr->GetAdjustX();
			float adjy = pSpr->GetAdjustY();
			adjx = ROUND_OFF2( adjx, 0.5f );
			adjy = ROUND_OFF2( adjy, 0.5f );
			switch( nChar )
			{
			case VK_LEFT:
				pSpr->ChangeAdjust( adjx-0.5f, adjy );
				break;
			case VK_RIGHT:
				pSpr->ChangeAdjust( adjx+0.5f, adjy );
				break;
			case VK_UP:
				pSpr->ChangeAdjust( adjx, adjy-0.5f );
				break;
			case VK_DOWN:
				pSpr->ChangeAdjust( adjx, adjy+0.5f );
				break;
			}
		}
		Invalidate(0);
	}

	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CAdjustView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CAdjustView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( zDelta < 0 )	// 아래로 굴림(축소하기)
	{
		XE::VEC2 vs = GetvScaleView() + XE::VEC2( -1.f, -1.f );
		if( vs.x < 1.f || vs.y < 1.f )
			vs.Set( 1, 1 );
		SetScaleView( vs.x );
	} else
	if( zDelta > 0 )
	{
		XE::VEC2 vs = GetvScaleView() + XE::VEC2( 1.f, 1.f );
		SetScaleView( vs.x );
	}

	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

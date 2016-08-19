// ThumbView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "MainFrm.h"
#include "ThumbView.h"
#include "AdjustView.h"
#include "XGraphicsD3DTool.h"
#include "Tool.h"
#include "ConsoleView.h"
#include "FrameView.h"
#include "AnimationView.h"
#include "XAniAction.h"
#include "UndoMng.h"
#include "Sprite.h"
#include "XKeyBase.h"
#include "XLayerBase.h"
#include "XLayerImage.h"
#include "DlgSpriteAttr.h"
#include "XActObj.h"
using namespace XE;
//#include "Graphic2D.h"
// CThumbView
static CThumbView *s_pThumbView = nullptr;
CThumbView *GetThumbView() { return s_pThumbView; }

IMPLEMENT_DYNCREATE(CThumbView, CScrollView)

CThumbView::CThumbView()
{
	s_pThumbView = this;
	
	m_fWPI = 64;
	m_pd3dTexture = nullptr;
	m_pd3dRTS = nullptr;
	m_pSurface = nullptr;
	m_pFont = SE::g_pFDBasic->CreateFontObj();
}

CThumbView::~CThumbView()
{
	SAFE_DELETE( m_pFont );
}


BEGIN_MESSAGE_MAP(CThumbView, CScrollView)
	ON_WM_DROPFILES()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_DEL_SEL_SPR, &CThumbView::OnDelSelSpr)
	ON_COMMAND(ID_ADD_SEL_SPR, &CThumbView::OnAddSelSpr)
	ON_COMMAND(ID_INIT_ADJ, &CThumbView::OnInitAdj)
	ON_COMMAND(ID_SET_BASE_SPR, &CThumbView::OnSetBaseSpr)
	ON_COMMAND(ID_CLEAR_BASE_SPR, &CThumbView::OnClearBaseSpr)
	ON_COMMAND(ID_INS_KEY_FRAME, &CThumbView::OnInsKeyFrame)
	ON_COMMAND(IDM_SPRITE_ATTR, &CThumbView::OnEditSpriteAttr)
	ON_WM_MOUSEACTIVATE()
	ON_COMMAND(ID_ADJ_CENTER, &CThumbView::OnAdjCenter)
	ON_COMMAND(IDM_SAVE_PNG, &CThumbView::OnSavePng)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// static
CThumbView *CThumbView::CreateOne( CWnd *pParent )
{
	CThumbView *pView = new CThumbView;
	//CThumbView *pFormView = nullptr;
	//CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CThumbView);
	//pFormView = (CThumbView *)pRuntimeClass->CreateObject();

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
		AfxMessageBox( _T("Failed in creating CThumbView") );
	}

	pView->OnInitialUpdate();

	return pView;
}

int CThumbView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
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

// CThumbView 그리기입니다.
int CThumbView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
//	m_ToolBar.Create( this );
//	m_ToolBar.LoadToolBar( IDR_MAINFRAME );
//	m_ToolBar.ShowWindow( SW_SHOW );
//	m_ToolBar.SetBarStyle( CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY );
//	RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0 );

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
    RECT rc;
    GetClientRect(&rc);

	return 0;
}

void CThumbView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	DragAcceptFiles( TRUE );

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
//	GetParentFrame()->RecalcLayout();
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
    RECT rc;
    GetClientRect(&rc);
//	ResizeParentToFit(FALSE);
//	XD3DView::CreateView( GetSafeHwnd() );
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );

}


// rect의 화면영역에 nStartSpr로부터 시작하는 스프라이트들을 썸네일로 표시한다. 
// WPI = WidthPerImage, 스프라이트그림 하나당 가로표시길이.
void CThumbView::DrawThumbNailView( int nStartSpr, float nWPI )
{
	if( !SPRDAT )	return;
	if( SPRDAT->GetNumSprite() == 0 )
		return;
	RECT rect;
	GetClientRect( &rect );
	int nXNum = (rect.right - rect.left + 1) / (int)nWPI;				// 화면크기에 섬네일하나의크기를 나누면 화면에 표시되는 섬네일 개수가 나온다.
	int nYNum = (rect.bottom - rect.top + 1) / (int)nWPI;
	int i, j, nSpr;
	// draw grid
	D3DCOLOR colRed = XCOLOR_RED;
	for( i = 1; i < nYNum+1; i ++ )
	{
		XE::DrawLine( 0, i*nWPI, (float)rect.right, i*nWPI, XCOLOR_DARKGRAY );
	}
	for( j = 1; j < nXNum+1; j ++ )
	{
		XE::DrawLine( j*nWPI,0, j*nWPI, (float)rect.bottom, XCOLOR_DARKGRAY );
	}
	// draw thumbnail
	nSpr = nStartSpr;
	SPRDAT->GetNextSprSet( nStartSpr );
	for( i = 0; i < nYNum; i ++ )
	{
		for( j = 0; j < nXNum; j ++ )
		{
			XSprite *pSpr = SPRDAT->GetNextSpr();
			if( pSpr == nullptr ) 
				goto END;
			float fScale = nWPI / (float)pSpr->GetWidth();		// 그림한장당 얼마나 확/축 시켜야할지 비율을 계산한다.
//			if( fScale < 1.0f )		// 확대는 시킬필요 없고 축소되야 하는경우만 스케일링 한다
			pSpr->SetScale( fScale, fScale );
			pSpr->DrawNoAdjust( j * nWPI, i * nWPI ); 
			// 선택박스
			float x = (nSpr % nXNum) * nWPI - 1;
			float y = (nSpr / nXNum) * nWPI - 1;
			if( TOOL->GetSelSpr().Find( pSpr ) )
				DrawRectangle( x, y, x+nWPI+2, y+nWPI+2, colRed );
			// 기준스프라이트 표시
			if( TOOL->GetidxBaseSpr() == nSpr )
			{
				DrawRectangle( x, y, x+nWPI+2, y+nWPI+2, XCOLOR_BLUE );
			}
			m_pFont->SetColor( XCOLOR_RED );
			m_pFont->DrawNumber( j * nWPI + 3, i * nWPI + 3, nSpr );
			pSpr->SetnIdx( nSpr );		// 화면에 그릴때마다 인덱스를 갱신해 넣는다
			nSpr ++;
		}
	}
END:
	i = 0;
}

void CThumbView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
	if( GetChain() )
	{
		LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer )
		{
			D3DDEVICE->SetRenderTarget( 0, pBackBuffer );
			D3DDEVICE->Clear( 0, nullptr, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			RECT rect;
			GetClientRect( &rect );
			XE::SetProjection( rect.right-rect.left, rect.bottom-rect.top );
			if(SUCCEEDED(D3DDEVICE->BeginScene()))
			{
				if( GetFocus() == this )
					GRAPHICS->DrawRect( 0, 0, (float)rect.right-1, (float)rect.bottom-1, XCOLOR_RED );
				DrawThumbNailView( 0, m_fWPI );
				if( TOOL->m_DragDrop.GetDrag() )
				{
					TOOL->m_DragDrop.GetSpr()->SetScale( 1.0f, 1.0f );
					TOOL->m_DragDrop.GetSpr()->DrawNoAdjust( (float)m_pointMouse.x, (float)m_pointMouse.y );
				}
				D3DDEVICE->EndScene();
			}
			GetChain()->Present( nullptr, nullptr, GetSafeHwnd(), nullptr, 0 );
			pBackBuffer->Release();
		}
	}

}


// CThumbView 진단입니다.

#ifdef _DEBUG
void CThumbView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CThumbView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CThumbView 메시지 처리기입니다.
bool Comp( const CString& str1, const CString& str2 )
{
	return str1 < str2;
}
void CThumbView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TCHAR szFilename[256];
//	BOOL bMultiDrop = FALSE;
	// 드랍하려는 파일이 2개이상이면 액션추가를 묻는다.
	memset( szFilename, 0, sizeof(szFilename) );
	DragQueryFile( hDropInfo, 1, szFilename, 256 );		
	BOOL bNewAction = FALSE;
	BOOL bCreateKey = FALSE;
	if( AfxMessageBox( _T("새로운 액션으로 추가하시겠습니까?"), MB_YESNO ) == IDYES ) {
		auto spActObj = TOOL->CreateAction( _T("noname") );
		if( spActObj == nullptr ) {
			CScrollView::OnDropFiles(hDropInfo);
			return;
		}
		TOOL->SetSelAct( spActObj->GetidAct() );			// 추가된 액션으로 현재 선택된 액션번호를 바꾼다.
		bNewAction = TRUE;
		bCreateKey = TRUE;
	} else {
		auto spAction = TOOL->GetSelAct();
		if( spAction ) {		// 현재 선택되어있는 액션이 있으면
			if( XALERT_YESNO( "%d:%s 액션에 키프레임으로 추가하시겠습니까?", 
					spAction->GetidAct(), spAction->GetszActName() ) == IDYES )
				bCreateKey = TRUE;
		}
	}
	// 드랍한 파일목록을 리스트로 만듬
	list<CString> listFiles;
	int i = 0;
	while(1) {
		memset( szFilename, 0, sizeof(szFilename) );
		DragQueryFile( hDropInfo, i, szFilename, 256 );
		if( szFilename[0] == 0 )
			break;
		listFiles.push_back( CString(szFilename) );
		i ++;
	}

	// 파일목록을 소트함
	listFiles.sort( Comp );
	// 현재 선택된 액션에 프레임을 추가한다.
	list<CString>::iterator itor;
	i = 0;
	for( itor = listFiles.begin(); itor != listFiles.end();)
	{
		CString strFullPath = (*itor);
		strFullPath.MakeLower();				
		if( strFullPath.Right( 3 ) == "png" || strFullPath.Right( 3 ) == "tif" )		// png파일만 처리한다
//		if( _tcsicmp( GET_FILE_EXT( strFullPath ), "png" ) == 0 )		// png파일만 처리한다
		{
			TOOL->AddAniFrame( i++, (LPCTSTR)(*itor), bCreateKey );
			GetConsoleView()->Message( _T("%s loaded"), (LPCTSTR)(*itor) );
		}
		listFiles.erase( itor++ );
	}
	listFiles.clear();
//	SPROBJ->SetAction( 0 );
//	XBaseKey::s_pAction = SPROBJ->GetAction(0);

	Invalidate( FALSE );
	GetAdjustView()->Update();
	GetFrameView()->Update();
	GetFrameView()->Update();

	CScrollView::OnDropFiles(hDropInfo);
}

void CThumbView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	if( D3DDEVICE )
	{
//		XD3DView::CreateView( GetSafeHwnd() );
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

int CThumbView::GetSprIdx( CPoint point )
{
	if( !SPRDAT ) return -1;
	RECT rect;
	GetClientRect( &rect );
	int nXNum = (rect.right - rect.left + 1) / (int)m_fWPI;				// 화면크기에 섬네일하나의크기를 나누면 화면에 표시되는 섬네일 개수가 나온다.
	int nYNum = (rect.bottom - rect.top + 1) / (int)m_fWPI;
	int nSelSpr = 0;
	if( point.x < nXNum * m_fWPI )
	{
		if( point.y < nYNum * m_fWPI )
		{
			nSelSpr = (point.y / (int)m_fWPI) * nXNum + (point.x / (int)m_fWPI);
			if( nSelSpr < SPRDAT->GetNumSprite() )
				return nSelSpr;
		}
	}
	return -1;
}

void CThumbView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_pointMouse = point;
	//
	int nSpr = GetSprIdx( point );
	if( nSpr >= 0 )
	{
		// fuck ???이건 무슨 코드여
		XSprite *pSpr = SPRDAT->GetSpriteIndex( nSpr );
		HRESULT hr;
		UINT w = (UINT)pSpr->GetWidth();
		UINT h = (UINT)pSpr->GetHeight();
		SAFE_RELEASE( m_pd3dTexture );
		hr = D3DDEVICE->CreateTexture( w, h,
													1,		// 밉맵레벨
													D3DUSAGE_DYNAMIC,		
													D3DFMT_A8R8G8B8,		// 텍스쳐 포맷
													D3DPOOL_DEFAULT,		
													&m_pd3dTexture,
													nullptr );
		SAFE_RELEASE( m_pd3dRTS );
		hr = D3DXCreateRenderToSurface( D3DDEVICE, 
													w, h,
													D3DFMT_A8R8G8B8,
													FALSE,
													D3DFMT_UNKNOWN,
													&m_pd3dRTS );
											
		D3DVIEWPORT9 vp;
		vp.X = vp.Y = 0;
		vp.Width = w;
		vp.Height = h;
		vp.MinZ = 0;
		vp.MaxZ = 1;
		LPDIRECT3DSURFACE9 pd3dSurface;
		hr = m_pd3dTexture->GetSurfaceLevel( 0, &pd3dSurface );
		XE::SetProjection( w, h );
		hr = m_pd3dRTS->BeginScene( pd3dSurface, &vp );

		pSpr->DrawNoAdjust( 0, 0 );

		SAFE_RELEASE( pd3dSurface );
		m_pd3dRTS->EndScene( D3DX_FILTER_LINEAR );

		m_w = w;
		m_h = h;
		D3DLOCKED_RECT lockedRect;
		hr = m_pd3dTexture->LockRect( 0, &lockedRect, nullptr, D3DLOCK_READONLY );

		DWORD *pDst = new DWORD[ w * h ];
		DWORD *pSrc = (DWORD *)lockedRect.pBits;
		for( int i = 0; i < (int)h; i ++ )
		{
			for( int j = 0; j < (int)w; j ++ )
			{
				pDst[ i * w + j ] = pSrc[ i * (lockedRect.Pitch/4) + j ];
			}
		}
		SAFE_DELETE( m_pSurface );
		m_pSurface = new XSurfaceD3D( 0, (float)w, (float)h, 0, 0, pDst, 1 );
//		SAFE_DELETE_ARRAY( pBits );

		hr = m_pd3dTexture->UnlockRect( 0 );

//		SAFE_RELEASE( m_pd3dRTS );
	}


	CScrollView::OnLButtonDown(nFlags, point);
}

void CThumbView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( GetFocus() != this )
	{
		SetFocus();
		ALL_VIEW_UPDATE();
	}
	m_pointMouse = point;
	if( nFlags & MK_LBUTTON )
	{
		if( TOOL->m_DragDrop.GetDrag() == FALSE )	
		{
			int nSpr = GetSprIdx( point );
			if( nSpr >= 0 )
			{
				TOOL->GetSelSpr().Clear();		// 선택되어있던 스프라이트는 모두 해제하고
				XSprite *pSpr = SPRDAT->GetSpriteIndex( nSpr );
				TOOL->m_DragDrop.SetDrag( TRUE );			// 버튼이 눌린채로 마우스가 움직이고 있을때 드래그중이 아니었다면 드래그앤드롭모드로 바꾼다.
				TOOL->m_DragDrop.SetSpr( pSpr );
				TOOL->GetSelSpr().Add( pSpr );
			}
		} else
			Invalidate( FALSE );
	}
	CScrollView::OnMouseMove(nFlags, point);
}

void CThumbView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( TOOL->m_DragDrop.GetDrag() )
		TOOL->m_DragDrop.SetDrag( FALSE );			// 일단은 드래그중에 이곳에 드롭했다면 취소시킨다.
	// 스프라이트 선택
	if( SPRDAT && SPRDAT->GetNumSprite() > 0 )
	{
		int nSpr = GetSprIdx( point );
		if( nSpr >= 0 )
		{
			XSprite *pSpr = SPRDAT->GetSpriteIndex( nSpr );
			if( nFlags & MK_CONTROL )
			{
				if( TOOL->GetSelSpr().Find( pSpr ) )
					TOOL->GetSelSpr().Del( pSpr );
				else
					TOOL->GetSelSpr().Add( pSpr );
			} else
			if( nFlags & MK_SHIFT )
			{
				if( TOOL->GetSelSpr().GetNum() == 0 )
					TOOL->GetSelSpr().Add( pSpr );
				else
				{
					XSprite *pLastSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetLast() );
					XSprite *pCurrSpr;
					SPRDAT->GetNextSprClear();
					BOOL bSel = FALSE;
					while( pCurrSpr = SPRDAT->GetNextSpr() )
					{
						if( bSel )
							TOOL->GetSelSpr().Add( pCurrSpr );
						if( pCurrSpr == pLastSpr )						// if( bSel )이 먼저 나와야 한다. shift로 선택할때는 첫번째 스프라이트가 이미 셀렉트되어 있는상태기때문에 중복 추가될수 있다
							bSel = TRUE;
						if( pCurrSpr == pSpr )		// 현재 클릭한 스프라이트까지 다 애드했으면 중지
							break;
					}
				}
			} else
			{
				TOOL->GetSelSpr().Clear();
				TOOL->GetSelSpr().Add( SPRDAT->GetSpriteIndex( nSpr ) );
			}
		} else
			TOOL->GetSelSpr().Clear();
	}
	Invalidate( FALSE );
	GetAdjustView()->Update();
	CScrollView::OnLButtonUp(nFlags, point);
}

void CThumbView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
{
	CMenu		menu, *popup;
	::POINT	p;
	
	m_pointMouse = point;
	menu.LoadMenu( IDR_POPUP_THUMB );
	popup = menu.GetSubMenu(0);
	
	GetCursorPos( &p );
	popup->TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, 
		p.x, p.y, this );
	
	menu.DestroyMenu();
}

	CScrollView::OnRButtonUp(nFlags, point);
}

void CThumbView::OnEditSpriteAttr()
{
	int idxSpr = GetSprIdx( m_pointMouse );		// 마우스 좌표위치의 스프라이트 인덱스를 구함
	if( idxSpr >= 0 )	{
		CDlgSpriteAttr dlg;
//		auto pSprite = SPRDAT->GetSpriteIndex( idxSpr );
		dlg.m_formatSurface = XE::xPF_ARGB4444;
		if( dlg.DoModal() == IDOK ) {
			for( auto pElem : TOOL->GetSelSpr().GetlistElem() ) {
				auto pSpr = SafeCast<XSprite*>( pElem );
				if( pSpr )
					pSpr->SetformatSurface( dlg.m_formatSurface );
			}
//			pSprite->SetformatSurface( dlg.m_formatSurface );
			Invalidate( FALSE );
		}
// 		auto pSprite = SPRDAT->GetSpriteIndex( idxSpr );
// 		dlg.m_formatSurface = pSprite->GetformatSurface();
// 		if( dlg.DoModal() == IDOK ) {
// 			pSprite->SetformatSurface( dlg.m_formatSurface );
// 			Invalidate( FALSE );
// 		}
	}
}
// 선택한 스프라이트를 모두 삭제합니다
void CThumbView::OnDelSelSpr()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XSprite *pSpr;
	TOOL->GetSelSpr().GetNextClear();
	BOOL bAppend = FALSE;
	while( pSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetNext() ) ) {
		XBaseKey *pKey = SPRDAT->FindUseSprite( pSpr );			// 모든액션의 키를 모두 뒤져서 pSpr을 사용하고 있는 곳이 있는지 찾는다
		if( pKey ) {
			auto spAction = pKey->GetspAction();
			XBREAK( spAction == nullptr );
			auto spLayer = SPROBJ->GetspLayer( pKey );
			if( XASSERT( spLayer ) ) {
				CString str;
				str.Format( _T( "삭제하려는 스프라이트를 사용하고 있는 액션이 있어 삭제가 실패했습니다. 액션을 먼저 삭제해 주세요\r\n사용액션:%d, %s 의 %s Layer의 키" )
										, spAction->GetidAct()
										, spAction->GetszActName()
										, (LPCTSTR)spLayer->GetLabel() );
				//										, (LPCTSTR)pKey->GetspLayer()->GetLabel() );
				XALERT( "%s", (LPCTSTR)str );
				GetConsoleView()->Message( str );
				break;
			}
		}
		TOOL->GetSelSpr().Del( pSpr );				// 선택 스프라이트리스트에서 삭제하고
		UNDOMNG->PushUndo( new XUndoDelSprite( SPROBJ, pSpr ), bAppend );
		bAppend = TRUE;
		SPRDAT->DelSprite( pSpr );				// SPROBJ에 들어있는 본 데이타 삭제
		pSpr = nullptr;
	}
	Invalidate( FALSE );
	GetAdjustView()->Update();
}
// 선택한 스프라이트들을 새 액션으로 추가
void CThumbView::OnAddSelSpr()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( TOOL->GetSelSpr().GetNum() == 0 )		// 스프라이트 선택한게 없으면 리턴
		return;
	// 새 액션을 추가
	XSPActObj spActObj = TOOL->CreateAction( _T("noname") );
	int numLayerImage = spActObj->GetNumLayerByType( xSpr::xLT_IMAGE );
	if( numLayerImage == 0 )
		spActObj->CreateAddLayer( xSpr::xLT_IMAGE );
	TOOL->SetSelAct( spActObj->GetidAct() );		// 생성한 액션을 선택
//	auto spLayerImage = spAction->GetspLayerImage( 0 );
//	auto spLayerImage = spActObj->GetspLayerByNumber<XLayerImage>( 0 );
	auto spLayerImage = spActObj->GetspLayerByIndex<XLayerImage>( 0 );
	if( XASSERT( spLayerImage ) )
		TOOL->AddKeyFromSelSpr( spLayerImage );		// 0번 이미지레이어를 찾아서 그곳에 추가한다
//	TOOL->AddKeyFromSelSpr( SPROBJ->GetLayer( XBaseLayer::xIMAGE_LAYER, 0 ) );		// 0번 이미지레이어를 찾아서 그곳에 추가한다
	GetFrameView()->UpdateOfsYBySelectedLayer();
	GetFrameView()->Update();
	GetAnimationView()->Update();
}
// 선택한 스프라이트들을 현재 프레임부터 차례로 이미지키로 입력
void CThumbView::OnInsKeyFrame()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 현재 선택된 레이어의 현재 프레임에 선택된 스프라이트 리스트를 키로 추가
	auto spBaseLayer = TOOL->GetspSelLayer();
	if( spBaseLayer ) {
		if( spBaseLayer->IsTypeLayerImage() ) {
			auto spLayerImage = std::static_pointer_cast<XLayerImage>( spBaseLayer );
			if( XASSERT(spLayerImage) )
				TOOL->AddKeyFromSelSpr( spLayerImage );
		} else {
			CONSOLE( "image layer를 선택해야 함." );
		}
	} else {
		CONSOLE( "레이어가 선택되지 않음." );
	}
	GetFrameView()->Update();
}

// 선택한 스프라이트의 adj를 모두 0으로 초기화
void CThumbView::OnInitAdj()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	BOOL bAppend = FALSE;
	XSprite *pSpr;
	TOOL->GetSelSpr().GetNextClear();
	while( pSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetNext() ) )
	{
		XUndoSpriteChange *pUndo = new XUndoSpriteChange( SPROBJ, pSpr, _T("adj를 0으로") );
		UNDOMNG->PushUndo( pUndo, bAppend );
		bAppend = TRUE;
		pSpr->ChangeAdjust( 0, 0 );
		pUndo->SetSpriteAfter( pSpr );
	}
	Update();
	GetAdjustView()->Update();
	GetAnimationView()->Update();
}

void CThumbView::OnAdjCenter()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	BOOL bAppend = FALSE;
	XSprite *pSpr;
	TOOL->GetSelSpr().GetNextClear();
	while( pSpr = static_cast<XSprite*>( TOOL->GetSelSpr().GetNext() ) )
	{
		XUndoSpriteChange *pUndo = new XUndoSpriteChange( SPROBJ, pSpr, _T("스프라이트 중앙정렬") );
		UNDOMNG->PushUndo( pUndo, bAppend );
		bAppend = TRUE;
		pSpr->ChangeAdjust( -pSpr->GetWidth() / 2.0f,		// 이미지의 중앙으로 정렬
								   -pSpr->GetHeight() / 2.0f );
		pUndo->SetSpriteAfter( pSpr );
	}
	Update();
	GetAdjustView()->Update();
	GetAnimationView()->Update();
}

void CThumbView::OnSetBaseSpr()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	int idxSpr = GetSprIdx( m_pointMouse );		// 마우스 좌표위치의 스프라이트 인덱스를 구함
	if( idxSpr >= 0 )
	{
		TOOL->SetidxBaseSpr( idxSpr );		// 기준스프라이트로 지정
	}

}

void CThumbView::OnClearBaseSpr()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	TOOL->SetidxBaseSpr( -1 );
}

void CThumbView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CThumbView::OnSavePng()
{
	XE::VEC2 sizeLT(9999), sizeRB(-9999);
	// 선택된 스프라이트들의 최대 바운더리를 구해서 그 크기의 중심으로 데이타를 옮기고 저장해야함.
	{
		auto listElem = TOOL->GetSelSpr().GetlistElem();
		for( auto pElem : listElem ) {
			auto pSpr = SafeCast<XSprite*>( pElem );
			if( pSpr ) {
				const auto vAdj = pSpr->GetMemAdjust();
				const auto vSize = pSpr->GetMemSize();
				if( vAdj.x < sizeLT.x )
					sizeLT.x = vAdj.x;
				if( vAdj.x + vSize.w > sizeRB.x )
					sizeRB.x = vAdj.x + vSize.w;

				if( vAdj.y < sizeLT.y )
					sizeLT.y = vAdj.y;
				if( vAdj.y + vSize.h > sizeRB.y )
					sizeRB.y = vAdj.y + vSize.h;
			}
		}
	}
	// 최대 크기로 메모리 할당.
	XE::POINT sizeBuff;
	sizeBuff.w = (int)(sizeRB.x - sizeLT.x);
	sizeBuff.h = (int)(sizeRB.y - sizeLT.y);
	DWORD* pBuff = new DWORD[ sizeBuff.w * sizeBuff.h * 4 ];
	//
	CString strFilename = TOOL->GetstrFilename();
	CString strFileTitle = XE::GetFileTitle( (LPCTSTR)strFilename );
	int idx = 0;
	auto listElem = TOOL->GetSelSpr().GetlistElem();
	for( auto pElem : listElem ) {
		auto pSpr = SafeCast<XSprite*>( pElem );
		if( pSpr ) {
			XE::VEC2 vSizeSrc;
			DWORD* pSrcImg = pSpr->GetSrcImg( &vSizeSrc );
			memset( pBuff, 0, sizeBuff.Size() * 4 );
			// 중심위치로 스프라이트를 옮김
			{
				const auto vMemSize = pSpr->GetMemSize();
				const auto vAdj = pSpr->GetMemAdjust();
				auto vCenter = XE::VEC2( sizeBuff.w - sizeRB.x, sizeBuff.h - sizeRB.y );
				const auto vLT = vCenter + XE::VEC2( vAdj.x, vAdj.y );
// 				auto vCenter = XE::VEC2( sizeBuff.w / 2, sizeBuff.h );
// 				auto vRB = vAdj + vMemSize;
// 				vCenter.y -= sizeRB.y - vRB.y;
// 				vCenter.x -= sizeRB.x - vRB.x;
// 				const auto vLT = vCenter + XE::VEC2( vAdj.x, -vMemSize.h );
				XBREAK( vLT.x < 0 || vLT.y < 0 );
				XBREAK( vLT.x + vSizeSrc.w > sizeBuff.w );
				XBREAK( vLT.y + vSizeSrc.h > sizeBuff.h );
				DWORD* pSrc = pSrcImg;
				DWORD* pDst = pBuff + (int)(vLT.y * sizeBuff.w + vLT.x);
				for( int y=0; y < (int)vSizeSrc.h; ++y ) {
					for( int x=0; x < (int)vSizeSrc.w; ++x ) {
						*pDst++ = *pSrc++;
					}
					pDst += sizeBuff.w - (int)vSizeSrc.w;
				}
			}

			CString str;
//			str.Format(_T("%s%03d.png"), (LPCTSTR)strFileTitle, idx++);
			if( XE::IsHave( pSpr->GetszSrcFile() ) )
				str = pSpr->GetszSrcFile();
			else {
				str.Format(_T("%s%03d.png"), (LPCTSTR)strFileTitle, idx++);
				CONSOLE( "경고: 저장중 파일명을 찾을수가 없어 %s로 저장함.", (LPCTSTR)str );
			}
			GRAPHICS_D3DTOOL->SavePng( (LPCTSTR)str, sizeBuff.w, sizeBuff.h, pBuff );
			CONSOLE("saved png: %s", (LPCTSTR)str );
		}
	}
}


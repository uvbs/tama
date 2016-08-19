// AnimationView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "AnimationView.h"
#include "MainFrm.h"
#include "Tool.h"
#include "ThumbView.h"
#include "AdjustView.h"
#include "FrameView.h"
#include "ConsoleView.h"
#include "DlgDrawProperty.h"
#include "DlgObjProperty.h"
#include "XLayerBase.h"
#include "SprDat.h"
#include "XAniAction.h"
#include "UndoAction.h"
#include "UndoMng.h"
#include "XKeyBase.h"
#include "XLayerAll.h"
#include "XKeyAll.h"
#include "XActObj.h"
//#include "Graphic2D.h"

#define MI_LOOP( I, E )	{			\
								XMI_Itor I; \
								for( I = m_listMI.begin(); I != m_listMI.end(); I ++ ) \
								{	\
									XMovableInterface *E = (*I);		\

#define MI_MANUAL_LOOP( I, E )	{			\
								XMI_Itor I; \
								for( I = m_listMI.begin(); I != m_listMI.end(); ) \
								{	\
									XMovableInterface *E = (*I);		\

#define END_LOOP	} }

// CAnimationView
static CAnimationView *s_pAnimationView = nullptr;
CAnimationView *GetAnimationView() { return s_pAnimationView; }



IMPLEMENT_DYNCREATE(CAnimationView, CFormView)

CAnimationView::CAnimationView()
	: CFormView(CAnimationView::IDD)
{
	s_pAnimationView = this;

	//m_fCenterX = m_fCenterY = 0;
	m_nDragLayer = 0;
	m_nDragRotate = 0;
	m_nDragScale = FALSE;
	m_nDragScale = FALSE;
	m_fPrevAngle = 0;
	m_fPrevDistFromAxis = 0;
	m_pNewAddKey = nullptr;
	m_pMICross = nullptr;
	m_pDragMI = nullptr;
	m_pMIEventCreateObj = nullptr;
	m_bViewAdjustAxis = TRUE;
	m_pLastUndoKeyChange = nullptr;
//	m_bAlt = FALSE;
// 	m_nDragPosKeyGenerate = 0;
// 	m_speedPosKeyGenerate = 1.0f;		// 1/60초당 전진하는 속도

	SetsizeObj( XE::VEC2( 6.0f, 6.0f ) );	// 셀렉트키 사각 기즈모의 크기
}

CAnimationView::~CAnimationView()
{
	SAFE_DELETE( m_pMICross );
	SAFE_DELETE( m_pMIEventCreateObj );
	MI_MANUAL_LOOP( itor, pmi )
	{
		m_listMI.erase( itor++ );
	}
	END_LOOP;
}

void CAnimationView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAnimationView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_COMMAND(ID_DRAW_PROPERTY, &CAnimationView::OnDrawProperty)
	ON_UPDATE_COMMAND_UI(ID_DRAW_PROPERTY, &CAnimationView::OnUpdateDrawProperty)
	ON_COMMAND(ID_CREATE_EVENT, &CAnimationView::OnCreateEvent)
	ON_UPDATE_COMMAND_UI(ID_CREATE_EVENT, &CAnimationView::OnUpdateCreateEvent)
	ON_COMMAND(ID_EDIT_EVENT, &CAnimationView::OnEditEvent)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EVENT, &CAnimationView::OnUpdateEditEvent)
	ON_COMMAND(ID_PROPERTY_OBJ, &CAnimationView::OnPropertyObj)
	ON_UPDATE_COMMAND_UI(ID_PROPERTY_OBJ, &CAnimationView::OnUpdatePropertyObj)
	ON_COMMAND(ID_VIEW_TRANS_AXIS, &CAnimationView::OnViewTransAxis)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRANS_AXIS, &CAnimationView::OnUpdateViewTransAxis)
	ON_COMMAND(ID_CREATE_DUMMY, &CAnimationView::OnCreateDummy)
	ON_COMMAND(ID_CREATE_SOUND, &CAnimationView::OnCreateSound)
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
END_MESSAGE_MAP()


// CAnimationView 진단입니다.

#ifdef _DEBUG
void CAnimationView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAnimationView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAnimationView 메시지 처리기입니다.
LRESULT CAnimationView::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls( this, FALSE );
	return (LRESULT)0;
}

void CAnimationView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//	int w, minw, h, minh;
	CMainFrame *pMainFrm = (CMainFrame *)AfxGetMainWnd();
////	pMainFrm->m_wndSplitter.GetRowInfo( 1, h, minh );	
////	pMainFrm->m_wndSplitter3.GetColumnInfo( 1, w, minw );		// 분할창의크기를 구함
//	pMainFrm->m_wndSplitter.GetRowInfo( 0, h, minh );	
//	pMainFrm->m_wndSplitter.GetColumnInfo( 0, w, minw );		// 분할창의크기를 구함
//	MoveWindow( 0, 0, w, h );
	{
		CSize size;
		size = GetTotalSize();
		size.cx = 0;
		size.cy = 0;
		SetScrollSizes( MM_TEXT, size );
	}
//	XD3DView::CreateView( GetSafeHwnd() );
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );

}

int CAnimationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
    RECT rc;
    GetClientRect(&rc);
//	m_ToolBar.Create( this );
//	m_ToolBar.LoadToolBar( IDR_MAINFRAME );
//	m_ToolBar.ShowWindow( SW_SHOW );
//	m_ToolBar.SetBarStyle( CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY );
//	RepositionBars( AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0 );
	m_pMICross = new XMICross( 0, 0, 64.0f, 64.0f );
	m_pMICross->SetbActive( TRUE );		// 디폴트로는 감춰놓는다
	m_pMIEventCreateObj = new XMICrossDir( 0, 0, 64.0f, 50.0f, 32.0f );
	return 0;
}

void CAnimationView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	RECT rect;
	GetClientRect( &rect );
	m_vCenter.Set( rect.left+(rect.right-rect.left)/2.0f, rect.top+(rect.bottom-rect.top)/2.0f );
	if( D3DDEVICE )
	{
//		XD3DView::CreateView( GetSafeHwnd() );
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
	}
}




void CAnimationView::Draw()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	auto spActObj = SPROBJ->GetspActObjCurr();
	static DWORD dwPrevTime = timeGetTime();
	DWORD dwTime = timeGetTime();
	float dt = (float)(dwTime - dwPrevTime) / (1000.0f / 60.0f);
	if( TOOL->GetbFrameSkip() == FALSE )
		dt = 1.0f;
	dwPrevTime = dwTime;
	auto vScaleView = GetvScaleView();
	if( GetChain() ) {
		LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
		GetChain()->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( pBackBuffer ) {
			RECT rect;
			GetClientRect( &rect );
			XE::SetProjection( rect.right-rect.left, rect.bottom-rect.top );
			D3DDEVICE->SetRenderTarget( 0, pBackBuffer );
			D3DRECT clearRect;
			clearRect.x1 = rect.left;
			clearRect.y1 = rect.top + 32;		// 툴바가 위치할곳은 비워둔다.
			clearRect.x2 = rect.right;
			clearRect.y2 = rect.bottom;
//			D3DDEVICE->Clear( 1, &clearRect, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			D3DDEVICE->Clear( 0, nullptr, D3DCLEAR_TARGET, XCOLOR_GRAY, 1.0f, 0 );
			if(SUCCEEDED(D3DDEVICE->BeginScene())) {
				// 설정된 배경이미지 놓기
				if( TOOL->GetpsfcBg() ) {
					auto pSurface = TOOL->GetpsfcBg();
					pSurface->SetScale( vScaleView );
					auto vTexSize = (pSurface->GetSize() * vScaleView ) * 0.5f;
					auto v = (m_vCenter + (m_vAdjBackBg * vScaleView) ) - vTexSize;
					TOOL->GetpsfcBg()->Draw( v );
				}
				// 현재창에 포커스가 되어있으면 빨간 외곽선 그리기
				if( GetFocus() == this )
					GRAPHICS->DrawRect( 0, 0, (float)(rect.right - rect.left)-1.f, (float)(rect.bottom - rect.top)-1.f, XCOLOR_RED );
				float fCenterX = m_vCenter.x;
				float fCenterY = m_vCenter.y;
				// 십자선
				if( SPROBJ->IsPause() ) {
					XE::DrawLine( 0, fCenterY,  (float)rect.right, fCenterY, XCOLOR_DARKGRAY );		// 가로선
					XE::DrawLine( fCenterX, 0,  fCenterX, (float)rect.bottom, XCOLOR_DARKGRAY );		// 세로선
				}
				// 전체 게임화면 영역
				{
					XE::VEC2 vLT( -640/2.0f, -360/2.0f );
					XE::VEC2 vRB( 640/2.0f, 360/2.0f ); 
					vLT = m_vCenter + vLT * GetvScaleView();
					vRB = m_vCenter + vRB * GetvScaleView();
					if( SPROBJ->IsPause() ) {
						XE::DrawRectangle( vLT, vRB, XCOLOR_DARKGRAY );
					}
				}
				// 그리기전에 이전에 그렸던 스프라이트와 다른스프라이트면 이전것의 D2D1Bitmap객체를 지워준다.
				// frameMove, Draw
				auto& spSelLayer = TOOL->GetspSelLayer();
				XSPLayerMove spLayerMove;
				if( spSelLayer && spSelLayer->IsTypeLayerMove() ) 
					spLayerMove = std::static_pointer_cast<XLayerMove>( spSelLayer );
				XSPAction spAction = SPROBJ->GetspAction();
				if( spAction ) {
					// 선택된 레이어의 이동키 위치를 보여주기
// 					if( spSelLayer ) {
// 						if( spSelLayer->IsTypeLayerMove() ) {
					if( spLayerMove ) {
							//SPLayerMove spLayerMove = SafeCast<XLayerMove*, SPBaseLayer>( TOOL->GetspSelLayer() );
						if( SPROBJ->IsPause() ) {
							DrawPathLayer( spAction, SPR::xLINE_SPLINE_CATMULLROM, spLayerMove, XCOLOR_LIGHTGRAY );
						}
					}
					// FrameMove, Draw
					XE::VEC2 vScale = spAction->GetvScale();
					XE::VEC3 dRotAct = spAction->GetvRotate();
					SPROBJ->SetScale( 1.0f, 1.0f );
					SPROBJ->SetRotate( 0, 0, 0 );
					SPROBJ->FrameMove( dt );
					SPROBJ->SetRotate( dRotAct );
					SPROBJ->SetScale( vScale );
					D3DXMATRIX mCamera;
					D3DXMATRIX mTrans, mScale;
					D3DXMatrixTranslation( &mTrans, fCenterX, fCenterY, 0 );
					D3DXMatrixScaling( &mScale, GetvScaleView().x, GetvScaleView().y, 1.0f );
					mCamera = mScale * mTrans;
// 					// 현재 선택된 레이어의 현재프레임의 포지션키의 랜덤영역 그리기
// 					if( spSelLayer ) {
// 						auto pKeyPos = spAction->FindKeyByidLayer<XKeyPos>( spSelLayer->GetidLayer()
// 							, SPROBJ->GetFrmCurr() );
// 						if( pKeyPos && pKeyPos->IsRandomFactor() ) {
// 							if( pKeyPos->GetRadius() != 0 ) {
// 								// 원으로
// 								auto vPos = pKeyPos->GetvPosOrig() * GetvScaleView();
// 								auto radius = pKeyPos->GetRadius() * GetvScaleView().x;
// 								GRAPHICS->DrawCircle( m_vCenter + vPos
// 																		, radius
// 																		, XCOLOR_ORANGE );
// 							}
// 							else {
// 								// 사각형으로
// 								auto rectArea = pKeyPos->GetRectRandom();
// 								rectArea = rectArea * GetvScaleView();
// 								GRAPHICS->DrawRect( m_vCenter + rectArea.vLT
// 																	, m_vCenter + rectArea.vRB, XCOLOR_ORANGE );
// 							}
// 
// 						}
// 					}
					// 스프라이트 객체 draw==============================================
					SPROBJ->Draw( mCamera );
					// draw bound box
					D3DXVECTOR2 vbbLT = spAction->GetBoundBoxLT();
					D3DXVECTOR2 vbbRB = spAction->GetBoundBoxRB();
					D3DXVECTOR2 vtLT, vtRB;
					D3DXVec2TransformCoord( &vtLT, &vbbLT, &mCamera );
					D3DXVec2TransformCoord( &vtRB, &vbbRB, &mCamera );
					XE::VEC2 xvLT( vtLT );
					XE::VEC2 xvRB( vtRB );
					// 액션 바운딩 박스
					if( SPROBJ->IsPause() ) {
						GRAPHICS->DrawRect( xvLT, xvRB, XCOLOR_BLUE );
					}
					if( SPROBJ->IsPlaying() )
						GetFrameView()->Update();
				}
				if( SPROBJ->IsPause() ) {
					XDragSelect::Draw();
				}
				// draw fps
				{
					CString str;
					str.Format( _T("fps:%d"), CalcFPS() );
					if( GetFrameView() ) {
						CString str2;
						str2.Format(_T(" yOfs=%d"), GetFrameView()->GetofsY() );
						str += str2;
					}
					SE::g_pFont->DrawString( 0, 0, (LPCTSTR)str );
				}
				// draw drag & drop 
				if( TOOL->m_DragDrop.GetDrag() ) {
					TOOL->m_DragDrop.GetSpr()->SetScale( 1.0f, 1.0f );
					TOOL->m_DragDrop.GetSpr()->DrawNoAdjust( m_vMouse.x, m_vMouse.y );
				}
				// draw rotate axis and angle
				// pause상태일때만 인터페이스를 그림
				if( SPROBJ->IsPause() ) {
					// 레이어가 선택되어있을때만 축을 그린다
					if( spLayerMove ) {
						if( spLayerMove->IsTypeLayerImage() 
							|| spLayerMove->IsTypeLayerObj() ) { // 더미레이어는 그리지 않음
							// 회전/스케일링 축 갱신 및 그리기
							if( m_pMICross->GetbActive() ) {
								m_pMICross->SetPos( spLayerMove->GetvAdjustAxis() );		// 파일에서 로드되었을때는 레이어에 값이 있기때문에 레이어값을 인터페이스로 옮겨야 한다
								m_pMICross->SetvScale( GetvScaleView() );
								m_pMICross->Draw( fCenterX, fCenterY, spLayerMove->Getx(), spLayerMove->Gety(), m_vMouse );		// 회전축 십자선그림
							}
							if( m_nDragRotate ) {	// 마우스로 잡고 회전중일때 각도 인터페이스 보여줌
								XKeyRot *pRotKey = nullptr;
								if( m_pNewAddKey && m_pNewAddKey->GetSubType() == xSpr::xKTS_ROT )
										pRotKey = dynamic_cast<XKeyRot *>( m_pNewAddKey );
								if( pRotKey ) {		// 회전모드에서 돌리고 있던 회전키가 있다면 인터페이스를 표시함
									CString str;
									float fAngleZ = ROUND_OFF( pRotKey->GetdAngleOrig() );
									str.Format( _T("%+3.0f˚"), fAngleZ );		// 돌아간 각도를 보여줌
									SE::g_pFont->DrawString( m_vMouse.x, m_vMouse.y - 16, str );
								}
							} // if dragRotate
							else 
							if( m_nDragScale ) {
								XKeyScale *pScaleKey = nullptr;
								if( m_pNewAddKey && m_pNewAddKey->GetSubType() == xSpr::xKTS_SCALE )
									pScaleKey = dynamic_cast<XKeyScale *>( m_pNewAddKey );
								if( pScaleKey ) {		// 확축모드에서 드래그하고 있던 키가 있다면 스케일값 표시함
									CString str;
									str.Format( _T("X:%+3.1f Y:%+3.1f"), pScaleKey->GetvScaleOrig().x, pScaleKey->GetvScaleOrig().y );		// 스케일값 표시
									SE::g_pFont->DrawString( m_vMouse.x, m_vMouse.y - 16, str );
								}
							} else // if dragScale
							if( 0/*m_nDragPosKeyGenerate*/ )	{ // pos키 자동생성모드시 라인그리기
							} else {
								if( m_pDragMI == m_pMICross ) {	// 회전축 인터페이스 드래그 중인가. 이거 나중에 일반화시켜 다시 만들어야 함
									CString str;
									str.Format( _T("%+3.1f, %+3.1f"), m_pDragMI->GetPosL().x, m_pDragMI->GetPosL().y );		// 인터페이스의 좌표를 표시
									SE::g_pFont->DrawString( m_vMouse.x, m_vMouse.y - 16, str );
								}
							}
						} // if em_rotate
					} // spLayerMove
					// draw event
					if( spAction ) {
//						auto spLayer = spAction->FindLayer( xSpr::xLT_EVENT, 0 );		
						auto spLayer = spActObj->GetspLayerByIndex<XLayerEvent>( /*xSpr::xLT_EVENT, */0 );
//						auto spLayer = spActObj->GetspLayerByNumber<XLayerEvent>( 0 );
						if( spLayer ) {		// 이벤트레이어가 있으면 십자선인터페이스를 그린다
							XKeyEvent *pEventKey = (XKeyEvent *)spAction->FindKey( spLayer, xSpr::xKT_EVENT, xSpr::xKTS_MAIN, SPROBJ->GetFrmCurr() );;	// 현재프레임에 이미 이벤트키를 얻음
							if( pEventKey ) {
								// 이제 타입에 관계없이 모두 십자선 인터페이스를 갖는다.
								m_pMIEventCreateObj->SetbActive( TRUE );
								m_pMIEventCreateObj->SetPos( pEventKey->Getlx(), pEventKey->Getly() );
								m_pMIEventCreateObj->SetfAngle( pEventKey->GetAngle() );
								m_pMIEventCreateObj->SetvScale( GetvScaleView() );
								m_pMIEventCreateObj->Draw( fCenterX, fCenterY, 0, 0, m_vMouse );
								if( m_pDragMI == m_pMIEventCreateObj ) {
									CString str;
									if( m_pMIEventCreateObj->GetbHighlightDir() ) {
										float fAngle = ROUND_OFF( m_pMIEventCreateObj->GetfAngle() );
										str.Format( _T("%+3.0f˚"), fAngle );		// 돌아간 각도를 보여줌
										SE::g_pFont->DrawString( m_vMouse.x, m_vMouse.y - 16, str );
									} else {
										str.Format( _T("%+3.1f, %+3.1f"), m_pDragMI->GetPosL().x, m_pDragMI->GetPosL().y );		// 인터페이스의 좌표를 표시
										SE::g_pFont->DrawString( m_vMouse.x, m_vMouse.y - 16, str );
									}
								}
							} else
								m_pMIEventCreateObj->SetbActive( FALSE );
						}
					} // if spAction
				} // if pause
				// HUD에 텍스트 표시
				{
					// 마우스 좌표 표시
					XE::VEC2 vCood = GetMouseCoord();
					CString str;
					if( m_vDragDist.IsZero() )
						str.Format( _T("XY:%d,%d"), (int)vCood.x, (int)vCood.y );
					else
						str.Format( _T("XY:%d,%d(%d,%d)"), (int)vCood.x, (int)vCood.y, (int)m_vDragDist.x, (int)m_vDragDist.y );
//					SE::g_pFont->DrawString( (float)(rect.right - (str.GetLength() * GRAPHICS_D3DTOOL->GetnFontSize()) + 16), 2, str );
					SE::g_pFont->DrawString( fCenterX + 5, 2.f, str );
					// 확축배율 표시
					str.Format( _T("%d%%"), (int)(GetvScaleView().x * 100.f) );
					SE::g_pFont->DrawString( (float)(rect.right-40), 2.f, str );
				}
				if( TOOL->GetbSlowDrag() )
					SE::g_pFont->DrawString( m_vMouse + XE::VEC2( 10, 0 ), _T("slow") );
				D3DDEVICE->EndScene();
			} // beginScene
			GetChain()->Present( nullptr, nullptr, GetSafeHwnd(), nullptr, 0 );
			pBackBuffer->Release();
		}
	}
//	extern float s_r;
//	s_r += 0.01f;;
}
// spAction내에서 선택된레이어 spLayer에 속하는 Pos키를 패스로 그려준다
void CAnimationView::DrawPathLayer( XSPAction spAction, SPR::xtLINE lineType, XSPLayerMove spLayer, XCOLOR col )
{
	if( spLayer == nullptr ) return;
	if( spAction == nullptr )	return;
	// 
	spAction->GetNextKeyClear();
	while( XBaseKey* pKey = spAction->GetNextKey() ) {
		// 주어진레이어에 속해있고 pos키인것.
// 		if( pKey->GetLayerType() == spLayer->GetType() && 
// 			pKey->GetnLayer() == spLayer->GetnLayer() && 
// 			pKey->GetSubType() == xSpr::xKTS_POS )
//		if( spLayer->IsSameLayer( pKey->GetspLayer() ) 
		if( spLayer->IsSameLayer( pKey )
				&& pKey->GetSubType() == xSpr::xKTS_POS ) {
			XKeyPos *pPosKey = SafeCast<XKeyPos*>( pKey );
			XBREAK( pPosKey == nullptr );
			// 보간키고 패스타입이 있을때만 선을 그림
// 			if( pPosKey->GetInterpolation() && pPosKey->GetPathType() )	{
// 				XCOLOR colPath = pPosKey->GetcolPath();
// 				XE::VEC2 v1 = pPosKey->GetPos();
// 				XE::VEC2 v2 = ( pPosKey->GetpNextPathKey() )? pPosKey->GetpNextPathKey()->GetPos() : pPosKey->GetPos();
// 				switch( pPosKey->GetPathType() ) {
// 				case SPR::xLINE_SPLINE_CATMULLROM: {
// 					XE::VEC2 v0 = ( pPosKey->GetpPrevPathKey() )? pPosKey->GetpPrevPathKey()->GetPos() : pPosKey->GetPos();
// 					XE::VEC2 v3;
// 					if( pPosKey->GetpNextPathKey() ) {
// 						if( pPosKey->GetpNextPathKey()->GetpNextPathKey( pPosKey->GetidPath() ) )
// 							v3 = pPosKey->GetpNextPathKey()->GetpNextPathKey( pPosKey->GetidPath() )->GetPos();
// 						else
// 							v3 = pPosKey->GetpNextPathKey()->GetPos();
// 					} else
// 						v3 = pPosKey->GetPos();
// 					XE::VEC2 vv0, vv1, vv2, vv3;
// 					vv0 = PosToViewCoord( v0 );
// 					vv1 = PosToViewCoord( v1 );
// 					vv2 = PosToViewCoord( v2 );
// 					vv3 = PosToViewCoord( v3 );
// 					GRAPHICS->DrawSplineCatmullRom( vv0, vv1, vv2, vv3, 10, colPath );		// v1~v2를 잇는구간을 스플라인으로 그려준다
// 				} break;
// 				case SPR::xLINE_LINE: {
// 					XE::VEC2 vv1, vv2;
// 					vv1 = PosToViewCoord( v1 );
// 					vv2 = PosToViewCoord( v2 );
// 					GRAPHICS->DrawLine( vv1, vv2, colPath );
// 				} break;
// 				case SPR::xLINE_NONE:
// 					break;
// 				default:
// 					XBREAK(1);
// 				}
// 			}
			// 각 이동키들의 위치를 사각형표시
			XE::VEC2 vPos;
			vPos = PosToViewCoord( pPosKey->GetPos() );
			if( GetlistSelectObj().Find( pPosKey->GetidObj() ) )		// 선택된 키들은 빨간색으로...
				XE::DrawRectangle( vPos - XE::VEC2( 3.f, 3.f ), 
										vPos + XE::VEC2( 3.f, 3.f ), XCOLOR_RED );
			else
				XE::DrawRectangle( vPos - XE::VEC2( 3.f, 3.f ), 
										vPos + XE::VEC2( 3.f, 3.f ), XCOLOR_LIGHTGRAY );
		} //xSpr::xKTS_POS
	} // while key loop
}

void CAnimationView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( nChar == VK_ESCAPE )	{
//		m_nDragPosKeyGenerate = 0;
		m_bAlt = 0;
	}
	//
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAnimationView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN ) {
		XE::VEC2 vDist;
		switch( nChar ) {
		case VK_LEFT:	vDist.x = -1.0f;	break;
		case VK_RIGHT:	vDist.x = 1.0f;	break;
		case VK_UP:		vDist.y = -1.0f;	break;
		case VK_DOWN:	vDist.y = 1.0f;	break;
		}
		auto& spLayerMove = TOOL->GetspSelLayerMove();
		if( spLayerMove ) {
			m_pNewAddKey = MoveLayer( SPROBJ->GetspActObjCurr(), spLayerMove, vDist, TRUE );
			if( m_pNewAddKey && m_pLastUndoKeyChange )
				m_pLastUndoKeyChange->SetKeyAfter( m_pNewAddKey );		// 드래그하고 난 후의 키를 복사해서 언두에 추가한다
			else {
				XBREAK( 1 );
			}
		}
	} 	
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}

XKeyPos* CAnimationView::MoveLayer( XSPActObj spActObj, XSPLayerMove spLayerMove, const XE::VEC2& vDist, BOOL bCreateUndo )
{
	if( spActObj == nullptr )	return nullptr;
	if( spLayerMove == nullptr )		return nullptr;
	auto spAction = spActObj->GetspAction();
// 	if( spLayer->IsTypeLayerMove() == FALSE || spLayer->IsTypeLayerDummy() )
	XASSERT( spLayerMove->IsTypeLayerMove() );
	if( spLayerMove->IsTypeLayerDummy() )	// 더미레이어는 해당없음.
		return nullptr;
//	SPLayerMove spMoveLayer = SafeCast<XLayerMove*, SPBaseLayer>( spLayer );
// 	if( XBREAK( pMoveLayer == nullptr ) )		
// 		return nullptr;
	BOOL bCreateKey = FALSE;
// 	XKeyPos *pPosKey;
// 	spAction->FindKey( spLayerMove, SPROBJ->GetFrmCurr(), &pPosKey );		// 현재 프레임/레이어에 위치이동 키가 없는가
	auto pPosKey = spAction->FindKeyByidLayer<XKeyPos>( spLayerMove->GetidLayer(), SPROBJ->GetFrmCurr() );
	if( pPosKey == nullptr ) {
		float x = spLayerMove->Getx();
		float y = spLayerMove->Gety();
		pPosKey = spAction->AddKeyPos( SPROBJ->GetFrmCurr(), spLayerMove, x, y );	// 없으면 새로만든다
		bCreateKey = TRUE;		// 키가 새로 생성되었음
// 		if( SPROBJ->GetFrmCurr() != 0 ) {		// 0프레임이 아닌곳에 키가 추가되면
// 			auto pPosKeyZero = spAction->FindKeyByidLayer<XKeyPos>( spLayerMove->GetidLayer(), 0 );
// 			if( pPosKeyZero == nullptr ) {		// 없으면
// 				pPosKeyZero = spAction->AddKeyPos( 0, spLayerMove, 0, 0 );			// 0프레임에 자동으로 넣어준다
// 				if( bCreateUndo )
// 					UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pPosKeyZero ), 1 );		// 추가모드로 역행동 푸쉬
// 			}
// 		}
	}
	// 드래그 시작한 최초시점에 언두스택에 역행동을 넣음
	if( bCreateUndo )
	{
		if( bCreateKey )
			UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pPosKey ) );		// pPosKey키를 생성/삭제하는 언두행동을 언두스택에 푸쉬
		m_pLastUndoKeyChange = new XUndoKeyChange( SPROBJ, spActObj, pPosKey );
		UNDOMNG->PushUndo( m_pLastUndoKeyChange, bCreateKey );	// 현재 pPosKey의 모든 변환값을 저장했다가 언두가 일어나면 그값으로 되돌림
	}
	pPosKey->AddPos( vDist.x, vDist.y );
	//						pPosKey->Execute( SPROBJ );
	SPROBJ->MoveFrame( pPosKey->GetfFrame() );
	return pPosKey;
}
/**
 @brief 
*/
void CAnimationView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_vMouse = point;
	m_vDragDist.Set( 0, 0 );		// 버튼을 떼면 다시 초기화
	m_vDragStart.Set( 0, 0 );
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) {
		CFormView::OnLButtonUp(nFlags, point);
		return;
	}
	// 키셀렉트 시스템으로 전달
	XDragSelect::OnLButtonUp( nFlags, m_vMouse );
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 드래그앤드롭중에 이곳에서 드롭되었다면
	if( m_nDragLayer )
	{
		if( m_nDragLayer == 2 )		// 드래그까지 해야 언두행동이 생긴다
			m_pLastUndoKeyChange->SetKeyAfter( m_pNewAddKey );		// 드래그하고 난 후의 키를 복사해서 언두에 추가한다
		m_nDragLayer = 0;
	}
	XSPBaseLayer spLayer = TOOL->GetspSelLayer();
	if( spLayer ) {
		if( 0/*m_nDragPosKeyGenerate*/ ) {		// pos키 자동생성 모드
// 			if( spLayer->IsTypeLayerMove() ) {
// //				SPLayerMove spLayerMove = SafeCast<XLayerMove*, SPBaseLayer>( spLayer );
// 				auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );
// 				if( XASSERT( spLayerMove ) ) {
// 					float dist = (m_vPosKeyGenerate - spLayerMove->GetPos()).Length();	// 레이어좌표와 마우스좌표의 거리
// 					float numFrame = dist / m_speedPosKeyGenerate;			// 프레임수 = 거리 / 속도 
// 					float frameNewKey = SPROBJ->GetFrmCurr() + numFrame * spAction->GetSpeed();		// 새키의 위치 = 현재 프레임위치 + 프레임개수 * 애니메이션스피드
// 					// 현재 프레임위치에 키가 있는지 확인
// // 					XKeyPos *pPosKey = nullptr;
// // 					spAction->FindKey( spLayer, SPROBJ->GetFrmCurr(), &pPosKey );
// 					auto pPosKey = spAction->FindKeyPos( spLayerMove, SPROBJ->GetFrmCurr() );
// 					if( pPosKey == nullptr ) {		// 키가 없으면 키 생성
// 						pPosKey = spAction->AddKeyPos( SPROBJ->GetFrmCurr(), spLayerMove, spLayerMove->GetPos() );
// 						UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pPosKey ) );		// 키생성 언두
// 					}
// 					// 추가될 새 키 위치에 키 생성
// //					spAction->FindKey( spLayer, frameNewKey, &pPosKey );
// 					pPosKey = spAction->FindKeyPos( spLayerMove, frameNewKey );
// 					if( pPosKey == nullptr ) {
// 						pPosKey = spAction->AddKeyPos( frameNewKey, spLayerMove, m_vPosKeyGenerate );
// 						UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pPosKey ) );		// 키생성 언두
// 					}
// 					// 새로추가된 위치가 maxFrame보다 큰위치면 맥스프레임수를 늘임
// 					if( frameNewKey >= spAction->GetfMaxFrame() )
// 						spAction->SetfMaxFrame( ROUND_UP( frameNewKey + 1.0f ) );		// 새위치를 반올림해서 maxFrame으로 씀
// 					// 새로 추가된 키위치로 프레임이동,
// 					GetFrameView()->MoveFrame( frameNewKey );
// 					GetFrameView()->Update();
// 				}
// 			}
		} else
		if( m_nDragRotate ) {
			XKeyRot *pRotKey = nullptr;
			if( m_pNewAddKey && m_pNewAddKey->GetSubType() == xSpr::xKTS_ROT )
				pRotKey = dynamic_cast<XKeyRot *>( m_pNewAddKey );
			if( pRotKey )
				pRotKey->SetfAngleZ( ROUND_OFF( pRotKey->GetdAngleOrig() ) ); 		// 손을뗄때 소숫점각도는 반올림시켜서 재저장한다
			if( m_nDragRotate == 2 )
				m_pLastUndoKeyChange->SetKeyAfter( m_pNewAddKey );
			m_nDragRotate = 0;
		} else 
		if( m_nDragScale ) {
			if( m_nDragScale == 2 )
				m_pLastUndoKeyChange->SetKeyAfter( m_pNewAddKey );
			m_nDragScale = 0;
		}
	}
	m_nDragScale = 0;
	if( m_pDragMI ) {
		m_pDragMI->OnLButtonUp( 0, 0, point );
		m_pDragMI = nullptr;
	}

	// thumbview로부터 드래그한 스프라이트를 키로 추가함
	if( SPROBJ->GetPause() ) {		// 애니메이션이 멈춰있을때만 처리
		if( TOOL->m_DragDrop.GetDrag() ) {
			if( nFlags & MK_SHIFT )	{ // 쉬프트 누르고 스프라이트 드래그&드롭 하면 자동으로 새 이미지 레이어가 생긴다
//				auto spLayer = spAction->AddImgLayer();
				auto spLayer = spActObj->AddLayerAuto<XLayerImage>();
				TOOL->SetspSelLayer( spLayer );
				SPROBJ->MoveFrame(0);
				if( TOOL->AddKeyFromSelSpr( spLayer ) )
					SPROBJ->MoveFrame();
				UNDOMNG->PushUndo( new XUndoCreateLayer( SPROBJ, spLayer, _T("레이어 생성") ) );
			} else {
				if( spLayer && spLayer->IsTypeLayerImage() ) {
					auto spLayerImage = std::static_pointer_cast<XLayerImage>( spLayer );
					if( TOOL->AddKeyFromSelSpr( spLayerImage ) )
						SPROBJ->MoveFrame();
				} else
						XALERT( "Image key를 추가할 Image Layer를 먼저 선택해야 합니다" );
			}	
			TOOL->m_DragDrop.SetDrag( FALSE );					// 드래그앤드롭 모드를 해제시킨다.
		}
	} // if pause
	GetFrameView()->Update();
	Invalidate( FALSE );

	CFormView::OnLButtonUp(nFlags, point);
}

static CPoint s_prevPoint;
/**
 @brief 여기 코드좀 정리
*/
void CAnimationView::OnMouseMove(UINT nFlags, CPoint point)
{
	if( GetFocus() != this ) {
		SetFocus();
		ALL_VIEW_UPDATE();
	}
	m_vMouse = point;
	static auto s_vPrev = m_vMouse;
	auto vDist = m_vMouse - s_vPrev;
	if( nFlags & MK_LBUTTON ) {
		XE::VEC2 vPoint = point;
		m_vDragDist = vPoint - m_vDragStart;		// 드래그로 움직인 거리
	}
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( spActObj ) {
		XSPAction spAction = SPROBJ->GetspAction();
		XE::VEC2 vDist;
		CPoint pt = point - s_prevPoint;
		vDist.x = (float)pt.x;
		vDist.y = (float)pt.y;
		vDist /= GetvScaleView();	// 뷰확축에 따라 움직인 거리를 조정.
		if( TOOL->GetbSlowDrag() )
			vDist *= 0.25f;
		RECT rect;
		GetClientRect( &rect );
		const float fCenterX = m_vCenter.x;			// 뷰 전체의 중심점
		const float fCenterY = m_vCenter.y;
		if( SPROBJ->GetPause() == TRUE ) {		// 애니메이션이 멈춰있을때만 처리
			if( m_bModeBackBg && nFlags & MK_LBUTTON ) {
				OnMouseMoveByBackBg( m_vMouse );
			} else {
				// 앵커 드래그, 
				if( TOOL->m_DragDrop.GetDrag() == FALSE ) {			// 썸네일창으로부터 드래그앤드롭상황이 아닐때
					// 키셀렉트 시스템으로 전달
					if( XDragSelect::OnMouseMove( nFlags, m_vMouse ) ) {	// 안에서 오브젝트 드래그같은거 하면 {} 안으로 진입하지 않는다
						if( m_pDragMI ) 
							m_pDragMI->DragMove( fCenterX, fCenterY, point, vDist );		// 드래그했을때 처리
						// 드래그중인 인터페이스가 이벤트:오브젝생성 인터페이스면...
						if( m_pDragMI == m_pMIEventCreateObj ) {
//							auto spLayer = spActObj->GetspLayerByNumber<XLayerEvent>( 0 );		
							auto spLayer = spActObj->GetspLayerByIndex<XLayerEvent>( /*xSpr::xLT_EVENT, */0 );
							if( spLayer ) {
								auto pEventKey = (XKeyEvent *)spAction->FindKey( spLayer, xSpr::xKT_EVENT, xSpr::xKTS_MAIN, SPROBJ->GetFrmCurr() );;	// 현재프레임에 이미 이벤트키를 얻음
								if( pEventKey ) {
									pEventKey->SetPos( m_pDragMI->GetPosL().x, m_pDragMI->GetPosL().y );
									pEventKey->SetAngle( m_pMIEventCreateObj->GetfAngle() );
								}
							}
						}
						auto spLayerMove = TOOL->GetspSelLayerMove();
						if( spLayerMove && !spLayerMove->IsTypeLayerDummy() ) {		
							// 변환축 드래그중
							if( m_pDragMI == m_pMICross ) {
								TOOL->AdjustAxisLock( &vDist.x, &vDist.y );
								spLayerMove->AddAdjustAxis( vDist.x, vDist.y );
							} else
							// PosKey 자동생성 드래그 모드
							if( 0 /*m_nDragPosKeyGenerate*/ ) {
// 								XE::VEC2 vMouse = m_vMouse;
// 								if( TOOL->GetAxisLock() == AL_X )
// 									vMouse.y = spLayerMove->GetPos().y;
// 								else if( TOOL->GetAxisLock() == AL_Y )
// 									vMouse.x = spLayerMove->GetPos().x;
// 								auto vPos = spLayerMove->GetPos();
// 								m_vPosKeyGenerate = vPos + (vMouse - PosToViewCoord( vPos )) / GetvScaleView();
							} else
							// 레이어(이미지,오브젝트,이벤트등)를 드래그했을때
							if( m_nDragLayer ) {
								TOOL->AdjustAxisLock( &vDist.x, &vDist.y );		// 드래그시 한방향으로만 움직일 수 있는...
								auto pPosKey = MoveLayer( spActObj, spLayerMove, vDist, (m_nDragLayer==1)? TRUE : FALSE );
								m_nDragLayer = 2;		// 드래그 상태로 전환
								m_pNewAddKey = pPosKey;		// Draw나 ButtonUp에서 쓰기 위함
							} // if m_nDragLayer
							// 레이어 회전모드
							else if( m_nDragRotate ) {		
								BOOL bCreateKey = FALSE;
								auto vAxis = m_vCenter + spLayerMove->GetPos() + spLayerMove->GetvAdjustAxis();
								auto vDist = m_vMouse - vAxis;
								float fAngleFromAxis = D3DXToDegree( atan2( vDist.y, vDist.x ) );  // 회전축으로부터 현재커서의 각도
								float fDistAngleLocal = XE::CalcDistAngle( m_fPrevAngle, fAngleFromAxis );		// m_fPrevAngle로부터 fAngleFromAxis의 상대각도(-180~+180의 범위가 된다)
//								auto pRotKey = spAction->FindKeyRot( spLayerMove, SPROBJ->GetFrmCurr() );
								auto pRotKey = spAction->FindKeyByidLayer<XKeyRot>( spLayerMove->GetidLayer(), SPROBJ->GetFrmCurr() );
								if( pRotKey == nullptr ) {
									bCreateKey = TRUE;
									float fAngleZ = spLayerMove->GetcnRot().fAngle;		// 레이어의 현재각도를 기본값으로 키를 만든다
									pRotKey = spAction->AddKeyRot( SPROBJ->GetFrmCurr(), spLayerMove, fAngleZ );
									if( m_nDragRotate == 1 )
										UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pRotKey ) );
// 									if( SPROBJ->GetFrmCurr() != 0 )	{	// 0프레임이 아닌곳에 키가 추가되면
// 										auto pRotKeyZero = spAction->FindKeyByidLayer<XKeyRot>( spLayerMove->GetidLayer(), SPROBJ->GetFrmCurr() );
// 										if( pRotKeyZero == nullptr ) {	// 없으면
// 											pRotKeyZero = spAction->AddKeyRot( 0, spLayerMove, 0 );			// 각도 0짜리 회전키를 0프레임에 자동으로 넣어준다
// 											if( m_nDragRotate == 1 )
// 												UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pRotKeyZero ), 1 );		// 추가모드로 역행동 푸쉬
// 										}
// 									}
								}
								if( m_nDragRotate == 1 ) {
									m_pLastUndoKeyChange = new XUndoKeyChange( SPROBJ, spActObj, pRotKey );
									UNDOMNG->PushUndo( m_pLastUndoKeyChange, bCreateKey );		// 키 좌표가 변하기전 언두행동을 푸쉬
								}
								m_nDragRotate = 2;
								pRotKey->AddAngleZ( fDistAngleLocal );			// 마우스가 움직인 만큼의 각도를 키각도에 누적시킨다. 2바퀴를 돌렸으면 720도가 된다
//								pRotKey->Execute( SPROBJ );
								pRotKey->Execute( spActObj, spLayerMove );
								m_pNewAddKey = pRotKey;		// Draw나 ButtonUp에서 사용하기 위해 
								m_fPrevAngle = fAngleFromAxis;
							} // if m_nDragRotate
							else if( m_nDragScale ) {
								BOOL bCreateKey = FALSE;
								auto vAxis = m_vCenter + spLayerMove->GetPos() + spLayerMove->GetvAdjustAxis();
								auto vDist = m_vMouse - vAxis;
	// 							float xAxis = fCenterX + pMoveLayer->Getx() + pMoveLayer->GetfAdjustAxisX();		// 선택된 레이어의 회전/스케일축
	// 							float yAxis = fCenterY + pMoveLayer->Gety() + pMoveLayer->GetfAdjustAxisY();
	// 							float xDist = point.x - xAxis;
	// 							float yDist = point.y - yAxis;
								float fDistFromAxis = D3DXVec2Length( &D3DXVECTOR2( vDist.x, vDist.y ) );	// 회전/스케일축으로부터 거리
								float fDistScaleX, fDistScaleY;
								fDistScaleX = fDistScaleY = fDistFromAxis / m_fPrevDistFromAxis;		// 기존길이로부터 몇%나 더 길어졌는가
	// 							XKeyScale *pScaleKey =  nullptr;
	// 							spAction->FindKey( spLayerMove, SPROBJ->GetFrmCurr(), &pScaleKey );		// 현재 프레임/레이어에 키가 있는가
//								auto pScaleKey = spAction->FindKeyScale( spLayerMove, SPROBJ->GetFrmCurr() );
								auto pScaleKey = spAction->FindKeyByidLayer<XKeyScale>( spLayerMove->GetidLayer(), SPROBJ->GetFrmCurr() );
								if( pScaleKey == nullptr ) {
									float fScaleX = spLayerMove->GetcnScale().vScale.x;		// 레이어의 현재값을 기본값으로 키를 만든다
									float fScaleY = spLayerMove->GetcnScale().vScale.y;
									pScaleKey = spAction->AddKeyScale( SPROBJ->GetFrmCurr(), spLayerMove, fScaleX, fScaleY );
									if( m_nDragScale == 1 )
										UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pScaleKey ) );
// 									if( SPROBJ->GetFrmCurr() != 0 ) {		// 0프레임이 아닌곳에 키가 추가되면
// 										auto pScaleKeyZero = spAction->FindKeyByidLayer<XKeyScale>( spLayerMove->GetidLayer(), 0 );
// 										if( pScaleKeyZero == nullptr ) {		// 없으면
// 											pScaleKeyZero = spAction->AddKeyScale( 0, spLayerMove, 1.0f, 1.0f );			// 1.0f짜리 스케일키를 0프레임에 자동으로 넣어준다
// 											if( m_nDragScale == 1 )
// 												UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pScaleKeyZero ) );
// 										}
// 									}
									bCreateKey = TRUE;
								}
								if( m_nDragScale == 1 ) {
									m_pLastUndoKeyChange = new XUndoKeyChange( SPROBJ, spActObj, pScaleKey );
									UNDOMNG->PushUndo( m_pLastUndoKeyChange, bCreateKey );		// 키 좌표가 변하기전 언두행동을 푸쉬
								}
								m_nDragScale = 2;

								if( TOOL->GetAxisLock() == AL_X )		// x축으로만 이동하는 모드
									fDistScaleY = 1.0f;
								else if( TOOL->GetAxisLock() == AL_Y )
									fDistScaleX =1.0f;
								pScaleKey->MulScale( fDistScaleX, fDistScaleY );		// 기존값부터 얼마나 더 커졌는지를 곱한다
								pScaleKey->Execute( spActObj, spLayerMove );
								m_pNewAddKey = pScaleKey;
								m_fPrevDistFromAxis = fDistFromAxis;
							} // m_nDragScale
							// 회전축 마우스 오버 체크
							if( m_nDragLayer == 0 && m_nDragRotate == 0 && m_nDragScale == 0 )		// 다른드래그 하고 있을땐 처리하지 않는다
								if( m_pMICross->GetbActive() )
									m_pMICross->OnMouseMove( point.x - (fCenterX + spLayerMove->Getx()), point.y - (fCenterY + spLayerMove->Gety()) );		//
						} // spLayer
						// 마우스오버시 하이라이트 처리
						if( m_pDragMI == nullptr ) {		// 어떤인터페이스를 드래그하고 있다면 오버처리 할필요 없다
							MI_LOOP( itor, pmi ) {
								XE::VEC2 vmi = pmi->GetPosT();
								pmi->OnMouseMove( point.x - (fCenterX + vmi.x), point.y - (fCenterY + vmi.y) );		//
							}
							END_LOOP;
							if( m_pMIEventCreateObj ) {
								XE::VEC2 vmi = m_pMIEventCreateObj->GetPosT();
								m_pMIEventCreateObj->OnMouseMove( point.x - (fCenterX + vmi.x), point.y - (fCenterY + vmi.y) );		//
							}
						}
					} // XDragSelect::OnMouseMove
				} // if GetDrag			}
			} // not bgmove
		} // if pause
	} // if spAction
	if( m_bPanningMode )
		m_vCenter += vDist;
	s_prevPoint = point;
	s_vPrev = m_vMouse;
	CFormView::OnMouseMove(nFlags, point);
}

/**
 @brief 배경이미지 이동만을 위한 마우스무브 처리
 @param vDrag 마우스로 긁은 거리
*/
void CAnimationView::OnMouseMoveByBackBg( const XE::VEC2& vMouse )
{
	auto vDist = vMouse - s_prevPoint;
	vDist / GetvScaleView();
	if( TOOL->GetbSlowDrag() )
		vDist *= 0.25f;
	m_vAdjBackBg += vDist;

}
//#include "XSoundMng.h"
void CAnimationView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_vMouse = point;
	m_vDragDist.Set( 0, 0 );	// 클릭하면  드래그 상대거리를 클리어
	m_vDragStart = point;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( SPROBJ->GetPause() == TRUE ) {		// 애니메이션이 멈춰있을때만 처리
		// 레이어들을 차례로 탐색하여 클릭한 그림이 있는지 검사하여 그 그림의레이어를 자동선택한다
		XSPAction spAction = SPROBJ->GetspAction();
		if( spAction )  {
			RECT rect;
			GetClientRect( &rect );
			float fCenterX = m_vCenter.x;			// 뷰 전체의 중심점
			float fCenterY = m_vCenter.y;
			// 십자선 기즈모의 클릭 처리
			m_pDragMI = nullptr;
			MI_LOOP( itor, pmi ) {
				if( pmi->IsHighlight() ) {			// mi인터페이스를 클릭했는가
					m_pDragMI = pmi;
					pmi->OnLButtonDown( fCenterX, fCenterY, point );
					break;		// 동시에 여러개의 인터페이스가 클릭되어선 안된다
				}
			} END_LOOP;
			//
			// 이미지 클릭 검사
			if( /*retv && */m_pDragMI == nullptr ) {		// 인터페이스 드래그 모드가 아닐때만 이벤트키 십자선 클릭 검사를 함
				if( m_pMIEventCreateObj && m_pMIEventCreateObj->IsHighlight() )	{	// event key 십자선을 클릭했는가
					m_pDragMI = m_pMIEventCreateObj;
					m_pDragMI->OnLButtonDown( fCenterX, fCenterY, point );
				} else {
					if( m_pMICross->IsHighlight() )	// MI가 하일라이트 되어있을때 클릭하면
						m_pDragMI = m_pMICross;	// 인터페이스 드래그 모드
				}
//			// 이미지 클릭 검사
// 				D3DXMATRIX mCamera, mTrans, mScale;
// 				D3DXMatrixTranslation( &mTrans, fCenterX, fCenterY, 0 );
// 				D3DXMatrixScaling( &mScale, GetvScaleView().x, GetvScaleView().y, 1.f );
// 				mCamera = mScale * mTrans;
//				auto spLayer = SPROBJ->GetLayerInPixel( mCamera, D3DXVECTOR2(m_vMouse.x, m_vMouse.y) );	// 클릭한 좌표에서 출력좌표를 빼서 상대좌표로 변환
				// 현재 선택된 레이어에서 드래그하도록 바뀜.
				auto spLayer = TOOL->GetspSelLayer();
				if( spLayer ) {
					// 클릭한 이미지레이어의 스프라이트를 선택상태로 만들어 adjust창에 보여준다
					if( spLayer->IsTypeLayerImage() ) {
						auto spLayerImage = std::static_pointer_cast<XLayerImage>( spLayer );
						XSprite *pSprCurr = spLayerImage->GetpSpriteCurr();
						if( pSprCurr ) {
							TOOL->GetSelSpr().Clear();
							TOOL->GetSelSpr().Add( pSprCurr );
							GetThumbView()->Update();
							GetAdjustView()->Update();
						}
					}
					// 변형가능한 레이어의 드래그 처리를 한다
//					TOOL->SetspSelLayer( spLayer );
					if( spLayer->IsTypeLayerMove() ) {
						auto spLayerMove = std::static_pointer_cast<XLayerMove>( spLayer );
// 						if( m_bAlt ) {
// 							// ALT누르고 이미지를 드래그하면 poskey가 자동으로 생성된다
// 							m_nDragPosKeyGenerate = 1;
// 							m_vPosKeyGenerate = spLayerMove->GetPos();
// 						} else
						{
							// 레이어의 클릭 처리
							auto vAxis = m_vCenter + spLayerMove->GetPos() + spLayerMove->GetvAdjustAxis();
							auto vDist = m_vMouse - vAxis;
							switch( TOOL->GetnEditMode() )
							{
							case EM_MOVE:		m_nDragLayer = 1;		break;
							case EM_ROTATE:		// 회전모드
								{
								m_nDragRotate = 1;		
								float fAngleFromAxis = D3DXToDegree( atan2( vDist.y, vDist.y ) );  // 회전축으로부터 현재커서의 각도
								m_fPrevAngle = fAngleFromAxis;
								// 클릭하는 순간에 키가 있었는지 검사
//								auto pRotKey = spAction->FindKeyRot( spLayerMove, SPROBJ->GetFrmCurr() );
								auto pRotKey = spAction->FindKeyByidLayer<XKeyRot>( spLayerMove->GetidLayer(), SPROBJ->GetFrmCurr() );
								m_pNewAddKey = pRotKey;
								}
								break;
							case EM_SCALE:
								{
								m_nDragScale = TRUE;
								float fDistFromAxis = D3DXVec2Length( &D3DXVECTOR2( vDist.x, vDist.y ) );	// 회전/스케일축으로부터 거리
								m_fPrevDistFromAxis = fDistFromAxis;
								// 클릭하는 순간에 키가 있었는지 검사
// 								XKeyScale *pScaleKey =  nullptr;
// 								spAction->FindKey( spLayer, SPROBJ->GetFrmCurr(), &pScaleKey );		// 현재 프레임/레이어에 키가 있는가
//								auto pScaleKey = spAction->FindKeyScale( spLayerMove, SPROBJ->GetFrmCurr() );
								auto pScaleKey = spAction->FindKeyByidLayer<XKeyScale>( spLayerMove->GetidLayer(), SPROBJ->GetFrmCurr() );
								m_pNewAddKey = pScaleKey;
								}
								break;
							} // switch
						}
					}
				} // spLayer
				else
				{
					// 이미지를 클릭하지 않았다면 드래그 모드
					XDragSelect::OnLButtonDown( nFlags, m_vMouse );	// 셀렉트키 검사
				}
			} // if m_pDragMI
			Invalidate( FALSE );
			GetFrameView()->Update();
		}
	}
	s_prevPoint = point;
	CFormView::OnLButtonDown(nFlags, point);
}

void CAnimationView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	Draw();
}

void CAnimationView::OnRButtonUp(UINT nFlags, CPoint point)
{
//	bool bAlt = ( nFlags & MK_ALT ) != 0;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_vRMouse = point;		// 우클릭했을때 마우스 위치
//	if( TOOL->GetspSelLayer() && TOOL->GetspSelLayer()->GetType() == XBaseLayer::xIMAGE_LAYER )		// 이미지 레이어가 선택되어 있을때만 
	if( SPROBJ->IsPlaying() == FALSE )	// 플레이중이 아닐때만 메뉴를 띄움
		if( !m_bAlt )
			PopupLayer( point );																												// 메뉴를 띄움
	m_bAlt = FALSE;		// OnSysKeyUp이 발생하지 않는 문제땜에 강제로 클리어 시킴.
	CFormView::OnRButtonUp(nFlags, point);
}

void CAnimationView::OnRButtonDown( UINT nFlags, CPoint point )
{
//	bool bAlt = ( nFlags & MK_ALT ) != 0;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_vRMouse = point;		// 우클릭했을때 마우스 위치
	//	if( TOOL->GetspSelLayer() && TOOL->GetspSelLayer()->GetType() == XBaseLayer::xIMAGE_LAYER )		// 이미지 레이어가 선택되어 있을때만 
	// 메뉴를 띄움
	if( SPROBJ->IsPlaying() == FALSE ) {	// 플레이중이 아닐때만 메뉴를 띄움
		if( m_bAlt ) {
			const float fCenterX = m_vCenter.x;			// 뷰 전체의 중심점
			const float fCenterY = m_vCenter.y;
			// 이미지 클릭 검사
			D3DXMATRIX mCamera, mTrans, mScale;
			D3DXMatrixTranslation( &mTrans, fCenterX, fCenterY, 0 );
			D3DXMatrixScaling( &mScale, GetvScaleView().x, GetvScaleView().y, 1.f );
			mCamera = mScale * mTrans;
			// 클릭한 좌표에서 출력좌표를 빼서 상대좌표로 변환
			auto spLayer 
				= SPROBJ->GetLayerInPixel( mCamera, D3DXVECTOR2(m_vMouse.x, m_vMouse.y) );	
			if( spLayer ) {
				// 클릭한 이미지레이어의 스프라이트를 선택상태로 만들어 adjust창에 보여준다
				if( spLayer->IsTypeLayerImage() ) {
					auto spLayerImage = std::static_pointer_cast<XLayerImage>( spLayer );
					XSprite *pSprCurr = spLayerImage->GetpSpriteCurr();
					if( pSprCurr ) {
						TOOL->GetSelSpr().Clear();
						TOOL->GetSelSpr().Add( pSprCurr );
						GetThumbView()->Update();
						GetAdjustView()->Update();
					}
					TOOL->SetspSelLayer( spLayer );
				}
				GetFrameView()->Update();
			} // if( spLayer ) {
		} // m_bAlt
	} 
	CFormView::OnRButtonDown( nFlags, point );
}

// 휠버튼 누르면 패닝
void CAnimationView::OnMButtonDown( UINT nFlags, CPoint point )
{
// 	const XE::VEC2 vMouse = point;
// 	static auto s_vPrev = vMouse;
// 	auto vDist = vMouse - s_vPrev;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SetCapture();
	m_bPanningMode = true;
	Invalidate( 0 );
// 	s_vPrev = vMouse;
	__super::OnMButtonDown( nFlags, point );
}

void CAnimationView::OnMButtonUp( UINT nFlags, CPoint point )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bPanningMode = false;
	ReleaseCapture();
	__super::OnMButtonUp( nFlags, point );
}

void CAnimationView::PopupLayer( CPoint point )
{
	CMenu		menu, *popup;
	POINT	p;
	
	menu.LoadMenu( IDR_POPUP_LAYER );
	popup = menu.GetSubMenu(0);
	
	GetCursorPos( &p );
	popup->TrackPopupMenu( TPM_CENTERALIGN | TPM_LEFTBUTTON, 
		p.x, p.y, AfxGetMainWnd() );
	
	menu.DestroyMenu();
}

// 이미지레이어 프로퍼티창 띄우기
void CAnimationView::OnDrawProperty()
{
	VALID_ACTOBJ( spActObj );
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	auto spLayerMove = TOOL->GetspSelLayerMove();
//	if( TOOL->GetspSelLayer() && TOOL->GetspSelLayer()->IsTypeLayerMove() )		
	if( spLayerMove ) {
//		SPBaseLayer spLayer = TOOL->GetspSelLayer();
		CDlgDrawProperty dlg;
		// 현재 위치에 Effect키를 얻고 없으면 만든다.
// 		XKeyEffect *pEffectKey = nullptr;
// 		spAction->FindKey( spLayerMove, SPROBJ->GetFrmCurr(), &pEffectKey );		// 현재 프레임/레이어에 이펙트키를 얻는다
		auto pEffectKey = spAction->FindKeyByidLayer<XKeyEffect>( spLayerMove->GetidLayer(), SPROBJ->GetFrmCurr() );
		if( pEffectKey )		// 키가 있으면 키 값을 디폴트로 다이얼로그를 띄운다
		{
			dlg.m_bFlipHoriz = pEffectKey->GetFlipHoriz();
			dlg.m_bFlipVert = pEffectKey->GetFlipVert();
			dlg.m_fOpacity = pEffectKey->GetfOpacityOrig();
			dlg.m_DrawMode = pEffectKey->GetDrawMode();
			dlg.m_Interpolation = pEffectKey->GetInterpolation();
		}
		
		if( dlg.DoModal() == IDOK )
		{
			DWORD dwFlag = 0;
			if( dlg.m_bFlipHoriz )				dwFlag |= EFF_FLIP_HORIZ;
			if( dlg.m_bFlipVert )					dwFlag |= EFF_FLIP_VERT;
			if( pEffectKey == nullptr )		// 현재위치에 이펙트 키가 없으면
			{
				// 이펙트 키를 새로 생성
				pEffectKey = spAction->AddKeyEffect( SPROBJ->GetFrmCurr(), spLayerMove, dlg.m_bInterpolation, dwFlag, dlg.m_DrawMode, dlg.m_fOpacity );
			} else
			{
				pEffectKey->Set( dlg.m_Interpolation, dwFlag, dlg.m_DrawMode, dlg.m_fOpacity );		// 이미 키가 있었다면 값 변경만 한다
			}
			pEffectKey->Execute( spActObj, spLayerMove );		// 키가 변경되었으므로 그것을 반영하기위해 키를 실행시킨다
			GetFrameView()->Update();
		}
	} else
		GetConsoleView()->Message( _T("image, object, dummy layer를 선택해 주십시요") );
}

BOOL CAnimationView::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( m_bPanningMode )
		SetCursor( AfxGetApp()->LoadCursor( IDC_HAND1 ) );
	else
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_ARROW ) );

	return TRUE;
	//	return __super::OnSetCursor(pWnd, nHitTest, message);
}

void CAnimationView::OnUpdateDrawProperty(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
//	if( TOOL->GetspSelLayer() && TOOL->GetspSelLayer()->GetType() == XBaseLayer::xIMAGE_LAYER )		// 이미지 레이어가 선택되어 있을때만 
	if( TOOL->GetspSelLayer() && TOOL->GetspSelLayer()->IsTypeLayerMove() )		
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}

// 이벤트키 생성
void CAnimationView::OnCreateEvent()
{
	VALID_ACTOBJ( spActObj );
	auto spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//	auto spLayer = spAction->GetspLayerEvent(0);
//	SPBaseLayer spLayer = spAction->FindLayer( XBaseLayer::xEVENT_LAYER, 0 );
//	auto spLayer = spActObj->GetspLayerByNumber<XLayerEvent>( 0 );
	auto spLayer = spActObj->GetspLayerByIndex<XLayerEvent>( /*xSpr::xLT_EVENT, */0 );
	if( spLayer == nullptr )		// 이벤트 레이어가 없으면 새로 만듬
	{
		//spLayer = SPROBJACT->CreateLayer( XBaseLayer::xEVENT_LAYER, 0 );
//		spLayer = spAction->AddEventLayer();
		spLayer = spActObj->AddLayerAuto<XLayerEvent>();
	}
//	RECT rect;
//	GetClientRect( &rect );
//	XE::VEC2 vCenter( rect.left+(rect.right-rect.left)/2.0f, rect.top+(rect.bottom-rect.top)/2.0f );
	XE::VEC2 vRMouseCood = GetRMouseCoord();
	auto pEventKey = spAction->AddKeyEventCreateObj( SPROBJ->GetFrmCurr(), spLayer, 1, vRMouseCood.x, vRMouseCood.y );
	pEventKey->Execute( spActObj, spLayer );
	GetFrameView()->Update();
}

// 현재프레임의 더미레이어에 더미키를 생성한다. 더미레이어가 없으면 새로 만든다.
void CAnimationView::OnCreateDummy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	auto spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 일단 더미
//	auto spLayer = spAction->AddDummyLayer();
//	auto spLayer = spActObj->GetspLayerByNumber<XLayerDummy>( 0 );
	auto spLayer = spActObj->GetspLayerByIndex<XLayerDummy>( /*xSpr::xLT_DUMMY, */0 );
	RegisterMI( spLayer->GetpMICrossDir() ); // fuck 이코드 밖으로 못뺌?

	XE::VEC2 vRMouseCood = GetRMouseCoord();;
	// 키 삽입
//	XKeyDummy *pMainKey = nullptr;
// 	if( (int)SPROBJ->GetFrmCurr() != 0 ) {		// 현재 0프레임에 위치한게 아니라면
// 		auto pMainKey = spAction->FindKeyByidLayer<XKeyDummy>( spLayer->GetidLayer(), 0 );		// 이 채널의 0프레임에 키가 있는지 검사해서
// 		if( pMainKey == nullptr ) {		// 없으면
// 			// 더미키를 추가
// 			pMainKey = spAction->AddKeyDummy( 0, spLayer, OFF );	// 0프레임에 off키를 삽입
// 		}
// 	}
	{		
		// 현재 프레임에 더미키를 추가한다
		auto pMainKey = spAction->AddKeyDummy( SPROBJ->GetFrmCurr(), spLayer, ON );	// on키를 삽입
		pMainKey->Execute( spActObj, spLayer );
	}
	// 편재 프레임에 포지션 키를 삽입한다
// 	XKeyPos *pPosKey = nullptr;
// 	spAction->FindKey( spLayer, SPROBJ->GetFrmCurr(), &pPosKey );		// 0프레임에 키가 있으면 현재프레임에 키가 있는지 검사
//	auto pPosKey = spAction->FindKeyPos( spLayer, SPROBJ->GetFrmCurr() );
	auto pPosKey = spAction->FindKeyByidLayer<XKeyPos>( spLayer->GetidLayer(), SPROBJ->GetFrmCurr() );	
	if( pPosKey == nullptr ) {
		// 더미키를 추가
		pPosKey = spAction->AddKeyPos( SPROBJ->GetFrmCurr(), spLayer, vRMouseCood.x, vRMouseCood.y );
		pPosKey->Execute( spActObj, spLayer );
	}
	GetFrameView()->Update();
}

// 사운드키 생성
void CAnimationView::OnCreateSound()
{
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	XKeySound *pSoundKey = nullptr;
//	auto spLayerSound = spAction->GetspLayerSound( 0 );
//	auto spLayerSound = spActObj->GetspLayerByNumber<XLayerSound>( 0 );
	auto spLayerSound = spActObj->GetspLayerByIndex<XLayerSound>( /*xSpr::xLT_SOUND, */0 );
	if( spLayerSound == nullptr )		// 사운드 레이어가 없으면 새로 만듬
	{
//		spLayerSound = spAction->AddSndLayer();
		spLayerSound = spActObj->AddLayerAuto<XLayerSound>();
	}
	ID idSound = 0;
	pSoundKey = spAction->AddKeySound( SPROBJ->GetFrmCurr(), spLayerSound, idSound );
	pSoundKey->EditDialog();
	pSoundKey->Execute( spActObj, spLayerSound );
	GetFrameView()->Update();
}

void CAnimationView::OnUpdateCreateEvent(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction ) 
		pCmdUI->Enable(1);
	else
		pCmdUI->Enable(0);
}
// 이벤트키 수정
void CAnimationView::OnEditEvent()
{
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	XSPAction spAction = SPROBJ->GetspAction();
	if( !spAction ) return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
//	XSPBaseLayer spLayer = spAction->FindLayer( xSpr::xLT_EVENT, 0 );
//	auto spLayer = spActObj->GetspLayerByNumber<XLayerEvent>( 0 );
	auto spLayer = spActObj->GetspLayerByIndex<XLayerEvent>( /*xSpr::xLT_EVENT, */0 );
	if( spLayer == nullptr )		// 이벤트 레이어가 있는지 찾음
		return;
//	XKeyEvent *pEventKey = (XKeyEvent *)spAction->FindKey( spLayer, xSpr::xKT_EVENT, xSpr::xKTS_MAIN, SPROBJ->GetFrmCurr() );;	// 현재프레임에 이미 이벤트키가 있으면 '수정'메뉴 활성화
	//if( !pEventKey )	return;
	// 이벤트키 수정 다이얼로그 띄움
}

void CAnimationView::OnUpdateEditEvent(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )
		return;
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction ) {
//		XSPBaseLayer spLayer = spAction->FindLayer( xSpr::xLT_EVENT, 0 );		// 이벤트레이어가 없으면 메뉴 표시 안함
//		auto spLayer = spActObj->GetspLayerByNumber<XLayerEvent>( 0 );
		auto spLayer = spActObj->GetspLayerByIndex<XLayerEvent>( /*xSpr::xLT_EVENT, */0 );
		if( spLayer ) {
//			auto pEventKey = (XKeyEvent*)spAction->FindKey( spLayer, xSpr::xKT_EVENT, xSpr::xKTS_MAIN, SPROBJ->GetFrmCurr() );;	// 현재프레임에 이미 이벤트키가 있으면 '수정'메뉴 활성화
			auto pEventKey = spAction->FindKeyByidLayer<XKeyEvent>( spLayer->GetidLayer(), SPROBJ->GetFrmCurr() );;	// 현재프레임에 이미 이벤트키가 있으면 '수정'메뉴 활성화
			if( pEventKey )
				pCmdUI->Enable(1);
			else
				pCmdUI->Enable(0);
		} else
			pCmdUI->Enable(0);
	} else
		pCmdUI->Enable(0);
}

// createobj layer의 오브젝트 속성값을 편집
void CAnimationView::OnPropertyObj()
{
	VALID_ACT( spAction );
	VALID_ACTOBJ( spActObj );
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	auto spLayer = TOOL->GetspSelLayerObject();
//	if( TOOL->GetspSelLayer() && TOOL->GetspSelLayer()->GetType() == XBaseLayer::xOBJ_LAYER )		// 이미지 레이어가 선택되어 있을때만 
	if( spLayer ) {
		XSprObj *pSprObj = spLayer->GetpSprObjCurr();
		if( !pSprObj )		return;
		CDlgObjProperty dlg;
		dlg.m_PlayType = pSprObj->GetPlayType();
		dlg.m_id = pSprObj->GetspAction()->GetidAct();
		if( dlg.DoModal() == IDOK )
		{
			if( pSprObj->GetpSprDat()->GetspAction( dlg.m_id ) == nullptr ) {
				XALERT( "ID %d는 없는 ID입니다", dlg.m_id );
				return;
			}
			// createobj의 키 정보도 바꿔준다
			// 현재프레임에서 바로앞에 있는 xSpr::xKT_CREATEOBJ를 찾는다
			auto pKey = dynamic_cast<XKeyCreateObj *>(
											spAction->FindPrevKey( xSpr::xKT_CREATEOBJ
																					, xSpr::xKTS_MAIN
																					, SPROBJ->GetFrmCurr()
																					, spLayer ));
			if( pKey )
			{
				pKey->SetPlayType( dlg.m_PlayType );
				pKey->SetnAction( dlg.m_id );
				SPROBJ->MoveFrame();			// 현재프레임까지 다시 실행시킨다
//				pSprObj->SetAction( dlg.m_id, dlg.m_PlayType );
			} else {
				XBREAKF(1, "이런 경우가 생기면 안된다" );
			}
			Update();
			GetFrameView()->Update();
		}
	} else
		GetConsoleView()->Message( _T("Object layer를 선택해 주십시요") );
}

void CAnimationView::OnUpdatePropertyObj(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(0);
	VALID_ACT( spAction );
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	XSPBaseLayer spLayer = TOOL->GetspSelLayer();
	if( spLayer && spLayer->IsSameType( xSpr::xLT_OBJECT ) )
	{
		pCmdUI->Enable(1);
	} else
		pCmdUI->Enable(0);
}

// 회전/스케일링 축을 보이게 하거나 감춥니다
void CAnimationView::OnViewTransAxis()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_bViewAdjustAxis = !m_bViewAdjustAxis;
	m_pMICross->SetbActive( !m_pMICross->GetbActive() );
}

void CAnimationView::OnUpdateViewTransAxis(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
//	if( m_bViewAdjustAxis )
	if( m_pMICross->GetbActive() )
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}


BOOL CAnimationView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

// OnKeyDown에서 쉬프트키의 nFlags값이 이상하게 들어와서 이걸썼음
BOOL CAnimationView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_SHIFT ) 
		{
			TOOL->SetbSlowDrag( TRUE );
		}
		else if( pMsg->wParam == VK_CONTROL ) {}
		else if( pMsg->wParam == VK_MENU ) {}
	} else
	if( pMsg->message == WM_KEYUP )
	{
		if( pMsg->wParam == VK_SHIFT ) 
		{
			TOOL->SetbSlowDrag( FALSE );
		}
		else if( pMsg->wParam == VK_CONTROL ) {}
		else if( pMsg->wParam == VK_MENU ) {}
	}
	return __super::PreTranslateMessage(pMsg);
}

void CAnimationView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bAlt = TRUE;
	__super::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CAnimationView::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bAlt = FALSE;
	__super::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

// 셀렉트 할수 있는 오브젝트를 화면에 투영하여 화면좌표를 얻어낸다
//void CAnimationView::Projection( XBaseElem *pObj )
//{
//	XE::VEC2 vScr = PosToViewCoord( pObj->GetPos() );
//	pObj->SetvDrawPos( vScr );		// AnimationView에서 pObj가 그려질 화면좌표를 계산해 넣는다
//	return vScr;
//}

void CAnimationView::SelectGetNextClear()
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction == nullptr )		return;
	spAction->GetNextKeyClear();
}

XBaseElem* CAnimationView::SelectGetNext()
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction == nullptr )		return nullptr;
	return spAction->GetNextKey();
}
void CAnimationView::SelectOnDragOnly( XBaseElem *pObj, const XE::VEC2& vObjDist )
{
	XE::VEC2 vDist = vObjDist;
	TOOL->AdjustAxisLock( &(vDist.x), &(vDist.y) );
	pObj->SetPos( pObj->GetPos() + vDist );
}

XBaseElem* CAnimationView::GetSelectObj( ID idObj ) 
{
	XSPAction spAction = SPROBJ->GetspAction();
	if( spAction == nullptr )		return nullptr;
	return spAction->GetpKeyMng()->GetKey( idObj );
}

// vScr은 스크린좌표
/*
BOOL CAnimationView::SelectObjInArea( XBaseElem *pObj, const XE::VEC2& vPoint )
{
	XKeyPos *pPosKey = dynamic_cast<XKeyPos*, XBaseElem*>( pObj );	// 아 귀찮아서 다이나믹 캐스트로 함
	if( pPosKey == nullptr )		return FALSE;
	//
	XE::VEC2 size = XE::VEC2( 6.0f, 6.0f );
	XE::VEC2 vLT = vScr - size / 2.0f
	if( XE::IsArea( vLT, size, vPoint ) )
		return TRUE;
	return FALSE;
}*/

/**
 @brief 현재 선택된 레이어의 현재프레임의 포지션키의 랜덤영역 그리기
*/
void CAnimationView::DrawLayerBefore( XSPBaseLayer spLayer )
{
	auto spAction = SPROBJ->GetspAction();
	auto spSelLayer = TOOL->GetspSelLayer();
	if( !spSelLayer )
		return;
	if( spSelLayer->GetidLayer() != spLayer->GetidLayer() )
		return;
	auto pKeyPos = spAction->FindKeyByidLayer<XKeyPos>( spSelLayer->GetidLayer()
		, SPROBJ->GetFrmCurr() );
	if( pKeyPos && pKeyPos->IsRandomFactor() ) {
		if( pKeyPos->GetRadius() != 0 ) {
			// 원으로
			auto vPos = pKeyPos->GetvPosOrig() * GetvScaleView();
			auto radius = pKeyPos->GetRadius() * GetvScaleView().x;
			GRAPHICS->DrawCircle( m_vCenter + vPos
				, radius
				, XCOLOR_ORANGE );
		}
		else {
			// 사각형으로
			auto rectArea = pKeyPos->GetRectRandom();
			rectArea = rectArea * GetvScaleView();
			GRAPHICS->DrawRect( m_vCenter + rectArea.vLT
				, m_vCenter + rectArea.vRB, XCOLOR_ORANGE );
		}
	}
}

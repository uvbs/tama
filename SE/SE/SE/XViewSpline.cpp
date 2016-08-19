// XViewSpline.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SE.h"
#include "XViewSpline.h"
#include "MainFrm.h"
#include "ThumbView.h"
#include "XSurfaceD3D.h"
// XViewSpline

#define LEN_JOINT		30.f			// 관절길이

IMPLEMENT_DYNCREATE(XViewSpline, CView)

BEGIN_MESSAGE_MAP(XViewSpline, CView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// XViewSpline 그리기입니다.

//void XViewSpline::OnDraw(CDC* pDC)
//{
//	CDocument* pDoc = GetDocument();
//	// TODO: 여기에 그리기 코드를 추가합니다.
//}
static XViewSpline *s_pViewSpline = nullptr;
XViewSpline *GetViewSpline() { return s_pViewSpline; }


// XViewSpline 진단입니다.

#ifdef _DEBUG
void XViewSpline::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void XViewSpline::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// static
XViewSpline *XViewSpline::CreateOne( CWnd *pParent )
{
	XViewSpline *pView = new XViewSpline;
	s_pViewSpline = pView;
	//XViewSpline *pFormView = nullptr;
	//CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(XViewSpline);
	//pFormView = (XViewSpline *)pRuntimeClass->CreateObject();

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
		AfxMessageBox( _T("Failed in creating XViewSpline") );
	}

	pView->OnInitialUpdate();

	return pView;
}

// XViewSpline 메시지 처리기입니다.
int XViewSpline::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
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

int XViewSpline::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}
void XViewSpline::OnInitialUpdate()
{
	__super::OnInitialUpdate();
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	for( int i = 0; i < MAX_JOINT; ++i )
	{
		m_listJoint[i].m_vLocal = D3DXVECTOR2( 30.f, 0 );
		m_listJoint[i].m_rAngle = 0;
	}
	UpdateMatrix();
	//
//	XD3DView::CreateView( GetSafeHwnd() );
	XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
}

void XViewSpline::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if( D3DDEVICE )
//		XD3DView::CreateView( GetSafeHwnd() );
		XD3DView::CreateView( GetSafeHwnd(), g_pMainFrm->GetDevice(), g_pMainFrm->m_d3dpp.BackBufferFormat );
}

static XE::VEC2 m_vPrev;
D3DXVECTOR2 vCenter( 300.f, 300.f );
void XViewSpline::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_vPrev = point;
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_idxSelected = -1;
	D3DXMATRIX mInv, mInvCenter;
	D3DXMATRIX mCenter;
	D3DXMatrixTranslation( &mCenter, vCenter.x, vCenter.y, 0 );
	D3DXMatrixInverse( &mInvCenter, nullptr, &mCenter );		// 원점의 역행렬
	for( int i = 0; i < MAX_JOINT; ++i )
	{
		JOINT *j = &m_listJoint[i];

		D3DXMATRIX mWorld = j->m_mWorld * mInvCenter;
		D3DXMatrixInverse( &mInv, nullptr, &mWorld );		// 월드행렬의 역행렬
		D3DXVECTOR2 vMouse( (float)point.x, (float)point.y );
		D3DXVECTOR2 vMouseLocal = vMouse - vCenter;		// 원점을 중심으로한 마우스 좌표
		D3DXVECTOR2 vT;
		D3DXVec2TransformCoord( &vT, &vMouseLocal, &mInv );		// 마우스좌표를 역회전시켜서 수평상태기준의 좌표가 됨
		D3DXVECTOR2 vDist = vT - D3DXVECTOR2( LEN_JOINT, 0 );			// 관절끝을 기준으로한 벡터
	
		if( D3DXVec2LengthSq( &vDist ) < 5.f * 5.f )		// 원의 반지름
		{
			m_idxSelected = i;
			break;
		}
	}
	Invalidate(0);
	__super::OnLButtonDown(nFlags, point);
}


void XViewSpline::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( m_idxSelected >= 0 )		// 관절이 선택되어 있을때
	{
		if( m_bIK )
		{
			for( int i = m_idxSelected; i >= 0; --i )
			{
				JOINT *j = &m_listJoint[ i ];

				D3DXVECTOR2 v0( LEN_JOINT, 0 ), v0T;
				D3DXVec2TransformCoord( &v0T, &v0, &j->m_mWorld );
				D3DXVECTOR2 vF = D3DXVECTOR2( (float)point.x, (float)point.y ) - v0T;		// 관절끝좌표에서 현재 마우스 위치로의 벡터
				D3DXVec2Normalize( &j->m_vForce, &vF );		// 노말라이즈
				float dot = D3DXVec2Dot( &j->m_vNormal, &j->m_vForce );
				j->dot = dot;
				float torque = dot * 0.01f;
				j->m_rAngle -= torque;
			}
		}
	}
	Invalidate(0);

	__super::OnRButtonDown(nFlags, point);
}

void XViewSpline::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Invalidate(0);
	m_idxSelected = -1;
	__super::OnLButtonUp(nFlags, point);
}

void XViewSpline::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	Invalidate(0);
	XE::VEC2 vPos = point;
	m_vMouse = point;
	m_vTarget = D3DXVECTOR2( vPos.x, vPos.y );

	//
	m_vPrev = vPos;
	__super::OnMouseMove(nFlags, point);
}

static D3DXVECTOR2 s_vC( 300.f, 150.f );
static D3DXVECTOR2 s_vB( 100.f, 0 );;

void XViewSpline::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	for( int i = 0; i < MAX_JOINT; ++i )
	{
		m_listJoint[i].m_vLocal = D3DXVECTOR2( 30.f, 0 );
		m_listJoint[i].m_rAngle = 0;
	}
	UpdateMatrix();
	//
	s_vB = D3DXVECTOR2( (float)point.x, (float)point.y ) - s_vC;		// 목표벡터

	__super::OnRButtonDown(nFlags, point);
}

// idx인덱스 이하의 모든 관절의 매트릭스를 갱신함.
void XViewSpline::UpdateMatrix( int idx )
{
	D3DXMATRIX mWorld;

	D3DXMatrixTranslation( &mWorld, vCenter.x, vCenter.y, 0 );
	for( int i = idx; i < MAX_JOINT; ++i )
	{
		JOINT *j = &m_listJoint[ i ];
		// 각도가 돌아갔으므로 행렬과 노말도 갱신
		D3DXMATRIX mRot, mTrans, mLocal;
		D3DXMatrixRotationZ( &mRot, j->m_rAngle );
		D3DXMatrixTranslation( &mTrans, j->m_vLocal.x, j->m_vLocal.y, 0 );
		mLocal = mRot * mTrans;
		if( i > 0 )
			mWorld = mLocal * m_listJoint[ i - 1 ].m_mWorld;
		else
			mWorld = mLocal * mWorld;
		j->m_mWorld = mWorld;
		// 관절 양쪽끝 좌표 계산
		D3DXVECTOR2 v0 = D3DXVECTOR2( 0, 0 );			// 관절원점
		D3DXVECTOR2 v1 = D3DXVECTOR2( LEN_JOINT, 0 );		// 관절끝위치
		D3DXVECTOR2 v0T, v1T;
		D3DXVec2TransformCoord( &j->m_vStart, &v0, &mWorld );		
		D3DXVec2TransformCoord( &j->m_vEnd, &v1, &mWorld );		
		
		// 노말 재계산
		D3DXVECTOR2 vN;
		D3DXVECTOR2 vNLocal( 0, -1.f );
		D3DXVec2TransformCoord( &vN, &vNLocal, &mWorld );		// 노말도 같이 돌림
		D3DXVECTOR2 vZero(0, 0), vStart;
	//	D3DXVec2TransformCoord( &vStart, &vZero, &mWorld );	// 관절의 시작지점(월드)
		j->m_vNormal = vN - j->m_vStart;		// 노말(월드)
	//	D3DXMATRIX mInv;
	//	D3DXMatrixInverse( &mInv, nullptr, &mWorld );
	//	D3DXVec2TransformCoord( &j->m_vNormal, &vN, &mInv );		// 월드로 변환된 노말을 원점으로 돌린다.
	}
}

// 0,0 ~ 100,0에 이르는 수평막대를 현재행렬 mWorld에 그린다
void DrawBar( const D3DXMATRIX& mWorld )
{
	D3DXVECTOR2 v0 = D3DXVECTOR2( 0, 0 );			// 관절원점
	D3DXVECTOR2 v1 = D3DXVECTOR2( LEN_JOINT, 0 );		// 관절끝위치
	D3DXVECTOR2 v0T, v1T;
	D3DXVec2TransformCoord( &v0T, &v0, &mWorld );		
	D3DXVec2TransformCoord( &v1T, &v1, &mWorld );		
	GRAPHICS->DrawLine( v0T.x, v0T.y, v1T.x, v1T.y, XCOLOR_WHITE );		// 
}

float GetAngle( const D3DXVECTOR2& v )
{
	return atan2( v.y, v.x );
}
void XViewSpline::FrameMove()
{
	// 관절을 클릭한상태로 움직인다.
	if( m_idxSelected >= 0 )
	{
		if( m_bIK )
		{
			// 목표지점이 주어지면 각각의 IK알고리즘에 따라 관절들을 회전시킨다.
			if( m_typeIK == IK_DOT )
			{
				for( int i = m_idxSelected; i >= 0; --i )
				{
					JOINT *j = &m_listJoint[ i ];

					D3DXVECTOR2 v0( LEN_JOINT, 0 ), v0T;
					D3DXVec2TransformCoord( &v0T, &v0, &j->m_mWorld );
					D3DXVECTOR2 vF = D3DXVECTOR2( m_vTarget.x, m_vTarget.y ) - v0T;		// 관절끝좌표에서 현재 마우스 위치로의 벡터
					D3DXVec2Normalize( &j->m_vForce, &vF );		// 노말라이즈
					float dot = D3DXVec2Dot( &j->m_vNormal, &j->m_vForce );
					j->dot = dot;
					float torque = dot * 0.02f;
					j->m_rAngle -= torque;
				}
			} else
			if( m_typeIK == IK_CCD )
			{
				// 관절의 끝에서 부터 시작
				// 관절의 시작점에서 관절의 최하위관절의 끝지점을 연결하는 벡터A를 구함
				// 관절의 시작점에서 목표지점까지를 연결하는 벡터B를 구함
				// 벡터B - 벡터A만큼의 각도를 현재관절에 회전시킴
				// 윗단계 관절로 이동 후 반복
				for( int i = m_idxSelected; i >= 0; --i )
				{
					JOINT *j = &m_listJoint[ i ];
					D3DXVECTOR2 vA = m_listJoint[ m_idxSelected ].m_vEnd - j->m_vStart;	// 관절의 시작점에서 관절의 최하위관절의 끝지점을 연결하는 벡터A를 구함
					D3DXVECTOR2 vB = m_vTarget - j->m_vStart;		// 관절의 시작점에서 목표지점까지를 연결하는 벡터B를 구함
					float rA = GetAngle( vA );		// 벡터의 각도 구함
					float rB = GetAngle( vB );
					float rDist = rB - rA;		// 두 벡터간의 각도차
					if( rDist > D2R(180.f) )
						rDist -= D2R(360.f);
					else
					if( rDist < D2R(-180.f) )
						rDist += D2R(360.f);

					if( D2R(0.01f) > fabs( rDist ) )
					{
						if( rDist > 0 )
							rDist = D2R(0.01f);
						else
						if( rDist < 0 )
							rDist = D2R(-0.01f);
					}
					
					float rOld = j->m_rAngle;
					j->m_rAngle += rDist;		// 현재 관절에 각도차를 더함
					// 각도제한
					if( j->m_rAngle > D2R(45.f) )
						j->m_rAngle = D2R(45.f);
					else 
					if( j->m_rAngle < D2R(-45.f) )
						j->m_rAngle = D2R(-45.f); 

					UpdateMatrix( i );
				}

			}
		}/* else
		{
			D3DXVECTOR2 vDist = m_vMouse - s_vPrev;		// 마우스가 움직인 거리
			JOINT *j = &m_listJoint[ m_idxSelected ];
			if( vDist.x > 0 )
				j->m_rAngle += D2R(1.f);
			else
				j->m_rAngle -= D2R(1.f);
		} */
		// 행렬, 노말 갱신
		UpdateMatrix();
	} 
}
void XViewSpline::Draw()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
				// FrameMove
				FrameMove();
				//
				D3DXVECTOR2 v( 0, 0 );		// 
				D3DXMATRIX mWorld, mRot, mTrans, mLocal;
				D3DXMatrixIdentity( &mWorld );
				D3DXMatrixTranslation( &mWorld, vCenter.x, vCenter.y, 0 );
				for( int i = 0; i < MAX_JOINT; ++i )
				{
					JOINT *j = &m_listJoint[i];

					// 관절부위에 원을 그림
					D3DCOLOR col = XCOLOR_WHITE;
					if( m_idxSelected == i )
						col = XCOLOR_RED;
					GRAPHICS->DrawCircle( j->m_vEnd.x, j->m_vEnd.y, 5.f, col );		// 관절끝 부위에 원을 그림
					GRAPHICS->DrawLine( j->m_vStart.x, j->m_vStart.y, j->m_vEnd.x, j->m_vEnd.y, col );		// 관절 그림
					// test
					{
						D3DXVECTOR2 vA;
						vA = D3DXVECTOR2( m_vMouse.x, m_vMouse.y ) - s_vC;
						float rDist = GetAngle( s_vB ) - GetAngle( vA );
						if( rDist > D2R(180.f) )
							rDist -= D2R(360.f);
						else
						if( rDist < D2R(-180.f) )
							rDist += D2R(360.f);

						// draw
						GRAPHICS->DrawLine( s_vC.x - 100.f, s_vC.y, s_vC.x + 100.f, s_vC.y, XCOLOR_BLUE );
						GRAPHICS->DrawLine( s_vC.x, s_vC.y - 100.f, s_vC.x, s_vC.y + 100.f, XCOLOR_BLUE );		// 십자선
						GRAPHICS->DrawLine( s_vC.x, s_vC.y, s_vC.x + s_vB.x, s_vC.y + s_vB.y, XCOLOR_RED );	// 목표벡터
						GRAPHICS->DrawLine( s_vC.x, s_vC.y, s_vC.x + vA.x, s_vC.y + vA.y, XCOLOR_WHITE );	// 시작 벡터
						SE::g_pFDBasic->DrawString( s_vC.x + 5.f, s_vC.y + 5.f, XCOLOR_WHITE, _T("%f(%f)"), R2D(rDist), rDist );

					}

					// info
					SE::g_pFDBasic->DrawString( 5.f, 5.f, XCOLOR_WHITE, _T("%d-%d"), (int)m_vMouse.x, (int)m_vMouse.y );
/*					D3DXMatrixRotationZ( &mRot, j->m_rAngle );
					D3DXMatrixTranslation( &mTrans, j->m_vLocal.x, j->m_vLocal.y, 0 );
					mLocal = mRot * mTrans;
					mWorld = mLocal * mWorld;
					j->m_mWorld = mWorld;

					D3DXVECTOR2 v0 = D3DXVECTOR2( 0, 0 );			// 관절원점
					D3DXVECTOR2 v1 = D3DXVECTOR2( LEN_JOINT, 0 );		// 관절끝위치
					D3DXVECTOR2 v0T, v1T;
					D3DXVec2TransformCoord( &v0T, &v0, &mWorld );		
					D3DXVec2TransformCoord( &v1T, &v1, &mWorld );		
					// 관절부위에 원을 그림
					D3DCOLOR col = XCOLOR_WHITE;
					if( m_idxSelected == i )
						col = XCOLOR_RED;
					GRAPHICS->DrawCircle( v1T.x, v1T.y, 5.f, col );		// 관절부위에 원을 그림
					GRAPHICS->DrawLine( v0T.x, v0T.y, v1T.x, v1T.y, col );		// 관절 그림
					if( m_bDrawDebug )
					{
	//					SE::g_pFDBasic->DrawString( v1T.x + 20.f, v1T.y - 20.f, XCOLOR_WHITE, _T("%f"), j->dot );
						// normal
						D3DXVECTOR2 v2T;
						v1 = j->m_vNormal * 50.f + v0T;
				//		D3DXVec2TransformCoord( &v2T, &v1, &mWorld );		
						GRAPHICS->DrawLine( v0T.x, v0T.y, v1.x, v1.y, XCOLOR_BLUE );		// 노말벡터
						// 힘벡터
						v1 = j->m_vForce * 300.f;
						v2T = v1T + v1;
						GRAPHICS->DrawLine( v1T.x, v1T.y, v2T.x, v2T.y, XCOLOR_GREEN );		// 노말벡터
					}
					*/
				}
				
				//
				D3DDEVICE->EndScene();
			}
			GetChain()->Present( nullptr, nullptr, GetSafeHwnd(), nullptr, 0 );
			pBackBuffer->Release();
		}
	}

}

void XViewSpline::OnDraw(CDC* /*pDC*/)
{
	Draw();
}


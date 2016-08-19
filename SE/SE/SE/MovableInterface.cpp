#include "stdafx.h"
#include "MovableInterface.h"
#include "Tool.h"
//#include "Graphic2D.h"
#include "UndoMng.h"
#include "XGraphicsD3DTool.h"
#include "XAniAction.h"
#include "XKeyBase.h"
#include "XKeyRot.h"
#include "XKeyPos.h"
#include "XLayerBase.h"
#include "XLayerMove.h"
#include "XActObj.h"
/*void XMovableInterface::DragMove( float cx, float cy, const CPoint &point, const CPoint &prevPoint ) 
{ 
	if( !GetbActive() )	return;
	CPoint dist = point - prevPoint;
	TOOL->AdjustAxisLock( &dist.x, &dist.y );
	AddPos( (float)dist.x, (float)dist.y );
}*/

void XMovableInterface::DragMove( float cx, float cy, const XE::VEC2& point, const XE::VEC2& vDist ) 
{ 
	if( !GetbActive() )	return;
	XE::VEC2 dist = vDist;
	TOOL->AdjustAxisLock( &dist.x, &dist.y );
	AddPos( dist.x, dist.y );
}

void XMICross::Draw( float cx, float cy, float clx, float cly, const XE::VEC2& vMouse )
{
	if( !GetbActive() )	return;
/*	cx += clx * GetvScale().x;
	cy += cly * GetvScale().y;
	float x = cx + Getx() * GetvScale().x;
	float y = cy + Gety() * GetvScale().y; 
	float hw = (m_w * GetvScale().x) / 2.0f;
	float hh = (m_h * GetvScale().y) / 2.0f;
	XCOLOR col = XCOLOR_BLACK;
	if( GetbHighlight() )
		col = XCOLOR_WHITE;
	XE::DrawLine( x-hw, y,  x+hw, y, col );		// 가로선
	XE::DrawLine( x, y-hh,  x, y+hh, col );		// 세로선
*/
	XE::VEC2 vCenter( cx, cy );
	XE::VEC2 vCenterLocal( clx, cly );
	vCenter += vCenterLocal * GetvScale();
	XE::VEC2 vPos = vCenter + GetPosT();
	XE::VEC2 sizeHalf = GetSizeT() / 2.0f;
	XCOLOR col = XCOLOR_BLACK;
	if( GetbHighlight() )
		col = XCOLOR_WHITE;
	XE::DrawLine( vPos.x-sizeHalf.w, vPos.y,  vPos.x+sizeHalf.w, vPos.y, col );		// 가로선
	XE::DrawLine( vPos.x, vPos.y-sizeHalf.h,  vPos.x, vPos.y+sizeHalf.h, col );		// 세로선

}
// point: center로부터의 상대적인 마우스좌표
// mxl, myl: mouse x,y local from layer center
void XMICross::OnMouseMove( float _mxl, float _myl )
{
	if( !GetbActive() )	return;
	float mxl = _mxl * GetvScale().x;
	float myl = _myl * GetvScale().y;
	float ht = m_fThickness / 2.0f;		// 두께의 절반
	XE::VEC2 sizeHalf = GetSizeT() / 2.0f;
	//float hw = m_w / 2.0f;
	//float hh = m_h / 2.0f;
	SetbHighlight( FALSE );
	// 가로선 검사
	XE::VEC2 vPos = GetPosT();
	if( mxl >= vPos.x - sizeHalf.w && mxl <= vPos.x + sizeHalf.w &&		// 가로크기 안에 들어가면서
		myl >= vPos.y - ht && myl <= vPos.y + ht )			// 세로두께 안에 들어가는가
		SetbHighlight( TRUE );
	// 세로선 검사
	if( myl >= vPos.y - sizeHalf.h && myl <= vPos.y + sizeHalf.h &&		// 세로크기 안에 들어가면서
		mxl >= vPos.x - ht && mxl <= vPos.x + ht )			// 세로두께 안에 들어가는가
		SetbHighlight( TRUE );
}

void XMICrossDir::Draw( float cx, float cy, float clx, float cly, const XE::VEC2& vMouse )
{
	if( !GetbActive() )	return;
	XE::VEC2 vCenter( cx, cy );
	XE::VEC2 vCenterLocal( clx, cly );
	XE::VEC2 vPos = vCenter + GetPosT();
	//float x = cx + Getx() * GetvScale().x;
	//float y = cy + Gety() * GetvScale().y; 
	float hw = GetwT() / 2.0f;
	D3DXMATRIX mRot;
	D3DXMatrixRotationZ( &mRot, D3DXToRadian( m_fAngle ) );
	D3DXVECTOR2 wv1 = D3DXVECTOR2( -hw, 0 );
	D3DXVECTOR2 wv2 = D3DXVECTOR2( hw, 0 );
	D3DXVec2TransformCoord( &wv1, &wv1, &mRot );
	D3DXVec2TransformCoord( &wv2, &wv2, &mRot );
	float toph = GetTophT();
	float bottomh = GetBottomhT();
	if( IsHighlight() )
	{
		CString str;
		if( GetbHighlightDir() )		// 방향쪽선이 하이라이트 되어있으면 긴선만 흰색으로 찍음
		{
			XE::DrawLine( vPos.x+wv1.x, vPos.y+wv1.y,  vPos.x+wv2.x, vPos.y+wv2.y, XCOLOR_BLACK );		// 가로선
			D3DXVECTOR2 v1 = D3DXVECTOR2( 0, -toph );
			D3DXVec2TransformCoord( &v1, &v1, &mRot );
			XE::DrawLine( vPos.x+v1.x, vPos.y+v1.y,  vPos.x, vPos.y, XCOLOR_WHITE );		// 긴쪽 세로선
			v1 = D3DXVECTOR2( 0, bottomh );
			D3DXVec2TransformCoord( &v1, &v1, &mRot );
			XE::DrawLine( vPos.x, vPos.y,  vPos.x+v1.x, vPos.y+v1.y, XCOLOR_BLACK );		// 짧은쪽 세로선
			// text info
			float fAngle = ROUND_OFF( GetfAngle() );
			str.Format( _T("%+3.0f˚"), fAngle );		// 돌아간 각도를 보여줌
			SE::g_pFont->DrawString( vMouse.x, vMouse.y - 16, str );
		} else
		{
			XE::DrawLine( vPos.x+wv1.x, vPos.y+wv1.y,  vPos.x+wv2.x, vPos.y+wv2.y, XCOLOR_WHITE );		// 가로선
			D3DXVECTOR2 v1 = D3DXVECTOR2( 0, -toph );
			D3DXVECTOR2 v2 = D3DXVECTOR2( 0, bottomh );
			D3DXVec2TransformCoord( &v1, &v1, &mRot );
			D3DXVec2TransformCoord( &v2, &v2, &mRot );
			XE::DrawLine( vPos.x+v1.x, vPos.y+v1.y,  vPos.x+v2.x, vPos.y+v2.y, XCOLOR_WHITE );		// 세로선
			// tetxt info
			str.Format( _T("%+3.1f, %+3.1f"), GetPosL().x, GetPosL().y );		// 인터페이스의 좌표를 표시
			SE::g_pFont->DrawString( vMouse.x, vMouse.y - 16, str );
		}
	} else
	{
		XE::DrawLine( vPos.x+wv1.x, vPos.y+wv1.y,  vPos.x+wv2.x, vPos.y+wv2.y, XCOLOR_BLACK );		// 가로선
		D3DXVECTOR2 v1 = D3DXVECTOR2( 0, -toph );
		D3DXVECTOR2 v2 = D3DXVECTOR2( 0, bottomh );
		D3DXVec2TransformCoord( &v1, &v1, &mRot );
		D3DXVec2TransformCoord( &v2, &v2, &mRot );
		XE::DrawLine( vPos.x+v1.x, vPos.y+v1.y,  vPos.x+v2.x, vPos.y+v2.y, XCOLOR_BLACK );		// 세로선
	}
}
// mxl, myl: XMovableInterface::m_xy로부터의 상대적인 마우스 위치
void XMICrossDir::OnMouseMove( float mxl, float myl )
{
	if( !GetbActive() )	return;
//	float mxl = _mxl * GetvScale().x;
//	float myl = _myl * GetvScale().y;
	float ht = m_fThickness / 2.0f;		// 두께의 절반
	float hw = GetwT() / 2.0f;
	SetbHighlight( FALSE );
	SetbHighlightDir( FALSE );
	D3DXMATRIX mRot, mInv;
	D3DXMatrixRotationZ( &mRot, D3DXToRadian( m_fAngle ) );
	D3DXMatrixInverse( &mInv, NULL, &mRot );
	D3DXVECTOR2 v1 = D3DXVECTOR2( mxl, myl );
	D3DXVec2TransformCoord( &v1, &v1, &mInv );			// 역회전하여 원점에 맞춤

	// 가로선 검사
	if( v1.x >= -hw && v1.x <= hw &&		// 가로크기 안에 들어가면서
		v1.y >= -ht && v1.y <= ht )			// 세로두께 안에 들어가는가
		SetbHighlight( TRUE );
	// 짧은쪽 세로선 검사
	if( v1.y >= 0 && v1.y <= GetBottomhT() &&		// 짧은쪽 세로크기 안에 들어가면서
		v1.x >= -ht && v1.x <= ht )			// 가로두께 안에 들어가는가
	{
		SetbHighlight( TRUE );
		SetbHighlightDir( FALSE );
	}
	// 긴쪽 세로선 검사
	if( v1.y >= -GetTophT() && v1.y <= 0 &&		// 긴쪽세로크기 안에 들어가면서
		v1.x >= -ht && v1.x <= ht )			// 가로두께 안에 들어가는가
	{
		SetbHighlight( TRUE );
		SetbHighlightDir( TRUE );
	}
}
void XMICrossDir::OnLButtonDown( float cx, float cy, const CPoint &point )
{
	if( IsHighlight() )
	{
		if( GetbHighlightDir() )
		{
			// 각도조절 모드
			XE::VEC2 vCenter( cx, cy );
			XE::VEC2 vAxis = vCenter + GetPosT() + m_vAdjustAxis;	// 회전축
//			float xAxis = cx + Getx() + m_vAdjustAxis.x;		
//			float yAxis = cy + Gety() + m_vAdjustAxis.y;
			float xDist = point.x - vAxis.x;
			float yDist = point.y - vAxis.y;
			float fAngleFromAxis = D3DXToDegree( atan2( yDist, xDist ) );  // 회전축으로부터 현재커서의 각도
			m_fPrevAngle = fAngleFromAxis;
		}
		m_nDragState = 1;		// 클릭만 한상태
	}
}
void XMICrossDir::OnLButtonUp( float cx, float cy, const CPoint &point )
{
	m_fAngle = ROUND_OFF( m_fAngle );		// 손을 뗄때 소숫점아래는 반올림시킨다
	m_nDragState = 0;
}
void XMICrossDir::DragMove( float cx, float cy, const XE::VEC2& point, const XE::VEC2& vDist ) 
{ 
	if( IsHighlight() )
	{
		if( GetbHighlightDir() )
		{
			// 각도조절 모드
			XE::VEC2 vCenter( cx, cy );
			XE::VEC2 vAxis = vCenter + GetPosT() + m_vAdjustAxis;	// 회전축
//			float xAxis = cx + Getx() + m_vAdjustAxis.x;		// 회전축
//			float yAxis = cy + Gety() + m_vAdjustAxis.y;
			float xDist = point.x - vAxis.x;
			float yDist = point.y - vAxis.y;
			float fAngleFromAxis = D3DXToDegree( atan2( yDist, xDist ) );  // 회전축으로부터 현재커서의 각도
			m_fDistAngleLocal = XE::CalcDistAngle( m_fPrevAngle, fAngleFromAxis );		// m_fPrevAngle로부터 fAngleFromAxis의 상대각도(-180~+180의 범위가 된다)
			m_fAngle += m_fDistAngleLocal;
//			m_fAngle = fmod( m_fAngle, 360.0f );		// LayerCrossDir에서 누적각도로 알수 있어야 하기땜에 뺌
			m_fPrevAngle = fAngleFromAxis;
		} else
		{
			// 좌표 이동모드
			XMovableInterface::DragMove( cx, cy, point, vDist );
		}
	} 
}

void XMILayerCrossDir::OnLButtonUp( float cx, float cy, const CPoint &point )
{
	int nDragState = m_nDragState;
	XMICrossDir::OnLButtonUp( cx, cy, point );
	if( nDragState == 2 )	 
	{
		if( XASSERT(IsHighlight()) )
		{
			if( GetbHighlightDir() )
			{
				XKeyRot *pRotKey = NULL;
				if( XASSERT( m_pDragKey && m_pDragKey->GetSubType() == xSpr::xKTS_ROT ) )
					pRotKey = dynamic_cast<XKeyRot *>( m_pDragKey );
				if( XASSERT(pRotKey) )
					pRotKey->SetfAngleZ( ROUND_OFF( pRotKey->GetdAngleOrig() ) ); 		// 손을뗄때 소숫점각도는 반올림시켜서 재저장한다
				
				((XUndoKeyChange *)UNDOMNG->GetLastQ())->SetKeyAfter( m_pDragKey );
			} else
			{
				((XUndoKeyChange *)UNDOMNG->GetLastQ())->SetKeyAfter( m_pDragKey );
			}
		}
	}
}
void XMILayerCrossDir::DragMove( float cx, float cy, const XE::VEC2& point, const XE::VEC2& vDist ) 
{ 
	if( !GetbActive() )	return;
	auto spActObj = SPROBJ->GetspActObjCurr();
	if( !spActObj )		return;
	auto spAction = spActObj->GetspAction();
	if( !spAction )		return;
	if( m_nDragState == 0 )		
		return;

	XMICrossDir::DragMove( cx, cy, point, vDist );
	
	if( IsHighlight() ) {
		const XSPLayerMove& spLayer = m_spLayer;
		const ID idLayer = m_spLayer->GetidLayer();
		if( GetbHighlightDir() ) {			// 각도조절 모드
			BOOL bCreateKey = FALSE;
// 			XKeyRot *pRotKey =  NULL;
// 			spAction->FindKey( spLayer, SPROBJ->GetFrmCurr(), &pRotKey );		// 현재 프레임/레이어에 회전키가 있는가
//			auto pRotKey = spAction->FindKeyRot( spLayer, SPROBJ->GetFrmCurr() );
			auto pRotKey = spAction->FindKeyByidLayer<XKeyRot>( spLayer->GetidLayer()
																								, SPROBJ->GetFrmCurr() );
			if( pRotKey == NULL ) {
				bCreateKey = TRUE;
				float fAngleZ = spLayer->GetcnRot().fAngle;		// 레이어의 현재각도를 기본값으로 키를 만든다
				pRotKey = spAction->AddKeyRot( SPROBJ->GetFrmCurr(), spLayer, fAngleZ );
				if( m_nDragState == 1 )
					UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pRotKey ) );
				if( SPROBJ->GetFrmCurr() != 0 )		// 0프레임이 아닌곳에 키가 추가되면
				{
// 					XKeyRot *pRotKeyZero =  NULL;
// 					spAction->FindKey( spLayer, 0, &pRotKeyZero );		// 이 채널의 0프레임에 회전키가 있는지 검사해서
//					auto pRotKeyZero = spAction->FindKeyRot( spLayer, 0 );
					auto pRotKeyZero 
						= spAction->FindKeyByidLayer<XKeyRot>( spLayer->GetidLayer()
																									, 0 );
					if( pRotKeyZero == NULL ) {		// 없으면
						pRotKeyZero = spAction->AddKeyRot( 0, spLayer, 0 );			// 각도 0짜리 회전키를 0프레임에 자동으로 넣어준다
						if( m_nDragState == 1 )
							UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pRotKeyZero ), 1 );		// 추가모드로 역행동 푸쉬
					}
				}
			}
			if( m_nDragState == 1 ) {
				UNDOMNG->PushUndo( new XUndoKeyChange( SPROBJ, spActObj, pRotKey ), bCreateKey );		// 키 좌표가 변하기전 언두행동을 푸쉬
			}
			m_nDragState = 2;
			pRotKey->SetfAngleZ( GetfAngle() );			
// 			pRotKey->Execute( SPROBJ );
			pRotKey->Execute( spActObj, m_spLayer );
			m_pDragKey = pRotKey;
		} else
		{									// 좌표조절 모드
			BOOL bCreateKey = FALSE;
// 			XKeyPos *pPosKey;
// 			spAction->FindKey( spLayer, SPROBJ->GetFrmCurr(), &pPosKey );		// 현재 프레임/레이어에 위치이동 키가 없는가
//			auto pPosKey = spAction->FindKeyPos( spLayer, SPROBJ->GetFrmCurr() );
			auto pPosKey = spAction->FindKeyByidLayer<XKeyPos>( idLayer
																												, SPROBJ->GetFrmCurr() );
			if( pPosKey == NULL ) {
				float x = spLayer->Getx();
				float y = spLayer->Gety();
				pPosKey = spAction->AddKeyPos( SPROBJ->GetFrmCurr(), spLayer, x, y );	// 없으면 새로만든다
				bCreateKey = TRUE;		// 키가 새로 생성되었음
			}
			// 드래그 시작한 최초시점에 언두스택에 역행동을 넣음
			if( m_nDragState == 1 )
			{
				if( bCreateKey )
					UNDOMNG->PushUndo( new XUndoCreateKey( SPROBJ, spAction, pPosKey ) );		// pPosKey키를 생성/삭제하는 언두행동을 언두스택에 푸쉬
				UNDOMNG->PushUndo( new XUndoKeyChange( SPROBJ, spActObj, pPosKey ), bCreateKey );	// 현재 pPosKey의 모든 변환값을 저장했다가 언두가 일어나면 그값으로 되돌림
			}
			m_nDragState = 2;		// 드래그 상태로 전환
			pPosKey->SetPos( GetPosL() );
//			pPosKey->SetPos( Getx(), Gety() );
			pPosKey->Execute( spActObj, spLayer );
			m_pDragKey = pPosKey;
		}
	}
}


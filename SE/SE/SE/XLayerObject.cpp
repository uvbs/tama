#include "stdafx.h"
#include "XLayerObject.h"
#include "SprObj.h"
#include "XAniAction.h"
#include "XKeyBase.h"
#include "SprObj.h"
#include "XActObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// XSPBaseLayer XLayerObject::CopyDeep()
// {
// 	return XSPBaseLayer( new XLayerObject( *this ) );
// }
XSPBaseLayer XLayerObject::CreateCopy( XSPActObjConst spActObjSrc ) const
{
	auto spNewLayer = XSPLayerObject( new XLayerObject( *this ) );
	spNewLayer->OnCreate();
	return spNewLayer;
}
void XLayerObject::Draw( float x, float y, const D3DXMATRIX &m, XSprObj *pParentSprObj )
{
	XSprObj *pSprObj = m_pSprObjCurr;
	if( pSprObj ) {
		auto vLocal = GetcnPos().m_vPos;
		pSprObj->SetvPos( vLocal );
		pSprObj->SetAdjustAxis( GetvAdjustAxis() );		// 회전축을 보정함
		pSprObj->SetRotateZ( GetcnRot().fAngle );
		pSprObj->SetScale( GetcnScale().vScale );
		pSprObj->SetFlipHoriz( ( GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ ) ? TRUE : FALSE );
		pSprObj->SetFlipVert( ( GetcnEffect().dwDrawFlag & EFF_FLIP_VERT ) ? TRUE : FALSE );
		pSprObj->SetDrawMode( GetcnEffect().DrawMode );
		pSprObj->SetfAlpha( GetcnEffect().fAlpha );
		// 바운딩박스
// 		if( TOOL->GetspSelLayer()
// 			&& TOOL->GetspSelLayer()->getid() == getid() ) {
// 			D3DXVECTOR4 vOut;
// 			auto vLocal = GetcnPos().vPos;
// 			D3DXVECTOR2 dvLocal = vLocal;
// 			D3DXVec2Transform( &vOut, &dvLocal, &m );
// 			XE::VEC2 vLT = vLocal + pSprObj->getadj
// 			XE::VEC2 vRB = vLT + pSprObj->GetSize();
// 			D3DXVECTOR2 dvLT = vLT;
// 			D3DXVECTOR2 dvRB = vRB;
// 			D3DXVec2Transform( &vOut, &dvLT, &m );		dvLT.x = vOut.x;	dvLT.y = vOut.y;
// 			D3DXVec2Transform( &vOut, &dvRB, &m );	dvRB.x = vOut.x;	dvRB.y = vOut.y;
// 			vLT = dvLT;
// 			vRB = dvRB;
// 			if( SPROBJ->IsPause() ) {
// 				XE::DrawRectangle( vLT, vRB, XCOLOR_WHITE );
// 			}
// 		}
		pSprObj->Draw( m );
	}
}
void XLayerObject::FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj )
{
	XLayerMove::FrameMove( dt, fFrmCurr, pSprObj );
	XSprObj *pSprObjCurr = m_pSprObjCurr;
	if( pSprObjCurr )
	{
		pSprObjCurr->FrameMove( dt );
	}
}

void XLayerObject::SetpSprObjCurr( XSprObj* pSprObj )
{
	m_pSprObjCurr = pSprObj;
	if( pSprObj ) {
		auto spActObj = pSprObj->GetspActObjCurr();
		if( spActObj )
			m_maxFrame = spActObj->GetMaxFrame();
// 		auto spAction = pSprObj->GetspActionCurr();
// 		if( spAction ) {
// 			m_maxFrame = spAction->GetfMaxFrame();
// 		}
	}
}

// void XLayerObject::CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj )
// {
// 	XLayerMove::FrameMove( dt, fFrmCurr, pSprObj );
// 	//
// 	XSprObj *pSprObjCurr = m_pSprObjCurr;
// 	if( pSprObjCurr ) {
// 		// child 오브젝트의 바운딩박스까지 반영된 액션전체 바운딩 박스를 구한다.
// 		// 차일드는 이미 spr파일에 계산된 바운딩박스값이 있다고 가정하고 읽기만 한다.
// 		XSPAction spActionChild = pSprObjCurr->GetspAction();
// 		if( XASSERT( spActionChild ) ) {
// 			XSPAction spAction = pSprObj->GetspAction();
// 			XBREAK( spAction == nullptr );
// 			if( spActionChild->IsHaveBoundBox() ) {
// 				D3DXVECTOR2 d3dvLT = spActionChild->GetBoundBoxLT();
// 				D3DXVECTOR2 d3dvRB = spActionChild->GetBoundBoxRB();
// 				D3DXVECTOR2 d3dvRT = D3DXVECTOR2( d3dvRB.x, d3dvLT.y );
// 				D3DXVECTOR2 d3dvLB = D3DXVECTOR2( d3dvLT.x, d3dvRB.y );
// 				// child바운딩박스를 오브젝레이어의 트랜스폼과 곱한다.
// 				XE::VEC2 vtLT, vtRT, vtLB, vtRB;
// 				D3DXVECTOR4 vResult;
// 				D3DXVec2Transform( &vResult, &d3dvLT, &GetMatrix() );	vtLT.Set( vResult.x, vResult.y );
// 				D3DXVec2Transform( &vResult, &d3dvRB, &GetMatrix() );	vtRB.Set( vResult.x, vResult.y );
// 				D3DXVec2Transform( &vResult, &d3dvRT, &GetMatrix() );	vtRT.Set( vResult.x, vResult.y );
// 				D3DXVec2Transform( &vResult, &d3dvLB, &GetMatrix() );	vtLB.Set( vResult.x, vResult.y );
// 				// 현재 액션의 바운딩박스에 반영
// 				if( XASSERT( spAction ) ) {
// 					spAction->UpdateBoundBox( vtLT );
// 					spAction->UpdateBoundBox( vtRT );
// 					spAction->UpdateBoundBox( vtLB );
// 					spAction->UpdateBoundBox( vtRB );
// 				}
// 			}
// 		}
// 	}
// }

XE::xRECT XLayerObject::GetBoundBox( const D3DXMATRIX& mParent ) const
{
	XSprObj *pSprObjCurr = m_pSprObjCurr;
	if( pSprObjCurr ) {
		// child 오브젝트의 바운딩박스까지 반영된 액션전체 바운딩 박스를 구한다.
		auto spActionChild = pSprObjCurr->GetspAction();
		if( XASSERT( spActionChild ) ) {
			if( spActionChild->IsHaveBoundBox() ) {
				// 네 귀퉁이 좌표를 만듬.
				D3DXVECTOR2 d3dvLT = spActionChild->GetBoundBoxLT();
				D3DXVECTOR2 d3dvRB = spActionChild->GetBoundBoxRB();
				D3DXVECTOR2 d3dvRT = D3DXVECTOR2( d3dvRB.x, d3dvLT.y );
				D3DXVECTOR2 d3dvLB = D3DXVECTOR2( d3dvLT.x, d3dvRB.y );
				// child바운딩박스를 오브젝레이어의 트랜스폼과 곱한다.
				XE::VEC2 vtLT, vtRT, vtLB, vtRB;
				D3DXVECTOR4 vResult;
				const auto mWorld = GetMatrix() * mParent;
				D3DXVec2Transform( &vResult, &d3dvLT, &mWorld );	vtLT.Set( vResult.x, vResult.y );
				D3DXVec2Transform( &vResult, &d3dvRB, &mWorld );	vtRB.Set( vResult.x, vResult.y );
				D3DXVec2Transform( &vResult, &d3dvRT, &mWorld );	vtRT.Set( vResult.x, vResult.y );
				D3DXVec2Transform( &vResult, &d3dvLB, &mWorld );	vtLB.Set( vResult.x, vResult.y );
				XE::xRECT rectBB;
				rectBB.UpdateBoundBox( vtLT );
				rectBB.UpdateBoundBox( vtRT );
				rectBB.UpdateBoundBox( vtLB );
				rectBB.UpdateBoundBox( vtRB );
				return rectBB;
			}
		}
	}
	return XE::xRECT();
}

// void XLayerObject::UpdateBoundBox( XSprObj *pSprObj )
// {
// 	XSprObj *pSprObjCurr = m_pSprObjCurr;
// 	if( pSprObjCurr ) {
// 		// child 오브젝트의 바운딩박스까지 반영된 액션전체 바운딩 박스를 구한다.
// 		XSPAction spActionChild = pSprObjCurr->GetspAction();
// 		if( XASSERT( spActionChild ) ) {
// 			XSPAction spAction = pSprObj->GetspAction();
// 			if( spActionChild->IsHaveBoundBox() ) {
// 				D3DXVECTOR2 d3dvLT = spActionChild->GetBoundBoxLT();
// 				D3DXVECTOR2 d3dvRB = spActionChild->GetBoundBoxRB();
// 				D3DXVECTOR2 d3dvRT = D3DXVECTOR2( d3dvRB.x, d3dvLT.y );
// 				D3DXVECTOR2 d3dvLB = D3DXVECTOR2( d3dvLT.x, d3dvRB.y );
// 				// child바운딩박스를 오브젝레이어의 트랜스폼과 곱한다.
// 				XE::VEC2 vtLT, vtRT, vtLB, vtRB;
// 				D3DXVECTOR4 vResult;
// 				D3DXVec2Transform( &vResult, &d3dvLT, &GetMatrix() );	vtLT.Set( vResult.x, vResult.y );
// 				D3DXVec2Transform( &vResult, &d3dvRB, &GetMatrix() );	vtRB.Set( vResult.x, vResult.y );
// 				D3DXVec2Transform( &vResult, &d3dvRT, &GetMatrix() );	vtRT.Set( vResult.x, vResult.y );
// 				D3DXVec2Transform( &vResult, &d3dvLB, &GetMatrix() );	vtLB.Set( vResult.x, vResult.y );
// 				// 현재 액션의 바운딩박스에 반영
// 				if( XASSERT( spAction ) ) {
// 					spAction->UpdateBoundBox( vtLT );
// 					spAction->UpdateBoundBox( vtRT );
// 					spAction->UpdateBoundBox( vtLB );
// 					spAction->UpdateBoundBox( vtRB );
// 				}
// 			}
// 		}
// 	}
// }

void XLayerObject::MoveFrame( float fFrmCurr, XSPActObjConst spActObj )
{
	XSprObj *psoChild = m_pSprObjCurr;
	if( psoChild ) {
		if( psoChild->GetpParentKey() ) {		// 어느키로부터 생성되었는가
			float fLocalCurrFrm = fFrmCurr - psoChild->GetpParentKey()->GetfFrame();
			if( fLocalCurrFrm >= 0 ) {		// 이경우엔 오브젝트생성키보다 앞프레임으로 이동한거기땜에 아직 오브젝트가 생성되지 않은상태라고 보는게 맞다 그래서 >= 0일때만 처리함
				// 초당 frm값
				float frmPerSecParent = XFPS * spActObj->GetSpeed();
				float secCurrParent = fFrmCurr / frmPerSecParent;	// 현재 플레이중인 위치의 sec
				psoChild->MoveFrameBySec( secCurrParent );
//				pSprObj->MoveFrame( fLocalCurrFrm );
			}
		}
	}
}
// lx, ly는 0,0기준 로컬좌표

DWORD XLayerObject::GetPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const
{
	XSprObj *pSprObjCurr = m_pSprObjCurr;
	if( pSprObjCurr ) {
// 		float lx = GetcnPos().vPos.x;
// 		float ly = GetcnPos().vPos.y;
		const auto vLocal = GetcnPos().m_vPos;
		pSprObjCurr->SetAdjustAxis( GetfAdjustAxisX(), GetfAdjustAxisY() );		// 회전축을 보정함
		pSprObjCurr->SetRotateZ( GetcnRot().fAngle );
		pSprObjCurr->SetScale( GetcnScale().vScale );
		pSprObjCurr->SetFlipHoriz( ( GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ ) ? TRUE : FALSE );
		pSprObjCurr->SetFlipVert( ( GetcnEffect().dwDrawFlag & EFF_FLIP_VERT ) ? TRUE : FALSE );
		DWORD pixel = pSprObjCurr->GetPixel( vLocal.x, vLocal.y, mx, my, m, pa, pr, pg, pb );
		pSprObjCurr->SetScale( 1.0f, 1.0f );
		pSprObjCurr->SetRotate( 0, 0, 0 );
		return pixel;
	}
	return 0;
}

DWORD XLayerObject::GetPixel( const D3DXMATRIX& mCamera,
															const D3DXVECTOR2& mv,
															BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const
{
	XSprObj *pSprObjCurr = m_pSprObjCurr;
	if( pSprObjCurr )
	{
		const XE::VEC2 vLocal = GetcnPos().m_vPos;
		pSprObjCurr->SetAdjustAxis( GetfAdjustAxisX(), GetfAdjustAxisY() );		// 회전축을 보정함
		pSprObjCurr->SetRotateZ( GetcnRot().fAngle );
		pSprObjCurr->SetScale( GetcnScale().vScale );
		pSprObjCurr->SetFlipHoriz( ( GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ ) ? TRUE : FALSE );
		pSprObjCurr->SetFlipVert( ( GetcnEffect().dwDrawFlag & EFF_FLIP_VERT ) ? TRUE : FALSE );
		D3DXMATRIX mWorld;
		pSprObjCurr->GetMatrix( &mWorld, vLocal.x, vLocal.y );
		D3DXMATRIX mInv;
		mWorld = mWorld * mCamera;
		D3DXMatrixInverse( &mInv, nullptr, &mWorld );		// 이제까지 반영된 모든 매트릭스를 역행렬로 만든다
		D3DXVECTOR4 vInv;
		D3DXVec2Transform( &vInv, &mv, &mInv );			// 최종 매트릭스 기준 좌표가 나온다.
		//		DWORD pixel = pSprObjCurr->GetPixel( vInv.x, vInv.y, pa, pr, pg, pb );
		auto spSel = pSprObjCurr->GetspActObjCurr()->GetLayerInPixel( mWorld, mv );
		if( spSel )
			*pa = 255;
		pSprObjCurr->SetScale( 1.0f, 1.0f );
		pSprObjCurr->SetRotate( 0, 0, 0 );
		return ( spSel ) ? 1 : 0;
		//		return pixel;
	}
	return 0;
}

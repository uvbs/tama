#include "stdafx.h"
#include "XLayerImage.h"
#include "Sprite.h"
#include "XAniAction.h"
#include "SprObj.h"
#include "Tool.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////
// XSPBaseLayer XLayerImage::CopyDeep()
// {
// 	return XSPBaseLayer( new XLayerImage( *this ) );
// }
XSPBaseLayer XLayerImage::CreateCopy( XSPActObjConst spActObjSrc ) const
{
	auto spNewLayer = xSPLayerImage( new XLayerImage( *this ) );
	spNewLayer->OnCreate();
	return spNewLayer;
}

/* 
바운딩 박스 계획
	. 게임에서 실시간으로 바운딩 박스를 계산하려면 시간이 걸린다.
	. 그러므로 툴에서 가능한크기의 바운딩박스를 액션마다 미리 구해놓고 게임에선 읽어다 쓰기만 한다.
	. 툴에서...
	  . XLayerImage, XLayerObject 의 FrameMove에서 액션쪽에 실시간으로 바운딩박스를 갱신.
	  . 이미지 크기는 물론이고 pos, rot, scale키를 모두 반영한 바운딩 박스여야 한다.
	  . 기존파일들에 바운딩박스 정보를 넣어주려면 컨버트 같은 방법을 한번은 동원해야 한다.
	  . 파일이 세이브될때 한번 계산해주던지...
*/
void XLayerImage::FrameMove( float dt, float fFrmCurr, XSprObj *pSprObj )
{
 	XLayerMove::FrameMove( dt, fFrmCurr, pSprObj );
//	UpdateBoundBox( pSprObj );
}

// void XLayerImage::CalcBoundBox( float dt, float fFrmCurr, XSprObj *pSprObj )
// {
// 	XLayerMove::FrameMove( dt, fFrmCurr, pSprObj );
// 	//
// 	UpdateBoundBox( pSprObj );
// }

// void XLayerImage::UpdateBoundBox( XSprObj *pSprObj )
// {
// 	// 이미지의 4귀퉁이를 이 매트릭스로 트랜스폼
// 	XSprite *pSpr = m_pSpriteCurr;
// 	if( pSpr )
// 	{
// 		D3DXVECTOR2 vLT = pSpr->GetAdjust();
// 		D3DXVECTOR2 vRT = D3DXVECTOR2( pSpr->GetAdjustX() + pSpr->GetWidth(), pSpr->GetAdjustY() );
// 		D3DXVECTOR2 vLB = D3DXVECTOR2( pSpr->GetAdjustX(), pSpr->GetAdjustY() + pSpr->GetHeight() );
// 		D3DXVECTOR2 vRB = D3DXVECTOR2( pSpr->GetAdjustX() + pSpr->GetWidth(), pSpr->GetAdjustY() + pSpr->GetHeight() );
// 		XE::VEC2 vtLT, vtRT, vtLB, vtRB;
// 		D3DXVECTOR4 vResult;
// 		D3DXVec2Transform( &vResult, &vLT, &GetMatrix() );		vtLT.Set( vResult.x, vResult.y );
// 		D3DXVec2Transform( &vResult, &vRT, &GetMatrix() );	vtRT.Set( vResult.x, vResult.y );
// 		D3DXVec2Transform( &vResult, &vLB, &GetMatrix() );		vtLB.Set( vResult.x, vResult.y );
// 		D3DXVec2Transform( &vResult, &vRB, &GetMatrix() );	vtRB.Set( vResult.x, vResult.y );
// 		// 변환된 좌표로 바운딩 박스 갱신.
// 		XSPAction spAction = pSprObj->GetspAction();
// 		if( XASSERT(spAction) ) {
// 			spAction->UpdateBoundBox( vtLT );
// 			spAction->UpdateBoundBox( vtRT );
// 			spAction->UpdateBoundBox( vtLB );
// 			spAction->UpdateBoundBox( vtRB );
// 		}
// 	}
// }

/**
 @brief 바운딩박스를 계산해 돌려준다.
*/
XE::xRECT XLayerImage::GetBoundBox( const D3DXMATRIX& mParent ) const 
{
	// 이미지의 4귀퉁이를 이 매트릭스로 트랜스폼
	XSprite *pSpr = m_pSpriteCurr;
	if( pSpr ) {
		const D3DXVECTOR2 vAdj = pSpr->GetAdjust();
		const auto vSize = pSpr->GetSize();
		D3DXVECTOR2 vLT = vAdj;
		D3DXVECTOR2 vRT = D3DXVECTOR2( vAdj.x + vSize.w, vAdj.y );
		D3DXVECTOR2 vLB = D3DXVECTOR2( vAdj.x					, vAdj.y + vSize.h );
		D3DXVECTOR2 vRB = D3DXVECTOR2( vAdj.x + vSize.w, vAdj.y + vSize.h );
		XE::VEC2 vtLT, vtRT, vtLB, vtRB;
		D3DXVECTOR4 vResult;
		const auto mWorld = GetMatrix() * mParent;
		D3DXVec2Transform( &vResult, &vLT, &mWorld );	vtLT.Set( vResult.x, vResult.y );
		D3DXVec2Transform( &vResult, &vRT, &mWorld );	vtRT.Set( vResult.x, vResult.y );
		D3DXVec2Transform( &vResult, &vLB, &mWorld );	vtLB.Set( vResult.x, vResult.y );
		D3DXVec2Transform( &vResult, &vRB, &mWorld );	vtRB.Set( vResult.x, vResult.y );
		XE::xRECT rectBB;
		rectBB.UpdateBoundBox( vtLT );
		rectBB.UpdateBoundBox( vtRT );
		rectBB.UpdateBoundBox( vtLB );
		rectBB.UpdateBoundBox( vtRB );
		return rectBB;
	}
	return XE::xRECT();
}

void XLayerImage::Draw( float x, float y, const D3DXMATRIX &m, XSprObj *pParentSprObj )
{
	XSprite *pSpr = m_pSpriteCurr;
	if( pSpr )
	{
		const XE::VEC2 vLocal = GetcnPos().m_vPos;
		pSpr->SetAdjustAxis( GetfAdjustAxisX(), GetfAdjustAxisY() );		// 회전축을 보정함
		pSpr->SetRotateZ( GetcnRot().fAngle );
		pSpr->SetScale( GetcnScale().vScale );
		pSpr->SetFlipHoriz( (GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE );
		pSpr->SetFlipVert( (GetcnEffect().dwDrawFlag & EFF_FLIP_VERT) ? TRUE : FALSE );
		pSpr->SetDrawMode( GetcnEffect().DrawMode );
		pSpr->SetfAlpha( GetcnEffect().fAlpha );
		//
		if( TOOL->GetspSelLayer()
			&& TOOL->GetspSelLayer()->getid() == getid() ) {
			D3DXVECTOR4 vOut;
			D3DXVECTOR2 dvLocal = vLocal;
			D3DXVec2Transform( &vOut, &dvLocal, &m ); 
			XE::VEC2 vLT = vLocal + pSpr->GetAdjust();
			XE::VEC2 vRB = vLT + pSpr->GetSize();
			D3DXVECTOR2 dvLT = vLT;
			D3DXVECTOR2 dvRB = vRB;
			D3DXVec2Transform( &vOut, &dvLT, &m );		dvLT.x = vOut.x;	dvLT.y = vOut.y;
			D3DXVec2Transform( &vOut, &dvRB, &m );	dvRB.x = vOut.x;	dvRB.y = vOut.y;
			vLT = dvLT;
			vRB = dvRB;
			if( SPROBJ->IsPause() ) {
				XE::DrawRectangle( vLT, vRB, XCOLOR_WHITE );
			}
		}
		//
		pSpr->Draw( vLocal, m );
	}
}
// lx, ly는 0,0기준 로컬좌표
DWORD XLayerImage::GetPixel( float cx, float cy, float mx, float my, const D3DXMATRIX &m, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const
{
	XSprite *pSpr = m_pSpriteCurr;
	if( pSpr )
	{
		XE::VEC2 vLocal = GetcnPos().m_vPos;
//		float lx = GetcnPos().x;
//		float ly = GetcnPos().y;
		pSpr->SetAdjustAxis( GetvAdjustAxis() );		// 회전축을 보정함
		pSpr->SetRotateZ( GetcnRot().fAngle );
		pSpr->SetScale( GetcnScale().vScale );
		pSpr->SetFlipHoriz( (GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ) ? TRUE : FALSE );
		pSpr->SetFlipVert( (GetcnEffect().dwDrawFlag & EFF_FLIP_VERT) ? TRUE : FALSE );
//		DWORD pixel = pSpr->GetPixel( lx, ly, m, pa, pr, pg, pb );
		D3DXMATRIX mWorld;
		pSpr->GetMatrix( &mWorld, vLocal.x, vLocal.y );		
//		mWorld *= m;			// 부모의행렬과 스프라이트의 로컬행렬을곱함
		D3DXMATRIX mInv;
//		D3DXMatrixInverse( &mInv, nullptr, &mWorld );		// 이제까지 반영된 모든 매트릭스를 역행렬로 만든다
		D3DXMatrixInverse( &mInv, nullptr, &m );		// 이제까지 반영된 모든 매트릭스를 역행렬로 만든다
		D3DXVECTOR2 v = D3DXVECTOR2( mx, my );		// 마우스 클릭한 좌표
		D3DXVECTOR4 vInv;
		D3DXVec2Transform( &vInv, &v, &mInv );			// 최종 매트릭스 기준 좌표가 나온다.
		DWORD pixel = pSpr->GetPixel( vInv.x, vInv.y, pa, pr, pg, pb );
		pSpr->SetScale( 1.0f, 1.0f );
		pSpr->SetRotate( 0, 0, 0 );
		return pixel;
	}
	return 0;
}

DWORD XLayerImage::GetPixel( const D3DXMATRIX& mCamera, 
							const D3DXVECTOR2& mv, 
							BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) const
{
	XSprite *pSpr = m_pSpriteCurr;
	if( pSpr )
	{
		const XE::VEC2 vLocal = GetcnPos().m_vPos;
		pSpr->SetAdjustAxis( GetfAdjustAxisX(), GetfAdjustAxisY() );		// 회전축을 보정함
		pSpr->SetRotateZ( GetcnRot().fAngle );
		pSpr->SetScale( GetcnScale().vScale );
		pSpr->SetFlipHoriz( ( GetcnEffect().dwDrawFlag & EFF_FLIP_HORIZ ) ? TRUE : FALSE );
		pSpr->SetFlipVert( ( GetcnEffect().dwDrawFlag & EFF_FLIP_VERT ) ? TRUE : FALSE );
		D3DXMATRIX mWorld;
		pSpr->GetMatrix( &mWorld, vLocal.x, vLocal.y );
		D3DXMATRIX mInv;
		mWorld = mWorld * mCamera;
		D3DXMatrixInverse( &mInv, nullptr, &mWorld );		// 이제까지 반영된 모든 매트릭스를 역행렬로 만든다
		D3DXVECTOR4 vInv;
		D3DXVec2Transform( &vInv, &mv, &mInv );			// 최종 매트릭스 기준 좌표가 나온다.
		DWORD pixel = pSpr->GetPixel( vInv.x, vInv.y, pa, pr, pg, pb );
		pSpr->SetScale( 1.0f, 1.0f );
		pSpr->SetRotate( 0, 0, 0 );
		return pixel;
	}
	return 0;
}
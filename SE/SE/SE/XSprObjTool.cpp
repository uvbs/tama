#include "StdAfx.h"
#include "XSprObjTool.h"
#include "AllView.h"
#include "Tool.h"
#include "XAniAction.h"
#include "XKeyBase.h"

// XLayerDummy* XObjActTool::AddDummyLayer() 
// {
// 	// 더미레이어를 추가
// 	XLayerDummy *spLayer = XObjAct::AddDummyLayer();
// 	// 더미레이어의 앵커를 애니메이션뷰에서 쓸수있도록 등록
// 	GetAnimationView()->RegisterMI( spLayer->GetpMICrossDir() );
// 	return spLayer;
// }
// 
// void XObjActTool::DrawLayerLabel( float left, float top, SPBaseLayer spPushLayer )
// {
// 	D3DCOLOR colWhite = XCOLOR_WHITE;
// 	auto pSelected = TOOL->GetspSelLayer();
// 	GetNextLayerClear();
// 	while( SPBaseLayer spLayer = GetNextLayer() ) {
// 		bool bHide = spLayer->GetbShow() == FALSE;
// 		XCOLOR col = XCOLOR_WHITE;
// 		// 레이어가 눌린상태면 바탕색 다르게
// 		if( pPushLayer == spLayer )
// 			XE::FillRectangle( 0, top + spLayer->Getscry() + 1, TRACK_X, top + spLayer->Getscry() + 1 + KEY_HEIGHT, XCOLOR_WHITE );	
// 		// + 표시
// 		if( spLayer->GetbAbleOpen() ) {
// 			float top2 = top + spLayer->Getscry() + 5;
// 			XE::DrawRectangle( 0, top2, 8, top2+8, XCOLOR_WHITE );							// 박스
// 			XE::DrawLine( 0, top2+4, 8, top2+4, XCOLOR_WHITE );		// 가로선
// 			if( spLayer->GetbOpen() == FALSE ) {	// 닫혀있는상태 +표시
// 				XE::DrawLine( 0+4, top2, 0+4, top2+8, XCOLOR_WHITE );		// 세로선
// 			}
// 		}
// 		// label 표시
// 		if( pPushLayer == spLayer )
// 			col = XCOLOR_BLACK;
// 		else {
// 			if( bHide )
// 				col = XCOLOR_LIGHTGRAY;
// 			else
// 			if( spLayer == pSelected )
// 				col = XCOLOR_YELLOW;
// 		}
// 		spLayer->DrawLabel( left, top, col );
// 	}
// }
// vPos좌표가 어떤키에 속하는지 검사
XBaseKey* XSprObjTool::GetInsideKey( const XE::VEC2& vPos, float fViewScale ) const 
{	
	if( GetspAction() == nullptr )		return nullptr;
	XBaseKey *pKey = nullptr;
	auto itorKey = GetspAction()->GetNextKeyFirst();
	while( pKey = GetspAction()->GetNextKey( &itorKey ) )	{
		if( pKey->IsInside( vPos, fViewScale ) )	// 키를 클릭했는가
			return pKey;
	}
	return nullptr;
}

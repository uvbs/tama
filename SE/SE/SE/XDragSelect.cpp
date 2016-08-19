#include "StdAfx.h"
#include "XDragSelect.h"
#include "XGraphics.h"

// pObj는 단순 작은 사각형일수도 있고 불특정모양의 이미지일수도 있다
// 단순사각형의 경우는 XDragSelect의 디폴트 멤버를 쓰면 되고
// 다른방식의 셀렉트가 필요하다면 하위클래스에서 이것을 구현해야 한다.
BOOL XDragSelect::SelectObjInArea( XBaseElem *pObj, const XE::VEC2& vPoint )
{
	if( pObj == nullptr )		return FALSE;
	//
	XBREAK( m_sizeObj.IsZero() );
	XE::VEC2 vLT = Projection( pObj->GetPos() ) - m_sizeObj / 2.0f;
	if( XE::IsArea( vLT, m_sizeObj, vPoint ) )
		return TRUE;
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int XDragSelect::OnLButtonDown( UINT nFlags, const XE::VEC2& vMouse )
{
	// 어떤 셀렉트오브젝트를 클릭했는지 검사
	m_idPushObj = 0;
	XBaseElem *pObj = nullptr;
	SelectGetNextClear();	// 하위클래스에서 구현해야 함
	while( pObj = SelectGetNext() )		// 모든 셀렉트 대상의 루프를 돈다
	{
		if( SelectObjInArea( pObj, vMouse ) )	// vMouse좌표가  pObj를 클릭했는지 검사
		{
			m_idPushObj = pObj->GetidObj();
			m_nDragState = 1;	// L down
			break;
		}
	}
	// 오브젝트를 클릭하지 않았다면 영역드래그 모드로...
	if( m_idPushObj == 0 )
	{
		m_nActive = 1;
		m_vStart = m_vEnd = vMouse;
	} else
		return 0;
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XDragSelect::SelectOnDragOnly( XBaseElem *pObj, const XE::VEC2& vObjDist )
{
	pObj->SetPos( pObj->GetPos() + vObjDist );
}
static XE::VEC2 vPrevMouse;
int XDragSelect::OnMouseMove( UINT nFlags, const XE::VEC2& vMouse )
{
	int retv = 1;
	if( m_idPushObj )	// 오브젝트가 눌린상태에서 드래그했다
	{
		if( m_nDragState == 1 )	// 드래그한 순간
		{
			// 선택안된 오브젝트를 드래그했으면 다 없애고 그 오브젝트만 다시 선택한다.
			if( !m_listSelectObj.Find( m_idPushObj ) ) {
				m_listSelectObj.clear();		// 셀렉트 된 키들을 모두 선택해제
				m_listSelectObj.Add( m_idPushObj );		// 클릭한 오브젝트만 추가
			}
			m_nDragState = 2;
		}
		if( nFlags & MK_CONTROL )
		{
			m_DragState = xDRAG_CONTROL;
		} else
		if( nFlags & MK_SHIFT )
		{
			m_DragState = xDRAG_SHIFT;
		} else
			m_DragState = xDRAG_ONLY;
		// 셀렉트된 키들에게 DragOnly명령 전달
		if( m_DragState == xDRAG_ONLY ) {
			for( auto idObj : m_listSelectObj ) {
				XE::VEC2 vObjDist = UnProjection( vMouse ) - UnProjection( vPrevMouse );	// 마우스 움직인 양
//				XE::VEC2 vObjDist = vMouse - vPrevMouse;
//				XE::VEC2 vObjDist = UnProjection( vDist );	// 오브젝트 좌표단위로 움직인 양
				XBaseElem *pObj = GetSelectObj( idObj );
				if( pObj )
					SelectOnDragOnly( pObj, vObjDist );
			}
		}
		retv = 0;		// 오브젝트 드래그중에는 뷰::OnMouseMove()에서 다른행동을 못하도록 0을 리턴
	} else {
		if( m_nActive )
			m_vEnd = vMouse;
	}
	vPrevMouse = vMouse;
	return retv;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 오브젝트를 직접 클릭한 경우의 처리
int XDragSelect::ProcessObjLButtonUp( UINT nFlags, const XE::VEC2& vMouse )
{
	if( nFlags & MK_CONTROL )
	{
		if( m_listSelectObj.Find( m_idPushObj ) )
			m_listSelectObj.Del( m_idPushObj );
		else
			m_listSelectObj.Add( m_idPushObj );
	} 
	else
	{
		// 그냥 클릭
		m_listSelectObj.clear();		// 셀렉트 된 키들을 모두 선택해제
		m_listSelectObj.Add( m_idPushObj );		// 클릭한 오브젝트만 추가
	}
	m_idPushObj = 0;
	m_nDragState = 0;
	m_DragState = xDRAG_NONE;
	return 0;
}

// 영역드래그를 해서 버튼을 뗀경우
int XDragSelect::ProcessDragAreaLButtonUp( UINT nFlags, const XE::VEC2& vMouse )
{
	if( m_nActive == 0 )	return 0;
	if( nFlags & MK_CONTROL )
	{
		XBaseElem *pObj = nullptr;
		SelectGetNextClear();	// 하위클래스에서 구현해야 함
		while( pObj = SelectGetNext() )		// 모든 셀렉트 대상의 루프를 돈다
		{
			XE::VEC2 vScr = Projection( pObj->GetPos() );		// 이 메소드를 상속받은 클래스는 주어진 좌표를 해당뷰에 그릴때 화면좌표를 계산해서 돌려줘야 한다
			if( XE::IsInside( vScr, m_vStart, m_vEnd, m_sizeObj ) )	// vStart-vEnd사각영역에 pObj가 들어가는지?
			{
				if( m_listSelectObj.Find( pObj->GetidObj() ) )	// 이미 선택되어 있는 키는 선택해제한다.
					m_listSelectObj.Del( pObj->GetidObj() );
				else
					m_listSelectObj.Add( pObj->GetidObj() );	// 선택목록에 추가시킨다
			}
		}
	} else
	{
		// 컨트롤 누르지 않고 그냥 드래그
		m_listSelectObj.clear();
		XBaseElem *pObj = nullptr;
		SelectGetNextClear();	// 하위클래스에서 구현해야 함
		while( pObj = SelectGetNext() )
		{
//			XBREAK( m_sizeObj.IsZero() == TRUE );
			XE::VEC2 vScr = Projection( pObj->GetPos() );		// 이 메소드를 상속받은 클래스는 주어진 좌표를 해당뷰에 그릴때 화면좌표를 계산해서 돌려줘야 한다
			if( XE::IsInside( vScr, m_vStart, m_vEnd, m_sizeObj ) )	// vStart-vEnd사각영역에 pObj가 들어가는지?
				m_listSelectObj.Add( pObj->GetidObj() );
		}
	}
	m_nActive = 0;	// 영역 드래그 모드 해제
	return 1;
}

int XDragSelect::OnLButtonUp( UINT nFlags, const XE::VEC2& vMouse )
{
	if( m_idPushObj )	
	{	
		// 오브젝트를 직접 클릭한 경우의 처리
		return ProcessObjLButtonUp( nFlags, vMouse );
	} else
	{
		// 영역드래그를 해서 버튼을 뗀경우
		return ProcessDragAreaLButtonUp( nFlags, vMouse );
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 드래그 영역 그려줌
void XDragSelect::Draw()
{
	if( m_nActive == 0 )	return;
	XE::DrawRectangle( m_vStart, m_vEnd, XCOLOR_RED, 2.0f );
}


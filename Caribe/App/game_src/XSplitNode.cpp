#include "stdafx.h"
#include "XSplitNode.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XE_NAMESPACE_START( xSplit )

void XNode::Destroy()
{
	SAFE_DELETE( m_Child[0] );
	SAFE_DELETE( m_Child[1] );
}
// id를 가진 이미지(크기를 가진)를 트리에 삽입한다.
XNode* XNode::Insert( ID idImg, const XE::VEC2& sizeImg )
{
	if( !IsLeaf() ) {
		auto pNewNode = m_Child[0]->Insert( idImg, sizeImg );
		if( pNewNode ) {
			XBREAK( m_Child[0]->GetRect().vRB.x > 255.f || m_Child[0]->GetRect().vRB.y > 255.f );
			XBREAK( m_Child[1]->GetRect().vRB.x > 255.f || m_Child[1]->GetRect().vRB.y > 255.f );
			return pNewNode;
		}
		XBREAK( m_Child[0]->GetRect().vRB.x > 255.f || m_Child[0]->GetRect().vRB.y > 255.f );
		XBREAK( m_Child[1]->GetRect().vRB.x > 255.f || m_Child[1]->GetRect().vRB.y > 255.f );
		// 방이 없으면 두번째 방에 삽입한다.
		return m_Child[1]->Insert( idImg, sizeImg );
	} else {
		if( m_idImg ) {
			return nullptr;
		}
		const auto wImg = sizeImg.w;
		const auto hImg = sizeImg.h;
		const auto wRect = m_Rect.GetWidth();
		const auto hRect = m_Rect.GetHeight();
//		XBREAK( (int)wRect <= 1 || (int)hRect <= 1 );
		// 삽입할 이미지가 노드크기보다 크면 널리턴
		if( (sizeImg.w > wRect || sizeImg.h > hRect) ) {
//				&& ((int)wRect > 1 && (int)hRect > 1) ) {
			return nullptr;
		}
		// ??
		if( IsFitPerfectly( sizeImg ) ) {
			return this;
		}
		m_Child[0] = new XNode();
		m_Child[1] = new XNode();
		const XE::VEC2 vd( wRect - wImg, hRect - hImg );
		if( vd.w > vd.h ) {
			m_Child[0]->SetRect( m_Rect.vLT, XE::VEC2( m_Rect.Left() + wImg - 1.f, m_Rect.Bottom() ) );
			m_Child[1]->SetRect( XE::VEC2( m_Rect.Left() + wImg, m_Rect.Top() ), m_Rect.vRB );
			XBREAK( m_Child[0]->GetRect().vRB.x > 255.f || m_Child[0]->GetRect().vRB.y > 255.f );
			XBREAK( m_Child[1]->GetRect().vRB.x > 255.f || m_Child[1]->GetRect().vRB.y > 255.f );
		} else {
			m_Child[0]->SetRect( m_Rect.vLT, XE::VEC2(m_Rect.Right(), m_Rect.Top()+hImg-1.f) );
			m_Child[1]->SetRect( XE::VEC2(m_Rect.Left(), m_Rect.Top()+hImg), m_Rect.vRB );
			XBREAK( m_Child[0]->GetRect().vRB.x > 255.f || m_Child[0]->GetRect().vRB.y > 255.f );
			XBREAK( m_Child[1]->GetRect().vRB.x > 255.f || m_Child[1]->GetRect().vRB.y > 255.f );
		}
		return m_Child[0]->Insert( idImg, sizeImg );
	}
}

XE_NAMESPACE_END;	// xSplit

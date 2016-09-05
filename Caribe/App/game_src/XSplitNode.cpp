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

// id를 가진 이미지(크기를 가진)를 트리에 삽입한다.
XNode* XNode::Insert( ID idImg, const XE::VEC2& sizeImg )
{
	if( IsLeaf() ) {

	} else {
		if( m_idImg ) {
			return nullptr;
		}
		const auto wImg = sizeImg.w;
		const auto hImg = sizeImg.h;
		const auto wRect = m_Rect.GetWidth();
		const auto hRect = m_Rect.GetHeight();
		// 삽입할 이미지가 노드크기보다 크면 널리턴
		if( sizeImg.w >= wRect && sizeImg.h >= hRect ) {
			return nullptr;
		}
		// ??
		if( IsFitPerfectly( sizeImg ) ) {
			return this;
		}
		m_Child[0] = new XNode();
		m_Child[1] = new XNode();
		const XE::VEC2 vd( wRect - sizeImg.w, hRect - sizeImg.h );
		if( vd.w > vd.h ) {
			const XE::VEC2 sizeSpace = XE::VEC2( wImg, m_Rect.GetHeight() );
		} else {
			const XE::VEC2 sizeSpace = XE::VEC2( wRect, hImg );
			m_Child[0]->SetRect( XE::xRECT( m_Rect.vLT, sizeSpace ) );
			m_Child[1]->SetRect( XE::xRECT()
		}
	}
}

XE_NAMESPACE_END;	// xSplit

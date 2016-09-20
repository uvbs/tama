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
XNode* XNode::Insert( const XE::VEC2& sizeImg )
{
	if( !IsLeaf() ) {
		auto pNewNode = m_Child[0]->Insert( sizeImg );
		if( pNewNode ) {
			return pNewNode;
		}
		// 방이 없으면 두번째 방에 삽입한다.
		return m_Child[1]->Insert( sizeImg );
	} else {
		if( m_idImg ) {
			return nullptr;
		}
		const auto wImg = sizeImg.w + 1.f;		// 1픽셀 여유를 둔 값으로 비교
		const auto hImg = sizeImg.h + 1.f;
		const auto wRect = m_Rect.GetWidth();
		const auto hRect = m_Rect.GetHeight();
		// 삽입할 이미지가 노드크기보다 크면 널리턴
		if( (wImg > wRect || hImg > hRect) ) {
			return nullptr;
		}
		// ??
		if( IsFitPerfectly( XE::VEC2(wImg, hImg) ) ) {
			return this;
		}
		m_Child[0] = new XNode();
		m_Child[1] = new XNode();
		// 현재 영역에서 그림크기를 뺀 가로세로 크기
		const XE::VEC2 vd( wRect - wImg, hRect - hImg );
		if( vd.w > vd.h ) {
			// 남은 영역의 가로가 더 큼
			// 가로로 분할(1픽셀 더 큰크기로 분할함)
			m_Child[0]->SetRect( m_Rect.vLT, XE::VEC2( m_Rect.Left() + wImg - 1.f, m_Rect.Bottom() ) );
			m_Child[1]->SetRect( XE::VEC2( m_Rect.Left() + wImg, m_Rect.Top() ), m_Rect.vRB );
		} else {
			// 세로로 분할
			// 첫째구역은 그림 사이즈
			m_Child[0]->SetRect( m_Rect.vLT, XE::VEC2(m_Rect.Right(), m_Rect.Top()+ hImg -1.f) );
			// 둘째구역은 나머지 크기
			m_Child[1]->SetRect( XE::VEC2(m_Rect.Left(), m_Rect.Top()+ hImg), m_Rect.vRB );
		}
		return m_Child[0]->Insert( sizeImg );		// 리커시브 할때는 +1픽셀한것이 아닌 오리지널 크기를 넘겨야 한다.
	}
}

void XNode::ResizeRoot( const XE::VEC2& sizeNew )
{
	const auto sizePrev = m_Rect.GetSize();
	m_Rect.SetSize( sizeNew );		// 일단 루트의 크기를 바꾸고
	RecusiveResize( sizeNew, sizePrev );
}

void XNode::RecusiveResize( const XE::VEC2& sizeNewRoot,
														const XE::VEC2& sizePrevRoot )
{
	// 자식들의 분할공간 크기 변경
	for( int i = 0; i < 2; ++i ) {
		auto pChild = m_Child[i];
		if( pChild ) {
			//XBREAK( vrbPrev > )
			// 차일드 분할공간의 우측좌표가 이전 맥스치였다면 새 맥스치값으로 변경
			if( (int)pChild->m_Rect.vRB.x == (int)sizePrevRoot.w - 1 )
				pChild->m_Rect.vRB.x = sizeNewRoot.w - 1.f;
			if( (int)pChild->m_Rect.vRB.y == (int)sizePrevRoot.h - 1 )
				pChild->m_Rect.vRB.y = sizeNewRoot.h - 1.f;
			pChild->RecusiveResize( sizeNewRoot, sizePrevRoot );
		}
	}
}
XE_NAMESPACE_END;	// xSplit

/********************************************************************
	@date:	동적 텍스쳐 아틀라스를 위한 공간 분할
	@file: 	
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once

#include "etc/Global.h"
#include "XE.h"

XE_NAMESPACE_START( xSplit )
class XNode;
// 벽(?)과 공간을 나타내는 노드
class XNode {
	static float c_spaceBorder;		// 아틀라스간 경계 너비
public:
	XNode() {
		memset( m_Child, 0, sizeof( m_Child ) );
	}
	XNode( const XE::VEC2& size ) 
		: m_Rect( XE::VEC2(0), size ) {
		memset( m_Child, 0, sizeof( m_Child ) );
	}
	~XNode() { Destroy(); }
	//
	void Destroy();
//	GET_ACCESSOR_CONST( const XE::xRECT&, Rect );
	// 실제 텍스쳐의 크기
	const XE::xRECT GetRectTex() const {
		const auto vSize = m_Rect.GetSize() - c_spaceBorder;		// 1픽셀 여유를 줬으므로 실제 텍스쳐의 크기는 -1을 해야한다.
		return XE::xRECT( m_Rect.vLT, vSize);	// 텍스쳐간에 1픽셀씩 거리를 둠.
	}
	GET_SET_ACCESSOR_CONST( ID, idImg );
	GET_SET_ACCESSOR_CONST( ID, idNode );
	inline void SetRect( const XE::VEC2& vLT, const XE::VEC2& vRB ) {
		m_Rect.vLT = vLT;
		m_Rect.vRB = vRB;
	}
	void ResizeRoot( const XE::VEC2& sizeNew );
	//
	XNode* Insert( const XE::VEC2& sizeImg );
	// 더이상 차일드가 없는지(잎사귀)
	inline bool IsLeaf() const {
		return !m_Child[0] && !m_Child[1];
	}
	// 삽입하려는 이미지가 공간에 딱맞아서 분할할게 없는지.
	inline bool IsFitPerfectly( const XE::VEC2& sizeImg ) const {
		return (int)m_Rect.GetWidth() == (int)sizeImg.w
			&& (int)m_Rect.GetHeight() == (int)sizeImg.h;
	}
	XNode* ReleaseNode( ID idNode );
	void Clear();
private:
	GET_ACCESSOR_CONST( const XE::xRECT&, Rect );
	void RecusiveResize( const XE::VEC2& sizeNewRoot, const XE::VEC2& sizePrevRoot );
private:
	XNode* m_Child[2];
	XE::xRECT m_Rect;			// 분할전 전체 공간의 위치와 크기
	ID m_idImg = 0;
	ID m_idNode = 0;		// 배치가 이뤄진 각 노드의 고유번호
};


XE_NAMESPACE_END;

XE_NAMESPACE_START( XE )
XE_NAMESPACE_END; // XE


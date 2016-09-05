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
public:
	XNode() {	}
	~XNode() { Destroy(); }
	//
	void Destroy();
	GET_ACCESSOR_CONST( const XE::xRECT&, Rect );
	GET_SET_ACCESSOR_CONST( ID, idImg );
	inline void SetRect( const XE::VEC2& vLT, const XE::VEC2& vRB ) {
		m_Rect.vLT = vLT;
		m_Rect.vRB = vRB;
	}
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
private:
	XNode* m_Child[2] = {nullptr, nullptr};
	XE::xRECT m_Rect;			// 분할전 전체 공간의 위치와 크기
	ID m_idImg = 0;
};


XE_NAMESPACE_END;

XE_NAMESPACE_START( XE )
XE_NAMESPACE_END; // XE


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
	XNode() {}
	~XNode() {}
	//
	GET_SET_ACCESSOR_CONST( const XE::xRECT&, Rect );
	//
	XNode* Insert( ID idImg, const XE::VEC2& sizeImg );
	inline bool IsLeaf() const {
		return aaa;
	}
	// ??
	inline bool IsFitPerfectly( const XE::VEC2& sizeImg ) const {
		return bbb;
	}
private:
	XNode* m_Child[2] = {nullptr, nullptr};
	XE::xRECT m_Rect;			// 분할전 전체 공간의 위치와 크기
	ID m_idImg = 0;
};


XE_NAMESPACE_END;
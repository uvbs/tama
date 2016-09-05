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
	XNode* Insert( ID idImg, const XE::VEC2& sizeImg );
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

// XE_NAMESPACE_START( XE )
// struct xRect2 {
// 	XE::VEC2 vLT;
// 	XE::VEC2 vRB;
// 	xRect2() {}
// 	xRect2( int left, int top, int right, int bottom )
// 		: vLT( left, top ), vRB( right, bottom ) { }
// 	xRect2( float left, float top, float right, float bottom )
// 		: vLT( left, top ), vRB( right, bottom ) { }
// 	xRect2( const XE::VEC2& _vLT, const XE::VEC2& _vRB )
// 		: vLT( _vLT ), vRB( _vRB ) {}
// 	//
// 	xRect2& operator += ( const xRect2& rhs ) {
// 		vLT += rhs.vLT;
// 		vRB += rhs.vRB;
// 		return *this;
// 	}
// 	xRect2& operator -= ( const xRect2& rhs ) {
// 		vLT -= rhs.vLT;
// 		vRB -= rhs.vRB;
// 		return *this;
// 	}
// 	xRect2& operator *= ( const xRect2& rhs ) {
// 		vLT *= rhs.vLT;
// 		vRB *= rhs.vRB;
// 		return *this;
// 	}
// 	xRect2& operator /= ( const xRect2& rhs ) {
// 		vLT /= rhs.vLT;
// 		vRB /= rhs.vRB;
// 		return *this;
// 	}
// 	xRect2& operator *= ( float num ) {
// 		vLT *= num;
// 		vRB *= num;
// 		return *this;
// 	}
// 	xRect2& operator /= ( float num ) {
// 		vLT /= num;
// 		vRB /= num;
// 		return *this;
// 	}
// 	xRect2 operator + ( const xRect2& rhs ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT + rhs.vLT;
// 		ret.vRB = vRB + rhs.vRB;
// 		return ret;
// 	}
// 	xRect2 operator - ( const xRect2& rhs ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT - rhs.vLT;
// 		ret.vRB = vRB - rhs.vRB;
// 		return ret;
// 	}
// 	xRect2 operator * ( const xRect2& rhs ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT * rhs.vLT;
// 		ret.vRB = vRB * rhs.vRB;
// 		return ret;
// 	}
// 	xRect2 operator / ( const xRect2& rhs ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT / rhs.vLT;
// 		ret.vRB = vRB / rhs.vRB;
// 		return ret;
// 	}
// 	xRect2 operator + ( const VEC2& rhs ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT + rhs;
// 		ret.vRB = vRB + rhs;
// 		return ret;
// 	}
// 	xRect2 operator - ( const VEC2& rhs ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT - rhs;
// 		ret.vRB = vRB - rhs;
// 		return ret;
// 	}
// 	xRect2 operator * ( const VEC2& rhs ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT * rhs;
// 		ret.vRB = vRB * rhs;
// 		return ret;
// 	}
// 	xRect2 operator / ( const VEC2& rhs ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT / rhs;
// 		ret.vRB = vRB / rhs;
// 		return ret;
// 	}
// 	xRect2 operator * ( float num ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT + num;
// 		ret.vRB = vRB + num;
// 		return ret;
// 	}
// 	xRect2 operator / ( float num ) const {
// 		xRect2 ret;
// 		ret.vLT = vLT / num;
// 		ret.vRB = vRB / num;
// 		return ret;
// 	}
// 	inline XE::VEC2 GetSize() const {
// 		return vRB - vLT;
// 	}
// 	inline float GetWidth() const {
// 		return vRB.w - vLT.w;
// 	}
// 	inline float GetHeight() const {
// 		return vRB.h - vLT.h;
// 	}
// 	inline void SetLeft( float left ) {
// 		vLT.x = left;
// 	}
// 	inline void SetRight( float right ) {
// 		vRB.x = right;
// 	}
// 	inline void SetTop( float top ) {
// 		vLT.y = top;
// 	}
// 	inline void SetBottom( float bottom ) {
// 		vRB.y = bottom;
// 	}
// 	inline float GetLeft() const {
// 		return vLT.x;
// 	}
// 	inline float GetRight() const {
// 		return vRB.x;
// 	}
// 	inline float GetTop() const {
// 		return vLT.y;
// 	}
// 	inline float GetBottom() const {
// 		return vRB.y;
// 	}
// 	inline float& Left() {
// 		return vLT.x;
// 	}
// 	inline float& Right() {
// 		return vRB.x;
// 	}
// 	inline float& Top() {
// 		return vLT.y;
// 	}
// 	inline float& Bottom() {
// 		return vRB.y;
// 	}
// 	inline XE::VEC2 GetvLT() const {
// 		return vLT;
// 	}
// 	inline XE::VEC2 GetvRB() const {
// 		return vRB;
// 	}
// }; // xRect2
// XE_NAMESPACE_END;
// 

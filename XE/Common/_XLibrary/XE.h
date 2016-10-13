#pragma once

#ifdef __cplusplus
#include <list>
#endif
#include <stdlib.h>
#include <math.h>
#include <chrono>
#ifdef _VER_DX
#ifdef _CLIENT
#include <d3dx9.h>
#endif
#endif
#include "etc/types.h"
#include "etc/xMath.h"
#include "xVector.h"
#include "xeDef.h"

namespace XE
{
//	struct VEC3;
	extern ID m_idGlobal;			// 다용도 고유 아이디
	
	enum {
		INVALID_INT = 0x80000000,
	};
	//	enum {
	/*#ifdef _XDYNA_RESO2
	 GAME_WIDTH=360,
	 GAME_HEIGHT=640,
	 #else
	 GAME_WIDTH=320,
	 GAME_HEIGHT=568,
	 #endif  */
	//	};
	extern XE::VEC2 x_sizeGame;	// 게임의논리적 해상도(240x360 or 360x640...)
	inline float GetGameWidth() { return x_sizeGame.w;	}
	inline float GetGameHeight() { return x_sizeGame.h; }
	inline const XE::VEC2& GetGameSize( void ) { return x_sizeGame; }
	// 게임의논리적 해상도(320x480 or 360x640...)를 지정한다.
	inline void SetLogicalGameSize( float w, float h ) {
		x_sizeGame.w = w;
		x_sizeGame.h = h;
	}
//	inline float GetGameWidth() { return (float)GAME_WIDTH;	}
//	inline float GetGameHeight() { return (float)GAME_HEIGHT; }
	
//	inline ID GenerateID() { return m_idGlobal++; }
	ID GenerateID();

	struct xRECT {
		XE::VEC2 vLT;
		XE::VEC2 vRB;
		xRECT() {}
		xRECT( int x, int y, int w, int h )
			: vLT( x, y ), vRB( x + w - 1, y + h - 1 ) { }
		xRECT( float x, float y, float w, float h )
			: vLT( x, y ), vRB( x + w - 1, y + h - 1 ) { }
		xRECT( const XE::VEC2& _vLT, const XE::VEC2& _vSize )
			: vLT( _vLT ), vRB( _vLT + _vSize - XE::VEC2( 1, 1 ) ) {}
		//
		xRECT& operator += ( const xRECT& rhs ) {
			vLT += rhs.vLT;
			vRB += rhs.vRB;
			return *this;
		}
		xRECT& operator -= ( const xRECT& rhs ) {
			vLT -= rhs.vLT;
			vRB -= rhs.vRB;
			return *this;
		}
		xRECT& operator *= ( const xRECT& rhs ) {
			vLT *= rhs.vLT;
			vRB *= rhs.vRB;
			return *this;
		}
		xRECT& operator /= ( const xRECT& rhs ) {
			vLT /= rhs.vLT;
			vRB /= rhs.vRB;
			return *this;
		}
		xRECT& operator *= ( float num ) {
			vLT *= num;
			vRB *= num;
			return *this;
		}
		xRECT& operator /= ( float num ) {
			vLT /= num;
			vRB /= num;
			return *this;
		}
		xRECT operator + ( const xRECT& rhs ) const {
			xRECT ret;
			ret.vLT = vLT + rhs.vLT;
			ret.vRB = vRB + rhs.vRB;
			return ret;
		}
		xRECT operator - ( const xRECT& rhs ) const {
			xRECT ret;
			ret.vLT = vLT - rhs.vLT;
			ret.vRB = vRB - rhs.vRB;
			return ret;
		}
		xRECT operator * ( const xRECT& rhs ) const {
			xRECT ret;
			ret.vLT = vLT * rhs.vLT;
			ret.vRB = vRB * rhs.vRB;
			return ret;
		}
		xRECT operator / ( const xRECT& rhs ) const {
			xRECT ret;
			ret.vLT = vLT / rhs.vLT;
			ret.vRB = vRB / rhs.vRB;
			return ret;
		}
		xRECT operator + ( const VEC2& rhs ) const {
			xRECT ret;
			ret.vLT = vLT + rhs;
			ret.vRB = vRB + rhs;
			return ret;
		}
		xRECT operator - ( const VEC2& rhs ) const {
			xRECT ret;
			ret.vLT = vLT - rhs;
			ret.vRB = vRB - rhs;
			return ret;
		}
		xRECT operator * ( const VEC2& rhs ) const {
			xRECT ret;
			ret.vLT = vLT * rhs;
			ret.vRB = vRB * rhs;
			return ret;
		}
		xRECT operator / ( const VEC2& rhs ) const {
			xRECT ret;
			ret.vLT = vLT / rhs;
			ret.vRB = vRB / rhs;
			return ret;
		}
		xRECT operator * ( float num ) const {
			xRECT ret;
			ret.vLT = vLT + num;
			ret.vRB = vRB + num;
			return ret;
		}
		xRECT operator / ( float num ) const {
			xRECT ret;
			ret.vLT = vLT / num;
			ret.vRB = vRB / num;
			return ret;
		}
		inline XE::VEC2 GetSize() const {
			return vRB - vLT + XE::VEC2(1,1);
		}
		inline float GetWidth() const {
			return vRB.w - vLT.w + 1.f;
		}
		inline float GetHeight() const {
			return vRB.h - vLT.h + 1.f;
		}
		inline void SetLeft( float left ) {
			vLT.x = left;
		}
		inline void SetRight( float right ) {
			vRB.x = right;
		}
		inline void SetTop( float top ) {
			vLT.y = top;
		}
		inline void SetSize( const XE::VEC2& size ) {
			vRB = vLT + size - XE::VEC2(1);
		}
		inline void SetBottom( float bottom ) {
			vRB.y = bottom;
		}
		inline float GetLeft() const {
			return vLT.x;
		}
		inline float GetRight() const {
			return vRB.x;
		}
		inline float GetTop() const {
			return vLT.y;
		}
		inline float GetBottom() const {
			return vRB.y;
		}
		inline float& Left() {
			return vLT.x;
		}
		inline float& Right() {
			return vRB.x;
		}
		inline float& Top() {
			return vLT.y;
		}
		inline float& Bottom() {
			return vRB.y;
		}
		inline XE::VEC2 GetvLT() const {
			return vLT;
		}
		inline XE::VEC2 GetvRB() const {
			return vRB;
		}
	}; // xRECT
	struct xRECTi {
		XE::POINT ptLT;
		XE::POINT ptRB;
		xRECTi() {}
		xRECTi( int x, int y, int w, int h ) 
			: ptLT(x, y), ptRB( x + w - 1, y + h - 1) { }
		xRECTi( const XE::POINT& _ptLT, const XE::POINT& _ptSize )
			: ptLT( _ptLT ), ptRB( _ptLT + _ptSize - XE::POINT(1,1) ) {}
		inline XE::POINT GetSize() const {
			return ptRB - ptLT + XE::POINT(1,1);		// +1하는게 맞나?
		}
		inline int GetWidth() const {
			return ptRB.w - ptLT.w + 1;
		}
		inline int GetHeight() const {
			return ptRB.h - ptLT.h + 1;
		}
		inline void SetLeft( int left ) {
			ptLT.x = left;
		}
		inline void SetRight( int right ) {
			ptRB.x = right;
		}
		inline void SetTop( int top ) {
			ptLT.y = top;
		}
		inline void SetBottom( int bottom ) {
			ptRB.y = bottom;
		}
		inline int GetLeft() const {
			return ptLT.x;
		}
		inline int GetRight() const {
			return ptRB.x;
		}
		inline int GetTop() const {
			return ptLT.y;
		}
		inline int GetBottom() const {
			return ptRB.y;
		}
		inline int& Left() {
			return ptLT.x;
		}
		inline int& Right() {
			return ptRB.x;
		}
		inline int& Top() {
			return ptLT.y;
		}
		inline int& Bottom() {
			return ptRB.y;
		}
	}; // xRECTi
	// l,t,r,b 버전 & 크기를 얻을때 -1하거나 +1하지 않는다.
	struct xRect2 {
		XE::VEC2 vLT;
		XE::VEC2 vRB;
		xRect2() {}
		xRect2( int left, int top, int right, int bottom )
			: vLT( left, top ), vRB( right, bottom ) { }
		xRect2( float left, float top, float right, float bottom )
			: vLT( left, top ), vRB( right, bottom ) { }
		xRect2( const XE::VEC2& _vLT, const XE::VEC2& _vRB )
			: vLT( _vLT ), vRB( _vRB ) {}
		//
		xRect2& operator += ( const xRect2& rhs ) {
			vLT += rhs.vLT;
			vRB += rhs.vRB;
			return *this;
		}
		xRect2& operator -= ( const xRect2& rhs ) {
			vLT -= rhs.vLT;
			vRB -= rhs.vRB;
			return *this;
		}
		xRect2& operator *= ( const xRect2& rhs ) {
			vLT *= rhs.vLT;
			vRB *= rhs.vRB;
			return *this;
		}
		xRect2& operator /= ( const xRect2& rhs ) {
			vLT /= rhs.vLT;
			vRB /= rhs.vRB;
			return *this;
		}
		xRect2& operator *= ( float num ) {
			vLT *= num;
			vRB *= num;
			return *this;
		}
		xRect2& operator /= ( float num ) {
			vLT /= num;
			vRB /= num;
			return *this;
		}
		xRect2 operator + ( const xRect2& rhs ) const {
			xRect2 ret;
			ret.vLT = vLT + rhs.vLT;
			ret.vRB = vRB + rhs.vRB;
			return ret;
		}
		xRect2 operator - ( const xRect2& rhs ) const {
			xRect2 ret;
			ret.vLT = vLT - rhs.vLT;
			ret.vRB = vRB - rhs.vRB;
			return ret;
		}
		xRect2 operator * ( const xRect2& rhs ) const {
			xRect2 ret;
			ret.vLT = vLT * rhs.vLT;
			ret.vRB = vRB * rhs.vRB;
			return ret;
		}
		xRect2 operator / ( const xRect2& rhs ) const {
			xRect2 ret;
			ret.vLT = vLT / rhs.vLT;
			ret.vRB = vRB / rhs.vRB;
			return ret;
		}
		xRect2 operator + ( const VEC2& rhs ) const {
			xRect2 ret;
			ret.vLT = vLT + rhs;
			ret.vRB = vRB + rhs;
			return ret;
		}
		xRect2 operator - ( const VEC2& rhs ) const {
			xRect2 ret;
			ret.vLT = vLT - rhs;
			ret.vRB = vRB - rhs;
			return ret;
		}
		xRect2 operator * ( const VEC2& rhs ) const {
			xRect2 ret;
			ret.vLT = vLT * rhs;
			ret.vRB = vRB * rhs;
			return ret;
		}
		xRect2 operator / ( const VEC2& rhs ) const {
			xRect2 ret;
			ret.vLT = vLT / rhs;
			ret.vRB = vRB / rhs;
			return ret;
		}
		xRect2 operator * ( float num ) const {
			xRect2 ret;
			ret.vLT = vLT + num;
			ret.vRB = vRB + num;
			return ret;
		}
		xRect2 operator / ( float num ) const {
			xRect2 ret;
			ret.vLT = vLT / num;
			ret.vRB = vRB / num;
			return ret;
		}
		inline XE::VEC2 GetSize() const {
			return vRB - vLT;
		}
		inline float GetWidth() const {
			return vRB.w - vLT.w;
		}
		inline float GetHeight() const {
			return vRB.h - vLT.h;
		}
		inline void SetLeft( float left ) {
			vLT.x = left;
		}
		inline void SetRight( float right ) {
			vRB.x = right;
		}
		inline void SetTop( float top ) {
			vLT.y = top;
		}
		inline void SetBottom( float bottom ) {
			vRB.y = bottom;
		}
		inline float GetLeft() const {
			return vLT.x;
		}
		inline float GetRight() const {
			return vRB.x;
		}
		inline float GetTop() const {
			return vLT.y;
		}
		inline float GetBottom() const {
			return vRB.y;
		}
		inline float& Left() {
			return vLT.x;
		}
		inline float& Right() {
			return vRB.x;
		}
		inline float& Top() {
			return vLT.y;
		}
		inline float& Bottom() {
			return vRB.y;
		}
		inline XE::VEC2 GetvLT() const {
			return vLT;
		}
		inline XE::VEC2 GetvRB() const {
			return vRB;
		}
	}; // xRect2
	template <typename T>
	typename std::list<T>::iterator FindList( std::list<T> &listSrc, T findElem )
	{
		typename std::list<T>::iterator itor; 
		for( itor = listSrc.begin(); itor != listSrc.end(); itor++ ) 
		{	
			if( (*itor) == findElem )
				return itor;
		}
		return itor;
	}

	// axis lock- 좌표축 고정
	enum xtAL { xAL_NONE=0, xAL_X, xAL_Y, xAL_XY };
	template<class T>
	void AdjustAxisLock( xtAL axisLock, T *x, T *y )
	{
		if( axisLock == xAL_X )		// x축으로만 이동하는 모드
			*y = 0;
		else if( axisLock == xAL_Y )
			*x = 0;
	}

} // XE

class XRandom2
{
 	std::random_device m_std_seed;
 	std::default_random_engine m_engineRandom;
	void Init() {}
	void Destroy() {}
public:
	XRandom2();
	virtual ~XRandom2() { Destroy(); }
	//
// 	GET_ACCESSOR( DWORD, Seed );
	// 시드를 바꿈.
//	void SetSeed64( XINT64 seed );
	void SetSeed( DWORD seed );
	// 시드값 새로 생성
	XINT64 GenerateSeed64() const {
		auto curr = std::chrono::system_clock::now();
		auto duration = curr.time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count();
		return millis;
	}
	DWORD GenerateSeed() {
		return m_std_seed();
	}
	DWORD GetRand();
	DWORD GetRandom( DWORD range );
	/**
	 max - min이 0x7fffffff를 넘지 않도록 주의
	 리턴값이 0x7fffffff을 넘지 않도록 주의
	*/
	int GetRandom( int min, int max );
	float GetRandomF( float range );
	float GetRandomF( float min, float max );
};


//#endif // 

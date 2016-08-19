#ifndef __XE_H__
#define __XE_H__
#ifdef __cplusplus
#include <list>
#include <algorithm>
#endif
#include <math.h>
#ifdef WIN32
#include <d3dx9.h>
#endif
#include "types.h"
#ifdef _VER_IPHONE
#include "Mathematics.h"
#endif

namespace XE
{
	struct VEC3;
	extern ID m_idGlobal;			// 다용도 고유 아이디
//	inline ID GenerateID() { return m_idGlobal++; }
	ID GenerateID();
#ifdef WIN32
	//--------------------------
	// 3D Vector
	//--------------------------
#ifdef DIRECT3D_VERSION
	struct VEC3 : public D3DXVECTOR3
	{
		VEC3() { x = y = z = 0; }
		VEC3( float x, float y, float z ) : D3DXVECTOR3( x, y, z ) {}
		template<typename T1, typename T2, typename T3>
		void Set( T1 _x, T2 _y, T3 _z ) { x = (float)_x; y = (float)_y; z = (float)_z; }
	};
#else
	struct VEC3
	{
		float x, y, z;
		VEC3() { x = y = z = 0; }
		VEC3( float _x, float _y, float _z ) { x = _x; y = _y; z = _z; }
		template<typename T1, typename T2, typename T3>
		void Set( T1 _x, T2 _y, T3 _z ) { x = (float)_x; y = (float)_y; z = (float)_z; }
	};
#endif 
#else
	struct VEC3 : public Vec3
	{
		VEC3() {}
		VEC3( float x, float y, float z ) : Vec3( x, y, z ) {}
		template<typename T1, typename T2, typename T3>
		void Set( T1 _x, T2 _y, T3 _z ) { x = (float)_x; y = (float)_y; z = (float)_z; }
	};
#endif
	//--------------------------
	// 2D Point
	//--------------------------
	typedef struct POINT
	{
	#ifdef __cplusplus
	public:
		POINT() { x = 0; y = 0; };
		template<typename T>
		POINT( T _x, T _y ) { x = (int)_x; y = (int)_y; }
#ifdef _AFX
		POINT( CPoint point ) { x = point.x; y = point.y; }
#endif
		template<typename T>
		POINT( const T& v ) {
			x = (int)v.x;	y = (int)v.y;
		}

		template<typename T>
		void Set( T _x, T _y ) { x = _x; y = _y; }
		template<typename T>
		void Set( T xy ) { x = xy; y = xy; }

		// casting
//		operator int* ();
//		operator const int* () const;

		// assignment operators
#ifdef WIN32
#ifdef _AFX
		POINT& operator = ( const CPoint& rhs ) {
			x = rhs.x;
			y = rhs.y;
			return *this;
		}
#endif
#ifdef DIRECT3D_VERSION
		operator D3DXVECTOR2 () const {
			D3DXVECTOR2 v( (float)x, (float)y );
			return v;
		}
		POINT& operator = ( const D3DXVECTOR2& rhs ) {
			x = (int)rhs.x;
			y = (int)rhs.y;
			return *this;
		}
#endif
#endif // win32
		POINT operator-() {
			POINT v( -x, -y );
			return v;
		}
		POINT& operator += ( const POINT& rhs ) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}
		POINT& operator -= ( const POINT& rhs ) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
		POINT& operator *= ( const POINT& rhs ) {
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}
		POINT& operator /= ( const POINT& rhs ) {
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}
		POINT& operator *= ( int num ) {
			x *= num;
			y *= num;
			return *this;
		}
		POINT& operator /= ( int num ) {
			x /= num;
			y /= num;
			return *this;
		}

		// unary operators
//		POINT operator + () const
//		POINT operator - () const;

		// binary operators
		POINT operator + ( const POINT& rhs ) const {
			POINT ret;
			ret.x = x + rhs.x;
			ret.y = y + rhs.y;
			return ret;
		}
		POINT operator - ( const POINT& rhs) const {
			POINT ret;
			ret.x = x - rhs.x;
			ret.y = y - rhs.y;
			return ret;
		}
		POINT operator * ( const POINT& rhs ) const {
			POINT ret;
			ret.x = x * rhs.x;
			ret.y = y * rhs.y;
			return ret;
		}
		POINT operator * ( int num ) const {
			POINT ret;
			ret.x = x * num;
			ret.y = y * num;
			return ret;
		}
		POINT operator / ( int num ) const {
			POINT ret;
			ret.x = x / num;
			ret.y = y / num;
			return ret;
		}
		POINT operator / ( const POINT& rhs ) const {
			POINT ret;
			ret.x = x / rhs.x;
			ret.y = y / rhs.y;
			return ret;
		}
		POINT operator % ( const POINT& rhs ) const {
			POINT ret;
			ret.x = x % rhs.x;
			ret.y = y % rhs.y;
			return ret;
		}
		POINT operator % ( const int rhs ) const {
			POINT ret;
			ret.x = x % rhs;
			ret.y = y % rhs;
			return ret;
		}

//		friend POINT operator * ( int, const POINT& );

		BOOL operator == ( const POINT& rhs ) const {
			return (x == rhs.x && y == rhs.y );
		}
		BOOL operator != ( const POINT& rhs ) const {
			return (x != rhs.x || y != rhs.y );
		}
		BOOL IsZero() {
			return (x == 0 && y == 0);
		}
		BOOL IsMinus() {
			return (x == -1 && y == -1 );
		}
		float Length() {
			return sqrt( (float)(x * x) + (float)(y * y) );
		}
		float Lengthsq() { 
			return (float)(x * x + y * y);
		}
		const POINT& Abs() {
			x = abs(x);
			y = abs(y);
			return *this;
		}
		int Size() { return w * h; }
		
	public:
	#endif //__cplusplus
		union {
			int x;
			int w;
		};
		union {
			int y;
			int h;
		};
	} POINT, *LPPOINT;
	//--------------------------
	// 2D Vector - 나중에 xMath로 옮길것
	//--------------------------
	struct VEC2
	{
	#ifdef __cplusplus
	public:
		VEC2() { x = 0; y = 0; };
		VEC2( float _x, float _y ) { x = _x; y = _y; }
		VEC2( int _x, int _y ) { x = (float)_x; y = (float)_y; }
		VEC2( float _x, int _y ) { x = _x; y = (float)_y; }
		VEC2( int _x, float _y ) { x = (float)_x; y = _y; }
		explicit VEC2( int xy ) { x = (float)xy; y = (float)xy; }
		explicit VEC2( float xy ) { x = xy; y = xy; }
#ifdef _AFX
		VEC2( CPoint point ) { x = (float)point.x; y = (float)point.y; }
#endif
		VEC2( POINT point ) { x = (float)point.x; y = (float)point.y; }
#ifdef DIRECT3D_VERSION
		VEC2( D3DXVECTOR2 vd3d ) { x = vd3d.x; y = vd3d.y; }
#endif

		template<typename T>
		void Set( T _x, T _y ) { x = (float)_x; y = (float)_y; }

		void Set( float xy ) { x = xy; y = xy; }
		void Set( int xy ) { x = (float)xy; y = (float)xy; }

		// casting
//		operator float* ();
//		operator const float* () const;

		// assignment operators
#ifdef WIN32
#ifdef _AFX
		VEC2& operator = ( const CPoint& rhs ) {
			x = (float)rhs.x;
			y = (float)rhs.y;
			return *this;
		}
		CPoint ToCPoint() const {
			return CPoint( (int)x, (int)y );
		}
#endif
#ifdef DIRECT3D_VERSION
		operator D3DXVECTOR2 () const {
			D3DXVECTOR2 v( x, y );
			return v;
		}
		VEC2& operator = ( const D3DXVECTOR2& rhs ) {
			x = rhs.x;
			y = rhs.y;
			return *this;
		}
#endif
#endif // win32
		VEC2& operator = ( const XE::POINT& rhs ) {
			x = (float)rhs.x;
			y = (float)rhs.y;
			return *this;
		}
		operator VEC3 () const {
			VEC3 v( x, y, 0 );
			return v;
		}
		VEC2 operator-() {
			VEC2 v( -x, -y );
			return v;
//			x = -x;
//			y = -y;
//			return *this;
		}
		VEC2& operator += ( const VEC2& rhs ) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}
		VEC2& operator -= ( const VEC2& rhs ) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
		VEC2& operator *= ( const VEC2& rhs ) {
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}
		VEC2& operator /= ( const VEC2& rhs ) {
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}
		VEC2& operator *= ( float num ) {
			x *= num;
			y *= num;
			return *this;
		}
		VEC2& operator /= ( float num ) {
			x /= num;
			y /= num;
			return *this;
		}

		// unary operators
//		VEC2 operator + () const
//		VEC2 operator - () const;

		// binary operators
		VEC2 operator + ( const VEC2& rhs ) const {
			VEC2 ret;
			ret.x = x + rhs.x;
			ret.y = y + rhs.y;
			return ret;
		}
		VEC2 operator - ( const VEC2& rhs) const {
			VEC2 ret;
			ret.x = x - rhs.x;
			ret.y = y - rhs.y;
			return ret;
		}
		VEC2 operator * ( const VEC2& rhs ) const {
			VEC2 ret;
			ret.x = x * rhs.x;
			ret.y = y * rhs.y;
			return ret;
		}
		VEC2 operator / ( const VEC2& rhs ) const {
			VEC2 ret;
			ret.x = x / rhs.x;
			ret.y = y / rhs.y;
			return ret;
		}
		VEC2 operator + ( float num ) const {
			VEC2 ret;
			ret.x = x + num;
			ret.y = y + num;
			return ret;
		}
		VEC2 operator - ( float num ) const {
			VEC2 ret;
			ret.x = x - num;
			ret.y = y - num;
			return ret;
		}
		VEC2 operator * ( float num ) const {
			VEC2 ret;
			ret.x = x * num;
			ret.y = y * num;
			return ret;
		}
		VEC2 operator / ( float num ) const {
			VEC2 ret;
			ret.x = x / num;
			ret.y = y / num;
			return ret;
		}
		VEC2 operator % ( const VEC2& rhs ) const {
			VEC2 ret;
			ret.x = fmod( x, rhs.x );
			ret.y = fmod( y, rhs.y );
			return ret;
		}
		VEC2 operator % ( const float rhs ) const {
			VEC2 ret;
			ret.x = fmod( x, rhs );
			ret.y = fmod( y, rhs );
			return ret;
		}

//		friend VEC2 operator * ( float, const VEC2& );

		BOOL operator == ( const VEC2& rhs ) const {
			return (x == rhs.x && y == rhs.y );
		}
		BOOL operator != ( const VEC2& rhs ) const {
			return (x != rhs.x || y != rhs.y );
		}
		BOOL IsZero() const {
			return (x == 0 && y == 0);
		}
		BOOL IsMinus() const {
			return (x == -1.f && y == -1.f );
		}
		// 초기화 상태인가
		bool IsInit() const {
			return x == 9999.f && y == 9999.f;
		}
		void Init() {
			x = 9999.f;
			y = 9999.f;
		}
		inline bool IsUninit() const {
			return !IsInit();
		}
		float Length() {
			return sqrt( x * x + y * y );
		}
		float Lengthsq() { 
			return (x * x + y * y);
		}
		void Normalize() {
			float len = Length();
			x /= len;
			y /= len;
		}
		float Dot(	const VEC2& rhs ) const {
			return x * rhs.x + y * rhs.y;
		}
		const VEC2& Abs() {
			x = fabs(x);
			y = fabs(y);
			return *this;
		}
		
	public:
	#endif //__cplusplus
		union {
			float x;
			float w;
			float v1;
		};
		union {
			float y;
			float h;
			float v2;
		};
	};

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
			return vRB - vLT + XE::VEC2( 1, 1 );
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
		// 현재 vLT와 파라메터 vlt를 비교해 최소값만 vLT에 저장한다.
		inline void SetMin( const XE::VEC2& v ) {
			vLT.x = std::min( vLT.x, v.x );
			vLT.y = std::min( vLT.y, v.y );
		}
		inline void SetMax( const XE::VEC2& v ) {
			vRB.x = std::max( vRB.x, v.x );
			vRB.y = std::max( vRB.y, v.y );
		}
		// 현재 박스좌표와 v를 비교해 최소,최대값을 갱신한다.,
		inline void UpdateBoundBox( const XE::VEC2& v ) {
			SetMin( v );
			SetMax( v );
		}

	};
	struct xRECTi {
		XE::POINT ptLT;
		XE::POINT ptRB;
		xRECTi() {}
		xRECTi( int x, int y, int w, int h )
			: ptLT( x, y ), ptRB( x + w - 1, y + h - 1 ) { }
		xRECTi( const XE::POINT& _ptLT, const XE::POINT& _ptSize )
			: ptLT( _ptLT ), ptRB( _ptLT + _ptSize - XE::POINT( 1, 1 ) ) {}
		inline XE::POINT GetSize() const {
			return ptRB - ptLT + XE::POINT( 1, 1 );		// +1하는게 맞나?
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
	};

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

enum xtENCODING {
	TXT_EUCKR=0,
	TXT_UTF8,
	TXT_UTF16,
};

#endif // 
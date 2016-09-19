//
//  xVector.h
//  xe
//
//  Created by JungWoo Sung on 13. 6. 8..
//  Copyright (c) 2013년 JungWoo Sung. All rights reserved.
//

#ifndef xe_xVector_h
#define xe_xVector_h
#include "etc/Types.h"
#include "etc/Global.h"
#include <math.h>
#include <stdlib.h>
// #ifdef _VER_DX
// #include <d3dx9.h>
// #endif
#ifdef WIN32
// 서버에서도 DirectX의 헤더를 사용함. 벡터의 사용을 위해.
#ifdef _VER_DX
#include <d3dx9.h>
#endif // _DEBUG
#endif // WIN32

namespace XE
{
	struct VEC2;
	//--------------------------
	// 2D Point
	//--------------------------
	typedef struct POINT
	{
//#ifdef __cplusplus
	public:
		POINT() { x = 0; y = 0; };
		POINT( int _x, int _y ) { x = _x; y = _y; }
		POINT( float _x, float _y ) { x = (int)_x; y = (int)_y; }
		POINT( float _x, int _y ) { x = (int)_x; y = _y; }
		POINT( int _x, float _y ) { x = _x; y = (int)_y; }
#ifdef _AFX
		POINT( CPoint point ) { x = point.x; y = point.y; }
#endif
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
  #ifdef _VER_DX
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
		POINT& operator %= ( const POINT& rhs ) {
			x %= rhs.x;
			y %= rhs.y;
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
		POINT& operator %= ( int num ) {
			x %= num;
			y %= num;
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
		BOOL IsZero() const {
			return (x == 0 && y == 0);
		}
		BOOL IsMinus() const {
			return (x == -1 && y == -1 );
		}
		BOOL IsHave() const {
			return (x > 0 && y > 0);
		}
		float Length() const {
			return sqrt( (float)(x * x) + (float)(y * y) );
		}
		float Lengthsq() const {
			return (float)(x * x + y * y);
		}
		const POINT& Abs() {
			x = abs(x);
			y = abs(y);
			return *this;
		}
		int Size() const {
			return w * h;
		}
		VEC2 ToVec2() const;
		
	public:
//#endif //__cplusplus
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
	// 2D Vector -
	//--------------------------
	typedef struct VEC2
	{
//#ifdef __cplusplus
	public:
		VEC2() { x = 0; y = 0; };
		VEC2( float _x, float _y ) { x = _x; y = _y; }
		VEC2( int _x, int _y ) { x = (float)_x; y = (float)_y; }
		VEC2( float _x, int _y ) { x = _x; y = (float)_y; }
		VEC2( int _x, float _y ) { x = (float)_x; y = _y; }
		VEC2( int xy ) { x = (float)xy; y = (float)xy; }
		VEC2( float xy ) { x = xy; y = xy; }
#ifdef _AFX
		VEC2( CPoint point ) { x = (float)point.x; y = (float)point.y; }
#endif
		VEC2( POINT point ) { x = (float)point.x; y = (float)point.y; }
#ifdef _VER_DX
		VEC2( D3DXVECTOR2 vd3d ) { x = vd3d.x; y = vd3d.y; }
#endif
		
		template<typename T>
		void Set( T _x, T _y ) { x = (float)_x; y = (float)_y; }
		template<typename T>
		void Set( T xy ) { x = (float)xy; y = (float)xy; }
		
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
  #endif
  #ifdef _VER_DX
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
//		operator VEC3 () const {
//			VEC3 v( x, y, 0 );
//			return v;
//		}
		VEC2 operator-() const {
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
		VEC2 operator / ( const VEC2& rhs ) const {
			VEC2 ret;
			ret.x = x / rhs.x;
			ret.y = y / rhs.y;
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
		
		inline bool operator == ( const VEC2& rhs ) const {
			return (x == rhs.x && y == rhs.y );
		}
		inline bool operator != ( const VEC2& rhs ) const {
			return (x != rhs.x || y != rhs.y );
		}
		inline bool IsZero() const {
			return (x == 0 && y == 0);
		}
		inline bool IsNotZero() const {
			return !IsZero();
		}
		inline bool IsMinus() const {
			return (x == -1.f && y == -1.f );
		}
		bool IsValid() const {
			return (x > 0 && y > 0 );
		}
		bool IsInvalid() const  {
			return !IsValid();
		}
		bool IsInit() const {
			return x == 9999.f && y == 9999.f;
		}
		bool IsUninit() const {
			return !IsInit();
		}
		void Init() {
			x = 9999.f;   y = 9999.f;
		}
		float Length() const {
			return sqrt( x * x + y * y );
		}
		float Lengthsq() const {
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
		VEC2 Cross() const {
			VEC2 v;
			v.x = y;
			v.y = -x;
			return v;
		}
		inline const VEC2& Abs() {
			x = fabs(x);
			y = fabs(y);
			return *this;
		}
		inline float Size() const {
			return w * h;
		}
		POINT ToPoint() const {
			return POINT( (int)x, (int)y );
		}
		// 소숫점을 잘라버리고 정수화 시킴.
		inline VEC2 ToInt() const {
			return VEC2( (int)x, (int)y );
		}
		// this의 x,y를 vDiv단위로 grid화 시켜서 결과를 리턴한다.
		inline VEC2 RoundUpDiv( const VEC2& vDiv ) const {
			int divx = (int)(x / vDiv.x);
			int divy = (int)(y / vDiv.y);
			int remx = (int)x % (int)vDiv.x;
			int remy = (int)y % (int)vDiv.y;
			if( remx > 0 )
				++divx;
			if( remy > 0 )
				++divy;
			return VEC2( divx * vDiv.x, divy * vDiv.y );
		}
		inline VEC2 RoundUpDiv( int div ) const {
			return RoundUpDiv( XE::VEC2(div, div) );
		}
// 		int Serialize( XArchive& ar ) {
// 			ar << x;
// 			ar << y;
// 			return 1;
// 		}
// 		int DeSerialize( XArchive& ar, int ) {
// 			ar >> x;
// 			ar >> y;
// 		}
	public:
//#endif //__cplusplus
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
	} VEC2, *LPVEC2;
	//--------------------------
	// 3D Vector - 2d게임에서 사용하는 3d벡터
	//--------------------------
	typedef struct VEC3
	{
//#ifdef __cplusplus
	public:
		VEC3() { x = 0; y = 0; z = 0; };
		template<typename T1, typename T2, typename T3>
		VEC3(T1 _x, T2 _y, T3 _z) { x = (float)_x; y = (float)_y; z = (float)_z; }
		template<typename T1, typename T2>
		VEC3(T1 _x, T2 _y) { x = (float)_x; y = (float)_y; z = 0; }
		VEC3( const VEC2& v2 ) {
			x = v2.x;
			y = v2.y;
			z  = 0;
		}
		//
		template<typename T1, typename T2, typename T3>
		void Set( T1 _x, T2 _y, T3 _z) { x = (float)_x; y = (float)_y; z = (float)_z; }
		template<typename T>
		void Set( T xyz ) { x = (float)xyz; y = (float)xyz; z = (float)xyz; }
		
		// assignment operators
//		operator VEC2() const {
//			return VEC2( x, y );
//		}
		VEC3 operator = ( const VEC2& rhs ) {
			x = rhs.x;
			y = rhs.y;
			// 대입할땐 기존z값은 건드리지 않는다.
			return *this;
		}
#ifdef WIN32
  #ifdef _AFX
  #endif
  #ifdef _VER_DX
		operator D3DXVECTOR3 () const {
			return D3DXVECTOR3( x, y, z );
		}
		VEC3& operator = ( const D3DXVECTOR3& rhs ) {
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			return *this;
		}
  #endif
#endif // win32
		VEC3 operator-() const {
			VEC3 v( -x, -y, -z );
			return v;
		}
		VEC3& operator += ( const VEC3& rhs ) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}
		VEC3& operator -= ( const VEC3& rhs ) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}
		VEC3& operator *= ( const VEC3& rhs ) {
			x *= rhs.x;
			y *= rhs.y;
			z *= rhs.z;
			return *this;
		}
		VEC3& operator /= ( const VEC3& rhs ) {
			x /= rhs.x;
			y /= rhs.y;
			z /= rhs.z;
			return *this;
		}
		VEC3& operator *= ( float num ) {
			x *= num;
			y *= num;
			z *= num;
			return *this;
		}
		VEC3& operator /= ( float num ) {
			x /= num;
			y /= num;
			z /= num;
			return *this;
		}
		
		// unary operators
		//		VEC3 operator + () const
		//		VEC3 operator - () const;
		
		// binary operators
		VEC3 operator + ( const VEC3& rhs ) const {
			VEC3 ret;
			ret.x = x + rhs.x;
			ret.y = y + rhs.y;
			ret.z = z + rhs.z;
			return ret;
		}
		VEC3 operator - ( const VEC3& rhs) const {
			VEC3 ret;
			ret.x = x - rhs.x;
			ret.y = y - rhs.y;
			ret.z = z - rhs.z;
			return ret;
		}
		VEC3 operator * ( const VEC3& rhs ) const {
			VEC3 ret;
			ret.x = x * rhs.x;
			ret.y = y * rhs.y;
			ret.z = z * rhs.z;
			return ret;
		}
		VEC3 operator * ( float num ) const {
			VEC3 ret;
			ret.x = x * num;
			ret.y = y * num;
			ret.z = z * num;
			return ret;
		}
		VEC3 operator / ( float num ) const {
			VEC3 ret;
			ret.x = x / num;
			ret.y = y / num;
			ret.z = z / num;
			return ret;
		}
		VEC3 operator / ( const VEC3& rhs ) const {
			VEC3 ret;
			ret.x = x / rhs.x;
			ret.y = y / rhs.y;
			ret.z = z / rhs.z;
			return ret;
		}
		VEC3 operator % ( const VEC3& rhs ) const {
			VEC3 ret;
			ret.x = fmod( x, rhs.x );
			ret.y = fmod( y, rhs.y );
			ret.z = fmod( z, rhs.z );
			return ret;
		}
		VEC3 operator % ( const float rhs ) const {
			VEC3 ret;
			ret.x = fmod( x, rhs );
			ret.y = fmod( y, rhs );
			ret.z = fmod( z, rhs );
			return ret;
		}
		
		//		friend VEC3 operator * ( float, const VEC3& );
		
		bool operator == ( const VEC3& rhs ) const {
			return (x == rhs.x && y == rhs.y && z == rhs.z );
		}
		inline bool operator != ( const VEC3& rhs ) const {
			return (x != rhs.x || y != rhs.y || z != rhs.z );
		}
		inline bool IsZero() const {
			return (x == 0 && y == 0 && z == 0 );
		}
		inline bool IsNotZero() const {
			return !IsZero();
		}
		inline bool IsMinus() const {
			return (x == -1.f && y == -1.f && z == -1.f );
		}
		inline bool IsHave() const {
			return (x > 0 && y > 0 && z > 0);
		}
		float Length() const {
			return sqrt( x * x + y * y + z * z );
		}
		float Lengthsq() const {
			return (x * x + y * y + z * z );
		}
		void Normalize() {
			float len = Length();
			x /= len;
			y /= len;
			z /= len;
		}
		float Dot(	const VEC3& rhs ) const {
			return x * rhs.x + y * rhs.y + z * rhs.z;
		}
		VEC3 Cross( const VEC3& rhs ) const {
			VEC3 v;
			v.x = y * rhs.z - z * rhs.y;
			v.y = z * rhs.x - x * rhs.z;
			v.z = x * rhs.y - y * rhs.x;
			return v;
		}
		const VEC3& Abs() {
			x = fabs(x);
			y = fabs(y);
			z = fabs(z);
			return *this;
		}
		float Size() const {
			return w * h * l;
		}
		VEC2 ToVec2() const {
			return VEC2( x, y );
		}
		VEC2 ToVec2XZ() const {
			return VEC2( x, z );
		}
		VEC2 GetXY() const {
			return VEC2( x, y );
		}
		VEC2 GetXZ() const {
			return VEC2( x, z );
		}
		
	public:
//#endif //__cplusplus
		union {
			float x;
			float w;
		};
		union {
			float y;
			float l;
		};
		union {
			float z;
			float h;
		};
	} VEC3, *LPVEC3;
	//////////////////////////////////////////////////////////////////////////
	class VEC4 : public VEC3 {
	public:
		VEC4( float _x, float _y, float _z, float _a ) 
		: VEC3( _x, _y, _z ) {
			a = _a;
		}
		float a;
	};
}; // XE

#endif



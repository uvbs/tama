#pragma once

#define	D2R(D)		(float)(3.141592f*(D)/180.f)
#define	R2D(R)		(float)(180.f*(R)/3.141592f)

namespace XE
{
	//--------------------------
	// 3D Vector
	//--------------------------
/*	struct VEC3 : public D3DXVECTOR3
	{
		VEC3() {}
		VEC3( float x, float y, float z ) : D3DXVECTOR3( x, y, z ) {}
		template<typename T1, typename T2, typename T3>
		void Set( T1 _x, T2 _y, T3 _z ) { x = (float)_x; y = (float)_y; z = (float)_z; }
	}; */
/*	typedef struct VEC3
	{
	#ifdef __cplusplus
	public:
		VEC3() { x = 0; y = 0; z = 0; };
		template<typename T1, typename T2, typename T3>
		VEC3( T1 _x, T2 _y, T3 _z ) { x = (float)_x; y = (float)_y; z = (float)_z; }
		template<typename T>
		void Set( T _x, T _y ) { x = (float)_x; y = (float)_y; }
		template<typename T>
		void Set( T xy ) { x = (float)xy; y = (float)xy; }

		// assignment operators
#ifdef WIN32
		// casting
		operator D3DXVECTOR3 () const {
			D3DXVECTOR3 v( x, y, z );
			return v;
		}
		VEC3& operator = ( const D3DXVECTOR3& rhs ) { x = rhs.x;	y = rhs.y;		z = rhs.z;	return *this;	}
#endif // win32
		VEC3& operator-() {	x = -x;		y = -y;		z = -z;		return *this;	}
		VEC3& operator += ( const VEC3& rhs ) {	x += rhs.x;	y += rhs.y;	z += rhs.z;	return *this;	}
		VEC3& operator -= ( const VEC3& rhs ) {	x -= rhs.x;	y -= rhs.y;	z -= rhs.z;	return *this;	}
		VEC3& operator *= ( const VEC3& rhs ) {	x *= rhs.x;	y *= rhs.y;	z *= rhs.z;	return *this;	}
		VEC3& operator /= ( const VEC3& rhs ) {	x /= rhs.x;	y /= rhs.y;	z /= rhs.z;	return *this;	}
		VEC3& operator *= ( float num ) {	x *= num;	y *= num;	z *= num;	return *this;	}
		VEC3& operator /= ( float num ) {	x /= num;	y /= num;	z /= num;	return *this;	}

		// unary operators
//		VEC3 operator + () const
//		VEC3 operator - () const;

		// binary operators
		VEC3 operator + ( const VEC3& rhs ) const {	VEC3 ret;	ret.x = x + rhs.x;	ret.y = y + rhs.y;	ret.z = z + rhs.z;	return ret;	}
		VEC3 operator - ( const VEC3& rhs) const {	VEC3 ret;	ret.x = x - rhs.x;		ret.y = y - rhs.y;		ret.z = z - rhs.z;		return ret;	}
		VEC3 operator * ( const VEC3& rhs ) const {	VEC3 ret;	ret.x = x * rhs.x;		ret.y = y * rhs.y;		ret.z = z * rhs.z;		return ret;	}
		VEC3 operator * ( float num ) const {	VEC3 ret;	ret.x = x * num;		ret.y = y * num;		ret.z = z * num;		return ret;	}
		VEC3 operator / ( float num ) const {	VEC3 ret;	ret.x = x / num;		ret.y = y / num;		ret.z = z / num;		return ret;	}
		VEC3 operator / ( const VEC3& rhs ) const {	VEC3 ret;	ret.x = x / rhs.x;		ret.y = y / rhs.y;		ret.z = z / rhs.z;		return ret;	}
//		friend VEC3 operator * ( float, const VEC3& );
		BOOL operator == ( const VEC3& rhs ) const {	return (x == rhs.x && y == rhs.y && z == rhs.z );		}
		BOOL operator != ( const VEC3& rhs ) const {	return (x != rhs.x || y != rhs.y || z != rhs.z );	}
		BOOL IsZero() {	return (x == 0 && y == 0 && z == 0 );	}
		float Length() {	return sqrt( x * x + y * y + z * z );	}
		float Lengthsq() { return (x * x + y * y + z * z);		}
		const VEC3& Abs() {		x = fabs(x);		y = fabs(y);		z = fabs(z);		return *this;	}
		
	public:
	#endif //__cplusplus
		float x;
		float y;
		float z;
	} VEC3, *LPVEC3;
*/
	// 보간(Interpolation)
	inline float xiLinearLerp( float timeLerp ) { return timeLerp; }
	// ex) xiSmoothStep( lerpTime, 1.0f, 0 );
	inline float xiSmoothStep( float timeLerp, float startLerp, float endLerp )
	{
		float v = timeLerp * timeLerp * (3 - 2 * timeLerp);
		float X = (startLerp * v) + (endLerp * (1 - v));
		return X;
	}
	// ex) xiHigherPowerDeAccel( lerpTime, 1.0f, 0 );
	inline float xiHigherPowerDeAccel( float timeLerp, float A, float B )
	{
		float v =1 - (1 - timeLerp) * (1 - timeLerp);
		float X = (A * v) + (B * (1 - v));
		return X;
	}
	// ex) xiHigherPowerAccel( lerpTime, 1.0f, 0 );
	inline float xiHigherPowerAccel( float timeLerp, float A, float B )
	{
		float v = timeLerp * timeLerp * timeLerp;
		float X = (A * v) + (B * (1 - v));
		return X;
	}
	// 점점빨라지며 튕기는 듯한 스플라인 방식 애니메이션
	// ex) xiCatmullrom( lerpTime, -10.0f, 0, 1, 1, )
	// -10.0이 바뀌면 팅기는 폭도 바뀐다
	inline float xiCatmullrom( float t, float p0, float p1, float p2, float p3 )	
	{
	return 0.5f * (
				  (2 * p1) +
				  (-p0 + p2) * t +
				  (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t +
				  (-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t
				  );
	}
/*
void MyVec2CatmullRom( D3DXVECTOR2 *pOut, D3DXVECTOR2 *v0, D3DXVECTOR2 *v1, D3DXVECTOR2 *v2, D3DXVECTOR2 *v3, FLOAT t )
{
	*pOut = 0.5f * (
			  (2 * (*v1)) +
			  (-(*v0) + (*v2)) * t +
			  (2 * (*v0) - 5 * (*v1) + 4 * (*v2) - (*v3)) * t * t +
			  (-(*v0) + 3 * (*v1) - 3 * (*v2) + (*v3)) * t * t * t
			  );
}

*/
/////////////////////////////////////////////////////////////////////////////////
} // namespace XE

inline void Vec2CatmullRom( XE::VEC2& pOut, XE::VEC2 v0, XE::VEC2 v1, XE::VEC2 v2, XE::VEC2 v3, float t )
{
    pOut = (
            (v1 * 2.0f) +
            ((-v0) + v2) * t +
            ((v0)*2.0f - (v1)*5.0f + (v2)*4.0f - (v3)) * t * t +
            (-(v0) + (v1)*3.0f - (v2)*3.0f + (v3)) * t * t * t
            ) * 0.5f;
    
}

#ifdef _VER_OPENGL
#include "GL/xMathGL.h"
#else
#include "D3D\xMathD3D.h"
#endif



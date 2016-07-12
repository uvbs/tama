#pragma once
#include "xVector.h"

// #define	D2R(D)		(3.141592f*(D)/180.f)
// #define	R2D(R)		(180.f*(R)/3.141592f)
template<typename T>
inline T D2R( const T& D ) {
	return (D * 3.141592f / 180.f);
}
template<typename T>
inline T R2D( const T& R ) {
	return (R * 180.f / 3.141592f);
}

#ifdef _CLIENT
#include "XE3d.h"
#endif
/*#ifndef WIN32
#include "XE.h"
#include "XE3d.h"
#include "Oolong_Engine2/include/Mathematics.h"		// oolong engine
#endif // not win32
*/
namespace XE
{
	float CalcDistAngle( float srcAngle, float destAngle );
	/**
	 @brief rSrc를 기준으로 rDst각도로의 상대각도(라디안버전)
	 @return 리턴값은 반드시 양수가 된다.
	*/
	inline float GetDistRadian( float rSrc, float rDst ) {
		const float fr = rDst - rSrc;
		if( fr < 0 )
			return fr + D2R( 360.f );
		return fr;
	}
	/**
		@brief ra각도가 rSrc각도와 rDst각도사이에 있는지 판별. 
		rSrc가 기준 좌표가 되고 rDst는 목표각도가 된다.
	*/
	inline bool IsInRadian( float rSrc, float rDst, float ra ) {
		const auto rDistToDst = GetDistRadian( rSrc, rDst );
		const auto rDistToRa = GetDistRadian( rSrc, ra );
		return rDistToRa < rDistToDst;
	}
	float CalcAngle( float x1, float y1, float x2, float y2 );
	float CalcAngle( float x, float y );
#ifdef __cplusplus	// 나중에 xeMath.cpp이런쪽으로 옮길것
	inline float CalcAngle( const XE::VEC2& vSrc, const XE::VEC2& vDst ) {
		return CalcAngle( vSrc.x, vSrc.y, vDst.x, vDst.y );
	}
	bool GetIntersectPoint(const XE::VEC2& AP1, const XE::VEC2& AP2, const XE::VEC2& BP1, const XE::VEC2& BP2, XE::VEC2* IP);
	BOOL IsInCircleWithPoint( const XE::VEC2& vCenter, float radius, const XE::VEC2& v );	
	BOOL IsInSphereWithPoint( const XE::VEC3& vCenter, float radius, const XE::VEC3& v );
	BOOL IsOverlapWithRect( const XE::VEC2& vLT1, const XE::VEC2& vRB1, const XE::VEC2& vLT2, const XE::VEC2& vRB2 );
	XE::VEC2 GetAngleVector( float dAng, float dist );
#endif
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
    /**
     박동같은 효과. 빠르게 커졌다가 천천히 작아진다.
     0 ~ max 구간까지 올라갔다가 endLerp% 수준 까지 내려간다.
     endLerp는 max의 %로 넣어야 한다.
     ex) 0에서 2.0까지 상승했다가 1.0까지 떨어지는 보간.
     xiPulse( timeLerp, 2.0f, 0.5f );
     */
    float xiPulse( float timeLerp, float max, float endLerp, float peak=0.2f );
    // 시간구간내에서 박동효과를 하며 점점 상승한다.
    float xiPulseHigher( float timeLerp, int num, float maxPerSlice, float endLerp );
    /**
     sin파형(0~1~0)의 보간값을 얻어낸다.
     min: 0
     max: 1
     slope : 0(수평)	기울기. 1이면 45도 방향으로 올라간다.
     */
    float xiSin( float timeLerp, float min, float max, float slope );
    /**
     cos파형(1~0~1)의 보간값을 얻어낸다.
     min: 0
     max: 1
     slope : 0(수평)	기울기. 1이면 45도 방향으로 올라간다.
     */
    float xiCos( float timeLerp, float min, float max, float slope );
	float xiGravity( float timeLerp, float startLerp, float endLerp );
/////////////////////////////////////////////////////////////////////////////////
} // namespace XE
// t: 0 ~ 1
// pOut: v1-v2 사이의 점
inline void Vec2CatmullRom( XE::VEC2& pOut, XE::VEC2 v0, XE::VEC2 v1, XE::VEC2 v2, XE::VEC2 v3, float t )
{
    pOut = (
            (v1 * 2.0f) +
            ((-v0) + v2) * t +
            ((v0)*2.0f - (v1)*5.0f + (v2)*4.0f - (v3)) * t * t +
            (-(v0) + (v1)*3.0f - (v2)*3.0f + (v3)) * t * t * t
            ) * 0.5f;
    
}
inline void Vec3CatmullRom( XE::VEC3& pOut, 
							const XE::VEC3& v0, 
							const XE::VEC3& v1, 
							const XE::VEC3& v2, 
							const XE::VEC3& v3, float t )
{
    pOut = (
            (v1 * 2.0f) +
            ((-v0) + v2) * t +
            ((v0)*2.0f - (v1)*5.0f + (v2)*4.0f - (v3)) * t * t +
            (-(v0) + (v1)*3.0f - (v2)*3.0f + (v3)) * t * t * t
            ) * 0.5f;
    
}
/*
#ifdef _VER_IOS
#include "xMathiOS.h"
 #endif
 */
//#if !defined(_CONSOLE) && defined(WIN32)	// win32 console app
  #if defined(_VER_DX)
    #include "_DirectX/xMathD3D.h"
	#elif defined(_VER_OPENGL)
		#ifdef XOOLONG
			#include "oolong_engine2/Include/Mathematics.h"
		#endif
	#else
		#ifndef MATRIX
		#error "not define MATRIX"
		#endif
  #endif
//#endif



#include "stdafx.h"
#include <math.h>
#include "etc/xMath.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////
//        Math
// srcAngle로부터 destAngle의 상대 각도(각도차이) -180' ~ +180 사이의 값
// 이것은 각도차가 180도이상 나오지 않게 보정하는 버전인듯 하다. 
float XE::CalcDistAngle( float srcAngle, float destAngle )
{
	float fa = destAngle - srcAngle;
	if( fa < -180.0f )
		fa += 360.0f;
	else if( fa > 180.0f )
		fa -= 360.0f;
	return fa;
}
// x1,y1 -> x2,y2로 향하는 벡터의 각도 구함
// 북: -90
// 동: 0
// 남 90
// 서: 180
// 북동: -45
// 남동: +45
// 북서: -135
// 남서: +135
/*              -90
        -135    |      -45
                 |
  +180 -------------------- 0
                 |       +45
         +135  |
                 +90

*/
float XE::CalcAngle( float x1, float y1, float x2, float y2 )
{
	float rAngle = atan2( y2 - y1, (x2 - x1) );	// x1에서 x2로의 각도(라디안)
	return R2D( rAngle );
}

float XE::CalcAngle( float x, float y )
{
	float rAngle = atan2( y, x );
	return R2D( rAngle );
}

bool XE::GetIntersectPoint(const XE::VEC2& AP1, const XE::VEC2& AP2, 
								const XE::VEC2& BP1, const XE::VEC2& BP2, XE::VEC2* IP) 
{
    double t;
    double s; 
    double under = (BP2.y-BP1.y)*(AP2.x-AP1.x)-(BP2.x-BP1.x)*(AP2.y-AP1.y);
    if(under==0) return false;

    double _t = (BP2.x-BP1.x)*(AP1.y-BP1.y) - (BP2.y-BP1.y)*(AP1.x-BP1.x);
    double _s = (AP2.x-AP1.x)*(AP1.y-BP1.y) - (AP2.y-AP1.y)*(AP1.x-BP1.x); 

    t = _t/under;
    s = _s/under; 

    if(t<0.0 || t>1.0 || s<0.0 || s>1.0) return false;
    if(_t==0 && _s==0) return false; 

    IP->x = (float)((double)AP1.x + t * (double)(AP2.x-AP1.x));
    IP->y = (float)((double)AP1.y + t * (double)(AP2.y-AP1.y));

    return true;
}

// 
/**
 @brief v1을 중심으로 반지름 radius인 구에 v2좌표가 들어갔는지 검사
*/
BOOL XE::IsInCircleWithPoint( const XE::VEC2& vCenter, float radius, const XE::VEC2& v )
{
	float len = (v - vCenter).Lengthsq();	// 두 점사이의 거리
	if( len < radius * radius )
		return TRUE;
	return FALSE;
}

/**
 @brief v1을 중심으로 반지름 radius인 원안에 v2좌표가 들어갔는지 검사
*/
BOOL XE::IsInSphereWithPoint( const XE::VEC3& vCenter, float radius, const XE::VEC3& v )
{
	float len = ( v - vCenter ).Lengthsq();	// 두 점사이의 거리
	if( len < radius * radius )
		return TRUE;
	return FALSE;
}

// 두개의 사각형이 충돌했는지 검사한다.
BOOL XE::IsOverlapWithRect( const XE::VEC2& vLT1, const XE::VEC2& vRB1, 
										const XE::VEC2& vLT2, const XE::VEC2& vRB2 )
{
	if( vRB2.x < vLT1.x )		return FALSE;		// 사각형2가 사각형1을 왼쪽으로 완전히 벗어났다.
	if( vLT2.x > vRB1.x )		return FALSE;		// 사각형2가 사각형1의 오른쪽으로 완전히 벗어났다.
	if( vRB2.y < vLT1.y )		return FALSE;		// 사각형2가 사각형1의 위쪽으로 완전히 벗어났다.
	if( vLT2.y > vRB1.y )		return FALSE;		// 사각형2가 사각형1의 아랫쪽으로 완전히 벗어났다.
	return TRUE;
}

XE::VEC2 XE::GetAngleVector( float dAng, float dist )
{
	XE::VEC2 v;
	float rAng = D2R( dAng );
	v.x = cosf( rAng ) * dist;
	v.y = sinf( rAng ) * dist;
	return v;
}
///////////////////////////////////////////////////////////////
// 박동같은 효과. 빠르게 커졌다가 천천히 작아진다.
float XE::xiPulse( float timeLerp, float max, float endLerp, float peak )
{
//    float peak = 0.2f;
	if( timeLerp <= peak )
	{
		float lerp = timeLerp / peak;	// 0~0.3초를 0~1로 변환한다.
		float value = max * lerp;
		return value;
	}
	float lerp = (timeLerp - peak) / (1.0f - peak);	// 0~0.7초를 0~1로 변환한다.
	float height = max * (1.0f - endLerp);		// 하강구간의 높이
	float value = max - height * lerp;
	return value;
}

// 시간구간내에서 박동효과를 하며 점점 상승한다.
float XE::xiPulseHigher( float timeLerp, int num, float maxPerSlice, float endLerp )
{
	if( num == 0 )
		num = 1;
	float timeSlice = 1.0f / num;		// 한구간당 시간
	float timeMod = fmodf( timeLerp, timeSlice );	// 현재시간을 구간당시간으로 % 한거
	int idx = (int)(timeLerp / timeSlice);		// 현재 몇번째 구간을 지나고 있는지
	float timeLerpSlice = timeMod / timeSlice;	// 한구간내에서의 시간을 0~1로 변환
	float v = xiPulse( timeLerpSlice, maxPerSlice, endLerp );		// 0에서 시작해서 end값으로 내려오게.
	float value = endLerp * idx + v;
	return value;
}

// sin파형(0~1~0)의 보간값을 얻어낸다.
// min: 0
// max: 1
// slope : 0(수평)	기울기. 1이면 45도 방향으로 올라간다.
float XE::xiSin( float timeLerp, float min, float max, float slope )
{
	float degree = 180.f * timeLerp;	// 0~18까지 변화
	float s = sinf( D2R(degree) );		// sin0(0)~sin90(1)~sin180(0)까지 변화
	float size = max - min;				// 상하 폭 계산
	s *= size;								// 폭만큼 비율조절
	s += min;
	s += slope * timeLerp;				// 시간변화에 따른 기울기값을 더해줌
	return s;
}
// cos파형(1~0~1)의 보간값을 얻어낸다.
// min: 0
// max: 1
// slope : 0(수평)	기울기. 1이면 45도 방향으로 올라간다.
float XE::xiCos( float timeLerp, float min, float max, float slope )
{
	float degree = 180.f * timeLerp;	// 0~18까지 변화
	float s = cosf( D2R(degree) );		// sin0(0)~sin90(1)~sin180(0)까지 변화
	s = fabs( s );
	float size = max - min;				// 상하 폭 계산
	s *= size;								// 폭만큼 비율조절
	s += min;
	s += slope * timeLerp;				// 시간변화에 따른 기울기값을 더해줌
	return s;
}

float XE::xiGravity( float timeLerp, float startLerp, float endLerp )
{
	float yDist = 0.5f*9.8f*( timeLerp * timeLerp );
	return yDist / 4.9f;
}






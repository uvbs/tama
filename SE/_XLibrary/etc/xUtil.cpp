#include "stdafx.h"
#include "path.h"
#include "xUtil.h"
#include "debug.h"
#ifndef WIN32
#include "xString.h"
#endif
#include "Timer.h"

static DWORD s_next;
DWORD xRand()
{
	s_next = s_next * 1103515245 + 12345;
    int a=1;
	return s_next;
}
DWORD xRandom( DWORD nRange )
{
	if( XBREAK(nRange == 0) )	return 0;
	return xRand() % nRange;
}
DWORD xRandom( DWORD min, DWORD max )
{
	if( max < min )
		SWAP( min, max );
	return min + xRandom( max - min );
}
// 최대 256.0까지 range를 가질수 있음
float xRandomF( float fRange )
{
	return (float)(((double)xRand() / 0xFFFFFFFF) * fRange);
    //	return ((float)xRand() / (float)0xFFFFFFFF) * (float)fRange;
    /*	if( fRange == 0 )
     return 0;
     int nRange = (int)(fRange * (float)0x7FFF);	// 큰수를 곱해서 정수를 만듬
     int r = xRandom( nRange );
     fRange = (float)((double)r / (double)0x7FFF);
     return fRange; */
}
float xRandomF( float start, float end ) 
{
	float f = start + xRandomF( end-start );
	return f;
}
void xSRand( DWORD seed )
{
	//srand( seed );
	s_next = seed;
}
DWORD xGetSeed()
{
    return s_next;
}

int CalcFPS()
{
	static float dt = 0;
	static int frame = 0;
	static int frameRate = 0;
	static DWORD dwPrev = GetTickCount();
	DWORD dwTick = GetTickCount();
	float t = (float)(dwTick - dwPrev) / 1000;
	dwPrev = dwTick;
	dt += t;
	frame ++;
	if( dt > 1.0f )
	{
		frameRate = (int)(frame / dt);
		frame = 0;
		dt = 0;
	}

	return frameRate;
}

////////////////////////////////////////////////////////////
// 파일타이틀과 확장자를 리턴  ex) test.wri
static TCHAR _szTempBuff[2048];
#ifdef WIN32
static char _cTempBuff[2048];
#endif

LPCTSTR XE::Format( LPCTSTR szFormat, ... )
{
	va_list vl;
	va_start( vl, szFormat );
	_vstprintf_s( _szTempBuff, szFormat, vl );
	int len = _tcslen( _szTempBuff );
	int size = (sizeof(_szTempBuff) / sizeof(_szTempBuff[0]));
	XBREAK( len >= size );
	va_end( vl );
	return _szTempBuff;
}
#ifdef WIN32
const char* XE::Format( const char *cFormat, ... )
{
	va_list vl;
	va_start( vl, cFormat );
	sprintf_s( _cTempBuff, cFormat, vl );
	int len = strlen( _cTempBuff );
	int size = (sizeof(_cTempBuff) / sizeof(_cTempBuff[0]));
	XBREAK( len >= size );
	va_end( vl );
	return _cTempBuff;
}
#endif // win32

LPCTSTR XE::StrTrimLeft( LPCTSTR szStr, int n )
{
	XBREAK( n+2 >= MAX_PATH );
	_tcsncpy_s( _szTempBuff, szStr, n );
	return _szTempBuff;
}

// strFullPath에서 패스부분만 szBuff로 옮긴다 끝에 \도 살린다.
LPCTSTR XE::GetFilePath( LPCTSTR szSrc )
{
	XBREAK( _tcslen(szSrc) >= MAX_PATH );
	_tcscpy_s( _szTempBuff, szSrc );
#ifdef WIN32
	LPTSTR p = _tcsrchr( _szTempBuff, '\\' );
#else
	LPTSTR p = _tcsrchr( _szTempBuff, '/' );
#endif
	*(p+1) = 0;		// \뒤에 널이 들어가도록
	return _szTempBuff;
}
#ifdef WIN32
const char * XE::GetFilePath( const char * cSrc )
{
	XBREAK( strlen(cSrc) >= MAX_PATH );
	strcpy_s( _cTempBuff, cSrc );
#ifdef WIN32
	char *p = strrchr( _cTempBuff, '\\' );
#else
	char *p = strrchr( _cTempBuff, '/' );
#endif
	*(p+1) = 0;		// \뒤에 널이 들어가도록
	return _cTempBuff;
}
#endif // win32

LPCTSTR	XE::GetFileName( LPCTSTR szSrc )
{
	int		len, i;
	// 파일명만 추출.
	memset( _szTempBuff, 0, 256 );
	len = _tcslen( szSrc );		// 풀네임 길이 읽음
	i = len;
	while( i-- )
	{	// 스트링 뒤에서 부터 훑으며 \가 나오는곳을 찾는다.
#ifdef WIN32
		if( szSrc[i] == '\\' )
#else
		if( szSrc[i] == '/' )
#endif
		{
			_tcscpy_s( _szTempBuff, szSrc + i + 1 );	// \가 나온 다음 칸부터 파일명으로 씀
			break;
		}
		if( i == 0 )		// 끝까지 검색했는데도 \가 나오지 않았으면 그대로 씀
			_tcscpy_s( _szTempBuff, szSrc );
	}

	return _szTempBuff;
}

// 파일타이틀만 임시버퍼에 저장	ex) test
LPCTSTR	XE::GetFileTitle( LPCTSTR szSrc )
{
	int		len, i;
	LPCTSTR szName;
	TCHAR buff[256];

	szName = GetFileName( szSrc );		// 패스는 떼네고 파일명의 시작 포인터를 리턴
	_tcscpy_s( buff, szName );
	szSrc = buff;		// xuzhu.txt

	// 파일명만 추출.
	len = _tcslen( szSrc );		// 풀네임 길이 읽음
	for( i = 0; i < len; i ++ )
	{
		if( szSrc[i] == '.' )	break;
		_szTempBuff[i] = szSrc[i];
	}
	_szTempBuff[i] = '\0';

	return _szTempBuff;
}

LPCTSTR XE::GetFileExt( LPCTSTR szSrc )
{
	LPCTSTR p = _tcsrchr( szSrc, '.' );
	if( p == nullptr )
		return _T("");
	return p + 1;
}

///////////////////////////////////////////////

//////////////////////////////////////////////////
//        Math
#include <math.h>
#ifdef _VER_IPHONE
#include "xMathGL.h"
#endif
// srcAngle로부터 destAngle의 상대 각도(각도차이) -180' ~ +180 사이의 값
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

XE::VEC2 XE::GetAngleVector( float dAng, float dist )
{
	XE::VEC2 v;
	float rAng = D2R( dAng );
	v.x = cosf( rAng ) * dist;
	v.y = sinf( rAng ) * dist;
	return v;
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

// v1을 중심으로 반지름 radius인 원안에 v2좌표가 들어갔는지 검사
BOOL XE::IsIntersectCircle_Point( const XE::VEC2& vCenter, float radius, const XE::VEC2& v )
{
	XE::VEC2 vDist = v - vCenter;
	float len = vDist.Lengthsq();		// 두 점사이의 거리
	if( len < radius * radius )
		return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//      Image Processing
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
void XE::ConvertBlockARGB_To_ABGR( DWORD *pBuffer, int len )
{
	BYTE a, r, g, b;
	DWORD dw1;
	for( int i = 0; i < len; ++i )
	{
		dw1 = *pBuffer;
		a = XCOLOR_RGB_A( dw1 );
		r = XCOLOR_RGB_R( dw1 );
		g = XCOLOR_RGB_G( dw1 );
		b = XCOLOR_RGB_B( dw1 );
		*pBuffer++ = XCOLOR_ABGR( r, g, b, a );
	}
}

void XE::ConvertBlockABGR_To_ARGB( DWORD *pBuffer, int len )
{
	BYTE a, r, g, b;
	DWORD dw1;
	for( int i = 0; i < len; ++i )
	{
		dw1 = *pBuffer;
		a = XCOLOR_BGR_A( dw1 );		// abgr로 저장되어있는 데이타를 argb로 바꾼다
		r = XCOLOR_BGR_R( dw1 );
		g = XCOLOR_BGR_G( dw1 );
		b = XCOLOR_BGR_B( dw1 );
		*pBuffer++ = XCOLOR_RGBA( r, g, b, a );
	}
}
#endif






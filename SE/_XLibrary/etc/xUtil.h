#pragma once
#include <stdlib.h>
#include "xe.h"

template <typename NEWT, typename OLDT>
NEWT SafeCast( OLDT p ) {
	if( p == nullptr )
		return nullptr;
#ifdef DEBUG
	NEWT newp = dynamic_cast<NEWT>( p );
	XBREAK( newp == nullptr );
	return newp;
#else
	NEWT newp = static_cast<NEWT>( p );
	return newp;
	//	return (NEWT)(p);
#endif
}

using namespace std;
#define		SWAP( A, B )	{ A ^= B; B ^= A; A ^= B; }

DWORD xRand();
DWORD xRandom( DWORD nRange );
DWORD xRandom( DWORD min, DWORD max );
float xRandomF( float fRange );
float xRandomF( float start, float end ); 
void xSRand( DWORD seed );
DWORD xGetSeed();

int CalcFPS();

namespace XE 
{
	// TCHAR
	inline BOOL IsEmpty( LPCTSTR str )	{
		if( str == nullptr )
			return TRUE;
		return (str[0] == _T('\0')) ? TRUE : FALSE;
	}
	inline	BOOL IsHave( LPCTSTR str )	{
		return !IsEmpty(str);
	}
	inline BOOL IsSame( LPCTSTR sz1, LPCTSTR sz2 ) {
		return (_tcsicmp( sz1, sz2 ) == 0);
	}
#ifdef WIN32
	// char
	inline BOOL IsEmpty( const char *str )	{
		if( str == nullptr )
			return TRUE;
		return (str[0] == 0) ? TRUE : FALSE;
	}
	inline	BOOL IsHave( const char *str )	{
		return !IsEmpty(str);
	}
	inline BOOL IsSame( const char *sz1, const char *sz2 ) {
		return (_stricmp( sz1, sz2 ) == 0);
	}
#endif // win32
	LPCTSTR Format( LPCTSTR szFormat, ... );
#ifdef WIN32
	const char* Format( const char* cFormat, ... );
#endif
	LPCTSTR StrTrimLeft( LPCTSTR szStr, int n );

	LPCTSTR GetFilePath( LPCTSTR szSrc );
	const char* GetFilePath( const char * cSrc );
	LPCTSTR	GetFileName( LPCTSTR szSrc );
	LPCTSTR	GetFileTitle( LPCTSTR szSrc );
	LPCTSTR		GetFileExt( LPCTSTR szSrc );

	#define GET_FILE_NAME(S) ((_tcsrchr((S), ('\\')) ? _tcsrchr((S), ('\\')) : (S) - 1) + 1) 
	#define GET_FILE_EXT(S) ((_tcsrchr((S), ('.')) ? _tcsrchr((S), ('.')) : (S) - 1) + 1) 

	template<typename T, typename P>
	inline BOOL IsArea( T l, T t, T r, T b, P x, P y )	{
		if( (T)x >= l && (T)x <= r )
			if( (T)y >= t && (T)y <= b )
				return TRUE;
		return FALSE;
	}
	template<typename T, typename P>
	inline BOOL IsArea2( T l, T t, T w, T h, P x, P y ) {
		return IsArea( l, t, l+w-1, t+h-1, x, y );
	}
	template<typename T>
	inline BOOL IsArea( T l, T t, T w, T h, const VEC2& v ) {
		return IsArea( l, t, l+w-1, t+h-1, v.x, v.y );
	}
	inline BOOL IsArea( const VEC2& vAreaLT, const VEC2& size, const VEC2& vPoint ) {	// vAreaLT(좌상귀) - size영역에 vPoint가 들어가는지 검사
		return IsArea2( vAreaLT.x, vAreaLT.y, size.w, size.h, vPoint.x, vPoint.y );
	}
	// v1-v2 사각영역안에 vScr좌표가 들어가는지. 아직 size는 지원하지 않는다
	inline BOOL IsInside( const VEC2& vScr, const VEC2& v1, const VEC2& v2, const VEC2& size )	{
		float l  = xmin( v1.x, v2.x );
		float r = xmax( v1.x, v2.x );
		float t = xmin( v1.y, v2.y );
		float b = xmax( v1.y, v2.y );
		if( vScr.x >= l && vScr.x < r )
			if( vScr.y >= t && vScr.y < b )
				return TRUE;
		return FALSE;
	}
	float CalcDistAngle( float srcAngle, float destAngle );
	float CalcAngle( float x1, float y1, float x2, float y2 );
	float CalcAngle( float x, float y );
	XE::VEC2 GetAngleVector( float dAng, float dist );
#ifdef __cplusplus	// 나중에 xeMath.cpp이런쪽으로 옮길것
	bool GetIntersectPoint(const XE::VEC2& AP1, const XE::VEC2& AP2, const XE::VEC2& BP1, const XE::VEC2& BP2, XE::VEC2* IP);
	BOOL IsIntersectCircle_Point( const XE::VEC2& vCenter, float radius, const XE::VEC2& v );	
#endif
	// Image
#ifdef WIN32
	void ConvertBlockARGB_To_ABGR( DWORD *pBuffer, int len );
	void ConvertBlockABGR_To_ARGB( DWORD *pBuffer, int len );
#endif
} // namespace XE
//#define xmax(a, b) ((a) > (b) ? (a) : (b))
//#define xmin(a, b) ((a) < (b) ? (a) : (b))
//#define XCLEAR_ARRAY(BUFF)	memset( BUFF, 0, sizeof(BUFF) )


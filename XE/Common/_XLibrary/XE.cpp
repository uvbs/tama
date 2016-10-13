#include "stdafx.h"
#include "xe.h"
#include "etc/xUtil.h"
#include "XArchive.h"
#include <random>

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef _VER_ANDROID
#ifndef XE_ANDROID_VALID_STDAFX
// 안드로이드에선 반드시 _XLibrary/android_xe/stdafx.를 인클루드 해야한다.
#error "invalid stdafx.h include. must be include android_xe/stdafx.h in xe android engine."
#endif
#endif

#ifdef _XCRYPT_PACKET
#pragma message("defined _XCRYPT_PACKET")
#endif

#ifdef _XUZHU
#pragma message("----------------------------build version _XUZHU")
#endif 

#if _DEV_LEVEL == DLV_LOCAL
#pragma message("DLV_LOCAL==============================================")
#elif	_DEV_LEVEL == DLV_DEV_PERSONAL
#pragma message("DLV_DEV_PERSONAL==============================================")
#elif	_DEV_LEVEL == DLV_DEV_CREW
#pragma message("DLV_DEV_CREW==============================================")
#elif	_DEV_LEVEL == DLV_DEV_EXTERNAL
#pragma message("DLV_DEV_EXTERNAL==============================================")
#elif	_DEV_LEVEL == DLV_OPEN_BETA
#pragma message("DLV_OPEN_BETA==============================================")
#else
#error "DLV_XXX중 뭐라도 되어있어야 함"
#endif 

#ifdef _XPATCH
#pragma message("_XPATCH build==============================================")
#endif

#ifdef _DEV
#pragma message("_DEV ==============================================")
#endif

#pragma message("=================RENDER OPTIMIZE==========================")

static XRandom2 s_Random;

// namespace XE
namespace XE
{
	ID m_idGlobal = 1;			// 다용도 고유 아이디
	XE::VEC2 x_sizeGame;	// 게임의논리적 해상도(240x360 or 360x640...)
	//
	ID GenerateID() 
	{ 
#ifdef _SERVER
		// 스레드 세이프 버전
		return s_Random.GetRand();
#else
		DWORD idRand = 0;
		int cnt = 0;
		while( idRand == 0 ) {		// 0은 생성하지 못하도록
			idRand = xRand();
			if( ++cnt > 5 )
				idRand = rand();
		}
		return idRand; 
#endif
	}
	void xYMD::Serialize( XArchive& ar )
	{
		ar << year << month << day;
	}
	void xYMD::DeSerialize( XArchive& ar )
	{
		ar >> year >> month >> day;
	}
} // namespace XE

//////////////////////////////////////////////////////////////////////////
XRandom2::XRandom2() 
	: m_engineRandom( m_std_seed() ) 
{ 
	Init(); 
}
DWORD XRandom2::GetRand() 
{
	return m_engineRandom();
}
DWORD XRandom2::GetRandom( DWORD range ) 
{
	if( XBREAK( range == 0 ) )	return 0;
	return GetRand() % range;
}

int XRandom2::GetRandom( int min, int max ) 
{
	if( min == max )
		return min;
	if( max < min )
		std::swap( min, max );
	XASSERT( max - min + 1 > 0 );
	XBREAK( max - min + 1 > 0x7fffffff );	// 이렇게 큰수가 넘어가면 (int)했을때 마이너스가 될 우려가 있다.
	return min + (int)GetRandom( max - min + 1 );
}
float XRandom2::GetRandomF( float range ) 
{
	return (float)( ( (double)GetRand() / 0xFFFFFFFF ) * range );
}
float XRandom2::GetRandomF( float min, float max ) 
{
	if( min == max )
		return min;
	if( max < min )
		std::swap( min, max );
	float f = min + GetRandomF( max - min );
	return f;
}

// void XRandom2::SetSeed64( XINT64 seed ) 
// {
// 	m_engineRandom.seed( seed );
// }

void XRandom2::SetSeed( DWORD seed )
{
	m_engineRandom.seed( seed );
}


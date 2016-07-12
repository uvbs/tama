// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// xe_client.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

#ifdef _XCRYPT_PACKET
#pragma message("------------------------define _XCRYPT_PACKET")
#endif
#ifdef _XUZHU
#pragma message("-----------------------define xe client version _XUZHU")
#endif
#ifdef _VER_OPENGL
#pragma message("------------------------------------------build OpenGL")
#elif defined(_VER_DX)
#pragma message("---------------------------------------------build DirectX")
#else
#error "OpenGL이나 DX중 하나를 선택해야함"
#endif

// void* operator new( size_t size )
// {
// 	void* p = malloc( size );
// 	char cBuff[ 256 ];
// 	sprintf_s( cBuff, "0x%08x size=%d\n", (unsigned long)p, size );
// 	::OutputDebugStringA( cBuff );
// 	return p;
// }
// 
// void* operator new[]( size_t size )
// {
// 	void* p = malloc( size );
// 	char cBuff[ 256 ];
// 	sprintf_s( cBuff, "0x%08x size=%d\n", (unsigned long)p, size );
// 	::OutputDebugStringA( cBuff );
// 	return p;
// }
// 
// void operator delete( void *p )
// {
// 	free( p );
// }
// 
// void operator delete[]( void *p )
// {
// 	free( p );
// }
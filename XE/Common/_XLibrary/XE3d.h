#pragma once
#include <math.h>
#ifdef _VER_DX
	#include <d3dx9.h>
#else
  #ifdef _XGLM
	#include "xMathGLM.h"
  #else
	#include "Oolong_Engine2/include/Mathematics.h"
	#define GetMatrixPtr(M)		M.f
  #endif
#endif
#include "etc/types.h"
#include "xVector.h"

namespace X3D
{
#ifdef _VER_DX
	struct VEC3;
	//--------------------------
	// 3D Vector
	//--------------------------
 #ifdef DIRECT3D_VERSION
	struct VEC3 : public D3DXVECTOR3
	{
		VEC3() { x = y = z = 0; }
		VEC3( float x, float y, float z ) : D3DXVECTOR3( x, y, z ) {}
		VEC3( const XE::VEC2& vec2 ) : D3DXVECTOR3( vec2.x, vec2.y, 0 ) {}
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
#else // win32
//  #ifdef _XGLM
	
//  #else
#ifdef _XGLM
	struct VEC3 : public glm::vec3 //Vec3
#else
	struct VEC3 : public Vec3
#endif
	{
		VEC3() {}
		VEC3( float x, float y, float z ) : Vec3( x, y, z ) {}
		VEC3( const XE::VEC2& vec2 ) : Vec3( vec2.x, vec2.y, 0 ) {}
		template<typename T1, typename T2, typename T3>
		void Set( T1 _x, T2 _y, T3 _z ) { x = (float)_x; y = (float)_y; z = (float)_z; }
	};
//  #endif
#endif // not win32 
} // XE


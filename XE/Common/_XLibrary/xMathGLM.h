#pragma once
#ifdef _XGLM
/**
 컨셉은 d3d의 3d함수와 좌표계를 똑같이 쓰는것이다.
*/
#include "glm/matrix.hpp"
#include "glm/vec4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define MATRIX	glm::mat4
#define Vec3	glm::vec3
#define Vec4	glm::vec4

namespace XE {
	extern glm::mat4 s_mIdentity;
}

inline void MatrixTranslation( MATRIX& m, float x, float y, float z )
{
	m = glm::translate( XE::s_mIdentity, glm::vec3(x, y, z) );
}

inline void MatrixRotationX( MATRIX& m, float rAng )
{
	m = glm::rotate( XE::s_mIdentity, R2D(rAng), glm::vec3(1,0,0) );
}

inline void MatrixRotationXByDegree( MATRIX& m, float dAng )
{
	m = glm::rotate( XE::s_mIdentity, dAng, glm::vec3(1,0,0) );
}

inline void MatrixRotationY( MATRIX& m, float rAng )
{
	m = glm::rotate( XE::s_mIdentity, R2D(rAng), glm::vec3(0,1,0) );
}

inline void MatrixRotationYByDegree( MATRIX& m, float dAng )
{
	m = glm::rotate( XE::s_mIdentity, dAng, glm::vec3(0,1,0) );
}

inline void MatrixRotationZ( MATRIX& m, float rAng )
{
	m = glm::rotate( XE::s_mIdentity, R2D(rAng), glm::vec3(0,0,1) );
}

inline void MatrixRotationZByDegree( MATRIX& m, float dAng )
{
	m = glm::rotate( XE::s_mIdentity, dAng, glm::vec3(0,0,1) );
}

inline void MatrixRotationYawPitchRoll( MATRIX& m, float yaw, float pitch, float roll )
{
	m = glm::rotate( XE::s_mIdentity, yaw,	 glm::vec3(0,1,0) );
	m = glm::rotate(			   m, pitch, glm::vec3(1,0,0) );
	m = glm::rotate(			   m, roll,  glm::vec3(0,0,1) );
}



inline void MatrixScaling( MATRIX& m, float sx, float sy, float sz )
{
	m = glm::scale( XE::s_mIdentity, glm::vec3(sx, sy, sz) );
}

inline void MatrixIdentity( MATRIX& m )
{
	m = XE::s_mIdentity;
}

#define MatrixMultiply( m, m1, m2 )		m = m2 * m1		// gml은 dx와 곱하기가 반대라서 m2 * m1이 되었음

inline void MatrixVec4Multiply( Vec4& vOut4d, Vec3& v3d, const MATRIX& m )
{
	glm::vec4 v4(v3d.x, v3d.y, v3d.z, 1.f);
	vOut4d = m * v4;
}

inline void MatrixInverse( MATRIX& mOut, MATRIX& m )
{
	mOut = glm::inverse( m );
}

inline void MatrixMakeOrtho( MATRIX& m, float left, float right, float bottom, float top, float zNear, float zFar )
{
	m = glm::ortho( left, right, bottom, top, zNear, zFar );
}

inline float* GetMatrixPtr( MATRIX& m )
{
	return glm::value_ptr(m);	
}


#endif // glm
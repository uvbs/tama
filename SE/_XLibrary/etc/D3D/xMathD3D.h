#ifndef __XMATH_D3D_H__
#define __XMATH_D3D_H__

#define MATRIX		D3DXMATRIX
#define Vec3			D3DXVECTOR3
#define Vec4			D3DXVECTOR4
//#define MatrixTranslation( m, vXYZ )			D3DXMatrixTranslation( &m, vXYZ.x, vXYZ.y, vXYZ.z )		D3D와의 호환을 위해 뺐음
#define MatrixTranslation( m, x, y, z )			D3DXMatrixTranslation( &m, x, y, z )
#define MatrixRotationX( m, rAngle )		D3DXMatrixRotationX( &m, rAngle )
#define MatrixRotationY( m, rAngle )		D3DXMatrixRotationY( &m, rAngle )
#define MatrixRotationZ( m, rAngle )		D3DXMatrixRotationZ( &m, rAngle )
#define MatrixScaling( m, sx, sy, sz )			D3DXMatrixScaling( &m, sx, sy, sz )
#define MatrixIdentity( m )					D3DXMatrixIdentity( &m )
#define MatrixMultiply( m, m1, m2 )			D3DXMatrixMultiply( &m, &m1, &m2 )
// 3d에도 쓸수 있어야 하므로 모두 XE::VEC3를 받도록 하자
#define MatrixVec4Multiply( vOut4d, v3d, m )		D3DXVec3Transform( &vOut4d, &v3d, &m )
#define MatrixInverse(	mOut, m )	D3DXMatrixInverse( &mOut, nullptr, &m )
/*#define Vec2CatmullRom( OUT, V0, V1, V2, V3, LERP )	{ \
	D3DXVECTOR2 _vOut, _v0, _v1, _v2, _v3;		\
	_v0 = V0;		_v1 = V1;		_v2 = V2;		_v3 = V3;		\
	D3DXVec2CatmullRom( &_vOut, &_v0, &_v1, &_v2, &_v3, LERP );	\
	OUT = _vOut;		\
}*/
/*
// iOS쪽이랑 소스코드 호환성을 맞추려고 이걸썼음
inline void Vec2CatmullRom( XE::VEC2& pOut, XE::VEC2 v0, XE::VEC2 v1, XE::VEC2 v2, XE::VEC2 v3, float t )
{
	 pOut = (
	 (v1 * 2.0f) +
	 ((-v0) + v2) * t +
	 ((v0)*2.0f - (v1)*5.0f + (v2)*4.0f - (v3)) * t * t +
	 (-(v0) + (v1)*3.0f - (v2)*3.0f + (v3)) * t * t * t
	 ) * 0.5f;
}
*/

#endif // __XMATH_H__
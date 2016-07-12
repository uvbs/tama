#include "ModelGlobal.h"

int g_nMaxTri = 0;

static MATRIX s_mViewProj;		// 
static MATRIX s_mView;		// 
static MATRIX s_mProj;		// 
static BOOL s_bLight;
static BOOL s_bNight;

void	SetTransformViewProj( const MATRIX &mViewProj )
{
	s_mViewProj = mViewProj;
}

void	SetTransformView( const MATRIX &mView )
{
	s_mView = mView;
}

void	SetTransformProj( const MATRIX &mProj )
{
	s_mProj = mProj;
}

MATRIX *GetTransformView( void )
{
	return &s_mView;
}

// 렌더스테이트 라이트가 켜져있는가 아닌가.
void	SetLight( BOOL bLight )
{
	s_bLight = bLight;
}

BOOL GetLight( void )
{
	return s_bLight;
}

// 밤이면 트루.
void	SetNight( BOOL bNight )
{
	s_bNight = bNight;
}

BOOL GetNight( void )
{
	return s_bNight;
}

/////////////////////////////////////////////////
//
// CModelGlobal
//
////////////////////////////////////////////////
CModelGlobal g_ModelGlobal;

void	CModelGlobal::SetCamera( const Vec3 &vPos, const Vec3 &vDest )
{
	m_vCameraPos = vPos;
	m_vCameraDest = vDest;
	m_vCameraForward = vDest - vPos;
	MatrixVec3Normalize( m_vCameraForward, m_vCameraForward );
}

xRESULT CModelGlobal::RestoreDeviceObjects( void )
{
	return xSUCCESS;
}


#include "stdafx.h"
#include "etc/XSurface.h"
#include "etc/xGraphics.h"
#include "etc/xMath.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

DWORD XSurface::s_dwMaxSurfaceWidth = 0;
int XSurface::s_sizeTotalVMem = 0;
int XSurface::s_cntDPCallNoBatch = 0;
int XSurface::s_cntDPCallNormal = 0;

XE_NAMESPACE_START( XE )
// xSurfaceInfo::~xSurfaceInfo() {	
// 	SAFE_DELETE_ARRAY( m_pImg );
// }
//
// xVertex::xVertex() 
// {
// 	pos.z = 0;
// }
//
XE_NAMESPACE_END; // XE


void XE::xRenderParam::GetmTransform( MATRIX* pOut ) const {
	MATRIX& mWorld = (*pOut);
	MATRIX m;
	MatrixIdentity( mWorld );
	const auto& vAdjAxis = m_vAdjAxis;
	if( !vAdjAxis.IsZero() ) {
		MatrixTranslation( m, -vAdjAxis.x, -vAdjAxis.y, 0 );
		MatrixMultiply( mWorld, mWorld, m );
	}
	const auto& vScale = m_vScale;
	if( vScale.x != 1.0f || vScale.y != 1.0f || vScale.z != 1.0f ) {
		MatrixScaling( m, vScale.x, vScale.y, 1.0f );
		MatrixMultiply( mWorld, mWorld, m );
	}
	const auto& vRot = m_vRot;
	if( vRot.z ) {
		MatrixRotationZ( m, D2R( vRot.z ) );
		MatrixMultiply( mWorld, mWorld, m );
	}
	if( vRot.y ) {
		MatrixRotationY( m, D2R( vRot.y ) );
		MatrixMultiply( mWorld, mWorld, m );
	}
	if( !vAdjAxis.IsZero() ) {
		MatrixTranslation( m, vAdjAxis.x, vAdjAxis.y, 0 );
		MatrixMultiply( mWorld, mWorld, m );
	}
	const auto& vPos = m_vPos;
	MatrixTranslation( m, vPos.x, vPos.y, 0 );
	MatrixMultiply( mWorld, mWorld, m );
}

void XE::xRenderParam::SetFlipHoriz( bool bFlag ) {
	if( bFlag ) {
		m_dwDrawFlag |= EFF_FLIP_HORIZ;
		m_vRot.y += 180.f;
	} else {
		m_dwDrawFlag &= ~EFF_FLIP_HORIZ;
		m_vRot.y += 0.f;
	}
}
void XE::xRenderParam::SetFlipVert( bool bFlag ) {
	if( bFlag ) {
		m_dwDrawFlag |= EFF_FLIP_VERT;
		m_vRot.x += 180.f;
	} else {
		m_dwDrawFlag &= ~EFF_FLIP_VERT;
		m_vRot.x += 0;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void XSurface::Destroy() 
{
	XBREAK( XRefRes::Getid64Res() > 0 && XRefRes::GetnRefCnt() > 0 );
	SAFE_DELETE_ARRAY( __pSrcImg );
	SAFE_DELETE_ARRAY( m_pMask );
}

void XSurface::ClearVertices() 
{
	memset( m_Vertices, 0, sizeof( m_Vertices ) );
	m_Vertices[0].uv.x = 0.f;		m_Vertices[0].uv.y = 1.f;
	m_Vertices[1].uv.x = 1.f;		m_Vertices[1].uv.y = 1.f;
	m_Vertices[2].uv.x = 0.f;		m_Vertices[2].uv.y = 0.f;
	m_Vertices[3].uv.x = 1.f;		m_Vertices[3].uv.y = 0.f;
	m_Vertices[0].rgba.w = 1.f;
	m_Vertices[1].rgba.w = 1.f;
	m_Vertices[2].rgba.w = 1.f;
	m_Vertices[3].rgba.w = 1.f;
}

void XSurface::DestroyDevice()
{
	if( !m_bAtlas ) {
		AddSizeByte( -GetbytesMemAligned() );
		XSurface::s_sizeTotalVMem -= GetbytesMemAligned();
	}
	//memset( m_Vertices, 0, sizeof( m_Vertices ) );
	ClearVertices();
}

void XSurface::RestoreDeviceFromSrcImg()
{
	if( !m_bAtlas ) {
		AddSizeByte( GetbytesMemAligned() );
		XSurface::s_sizeTotalVMem += GetbytesMemAligned();
	}
}
/**
 @brief 
 외부 노출용 최초 진입점.
 모든 서피스 생성부는 
*/
bool XSurface::Create( const XE::POINT& sizeSurfaceOrig
										, const XE::VEC2& vAdj
										, XE::xtPixelFormat formatSurface
										, void* const pImgSrc
										, XE::xtPixelFormat formatImgSrc
										, const XE::POINT& sizeMemSrc
										, bool bSrcKeep, bool bMakeMask, bool bUseAtlas )
{
	SetAdjust( vAdj );
	// XE::xPF_ARGB8888 이외의 포맷은 아직 지원하지 않음
	XBREAK( formatImgSrc != XE::xPF_ARGB8888 );
	m_formatSurface = formatSurface;
	const int bppSurface = XE::GetBpp( formatSurface );
	SetsizeSurface( sizeSurfaceOrig );
	SetsizeMem( sizeMemSrc );
	// 실제 텍스쳐의 크기는 메모리의 크기 + aligned가 된다. 다만 화면에 그려질때 텍스쳐의 절반크기로 그려지는것 뿐이다.
 	m_sizeMemAligned = GRAPHICS->sAlignPowSize( sizeMemSrc );;
	//SetsizeAligned( sizeMemAligned );
	// 마스크 이미지 만들기 옵션
	if( bMakeMask ) {
		// 알파가 없는데도 마스크를 만드는 경우가 있나?
		XBREAK( !XE::IsAlphaFormat( formatSurface ) );
		const int sizeSrc = sizeMemSrc.Size();
		DWORD *pSrc = (DWORD*)pImgSrc;
		BYTE *pMask = new BYTE[ sizeSrc ];
		BYTE *_pMask = pMask;
		for( int i = 0; i < sizeSrc; i++ ) {
			*_pMask++ = XCOLOR_RGB_A( *pSrc++ );
		}
		SetpMask( pMask );
	}
	if( bSrcKeep ) {
		DWORD *pDst = new DWORD[ sizeMemSrc.Size() ];
		const int byteSrcImg = sizeMemSrc.Size() * sizeof(DWORD);
		memcpy_s( pDst, byteSrcImg, pImgSrc, byteSrcImg );
		SetSrcImg( pDst, sizeMemSrc );
	}
	// pure virtual
	bool bOk = Create( sizeSurfaceOrig
									, vAdj
									, formatSurface
									, pImgSrc
									, formatImgSrc
									, sizeMemSrc
									, m_sizeMemAligned
									, bUseAtlas );
	if( bOk ) {
		m_bAtlas = bUseAtlas;
		if( !m_bAtlas )
			AddSizeByte( m_sizeMemAligned.Size() * bppSurface );
	}
	return bOk;
}

/**
 @brief 외부 노출용
*/
bool XSurface::CreateSub( const XE::POINT& posMemSrc
											, const XE::POINT& sizeArea
											, const XE::POINT& sizeMemSrc
											, void* const _pSrcImg
											, XE::xtPixelFormat formatImgSrc
											, const XE::VEC2& sizeRender
											, const XE::VEC2& vAdj
											, XE::xtPixelFormat formatSurface
											, bool bSrcKeep, bool bMakeMask )
{
	SetAdjust( vAdj );
	// XE::xPF_ARGB8888 이외의 포맷은 아직 지원하지 않음
	XBREAK( formatImgSrc != XE::xPF_ARGB8888 );
	const int bppSurface = XE::GetBpp( formatSurface );
	m_formatSurface = formatSurface;
	SetsizeSurface( sizeRender );
	SetsizeMem( sizeMemSrc );
	// 실제 텍스쳐의 크기는 메모리의 크기 + aligned가 된다. 다만 화면에 그려질때 텍스쳐의 절반크기로 그려지는것 뿐이다.
 	m_sizeMemAligned = GRAPHICS->sAlignPowSize( sizeArea );;
	// 마스크 이미지 만들기 옵션
	if( bMakeMask ) {
		// 알파가 없는데도 마스크를 만드는 경우가 있나?
		XBREAK( !XE::IsAlphaFormat( formatSurface ) );
		DWORD *pSrc = (DWORD*)_pSrcImg;
		BYTE *pMask = new BYTE[ sizeArea.Size() ];
		BYTE *_pMask = pMask;
		for( int i = 0; i < sizeArea.h; ++i ) {
			for( int j = 0; j < sizeArea.w; ++j )
				*_pMask++ = XCOLOR_RGB_A( *pSrc++ );
			pSrc += ( sizeMemSrc.w - sizeArea.w );
		}
		SetpMask( pMask );
	}
	if( bSrcKeep ) {
		// 소스측 이미지 보관(잘린부분만)
		DWORD *pSrc = (DWORD*)_pSrcImg;
		DWORD *pDst = new DWORD[ sizeArea.Size() ];
		DWORD *_pDst = pDst;
		for( int i = 0; i < sizeArea.h; ++i ) {
			for( int j = 0; j < sizeArea.w; ++j )
				*_pDst++ = *pSrc++;
			pSrc += ( sizeMemSrc.w - sizeArea.w );
		}
		SetSrcImg( pDst, sizeArea );
	}
	const bool bOk = CreateSub( posMemSrc
														, sizeArea
														, m_sizeMemAligned
														, sizeMemSrc
														, _pSrcImg
														, formatImgSrc
														, sizeRender
														, vAdj
														, formatSurface );
	if( bOk ) {
		if( !m_bAtlas )
			AddSizeByte( m_sizeMemAligned.Size() * bppSurface );
	}
	return bOk;
}


// __pSrcImg의 처리들.
// lx,ly: 서피스크기 기준 로컬좌표
DWORD XSurface::GetPixel( float lx, float ly, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) 
{
	int w, h;
	DWORD *pImg = GetSrcImg( &w, &h );
	if( XBREAK( pImg == NULL ) )
		return 0;
	lx -= GetAdjustX();			// 버텍스좌표는 adj가 적용되어있으므로 그것을 원점기준으로 이동시킴
	ly -= GetAdjustY();			// 이미지 좌상귀 기준좌표로 변환
	if( lx < 0 || lx >= GetWidth() )		// 영역밖을 벗어나면 실패로 0리턴
		return 0;
	if( ly < 0 || ly >= GetHeight() )
		return 0;
	const XE::VEC2 vLocal = XE::VEC2( lx, ly ) * 2.f;
	auto posMem = XE::POINT( vLocal.x, vLocal.y );
// 	int memlx = ConvertToMemSize( lx );
// 	int memly = ConvertToMemSize( ly );
	XBREAK( posMem.w >= w );
	XBREAK( posMem.h >= h );
	XBREAKF( posMem.w < 0, "memlx(%d) < 0, lx=%f, ly=%f", posMem.w, lx, ly );
	XBREAKF( posMem.h < 0, "memly(%d) < 0, lx=%f, ly=%f", posMem.h, lx, ly );
	// 픽셀을 읽어냄
	DWORD pixel = pImg[ posMem.y * w + posMem.x ];
	if( pa ) *pa = XCOLOR_RGB_A( pixel );
	if( pr ) *pr =  XCOLOR_RGB_R( pixel );
	if( pg ) *pg =  XCOLOR_RGB_G( pixel );
	if( pb ) *pb =  XCOLOR_RGB_B( pixel );
	return pixel;
}

/**
 마스킹 이미지에서 lx,ly지점의 알파값을 얻어낸다.
 lx, ly좌표는 메모리 좌표가 아닌 서피스좌표
*/
BYTE XSurface::GetMask( float lx, float ly )
{
	int w = GetMemWidth();
	int h = GetMemHeight();
	BYTE *pImg = GetpMask();
	if( XBREAK( pImg == NULL ) )
		return 0;
	if( XBREAK( w == 0 ) )
		return 0;
	if( XBREAK( h == 0 ) )
		return 0;
	lx -= GetAdjustX();			// 버텍스좌표는 adj가 적용되어있으므로 그것을 원점기준으로 이동시킴
	ly -= GetAdjustY();			// 이미지 좌상귀 기준좌표로 변환
	if( lx < 0 || lx >= GetWidth() )		// 영역밖을 벗어나면 실패로 0리턴
		return 0;
	if( ly < 0 || ly >= GetHeight() )
		return 0;
	const XE::VEC2 vLocal = XE::VEC2( lx, ly ) * 2.f;
	auto posMem = XE::POINT( vLocal.x, vLocal.y );
//	auto posMem = XE::POINT(lx, ly) * 2;
// 	int memlx = ConvertToMemSize( lx );
// 	int memly = ConvertToMemSize( ly );
	if( XBREAK( posMem.x >= w ) )
		return 0;
	if( XBREAK( posMem.y >= h ) )
		return 0;
	if( XBREAKF( posMem.x < 0, "posMem.x(%d) < 0, lx=%f, ly=%f", posMem.x, lx, ly ) )
		return 0;
	if( XBREAKF( posMem.y < 0, "posMem.y(%d) < 0, lx=%f, ly=%f", posMem.y, lx, ly ) )
		return 0;
	BYTE a = pImg[ posMem.y * w + posMem.x ];
	return a;
}


// 서피스의 xSrc,ySrc - wDst,hDst 영역의 픽셀을 읽어 pDst에 카피한다.
void XSurface::CopyRectTo( DWORD *pDst, int wDst, int hDst, int xSrc, int ySrc )
{
	XBREAK( pDst == NULL );
	int w, h;
	DWORD *pImg = GetSrcImg( &w, &h );
	XBREAK( pImg == NULL );
	XBREAK( w <= 0 );
	XBREAK( h <= 0 );
//	XBREAK( xSrc < 0 );
//	XBREAK( ySrc < 0 );
	XBREAK( wDst <= 0 );
	XBREAK( hDst <= 0 );
//	XBREAK( xSrc+wDst > GetMemWidth() );
//	XBREAK( ySrc+hDst > GetMemHeight() );
	//
	DWORD *p = pDst;
	for( int i = ySrc; i < ySrc+hDst; ++i )
	{
		for( int j = xSrc; j < xSrc+wDst; ++j )
		{
			if( j >= 0 && j < w && i >= 0 && i < h )
				*p++ = pImg[ i * w + j ];
			else
				*p++ = 0;
		}
	}
}

// __pSrcImg의 처리들 끝
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _CLIENT

BOOL XSurface::IsInViewport( float x, float y, const MATRIX &mTM ) const
{
	XE::VEC2 vLT = XE::VEC2( x, y ) + XE::VEC2(m_AdjustX, m_AdjustY);
	XE::VEC2 vRB = XE::VEC2( x+GetWidth(), y+GetHeight() );
	X3D::VEC3 v3LT( vLT );
	X3D::VEC3 v3RB( vRB );
	Vec4 vtLT, vtRB;
	MatrixVec4Multiply( vtLT, v3LT, mTM );
	MatrixVec4Multiply( vtRB, v3RB, mTM );
	if( vtRB.x < vtLT.x )
		std::swap( vtRB.x, vtLT.x );
	if( vtRB.y < vtLT.y )
		std::swap( vtRB.y, vtLT.y );
	//
	XE::VEC2 vvpRB = GRAPHICS->GetViewportSize();
	if( vtLT.x > vvpRB.x )
		return FALSE;
	if( vtLT.y > vvpRB.y )
		return FALSE;
	if( vtRB.x < 0 )	// 좌표는 항상 뷰포트 기준으로 들어오고 뷰포트의 LT는 항상 0,0이기때문
		return FALSE;
	if( vtRB.y < 0 )
		return FALSE;
/*	if( vtLT.x > (float)GRAPHICS->GetViewportRight() )
		return FALSE;
	if( vtLT.y > (float)GRAPHICS->GetViewportBottom() )
		return FALSE;
	if( vtRB.x < (float)GRAPHICS->GetViewportLeft() )
		return FALSE;
	if( vtRB.y < (float)GRAPHICS->GetViewportTop() )
		return FALSE; */
	return TRUE;
}

BOOL XSurface::IsInViewport( float x, float y, float w, float h ) const
{
	XE::VEC2 vLT = XE::VEC2( x, y );
	XE::VEC2 vRB = XE::VEC2( x + w * m_fScaleX, y + h * m_fScaleY );
	if( vRB.x < vLT.x )
		std::swap( vRB.x, vLT.x );
	if( vRB.y < vLT.y )
		std::swap( vRB.y, vLT.y );
	//
	XE::VEC2 vvpRB = GRAPHICS->GetViewportSize();
	if( vLT.x > vvpRB.x )
		return FALSE;
	if( vLT.y > vvpRB.y )
		return FALSE;
	if( vRB.x < 0 )	// 좌표는 항상 뷰포트 기준으로 들어오고 뷰포트의 LT는 항상 0,0이기때문
		return FALSE;
	if( vRB.y < 0 )
		return FALSE;
/*	if( vLT.x > (float)GRAPHICS->GetViewportRight() )
		return FALSE;
	if( vLT.y > (float)GRAPHICS->GetViewportBottom() )
		return FALSE;
	if( vRB.x < (float)GRAPHICS->GetViewportLeft() )
		return FALSE;
	if( vRB.y < (float)GRAPHICS->GetViewportTop() )
		return FALSE; */
	return TRUE;
}

#endif

/**
 픽킹용 마스크를 만든다.
*/
BYTE* XSurface::MakeMask( DWORD *src, int memDstw, int memDsth )
{
	const int size = memDstw * memDsth;
	BYTE *pMask = new BYTE[ size ];
	BYTE *_pMask = pMask;
	memset( pMask, 0, size );
	for( int i = 0; i < size; i++ ) {
		*_pMask++ = XCOLOR_RGB_A( *src++ );
	}
	SetpMask( pMask );
	return pMask;
}

/**
 @brief 이미지를 vStart지점에서 vEnd지점쪽으로 늘이거나 줄여 찍는다.
 보통 화살표선 같은 이미지를 그릴때 사용한다.
 화살표의 경우 왼쪽에서 오른쪽으로 향하는 그림을 준비하고 화살표의 양끝은 
 이미지의 양끝에 딱 맞아야 한다. 
*/
void XSurface::DrawDirection( const XE::VEC2& vStart, const XE::VEC2& vEnd )
{
	XE::VEC2 vSize = GetSize();
	SetAdjustAxis( XE::VEC2( 0.f, vSize.h / 2.f ) );
//	pSurface->SetAdjustAxis( XE::VEC2( 16, 0 ) );
	float dAng = XE::CalcAngle( vStart, vEnd );
	SetRotateZ( dAng );
	XE::VEC2 vDist = vEnd - vStart;
	float dist = vDist.Length();
	XE::VEC2 vScale( 1.f, 1.0f );
	vScale.x *= dist / vSize.w;
	SetScale( vScale );
	// 이미지의 회전축을 draw좌표로 사용하기 위해 보정함.
	DrawByAxis( vStart );
}

/**
 @brief draw를 위해 세팅했던 임시 값들을 클리어시킴
*/
void XSurface::ClearAttr()
{
	m_fRotX = m_fRotY = m_fRotZ = 0;
	m_fScaleX = m_fScaleY = 1.0f;
	m_fAdjustAxisX = m_fAdjustAxisY = 0;
	m_fAlpha = 1.f;
	m_ColorR = m_ColorG = m_ColorB = 1.f;
	m_dwDrawFlag = 0;
	m_DrawMode = xDM_NORMAL;
	m_adjZ = 0;
}

void XSurface::GetMatrix( const XE::VEC2& vPos, MATRIX* pOut ) const 
{
	MATRIX& mWorld = (*pOut);
	MATRIX m;
	MatrixIdentity( mWorld );
	if( !GetAdjustAxis().IsZero() ) {
		MatrixTranslation( m, -GetfAdjustAxisX(), -GetfAdjustAxisY(), 0 );
		MatrixMultiply( mWorld, mWorld, m );
	}
	const auto vScale = GetScale();
	if( vScale.x != 1.0f || vScale.y != 1.0f || vScale.z != 1.0f ) {
		MatrixScaling( m, vScale.x, vScale.y, 1.0f );
		MatrixMultiply( mWorld, mWorld, m );
	}
	if( GetfRotZ() ) {
		MatrixRotationZ( m, D2R( GetfRotZ() ) );
		MatrixMultiply( mWorld, mWorld, m );
	}
	if( GetfRotY() ) {
		MatrixRotationY( m, D2R( GetfRotY() ) );
		MatrixMultiply( mWorld, mWorld, m );
	}
	if( !GetAdjustAxis().IsZero() ) {
		MatrixTranslation( m, GetfAdjustAxisX(), GetfAdjustAxisY(), 0 );
		MatrixMultiply( mWorld, mWorld, m );
	}
	MatrixTranslation( m, vPos.x, vPos.y, 0 );
	MatrixMultiply( mWorld, mWorld, m );
}

void XSurface::SetDrawMode( xDM_TYPE drawMode ) 
{
	m_DrawMode = drawMode;
	m__funcBlend = XE::ConvertDMTypeToBlendFunc( drawMode );
// 	switch( drawMode ) {
// 	case xDM_NORMAL:
// 	case xDM_MULTIPLY:
// 		m__funcBlend = XE::xBF_MULTIPLY;
// 		break;
// 	case xDM_SCREEN:
// 		m__funcBlend = XE::xBF_ADD;
// 		break;
// 	case xDM_SUBTRACT:
// 		m__funcBlend = XE::xBF_SUBTRACT;
// 		break;
// 	case xDM_GRAY:
// 		m__funcBlend = XE::xBF_GRAY;
// 		break;
// 	default:
// 		break;
// 	}
}
void XSurface::sSetglBlendFunc( XE::xtBlendFunc funcBlend,
																GLenum *pOutsfactor,
																GLenum *pOutdfactor )
{
	switch( funcBlend ) {
	case XE::xBF_NONE:
		XBREAK( 1 );
		*pOutsfactor = 0;
		*pOutdfactor = 0;
		break;
	case XE::xBF_NO_DRAW:
	case XE::xBF_GRAY:
	case XE::xBF_MULTIPLY:
		*pOutsfactor = GL_SRC_ALPHA;
		*pOutdfactor = GL_ONE_MINUS_SRC_ALPHA;
		break;
	case XE::xBF_ADD:
		*pOutsfactor = GL_SRC_ALPHA;
		*pOutdfactor = GL_ONE;
		break;
	case XE::xBF_SUBTRACT:
		*pOutsfactor = GL_ONE;
		*pOutdfactor = GL_ONE;
		break;
	default:
		break;
	}
}

void XSurface::DestroySurfaceInfo()
{
	if( m_spSurfaceInfo ) {
		SAFE_DELETE_ARRAY( m_spSurfaceInfo->m_pImg );
		m_spSurfaceInfo.reset();
	}
}


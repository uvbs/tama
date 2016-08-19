#include "stdafx.h"
#include "Mainfrm.h"
#include "xColor.h"
#include "Sprite.h"
//#include "SprDat.h"
#include "XSystem.h"

void XSprite::Destroy()
{
}

XSprite::XSprite( BOOL bHighReso, float width, float height, float adjustX, float adjustY, DWORD *pImg )
: XSurfaceD3D( bHighReso, width, height, adjustX, adjustY, pImg, TRUE )
{
	Init();
}

void XSprite::DrawNoAdjust( float x, float y )
{
	XSurfaceD3D::DrawNoAdjust( x, y );
}

void XSprite::Draw( float x, float y, const D3DXMATRIX &m )
{
	if( m_dwDrawFlag & EFF_FLIP_HORIZ )
		XSurface::SetRotateY( 180.0f );
	if( m_dwDrawFlag & EFF_FLIP_VERT )
		XSurface::SetRotateX( 180.0f );
	XSurfaceD3D::Draw( x, y, m );
	m_dwDrawFlag = 0;
}


void XSprite::Save( XResFile *pRes )
{
	int strSize = _tcslen( m_szSrcFile ) * sizeof(TCHAR) + sizeof(TCHAR);
	pRes->Write( &strSize, 4 );
	pRes->Write( m_szSrcFile, strSize );
//	pRes->Write( &m_rectSrc, sizeof(RECT) );
	DWORD dw0;
	BYTE b0 = (BYTE)m_formatSurface;
	pRes->Write( &b0, 1 );
	pRes->Write( &dw0, 3 );
	pRes->Write( &dw0, 4 );
	pRes->Write( &dw0, 4 );
	pRes->Write( &dw0, 4 );
	float width = GetWidth();
	float height = GetHeight();
	pRes->Write( &width, 4 );
	pRes->Write( &height, 4 );
	float adjustX = GetAdjustX();
	float adjustY = GetAdjustY();
	pRes->Write( &adjustX, 4 );
	pRes->Write( &adjustY, 4 );
	DWORD *pImg;
	int memw, memh;
	pImg = GetSrcImg( &memw, &memh );
	int size = memw * memh;
	DWORD *pTemp = new DWORD[ size ];
	memcpy( pTemp, pImg, size * 4 );
	DWORD *_pTemp = pTemp;
	XE::ConvertBlockARGB_To_ABGR( pTemp, size );		// ARGB를 ABGR로 변환
	// 압축
	BYTE *pComp;
	DWORD sizeComp;
	int sizeUncomp = size * 4;
	if( XSYSTEM::CreateCompressMem( &pComp, &sizeComp, (BYTE*)pTemp, size * 4 ) )
	{
		pRes->Write( &sizeComp, 4 );	// 압축된 용량
		pRes->Write( &sizeUncomp, 4 );	// 압축되기전 용량
		pRes->Write( pComp, sizeComp );
		SAFE_DELETE_ARRAY( pComp );
	} else
	{
		sizeComp = 0;
		sizeUncomp = 0;
		pRes->Write( &sizeComp, 4 );
		pRes->Write( &sizeUncomp, 4 );
		// 사이즈가 0이면 바디를 저장하지 않음.
	}
//	pRes->Write( pTemp, size * 4 );			// RGBA8888포맷
	SAFE_DELETE_ARRAY( pTemp );
}

/**
 @brief 
*/
void XSprite::Load( XResFile *pRes, int ver )
{
	int strSize;
	pRes->Read( &strSize, 4 );	// 널포함 크기
	pRes->Read( m_szSrcFile, strSize );
	DWORD dw0;
	if( ver >= 27 ) {
		BYTE b0;
		pRes->Read( &b0, 1 );		m_formatSurface = ( XE::xtPixelFormat )b0;
		if( XBREAK( m_formatSurface <= XE::xPF_NONE || m_formatSurface >= XE::xPF_MAX ) )
			m_formatSurface = XE::xPF_ARGB8888;
		pRes->Read( &dw0, 3 );
	} else {
		pRes->Read( &dw0, 4 );
	}
	pRes->Read( &dw0, 4 );
	pRes->Read( &dw0, 4 );
	pRes->Read( &dw0, 4 );
//	pRes->Read( &m_rectSrc, sizeof(RECT) );
	float width, height, adjx, adjy;
	pRes->Read( &width, 4 );		// 15버전 이후부터는 바로 float로 읽음
	pRes->Read( &height, 4 );
	pRes->Read( &adjx, 4 );
	pRes->Read( &adjy, 4 );
	SetAdjust( adjx, adjy );
	DWORD *pImg = NULL;
	// 예전 비압축 포맷
	int memw = ConvertToMemSize( width );
	int memh = ConvertToMemSize( height );
	int size = memw * memh;
	if( ver >= 25 )
	{
		// 압축포맷
		int sizeComp, sizeUncomp;
		pRes->Read( &sizeComp, 4 );
		pRes->Read( &sizeUncomp, 4 );
		if( sizeComp > 0 )
		{
			BYTE *pComp = new BYTE[ sizeComp ];
			pRes->Read( pComp, sizeComp );
			BYTE *pUncomp;
			if( XSYSTEM::UnCompressMem( &pUncomp, sizeUncomp, pComp, sizeComp ) )
			{
				XBREAK( size * 4 != sizeUncomp );
				SAFE_DELETE_ARRAY( pComp );
				pImg = (DWORD*) pUncomp;
			}
		}
	} else
	{
		pImg = new DWORD[ size ];
		pRes->Read( pImg, 4 * size );			// RGBA8888포맷
	}
	XE::ConvertBlockABGR_To_ARGB( pImg, size );		// BGR로 저장되어 있는 픽셀을 RGB로 바꿈
	//
	SetSrcImg( pImg, memw, memh );
	// 15버전 이후부터는 파일에 저장되는 사이즈 규격이 메모리크기에서 서피스 크기로 변경되었다.
	// 그러므로 15버전 이전파일들은 사이즈값을 줄여줘야 한다

	XSurfaceD3D::SetDevice( g_pMainFrm->GetDevice() );
	XSurfaceD3D::Create( width, height, adjx, adjy, xNO_ALPHA, pImg, 4, 0, TRUE );
}

// lx,ly: 서피스크기 기준 로컬좌표
/*DWORD XSprite::GetPixel( float lx, float ly, BYTE *pa, BYTE *pr, BYTE *pg, BYTE *pb ) 
{
	lx -= GetAdjustX();			// 버텍스좌표는 adj가 적용되어있으므로 그것을 원점기준으로 이동시킴
	ly -= GetAdjustY();			// 이미지 좌상귀 기준좌표로 변환
	if( lx < 0 || lx >= GetWidth() )		// 영역밖을 벗어나면 실패로 0리턴
		return 0;
	if( ly < 0 || ly >= GetHeight() )
		return 0;
	int w, h;
	DWORD *pImg = GetSrcImg( &w, &h );
	int memlx = ConvertToMemSize( lx );
	int memly = ConvertToMemSize( ly );
	XBREAK( memlx >= w );
	XBREAK( memly >= h );
	XBREAK( memlx < 0 );
	XBREAK( memly < 0 );
	DWORD pixel = pImg[ memly * w + memlx ];
	if( pa ) *pa = XCOLOR_RGB_A( pixel );
	if( pr ) *pr =  XCOLOR_RGB_R( pixel );
	if( pg ) *pg =  XCOLOR_RGB_G( pixel );
	if( pb ) *pb =  XCOLOR_RGB_B( pixel );
	return pixel;
}*/
D3DXMATRIX* XSprite::GetMatrix( D3DXMATRIX *pOut, float lx, float ly )
{
	lx += m_fAdjustAxisX;		// 좌표축 보정
	ly += m_fAdjustAxisY;
	D3DXMATRIX mTrans, mScale, mRot, mAxis;
	D3DXMatrixTranslation( &mTrans, lx, ly, 0 );
	D3DXMatrixRotationYawPitchRoll( &mRot, D3DXToRadian(m_fRotY), D3DXToRadian(m_fRotX), D3DXToRadian(m_fRotZ) );
	D3DXMatrixScaling( &mScale, m_fScaleX, m_fScaleY, 1.0f );
	D3DXMatrixTranslation( &mAxis, -m_fAdjustAxisX, -m_fAdjustAxisY, 0 );
	*pOut = mAxis * mScale * mRot * mTrans;
	return pOut;
}
BOOL XSprite::IsSameImage( DWORD *pImg )
{
	DWORD *pThisImg;
	int memw, memh;
	pThisImg = GetSrcImg( &memw, &memh );
	int size = memw * memh;
	while( size-- )
	{
		if( *pThisImg++ != *pImg++ )		// 픽셀을 검사하여 한점이라도 다르면 false를 리턴한다
			return FALSE;
	}
	return TRUE;
}

// 스프라이트 이미지를 저해상도로 변환한다.
BOOL XSprite::ConvertLowReso()
{
	SetHighReso( FALSE );		// 저해상도로 바꿈
	LPDIRECT3DTEXTURE9 pd3dTexture;		// 렌더 타겟 텍스쳐
	LPD3DXRENDERTOSURFACE pd3dRTS;	

	// create d3d resource
	HRESULT hr;
	const UINT w = (UINT)GetWidth();
	const UINT h = (UINT)GetHeight();
	hr = D3DDEVICE->CreateTexture( w, h,
												1,		// 밉맵레벨
												D3DUSAGE_DYNAMIC,		
												D3DFMT_A8R8G8B8,		// 텍스쳐 포맷
												D3DPOOL_DEFAULT,		
												&pd3dTexture,
												NULL );
	hr = D3DXCreateRenderToSurface( D3DDEVICE, 
												w, h,
												D3DFMT_A8R8G8B8,
												FALSE,
												D3DFMT_UNKNOWN,
												&pd3dRTS );
	// render to texture
	D3DVIEWPORT9 vp;
	vp.X = vp.Y = 0;
	vp.Width = w;
	vp.Height = h;
	vp.MinZ = 0;
	vp.MaxZ = 1;
	LPDIRECT3DSURFACE9 pd3dSurface;
	hr = pd3dTexture->GetSurfaceLevel( 0, &pd3dSurface );
	XE::SetProjection( w, h );
	hr = pd3dRTS->BeginScene( pd3dSurface, &vp );

	D3DDEVICE->Clear( 0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0 );
	DrawNoAdjust( 0, 0 );

	SAFE_RELEASE( pd3dSurface );
	pd3dRTS->EndScene( D3DX_FILTER_LINEAR );
	// 픽셀 추출해서 this에 다시 세팅함
	D3DLOCKED_RECT lockedRect;
	hr = pd3dTexture->LockRect( 0, &lockedRect, NULL, D3DLOCK_READONLY );

	DWORD *pDst = new DWORD[ w * h ];
	DWORD *pSrc = (DWORD *)lockedRect.pBits;
	for( int i = 0; i < (int)h; i ++ )
	{
		for( int j = 0; j < (int)w; j ++ )
		{
			pDst[ i * w + j ] = pSrc[ i * (lockedRect.Pitch/4) + j ];
		}
	}
	DestroySrcImg();		// 원래 비트맵 이미지 삭제
	SetSrcImg( pDst, w, h );		// 바뀐 비트맵으로 교체
	
	SAFE_RELEASE( pd3dTexture );
	SAFE_RELEASE( pd3dRTS );
	return TRUE;
}


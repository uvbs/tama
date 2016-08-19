#include "stdafx.h"
#include "XMapLayerImage.h"
#include "XGraphics.h"
#include "XMap.h"

XMapLayerImage::XMapLayerImage( XMap *pMap, LPCTSTR szImg, int wLayer, int hLayer )
	: XMapLayer( pMap, xTYPE_IMAGE )
{
	Init();
	LoadImg( szImg );
	m_wSize = wLayer;
	m_hSize = hLayer;
	m_DrawType = xNORMAL;
}

BOOL XMapLayerImage::LoadImg( LPCTSTR szImg )
{
	// 이미지를 통째로 읽음.
	int srcw, srch;
	DWORD *pSrcImg;
	if( GRAPHICS->LoadImg( szImg, &srcw, &srch, &pSrcImg ) == FALSE )
		return FALSE;
	_tcscpy_s( m_szFilename, XE::GetFileName( szImg ) );
	m_wTexture = srcw;
	m_hTexture = srch;
	// 최대텍스쳐 크기로 png를 분할
	int x = 0;
	// 윈도우에선 4096까지 쓸수 있지만 아이폰환경과 똑같이맞추기 위해 강제로 1024로 맞춤
#ifdef WIN32
	int pw = 1024;		// 최대 텍스쳐 가로크기 
#else
	int pw = XSurface::GetMaxSurfaceWidth();		// 최대 텍스쳐 가로크기 
#endif
	int num = srcw / pw;		// 배열이 전부 몇개가 필요할지 계산.
	if( srcw % pw )
		++num;
	m_listImage.Create( num );
	while( 1 )
	{
		if( pw > srcw - x )
			pw = srcw - x;
		XSurface *psfcNewBg = GRAPHICS->CreateSurface( FALSE, x, 0, srcw, srch, (float)pw, (float)srch, 0, 0, pSrcImg, FALSE );
		m_listImage.Add( psfcNewBg );
		x += pw;
		if( x >= srcw )
			break;
	}
	SAFE_DELETE_ARRAY( pSrcImg );
	return TRUE;
}

void XMapLayerImage::Draw( void )
{
	switch( m_DrawType )
	{
	case xNORMAL:
		DrawNormal();
		break;
	case xSTRETCH:
		DrawStretch();
		break;
	case xTILE:
		DrawTile();
		break;
	default:
		XBREAK(1);
	}
}
// 일반 찍기
void XMapLayerImage::DrawNormal( void )
{
	float fCameraScale = 1.0f;
	XE::VEC2 sizeView = m_prefMap->GetsizeViewport() / fCameraScale;	// 스케일된 뷰포트크기
//	XE::VEC2 vLT = m_prefMap->GetvwCurrent() - ( sizeView / 2.0f );		// 좌상귀 월드좌표
	XE::VEC2 vLT = m_prefMap->GetvwCurrent();		// 좌상귀 월드좌표
	float x = -(vLT.x * fCameraScale);
	float y = -(vLT.y * fCameraScale);
	float scaleh = 1.0f;
	float scalew = 1.0f;

	int num = m_listImage.GetNum();
	for( int i = 0; i < num; ++i )
	{
		m_listImage[i]->Draw( x, y );
		x += m_listImage[i]->GetWidth() * scalew;
	}
}
// 늘여 찍기
void XMapLayerImage::DrawStretch( void )
{
	float fCameraScale = m_prefMap->GetfCameraScale();
	XE::VEC2 sizeView = m_prefMap->GetsizeViewport() / fCameraScale;	// 스케일된 뷰포트크기
//	XE::VEC2 vLT = m_prefMap->GetvwCurrent() - ( sizeView / 2.0f );		// 좌상귀 월드좌표
	XE::VEC2 vLT = m_prefMap->GetvwCurrent();		// 좌상귀 월드좌표
	XE::VEC2 vPos = -(vLT * fCameraScale);
	vPos += (m_vDrawOffset * fCameraScale);
//	float x = -(vLT.x * fCameraScale);
//	float y = -(vLT.y * fCameraScale);
//	x += m_vDrawOffset.x;
//	y += m_vDrawOffset.y;
	//
	float scalew = m_prefMap->GetsizeWorld().w / (float)m_wTexture;	// 전체 월드에 꽉차게 늘이려면 얼마나 확대해야 하는지 계산.
	float scaleh = m_prefMap->GetsizeWorld().h / (float)m_hTexture;	// 전체 월드에 꽉차게 늘이려면 얼마나 확대해야 하는지 계산.
	scalew *= fCameraScale;
	scaleh *= fCameraScale;
	int num = m_listImage.GetNum();
	for( int i = 0; i < num; ++i )
	{
		m_listImage[i]->SetScale( scalew, scaleh, 1.0f );		
		m_listImage[i]->Draw( vPos );
		vPos.x += m_listImage[i]->GetWidth() * scalew;
	}
}
// 타일화 찍기
void XMapLayerImage::DrawTile( void )
{
}

//
void XMapLayerImage::Load( XResFile *pRes, LPCTSTR szPath )
{
	int size;
	pRes->Read( &size, 4 );
	UNICHAR szBuff[ 32 ];
	pRes->Read( szBuff, size );
#ifdef WIN32
	_tcscpy_s( m_szFilename, szBuff );
#else
	strcpy_s( m_szFilename, _ConvertUTF16ToUTF8( szBuff ) );
#endif
	DWORD dw;
	pRes->Read( &dw, 4 );
	m_DrawType = (xtType)dw;
	pRes->Read( &m_wSize, 4 );
	pRes->Read( &m_hSize, 4 );
	//
	TCHAR szFullPath[ 1024 ];
	_tcscpy_s( szFullPath, XE::Format( _T("%s%s"), szPath, m_szFilename ) );
	LoadImg( szFullPath );
}


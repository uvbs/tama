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
	// �̹����� ��°�� ����.
	int srcw, srch;
	DWORD *pSrcImg;
	if( GRAPHICS->LoadImg( szImg, &srcw, &srch, &pSrcImg ) == FALSE )
		return FALSE;
	_tcscpy_s( m_szFilename, XE::GetFileName( szImg ) );
	m_wTexture = srcw;
	m_hTexture = srch;
	// �ִ��ؽ��� ũ��� png�� ����
	int x = 0;
	// �����쿡�� 4096���� ���� ������ ������ȯ��� �Ȱ��̸��߱� ���� ������ 1024�� ����
#ifdef WIN32
	int pw = 1024;		// �ִ� �ؽ��� ����ũ�� 
#else
	int pw = XSurface::GetMaxSurfaceWidth();		// �ִ� �ؽ��� ����ũ�� 
#endif
	int num = srcw / pw;		// �迭�� ���� ��� �ʿ����� ���.
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
// �Ϲ� ���
void XMapLayerImage::DrawNormal( void )
{
	float fCameraScale = 1.0f;
	XE::VEC2 sizeView = m_prefMap->GetsizeViewport() / fCameraScale;	// �����ϵ� ����Ʈũ��
//	XE::VEC2 vLT = m_prefMap->GetvwCurrent() - ( sizeView / 2.0f );		// �»�� ������ǥ
	XE::VEC2 vLT = m_prefMap->GetvwCurrent();		// �»�� ������ǥ
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
// �ÿ� ���
void XMapLayerImage::DrawStretch( void )
{
	float fCameraScale = m_prefMap->GetfCameraScale();
	XE::VEC2 sizeView = m_prefMap->GetsizeViewport() / fCameraScale;	// �����ϵ� ����Ʈũ��
//	XE::VEC2 vLT = m_prefMap->GetvwCurrent() - ( sizeView / 2.0f );		// �»�� ������ǥ
	XE::VEC2 vLT = m_prefMap->GetvwCurrent();		// �»�� ������ǥ
	XE::VEC2 vPos = -(vLT * fCameraScale);
	vPos += (m_vDrawOffset * fCameraScale);
//	float x = -(vLT.x * fCameraScale);
//	float y = -(vLT.y * fCameraScale);
//	x += m_vDrawOffset.x;
//	y += m_vDrawOffset.y;
	//
	float scalew = m_prefMap->GetsizeWorld().w / (float)m_wTexture;	// ��ü ���忡 ������ ���̷��� �󸶳� Ȯ���ؾ� �ϴ��� ���.
	float scaleh = m_prefMap->GetsizeWorld().h / (float)m_hTexture;	// ��ü ���忡 ������ ���̷��� �󸶳� Ȯ���ؾ� �ϴ��� ���.
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
// Ÿ��ȭ ���
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


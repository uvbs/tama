#include "stdafx.h"
#ifdef _XEMAP
#include "XEMapLayerImage.h"
#include "XEMap.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XEMapLayerImage::XEMapLayerImage( XEMap *pMap, ID idLayer, LPCTSTR resImg ) 
	: XEMapLayer( pMap, xTYPE_IMAGE, idLayer, resImg ) 
{ 
	Init(); 
	LoadImg( resImg );
	m_wSize = m_sizeSurface.w;
	m_hSize = m_sizeSurface.h;
	m_DrawType = xNORMAL;
}

XEMapLayerImage::XEMapLayerImage( XEMap *pMap, LPCTSTR resImg, int wLayer, int hLayer )
	: XEMapLayer( pMap, xTYPE_IMAGE )
{
	Init();
	LoadImg( resImg );
	m_wSize = (float)wLayer;
	m_hSize = (float)hLayer;
	m_DrawType = xNORMAL;
}

void XEMapLayerImage::Destroy() 
{
	for( int i = 0; i < m_listImage.GetNum(); ++i )
	{
		XSurface *p = m_listImage[i]; 
		SAFE_DELETE( p );
	}
}

BOOL XEMapLayerImage::LoadImg( LPCTSTR resImg )
{
	m_strResPath = resImg;
	BOOL bResult = RestoreDevice();
	return bResult;
}

BOOL XEMapLayerImage::RestoreDevice( void )
{
	// 이미지를 통째로 읽음.
	int srcw, srch;
	DWORD *pSrcImg;
	if( GRAPHICS->LoadImg( m_strResPath.c_str(), &srcw, &srch, &pSrcImg ) == FALSE )
	{
		XLOG("XMapLayerImage::RestoreDevice: failed LoadImg(%s)", m_strResPath.c_str());
		return FALSE;
	}
	//	_tcscpy_s( m_szFilename, XE::GetFileName( szImg ) );
	m_sizeTexture = XE::VEC2( srcw, srch );
	m_sizeSurface = XE::VEC2( srcw, srch ) / 2.f;	// HighReso옵션일 경우임.
	XBREAK( srch > 1024 );
	// 최대텍스쳐 크기로 png를 분할
	int x = 0;
	// 윈도우에선 4096까지 쓸수 있지만 아이폰환경과 똑같이맞추기 위해 강제로 1024로 맞춤
#ifdef WIN32
	int pw = 1024;		// 최대 텍스쳐 가로크기 
#else
	int pw = XSurface::GetMaxSurfaceWidth();		// 최대 텍스쳐 가로크기 
#endif
	XBREAK( pw == 0 );
	int num = srcw / pw;		// 배열이 전부 몇개가 필요할지 계산.
	if( srcw % pw )
		++num;
	// 리스토어전에 서피스가 할당되어있었으면 디바이스 아이디 클리어시키고 삭제시킨다.
	XARRAYLINEAR_LOOP( m_listImage, XSurface*, psfcImg )
	{
		psfcImg->ClearDevice();
		SAFE_DELETE( psfcImg );
	} END_LOOP;
	// 어레이 파괴하고 다시 생성
	m_listImage.DestroyAll();
	m_listImage.Create( num );
	while( 1 )
	{
		if( pw > srcw - x )
			pw = srcw - x;
		// 가로로 긴 그림일경우 최대텍스쳐크기에 맞춰 각각 잘라냄.
		XSurface *psfcNewBg = GRAPHICS->CreateSurface( TRUE, 
													x, 0, 
													srcw, srch, 
													(float)pw/2.f, (float)srch/2.f, 
													0, 0, 
													pSrcImg, FALSE );
		m_listImage.Add( psfcNewBg );
		x += pw;
		if( x >= srcw )
			break;
	}
	SAFE_DELETE_ARRAY( pSrcImg );
	return TRUE;
}


void XEMapLayerImage::Draw( const XE::VEC2& vsLT )
{
	switch( m_DrawType )
	{
	case xNORMAL:
		DrawNormal( vsLT );
		break;
	case xSTRETCH:
		DrawStretch( vsLT );
		break;
	case xTILE:
		DrawTile( vsLT );
		break;
	case xLOOP:
		DrawLoop( vsLT );
		break;
	default:
		XBREAK(1);
	}
}
// 일반 찍기
void XEMapLayerImage::DrawNormal( const XE::VEC2& vsLT )
{
	float fCameraScale = 1.0f;
	XE::VEC2 sizeView = m_prefMap->GetsizeViewport() / fCameraScale;	// 스케일된 뷰포트크기
//	XE::VEC2 vLT = m_prefMap->GetvwCurrent() - ( sizeView / 2.0f );		// 좌상귀 월드좌표
	XE::VEC2 vLT = m_prefMap->GetvwCurrent();		// 좌상귀 월드좌표
	float x = -(vLT.x * fCameraScale);
	float y = -(vLT.y * fCameraScale);
	float scaleh = 1.0f;
	float scalew = 1.0f;
	x += vsLT.x;
	y += vsLT.y;

	int num = m_listImage.GetNum();
	for( int i = 0; i < num; ++i )
	{
		m_listImage[i]->Draw( x, y );
		x += m_listImage[i]->GetWidth() * scalew;
	}
}
// 늘여 찍기
void XEMapLayerImage::DrawStretch( const XE::VEC2& vsLT )
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
	float scalew = m_prefMap->GetsizeWorld().w / m_sizeTexture.w;	// 전체 월드에 꽉차게 늘이려면 얼마나 확대해야 하는지 계산.
	float scaleh = m_prefMap->GetsizeWorld().h / m_sizeTexture.h;	// 전체 월드에 꽉차게 늘이려면 얼마나 확대해야 하는지 계산.
	scalew *= fCameraScale;
	scaleh *= fCameraScale;
	int num = m_listImage.GetNum();
	vPos += vsLT;
	for( int i = 0; i < num; ++i )
	{
		m_listImage[i]->SetScale( scalew, scaleh, 1.0f );		
		m_listImage[i]->Draw( vPos );
		vPos.x += m_listImage[i]->GetWidth() * scalew;
	}
}
// 타일화 찍기
void XEMapLayerImage::DrawTile( const XE::VEC2& vsLT )
{
}

// 뺑뺑이 찍기
void XEMapLayerImage::DrawLoop( const XE::VEC2& vsLT )
{
	float fCameraScale = 1.0f;
	XE::VEC2 sizeView = m_prefMap->GetsizeViewport() / fCameraScale;	// 스케일된 뷰포트크기
//	XE::VEC2 vLT = m_prefMap->GetvwCurrent() - ( sizeView / 2.0f );		// 좌상귀 월드좌표
	XE::VEC2 vLT = m_prefMap->GetvwCurrent();		// 좌상귀 월드좌표
	int wx = (int)vLT.x;
	wx %= (int)m_sizeSurface.w;
	vLT.x = (float)wx;
	float x = -(vLT.x * fCameraScale);
	float y = -(vLT.y * fCameraScale);
	x += vsLT.x;
	y += vsLT.y;
	float scaleh = 1.0f;
	float scalew = 1.0f;

	DrawTexture( x, y );
	x += m_sizeSurface.w;
	// 텍스쳐를 찍고도 오른쪽이 남았으면 채울때까지 반복해서 찍는다.
	while( x < sizeView.w )
	{
		DrawTexture( x, y );
		x += m_sizeSurface.w;
	}
}

void XEMapLayerImage::DrawTexture( float x, float y )
{
	float scalew = 1.0f;
	int num = m_listImage.GetNum();
	for( int i = 0; i < num; ++i )
	{
		m_listImage[i]->Draw( x, y );
		x += m_listImage[i]->GetWidth() * scalew;
	}
}

//
void XEMapLayerImage::Load( XResFile *pRes, LPCTSTR szPath )
{
	int size;
	pRes->Read( &size, 4 );
	UNICHAR szBuff[ 32 ];
	pRes->Read( szBuff, size );
#ifdef WIN32
//	_tcscpy_s( m_szFilename, szBuff );
	m_strResPath = XE::MakePath( DIR_IMG, szBuff );
#else
	TCHAR szUTF8[ 256 ];
	_ConvertUTF16ToUTF8( szUTF8, szBuff );
	m_strResPath = XE::MakePath( DIR_IMG, szUTF8 );
#endif
	DWORD dw;
	pRes->Read( &dw, 4 );
	m_DrawType = (xtType)dw;
	pRes->Read( &m_wSize, 4 );
	pRes->Read( &m_hSize, 4 );
	//
	TCHAR szFullPath[ 1024 ];
	_tcscpy_s( szFullPath, XE::Format( _T("%s%s"), szPath, m_strResPath.c_str() ) );
	LoadImg( szFullPath );
}

#endif // _XEMAP

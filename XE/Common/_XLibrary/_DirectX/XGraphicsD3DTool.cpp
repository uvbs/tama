#include "stdafx.h"

#ifdef _VER_DX
#ifdef _VER_OPENGL
#error "_VER_DX와 _VER_OPENGL은 함께 쓸 수 없음."
#endif
#ifndef _XLIBPNG	// libpng를 사용하지 않을때만 wincodec을 사용한다.
#include "XGraphicsD3DTool.h"
#include "XResMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XGraphicsD3DTool* XGraphicsD3DTool::s_pGraphicsD3DTool = NULL;		// 일단 이렇게 하고 나중에 멀티플랫폼용으로 고치자.

// 이제 LIBPNG를 쓰므로 D3DTool은 사용하지 않는다.
#error "not use XGraphicsD3DTool because use libpng"

void XGraphicsD3DTool::Init( void ) 
{
//	m_pFont9 = NULL;
//	m_pSprite9 = NULL;
//	m_pTxtHelper = NULL;
//	m_nFontSize = 15;
	// Ignore the return value because we want to continue running even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    if (SUCCEEDED(CoInitialize(NULL)))
	{
		HRESULT hr;
		// Create WIC factory.
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<void **>(&m_pWICFactory)
			);
	} else
		XERROR( "CoInitialize Failed" );
}

void XGraphicsD3DTool::Destroy()
{
    SafeRelease(&m_pWICFactory);
//	SAFE_RELEASE( m_pSprite9 );
//	SAFE_RELEASE( m_pFont9 );
//	SAFE_DELETE( m_pTxtHelper );
	CoUninitialize();
}

/*HRESULT XGraphicsD3DTool::CreateFont( int nFontSize ) 
{
	HRESULT hr;
	m_nFontSize = nFontSize;
	hr = D3DXCreateFont( GetDevice(), m_nFontSize, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"맑은 고딕", &m_pFont9 );
	if( XBREAK( hr != S_OK ) )
		return hr;
	if( m_pFont9 ) V_RETURN( m_pFont9->OnResetDevice() );
	m_pTxtHelper = new CDXUTTextHelper( m_pFont9, m_pSprite9, NULL, NULL, m_nFontSize );
	return hr;
}*/

XSurface*	XGraphicsD3DTool::CreateSurface( BOOL bHighReso, LPCTSTR szRes, BOOL bSrcKeep/*=FALSE*/, BOOL bMakeMask/*=FALSE*/ )
{
	int w, h;
	DWORD *pImg;
	if( LoadImg( szRes, &w, &h, &pImg ) == FALSE )
		return NULL;

	if( bHighReso )
	{
		if( w & 1 )
			XERROR( "경고: %s의 가로크기가 2로 나누어 떨어지지 않습니다", szRes );
		w /= 2;
		h /= 2;		// 이 파일이 고해상도로 지정되어 있으면 실제 서피스 크기는 절반이 된다
	}
	XSurface *pSurface = new XSurfaceD3D( bHighReso, (float)w, (float)h, 0, 0, pImg, bSrcKeep, bMakeMask );
	pSurface->SetstrRes( szRes );
	return pSurface;

}

// png를 로드하여 메모리에 올려 ppImage로 되돌려준다.
BOOL XGraphicsD3DTool::LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage )		
{
	HRESULT hr;
    IWICBitmapDecoder *pDecoder = NULL;
    IWICBitmapFrameDecode *pSource = NULL;
    IWICFormatConverter *pConverter = NULL;
	BOOL bRet = FALSE;
	*pWidth = *pHeight = 0;		// 에러나서 리턴될수도 있기때문에 미리 초기화시킨다 초기화안한 변수였을경우 쓰레기값이 그대로 남아있는 문제를 방지하기 위함
	*ppImage = NULL;
	TCHAR szFullpath[ 1024 ];
	LPCTSTR szRes = szFilename;
	// szRes의 풀패스를 얻어낸다.
	XE::SetReadyRes( szFullpath, szRes );
	if( XE::IsEmpty( szFullpath ) == TRUE )
		return FALSE;
	XBREAK( m_pWICFactory == NULL );
	hr = m_pWICFactory->CreateDecoderFromFilename(
        szFullpath,
        NULL,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &pDecoder
        );
    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = pDecoder->GetFrame(0, &pSource);
    } else
	{
//		XERROR( "파일읽기 실패:%s", szFilename );
//		XALERT( "파일읽기 실패:%s", szFilename );
		return FALSE;
	}
    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = m_pWICFactory->CreateFormatConverter(&pConverter);
    }
    if (SUCCEEDED(hr))
    {
		hr = pConverter->Initialize(
                pSource,
                GUID_WICPixelFormat32bppBGRA,
//                GUID_WICPixelFormat32bppPBGRA,	// 아씨바 이옵션 쓰면 알파부분이 시커멓게 나온다.
                WICBitmapDitherTypeNone,
                NULL,
                0.f,
                WICBitmapPaletteTypeMedianCut
                );
	}

    if (SUCCEEDED(hr))
	{
		WICPixelFormatGUID pPixelFormat;
		double dpiX, dpiY;
		UINT w, h;
		pConverter->GetPixelFormat( &pPixelFormat );
		pConverter->GetResolution( &dpiX, &dpiY );
		pConverter->GetSize( &w, &h );
		IWICPixelFormatInfo *piPixelFormatInfo = NULL;
		IWICComponentInfo *piCompoInfo = NULL;
		hr = m_pWICFactory->CreateComponentInfo( pPixelFormat, &piCompoInfo );
		if( SUCCEEDED(hr) )
		{
			hr = piCompoInfo->QueryInterface( IID_IWICPixelFormatInfo,(LPVOID *)&piPixelFormatInfo );
		} else
			XBREAKF( 1, "CreateComponentInfo failed" );
//			XERROR( "CreateComponentInfo 실패" );
		UINT bpp=0;
		if( SUCCEEDED(hr) )
		{
			hr = piPixelFormatInfo->GetBitsPerPixel( &bpp );
			SafeRelease( &piPixelFormatInfo );
			if( bpp != 32 )
			{
				//XERROR( "32bit포맷만 가능합니다" );
				XBREAKF( 1, "32bit포맷만 가능합니다" );
				return FALSE;
			}
			UINT stride = (w * (bpp/8) + 3) & ~3;
			UINT size = w * h * (bpp/8);
			BYTE *pData = new BYTE[ size ];
			pConverter->CopyPixels( NULL, stride, size, pData );
			
			*pWidth = w;	*pHeight = h;
			*ppImage = (DWORD *)pData;
			bRet = TRUE;
		} else
			XBREAKF( 1, "pDecoder->GetFrame 실패" );
			//XERROR( "pDecoder->GetFrame 실패" );
	}

    SafeRelease(&pDecoder);
    SafeRelease(&pSource);
    SafeRelease(&pConverter);

	return bRet;
}
/*
void XGraphicsD3DTool::DrawNumber( float x, float y, int num, D3DCOLOR col )
{
	TCHAR szBuff[256];
	_stprintf_s( szBuff, _T("%d"), num );
	DrawString( x, y, szBuff, col );
}
void XGraphicsD3DTool::DrawNumber( float x, float y, float num, D3DCOLOR col )
{
	TCHAR szBuff[256];
	_stprintf_s( szBuff, _T("%.1f"), num );
	DrawString( x, y, szBuff, col );
}
void XGraphicsD3DTool::DrawString( float x, float y, LPCTSTR szStr, D3DCOLOR color )
{
	x = x * ((float)GetPhyScreenWidth() / GetScreenWidth()) ;
	y = y * ((float)GetPhyScreenHeight() / GetScreenHeight());
	m_pTxtHelper->Begin();
	m_pTxtHelper->SetInsertionPos( (int)x, (int)y );
	m_pTxtHelper->SetForegroundColor( color );
	m_pTxtHelper->DrawTextLine( szStr );
	m_pTxtHelper->End(); 
}
*/
#endif // not _XLIBPNG
#endif // _VER_DX

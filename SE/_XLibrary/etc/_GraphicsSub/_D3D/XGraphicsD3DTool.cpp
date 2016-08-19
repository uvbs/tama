#include "stdafx.h"
#include "XGraphicsD3DTool.h"
#include <stdio.h>

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XGraphicsD3DTool* XGraphicsD3DTool::s_pGraphicsD3DTool = nullptr;		// 일단 이렇게 하고 나중에 멀티플랫폼용으로 고치자.

void XGraphicsD3DTool::Init() 
{
//	m_pFont9 = nullptr;
//	m_pSprite9 = nullptr;
//	m_pTxtHelper = nullptr;
//	m_nFontSize = 15;
	// Ignore the return value because we want to continue running even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
    if (SUCCEEDED(CoInitialize(nullptr)))
	{
		HRESULT hr;
		// Create WIC factory.
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
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
	m_pTxtHelper = new CDXUTTextHelper( m_pFont9, m_pSprite9, nullptr, nullptr, m_nFontSize );
	return hr;
}*/

XSurface*	XGraphicsD3DTool::CreateSurface( BOOL bHighReso, LPCTSTR szFilename, BOOL bSrcKeep )
{
	int w, h;
	DWORD *pImg;
	if( LoadImg( szFilename, &w, &h, &pImg ) == FALSE )
		return nullptr;

	if( bHighReso )
	{
		w /= 2;
		h /= 2;		// 이 파일이 고해상도로 지정되어 있으면 실제 서피스 크기는 절반이 된다
	}
	XSurface *pSurface = new XSurfaceD3D( bHighReso, (float)w, (float)h, 0, 0, pImg, FALSE );
	pSurface->SetszFilename( XE::GetFileName( szFilename ) );
	return pSurface;

}

// png를 로드하여 메모리에 올려 ppImage로 되돌려준다.
BOOL XGraphicsD3DTool::LoadImg( LPCTSTR szFilename, int *pWidth, int *pHeight, DWORD **ppImage )		
{
	HRESULT hr;
    IWICBitmapDecoder *pDecoder = nullptr;
    IWICBitmapFrameDecode *pSource = nullptr;
    IWICFormatConverter *pConverter = nullptr;
	BOOL bRet = FALSE;

	*pWidth = *pHeight = 0;		// 에러나서 리턴될수도 있기때문에 미리 초기화시킨다 초기화안한 변수였을경우 쓰레기값이 그대로 남아있는 문제를 방지하기 위함
	*ppImage = nullptr;
    hr = m_pWICFactory->CreateDecoderFromFilename(
        szFilename,
        nullptr,
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
		XALERT_OKCANCEL( "파일읽기 실패:%s", szFilename );
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
                nullptr,
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
		IWICPixelFormatInfo *piPixelFormatInfo = nullptr;
		IWICComponentInfo *piCompoInfo = nullptr;
		hr = m_pWICFactory->CreateComponentInfo( pPixelFormat, &piCompoInfo );
		if( SUCCEEDED(hr) )
		{
			hr = piCompoInfo->QueryInterface( IID_IWICPixelFormatInfo,(LPVOID *)&piPixelFormatInfo );
		} else
			XBREAK_ERROR( 1, "CreateComponentInfo 실패" );
//			XERROR( "CreateComponentInfo 실패" );
		UINT bpp=0;
		if( SUCCEEDED(hr) )
		{
			hr = piPixelFormatInfo->GetBitsPerPixel( &bpp );
			SafeRelease( &piPixelFormatInfo );
			if( bpp != 32 )
			{
				//XERROR( "32bit포맷만 가능합니다" );
				XBREAK_ERROR( 1, "32bit포맷만 가능합니다" );
				return FALSE;
			}
			UINT stride = (w * (bpp/8) + 3) & ~3;
			UINT size = w * h * (bpp/8);
			BYTE *pData = new BYTE[ size ];
			pConverter->CopyPixels( nullptr, stride, size, pData );
			
			*pWidth = w;	*pHeight = h;
			*ppImage = (DWORD *)pData;
			bRet = TRUE;
		} else
			XBREAK_ERROR( 1, "pDecoder->GetFrame 실패" );
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

bool XGraphicsD3DTool::SavePng( LPCTSTR szFilename, int width, int height, DWORD *pImage )		
{
	const int LenFilespecMAX = 1024;
// 	char FileSpecPNG[LenFilespecMAX + 1];
	char tstr[2000];
	wchar_t wcstring[LenFilespecMAX+1];
	size_t NumCharsConverted;
	IWICStream* piStream = nullptr;
	IWICBitmapEncoder *piEncoder = NULL;
	IWICBitmapFrameEncode *piBitmapFrame = NULL;
	IPropertyBag2 *piPropertybag = NULL;
// 	strcpy_s( FileSpecPNG, SZ2C(szFilename) );

	//--------------------------------------------------------------------------------
//	strcpy_s( FileSpecPNG, "test001.png" );
// 	clasFilespecInfo *FileSpecInfo = NULL;
// 	FileSpecInfo = new clasFilespecInfo( szFileSpec );
// 	if( FileSpecInfo->DirIsValid()
// 		&& FileSpecInfo->FnameIsValid()
// 		&& FileSpecInfo->DirExists()
// 		&& !FileSpecInfo->FnameIsWild()
// 		) {
// 		strcpy( FileSpecPNG, FileSpecInfo->szDir() );
// 		strcat( FileSpecPNG, FileSpecInfo->szNameLessExt() );
// 		strcat( FileSpecPNG, ".png" );
// 		delete FileSpecInfo;
// 	} else {
// 		delete FileSpecInfo;
// 		return 0;
// 	}

	HRESULT hr = m_pWICFactory->CreateStream( &piStream );
	if( SUCCEEDED( hr ) ) {
//		mbstowcs_s( &NumCharsConverted, wcstring, ( sizeof FileSpecPNG ), FileSpecPNG, strlen( FileSpecPNG ) + 1 );
// 		hr = piStream->InitializeFromFilename( wcstring, GENERIC_WRITE );
		hr = piStream->InitializeFromFilename( szFilename, GENERIC_WRITE );
	}

	if( SUCCEEDED( hr ) ) {
		hr = m_pWICFactory->CreateEncoder( GUID_ContainerFormatPng, NULL, &piEncoder );
	}

	if( SUCCEEDED( hr ) ) {
		hr = piEncoder->Initialize( piStream, WICBitmapEncoderNoCache );
	}

	if( SUCCEEDED( hr ) ) {
		hr = piEncoder->CreateNewFrame( &piBitmapFrame, &piPropertybag );
	}

	if( SUCCEEDED( hr ) ) {
		//X   // This is how you customize the TIFF output.
		//X   PROPBAG2 option = { 0 };
		//X   option.pstrName = L"PngCompressionMethod";
		//X   VARIANT varValue;    
		//X   VariantInit(&varValue);
		//X   varValue.vt = VT_UI1;
		//X   varValue.bVal = WICPngCompressionZIP;      
		//X   hr = piPropertybag->Write(1, &option, &varValue);        
		if( SUCCEEDED( hr ) ) {
			hr = piBitmapFrame->Initialize( piPropertybag );
		}
	}

	if( SUCCEEDED( hr ) ) {
		hr = piBitmapFrame->SetSize( UINT( width ), UINT( height ) );
		if( !SUCCEEDED( hr ) )
			sprintf( tstr, "SetSize() returned HRESULT = %08x.\n", long( hr ) );
	}

// 	WICPixelFormatGUID formatGUID = GUID_WICPixelFormat24bppBGR;
// 	if( SUCCEEDED( hr ) ) {
// 		hr = piBitmapFrame->SetPixelFormat( &formatGUID );
// 	}
// 
// 	if( SUCCEEDED( hr ) ) {
// 		// We're expecting to write out 24bppRGB. Fail if the encoder cannot do it.
// 		hr = IsEqualGUID( formatGUID, GUID_WICPixelFormat24bppBGR ) ? S_OK : E_FAIL;
// 	}
	WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppBGRA;
	if( SUCCEEDED( hr ) ) {
		hr = piBitmapFrame->SetPixelFormat( &formatGUID );
	}

	if( SUCCEEDED( hr ) ) {
		// We're expecting to write out 24bppRGB. Fail if the encoder cannot do it.
		hr = IsEqualGUID( formatGUID, GUID_WICPixelFormat32bppBGRA ) ? S_OK : E_FAIL;
	}

	//X if (SUCCEEDED(hr))
	//X {
	//X   //X UINT cbStride = (pBitmap->bmWidth * pBitmap->bmBitsPixel + 7)/8;   /*  **WICGetStride** */
	//X   //X UINT cbBufferSize = pBitmap->bmHeight * cbStride;
	//X   UINT cbStride = pBitmap->bmWidthBytes;
	//X   UINT cbBufferSize = pBitmap->bmHeight * cbStride;
	//X   BYTE *pbBuffer = new BYTE[cbBufferSize];
	//X   if (pbBuffer != NULL)
	//X   {
	//X     //X for (UINT i = 0; i < cbBufferSize; i++)
	//X     //X   pbBuffer[i] = static_cast<BYTE>(rand());
	//X     //X hr = piBitmapFrame->WritePixels(uiHeight, cbStride, cbBufferSize, pbBuffer);
	//X     hr = piBitmapFrame->WritePixels(pBitmap->bmHeight, pBitmap->bmWidthBytes, cbBufferSize, pBitmap->bmBits);
	//X     delete[] pbBuffer;
	//X   }
	//X   else
	//X   {
	//X     hr = E_OUTOFMEMORY;
	//X   }
	//X }

	if( SUCCEEDED( hr ) ) {
		const int bmWidthBytes = width * 4;
		int i, j, k;
		UINT cbBufferSize = height * bmWidthBytes;
		BYTE *pbBuffer = new BYTE[cbBufferSize];
		if( pbBuffer != NULL ) {
			BYTE /**pByteDST, */*pByteSRC, *pB;
			pB = (BYTE*)pImage;
			k = 0;
			pByteSRC = pB;
			for( i = 0; i < height; i++ ) {
				for( j = 0; j < bmWidthBytes; j++ ) {
					pbBuffer[k++] = *( pByteSRC++ );
// 					pbBuffer[k++] = *( pByteSRC++ );
// 					pbBuffer[k++] = *( pByteSRC++ );
// 					pbBuffer[k++] = *( pByteSRC++ );
				}
//				pByteSRC += bmWidthBytes;
			}
			//X for (k=0; k<cbbuffersize; k++)
			//x   pbBuffer[k] = *(pB++);
			hr = piBitmapFrame->WritePixels( height, bmWidthBytes
				, cbBufferSize, pbBuffer );
			delete pbBuffer;
		}
	}
// 	if( SUCCEEDED( hr ) ) {
// 		const int bmWidthBytes = width * 4;
// 		int i, j, k;
// 		UINT cbBufferSize = height * bmWidthBytes;
// 		BYTE *pbBuffer = new BYTE[cbBufferSize];
// 		if( pbBuffer != NULL ) {
// 			BYTE /**pByteDST, */*pByteSRC, *pB;
// 			pB = (BYTE*)pImage;
// 			k = 0;
// 			pByteSRC = pB + cbBufferSize - bmWidthBytes;
// 			for( i = 0; i < height; i++ ) {
// 				for( j = 0; j < bmWidthBytes; j++ ) {
// 					pbBuffer[k++] = *( pByteSRC++ );
// 				}
// 				pByteSRC -= 2 * bmWidthBytes;
// 			}
// 			//X for (k=0; k<cbbuffersize; k++)
// 			//x   pbBuffer[k] = *(pB++);
// 			hr = piBitmapFrame->WritePixels( height, bmWidthBytes
// 																		, cbBufferSize, pbBuffer );
// 			delete pbBuffer;
// 		}
// 	}

	if( SUCCEEDED( hr ) )
		hr = piBitmapFrame->Commit();

	if( SUCCEEDED( hr ) )
		hr = piEncoder->Commit();

// 	if( m_pWICFactory )
// 		m_pWICFactory->Release();
	if( piBitmapFrame )
		piBitmapFrame->Release();
	if( piEncoder )
		piEncoder->Release();
	if( piStream )
		piStream->Release();
	return hr == S_OK;
}


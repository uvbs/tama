#ifndef __XGRAPHICSD3DTOOL_H__
#define __XGRAPHICSD3DTOOL_H__

#pragma warning(disable: 4005)		// 위 워닝을 끄는 명령임

#include "XGraphicsD3D.h"
//#include "DXUT.h"
//#include "SDKmisc.h"
#include <wincodec.h>

// 툴등에서 사용하는 확장판 DXUT/WIC같은걸 쓴다
// 게임에서 png를 로드하는등의 일을 해야할수도 있다. 그러므로 D3DTool이라는 이름을 쓰지말고
// D3DEX 같은이름으로 바꿔서 WIC/DXUT를 링크시켜야할때 확장된 클래스를 쓰도록 하자
class XGraphicsD3DTool : public XGraphicsD3D {
public:
	static XGraphicsD3DTool *s_pGraphicsD3DTool;	
private:
    IWICImagingFactory *m_pWICFactory;
//	ID3DXFont*                  m_pFont9;
//	ID3DXSprite*                m_pSprite9;
//	CDXUTTextHelper*            m_pTxtHelper;
//	int m_nFontSize;
	
	void Init();
	void Destroy();
public:
	XGraphicsD3DTool() { Init(); }
	XGraphicsD3DTool( int nResolutionWidth, int nResolutionHeight, xPixelFormat pixelFormat ) : XGraphicsD3D( nResolutionWidth, nResolutionHeight, pixelFormat ) 
	{
		XLOG("");
		Init();
	}
	virtual ~XGraphicsD3DTool() { Destroy(); }
	// get/set
//	GET_ACCESSOR( int, nFontSize );
//	float GetnFontSize() { return WcToGcF( (float)m_nFontSize ); }
//	GET_ACCESSOR( CDXUTTextHelper*, pTxtHelper );
	//
	XE::VEC2 GcToWc( XE::VEC2 vg ) {		// 게임좌표를 윈도우좌표로
		float rx = ((float)GetPhyScreenWidth() / GetScreenWidth());
		float ry = ((float)GetPhyScreenHeight() / GetScreenHeight());
		vg.x *= rx;
		vg.y *= ry;
		return vg;
	}
	XE::VEC2 GcToWc( float gPos ) {		// 게임좌표를 윈도우좌표로
		float rx = ((float)GetPhyScreenWidth() / GetScreenWidth());
		float ry = ((float)GetPhyScreenHeight() / GetScreenHeight());
		return XE::VEC2( gPos * rx, gPos * ry );
	}
	float GcToWcF( float gPos ) {		// 게임좌표를 윈도우좌표로
		float rx = ((float)GetPhyScreenWidth() / GetScreenWidth());		// 이때는 가로해상도기준으로만 한다
		return gPos * rx;
	}
	float WcToGcF( float gPos ) {		// 윈도우좌표->게임좌표
		float rx = ((float)GetScreenWidth() / GetPhyScreenWidth());		// 이때는 가로해상도기준으로만 한다
		return gPos * rx;
	}
	XE::VEC2 WcToGcF( XE::VEC2 vw ) {		// 윈도우좌표->아이폰좌표
		float rx = ((float)GetScreenWidth() / GetPhyScreenWidth());
		float ry = ((float)GetScreenHeight() / GetPhyScreenHeight());
		vw.x *= rx;
		vw.y *= ry;
		return vw;
	}
	virtual XSurface*	CreateSurface( BOOL bHighReso, LPCTSTR szFilename, BOOL bSrcKeep = FALSE );
	BOOL LoadImg( LPCTSTR szFilename , int *pWidth, int *pHeight,DWORD **ppImage );		// png 로드하여 메모리에 올린다.
//	HRESULT CreateFont( int nFontSize );
/*	void DrawNumber( float x, float y, float num, D3DCOLOR col = XCOLOR_WHITE );
	void DrawNumber( float x, float y, int num, D3DCOLOR col = XCOLOR_WHITE );
	void DrawString( float x, float y, LPCTSTR szStr, D3DCOLOR color = XCOLOR_WHITE );
	void DrawString( int x, int y, LPCTSTR szStr, D3DCOLOR color ) { DrawString( (float)x, (float)y, szStr, color ); }
	template<typename T>
	void DrawString( T x, T y, D3DCOLOR color, LPCTSTR szFormat, ...  ) {
		TCHAR szBuff[1024];
		va_list         vl;
		va_start(vl, szFormat);
		_vstprintf_s(szBuff, 1024, szFormat, vl);
		va_end(vl);
		if( XBREAK( _tcslen(szBuff) >= 1024 ) )
			return;
		DrawString( (float)x, (float)y, szBuff, color );
	}
	void DrawString( XE::VEC2 pos, LPCTSTR szStr, D3DCOLOR color = XCOLOR_WHITE ) { DrawString( pos.x, pos.y, szStr, color ); }
*/
	bool SavePng( LPCTSTR szFilename, int width, int height, DWORD *pImage );
};

#define	GRAPHICS_D3DTOOL	XGraphicsD3DTool::s_pGraphicsD3DTool

//#define  XFONT_SIZE	(LE::g_pFont->GetFontWidth())
#endif // __XGRAPHICSD3DTOOL_H__
#ifndef __XSURFACED3DMOBILE_H__
#define __XSURFACED3DMOBILE_H__

#include <windows.h>
#include <d3dx9.h>
#include "dxerr.h"
//#include "XGraphicsD3D.h"
#include "XSurface.h"
namespace XE
{
	extern D3DXMATRIX s_mViewProj;
	extern LPD3DXEFFECT s_pEffect;
	inline const D3DXMATRIX& GetMatrixViewProj() { return s_mViewProj; }
	inline void SetMatrixViewProj( const D3DXMATRIX *pm ) { s_mViewProj = *pm; } 
	inline LPD3DXEFFECT GetEffect() { return s_pEffect; }
	inline void SetEffect( LPD3DXEFFECT pEffect ) { s_pEffect = pEffect; }
	BOOL LoadShader( LPCTSTR szFilename );
	float GetDT();
	// 초기화와 파괴
	BOOL Init( LPDIRECT3DDEVICE9 pd3dDevice, int nScrWidth, int nScrHeight );
	BOOL Init( LPDIRECT3DDEVICE9 pd3dDevice, int nScrWidth, int nScrHeight, LPD3DXEFFECT pEffect );
	void Destroy();
}

/*#ifdef _XTOOL
	// 툴에서는 DXUT의 코드를 쓴다
	#define XV(x)           V(x)
	#define XV_RETURN(x)    V_RETURN(x)
#else */
	// 게임에선 DXUT를 안쓸수도 있으므로 독자적인 매크로를 쓴다
	//--------------------------------------------------------------------------------------
	// Display error msg box to help debug 
	//--------------------------------------------------------------------------------------
	inline HRESULT WINAPI DXXTrace( const CHAR* strFile, DWORD dwLine, HRESULT hr,
							  const WCHAR* strMsg, bool bPopMsgBox )
	{
	//    bool bShowMsgBoxOnError = DXUTGetShowMsgBoxOnError();
	//    if( bPopMsgBox && bShowMsgBoxOnError == false )
	//        bPopMsgBox = false;
#if (_MSC_VER < 1900) // vs2015이하
		return DXTrace( strFile, dwLine, hr, strMsg, bPopMsgBox );
#else
//		if( bPopMsgBox )
//			AfxMessageBox( strFile );
		::OutputDebugString( strMsg );
		return hr;
#endif
	}
	#if defined(DEBUG) || defined(_DEBUG)
	#ifndef XV
	#define XV(x)           { hr = (x); if( FAILED(hr) ) { DXXTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
	#endif
	#ifndef XV_RETURN
	#define XV_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return DXXTrace( __FILE__, (DWORD)__LINE__, hr, L#x, true ); } }
	#endif
	#else
	#ifndef XV
	#define XV(x)           { hr = (x); }
	#endif
	#ifndef XV_RETURN
	#define XV_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
	#endif
	#endif
//#endif // not _XTOOL

struct XSURFACE_VERTEX
{
	D3DXVECTOR3 position;     // The position
	float       tu, tv;   // The texture coordinates
};
// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_XSURFACE_VERTEX (D3DFVF_XYZ | D3DFVF_TEX1 )

class XSurfaceD3D: public XSurface
{
public:
	static LPDIRECT3DDEVICE9       s_pd3dDevice;
	static void SetDevice( 	LPDIRECT3DDEVICE9 pd3dDevice ) { s_pd3dDevice = pd3dDevice; }
private:
	D3DPOOL						m_TexPool;
	D3DPOOL						m_VBPool;
	LPDIRECT3DTEXTURE9 m_pTexture; 
	LPDIRECT3DVERTEXBUFFER9		m_pVB;		
	LPDIRECT3DVERTEXBUFFER9		m_pVBSub;		// DrawSub()를 위한 별도의 vertex buffer 

	void Init() {
		m_pTexture = nullptr;
		m_pVB = nullptr;
		m_pVBSub = nullptr;

		m_TexPool = D3DPOOL_DEFAULT;
		m_VBPool = D3DPOOL_DEFAULT;
		m_nAlignedWidth = m_nAlignedHeight = 0;
	}
	void Destroy();
protected:
//	DWORD *m_pImg;
public:
	XSurfaceD3D& operator=( const XSurfaceD3D& rhs ) {
		XSurface::ValueAssign( &rhs );
		// 메모리할당이 필요없는 값들만 대입하기위해 포인터값들은 모두 제외
		return *this;
	}
	virtual void ValueAssign( const XSurface *pSrc ) {
		XSurface::ValueAssign( pSrc );
//		*this = *(dynamic_cast<XSurfaceD3D *>(pSrc));
		// 값만 대입하므로 포인터관련 변수들은 카피하지 않음
	}

	XSurfaceD3D( BOOL bHighReso ) : XSurface( bHighReso ) { Init(); }
	XSurfaceD3D( BOOL bHighReso, LPCTSTR szFilename, BOOL bKeepSrc=FALSE ) : XSurface( bHighReso, szFilename ) { Init(); LoadImg( szFilename, bKeepSrc ); }
	XSurfaceD3D( BOOL bHighReso, float width, float height, float adjustX, float adjustY, DWORD *pImg, BOOL bSrcKeep ) : XSurface( bHighReso, width, height, adjustX, adjustY ) {
		Init();
		Create( width, height, adjustX, adjustY, xALPHA, pImg, sizeof(DWORD), 0, bSrcKeep );
	}
	XSurfaceD3D( BOOL bHighReso, const int srcx, const int srcy, const int srcw, const int srch, const float dstw, const float dsth, const float adjx, const float adjy, void *_pSrcImg, BOOL bSrcKeep = FALSE ) : XSurface( bHighReso ) {
		Init();
		Create( srcx, srcy, srcw, srch, dstw, dsth, adjx, adjy, _pSrcImg, 4, bSrcKeep );
	}
	XSurfaceD3D( int wSurface, int hSurface, int wTexture, int hTexture, int adjx, int adjy, DWORD *pImg, BOOL bSrcKeep=FALSE ) 
		: XSurface( (wSurface != wTexture)? TRUE : FALSE ) {
		Init();
		XBREAK( wTexture > wSurface && wSurface * 2 != wTexture );
		XBREAK( hTexture > hSurface && hSurface * 2 != hTexture );
		Create( (float)wSurface, (float)hSurface, (float)adjx, (float)adjy, xALPHA, (const void *)pImg, 4, 0, bSrcKeep );
	}
	virtual ~XSurfaceD3D() { Destroy(); }

	XE::xRESULT ChangeAdjust( float adjx, float adjy );		// SE에서 쓰는듯
	BOOL SetImage( const float width, const float height, const float adjx, const float adjy, DWORD *pNewImg );
	virtual XE::xRESULT Create( const float width, const float height, const float _adjx, const float _adjy, xAlpha alpha, const void *pImg, int bytePerPixel, XCOLOR dwKeyColor=0, BOOL bSrcKeep = FALSE );
	virtual XE::xRESULT Create( const int srcx, const int srcy, const int srcw, const int srch, const float dstw, const float dsth, const float adjx, const float adjy, void *_pSrcImg, const int bpp, BOOL bSrcKeep = FALSE );		// 이미지의 일부분만 서피스로 만드는 버전

	XE::xRESULT	LoadImg( LPCTSTR szFilename, BOOL bKeepSrc=FALSE );

	void CopySurface( XSurface *src );

	virtual void Draw( float x, float y ) { 
		D3DXMATRIXA16 m; 
		D3DXMatrixIdentity( &m ); 
		Draw( x, y, m ); 
	}
	void Draw( float x, float y, const D3DXMATRIX &m );
	void Draw( const XE::VEC2& vPos ) { Draw( vPos.x, vPos.y ); }
	virtual void DrawLocal( float x, float y, float lx, float ly, const D3DXMATRIX &mParent );	// x,y:최종스크린좌표, lx, ly 로컬내에서 좌표. XWndDialog::Draw의 타일화 draw참고
	void DrawLocal( float x, float y, float lx, float ly ) { D3DXMATRIXA16 m; D3DXMatrixIdentity( &m ); DrawLocal( x, y, lx, ly, m ); }		// parent매트릭스 생략버전
	void DrawLocal( const XE::VEC2& vScreen, const XE::VEC2& vLocal, const D3DXMATRIX &mParent ) { DrawLocal( vScreen.x, vScreen.y, vLocal.x, vLocal.y, mParent ); }	// XE::VEC2버전
	void DrawCore( int nAlpha=255 );
	void DrawCoreSub();
	void DrawNoAdjust( float x, float y ) {
		x -= GetAdjustX() * m_fScaleX;
		y -= GetAdjustY() * m_fScaleY;
		Draw( x, y );
	}
	void DrawSub( float x, float y, const RECT *src );
	void Fill( DWORD col );
	void RenderTexture( float x, float y, float w, float h, LPDIRECT3DTEXTURE9 pd3dTexture );

public:
	static DWORD s_dwTextureCaps;
	static void Initialize( DWORD dwTextureCaps ) { s_dwTextureCaps = dwTextureCaps; }

};

//extern D3DXMATRIX g_mViewProj;
//extern LPD3DXEFFECT g_pEffect;


#endif // __XSURFACED3DMOBILE_H__
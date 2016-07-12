#pragma once
#ifdef _VER_DX
#ifdef _VER_OPENGL
#error "_VER_DX와 _VER_OPENGL은 함께 쓸 수 없음."
#endif
#include <windows.h>
#include <d3dx9.h>
#include "dxerr.h"
//#include "XGraphicsD3D.h"
#include "etc/XSurface.h"
namespace XE
{
	extern D3DXMATRIX s_mViewProj;
	extern LPD3DXEFFECT s_pEffect;
	inline const D3DXMATRIX& GetMatrixViewProj( void ) { return s_mViewProj; }
	inline void SetMatrixViewProj( const D3DXMATRIX *pm ) { 
		s_mViewProj = *pm; 
	}
	inline LPD3DXEFFECT GetEffect( void ) { return s_pEffect; }
	inline void SetEffect( LPD3DXEFFECT pEffect ) { s_pEffect = pEffect; }
	BOOL LoadShader( LPCTSTR szFilename );
	float GetDT( void );
	// 초기화와 파괴
	BOOL Init( LPDIRECT3DDEVICE9 pd3dDevice, int nScrWidth, int nScrHeight );
	BOOL Init( LPDIRECT3DDEVICE9 pd3dDevice, int nScrWidth, int nScrHeight, LPD3DXEFFECT pEffect );
	void Destroy( void );
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

		return DXTrace( strFile, dwLine, hr, strMsg, bPopMsgBox );
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
	int m_sizeByte = 0;
	// 
	void Init( void ) {
		m_pTexture = NULL;
		m_pVB = NULL;
		m_pVBSub = NULL;

		m_TexPool = D3DPOOL_DEFAULT;
		m_VBPool = D3DPOOL_DEFAULT;
//		m_nAlignedWidth = m_nAlignedHeight = 0;
	}
	void Destroy( void );
protected:
//	DWORD *m_pImg;
public:
// 	XSurfaceD3D& operator=( const XSurfaceD3D& rhs ) {
// 		XSurface::ValueAssign( &rhs );
// 		// 메모리할당이 필요없는 값들만 대입하기위해 포인터값들은 모두 제외
// 		return *this;
// 	}
// 	virtual void ValueAssign( const XSurface *pSrc ) {
// 		XSurface::ValueAssign( pSrc );
// //		*this = *(dynamic_cast<XSurfaceD3D *>(pSrc));
// 		// 값만 대입하므로 포인터관련 변수들은 카피하지 않음
// 	}

	XSurfaceD3D( BOOL bHighReso ) : XSurface( bHighReso ) { Init(); }	// 이제 사용하지 않을예정
	XSurfaceD3D() : XSurface( TRUE ) { 
		Init(); 
	}
/*	XSurfaceD3D( BOOL bHighReso, LPCTSTR szFilename, BOOL bKeepSrc=FALSE ) 
		: XSurface( bHighReso, szFilename ) { 
			Init(); 
			LoadImg( szFilename, bKeepSrc ); 
	} */
	XSurfaceD3D( BOOL bHighReso, 
							float width, float height, 
							float adjustX, float adjustY, 
							DWORD *pImg, 
							BOOL bSrcKeep, BOOL bMakeMask );
// 	XSurfaceD3D( const XE::VEC2& sizeSurface
// 						, const XE::VEC2& vAdj
// 						, XE::xtPixelFormat formatSurface
// 						, DWORD *pSrcImg
// 						, const XE::VEC2& sizeSrc
// 						, BOOL bSrcKeep, BOOL bMakeMask );
	XSurfaceD3D( BOOL bHighReso, const int srcx, const int srcy, const int srcw, const int srch, const float dstw, const float dsth, const float adjx, const float adjy, void *_pSrcImg, BOOL bSrcKeep = FALSE );
	XSurfaceD3D( int wSurface, int hSurface, int wTexture, int hTexture, int adjx, int adjy, DWORD *pImg, BOOL bSrcKeep=FALSE );
	virtual ~XSurfaceD3D() { Destroy(); }

	xRESULT ChangeAdjust( float adjx, float adjy );		// SE에서 쓰는듯
	BOOL SetImage( const float width, const float height, const float adjx, const float adjy, DWORD *pNewImg );
//	xRESULT		Create( const float width, const float height, const float adjx, const float adjy, xAlpha alpha, DWORD *pImg );
// 	xRESULT Create( const float width, const float height
// 								, const float _adjx, const float _adjy
// 								, xAlpha alpha
// 								, const void *pImg
// 								, int bytePerPixel
// 								, XCOLOR dwKeyColor/*=0*/
// 								, BOOL bSrcKeep/* = FALSE*/
// 								, BOOL bMakeMask/*=FALSE*/ ) override;
#if 0
// 	xRESULT Create( const int srcx, const int srcy
// 								, const int srcw, const int srch
// 								, const float dstw, const float dsth
// 								, const float adjx, const float adjy
// 								, void *_pSrcImg
// 								, const int bpp
// 								, BOOL bSrcKeep/* = FALSE*/
// 								, BOOL bMakeMask/*=FALSE*/ ) override;		// 이미지의 일부분만 서피스로 만드는 버전
#endif // 0	
	bool Create( const XE::POINT& sizeSurfaceOrig
						, const XE::VEC2& vAdj
						, XE::xtPixelFormat formatSurface								
						, void* const pImgSrc
						, XE::xtPixelFormat formatImgSrc
						, const XE::POINT& sizeMemSrc
						, const XE::POINT& sizeMemSrcAligned ) override;
	// pure virtual
	bool CreateSub( const XE::POINT& posMemSrc
						, const XE::POINT& sizeArea
						, const XE::POINT& sizeAreaAligned
						, const XE::POINT& sizeMemSrc
						, void* const _pSrcImg
						, XE::xtPixelFormat formatImgSrc
						, const XE::VEC2& sizeRender
						, const XE::VEC2& vAdj
						, XE::xtPixelFormat formatSurface ) override;
	//
	void RestoreDevice( void ) override { } // d3d에서도 restore해야하지만 개발용이라 별로 중요하지 않아서 구현 안함.
//	xRESULT	LoadImg( LPCTSTR szFilename, BOOL bKeepSrc=FALSE );
//	BOOL CreatePNG( LPCTSTR szRes, BOOL bSrcKeep, BOOL bMakeMask ) override;
	void CopySurface( XSurface *src );
	void Draw( float x, float y ) override { 
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
	void DrawCoreSub( void );
	void DrawNoAdjust( float x, float y ) {
		x -= GetAdjustX() * m_fScaleX;
		y -= GetAdjustY() * m_fScaleY;
		Draw( x, y );
	}
	void Draw( const MATRIX& mLocal, const MATRIX &mParent );
	void DrawSub( float x, float y, const XE::xRECTi *src ) override;
	void Fill( DWORD col );
	void RenderTexture( float x, float y, float w, float h, LPDIRECT3DTEXTURE9 pd3dTexture );

public:
	static DWORD s_dwTextureCaps;
	static void Initialize( DWORD dwTextureCaps ) { s_dwTextureCaps = dwTextureCaps; }
private:
	int GetSizeByte() override {
		return m_sizeByte;
	}
	// D3DXCreateTexture()를 사용하는곳에선 다 호출해줘야 한다. 
	// 현재는 XSurfaceD3D에서만 사용하지만 만약 다른 클래스에서 createTexture를 쓴다면 그곳에서 책임지고 XSurface::s_sizeTotalVMem에 더해줘야 한다.
	void AddSizeByte( int bytes ) override {
		XSurface::s_sizeTotalVMem += bytes;
		m_sizeByte += bytes;
	}
	void ClearSizeByte() override {
		m_sizeByte = 0;
	}

};

//extern D3DXMATRIX g_mViewProj;
//extern LPD3DXEFFECT g_pEffect;



#endif
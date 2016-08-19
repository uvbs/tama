#pragma once
#include "path.h"
//#include "XGraphics.h"
#include "global.h"
#include <stdlib.h>
#include "XSurfaceDef.h"
#include "XGraphicsDef.h"
#include "xColor.h"
class XSurface
{
public:
	static DWORD GetMaxSurfaceWidth() { XBREAK(s_dwMaxSurfaceWidth==0); return s_dwMaxSurfaceWidth; }
	static void SetMaxSurfaceWidth( DWORD dwMaxWidth ) { s_dwMaxSurfaceWidth = dwMaxWidth; }
	static int s_sizeTotalVMem;
private:
	static DWORD s_dwMaxSurfaceWidth;		// D3DCAPS.MaxTextureWidth같은거임
	void Init() {
		m_bHighReso = FALSE;
		__fResoScale = 1.0f;
		m_Width = m_Height = 0;
		m_bColorKey = FALSE;
		m_dwKeyColor = 0;
		m_fAlpha = 1.0f;
		m_ColorR = m_ColorG = m_ColorB = 1.0f;
		m_AdjustX = m_AdjustY = 0;
		m_fScaleX = m_fScaleY = m_fScaleZ = 1.0f;
		m_fRotX = m_fRotY = m_fRotZ = 0;
		m_fAdjustAxisX = m_fAdjustAxisY = 0;
		__pSrcImg = nullptr;
		m_DrawMode = xDM_NORMAL;
		m_dwDrawFlag = 0;
	}
	void Destroy() { 
		SAFE_DELETE_ARRAY( __pSrcImg );
	}
	BOOL m_bHighReso;							// 아이폰용 고해상도 리소스
	float	__fResoScale;		// 바른계산을 위해 고해상도용 스케읽밧을 받아둔다
	float m_Width, m_Height;					// 서피스 크기(주의:메모리 크기가 아님!). 고해상 메모리데이타의 크기가 11인경우 서피스크기는 5.5가 되므로 이제부턴 w,h값을 실수형으로 써야 한다
	float	m_AdjustX, m_AdjustY;				// 이거 하위클래스에서 
	DWORD *__pSrcImg;		// 반드시 SetSrcImg()로 세팅하게 바꼈으므로 private이 됨

protected:
	// 아이폰용 고해상도 리소스로 지정되면 외부에선 이 리소스의 크기는 원래크기의 1/2로 인식한다.
	// 다만 내부메모리는 원래크기의 양이 할당되어 있다.
	// 예) 64x64 png를 고해상도 지정했을때
	// GetWidth() == 32
	// GetMemWidth() == 64
	int		m_nAlignedWidth, m_nAlignedHeight;		// ^2로 정렬된 실제메모리 크기
	BOOL		m_bColorKey;				// 컬러키 사용여부
	XCOLOR	m_dwKeyColor;		// 컬러키 값.
	float	m_fAlpha;
	float	m_ColorR;
	float	m_ColorG;
	float	m_ColorB;
	float	m_fScaleX, m_fScaleY, m_fScaleZ;
	float	m_fRotX, m_fRotY, m_fRotZ;
	float	m_fAdjustAxisX, m_fAdjustAxisY;	// 회전축보정
	DWORD m_dwDrawFlag;		// EFF_****
	xDM_TYPE m_DrawMode;
	TCHAR m_szFilename[MAX_PATH];

	// 만약 외부에서 사용하는 SetSrcImg를 만들거라면 고해상도플래그도 받고, 버텍스버퍼/텍스쳐서피스까지 교체하는 함수를 만들어야 한다. 근데 가급적이면 그런식으로 사용하지 말고 필요하다면 걍 XSurface를 새로 생성하는게 나을듯
	void SetSrcImg( DWORD *pSrcImg, int memW, int memH ) { __pSrcImg = pSrcImg; m_Width = memW * __fResoScale; m_Height = memH * __fResoScale; }	// 
	void CreateSrcImg( int memW, int memH ) { 
		__pSrcImg = new DWORD[ memW * memH ];
		m_Width = memW * __fResoScale; 
		m_Height = memH * __fResoScale;
#ifdef _XDEBUG
		ClearSrcImg();		// 디버깅모드일땐 흰색으로 채워넣는다.
#endif
	}
	void ClearSrcImg() {		
		int w, h;
		DWORD *p = GetSrcImg( &w, &h );
		memset( p, 0xff, w * h * sizeof(DWORD) );		// 문제가 있으면 바로 보이도록 빈칸을 흰색으로 채웠다.
	}
	// SetWidth, SetSize같은걸 따로 만들지 말것. 반드시 이미지와 함께 세팅하도록
	int GetAlignedWidth() { return m_nAlignedWidth; }		// 이제 이 함수를 외부에서 쓰는일은 없어야 한다. 외부에선 이게 어떻게 정렬됐는지 알필요 없게 하자
	int GetAlignedHeight() { return m_nAlignedHeight; }
	int ConvertToMemSize( float num ) { return (int)(num / __fResoScale); }
	float ConvertToSurfaceSize( int num ) { return num * __fResoScale; }
	void SetAdjust( float adjx, float adjy ) { m_AdjustX = adjx; m_AdjustY = adjy; }	// 외부에서 쓰려면 ChangeAdjust를 쓸것
	void SetHighReso( BOOL bHighReso ) {
		m_bHighReso = bHighReso;
		if( bHighReso )
			__fResoScale = 0.5f;
		else
			__fResoScale = 1.0f;
	}
public:
//	XSurface& operator=( const XSurface& rhs ) {
//		return *this;
//	}
	virtual void ValueAssign( const XSurface *pSrc ) {
		DWORD *pSrcImg = __pSrcImg;		// 백업
		*this = *pSrc;
		__pSrcImg = pSrcImg;				// 복구
	}

	// 고해상도리소스로 지정되면 화면에 찍힐땐 실제크기의 반으로 찍힌다
	XSurface( BOOL bHighReso ) { Init(); m_bHighReso = bHighReso; if( bHighReso ) __fResoScale = 0.5f; }
	XSurface( BOOL bHighReso, LPCTSTR szFilename ) { Init(); m_bHighReso = bHighReso; if( bHighReso ) __fResoScale = 0.5f; _tcscpy_s( m_szFilename, XE::GetFileName( szFilename ) ); }
	XSurface( BOOL bHighReso, float w, float h, float adjx, float adjy ) { 
		Init();
		m_bHighReso = bHighReso;
		if( bHighReso ) __fResoScale = 0.5f;
		m_Width = w;		m_Height = h;
		m_AdjustX = adjx; m_AdjustY = adjy;
	}
	virtual ~XSurface() { Destroy(); }

	GET_ACCESSOR( BOOL, bHighReso );
	int GetMemWidth() { 
		return (int)(m_Width / __fResoScale); 
	}		// 실제 메모리 크기
	int GetMemHeight() { 
		return (int)(m_Height / __fResoScale); 
	}	// 실제 메모리 크기
	int GetTextureWidth() { return GetMemWidth(); }
	int GetTextureHeight() { return GetMemHeight(); }
	XE::VEC2 GetMemSize() { return XE::VEC2( GetMemWidth(), GetMemHeight() ); }
	XE::VEC2 GetTextureSize() { 
		return XE::VEC2( GetTextureWidth(), GetTextureHeight() ); 
	}
	DWORD* GetSrcImg( XE::VEC2 *pvOutSize ) { *pvOutSize = GetMemSize(); return __pSrcImg; }	// 이거쓰지 말고 GetPixel()로 할것. 일단 m_Width로 만들었다고 가정하고 밖으로 뺌.밖으로 노출시키지 말것 m_pSrcImg크기를 m_Width로 만들었는지 AlignedWidth로 만들었는지 애매해지기땜에...필요하다면 XSurface내부에 만들것.
	DWORD* GetSrcImg( int *pOutMemW, int *pOutMemH ) { *pOutMemW = (int)GetMemWidth(); *pOutMemH = (int)GetMemHeight(); return __pSrcImg; }
	void DestroySrcImg() { SAFE_DELETE_ARRAY( __pSrcImg ); }
	
	// 서피스의 크기(텍스쳐의 크기가 아님)
	float GetWidth() { 
		return m_Width; 
	}
	// 서피스의 크기(텍스쳐의 크기가 아님)
	float GetHeight() { 
		return m_Height; 
	}
	float GetAdjustX() { 
		return m_AdjustX; 
	}
	float GetAdjustY() { 
		return m_AdjustY; 
	}
	XE::VEC2 GetSize() { 
		return XE::VEC2( m_Width, m_Height ); 
	}
	float GetTransformedWidth() { return GetWidth() * m_fScaleX; }		// 스케일링 된 크기
	float GetTransformedHeight() { return GetHeight() * m_fScaleY; }		// 스케일링 된 크기
	int GetMemAdjustX() { return (int)(m_AdjustX / __fResoScale); }	// 실제 메모리상의 adjust값
	int GetMemAdjustY() { return (int)(m_AdjustY / __fResoScale); }	// 실제 메모리상의 adjust값
	XE::VEC2 GetMemAdjust() { 
		return XE::VEC2( GetMemAdjustX(), GetMemAdjustY() );
	}	// 실제 메모리상의 adjust값
	XE::VEC2 GetAdjust() { return XE::VEC2( m_AdjustX, m_AdjustY ); }
	float GetTransformedAdjustX() { return m_AdjustX * m_fScaleX; }		// 스케일링 된 크기
	float GetTransformedAdjustY() { return m_AdjustY * m_fScaleY; }		// 스케일링 된 크기

	void SetAdjustAxis( float adjx, float adjy ) { m_fAdjustAxisX = adjx; m_fAdjustAxisY = adjy; }
	void SetAdjustAxis( const XE::VEC2& vAdjAxis ) { m_fAdjustAxisX = vAdjAxis.x; m_fAdjustAxisY = vAdjAxis.y; }
	GET_ACCESSOR( float, fAdjustAxisX );
	GET_ACCESSOR( float, fAdjustAxisY );
	void SetColorKey( BOOL useFlag ) { m_bColorKey = useFlag; }
	void SetKeyColor( DWORD dwColor ) { m_dwKeyColor = dwColor; }
	SET_ACCESSOR( float, fAlpha );
//	void SetAlpha( float fAlpha ) { m_fAlpha = fAlpha; }
	LPCTSTR GetFilename() { return m_szFilename; }
	void SetszFilename( LPCTSTR szFilename ) { _tcscpy_s( m_szFilename, szFilename ); }
	void SetScale( float sx, float sy, float sz = 1.0f ) { m_fScaleX = sx; m_fScaleY = sy; m_fScaleZ = sz; }	
	void SetScale( float sxy ) { m_fScaleX = m_fScaleY = sxy; m_fScaleZ = 1.0f; }	
	void SetScale( const XE::VEC2& vScale ) { m_fScaleX = vScale.x; m_fScaleY = vScale.y; m_fScaleZ = 1.0f; }
	void SetRotateX( float angle ) { m_fRotX = angle; }
	void SetRotateY( float angle ) { m_fRotY = angle; }
	void SetRotateZ( float angle ) { m_fRotZ = angle; }
	void SetRotate( float ax,float ay, float az ) { SetRotateX(ax); SetRotateY(ay); SetRotateZ(az); }
	GET_ACCESSOR( float, fScaleX );
	GET_ACCESSOR( float, fScaleY );
	GET_SET_ACCESSOR( xDM_TYPE, DrawMode );
	void SetColor( float r, float g, float b ) {
		m_ColorR = r;
		m_ColorG = g;
		m_ColorB = b;
	}
	void SetFlipHoriz( BOOL bFlag ) { 
		if( bFlag ) {
			m_dwDrawFlag |= EFF_FLIP_HORIZ;
			SetRotateY(180.0f);
		}
		else {
			m_dwDrawFlag &= ~EFF_FLIP_HORIZ;  
			SetRotateY(0);
		}
	}
		
	void SetFlipVert( BOOL bFlag ) { 
		if( bFlag ) {
			m_dwDrawFlag |= EFF_FLIP_VERT;
			SetRotateX(180.0f);
		}
		else {
			m_dwDrawFlag &= ~EFF_FLIP_VERT;  
			SetRotateX(0);
		}
	}
	DWORD GetPixel( const XE::VEC2& vPos, BYTE *pa = nullptr, BYTE *pr = nullptr, BYTE *pg = nullptr, BYTE *pb = nullptr  ) {	return GetPixel( vPos.x, vPos.y, pa, pr, pg, pb ); 	}
	DWORD GetPixel( float lx, float ly, BYTE *pa = nullptr, BYTE *pr = nullptr, BYTE *pg = nullptr, BYTE *pb = nullptr  );

	virtual XE::xRESULT Create( const float width, const float height ) { return Create(width, height, 0, 0, xNO_ALPHA, nullptr, 0, 0 ); } 
	virtual XE::xRESULT Create( const float width, const float height, const float adjx, const float adjy, xAlpha alpha, const void *pImg, int bytePerPixel, XCOLOR dwKeyColor=0, BOOL bSrcKeep = FALSE ) = 0;
	virtual XE::xRESULT Create( const int srcx, const int srcy, const int srcw, const int srch, const float dstw, const float dsth, const float adjx, const float adjy, void *_pSrcImg, const int bpp, BOOL bSrcKeep = FALSE ) = 0;		// 이미지의 일부분만 서피스로 만드는 버전
//	xRESULT Create( const int srcx, const int srcy, const int srcw, const int srch, const int adjx, const int adjy, void *pSrcImg, const int bpp, BOOL bSrcKeep ) { return Create( srcx, srcy, srcw, srch, srcw, srch, adjx, adjy, pSrcImg, bpp, bSrcKeep );	}	// srcwh와 dstwh가 같은 버전

	virtual XE::xRESULT	LoadTexture( LPCSTR szFilename, XCOLOR dwColorKey ) { return XE::xSUCCESS; }
	virtual BOOL			Restore() { return TRUE; }
	virtual void*	Lock( int *pWidth, BOOL bReadOnly = TRUE ) { return nullptr; }
	virtual void		Unlock() {}
    virtual void SetTexture() {}

	virtual void CopySurface( XSurface *src ) {};
	virtual void CopyRect( int dstx, int dsty, DWORD *pSrc, int srcw, int srch ) {}
	virtual void CopyRectTo( DWORD *pDst, int wDst, int hDst, int xSrc, int ySrc );

	virtual void Draw( float x, float y ) { XBREAKF(1, "구현되지않음"); }
	virtual void DrawLocal( float x, float y, float lx, float ly ) { XBREAKF(1, "구현되지않음"); }	// x,y:최종스크린좌표, lx, ly 로컬내에서 좌표. XWndDialog::Draw의 타일화 draw참고
	void Draw( const XE::VEC2& vPos ) { Draw( vPos.x, vPos.y ); }
	virtual void DrawSub( float x, float y, const RECT *src ) { XBREAKF(1, "구현되지않음"); }
	virtual void Fill( XCOLOR col ) { XBREAKF(1, "구현되지않음"); }
	// lua
	void LuaDraw( float x, float y ) { Draw( x, y ); }
};

XE_NAMESPACE_START( XE )
//
enum xtPixelFormat {
	xPF_NONE,
	xPF_ARGB8888,
	xPF_ARGB4444,
	xPF_ARGB1555,
	xPF_RGB565,
	xPF_RGB555,
	xPF_MAX,
};
LPCTSTR GetstrEnum( XE::xtPixelFormat pixelFormat );
//
XE_NAMESPACE_END; // XE


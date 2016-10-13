#pragma once

//#define _XDEBUG_SPR

#ifdef _XDEBUG_SPR
#define XSPR_TRACE(F,...)	XTRACE(F,##__VA_ARGS__)
#else
	#define XSPR_TRACE(...)	(0)
#endif

class XSprDat;
class XBaseRes;
class XSurface;
namespace XE {
struct xSurfaceInfo;
struct xHSL;
}
enum xDM_TYPE : int;
	//////////////////////////////////////////////////////////////////////////
// #include "etc/XGraphics.h"
// #include "etc/XSurface.h"
class XSprite
{
public:
	static int s_sizeTotalMem;		// 스프라이트들의 총 메모리할당량(이미지데이타만..)
private:
	XSurface *m_pSurface;
	int m_idxSprite;	// 이 스프라이트의 인덱스
	void Init() {
		m_idxSprite = 0;
		m_pSurface = NULL;
	}
	void Destroy();
public:
	XSprite( int idx );
	~XSprite() { Destroy(); }	// 부모의 파괴자를 호출하는지 확인할것
	//
	GET_ACCESSOR_CONST( int, idxSprite );
	GET_ACCESSOR( XSurface*, pSurface );
	//
	void Save( XBaseRes *pRes ) {}
	void Load( XSprDat *pSprDat,
						 XBaseRes *pRes, 
						 bool bUseAtlas, 
						 bool bAsyncLoad,
						 const XE::xHSL& hsl,
						 BOOL bSrcKeep, 
						 bool bBatch,
						 BOOL bRestore );
	void CreateDevice( const XE::xSurfaceInfo& infoSurface );
	void CreateDevice();
	void UpdateUV( ID idTex, const XE::POINT& sizePrev, const XE::POINT& sizeNew );
	void SetScale( float sx, float sy );	// 여기는 GRAPHICS->GetfGScale과 곱하지 않는다 XLayerImage::Draw에서 이걸 계속 호출하기 때문이다
	void SetScale( const XE::VEC2& vScale );
	void SetScale( float scalexy );
	MATRIX* GetMatrix( MATRIX *pOut, float lx, float ly );
	float GetWidth() const;
	float GetHeight() const;
	XE::VEC2 GetSize() const;
	XE::VEC2 GetMemSize() const;
	XE::VEC2 GetAdjust() const;
	float GetAdjustX() const;
	float GetAdjustY() const;
	XE::VEC2 GetsizeMemAligned() const;
	// 스프라이트 이미지데이타(메모리/VRAM)의 바이트크기
	int GetbytesMemAligned() const;
	void SetfAlpha( float alpha );
	void SetColor( XCOLOR col );
	void SetColor( float r, float g, float b );
	void Draw( float x, float y );
	void Draw( float x, float y, const MATRIX &mParent );
	void Draw( const XE::VEC2& vPos, const MATRIX &mParent );
	void Draw( const XE::VEC2& v );
	void SetAdjustAxis( float ax, float ay ); // m_pSurface->SetAdjustAxis( ax, ay ); }
	void SetAdjustAxis( const XE::VEC2& vAdjAxis ); // m_pSurface->SetAdjustAxis( vAdjAxis ); }
	void SetRotate( float dX, float dY, float dZ ); // m_pSurface->SetRotate( dX, dY, dZ ); }
	void SetRotateZ( float dAng ); // m_pSurface->SetRotateZ( dAng ); }
	void SetFlipHoriz( BOOL bFlag ); // m_pSurface->SetFlipHoriz( bFlag ); }
	void SetFlipVert( BOOL bFlag ); // m_pSurface->SetFlipVert( bFlag ); }
	void SetDrawMode( xDM_TYPE t ); // m_pSurface->SetDrawMode( t ); }
	DWORD GetPixel( float lx, float ly, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL  );
#ifdef WIN32
	void ConvertMemBGR2RGB( DWORD *pImg, int sizeBytes );
#endif // WIN32
//	void ApplyHSL( DWORD *pImg, const int sizeBytes, const float ah, const float as, const float al );
	void ApplyHSLRanged( DWORD *pImg, const int sizeBytes, const float ah, const float as, const float al
		, const XE::VEC2& range1 = XE::VEC2()
		, const XE::VEC2& range2 = XE::VEC2() );
	void ApplyHSLNormal( DWORD *pImg, const int sizeBytes, const float ah, const float as, const float al );
	bool IsBatch() const;
	void DestroyDevice();
};

#pragma once

#include "path.h"
//#include "etc/xGraphics.h"
#include "etc/global.h"
#include <stdlib.h>
#include "XSurfaceDef.h"
#include "XGraphicsDef.h"
#include "xColor.h"
#include "xUtil.h"
#include "XRefRes.h"
#include "xMath.h"
#include "XGraphicsUtil.h"
#if !defined(MATRIX) && !defined(CONSOLE)
#error "not define MATRIX"
#endif

XE_NAMESPACE_START( XE )
//
struct xSurfaceInfo {
	XE::POINT m_ptSizeSurface;
	XE::POINT m_ptSizeMem;
	XE::VEC2 m_vAdj;
	XE::xtPixelFormat m_fmtSurface = xPF_NONE;
	XE::xtPixelFormat m_fmtSrc = xPF_NONE;
	void* m_pImg = nullptr;
	bool m_bSrcKeep = false;
	bool m_bMakeMask = false;
	bool m_bUseAtlas = false;
	xSurfaceInfo( const XE::POINT& ptSizeSurface,
								const XE::VEC2& vAdj,
								XE::xtPixelFormat fmtSurface,
								void* pImg,
								const XE::POINT& ptSizeMem,
								XE::xtPixelFormat fmtSrc,
								bool bSrcKeep,
								bool bMakeMask,
								bool bUseAtlas )
		: m_ptSizeSurface( ptSizeSurface )
		, m_ptSizeMem( ptSizeMem )
		, m_vAdj( vAdj )
		, m_fmtSurface( fmtSurface )
		, m_pImg( pImg )
		, m_fmtSrc( fmtSrc )
		, m_bSrcKeep( bSrcKeep )
		, m_bMakeMask( bMakeMask )
		, m_bUseAtlas( bUseAtlas ) {	}
	~xSurfaceInfo() {	}
};

//
XE_NAMESPACE_END; // XE


class XSurface : public XRefRes
{
public:
	static DWORD GetMaxSurfaceWidth( void ) { XBREAK(s_dwMaxSurfaceWidth==0); return s_dwMaxSurfaceWidth; }
	static void SetMaxSurfaceWidth( DWORD dwMaxWidth ) { s_dwMaxSurfaceWidth = dwMaxWidth; }
	static int sGetsizeTotalVMem() {
		return s_sizeTotalVMem;
	}
	static void sAddSizeTotalVMem( int bytes ) {
		s_sizeTotalVMem += bytes;
	}
private:
	static int s_sizeTotalVMem;
	static DWORD s_dwMaxSurfaceWidth;		// D3DCAPS.MaxTextureWidth같은거임
	void Init( void ) {
		m_bHighReso = FALSE;
		__fResoScale = 1.0f;
		m_Width = m_Height = 0;
		m_bColorKey = FALSE;
		m_dwKeyColor = 0;
		m_fAlpha = 1.0f;
		m_ColorR = m_ColorG = m_ColorB = 1.0f;
		m_AdjustX = m_AdjustY = m_adjZ = 0;
		m_fScaleX = m_fScaleY = m_fScaleZ = 1.0f;
		m_fRotX = m_fRotY = m_fRotZ = 0;
		m_fAdjustAxisX = m_fAdjustAxisY = 0;
		__pSrcImg = NULL;
		m_DrawMode = xDM_NORMAL;
		m_dwDrawFlag = 0;
		m_pMask = NULL;
	}
	void Destroy( void );
	BOOL m_bHighReso;				// 아이폰용 고해상도 리소스
	float	__fResoScale;		// 바른계산을 위해 고해상도용 스케읽밧을 받아둔다
	float m_Width, m_Height;		///< 서피스 크기(주의:메모리 크기가 아님!). 고해상 메모리데이타의 크기가 11인경우 서피스크기는 5.5가 되므로 이제부턴 w,h값을 실수형으로 써야 한다
	float	m_AdjustX, m_AdjustY, m_adjZ;	// 이거 하위클래스에서 
	XE::VEC2 m_sizeMem;					// 메모리데이타 원본의 크기
	DWORD *__pSrcImg;		// 반드시 SetSrcImg()로 세팅하게 바꼈으므로 private이 됨
	BYTE *m_pMask;			// 픽킹용 마스크 이미지. 알파값만 들어있다.
	XE::xtPixelFormat m_formatSurface = XE::xPF_NONE;	// vRam에 만들어진 서피스의 픽셀포맷.
	int m_sizeByte = 0;			// vram에 로딩된 텍스쳐의 크기
	bool m_bAtlas = false;		// atlas에 텍스쳐가 있는지.
	std::shared_ptr<XE::xSurfaceInfo> m_spSurfaceInfo;		// 비동기로 로딩했을경우 파일에서 읽어온 데이터.
	inline void SetsizeSurface( float w, float h ) {
		m_Width = w;
		m_Height = h;
	}
	inline void SetsizeSurface( const XE::POINT& sizeSurface ) {
		SetsizeSurface( (float)sizeSurface.w, (float)sizeSurface.h );
	}
	inline void SetsizeSurface( const XE::VEC2& sizeSurface ) {
		SetsizeSurface( sizeSurface.w, sizeSurface.h );
	}
	// XGraphicsGL::CreateTextureGL()을 부르는곳에선 다 호출해줄것.
	void AddSizeByte( int bytes ) {
		m_sizeByte += bytes;
	}
	void ClearSizeByte() {
		m_sizeByte = 0;
	}
public:
	// 서피스 복사를 위해 편법으로 만든거. 객체 복사를 한 후 원본객체가 파괴될때 문제가 되는것들을 클리어 시킨다. 비동기로딩때문에 생긴건데 애초에 surface를 생성할때 내부에서 생성해서 리턴하지 말고 외부에서 생성후 넣어주면 해결된다.
	virtual void ClearCreated() {
		__pSrcImg = nullptr;
		m_pMask = nullptr;
	}
	int GetSizeByte() const {
		return m_sizeByte;
	}
	// vram에 로딩한 텍스쳐의 크기를 집계해서 돌려줘야 함.
private:
	// 아이폰용 고해상도 리소스로 지정되면 외부에선 이 리소스의 크기는 원래크기의 1/2로 인식한다.
	// 다만 내부메모리는 원래크기의 양이 할당되어 있다.
	// 예) 64x64 png를 고해상도 지정했을때
	// GetWidth() == 32
	// GetMemWidth() == 64
	XE::POINT m_sizeMemAligned;		// 메모리 크기를 2^로 정렬한 크기
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
	XE::xtBlendFunc m__funcBlend = XE::xBF_MULTIPLY;	// 장차 m_DrawMode는 blendfunc으로 바꾼다.

	// 만약 외부에서 사용하는 SetSrcImg를 만들거라면 고해상도플래그도 받고, 버텍스버퍼/텍스쳐서피스까지 교체하는 함수를 만들어야 한다. 근데 가급적이면 그런식으로 사용하지 말고 필요하다면 걍 XSurface를 새로 생성하는게 나을듯
	// 장차 private으로 들어가야함.
private:
	void SetSrcImg( DWORD *pSrcImg, int memW, int memH ) {
		__pSrcImg = pSrcImg;
	}	//
	inline void SetSrcImg( DWORD *pSrcImg, const XE::POINT& sizeMem ) {
		SetSrcImg( pSrcImg, sizeMem.w, sizeMem.h );
	}
//protected:
	inline bool IsHaveSrcImg( void ) const {
		return (__pSrcImg != nullptr);
	}
//protected:
//	float ConvertToSurfaceSize( int num ) { return num * __fResoScale; }
	// 외부에서 쓰려면 ChangeAdjust를 쓸것
private:
	inline void SetAdjust( float adjx, float adjy ) {
		m_AdjustX = adjx;
		m_AdjustY = adjy;
	}
	inline void SetAdjust( const XE::VEC2& vAdj ) {
		SetAdjust( vAdj.x, vAdj.y );
	}
//protected:
	void SetHighReso( BOOL bHighReso ) {
		m_bHighReso = bHighReso;
		if( bHighReso )
			__fResoScale = 0.5f;
		else
			__fResoScale = 1.0f;
	}
private:
	SET_ACCESSOR( BYTE*, pMask );
	SET_ACCESSOR( const XE::POINT&, sizeMemAligned );		// 기존코드와 호환을 위해 protected로 했지만 장기적으로 private로 들어가야한다.
	// 구코드 호환을 위해 남겨둠.
	inline void _SetAdjust( float adjx, float adjy ) {
		SetAdjust( adjx, adjy );
	}
	inline void _SetAdjust( const XE::VEC2& vAdj ) {
		SetAdjust( vAdj.x, vAdj.y );
	}
private:
	SET_ACCESSOR( const XE::VEC2&, sizeMem );
protected:
public:
	XE::xtBlendFunc GetfuncBlend() const {
		return m__funcBlend;
	}
	GET_SET_BOOL_ACCESSOR( bAtlas );
	GET_SET_ACCESSOR_CONST( float, adjZ );
	GET_ACCESSOR_CONST( const XE::VEC2&, sizeMem );
	GET_ACCESSOR_CONST( const XE::POINT&, sizeMemAligned );
	GET_ACCESSOR_CONST( XE::xtPixelFormat, formatSurface );
	GET_SET_ACCESSOR_CONST( std::shared_ptr<XE::xSurfaceInfo>, spSurfaceInfo );
	XE::POINT GetsizeMemToPoint() const {
		return XE::POINT( (int)m_sizeMem.w, (int)m_sizeMem.h );
	}
	XE::VEC2 GetsizeMemAlignedVec2() {
		return XE::VEC2( m_sizeMemAligned.w, m_sizeMemAligned.h );
	}
	// 이미지데이타(메모리/VRAM)의 바이트크기
	int GetbytesMemAligned() {
		XBREAK( !m_formatSurface );
		return m_sizeMemAligned.Size() * XE::GetBpp( m_formatSurface );
	}
	// 고해상도리소스로 지정되면 화면에 찍힐땐 실제크기의 반으로 찍힌다
	XSurface( BOOL bHighReso ) {
		Init();
		m_bHighReso = bHighReso;
		if( bHighReso )
			__fResoScale = 0.5f;
	}
	XSurface( BOOL bHighReso, float w, float h, float adjx, float adjy ) { 
		Init();
		m_bHighReso = bHighReso;
		if( bHighReso ) __fResoScale = 0.5f;
		m_Width = w;		m_Height = h;
		m_AdjustX = adjx; m_AdjustY = adjy;
	}
	XSurface( const XE::VEC2& size, const XE::VEC2& vAdj ) 
		: XSurface( TRUE, size.w, size.h, vAdj.x, vAdj.y ) {}
	//
	virtual ~XSurface() { Destroy(); }

	GET_ACCESSOR( BOOL, bHighReso );
	GET_ACCESSOR( BYTE*, pMask );
	int GetMemWidth( void ) { return (int)(m_Width / __fResoScale); }		// 실제 메모리 크기
	int GetMemHeight( void ) { return (int)(m_Height / __fResoScale); }	// 실제 메모리 크기
	int GetTextureWidth( void ) { return GetMemWidth(); }
	int GetTextureHeight( void ) { return GetMemHeight(); }
	XE::VEC2 GetMemSize( void ) {
		return XE::VEC2( GetMemWidth(), GetMemHeight() );
	}
	XE::POINT GetMemSizei() {
		return XE::POINT( GetMemWidth(), GetMemHeight() );
	}
	XE::VEC2 GetTextureSize( void ) {
		return XE::VEC2( GetTextureWidth(), GetTextureHeight() );
	}
	DWORD* GetSrcImg( XE::VEC2 *pvOutSize ) {
		*pvOutSize = GetMemSize();
		return __pSrcImg;
	}	
	DWORD* GetSrcImg( XE::POINT *pvOutSize ) {
		*pvOutSize = GetMemSizei();
		return __pSrcImg;
	}	// 이거쓰지 말고 GetPixel()로 할것. 일단 m_Width로 만들었다고 가정하고 밖으로 뺌.밖으로 노출시키지 말것 m_pSrcImg크기를 m_Width로 만들었는지 AlignedWidth로 만들었는지 애매해지기땜에...필요하다면 XSurface내부에 만들것.
	DWORD* GetSrcImg( int *pOutMemW, int *pOutMemH ) {
		*pOutMemW = (int)GetMemWidth();
		*pOutMemH = (int)GetMemHeight();
		return __pSrcImg;
	}
	void DestroySrcImg( void ) {
		SAFE_DELETE_ARRAY( __pSrcImg );
	}
	
	// 서피스의 크기(텍스쳐의 크기가 아님)
	float GetWidth( void ) const { return m_Width; }
	// 서피스의 크기(텍스쳐의 크기가 아님)
	float GetHeight( void ) const { return m_Height; }
	inline float GetAdjustX( void ) const { return m_AdjustX; }
	inline float GetAdjustY( void ) const { return m_AdjustY; }
	XE::VEC2 GetSize( void ) const { return XE::VEC2( m_Width, m_Height ); }
	XE::VEC2 GetsizeSurface() const { return XE::VEC2( m_Width, m_Height ); }
	float GetTransformedWidth( void ) const { return GetWidth() * m_fScaleX; }		// 스케일링 된 크기
	float GetTransformedHeight( void ) const { return GetHeight() * m_fScaleY; }		// 스케일링 된 크기
	int GetMemAdjustX( void ) const { return (int)(m_AdjustX / __fResoScale); }	// 실제 메모리상의 adjust값
	int GetMemAdjustY( void ) const { return (int)(m_AdjustY / __fResoScale); }	// 실제 메모리상의 adjust값
	XE::VEC2 GetAdjust( void ) const { return XE::VEC2( m_AdjustX, m_AdjustY ); }
	float GetTransformedAdjustX( void ) const { return m_AdjustX * m_fScaleX; }		// 스케일링 된 크기
	float GetTransformedAdjustY( void ) const { return m_AdjustY * m_fScaleY; }		// 스케일링 된 크기
	XE::VEC3 GetScale() const {
		return XE::VEC3( m_fScaleX, m_fScaleY, m_fScaleZ );
	}
	XE::VEC3 GetScalev2() const {
		return XE::VEC3( m_fScaleX, m_fScaleY );
	}

	/**
	 @brief 좌표축을 보정한다.
	 XE엔진은 편의성을 위해서 좌상귀 좌표를 draw()좌표로 사용하고 있는데
	 이때 이미지는 중앙이 중심축이 되어야 한다면 이미지 크기의 절반의 양수값을 AdjustAxis로 지정해야 한다.
	 만일 draw()좌표를 이미지의 좌상귀가 아닌 좌표축으로 사용하려 한다면 
	 pSurface->draw( vLT + pSurface->GetAdjustAxis() ) 로 draw좌표를 보정해야 한다.
	*/
	void SetAdjustAxis( float adjx, float adjy ) { m_fAdjustAxisX = adjx; m_fAdjustAxisY = adjy; }
	void SetAdjustAxis( const XE::VEC2& vAdjAxis ) { m_fAdjustAxisX = vAdjAxis.x; m_fAdjustAxisY = vAdjAxis.y; }
	void SetAdjustAxisCenter( void ) {
		SetAdjustAxis( GetSize() / 2.f );
	}
	GET_ACCESSOR( float, fAdjustAxisX );
	GET_ACCESSOR( float, fAdjustAxisY );
	XE::VEC2 GetAdjustAxis() {
		return XE::VEC2( m_fAdjustAxisX, m_fAdjustAxisY );
	}
	void SetColorKey( BOOL useFlag ) { m_bColorKey = useFlag; }
	void SetKeyColor( DWORD dwColor ) { m_dwKeyColor = dwColor; }
	GET_SET_ACCESSOR( float, fAlpha );
#ifdef _CLIENT
	BOOL IsInViewport( float x, float y, const MATRIX &mParent );
	BOOL IsInViewport( float x, float y, float w, float h );
#endif
//	void SetAlpha( float fAlpha ) { m_fAlpha = fAlpha; }
	// GetstrRes()로 바뀜.
//	LPCTSTR GetFilename( void ) {
//		return GetszName();
//	}
	void SetScale( float sx, float sy, float sz = 1.0f ) { m_fScaleX = sx; m_fScaleY = sy; m_fScaleZ = sz; }
	void SetScale( float sxy ) { m_fScaleX = m_fScaleY = sxy; m_fScaleZ = 1.0f; }	
	void SetScale( const XE::VEC2& vScale ) { m_fScaleX = vScale.x; m_fScaleY = vScale.y; m_fScaleZ = 1.0f; }
	void SetRotateX( float dAng ) { m_fRotX = dAng; }
	void SetRotateY( float dAng ) { m_fRotY = dAng; }
	void SetRotateZ( float dAng ) { m_fRotZ = dAng; }
	void SetRotate( float dX, float dY, float dZ ) { SetRotateX(dX); SetRotateY(dY); SetRotateZ(dZ); }
	GET_ACCESSOR( float, fRotX );
	GET_ACCESSOR( float, fRotY );
	GET_ACCESSOR( float, fRotZ );
	GET_ACCESSOR_CONST( float, fScaleX );
	GET_ACCESSOR_CONST( float, fScaleY );
	GET_ACCESSOR_CONST( xDM_TYPE, DrawMode );
	void SetDrawMode( xDM_TYPE drawMode );
	void SetBlendFunc( XE::xtBlendFunc blendFunc ) {
		m__funcBlend = blendFunc;
		m_DrawMode = XE::ConvertBlendFuncDMTypeDmType( blendFunc );
	}
	void SetColor( float r, float g, float b ) {
		m_ColorR = r;
		m_ColorG = g;
		m_ColorB = b;
	}
	GET_ACCESSOR_CONST( float, ColorR );
	GET_ACCESSOR_CONST( float, ColorG );
	GET_ACCESSOR_CONST( float, ColorB );
	// 주의: alpha값은 넣지 않음.
	void SetColor( DWORD rgb ) {
		m_ColorR = XCOLOR_RGB_R(rgb) / 255.f;
		m_ColorG = XCOLOR_RGB_G(rgb) / 255.f;
		m_ColorB = XCOLOR_RGB_B(rgb) / 255.f;
	}
	XE::VEC4 Getv4Color() const {
		return XE::VEC4( m_ColorR, m_ColorG, m_ColorB, m_fAlpha );
	}
	inline void SetFlipHoriz( BOOL bFlag ) { 
		if( bFlag ) {
			m_dwDrawFlag |= EFF_FLIP_HORIZ;
			SetRotateY(180.0f);
		}
		else {
			m_dwDrawFlag &= ~EFF_FLIP_HORIZ;  
			SetRotateY(0);
		}
	}
	inline void SetFlipVert( BOOL bFlag ) { 
		if( bFlag ) {
			m_dwDrawFlag |= EFF_FLIP_VERT;
			SetRotateX(180.0f);
		}
		else {
			m_dwDrawFlag &= ~EFF_FLIP_VERT;  
			SetRotateX(0);
		}
	}
	inline void SetFlipHoriz( bool bFlag ) {
		SetFlipHoriz( xboolToBOOL(bFlag) );
	}
	inline void SetFlipVert( bool bFlag ) {
		SetFlipVert( xboolToBOOL(bFlag) );
	}
	DWORD GetPixel( const XE::VEC2& vPos, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL  ) {	
		return GetPixel( vPos.x, vPos.y, pa, pr, pg, pb ); 	
	}
	DWORD GetPixel( float lx, float ly, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL  );
	BYTE GetMask( float lx, float ly );
	BYTE GetMask( const XE::VEC2& vLocal ) {
		return GetMask( vLocal.x, vLocal.y );
	}

	virtual xRESULT Create( const float width, const float height ) { 
		m_formatSurface = XE::xPF_ARGB4444;
		return Create(width, height
								, 0, 0
								, xNO_ALPHA
								, nullptr
								, 0
								, 0
								, FALSE, FALSE ); 
	} 
private:
	// pure virtual
	virtual bool Create( const XE::POINT& sizeSurfaceOrig
										, const XE::VEC2& vAdj
										, XE::xtPixelFormat formatSurface
										, void* const pImgSrc
										, XE::xtPixelFormat formatImgSrc
										, const XE::POINT& sizeMemSrc
										, const XE::POINT& sizeMemSrcAligned
										, bool bUseAtlas ) = 0;
	// pure virtual
	virtual bool CreateSub( const XE::POINT& posMemSrc
										, const XE::POINT& sizeArea
										, const XE::POINT& sizeAreaAligned
										, const XE::POINT& sizeMemSrc
										, void* const _pSrcImg
										, XE::xtPixelFormat formatImgSrc
										, const XE::VEC2& sizeRender
										, const XE::VEC2& vAdj
										, XE::xtPixelFormat formatSurface ) = 0;
public:
	// 외부노출용 인터페이스(간편버전)
	bool Create( const XE::POINT& sizeSurface
						, const XE::VEC2& vAdj
						, XE::xtPixelFormat formatSurface								
						, void* const pImgSrc
						, XE::xtPixelFormat formatImgSrc
						, const XE::POINT& sizeMemSrc
						, bool bSrcKeep, bool bMakeMask, bool bUseAtlas );
	bool CreateSub( const XE::POINT& posMemSrc
							, const XE::POINT& sizeArea
							, const XE::POINT& sizeMemSrc
							, void* const _pSrcImg
							, XE::xtPixelFormat formatImgSrc
							, const XE::VEC2& sizeRender
							, const XE::VEC2& vAdj
							, XE::xtPixelFormat formatSurface
							, bool bSrcKeep, bool bMakeMask );
	// 기존코드 호환용
	inline xRESULT Create( const float width, const float height,
												 const float adjx, const float adjy,
												 xAlpha alpha,
												 const void *pImg,
												 int bytePerPixel,
												 XCOLOR dwKeyColor/* = 0*/,
												 BOOL bSrcKeep, BOOL bMakeMask ) {
		auto bOk = Create( XE::POINT( width, height )
											 , XE::VEC2( adjx, adjy )
											 , XE::xPF_ARGB4444
											 , (void* const)pImg
											 , XE::xPF_ARGB8888
											 , m_sizeMem.ToPoint() // sizeMemSrc
											 , bSrcKeep != FALSE
											 , bMakeMask != FALSE
											 , false );
		return (bOk) ? xSUCCESS : xFAIL;
	}
	BYTE* MakeMask( DWORD *src, int memDstw, int memDsth );

	virtual void RestoreDevice() = 0;
	virtual void DestroyDevice() = 0;
protected:
	void RestoreDeviceFromSrcImg();
public:
	virtual void ClearDevice( void ) {}
	virtual void*	Lock( int *pWidth, BOOL bReadOnly = TRUE ) { return NULL; }
	virtual void	Unlock( void ) {}
  virtual void SetTexture( void ) {}

	virtual void CopySurface( XSurface *src ) {};
	virtual void CopyRect( int dstx, int dsty, DWORD *pSrc, int srcw, int srch ) {}
	virtual void CopyRectTo( DWORD *pDst, int wDst, int hDst, int xSrc, int ySrc );

	/**
	 @brief 서피스를 x.y를 좌상귀로 하는 지점에 그린다.
	*/
	virtual void Draw( float x, float y ) = 0; //{ XBREAKF(1, "구현되지않음"); }
	virtual void Draw( float x, float y, const MATRIX &mParent ) = 0;
	inline void Draw( const XE::VEC2& vPos, const MATRIX &mParent ) {
		Draw( vPos.x, vPos.y, mParent );
	}
	virtual void DrawLocal( float x, float y, float lx, float ly ) { XBREAKF(1, "구현되지않음"); }	// x,y:최종스크린좌표, lx, ly 로컬내에서 좌표. XWndDialog::Draw의 타일화 draw참고
	void Draw( const XE::VEC2& vPos ) { Draw( vPos.x, vPos.y ); }
	virtual void Draw( const MATRIX& mLocal, const MATRIX &mParent ) {}
	/**
	 @brief 서피스를 vCenter지점이 좌표축이 되도록 그린다.
	*/
	inline void DrawByAxis( const XE::VEC2& vCenter ) {
		Draw( vCenter - GetAdjustAxis() );
	}
	virtual void DrawSub( float x, float y, const XE::xRECTi *src ) = 0;
	virtual void Fill( XCOLOR col ) { XBREAKF(1, "구현되지않음"); }
	void ClearAttr();
	void DrawDirection( const XE::VEC2& vStart, const XE::VEC2& vEnd );
	/// 디바이스 자원이 해제되어 비어있는 상태인지.
	virtual bool IsEmpty() = 0;
	// lua
	void LuaDraw( float x, float y ) { Draw( x, y ); }
	void GetMatrix( const XE::VEC2& vPos, MATRIX* pOut );
}; // XSurface




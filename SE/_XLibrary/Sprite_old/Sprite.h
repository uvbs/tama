#ifndef __SPRITE_D3D__
#define __SPRITE_D3D__
#include "ResMng.h"

class XSprDat;

#ifdef _VER_OPENGL
    #include "XSurfaceOpenGL.h"
    #define XSURFACE_SDK    XSurfaceOpenGL
#else
    #include "XSurfaceD3D.h"
    #define XSURFACE_SDK    XSurfaceD3D
#endif
//class XSprite : public XSurfaceD3D
class XSprite : public XSURFACE_SDK
{
public:
	static int s_sizeTotalMem;		// 스프라이트들의 총 메모리할당량(이미지데이타만..)
private:
	void Init( void ) {
	}
	void Destroy( void );
public:
	XSprite( BOOL bHighReso ) : XSURFACE_SDK( bHighReso ) { Init(); }
	virtual ~XSprite() { Destroy(); }	// 부모의 파괴자를 호출하는지 확인할것
	//
	void Save( XBaseRes *pRes ) {}
	void Load( XSprDat *pSprDat,XBaseRes *pRes, BOOL bSrcKeep=FALSE );
//	DWORD GetPixel( int lx, int ly, BYTE *pa = NULL, BYTE *pr = NULL, BYTE *pg = NULL, BYTE *pb = NULL  );
	void SetScale( float sx, float sy ) { m_fScaleX = sx; m_fScaleY = sy; }	// 여기는 GRAPHICS->GetfGScale과 곱하지 않는다 XLayerImage::Draw에서 이걸 계속 호출하기 때문이다
	void SetScale( XE::VEC2 vScale ) { m_fScaleX = vScale.x; m_fScaleY = vScale.y; }
	void SetScale( float scalexy ) { m_fScaleX = m_fScaleY = scalexy; }
	MATRIX* GetMatrix( MATRIX *pOut, float lx, float ly );
};


#endif // __SPRITE_D3D__
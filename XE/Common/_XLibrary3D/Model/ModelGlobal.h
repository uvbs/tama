#ifndef		__MODELGLOBAL_H__
#define		__MODELGLOBAL_H__

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include "Mathematics.h"

#define XE_NONE				0
#define XE_REFLECT			(0x00000001)
#define XE_OPACITY			(0x00000002)
#define XE_2SIDE			(0x00000004)
#define XE_SELF_ILLUMINATE	(0x00000008)
#define XE_SPECULAR			(0x00000010)
#define XE_BUMP				(0x00000020)
#define XE_HIDE				(0x00000040)			// 렌더링 금지. 잠시 꺼두셔도 좋습니다.
//#define XE_NO_OPTION_EFFECT	(0x10000000)		// CreateParticle()을 실행안하는 옵션.
#define XE_ITEM_FIRE		(0x00010000)		// 불 파티클을 생성하는 옵션
#define XE_ITEM_ELEC		(0x00020000)		// 전기 이펙트를 생성하는 옵션.
#ifdef __XEFFECT1006
#define XE_ITEM_WATER		(0x00040000)		// 물 이펙트를 생성하는 옵션.
#define XE_ITEM_WIND		(0x00080000)		// 바람 이펙트를 생성하는 옵션.
#define XE_ITEM_EARTH		(0x00100000)		// 땅 이펙트를 생성하는 옵션.
#endif // xEffect1006
#define XE_NOBUMP			(0x00200000)		// 범프사용않음
#define XE_HIGHLIGHT_OBJ	(0x00400000)		// 하이라이트 오브젝트(선택시)
// 상위 8비트는 쓰지 말것.  속성레벨로 쓸꺼다. 0 ~ 5범위.
extern int g_nMaxTri;

void	SetTransformViewProj( const MATRIX &mViewProj );
void	SetTransformView( const MATRIX &mView );
void	SetTransformProj( const MATRIX &mProj );
MATRIX *GetTransformView( void );
void	SetLight( BOOL bLight );
BOOL GetLight( void );
void	SetNight( BOOL bNight );
BOOL GetNight( void );

class CModelGlobal
{
	GLuint	m_nGL_ExtTexture;		// 외부 지정 텍스쳐.
	
public:
	Vec3		m_vCameraPos, m_vCameraDest, m_vCameraForward;
	BOOL	m_bDepthBias;
	
	CModelGlobal() 
	{
		m_bDepthBias = FALSE;
	}
	~CModelGlobal() {}
	
	void	SetCamera( const Vec3 &vPos, const Vec3 &vDest );
	
	// 외부 지정 텍스쳐
	// 이것을 지정한 후 CObject3D::Render()를 하면 지정된 텍스쳐로 나온다.
	GLuint	GetTexture( void ) { return m_nGL_ExtTexture; }
	void	SetTexture( GLuint nGL_Texture ) { m_nGL_ExtTexture = nGL_Texture; }		// 외부지정 텍스쳐.
		
	xRESULT RestoreDeviceObjects( void );
};

extern CModelGlobal g_ModelGlobal;




#endif


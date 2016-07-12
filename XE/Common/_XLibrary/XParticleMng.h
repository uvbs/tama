#pragma once

#include "XList.h"
#include "XPool.h"
#include "etc/Timer.h"
#include "XParticle.h"
#include "XFramework/client/XEmitter.h"

// 파티클 매니저 기본형
class XBaseParticleMng
{
	XParticleDelegate *m_pDelegate;
	void Init() {
        m_MaxParticle = 0;
		m_pDelegate = NULL;
	}
	void Destroy() {
		DestroyAll();
	}
protected:
	XList2<XBaseParticle> m_listParticle;
//	list<XBaseParticle*> m_listParticle;
	XList4<XEmitter*> m_listEmitter;
    int m_MaxParticle;
public:
	XBaseParticleMng( XParticleDelegate *pDelegate, int maxParticle )
	: m_listParticle(_T("particle")) { 
		Init(); 
		m_pDelegate = pDelegate;
		m_listParticle.Create( maxParticle );		// 파티클 포인터를 maxParticle만큼 만든다.
        m_MaxParticle = maxParticle;
	}
	virtual ~XBaseParticleMng() { Destroy(); }
	//
	int GetNumParticle( void ) {
		return m_listParticle.GetnNum();
	}
	GET_ACCESSOR( XParticleDelegate*, pDelegate );
	//
	XBaseParticle* Add( XBaseParticle *pParticle ) {
		if( pParticle == NULL )
			return NULL;
		if( m_listParticle.GetnNum() < m_MaxParticle )
			m_listParticle.Add( pParticle );
		else	{
			SAFE_DELETE( pParticle );
		}
		return pParticle;
	}
	void AddEmitter( XEmitter* pEmitter );
	void DestroyAll( void );
	//
	virtual void FrameMove( float dt );
	virtual int Draw( void )=0;
};
//////////////////////////////////////////////////////////////////////////////////////
// 포인트 스프라이트용 파티클 매니저
class XPointSpriteParticleMng : public XBaseParticleMng
{
public:
private:
	xDM_TYPE m_DrawMode;
	void Init() {
		m_pBuffer = NULL;
		m_DrawMode = xDM_SCREEN;
	}
	void Destroy() {
		SAFE_DELETE_ARRAY( m_pBuffer );
	}
protected:
	XPointSpriteParticle::xOUT *m_pBuffer;
public:
	XPointSpriteParticleMng( XParticleDelegate *pDelegate, int maxParticle );
	virtual ~XPointSpriteParticleMng() { Destroy(); }
	GET_SET_ACCESSOR( xDM_TYPE, DrawMode );
	//
	XPointSpriteParticle* Add( XPointSpriteParticle *pParticle ) {
		XBaseParticleMng::Add( pParticle );
		return pParticle;
	}
	virtual int Draw( void );
	virtual void RestoreDevice( void ) {}
};

//extern XPointSpriteParticleMng *PARTICLE_MNG;
XE_NAMESPACE_START( xParticle )
struct xFunc;
XE_NAMESPACE_END
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/10 15:25
*****************************************************************/
class XParticleMng
{
public:
//	static std::shared_ptr<XParticleMng>& sGet();
	static XParticleMng* sCreate();
private:
//	static std::shared_ptr<XParticleMng> s_spInstance;
	XParticleDelegate *m_pDelegate = nullptr;
	XList4<XEmitter*> m_listEmitter;
	XList4<XParticleSprite*> m_listSprs;		// sprObj파티클 리스트
	void Init() {}
	void Destroy();
protected:
	XList4<XPointSpriteParticle*> m_listPoints;	// 포인트 파티클 리스트
	int m_maxBuffer = 1000;
	XPointSpriteParticle::xOUT *m_pBuffer = nullptr;
	XE::xtBlendFunc m_BlendFunc = XE::xBF_MULTIPLY;
public:
	XParticleMng();
	virtual ~XParticleMng() { Destroy(); }
	//
	SET_ACCESSOR( XParticleDelegate*, pDelegate );
	GET_SET_ACCESSOR( XE::xtBlendFunc, BlendFunc );
	void Add( XPointSpriteParticle* pParticle );
	void Add( XParticleSprite* pParticle );
	void AddEmitter( XEmitter* pEmitter );
	void DestroyAll( void );
	//
	virtual void FrameMove( float dt );
	virtual void Draw();
	/// 현재 m_pBuffer에 쌓인 포인트스프라이트를 draw한다.
	virtual void DrawPointSprites( int numDraw, XSurface* pTexture, XE::xtBlendFunc blendFunc ) = 0;	// 플랫폼마다 따로 구현해준다.
	virtual void RestoreDevice( void ) = 0;
	void CreateSfx( const char* cIdentifier, const XE::VEC2& vPos );
	static xParticle::XCompFunc* sCreateFuncComponent( xParticle::xFunc& funcProp );
}; // class XParticleMng


/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/10 18:11
*****************************************************************/
#ifdef _VER_DX
class XParticleMngDX : public XParticleMng
{
	void Init() {}
	void Destroy() {}
public:
	XParticleMngDX() { Init(); }
	virtual ~XParticleMngDX() { Destroy(); }
	//
	void DrawPointSprites( int numDraw, XSurface* pTexture, XE::xtBlendFunc blendFunc ) override;
	void RestoreDevice( void ) override {}
}; // class XParticleMngDX
#endif // _VER_DX

////////////////////////////////////////////////////////////////
#ifdef _VER_OPENGL
#include "OpenGL2/XParticleOpenGL.h"
#endif

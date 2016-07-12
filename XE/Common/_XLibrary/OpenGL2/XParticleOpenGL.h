/********************************************************************
	@date:	2015/06/10 18:24
	@file: 	C:\xuzhu_work\Project\iPhone_may\XE\Common\_XLibrary\OpenGL2\XParticleOpenGL.h
	@author:	xuzhu
	
	@brief:	파티클 매니저 OpenGL버전
*********************************************************************/
#pragma once
#ifdef _VER_OPENGL
#include "XParticleMng.h"
#include "XGraphicsOpenGL.h"
/****************************************************************
* @brief 
* @author xuzhu
* @date	2015/06/10 18:18
*****************************************************************/
class XParticleMngGL : public XParticleMng
{
#ifdef _XVAO
	GLuint m_glArray = 0;
#endif
	GLuint m_glVB = 0;
	XShader *m_pShader= nullptr;
	void Init() {}
	void Destroy();
public:
	XParticleMngGL();
	virtual ~XParticleMngGL() { Destroy(); }
	//
	void DrawPointSprites( int numDraw, XSurface* pTexture, XE::xtBlendFunc blendFunc ) override;
	void RestoreDevice( void ) override;
}; // class XParticleMngGL

#if 0
/*
#include "XGraphicsOpenGL.h"
#include "XParticleMng.h"

class XParticleMngGL : public XPointSpriteParticleMng
{
#ifdef _XVAO
    GLuint m_glArray;
#endif
    GLuint m_glVB;
    XShader *m_pShader;
    void Init() {
        m_pShader = NULL;
#ifdef _XVAO
        m_glArray = 0;
#endif
        m_glVB = 0;
    }
    void Destroy() {
        glDeleteBuffers( 1, &m_glVB );
#ifdef _XVAO
        glDeleteVertexArraysOES(1, &m_glArray );
#endif
        SAFE_DELETE( m_pShader );
    }
public:
    XParticleMngGL( XParticleDelegate *pDelegate, int maxParticle );
    virtual ~XParticleMngGL() { Destroy(); }
    //
    int Draw( void );
	void RestoreDevice( void );
};
*/
#endif // 0
#endif // gl
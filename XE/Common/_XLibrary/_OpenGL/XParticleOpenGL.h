/*
 *  XParticleOpenGL.h
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#pragma once
#include "XGraphicsOpenGL.h"
#include "XParticleMng.h"

class XParticleMngGL : public XPointSpriteParticleMng
{
    GLuint m_glVB;
    void Init() {
        m_glVB = 0;
    }
    void Destroy() {
        glDeleteBuffers( 1, &m_glVB );
    }
public:
    XParticleMngGL( int maxParticle );
    virtual ~XParticleMngGL() { Destroy(); }
    //
    int Draw( void );
};

/*
typedef struct _PointSprite {
	GLfloat x;
	GLfloat y;
	GLfloat size;
} PointSprite;
typedef struct _Color4f {
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	GLfloat alpha;
} Color4f;

struct XPARTICLE
{
	float dx, dy;
	int life;
};
class XParticle
{
	int m_nMax;
	GLuint m_idTexture;
	GLuint m_idVertices;
	GLuint m_idColors;
	PointSprite *m_vertices;
	Color4f *m_colors;
	XSurfaceOpenGL *m_pTexture;
	XPARTICLE *m_pParticles;
	void Init() {
		m_idTexture = m_idVertices = m_idColors = 0;
		m_vertices = NULL;
		m_colors = NULL;
		m_pTexture = NULL;
		m_pParticles = NULL;
	}
	void Destroy() {}
public:
	XParticle() { Init(); }
	virtual ~XParticle() { Destroy(); }
	
	void Create( LPCTSTR szFilename, int maxParticle );
	void FrameMove( void );
	void Draw( void );
};

*/

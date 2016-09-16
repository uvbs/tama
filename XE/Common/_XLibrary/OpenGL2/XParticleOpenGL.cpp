/*
 *  XParticleOpenGL.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#ifdef _VER_OPENGL
#include "XParticleOpenGL.h"
#include "XShader.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//////////////////////////////////////////////////////////////////////////
XParticleMngGL::XParticleMngGL()
{
	Init();
	m_pShader = new XShader;
	m_pShader->LoadShaderFromStr( xvShader_PointSprite, xfShader_PointSprite, "point_spr_shader" );
#ifdef _XVAO
	glGenVertexArraysOES(1, &m_glArray );
#endif
	glGenBuffers( 1, &m_glVB );
}
void XParticleMngGL::Destroy() 
{
	glDeleteBuffers( 1, &m_glVB );
#ifdef _XVAO
	glDeleteVertexArraysOES( 1, &m_glArray );
#endif
	SAFE_DELETE( m_pShader );
}

void XParticleMngGL::RestoreDevice( void ) 
{
	XBREAK( m_pShader == NULL );
	if( m_pShader ) {
		m_pShader->LoadShaderFromStr( xvShader_PointSprite, xfShader_PointSprite, "point_spr_shader" );
	}
#ifdef _XVAO
	glGenVertexArraysOES( 1, &m_glArray );
#endif
	glGenBuffers( 1, &m_glVB );
}

void XParticleMngGL::DrawPointSprites( int numDraw, XSurface* pTexture, XE::xtBlendFunc blendFunc )
{
	int itor = 0;
	XBREAK( pTexture == NULL );
	if( blendFunc == XE::xBF_NO_DRAW )
		return;
	XBREAK( m_pShader == nullptr );
	pTexture->SetTexture();
	m_pShader->SetShader(XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );
#ifdef _XVAO
	glBindVertexArrayOES( m_glArray );
#endif
	glBindBuffer( GL_ARRAY_BUFFER, m_glVB );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glEnableVertexAttribArray( XE::ATTRIB_POS );
	glEnableVertexAttribArray( XE::ATTRIB_SIZE );
	glEnableVertexAttribArray( XE::ATTRIB_COLOR );
	glDisableVertexAttribArray( XE::ATTRIB_TEXTURE );
	glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, sizeof(XPointSpriteParticle::xOUT), (void*)offsetof(XPointSpriteParticle::xOUT,x));
	glVertexAttribPointer( XE::ATTRIB_SIZE, 1, GL_FLOAT, GL_FALSE, sizeof(XPointSpriteParticle::xOUT), (void*)offsetof(XPointSpriteParticle::xOUT,size));
	glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(XPointSpriteParticle::xOUT), (void*)offsetof(XPointSpriteParticle::xOUT,r) );
	glBufferData( GL_ARRAY_BUFFER, sizeof(XPointSpriteParticle::xOUT) * numDraw, m_pBuffer, GL_DYNAMIC_DRAW );
	glEnable( GL_BLEND );
	glBlendEquation (GL_FUNC_ADD);
	if( blendFunc == XE::xBF_ADD )
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	else
	if( blendFunc == XE::xBF_SUBTRACT ) {
		glBlendFunc (GL_ONE, GL_ONE);
		glBlendEquation (GL_FUNC_REVERSE_SUBTRACT);
	} else
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays( GL_POINTS, 0, numDraw );
	glDisableVertexAttribArray( XE::ATTRIB_SIZE );
	glDisableVertexAttribArray( XE::ATTRIB_COLOR );
}

#if 0
/*
XParticleMngGL::XParticleMngGL( XParticleDelegate *pDelegate, int maxParticle )
: XPointSpriteParticleMng( pDelegate, maxParticle )
{ 
    Init();
    m_pShader = new XShader;
    m_pShader->LoadShaderFromString( xvShader_PointSprite, xfShader_PointSprite );
#ifdef _XVAO
    glGenVertexArraysOES(1, &m_glArray );
#endif
    glGenBuffers( 1, &m_glVB );
}

void XParticleMngGL::RestoreDevice( void )
{
	XBREAK( m_pShader == NULL );
    m_pShader->LoadShaderFromString( xvShader_PointSprite, xfShader_PointSprite );
#ifdef _XVAO
    glGenVertexArraysOES(1, &m_glArray );
#endif
    glGenBuffers( 1, &m_glVB );
	
}

int XParticleMngGL::Draw( void )
{
    int num = XPointSpriteParticleMng::Draw();
    if( num == 0 )
        return 0;
    //
	int itor = 0;
	XBaseParticle *pParticle = m_listParticle.GetNext( &itor );
	if( pParticle == NULL )     return 0;
    XPointSpriteParticle *pPoint = static_cast<XPointSpriteParticle*>( pParticle );
    XSurface *psfcTexture = pPoint->GetpsfcTexture();
    XBREAK( psfcTexture == NULL );
    //
    psfcTexture->SetTexture();
    //
    m_pShader->SetShader(XE::x_mViewProjection, 1.0f, 1.0f, 1.0f, 1.0f );
//    MATRIX mModel;
//    MatrixIdentity( mModel );
//    m_pShader->SetMatrixModel( mModel );
#ifdef _XVAO
    glBindVertexArrayOES( m_glArray );
#endif
    glBindBuffer( GL_ARRAY_BUFFER, m_glVB );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glEnableVertexAttribArray( XE::ATTRIB_POS );
    glEnableVertexAttribArray( XE::ATTRIB_SIZE );
    glEnableVertexAttribArray( XE::ATTRIB_COLOR );
    glDisableVertexAttribArray( XE::ATTRIB_TEXTURE );
    glVertexAttribPointer( XE::ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, sizeof(XPointSpriteParticle::xOUT), (void*)offsetof(XPointSpriteParticle::xOUT,x));
    glVertexAttribPointer( XE::ATTRIB_SIZE, 1, GL_FLOAT, GL_FALSE, sizeof(XPointSpriteParticle::xOUT), (void*)offsetof(XPointSpriteParticle::xOUT,size));
    glVertexAttribPointer( XE::ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(XPointSpriteParticle::xOUT), (void*)offsetof(XPointSpriteParticle::xOUT,r) );
    glBufferData( GL_ARRAY_BUFFER, sizeof(XPointSpriteParticle::xOUT) * num, m_pBuffer, GL_DYNAMIC_DRAW );

    glEnable( GL_BLEND );
	if( GetDrawMode() == xDM_NORMAL )
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	else
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    glDrawArrays( GL_POINTS, 0, num );
    
    glDisableVertexAttribArray( XE::ATTRIB_SIZE );
    glDisableVertexAttribArray( XE::ATTRIB_COLOR );
                 
    //
//     glEnable( GL_POINT_SPRITE_OES );
//     glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
//     glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
//     //
//     glBindBuffer( GL_ARRAY_BUFFER, m_glVB );
//     glEnableClientState( GL_VERTEX_ARRAY );
//     glEnableClientState( GL_POINT_SIZE_ARRAY_OES );
//     glEnableClientState( GL_COLOR_ARRAY );
//     glDisableClientState( GL_TEXTURE_COORD_ARRAY ); // disable
//     //
//     glVertexPointer( 2, GL_FLOAT, sizeof(XPointSpriteParticle::xOUT), 0 );
//     glPointSizePointerOES( GL_FLOAT, sizeof(XPointSpriteParticle::xOUT), (GLvoid*)(sizeof(GL_FLOAT)*2) );
//     glColorPointer( 4, GL_FLOAT, sizeof(XPointSpriteParticle::xOUT), (GLvoid*)(sizeof(GL_FLOAT)*3) );
//     //
//     glBufferData( GL_ARRAY_BUFFER, sizeof(XPointSpriteParticle::xOUT) * num, m_pBuffer, GL_DYNAMIC_DRAW );
//     //
//     glDrawArrays( GL_POINTS, 0, num );
// 
// 	glDisableClientState(GL_POINT_SPRITE_OES);
// 	glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
// 	glDisableClientState(GL_COLOR_ARRAY);
// 	glDisable(GL_POINT_SPRITE_OES);
     return num;
}*/
#endif // 0
#endif // gl
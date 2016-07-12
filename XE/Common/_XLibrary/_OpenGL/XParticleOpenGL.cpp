/*
 *  XParticleOpenGL.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "XParticleOpenGL.h"

XParticleMngGL::XParticleMngGL( int maxParticle ) 
: XPointSpriteParticleMng( maxParticle ) 
{ 
    Init(); 
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
    glEnable( GL_POINT_SPRITE_OES );
    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    //
    glBindBuffer( GL_ARRAY_BUFFER, m_glVB );
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_POINT_SIZE_ARRAY_OES );
    glEnableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY ); // disable
    //
    glVertexPointer( 2, GL_FLOAT, sizeof(XPointSpriteParticle::xOUT), 0 );
    glPointSizePointerOES( GL_FLOAT, sizeof(XPointSpriteParticle::xOUT), (GLvoid*)(sizeof(GL_FLOAT)*2) );
    glColorPointer( 4, GL_FLOAT, sizeof(XPointSpriteParticle::xOUT), (GLvoid*)(sizeof(GL_FLOAT)*3) );
    //
    glBufferData( GL_ARRAY_BUFFER, sizeof(XPointSpriteParticle::xOUT) * num, m_pBuffer, GL_DYNAMIC_DRAW );
    //
    glDrawArrays( GL_POINTS, 0, num );

	glDisableClientState(GL_POINT_SPRITE_OES);
	glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_POINT_SPRITE_OES);
    return num;
}

/*
void XParticle::Create( LPCTSTR szFilename, int maxParticle )
{
	m_nMax = maxParticle;
	m_vertices = (PointSprite *)malloc( sizeof(PointSprite) * m_nMax );
	m_colors = (Color4f *)malloc( sizeof(Color4f) * m_nMax );
	m_pParticles = (XPARTICLE *)malloc( sizeof(XPARTICLE) * m_nMax );
	memset( m_vertices,0, sizeof(PointSprite) * m_nMax );
	memset( m_colors, 0, sizeof(Color4f) * m_nMax );
	memset( m_pParticles, 0, sizeof(XPARTICLE) * m_nMax );
	for( int i = 0; i < m_nMax; i ++ )
	{
		m_vertices[i].x = xRandomF(480.0f);
		m_vertices[i].y = xRandomF(320.0f);
		m_vertices[i].size = 10.0f + xRandomF(50.0f);
		m_pParticles[i].dx = -5.0f + xRandomF(10.0f);
		m_pParticles[i].dy = -5.0f + xRandomF(10.0f);
		m_colors[i].red = 0.5f + xRandomF(0.5f);
		m_colors[i].green = 0.5f + xRandomF(0.5f);
		m_colors[i].blue = 0.5f + xRandomF(0.5f);
		m_colors[i].alpha = 1.0f;
	}
	glGenBuffers(1, &m_idVertices );
	glGenBuffers(1, &m_idColors );
	glBindBuffer(GL_ARRAY_BUFFER, m_idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PointSprite)*m_nMax, m_vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_idColors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color4f)*m_nMax, m_colors, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	m_pTexture = new XSurfaceOpenGL( TRUE );
	m_pTexture->CreatePNG( szFilename );
}

void XParticle::FrameMove( void )
{
	for( int i = 0; i < m_nMax; i ++ )
	{
		m_vertices[i].x += m_pParticles[i].dx;
		m_vertices[i].y += m_pParticles[i].dy;
		if( m_vertices[i].x > 480.0f || m_vertices[i].x < 0 ) 
		{
			m_vertices[i].x -= m_pParticles[i].dx;
			m_pParticles[i].dx = -m_pParticles[i].dx;
		}
		if( m_vertices[i].y > 320.0f || m_vertices[i].y < 0 )
		{
			m_vertices[i].y -= m_pParticles[i].dy;
			m_pParticles[i].dy = -m_pParticles[i].dy;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PointSprite)*m_nMax, m_vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_idColors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color4f)*m_nMax, m_colors, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

void XParticle::Draw( void )
{
	glPushMatrix();
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrthof(0, GRAPHICS->GetScreenWidth(), GRAPHICS->GetScreenHeight(), 0, -1.0f, 1.0f);
//	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_pTexture->GetTextureID() );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
//	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//	glBlendFunc( GL_ONE, GL_ONE );
	glEnable( GL_POINT_SPRITE_OES );
	glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
	glEnableClientState( GL_VERTEX_ARRAY );
	glBindBuffer( GL_ARRAY_BUFFER, m_idVertices );
	glVertexPointer(2, GL_FLOAT, sizeof(PointSprite), 0);
	glEnableClientState( GL_POINT_SIZE_ARRAY_OES );
	glPointSizePointerOES( GL_FLOAT, sizeof(PointSprite), (GLvoid*) (sizeof(GL_FLOAT)*2));
	glEnableClientState( GL_COLOR_ARRAY );
	glBindBuffer(GL_ARRAY_BUFFER, m_idColors);
	glColorPointer(4, GL_FLOAT, 0, 0);
	glDrawArrays(GL_POINTS, 0, m_nMax);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_POINT_SPRITE_OES);
	glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_POINT_SPRITE_OES);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}
*/
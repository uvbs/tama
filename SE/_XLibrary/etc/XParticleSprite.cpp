/*
 *  _XParticleD3D.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "XParticleSprite.h"
//#include "XMap.h"
#include "XMapCiv.h"		
#if 1

XPool<_XParticle>* _XParticle::s_pPool = NULL;

_XParticle* _XParticleMng::Add( float x, float y, float dx, float dy, DWORD idTexture, float secLife, float size )
{
	_XParticle *pParticle = new _XParticle( x, y, dx, dy, size, idTexture, secLife, XCOLOR_WHITE );
	m_listParticle.Add( pParticle );
	return pParticle;
}

void _XParticleMng::FrameMove( float dt )
{
	XLIST2_MANUAL_LOOP( m_listParticle, _XParticle*, itor, pNode )
	{
		CTimer *pTimer = &pNode->GettimerLife();
		if( pTimer->IsOver() )
		{
//			m_poolParticle.DeleteCurrent( pNode_idxPool );
			m_listParticle.Delete( itor++ );
			continue;
		}
		if( pTimer->GetPassTime() > (pTimer->GetWaitTime() / 3) * 2 )
			pNode->m_idTexture = 20;
		else
		if( pTimer->GetPassTime() > pTimer->GetWaitTime() / 3 )
			pNode->m_idTexture = 19;

		pNode->m_x += ( pNode->m_dx * dt );
		pNode->m_y += ( pNode->m_dy * dt );
		++itor;
	} END_LOOP;
}

//----------------------------------------------------------------------------------------------------

int CompParticle( const void *pa, const void *pb )
{
	_XParticle **pp1 = (_XParticle **)pa;
	_XParticle **pp2 = (_XParticle **)pb;
	DWORD id1 = (*pp1)? (*pp1)->GetidTexture() : 0xffffffff;
	DWORD id2 = (*pp2)? (*pp2)->GetidTexture() : 0xffffffff;
	return id1 - id2;
}

void _XParticleMngSprite::Draw( void )
{
	XSprite *pSpr = NULL;
	XSprDat *pSprDat = m_pSprDat;
	XBREAK( pSprDat == NULL );
	// sort
	Sort( CompParticle );
	//
	XE::VEC2 vPos, vposNode;
	float fScaLE2View = WORLD->GetfCameraScale();		// 이거 이렇게 하지말고 파라메터로 scale값을 받든지 월드를 받든지 해서 쓰자.
//	_XParticle *pNode;
//	GetNextClear();
//	while( pNode = GetNext() )
	
	XLIST2_LOOP( m_listParticle, _XParticle*, pNode )
	{
		pSpr = pSprDat->GetSprite( pNode->m_idTexture );
		XBREAK( pSpr == NULL );
		vposNode.Set( pNode->Getx(), pNode->Gety() );
		vPos = WORLD->Projection( vposNode );
		pSpr->SetScale( fScaLE2View );
		pSpr->Draw( vPos );
	} END_LOOP;
}

#endif // 0 
///////////////////////////////////////////////////////////////////////////////////////
void _XParticleMngSnow::FrameMove( float dt )
{
	XLIST2_MANUAL_LOOP( m_listParticle, _XParticle*, itor, pNode )
	{
		if( pNode->Getdy() )
		{
			if( ProcessCollision( pNode ) )
				OnCollision( pNode );
		} else
		{
			if( pNode->GettimerLife().IsOver() )
				pNode->SetDestroy( TRUE );
		}
		if( pNode->IsLive() == FALSE )
		{
			SAFE_DELETE( pNode );
			m_listParticle.Delete( itor++ );
			continue;
		}
		{
			float dx = 0;
			if( pNode->GettimerLife().IsOff() )
				dx = xRandomF( -0.3f, 0.1f );
			pNode->m_x += dx * dt;
			pNode->m_y += pNode->Getdy() * dt;
		}
		++itor;
	} END_LOOP;
}

void _XParticleMngSnow::Draw( void )
{
	XLIST2_LOOP( m_listParticle, _XParticle*, pNode )
	{
		XE::VEC2 vPos = Projection( pNode->GetvPos() );		// 파티클 좌표를 화면좌표로 바꿈.
		if( vPos.x >= 0 && vPos.x < 480.f )
		{
			if( vPos.y >= 0 && vPos.y < 320.f )
			{
				XCOLOR col;
				if( pNode->GettimerLife().IsOn() )
				{
					float alpha = 1.0f - pNode->GettimerLife().GetSlerp();
					col = XCOLOR_RGBA( 255, 255, 255, (BYTE)(255*alpha) );
				}
				else
					col = XCOLOR_WHITE;

				GRAPHICS->DrawPoint( vPos.x, vPos.y, 1.f, col );
				GRAPHICS->DrawPoint( vPos.x-0.5f, vPos.y, 1.f, col );
				GRAPHICS->DrawPoint( vPos.x+0.5f, vPos.y, 1.f, col );
				GRAPHICS->DrawPoint( vPos.x, vPos.y-0.5f, 1.f, col );
				GRAPHICS->DrawPoint( vPos.x, vPos.y+0.5f, 1.f, col );
			}
		}
	} END_LOOP;
}

XE::VEC2 XCivParticleMngSnow::Projection( const XE::VEC2& vPos )
{
	return m_prefMap->Projection( vPos );
}
BOOL XCivParticleMngSnow::ProcessCollision( _XParticle *pNode )
{
	if( pNode->Gety() > m_prefMap->GetHeights( pNode->Getx() ) )
		return TRUE;
	return FALSE;
}
void XCivParticleMngSnow::OnCollision( _XParticle *pNode )
{
	pNode->GettimerLife().Set( 1.0f );
	pNode->Setdy( 0 );
}
///////////////////////////////////////////////////////////////////////////////////////
void _XParticleMngRain::Draw( void )
{
	if( m_bEnable )
	{
		XLIST2_LOOP( m_listParticle, _XParticle*, pNode )
		{
			XE::VEC2 vPos = Projection( pNode->GetvPos() );		// 파티클 좌표를 화면좌표로 바꿈.
			XE::VEC2 vPrev = Projection( pNode->GetvPrev() );		// 파티클 좌표를 화면좌표로 바꿈.
			if( vPos.x >= 0 && vPos.x < 480.f )
			{
				if( vPos.y >= 0 && vPos.y < 320.f )
				{
					GRAPHICS->DrawLine( vPos, vPrev, XCOLOR_RGBA( 255, 255, 255, 64 ), XCOLOR_RGBA( 255, 255, 255, 0 ) );
				}
			}
		} END_LOOP;
		// 안개
		BYTE a = 255;
		if( m_State == 1 )		// 서서히 안개가 덮히는중.
		{
			a = (BYTE)(m_timerFog.GetSlerp() * 255.f);
		} else
		if( m_State == 4 )
		{
			a = (BYTE)((1.0f - m_timerFog.GetSlerp()) * 255.f);
		}
			
		GRAPHICS->FillRect( 0, 80, 480, 160, XCOLOR_RGBA( 255,255,255,0), XCOLOR_RGBA( 255,255,255,0),
												XCOLOR_RGBA( 255,255,255,a),  XCOLOR_RGBA( 255,255,255,a) );
		GRAPHICS->FillRect( 0, 240, 480, 80, XCOLOR_RGBA( 255,255,255,a) );
	}
}

void XCivParticleMngRain::FrameMove( float dt )
{
	if( m_State == 0 )	// off->on init
	{
		m_bEnable = TRUE;
		m_timerFog.Set( 1.5f );
		m_State = 1;
		XLIST2_DESTROY( m_listParticle, _XParticle* );
		m_listParticle.Clear();
		
	} else
	if( m_State == 1 )	// 서서히 안개가 덮힘.
	{
		if( m_timerFog.IsOver() )		// 타이머 다되면 완전히 안개켜진상태
			m_State = 2;		
	} else
	if( m_State == 2 )	// on상태
	{
	} else
	if( m_State == 3 )	// on->off init
	{
		m_timerFog.Set( 1.5f );
		m_State = 4;
	} else
	if( m_State == 4 )	// 서서히 안개가 걷힘.
	{
		if( m_timerFog.IsOver() )		// 타이머가 다되면 안개가 완전히 걷힘.
		{
			m_State = 5;
			m_bEnable = FALSE;		// 안개끔.
		}
	} else
	if( m_State == 5 )
	{
	}
	
	if( m_bEnable == FALSE )		return;
	XLIST2_MANUAL_LOOP( m_listParticle, _XParticle*, itor, pNode )
	{
		// 시작점이 바닥에 충돌했는가?
		if( pNode->Gety() > m_prefMap->GetHeights( pNode->Getx() ) )
		{
			pNode->Setdx( 0 );
			pNode->Setdy( 0 );		// 시작점 이동 멈춤.	
			pNode->Sety( m_prefMap->GetHeights( pNode->Getx() ) );
		}
		if( pNode->GetvPrev().y > m_prefMap->GetHeights( pNode->GetvPrev().x ) )
		{
			pNode->GetvPrevDelta().y = 0;
			pNode->GetvPrev().y = 0;
			pNode->SetDestroy( TRUE );
		}
		if( pNode->IsLive() == FALSE )
		{
			SAFE_DELETE( pNode );
			m_listParticle.Delete( itor++ );
			continue;
		}
		{
			pNode->GetvPrev() += pNode->GetvPrevDelta() * dt;
			pNode->m_x += pNode->Getdx() * dt;
			pNode->m_y += pNode->Getdy() * dt;
		}
		++itor;
	} END_LOOP;
}

XE::VEC2 XCivParticleMngRain::Projection( const XE::VEC2& vPos )
{
	return m_prefMap->Projection( vPos );
}
BOOL XCivParticleMngRain::ProcessCollision( _XParticle *pNode )
{
	if( pNode->Gety() > m_prefMap->GetHeights( pNode->Getx() ) )
		return TRUE;
	return FALSE;
}
void XCivParticleMngRain::OnCollision( _XParticle *pNode )
{
//	pNode->GettimerLife().Set( 1.0f );
//	pNode->Setdy( 0 );
	pNode->SetDestroy( TRUE );
}

/*
void _XParticle::Create( LPCTSTR szFilename, int ma_XParticle )
{
	m_nMax = ma_XParticle;
	m_vertices = (PointSprite *)malloc( sizeof(PointSprite) * m_nMax );
	m_colors = (Color4f *)malloc( sizeof(Color4f) * m_nMax );
	m_pParticles = (_XParticle *)malloc( sizeof(_XParticle) * m_nMax );
	memset( m_vertices,0, sizeof(PointSprite) * m_nMax );
	memset( m_colors, 0, sizeof(Color4f) * m_nMax );
	memset( m_pParticles, 0, sizeof(_XParticle) * m_nMax );
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

void _XParticle::FrameMove( void )
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

void _XParticle::Draw( void )
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
/*
 *  XParticle.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "XParticleMng.h"
#include "XGraphics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
XPointSpriteParticleMng *PARTICLE_MNG = NULL;
void XBaseParticleMng::DestroyAll( void ) 
{
	XLIST2_LOOP( m_listParticle, XBaseParticle*, pObj )
//	LIST_LOOP( m_listParticle, XBaseParticle*, itor, pObj )
	{
		SAFE_DELETE( pObj );
	} END_LOOP;
//	XLIST2_DESTROY( m_listParticle, XBaseParticle* );
	m_listParticle.Clear();
//	m_listParticle.clear();
}

void XBaseParticleMng::FrameMove( float dt )
{
	XLIST2_MANUAL_LOOP( m_listParticle, XBaseParticle*, itor, pNode )
//	XBaseParticle *pNode;
//	int itor = 0;
//	while( pNode = m_listParticle.GetManualNext( &itor ) )
//	list<XBaseParticle*>::iterator itor;
//	for( itor = m_listParticle.begin(); itor != m_listParticle.end(); )
	{
//		XBaseParticle *pNode = (*itor);
		// �ı��� ���� ������Ű�� FrameMove�� ó������ �ʴ´�.
		if( pNode->IsDestroy() )
		{
			SAFE_DELETE( pNode );
			m_listParticle.Delete( itor++ );
//			m_listParticle.erase( itor++ );
			continue;
		}
		//
		pNode->FrameMove( dt );
		//
		++itor;
//	}
	} END_LOOP;
}
//
int XPointSpriteParticleMng::Draw( void )
{
	// �ؽ��� ���̵𺰷μ�Ʈ�� �Ѵ�.
	// ���� �ؽ��� ���̵𳢸� ������ ���鼭 ���ؽ� ���۸� �����.
	// �ؽ��ĸ� �ε��ϰ�
	// ���ؽ� ���۸� �о�ְ�
	// �׸���.
	float x, y;
	XPointSpriteParticle::xOUT *pBuffer = m_pBuffer;
	int num = 0;
	// ��ƼŬ���� ����̽������� �߰����ۿ� �ű��.
	XLIST2_LOOP( m_listParticle, XBaseParticle*, pNode )
//	LIST_LOOP( m_listParticle, XBaseParticle*, itor, pNode )
	{
		XPointSpriteParticle *pPoint = static_cast<XPointSpriteParticle*>( pNode );
		pPoint->Projection( pPoint->GetvPos(), &x, &y );			// ȭ����ǥ�� ����.
		if( pPoint->IsInScreen( x, y ) )		// ȭ�� �ȿ� ���°͵鸸 draw���� ��.
		{
			if( pPoint->Draw( x, y, pBuffer ) )	// ��ο쿡 �����ؾ�
			{
				++pBuffer;							// ������ ����
				++num;
			}
		}
	} END_LOOP;
	// D3D, GL�� ���� ������ ������ �Ѳ����� �׸���.
#ifdef WIN32
	int itor = 0;
	XBaseParticle *pParticle = m_listParticle.GetNext( &itor );
//	list<XBaseParticle*>::iterator itor = m_listParticle.begin();
//	XBaseParticle *pParticle = NULL;
//	if( itor != m_listParticle.end() )
//		pParticle = (*itor);
	if( pParticle )
	{
		XPointSpriteParticle *pPoint = static_cast<XPointSpriteParticle*>( pParticle );
		XSurface *psfcTexture = pPoint->GetpsfcTexture();
		XBREAK( psfcTexture == NULL );
		for( int i = 0; i < num; ++i )
		{
			XPointSpriteParticle::xOUT *p = &m_pBuffer[i];
			psfcTexture->SetColor( p->r, p->g, p->b );
			psfcTexture->SetfAlpha( p->a );
//			psfcTexture->SetfAlpha( 0.99f );
			float scale = p->size / (float)psfcTexture->GetTextureWidth();		// ��ƼŬ ����� ���� �ؽ��Ŀ� ���� �󸶳� ū��,������
			psfcTexture->SetScale( scale );
			psfcTexture->Draw( p->x, p->y );
			x = p->x;
			y = p->y;
			XCOLOR col = XCOLOR_RGBA( (BYTE)(p->r * 255), (BYTE)(p->g * 255), (BYTE)(p->b * 255), (BYTE)(p->a * 255) );
		}
	}
#else
#endif
    return num;
}


void XParticleMngRain::FrameMove( float dt )
{
	if( m_bEnable == FALSE )
	{
		if( m_timerWeather.IsOff() || m_timerWeather.IsOver() )
		{
			m_timerWeather.Set( 60.f );		// xx�ʰ� ���ȿ�� ����.
			EnableWeather( TRUE );
		}
	} else
	{
		if( m_timerWeather.IsOver() )		// �ð��� ������
		{
			EnableWeather( FALSE );		// ���ȿ�� ��.
			m_timerWeather.Set( 20.0f );	// xx�ʰ� ����.
		}
	}

	if( m_State == 0 )	// off->on init
	{
		m_bEnable = TRUE;
		m_timerFog.Set( 1.5f );
		m_State = 1;
		DestroyAll();
		
	} else
	if( m_State == 1 )	// ������ �Ȱ��� ����.
	{
		if( m_timerFog.IsOver() )		// Ÿ�̸� �ٵǸ� ������ �Ȱ���������
			m_State = 2;		
	} else
	if( m_State == 2 )	// on����
	{
	} else
	if( m_State == 3 )	// on->off init
	{
		m_timerFog.Set( 1.5f );
		m_State = 4;
	} else
	if( m_State == 4 )	// ������ �Ȱ��� ����.
	{
		if( m_timerFog.IsOver() )		// Ÿ�̸Ӱ� �ٵǸ� �Ȱ��� ������ ����.
		{
			m_State = 5;
			m_bEnable = FALSE;		// �Ȱ���.
		}
	} else
	if( m_State == 5 )
	{
	}
	//
	XBaseParticleMng::FrameMove( dt );
}
int XParticleMngRain::Draw( void ) 
{
	float x, y;
	int num = 0;
	XGraphics::xVERTEX *pBuffer = m_pBuffer;
	XBREAK( m_pBuffer == NULL );
	XLIST2_LOOP( m_listParticle, XBaseParticle*, _pNode )
//	LIST_LOOP( m_listParticle, XBaseParticle*, itor, _pNode )
	{
		XParticleRain *pNode = static_cast<XParticleRain*>( _pNode );
		pNode->Projection( pNode->GetvPos(), &x, &y );			// ȭ����ǥ�� ����.
		if( pNode->IsInScreen( x, y ) )		// ȭ�� �ȿ� ���°͵鸸 draw���� ��.
		{
			if( pNode->Draw( x, y, pBuffer ) )	// ��ο쿡 �����ؾ�
			{
				pBuffer += 2;							// ������ ����
				++num;
			}
		}
	} END_LOOP;

	if( num > 0 )
    {
        GRAPHICS->SetLineWidth( 1.0f );
		GRAPHICS->DrawLineList( m_pBuffer, num );
    }

	if( m_bEnable )
	{
		// �Ȱ�
		BYTE a = 255;
		if( m_State == 1 )		// ������ �Ȱ��� ��������.
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
	return num;
}


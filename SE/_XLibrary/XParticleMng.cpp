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
		// 파괴된 노드는 삭제시키고 FrameMove를 처리하지 않는다.
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
	// 텍스쳐 아이디별로소트를 한다.
	// 같은 텍스쳐 아이디끼리 루프를 돌면서 버텍스 버퍼를 만든다.
	// 텍스쳐를 로딩하고
	// 버텍스 버퍼를 밀어넣고
	// 그린다.
	float x, y;
	XPointSpriteParticle::xOUT *pBuffer = m_pBuffer;
	int num = 0;
	// 파티클들을 디바이스정보만 중간버퍼에 옮긴다.
	XLIST2_LOOP( m_listParticle, XBaseParticle*, pNode )
//	LIST_LOOP( m_listParticle, XBaseParticle*, itor, pNode )
	{
		XPointSpriteParticle *pPoint = static_cast<XPointSpriteParticle*>( pNode );
		pPoint->Projection( pPoint->GetvPos(), &x, &y );			// 화면좌표를 얻음.
		if( pPoint->IsInScreen( x, y ) )		// 화면 안에 들어온것들만 draw콜을 함.
		{
			if( pPoint->Draw( x, y, pBuffer ) )	// 드로우에 성공해야
			{
				++pBuffer;							// 포인터 증가
				++num;
			}
		}
	} END_LOOP;
	// D3D, GL에 따라서 버퍼의 내용을 한꺼번에 그린다.
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
			float scale = p->size / (float)psfcTexture->GetTextureWidth();		// 파티클 사이즈가 원래 텍스쳐에 비해 얼마나 큰가,작은가
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
			m_timerWeather.Set( 60.f );		// xx초간 기상효과 시작.
			EnableWeather( TRUE );
		}
	} else
	{
		if( m_timerWeather.IsOver() )		// 시간이 지나면
		{
			EnableWeather( FALSE );		// 기상효과 끔.
			m_timerWeather.Set( 20.0f );	// xx초간 맑음.
		}
	}

	if( m_State == 0 )	// off->on init
	{
		m_bEnable = TRUE;
		m_timerFog.Set( 1.5f );
		m_State = 1;
		DestroyAll();
		
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
		pNode->Projection( pNode->GetvPos(), &x, &y );			// 화면좌표를 얻음.
		if( pNode->IsInScreen( x, y ) )		// 화면 안에 들어온것들만 draw콜을 함.
		{
			if( pNode->Draw( x, y, pBuffer ) )	// 드로우에 성공해야
			{
				pBuffer += 2;							// 포인터 증가
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
	return num;
}


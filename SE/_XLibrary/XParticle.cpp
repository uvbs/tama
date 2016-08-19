/*
 *  XParticle.cpp
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#include "stdafx.h"
#include "XParticle.h"

// .cpp
//template<> XPool<XBaseParticle>* XMemPool<XBaseParticle>::s_pPool = NULL;
template<> XPool<XCivParticleSnow>* XMemPool<XCivParticleSnow>::s_pPool = NULL;
template<> XPool<XCivParticleRain>* XMemPool<XCivParticleRain>::s_pPool = NULL;
// XParticleSnow::s_pPool = new XPool<XParticleSnow>( ??? );		// �� ��ƼŬ �޸�Ǯ ����.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �Ϲ����� ��. �������� �ӵ��� ������ ����Ʈ ������ ��������.
XParticleSnow::XParticleSnow( float x, float y, XSurface *psfcSnow, float scale )
	: XPointSpriteParticle( x, y, psfcSnow, scale, XCOLOR_WHITE ) 
{
	Init();
	m_vDelta.y = xRandomF( 0.2f, 0.5f );
}
void XParticleSnow::FrameMove( float dt )
{
	if( m_vDelta.y )		// ���� �����̰� ������
	{
		float yColl;
		if( IsCollision( m_vPos.x, m_vPos.y, &yColl ) )		// �浹�˻縦 �Ѵ�.
			OnCollision( m_vPos.x, yColl );
		else
			m_vDelta.x = xRandomF( -0.2f, 0.2f );
	} else
	{
		if( m_timerLife.IsOver() )	// �ٴڿ� �������� ���� �����ð��� �帣��
			SetbDestroy( TRUE );	// ���� �������.
	}
	//
	//
	XBaseParticle::FrameMove( dt );
}

BOOL XParticleSnow::Draw( float x, float y, xOUT *pOut )
{
	if( m_timerLife.IsOn() )		// �ٴڿ� ����������
		m_a = 1.0f - m_timerLife.GetSlerp();		// ��½ð����� ���İ��� ���� ������
	else
		m_a = 1.0f;
	return XPointSpriteParticle::Draw( x, y, pOut );
}

////////////////////////////////////////////////////////////////////////
XParticleRain::XParticleRain( float x, float y, float dx, float dy )
: XBaseParticle( x, y, 0x12345678, 1.0f, XCOLOR_WHITE )
{
	m_vDelta.Set( dx, dy );
	XE::VEC2 vDelta( dx, dy );
	vDelta.Normalize();
	m_vPrev = XE::VEC2( x, 0 ) - vDelta * 100.f;
	m_vPrevDelta = XE::VEC2( dx, dy );
}

void XParticleRain::FrameMove( float dt )
{
	float yColl = 0;
	// ���ٱ� �պκ��� �浹�˻�
	if( IsCollision( m_vPos.x, m_vPos.y, &yColl ) )
	{
		m_vDelta.Set( 0, 0 );
		m_vPos.y = yColl;		// ���ٱ��� �պκ��� �浹�� ��ǥ�� ����.
	}
	// ���ٱ� �޺κ��� �浹�˻�.
	if( IsCollision( m_vPrev.x, m_vPrev.y, &yColl ) )
	{
		m_vPrevDelta.Set( 0 );
		m_vPrev.y = yColl;
		SetbDestroy( TRUE );		//  ���ٱ� �޺κб��� ���� ���������� ����
	}
	if( IsDestroy() )
		return;
	m_vPrev += m_vPrevDelta * dt;
	m_vPos += m_vDelta * dt;
}

BOOL XParticleRain::Draw( float x, float y, XGraphics::xVERTEX *pOut ) 
{
	float x2, y2;
	Projection( m_vPrev, &x2, &y2 );
	pOut->x = x;
	pOut->y = y;
	pOut->r = 1.0f;
	pOut->g = 1.0f;
	pOut->b = 1.0f;
	pOut->a = 0.25f;
	++pOut;
	pOut->x = x2;
	pOut->y = y2;
	pOut->r = 1.0f;
	pOut->g = 1.0f;
	pOut->b = 1.0f;
	pOut->a = 0;
	return TRUE;
}


/////////////////////// CIV //////////////////////////////////////////////////////////////////
XMapCiv* XCivParticleSnow::s_prefMap = NULL;
XMapCiv* XCivParticleRain::s_prefMap = NULL;

#include "XMapCiv.h"
void XCivParticleSnow::Projection( const XE::VEC2& vPos, float *pOutx, float *pOuty ) 
{ 
	XBREAK( s_prefMap == NULL );
	s_prefMap->Projection( vPos, pOutx, pOuty );
}
BOOL XCivParticleSnow::IsInScreen( float x, float y ) 
{ 
	if( x < 0 )		return FALSE;
	if( y < 0 )		return FALSE;
	if( x > XSCREEN_WIDTH )	return FALSE;
	if( y > XSCREEN_HEIGHT )	return FALSE;
	return TRUE; 
}
BOOL XCivParticleSnow::IsCollision( float x, float y, float *pOutyColl ) 
{
	XBREAK( s_prefMap == NULL );
	float yColl = s_prefMap->GetHeights( m_vPos.x );
	if( y > yColl )
	{
		*pOutyColl = yColl;
		return TRUE;
	}
	return FALSE;
}
void XCivParticleSnow::OnCollision( float x, float y ) 
{
	m_timerLife.Set( 1.0f );		// 1�ʵ��� ��´�.
	m_vPos.x = x;
	m_vPos.y = y;
	m_vDelta.y = 0;				// ���̻� �������� �ʴ´�.
	m_vDelta.x = 0;
}
//////////////////////////////////////////
void XCivParticleRain::Projection( const XE::VEC2& vPos, float *pOutx, float *pOuty ) 
{ 
	XBREAK( s_prefMap == NULL );
	s_prefMap->Projection( vPos, pOutx, pOuty );
}
BOOL XCivParticleRain::IsInScreen( float x, float y ) 
{
	if( x < 0 )		return FALSE;
	if( y < 0 )		return FALSE;
	if( x > XSCREEN_WIDTH )	return FALSE;
	if( y > XSCREEN_HEIGHT )	return FALSE;
	return TRUE; 
}
BOOL XCivParticleRain::IsCollision( float x, float y, float *pOutyColl ) 
{
	XBREAK( s_prefMap == NULL );
	float yColl = s_prefMap->GetHeights( x );
	if( y > yColl )
	{
		*pOutyColl = yColl;
		return TRUE;
	}
	return FALSE;
}

#include "stdafx.h"
#include "XParticleFramework.h"
#include "etc/XGraphics.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#if 0
//////////////////////////////////////////////////////////////////////////
void XParticleMngRain::FrameMove( float dt )
{
	// 비생성
	float x = (float)random( (int)m_vFogSize.w );
	float dx = xRandomF( -1.f, 1.f );
	float dy = 16.0f;
	XE::VEC2 vRainLT;
	if( GetpDelegate() )
		vRainLT = GetpDelegate()->OnDelegateGetRainAreaLT();
	Add( new XParticleRain( GetpDelegate(), this, vRainLT.x + x, vRainLT.y + 0, dx, dy ) );
	//
	if( m_bEnable == FALSE )
	{
		if( m_timerWeather.IsOff() || m_timerWeather.IsOver() )
		{
			if( m_bChangeWeather )
				m_timerWeather.Set( 30.f );		// xx초간 기상효과 시작.
			EnableWeather( TRUE );
		}
	} else
	{
		if( m_timerWeather.IsOver() )		// 시간이 지나면
		{
			EnableWeather( FALSE );		// 기상효과 끔.
			m_timerWeather.Set( 15.0f );	// xx초간 맑음.
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
	int num = 0;
	if( m_bEnable || m_bChangeWeather == FALSE )
	{
		float x, y;
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

		// 안개
		BYTE a = 200;
		if( m_State == 1 )		// 서서히 안개가 덮히는중.
		{
			a = (BYTE)(m_timerFog.GetSlerp() * 200.f);
		} else
		if( m_State == 4 )
		{
			a = (BYTE)((1.0f - m_timerFog.GetSlerp()) * 200.f);
		}
			
		float y1 = m_vFogLT.y;
		float h1 = m_vFogSize.h * 0.66f;
		float h2 = m_vFogSize.h - h1;
		GRAPHICS->FillRect( m_vFogLT.x, y1, m_vFogSize.w, h1, 
							XCOLOR_RGBA( 255,255,255,0), XCOLOR_RGBA( 255,255,255,0),
							XCOLOR_RGBA( 255,255,255,a), XCOLOR_RGBA( 255,255,255,a) );
//		GRAPHICS->FillRect( 0, 208, 360, 68, XCOLOR_RGBA( 255,255,255,a) );
		GRAPHICS->FillRect( m_vFogLT.x, y1+h1, m_vFogSize.w, h2, 
							XCOLOR_RGBA( 255,255,255,a) );
/*		GRAPHICS->FillRect( m_vFogLT.x, 80, m_vFogSize.w, 160, XCOLOR_RGBA( 255,255,255,0), XCOLOR_RGBA( 255,255,255,0),
												XCOLOR_RGBA( 255,255,255,a),  XCOLOR_RGBA( 255,255,255,a) );
		GRAPHICS->FillRect( m_vFogLT.x, 240, m_vFogSize.w, 80, XCOLOR_RGBA( 255,255,255,a) );
*/
	}
	return num;
}

//////////////////////////////////////////////////////////////////////////
XSurface* XParticleMngSnow::s_psfcSnow = NULL;	// 눈 파티클 텍스쳐

void XParticleMngSnow::FrameMove( float dt )
{
	// 한프레임(1/60초)당 발생해야하는 눈 파티클 개수
	m_numAddSnow += m_numSnowPerFrame * dt;
	if( m_numAddSnow >= 1.0 )
	{
		// 눈 파티클 생성
//		if( m_cntDraw & 1 )
		for( int i = 0; i < (int)m_numAddSnow; ++i )
		{
			float x = (float)random( (int)m_sizeArea.w );
	//		float x = (float)random( (int)XE::GetGameWidth() * 3 );
			float size = 0.4f;
			size += xRandomF( size );
			XE::VEC3 vSnowAreaLT( m_vAreaLT );
			// 델리게이트가 지정해준 위치가 있다면 그것을 우선으로 지정함.
			if( GetpDelegate() )
				vSnowAreaLT = GetpDelegate()->OnDelegateGetRainAreaLT();
			XParticleSnow *pSnow = new XParticleSnow( GetpDelegate(), 
													vSnowAreaLT + XE::VEC3(x,0,0), 
													s_psfcSnow, 
													size );
			Add( pSnow );
		}
		float numSnowPerFrameDT = m_numSnowPerFrame * dt;
		if( numSnowPerFrameDT < 1.0f )
			m_numAddSnow -= 1.0f;
		else
			m_numAddSnow -= (float)((int)numSnowPerFrameDT);	// 정수부분만 빼수 소숫점 부분만 남긴다.
	}
	//
	XPointSpriteParticleMng::FrameMove( dt );
}

int XParticleMngSnow::Draw( void )
{
	++m_cntDraw;
	//
#ifdef _VER_OPENGL
	return XParticleMngGL::Draw();
#else
	return XPointSpriteParticleMng::Draw();
#endif
}


#endif // 0
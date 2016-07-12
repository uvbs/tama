#pragma once
#include "XList.h"
#include "XPool.h"
#include "etc/Timer.h"
#include "XParticleMng.h"

#if 0
#ifdef _VER_OPENGL
#include "opengl2/XParticleOpenGL.h"
class XParticleMngSnow : public XParticleMngGL
#else
class XParticleMngSnow : public XPointSpriteParticleMng
#endif
{
	XE::VEC2 m_vAreaLT;		// 눈이 오는 뷰포트 영역의 좌상귀
	XE::VEC2 m_sizeArea;	// 눈이오는 뷰포트영역의 크기
	float m_numSnowPerFrame;	// 한프레임(1/60초)당 눈 파티클 발생량
	float m_numAddSnow;
public:
	static XSurface *s_psfcSnow;	// 눈 파티클 텍스쳐
private:
	int m_cntDraw;
	void Init() {
		m_cntDraw = 0;
		m_numSnowPerFrame = 0;
		m_numAddSnow = 0;
	}
	void Destroy() {}
public:
#ifdef _VER_OPENGL
	XParticleMngSnow( XParticleDelegate *pDelegate, int maxSnow,
						float x, float y, float w, float h )
	: XParticleMngGL( pDelegate, maxSnow ) {
		Init();
		m_vAreaLT = XE::VEC2( x, y );
		m_sizeArea = XE::VEC2( w, h );
		m_numSnowPerFrame = 1.f;
	}
#else
	XParticleMngSnow( XParticleDelegate *pDelegate, int maxSnow,
						float x, float y, float w, float h )
		: XPointSpriteParticleMng( pDelegate, maxSnow ) {
		Init(); 
		m_vAreaLT = XE::VEC2( x, y );
		m_sizeArea = XE::VEC2( w, h );
		m_numSnowPerFrame = 1.f;	// 디폴트로 1/60초마다 눈파티클 1개 생성된다.
	}
#endif
	virtual ~XParticleMngSnow() { Destroy(); }
	//
	GET_SET_ACCESSOR( float, numSnowPerFrame );
	//
	virtual void FrameMove( float dt );
	virtual int Draw( void );
};

//////////////////////////////////////////////////////////////////////////////////////
class XParticleMngRain : public XBaseParticleMng
{
	void Init() {
		XCLEAR_ARRAY( m_pBuffer );
		m_bEnable = FALSE;
		m_State = 0;
		m_bChangeWeather = TRUE;
	}
	void Destroy() {
	}
protected:
	XGraphics::xVERTEX m_pBuffer[ 100 ];	// 버텍스100개 곧 라인 50개를 최대로 그릴수 있다.
	// weather
	CTimer m_timerWeather;					// 주기적으로 눈/비가 왔다 안왔다를 하기위한..
	CTimer m_timerFog;
	BOOL m_bEnable;		// 기상효과가 작동중인가
	int m_State;
	XE::VEC2 m_vFogLT;		// 안개가 그려질 영역
	XE::VEC2 m_vFogSize;
	BOOL m_bChangeWeather;		// 비오다 맑았다가 변하게 할지 말지.
public:
	XParticleMngRain( XParticleDelegate *pDelegate, int maxRain, float x, float y, float w, float h ) 
		: XBaseParticleMng( pDelegate, maxRain ) { 
		Init(); 
		m_vFogLT = XE::VEC2( x, y );
		m_vFogSize = XE::VEC2( w, h );
	}
	virtual ~XParticleMngRain() { Destroy(); }
	//
	GET_SET_ACCESSOR( int, State );
	GET_SET_ACCESSOR( BOOL, bEnable );
	GET_SET_ACCESSOR( BOOL, bChangeWeather );
	GET_ACCESSOR( const XE::VEC2&, vFogLT );
	GET_ACCESSOR( const XE::VEC2&, vFogSize );
	void EnableWeather( BOOL bFlag ) {
		if( bFlag == TRUE )
			m_State = 0;		// off->on
		else
			m_State = 3;	// on->off
	}
	//
	virtual int Draw( void );
	virtual void FrameMove( float dt );
};

extern XPointSpriteParticleMng *PARTICLE_MNG;

#endif // 0
#pragma once
#include "XList.h"
#include "XPool.h"
#include "Timer.h"
/*
  * 스킬이펙트, 눈, 비
  * 여러가지 텍스쳐의 파티클들이 함께 존재 하므로 텍스쳐id로 소트를 해서 뭉텅이로 glDraw를 불러야 한다.
  * 스킬의 경우 아주 다양한 파티클텍스쳐가 사용되므로 파티클 매니저안에서 다양한 텍스쳐가 쓰일수 있도록 해야 한다.
  * 파티클 개별의 FrameMove처리 방식
    1. 파티클 하나를 객체로 설계해서 상속받아 처리한다 파티클 매니저는 1개가 이들 모두를 처리한다.
	  장점: 설계가 쉬워진다. 직관적이다. 파티클 각각의 특징의 변수를 갖기 쉬워진다.
	  단점: 파티클 매니저에서 모든 파티클의 virtual FrameMove()를 호출해야 한다.
	2. 파티클 하나는 struct형태로 존재하고. 파티클의 종류별로 매니저가 각각 존재한다.
	   파티클 종류별로 struct가 생겨야 한다. struct XPARTICLE_SNOW, XPARTICLE_SPARK 이런식.
	  장점: 파티클 매니저 내에서 파티클 struct의 루프만 돌면 되므로 FrameMove()콜 부하가 없어진다.
	  단점: 파티클 종류별로 struct가 모두 생겨야 한다. x, y, size, color같은 변수는 각각 가져야 한다.
			 파티클 종류별로 매니저가 생겨야 한다. class XParticleMngSnow 등등...
	    . 해결책: 상속. XPARTICLE_SNOW : public BASE_PARTICLE

  * 일단은 1번안으로 정석으로 설계하자. 해보고 속도가 느려지는 부분이 있으면 2번으로 최적화를 고려해 보자.
*/

class XBaseParticle;
class XBaseParticle// : public XMemPool<XBaseParticle>
{
public:
private:
	void Init() {
		m_r = m_g = m_b = m_a = 1.0f;
		m_idTexture = 0;
		m_bDestroy = FALSE;
		m_Scale = 1.0f;
	}
	void Destroy() {
	}
protected:
	// 사전 계산값
	float m_r, m_g, m_b, m_a;
	//
	XE::VEC2 m_vPos;
	float m_Scale;		// 원래 텍스쳐 이미지에 대한 스케일값. 이값으로 m_Size값이 정해진다.
	XE::VEC2 m_vDelta;
	ID m_idTexture;
	BOOL m_bDestroy;
public:
	XBaseParticle( float x, float y, DWORD idTexture, float scale, XCOLOR col ) { 
		Init(); 
		m_vPos.Set( x, y );
//		XBREAK( psfcTexture == NULL );
		XBREAK( scale == 0 );
//		m_psfcTexture = psfcTexture;
//		m_idTexture = (DWORD)psfcTexture;		// 일단은 쓸게 없으니까. 서피스포인터를 아이디화해서 쓴다.
		m_idTexture = idTexture;
		m_Scale = scale;
		m_r = XCOLOR_RGB_R(col) / 255.f;
		m_g = XCOLOR_RGB_G(col) / 255.f;
		m_b = XCOLOR_RGB_B(col) / 255.f;
		m_a = XCOLOR_RGB_A(col) / 255.f;
	}
	virtual ~XBaseParticle() { Destroy(); }
	//
	GET_ACCESSOR( const XE::VEC2&, vPos );
	GET_ACCESSOR( const XE::VEC2&, vDelta );
	GET_ACCESSOR( DWORD, idTexture );
	GET_SET_ACCESSOR( BOOL, bDestroy );
	BOOL IsDestroy( void ) { return m_bDestroy; }
	// virtual
	virtual inline void FrameMove( float dt ) {
		m_vPos += m_vDelta;
	}
	// x, y는 스크린좌표다. 화면안에 있는 좌표만 call이된다.
/*	virtual inline BOOL Draw( float x, float y, xOUT *pOut ) {
		pOut->x = x;
		pOut->y = y;
		pOut->size = m_Size;		// 값이 바뀌지 않은건 굳이 재저장 하지 말도록 하면 어떨까?
		pOut->r = m_r;
		pOut->g = m_g;
		pOut->b = m_b;
		pOut->a = m_a;
		return TRUE;
	} */
	virtual inline void Projection( const XE::VEC2& vPos, float *pOutx, float *pOuty ) {
		*pOutx = vPos.x;
		*pOuty = vPos.y;
	}
	virtual inline BOOL IsInScreen( float x, float y )=0;
	virtual inline BOOL IsCollision( float x, float y, float *pOutyColl ) { return ( y >= 320.f )? TRUE : FALSE;  }
	virtual inline void OnCollision( void ) {}
}; // class XBaseParticle

// 포인트 스프라이트 파티클
class XPointSpriteParticle : public XBaseParticle//, public XMemPool<XPointSpriteParticle>
{
public:
	struct xOUT {			// 포인트 스프라이트 디바이스 버퍼의 구조체
		float x, y;
		float size;
		float r, g, b, a;
	};
private:
	void Init() {
		m_Size = 1.0f;
		m_psfcTexture = NULL;
	}
protected:
	XSurface *m_psfcTexture;
	float m_Size;
public:
	XPointSpriteParticle( float x, float y, XSurface *psfcTexture, float scale, XCOLOR col )
		: XBaseParticle( x, y, (ID)psfcTexture, scale, col ) { 
			Init();
			m_psfcTexture = psfcTexture;
			m_Size = (psfcTexture->GetTextureSize() * m_Scale).w;
	}
	virtual ~XPointSpriteParticle() {}
	//
	GET_ACCESSOR( float, Size );
	GET_ACCESSOR( XSurface*, psfcTexture );
	// x, y는 스크린좌표다. 화면안에 있는 좌표만 call이된다.
	virtual inline BOOL Draw( float x, float y, xOUT *pOut ) {
		pOut->x = x;
		pOut->y = y;
		pOut->size = m_Size;		// 값이 바뀌지 않은건 굳이 재저장 하지 말도록 하면 어떨까?
		pOut->r = m_r;
		pOut->g = m_g;
		pOut->b = m_b;
		pOut->a = m_a;
		return TRUE;
	}
};

class XParticleSnow : public XPointSpriteParticle//, public XMemPool<XParticleSnow>
{
	void Init() {}
protected:
	CTimer m_timerLife;			// 바닥에 떨어지고 난 후의 타이머
public:
	XParticleSnow( float x, float y, XSurface *psfcSnow, float scale ); 
	virtual ~XParticleSnow() {}
	// virtual
	virtual void FrameMove( float dt );
	virtual BOOL Draw( float x, float y, xOUT *pOut );
	virtual void Projection( const XE::VEC2& vPos, float *pOutx, float *pOuty ) { XBaseParticle::Projection( vPos, pOutx, pOuty ); }
	virtual inline BOOL IsInScreen( float x, float y ) { return TRUE; }
	virtual inline void OnCollision( float x, float y ) {}
};

class XParticleRain : public XBaseParticle//, public XMemPool<XParticleRain>
{
	void Init() {}
protected:
	CTimer m_timerFog;
	XE::VEC2 m_vPrev, m_vPrevDelta;
public:
	XParticleRain( float x, float y, float dx, float dy ); 
	// virtual
	virtual void FrameMove( float dt );
	virtual BOOL Draw( float x, float y, XGraphics::xVERTEX *pOut );
	virtual void Projection( const XE::VEC2& vPos, float *pOutx, float *pOuty ) { XBaseParticle::Projection( vPos, pOutx, pOuty ); }
	virtual inline BOOL IsInScreen( float x, float y ) { return TRUE; }
	virtual inline void OnCollision( void ) {}
};

//#include "XMapCiv.h"
//#include "XGraphics.h"
class XMapCiv;
class XCivParticleSnow : public XParticleSnow, public XMemPool<XCivParticleSnow>
{
public:
	static XMapCiv *s_prefMap;
private:
	void Init() {}
public:
	XCivParticleSnow( float x, float y, XSurface *psfcSnow, float scale ) : XParticleSnow( x, y, psfcSnow, scale ) {
		Init();
	}
	virtual ~XCivParticleSnow() {}
	// virtual
	virtual void Projection( const XE::VEC2& vPos, float *pOutx, float *pOuty );
	virtual BOOL IsInScreen( float x, float y );
	virtual BOOL IsCollision( float x, float y, float *pOutyColl );
	virtual void OnCollision( float x, float y );
};

class XCivParticleRain : public XParticleRain, public XMemPool<XCivParticleRain>
{
public:
	static XMapCiv *s_prefMap;
public:
	XCivParticleRain( float x, float y, float dx, float dy ) : XParticleRain( x, y, dx, dy ) {}
	// virtual
	virtual void Projection( const XE::VEC2& vPos, float *pOutx, float *pOuty );
	virtual BOOL IsInScreen( float x, float y );
	virtual BOOL IsCollision( float x, float y, float *pOutyColl );
};

/*
class XWeatherController
{
public:
	enum xtType { xNONE, xSNOW, xRAIN };
private:
	void Init() {
		m_Type = xNONE;
		m_bEnable = FALSE;
		m_State = 0;
	}
	void Destroy() {}
protected:
	BOOL m_bEnable;	// 기상효과가 작동중인가
	CTimer m_Timer;		// 다용도 타이머
	xtType m_Type;		// 현재 기상효과 종류
	int m_State;
public:
	XWeatherController() { Init(); }
	virtual ~XWeatherController() { Destroy(); }
	//
	virtual void InitSnow( int maxParticle );
	virtual void InitRain( int maxParticle );
	virtual void FrameMove( float dt );
};

void XWeatherController::InitSnow( int maxParticle )
{
	m_Type = xSNOW;
	m_bEnable = TRUE;
	XParticleSnow::s_pPool = new XPool<XParticleSnow>( 10000 );		// 눈 파티클 메모리풀 생성.
}

void XWeatherController::InitRain( int maxParticle )
{
	m_Type = xRAIN;
	m_bEnable = TRUE;
	m_Timer.Set( 60.f );		// xx초간 기상효과 ON
	m_State = 0;				// 상태 초기화
}

void XWeatherController::FrameMove( float dt )
{
	if( m_bEnable )
	{
		if( m_Type == xSNOW )
		{
			float x = (float)xRandom( (int)GetsizeWorld().w );
			PARTICLE_MNG->Add( new XCivParticleSnow( x, 0, CIV->GetpsfcSnow(), 0.09375f ) );
		} else
		if( m_Type == xRAIN )
		{
		}
	}
}

// 날씨 컨트롤러. XMap에 연동되는 버전
class XWeatherControllerMap : public XWeatherController
{
	void Init() {
	}
	void Destroy() {}
public:
	XWeatherControllerMap() { Init(); }
	virtual ~XWeatherControllerMap() { Destroy(); }
	//
	virtual void InitSnow( int maxParticle ) {
		XWeatherController::InitSnow();
	}
};

class XCivWeatherControllerMap : public XWeatherControllerMap
{
	void Init() {
	}
	void Destroy() {}
public:
	XCivWeatherControllerMap() { Init(); 	}
	virtual ~XCivWeatherControllerMap() { Destroy(); }
	//
	virtual void InitSnow( int maxParticle );
	virtual void InitRain( int maxParticle );
};

void XCivWeatherControllerMap::InitSnow( int maxParticle ) 
{
	XCivParticleSnow::s_prefMap = this;
	XWeatherControllerMap::InitSnow( maxParticle );
}

void XCivWeatherControllerMap::InitRain( int maxParticle )
{
}
*/
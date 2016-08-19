#pragma once
#include "XList.h"
#include "XPool.h"
#include "Timer.h"
/*
  * ��ų����Ʈ, ��, ��
  * �������� �ؽ����� ��ƼŬ���� �Բ� ���� �ϹǷ� �ؽ���id�� ��Ʈ�� �ؼ� �����̷� glDraw�� �ҷ��� �Ѵ�.
  * ��ų�� ��� ���� �پ��� ��ƼŬ�ؽ��İ� ���ǹǷ� ��ƼŬ �Ŵ����ȿ��� �پ��� �ؽ��İ� ���ϼ� �ֵ��� �ؾ� �Ѵ�.
  * ��ƼŬ ������ FrameMoveó�� ���
    1. ��ƼŬ �ϳ��� ��ü�� �����ؼ� ��ӹ޾� ó���Ѵ� ��ƼŬ �Ŵ����� 1���� �̵� ��θ� ó���Ѵ�.
	  ����: ���谡 ��������. �������̴�. ��ƼŬ ������ Ư¡�� ������ ���� ��������.
	  ����: ��ƼŬ �Ŵ������� ��� ��ƼŬ�� virtual FrameMove()�� ȣ���ؾ� �Ѵ�.
	2. ��ƼŬ �ϳ��� struct���·� �����ϰ�. ��ƼŬ�� �������� �Ŵ����� ���� �����Ѵ�.
	   ��ƼŬ �������� struct�� ���ܾ� �Ѵ�. struct XPARTICLE_SNOW, XPARTICLE_SPARK �̷���.
	  ����: ��ƼŬ �Ŵ��� ������ ��ƼŬ struct�� ������ ���� �ǹǷ� FrameMove()�� ���ϰ� ��������.
	  ����: ��ƼŬ �������� struct�� ��� ���ܾ� �Ѵ�. x, y, size, color���� ������ ���� ������ �Ѵ�.
			 ��ƼŬ �������� �Ŵ����� ���ܾ� �Ѵ�. class XParticleMngSnow ���...
	    . �ذ�å: ���. XPARTICLE_SNOW : public BASE_PARTICLE

  * �ϴ��� 1�������� �������� ��������. �غ��� �ӵ��� �������� �κ��� ������ 2������ ����ȭ�� ����� ����.
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
	// ���� ��갪
	float m_r, m_g, m_b, m_a;
	//
	XE::VEC2 m_vPos;
	float m_Scale;		// ���� �ؽ��� �̹����� ���� �����ϰ�. �̰����� m_Size���� ��������.
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
//		m_idTexture = (DWORD)psfcTexture;		// �ϴ��� ���� �����ϱ�. ���ǽ������͸� ���̵�ȭ�ؼ� ����.
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
	// x, y�� ��ũ����ǥ��. ȭ��ȿ� �ִ� ��ǥ�� call�̵ȴ�.
/*	virtual inline BOOL Draw( float x, float y, xOUT *pOut ) {
		pOut->x = x;
		pOut->y = y;
		pOut->size = m_Size;		// ���� �ٲ��� ������ ���� ������ ���� ������ �ϸ� ���?
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

// ����Ʈ ��������Ʈ ��ƼŬ
class XPointSpriteParticle : public XBaseParticle//, public XMemPool<XPointSpriteParticle>
{
public:
	struct xOUT {			// ����Ʈ ��������Ʈ ����̽� ������ ����ü
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
	// x, y�� ��ũ����ǥ��. ȭ��ȿ� �ִ� ��ǥ�� call�̵ȴ�.
	virtual inline BOOL Draw( float x, float y, xOUT *pOut ) {
		pOut->x = x;
		pOut->y = y;
		pOut->size = m_Size;		// ���� �ٲ��� ������ ���� ������ ���� ������ �ϸ� ���?
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
	CTimer m_timerLife;			// �ٴڿ� �������� �� ���� Ÿ�̸�
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
	BOOL m_bEnable;	// ���ȿ���� �۵����ΰ�
	CTimer m_Timer;		// �ٿ뵵 Ÿ�̸�
	xtType m_Type;		// ���� ���ȿ�� ����
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
	XParticleSnow::s_pPool = new XPool<XParticleSnow>( 10000 );		// �� ��ƼŬ �޸�Ǯ ����.
}

void XWeatherController::InitRain( int maxParticle )
{
	m_Type = xRAIN;
	m_bEnable = TRUE;
	m_Timer.Set( 60.f );		// xx�ʰ� ���ȿ�� ON
	m_State = 0;				// ���� �ʱ�ȭ
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

// ���� ��Ʈ�ѷ�. XMap�� �����Ǵ� ����
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
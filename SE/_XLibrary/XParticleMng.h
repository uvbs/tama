#pragma once
#include "XList.h"
#include "XPool.h"
#include "Timer.h"
#include "XParticle.h"

// ��ƼŬ �Ŵ��� �⺻��
class XBaseParticleMng
{
	void Init() {
        m_MaxParticle = 0;
	}
	void Destroy() {	}
protected:
	XList2<XBaseParticle> m_listParticle;
//	list<XBaseParticle*> m_listParticle;
    int m_MaxParticle;
public:
	XBaseParticleMng( int maxParticle ) { 
		Init(); 
		m_listParticle.Create( maxParticle );		// ��ƼŬ �����͸� maxParticle��ŭ �����.
        m_MaxParticle = maxParticle;
	}
	virtual ~XBaseParticleMng() { Destroy(); }
	//
	XBaseParticle* Add( XBaseParticle *pParticle ) {
		if( pParticle == NULL )
			return NULL;
		m_listParticle.Add( pParticle );
//		m_listParticle.push_back( pParticle );
		return pParticle;
	}
	void DestroyAll( void );
	//
	virtual void FrameMove( float dt );
	virtual int Draw( void )=0;
};
//////////////////////////////////////////////////////////////////////////////////////
// ����Ʈ ��������Ʈ�� ��ƼŬ �Ŵ���
class XPointSpriteParticleMng : public XBaseParticleMng
{
public:
private:
	void Init() {
		m_pBuffer = NULL;
	}
	void Destroy() {
		SAFE_DELETE_ARRAY( m_pBuffer );
	}
protected:
	XPointSpriteParticle::xOUT *m_pBuffer;
public:
	XPointSpriteParticleMng( int maxParticle ) : XBaseParticleMng( maxParticle ) { 
		Init(); 
		m_pBuffer = new XPointSpriteParticle::xOUT[ maxParticle ];		// ����Ʈ ��������Ʈ�� draw����
	}
	virtual ~XPointSpriteParticleMng() { Destroy(); }
	//
	XPointSpriteParticle* Add( XPointSpriteParticle *pParticle ) {
		XBaseParticleMng::Add( pParticle );
		return pParticle;
	}
	virtual int Draw( void );
};
//////////////////////////////////////////////////////////////////////////////////////
class XParticleMngRain : public XBaseParticleMng
{
	void Init() {
		XCLEAR_ARRAY( m_pBuffer );
		m_bEnable = FALSE;
		m_State = 0;
	}
	void Destroy() {
	}
protected:
	XGraphics::xVERTEX m_pBuffer[ 100 ];	// ���ؽ�100�� �� ���� 50���� �ִ�� �׸��� �ִ�.
	// weather
	CTimer m_timerWeather;					// �ֱ������� ��/�� �Դ� �ȿԴٸ� �ϱ�����..
	CTimer m_timerFog;
	BOOL m_bEnable;		// ���ȿ���� �۵����ΰ�
	int m_State;
public:
	XParticleMngRain( int maxRain ) : XBaseParticleMng( maxRain ) { Init(); }
	virtual ~XParticleMngRain() { Destroy(); }
	//
	GET_SET_ACCESSOR( int, State );
	GET_SET_ACCESSOR( BOOL, bEnable );
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


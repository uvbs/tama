/*
 *  _XParticleD3D.h
 *  Game
 *
 *  Created by xuzhu on 11. 3. 28..
 *  Copyright 2011 LINKS CO.,LTD. All rights reserved.
 *
 */

#pragma once
#include "containerSTL.h"
#include "SprDat.h"
#include "Timer.h"
#include "XList.h"
#include "XPool.h"
#include "SprMng.h"
/*
	XPool<_XParticle> *m_pPoolParticle;	// 스트럭트 형태로 넣으면 내부적으로 포인터 리스트로 관리한다
	m_pPoolParticle = new XPool( 10000 );

	pPool = m_pPoolParticle->GetEmptyPool();
	pPool->x = x;
	pPool->y = y;
	m_pPoolParticle->Add( pPool );
*/

class _XParticle;
class _XParticle
{
public:
	static XPool<_XParticle>* s_pPool;
	void* operator new( size_t size )	{
		return s_pPool->NewNode();
	}
	void operator delete( void *p ) {
		s_pPool->Delete( (_XParticle*)p );
	}
	// device elem
	XE::VEC2 m_vPrev;		// 이전 좌표
	XE::VEC2 m_vPrevDelta;
	float m_x, m_y;
	float m_size;
	float m_r, m_g, m_b, m_a;
	// member
	float m_dx, m_dy;
	DWORD m_idTexture;	// 텍스쳐 고유 아이디
	CTimer m_timerLife;	// 살아있는 시간
	BOOL m_bDestroy;
private:
	void Init() {
		m_x = m_y = 0;
		m_size = 1.0f;
		m_r = m_g = m_b = m_a = 1.0f;
		m_dx = m_dy = 0;
		m_idTexture = 0;
		m_bDestroy = FALSE;
	}
	void Destroy() {}
public:
	_XParticle() { Init(); }
	_XParticle( float x, float y, float dx, float dy, float size, DWORD idTexture, float secLife, XCOLOR col ) { 
		Init(); 
		m_x = x;		m_y = y;
		m_dx = dx;	m_dy = dy;
		m_size = size;
		m_r = XCOLOR_RGB_R( col ) / 255.f;
		m_g = XCOLOR_RGB_G( col ) / 255.f;
		m_b = XCOLOR_RGB_B( col ) / 255.f;
		m_a = XCOLOR_RGB_A( col ) / 255.f;
		if( secLife > 0 )
			m_timerLife.Set( secLife );
	}
	virtual ~_XParticle() { Destroy(); }
	void Create( void ) {}
	GET_SET_ACCESSOR( float, x );
	GET_SET_ACCESSOR( float, y );
	XE::VEC2 GetvPos( void ) { return XE::VEC2( m_x, m_y ); }
	GET_SET_ACCESSOR( float, dx );
	GET_SET_ACCESSOR( float, dy );
	GET_SET_ACCESSOR( XE::VEC2&, vPrev );
	GET_SET_ACCESSOR( XE::VEC2&, vPrevDelta );
	GET_ACCESSOR( float, size );
	GET_SET_ACCESSOR( DWORD, idTexture );
	GET_ACCESSOR( CTimer&, timerLife );
	void SetDestroy( BOOL bDestroy ) { m_bDestroy = bDestroy; }
	BOOL IsLive( void ) { return !m_bDestroy; }

};

#define MAX_PARTICLE	10000
class _XParticleMng
{
private:
	void Init() {
		m_State = 0;
		m_bEnable = 0;
	}
	void Destroy() {
		SAFE_DELETE( _XParticle::s_pPool );
	}
protected:
	XList2<_XParticle> m_listParticle;
	int m_State;
public:
	BOOL m_bEnable;
	_XParticleMng( void ) { 
		Init(); 
		m_listParticle.Create( MAX_PARTICLE );
		_XParticle::s_pPool = new XPool<_XParticle>( MAX_PARTICLE );
	}
	virtual ~_XParticleMng() { Destroy(); }
	// get/set
	//
	_XParticle* Add( const XE::VEC2& vPos, float dx, float dy, DWORD idTexture, float secLife=1.0f, float size=1.0f ) { 
		return Add( vPos.x, vPos.y, dx, dy, idTexture, secLife, size ); 
	}
	_XParticle* Add( float x, float y, float dx, float dy, DWORD idTexture, float secLife=1.0f, float size=1.0f );
//	void GetNextClear( void ) { m_poolParticle.GetNextClear(); }
	_XParticle* GetNext( int *pItorIdx ) { return m_listParticle.GetNext( pItorIdx ); }
	void Sort( int (*ptFuncCompare)( const void *p1, const void *p2 ) ) {
		m_listParticle.Sort( ptFuncCompare );
	}
	GET_SET_ACCESSOR( int, State );
	// virtual
	virtual void FrameMove( float dt );
	virtual void Draw( void ) = 0;
	virtual XE::VEC2 Projection( const XE::VEC2& vPos ) { return vPos; }
};

// 파티클을 XSurface로 그리는 버전
class _XParticleMngSprite : public _XParticleMng
{
	XSprDat *m_pSprDat;
	void Init() {
		m_pSprDat = NULL;
	}
	void Destroy() {
		SAFE_RELEASE2( SPRMNG, m_pSprDat );
	}
public:
	_XParticleMngSprite( LPCTSTR szSpr ) { 
		Init(); 
		m_pSprDat = SPRMNG->Load( szSpr );
	}
	virtual ~_XParticleMngSprite() { Destroy(); }
	//
	GET_SET_ACCESSOR( XSprDat*, pSprDat );
	
	virtual void Draw( void );
};

// 눈 파티클 매니저(범용)
class _XParticleMngSnow : public _XParticleMng
{
	void Init() {}
public:
	_XParticleMngSnow() { Init(); }
	virtual ~_XParticleMngSnow() {}
	//
	virtual void FrameMove( float dt );
	virtual void Draw( void );
	virtual BOOL ProcessCollision( _XParticle *pNode ) { return FALSE; }
	virtual void OnCollision( _XParticle *pNode ) {};
};

class XMapCiv;
class XCivParticleMngSnow : public _XParticleMngSnow
{
	XMapCiv *m_prefMap;
	void Init() {
		m_prefMap = NULL;
	}
public:
	XCivParticleMngSnow( XMapCiv *pMap ) {
		Init();
		m_prefMap = pMap;
	}
	virtual ~XCivParticleMngSnow() {}

	virtual XE::VEC2 Projection( const XE::VEC2& vPos );
	virtual BOOL ProcessCollision( _XParticle *pNode );
	virtual void OnCollision( _XParticle *pNode );
};

// 비 파티클 매니저(범용)
class _XParticleMngRain : public _XParticleMng
{
	void Init() {
	}
public:
	CTimer m_timerFog;
	_XParticleMngRain() { Init(); }
	virtual ~_XParticleMngRain() {}
	//
	virtual void FrameMove( float dt )=0;
	virtual void Draw( void );
	virtual BOOL ProcessCollision( _XParticle *pNode ) { return FALSE; }
	virtual void OnCollision( _XParticle *pNode ) {};
};

class XMapCiv;
class XCivParticleMngRain : public _XParticleMngRain
{
	XMapCiv *m_prefMap;
	void Init() {
		m_prefMap = NULL;
	}
public:
	XCivParticleMngRain( XMapCiv *pMap ) {
		Init();
		m_prefMap = pMap;
	}
	virtual ~XCivParticleMngRain() {}

	virtual void FrameMove( float dt );
	virtual XE::VEC2 Projection( const XE::VEC2& vPos );
	virtual BOOL ProcessCollision( _XParticle *pNode );
	virtual void OnCollision( _XParticle *pNode );
};

#pragma once
#include "XList.h"
#include "XPool.h"
#include "etc/Timer.h"
#include "etc/xGraphics.h"
#include "XFramework/client/XPropParticle.h"
#include "etc/XGraphicsDef.h"
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
class XParticleMngRain;

class XParticleDelegate
{
	void Init() {}
	void Destroy() {}
public:
	XParticleDelegate() { Init(); }
	virtual ~XParticleDelegate() { Destroy(); }
	// 좌표가 뷰포트안에 있는지 검사한다.
	virtual BOOL OnDelegateIsInScreen( float x, float y ) { return TRUE; };
	virtual bool OnDelegateIsInScreen( const XE::VEC2& vScr ) { return true; };
	virtual inline BOOL OnDelegateIsCollision( XBaseParticle *pParticle, float x, float y, float *pOutyColl ) { 
		return ( y >= XE::GetGameHeight() )? TRUE : FALSE;  
	}
	virtual inline void OnDelegateProjection( const XE::VEC3& vPos, float *pOutx, float *pOuty ) {
		*pOutx = vPos.x;
		*pOuty = vPos.y;
	}
	// 월드좌표 vPos를 2d뷰포트좌표로 변환한다.
	virtual inline XE::VEC2 OnDelegateProjection2( const XE::VEC3& vPos ) {
		// 파티클의 좌표계는 디폴트가 3d이므로 디폴트 프로젝션은 2d로 변환한다.
		// z가 높이좌표다. (-)가 위쪽, (+)가 아래쪽.
		return XE::VEC2( vPos.x, vPos.y + vPos.z );
	}
	/// 뷰포트의 스케일값을 얻는다.
	virtual inline float OnDelegateGetScaleViewport() { return 1.f; }
	// 비가내릴 영역의 시작위치를 요구한다.
	virtual XE::VEC2 OnDelegateGetRainAreaLT( void ) { return XE::VEC2(0); }
	virtual BOOL OnDelegateIsDestroy( XBaseParticle *pParticle ) { return FALSE; }
	// 눈비가 떨어질때 미리 바닥의 위치를 갖고 있을수있다.
	virtual XE::VEC2 OnDelegateGroundPos( XBaseParticle *pParticle ) { return XE::VEC2(0, XE::GetGameHeight()); }
};

XE_NAMESPACE_START( xParticle )
/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/09 17:19
*****************************************************************/
class XCompFunc
{
	void Init() {}
	void Destroy() {}
public:
	XCompFunc() { Init(); }
	virtual ~XCompFunc() { Destroy(); }
	//
	virtual float GetValue() = 0;
	virtual float GetValue( float lerpTime ) { return 1.f; }
}; // class XCompFunc

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/09 17:20
*****************************************************************/
class XCompFuncFixed : public XCompFunc
{
	float m_Func = 0.f;
	void Init() {}
	void Destroy() {}
public:
	XCompFuncFixed( float Func ) {
		Init();
		m_Func = Func;
	}
	virtual ~XCompFuncFixed() { Destroy(); }
	//
	float GetValue() override {
		return m_Func;
	}
}; // class XCompFuncFixed

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/09 18:32
*****************************************************************/
class XCompFuncRandom : public XCompFunc
{
	float m_Func1 = 0.f;
	float m_Func2 = 0.f;
	void Init() {}
	void Destroy() {}
public:
	XCompFuncRandom( float Func1, float Func2 ) {
		Init();
		m_Func1 = Func1;
		m_Func2 = Func2;
	}
	virtual ~XCompFuncRandom() { Destroy(); }
	//
	float GetValue() override {
		return xRandomF( m_Func1, m_Func2 );
	}
}; // class XCompFuncRandom
/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/09 18:40
*****************************************************************/
class XCompFuncLinear : public XCompFunc
{
	float m_Func1 = 0.f;
	float m_Func2 = 0.f;
	CTimer m_timerCycle;
	void Init() {}
	void Destroy() {}
public:
	XCompFuncLinear( float Func1, float Func2, float secCycle ) {
		Init();
		m_Func1 = Func1;
		m_Func2 = Func2;
		m_timerCycle.Set( secCycle );
	}
	virtual ~XCompFuncLinear() { Destroy(); }
	//
	float GetValue() override {
		float timeLerp = m_timerCycle.GetSlerp();
		if( timeLerp > 1.f )
			timeLerp = 1.f;
		float lerp = XE::xiLinearLerp( timeLerp );
		float Func = m_Func1 + lerp * ( m_Func2 - m_Func1 );
		if( m_timerCycle.IsOver() )
			m_timerCycle.Reset();
		return Func;
	}
	float GetValue( float lerpTime ) override {
		float lerp = XE::xiLinearLerp( lerpTime );
		float Func = m_Func1 + lerp * ( m_Func2 - m_Func1 );
		if( m_timerCycle.IsOver() )
			m_timerCycle.Reset();
		return Func;
	}
}; // class XCompFuncLinear

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/06/09 18:40
*****************************************************************/
class XCompFuncSin : public XCompFunc
{
	float m_Func1 = 0.f;
	float m_Func2 = 0.f;
	CTimer m_timerCycle;
	void Init() {}
	void Destroy() {}
public:
	XCompFuncSin( float Func1, float Func2, float secCycle ) {
		Init();
		m_Func1 = Func1;
		m_Func2 = Func2;
		m_timerCycle.Set( secCycle );
	}
	virtual ~XCompFuncSin() { Destroy(); }
	//
	float GetValue() override {
		float timeLerp = m_timerCycle.GetSlerp();
		if( timeLerp > 1.f )
			timeLerp = 1.f;
		float lerp = XE::xiSin( timeLerp, m_Func1, m_Func2, 0 );
		if( m_timerCycle.IsOver() )
			m_timerCycle.Reset();
		return lerp;
	}
	float GetValue( float lerpTime ) override {
		float lerp = XE::xiSin( lerpTime, m_Func1, m_Func2, 0 );
		if( m_timerCycle.IsOver() )
			m_timerCycle.Reset();
		return lerp;
	}
}; // class XCompFuncSin

XE_NAMESPACE_END;

//////////////////////////////////////////////////////////////////////////
class XBaseParticle// : public XMemPool<XBaseParticle>
{
public:
private:
	XParticleDelegate *m_pDelegate;
	int m_Type;			// 델리게이트를 위한 파티클 식별타입
	void Init() {
		m_Type = 0;
		m_r = m_g = m_b = m_a = 1.0f;
		m_idTexture = 0;
		m_bDestroy = FALSE;
		m_Scale = 1.0f;
		m_pDelegate = NULL;
		m_aryComponents.resize( xParticle::xIC_MAX );
	}
	void Destroy();
protected:
	std::vector<xParticle::XCompFunc*> m_aryComponents;
	// 사전 계산값
	float m_r, m_g, m_b, m_a;
	//
	XE::VEC3 m_vPos;
	float m_Scale;		// 원래 텍스쳐 이미지에 대한 스케일값. 이값으로 m_Size값이 정해진다.
	XE::VEC3 m_vDelta;	// 이동벡터(방향과 속도가 녹아있다)
	ID m_idTexture;
	BOOL m_bDestroy;
	CTimer m_timerLife;		// 생존시간.
	XE::xtBlendFunc m_BlendFunc;
public:
	XBaseParticle( XParticleDelegate *pDelegate, 
								int type, 
								const XE::VEC3& vPos, 
								DWORD idTexture, 
								float scale, 
								XCOLOR col );
	virtual ~XBaseParticle() { Destroy(); }
	//
	GET_ACCESSOR( const XE::VEC3&, vPos );
	GET_SET_ACCESSOR( const XE::VEC3&, vDelta );
	GET_ACCESSOR( DWORD, idTexture );
	GET_ACCESSOR( XParticleDelegate*, pDelegate );
	GET_SET_ACCESSOR( BOOL, bDestroy );
	BOOL IsDestroy( void ) { return m_bDestroy; }
	GET_SET_ACCESSOR( XE::xtBlendFunc, BlendFunc );
	GET_ACCESSOR( int, Type );
	GET_ACCESSOR( float, Scale );
	GET_ACCESSOR( float, r );
	GET_ACCESSOR( float, g );
	GET_ACCESSOR( float, b );
	GET_ACCESSOR( float, a );
	void SetsecLife( float secLife ) {
		m_timerLife.Set( secLife );
	}
	// virtual
	virtual void FrameMove( float dt );
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
	virtual inline void Projection( const XE::VEC3& vPos, float *pOutx, float *pOuty ) {
		if( m_pDelegate )
			return m_pDelegate->OnDelegateProjection( vPos, pOutx, pOuty );
		*pOutx = vPos.x;
		*pOuty = vPos.y;
	}
	virtual BOOL IsInScreen( float x, float y ) { return true; }
	virtual inline BOOL IsCollision( float x, float y, float *pOutyColl ) { 
		if( m_pDelegate )
			return m_pDelegate->OnDelegateIsCollision( this, x, y, pOutyColl );
		return ( y >= XE::GetGameHeight() )? TRUE : FALSE;  
	}
	virtual void OnCollision(  float x, float y ) {}
	void AddComponent( xParticle::xtIdxComp idxComp, xParticle::XCompFunc *pComp ) {
		m_aryComponents[ idxComp ] = pComp;
	}
	xParticle::XCompFunc* GetComponent( xParticle::xtIdxComp idxComp ) {
		return m_aryComponents[ idxComp ];
	}
// 	void AddvPos( const XE::VEC3& vDelta ) {
// 		m_vPos += vDelta;
// 	}

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
	XPointSpriteParticle( XParticleDelegate *pDelegate, int type, const XE::VEC3& vPos, XSurface *psfcTexture, float scale, XCOLOR col );
	XPointSpriteParticle( XParticleDelegate *pDelegate,
												int type,
												const XE::VEC3& vPos,
												XSurface *psfcTexture,
												ID idTexture,
												float scale,
												XCOLOR col );
	virtual ~XPointSpriteParticle() {}
	//
	GET_ACCESSOR( float, Size );
	GET_ACCESSOR( XSurface*, psfcTexture );
	// x, y는 스크린좌표다. 화면안에 있는 좌표만 call이된다.
	virtual inline BOOL Draw( float x, float y, xOUT *pOut ) {
		pOut->x = x;
		pOut->y = y;
		pOut->size = m_Size * m_Scale;		// 값이 바뀌지 않은건 굳이 재저장 하지 말도록 하면 어떨까?
		pOut->r = m_r;
		pOut->g = m_g;
		pOut->b = m_b;
		pOut->a = m_a;
		return TRUE;
	}
};

/****************************************************************
* @brief 파티클입자가 스프라이트 애니메이션으로 되어있는 파티클
* @author xuzhu
* @date	2015/06/09 19:02
*****************************************************************/
class XSprObj;
class XParticleSprite : public XBaseParticle
{
	XSprObj *m_pSprObj = nullptr;		// 스프라이트 애니메이션이 있는 spr객체
	void Init() {}
	void Destroy();
public:
	XParticleSprite( LPCTSTR szSpr, ID idAct, const XE::VEC3& vPos, float scale = 1.f );
	XParticleSprite( const _tstring& strSpr, ID idAct, const XE::VEC3& vPos, float scale = 1.f ) 
		: XParticleSprite( strSpr.c_str(), idAct, vPos, scale ) {}
	virtual ~XParticleSprite() { Destroy(); }
	//
	XE::VEC2 GetSize() {
		return XE::VEC2(0);	// 아직 사이즈 처리 안함.
	}
	//
	void FrameMove( float dt ) override;
	void Draw( const XE::VEC2& vScr, float scaleParent );
}; // class XParticleSprite

//////////////////////////////////////////////////////////////////////////
#if 0
class XParticleSnow : public XPointSpriteParticle//, public XMemPool<XParticleSnow>
{
	float m_yGround;
	void Init() {
		m_yGround = 0;
	}
protected:
	CTimer m_timerLife;			// 바닥에 떨어지고 난 후의 타이머
public:
	XParticleSnow( XParticleDelegate *pDelegate, const XE::VEC3& vPos, XSurface *psfcSnow, float scale );
	virtual ~XParticleSnow() {}
	// virtual
	virtual void FrameMove( float dt );
	virtual BOOL Draw( float x, float y, xOUT *pOut );
	virtual void Projection( const XE::VEC3& vPos, float *pOutx, float *pOuty ) { XBaseParticle::Projection( vPos, pOutx, pOuty ); }
	virtual BOOL IsInScreen( float x, float y ) { return TRUE; }
	virtual void OnCollision( float x, float y );
};

class XParticleRain : public XBaseParticle//, public XMemPool<XParticleRain>
{
	XParticleMngRain *m_pRainMng;
	void Init() {
		m_pRainMng = NULL;
	}
protected:
	//	CTimer m_timerFog;
	XE::VEC2 m_vPrev, m_vPrevDelta;
public:
	XParticleRain( XParticleDelegate *pDelegate, XParticleMngRain *pRainMng, float x, float y, float dx, float dy );
	// virtual
	virtual void FrameMove( float dt );
	virtual BOOL Draw( float x, float y, XGraphics::xVERTEX *pOut );
	virtual void Projection( const XE::VEC3& vPos, float *pOutx, float *pOuty ) { XBaseParticle::Projection( vPos, pOutx, pOuty ); }
	virtual BOOL IsInScreen( float x, float y ) {
		if( GetpDelegate() )
			return GetpDelegate()->OnDelegateIsInScreen( x, y );
		return TRUE;
	}
	virtual void OnCollision( float x, float y ) {}
	virtual BOOL IsCollision( float x, float y, float *pOutyColl );
};

#endif // 0
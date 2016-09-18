#pragma once
/*
 *  XTransition.h
 *  ShakeMP
 *
 *  Created by xuzhu on 10. 7. 2..
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "etc/XSurface.h"
#include "etc/Timer.h"
//--------------------------------------------------------------------------------------------------------- 
class XTransition
{
	BOOL m_bHighReso;	// 리소스를 고해상도로 쓸건지
	BOOL	m_bComplete;	// 트랜지션이 끝났나
	float m_fPer;			
	float m_fSlerp;			// 어디까지 보간되었는지
	float m_fSec;				// 속도. 몇초간 트랜지션이 일어날지...
	CTimer m_Timer;
	int m_nDrawCnt;		
	XSurface *m_pSurface1;
	XSurface *m_pSurface2;
	BOOL m_bDestroy;
	BOOL m_nTransIn;
protected:
	SET_ACCESSOR( float, fSec );
	SET_ACCESSOR( XSurface*, pSurface1 );
	SET_ACCESSOR( XSurface*, pSurface2 );
public:
	XTransition( BOOL bHighReso, float fSec, BOOL bTransIn ) { 
		m_bHighReso = bHighReso; 
		m_bComplete = FALSE; 
		m_fPer = 0;
		m_fSlerp = 0; 
		m_fSec = fSec;
		m_pSurface1 = m_pSurface2 = NULL;
		m_nDrawCnt = 0;
		m_bDestroy = FALSE;
		m_nTransIn = bTransIn;
	}
	virtual ~XTransition();
	//
	GET_ACCESSOR( BOOL, bHighReso );
	GET_SET_ACCESSOR( float, fSlerp );
	GET_ACCESSOR( float, fSec );
	GET_ACCESSOR( CTimer&, Timer );
	GET_ACCESSOR( XSurface*, pSurface1 );
	GET_ACCESSOR( XSurface*, pSurface2 );
	GET_ACCESSOR( int, nDrawCnt );
	GET_SET_ACCESSOR( BOOL, bDestroy );
	BOOL    IsComplete(){ return m_bComplete;}
	void SetComplete( BOOL bComplete ) { m_bComplete = bComplete; }
	BOOL IsTransIn( void ) { return m_nTransIn; }

	// virtual
	virtual BOOL Create( void )=0;	// 스크린에서 픽셀얻어오는건 반드시 Create()에서 해야한다. 오버랩같은건 씬을 한번 그리고 다음턴에서 그화면을 얻어와야 하기때문에 생성자에서 스크린픽셀을 잡으면 안되기 때문이다
	virtual BOOL Process( float dt );
	virtual void Draw( void ) { 
		if( m_nDrawCnt == 0 ) 
			Create();		// 최초draw시에 현재 백버퍼에 그려진 새씬의 화면을 캡쳐함
		m_nDrawCnt ++; 
	}	
	virtual void Update( float dt ){ Process(dt); Draw(); }
};

class XFadeInOut : public XTransition
{
	void Init() {
	}
	void Destroy() {
	}
public:
	XFadeInOut( BOOL bHighReso, BOOL bTransIn, float fSec );
	virtual ~XFadeInOut() { Destroy(); }
	//
	//
	virtual BOOL Create( void ) {return 1;}
	virtual void Draw( void );

};
class XSlideInOut : public XTransition
{
	int m_nType;
	int m_nSpeed;
	int m_nXPos;
public:
	XSlideInOut( BOOL bHighReso, XSurface *pFrom, int nTransIn = TRUE, int nSpeed=20 );
	virtual ~XSlideInOut() {}
	
	virtual BOOL Create( void );
	virtual void Draw( void );
	
};
//-------------------------------------------------------------------------------------
// 사용법:
// d3d
//		new XOverlap이 되는시기는 XGraphicsD3D::s_pCaptureBackBuffer에 이전화면이 저장되어있는 상태에서 호출되어야 한다
// opengl
//		gl은 프론트버퍼를 읽을수 있으므로 프론트화면에 이전화면이 나타나있을때 호출되어야 한다.
class XOverlap : public XTransition
{
	TCHAR m_szImgTo[1024];
	void Init() { XCLEAR_ARRAY( m_szImgTo );	}
	void Destroy() {	}
public:
	XOverlap( BOOL bHighReso, LPCTSTR szImgTo, float fSec );
	XOverlap( BOOL bHighReso, float fSec );
	virtual ~XOverlap() { Destroy(); }

	virtual BOOL Create( void );
	virtual void Draw( void );
};



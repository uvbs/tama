#pragma once
#include "XFramework/XEBaseScene.h"

class XEContent;
class XESceneSlide : public XEBaseScene
{
private:
	//
	CTimer m_timerSpeed;		// 로딩속도 테스트용 타이머
	DWORD m_Param[2];
	void Init()  {
		m_pCurr = NULL;
		m_pNext = NULL;
		m_Trans = xTR_NONE;
		m_idxScene = 0;
		m_timeLoad = 0;
		XCLEAR_ARRAY( m_Param );
	}
	void Destroy();
	int m_idxScene;			// 현재 씬 인덱스
protected:
	enum xtTrans { xTR_NONE,
						xTR_LEFT,			// 현재씬이 왼쪽으로 사라지고 다음씬이 오른쪽에서 나타남.
						xTR_RIGHT,
	};
	CTimer m_timerTrans;
	XWnd *m_pCurr;
	XWnd *m_pNext;
	xtTrans m_Trans;
	DWORD m_timeLoad;		// 현재씬 로딩타임
	DWORD GetParam( int idx ) {
		XBREAK( idx < 0 || idx >= XNUM_ARRAY(m_Param) );
		return m_Param[ idx ];
	}
public:
	XESceneSlide( XEContent *pGame, ID idScene );
	virtual ~XESceneSlide(void) { Destroy(); }
	//
//	BOOL IsSliding( void ) {		// 화면이 슬라이딩 중인가.
//		return m_Trans != xTR_NONE;
//	}
//	GET_ACCESSOR( int, idxScene );
	SET_ACCESSOR( int, idxScene );
	int GetidxScene( void ) {
		XBREAK( m_idxScene == -1 );
		return m_idxScene;
	}
	int AddidxScene( int add ) {
		m_idxScene += add;
		return m_idxScene;
	}
	//
	BOOL SetSlideLeft( int idx, DWORD p1=0, DWORD p2=0 );
	BOOL SetSlideRight( int idx, DWORD p1=0, DWORD p2=0 );
	// virtual
	virtual void Create( int idxStart=0 );
	virtual int Process( float dt );
	virtual void Draw( void );
	virtual void OnLButtonDown( float lx, float ly );
	virtual void OnLButtonUp( float lx, float ly );
	virtual void OnMouseMove( float lx, float ly );
	virtual XWnd* CreateScene( int idx, DWORD p1, DWORD p2 ) { return NULL; }
	virtual void OnFinishSlide( void ) {}
	virtual void OnCheatMode( void ) {}
	virtual BOOL OnKeyUp( int keyCode );
	virtual int OnBack( XWnd *pWnd, DWORD p1, DWORD p2 ) { return 1; }
};


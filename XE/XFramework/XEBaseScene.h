#pragma once
#include "_Wnd2/XWnd.h"
#include "XTransition.h"
#include "etc/Timer.h"
#include "XFramework/XESceneMng.h"

/**
 @brief 
 브릿지 씬의 정의
	다이나믹 브릿지씬.
	 병영씬에서 월드씬으로 넘어간다고 했을때 직관적인 설계는 로딩씬을 제외하면 편하다.
	 그래서 이벤트에 의해 씬과 씬사이를 이동할때는 로딩씬을 신경쓰지 않고 할수 있도록 지원해주고
	 로딩씬이 필요한씬만 별도로 플래그를 줘서 시스템이 자동으로 브릿지씬을 삽입시켜주고 브릿지씬이
	 끝나면 다음으로 갈 씬과 파라메터를 그대로 토스 해주는 방식으로 한다.
	 또한 브릿지 씬은 정적으로 설계된 씬 흐름사이에서 어떤 이벤트에 의해 동적 삽입될수도 있다.
	 브릿지씬은 스스로 종료하지 않고 이벤트에 의해서만 종료된다. 로딩씬으로 쓰일경우 다음씬의 로딩이 끝나면 이벤트를 받아 종료를시작(페이드아웃후 파괴)한다.
	 브릿지씬은 씬 Process중에 다음씬의 로딩을 하고 있을수도 있다(멀티스레드 로딩)
	 멀티스레드를 쓰지 않는 정적인 로딩씬이라면 브릿지씬이 한번 화면에 표시된후 다음씬을 생성해서 스스로를 종료시킨다.
	 로딩씬은 브릿지속성을 가지고 한단계 더 상속을 받는게 나을듯.
*/
class XEBaseScene;
class XEContent;
class XESceneMng;
class XRenderCmdMng;
class XTextureAtlas;
//
class XEBaseScene : public XWnd
{
private:
	XESceneMng *m_pSceneMng;
	ID m_idScene;			// 씬 아이디
	int m_Destroy;
	XTransition *m_pTransition;		// 트랜지션 효과 객체
	ID m_idNextScene;				// 씬이 파괴되어 빠져나갈때 다음에 실행되어야 할 씬의 아이디
	XEBaseScene *m_pParentScene;	// child지정된 씬의 경우 부모씬이 있음.
	XEBaseScene *m_pSceneChild;
	XEContent *m_pGame;
	XLua *m_pLua;
	bool m_bBridge = false;			/// 브릿지씬(로딩씬같은)속성이냐(위 설명참조)
	ID m_idBridgeNextScene = 0;		/// 이 씬이 끝날때 불리게 될 씬의 아이디
// 	XRenderCmdMng* m_pRenderer = nullptr;
	XTextureAtlas* m_pAtlas = nullptr;
#ifdef _DEBUG
	XE::VEC2 m_vMouse;
#endif
	//
	void Init()  {
		m_idScene = 0;
		m_Destroy = FALSE;
		m_pTransition = NULL;
		m_pSceneChild = NULL;
		m_idNextScene = 0;
		m_pParentScene = NULL;
		m_pGame = NULL;
		m_pLua = NULL;
		m_pSceneMng = NULL;
		SetID( XE::GenerateID() );		// 디폴트 윈도우 아이디. 생성자(XWnd::Add되기전)에서 m_id를 쓰는경우가 간혹있어서 걍 넣었음.

	}
	void Destroy();
	virtual void CallScriptImplement( XLua *pLua, const char *cFunc, DWORD p1, DWORD p2 );
protected:
	GET_ACCESSOR( XEBaseScene*, pParentScene );
public:
	XEBaseScene( XEContent *pGame, ID idScene, BOOL bTransition=TRUE );
	XEBaseScene( XEContent *pGame, const std::string& idsScene, bool bTransition );
	virtual ~XEBaseScene(void) { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( int, Destroy );
	GET_ACCESSOR_CONST( ID, idScene );
	GET_SET_ACCESSOR( XTransition*, pTransition );
	GET_SET_ACCESSOR( XESceneMng*, pSceneMng );
	GET_BOOL_ACCESSOR( bBridge );
	/// 브릿지 씬으로 설정한다. 브릿지씬은 자신이 끝나고 다음에 부를 씬의 번호를 기억하고 있어야 한다.
	void SetBridge( bool bBridge, ID idNextScene ) {
		m_bBridge = bBridge;
		if( bBridge ) {
			m_idBridgeNextScene = idNextScene;
		} else {
			m_idBridgeNextScene = 0;
		}
	}
	GET_ACCESSOR_CONST( ID, idBridgeNextScene );
	ID GetNextScene( void ) 	{
		if( m_pParentScene )
			return m_pParentScene->GetNextScene();
		return m_idNextScene;
	}
	void SetNextScene( ID idNextScene ) {
		if( m_pParentScene )
			m_pParentScene->SetNextScene( idNextScene );
		m_idNextScene = idNextScene;
	}
	BOOL IsExit( void ) {
		// 트랜지션이 있고 페이드 아웃 상태면 exit상태로 본다.
		if( m_pTransition && m_pTransition->IsTransIn() == FALSE )
			return TRUE;
		return FALSE;
	}
	bool IsTransition() {
		return m_pTransition != nullptr;
	}
	SET_ACCESSOR( XEBaseScene*, pParentScene );
	GET_ACCESSOR( XEBaseScene*, pSceneChild );
	GET_ACCESSOR( XLua*, pLua );
#ifdef _DEBUG
	GET_ACCESSOR( const XE::VEC2&, vMouse );
#endif
	XEBaseScene* SetChildScene( XEBaseScene *pChildScene ) {
		m_pSceneChild = pChildScene;
		pChildScene->SetpParentScene( this );
		Add( pChildScene );
		return pChildScene;
	}
	XEBaseScene* InsertChildScene( const char *cBaseKey, XEBaseScene *pChildScene ) {
		m_pSceneChild = pChildScene;
		pChildScene->SetpParentScene( this );
		Insert( cBaseKey, pChildScene );
		return pChildScene;
	}
	//
	void DrawTransition( void );
	void DoExit( ID idNextScene, SceneParamPtr spParam = SceneParamPtr() );
	// virtual
	// 앞으로 이거 쓰지말고 OnCreate를 쓸것
	virtual void Create( void ) {}
	virtual int Process( float dt ) override;
	virtual void Draw( void ) override;
	virtual void OnLButtonDown( float lx, float ly ) {
		if( IsTransition() )	// 씬이 전환중이면 입력 못받게 한다.
			return;
#ifdef _DEBUG
		m_vMouse.Set( lx, ly );
#endif
		XWnd::OnLButtonDown( lx, ly );
	}
	virtual void OnLButtonUp( float lx, float ly ) {
		if( IsTransition() )	// 씬이 전환중이면 입력 못받게 한다.
			return;
		XWnd::OnLButtonUp( lx, ly );
	}
	virtual void OnMouseMove( float lx, float ly ) {
		if( IsTransition() )	// 씬이 전환중이면 입력 못받게 한다.
			return;
#ifdef _DEBUG
		m_vMouse.Set( lx, ly );
#endif // _DEBUG
		XWnd::OnMouseMove( lx, ly );
	}
	virtual void OnRButtonUp( float lx, float ly ) {
		XWnd::OnRButtonUp( lx, ly );
	}
	virtual void OnRButtonDown( float lx, float ly ) {
		XWnd::OnRButtonDown( lx, ly );
	}
	virtual void OnEndTransition( XTransition *pTrans ) {}
	virtual void OnEndTransitionOut( XTransition *pTrans ) {}	// 씬 빠져나갈때.
	virtual void OnDestroySceneBefore() {}	// 씬 파괴되기 직전
private:
	virtual void OnEndTransitionIn( XTransition *pTrans ) final {		// 씬 들어올때
		OnEnterScene( nullptr, 0, 0 );
	}	
public:
	virtual void OnDidFinishCreate( void ) {		// 하위씬까지 로딩이 모두 끝났을때
//		m_timerEnter.Set( 1.0f );
	}
//	virtual void OnEnterScene( void );		// 씬이 모두 밝아지고 호출됨
	virtual int OnEnterScene( XWnd*, DWORD, DWORD );		// 씬이 모두 밝아지고 호출됨
	virtual void OnCheatMode( void ) {}
	virtual int OnBack( XWnd*, DWORD, DWORD ) { return 1;}
	BOOL OnKeyUp( int keyCode );
	void CallScript( const char *cFunc, DWORD p1=0, DWORD p2=0 );
	virtual void OnRecvPacket( ID idPacket ) {}
	virtual void OnReload() {}
	virtual void SetActive( bool bFlag ) {
		SetbActive( bFlag );
	}
	/// this가 브릿지 씬일경우 씬종료를 시작해야하는 시점에 핸들러가 호출된다.
	virtual void OnExitBridge();
	//
	void LuaSetActive( BOOL flag ) {
		SetbActive( flag );
	}
	ID LuaGetType( void ) {
		return m_idScene;
	}
	template<typename SCENE_TYPE, typename P1, typename P2>
	void CallScriptTemplate( const char *cFunc, P1 p1, P2 p2 ) {
		XLua *pLua = GetpLua();
		if( pLua == NULL )		return;
		if( pLua->IsHaveFunc( cFunc ) ) {
			pLua->Call<SCENE_TYPE,P1,P2>( cFunc, p1, p2 );
		}
	}
};


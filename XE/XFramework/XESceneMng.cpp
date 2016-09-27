#include "stdafx.h"
#include "XFramework/XESceneMng.h"
#include "XFramework/client/XEContent.h"
#include "XFramework/XEBaseScene.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XESceneMng::XESceneMng( XEContent *pGame, XEDelegateSceneMng *pDelegate )
{
	Init();
	m_pDelegate = pDelegate;
	m_pGame = pGame;
	XBREAK( m_pGame == NULL );
	XBREAK( m_pDelegate == NULL );

}

void XESceneMng::Destroy()
{
//	SAFE_DELETE( m_pParam );
}

void XESceneMng::Release()
{
	m_spParam.reset();
}

/**
 @brief 다음에 불려지게 될 씬을 생성만하고 씬리스트엔 올리지 않는다.
*/
XEBaseScene* XESceneMng::CreatePreNextScene( ID idNextScene, XSPSceneParam& spParam )
{
	if( XASSERT(m_pDelegate) ) {
		auto pScene = m_pDelegate->DelegateCreateScene( this, idNextScene, spParam );
		if( pScene ) {
			// pScene->Update()를 위해 id/ids/parent를 미리 등록시켜둠.
			pScene->SetID( XE::GenerateID() );		// 고유아이디 부여
			pScene->_SetpParent( m_pGame->GetpRootScene() );			
			pScene->SetpSceneMng( this );
			pScene->OnCreate();
			pScene->SetbUpdate( false );
			pScene->_Update();		// 최초 대부분의 리소스를 로딩.
			pScene->CallEventHandler( XWM_FINISH_LOADED );
			pScene->Create();		// 앞으로 이거 사용하지 말고 OnCreate를 사용할 것.
			pScene->OnDidFinishCreate();
			m_pGame->OnFinishLoadedByScene( pScene );
		}
//		m_pSceneNextPreCreated = pScene;
		return pScene;
	}
	return nullptr;
}

/**
 @brief 브릿지씬이 파괴되면 브릿지씬에서 갖고있던 미리로딩된 다음씬을 받아서 윈도우리스트에 정식등록시킨다.
*/
void XESceneMng::OnDestroyBridge( XEBaseScene *pScenePreCreated )
{
	XBREAK( pScenePreCreated == nullptr );
	// 미리 생성해둔 다음씬을 부모윈도우의 자식으로 정식 등록함.
	auto pParent = pScenePreCreated->GetpParent();
	// OnCreate등을 다시 부르지 않게하기 위해 AddOnly로 호출함.
	pParent->AddOnly( pScenePreCreated->getid(), pScenePreCreated );
	// 현재씬(브릿지)을 삭제
	DestroyCurrScene();
	m_pScene = pScenePreCreated;
}

/**
 @brief 현재씬을 삭제한다.
*/
void XESceneMng::DestroyCurrScene()
{
	if( m_pDelegate )
		m_pDelegate->DelegateOnDestroy( m_pScene );
	ID idScene = m_pScene->GetidScene();
	m_pGame->DestroyWnd( m_pScene );
	if( m_pDelegate )
		m_pDelegate->DelegateOnDestroyAfter( idScene, m_idNextScene, m_spParam );
	m_pScene = nullptr;
	if( m_pGame->GetpLua() )
		m_pGame->GetpLua()->Call<void, ID, ID>( "OnEventFinishScene", 
												m_idLastScene, 
												m_idNextScene );
}
/**
 @brief 
*/
int XESceneMng::Process( float dt )
{
	if( m_pScene == nullptr ) {
		if( m_pDelegate ) {
			m_pScene = m_pDelegate->DelegateCreateScene( this, m_idNextScene, m_spParam );
			if( m_pScene ) {
				m_pScene->PopAtalsMng();
				m_pScene->SetpSceneMng( this );
				m_pGame->GetpRootScene()->Add( m_pScene );
				m_pScene->Create();		// 앞으로 이거 사용하지 말고 OnCreate를 사용할 것.
				m_pScene->OnDidFinishCreate();
				m_pGame->OnFinishLoadedByScene( m_pScene );
			}
		}
	}
	// 페이드아웃이 끝나면 SetDestroy(1)로 세팅된다.
	if( m_pScene && m_pScene->GetDestroy() == 1 ) {
		const bool bBridge = m_pScene->GetbBridge();		// 파괴된씬이 브릿지씬이었나.
		m_idLastScene = m_pScene->GetidScene();
		m_idNextScene = m_pScene->GetNextScene();
		// m_pScene을 삭제한다.
		DestroyCurrScene();
// 		if( m_pDelegate )
// 			m_pDelegate->DelegateOnDestroy( m_pScene );
// 		m_pGame->DestroyWnd( m_pScene );
// 		m_pScene = NULL;
// 		if( m_pGame->GetpLua() )
// 			m_pGame->GetpLua()->Call<void, ID, ID>( "OnEventFinishScene", 
// 													m_idLastScene, 
// 													m_idNextScene );
		// 다음씬의 정보를 얻음.
		if( !bBridge ) {
			// 브릿지 씬일때만 체크함.
			const auto& infoNext = GetSceneInfo( m_idNextScene );
			if( infoNext.m_idScene && infoNext.m_idNeedBridgePrev ) {
				// 다음씬을 위해 로딩씬이 필요하다면
				if( XASSERT(m_pDelegate) ) {
					// 자동으로 브릿지씬을 생성해서 삽입한다.
					m_pScene = m_pDelegate->DelegateCreateScene( this, infoNext.m_idNeedBridgePrev, m_spParam );
					if( XASSERT(m_pScene) ) {
						m_pScene->SetBridge( true, m_idNextScene );		// 브릿지 속성을 줌.
						m_pScene->SetpSceneMng( this );
						m_pGame->GetpRootScene()->Add( m_pScene );
						m_pScene->Create();		// 앞으로 이거 사용하지 말고 OnCreate를 사용할 것.
						m_pScene->OnDidFinishCreate();
						m_pGame->OnFinishLoadedByScene( m_pScene );
					}
				}
			}
		}
	}
	return 1;
}

void XESceneMng::OnCheatMode( void )
{
	if( m_pScene )
		m_pScene->OnCheatMode();
}

const XGAME::xSceneInfo XESceneMng::GetSceneInfo( ID idScene )
{
	for( const auto& info : m_arySceneInfos ) {
		if( info.m_idScene == idScene )
			return info;
	}
	return XGAME::xSceneInfo();
}

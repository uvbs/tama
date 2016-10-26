#include "stdafx.h"
#include "XSceneProcess.h"
#include "XSceneBattle.h"
#include "XLegionObj.h"
#include "XBattleField.h"
#include "sprite/SprObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

////////////////////////////////////////////////////////////////
XSceneProcess::XSceneProcess( XSceneBattle *pScene )
{
	Init();
	m_pScene = pScene;
}

void XSceneProcess::Destroy()
{
}

////////////////////////////////////////////////////////////////
/**
 @brief 3,2,1을 하는 Ready씬
*/
XSceneProcessReady::XSceneProcessReady( XSceneBattle *pScene )
	: XSceneProcess( pScene )
{
	Init();
	m_pso321 = new XSprObj(_T("321go.spr") ); 
	m_pso321->SetAction( 1, xRPT_1PLAY );
}

void XSceneProcessReady::Destroy()
{
	SAFE_DELETE( m_pso321 );
}

int XSceneProcessReady::Process( float dt )
{
	if( m_pso321 ) {
		m_pso321->FrameMove( dt );
		if( m_pso321->IsFinish() ) {
			// 레디씬이 끝나면 프레임스킵을 리셋시켜 애니메이션이 갑자기 튀지 않도록 한다.
			SAFE_DELETE( m_pso321 );
			m_pScene->OnEndSceneProcess( this );
			m_timerCount.Off();
		}
	}
	return 1;
}
void XSceneProcessReady::Draw()
{
	if( m_pso321 )
		m_pso321->Draw( XE::GetGameWidth() * 0.5f, 110.f );
	XSceneProcess::Draw();
}

////////////////////////////////////////////////////////////////
XSceneProcessBattle::XSceneProcessBattle( XSceneBattle *pScene, 
																					std::shared_ptr<XBattleField> spWorld )
	: XSceneProcess( pScene )
{
	Init();
	//
//	m_aryLegion = aryLegion;
	// 유닛 인공지능 시작
	spWorld->OnStartBattle();
	CONSOLE("xJC_START_BATTLE");
}

void XSceneProcessBattle::Destroy()
{
	m_pScene->OnEndSceneProcess( this );
}

int XSceneProcessBattle::Process( float dt )
{
	return 1;
}


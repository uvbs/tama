#pragma once
#include "_Wnd2/XWnd.h"

class XSprObj;
class XSceneBattle;
class XLegionObj;
class XBattleField;
////////////////////////////////////////////////////////////////
// 씬의 흐름(프로세스)을 객체화 시켜서 상황에 맞는 적절한 프로세스를 실행할수 있게 한다.
class XSceneProcess : public XWnd
{
//	BOOL m_bExit;
	void Init() {
//		m_bExit = FALSE;
		m_pScene = nullptr;
	}
	void Destroy();
protected:
	XSceneBattle *m_pScene;
//	SET_ACCESSOR( BOOL, bExit );
public:
	XSceneProcess( XSceneBattle *pScene );
	virtual ~XSceneProcess() { Destroy(); }
	//
//	GET_ACCESSOR( BOOL, bExit );
};
////////////////////////////////////////////////////////////////
class XSceneProcessReady : public XSceneProcess
{
	CTimer m_timerCount;
	BOOL m_bFrameSkipOld = FALSE;
	XSprObj *m_pso321 = nullptr;
	void Init() {}
	void Destroy();
public:
	XSceneProcessReady( XSceneBattle *pScene );
	virtual ~XSceneProcessReady() { Destroy(); }
	//
	int Process( float dt );
	void Draw() override;
};

////////////////////////////////////////////////////////////////
class XSceneProcessBattle : public XSceneProcess
{
	void Init() {
	}

	void Destroy();
public:
	XSceneProcessBattle( XSceneBattle *pScene, 
											 std::shared_ptr<XBattleField> spWorld );
	virtual ~XSceneProcessBattle() { Destroy(); }
	;
	int Process( float dt );
};

/********************************************************************
	@date:	2015/01/15 20:23
	@file: 	C:\xuzhu_work\Project\iPhone\Caribe\App\game_src\XTool.h
	@author:	xuzhu
	
	@brief:	인게임 툴
*********************************************************************/
#pragma once
#ifdef _xIN_TOOL
//#include "XWindow.h"
#include "XPropWorld.h"

#ifdef _xIN_TOOL
enum xtToolMode {
	xTM_NONE,
	xTM_CLOUD,
	xTM_TECH,
	xTM_SPOT,
	xTM_OBJ,
};
#endif // _xIN_TOOL

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/24 16:46
*****************************************************************/
class XDelegateTool
{
public:
	XDelegateTool() {}
	virtual ~XDelegateTool() {}
	//
	/// 툴모드가 전환될때 발생한다.
	virtual void DelegateChangeToolMode( xtToolMode modeOld, xtToolMode modeCurr ) {}
}; // class XDelegateTool

/****************************************************************
* @brief
* @author xuzhu
* @date	2015/01/15 20:20
*****************************************************************/
class XBaseTool 
{
public:
	static XBaseTool* s_pCurr;		// 현재 사용중인 툴
	static void sSetpCurr( XBaseTool* pTool ) {
		s_pCurr = pTool;
	}
	static void sDoChangeMode( xtToolMode modeNew );
	static xtToolMode sGetCurrMode() {
		if( s_pCurr == nullptr )
			return xTM_NONE;
		return s_pCurr->GetToolMode();
	}
	static bool sIsToolMode() {
		return s_pCurr != nullptr;
	}
	static bool sIsToolCloudMode() {
		return sGetCurrMode() == xTM_CLOUD;
	}
	static bool sIsToolSpotMode() {
		return sGetCurrMode() == xTM_SPOT;
	}
	static bool sIsToolTechMode() {
		return sGetCurrMode() == xTM_TECH;
	}
	static bool sIsToolBgObjMode() {
		return sGetCurrMode() == xTM_OBJ;
	}
private:
// 	static XDelegateTool* s_pDelegate;
	xtToolMode m_ToolMode = xTM_NONE;
	CTimer m_timerAutoSave;		///< 3초가 지나면 자동세이브 한다.
	void Init() {}
	void Destroy() {}
public:
	XBaseTool( xtToolMode mode ) { 
    Init(); 
    m_ToolMode = mode;
  }
	virtual ~XBaseTool() { Destroy(); }
	//
	GET_ACCESSOR_CONST( xtToolMode, ToolMode );
  bool IsGameMode() {
    return m_ToolMode == xTM_NONE;
  }
	bool IsCloudTool() {
		return m_ToolMode == xTM_CLOUD;
	}
	bool IsTechTool() {
		return m_ToolMode == xTM_TECH;
	}
	bool IsSpotTool() {
		return m_ToolMode == xTM_SPOT;
	}
	BOOL IsAutoSave() {
		return m_timerAutoSave.IsOver();
	}
	void UpdateAutoSave() {
		if( m_timerAutoSave.IsOff() )
			m_timerAutoSave.Set( 1.5f );
		else
			m_timerAutoSave.Reset();
	}
	void ResetAutoSave() {
		if( m_timerAutoSave.IsOn() ) {
			m_timerAutoSave.Reset();
		}
	}
	virtual bool Save() { return true; }
	virtual int Process( float dt );
	virtual void DelSelected() {}
	virtual void CopySelected() {};
	virtual void PasteSelected() {};
	virtual void Undo() {}
	virtual void OnKeyDown( XE::xtDir dir ) {}
	virtual void OnEnterMode() = 0;
	virtual void OnLeaveMode() = 0;
}; // class XBaseTool

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/24 16:22
*****************************************************************/
class XToolCloud : public XBaseTool
{
public:
	static std::shared_ptr<XToolCloud>& sGet();
public:
	XToolCloud();
	virtual ~XToolCloud() { Destroy(); }
	//
//	BOOL OnCreate() override;
	virtual bool Save() override;
private:
	static std::shared_ptr<XToolCloud> s_spInstance;
	void Init() {}
	void Destroy();
	void OnEnterMode() override;
	void OnLeaveMode() override;
}; // class XToolCloud

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/24 16:22
*****************************************************************/
class XToolSpot : public XBaseTool
{
public:
	static std::shared_ptr<XToolSpot>& sGet();
public:
	XToolSpot();
	virtual ~XToolSpot() { Destroy(); }
	//
	bool Save() override;
	void DelSelected() override;
	void CopySelected() override;
	void PasteSelected() override;
	void Undo() override;
private:
	XPropWorld::xBASESPOT *m_pDeleted = nullptr;	// 삭제된 노드. 언두를 위해 받아둠.
	ID m_idSpotInClipboard = 0;		// 카피를 해서 클립보드에 들어간 아이디
private:
	static std::shared_ptr<XToolSpot> s_spInstance;
	void Init() {}
	void Destroy();
	void OnEnterMode() override;
	void OnLeaveMode() override;
}; // class XToolSpot

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/24 16:14
*****************************************************************/
class XToolBgObj : public XBaseTool
{
public:
	static std::shared_ptr<XToolBgObj>& sGet();
public:
	XToolBgObj();
	virtual ~XToolBgObj() { Destroy(); }
	//
	bool Save() override;
	void DelSelected() override;
	void CopySelected() override;
	void PasteSelected() override;
	void Undo() override;
	void OnEnterMode() override;
	void OnLeaveMode() override;
private:
	static std::shared_ptr<XToolBgObj> s_spInstance;
	void Init() {}
	void Destroy();
//	BOOL OnCreate() override;
}; // class XToolObj





#endif // _xIN_TOOL

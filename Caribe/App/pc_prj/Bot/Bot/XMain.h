#pragma once

#include "XFramework/XConsoleMain.h"
#include "XFramework/XEOption.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/11 11:20
*****************************************************************/
class XLibMFC : public XLibConsole
{
public:
	XLibMFC( XConsoleMain* pMain ) { 
		Init(); 
		m_pMain = pMain;
	}
	virtual ~XLibMFC() { Destroy(); }
	// get/setter
	// public member
	void ConsoleMessage( LPCTSTR szMsg ) override;
private:
	// private member
	XConsoleMain* m_pMain = nullptr;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XLibMFC

/****************************************************************
* @brief
* @author xuzhu
* @date	2016/04/06 13:30
*****************************************************************/
class XOption : public XEOption
{
public:
	XOption() {
		Init();
	}
	virtual ~XOption() {
		Destroy();
	}
	// get/setter
	// public member
	GET_SET_ACCESSOR_CONST( int, numBots );
	GET_ACCESSOR_CONST( int, idStartBot );
	GET_SET_ACCESSOR_CONST( const std::string&, strIP );
	GET_SET_ACCESSOR_CONST( int, secOnline );
private:
	// private member
	int m_numBots = 1;
	int m_idStartBot = 1;			// 봇 시작 아이디
	std::string m_strIP;			// 로그인서버 아이피
	int m_secOnline = -1;			// 온라인상태 유지시간(-1은 로그아웃하지 않음)
private:
	// private method
	void Init() {}
	void Destroy() {}
	bool Parse( const std::string& strcKey, CToken& token ) override;
	bool Write( std::string* pOut ) override {
		return true;
	}
}; // class XOption


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/02 11:46
*****************************************************************/
class XMain : public XConsoleMain
{
public:
	XFps m_fpsConnL;
	XFps m_fpsConnG;
	XFps m_fpsLoginL;
	XFps m_fpsLoginG;
	XFps m_fpsDupl;
	static std::shared_ptr<XMain>& sGet();
	static void sDestroyInstance();
public:
	XMain();
	virtual ~XMain() { Destroy(); }
	//
	int m_FPS = 0;
	//
	void Process() override;
	_tstring GetstrInfo( int idxCtrl );
	void ConsoleMessage( LPCTSTR szMsg ) override;
	GET_SET_BOOL_ACCESSOR( bReadyOffline );
	GET_ACCESSOR( XOption&, Option );
private:
	static std::shared_ptr<XMain> s_spInstance;
	CTimer m_timerSec;
	bool m_bReadyOffline = false;
	XOption m_Option;
private:
	void Init() {}
	void Destroy();
	virtual XLibrary* CreateXLibrary( void ) {
		return new XLibMFC( this );
	}
	void DidFinishCreate( void ) override;
	float CalcDT();
}; // class XMain


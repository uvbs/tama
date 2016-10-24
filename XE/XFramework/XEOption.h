/********************************************************************
	@date:	2016/03/29 14:40
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XEOption.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XParamObj.h"

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/20 13:25
*****************************************************************/
class XDelegateOption
{
public:
	XDelegateOption() { }
	virtual ~XDelegateOption() { }
	// get/setter
	// public member
	virtual bool DelegateDidFinishLoad( const XParamObj2& param ) = 0;
private:
	// private member
private:
	// private method
}; // class XDelegateOption
////////////////////////////////////////////////////////////////
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/29 14:40
*****************************************************************/
#define KEY_SLEEP_MAIN					"sleep_main"
#define KEY_SLEEP_DBMNG_READY		"sleep_dbmng_ready"
#define KEY_SLEEP_DBMNG_PUMP		"sleep_dbmng_pump"
#define KEY_SLEEP_HTTP_READY		"sleep_http_ready"
#define KEY_SLEEP_HTTP_PUMP			"sleep_http_pump"
#define KEY_SLEEP_SVR_CONN			"sleep_svr_connect"
#define KEY_SLEEP_SVR_RECV			"sleep_svr_recv"

class XEOption
{
	static XEOption* s_pInstance;
public:
	static XEOption* sGet() {
		return s_pInstance;
	}
public:
	XEOption() { 
		s_pInstance = this;
		Init(); 
	}
	XEOption( XDelegateOption* pDelegate ) 
		: m_pDelegate(pDelegate) { 
		Init(); 
	}
	virtual ~XEOption() { 
		Destroy(); 
		s_pInstance = nullptr;
	}
	// get/setter
// 	GET_BOOL_ACCESSOR( bFirst );
	GET_ACCESSOR_CONST( const XParamObj2&, Param );
	int GetmsecSleep( const char* cKey ) const {
		return m_Param.GetInt( cKey );
	}
	// public member
	bool Load( LPCTSTR szFile = _T("option.ini") );
	bool Load2( LPCTSTR szFile = _T("option.ini") );
	bool LoadAbs( LPCTSTR szFile = _T("option.ini") );
	bool Reload2() {
		Clear();
		return Load2( m_strFile.c_str() );
	}
	bool Save();
	virtual bool Parse( const std::string& strcKey, CToken& token ) { return true; }
	virtual bool Write( std::string* pOut ) { return true; }
	virtual void DidFinishLoad( const XParamObj2& param ) {}
private:
	// private member
	XDelegateOption* m_pDelegate = nullptr;
	_tstring m_strFile;
	XParamObj2 m_Param;
// 	bool m_bFirst = false;		// 최초 앱을 실행했는지
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Clear() {
		m_Param.Clear();
	}
}; // class XEOption

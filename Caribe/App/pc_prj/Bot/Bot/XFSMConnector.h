/********************************************************************
	@date:	2016/01/11 20:40
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XFSMConnector.h
	@author:	xuzhu
	
	@brief:	서버커넥터 FSM
*********************************************************************/
#pragma once
#include "XFramework/XParamObj.h"
class XFSMBaseConnector;
class XConnector;
class XBotObj;

XE_NAMESPACE_START( xConnector )
//
enum xtFSMEvent {
	xFE_NONE,
	xFE_START_CONNECTING,
	xFE_CONNECTED_LOGINSVR,
	xFE_LOGINED_LOGINSVR,
	xFE_CONNECTED_GAMESVR,
	xFE_LOGINED_GAMESVR,
	xFE_DISCONNECTED_LOGINSVR,
	xFE_DISCONNECTED_GAMESVR,
	xFE_NO_RESPONSE,
	xFE_FAILED_CREATE_SOCKET,
	xFE_ETC_FAILED,
	xFE_NO_ACCOUNT,
	xFE_DUPLICATE_NICK,
	xFE_DELAY_OVER,
	xFE_WILL_CONNECT,		// 접속할 예정
	xFE_WILL_DISCONNECT,		// 자의로 접속 끊을거임
	xFE_RECONNECT_TRY_LOGINSVR,			// 이미 접속되어있던 계정이 있어 끊었으니 다시 로그인해주세요.
	xFE_RECONNECT_TRY_GAMESVR,	// 게임서버로의 로그인을 다시 시도해주세요.
};
struct xChangeFSM {
	xtFSMEvent m_Event;
	XFSMBaseConnector* m_fsmObj;
	xChangeFSM( xtFSMEvent event, XFSMBaseConnector* fsmObj )
		: m_Event(event), m_fsmObj(fsmObj) {}
};
// fsm객체의 아이디
enum xtFSMId {
	xFID_NONE,
	xFID_OFFLINE,
	xFID_CONNECTING_LOGINSVR,
	xFID_LOGINING_LOGINSVR,
	xFID_CONNECTING_GAMESVR,
	xFID_LOGINING_GAMESVR,
	xFID_MAKE_NEW_ACCOUNT,
	xFID_ONLINE,
	xFID_DELAY_AFTER_CONNECTING_LOGINSVR,
	xFID_DELAY_AFTER_CONNECTING_GAMESVR,
	xFID_MAX,
};

LPCTSTR GetenumFSMId( xtFSMId idFSM );

struct xConnectParam {
private:
	XGAME::xtConnectParam m_paramConnect = XGAME::xCP_NONE;
	std::string m_strcUUID;
	std::string m_strcID;
	std::string m_strcPassword;
	_tstring m_strtNick;
	std::string m_strcFbUserid;
public:
	void SetConnectParamByUUID( const std::string& strcUUID ) {
		m_paramConnect = XGAME::xCP_LOGIN_UUID;
		m_strcUUID = strcUUID;
		m_strcID.clear();
		m_strcPassword.clear();
		m_strtNick.clear();
	}
	void SetConnectParamByFacebook( const std::string& strFbuserid ) {
		m_paramConnect = XGAME::xCP_LOGIN_FACEBOOK;
		m_strcFbUserid = strFbuserid;
		m_strcUUID.clear();
		m_strcID.clear();
		m_strcPassword.clear();
		m_strtNick.clear();
	}
	void SetConnectParamByIDPW( const std::string& strcID, const std::string& strcPassword ) {
		m_paramConnect = XGAME::xCP_LOGIN_IDPW;
		m_strcID = strcID;
		m_strcPassword = strcPassword;
		m_strtNick.clear();
		m_strcUUID.clear();
	}
	void SetConnectParamByNewAcc( const _tstring& strNick ) {
		m_paramConnect = XGAME::xCP_LOGIN_NEW_ACC;
		m_strtNick = strNick;
		m_strcID.clear();
		m_strcPassword.clear();
		m_strcUUID.clear();
	}
	void SetConnectParamByNewAcc( const _tstring& strNick, const std::string& strFbUserid ) {
		m_paramConnect = XGAME::xCP_LOGIN_NEW_ACC_BY_FACEBOOK;
		m_strtNick = strNick;
		m_strcFbUserid = strFbUserid;
		m_strcID.clear();
		m_strcPassword.clear();
		m_strcUUID.clear();
	}
	GET_ACCESSOR_CONST( XGAME::xtConnectParam, paramConnect );
	GET_ACCESSOR_CONST( const std::string&, strcUUID );
	GET_ACCESSOR_CONST( const std::string&, strcID );
	GET_ACCESSOR_CONST( const std::string&, strcPassword );
	GET_ACCESSOR_CONST( const _tstring&, strtNick );
	GET_ACCESSOR_CONST( const std::string&, strcFbUserid );
};
//
XE_NAMESPACE_END; // xConnector

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/12 12:39
*****************************************************************/
// template<typename T>
// class XParamType
// {
// public:
// 	XParamType() { Init(); }
// 	virtual ~XParamType() { Destroy(); }
// 	// get/setter
// 	// public member
// 	void Set( const char* cKey, const T& val ) {
// 		const std::string strKey(cKey);
// 		auto itor = m_mapVal.find( strKey );
// 		if( XBREAK(itor != m_mapVal.end()) )	// 중복된 키
// 			return;
// 		m_mapVal[ strKey ] = val;
// 	}
// 	T Get( const char* cKey ) const {
// 		auto itor = m_mapVal.find( std::string(cKey) );
// 		if( itor == m_mapVal.end() )
// 			return T();
// 		return (*itor).second;
// 	}
// 	bool IsEmpty() const {
// 		return m_mapVal.empty();
// 	}
// private:
// 	// private member
// 	std::map<std::string,T> m_mapVal;
// private:
// 	// private method
// 	void Init() {}
// 	void Destroy() {}
// }; // class XParamObj

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/12 14:33
*****************************************************************/
// class XParamObj
// {
// public:
// 	XParamObj() { Init(); }
// 	virtual ~XParamObj() { Destroy(); }
// 	// get/setter
// 	// public member
// 	void Set( const char* cKey, int val ) {
// 		m_paramInt.Set( cKey, val );
// 	}
// 	void Set( const char* cKey, WORD val ) {
// 		m_paramWord.Set( cKey, val );
// 	}
// 	void Set( const char* cKey, DWORD val ) {
// 		m_paramDword.Set( cKey, val );
// 	}
// 	void Set( const char* cKey, float val ) {
// 		m_paramFloat.Set( cKey, val );
// 	}
// 	void Set( const char* cKey, char val ) {
// 		m_paramChar.Set( cKey, val );
// 	}
// 	void Set( const char* cKey, const std::string& val ) {
// 		m_paramStrc.Set( cKey, val );
// 	}
// #ifdef WIN32
// 	void Set( const char* cKey, const _tstring& val ) {
// 		m_paramStrt.Set( cKey, val );
// 	}
// #endif // WIN32
// 	int GetInt( const char* cKey ) const {
// 		return m_paramInt.Get( cKey );
// 	}
// 	DWORD GetDword( const char* cKey ) const {
// 		return m_paramDword.Get( cKey );
// 	}
// 	DWORD GetWord( const char* cKey ) const {
// 		return m_paramWord.Get( cKey );
// 	}
// 	float GetFloat( const char* cKey ) const {
// 		return m_paramFloat.Get( cKey );
// 	}
// 	char GetChar( const char* cKey ) const {
// 		return m_paramChar.Get( cKey );
// 	}
// 	std::string GetStrc( const char* cKey ) const {
// 		return m_paramStrc.Get( cKey );
// 	}
// 	_tstring GetStrt( const char* cKey ) const {
// #ifdef WIN32
// 		return m_paramStrt.Get( cKey );
// #else	// WIN32
// 		return m_paramStrc.Get( cKey );
// #endif // not win32
// 	}
// 	bool IsEmpty() const {
// 		return m_paramInt.IsEmpty() && m_paramDword.IsEmpty() && m_paramWord.IsEmpty()
// 				&& m_paramFloat.IsEmpty() && m_paramChar.IsEmpty() && m_paramStrc.IsEmpty() 
// #ifdef WIN32
// 				&& m_paramStrt.IsEmpty()
// #endif // WIN32
// 				;
// 	}
// private:
// 	// private member
// 	XParamType<int> m_paramInt;
// 	XParamType<WORD> m_paramWord;
// 	XParamType<DWORD> m_paramDword;
// 	XParamType<float> m_paramFloat;
// 	XParamType<char> m_paramChar;
// 	XParamType<std::string> m_paramStrc;
// #ifdef WIN32
// 	XParamType<_tstring> m_paramStrt;
// #endif // WIN32
// private:
// 	// private method
// 	void Init() {}
// 	void Destroy() {}
// }; // class XParam


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/11 20:41
*****************************************************************/
class XFSMBaseConnector
{
public:
	XFSMBaseConnector( xConnector::xtFSMId idFSM, XBotObj* pOwner ) { 
		Init(); 
		m_idFSM = idFSM;
		m_pOwner = pOwner;
	}
	virtual ~XFSMBaseConnector() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( const XParamObj&, Param );
	GET_ACCESSOR_CONST( xConnector::xtFSMId, idFSM );
	// public member
	void AddFSMEvent( xConnector::xtFSMEvent event, XFSMBaseConnector* pfsmObj );
	// fsm개별 추가 이벤트 처리
//	virtual void AdditionalFSMEvent( xConnector::xtFSMEvent event ) {}
	XFSMBaseConnector* GetpfsmByEvent( xConnector::xtFSMEvent event );
	void SetParam( const XParamObj& paramObj ) {
		if( paramObj.IsEmpty() )
			return;
		m_Param = paramObj;
	}
	virtual void Process();
	virtual void OnStart() {}
	virtual void OnFinish() {}
protected:
	void SetTimeout( int sec ) {
		m_timerTimeout.Set( (float)sec );
	}
	XBotObj* m_pOwner = nullptr;
private:
	// private member
	xConnector::xtFSMId m_idFSM = xConnector::xFID_NONE;
	XVector<xConnector::xChangeFSM> m_aryChangeFSM;
	XParamObj m_Param;
	CTimer m_timerTimeout;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XFSMBaseConnector

/****************************************************************
* @brief
* @author xuzhu
* @date	2016/01/11 20:52
*****************************************************************/
class XFSMOffline : public XFSMBaseConnector {
public:
	XFSMOffline( XBotObj* pOwner ) : XFSMBaseConnector( xConnector::xFID_OFFLINE, pOwner ) {
		Init();
	}
	virtual ~XFSMOffline() {
		Destroy();
	}
	// get/setter
	// public member
protected:
	void OnStart() override;
private:
	// private member
	CTimer m_timerDelay;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Process() override;
}; // class XFSMOffline

/****************************************************************
* @brief Login서버 connecting 상태 fsm
* @author xuzhu
* @date	2016/01/11 20:41
*****************************************************************/
class XFSMConnectingL : public XFSMBaseConnector
{
public:
	XFSMConnectingL( XBotObj* pOwner );
	~XFSMConnectingL() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( const xConnector::xConnectParam&, paramConnect );
	xConnector::xConnectParam& GetparamConnectMutable() {
		return m_paramConnect;
	}
	// public member
protected:
	void OnStart() override;
private:
	// private member
	xConnector::xConnectParam m_paramConnect;
private:
	// private method
	void Init() {}
	void Destroy() {}
//	void Process() override;
}; // class XFSMConnectingLoginSvr

/****************************************************************
* @brief 로그인서버 Login중 fsm
* @author xuzhu
* @date	2016/01/11 20:48
*****************************************************************/
class XFSMLoginingL : public XFSMBaseConnector
{
public:
	static int s_cntSample;		// 평균로그인시간 측정용 카운터
	static double s_secSum;			// 누적
public:
	XFSMLoginingL( XBotObj* pOwner );
	virtual ~XFSMLoginingL() { Destroy(); }
	// get/setter
	GET_ACCESSOR_CONST( const xConnector::xConnectParam&, paramConnect );
	xConnector::xConnectParam& GetparamConnectMutable() {
		return m_paramConnect;
	}
	// public member
protected:
	void OnStart() override;
	void OnFinish() override;
private:
	// private member
	xConnector::xConnectParam m_paramConnect;
	CTimer m_timerLogin;		// 평균 로그인시간 측정용
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XFSMLoginingL

/****************************************************************
* @brief 게임서버 connect중 fsm
* @author xuzhu
* @date	2016/01/11 20:50
*****************************************************************/
class XFSMConnectingG : public XFSMBaseConnector
{
public:
	XFSMConnectingG( XBotObj* pOwner ) 
		: XFSMBaseConnector( xConnector::xFID_CONNECTING_GAMESVR, pOwner ) { Init(); }
	virtual ~XFSMConnectingG() { Destroy(); }
	// get/setter
protected:
	void OnStart() override;
	// public member
private:
	// private member
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XFSMConnectingG

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/11 20:51
*****************************************************************/
class XFSMLoginingG : public XFSMBaseConnector
{
public:
	XFSMLoginingG( XBotObj* pOwner ) 
		: XFSMBaseConnector( xConnector::xFID_LOGINING_GAMESVR, pOwner ) { Init(); }
	virtual ~XFSMLoginingG() { Destroy(); }
	// get/setter
	// public member
protected:
	void OnStart() override;
private:
	// private member
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XFSMLoginingG
////////////////////////////////////////////////////////////////

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/11 20:52
*****************************************************************/
class XFSMOnline : public XFSMBaseConnector
{
public:
	XFSMOnline( XBotObj* pOwner ) : XFSMBaseConnector( xConnector::xFID_ONLINE, pOwner ){ Init(); }
	virtual ~XFSMOnline() { Destroy(); }
	// get/setter
	// public member
protected:
	void OnStart() override;
	void OnFinish() override;
private:
	// private member
	CTimer m_timerDelay;
private:
	// private method
	void Init() {}
	void Destroy() {}
	void Process() override;
}; // class XFSMOnline
////////////////////////////////////////////////////////////////
/**
 @brief 
*/
class XFSMMakeNewAccount : public XFSMBaseConnector {
public:
	XFSMMakeNewAccount( XBotObj* pOwner ) 
		: XFSMBaseConnector( xConnector::xFID_MAKE_NEW_ACCOUNT, pOwner ) {
		Init();
	}
	virtual ~XFSMMakeNewAccount() {
		Destroy();
	}
	// get/setter
	// public member
protected:
	void OnStart() override;
private:
	// private member
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XFSMMakeNewAccount

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/01/11 20:52
*****************************************************************/
class XFSMDelayAfterConnectingL : public XFSMBaseConnector
{
public:
	XFSMDelayAfterConnectingL( XBotObj* pOwner )
		: XFSMBaseConnector( xConnector::xFID_DELAY_AFTER_CONNECTING_LOGINSVR, pOwner ) { Init(); }
	virtual ~XFSMDelayAfterConnectingL() { Destroy(); }
	// get/setter
	// public member
	void Process() override;
protected:
	void OnStart() override;
private:
	// private member
	xSec m_secDelay = 1;
	CTimer m_timerDelay;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XFSMDelayAfterConnectingL
////////////////////////////////////////////////////////////////
/****************************************************************
* @brief
* @author xuzhu
* @date	2016/01/11 20:52
*****************************************************************/
class XFSMDelayAfterConnectingG : public XFSMBaseConnector
{
public:
	XFSMDelayAfterConnectingG( XBotObj* pOwner )
		: XFSMBaseConnector( xConnector::xFID_DELAY_AFTER_CONNECTING_GAMESVR, pOwner ) { Init(); }
	virtual ~XFSMDelayAfterConnectingG() { Destroy(); }
	// get/setter
	// public member
	void Process() override;
protected:
	void OnStart() override;
private:
	// private member
	xSec m_secDelay = 1;
	CTimer m_timerDelay;
private:
	// private method
	void Init() {}
	void Destroy() {}
}; // class XFSMDelayAfterConnectingG
////////////////////////////////////////////////////////////////



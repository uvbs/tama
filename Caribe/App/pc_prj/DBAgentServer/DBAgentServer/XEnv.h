/********************************************************************
	@date:	2016/03/29 14:40
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XOption.h
	@author:	xuzhu
	
	@brief:	
*********************************************************************/
#pragma once
#include "XFramework/XEOption.h"
/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/03/29 14:40
*****************************************************************/

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/04/20 13:35
*****************************************************************/
class XEnv : public XEOption
{
public:
	static std::shared_ptr<XEnv>& sGet();
	static void sDestroyInstance();
public:
	XEnv();
	virtual ~XEnv() { Destroy(); }
	// get/set
	GET_ACCESSOR_CONST( int, maxConnect );
	GET_ACCESSOR_CONST( WORD, GSvrID );
	GET_ACCESSOR_CONST( const std::string&, strPublicKey );
	GET_ACCESSOR_CONST( WORD, PortLoginSvr );
	GET_ACCESSOR_CONST( WORD, PortDBASvr );
	GET_ACCESSOR_CONST( WORD, PortForClient );
	GET_STRING_ACCESSOR( strIPLoginSvr );	// const char* GetstrIPExternal();
	GET_STRING_ACCESSOR( strIPDBASvr );
	GET_STRING_ACCESSOR( strIPExternal );
	//
private:
	static std::shared_ptr<XEnv> s_spInstance;
	int m_maxConnect = 0;
	WORD m_PortLoginSvr = 0;			// 로그인서버 포트
	WORD m_PortDBASvr = 0;			// DBA서버 포트
	WORD m_PortForClient = 0;	// 클라이언트가 접속할 포트
	WORD m_GSvrID = 0;				// 게임서버들의 아이디
	std::string m_strIPLoginSvr;	// 로그인 서버 아이피
	std::string m_strIPDBASvr;		// DBA서버 아이피
	std::string m_strIPExternal;	// 클라가 접속해올 외부아이피
	std::string m_strPublicKey;		// 구글플레이용 공개키
private:
	void Init() {}
	void Destroy() {}
	void DidFinishLoad( const XParamObj2& param ) override;
}; // class XEnv



#include "stdafx.h"
#include "XEnv.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

std::shared_ptr<XEnv> XEnv::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XEnv>& XEnv::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XEnv>( new XEnv );
	return s_spInstance;
}
void XEnv::sDestroyInstance() {
	s_spInstance.reset();
}

////////////////////////////////////////////////////////////////
XEnv::XEnv()
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

////////////////////////////////////////////////////////////////
void XEnv::DidFinishLoad( const XParamObj2& param ) 
{
	m_GSvrID = param.GetWord( "id_game_svr");
	m_maxConnect = param.GetInt( "max_connect" );
	m_PortLoginSvr = param.GetWord( "port_loginsvr" );
	m_PortDBASvr = param.GetWord( "port_dbsvr" );
	m_PortForClient = param.GetWord( "port_client" );
	m_strIPLoginSvr = param.GetStrc( "ip_loginsvr" );
	m_strIPDBASvr = param.GetStrc( "ip_dbsvr" );
	m_strIPExternal = param.GetStrc( "ip_external" );
	m_strPublicKey = param.GetStrc( "google_public_key" );
	// 로긴시뮬에선 게임서버 아이피가 있었지만
	// 실제서버에선 접속하는 게임서버의 아이피를 따로 받아둬야 한다.
	CONSOLE( "max connect=%d", m_maxConnect );
}



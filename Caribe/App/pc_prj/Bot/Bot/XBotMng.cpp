#include "stdafx.h"
#include "XBotMng.h"
#include "XBotObj.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace XGAME;
using namespace xConnector;

std::shared_ptr<XBotMng> XBotMng::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XBotMng>& XBotMng::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XBotMng>( new XBotMng );
	return s_spInstance;
}
void XBotMng::sDestroyInstance() {
	s_spInstance.reset();
}
////////////////////////////////////////////////////////////////
XBotMng::XBotMng()
	: m_aryNumBots( xFID_MAX )
{
	Init();
}

void XBotMng::Destroy()
{
}

XBotObj* XBotMng::AddBotObj( XBotObj* pObj )
{
	m_listBots.Add( pObj );
	return pObj;
}
void XBotMng::Process( float dt )
{
	m_aryNumBots.assign( m_aryNumBots.size(), 0 );
	// process
	int idx = 0;
	for( auto pObj : m_listBots ) {
		if( !pObj->IsbDestroy() ) {
			pObj->Process( dt );
			auto stateConn = pObj->GetstateConnect();
			++m_aryNumBots[ stateConn ];
		}
		++idx;
#ifdef _DEBUG
// 		CString str;
// 		str.Format(_T("idx=%d\n"), idx );
// 		::OutputDebugString( str );
#endif // _DEBUG
	}
	// 파괴
	for( auto itor = m_listBots.begin(); itor != m_listBots.end(); ) {
		XBotObj* pObj = (*itor);
		if( pObj->IsbDestroy() ) {
			SAFE_DELETE( pObj );
			m_listBots.erase( itor++ );
		} else
			++itor;
	}
	

}
void XBotMng::DestroyBotObj( XBotObj* pObj )
{
	
}

void XBotMng::DoStart()
{
	for( auto pObj : m_listBots ) {
		pObj->OnStart();
	}
}


#include "stdafx.h"
#include "XFramework/XReceiverCallback.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/client/XEContent.h"
#include "XFramework/XReceiverCallbackH.h"
#ifdef _VER_ANDROID
#include "XFramework/android/JniHelper.h"
#elif defined(_VER_IOS)
#include "objc/xe_ios.h"
#endif

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
using namespace xnReceiverCallback;

std::shared_ptr<XReceiverCallback> XReceiverCallback::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XReceiverCallback>& XReceiverCallback::sGet() { if( s_spInstance == nullptr )	s_spInstance = std::shared_ptr<XReceiverCallback>( new XReceiverCallback );	return s_spInstance; }
void XReceiverCallback::sDestroyInstance() {
	s_spInstance.reset();
}
////////////////////////////////////////////////////////////////
/**
 @brief 디바이스의 콜백스레드로부터 데이타를 받음.
*/
void XReceiverCallback::cbOnReceiveCallback( const std::string& strIds
																				, const std::string& strParam1
																				, const std::string& strParam2
																				, const std::string& strParam3 )
{
	XBREAK( strIds.empty() );
	// 큐에 밀어넣는다.
	XLockObj lockObj( &m_Lock );
	m_listCallbackData.Add( xData(strIds, strParam1, strParam2, strParam3 ) );
}

/**
 @brief 게임의 메인스레드에서 호출됨.
 큐에 쌓인 콜백데이타를 하나씩 뽑아 ids에 따라 
*/
void XReceiverCallback::Process()
{
	XLockObj lockObj( &m_Lock );
	// 큐를 한번에 다 처리하지 않고 블로킹을 방지하기 위해 하나씩 뽑아서 처리한다.
	if( m_listCallbackData.size() > 0 ) {
		auto itor = m_listCallbackData.begin();
		const xData& data = (*itor);
		bool bOk = XEContent::sGet()->OnReceiveCallbackData( data );
// 		if( data.m_strIds == "gcm_regid" ) {
// 			// 게임앱(XGame)으로 이벤트 보냄 virtual
// 			bOk = XEContent::sGet()->OnReceiveCallbackData( data );
// 		} else {
// 			XBREAKF(1, "unknown callback ids: ids=%s", data.m_strIds.c_str() );
// 		}
		// 
		if( !bOk ) {
			// 콜백처리가 실패했으면 큐 뒤로 옮기고 다른 이벤트들을 다 처리하고 난 후 다시 처리하도록 한다.
			m_listCallbackData.push_back( data );
		}
		// 큐를 pop한다.
		m_listCallbackData.erase( itor );
	}
	
}
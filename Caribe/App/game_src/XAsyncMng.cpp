#include "stdafx.h"
#include "XAsyncMng.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

using namespace xnAsync;

std::shared_ptr<XAsyncMng> XAsyncMng::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XAsyncMng>& XAsyncMng::sGet() {
	if( s_spInstance == nullptr )
		s_spInstance = std::shared_ptr<XAsyncMng>( new XAsyncMng );
	return s_spInstance;
}
void XAsyncMng::sDestroyInstance() {
	s_spInstance.reset();
}

////////////////////////////////////////////////////////////////
XAsyncMng::XAsyncMng()
{
	XBREAK( s_spInstance != nullptr );
	Init();
}

/** //////////////////////////////////////////////////////////////////
 @brief 비동기로 실행할 함수를 등록한다.
*/
void XAsyncMng::Add( xSec secAfter, std::function<void()> func )
{
	xAsync async;
	async.m_secAfter = secAfter;
	async.m_funcAsync = func;
	async.m_Timer.Set( secAfter );
	m_listAsync.push_back( async );
}

void XAsyncMng::Process()
{
	for( auto& async : m_listAsync ) {
		if( async.m_Timer.IsOver() ) {
			async.m_funcAsync();
		}
	}
}
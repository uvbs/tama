#include "stdafx.h"
#include "XTemp.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif



/**
 @brief 
 @param pWndLayer add받은 ui를 뿌려야 하는 레이어
 @param secDelay 몇초간격으로 뿌려줄건가.
*/
XMsgPusher::XMsgPusher( XWnd* pWndLayer, float secDelay )
{
	m_pWndLayer = pWndLayer;
	m_secDelay = secDelay;
}

void XMsgPusher::Add( XWnd* pPushUI )
{
	m_qObj.push_back( pPushUI );
}

/**
 @brief add된 ui들을 m_pWndLayer에 뿌린다.
*/
void XMsgPusher::Process( float dt )
{
	// a
	if( m_qObj.size() > 0 ) {
		if( m_timerDelay.IsOver() || m_timerDelay.IsOff() ) {
			auto pWnd = m_qObj.GetFirst();
			m_pWndLayer->Add( pWnd );
			m_qObj.DelByIdx( 0 );
			m_timerDelay.Set( m_secDelay );
		}
	}
}

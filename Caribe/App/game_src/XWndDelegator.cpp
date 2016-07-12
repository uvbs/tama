#include "stdafx.h"
#include "XWndDelegator.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

// template<typename T>
// std::shared_ptr<XDelegator<T>> XDelegator<T>::s_spInstance;
// ////////////////////////////////////////////////////////////////
// template<typename T>
// std::shared_ptr<XDelegator<T>>& XDelegator<T>::sGet() {
// 	if( s_spInstance == nullptr )
// 		s_spInstance = std::shared_ptr<XDelegator>( new XDelegator );
// 	return s_spInstance;
// }
// 
// void XDelegator<T>::sDestroyInstance() {
// 	s_spInstance.reset();
// }

////////////////////////////////////////////////////////////////
// void XDelegator::DelegateTrainComplete( XGAME::xtTrain type, XHero *pHero )
// {
// 	
// 	for( auto itor = m_listWnd.begin(); itor != m_listWnd.end(); )	{
// 		const std::string& idsWnd = (*itor);
// 		const std::string idsEvent;
// 		// 훈련완료 요청을 한 윈도우에게만 이벤트를 보낸다.
// 		if( idsEvent == "train.complete" ) {
// 			auto pWndDelegate = SafeCast<XDelegateEvent*>( GAME->Find( idsWnd ) );
// 			if( pWndDelegate ) {
// 				pWndDelegate->DelegateTrainComplete( type, pHero );
// 			}
// 			// 델리게이팅을 수행했으면 바로 삭제한다. 받든 못받든 상관없이 삭제한다.
// 			m_listWnd.erase( itor++ );
// 		}
// 	}
// 	
// }

/********************************************************************
	@date:	2016/02/03 16:08
	@file: 	C:\xuzhu_work\Project\iPhone_zero\Caribe\App\game_src\XWndDelegator.h
	@author:	xuzhu
	
	@brief:	각종 이벤트의 UI로의 전달을 위한 델리게이터
*********************************************************************/
#pragma once
//#include "_Wnd2/XWnd.h"

class XHero;

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/02/03 16:09
*****************************************************************/
// class XDelegateEvent
// {
// public:
// 	XDelegateEvent() { Init(); }
// 	virtual ~XDelegateEvent() { Destroy(); }
// 	// get/setter
// 	// public member
// 	virtual void DelegateTrainComplete( XGAME::xtTrain type, XSPHero pHero ) {}
// 	virtual void DelegateEvent( const std::string& idsEvent ) {}
// 	template<typename T1>
// 	virtual void DelegateEvent( const std::string& idsEvent, T1 p1 ) {}
// 	template<typename T1, typename T2>
// 	virtual void DelegateEvent( const std::string& idsEvent, T1 p1, T2 p2 ) {}
// private:
// 	// private member
// 	int m_Test = 0;
// private:
// 	// private method
// 	void Init() {}
// 	void Destroy() {}
// }; // class XDelegateEvent
// ////////////////////////////////////////////////////////////////
// XDelegateEvent::XDelegateEvent()
// {
// 	Init();
// }
// 
// void XDelegateEvent::Destroy()
// {
// }


/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/02/03 20:14
*****************************************************************/
// class Foo
// {
// public:
// 	template<typename T>
// 	struct xCALLBACK {
// 		std::string m_idsEvent;		// 이벤트 종류
// // 		std::function<void( XWnd*, const std::string& )> m_callback;
// 		T m_callback;
// 		std::string m_idsWnd;			// 이벤트 받을 윈도우
// 	};
// 	XList4<xCALLBACK<T>> m_listReceiver;
// public:
// 	Foo() { Init(); }
// 	virtual ~Foo() { Destroy(); }
// 	// get/setter
// 	// public member
// 	template<typename F>
// 	void DoRequest( const std::string& idsEvent, const std::string& idsWnd, F func ) {
// 		xCALLBACK callbackObj;
// 		callbackObj.m_callback 
// 			= std::bind( static_cast<T>( func ), std::placeholders::_1, std::placeholders::_2 );
// 		callbackObj.m_idsWnd = idsWnd;
// 		callbackObj.m_idsEvent = idsEvent;
// 		m_listReceiver.Add( callbackObj );
// 	}
// private:
// 	// private member
// private:
// 	// private method
// 	void Init() {}
// 	void Destroy() {}
// }; // class Foo

/****************************************************************
* @brief 
* @author xuzhu
* @date	2016/02/03 16:36
*****************************************************************/
class XDelegator
{
	struct xCALLBACK {
		ID m_idWnd = 0;
		std::string m_idsEvent;		// 이벤트 종류
		std::function<void( XWnd*, const std::string& )> m_callback;
		std::function<void( XWnd*, const std::string&, XGAME::xtTrain, XSPHero )> m_callback2;
		std::string m_idsWnd;			// 이벤트 받을 윈도우
	};
public:
	static XDelegator& sGet() {	static XDelegator instance;		return instance;	}
	static void sDestroyInstance();
	void Init() {}
	void Destroy() {}
	void DelegateEvent( const std::string& idsEvent ) {
		for( auto itor = m_listReceiver.begin(); itor != m_listReceiver.end(); ) {
			const auto& receiver = (*itor);
			if( receiver.m_idsEvent == idsEvent ) {
				XBREAK( m_pRoot == nullptr );
				auto pWndReceiver = m_pRoot->Find( receiver.m_idsWnd );
				if( pWndReceiver && pWndReceiver->getid() == receiver.m_idWnd ) {
					receiver.m_callback( pWndReceiver, receiver.m_idsEvent );
				}
				m_listReceiver.erase( itor++ );
			} else {
				++itor;
			}
		} // for
	}
	template<typename T1, typename T2>
	void DelegateEvent( const std::string& idsEvent, T1 p1, T2 p2 ) {
		for( auto itor = m_listReceiver.begin(); itor != m_listReceiver.end(); ) {
			const auto& receiver = (*itor);
			if( receiver.m_idsEvent == idsEvent ) {
				XBREAK( m_pRoot == nullptr );
				auto pWndReceiver = m_pRoot->Find( receiver.m_idsWnd );
				if( pWndReceiver && pWndReceiver->getid() == receiver.m_idWnd ) {
					receiver.m_callback2( pWndReceiver, receiver.m_idsEvent, p1, p2 );
				}
				m_listReceiver.erase( itor++ );
			} else {
				++itor;
			}
		} // for
	}
	///< 
	template<typename F>
	void DoRequest( const std::string& idsEvent, XWnd* pReceiver, F func ) {
		typedef void (XWnd::*CallbackFunc)( const std::string& );
		xCALLBACK callbackObj;
		callbackObj.m_callback 
// 			= std::bind( func, std::placeholders::_1, std::placeholders::_2 );
		= std::bind( static_cast<CallbackFunc>( func ), std::placeholders::_1, std::placeholders::_2 );
		callbackObj.m_idWnd = pReceiver->getid();
		callbackObj.m_idsWnd = pReceiver->GetstrIdentifier();
		callbackObj.m_idsEvent = idsEvent;
		m_listReceiver.Add( callbackObj );
	}
	template<typename F>
	void DoRequest2( const std::string& idsEvent, XWnd* pReceiver, F func ) {
		typedef void ( XWnd::*CallbackFunc )( const std::string&, XGAME::xtTrain, XSPHero );
		xCALLBACK callbackObj;
		callbackObj.m_callback2 
			= std::bind( static_cast<CallbackFunc>( func ), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
		callbackObj.m_idWnd = pReceiver->getid();
		callbackObj.m_idsWnd = pReceiver->GetstrIdentifier();
		callbackObj.m_idsEvent = idsEvent;
		m_listReceiver.Add( callbackObj );
	}
public:
	XDelegator() { Init(); }
	virtual ~XDelegator() { Destroy(); }
	//
	GET_SET_ACCESSOR_CONST( XWnd*, pRoot );
//	Foo m_Foo;
 	XList4<xCALLBACK> m_listReceiver;
private:
//	static std::shared_ptr<XDelegator<T>> s_spInstance;
	XWnd *m_pRoot = nullptr;
private:
}; // class XDelegator


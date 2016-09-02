#include "stdafx.h"
#include "XFramework/XInApp.h"
#include "XFramework/XReceiverCallback.h"
#include "XSystem.h"
#include "XFramework/client/XApp.h"
#if defined(_VER_ANDROID)
#include "XFramework/android/JniHelper.h"
#elif defined(_VER_IOS)
#endif 


#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

std::shared_ptr<XInApp> XInApp::s_spInstance;
////////////////////////////////////////////////////////////////
std::shared_ptr<XInApp>& XInApp::sGet() { if( s_spInstance == nullptr )		s_spInstance = std::shared_ptr<XInApp>( new XInApp );	return s_spInstance; }
void XInApp::sDestroyInstance() {
	s_spInstance.reset();
}
//////////////////////////////////////////////////////////////////////////
void XInApp::InitGoogleIAPAsync( const std::string& strPublicKey )
{
#if defined(_VER_ANDROID)
	JniHelper::InitGoogleIAPAsync( strPublicKey );
#elif defined(_VER_IOS)
#elif defined(WIN32)
	cbInitGoogleIAPAsync();
#endif 

}

/**
 @brief 인앱시스템이 초기화 되었다.
*/
void XInApp::cbInitGoogleIAPAsync()
{
	m_bInited = true;
}

void XInApp::cbDetectedUnConsumedItem( const std::string& strcJson, const std::string& strcSignature )
{
	m_strcJsonByUnconsumed = strcJson;
	XReceiverCallback::sGet()->cbOnReceiveCallback( "iap_unconsumed_item", strcJson, strcSignature );
}

/**
 @brief 구매요청의 결과가 돌아옴.
 JniHelper::OnClickedBuyItem의 콜백
 @param strcJsonReceipt IAP에서 돌려준 오리지날 json
 @param strcSignature json의 암호화 버전인듯.
 @param idsProduct 구글스토어에선 사용하지 않는다.
*/
void XInApp::cbOnRecvBuyRequest( bool bOk
															, const std::string& strcJsonReceipt
															, const std::string& strcSignature 
															, const std::string& strcIdsProduct )
{
	XReceiverCallback::sGet()->cbOnReceiveCallback( "iap_buy_request"
																								, strcJsonReceipt
																								, strcSignature
																								, strcIdsProduct );
 	XE::SetbResumeBlock( FALSE );		// OnResume처리가 일어나지 않도록 함.
}

/**
 @brief idsProduct상품을 소진시킨다.,
*/
void XInApp::DoConsumeItemAsync( const _tstring& _idsProduct )
{
	CONSOLE("%s:", __TFUNC__);
	const std::string cIdsProduct = SZ2C(_idsProduct);
#if defined(_VER_ANDROID)
	JniHelper::DoConsumeItemAsync( cIdsProduct );
#elif defined(_VER_IOS)
#elif defined(WIN32)
#else
#error "unknown platform"
#endif 
}

/**
 @brief 소진 완료
*/
void XInApp::cbFinishedConsumItem( bool bOk, const _tstring& cidsProduct )
{
	if( !bOk ) {
		// 소진에 실패하면 다시 시도.
		XSYSTEM::xSleep( 3 );
		XInApp::DoConsumeItemAsync( cidsProduct );
	}
}

/**
 @brief 아직 소진시키지 못한 아이템이 있는지 검사한다.
*/
void XInApp::DoCheckUnconsumedItemAsync()
{
#if defined(_VER_ANDROID)
	JniHelper::DoCheckUnconsumedItemAsync();
#elif defined(_VER_IOS)
#elif defined(WIN32)
#else
#error "unknown platform"
#endif 
}
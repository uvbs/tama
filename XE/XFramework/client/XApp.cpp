#include "stdafx.h"
#include "XApp.h"
#include "XClientMain.h"
#include "XEContent.h"
#include "XFramework/XFacebook.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

XClientMain *_APP = NULL;

namespace XE {
//BOOL x_bResumeBlock = FALSE;		// OnResume()이 일어나지 않게 함.
// xResultFacebookProfile x_resultFacebookProfile;
/////////////////////////////////////////////////////////////////////////
XClientMain* GetMain()
{
      return (XClientMain*)_APP;
}

void SetApp( XClientMain *pApp ) {
	_APP = pApp;
}
XEContent* GetGame() {
	return _APP->GetpGame();
}

void RestoreDevice()
{
//		_APP->RestoreDevice();
	XLOGXN("XE::RestoreDevice");
	_APP->m_Restore = XClientMain::xRST_TRUE;
}
// 기기에서 홈버튼으로 나갔을때 호출되는 핸들러
void OnPauseHandler()
{
	if( GetMain() )
		GetMain()->OnPauseHandler();
}
void OnResumeHandler()
{
//	XLOGXN( "XE::OnResumeHandler()" );
	XTRACE( "XE::OnResumeHandler()" );
	RestoreDevice();
	if( GetMain() )
		GetMain()->OnResumeHandler();
}
/*	void OnKeyDown( int keyCode, int keyAction )
{
	if( _APP )
		_APP->OnKeyDown( keyCode, keyAction );
} */
void OnPurchaseFinishedConsume( BOOL bGoogle,
								const char *cJson,
								const char *cidProduct, 
								const char *cToken, 
								const char *cPrice,
								const char *cOrderId,
								const char *cTransactDate )
{
	if( cidProduct )
	{
		CONSOLE("XE::OnPurchaseFinishedConsume:%s", C2SZ(cidProduct) );
		if( GetMain()->GetpGame() )
			GetMain()->GetpGame()->OnPurchaseFinishedConsume( bGoogle,
																cJson,
																cidProduct,
																cToken,
																cPrice,
																cOrderId,
																cTransactDate );
	}
}
void OnPurchaseError( const char *cErr )
{
	if( cErr )
		CONSOLE("XE::OnPurchaseError:%s", C2SZ(cErr) );
	if( GetMain()->GetpGame() )
		GetMain()->GetpGame()->OnPurchaseError( cErr );
}
/**
	구매를 위해 비번까지 입력한 이후 실제 구매요청을 보낸 직후에 호출된다.
	iOS만 적용된다.
	*/
void OnPurchaseStart( const char *cSku )
{
	CONSOLE("XE::OnPurchaseStart");
	if( GetMain()->GetpGame() )
		GetMain()->GetpGame()->OnPurchaseStart( cSku );
}
/*
xRESULT_AUTHEN* GetpResultAuthen() {
	return x_pResultAuthen;
}
void DestroyResultAuthen( void ) {
	SAFE_DELETE( x_pResultAuthen );
}
*/
// 인증서버의 인증결과를 받아옴. xResultAuthen은 범용 구조체
void OnResultLoginAuthen( xRESULT_AUTHEN *pResultAuthen )
{
//		XBREAK( x_pResultAuthen != NULL );
//		x_pResultAuthen = pResultAuthen;
	XBREAK( GetMain()->GetpResultAuthen() != NULL );
	GetMain()->SetpResultAuthen( pResultAuthen );
}
 void SetbResumeBlock( BOOL bBlock ) {
// 	x_bResumeBlock = bBlock;
 }
//  BOOL GetbResumeBlock( void ) {
// // 	return x_bResumeBlock;
//  }

// void cbOnRecvProfileImageByFacebook( const std::string& strFbUserId, DWORD* pImage, int w, int h, int bpp )
// {
// 	XBREAK( bpp != 4 );
// //	XBREAK( GetMain()->GetpGame() == nullptr );
// 	xResult_FacebookProfileImage result( strFbUserId, pImage, w, h );
// 	XLockObj lockObj( &x_resultFacebookProfile.m_Lock );
// 	XTRACE("XE::cbOnRecvProfileImageByFacebook");
// 	if( x_resultFacebookProfile.m_qResult.size() > 0 ) {
// 		XTRACE("fb profile result size=%d", x_resultFacebookProfile.m_qResult.size() );
// 	}
// 	x_resultFacebookProfile.m_qResult.push( result );
// //	XTRACE( "XE::cbOnRecvProfileImageByFacebook 2" );
// //	GetMain()->SetresultFacebookProfileImage( result );
// // 	XFacebook::sGet()->OnRecvProfileImage( strFbUserId );
// // 	GetMain()->GetpGame()->OnRecvProfileImageByFacebook( strFbUserId, pImage, w, h, bpp );
// }

// xResult_FacebookProfileImage
// PopResultFacebookProfileImage() 
// {
// 	XLockObj lockObj( &x_resultFacebookProfile.m_Lock );
// 	if( XE::x_resultFacebookProfile.m_qResult.size() == 0 )
// 		return xResult_FacebookProfileImage();
// 	XTRACE( "XE::PopResultFacebookProfileImage" );
// 	auto result = XE::x_resultFacebookProfile.m_qResult.front();
// 	XE::x_resultFacebookProfile.m_qResult.pop();
// 	return result;
// }

}; // namespace XE


//////////////////////////////////////////////////////////////////////////
XClientMain* XApp::s_pAppMain = NULL;

bool XApp::s_bInitedIAP = false;

XApp::XApp()
{
	Init();
}

void XApp::Destroy()
{
	XBREAK( s_pAppMain == NULL );
	SAFE_DELETE( s_pAppMain );
}

/**
 디바이스의 물리적 해상도를 받는다.
*/
void XApp::Create( XE::xtDevice device, float widthPhy, float heightPhy )
{
	// 자동으로 AppMain을 삭제시켜주기 위해 받아둔다.
	s_pAppMain = OnCreateAppMain( device, widthPhy, heightPhy );
	XBREAK( s_pAppMain == NULL );
}


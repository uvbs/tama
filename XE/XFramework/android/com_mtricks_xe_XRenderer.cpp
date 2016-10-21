#include "stdafx.h"
//#ifdef ANDROID
#include "XFramework/android/com_mtricks_xe_XRenderer.h"
#include <string>
#include "XFramework/client/XApp.h"
#include "XFramework/client/XClientMain.h"
#include "XFramework/XFacebook.h"
#include "JniHelper.h"
#include "XFramework/XInApp.h"
#include <android/log.h>

using namespace std;
//#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,"xuzhu",__VA_ARGS__)
#ifdef _XDEBUG
#define  LOG_TAG    "xuzhu"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGFB(...)  __android_log_print(ANDROID_LOG_DEBUG,"xfacebook",__VA_ARGS__)
#else
#define  LOGD(...)
#define  LOGFB(...)  
#endif

namespace XE {
// #ifdef _XNEW_APP
	void DestroyAppDelegate( void );
// #else
// 	void CreateMain(int width, int height);
// 	void DestroyMain();
// #endif
};

static bool s_bDebugFB = false;

extern "C"
{
// #ifndef _XNEW_APP
// JNIEXPORT void JNICALL
// Java_com_mtricks_xe_XRenderer_nativeInit(JNIEnv *, jobject, jint width, jint height)
// {
// 	if( XE::GetMain() == NULL )
// 	{
// 		LOGD("nativeCreateMain:%dx%d",width, height);
// 		XE::CreateMain(width, height);
// 	} else
// 	{
// 		LOGD("nativeInit:%dx%d",width, height);
// 		XE::OnResumeHandler();
// 	}
// }
// #endif // not _XNEW_APP

JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeFrameMove(JNIEnv *, jobject, jfloat)
{
	XE::GetMain()->FrameMove();
	if( XE::GetMain()->GetbDestroy() )
	{
// #ifdef _XNEW_APP
		//XE::DestroyAppDelegate();
// #else
// 		XE::DestroyMain();
// #endif
		JniHelper::DoExitApp();
	}
}

JNIEXPORT void JNICALL Java_com_mtricks_xe_XRenderer_nativeDraw
  (JNIEnv *, jobject)
{
	XE::GetMain()->Draw();
}

JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeTouchesDown(JNIEnv *, jobject, jint id, jfloat mx, jfloat my)
{
//	LOGD("touch down:%d %d,%d",id, (int)mx, (int)my);
//	XE::GetMain()->OnTouchesDown(id, mx, my);
	XE::GetMain()->OnTouchEvent( XClientMain::xTE_DOWN, id, mx, my);
}

JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeTouchesUp(JNIEnv *, jobject, jint id, jfloat mx, jfloat my)
{
//	LOGD("touch up:id,%d %d,%d", id, (int)mx, (int)my);
//	XE::GetMain()->OnTouchesUp(id, mx, my);
	XE::GetMain()->OnTouchEvent( XClientMain::xTE_UP, id, mx, my);
}

JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeTouchesMove
(JNIEnv *, jobject, jint id, jfloat mx, jfloat my)
{
//	LOGD("touch move:%d %d,%d", id, (int)mx, (int)my);
//	XE::GetMain()->OnTouchesMove(id, mx, my);
	XE::GetMain()->OnTouchEvent( XClientMain::xTE_MOVE, id, mx, my);
}

JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeTouchesZoom(JNIEnv *, jobject, jint id, jfloat mx, jfloat my, jfloat scale)
{
//	LOGD("touch zoom:%d %d,%d - %f",id, (int)mx, (int)my, scale);
	XE::GetMain()->OnTouchEvent( XClientMain::xTE_ZOOM, id, mx, my, scale);
}



JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeOnKeyEvent
(JNIEnv *env, jobject, jint keyCode, jint keyAction )
{
	if( keyAction == 1000 )
	{
		LOGD("keyDown: keyCode=%d, keyAct=%d", keyCode, keyAction);
		XE::GetMain()->OnKeyDown( keyCode );
	} else
	if( keyAction == 1001 )
	{
		LOGD("keyUp: keyCode=%d, keyAct=%d", keyCode, keyAction);
		XE::GetMain()->OnKeyUp( keyCode );
	} else
	{
		LOGD("keyEvent: keyCode=%d, keyAct=%d", keyCode, keyAction);
	}

}


JNIEXPORT void JNICALL Java_com_mtricks_xe_XRenderer_nativeOnPause
(JNIEnv *, jobject)
{
	LOGD("nativeOnPause");
	XE::OnPauseHandler();
}
JNIEXPORT void JNICALL Java_com_mtricks_xe_XRenderer_nativeOnResume
(JNIEnv *, jobject)
{
	LOGD("cpp: nativeOnResume");
	//XE::OnResumeHandler();	//  ϰ ϵ ϱ ʿ ȣϵ ٲ

}
JNIEXPORT void JNICALL Java_com_mtricks_xe_XRenderer_nativeOnDestroy
(JNIEnv *, jobject)
{
	XLOG("cpp: nativeOnDestroy. maybe out of memory?");
	XE::OnPauseHandler();
}

JNIEXPORT
void JNICALL Java_com_mtricks_xe_XRenderer_nativeOnPurchaseFinishedConsume
	(JNIEnv *env, jobject, jstring jstrJson,
							jstring jstridProduct,
							jstring jstrToken,
							jstring jstrPrice,
							jstring jstrOrderId,
							jlong jlongTime)
{
	string strJson = JniHelper::jstring2string( jstrJson );
	string stridProduct = JniHelper::jstring2string( jstridProduct );
	string strToken = JniHelper::jstring2string( jstrToken );
	string strPrice = JniHelper::jstring2string( jstrPrice );
//	long long usTime = jlongTime;
	string strOrderId = JniHelper::jstring2string( jstrOrderId );
	char cBuff[256];
	sprintf_s( cBuff, "%llu", jlongTime );
	string strUsTime = cBuff;
	LOGD("sku:%s\ntoken:%s\nprice%s\ntime=%s\nOrderId=%s", stridProduct.c_str(),
															strToken.c_str(),
															strPrice.c_str(),
															strUsTime.c_str(),
															strOrderId.c_str() );
	XE::OnPurchaseFinishedConsume( TRUE,
									strJson.c_str(),
									stridProduct.c_str(),
									strToken.c_str(),
									strPrice.c_str(),
									strOrderId.c_str(),
									strUsTime.c_str() );
}

// JNIEXPORT
// void JNICALL Java_com_mtricks_xe_XRenderer_nativeGetMemoryInfo
// 	(JNIEnv *env, jobject, jlong jl ,
// 												jstring jstridProduct,
// 												jstring jstrToken,
// 												jstring jstrPrice,
// 												jstring jstrOrderId,
// 												jlong jlongTime)
// {
// 	XE::OnGetMemoryInfo( jl );
// }

JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeOnPurchaseError(JNIEnv *env, jobject, jstring jstrErr )
{
	string strErr = JniHelper::jstring2string( jstrErr );
	LOGD("purchase error:%s", strErr.c_str() );
	XE::OnPurchaseError( strErr.c_str() );
}
// Ini3���� ��� �Ѿ�´�.
// id/pw�����̳� ���̽��� �����̳� ��� �������� �Ѿ�´�.
JNIEXPORT void JNICALL
	Java_com_mtricks_xe_XRenderer_nativeOnResultAuthenIni3
	(JNIEnv *env, jobject, jstring jstrUserId, jstring jstrUserName, jboolean jbByFacebookLogin )
{
	string strUserId = JniHelper::jstring2string( jstrUserId );
	string strUserName = JniHelper::jstring2string( jstrUserName );
	LOGD("nativeOnResultAuthenIni3:id=%s, username=%s, byFacebook=%d", strUserId.c_str(), strUserName.c_str(), (int)jbByFacebookLogin );
	// ������� ��뱸��ü�� ��Ƽ� ���� �Ѱ��ش�.
	XE::xRESULT_AUTHEN *pResult = new XE::xRESULT_AUTHEN;
	pResult->string[0] = strUserId;		// ini3�� ��Ʈ�� 1,2���� ����.
	pResult->string[1] = strUserName;
	pResult->dwValue[0] = (DWORD)jbByFacebookLogin;
	XE::OnResultLoginAuthen( pResult );
}

// ��� Facebook sdk�� ���� ��� �Ѿ�´�.
JNIEXPORT void JNICALL 
	Java_com_mtricks_xe_XRenderer_nativeOnResultAuthenFacebook
	(JNIEnv *env, jobject, jstring jstrUserId, jstring jstrUserName )
{
	string strUserId = JniHelper::jstring2string( jstrUserId );
	string strUserName = JniHelper::jstring2string( jstrUserName );
	LOGD("nativeOnResultAuthenFacebook:id=%s, username=%s", strUserId.c_str(), strUserName.c_str());
	// ������� ��뱸��ü�� ��Ƽ� ���� �Ѱ��ش�.
	XE::xRESULT_AUTHEN *pResult = new XE::xRESULT_AUTHEN;
	pResult->string[0] = strUserId;		// 
	pResult->string[1] = strUserName;
	XE::OnResultLoginAuthen( pResult );
}

JNIEXPORT void JNICALL 
	Java_com_mtricks_xe_XRenderer_nativeOnRecvProfileImage
	(JNIEnv *env, jobject, jstring jstrFbUserId, jintArray ary, jint w, jint h, jint bpp)
{
	if( s_bDebugFB )
		LOGFB("nativeOnRecvProfileImage:recv image binary");
	const std::string strFbUserId = JniHelper::jstring2string( jstrFbUserId );
	if( ary ) {
		jint* pData = env->GetIntArrayElements( ary, 0 );
		if( s_bDebugFB )
			LOGFB("nativeOnRecvProfileImage: get facebook profile image:(%dx%d) bpp=%d", w, h, bpp);
		if( pData && w > 0 && h > 0 ) {
			DWORD *pBuff = new DWORD[ w * h ];
			// RGB포맷을 기기에 맞는 BGR포맷으로 변환.
			XE::ConvertBlockARGB8888ToABGR8888( pBuff, w, h, (DWORD*)pData, w, h );
			XFacebook::sGet()->cbOnRecvProfileImageByFacebook( strFbUserId, pBuff, w, h, bpp );
		} else {
			XFacebook::sGet()->cbOnRecvProfileImageByFacebook( strFbUserId, nullptr, 0, 0, 0 );
		}
		env->ReleaseIntArrayElements( ary, pData, JNI_ABORT );
	} else {
		XFacebook::sGet()->cbOnRecvProfileImageByFacebook( strFbUserId, nullptr, 0, 0, 0 );
	}
}


/**
 @brief InitGoogleIAPAsync의 결과를 받는다.
 consume되지 않은 상품이 있다면 그 정보는 별도로 온다.
*/
#pragma message("define _NEW_INAPP =============================================================")

JNIEXPORT void JNICALL 
	Java_com_mtricks_xe_XRenderer_nativeOnInitGoogleIAPAsync(JNIEnv *env, jobject)
{
	LOGD("excute %s", __FUNCTION__ );
	XInApp::sGet()->cbInitGoogleIAPAsync();
}

/**
 @brief IAP초기화중 소진되지 않은 아이템 발견
*/
JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeOnDetectedUnConsumedItem( JNIEnv *env, jobject
																														, jstring jstrJson
																														, jstring jstrSignature )
{
	LOGD( "excute %s", __FUNCTION__ );
	const std::string strJson = JniHelper::jstring2string( jstrJson );
	const std::string strSignature = JniHelper::jstring2string( jstrSignature );
	LOGD( "%s: json=\"%s\" signature=\"%s\"", __FUNCTION__, strJson.c_str(), strSignature.c_str() );
	XInApp::sGet()->cbDetectedUnConsumedItem( strJson, strSignature );
}

/**
 @brief JniHelper::OnClickedBuyItem의 결과 콜백
 구매요청의 결과가 돌아온다.
 @param jstrJson
*/
JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeOncbOnClickedBuyItem( JNIEnv *env, jobject
																												, jboolean _bOk
																												, jstring jstrJsonReceipt
																												, jstring jstrSignature
																												, jstring jstrIdsProduct )
{
	LOGD( "excute %s", __FUNCTION__ );
	const std::string strJson = JniHelper::jstring2string( jstrJsonReceipt );
	const std::string strSignature = JniHelper::jstring2string( jstrSignature );
	const std::string strIdsProduct = JniHelper::jstring2string( jstrIdsProduct );
	const bool bOk = _bOk;
	LOGD( "%s: receipt=\"%s\" Signature=\"%s\", idsProduct=%s", __FUNCTION__
																														, strJson.c_str()
																														, strSignature.c_str()
																														, strIdsProduct.c_str() );
	// 구매요청의 결과를 돌려준다.
	XInApp::sGet()->cbOnRecvBuyRequest( bOk, strJson, strSignature, strIdsProduct );
}

JNIEXPORT void JNICALL
Java_com_mtricks_xe_XRenderer_nativeOnFinishedConsumeItem( JNIEnv *env, jobject
																												, jboolean _bOk
																												, jstring jstrSku)
{
	LOGD( "callback %s", __FUNCTION__ );
	const bool bOk = _bOk;
	const std::string idsProduct = JniHelper::jstring2string( jstrSku );
	if( bOk )
		LOGD( "%s: finished consume success:%s", __FUNCTION__, idsProduct.c_str() );
	else
		LOGD( "%s: finished consume failed:%s", __FUNCTION__, idsProduct.c_str() );
	// 컨슘 결과를 돌려준다.
	XInApp::sGet()->cbFinishedConsumItem( bOk, idsProduct );
}




} // extern C
//#endif // android
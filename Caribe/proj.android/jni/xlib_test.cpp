#include "stdafx.h"
//#ifdef _VER_ANDROID
#include "../../App/game_src/client/XAppDelegate.h"
#include "android/JniHelper.h"
#include <android/log.h>

#ifdef _XDEBUG
#define  LOG_TAG    "xuzhu"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

#ifdef _XSINGLE
#pragma message("=================single build================================")
#endif


XAppDelegate *APP_DELEGATE = NULL;

extern "C"
{
jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	LOGD("JNI_OnLoad");
    JniHelper::setJavaVM(vm);

    return JNI_VERSION_1_4;
}

//#pragma message("-------------_XNEW_APP skeleton")
JNIEXPORT void JNICALL
	Java_com_mtricks_xe_XRenderer_nativeInit(JNIEnv *, jobject, jint width, jint height)
{
	if( XE::GetMain() == NULL )
	{
		LOGD("nativeCreateMain:%dx%d",width, height);
		XAppDelegate *pDelegate = new XAppDelegate;
		pDelegate->Create( XE::DEVICE_ANDROID, width, height );
	} else
	{
		LOGD("nativeInit:%dx%d",width, height);
		XE::OnResumeHandler();
	}
}

} // extern C

namespace XE
{
void DestroyAppDelegate( void )
{
	SAFE_DELETE( APP_DELEGATE );
}
}
//#endif // _VER_ANDROID
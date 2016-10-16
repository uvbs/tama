#include "stdafx.h"
#ifdef ANDROID
#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <string>
#include "JniHelper.h"
//#include "cocoa/CCString.h"
#include "com_mtricks_xe_Cocos2dxHelper.h"


//#define  LOG_TAG    "xuzhu"
//#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#ifdef _XDEBUG
#define  LOG_TAG    "xuzhu"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...)
#endif

#define  CLASS_NAME "com/mtricks/xe/Cocos2dxHelper"

static EditTextCallback s_pfEditTextCallback = NULL;
static void* s_ctx = NULL;

//using namespace cocos2d;
using namespace std;

extern "C" {
    string g_apkPath;
    
//    JNIEXPORT void JNICALL
//    Java_com_mtricks_xe_Cocos2dxHelper_nativeSetApkPath(JNIEnv*  env, jobject thiz, jstring apkPath) {
    JNIEXPORT void JNICALL
    Java_com_mtricks_xe_Cocos2dxHelper_nativeSetApkPath(JNIEnv *env, jclass, jstring apkPath)
    {
        g_apkPath = JniHelper::jstring2string(apkPath);
    	LOGD("nativeSetApkPath1:%s", g_apkPath.c_str());
    }
    JNIEXPORT void JNICALL Java_com_mtricks_xe_Cocos2dxHelper_nativeSetExternalAssetPath
      (JNIEnv *, jclass, jstring)
    {
    }
    JNIEXPORT void JNICALL Java_com_mtricks_xe_Cocos2dxHelper_nativeSetExternalResourcePath
    (JNIEnv *env, jclass, jstring path)
    {
        const char* cPath = env->GetStringUTFChars(path, NULL);
        JniHelper::setExternalResourcePath(cPath);
        env->ReleaseStringUTFChars(path, cPath);
    }
    JNIEXPORT void JNICALL Java_com_mtricks_xe_Cocos2dxHelper_nativeSetInternalResourcePath
    (JNIEnv *env, jclass, jstring path)
    {
        const char* cPath = env->GetStringUTFChars(path, NULL);
        JniHelper::setInternalResourcePath(cPath);
        env->ReleaseStringUTFChars(path, cPath);
    }
//    JNIEXPORT void JNICALL Java_com_mtricks_xe_Cocos2dxHelper_nativeSetEditTextDialogResult
//      (JNIEnv *, jclass, jbyteArray) {}

		const char * getApkPath()
		{
			return g_apkPath.c_str();
		}

		void showEditTextDialogJNI( const char* pszTitle,
			const char* pszMessage,
			int nInputMode,
			int nInputFlag,
			int nReturnType,
			int nMaxLength,
			EditTextCallback pfEditTextCallback,
			void* ctx )
		{
			if( pszMessage == NULL ) {
				return;
			}

			s_pfEditTextCallback = pfEditTextCallback;
			s_ctx = ctx;

			JniMethodInfo t;
			if( JniHelper::getStaticMethodInfo( t, CLASS_NAME, "showEditTextDialog", "(Ljava/lang/String;Ljava/lang/String;IIII)V" ) )
			{
				jstring stringArg1;

				if( !pszTitle ) {
					stringArg1 = t.env->NewStringUTF( "" );
				}
				else {
					stringArg1 = t.env->NewStringUTF( pszTitle );
				}

				jstring stringArg2 = t.env->NewStringUTF( pszMessage );

				t.env->CallStaticVoidMethod( t.classID, t.methodID, stringArg1, stringArg2, nInputMode, nInputFlag, nReturnType, nMaxLength );

				t.env->DeleteLocalRef( stringArg1 );
				t.env->DeleteLocalRef( stringArg2 );
				t.env->DeleteLocalRef( t.classID );
				LOGD( "call showEditTextDialogJNI" );
			}
			else
				LOGD( "failed call: showEditTextDialogJNI" );
		}

		void Java_com_mtricks_xe_Cocos2dxHelper_nativeSetEditTextDialogResult( JNIEnv * env, jobject obj, jbyteArray text )
		{
			jsize  size = env->GetArrayLength( text );
			LOGD( "EditBox Result: %d", size );
			if( size > 0 )
			{
				jbyte * data = (jbyte*)env->GetByteArrayElements( text, 0 );
				char* pBuf = (char*)malloc( size + 1 );
				if( pBuf != NULL )
				{
					memcpy( pBuf, data, size );
					pBuf[ size ] = '\0';
					// pass data to edittext's delegate
					if( s_pfEditTextCallback ) s_pfEditTextCallback( pBuf, s_ctx );
					free( pBuf );
				}
				env->ReleaseByteArrayElements( text, data, 0 );
			}
			else {
				if( s_pfEditTextCallback ) s_pfEditTextCallback( "", s_ctx );
			}
		}
	
}

#endif // android
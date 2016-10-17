#include "SimpleAudioEngineJni.h"
#include "XFramework/android/JniHelper.h"

#include <android/log.h>

#define  LOG_TAG    "xuzhu snd"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  CLASS_NAME "com/mtricks/xe/Cocos2dxHelper"

namespace COCOS_SIMPLE_AUDIO {
typedef struct JniMethodInfo_
{
	JNIEnv *    env;
	jclass      classID;
	jmethodID   methodID;
} JniMethodInfo;


extern "C"
{
// get env and cache it
static JNIEnv* getJNIEnv( void )
{

	JavaVM* jvm = JniHelper::getJavaVM();
	if( NULL == jvm ) {
		LOGD( "Failed to get JNIEnv. JniHelper::getJavaVM() is NULL" );
		return NULL;
	}

	JNIEnv *env = NULL;
	// get jni environment
	jint ret = jvm->GetEnv( (void**)&env, JNI_VERSION_1_4 );

	switch( ret ) {
	case JNI_OK:
		// Success!
		return env;

	case JNI_EDETACHED:
		// Thread not attached

		// TODO : If calling AttachCurrentThread() on a native thread
		// must call DetachCurrentThread() in future.
		// see: http://developer.android.com/guide/practices/design/jni.html

		if( jvm->AttachCurrentThread( &env, NULL ) < 0 ) {
			LOGD( "Failed to get the environment using AttachCurrentThread()" );
			return NULL;
		} else {
			// Success : Attached and obtained JNIEnv!
			return env;
		}

	case JNI_EVERSION:
		// Cannot recover from this error
		LOGD( "JNI interface version 1.4 not supported" );
	default:
		LOGD( "Failed to get the environment using GetEnv()" );
		return NULL;
	}
}

// get class and make it a global reference, release it at endJni().
static jclass getClassID( JNIEnv *pEnv )
{
	jclass ret = pEnv->FindClass( CLASS_NAME );
	if( !ret ) {
		LOGD( "Failed to find class of %s", CLASS_NAME );
	}

	return ret;
}

static bool getStaticMethodInfo( JniMethodInfo &methodinfo, const char *methodName, const char *paramCode )
{
	jmethodID methodID = 0;
	JNIEnv *pEnv = 0;
	bool bRet = false;

	do {
		pEnv = getJNIEnv();
		if( !pEnv ) {
			break;
		}

		jclass classID = getClassID( pEnv );

		methodID = pEnv->GetStaticMethodID( classID, methodName, paramCode );
		if( !methodID ) {
			LOGD( "Failed to find static method id of %s", methodName );
			break;
		}

		methodinfo.classID = classID;
		methodinfo.env = pEnv;
		methodinfo.methodID = methodID;

		bRet = true;
	} while( 0 );

	return bRet;
}

void preloadBackgroundMusicJNI( const char *path )
{
	LOGD( "%s:%s", __FUNCTION__, path );
	// void playBackgroundMusic(String,boolean)
	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "preloadBackgroundMusic", "(Ljava/lang/String;)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	jstring stringArg = methodInfo.env->NewStringUTF( path );
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, stringArg );
	methodInfo.env->DeleteLocalRef( stringArg );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void playBackgroundMusicJNI( const char *path, bool isLoop )
{
	LOGD( "%s:%s", __FUNCTION__, path );
	// void playBackgroundMusic(String,boolean)

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "playBackgroundMusic", "(Ljava/lang/String;Z)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	jstring stringArg = methodInfo.env->NewStringUTF( path );
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, stringArg, isLoop );
	methodInfo.env->DeleteLocalRef( stringArg );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void stopBackgroundMusicJNI()
{
	LOGD( "%s", __FUNCTION__ );
	// void stopBackgroundMusic()

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "stopBackgroundMusic", "()V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void pauseBackgroundMusicJNI()
{
	LOGD( "%s", __FUNCTION__ );
	// void pauseBackgroundMusic()

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "pauseBackgroundMusic", "()V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void resumeBackgroundMusicJNI()
{
	LOGD( "%s", __FUNCTION__ );
	// void resumeBackgroundMusic()

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "resumeBackgroundMusic", "()V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void rewindBackgroundMusicJNI()
{
	LOGD( "%s", __FUNCTION__ );
	// void rewindBackgroundMusic()

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "rewindBackgroundMusic", "()V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

bool isBackgroundMusicPlayingJNI()
{
	LOGD( "%s", __FUNCTION__ );
	// boolean rewindBackgroundMusic()

	JniMethodInfo methodInfo;
	jboolean ret = false;

	if( !getStaticMethodInfo( methodInfo, "isBackgroundMusicPlaying", "()Z" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return ret;
	}

	ret = methodInfo.env->CallStaticBooleanMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );

	return ret;
}

float getBackgroundMusicVolumeJNI()
{
	LOGD( "%s", __FUNCTION__ );
	// float getBackgroundMusicVolume()

	JniMethodInfo methodInfo;
	jfloat ret = -1.0;

	if( !getStaticMethodInfo( methodInfo, "getBackgroundMusicVolume", "()F" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return ret;
	}

	ret = methodInfo.env->CallStaticFloatMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );

	return ret;
}

void setBackgroundMusicVolumeJNI( float volume )
{
	LOGD( "%s:vol=%.1f", __FUNCTION__, volume );
	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "setBackgroundMusicVolume", "(F)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, volume );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

unsigned int playEffectJNI( const char* path, bool bLoop )
{
	// int playEffect(String)

	JniMethodInfo methodInfo;
	int ret = 0;

	if( !getStaticMethodInfo( methodInfo, "playEffect", "(Ljava/lang/String;Z)I" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return ret;
	}

	jstring stringArg = methodInfo.env->NewStringUTF( path );
	ret = methodInfo.env->CallStaticIntMethod( methodInfo.classID, methodInfo.methodID, stringArg, bLoop );
	methodInfo.env->DeleteLocalRef( stringArg );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );

	return (unsigned int)ret;
}

void stopEffectJNI( unsigned int nSoundId )
{
	// void stopEffect(int)

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "stopEffect", "(I)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, (int)nSoundId );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void endJNI()
{
	// void end()

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "end", "()V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

float getEffectsVolumeJNI()
{
	// float getEffectsVolume()

	JniMethodInfo methodInfo;
	jfloat ret = -1.0;

	if( !getStaticMethodInfo( methodInfo, "getEffectsVolume", "()F" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return ret;
	}

	ret = methodInfo.env->CallStaticFloatMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );

	return ret;
}

void setEffectsVolumeJNI( float volume )
{
	// void setEffectsVolume(float)
	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "setEffectsVolume", "(F)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, volume );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void setEffectsVolumeByStreamJNI( unsigned int idStream, float volume )
{
	// void setEffectsVolume(float)
	JniMethodInfo methodInfo;
	if( !getStaticMethodInfo( methodInfo, "setEffectsVolumeByStream", "(IF)V" ) ) {
		LOGD( "jni call failed:%s:%s", __FUNCTION__, "setEffectsVolumeByStream" );
		return;
	}

	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, idStream, volume );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void preloadEffectJNI( const char *path )
{
	// void preloadEffect(String)

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "preloadEffect", "(Ljava/lang/String;)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	jstring stringArg = methodInfo.env->NewStringUTF( path );
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, stringArg );
	methodInfo.env->DeleteLocalRef( stringArg );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void unloadEffectJNI( const char* path )
{
	// void unloadEffect(String)

	JniMethodInfo methodInfo;

	if( !getStaticMethodInfo( methodInfo, "unloadEffect", "(Ljava/lang/String;)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}

	jstring stringArg = methodInfo.env->NewStringUTF( path );
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, stringArg );
	methodInfo.env->DeleteLocalRef( stringArg );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void pauseEffectJNI( unsigned int nSoundId )
{
	JniMethodInfo methodInfo;
	if( !getStaticMethodInfo( methodInfo, "pauseEffect", "(I)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, (int)nSoundId );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void pauseAllEffectsJNI()
{
	JniMethodInfo methodInfo;
	if( !getStaticMethodInfo( methodInfo, "pauseAllEffects", "()V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void resumeEffectJNI( unsigned int nSoundId )
{
	JniMethodInfo methodInfo;
	if( !getStaticMethodInfo( methodInfo, "resumeEffect", "(I)V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID, (int)nSoundId );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void resumeAllEffectsJNI()
{
	JniMethodInfo methodInfo;
	if( !getStaticMethodInfo( methodInfo, "resumeAllEffects", "()V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}

void stopAllEffectsJNI()
{
	JniMethodInfo methodInfo;
	if( !getStaticMethodInfo( methodInfo, "stopAllEffects", "()V" ) ) {
		LOGE( "error01:%s", __FUNCTION__ );
		return;
	}
	methodInfo.env->CallStaticVoidMethod( methodInfo.classID, methodInfo.methodID );
	methodInfo.env->DeleteLocalRef( methodInfo.classID );
}
}
}; // COCOS_SIMPLE_AUDIO

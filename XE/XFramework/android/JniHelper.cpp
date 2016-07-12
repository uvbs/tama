/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
static {};
   ()V   
Hello();
   ()V   
public static void coo( java.lang.String, java.lang.String );
   (Ljava/lang/String;Ljava/lang/String;)V   
private void foo( java.lang.String, java.lang.String );
   (Ljava/lang/String;Ljava/lang/String;)V   
public native boolean getBoolean( boolean );
   (Z)Z   
public native byte getByte( byte );
   (B)B   
public native char getChar( char );
   (C)C   
public native double getDouble( double );
   (D)D   
public native float getFloat( float );
   (F)F   
public native java.lang.String getHello( java.lang.String );
   (Ljava/lang/String;)Ljava/lang/String;   
public native int getInt( int );
   (I)I   
public native int getIntArray( int[] )[];
   ([I)[I   
public native long getLong( long );
   (J)J   
public native short getShort( short );
   (S)S   
public static void main( java.lang.String[] );
   ([Ljava/lang/String;)V   
****************************************************************************/
#include "stdafx.h"
#include "XFramework/XReceiverCallback.h"
// versionAndroid.h¥¬ XE∞° æ∆¥— ∞‘¿”¬  µ∆ƒ¿Œ¿Ãπ«∑Œ ø©±‚º≠ ¿ŒΩƒ¿Ã µ«¡ˆ æ ¿Ω.
#ifdef _VER_ANDROID
#include "JniHelper.h"
#include <android/log.h>
#include <string.h>

#ifdef _XDEBUG
#define  LOG_TAG    "xuzhu"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGFB(...)  __android_log_print(ANDROID_LOG_DEBUG,"xfacebook",__VA_ARGS__)
#else
#define  LOGD(...) 
#define  LOGI(...) 
#define  LOGE(...) 
#define  LOGFB(...)
#endif

//#define  BLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define	 BLOGD(...)	(0)

//#define JAVAVM    cocos2d::JniHelper::getJavaVM()
#define JAVAVM    JniHelper::getJavaVM()

//using namespace std;

extern "C"
{

//////////////////////////////////////////////////////////////////////////
// java vm helper function
//////////////////////////////////////////////////////////////////////////

static bool getEnv(JNIEnv **env)
{
	bool bRet = false;

	do {
		if( JAVAVM->GetEnv( (void**)env, JNI_VERSION_1_4 ) != JNI_OK ) {
			LOGD( "Failed to get the environment using GetEnv()" );
			break;
		}

		if( JAVAVM->AttachCurrentThread( env, 0 ) < 0 ) {
			LOGD( "Failed to get the environment using AttachCurrentThread()" );
			break;
		}

		bRet = true;
	} while( 0 );

	return bRet;
}
static jclass getClassID_(const char *className, JNIEnv *env)
{
	JNIEnv *pEnv = env;
	jclass ret = 0;

	do {
		if( !pEnv ) {
			if( !getEnv( &pEnv ) ) {
				break;
			}
		}

		BLOGD( "ret = pEnv->FindClass(className);[%s]", className );
		ret = pEnv->FindClass( className );
		BLOGD( "ret = pEnv->FindClass(className);[]2" );
		if( !ret ) {
			LOGD( "Failed to find class of %s", className );
			break;
		}
	} while( 0 );

	return ret;
}
static bool getStaticMethodInfo_(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{
	jmethodID methodID = 0;
	JNIEnv *pEnv = 0;
	bool bRet = false;

	do {
		if( !getEnv( &pEnv ) ) {
			break;
		}

		jclass classID = getClassID_( className, pEnv );

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

static std::string jstring2string_( jstring jstr )
{
	if( jstr == NULL ) {
		return "";
	}
	JNIEnv *env = 0;
	if( !getEnv( &env ) ) {
		return 0;
	}
	const char* chars = env->GetStringUTFChars( jstr, NULL );
	std::string ret( chars );
	env->ReleaseStringUTFChars( jstr, chars );
	return ret;
}
//
} // extern "C"

//NS_CC_BEGIN

JavaVM* JniHelper::m_psJavaVM = NULL;
std::string JniHelper::m_externalResourcePath;
std::string JniHelper::m_internalResourcePath;

JavaVM* JniHelper::getJavaVM()
{
    return m_psJavaVM;
}

void JniHelper::setJavaVM(JavaVM *javaVM)
{
    m_psJavaVM = javaVM;
}
void JniHelper::setExternalResourcePath(const char * cPath)
{
    m_externalResourcePath = cPath;
}

const char* JniHelper::getExternalResourcePath()
{
    return m_externalResourcePath.c_str();
}
void JniHelper::setInternalResourcePath(const char * cPath)
{
    m_internalResourcePath = cPath;
}

const char* JniHelper::getInternalResourcePath()
{
    return m_internalResourcePath.c_str();
}

bool JniHelper::getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{
    return getStaticMethodInfo_(methodinfo, className, methodName, paramCode);
}

/*
string JniHelper::m_externalAssetPath;



jclass JniHelper::getClassID(const char *className, JNIEnv *env)
{
    return getClassID_(className, env);
}

bool JniHelper::getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{
    return getMethodInfo_(methodinfo, className, methodName, paramCode);
}
*/
std::string JniHelper::jstring2string(jstring str)
{
    return jstring2string_(str);
}
char* JniHelper::__x_unicode_test()
{
/*    JNIEnv *_env = 0;
    BLOGD("x1");
    if (! getEnv(&_env))
    {
        return 0;
    }
    BLOGD("x2");
    jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	char *str = "ÔøΩÔøΩÔøΩ⁄µÔøΩ ÔøΩÔøΩ ÔøΩ—±ÔøΩ ÔøΩÔøΩÔøΩ⁄¥ÔøΩ";
    BLOGD("x3");
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_GetUnicodeString",  "([B)[B");
//	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_GetUnicodeString",  "([BLjava/lang/String;)[B");
//	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_test",  "([B)[B");
	if( mid == NULL ) {
		BLOGD("not found");
	}
    BLOGD("x4");
	jbyteArray toJavaStr = _env->NewByteArray(strlen(str));
    BLOGD("x41");
	_env->SetByteArrayRegion(toJavaStr, 0, strlen(str), (jbyte*)str);
    LOGD("x42");
	jbyteArray uniByteArray = (jbyteArray)_env->CallStaticObjectMethod(_javaClass, mid, toJavaStr);
    BLOGD("x5");

	// javaÔøΩÔøΩÔøΩÔøΩ ÔøΩ—æÔøΩÔø?byte arrayÔøΩÔøΩ char* ÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	jint len = _env->GetArrayLength(uniByteArray);
	jbyte* uniBytes = _env->GetByteArrayElements(uniByteArray, NULL);

    BLOGD("x6");
	char* uniChars = new char[len+1];
	strncpy(uniChars, (char*)uniBytes, len);
	uniChars[len] = '\0';
	//
	//delete [] uniChars;

    BLOGD("x7");
	_env->ReleaseByteArrayElements(uniByteArray, uniBytes, JNI_ABORT);
    BLOGD("x8:%s", uniChars);
	return uniChars; */
    return NULL;
}
char* JniHelper::Create_UTF16_To_UTF8( char *pOutUtf8, const unsigned short *utf16, int sizeBuff )
{
    JNIEnv *_env = 0;
    BLOGD("x1");
    if (! getEnv(&_env))
    {
        return 0;
    }
    BLOGD("x2");
    jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	char *str = (char*)utf16;
    BLOGD("x3");
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_utf16_to_utf8",  "([B)[B");
	if( mid == NULL ) {
		BLOGD("not found");
	}
    BLOGD("x4");
	jbyteArray toJavaStr = _env->NewByteArray(strlen(str));
    BLOGD("x41");
	_env->SetByteArrayRegion(toJavaStr, 0, strlen(str), (jbyte*)str);
    BLOGD("x42");
	jbyteArray uniByteArray = (jbyteArray)_env->CallStaticObjectMethod(_javaClass, mid, toJavaStr);
    BLOGD("x5");

	// javaÔøΩÔøΩÔøΩÔøΩ ÔøΩ—æÔøΩÔø?byte arrayÔøΩÔøΩ char* ÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	jint len = _env->GetArrayLength(uniByteArray);
	jbyte* uniBytes = _env->GetByteArrayElements(uniByteArray, NULL);

    BLOGD("x6");
//	char* uniChars = new char[len+1];
//	strncpy(uniChars, (char*)uniBytes, len);
//	uniChars[len] = '\0';
    if( len >= sizeBuff )
    {
        XALERT("[%s] string length(%d) is too long", (char*)uniBytes, (int)len );
        return NULL;
    }
    strcpy_s( pOutUtf8, sizeBuff, (char*)uniBytes );
	//
	//delete [] uniChars;

    BLOGD("x7");
	_env->ReleaseByteArrayElements(uniByteArray, uniBytes, JNI_ABORT);
//    LOGD("x8:%s, %d", pOutUtf8, (int)len );
    BLOGD("x8" );
	return pOutUtf8;
}
char* JniHelper::Create_EUCKR_To_UTF8( char *pOutUtf8, const char *euckr, int sizeBuff )
{
    JNIEnv *_env = 0;
    if (! getEnv(&_env))
    {
        return 0;
    }
		BLOGD("getClassID_(com/mtricks/xe/Cocos2dxHelper, _env);");
//    jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
		jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
		BLOGD("getClassID_(com/mtricks/xe/Cocos2dxHelper, _env); 2");
	char *str = (char*)euckr;
	BLOGD("->GetStaticMethodID(_javaClass, ");
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_euckr_to_utf8",  "([B)[B");
	BLOGD("->GetStaticMethodID(_javaClass, 2");
	if( mid == NULL ) {
		LOGD("not found JNI");
		return nullptr;
	}
	jbyteArray toJavaStr = _env->NewByteArray(strlen(str));
	_env->SetByteArrayRegion(toJavaStr, 0, strlen(str), (jbyte*)str);
	jbyteArray uniByteArray = (jbyteArray)_env->CallStaticObjectMethod(_javaClass, mid, toJavaStr);
    
	// javaÔøΩÔøΩÔøΩÔøΩ ÔøΩ—æÔøΩÔø?byte arrayÔøΩÔøΩ char* ÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ
	jint len = _env->GetArrayLength(uniByteArray);
	jbyte* uniBytes = _env->GetByteArrayElements(uniByteArray, NULL);
    
    //BLOGD("jni:len:%d", len);
//	char* uniChars = new char[len+1];
//	strncpy(uniChars, (char*)uniBytes, len);
//	uniChars[len] = '\0';
    if( len >= sizeBuff )
    {
        XALERT("[%s] string length(%d) is too long", (char*)uniBytes, (int)len );
        return NULL;
    }
    strcpy_s( pOutUtf8, sizeBuff, (char*)uniBytes );
    pOutUtf8[len] = 0;  // uniBytesÔøΩÔøΩ ÔøΩŒπÔøΩÔøΩ⁄∞ÔøΩ ÔøΩ»µÔøΩÔø?ÔøΩ÷¥ÔøΩÔøΩÔøΩ
	//
//    BLOGD("euckr-utf8:%s, %d", pOutUtf8, (int)len);
	_env->ReleaseByteArrayElements(uniByteArray, uniBytes, JNI_ABORT);
	return pOutUtf8;
}
//cPathÔøΩÔøΩ ÔøΩ÷¥ÔøΩÔøΩÔøΩ ÔøΩÀªÔøΩÔøΩ—¥ÔøΩ.
/*BOOL JniHelper::IsExistDir( const char *cPath )
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getCocos2dxPackageName", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        CCString *ret = new CCString(JniHelper::jstring2string(str).c_str());
        ret->autorelease();
        t.env->DeleteLocalRef(str);

        return ret->m_sString.c_str();
    }

}
 */

/**
 @brief ±∏±€IAPΩ√Ω∫≈€ø° cidProductæ∆¿Ã≈€¿« ±∏∏≈∏¶ ø‰√ª«‘.
*/
// void JniHelper::OnClickedBuyItem( const char *cidProduct, const std::string& strPayload  )
// {
// 	JNIEnv *_env = 0;
// 	if (! getEnv(&_env)){
// 		return;
// 	}
// 	if( XBREAK( cidProduct == NULL ) )
// 		return;
// 	LOGD("OnClickedBuyItem:%s",cidProduct);
// 	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
// 	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_OnClickedBuyItem",  "(Ljava/lang/String;)V");
// 	if( mid == NULL ) {
// 		LOGD("not found java function");
// 	} else	{
// 		LOGD("get Callback_OnClickedBuyItem");
// 		jstring jstridProduct = _env->NewStringUTF(cidProduct);
// 		_env->CallStaticObjectMethod(_javaClass, mid, jstridProduct);
// 		LOGD("call Callback_OnClickedBuyItem");
// 	}
// }

void JniHelper::OnClickedBuyItem( const char *cidProduct, const std::string& strPayload  )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return;
	}
	if( XBREAK( strPayload.empty() ) )
		return;
	LOGD("%s:%s", __FUNCTION__, cidProduct );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "Callback_OnClickedBuyItem";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc, "(Ljava/lang/String;Ljava/lang/String;)V");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
		LOGD("call [%s]", cFunc);
 		jstring jstridProduct = _env->NewStringUTF(cidProduct);
		jstring jstrPayload = _env->NewStringUTF(strPayload.c_str());
		_env->CallStaticObjectMethod(_javaClass, mid, jstridProduct, jstrPayload );
		LOGD("successed call [%s]", cFunc);
	}
}

void JniHelper::DoLoginFacebook()
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))   {
		return;
	}
	LOGFB("%s", __FUNCTION__);
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const std::string strFunc = "Callback_DoLoginFacebook";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, strFunc.c_str(), "()V");
	if( mid == nullptr ) {
		LOGFB("not found java function:%s", strFunc.c_str() );
	} else	{
		LOGFB("call %s",  strFunc.c_str() );
		_env->CallStaticObjectMethod(_javaClass, mid);
		LOGFB("called %s", strFunc.c_str() );
	}
}

void JniHelper::DoLogoutFacebook()
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))   {
		return;
	}
	LOGFB("%s", __FUNCTION__);
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const std::string strFunc = "Callback_DoLogoutFacebook";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, strFunc.c_str(), "()V");
	if( mid == nullptr ) {
		LOGFB("not found java function:%s", strFunc.c_str() );
	} else	{
		LOGFB("call %s",  strFunc.c_str() );
		_env->CallStaticObjectMethod(_javaClass, mid);
		LOGFB("called %s", strFunc.c_str() );
	}
}

// ÔøΩ¬±ÔøΩ ini3ÔøΩÔøΩÔø?ÔøΩÔøΩÔøΩÃΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩŒ±ÔøΩÔøΩÔøΩ
void JniHelper::DoLoginIni3ByFacebook( void )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))
	{
		return;
	}
	LOGD("DoLoginIni3ByFacebook");
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_DoLoginIni3ByFacebook",  "()V");
	if( mid == NULL ) {
		LOGD("not found java function");
	} else
	{
		LOGD("get DoLoginIni3ByFacebook");
		_env->CallStaticObjectMethod(_javaClass, mid);
		LOGD("call DoLoginIni3ByFacebook");
	}

}

// ÔøΩŒµÔøΩÔøΩ◊Ω√æÔøΩ LytoÔøΩÔøΩÔø?ÔøΩÔøΩÔøΩÃΩÔøΩÔøΩÔøΩ ÔøΩÔøΩÔøΩÔøΩ ÔøΩŒ±ÔøΩÔøΩÔøΩ
void JniHelper::DoLoginLytoByFacebook( void )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))
	{
		return;
	}
	LOGFB("DoLoginLytoByFacebook");
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_DoLoginLytoByFacebook",  "()V");
	if( mid == NULL ) {
		LOGD("not found java function");
	} else
	{
		LOGD("get DoLoginLytoByFacebook");
		_env->CallStaticObjectMethod(_javaClass, mid);
		LOGD("call DoLoginLytoByFacebook");
	}

}
/**
 @brief «¡∑Œ« ªÁ¡¯¿ª ∫Òµø±‚∑Œ ø‰√ª«—¥Ÿ.
 ªÁ¡¯¿Ã µµ¬¯«œ∏È XE::cbOnRecvProfileImageByFacebook∑Œ ¿¿¥‰¿ª πﬁ¥¬¥Ÿ.
*/
void JniHelper::GetUserProfileImage(const char *cUserID)
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))	{
		return;
	}
	LOGFB("JniHelper::GetUserProfileImage");
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "Callback_GetUserProfileImage";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc,  "(Ljava/lang/String;)V");
	if( mid == NULL ) {
		LOGFB("not found java function:%s", cFunc);
	} else	{
		LOGFB("get %s", cFunc);
		jstring jstrID = _env->NewStringUTF(cUserID);
		_env->CallStaticObjectMethod(_javaClass, mid, jstrID);
		LOGFB("called %s", cFunc );
	}
}

void JniHelper::SendCertificationIni3( const char *cID, const char *cPassword )
{
    JNIEnv *_env = 0;
    if (! getEnv(&_env))    {
        return;
    }
    if( XBREAK( cID == NULL || cPassword == NULL ) )
    	return;
	LOGD("SendCertificationIni3:%s, %s",cID, cPassword);
    jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_OnSendAuthenIni3ByIDPW",  "(Ljava/lang/String;Ljava/lang/String;)V");
	if( mid == NULL ) {
		LOGD("not found java function");
	} else
	{
		LOGD("get Callback_OnSendAuthenIni3ByIDPW");
	    jstring jstrID = _env->NewStringUTF(cID);
	    jstring jstrPassword = _env->NewStringUTF(cPassword);
		_env->CallStaticObjectMethod(_javaClass, mid, jstrID, jstrPassword);
		LOGD("call Callback_OnSendAuthenIni3ByIDPW");
	}

}

const char* JniHelper::GetDevideID( char *pOut, int size )
{
    JNIEnv *_env = 0;
    if (! getEnv(&_env))    {
        return NULL;
    }

    jclass _javaClass = getClassID_("com/mtricks/xe/MyMainActivity", _env);
    jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_getDeviceID",  "");
	if( mid == NULL ) {
		LOGD("not found java function");
	} else
	{
		jstring str = (jstring)_env->CallStaticObjectMethod(_javaClass, mid);
		const char *msg = _env->GetStringUTFChars(str,0);
		strcpy_s( pOut, size, msg );
		LOGD("call getDeviceID:%s", msg);
		return pOut;
	}
	return NULL;
}

void JniHelper::GoURL(const char *url)
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))
	{
		return;
	}
	if( XBREAK( url == NULL ) )
		return;
	LOGD("GoURL:%s", url);
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_goURL",  "(Ljava/lang/String)V");
	if( mid == NULL ) {
		LOGD("not found java function");
	} else
	{
		LOGD("get Callback_goURL");
		jstring jstrURL = _env->NewStringUTF(url);
		_env->CallStaticObjectMethod(_javaClass, mid, jstrURL);
		LOGD("call goURL");
		}
}

int JniHelper::RenameFile( const char *cFileOld, const char *cFileNew )
{
    JNIEnv *_env = 0;
    if (! getEnv(&_env))
    {
        return 0;
    }
    if( XBREAK( cFileOld == NULL || cFileNew == NULL ) )
    	return 0;
	LOGD("RenameFile:%s, %s",cFileOld, cFileNew);
    jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_renameFile",  "(Ljava/lang/String;Ljava/lang/String;)I");
	if( mid == NULL ) {
		LOGD("not found java function");
		return 0;
	} else
	{
		LOGD("get Callback_renameFile");
	    jstring jstrFileOld = _env->NewStringUTF(cFileOld);
	    jstring jstrFileNew = _env->NewStringUTF(cFileNew);
		_env->CallStaticObjectMethod(_javaClass, mid, jstrFileOld, jstrFileNew);
		LOGD("call Callback_renameFile");
	}
	return 1;
}

void JniHelper::DoExitApp( void )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))
	{
		return;
	}
	LOGD("DoExitApp");
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_DoExitApp",  "()V");
	if( mid == NULL ) {
		LOGD("not found java function");
	} else
	{
		LOGD("get DoExitApp");
		_env->CallStaticObjectMethod(_javaClass, mid);
		LOGD("call DoExitApp");
	}

}


bool JniHelper::Check3G()
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))
	{
		return NULL;
	}

	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_check3G",  "()Z");
	if( mid == NULL ) {
		LOGD("not found java function");
	} else
	{
		jboolean r = (jboolean)_env->CallStaticBooleanMethod(_javaClass, mid);
		LOGD("call check3G:%d", (int)r);
		return r;
	}
	return false;
}

bool JniHelper::CheckWiFi()
{
	JNIEnv *_env = 0;
		if (! getEnv(&_env))
		{
			return NULL;
		}

		jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
		jmethodID mid = _env->GetStaticMethodID(_javaClass, "Callback_checkWiFi",  "()Z");
		if( mid == NULL ) {
			LOGD("not found java function");
		} else
		{
			jboolean r = (jboolean)_env->CallStaticBooleanMethod(_javaClass, mid);
			LOGD("call checkWiFi:%d", (int)r);
			return r;
		}
		return false;
}

/*
void JniHelper::LoadURL(const char *url)
{
	JNIEnv *_env = 0;
	if (!getEnv(&_env))
	{
		return;
	}

	jclass _javaClass = getClassID_("com/example/gltest2/MyMainActivity", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "LoadURL", "(Ljava/lang/String)V");
	if (mid == NULL) {
		LOGD("not found java function");
	}
	else
	{
		jstring jstrUrl = _env->NewStringUTF(url);
		_env->CallStaticBooleanMethod(_javaClass, mid, jstrUrl);
		LOGD("call LoadURL");
		return;
	}
	return;
}
*/

void JniHelper::LoadMarketURL( const char *url )
{
	JNIEnv *_env = 0;
	if (!getEnv(&_env))
	{
		return;
	}
	if (XBREAK(url == NULL))
		return;

	LOGD("LoadMarketURL:%s", url);
	jclass _javaClass = getClassID_("com/gemtree/caribe/MyMainActivity", _env);
	jmethodID mid = _env->GetStaticMethodID(_javaClass, "LoadMarketURL", "(Ljava/lang/String;)V");
	if (mid == NULL) {
		LOGD("not found java function");
	}
	else
	{
		LOGD("get ");
		jstring jstrurl = _env->NewStringUTF(url);
		_env->CallStaticObjectMethod(_javaClass, mid, jstrurl);
		LOGD("call LoadMarketURL");
	}
}

/**
 @brief ±∏±€ ¿Œæ€Ω√Ω∫≈€ √ ±‚»≠
 IAPΩ√Ω∫≈€ º¬æ˜
 ±∏∏≈∏Ò∑œ »Æ¿Œ
 πÃº“¡¯≈€ consume
*/
void JniHelper::InitGoogleIAPAsync( const std::string& strPublicKey )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return;
	}
	if( XBREAK( strPublicKey.empty() ) )
		return;
	LOGD("%s:", __FUNCTION__ );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "Callback_InitGoogleIAP";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc, "(Ljava/lang/String;)V");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
		LOGD("%s:call [%s]", __FUNCTION__, cFunc);
		jstring jstrPublicKey = _env->NewStringUTF(strPublicKey.c_str());
		_env->CallStaticObjectMethod(_javaClass, mid, jstrPublicKey );
		LOGD("%s:successed call [%s]", __FUNCTION__, cFunc);
	}
}
/**
 @brief idsProduct∏¶ ∫Òµø±‚∑Œ º“¡¯Ω√≈≤¥Ÿ.
*/
void JniHelper::DoConsumeItemAsync( const std::string& idsProduct )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return;
	}
	if( XBREAK( idsProduct.empty() ) )
		return;
	LOGD("%s:", __FUNCTION__ );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "Callback_DoConsumeItem";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc, "(Ljava/lang/String;)V");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
		LOGD("%s:call [%s]", __FUNCTION__, cFunc);
		jstring jstrIdsProduct = _env->NewStringUTF(idsProduct.c_str());
		_env->CallStaticObjectMethod(_javaClass, mid, jstrIdsProduct );
		LOGD("%s:successed call [%s]", __FUNCTION__, cFunc);
	}
}

void JniHelper::DoCheckUnconsumedItemAsync()
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return;
	}
	LOGD("%s:", __FUNCTION__ );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "Callback_CheckUnConsumedItem";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc, "()V");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
		LOGD("%s:call [%s]", __FUNCTION__, cFunc);
		_env->CallStaticObjectMethod(_javaClass, mid );
		LOGD("%s:successed call [%s]", __FUNCTION__, cFunc);
	}
}

void JniHelper::ShowDialog( const std::string& strTitle, const std::string& strText )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return;
	}
// 	if( XBREAK( idsProduct.empty() ) )
// 		return;
//	LOGD("%s:", __FUNCTION__ );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "showDialog";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc, "(Ljava/lang/String;Ljava/lang/String;)V");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
//		LOGD("%s:call [%s]", __FUNCTION__, cFunc);
		jstring jstrTitle = _env->NewStringUTF(strTitle.c_str());
		jstring jstrText = _env->NewStringUTF(strText.c_str());
		_env->CallStaticObjectMethod(_javaClass, mid, jstrTitle, jstrText );
//		LOGD("%s:successed call [%s]", __FUNCTION__, cFunc);
	}
}

void JniHelper::SoftnyxLogin( )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return;
	}
	LOGD("%s:", __FUNCTION__ );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "Callback_SoftnyxLogin";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc, "()V");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
		LOGD("%s:call [%s]", __FUNCTION__, cFunc);
		_env->CallStaticObjectMethod(_javaClass, mid );
		LOGD("%s:successed call [%s]", __FUNCTION__, cFunc);
	}
}

void JniHelper::SoftnyxLogout( )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return;
	}
	LOGD("%s:", __FUNCTION__ );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "Callback_SoftnyxLogout";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc, "()V");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
		LOGD("%s:call [%s]", __FUNCTION__, cFunc);
		_env->CallStaticObjectMethod(_javaClass, mid );
		LOGD("%s:successed call [%s]", __FUNCTION__, cFunc);
	}
}

void JniHelper::SoftnyxBuyItem( const std::string& idsProduct, int price, const std::string& strcPayload )
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return;
	}
	LOGD("%s:", __FUNCTION__ );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "Callback_SoftnyxBuyItem";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc, "(Ljava/lang/String;Ljava/lang/String;I)V");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
		LOGD("%s:call [%s]", __FUNCTION__, cFunc);
		jstring jstrProduct = _env->NewStringUTF(idsProduct.c_str());
		jint jprice = price;
		jstring jstrPayload = _env->NewStringUTF(strcPayload.c_str());
		_env->CallStaticObjectMethod(_javaClass, mid, jstrProduct, jstrPayload, jprice );
		LOGD("%s:successed call [%s]", __FUNCTION__, cFunc);
	}
}

std::string JniHelper::GetDeviceModel()
{
	JNIEnv *_env = 0;
	if (! getEnv(&_env))    {
		return std::string();
	}
	LOGD("%s:", __FUNCTION__ );
	jclass _javaClass = getClassID_("com/mtricks/xe/Cocos2dxHelper", _env);
	const char* cFunc = "getDeviceModel";
	jmethodID mid = _env->GetStaticMethodID(_javaClass, cFunc,  "()Ljava/lang/String");
	if( mid == nullptr ) {
		LOGD("%s:not found java function:[%s]", __FUNCTION__, cFunc );
	} else {
		LOGD("%s:call [%s]", __FUNCTION__, cFunc);
		jstring str = (jstring)_env->CallStaticObjectMethod(_javaClass, mid);
		const char *msg = _env->GetStringUTFChars(str,0);
//		strcpy_s( pOut, size, msg );
		std::string strRet = (msg)? msg : "";
		LOGD("%s:successed call [%s]", __FUNCTION__, cFunc);
		return strRet;
	}
	return nullptr;
}

#ifdef __cplusplus
extern "C"
{
#endif

	//////////////////////////////////////////////////////////////////////////
	// JAVA-> C++

	//  ¿Œæ€
	JNIEXPORT jstring JNICALL 
	Java_com_gemtree_caribe_MyMainActivity_JavaToCpp( JNIEnv* env
																									, jobject thiz
																									, jstring id
																									, jstring param1
																									, jstring param2 )
	{
		LOGD("%s", __FUNCTION__ );
		const char *m_id = env->GetStringUTFChars(id, NULL);
		const char *m_param1 = env->GetStringUTFChars(param1, NULL);
		const char *m_param2 = env->GetStringUTFChars(param2, NULL);

		std::string strID = (m_id)? m_id : "";
		std::string strParam1 = (m_param1)? m_param1 : "";
		std::string strParam2 = (m_param2)? m_param2 : "";
		LOGD("JavaToCpp:%s, %s, %s", strID.c_str(), strParam1.c_str(), strParam2.c_str() );
		if(strID == "InitInApp") {
			// m_id : ¿Œæ€ √ ±‚»≠
			// m_parma1 :
			// m_parma2 :
		} else
		if( strID == "gcm_regid" ) {
			XReceiverCallback::sGet()->cbOnReceiveCallback( strID, strParam1, strParam2 );
		} else {
			XBREAKF(1, "unknown JavaToCpp id: strid=%s", strID.c_str() );
		}

		return env->NewStringUTF("");
	}

	//////////////////////////////////////////////////////////////////////////
	// C++ -> JAVA

	// «Ô∆€ «‘ºˆ
	std::string CppToJava1( const char* methodName
												, const char* id
												, const char* param1
												, const char* param2 )
	{
		LOGD( "%s:%s, %s, %s, %s", __FUNCTION__, methodName, id, param1, param2 );
		JNIEnv *_env = 0;
		if( !getEnv( &_env ) ) {
			return "";
		}
		jstring r;
		jclass _javaClass = getClassID_( "com/gemtree/caribe/MyMainActivity", _env );
		jmethodID mid = _env->GetStaticMethodID( _javaClass
																					, methodName
			, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;" );
		if( mid == NULL ) {
			LOGD( "not found java function" );
		} else {
			//LOGD("get Callback_OnClickedBuyItem");
			jstring tid = _env->NewStringUTF( id );
			jstring tparam1 = _env->NewStringUTF( param1 );
			jstring tparam2 = _env->NewStringUTF( param2 );
			r = (jstring)_env->CallStaticObjectMethod( _javaClass, mid, tid, tparam1, tparam2 );

			//LOGD("call LoadMarketURL");
		}
		const char* rchar = _env->GetStringUTFChars( r, NULL );
		std::string rr( rchar );
//		rr = rchar;
		_env->ReleaseStringUTFChars( r, rchar );
		LOGD( "JniHelper:CppToJava: id=%s", ( id ) ? id : "null" );
		return rr;	
	}
	//  ¿Œæ€
	std::string CppToJava( const char* id, const char* param1, const char* param2 )
	{
		LOGD("CppToJava : %s	%s	%s",id, param1, param2);
		return CppToJava1("CppToJava", id, param1, param2);
	}

#ifdef __cplusplus
} // extern "C"
#endif


#endif // android
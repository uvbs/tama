#ifdef ANDROID
/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

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
****************************************************************************/
#ifndef __ANDROID_JNI_HELPER_H__
#define __ANDROID_JNI_HELPER_H__
#ifdef ANDROID
#include <jni.h>
#include <string>
//#include "platform/CCPlatformMacros.h"
#define CC_DLL

//NS_CC_BEGIN

typedef struct JniMethodInfo_
{
    JNIEnv *    env;
    jclass      classID;
    jmethodID   methodID;
} JniMethodInfo;

class CC_DLL JniHelper
{
public:
    static JavaVM* getJavaVM();
    static void setJavaVM(JavaVM *javaVM);
    static void setExternalResourcePath(const char* cPath);
    static const char* getExternalResourcePath();
    static void setInternalResourcePath(const char* cPath);
    static const char* getInternalResourcePath();
    static bool getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);
/*
    static const char* getExternalAssetPath();
    static jclass getClassID(const char *className, JNIEnv *env=0);
    static bool getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);
 */
    static std::string jstring2string(jstring str);
    static char* __x_unicode_test();
    static char* Create_UTF16_To_UTF8(char *pOutUtf8, const unsigned short *utf16, int sizeBuff );
    static char* Create_EUCKR_To_UTF8(char *pOutUtf8, const char *euckr, int sizeBuff );
    template<int _Size>
    static char* Create_UTF16_To_UTF8(char (&buff)[_Size], const unsigned short *utf16 ) {
        return Create_UTF16_To_UTF8( buff, utf16, _Size );
    }
    template<int _Size>
    static char* Create_EUCKR_To_UTF8(char (&buff)[_Size], const char *euckr ) {
        return Create_EUCKR_To_UTF8( buff, euckr, _Size );
    }
	static void OnClickedBuyItem( const char *cidProduct, const std::string& strPayload );
	static void SendCertificationIni3( const char *cID, const char *cPassword );
	static const char* GetDevideID(char *pOut, int size);
	static void GoURL(const char *url);
	static void DoLoginFacebook();
	static void DoLogoutFacebook();
	static void DoLoginIni3ByFacebook( void );
	static void DoLoginLytoByFacebook( void );
	static void GetUserProfileImage(const char *cUserID);
	static int RenameFile( const char *cFileOld, const char *cFileNew );
	static void DoExitApp( void );

	static bool Check3G();
	static bool CheckWiFi();

	static void LoadMarketURL(const char *url);
	static void InitGoogleIAPAsync( const std::string& strPublicKey );
	static void DoConsumeItemAsync( const std::string& idsProduct );
	static void DoCheckUnconsumedItemAsync();
	static void ShowDialog( const std::string& strTitle, const std::string& strText );
	static void SoftnyxLogin();
	static void SoftnyxLogout( );
	static void SoftnyxBuyItem( const std::string& idsProduct, int price, const std::string& strcPayload );
	static std::string GetDeviceModel();
private:
    static JavaVM *m_psJavaVM;
    static std::string m_externalResourcePath;
    static std::string m_internalResourcePath;
};
//NS_CC_END
//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C"
{
#endif
	// æ»µÂ∑Œ¿ÃµÂ
	#include <string>
	//////////////////////////////////////////////////////////////////////////
	// JAVA-> C++
	//  ¿Œæ€
	JNIEXPORT jstring JNICALL Java_com_gemtree_caribe_MyMainActivity_JavaToCpp(JNIEnv* env, jobject thiz, jstring id, jstring param1, jstring param2);
	/////////////////////////////////////////////////////////////////////////
	// C++ -> JAVA
	//  ¿Œæ€
	std::string CppToJava(const char* id, const char* param1 = "", const char* param2 = "");
#ifdef __cplusplus
}
#endif

#else

#ifdef __cplusplus
extern "C"
{
#endif
	// IOS
	// IOS -> C++
	//  ¿Œæ€
	const char* iOSToCpp(const char* id, const char* param1, const char* param2);
	// C++ -> IOS
	//  ¿Œæ€
	extern const char* CppToJava(const char* id, const char* param1, const char* param2);
#ifdef __cplusplus
}
#endif
#endif // ANDROID
#endif // __ANDROID_JNI_HELPER_H__
#else
	// ¿©µµøÏ
	#include <string>
	//  ¿Œæ€
	inline std::string CppToJava(const char* id, const char* param1 = "", const char* param2 = "") { return std::string(""); }
#endif // android
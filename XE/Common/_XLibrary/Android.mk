LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xe_static

LOCAL_MODULE_FILENAME := libxlib

LOCAL_SRC_FILES := \
XE.cpp \
XResMng.cpp \
XHSLMap.cpp \
xVector.cpp \
XXMLDoc.cpp \
XArchive.cpp \
Sound/Android/XSoundMngAndroid2.cpp \
XLock.cpp \
XPThread.cpp \
XRefObj.cpp \
XLockPThreadMutex.cpp \
XSystem.cpp \
XLibrary.cpp \
XCrypto.cpp \
XImage.cpp \
XImageLibPng.cpp \
XLua.cpp \
lua_tinker.cpp \
XResObj.cpp \
XAutoPtr.cpp \
XBaseFontDat.cpp \
XBaseFontObj.cpp \
XFontMng.cpp \
XFontSpr.cpp \
XParticle.cpp \
XParticleMng.cpp \
XParticleFramework.cpp \
XPool.cpp \
XRefRes.cpp \
XSoundMng.cpp \
XTransition.cpp \
XImageMng.cpp \
android_xe/ConvertString_android.cpp \
android_xe/CrashHandler.cpp \
Map/XEMap.cpp \
Map/XEMapLayer.cpp \
Map/XEMapLayerImage.cpp \
Sound/Android/XSoundMngAndroid.cpp \
../XDrawGraph.cpp \
../XGameUtil.cpp \
../XInterpolationObj.cpp \



LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
						$(LOCAL_PATH)/../ \
						$(LOCAL_PATH)/../../ \
						$(LOCAL_PATH)/android_xe/ \
						$(LOCAL_PATH)/android_xe/prebuilt/libpng/include/ \
						$(LOCAL_PATH)/android_xe/prebuilt/libcurl/include/curl/ \
						$(LOCAL_PATH)/../../Third_Party_lib/Oolong_Engine2/include \
						$(LOCAL_PATH)/../../Third_Party_lib/zip_support \
						$(LOCAL_PATH)/../../Third_Party_lib/lua514/src/ \
						$(LOCAL_PATH)/../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
						$(LOCAL_PATH)/../../Third_Party_lib/ftgles/freetype2_android/include/ \
						$(LOCAL_PATH)/../../Third_Party_lib/tinyxml/ \
						$(LOCAL_PATH)/../../Third_Party_lib \
						

LOCAL_EXPORT_LDLIBS := -llog\
                       -lz \
                       -lGLESv2 \
                       

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/android_xe/ \
					$(LOCAL_PATH)/android_xe/prebuilt/libpng/include/ \
					$(LOCAL_PATH)/android_xe/prebuilt/libcurl/include/curl/ \
					$(LOCAL_PATH)/../../Third_Party_lib/Oolong_Engine2/include \
					$(LOCAL_PATH)/../../Third_Party_lib/zip_support \
					$(LOCAL_PATH)/../../Third_Party_lib/lua514/src/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/freetype2_android/include/ \
					$(LOCAL_PATH)/../../Third_Party_lib/tinyxml/ \
					$(LOCAL_PATH)/../../Third_Party_lib \
					$(LOCAL_PATH)/../../ \

					
LOCAL_LDLIBS := -lGLESv2 \
                -lEGL \
                -llog \
                -lz \

LOCAL_CFLAGS += $(COMMON_CFLAGS)

#LOCAL_WHOLE_STATIC_LIBRARIES := cocos_libpng_static cocos_curl_static
LOCAL_WHOLE_STATIC_LIBRARIES := cocos_curl_static

include $(BUILD_STATIC_LIBRARY)

#$(call import-module,libpng)
$(call import-module,libcurl)

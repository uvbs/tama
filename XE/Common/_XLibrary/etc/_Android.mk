LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xe_etc_static

LOCAL_MODULE_FILENAME := libxetc

LOCAL_SRC_FILES := \
Alert.cpp \
Debug.cpp \
InputMng.cpp \
path.cpp \
Timer.cpp \
Token.cpp \
XGraphics.cpp \
xLang.cpp \
xMath.cpp \
XSurface.cpp \
XTimer2.cpp \
xUtil.cpp \
iOS/xStringiOS.cpp \
XSurfaceMem.cpp \

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
						$(LOCAL_PATH)/../ \
						$(LOCAL_PATH)/etc/ \
						$(LOCAL_PATH)/etc/iOS/ \
						$(LOCAL_PATH)/android/prebuilt/libpng/include/ \
						$(LOCAL_PATH)/android/prebuilt/libcurl/include/curl/ \
						$(LOCAL_PATH)/OpenGL2/ \
						$(LOCAL_PATH)/OpenGL2/Shader/ \
						$(LOCAL_PATH)/Sprite/ \
						$(LOCAL_PATH)/_Wnd2/ \
						$(LOCAL_PATH)/Network/ \
						$(LOCAL_PATH)/../../Third_Party_lib/Oolong_Engine2/include \
						$(LOCAL_PATH)/../../Third_Party_lib/zip_support \
						$(LOCAL_PATH)/../../Third_Party_lib/lua514/src/ \
						$(LOCAL_PATH)/../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
						$(LOCAL_PATH)/../../Third_Party_lib/ftgles/freetype2_android/include/ \
						$(LOCAL_PATH)/../../Third_Party_lib/tinyxml/ \
						$(LOCAL_PATH)/../../Third_Party_lib \
						$(LOCAL_PATH)/../../Third_Party_lib\boost_1_55_0/ \
						$(LOCAL_PATH)/../../XFramework/ \
						$(LOCAL_PATH)/../../ \

#						$(LOCAL_PATH)/../../../MirrorWar/proj.android/jni/ \
#						$(LOCAL_PATH)/../../../MirrorWar/proj.android/jni/CocosDenshion/include \
						

LOCAL_EXPORT_LDLIBS := -llog\
                       -lz \
                       -lGLESv2 \
                       

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/etc/ \
					$(LOCAL_PATH)/etc/iOS/ \
					$(LOCAL_PATH)/android/ \
					$(LOCAL_PATH)/android/prebuilt/libpng/include/ \
					$(LOCAL_PATH)/android/prebuilt/libcurl/include/curl/ \
					$(LOCAL_PATH)/OpenGL2/ \
					$(LOCAL_PATH)/OpenGL2/Shader/ \
					$(LOCAL_PATH)/Sprite/ \
					$(LOCAL_PATH)/_Wnd2/ \
					$(LOCAL_PATH)/Network/ \
					$(LOCAL_PATH)/../../Third_Party_lib/Oolong_Engine2/include \
					$(LOCAL_PATH)/../../Third_Party_lib/zip_support \
					$(LOCAL_PATH)/../../Third_Party_lib/lua514/src/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/freetype2_android/include/ \
					$(LOCAL_PATH)/../../Third_Party_lib/tinyxml/ \
					$(LOCAL_PATH)/../../Third_Party_lib\boost_1_55_0/ \
					$(LOCAL_PATH)/../../Third_Party_lib \
					$(LOCAL_PATH)/../../XFramework/ \
					$(LOCAL_PATH)/../../XFramework/client/ \
					$(LOCAL_PATH)/../../XFramework/Network/android/ \
					$(LOCAL_PATH)/../../XFramework/Network/ \
					$(LOCAL_PATH)/../../ \
				
LOCAL_LDLIBS := -lGLESv2 \
                -lEGL \
                -llog \
                -lz \

LOCAL_CFLAGS += $(COMMON_CFLAGS)

LOCAL_WHOLE_STATIC_LIBRARIES := cocos_libpng_static cocos_curl_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libpng)
$(call import-module,libcurl)

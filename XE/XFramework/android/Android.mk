LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := jni_static

LOCAL_MODULE_FILENAME := libjni

LOCAL_SRC_FILES := \
com_mtricks_xe_XRenderer.cpp \
com_mtricks_xe_Cocos2dxHelper.cpp \
JniHelper.cpp \
CocosDenshion/android/SimpleAudioEngine.cpp \
CocosDenshion/android/jni/SimpleAudioEngineJni.cpp \
CocosDenshion/android/opensl/OpenSLEngine.cpp \
CocosDenshion/android/opensl/SimpleAudioEngineOpenSL.cpp \

LOCAL_EXPORT_LDLIBS := -llog\
                       -lz \
                       -lGLESv2 \
                       
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
							$(LOCAL_PATH)/../ \
							$(LOCAL_PATH)/../../ \
							$(LOCAL_PATH)/../../Common/_XLibrary/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/./ \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../Common/_XLibrary/android_xe \
					$(LOCAL_PATH)/../../Common/_XLibrary/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ \
					
LOCAL_LDLIBS := -lGLESv2 \
                -lEGL \
                -llog \
                -lz \

LOCAL_CFLAGS += $(COMMON_CFLAGS)

include $(BUILD_STATIC_LIBRARY)

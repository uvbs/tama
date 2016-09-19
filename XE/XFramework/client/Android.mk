LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xframework_client_static

LOCAL_MODULE_FILENAME := libframeworkclient

LOCAL_SRC_FILES := \
XApp.cpp \
XEContent.cpp \
XClientMain.cpp \
XLayout.cpp \
XLayoutObj.cpp \
XTimeoutMng.cpp \
XWndBatchRender.cpp 

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../Common/_XLibrary \
					$(LOCAL_PATH)/../../Common/_XLibrary/android_xe \
					$(LOCAL_PATH)/../../../ \
					$(LOCAL_PATH)/../../Third_Party_lib/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/freetype2_android/include/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../Common/_XLibrary \
					$(LOCAL_PATH)/../../Common/_XLibrary/android_xe \
					$(LOCAL_PATH)/../../../ \
					$(LOCAL_PATH)/../../Third_Party_lib/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
					$(LOCAL_PATH)/../../Third_Party_lib/ftgles/freetype2_android/include/ \
					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
include $(BUILD_STATIC_LIBRARY)


LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xframework_game_static

LOCAL_MODULE_FILENAME := libframeworkgame

LOCAL_SRC_FILES := \
XFLevel.cpp \
XEBaseWorldObj.cpp \
XEWndWorld.cpp \
XEWndWorldImage.cpp \
XEObjMng.cpp \
XEWorld.cpp \
XEComponents.cpp \
XEWndWorldObjs.cpp \
XEWorldCamera.cpp 

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../Common/_XLibrary \
					$(LOCAL_PATH)/../../Common/_XLibrary/android_xe \
					$(LOCAL_PATH)/../../../ \
					$(LOCAL_PATH)/../../Third_Party_lib/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../Common/_XLibrary \
					$(LOCAL_PATH)/../../Common/_XLibrary/android_xe \
					$(LOCAL_PATH)/../../../ \
					$(LOCAL_PATH)/../../Third_Party_lib/ \
					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
include $(BUILD_STATIC_LIBRARY)


LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xe_etc_static

LOCAL_MODULE_FILENAME := libetc

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
XSurfaceMem.cpp 

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
							$(LOCAL_PATH)/../ \
							$(LOCAL_PATH)/../../../XE/Common/_XLibrary/android_xe/ \
							$(LOCAL_PATH)/../../../XE/ \
							$(LOCAL_PATH)/../../../XE/Common/_XLibrary/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../android_xe/ \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../../Resource \
					$(LOCAL_PATH)/../../../../XE/Common/_XLibrary/ \
					$(LOCAL_PATH)/../../../../XE/Third_Party_lib/ \
					$(LOCAL_PATH)/../../../../XE/ \
					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
include $(BUILD_STATIC_LIBRARY)


LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xml_static

LOCAL_MODULE_FILENAME := libxml

LOCAL_SRC_FILES := \
tinystr.cpp \
tinyxml.cpp \
tinyxmlerror.cpp \
tinyxmlparser.cpp

LOCAL_EXPORT_C_INCLUDES :=	$(LOCAL_PATH) \
							$(LOCAL_PATH)/../ \
							$(LOCAL_PATH)/../../ \
							$(LOCAL_PATH)/../../Common/_XLibrary \
							$(LOCAL_PATH)/../../Common/_XLibrary/android_xe \

#														$(LOCAL_PATH)/../ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../Common/_XLibrary \
					$(LOCAL_PATH)/../../Common/_XLibrary/android_xe \

#							$(LOCAL_PATH)/../ \
					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
include $(BUILD_STATIC_LIBRARY)


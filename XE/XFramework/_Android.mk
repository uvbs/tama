LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := xframework_static

LOCAL_MODULE_FILENAME := libxframework

LOCAL_SRC_FILES := \
client/XClientMain.cpp \


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/client/ \
					$(LOCAL_PATH)/../T2/ \
					$(LOCAL_PATH)/../Common/_XLibrary/ \
					$(LOCAL_PATH)/../Common/_XLibrary/etc/ \
					$(LOCAL_PATH)/../Common/_XLibrary/etc/iOS/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/client/ \
					$(LOCAL_PATH)/../T2/ \
					$(LOCAL_PATH)/../Common/_XLibrary/ \
					$(LOCAL_PATH)/../Common/_XLibrary/etc/ \
					$(LOCAL_PATH)/../Common/_XLibrary/etc/iOS/ \

include $(BUILD_STATIC_LIBRARY)


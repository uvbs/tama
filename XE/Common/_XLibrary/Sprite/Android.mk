LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xe_spr_static

LOCAL_MODULE_FILENAME := libspr

LOCAL_SRC_FILES := \
Key.cpp \
Layer.cpp \
SprDat.cpp \
Sprite.cpp \
SprMng.cpp \
SprMng_old.cpp \
SprObj.cpp \
XActDat.cpp \
XActObj.cpp \
XActObj2.cpp \
XBaseObj.cpp

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


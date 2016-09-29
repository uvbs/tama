LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := libpng_static

LOCAL_MODULE_FILENAME := libpng

LOCAL_SRC_FILES := \
png.c \
pngerror.c \
pngget.c \
pngmem.c \
pngpread.c \
pngread.c \
pngrio.c \
pngrtran.c \
pngrutil.c \
pngset.c \
pngtrans.c \
pngwio.c \
pngwrite.c \
pngwtran.c \
pngwutil.c \



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
					
LOCAL_CFLAGS += $(COMMON_CFLAGS) -O
					
include $(BUILD_STATIC_LIBRARY)


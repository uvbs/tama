LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xe_gl2_static

LOCAL_MODULE_FILENAME := libxegl

LOCAL_SRC_FILES := \
XGraphicsOpenGL.cpp \
XSurfaceOpenGL.cpp \
XShader.cpp \
XFontDatFTGL.cpp \
XFontObjFTGL.cpp \
XParticleOpenGL.cpp \
XTextureAtlas.cpp \
XTextureAtlasSub.cpp \
XSurfaceGLAtlasBatch.cpp \
XSurfaceGLAtlasNoBatch.cpp \
XRenderCmd.cpp \

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
							$(LOCAL_PATH)/../ \
							$(LOCAL_PATH)/../../ \
							$(LOCAL_PATH)/../android_xe/ \
							$(LOCAL_PATH)/../../../../XE/ \
							$(LOCAL_PATH)/../android_xe/prebuilt/libpng/include/ \
							$(LOCAL_PATH)/../../../Third_Party_lib/ \
							$(LOCAL_PATH)/../../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
							$(LOCAL_PATH)/../../../Third_Party_lib/ftgles/freetype2_android/include/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../android_xe/ \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../../../../XE/ \
					$(LOCAL_PATH)/../android_xe/prebuilt/libpng/include/ \
					$(LOCAL_PATH)/../../../Third_Party_lib/ftgles/ftgles2/ftgles/src/ \
					$(LOCAL_PATH)/../../../Third_Party_lib/ftgles/freetype2_android/include/ \
					$(LOCAL_PATH)/../../../Third_Party_lib/ \
					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
include $(BUILD_STATIC_LIBRARY)


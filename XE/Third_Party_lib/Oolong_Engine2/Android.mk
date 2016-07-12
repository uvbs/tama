LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := oolong_math_static

LOCAL_MODULE_FILENAME := liboolong_math

LOCAL_SRC_FILES := \
Math/Matrix.cpp \
Math/Transform.cpp \
Math/Vector.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
							$(LOCAL_PATH)/Math/ \
#							$(LOCAL_PATH)/../../T2/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/Math/ \
					$(LOCAL_PATH)/include/ \
					$(LOCAL_PATH)/../../Common/_XLibrary/ \
					
#					$(LOCAL_PATH)/../../T2/ \
#					$(LOCAL_PATH)/../../T2/game_src/ \
#					$(LOCAL_PATH)/../../Common/_XLibrary/ \
#					$(LOCAL_PATH)/../../Common/_XLibrary/etc/iOS/ \
#					$(LOCAL_PATH)/../../Common/_XLibrary/OpenGL2/ \
#					$(LOCAL_PATH)/../../T2/Resource \
#					$(LOCAL_PATH)/../../XFramework/ \
#					$(LOCAL_PATH)/../../XFramework/client/ \

include $(BUILD_STATIC_LIBRARY)


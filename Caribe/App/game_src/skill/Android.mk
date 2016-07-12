LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := skill_static

LOCAL_MODULE_FILENAME := libskill

LOCAL_SRC_FILES := \
XAdjParam.cpp \
XBuffObj.cpp \
XESkillMng.cpp \
XLuaSkill.cpp \
xSkill.cpp \
XSkillDat.cpp \
XSkillObj.cpp \
XSkillReceiver.cpp \
XSkillUser.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
							$(LOCAL_PATH)/../ \
							$(LOCAL_PATH)/../../../XE/ \
							$(LOCAL_PATH)/../../../XE/Common/_XLibrary/ \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../android/ \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../../Resource \
					$(LOCAL_PATH)/../../../../XE/Common/_XLibrary/ \
					$(LOCAL_PATH)/../../../../XE/Third_Party_lib/ \
					$(LOCAL_PATH)/../../../../XE/ \
					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
include $(BUILD_STATIC_LIBRARY)


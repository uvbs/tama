LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := xframework_static

LOCAL_MODULE_FILENAME := libxframework

LOCAL_SRC_FILES := \
XSplitNode.cpp \
XParamObj.cpp \
XEOption.cpp \
XInApp.cpp \
XReceiverCallback.cpp \
Game/XEComponentMng.cpp \
client/XEmitter.cpp \
client/XPropParticle.cpp \
XEToolTip.cpp \
XDBAccount.cpp \
XConstant.cpp \
XTextTable.cpp \
XTextTableUTF8.cpp \
XF.cpp \
XConsoleMain.cpp \
XScroll.cpp \
XSoundTable.cpp \
XGlobal.cpp \
XFacebook.cpp \
XEBaseScene.cpp \
XEBaseProp.cpp \
XESceneMng.cpp \
XEProfile.cpp 




LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../Common/_XLibrary \
					$(LOCAL_PATH)/../Common/_XLibrary/android_xe \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../Third_Party_lib/ \


LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../Common/_XLibrary \
					$(LOCAL_PATH)/../Common/_XLibrary/android_xe \
					$(LOCAL_PATH)/../../ \
					$(LOCAL_PATH)/../Third_Party_lib/ \

include $(BUILD_STATIC_LIBRARY)


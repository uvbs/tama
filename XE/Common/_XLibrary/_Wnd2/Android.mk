LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_MODULE := xe_wnd_static

LOCAL_MODULE_FILENAME := libwnd

LOCAL_SRC_FILES := \
XWnd.cpp \
XWndButton.cpp \
XWndButtonRadio.cpp \
XWndEdit.cpp \
XWndCtrls.cpp \
XWndSprObj.cpp \
XWndImage.cpp \
XWndPageSlide.cpp \
XWndText.cpp \
XWndView.cpp \
XWndPopup.cpp \
XWndList.cpp \
XWndProgressbar.cpp \
XWndScrollview.cpp 

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
							$(LOCAL_PATH)/../ \
							$(LOCAL_PATH)/../android_xe/ \
							$(LOCAL_PATH)/../../../Third_Party_lib/ \
							$(LOCAL_PATH)/../../../ \


LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../android_xe/ \
					$(LOCAL_PATH)/../../../Third_Party_lib/ \
					$(LOCAL_PATH)/../../../ \

					
LOCAL_CFLAGS += $(COMMON_CFLAGS)
					
include $(BUILD_STATIC_LIBRARY)


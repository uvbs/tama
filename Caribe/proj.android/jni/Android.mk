# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := xlib_test

LOCAL_SRC_FILES := xlib_test.cpp 

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../App/game_src/android/ \
					$(LOCAL_PATH)/../../App/Resource/ \
					$(LOCAL_PATH)/../../../XE/Common/_XLibrary/ \
					$(LOCAL_PATH)/../../../XE/ \
					$(LOCAL_PATH)/../../App/game_src/ \
					


LOCAL_EXPORT_LDLIBS := -llog\
                       -lz \
                       -lGLESv2 \

                    
LOCAL_LDLIBS := -lGLESv2 \
                -llog \
                -lz \


#COMMON_CFLAGS := -DDEBUG -D_DEBUG

COMMON_CFLAGS := -DNDEBUG -D_SPR_USE_LUA -D_XNEW_APP -marm -fno-omit-frame-pointer -fexceptions -Wparentheses -fpermissive

LOCAL_CFLAGS += $(COMMON_CFLAGS)				

LOCAL_EXPORT_CFLAGS += $(COMMON_CFLAGS) 

COMMON_CPPFLAGS := -DNDEBUG -marm -fno-omit-frame-pointer -fexceptions -Wparentheses -fpermissive -O2

LOCAL_CPPFLAGS += $(COMMON_CPPFLAGS)

LOCAL_EXPORT_CPPFLAGS += $(COMMON_CPPFLAGS)

#LOCAL_CPPFLAGS += -DDEBUG \
#				-D_DEBUG 
				
#LOCAL_EXPORT_CPPFLAGS += -DDEBUG \
#						-D_DEBUG 

LOCAL_WHOLE_STATIC_LIBRARIES := \
	xe_static \
	xe_spr_static \
	xe_gl2_static \
	xe_wnd_static \
	xe_etc_static \
	xe_network_static \
	xframework_static \
	xframework_client_static \
	xframework_game_static \
	gamesrc_static \
	skill_static \
	jni_static \
	oolong_math_static \
	lua514_static \
	freetype2-static \
	zip_static \
	libpng_static \
	xml_static \
	ftgl2_static


#BUILD_STATIC_LIBRARY
#BUILD_MULTI_PREBUILT
include $(BUILD_SHARED_LIBRARY)

$(call import-module,../XE/Common/_XLibrary)
$(call import-module,../XE/Common/_XLibrary/etc)
$(call import-module,../XE/Common/_XLibrary/_Wnd2)
$(call import-module,../XE/Common/_XLibrary/Sprite)
$(call import-module,../XE/Common/_XLibrary/OpenGL2)
$(call import-module,../XE/Common/_XLibrary/network)
$(call import-module,../XE/XFramework)
$(call import-module,../XE/XFramework/android)
$(call import-module,../XE/XFramework/client)
$(call import-module,../XE/XFramework/Game)
$(call import-module,../XE/XFramework/lpng1256)
$(call import-module,../XE/Third_Party_lib/Oolong_Engine2)
$(call import-module,../XE/Third_Party_lib/lua514)
$(call import-module,../XE/Third_Party_lib/ftgles/freetype2_android/Android/jni)
$(call import-module,../XE/Third_Party_lib/ftgles/ftgles2/ftgles)
$(call import-module,../XE/Third_Party_lib/zip_support)
$(call import-module,../XE/Third_Party_lib/tinyxml)
$(call import-module,App/game_src)
$(call import-module,App/game_src/skill)

#$(call import-module,../XE/Third_Party_lib/CocosDenshion/android)


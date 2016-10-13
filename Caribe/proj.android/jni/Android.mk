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

# ������������ �����ϴ� ���丮��θ� ��ȯ�Ѵ�.(�����)

LOCAL_PATH := $(call my-dir)

# Local_path�� ������ ��� LOCAL_�������� �ʱ�ȭ ����.
include $(CLEAR_VARS)

#���� ���̺귯�������� �̸��� ������. .so������ ���.
LOCAL_MODULE    := xlib_test

#���̺귯�� �� �ʿ��� �ҽ��ڵ� ����� �����ϴ� �κ�.(Local_Path����.) \�� ������.
LOCAL_SRC_FILES := xlib_test.cpp \

#Ŭ���� ������ �����ϴ� ���ϵ��� ��θ� �������ִ� �κ�.
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/../../App/game_src/android/ \
					$(LOCAL_PATH)/../../App/Resource/ \
					$(LOCAL_PATH)/../../../XE/Common/_XLibrary/ \
					$(LOCAL_PATH)/../../../XE/ \
					$(LOCAL_PATH)/../../App/game_src/ \
					


LOCAL_EXPORT_LDLIBS := -llog\
                       -lz \
                       -lGLESv2 \

#				-lEGL \
                       
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
<<<<<<< HEAD
=======
	xe_spr_static \
	xe_gl2_static \
	xe_wnd_static \
	xe_etc_static \
	xe_network_static \
	xframework_static \
	xframework_client_static \
	xframework_game_static \
>>>>>>> private_raid
	gamesrc_static \
	skill_static \
	jni_static \
	oolong_math_static \
	lua514_static \
	freetype2-static \
<<<<<<< HEAD
=======
	zip_static \
	libpng_static \
	xml_static \
>>>>>>> private_raid
	ftgl2_static


#share library�� ���϶�� ����,
#BUILD_STATIC_LIBRARY
#BUILD_MULTI_PREBUILT
#���� ����
include $(BUILD_SHARED_LIBRARY)


$(call import-module,../XE/Common/_XLibrary)
<<<<<<< HEAD
$(call import-module,../XE/XFramework/android)
=======
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
>>>>>>> private_raid
$(call import-module,../XE/Third_Party_lib/Oolong_Engine2)
$(call import-module,../XE/Third_Party_lib/lua514)
$(call import-module,../XE/Third_Party_lib/ftgles/freetype2_android/Android/jni)
$(call import-module,../XE/Third_Party_lib/ftgles/ftgles2/ftgles)
<<<<<<< HEAD
$(call import-module,App/game_src)
$(call import-module,App/game_src/skill)
=======
$(call import-module,../XE/Third_Party_lib/zip_support)
$(call import-module,../XE/Third_Party_lib/tinyxml)
$(call import-module,App/game_src)
$(call import-module,App/game_src/skill)

>>>>>>> private_raid
#$(call import-module,../XE/Third_Party_lib/CocosDenshion/android)


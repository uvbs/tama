add_library( jni STATIC
    com_mtricks_xe_XRenderer.cpp
    com_mtricks_xe_Cocos2dxHelper.cpp
    JniHelper.cpp
    CocosDenshion/android/SimpleAudioEngine.cpp
    CocosDenshion/android/jni/SimpleAudioEngineJni.cpp
    CocosDenshion/android/opensl/OpenSLEngine.cpp
    CocosDenshion/android/opensl/SimpleAudioEngineOpenSL.cpp
            )
include_directories(
    ../../Common/_XLibrary/android_xe
    ../../Common/_XLibrary
    ../../Third_Party_lib
    ../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../Third_Party_lib/ftgles/freetype2_android/include
    ../../
    )

target_link_libraries( jni xframework )
cmake_minimum_required(VERSION 3.4.1)

SET(CMAKE_VERBOSE_MAKEFILE ON)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frtti -fno-omit-frame-pointer -fexceptions -Wparentheses -fpermissive -O2 -w -D_XGMO -D_XPATCH -DFT2_BUILD_LIBRARY")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -D_SPR_USE_LUA -D_XNEW_APP -marm -fno-omit-frame-pointer -fexceptions -Wparentheses -fpermissive -DANDROID_NDK -DFT2_BUILD_LIBRARY=1")

add_library(xlib_test SHARED
            xlib_test.cpp
            )

# build the ndk-helper library
##set(xe_dir ../../../../../../XE/XFramework/android)
##add_subdirectory( ${xe_dir} xebin )

add_subdirectory( ../../../../../../XE/XFramework xframework )
add_subdirectory( ../../../../../../XE/XFramework/android jni )
add_subdirectory( ../../../../../../XE/XFramework/client xframework_client )
add_subdirectory( ../../../../../../XE/XFramework/Game xframework_game )
add_subdirectory( ../../../../../../XE/Common/_XLibrary xlib )
add_subdirectory( ../../../../../../XE/Common/_XLibrary/_Wnd2 xe_wnd )
add_subdirectory( ../../../../../../XE/Common/_XLibrary/etc xe_etc )
add_subdirectory( ../../../../../../XE/Common/_XLibrary/Network xe_network )
add_subdirectory( ../../../../../../XE/Common/_XLibrary/OpenGL2 xe_gl2 )
add_subdirectory( ../../../../../../XE/Common/_XLibrary/Sprite xe_spr )
add_subdirectory( ../../../../../../XE/Third_Party_lib/ftgles/ftgles2/ftgles ftgl2 )
add_subdirectory( ../../../../../../XE/Third_Party_lib/ftgles/freetype2_android/Android/jni freetype2 )

#target_include_directories(xlib_test PRIVATE
#                           ${xe_dir})

include_directories(
    ../../../../../../Caribe/App/game_src/android
    ../../../../../../Caribe/App/game_src
    ../../../../../../XE/Common/_XLibrary
    ../../../../../../XE/Third_Party_lib
    ../../../../../../XE/Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../../../../XE/Third_Party_lib/ftgles/freetype2_android/include
    ../../../../../../XE
    )

# add lib dependencies
target_link_libraries(xlib_test
                      android
                      log
                      EGL
                      GLESv2
                      xframework
                      jni
                      xframework_client
                      xframework_game
                      xlib
                      xe_wnd
                      xe_etc
                      xe_network
                      xe_gl2
                      xe_spr
                      ftgl2
                      freetype
                      curl
                      )


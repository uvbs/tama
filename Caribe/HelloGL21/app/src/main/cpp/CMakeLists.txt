cmake_minimum_required(VERSION 3.4.1)

# now build app's shared lib
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frtti -marm -fno-omit-frame-pointer -fexceptions -Wparentheses -fpermissive -D_XGMO -D_XPATCH")

# build the ndk-helper library
set(xe_dir ../../../../common/xe)
add_subdirectory(${xe_dir} xe)


add_library(xlib_test SHARED
            xlib_test.cpp

            )

target_include_directories(xlib_test PRIVATE
                           ${xe_dir})

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
                      atomic
                      log 
                      EGL
                      GLESv2
                      xe)


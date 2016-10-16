cmake_minimum_required(VERSION 3.4.1)

# configure import libs
set(distribution_DIR ${CMAKE_SOURCE_DIR}/../../../../distribution)

add_library(lib_gmath STATIC IMPORTED)
set_target_properties(lib_gmath PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/gmath/lib/${ANDROID_ABI}/libgmath.a)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_library(gl2jni SHARED
            gl_code.cpp)

include_directories(
    C:/p4v/iPhone_zero/Caribe/App/game_src/android
    C:/p4v/iPhone_zero/Caribe/App/game_src
    C:/p4v/iPhone_zero/XE/Common/_XLibrary
    C:/p4v/iPhone_zero/XE/Third_Party_lib
    C:/p4v/iPhone_zero/XE
    )

# add lib dependencies
target_link_libraries(gl2jni
                      android
                      lib_gmath
                      log
                      EGL
                      GLESv2)
add_library(xe STATIC
            JniHelper.cpp
            )
include_directories(
    ../../../../../../XE/Common/_XLibrary/android_xe
    ../../../../../../XE/Common/_XLibrary
    ../../../../../../XE/Third_Party_lib
    ../../../../../../XE/Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../../../../XE/Third_Party_lib/ftgles/freetype2_android/include
    ../../../../../../XE
    )


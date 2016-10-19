add_library(xframework_client STATIC
    XApp.cpp
    XEContent.cpp
    XClientMain.cpp
    XLayout.cpp
    XLayoutObj.cpp
    XTimeoutMng.cpp
    XWndBatchRender.cpp
            )
include_directories(
    ../../Common/_XLibrary/android_xe
    ../../Common/_XLibrary
    ../../Third_Party_lib
    ../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../Third_Party_lib/ftgles/freetype2_android/include
    ../../
    )


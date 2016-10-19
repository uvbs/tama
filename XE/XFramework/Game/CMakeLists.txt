add_library(xframework_game STATIC
    XFLevel.cpp
    XEBaseWorldObj.cpp
    XEWndWorld.cpp
    XEWndWorldImage.cpp
    XEObjMng.cpp
    XEWorld.cpp
    XEComponents.cpp
    XEWndWorldObjs.cpp
    XEWorldCamera.cpp
            )
include_directories(
    ../../Common/_XLibrary/android_xe
    ../../Common/_XLibrary
    ../../Third_Party_lib
    ../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../Third_Party_lib/ftgles/freetype2_android/include
    ../../
    )


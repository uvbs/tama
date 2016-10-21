add_library(xframework_client STATIC
    XApp.cpp
    XEContent.cpp
    XClientMain.cpp
    XTimeoutMng.cpp
    XWndBatchRender.cpp
            )

add_library(xe_layout STATIC XLayout.cpp XLayoutObj.cpp )

include_directories(
    ../../Common/_XLibrary/android_xe
    ../../Common/_XLibrary
    ../../Third_Party_lib
    ../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../Third_Party_lib/ftgles/freetype2_android/include
    ../../
    )

target_link_libraries( xframework_client xframework )
target_link_libraries( xframework_client xframework_game )
target_link_libraries( xframework_client xe_gl2 )
target_link_libraries( xframework_client xlib )
target_link_libraries( xframework_client xe_etc )
target_link_libraries( xframework_client xe_wnd )
target_link_libraries( xframework_client xe_spr )

target_link_libraries( xe_layout xe_wnd )


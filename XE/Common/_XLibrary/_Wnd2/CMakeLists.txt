add_library(xe_wnd STATIC
XWnd.cpp 
XWndButton.cpp 
XWndButtonRadio.cpp 
XWndEdit.cpp 
XWndCtrls.cpp 
XWndSprObj.cpp 
XWndImage.cpp 
XWndPageSlide.cpp 
XWndText.cpp 
XWndView.cpp 
XWndPopup.cpp 
XWndList.cpp 
XWndProgressbar.cpp 
XWndScrollview.cpp 
            )
include_directories(
    ../android_xe
    ../
    ../../../Common/_XLibrary
    ../../../XFramework
    ../../../XFramework/client
    ../../../XFramework/Game
    ../../../Third_Party_lib
    ../../../Third_Party_lib/lua514/src
    ../../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../Third_Party_lib/ftgles/freetype2_android/include
    ../../../
    )

target_link_libraries( xe_wnd xframework )
target_link_libraries( xe_wnd xframework_client )
target_link_libraries( xe_wnd xframework_game )
target_link_libraries( xe_wnd xe_gl2 )
target_link_libraries( xe_wnd xlib )
target_link_libraries( xe_wnd xe_etc )
target_link_libraries( xe_wnd xe_spr )
target_link_libraries( xe_wnd oolong )
target_link_libraries( xe_wnd lua )
target_link_libraries( xe_wnd xe_layout )
target_link_libraries( xe_wnd xe_img )
target_link_libraries( xe_wnd xe_lua_tinker )
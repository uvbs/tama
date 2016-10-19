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
    ../../../Third_Party_lib
    ../../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../Third_Party_lib/ftgles/freetype2_android/include
    ../../../
    )


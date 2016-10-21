add_library(xe_etc STATIC
Alert.cpp 
Debug.cpp 
InputMng.cpp 
path.cpp 
Timer.cpp 
Token.cpp 
XGraphics.cpp 
xLang.cpp 
xMath.cpp 
XSurface.cpp 
XTimer2.cpp 
xUtil.cpp 
iOS/xStringiOS.cpp 
XSurfaceMem.cpp 
            )
include_directories(
    ../android_xe
    ../
    ../../../Third_Party_lib
    ../../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../Third_Party_lib/ftgles/freetype2_android/include
    ../../../
)

target_link_libraries( xe_etc xframework_client )
target_link_libraries( xe_etc xlib )
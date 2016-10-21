add_library(xe_spr STATIC
Key.cpp 
Layer.cpp 
SprDat.cpp 
Sprite.cpp 
SprMng.cpp 
SprMng_old.cpp 
SprObj.cpp 
XActDat.cpp 
XActObj.cpp 
XActObj2.cpp 
XBaseObj.cpp
            )
include_directories(
    ../android_xe
    ../
    ../../../Third_Party_lib
    ../../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../Third_Party_lib/ftgles/freetype2_android/include
    ../../../
        )

#target_link_libraries( xe_spr xframework )
#target_link_libraries( xe_spr xe_gl2 )
#target_link_libraries( xe_spr xlib )
#target_link_libraries( xe_spr xe_etc )
#target_link_libraries( xe_spr xe_wnd )
#target_link_libraries( xe_spr oolong )
#target_link_libraries( xe_spr lua )


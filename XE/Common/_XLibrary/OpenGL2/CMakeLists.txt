add_library(xe_gl2 STATIC
XGraphicsOpenGL.cpp 
XSurfaceOpenGL.cpp 
XShader.cpp 
XFontDatFTGL.cpp 
XFontObjFTGL.cpp 
XParticleOpenGL.cpp 
XTextureAtlas.cpp 
XTextureAtlasSub.cpp 
XSurfaceGLAtlasBatch.cpp 
XSurfaceGLAtlasNoBatch.cpp 
XBatchRenderer.cpp 
            )
include_directories(
    ../android_xe
    ../
    ../../../Common/_XLibrary
    ../../../XFramework
    ../../../XFramework/client
    ../../../XFramework/Game
    ../../../Third_Party_lib
    ../../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../Third_Party_lib/ftgles/freetype2_android/include
    ../../../
        )

target_link_libraries( xe_gl2 xframework )
target_link_libraries( xe_gl2 xlib )
target_link_libraries( xe_gl2 xe_etc )
target_link_libraries( xe_gl2 xe_wnd )
target_link_libraries( xe_gl2 oolong )
target_link_libraries( xe_gl2 lua )
target_link_libraries( xe_gl2 ftgl2 )
target_link_libraries( xe_gl2 xe_font )

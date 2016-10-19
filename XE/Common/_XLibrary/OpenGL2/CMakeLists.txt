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
    ../../../Third_Party_lib
    ../../../Third_Party_lib/ftgles/ftgles2/ftgles/src
    ../../../Third_Party_lib/ftgles/freetype2_android/include
    ../../../
        )


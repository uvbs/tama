#ifdef WIN32
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#ifdef TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#endif

#ifdef ANDROID
#include <GLES2/gl2platform.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

namespace xFTGL {
    const char *xvShader_Color_Texture =
    #include "Shader/ftgles_vshader_color_texture.h"
    const char *xfShader_Color_Texture =
    #include "Shader/ftgles_fshader_color_texture.h"
};
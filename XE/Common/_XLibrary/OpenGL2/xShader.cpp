#include "stdafx.h"
#ifdef _VER_OPENGL
#include "XOpenGL.h"

#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

const char *xvShader_Color_Texture =
#include "OpenGL2/Shader/vshader_color_texture.h"
const char *xfShader_Color_Texture =
#include "OpenGL2/Shader/fshader_color_texture.h"

const char *xfShader_Gray =
#include "OpenGL2/Shader/fshader_gray.h"

const char *xvShader_Texture =
#include "OpenGL2/Shader/vshader_texture.h"
const char *xfShader_Texture =
#include "OpenGL2/Shader/fshader_texture.h"

const char *xvShader_Color =
#include "OpenGL2/Shader/vshader_color.h"
const char *xfShader_Color =
#include "OpenGL2/Shader/fshader_color.h"

const char *xvShader_OneColor =
#include "OpenGL2/Shader/vshader_onecolor.h"
const char *xfShader_OneColor =
#include "OpenGL2/Shader/fshader_onecolor.h"

const char *xvShader_PointSprite =
#include "OpenGL2/Shader/vshader_point_sprite.h"
const char *xfShader_PointSprite =
#include "OpenGL2/Shader/fshader_point_sprite.h"

const char *xfShader_blurH =
#include "OpenGL2/Shader/fshader_blur_horiz.h"
const char *xfShader_blurV =
#include "OpenGL2/Shader/fshader_blur_vert.h"
const char *xvShader_fastblurH =
#include "OpenGL2/Shader/vshader_fastblurH.h"
const char *xvShader_fastblurV =
#include "OpenGL2/Shader/vshader_fastblurV.h"

const char *xfShader_fastblur =
#include "OpenGL2/Shader/fshader_fastblur.h"


#endif // gl
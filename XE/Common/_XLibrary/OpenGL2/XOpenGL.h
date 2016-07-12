#pragma once
#ifdef _VER_OPENGL

#define glDeleteVertexArrays        glDeleteVertexArraysOES
#define glGenVertexArrays           glGenVertexArraysOES
#define glBindVertexArray           glBindVertexArrayOES

#ifdef WIN32
	#include "GLES2/gl2.h"
	#include "GLES2/gl2ext.h"
#elif defined(_VER_IOS)
    #import <OpenGLES/ES2/gl.h>
    #import <OpenGLES/ES2/glext.h>
#elif defined(_VER_ANDROID)
    // GL_GLEXT_PROTOTYPES isn't defined in glplatform.h on android ndk r7
    // we manually define it here
    #include <GLES2/gl2platform.h>
    #define GL_GLEXT_PROTOTYPES
//    #ifndef GL_GLEXT_PROTOTYPES
//        #define GL_GLEXT_PROTOTYPES 1
//    #endif
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#endif
#endif // 
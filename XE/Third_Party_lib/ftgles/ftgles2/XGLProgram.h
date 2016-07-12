#pragma once
#ifdef WIN32
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#if defined(TARGET_OS_IPHONE)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#ifdef ANDROID
#include <GLES2/gl2platform.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif
#include "xftglShader.h"

namespace xFTGL {
    enum { ATTRIB_POS,
        ATTRIB_COLOR,
        ATTRIB_TEXTURE
    };
    union _xGLKMatrix4
    {
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
        float m[16];
#ifdef WIN32
	};
#else
    } __attribute__((aligned(16)));
#endif
    typedef union _xGLKMatrix4 xGLKMatrix4;

class XShader
{
    enum
    {
        UNIFORM_MODELVIEWPROJECTION_MATRIX,
        NUM_UNIFORMS
    };
    GLint m_glProgram;
    GLint m_locUniforms[NUM_UNIFORMS];
    void Init() {
        m_glProgram = 0;
        for( int i = 0; i < NUM_UNIFORMS; ++i )
            m_locUniforms[i] = 0;
    }
    void Destroy() {
        glDeleteProgram( m_glProgram );
    }
public:
    XShader() { Init(); }
    virtual ~XShader() { Destroy(); }
    //
    int LoadShaderFromString( const GLchar *cVertShader, const GLchar *cFragShader );
    int CompileShaderFromString( GLuint *shader, GLenum type, const GLchar *cShader );
    int LinkShader( GLuint prog );
    void SetShader( xGLKMatrix4& mModelViewProjection );
};
}; // FTGL
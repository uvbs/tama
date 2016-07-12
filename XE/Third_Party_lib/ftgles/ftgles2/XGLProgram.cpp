#include "stdio.h"
#include "XGLProgram.h"
#include "stdlib.h"

namespace xFTGL
{
int XShader::LoadShaderFromString( const GLchar *cVertShader, const GLchar *cFragShader )
{
    GLuint vertShader, fragShader;
    // Create shader program.
    m_glProgram = glCreateProgram();
    
    // Create and compile vertex shader.
    if( !CompileShaderFromString( &vertShader, GL_VERTEX_SHADER, cVertShader ) )
    {
//        NSLog(@"Failed to compile vertex shader");
        return 0;
    }
    
    // Create and compile fragment shader.
    if( !CompileShaderFromString( &fragShader, GL_FRAGMENT_SHADER, cFragShader ) )
    {
//        NSLog(@"Failed to compile fragment shader");
        return 0;
    }
    
    // Attach vertex shader to program.
    glAttachShader(m_glProgram, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(m_glProgram, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(m_glProgram, ATTRIB_POS, "position");
    glBindAttribLocation(m_glProgram, ATTRIB_TEXTURE, "texture");
    glBindAttribLocation(m_glProgram, ATTRIB_COLOR, "color");
    
    // Link program.
    if (!LinkShader(m_glProgram)) {
//        NSLog(@"Failed to link program: %d", m_glProgram);
        
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (m_glProgram) {
            glDeleteProgram(m_glProgram);
            m_glProgram = 0;
        }
        
        return 0;
    }
    
    // Get uniform locations.
    m_locUniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX] = glGetUniformLocation(m_glProgram, "modelViewProjectionMatrix");
//    m_locUniforms[UNIFORM_MODEL_MATRIX] = glGetUniformLocation(m_glProgram, "worldMatrix");
    
    // Release vertex and fragment shaders.
    if (vertShader) {
        glDetachShader(m_glProgram, vertShader);
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDetachShader(m_glProgram, fragShader);
        glDeleteShader(fragShader);
    }
    //
//    xGLKMatrix4 mWorld = GLKMatrix4Identity;
//    glUseProgram(m_glProgram);
//    glUniformMatrix4fv(m_locUniforms[UNIFORM_MODEL_MATRIX], 1, 0,mWorld.m);
    
    return 1;
}


int XShader::CompileShaderFromString( GLuint *shader, GLenum type, const GLchar *cShader )
{
    GLint status;
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &cShader, 0);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        printf("Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return 0;
    }
    
    return 1;
}

int XShader::LinkShader( GLuint prog )
{
    GLint status;
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
//        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return 0;
    }
    
    return 1;
}

void XShader::SetShader( xGLKMatrix4& mModelViewProjection )
{
    glUseProgram(m_glProgram);
    glUniformMatrix4fv(m_locUniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0,mModelViewProjection.m);
}

} // FTGL namespace
/*
 
 Copyright (c) 2010 David Petrie
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 */

#include "ftglesGlue.h"
#include "../../../XGLProgram.h"
#include "string.h"


#define FTGLES_GLUE_MAX_VERTICES 32768

#ifdef ANDROID
#define printf	LOGD
#endif

typedef struct 
{
	float xyz[3];
	float st[2];
	GLubyte rgba[4];
} ftglesVertex_t;


typedef struct 
{
	ftglesVertex_t vertices[FTGLES_GLUE_MAX_VERTICES];
	short quadIndices[FTGLES_GLUE_MAX_VERTICES * 3 / 2];
	ftglesVertex_t currVertex;
	unsigned int currIndex;
} ftglesGlueArrays_t;

ftglesGlueArrays_t ftglesGlueArrays;
//
namespace xFTGL {
	const int x_ver = 105;
    XShader *g_pShader = NULL;
//    int x_refCnt;
    xGLKMatrix4 g_mModelViewProjection;
    float x_colorR = 1.0f;
    float x_colorG = 1.0f;
    float x_colorB = 1.0f;
    float x_colorA = 1.0f;
    void SetMatrixModelViewProjection( void *m ) {
        memcpy( g_mModelViewProjection.m, m, sizeof(xGLKMatrix4) );
    }
    void SetColor( float r, float g, float b, float a ) {
        x_colorR = r;
        x_colorG = g;
        x_colorB = b;
        x_colorA = a;
    }
    float GetColorR( void ) { return x_colorR; }
    float GetColorG( void ) { return x_colorG; }
    float GetColorB( void ) { return x_colorB; }
    float GetColorA( void ) { return x_colorA; }
}

GLenum ftglesCurrentPrimitive = GL_TRIANGLES;
bool ftglesQuadIndicesInitted = false;

unsigned char *memTexture[ 1024 * 1024 ] = { 0, };

void xFTGL::CreateShader( void )
{
//	XBREAK( FTGL::g_pShader != NULL );
//    if( FTGL::g_pShader == NULL )
//    {
        g_pShader = new XShader;
        g_pShader->LoadShaderFromString(xFTGL::xvShader_Color_Texture, xFTGL::xfShader_Color_Texture );
//        FTGL::x_refCnt = 1;
//    } else
//        ++FTGL::x_refCnt;
}

void xFTGL::DestroyShader( void )
{
//    if( --FTGL::x_refCnt == 0 )
    {
        delete g_pShader;
        g_pShader = NULL;
    }
}

void xFTGL::Restore( void )
{
	g_pShader->LoadShaderFromString(xFTGL::xvShader_Color_Texture, xFTGL::xfShader_Color_Texture );
//	x_refCnt = 1;
}

GLvoid ftglBegin(GLenum prim)
{
	if (!ftglesQuadIndicesInitted)
	{
		for (int i = 0; i < FTGLES_GLUE_MAX_VERTICES * 3 / 2; i += 6) 
		{
			int q = i / 6 * 4;
			ftglesGlueArrays.quadIndices[i + 0] = q + 0;
			ftglesGlueArrays.quadIndices[i + 1] = q + 1;
			ftglesGlueArrays.quadIndices[i + 2] = q + 2;
			
			ftglesGlueArrays.quadIndices[i + 3] = q + 0;
			ftglesGlueArrays.quadIndices[i + 4] = q + 2;
			ftglesGlueArrays.quadIndices[i + 5] = q + 3;
		}
		ftglesQuadIndicesInitted = true;
	}
	ftglesGlueArrays.currIndex = 0;
	ftglesCurrentPrimitive = prim;
}


GLvoid ftglVertex3f(float x, float y, float z) 
{
	if (ftglesGlueArrays.currIndex >= FTGLES_GLUE_MAX_VERTICES)
	{
		return;
	}
	
	ftglesGlueArrays.currVertex.xyz[0] = x;
	ftglesGlueArrays.currVertex.xyz[1] = y;
	ftglesGlueArrays.currVertex.xyz[2] = z;
	ftglesGlueArrays.vertices[ftglesGlueArrays.currIndex] = ftglesGlueArrays.currVertex;
	ftglesGlueArrays.currIndex++;
}


GLvoid ftglVertex2f(float x, float y) 
{
	if (ftglesGlueArrays.currIndex >= FTGLES_GLUE_MAX_VERTICES)
	{
		return;
	}
	
	ftglesGlueArrays.currVertex.xyz[0] = x;
	ftglesGlueArrays.currVertex.xyz[1] = y;
	ftglesGlueArrays.currVertex.xyz[2] = 0.0f;
	ftglesGlueArrays.vertices[ftglesGlueArrays.currIndex] = ftglesGlueArrays.currVertex;
	ftglesGlueArrays.currIndex++;
}


GLvoid ftglColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a) 
{
	ftglesGlueArrays.currVertex.rgba[0] = r;
	ftglesGlueArrays.currVertex.rgba[1] = g;
	ftglesGlueArrays.currVertex.rgba[2] = b;
	ftglesGlueArrays.currVertex.rgba[3] = a;
}


GLvoid ftglColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) 
{
	ftglesGlueArrays.currVertex.rgba[0] = (GLubyte) (r * 255);
	ftglesGlueArrays.currVertex.rgba[1] = (GLubyte) (g * 255);
	ftglesGlueArrays.currVertex.rgba[2] = (GLubyte) (b * 255);
	ftglesGlueArrays.currVertex.rgba[3] = (GLubyte) (a * 255);
}


GLvoid ftglTexCoord2f(GLfloat s, GLfloat t) 
{
	ftglesGlueArrays.currVertex.st[0] = s;
	ftglesGlueArrays.currVertex.st[1] = t;
}


GLvoid bindArrayBuffers()
{
}


GLvoid ftglBindTexture()
{
	
}


GLvoid ftglEnd() 
{
//	GLboolean vertexArrayEnabled;
//	GLboolean texCoordArrayEnabled;
//	GLboolean colorArrayEnabled;
	
//	GLvoid * s_vertexArrayPointer;
//	GLvoid * s_texCoordArrayPointer;
//	GLvoid * s_colorArrayPointer;
	
//	GLint vertexArrayType, texCoordArrayType, colorArrayType;
//	GLint vertexArraySize, texCoordArraySize, colorArraySize;
//	GLsizei vertexArrayStride, texCoordArrayStride, colorArrayStride;
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 1024, 1024,
//                 0, GL_ALPHA, GL_UNSIGNED_BYTE, memTexture);
    // glTexSubImage2D할때메모리에다 쓰게바꾸고 최종 ftglEnd 할때 한번에 glTexImage2d로 전송하게하자
	
	bool resetPointers = false;
	
//	glGetPointerv(GL_VERTEX_ARRAY_POINTER, &vertexArrayPointer);
//	glGetPointerv(GL_TEXTURE_COORD_ARRAY_POINTER, &texCoordArrayPointer);
//	glGetPointerv(GL_COLOR_ARRAY_POINTER, &colorArrayPointer);

//	glGetBooleanv(GL_VERTEX_ARRAY, &vertexArrayEnabled);
//	glGetBooleanv(GL_TEXTURE_COORD_ARRAY, &texCoordArrayEnabled);
//	glGetBooleanv(GL_COLOR_ARRAY, &colorArrayEnabled);
    xFTGL::g_pShader->SetShader( xFTGL::g_mModelViewProjection );
#ifdef _XVAO
    glBindVertexArrayOES( 0 );
#endif
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray( xFTGL::ATTRIB_POS );
//	if (!vertexArrayEnabled)
//	{
//		glEnableClientState(GL_VERTEX_ARRAY);
//	}
	
//	if (s_vertexArrayPointer != &ftglesGlueArrays.vertices[0].xyz)
//	{
//		glGetIntegerv(GL_VERTEX_ARRAY_TYPE, &vertexArrayType);
//		glGetIntegerv(GL_VERTEX_ARRAY_SIZE, &vertexArraySize);
//		glGetIntegerv(GL_VERTEX_ARRAY_STRIDE, &vertexArrayStride);
//		if (texCoordArrayEnabled)
//		{
//			glGetIntegerv(GL_TEXTURE_COORD_ARRAY_TYPE, &texCoordArrayType);
//			glGetIntegerv(GL_TEXTURE_COORD_ARRAY_SIZE, &texCoordArraySize);
//			glGetIntegerv(GL_TEXTURE_COORD_ARRAY_STRIDE, &texCoordArrayStride);
//		}
//		if (colorArrayEnabled)
//		{
//			glGetIntegerv(GL_COLOR_ARRAY_TYPE, &colorArrayType);
//			glGetIntegerv(GL_COLOR_ARRAY_SIZE, &colorArraySize);
//			glGetIntegerv(GL_COLOR_ARRAY_STRIDE, &colorArrayStride);
//		}
//		glVertexPointer(3, GL_FLOAT, sizeof(ftglesVertex_t), ftglesGlueArrays.vertices[0].xyz);
//		glTexCoordPointer(2, GL_FLOAT, sizeof(ftglesVertex_t), ftglesGlueArrays.vertices[0].st);
//		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ftglesVertex_t), ftglesGlueArrays.vertices[0].rgba);
        glVertexAttribPointer( xFTGL::ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, sizeof(ftglesVertex_t), ftglesGlueArrays.vertices[0].xyz);
        glVertexAttribPointer( xFTGL::ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(ftglesVertex_t), ftglesGlueArrays.vertices[0].st);
// 		ftglesGlueArrays.vertices[0].rgba[3] = 0;
// 		ftglesGlueArrays.vertices[0].rgba[0] = 255;
        glVertexAttribPointer( xFTGL::ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(ftglesVertex_t), ftglesGlueArrays.vertices[0].rgba);
		
		resetPointers = true;
//	}
	
//	if (!texCoordArrayEnabled)
//		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
//	if (!colorArrayEnabled)
//		glEnableClientState(GL_COLOR_ARRAY);
    glEnableVertexAttribArray( xFTGL::ATTRIB_COLOR );
    glEnableVertexAttribArray( xFTGL::ATTRIB_TEXTURE );
	
	if (ftglesGlueArrays.currIndex == 0) 
	{
		ftglesCurrentPrimitive = 0;
		return;
	}
	
	if (ftglesCurrentPrimitive == GL_QUADS) 
	{
		glDrawElements(GL_TRIANGLES, ftglesGlueArrays.currIndex / 4 * 6, GL_UNSIGNED_SHORT, ftglesGlueArrays.quadIndices);
	} 
	else 
	{
		glDrawArrays(ftglesCurrentPrimitive, 0, ftglesGlueArrays.currIndex);
	}
	ftglesGlueArrays.currIndex = 0;
	ftglesCurrentPrimitive = 0;
	
	if (resetPointers)
	{
/*		if (vertexArrayEnabled)
		{
			glVertexPointer(vertexArraySize, vertexArrayType, 
							vertexArrayStride, vertexArrayPointer);	
		}
		if (texCoordArrayEnabled)
		{
			glTexCoordPointer(texCoordArraySize, texCoordArrayType, 
							  texCoordArrayStride, texCoordArrayPointer);
		}
		if (colorArrayEnabled)
		{
			glColorPointer(colorArraySize, colorArrayType, 
						   colorArrayStride, colorArrayPointer);
		}
 */
	}
	
    glDisableVertexAttribArray(xFTGL::ATTRIB_COLOR);
    glDisableVertexAttribArray(xFTGL::ATTRIB_TEXTURE);
/*	if (!vertexArrayEnabled)
		glDisableClientState(GL_VERTEX_ARRAY);
	
	if (!texCoordArrayEnabled)
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	if (!colorArrayEnabled)
		glDisableClientState(GL_COLOR_ARRAY);
 */
}


GLvoid ftglError(const char *source)
{
	GLenum error = glGetError();
	 
	switch (error) {
		case GL_NO_ERROR:
			break;
		case GL_INVALID_ENUM:
			printf("GL Error (%x): GL_INVALID_ENUM. %s\n\n", error, source);
			break;
		case GL_INVALID_VALUE:
			printf("GL Error (%x): GL_INVALID_VALUE. %s\n\n", error, source);
			break;
		case GL_INVALID_OPERATION:
			printf("GL Error (%x): GL_INVALID_OPERATION. %s\n\n", error, source);
			break;
//		case GL_STACK_OVERFLOW:
//			printf("GL Error (%x): GL_STACK_OVERFLOW. %s\n\n", error, source);
//			break;
//		case GL_STACK_UNDERFLOW:
//			printf("GL Error (%x): GL_STACK_UNDERFLOW. %s\n\n", error, source);
//			break;
		case GL_OUT_OF_MEMORY:
			printf("GL Error (%x): GL_OUT_OF_MEMORY. %s\n\n", error, source);
			break;
		default:
			printf("GL Error (%x): %s\n\n", error, source);
			break;
	}
}


LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ftgl2_static

LOCAL_MODULE_FILENAME := libftgl2


LOCAL_SRC_FILES := \
../xftglShader.cpp \
../XGLProgram.cpp \
src/FTBuffer.cpp \
src/FTCharmap.cpp \
src/FTContour.cpp \
src/FTFace.cpp \
src/FTGlyphContainer.cpp \
src/FTLibrary.cpp \
src/FTPoint.cpp \
src/FTSize.cpp \
src/FTVectoriser.cpp \
src/FTFont/FTFont.cpp \
src/FTFont/FTFontGlue.cpp \
src/FTFont/FTTextureFont.cpp \
src/FTGL/ftglesGlue.cpp \
src/FTGlyph/FTGlyph.cpp \
src/FTGlyph/FTGlyphGlue.cpp \
src/FTGlyph/FTTextureGlyph.cpp \
src/FTLayout/FTLayout.cpp \
src/FTLayout/FTLayoutGlue.cpp \
src/FTLayout/FTSimpleLayout.cpp \
src/iGLU-1.0.0/libtess/dict.c \
src/iGLU-1.0.0/libtess/geom.c \
src/iGLU-1.0.0/libtess/memalloc.c \
src/iGLU-1.0.0/libtess/mesh.c \
src/iGLU-1.0.0/libtess/normal.c \
src/iGLU-1.0.0/libtess/priorityq.c \
src/iGLU-1.0.0/libtess/render.c \
src/iGLU-1.0.0/libtess/sweep.c \
src/iGLU-1.0.0/libtess/tess.c \
src/iGLU-1.0.0/libtess/tessmono.c \
src/iGLU-1.0.0/libutil/error.c \
src/iGLU-1.0.0/libutil/glue.c \
src/iGLU-1.0.0/libutil/project.c \
src/iGLU-1.0.0/libutil/registry.c \


LOCAL_C_INCLUDES := $(LOCAL_PATH) \
					$(LOCAL_PATH)/src/ \
					$(LOCAL_PATH)/src/iGLU-1.0.0/include/ \
					$(LOCAL_PATH)/../../freetype2_android/include/ \
					$(LOCAL_PATH)/../ \
					$(LOCAL_PATH)/../Shader/ \
					$(LOCAL_PATH)/../../../../Common/_XLibrary/ \
					

include $(BUILD_STATIC_LIBRARY)


"attribute vec4 position;           \n\
attribute vec2 texture;             \n\
attribute vec4 color;               \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
                                    \n\
uniform mat4 modelViewProjectionMatrix; \n\
                                    \n\
void main()                         \n\
{                                   \n\
     colorVarying = color;          \n\
     texCoord = texture;            \n\
                                    \n\
    gl_Position = modelViewProjectionMatrix * position;   \n\
}   \n\
";

"attribute vec4 position;           \n\
attribute vec4 color;               \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
                                    \n\
uniform mat4 mMVP; \n\
                                    \n\
void main()                         \n\
{                                   \n\
     colorVarying = color;          \n\
                                    \n\
    gl_Position = mMVP * position;        \n\
}   \n\
";

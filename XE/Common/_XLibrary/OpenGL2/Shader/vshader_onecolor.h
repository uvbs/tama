"attribute vec4 position;           \n\
attribute vec2 texture;             \n\
attribute vec4 color;               \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
                                    \n\
uniform mat4 mMVP; \n\
uniform vec4 col; \n\
                                    \n\
void main()                         \n\
{                                   \n\
	colorVarying = vec4(1.0, 1.0, 1.0, 1.0);          \n\
	texCoord = texture;            \n\
                                    \n\
	gl_Position = mMVP * position;        \n\
}   \n\
";

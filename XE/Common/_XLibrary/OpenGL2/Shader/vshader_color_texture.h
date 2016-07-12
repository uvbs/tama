"attribute vec4 position;           \n\
attribute vec2 texture;             \n\
attribute vec4 color;               \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
varying lowp float value;         \n\
                                    \n\
uniform mat4 mMVP; \n\
uniform vec4 col; \n\
uniform float val; \n\
                                    \n\
void main()                         \n\
{                                   \n\
	colorVarying = color * col;          \n\
	texCoord = texture;            \n\
	value = val;					\n\
	gl_Position = mMVP * position;   \n\
}   \n\
";



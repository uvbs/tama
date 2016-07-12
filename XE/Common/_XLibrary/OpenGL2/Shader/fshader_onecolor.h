"uniform sampler2D tex;             \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
                                    \n\
void main()                         \n\
{                                   \n\
	lowp vec4 t = texture2D(tex, texCoord); \n\
	t.x = colorVarying.x; \n\
	t.y = colorVarying.y; \n\
	t.z = colorVarying.z; \n\
	gl_FragColor = t;     \n\
}                                   \n\
";

"uniform sampler2D tex;             \n\
                                    \n\
varying lowp vec2 texCoord;         \n\
                                    \n\
void main()                         \n\
{                                   \n\
	gl_FragColor = texture2D(tex, texCoord);     \n\
}                                   \n\
";

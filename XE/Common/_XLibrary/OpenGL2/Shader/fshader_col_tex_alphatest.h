"uniform sampler2D tex;             \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
                                    \n\
void main()                         \n\
{                                   \n\
	lowp vec4 pix = texture2D(tex, texCoord) * colorVarying;     \n\
	if( pix.w < 0.5 )		discard; \n\
	gl_FragColor = pix; \n\
}                                   \n\
";

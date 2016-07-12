"uniform sampler2D tex;             \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
                                    \n\
void main()                         \n\
{                                   \n\
	highp vec4 pixel = texture2D(tex, texCoord) * colorVarying; \n\
	lowp float gr = (pixel.r + pixel.g + pixel.b) / 3.0; \n\
	gl_FragColor = vec4(gr,gr,gr,pixel.a);     \n\
}                                   \n\
";


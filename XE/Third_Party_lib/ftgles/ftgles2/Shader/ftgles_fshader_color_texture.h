"uniform sampler2D tex;             \n\
                                    \n\
varying highp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
                                    \n\
void main()                         \n\
{                                    \n\
    lowp vec4 col = texture2D(tex, texCoord); \n\
    highp vec4 c = vec4( colorVarying.r/255.0, colorVarying.g/255.0, colorVarying.b/255.0, colorVarying.a/255.0 ); \n\
    gl_FragColor = vec4( 1.0, 1.0, 1.0, col.a ) * c;     \n\
}                                   \n\
";

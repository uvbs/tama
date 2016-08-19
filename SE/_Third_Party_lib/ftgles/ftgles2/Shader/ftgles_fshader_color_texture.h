"uniform sampler2D tex;             \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
                                    \n\
void main()                         \n\
{                                    \n\
    lowp vec4 col = texture2D(tex, texCoord); \n\
    gl_FragColor = vec4( col.a, col.a, col.a, col.a ) * colorVarying;     \n\
}                                   \n\
";
//    vec4 ccc = vec4( 1.0, 0, 0, 1.0 ); \n\
//vec4 col = vec4( 1.0, 1.0, 1.0, 1.0 );  \n\
//gl_FragColor = texture2D(tex, texCoord) * colorVarying;     \n\

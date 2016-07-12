"uniform sampler2D tex;             \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
                                    \n\
void main()                         \n\
{                                   \n\
    gl_FragColor = texture2D(tex, gl_PointCoord) * colorVarying;     \n\
}                                   \n\
";


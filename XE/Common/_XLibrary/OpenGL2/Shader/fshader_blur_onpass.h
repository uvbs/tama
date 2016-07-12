"uniform sampler2D tex;             \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
lowp float blurSize = 1.0/84.0;    \n\
                                    \n\
void main()                         \n\
{                                   \n\
    lowp vec4 sum = vec4(0.0);      \n\
    sum += texture2D(tex, vec2(texCoord.x - 4.0*blurSize, texCoord.y));     \n\
    sum += texture2D(tex, vec2(texCoord.x - 3.0*blurSize, texCoord.y));     \n\
    sum += texture2D(tex, vec2(texCoord.x - 2.0*blurSize, texCoord.y));  \n\
    sum += texture2D(tex, vec2(texCoord.x - blurSize, texCoord.y));  \n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y));  \n\
    sum += texture2D(tex, vec2(texCoord.x + blurSize, texCoord.y));  \n\
    sum += texture2D(tex, vec2(texCoord.x + 2.0*blurSize, texCoord.y));  \n\
    sum += texture2D(tex, vec2(texCoord.x + 3.0*blurSize, texCoord.y));  \n\
    sum += texture2D(tex, vec2(texCoord.x + 4.0*blurSize, texCoord.y));  \n\
\n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 4.0*blurSize)); \n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 3.0*blurSize)); \n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 2.0*blurSize)); \n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y - blurSize)); \n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y + blurSize)); \n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 2.0*blurSize)); \n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 3.0*blurSize)); \n\
    sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 4.0*blurSize)); \n\
    \n\
    gl_FragColor = sum / 17.0;     \n\
}                                   \n\
";

/*
*/

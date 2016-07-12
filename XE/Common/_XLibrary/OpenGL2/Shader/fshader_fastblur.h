"precision mediump float;   \n\
uniform sampler2D s_texture;             \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 v_texCoord;         \n\
varying vec2 v_blurTexCoords[14];   \n\
                                    \n\
void main()                         \n\
{                                   \n\
    lowp vec4 sum = vec4(0,0,0,0);      \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 0])*0.0044299121055113265;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 1])*0.00895781211794;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 2])*0.0215963866053;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 3])*0.0443683338718;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 4])*0.0776744219933;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 5])*0.115876621105;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 6])*0.147308056121;    \n\
    sum += texture2D(s_texture, v_texCoord         )*0.159576912161;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 7])*0.147308056121;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 8])*0.115876621105;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[ 9])*0.0776744219933;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[10])*0.0443683338718;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[11])*0.0215963866053;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[12])*0.00895781211794;    \n\
    sum += texture2D(s_texture, v_blurTexCoords[13])*0.0044299121055113265;    \n\
\n\
    gl_FragColor = sum;     \n\
}                                   \n\
";


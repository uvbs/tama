"attribute vec4 position;           \n\
attribute vec2 texture;             \n\
attribute vec4 color;               \n\
                                    \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 v_texCoord;         \n\
varying vec2 v_blurTexCoords[14];   \n\
uniform mat4 mMVP; \n\
                                    \n\
void main()                         \n\
{                                   \n\
     colorVarying = color;          \n\
     v_texCoord = texture;            \n\
                                    \n\
    gl_Position = mMVP * position;   \n\
    v_blurTexCoords[ 0] = v_texCoord + vec2(-0.028, 0.0);   \n\
    v_blurTexCoords[ 1] = v_texCoord + vec2(-0.024, 0.0);   \n\
    v_blurTexCoords[ 2] = v_texCoord + vec2(-0.020, 0.0);   \n\
    v_blurTexCoords[ 3] = v_texCoord + vec2(-0.016, 0.0);   \n\
    v_blurTexCoords[ 4] = v_texCoord + vec2(-0.012, 0.0);   \n\
    v_blurTexCoords[ 5] = v_texCoord + vec2(-0.008, 0.0);   \n\
    v_blurTexCoords[ 6] = v_texCoord + vec2(-0.004, 0.0);   \n\
    v_blurTexCoords[ 7] = v_texCoord + vec2( 0.004, 0.0);   \n\
    v_blurTexCoords[ 8] = v_texCoord + vec2( 0.008, 0.0);   \n\
    v_blurTexCoords[ 9] = v_texCoord + vec2( 0.012, 0.0);   \n\
    v_blurTexCoords[10] = v_texCoord + vec2( 0.016, 0.0);   \n\
    v_blurTexCoords[11] = v_texCoord + vec2( 0.020, 0.0);   \n\
    v_blurTexCoords[12] = v_texCoord + vec2( 0.024, 0.0);   \n\
    v_blurTexCoords[13] = v_texCoord + vec2( 0.028, 0.0);   \n\
}   \n\
";

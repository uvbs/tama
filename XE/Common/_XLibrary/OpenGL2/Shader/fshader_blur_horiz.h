"uniform sampler2D tex;             \n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
varying lowp float value;         \n\
lowp float blurSize = 1.0/512.0;    \n\
void main()                         \n\
{                                   \n\
lowp vec4 sum = vec4(0,0,0,0);      \n\
sum += texture2D(tex, vec2(texCoord.x - 7.0*blurSize, texCoord.y)) * 0.02161;  \n\
sum += texture2D(tex, vec2(texCoord.x - 6.0*blurSize, texCoord.y)) * 0.03244;  \n\
sum += texture2D(tex, vec2(texCoord.x - 5.0*blurSize, texCoord.y)) * 0.04574;  \n\
sum += texture2D(tex, vec2(texCoord.x - 4.0*blurSize, texCoord.y)) * 0.06060;  \n\
sum += texture2D(tex, vec2(texCoord.x - 3.0*blurSize, texCoord.y)) * 0.07541;  \n\
sum += texture2D(tex, vec2(texCoord.x - 2.0*blurSize, texCoord.y)) * 0.08817;  \n\
sum += texture2D(tex, vec2(texCoord.x - blurSize, texCoord.y))      * 0.09683;  \n\
sum += texture2D(tex, vec2(texCoord.x, texCoord.y))                 * 0.09991;  \n\
sum += texture2D(tex, vec2(texCoord.x + blurSize, texCoord.y))      * 0.09683;  \n\
sum += texture2D(tex, vec2(texCoord.x + 2.0*blurSize, texCoord.y)) * 0.08817;  \n\
sum += texture2D(tex, vec2(texCoord.x + 3.0*blurSize, texCoord.y)) * 0.07541;  \n\
sum += texture2D(tex, vec2(texCoord.x + 4.0*blurSize, texCoord.y)) * 0.06060;  \n\
sum += texture2D(tex, vec2(texCoord.x + 5.0*blurSize, texCoord.y)) * 0.04574;  \n\
sum += texture2D(tex, vec2(texCoord.x + 6.0*blurSize, texCoord.y)) * 0.03244;  \n\
sum += texture2D(tex, vec2(texCoord.x + 7.0*blurSize, texCoord.y)) * 0.02161;  \n\
//lowp float str = 1.0 + (0.5 - sum.x) * 5.0;	\n\
sum.a *= 3.0; \n\
gl_FragColor = sum;     \n\
}                                   \n\
";

/*
"uniform sampler2D tex;             \n\
\n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
lowp float blurSize = 1.0/512.0;    \n\
\n\
void main()                         \n\
{                                   \n\
	lowp vec4 sum = vec4(0); \n\
	lowp vec2 texcoord = texCoord; \n\
	int j; \n\
	int i; \n\
	\n\
	for( i= -4 ;i < 4; i++) \n\
	{ \n\
		for (j = -3; j < 3; j++) \n\
		{ \n\
			sum += texture2D(tex, texcoord + vec2(j, i)*blurSize) * 0.25; \n\
		} \n\
	} \n\
	if (texture2D(tex, texcoord).r < 0.3) \n\
	{ \n\
		gl_FragColor = sum*sum*0.012 + texture2D(tex, texcoord); \n\
	} \n\
	else \n\
	{ \n\
		if (texture2D(tex, texcoord).r < 0.5) \n\
		{ \n\
			gl_FragColor = sum*sum*0.009 + texture2D(tex, texcoord); \n\
		} \n\
		else \n\
		{ \n\
			gl_FragColor = sum*sum*0.0075 + texture2D(tex, texcoord); \n\
		} \n\
	} \n\
}                                   \n\
";
 */

// 잘 안보이면 증폭도 시켜볼것
/*
 소프트 라이트 공식.
 if (Blend > ½) R = 1 - (1-Base) × (1-(Blend-½))
 if (Blend <= ½) R = Base × (Blend+½) 
 pixel shader code
 float4 Overlay;
 if( Color.r > 0.5f )      Overlay.r = 1.0f - (1.0f-Color.r) * (1-(Color.r-0.5f));
 else if( Color.r <= 0.5f) Overlay.r = Color.r * (Color.r+0.5f);
 
 Color += Overlay;
 */
/*
"uniform sampler2D tex;             \n\
\n\
varying lowp vec4 colorVarying;     \n\
varying lowp vec2 texCoord;         \n\
lowp float blurSize = 1.0/512.0;    \n\
\n\
void main()                         \n\
{                                   \n\
lowp vec4 sum = vec4(0,0,0,0);      \n\
sum += texture2D(tex, vec2(texCoord.x - 7.0*blurSize, texCoord.y)) * 0.02161;  \n\
sum += texture2D(tex, vec2(texCoord.x - 6.0*blurSize, texCoord.y)) * 0.03244;  \n\
sum += texture2D(tex, vec2(texCoord.x - 5.0*blurSize, texCoord.y)) * 0.04574;  \n\
sum += texture2D(tex, vec2(texCoord.x - 4.0*blurSize, texCoord.y)) * 0.06060;  \n\
sum += texture2D(tex, vec2(texCoord.x - 3.0*blurSize, texCoord.y)) * 0.07541;  \n\
sum += texture2D(tex, vec2(texCoord.x - 2.0*blurSize, texCoord.y)) * 0.08817;  \n\
sum += texture2D(tex, vec2(texCoord.x - blurSize, texCoord.y))      * 0.09683;  \n\
sum += texture2D(tex, vec2(texCoord.x, texCoord.y))                 * 0.09991;  \n\
sum += texture2D(tex, vec2(texCoord.x + blurSize, texCoord.y))      * 0.09683;  \n\
sum += texture2D(tex, vec2(texCoord.x + 2.0*blurSize, texCoord.y)) * 0.08817;  \n\
sum += texture2D(tex, vec2(texCoord.x + 3.0*blurSize, texCoord.y)) * 0.07541;  \n\
sum += texture2D(tex, vec2(texCoord.x + 4.0*blurSize, texCoord.y)) * 0.06060;  \n\
sum += texture2D(tex, vec2(texCoord.x + 5.0*blurSize, texCoord.y)) * 0.04574;  \n\
sum += texture2D(tex, vec2(texCoord.x + 6.0*blurSize, texCoord.y)) * 0.03244;  \n\
sum += texture2D(tex, vec2(texCoord.x + 7.0*blurSize, texCoord.y)) * 0.02161;  \n\
\n\
gl_FragColor = sum;     \n\
}                                   \n\
";
*/

/*
"uniform sampler2D image; \n\
out vec4 FragmentColor; \n\
uniform float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 ); \n\
uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, \n\
								  0.0540540541, 0.0162162162 ); \n\
void main(void) \n\
{ \n\
	FragmentColor = texture2D( image, vec2(gl_FragCoord)/1024.0 ) * weight[0]; \n\
	for (int i=1; i<5; i++) { \n\
		FragmentColor += \n\
		texture2D( image, ( vec2(gl_FragCoord)+vec2(0.0, offset[i]) )/1024.0 ) \n\
							* weight[i]; \n\
		FragmentColor += \n\
		texture2D( image, ( vec2(gl_FragCoord)-vec2(0.0, offset[i]) )/1024.0 ) \n\
							* weight[i]; \n\
	} \n\
} \n\
";
*/
/*
 "uniform sampler2D tex;             \n\
 \n\
 varying lowp vec4 colorVarying;     \n\
 varying lowp vec2 texCoord;         \n\
 lowp float blurSize = 1.0/512.0;    \n\
 \n\
 void main()                         \n\
 {                                   \n\
 lowp vec4 sum = vec4(0,0,0,0);      \n\
 sum += texture2D(tex, vec2(texCoord.x - 7.0*blurSize, texCoord.y)) * 0.0044299121055113265;  \n\
 sum += texture2D(tex, vec2(texCoord.x - 6.0*blurSize, texCoord.y)) * 0.00895781211794;  \n\
 sum += texture2D(tex, vec2(texCoord.x - 5.0*blurSize, texCoord.y)) * 0.0215963866053;  \n\
 sum += texture2D(tex, vec2(texCoord.x - 4.0*blurSize, texCoord.y)) * 0.0443683338718;  \n\
 sum += texture2D(tex, vec2(texCoord.x - 3.0*blurSize, texCoord.y)) * 0.0776744219933;  \n\
 sum += texture2D(tex, vec2(texCoord.x - 2.0*blurSize, texCoord.y)) * 0.115876621105;  \n\
 sum += texture2D(tex, vec2(texCoord.x - blurSize, texCoord.y))      * 0.147308056121;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y))                 * 0.159576912161;  \n\
 sum += texture2D(tex, vec2(texCoord.x + blurSize, texCoord.y))      * 0.147308056121;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 2.0*blurSize, texCoord.y)) * 0.115876621105;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 3.0*blurSize, texCoord.y)) * 0.0776744219933;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 4.0*blurSize, texCoord.y)) * 0.0443683338718;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 5.0*blurSize, texCoord.y)) * 0.0215963866053;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 6.0*blurSize, texCoord.y)) * 0.00895781211794;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 7.0*blurSize, texCoord.y)) * 0.0044299121055113265;  \n\
 \n\
 gl_FragColor = sum;     \n\
 }                                   \n\
 ";
*/


/*
 sum += texture2D(tex, vec2(texCoord.x - 4.0*blurSize, texCoord.y)) * 0.05;  \n\
 sum += texture2D(tex, vec2(texCoord.x - 3.0*blurSize, texCoord.y)) * 0.09;  \n\
 sum += texture2D(tex, vec2(texCoord.x - 2.0*blurSize, texCoord.y)) * 0.12;  \n\
 sum += texture2D(tex, vec2(texCoord.x - blurSize, texCoord.y)) * 0.15;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y)) * 0.16;  \n\
 sum += texture2D(tex, vec2(texCoord.x + blurSize, texCoord.y)) * 0.15;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 2.0*blurSize, texCoord.y)) * 0.12;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 3.0*blurSize, texCoord.y)) * 0.09;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 4.0*blurSize, texCoord.y)) * 0.05;  \n\
*/


/*
 "uniform sampler2D tex;             \n\
 \n\
 varying lowp vec4 colorVarying;     \n\
 varying lowp vec2 texCoord;         \n\
 lowp float blurSize = 1.0/512.0;    \n\
 lowp float offset[8] = float[](0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0); \n\
 \n\
 lowp float weight[8] = float[]( 0.159576912161,\n\
 0.147308056121, \n\
 0.115876621105, \n\
 0.0776744219933, \n\
 0.0443683338718, \n\
 0.0215963866053, \n\
 0.00895781211794, \n\
 0.0044299121055113265 ); \n\
 \n\
 \n\
 void main()                         \n\
 {\n\
 //	lowp vec4 sum = texture2D(tex, vec2(texCoord.x, texCoord.y)) * weight[0];      \n\
 lowp vec4 sum = vec4(0,0,0,1.0);\n\
 for(int i=1; i<0; i++) { \n\
 sum += texture2D(tex, vec2(texCoord.x - offset[i] * blurSize, texCoord.y)) * weight[i]; \n\
 sum += texture2D(tex, vec2(texCoord.x + offset[i] * blurSize, texCoord.y)) * weight[i]; \n\
 } \n\
 gl_FragColor = sum;     \n\
 } \n\
 ";

*/

/*
 sum += texture2D(tex, vec2(texCoord.x - 2.0*blurSize, texCoord.y)) * 0.05401;  \n\
 sum += texture2D(tex, vec2(texCoord.x - blurSize, texCoord.y))      * 0.24204;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y))                 * 0.39905;  \n\
 sum += texture2D(tex, vec2(texCoord.x + blurSize, texCoord.y))      * 0.24204;  \n\
 sum += texture2D(tex, vec2(texCoord.x + 2.0*blurSize, texCoord.y)) * 0.05401;  \n\

*/

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

 "uniform sampler2D tex;             \n\
 \n\
 varying lowp vec4 colorVarying;     \n\
 varying lowp vec2 texCoord;         \n\
 lowp float blurSize = 1.0/512.0;    \n\
 \n\
 void main()                         \n\
 {                                   \n\
 lowp vec4 sum = vec4(0.0);      \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 7.0*blurSize)) * 0.02161;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 6.0*blurSize)) * 0.03244;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 5.0*blurSize)) * 0.04574;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 4.0*blurSize)) * 0.06060;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 3.0*blurSize)) * 0.07541;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 2.0*blurSize)) * 0.08817;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - blurSize))      *0.09683;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y))                 *0.09991;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + blurSize))      *0.09683;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 2.0*blurSize)) * 0.08817;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 3.0*blurSize)) * 0.07541;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 4.0*blurSize)) * 0.06060;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 5.0*blurSize)) * 0.04574;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 6.0*blurSize)) * 0.03244;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 7.0*blurSize)) * 0.02161;  \n\
// lowp float str = 1.0 + (1.0 - sum.x) * 5.0;	\n\
sum.a *= 3.0; \n\
 gl_FragColor = sum;     \n\
 }                                   \n\
 ";

/*
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 2.0*blurSize)) * 0.05401;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - blurSize))     * 0.24204;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y))                * 0.39905;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + blurSize))     * 0.24204;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 2.0*blurSize)) * 0.05401;  \n\

*/

/*

 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 7.0*blurSize)) * 0.0044299121055113265;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 6.0*blurSize)) * 0.00895781211794;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 5.0*blurSize)) * 0.0215963866053;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 4.0*blurSize)) * 0.0443683338718;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 3.0*blurSize)) * 0.0776744219933;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - 2.0*blurSize)) * 0.115876621105;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y - blurSize))      * 0.147308056121;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y))                 * 0.159576912161;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + blurSize))      * 0.147308056121;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 2.0*blurSize)) * 0.115876621105;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 3.0*blurSize)) * 0.0776744219933;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 4.0*blurSize)) * 0.0443683338718;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 5.0*blurSize)) * 0.0215963866053;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 6.0*blurSize)) * 0.00895781211794;  \n\
 sum += texture2D(tex, vec2(texCoord.x, texCoord.y + 7.0*blurSize)) * 0.0044299121055113265;  \n\
 
 */


/*
 vec4 sum = vec4(0);
 vec2 texcoord = vec2(gl_TexCoord[0]);
 int j;
 int i;
 
 for( i= -4 ;i < 4; i++)
 {
 for (j = -3; j < 3; j++)
 {
 sum += texture2D(bgl_RenderedTexture, texcoord + vec2(j, i)*0.004) * 0.25;
 }
 }
 if (texture2D(bgl_RenderedTexture, texcoord).r < 0.3)
 {
 gl_FragColor = sum*sum*0.012 + texture2D(bgl_RenderedTexture, texcoord);
 }
 else
 {
 if (texture2D(bgl_RenderedTexture, texcoord).r < 0.5)
 {
 gl_FragColor = sum*sum*0.009 + texture2D(bgl_RenderedTexture, texcoord);
 }
 else
 {
 gl_FragColor = sum*sum*0.0075 + texture2D(bgl_RenderedTexture, texcoord);
 }
 }*/

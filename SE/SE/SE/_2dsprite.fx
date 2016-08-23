//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

float4x4 mViewProj : mViewProj;
float4x4 mWorld : mWorld;
texture Sprite;
float fOpacity;
float col_r;
float col_g;
float col_b;

//-----------------------------------------------------------------------------

struct VS_INPUT {
    float4 Position : POSITION;
//    float4 Diffuse : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 Position  : POSITION;
    float4 Diffuse : COLOR0;
    float2 TexCoord0 : TEXCOORD0;
};

//-----------------------------------------------------------------------------

sampler SpriteSampler = sampler_state
{
    Texture = <Sprite>;
    MinFilter = Point;  
    MagFilter = Point;
    MipFilter = None;
    AddressU  = Clamp;
    AddressV  = Clamp;
};

//-----------------------------------------------------------------------------

VS_OUTPUT MainVS(VS_INPUT IN)
{
    VS_OUTPUT OUT;
    
    float4 pos;
    pos = mul( IN.Position, mWorld );
    pos.z = 0;										// 3d�� 2d�� ���� x,y,z�� z�� ������� �ʴµ� Y�� ȸ������Ű�� z�� ���� ���ܹ����� ����� �ȵǴ� ������ ���� �׷��� z�� ������ 0���� ������
    OUT.Position = mul( pos, mViewProj );
    OUT.Diffuse = float4( 1, 1, 1, 1 );
    OUT.TexCoord0 = IN.TexCoord0;

    return OUT;
}

//-----------------------------------------------------------------------------
float4 MainPS(VS_OUTPUT IN) : COLOR 
{
	float4 color;
	color = tex2D( SpriteSampler, IN.TexCoord0 );
	color.a = fOpacity * color.a;
	color.r = color.r * col_r;
	color.g = color.g * col_g;
	color.b = color.b * col_b;
	return color;
}

//-----------------------------------------------------------------------------

technique RenderSprite
{
    pass P0
    {
        VertexShader = compile vs_2_0 MainVS();
        PixelShader = compile ps_2_0 MainPS();
        
        ZEnable          = False;
        AlphaBlendEnable = True;
//		SrcBlend = SRCALPHA;
//		DestBlend = INVSRCALPHA;
        Lighting         = False;
//        CullMode         = CCW;
    
//        TexCoordIndex[0] = 0;
//        TexCoordIndex[1] = 1;
//       TextureTransformFlags[0] = Projected;
//        TextureTransformFlags[1] = Projected;
    }
}

//-----------------------------------------------------------------------------

Texture2D tex1 : register(t0);
SamplerState sam1 : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    float4 time;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = float4(input.uv, sin(input.uv.x * 12), 1);
     
    float2 uv = input.uv ;
    uv.y -= 2;
    color = tex1.Sample(sam1, uv);
    color.a = dot(color.rgb, 1);
    
    return color;
}


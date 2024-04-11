#include <../lib/constBuf.shader>
#include <../lib/io.shader>

Texture2D tex1 : register(t0);
Texture2D tex2 : register(t1);

SamplerState sam1 : register(s0);

//[
cbuffer params : register(b1)
{
    float mix;  
}
//]

float4 PS(VS_OUTPUT input) : SV_Target
{
     
    float2 uv = input.uv;
    uv.y -= 2;
    float4 color1 = tex1.Sample(sam1, uv);
    float4 color2 = tex2.Sample(sam1, uv);

    float4 color = lerp(color1,color2,mix);
    
    return color;
}
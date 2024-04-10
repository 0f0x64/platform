#include <../lib/constBuf.shader>
#include <../lib/io.shader>

Texture2D tex1 : register(t0);
SamplerState sam1 : register(s0);

//[
cbuffer params : register(b3)
{
    float4 tone;
}
//]

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = float4(input.uv, sin(input.uv.x * 12), 1);
     
    float2 uv = input.uv * 5;
    uv.y -= 2;
    color = tex1.Sample(sam1, uv);

    return color;
}
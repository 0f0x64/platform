#include <../lib/constBuf.shader>
#include <../lib/io.shader>

Texture2D albedo : register(t0);
Texture2D metalness : register(t1);
SamplerState sam1 : register(s0);

//[
cbuffer params : register(b1)
{
    float tone;
    float p2;
}
//]

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = float4(input.uv, sin(input.uv.x * 12), 1);
     
    float2 uv = input.uv * 5;
    uv.y -= 2;
    color = albedo.Sample(sam1, uv);

    return color;
}
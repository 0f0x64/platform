#include <../lib/constBuf.shader>
#include <../lib/io.shader>

Texture2D tex : register(t0);
SamplerState s1 : register(s0);

//[
cbuffer params : register(b1)
{
    float r, g, b, a;
    float bold;
}
//]


float4 PS(VS_OUTPUT_POS_UV input) : SV_Target
{
    float4 color = tex.SampleLevel(s1, input.uv, 0);
    color.a = dot(color.rgb, 1);
    color *= float4(r, g, b, a);
    return float4(color);
}
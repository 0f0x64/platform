#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>


Texture2D screen : register(t0);
SamplerState sam1 : register(s0);


cbuffer params : register(b0)
{
    float hilight;
};



float4 PS(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float4 c = screen.SampleLevel(sam1,input.uv,0);
    c.b+=.25;
    c.rgb=ACESFilm(max(c.rgb,0)*.2);
    return float4(c.rgb,1);

}

#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>


Texture2D screen : register(t0);
Texture2D screenMid : register(t1);
Texture2D screenLow : register(t2);
SamplerState sam1 : register(s0);


cbuffer params : register(b0)
{
    float hilight;
};



float4 PS(VS_OUTPUT input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float4 c = screen.SampleLevel(sam1,input.uv,0);
    float4 c2 = screenLow.SampleLevel(sam1,input.uv,0);
    float4 c3 = screenMid.SampleLevel(sam1,input.uv,0);
    c+=c2+c3;
    
    c.rgb=ACESFilm(max(c.rgb,0)*.2);
    return float4(c.rgb,1);

}

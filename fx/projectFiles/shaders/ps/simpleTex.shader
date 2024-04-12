#include <../lib/constBuf.shader>
#include <../lib/io.shader>

TextureCube env : register(t0);
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
    
    float4 color = env.SampleLevel(sam1, float4(normalize(input.wpos.xyz),0), 0);
   
   
    return color;
}
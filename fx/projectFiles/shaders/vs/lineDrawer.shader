#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[
cbuffer params : register(b0)
{
    float4 _pos [4000];
}

cbuffer geo : register(b6)
{
    float4 position[4000];
}

//]


VS_OUTPUT_POS_UV VS(uint vID : SV_VertexID)
{
    VS_OUTPUT_POS_UV output = (VS_OUTPUT_POS_UV) 0;
    output.pos.xy = position[vID].xy * float2(2, -2) + float2(-1,1);
    output.pos.z = 0;
    output.pos.w = 1;
    output.uv = 0.;
    output.uv = 1;
    
    return output;
}

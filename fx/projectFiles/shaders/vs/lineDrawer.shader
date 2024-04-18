#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[
cbuffer params : register(b0)
{
    float x1,y1,x2,y2;
}

cbuffer geo : register(b6)
{
    float4 position[256];
}

//]


VS_OUTPUT_POSONLY VS(uint vID : SV_VertexID)
{
    VS_OUTPUT_POSONLY output = (VS_OUTPUT_POSONLY) 0;
    output.pos.xy = position[vID].xy * float2(2, -2) + float2(-1,1);
    output.pos.z = 0;
    output.pos.w = 1;

    return output;
}

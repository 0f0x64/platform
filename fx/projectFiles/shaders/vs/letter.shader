#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

//[
cbuffer params : register(b0)
{
    float width, height;
}

cbuffer geo : register(b6)
{
    float4 pos_size[256];
}
//]

VS_OUTPUT_POS_UV VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT_POS_UV output;
    float2 quad[6] = { 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1 };
    float2 p = (quad[vID] - float2(0,1)) * float2(width, height) + pos_size[iID].xy;
    output.pos = float4(p, 0, 1);
    output.uv = float2(quad[vID].x, 1- quad[vID].y);
    output.uv.y /= 96.;
    output.uv.y+= pos_size[iID].z/96.;
    
    //output.sz = float2(width, height);
    return output;
}
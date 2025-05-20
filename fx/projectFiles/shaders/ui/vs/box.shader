#include <../../lib/constBuf.shader>
#include <../../lib/io.shader>
#include <../../lib/constants.shader>
#include <../../lib/utils.shader>

//[
cbuffer params : register(b0)
{
    float4 pos_size[256];
}
//]

VS_OUTPUT_POS_UV VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT_POS_UV output;
    float2 quad[6] = { 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1 };
    float2 p = (quad[vID] - float2(0, 1)) * pos_size[iID].zw*2. + pos_size[iID].xy * float2(2, -2) - float2(1, -1);
    output.pos = float4(p, 0, 1);
    output.uv = quad [vID];
    output.uv.y = 1 - output.uv.y;
    output.sz = pos_size[iID].zw;
    return output;
}


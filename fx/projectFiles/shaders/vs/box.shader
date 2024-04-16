#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

//[
cbuffer params : register(b0)
{
    float gX, gY;
}

cbuffer geo : register(b6)
{
    float4 pos_size[256];
}
//]

VS_OUTPUT_POS_UV VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT_POS_UV output;
    float2 quad[6] = { -1, -1, 1, -1, -1, 1, 1, -1, 1, 1, -1, 1 };
    float2 p = quad[vID];
    p *= pos_size[iID].zw;
    p += pos_size[iID].xy;
    output.pos = float4(p, 0, 1);
    output.uv = float2(1, -1) * quad [vID]/ 2. + .5;
    output.sz = pos_size[iID].zw;
    return output;
}


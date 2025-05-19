#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[

cbuffer params : register(b0)
{
    float4x4 model;
    float4 position [4000];
}
//]


VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 pos = float4(position[vID].xyz, 1);
    pos.xyz = mul(pos.xyz, (float3x3)model);
    pos.z += 12;
 
    output.pos = mul(pos, proj[0]);
    output.wpos = position[vID];
    output.uv = position[vID].w;
    return output;
}

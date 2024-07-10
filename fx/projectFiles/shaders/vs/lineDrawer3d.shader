#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[

cbuffer geo : register(b6)
{
    float4 position[4000];
}

//]


VS_OUTPUT_POSONLY VS(uint vID : SV_VertexID)
{
    VS_OUTPUT_POSONLY output = (VS_OUTPUT_POSONLY) 0;
    
    float4 pos = float4(position[vID].xyz, 1);
    pos.xyz = mul(pos.xyz, (float3x3)model);
    pos.z += 12;
 
    output.pos = mul(pos, proj[0]);
    
    return output;
}

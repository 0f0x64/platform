#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

Texture2D positions : register(t0);
Texture2D normals : register(t0);
SamplerState sam1 : register(s0);

//[
cbuffer params : register(b0)
{
    float gX, gY;
}
//]

float3 rotY(float3 p, float a)
{
    float3x3 r;
    r[0] = float3(cos(a), 0, sin(a));
    r[1] = float3(0, 1, 0);
    r[2] = float3(-sin(a), 0, cos(a));
    return mul(p, r);
}


VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 grid = getGrid(vID, 1, float2(gX,gY));
    float2 uv = grid.xy;
    
    float4 pos = positions.SampleLevel(sam1, uv, 0);
    //---
    output.wpos = float4(pos.xyz, 0);
    output.vpos = mul(float4(pos.xyz, 1), view[0]);
    
    output.pos = mul(float4(pos.xyz, 1), mul(view[0], proj[0]));
    output.uv = grid.xy;
    return output;
}

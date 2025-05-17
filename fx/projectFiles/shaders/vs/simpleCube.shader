#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

//[
cbuffer params : register(b0)
{
    int  gX,gY;
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
    
    float4 grid = getGrid(vID, 1, float2(gX, gY));
    float2 uv = grid.xy-.5;
    
    float2 a = uv * PI * 2;
    float3 pos = float3(sin(a.x), sin(a.y / 2), cos(a.x));
    pos.xz *= cos(a.y / 2);
    pos = rotY(pos, 45 * PI / 180.);
    pos = clamp(pos, -.5, .5)*6;

    //---
    output.wpos = float4(pos, 0);

    pos = mul(pos, (float3x3) view[0]);
    output.pos = mul(float4(pos, 1), proj[0]);
    
    output.uv = grid.xy;
    return output;
}

#include <../lib/constBuf.shader>
#include <../lib/io.shader>

//[
cbuffer params : register(b0)
{
    float  gX,gY;
}
//]

#define PI 3.1415926535897932384626433832795

float3 rotY(float3 p, float a)
{
    float3x3 r;
    r[0] = float3(cos(a), 0, sin(a));
    r[1] = float3(0, 1, 0);
    r[2] = float3(-sin(a), 0, cos(a));
    return mul(p, r);
}

float4 getGrid(uint vID,float sep)
{
    uint index = vID / 6;
    float2 map[6] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };
    float2 uv = map[vID % 6];
    float2 grid = (float2((index % uint(gX)), index / uint(gX)) + (uv-.5) * sep+.5) / float2(gX, gY);
    return float4(grid, uv);
}
VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 grid = getGrid(vID,.98);
    float2 uv = grid.xy-.5;
    
    
    float2 a = uv * PI * 2;
    float3 pos = float3(sin(a.x), sin(a.y / 2), cos(a.x));
    pos.xz *= cos(a.y / 2);
//    pos *= 12;

    pos = clamp(pos, -1, 1);

    pos *= .70;
    pos.y /= sqrt(2);
    
    float4 p = float4(pos*.81, 1);
    //---
    output.wpos = float4(pos, 0);
    output.pos = mul(p, mul(view[0], proj[0]));
    
    output.uv = grid.xy;
    return output;
}

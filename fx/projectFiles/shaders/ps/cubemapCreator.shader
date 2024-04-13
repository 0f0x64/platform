#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[
cbuffer params : register(b1)
{
    float p;
}
//]

struct PS_OUTPUT
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
    float4 c2 : SV_Target2;
    float4 c3 : SV_Target3;
    float4 c4 : SV_Target4;
    float4 c5 : SV_Target5;
    
};

PS_OUTPUT PS(VS_OUTPUT input) 
{

    float2 uv = input.uv;
    float4 color = float4(uv,0,1);

    PS_OUTPUT cube;
    color.xyz = saturate((sin(uv.x * 32) * sin(uv.y * 32 )) * 100).xxx;
    cube.c0 = color * float4(0, 0, 1 ,1);
    cube.c1 = color * float4(0, 1, 0, 1);
    cube.c2 = color * float4(0, 1, 1, 1);
    cube.c3 = color * float4(1, 0, 0, 1);
    cube.c4 = color * float4(1, 0, 1, 1);
    cube.c5 = color * float4(1, 1, 0, 1);
    return cube;
}
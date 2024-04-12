#include <../lib/constBuf.shader>
#include <../lib/io.shader>


//[
cbuffer params : register(b1)
{
    float p;
}
//]

struct PS_OUTPUT
{
    float4 face0 : SV_Target0;
    float4 face1 : SV_Target1;
    float4 face2 : SV_Target2;
    float4 face3 : SV_Target3;
    float4 face4 : SV_Target4;
    float4 face5 : SV_Target5;
    
};

PS_OUTPUT PS(VS_OUTPUT input) : SV_Target
{

    float2 uv = input.uv;
    float4 color = float4(uv,0,1);

    PS_OUTPUT cube;
    cube.face0 = color;
    cube.face1 = color;
    cube.face2 = color;
    cube.face3 = color;
    cube.face4 = color;
    cube.face5 = color;
    return cube;
}
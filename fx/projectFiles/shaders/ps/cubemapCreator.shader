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
    float4 color = 1;//    float4(uv, 0, 1);

    PS_OUTPUT cube;
    float a = PI*4;
    float walls = saturate(1 - 2 * length(uv - .5));
    color.xyz = saturate(-sign(max(sin(uv.x * a), sin(uv.y * a))));
    cube.c0 = walls * float4(0, 0, 1 ,1);
    cube.c1 = walls * float4(0, 1, 0, 1);
    float top = saturate(-sign(max(sin(uv.x * a), sin(uv.y * a)))) * 12;
    cube.c2 = top;
    float floor = saturate(-sign((sin(uv.x * a * 4) * sin(uv.y * a * 4)))) * (1 - length(uv - .5)*2);
    cube.c3 = float4(.5, .5, .35, 1)*floor;
    cube.c4 = walls * float4(1, 0, 0, 1);
    cube.c5 = walls * float4(1, 0, 1, 1);
    return cube;
}
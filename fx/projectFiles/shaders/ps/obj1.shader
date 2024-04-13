#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>

//[
cbuffer params : register(b1)
{
    float sx,sy,sz;
};
//]

float3 sphere(float2 uv)
{

    float2 a = uv * PI * 2;
    //   a.x *= -1;
    float3 pos = float3(sin(a.x), sin(a.y / 2), cos(a.x));
    pos.xz *= cos(a.y / 2);
       pos = clamp(pos, -.5, .5);
    pos *= .45;
    return pos;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv - .5;
    float3 pos = sphere(uv)*2;
    
    return float4(pos, 1.);

}

#include <../lib/io.shader>
#include <../lib/constBuf.shader>

//[
cbuffer params : register(b1)
{
    float r, g, b, a;
};
//]



float4 PS(VS_OUTPUT input) : SV_Target
{
    float m = sign(sin(input.uv.x * 50 - time.x * 1) + .75);
    return float4(saturate(abs(sign(input.wpos.xyz)) * m), a);
}
#include <../lib/io.shader>

//[
cbuffer params : register(b1)
{
    float r,g,b,a;
};
//]



float4 PS(VS_OUTPUT_POSONLY input) : SV_Target
{
    return float4(r, g, b, a);
}
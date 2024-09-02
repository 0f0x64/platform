#include <../lib/io.shader>
#include <../lib/constBuf.shader>

//[
cbuffer params : register(b1)
{
    float r,g,b,a;
};
//]



float4 PS(VS_OUTPUT_POS_UV input) : SV_Target
{
    return float4(r, g, b, a );
}
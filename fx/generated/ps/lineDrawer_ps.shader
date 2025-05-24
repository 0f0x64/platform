#include<../../lib/io.shader>
#include<../../lib/constBuf.shader>
cbuffer params:register(b0)
{
float4 color;
};
float4 PS(VS_OUTPUT_POS_UV c):SV_Target{return color;}
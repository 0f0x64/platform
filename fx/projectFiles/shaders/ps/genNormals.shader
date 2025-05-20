#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>


Texture2D geo : register(t0);
SamplerState sam1 : register(s0);


float4 PS(VS_OUTPUT input) : SV_Target
{
    uint sgX, sgY;
    geo.GetDimensions(sgX, sgY);

    float a = 1;
    sgY *= a;
    sgX *= a;
    float2 uv = input.uv;

    float4 pt0 = geo.Sample(sam1, uv + float2(-1. / sgX, 0));
    float4 pt1 = geo.Sample(sam1, uv + float2(1. / sgX, 0));
    float4 pt2 = geo.Sample(sam1, uv + float2(0, -1. / sgY));
    float4 pt3 = geo.Sample(sam1, uv + float2(0, 1. / sgY));
    float3 n = normalize(cross(pt0.xyz - pt1.xyz, pt2.xyz - pt3.xyz));
    
    float3 color = n;
    return float4(color, 1);

}

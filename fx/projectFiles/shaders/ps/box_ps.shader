#include <../lib/constBuf.shader>
#include <../lib/io.shader>

//[
cbuffer params : register(b1)
{
    float _aspect;
    float rad;
    float r, g, b, a;
    float soft;
    float edge;
    float outlineBrightness;
}
//]

float roundedBoxSDF(float2 CenterPosition, float2 Size, float Radius)
{
    return length(max(abs(CenterPosition) - Size + Radius, 0.0)) - Radius;
}

float calcRA(float2 uv, float2 sz, float radius)
{
    float2 ca = float2(1, aspect.x) ;
    float2 s = sz * ca;
    float r1 = (.001 + radius) * min(s.x, s.y);
    float r2 = roundedBoxSDF(uv*s, s*.5, r1)  / r1;
    return -r2;

}

float4 PS(VS_OUTPUT_POS_UV input) : SV_Target
{
    float4 color = float4(r, g, b, a);
    float2 uvs = (input.uv - .5);
    float d = calcRA(uvs, input.sz, rad);
    float embossMask = sign(d) - saturate(d * edge * input.sz);
    float emboss = embossMask * dot(atan(uvs - .1), -.25);
    color.rgb += emboss;
    float2 s = input.sz;
    float m = min(s.x, s.y) / max(s.x, s.y);
    m *= length(s);
    float outline = 1 - saturate(d * 8. * sqrt(m)); // * float3(r, g, b);
    color.rgb += outline*outlineBrightness;
    return float4(color.rgb, saturate(d*soft)*color.a);
}
#include <../lib/constBuf.shader>
#include <../lib/io.shader>

//[
cbuffer params : register(b1)
{
    float Aspect;
    float iAspect;
    float ResolutionX;
    float ResolutionY;
    float emboss;
};
//]

float roundedBoxSDF(float2 CenterPosition, float2 Size, float Radius)
{
    return length(max(abs(CenterPosition) - Size + Radius, 0.0)) - Radius;
}

float calcRA(float2 uv, float2 sz, float r)
{
    float2 s = sz;
    float r1 = min(s.x, s.y) * (0.001 + r );
    float r2 = roundedBoxSDF(uv, s*.5, r1)  / r1;
    return r2;

}

float sdRoundBox(float2 p, float2 sz, float rb)
{
    p = abs(p) - sz;
    p = max(p, p.yx - rb);
    float a = 0.5 * (p.x + p.y);
    float b = 0.5 * (p.x - p.y);
    return a - sqrt(rb * rb * 0.5 - b * b);
}

float4 PS(VS_OUTPUT_POS_UV input) : SV_Target
{
    float2 uv = input.uv;
    float2 sz = input.sz;
    float2 nsz = normalize(sz);
    float ratio = nsz.x / nsz.y;
    float2 uvs = (input.uv - .5);

    float2 ca = float2(1,Aspect);

    float d = calcRA(uvs*sz*ca, sz*ca, .5);
    float c = sign(-d) - saturate(-d*.4 );
    c *= pow(dot(atan(uvs-.1), -.25), 1);
    c += .25;
    
    return float4(c, c, c, saturate(-d*10));
}
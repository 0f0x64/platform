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
    float progress_x;
    float progress_y;
    float progress_radial;
    float signed_progress;
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

    float progressX = progress_x < input.uv.x ? 0. : .125;
    progressX+=progress_x>0 ? abs(uvs.y)*saturate(pow(saturate(1-2*abs(input.uv.x-progress_x)),28)) :0;
    float signedProgressX = saturate(pow(saturate(1-2*abs(uvs.x-progress_x)),8));
    signedProgressX*=4*pow(abs(uvs.y),2);
    progressX=lerp(progressX,signedProgressX,signed_progress);

    float iuv = 1. - input.uv.y;
    float progressY = progress_y < iuv ? 0. : .125;
    progressY+=progress_y>0 ? abs(uvs.x)*saturate(pow(saturate(1-2*abs(iuv-progress_y)),28)) :0;
    float signedProgressY = saturate(pow(saturate(1-2*abs(uvs.y-progress_y)),8));
    signedProgressY*=4*pow(abs(uvs.x),2);
    progressY=lerp(progressY,signedProgressY,signed_progress);


    color += progressX+progressY;
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
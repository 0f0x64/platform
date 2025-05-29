#include <../../lib/constBuf.shader>
#include <../../lib/io.shader>
#include <../../lib/constants.shader>

//[
cbuffer params : register(b0)
{
    float _aspect;
    float rad;
    float r, g, b, a;
    float soft;
    float edge;
    float outlineBrightness;
    float progress;
    float signed_progress;
    float slider_type;
}
//]

float2 rot(float2 pos,float a)
{
    float2x2 m =
    {
        cos(a), -sin(a),
        sin(a), cos(a)
    };
    return mul(pos, m);

}

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

    float signedProgress =0;
    float Progress =0;

    if (slider_type==1)
    {
    Progress = progress < input.uv.x ? 0. : .125;
    Progress+=progress>0 ? abs(uvs.y)*saturate(pow(saturate(1-2*abs(input.uv.x-progress)),28)) :0;
    signedProgress = saturate(pow(saturate(1-2*abs(uvs.x-progress)),8));
    signedProgress*=4*pow(abs(uvs.x),2);
    }

    if (slider_type==2)
    {
    float iuv = 1. - input.uv.y;
    Progress = progress < iuv ? 0. : .125;
    Progress+=progress>0 ? (.4+abs(uvs.x*2))*saturate(pow(saturate(1-2*abs(iuv-progress)),28)) :0;
    signedProgress = saturate(pow(saturate(1-2*abs(uvs.y-progress)),8));
    signedProgress*=4*pow(abs(uvs.y),2);
    
    }

    if (slider_type==3)
    {
    float2 uvsR = rot(uvs,progress.x*PI*3/2);
    float ang = sign(saturate(1.-28*abs(uvsR.x)));
    Progress = ang*saturate(sign(-uvsR.y-.2));
    Progress += saturate(1-25*abs(length(uvs)-.45));
    //Progress *= saturate(1-2.5*length(uvs))*4;
    Progress *= (abs(atan2(uvs.x,uvs.y))>PI/4.2)*.5;

    float pAng = (sign(uvs.x)*sign(progress.x));
    pAng*=saturate(-sign(progress.x)*sign(uvsR.x));
    pAng*=saturate(1-25*abs(length(uvs)-.45));
    Progress +=saturate(pAng);

    signedProgress = Progress;
    
    }
  
    
    
    Progress=lerp(Progress,signedProgress,signed_progress);
   
    color += Progress-sign(slider_type)*.1;

    float d = calcRA(uvs, input.sz, rad);
    float embossMask = sign(d) -saturate(d * edge * input.sz);
    float emboss = embossMask * dot(atan(uvs - .1), -.25);
    color.rgb += emboss;
    float2 s = input.sz;
    float m = min(s.x, s.y) / max(s.x, s.y);
    m *= length(s);
    float outline = 1 - saturate(d * 8. * sqrt(m)); // * float3(r, g, b);
    color.rgb += outline*outlineBrightness;
    //return 1;
    return float4(color.rgb, saturate(d*soft)*color.a);
}
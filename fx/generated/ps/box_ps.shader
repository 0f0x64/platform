#include<../../lib/constBuf.shader>
#include<../../lib/io.shader>
#include<../../lib/constants.shader>
cbuffer params:register(b0)
{
float _aspect;
float rad;
float r,g,b,a;
float soft;
float edge;
float outlineBrightness;
float progress;
float signed_progress;
float slider_type;
}
float2 rot(float2 s,float a){float2x2 f={cos(a),-sin(a),sin(a),cos(a)};return mul(s,f);}float roundedBoxSDF(float2 s,float2 p,float a){return length(max(abs(s)-p+a,0.))-a;}float calcRA(float2 s,float2 p,float a){float2 f=float2(1,aspect.x);f=p*f;a=(.001+a)*min(f.x,f.y);a=roundedBoxSDF(s*f,f*.5,a)/a;return-a;}float4 PS(VS_OUTPUT_POS_UV s):SV_Target{float4 a=float4(r,g,b,a);float2 f=s.uv-.5;float p=0,r=0;if(slider_type==1)r=progress<s.uv.x?0.:.125,r+=progress>0?abs(f.y)*saturate(pow(saturate(1-2*abs(s.uv.x-progress)),28)):0,p=saturate(pow(saturate(1-2*abs(f.x-progress)),8)),p*=4*pow(abs(f.x),2);if(slider_type==2){float a=1.-s.uv.y;r=progress<a?0.:.125;r+=progress>0?(.4+abs(f.x*2))*saturate(pow(saturate(1-2*abs(a-progress)),28)):0;p=saturate(pow(saturate(1-2*abs(f.y-progress)),8));p*=4*pow(abs(f.y),2);}if(slider_type==3){float2 s=rot(f,progress.x*PI*3/2);float a=saturate(1.-28*abs(s.x));r=a*saturate(sign(-s.y-.2));r+=saturate(1-38*abs(length(f)-.45));r*=abs(atan2(f.x,f.y))>PI/4.2;p=r;}r=lerp(r,p,signed_progress);a+=r;p=calcRA(f,s.sz,rad);r=sign(p)-saturate(p*edge*s.sz);r*=dot(atan(f-.1),-.25);a.xyz+=r;f=s.sz;r=min(f.x,f.y)/max(f.x,f.y)*length(f);r=1-saturate(p*8.*sqrt(r));a.xyz+=r*outlineBrightness;return float4(a.xyz,saturate(p*soft)*a.w);}
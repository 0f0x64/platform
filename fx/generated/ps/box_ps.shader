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
float2 rot(float2 s,float p){float2x2 x={cos(p),-sin(p),sin(p),cos(p)};return mul(s,x);}float roundedBoxSDF(float2 s,float2 p,float x){return length(max(abs(s)-p+x,0.))-x;}float calcRA(float2 s,float2 p,float x){float2 a=float2(1,aspect.x);a=p*a;x=(.001+x)*min(a.x,a.y);x=roundedBoxSDF(s*a,a*.5,x)/x;return-x;}float4 PS(VS_OUTPUT_POS_UV s):SV_Target{float4 a=float4(r,g,b,a);float2 x=s.uv-.5;float p=0,f=0;if(slider_type==1)f=progress<s.uv.x?0.:.125,f+=progress>0?abs(x.y)*saturate(pow(saturate(1-2*abs(s.uv.x-progress)),28)):0,p=saturate(pow(saturate(1-2*abs(x.x-progress)),8)),p*=4*pow(abs(x.x),2);if(slider_type==2){float a=1.-s.uv.y;f=progress<a?0.:.125;f+=progress>0?(.4+abs(x.x*2))*saturate(pow(saturate(1-2*abs(a-progress)),28)):0;p=saturate(pow(saturate(1-2*abs(x.y-progress)),8));p*=4*pow(abs(x.y),2);}if(slider_type==3){float2 s=rot(x,progress.x*PI*3/2);float a=sign(saturate(1.-28*abs(s.x)));f=a*saturate(sign(-s.y-.2));f+=saturate(1-25*abs(length(x)-.45));f*=(abs(atan2(x.x,x.y))>PI/4.2)*.5;a=sign(x.x)*sign(progress.x);a*=saturate(-sign(progress.x)*sign(s.x));a*=saturate(1-25*abs(length(x)-.45));f+=saturate(a);p=f;}f=lerp(f,p,signed_progress);a+=f-sign(slider_type)*.1;p=calcRA(x,s.sz,rad);f=sign(p)-saturate(p*edge*s.sz);f*=dot(atan(x-.1),-.25);a.xyz+=f;x=s.sz;f=min(x.x,x.y)/max(x.x,x.y)*length(x);f=1-saturate(p*8.*sqrt(f));a.xyz+=f*outlineBrightness;return float4(a.xyz,saturate(p*soft)*a.w);}
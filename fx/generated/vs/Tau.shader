#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float4x4 model;int gX;int gY;int mode;int skipper;float4 base_color;};float toRad(float z){return z*PI/180.;}float smin(float f,float m,float z){float s=clamp(.5+.5*(m-f)/z,0.,1.);return lerp(m,f,s)-z*s*(1.-s);}float3 pillar(uint m,uint b,float2 z,float f,float s,float I){float3 t=float3(hash(b/2e2),hash(b/140.),hash(b/120.))-.5;f=10;float3 r;r.xz=normalize(t.xz)*f*(1-.5*cos(t.y*PI));r.y=t.y*f;r.xz*=cos(r.y/PI);r*=3;s=sign(b%2-.5);r.x+=f*1.5;r.x*=s;r.xz=lerp(r.xz,r.xz*float2(-1,1),pow(5/length(r.xz),8));return r/5;}pos_color CalcParticles(uint f,uint m,float4 r){f*=skipper;float z=time.x*.004;uint s=3623;if(mode==1||m%s==0)z=0;float3 t=pillar(f,m,r.xy,0,z,0),b=t;pos_color i;i.color=float4(noise3_u(111+float3(113,11,111)*221+177+sin(b*.48)),1)/90.+.015;if(mode==1){float z=hash(m)*33+11;i.pos=transform(t,r.zw,z);i.color*=0;i.sz=172;}else{i.pos=transform_unisize(t,r.zw,1.75);i.sz=1;i.color*=1.2;if(m%s==0)i.pos=transform_unisize(t,r.zw,31.5),i.sz=2,i.color*=23;}return i;}
#include<../lib/particleVS_main2.shader>

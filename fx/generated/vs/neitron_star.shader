#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float4x4 model;int gX;int gY;int mode;int skipper;};pos_color CalcParticles(uint f,float4 r){float2 i=r.xy;uint s=42;i=frac(i*s);float b=quantize(r.x,s),x=b*360.;i=i*PI*2;i.x*=-1;float3 I=0;b=hash(b*3111.123)*6+7;b/=2;float g=f%15;I.x-=b;float y=frac(i.y/PI+time.x*.0051*(f%3+1)+f/9e2)*PI*2;if(g!=0)I=rotZ(I,y);else I=rotZ(I,y/110),I.y=(y-PI)*10,I.xz*=1+pow(abs(y-PI),7)*1e-4;I.x+=b;if(g!=0)I+=rot3(I,x*float3(1,2,3)+length(I)*.2-time.x*.03);I=rotY(I,x*PI/180.);float3 m=smoothstep(0,1,saturate(length(I)/5*(1+abs(I.y))));I=lerp(normalize(I+noise3(float3(i,f/1e3)))*3,I,m);I*=2;I+=I/12/(noise3(f*float3(1,2,3)/68.+222)+.6+.1);if(g==0)I=rotX(I,.1);pos_color e;e.xyzw=float4(float3(1,2,3),1)*.015+.0015;if(f==0)e.xyzw*=4.8,e.pos=transform(float3(0,0,0),r.zw,552),e.sz=2;else e.pos=transform_unisize(I/16,r.zw,3.),e.sz=2;return e;}
#include<../lib/particleVS_main.shader>

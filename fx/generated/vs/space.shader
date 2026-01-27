#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float4x4 model;int gX;int gY;int mode;int skipper;};float4 transform2(float3 f,float2 m,float w){m-=.5;float2 i=float2(proj[0]._m00,proj[0]._m11);float4 p;p.w=1;p.xyz=mul(f/1.2,(float3x3)view[0]);float4 z=mul(p,proj[0]);i=m*w*.1;p.xy+=i;return mul(p,proj[0]);}float4 transform_unisize2(float3 f,float2 m,float w){m-=.5;float2 i=float2(proj[0]._m00,proj[0]._m11);float4 p;p.w=1;p.xyz=mul(f,(float3x3)view[0]);float4 z=mul(p,proj[0]);i=m*.002*(p.z/p.w)*w*(normalize(i)*2);p=mul(p,proj[0]);p.xy+=i;return p;}pos_color CalcParticles(uint p,float4 f){p*=skipper;float3 i=shp(f.xy);i+=.7*rot3(i,31/i+.1*noise3(i*3));i=lerp(normalize(i)*33,i,.45);pos_color m;m.xyzw=float4(float3(3,6,9),1)*.051+.0015;if(mode==1)m.pos=transform2(i,f.zw,302),m.xyzw/=5,m.sz=2;else{m.pos=transform_unisize2(i,f.zw,1.);m.sz=1;m.xyzw*=3.8*hash(p)+.01;if(p%8==0)m.pos=transform2(i,f.zw,63),m.xyzw*=.051,m.sz=2;}return m;}
#include<../lib/particleVS_main.shader>

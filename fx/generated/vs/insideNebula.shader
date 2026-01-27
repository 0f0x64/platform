#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float4x4 model;int gX;int gY;int mode;int skipper;float4 base_color;};float toRad(float r){return r*PI/180.;}float3 pillar(uint m,uint b,float2 w,float i,float f,float r){float3 s=float3(hash(b/2e2),hash(b/140.),hash(b/120.))-.5;s=normalize(s)*2;s+=s*noise3(s);s*=4;i=hash(b/1e3);s=rot3(s,noise3(s*.8+float3(0,f,0))/6);s+=noise3(s)*.8;s=rot3(s,noise3(s.zyx*1.6+float3(0,-f,0))/12);float3 z=s;s=(frac(s/7)-.5)*14;return s*2;}pos_color CalcParticles(uint s,uint f,float4 r){s*=skipper;float w=time.x*.004;uint z=10000;if(mode==1||f%z==0)w=0;float3 b=pillar(s,f,r.xy,0,w,0),u=b;pos_color i;i.color=float4(noise3_u(float3(113,115,1)*221+177+sin(u*.48)),1)/110.+.0015;i.color*=base_color/2;if(mode==1){float s=hash(f)*33+11;i.pos=transform(b,r.zw,s);i.color*=12;i.sz=172;}else{i.pos=transform_unisize(b,r.zw,1.5);i.sz=1;if(f%z==0)i.pos=transform_unisize(b,r.zw,151.5),i.sz=2,i.color*=3;}return i;}
#include<../lib/particleVS_main2.shader>

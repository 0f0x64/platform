#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float4x4 model;int gX;int gY;int mode;int skipper;};float3 pillar(float2 s,float m,float n,float r){float3 i=shp(s.xy);i.y*=.8+r;i.y+=r;i*=1+noise3(i*111.6+1111/(m+1))*1.6;i.y*=1.8;i*=1+noise3(i*1.6+1111/(m+1)+n);i=rot3(i,2.2/(pow(i,14)+1));i.y*=1.5;i*=1+noise3(i+1111/(m+1)+n);i*=1+noise3(i*3.5)/3;return i*2;}pos_color CalcParticles(uint i,float4 m){i*=skipper;float s=time.x*.01;uint z=10000;if(mode==1||i%z==0)s=0;uint l=7;float w=i%l*PI/180.*(360./l),r=(sin(w*3)+2)/2;float3 f=pillar(m.xy,w,s,r),y=f;f.x+=17;f=rot3(f,float3(-.9,0,.4));f.y/=1.3;f.y-=r*10-8;if(i%l)f=rotY(f,w);f*=-.5;f.y*=1.4;f.y-=r;pos_color p;p.xyzw=float4(noise3_u(w*float3(13,15,12)*221+77+sin(y*1.4)),1)/30.+.0015;if(mode==1)p.pos=transform(f,m.zw,52),p.xyzw*=8,p.sz=172;else{p.pos=transform_unisize(f,m.zw,1.5);p.sz=1;if(i%z==0)p.pos=transform_unisize(f,m.zw,51.5),p.sz=2,p.xyzw*=15;}p.xyzw/=min(pow(p.pos.w,1.1)*.21+.5,11);return p;}
#include<../lib/particleVS_main.shader>

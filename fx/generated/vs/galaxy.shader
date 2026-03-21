#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float4x4 model;int gX;int gY;int mode;int skipper;float4 base_color;};float3 pillar(float2 m,float p,float n,float r){float3 i=shp(m.xy);i.y*=.8+r;i.y+=r;i*=1+noise3(i*111.6+1111/(p+1))*1.6;i.y*=1.8;i*=1+noise3(i*1.6+1111/(p+1)+n);i=rot3(i,2.2/(pow(i,14)+1));i.y*=1.5;i*=1+noise3(i+1111/(p+1)+n);i*=1+noise3(i*3.5)/3;return i*2;}pos_color CalcParticles(uint i,float4 p){uint m=i%340,r=7;float n=i%r*PI/180.*(360./r),z=time.x*.1;float3 e=pillar(p.xy,0,z*.005,.5)/5,f=e;e.y*=0;e.xz*=9;e.x*=.6;e=rotY(e,length(e)*.3+n-time.x*.005);e.y+=length(e)/7;e.y-=4.5;e+=noise3(i*float3(1,2,3)*11.1)*2;pos_color s;s.color=float4(1,2,3,1)/120.+.0015;if(m==0)s.pos=transform_unisize(e,p.zw,1.),s.sz=1;else if(mode==0){float m=28;s.pos=transform_unisize(e,p.zw,m);s.sz=2;}else{float m=4;s.pos=transform(e,p.zw,m);s.sz=2;}s.color=float4(noise3_u(n*base_color.xyz+77+sin(f*11.4)),1)/50.+15e-5;s.color*=base_color/2;if(m==0&&mode==0)s.color*=540,s.sz=1;if(m!=0&&mode==1)s.color*=2.5;if(m!=0&&mode==0)s.color*=1.5;return s;}
#include<../lib/particleVS_main.shader>

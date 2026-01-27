#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){float4x4 model;int gX;int gY;int mode;int skipper;float4 base_color;};float toRad(float t){return t*PI/180.;}float2 random2(float2 t){return frac(sin(float2(dot(t,float2(127.1,311.7)),dot(t,float2(269.5,183.3))))*43758.5453);}float cellnoise(float2 p)
{
float2 st=p;
float3 color=.0;
//Scale
st*=3.;
//Tile the space
float2 i_st=floor(st);
float2 f_st=frac(st);
float m_dist=1.;//minimum distance
for(int y=-1;y<=1;y++){
for(int x=-1;x<=1;x++){
//Neighbor place in the grid
float2 neighbor=float2((x),(y));
//Random position from current+neighbor place in the grid
float2 pt=random2(i_st+neighbor);
//Animate the point
pt=0.5+0.5*sin(0+6.2831*pt);
//Vector between the pixel and the point
float2 diff=neighbor+pt-f_st;
//Distance to the point
float dist=length(diff);
//Keep the closer distance
m_dist=min(m_dist,dist);
}
}
//Draw the min distance(distance field)
color+=m_dist;
return color.x;
}
float2 toPolar(float2 t){return float2(atan2(t.y,t.x),length(t));}float2 fromPolar(float2 t){return float2(sin(t.x),cos(t.x))*t.y;}float3 pillar(uint t,uint f,float2 p,float i,float c,float d){float3 r=float3(hash(f/2e2),hash(f/140.),hash(f/120.))-.5;r=normalize(r)*2;r+=r*noise3(r)/2;r*=4;r*=1-length(r.xz)/4;i=hash(f/1e3);r=rot3(r,noise3(r*.8+float3(0,time.x/47,0))/6);r+=noise3(r)*.8;r=rot3(r,noise3(r.zyx*1.6+float3(0,-time.x/31,0))/12);float3 s=r;r.y=0;r.y-=cellnoise(r.xz/20)*3;r.y-=cellnoise(r.xz/6);r.y-=32/pow(length(r),2);r.y+=4;r.xz=toPolar(r.xz);r.y-=noise(r*10-float3(0,time.x/11,0))/6;r.y-=cellnoise(r.xz/2-float2(0,time.x/100))/3;r.xz=fromPolar(r.xz);r.y-=noise(r*5)/3;r.y*=-1;if(f%8==0)r.xz=noise3(f/1334)*12+normalize(r.xz)*14,r.xz/=3,r.y=0,r+=noise3(r*4)*.8,r.y+=cellnoise(r.xz/14)*sign(r.y)*2,r=rotY(r,length(r)),r.y+=4,r.y+=41/length(r)-6,r.x+=hash(f)/2;else r+=hash3(f)*length(r)/82;r.y-=2;return r*2;}pos_color CalcParticles(uint r,uint t,float4 f){r*=skipper;float p=time.x*.004;uint s=10000;if(mode==1||t%s==0)p=0;float3 c=pillar(r,t,f.xy,0,p,0),z=c;pos_color i;i.color=float4(noise3_u(float3(113,115,1)*221+177+sin(z*.48-111)),1)/110.+.0015;i.color*=base_color/2;if(mode==1){float r=hash(t)*33+11;i.pos=transform(c,f.zw,r);i.color*=12;i.sz=172;}else{i.pos=transform_unisize(c,f.zw,1.5);i.sz=1;if(t%s==0)i.pos=transform_unisize(c,f.zw,151.5),i.sz=2,i.color*=3;}return i;}
#include<../lib/particleVS_main2.shader>

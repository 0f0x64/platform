#include<../lib/constBuf.shader>
#include<../lib/io.shader>
cbuffer params:register(b0)
{
float gX,gY;
}

#define PI 3.1415926535897932384626433832795
float3 rotY(float3 g,float f){float3x3 r;r[0]=float3(cos(f),0,sin(f));r[1]=float3(0,1,0);r[2]=float3(-sin(f),0,cos(f));return mul(g,r);}float4 getGrid(uint f,float P){uint g=f/6;float2 r[6]={0,0,1,0,1,1,0,0,1,1,0,1},s=r[f%6],c=(float2(g%uint(gX),g/uint(gX))+(s-.5)*P+.5)/float2(gX,gY);return float4(c,s);}VS_OUTPUT VS(uint f:SV_VertexID){VS_OUTPUT r=(VS_OUTPUT)0;float4 g=getGrid(f,1);float2 c=g.xy-.5,s=c*PI*2;float3 P=float3(sin(s.x),sin(s.y/2),cos(s.x));P.xz*=cos(s.y/2);P=rotY(P,45*PI/180.);P=clamp(P,-.5,.5)*20;r.wpos=float4(P,0);r.pos=mul(float4(P,1),mul(view[0],proj[0]));r.uv=g.xy;return r;}
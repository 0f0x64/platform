#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0)
{
float gX,gY;
}
float3 rotY(float3 V,float f){float3x3 g;g[0]=float3(cos(f),0,sin(f));g[1]=float3(0,1,0);g[2]=float3(-sin(f),0,cos(f));return mul(V,g);}VS_OUTPUT VS(uint f:SV_VertexID){VS_OUTPUT g=(VS_OUTPUT)0;float4 s=getGrid(f,1,float2(gX,gY));float2 V=s.xy-.5,c=V*PI*2;float3 r=float3(sin(c.x),sin(c.y/2),cos(c.x));r.xz*=cos(c.y/2);r=rotY(r,45*PI/180.);r=clamp(r,-.5,.5)*5;g.wpos=float4(r,0);g.pos=mul(float4(r,1),mul(view[0],proj[0]));g.uv=s.xy;return g;}
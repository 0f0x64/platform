#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){int gX,gY;};float3 rotY(float3 f,float V){float3x3 g;g[0]=float3(cos(V),0,sin(V));g[1]=float3(0,1,0);g[2]=float3(-sin(V),0,cos(V));return mul(f,g);}VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float4 s=getGrid(V,1,float2(gX,gY));float2 g=(s.xy-.5)*PI*2;float3 r=float3(sin(g.x),sin(g.y/2),cos(g.x));r.xz*=cos(g.y/2);r=rotY(r,45*PI/180.);r=clamp(r,-.5,.5)*6;f.wpos=float4(r,0);r=mul(r,(float3x3)view[0]);f.pos=mul(float4(r,1),proj[0]);f.uv=s.xy;return f;}
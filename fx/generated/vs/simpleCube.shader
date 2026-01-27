#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
cbuffer params:register(b0){int gX,gY;};VS_OUTPUT VS(uint V:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float4 s=getGrid(V,1,float2(gX,gY));float2 g=(s.xy-.5)*PI*2;float3 X=float3(sin(g.x),sin(g.y/2),cos(g.x));X.xz*=cos(g.y/2);X=rotY(X,45*PI/180.);X=clamp(X,-.5,.5)*6;f.wpos=float4(X,0);X=mul(X,(float3x3)view[0]);f.pos=mul(float4(X,1),proj[0]);f.uv=s.xy;return f;}
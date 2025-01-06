#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
Texture2D positions:register(t0),normals:register(t0);SamplerState sam1:register(s0);cbuffer params:register(b0)
{
float gX,gY;
}
float3 rotY(float3 f,float s){float3x3 g;g[0]=float3(cos(s),0,sin(s));g[1]=float3(0,1,0);g[2]=float3(-sin(s),0,cos(s));return mul(f,g);}VS_OUTPUT VS(uint s:SV_VertexID){VS_OUTPUT g=(VS_OUTPUT)0;float4 f=getGrid(s,1,float2(gX,gY));float2 p=f.xy;float4 V=positions.SampleLevel(sam1,p,0);g.wpos=float4(V.xyz,0);g.vpos=mul(float4(V.xyz,1),view[0]);g.pos=mul(float4(V.xyz,1),mul(view[0],proj[0]));g.uv=f.xy;return g;}
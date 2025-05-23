#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
Texture2D positions:register(t0),normals:register(t1);SamplerState sam1:register(s0);cbuffer params:register(b0){float4x4 model;int gX;int gY;};float3 rotY(float3 s,float g){float3x3 f;f[0]=float3(cos(g),0,sin(g));f[1]=float3(0,1,0);f[2]=float3(-sin(g),0,cos(g));return mul(s,f);}VS_OUTPUT VS(uint s:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float4 m=getGrid(s,1,int2(gX,gY));float2 g=m.xy;float4 z=float4(positions.SampleLevel(sam1,g,0).xyz,1);z=mul(z,model);float4 p=float4(normals.SampleLevel(sam1,g,0).xyz,1);p=mul(p,transpose(model));f.vnorm=p;f.wpos=float4(z.xyz,0);f.vpos=mul(float4(z.xyz,1),view[0]);f.pos=mul(z,mul(view[0],proj[0]));f.uv=m.xy;return f;}
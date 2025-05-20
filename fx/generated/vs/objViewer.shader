#include<../lib/constBuf.shader>
#include<../lib/io.shader>
#include<../lib/constants.shader>
#include<../lib/utils.shader>
Texture2D positions:register(t0),normals:register(t1);SamplerState sam1:register(s0);cbuffer params:register(b0){int gX,gY;float4x4 model;};float3 rotY(float3 s,float m){float3x3 f;f[0]=float3(cos(m),0,sin(m));f[1]=float3(0,1,0);f[2]=float3(-sin(m),0,cos(m));return mul(s,f);}VS_OUTPUT VS(uint s:SV_VertexID){VS_OUTPUT f=(VS_OUTPUT)0;float4 m=getGrid(s,1,int2(gX,gY));float2 z=m.xy;float4 p=float4(positions.SampleLevel(sam1,z,0).xyz,1);p=mul(p,model);float4 g=float4(normals.SampleLevel(sam1,z,0).xyz,1);g=mul(g,transpose(model));f.vnorm=g;f.wpos=float4(p.xyz,0);f.vpos=mul(float4(p.xyz,1),view[0]);f.pos=mul(p,mul(view[0],proj[0]));f.uv=m.xy;return f;}
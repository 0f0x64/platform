#include<../../lib/constBuf.shader>
#include<../../lib/io.shader>
#include<../../lib/constants.shader>
cbuffer params:register(b0)
{
float4x4 model;
float4 position[4000];
}
VS_OUTPUT VS(uint p:SV_VertexID){VS_OUTPUT s=(VS_OUTPUT)0;float4 f=float4(position[p].xyz,1);f.xyz=mul(f.xyz,(float3x3)model);f.z+=12;s.pos=mul(f,proj[0]);s.wpos=position[p];s.uv=position[p].w;return s;}
#include<../../lib/constBuf.shader>
#include<../../lib/io.shader>
#include<../../lib/constants.shader>
#include<../../lib/utils.shader>
cbuffer params:register(b0)
{
float width,height;
float2 padding;
float4 pos_size[256];
}
cbuffer geo:register(b6)
{
float www;
float hjhh;
float4 _pos_size[256];
}
VS_OUTPUT_POS_UV VS(uint p:SV_VertexID,uint w:SV_InstanceID){VS_OUTPUT_POS_UV f;float2 e[6]={0,0,1,0,0,1,1,0,1,1,0,1},h=(e[p]-float2(0,1))*float2(width,height)+pos_size[w].xy*float2(2,-2)-float2(1,-1);f.pos=float4(h,0,1);f.uv=float2(e[p].x,1-e[p].y);f.uv.y/=96.;f.uv.y+=pos_size[w].z/96.;return f;}